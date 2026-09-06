// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaOverlayMapGrid.hpp"

#include <sstream>

#include <QApplication>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QGLWidget>
#include <QPainter>
#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>
#include <osg/Program>
#include <osg/Texture2D>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtoMemory.hpp"
#include "VaCamera.hpp"
#include "VaDecoratorNode.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

namespace
{
unsigned int numPtsPerLatLine = 60;
unsigned int numPtsPerLonLine = 60;
} // namespace

namespace vespa
{

VaOverlayMapGrid::VaOverlayMapGrid()
   : VaOverlay("graticule", cOVERLAY_WORLD)
   , mWidth(1.0f)
   , mDensity(1.0f)
   , mGridDataGenerated(false)
{
   SetType<VaOverlayMapGrid>();
   mColor[0] = 0.0f;
   mColor[1] = 0.0f;
   mColor[2] = 0.0f;
   mColor[3] = 1.0f;
}


VaOverlayMapGrid::VaOverlayMapGrid(const std::string& aName)
   : VaOverlay(aName, cOVERLAY_WORLD)
   , mWidth(1.0f)
   , mDensity(1.0f)
   , mGridDataGenerated(false)
{
   SetType<VaOverlayMapGrid>();
   mColor[0] = 0.0f;
   mColor[1] = 0.0f;
   mColor[2] = 0.0f;
   mColor[3] = 1.0f;
}

VaOverlayMapGrid* VaOverlayMapGrid::Clone() const
{
   return new VaOverlayMapGrid(*this);
}

bool VaOverlayMapGrid::Initialize()
{
   bool initialized = false;

   UtoRawShape shapePtr;
   mShapePtr = dynamic_cast<UtoRawShape*>(BindShape("mapgrid", shapePtr));
   if (mShapePtr == nullptr)
   {
      ut::log::warning() << "Could not bind shape: map grid";
   }

   if (mShapePtr != nullptr)
   {
      SetVisible(true);
      initialized = true;
   }
   mMatrixPtr = new osg::MatrixTransform;
   mShapePtr->GetTree()->addChild(mMatrixPtr.get());
   mMatrixPtr->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, true);
   mMatrixPtr->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(mWidth), osg::StateAttribute::ON);
   mMatrixPtr->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
   mMatrixPtr->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(), osg::StateAttribute::ON);
   mMatrixPtr->getOrCreateStateSet()->setRenderBinDetails(1000, "DepthSortedBin");

   osg::Geode* geode = new osg::Geode;
   mMatrixPtr->addChild(geode);
   mLabelGroup = new osg::Group;
   mMatrixPtr->addChild(mLabelGroup);
   mGeometryPtr = new osg::Geometry;
   geode->addDrawable(mGeometryPtr.get());

   mVArrayPtr = new osg::Vec3Array;
   mCArrayPtr = new osg::Vec4Array;
   mGeometryPtr->setVertexArray(mVArrayPtr.get());
   mGeometryPtr->setColorArray(mCArrayPtr.get());
   mCArrayPtr->push_back(osg::Vec4(mColor[0], mColor[1], mColor[2], mColor[3]));
   mGeometryPtr->setColorBinding(osg::Geometry::BIND_OVERALL);

   if (!mGridDataGenerated)
   {
      mCallbacks.Add(VaObserver::CameraUpdated.Connect(&VaOverlayMapGrid::CameraUpdatedCB, this));
   }

   GenerateGridData(GetViewer()->GetCamera());

   return initialized;
}

void VaOverlayMapGrid::GenerateGridData(VaCamera* aCameraPtr)
{
   osg::Matrix m;
   m.makeIdentity();
   if (aCameraPtr->IsFlat())
   {
      m.preMultTranslate(osg::Vec3(0, 0, 100));
   }
   mMatrixPtr->setMatrix(m);

   mGeometryPtr->removePrimitiveSet(0, mGeometryPtr->getNumPrimitiveSets());
   mVArrayPtr->clear();

   // Determine how much of the world is visible.

   GeoRect rect;
   int     numCycles = aCameraPtr->GetViewableArea(rect);
   double  minLat    = rect.GetSWLat();
   double  maxLat    = rect.GetNELat();
   double  minLon    = rect.GetSWLon();
   double  maxLon    = rect.GetNELon();

   // Make sure we don't have coincident points in the bounding rectangle.
   if (minLat == maxLat)
   {
      maxLat += 0.01;
   }
   if (minLon == maxLon)
   {
      maxLon += 0.01;
   }

   double rangeLat = maxLat - minLat;
   double rangeLon = maxLon - minLon;
   if (rangeLon < 0.0)
   {
      rangeLon += 360.0;
   }
   rangeLon += numCycles * 360.0;
   maxLon = minLon + rangeLon;

   // Determine the increment between grid lines.  If the area of regard is
   // in the mid-latitudes then the same scale is used for both families of
   // grid lines.  In the polar regions we decouple them because the meridian
   // lines start bunching up.
   //
   // NOTE: We use the un-rotated/tilted view to determine delta.  Otherwise
   //       we can get constantly changing scales as one changes azimuth,
   //       which looks weird...

   double       metersPerPixel = aCameraPtr->GetMetersPerPixel();
   unsigned int vpWidth;
   unsigned int vpHeight;
   aCameraPtr->GetViewport(vpWidth, vpHeight);

   rangeLat = vpWidth * metersPerPixel / aCameraPtr->GetMetersPerDegreeLat();
   if (vpHeight > vpWidth)
   {
      rangeLat = (rangeLat / vpWidth) * vpHeight;
   }
   rangeLon = vpWidth * metersPerPixel / aCameraPtr->GetMetersPerDegreeLon();
   if (vpHeight > vpWidth)
   {
      rangeLon = (rangeLon / vpWidth) * vpHeight;
   }
   double deltaLat = ComputeDelta(rangeLat);
   double deltaLon = deltaLat;
   if ((minLat > 45.0) || (maxLat < -45.0))
   {
      deltaLon = ComputeDelta(rangeLon);
      if (deltaLat / deltaLon >= 4.0)
      {
         deltaLon = 2.0 * deltaLat;
      }
   }

   if (deltaLat >= 1.0)
   {
      mLabelFormat = UtAngle::FmtD;
   }
   else if (deltaLat >= 1.0 / 60.0)
   {
      mLabelFormat = UtAngle::FmtDM;
   }
   else if (deltaLat >= 1.0 / 3600.0)
   {
      mLabelFormat = UtAngle::FmtDMS;
   }
   else
   {
      mLabelFormat = static_cast<UtAngle::Fmt>(UtAngle::FmtDMS + 1);
   }

   // Make the limits an even multiple of the increment.

   minLat -= fmod(minLat, deltaLat);
   maxLat -= fmod(maxLat, deltaLat);
   minLon -= fmod(minLon, deltaLon);
   maxLon -= fmod(maxLon, deltaLon);

   minLat -= deltaLat;
   maxLat += deltaLat;
   minLon -= deltaLon;
   maxLon += deltaLon;

   // If either pole is included then all longitudes are visible.

   if (GetViewer()->GetMapProjection().projection == UtoMapProjection::Geocentric)
   {
      if (maxLat >= 90.0)
      {
         maxLat = 90.0;
         minLon = -180.0;
         maxLon = 180.0;
      }
      if (minLat <= -90.0)
      {
         minLat = -90.0;
         minLon = -180.0;
         maxLon = 180.0;
      }
   }
   else
   {
      if (maxLat >= 90.0)
      {
         maxLat = 90.0;
      }
      if (minLat <= -90.0)
      {
         minLat = -90.0;
      }
   }

   // Determine parameters for lines of constant latitude.

   mMinLat      = minLat;
   mDeltaLat    = deltaLat;
   rangeLat     = maxLat - minLat;
   mNumLatLines = static_cast<unsigned int>((rangeLat + 0.5 * mDeltaLat) / mDeltaLat);
   ++mNumLatLines;
   mMaxLat = mMinLat + (mNumLatLines - 1) * mDeltaLat;

   // Determine parameters for lines of constant longitude.

   mMinLon      = minLon;
   mDeltaLon    = deltaLon;
   rangeLon     = maxLon - minLon;
   mNumLonLines = static_cast<unsigned int>((rangeLon + 0.5 * mDeltaLon) / mDeltaLon);
   ++mNumLonLines;

   // Limit longitudinal range to one complete cycle

   if (rangeLon > 360.0)
   {
      unsigned int numNeeded = static_cast<unsigned int>((360.0 + 0.5 * mDeltaLon) / mDeltaLon);
      ++numNeeded;
      unsigned int numMinDiscard = (mNumLonLines - numNeeded) / 2;
      mMinLon -= numMinDiscard * mDeltaLon;
      mNumLonLines = numNeeded;
   }

   // Limit number of lines drawn to something sane  TODO: this could be improved
   mNumLonLines = std::min(1000u, mNumLonLines);
   mNumLatLines = std::min(1000u, mNumLatLines);

   mMaxLon  = mMinLon + (mNumLonLines - 1) * mDeltaLon;
   rangeLon = mMaxLon - mMinLon;

   // Put the longitudinal coordinates back in the range [-180, 180]

   while (mMinLon < -180.0)
   {
      mMinLon += 360.0;
   }
   while (mMinLon > 180.0)
   {
      mMinLon -= 360.0;
   }
   while (mMaxLon < -180.0)
   {
      mMaxLon += 360.0;
   }
   while (mMaxLon > 180.0)
   {
      mMaxLon -= 360.0;
   }
   if (mMinLon == mMaxLon)
   {
      mMinLon = -180.0;
      mMaxLon = 180.0;
   }

   // Check for special dateline processing for flat grids.

   mCrossesDateline = false;
   if (mMinLon > mMaxLon)
   {
      mCrossesDateline = true;
   }

   if (GetViewer()->GetMapProjection().projection == UtoMapProjection::Geocentric)
   {
      GenerateCurvedGrid(aCameraPtr);
   }
   else
   {
      GenerateRectangularGrid(aCameraPtr);
   }
   GenerateLabels(aCameraPtr);
   mGridDataGenerated = true;
}

double VaOverlayMapGrid::ComputeDelta(double aAngle)
{
   double angle = aAngle;
   double delta = 1.0;

   if (angle < 1.0 / 3600.0)
   {
      delta = 0.2 / 3600.0; // 0.2"
   }
   else if (angle < 3.0 / 3600.0)
   {
      delta = 0.5 / 3600.0; // 0.5"
   }
   else if (angle < 7.0 / 3600.0)
   {
      delta = 1.0 / 3600.0; // 1"
   }
   else if (angle < 14.0 / 3600.0)
   {
      delta = 2.0 / 3600.0; // 2"
   }
   else if (angle < 21.0 / 3600.0)
   {
      delta = 3.0 / 3600.0; // 3"
   }
   else if (angle < 35.0 / 3600.0)
   {
      delta = 5.0 / 3600.0; // 5"
   }
   else if (angle < 70.0 / 3600.0)
   {
      delta = 10.0 / 3600.0; // 10"
   }
   else if (angle < 105.0 / 3600.0)
   {
      delta = 15.0 / 3600.0; // 15"
   }
   else if (angle < 140.0 / 3600.0)
   {
      delta = 20.0 / 3600.0; // 20"
   }
   else if (angle < 210.0 / 3600.0)
   {
      delta = 30.0 / 3600.0; // 30"
   }
   else if (angle < 7.0 / 60.0)
   {
      delta = 1.0 / 60.0; // 1'
   }
   else if (angle < 14.0 / 60.0)
   {
      delta = 2.0 / 60.0; // 2'
   }
   else if (angle < 21.0 / 60.0)
   {
      delta = 3.0 / 60.0; // 3'
   }
   else if (angle < 35.0 / 60.0)
   {
      delta = 5.0 / 60.0; // 5'
   }
   else if (angle < 70.0 / 60.0)
   {
      delta = 10.0 / 60.0; // 10'
   }
   else if (angle < 105.0 / 60.0)
   {
      delta = 15.0 / 60.0; // 15'
   }
   else if (angle < 140.0 / 60.0)
   {
      delta = 20.0 / 60.0; // 20'
   }
   else if (angle < 210.0 / 60.0)
   {
      delta = 30.0 / 60.0; // 30'
   }
   else if (angle < 7.0)
   {
      delta = 1.0; // 1 deg
   }
   else if (angle < 14.0)
   {
      delta = 2.0; // 2 deg
   }
   else if (angle < 21.0)
   {
      delta = 3.0; // 3 deg
   }
   else if (angle < 35.0)
   {
      delta = 5.0; // 5 deg
   }
   else if (angle < 50.0)
   {
      delta = 10.0; // 10 deg
   }
   else if (angle < 75.0)
   {
      delta = 15.0; // 15 deg
   }
   else
   {
      delta = 30.0;
   }

   return delta / mDensity;
}

void VaOverlayMapGrid::GenerateCurvedGrid(VaCamera* aCameraPtr)
{
   unsigned int i;
   unsigned int j;

   // Determine the altitude offset that must be added so that line segments
   // skim the perfect circle.

   double xyz[3];
   aCameraPtr->ConvertLLAToXYZ(xyz, 0.0, 0.0, 0.0);
   double earthRadius = sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]);
   double theta       = ((mNumLonLines - 1) * mDeltaLon) / (numPtsPerLatLine - 1);
   mAltOffset         = earthRadius * (1.0 / cos(0.5 * theta * UtMath::cRAD_PER_DEG) - 1.0);

   double lat;
   double lon;
   double alt = mAltOffset;

   // Generate lines of constant latitude
   unsigned int vTotal = 0;
   unsigned int vStart = 0;

   double incLon = ((mNumLonLines - 1) * mDeltaLon) / (numPtsPerLatLine - 1);
   for (i = 0; i < mNumLatLines; ++i)
   {
      vStart = vTotal;
      lat    = mMinLat + i * mDeltaLat;
      for (j = 0; j < numPtsPerLatLine; ++j)
      {
         lon = mMinLon + j * incLon;
         double xyz[3];
         aCameraPtr->ConvertLLAToXYZ(xyz, lat, lon, alt);
         mVArrayPtr->push_back(osg::Vec3(xyz[0], xyz[1], xyz[2]));
         ++vTotal;
      }
      mGeometryPtr->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, vStart, vTotal - vStart));
   }

   // Generate lines of constant longitude

   double incLat = ((mNumLatLines - 1) * mDeltaLat) / (numPtsPerLonLine - 1);
   for (i = 0; i < mNumLonLines; ++i)
   {
      vStart = vTotal;
      lon    = mMinLon + i * mDeltaLon;
      for (j = 0; j < numPtsPerLonLine; ++j)
      {
         lat = mMinLat + j * incLat;
         double xyz[3];
         aCameraPtr->ConvertLLAToXYZ(xyz, lat, lon, alt);
         mVArrayPtr->push_back(osg::Vec3(xyz[0], xyz[1], xyz[2]));
         ++vTotal;
      }
      mGeometryPtr->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, vStart, vTotal - vStart));
   }
}

void VaOverlayMapGrid::GenerateRectangularGrid(VaCamera* aCameraPtr)
{
   unsigned int i;
   unsigned int vStart = 0;
   unsigned int vTotal = 0;

   mAltOffset = 0.0;
   double alt = mAltOffset;

   double easternOffsetStart = aCameraPtr->GetEasternOffsetStart();
   double easternOffsetValue = aCameraPtr->GetEasternOffsetValue();
   double westernOffsetStart = aCameraPtr->GetWesternOffsetStart();
   double westernOffsetValue = aCameraPtr->GetWesternOffsetValue();

   // Generate lines of constant latitude.

   if (mCrossesDateline)
   {
      double minLon = mMinLon;
      double maxLon = mMaxLon;
      if (minLon > maxLon)
      {
         minLon = mMaxLon;
         maxLon = mMinLon;
      }
      double lon = 180.0;
      if (minLon < 0.0)
      {
         lon = -180.0;
      }
      GenerateLatLines(aCameraPtr, minLon, lon, vTotal);
      GenerateLatLines(aCameraPtr, -lon, maxLon, vTotal);
   }
   else if ((mMinLon < -30) && (mMaxLon > -30))
   {
      GenerateLatLines(aCameraPtr, mMinLon, -29.999, vTotal);
      GenerateLatLines(aCameraPtr, -30.001, mMaxLon - 0.001, vTotal);
   }
   else
   {
      GenerateLatLines(aCameraPtr, mMinLon, mMaxLon, vTotal);
   }

   // Generate lines of constant longitude.

   vStart = vTotal;
   for (i = 0; i < mNumLonLines; ++i)
   {
      double lon = mMinLon + i * mDeltaLon;
      while (lon > 180.0)
      {
         lon -= 360.0;
      }
      double xyz1[3];
      double xyz2[3];
      aCameraPtr->ConvertLLAToXYZ(xyz1, mMinLat, lon, alt);
      aCameraPtr->ConvertLLAToXYZ(xyz2, mMaxLat, lon, alt);
      if (lon > easternOffsetStart)
      {
         xyz1[0] += easternOffsetValue;
         xyz2[0] += easternOffsetValue;
      }
      else if (lon < westernOffsetStart)
      {
         xyz1[0] += westernOffsetValue;
         xyz2[0] += westernOffsetValue;
      }
      mVArrayPtr->push_back(osg::Vec3(xyz1[0], xyz1[1], xyz1[2]));
      mVArrayPtr->push_back(osg::Vec3(xyz2[0], xyz2[1], xyz2[2]));
      vTotal += 2;
   }
   mGeometryPtr->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, vStart, vTotal - vStart));
}

void VaOverlayMapGrid::GenerateLabels(VaCamera* aCameraPtr)
{
   std::set<size_t> toDel;
   for (auto&& it : mNodeMap)
   {
      toDel.insert(it.first);
   }
   mLabelGroup->removeChildren(0, mLabelGroup->getNumChildren());

   std::ostringstream oss;

   QFontMetrics fm(QFontDatabase::systemFont(QFontDatabase::FixedFont));
   double       metersPerPixel     = aCameraPtr->GetMetersPerPixel();
   double       metersPerDegreeLon = aCameraPtr->GetMetersPerDegreeLon();
   double       metersPerDegreeLat = aCameraPtr->GetMetersPerDegreeLat();
   double       latOffset;
   double       lonOffset;
   double       lat;
   double       lon;
   double       alt = fm.height() * metersPerPixel;
   unsigned int i;

   // clamp because if lower can't see them anyway
   if (metersPerDegreeLon < 1.0)
   {
      metersPerDegreeLon = 1.0;
   }

   // Locate the lat/lon grid lines that are closest to the view reference.

   double refLat;
   double refLon;
   aCameraPtr->GetLatLon(refLat, refLon);

   double midLat = mMinLat;
   for (i = 1; i < mNumLatLines; ++i)
   {
      lat = mMinLat + i * mDeltaLat;
      if (fabs(lat - refLat) < fabs(midLat - refLat))
      {
         midLat = lat;
      }
   }

   double midLon = mMinLon;
   if ((refLon < 0.0) && (midLon > 0.0))
   {
      refLon += 360.0;
   }
   for (i = 1; i < mNumLonLines; ++i)
   {
      lon = mMinLon + i * mDeltaLon;
      if (fabs(lon - refLon) < fabs(midLon - refLon))
      {
         midLon = lon;
      }
   }
   while (midLon > 180.0)
   {
      midLon -= 360.0;
   }
   while (midLon < -180.0)
   {
      midLon += 360.0;
   }

   double easternOffsetStart = aCameraPtr->GetEasternOffsetStart();
   double easternOffsetValue = aCameraPtr->GetEasternOffsetValue();
   double westernOffsetStart = aCameraPtr->GetWesternOffsetStart();
   double westernOffsetValue = aCameraPtr->GetWesternOffsetValue();

   double      xyz[3];
   std::string text;

   // Generate the labels for lines of constant latitude.

   lon       = midLon;
   latOffset = 5.0 * metersPerPixel / metersPerDegreeLat;
   lonOffset = 5.0 * metersPerPixel / metersPerDegreeLon;
   for (i = 0; i < mNumLatLines; ++i)
   {
      lat = mMinLat + i * mDeltaLat;

      oss << UtLatPos(lat, mLabelFormat) << std::ends;
      text = oss.str();
      oss.str(""); // seekp requires this (see "The C++ STL" by Josuttis)

      aCameraPtr->ConvertLLAToXYZ(xyz, lat + latOffset, lon + lonOffset, alt);
      if (lon < westernOffsetStart)
      {
         xyz[0] += westernOffsetValue;
      }
      else if (lon > easternOffsetStart)
      {
         xyz[0] += easternOffsetValue;
      }
      CreateLabelNode(text, xyz, toDel);
   }

   // Generate the labels for lines of constant longitude.

   lat       = midLat;
   latOffset = (5.0 + fm.height() - fm.descent()) * metersPerPixel / metersPerDegreeLat;

   for (i = 0; i < mNumLonLines; ++i)
   {
      lon = mMinLon + i * mDeltaLon;
      while (lon < -180.0)
      {
         lon += 360.0;
      }
      while (lon > 180.0)
      {
         lon -= 360.0;
      }

      oss << UtLonPos(lon, mLabelFormat) << std::ends;
      text = oss.str();
      oss.str(""); // seekp requires this (see "The C++ STL" by Josuttis)

      aCameraPtr->ConvertLLAToXYZ(xyz, lat - latOffset, lon + lonOffset, alt);
      if (lon < westernOffsetStart)
      {
         xyz[0] += westernOffsetValue;
      }
      else if (lon + lonOffset > easternOffsetStart)
      {
         xyz[0] += easternOffsetValue;
      }
      CreateLabelNode(text, xyz, toDel);
   }
   for (auto&& it : toDel)
   {
      mNodeMap.erase(it);
   }
}

void VaOverlayMapGrid::GenerateLatLines(VaCamera* aCameraPtr, double aMin, double aMax, unsigned int& aVTotal)
{
   double alt = mAltOffset;

   double easternOffsetStart = aCameraPtr->GetEasternOffsetStart();
   double easternOffsetValue = aCameraPtr->GetEasternOffsetValue();
   double westernOffsetStart = aCameraPtr->GetWesternOffsetStart();
   double westernOffsetValue = aCameraPtr->GetWesternOffsetValue();

   double offsetValue1 = 0.0;
   double offsetValue2 = 0.0;
   if (aMin > easternOffsetStart)
   {
      offsetValue1 = easternOffsetValue;
   }
   else if (aMin < westernOffsetStart)
   {
      offsetValue1 = westernOffsetValue;
   }
   if (aMax > easternOffsetStart)
   {
      offsetValue2 = easternOffsetValue;
   }
   else if (aMax < westernOffsetStart)
   {
      offsetValue2 = westernOffsetValue;
   }
   if ((offsetValue1 == 0.0) && (offsetValue2 != 0.0))
   {
      offsetValue1 = offsetValue2;
      offsetValue2 = 0.0;
   }
   unsigned int vStart = aVTotal;
   for (unsigned int i = 0; i < mNumLatLines; ++i)
   {
      double lat = mMinLat + i * mDeltaLat;
      double xyz1[3];
      double xyz2[3];
      aCameraPtr->ConvertLLAToXYZ(xyz1, lat, aMin, alt);
      aCameraPtr->ConvertLLAToXYZ(xyz2, lat, aMax, alt);
      if (offsetValue1 != offsetValue2)
      {
         mVArrayPtr->push_back(osg::Vec3(xyz1[0], xyz1[1], xyz1[2]));
         mVArrayPtr->push_back(osg::Vec3(xyz2[0], xyz2[1], xyz2[2]));
         aVTotal += 2;
      }
      xyz1[0] += offsetValue1;
      xyz2[0] += offsetValue1;

      mVArrayPtr->push_back(osg::Vec3(xyz1[0], xyz1[1], xyz1[2]));
      mVArrayPtr->push_back(osg::Vec3(xyz2[0], xyz2[1], xyz2[2]));
      aVTotal += 2;
   }
   mGeometryPtr->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, vStart, aVTotal - vStart));
}

void VaOverlayMapGrid::CreateLabelNode(const std::string& aText, const double aPosition[3], std::set<size_t>& aToDel)
{
   osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
   pat->setPosition(osg::Vec3(aPosition[0], aPosition[1], aPosition[2]));
   mLabelGroup->addChild(pat);

   size_t nodeId = std::hash<std::string>()(aText);
   auto&& it     = mNodeMap.find(nodeId);
   if (it != mNodeMap.end())
   {
      aToDel.erase(nodeId);
      pat->addChild(mNodeMap[nodeId]);
   }
   else
   {
      vespa::VaDecoratorNode* node = new vespa::VaDecoratorNode();
      node->SetText(aText.c_str(), QColor(0, 0, 0, 0), QColor(mColor[0] * 255, mColor[1] * 255, mColor[2] * 255));
      mNodeMap[nodeId] = node;
      pat->addChild(mNodeMap[nodeId]);
   }
}

void VaOverlayMapGrid::CameraUpdatedCB(VaViewer* aViewerPtr)
{
   if (GetViewer()->GetUniqueId() == aViewerPtr->GetUniqueId())
   {
      if (mGridDataGenerated)
      {
         GenerateGridData(GetViewer()->GetCamera());
      }
   }
}

void VaOverlayMapGrid::SetColor(const float* aColor)
{
   mColor[0] = aColor[0];
   mColor[1] = aColor[1];
   mColor[2] = aColor[2];
   mColor[3] = aColor[3];
   if (mCArrayPtr.valid())
   {
      mCArrayPtr->clear();
      mCArrayPtr->push_back(osg::Vec4(mColor[0], mColor[1], mColor[2], mColor[3]));
   }
   mLabelGroup->removeChildren(0, mLabelGroup->getNumChildren());
   mNodeMap.clear();
   GenerateGridData(GetViewer()->GetCamera());
}

void VaOverlayMapGrid::SetWidth(const float& aWidth)
{
   mWidth = aWidth;
   if (mMatrixPtr.valid())
   {
      osg::LineWidth* lw =
         dynamic_cast<osg::LineWidth*>(mMatrixPtr->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINEWIDTH));
      if (lw)
      {
         lw->setWidth(mWidth);
      }
   }
}

void VaOverlayMapGrid::SetDensityFactor(float aValue)
{
   mDensity = aValue;
   GenerateGridData(GetViewer()->GetCamera());
}

} // namespace vespa
