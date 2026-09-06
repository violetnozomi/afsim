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

#include "VaOverlayShapeFile.hpp"

#include <sstream>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>

#include "GeoShapeFile.hpp"
#include "UtLog.hpp"
#include "UtoRawShape.hpp"
#include "VaCamera.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

namespace vespa
{

VaOverlayShapeFile::VaOverlayShapeFile()
   : VaOverlay("shapeFile", cOVERLAY_WORLD)
   , mShapePtr(nullptr)
   , mShapeName("")
   , mFilename("")
{
   SetType<VaOverlayShapeFile>();
   mColor[0] = 1;
   mColor[1] = 0;
   mColor[2] = 0;
   mWidth    = 2;
}


VaOverlayShapeFile::VaOverlayShapeFile(const std::string& aName)
   : VaOverlay(aName, cOVERLAY_WORLD)
   , mShapePtr(nullptr)
   , mShapeName("")
   , mFilename("")
{
   SetType<VaOverlayShapeFile>();
   mColor[0] = 1;
   mColor[1] = 0;
   mColor[2] = 0;
   mWidth    = 2;
}

VaOverlayShapeFile* VaOverlayShapeFile::Clone() const
{
   return new VaOverlayShapeFile(*this);
}

bool VaOverlayShapeFile::Initialize()
{
   bool initialized = false;

   std::ostringstream  oss;
   static unsigned int uid = 0;
   oss << "VaOverlayShapeFile"
       << "_" << uid++;
   mShapeName = oss.str().c_str();

   UtoRawShape shape;
   mShapePtr = dynamic_cast<UtoRawShape*>(BindShape(mShapeName, shape));
   if (mShapePtr == nullptr)
   {
      ut::log::warning() << "Could not bind shape: compass";
   }

   if (mShapePtr != nullptr)
   {
      SetVisible(true);
      initialized = true;
   }

   return initialized;
}

VaOverlayShapeFile::~VaOverlayShapeFile()
{
   UnbindShape(mShapeName);
}

void VaOverlayShapeFile::LoadFile(const std::string& aFilename)
{
   mFilename          = aFilename;
   unsigned int pos   = 0;
   unsigned int start = 0;
   UtoPosition  segment[2];
   bool         geocentric = (GetViewer()->GetMapProjection().projection == UtoMapProjection::Geocentric);

   GeoShapeFile gsf(aFilename, "", 0.0, "", 0.0, geocentric, true);
   if (!gsf.IsValid())
   {
      VaObserver::UserMessage(1, gsf.GetErrorMessage());
      return;
   }

   const auto& gs = gsf.GetShapeList();
   double      cxyz[3];
   gsf.GetCenter(cxyz[0], cxyz[1], cxyz[2]);

   osg::Group* root = mShapePtr->GetRoot();


   if (!mGeode.valid())
   {
      if (geocentric)
      {
         osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
         pat->setPosition(osg::Vec3d(cxyz[0], cxyz[1], cxyz[2]));
         root->addChild(pat.get());
         root = pat;
      }
      mCallbacks.Add(vespa::VaObserver::MapProjectionChanged.Connect(&VaOverlayShapeFile::MapProjectionChangedCB,
                                                                     this,
                                                                     GetViewer()->GetUniqueId()));
      mGeode = new osg::Geode;
      mGeode->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
      mGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      root->addChild(mGeode.get());
   }
   else if (mGeode.valid())
   {
      if (geocentric)
      {
         osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*>(root->getChild(0));
         if (pat != nullptr)
         {
            pat->setPosition(osg::Vec3d(cxyz[0], cxyz[1], cxyz[2]));
            root = pat;
         }
      }
      else
      {
         osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*>(root->getChild(0));
         if (pat != nullptr)
         {
            pat->setPosition(osg::Vec3d(0, 0, 0));
            root = pat;
         }
      }
      int drawableCount = mGeode->getNumDrawables();
      mGeode->removeDrawables(0, drawableCount);
   }
   osg::ref_ptr<osg::Geometry> myGeometry = new osg::Geometry;
   mGeode->addDrawable(myGeometry.get());


   osg::Vec3Array* vrtArray = new osg::Vec3Array;
   osg::Vec4Array* colArray = new osg::Vec4Array;

   myGeometry->setVertexArray(vrtArray);
   myGeometry->setColorArray(colArray);
   myGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   colArray->push_back(osg::Vec4(mColor[0], mColor[1], mColor[2], 1));

   for (auto&& shape : gs)
   {
      for (auto&& part : shape.GetPartList())
      {
         if ((shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolygon) ||
             (shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolygonM) ||
             (shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolygonZ) ||
             (shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolyLine) ||
             (shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolyLineM) ||
             (shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolyLineZ) ||
             (shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypeMultiPatch))
         {
            // NOTE: this may not strictly draw right for triangle fan and triangle strip part types in the multipatch shapes
            auto firstspit = part.mPointList.begin();
            auto thisspit  = firstspit;
            auto nextspit  = thisspit;
            if (firstspit != part.mPointList.end())
            {
               ++nextspit;
            }

            bool broken = false;
            while (nextspit != part.mPointList.end())
            {
               VTransform(segment[0], *thisspit, cxyz, geocentric);
               VTransform(segment[1], *nextspit, cxyz, geocentric);
               double fullCircumference = 360.0 * UtoMapProjection::msEqrFactor;

               if ((!geocentric) &&
                   (fabs(segment[0].x() - segment[1].x()) > 0.5 * fullCircumference)) // if the segment spans 180 deg,
                                                                                      // it should cross the date line
               {
                  osg::Vec3 brokenPoint;
                  broken       = true;
                  double    t  = 0.0;
                  osg::Vec3 s2 = segment[1];
                  if (segment[0].x() > 0)
                  {
                     s2.x() += fullCircumference;
                     t           = (fullCircumference * 0.5 - 0.1 + segment[0].x()) / (segment[0].x() + s2.x());
                     brokenPoint = segment[0] * (1.0 - t) + s2 * t;
                  }
                  else
                  {
                     s2.x() -= fullCircumference;
                     t           = (-fullCircumference * 0.5 + 0.1 + segment[0].x()) / (segment[0].x() + s2.x());
                     brokenPoint = segment[0] * t + s2 * (1.0 - t);
                  }
                  vrtArray->push_back(osg::Vec3d(segment[0]));
                  vrtArray->push_back(osg::Vec3d(brokenPoint));
                  pos += 2;

                  // add a break point just before the dateline (on the appropriate side)
                  myGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, start, pos));
                  start += pos;
                  brokenPoint.x() = -brokenPoint.x();
                  vrtArray->push_back(osg::Vec3d(brokenPoint));
                  pos = 1;
               }
               else
               {
                  vrtArray->push_back(osg::Vec3d(segment[0]));
                  vrtArray->push_back(osg::Vec3d(segment[1]));
                  pos += 2;
               }

               ++thisspit;
               ++nextspit;
            }

            if ((shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolygon) ||
                (shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolygonM) ||
                (shape.GetShapeType() == GeoShapeFile::Shape::ShapeTypePolygonZ) ||
                (shape.GetShapeType() ==
                 GeoShapeFile::Shape::ShapeTypeMultiPatch)) // if it is a polygon, we will want it closed
            {
               if (broken)
               {
                  myGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, start, pos));
                  start += pos;
                  pos = 0;
               }
               else
               {
                  myGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, start, pos));
                  start += pos;
                  pos = 0;
               }
            }
            else
            {
               if (pos == 1) // degenerate case
               {
                  vrtArray->pop_back();
                  pos = 0;
               }
               else
               {
                  myGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, start, pos));
                  start += pos;
                  pos = 0;
               }
            }
         }
      }
   }
   mGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(mWidth));
}

void VaOverlayShapeFile::VTransform(UtoPosition&                  aOutput,
                                    const GeoShapeFile::Position& aInput,
                                    const double                  aTransform[3],
                                    bool                          aGeocentric)
{
   if (aGeocentric)
   {
      aOutput.x() = aInput.x - aTransform[0];
      aOutput.y() = aInput.y - aTransform[1];
      aOutput.z() = aInput.z - aTransform[2];
   }
   else
   {
      double     dxyz[3];
      VaPosition pos(aInput.x, aInput.y, aInput.z + 100);
      pos.GetSceneXYZ(dxyz, GetViewer()->GetUniqueId());
      aOutput.x() = dxyz[0];
      aOutput.y() = dxyz[1];
      aOutput.z() = dxyz[2];
   }
}

void VaOverlayShapeFile::SetColor(const float* aColor)
{
   mColor[0] = aColor[0];
   mColor[1] = aColor[1];
   mColor[2] = aColor[2];

   if (nullptr != mGeode)
   {
      osg::Geometry* myGeometry = dynamic_cast<osg::Geometry*>(mGeode->getDrawable(0));
      if (nullptr != myGeometry)
      {
         osg::Vec4Array* colArray = new osg::Vec4Array;

         colArray->push_back(osg::Vec4(aColor[0], aColor[1], aColor[2], 1.0));

         myGeometry->setColorArray(colArray);
         myGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
      }
   }
}

UtColor VaOverlayShapeFile::GetColor()
{
   UtColor result;

   osg::Geometry* myGeometry = dynamic_cast<osg::Geometry*>(mGeode->getDrawable(0));
   if (nullptr != myGeometry)
   {
      osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(myGeometry->getColorArray());
      osg::Vec4f      color  = colors[0][0];

      result.Set(color[0], color[1], color[2], color[3]);
   }

   return result;
}

void VaOverlayShapeFile::SetColor(const UtColor& aColor)
{
   osg::Geometry* myGeometry = dynamic_cast<osg::Geometry*>(mGeode->getDrawable(0));
   float          a;
   aColor.Get(mColor, a);
   if (nullptr != myGeometry)
   {
      osg::Vec4Array* colArray = new osg::Vec4Array;

      float r, g, b, a;
      aColor.Get(r, g, b, a);
      colArray->push_back(osg::Vec4(r, g, b, a));

      myGeometry->setColorArray(colArray);
      myGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
      myGeometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
   }
}

void VaOverlayShapeFile::SetWidth(const float& aWidth)
{
   mWidth = aWidth;

   if (mGeode.valid())
   {
      mGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(aWidth));
   }
}


void VaOverlayShapeFile::MapProjectionChangedCB(unsigned int aViewer, const UtoMapProjection* aProjection)
{
   LoadFile(mFilename);
}

} // namespace vespa
