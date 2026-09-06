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

#include "VaOverlayMapRings.hpp"

#include <sstream>

#include <QColor>
#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtoRawShape.hpp"
#include "VaCamera.hpp"
#include "VaDecoratorNode.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

namespace vespa
{

VaOverlayMapRings::VaOverlayMapRings()
   : VaOverlay("maprings", cOVERLAY_WORLD)
   , mWidth(1.0f)
   , mFormat(UtLength::FmtNM)
   , mRingDataGenerated(false)
   , mUserToMeters(UtMath::cM_PER_NM)
{
   SetType<VaOverlayMapRings>();
   mColor[0] = 0.0f;
   mColor[1] = 0.0f;
   mColor[2] = 0.0f;
   mColor[3] = 1.0f;
}


VaOverlayMapRings::VaOverlayMapRings(const std::string& aName)
   : VaOverlay(aName, cOVERLAY_WORLD)
   , mWidth(1.0f)
   , mFormat(UtLength::FmtNM)
   , mRingDataGenerated(false)
   , mUserToMeters(UtMath::cM_PER_NM)
{
   SetType<VaOverlayMapRings>();
   mColor[0] = 0.0f;
   mColor[1] = 0.0f;
   mColor[2] = 0.0f;
   mColor[3] = 1.0f;
}

VaOverlayMapRings* VaOverlayMapRings::Clone() const
{
   return new VaOverlayMapRings(*this);
}

bool VaOverlayMapRings::Initialize()
{
   bool initialized = false;

   UtoRawShape shapePtr;
   mShapePtr = dynamic_cast<UtoRawShape*>(BindShape("maprings", shapePtr));
   if (mShapePtr == nullptr)
   {
      ut::log::warning() << "Could not bind shape: map range rings";
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
   mLabelGroup = new osg::Group;
   mMatrixPtr->addChild(mLabelGroup);
   osg::Geode* geode = new osg::Geode;
   mMatrixPtr->addChild(geode);
   mGeometryPtr = new osg::Geometry;
   geode->addDrawable(mGeometryPtr.get());

   mVArrayPtr = new osg::Vec3Array;
   mCArrayPtr = new osg::Vec4Array;
   mGeometryPtr->setVertexArray(mVArrayPtr.get());
   mGeometryPtr->setColorArray(mCArrayPtr.get());
   mCArrayPtr->push_back(osg::Vec4(mColor[0], mColor[1], mColor[2], mColor[3]));
   mGeometryPtr->setColorBinding(osg::Geometry::BIND_OVERALL);

   if (!mRingDataGenerated)
   {
      mCallbacks.Add(VaObserver::CameraUpdated.Connect(&VaOverlayMapRings::CameraUpdatedCB, this));
      mCallbacks.Add(VaObserver::ChangedUnitFormat.Connect(&VaOverlayMapRings::UnitFormatChangedCB, this));
   }

   GenerateRingData(GetViewer()->GetCamera());

   return initialized;
}

void VaOverlayMapRings::GenerateRingData(VaCamera* aCameraPtr)
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

   // Compute the distance in world coordinates from the center of the viewport
   // to the closest edge.

   unsigned int vpWidth;
   unsigned int vpHeight;
   aCameraPtr->GetViewport(vpWidth, vpHeight);

   double metersPerPixel = aCameraPtr->GetMetersPerPixel();
   double rangeToEdge    = 0.5 * metersPerPixel * std::min(vpWidth, vpHeight);
   double rangeToCorner =
      0.5 * metersPerPixel * std::sqrt(static_cast<double>((vpWidth * vpWidth) + (vpHeight * vpHeight)));

   // Clamp the range to 1/4 of the earth's circumference

   rangeToEdge   = std::min(rangeToEdge, 11000000.0);
   rangeToCorner = std::min(rangeToCorner, 11000000.0);

   // Determine a nice increment.
   double ringDelta = ComputeDelta(rangeToEdge / mUserToMeters);

   // Draw as many rings as necessary to fill the viewport

   int ringCount = static_cast<int>((1.001 * rangeToCorner / mUserToMeters) / ringDelta);
   mFormat -= mFormat & UtReal::FmtFracMask; // clear our the precision values before we add new ones
   if (ringDelta < 0.001)
   {
      mFormat += 4;
   }
   else if (ringDelta < 0.01)
   {
      mFormat += 3;
   }
   else if (ringDelta < 0.1)
   {
      mFormat += 2;
   }
   else if (ringDelta < 1.0)
   {
      mFormat += 1;
   }

   // Determine the center of the range rings - either a user-specified value
   // or the current camera center.

   double centerLat;
   double centerLon;
   aCameraPtr->GetLatLon(centerLat, centerLon);

   // Generate the display data...

   // Generate radial lines

   int    segCount = static_cast<int>(ringDelta * mUserToMeters / 200000.0) + 1;
   double segDelta = ringDelta / segCount;

   // For non-flat maps, adjust the altitude to the generated lines go skim the
   // surface rather than cutting through.

   double altOffset = 1.0;
   if (!aCameraPtr->IsFlat())
   {
      double xyz[3];
      aCameraPtr->ConvertLLAToXYZ(xyz, 0.0, 0.0, 0.0);
      double earthRadius = sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]);
      double theta       = (segDelta * mUserToMeters) / earthRadius;
      altOffset          = earthRadius * (1.0 / cos(0.5 * theta) - 1.0);
      altOffset          = std::max(1.0, altOffset);
   }

   int          intAngle;
   double       lat;
   double       lon;
   double       alt    = altOffset;
   unsigned int vTotal = 0;
   unsigned int vStart;
   double       xyz[3];

   for (intAngle = 0; intAngle < 360; intAngle += 30)
   {
      vStart       = vTotal;
      double angle = intAngle;
      for (int segIndex = 0; segIndex <= (ringCount * segCount); ++segIndex)
      {
         double range = segIndex * segDelta;
         UtSphericalEarth::ExtrapolateGreatCirclePosition(centerLat, centerLon, angle, range * mUserToMeters, lat, lon);
         aCameraPtr->ConvertLLAToXYZ(xyz, lat, lon, alt);
         mVArrayPtr->push_back(osg::Vec3(xyz[0], xyz[1], xyz[2]));
         vTotal++;
      }
      mGeometryPtr->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, vStart, vTotal - vStart));
   }

   // Generate circumferential rings

   int ringIndex;
   for (ringIndex = 1; ringIndex <= ringCount; ++ringIndex)
   {
      vStart       = vTotal;
      double range = ringIndex * ringDelta;

      // Compute an angular step that will render the circle with reasonable
      // accuracy.  Big circles will have more points.

      unsigned int angleDelta = 5; // assume a big circle
      double       circum     = UtMath::cTWO_PI * range * mUserToMeters;
      if (circum / 36.0 < 200000.0)
      {
         angleDelta = 10;
      }
      unsigned int pointsOnRing = 360 / angleDelta;
      for (unsigned int angleIndex = 0; angleIndex < pointsOnRing; ++angleIndex)
      {
         double angle = static_cast<double>(angleIndex * angleDelta);
         UtSphericalEarth::ExtrapolateGreatCirclePosition(centerLat, centerLon, angle, range * mUserToMeters, lat, lon);
         aCameraPtr->ConvertLLAToXYZ(xyz, lat, lon, alt);
         mVArrayPtr->push_back(osg::Vec3(xyz[0], xyz[1], xyz[2]));
         vTotal++;
      }
      mGeometryPtr->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, vStart, vTotal - vStart));
   }

   std::set<size_t> toDel;
   for (auto&& it : mNodeMap)
   {
      toDel.insert(it.first);
   }
   mLabelGroup->removeChildren(0, mLabelGroup->getNumChildren());

   // Generate the labels
   std::ostringstream oss;

   double      latOffset = 5.0 * metersPerPixel / aCameraPtr->GetMetersPerDegreeLon();
   double      lonOffset = 5.0 * metersPerPixel / aCameraPtr->GetMetersPerDegreeLat();
   std::string text;

   for (ringIndex = 1; ringIndex <= ringCount; ++ringIndex)
   {
      double range = ringIndex * ringDelta;
      ;

      oss << UtLength(range * mUserToMeters, mFormat) << std::ends;
      text = oss.str().c_str();
      oss.str(""); // seekp requires this (see "The C++ STL" by Josuttis)

      UtSphericalEarth::ExtrapolateGreatCirclePosition(centerLat, centerLon, 0.0, range * mUserToMeters, lat, lon);
      aCameraPtr->ConvertLLAToXYZ(xyz, lat + latOffset, lon + lonOffset, alt);
      CreateLabelNode(text, xyz, toDel);
   }

   for (auto&& it : toDel)
   {
      mNodeMap.erase(it);
   }
   mRingDataGenerated = true;
}

double VaOverlayMapRings::ComputeDelta(double aRange) const
{
   // Choose a delta that will give 5 rings

   double delta      = aRange / 5.0;
   double finalDelta = delta;

   bool done = false;
   while (!done)
   {
      double logDelta = log10(std::max(delta, 0.000001));
      int    exponent = static_cast<int>(logDelta);
      if (exponent > logDelta)
      {
         --exponent;
      }
      double mantissa = delta / pow(10.0, static_cast<double>(exponent));
      mantissa        = static_cast<int>(std::max(mantissa, 1.0));
      finalDelta      = mantissa * pow(10.0, static_cast<double>(exponent));
      if (static_cast<int>(aRange / finalDelta) > 6)
      {
         delta *= 1.25;
      }
      else
      {
         done = true;
      }
   }
   return finalDelta;
}

void VaOverlayMapRings::CreateLabelNode(const std::string& aText, const double aPosition[3], std::set<size_t>& aToDel)
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

void VaOverlayMapRings::CameraUpdatedCB(VaViewer* aViewerPtr)
{
   if (GetViewer()->GetUniqueId() == aViewerPtr->GetUniqueId())
   {
      if (mRingDataGenerated)
      {
         GenerateRingData(aViewerPtr->GetCamera());
      }
   }
}

void VaOverlayMapRings::SetColor(const float* aColor)
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
   GenerateRingData(GetViewer()->GetCamera());
}

void VaOverlayMapRings::SetWidth(const float& aWidth)
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

void VaOverlayMapRings::UnitFormatChangedCB(int aMeasureType, int aUnit)
{
   if (aMeasureType == VaEnvironment::DistanceMeasure)
   {
      mFormat = aUnit;
      switch (mFormat & UtReal::FmtUnitMask)
      {
      case UtLength::FmtM:
         mUserToMeters = 1.0;
         break;
      case UtLength::FmtKM:
         mUserToMeters = 1000.0;
         break;
      case UtLength::FmtFT:
         mUserToMeters = UtMath::cM_PER_FT;
         break;
      case UtLength::FmtMI:
         mUserToMeters = UtMath::cM_PER_MI;
         break;
      case UtLength::FmtNM:
         mUserToMeters = UtMath::cM_PER_NM;
         break;
      default:
         mFormat       = UtLength::FmtNM;
         mUserToMeters = UtMath::cM_PER_NM;
         break;
      }
   }
}

} // namespace vespa
