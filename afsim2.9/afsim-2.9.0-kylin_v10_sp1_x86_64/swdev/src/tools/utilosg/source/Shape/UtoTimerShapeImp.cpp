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

/////////////////////////////////////////////////////////////////////////////
// UtoTimerShapeImp.cpp : implementation file

#include "UtoTimerShapeImp.hpp"

#include <osg/Geode>
#include <osg/Geometry>

#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtoShape.hpp"
#include "UtoTimerShape.hpp"

UtoTimerShapeImp::UtoTimerShapeImp()
   : mStartTime(0.0)
   , mDuration(0.0)
   , mCurrentTime(0.0)
   , mStyle(UtoTimerShape::STYLE_ANALOG1)
{
   mGeode = new osg::Geode();
   mGeode->setName("UtoTimerShape");
   m_RootNode->addChild(mGeode.get());
   SetShapeFlags(UtoShape::AUTO_SCALE | UtoShape::BILLBOARD, UtoShape::AUTO_SCALE | UtoShape::BILLBOARD);
   BuildGeometry();
}


UtoTimerShapeImp::UtoTimerShapeImp(const UtoTimerShapeImp& aRHS)
   : UtoShapeImp(aRHS)
   , mStartTime(0.0)
   , mDuration(0.0)
   , mCurrentTime(0.0)
   , mSize(20)
   , mStyle(UtoTimerShape::STYLE_ANALOG1)
{
   mGeode = new osg::Geode;
   mGeode->setName("UtoTimerShape");
   m_RootNode->addChild(mGeode.get());
   SetShapeFlags(UtoShape::AUTO_SCALE | UtoShape::BILLBOARD, UtoShape::AUTO_SCALE | UtoShape::BILLBOARD);
   BuildGeometry();
}


UtoTimerShapeImp::~UtoTimerShapeImp() {}

// virtual
void UtoTimerShapeImp::BuildGeometry()
{
   int drawableCount = mGeode->getNumDrawables();
   mGeode->removeDrawables(0, drawableCount);

   if (mStyle == UtoTimerShape::STYLE_ANALOG1)
   {
      osg::ref_ptr<osg::Geometry> bggeom;
      bggeom = new osg::Geometry;
      bggeom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      osg::ref_ptr<osg::Geometry> fggeom;
      fggeom = new osg::Geometry;
      fggeom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

      osg::Vec3Array* bgVertArray  = new osg::Vec3Array;
      osg::Vec4Array* bgColorArray = new osg::Vec4Array;
      osg::Vec3Array* fgVertArray  = new osg::Vec3Array;
      osg::Vec4Array* fgColorArray = new osg::Vec4Array;

      bgColorArray->push_back(osg::Vec4(1.0, 1.0, 1.0, Color().AlphaF()));
      fgColorArray->push_back(osg::Vec4(Color().RedF(), Color().GreenF(), Color().BlueF(), Color().AlphaF()));
      bggeom->setVertexArray(bgVertArray);
      bggeom->setColorArray(bgColorArray);
      bggeom->setColorBinding(osg::Geometry::BIND_OVERALL);
      fggeom->setVertexArray(fgVertArray);
      fggeom->setColorArray(fgColorArray);
      fggeom->setColorBinding(osg::Geometry::BIND_OVERALL);

      int index   = 0;
      int fgindex = 0;
      int iang;
      int fgstop = 20 * (mCurrentTime - mStartTime) / mDuration;

      fgVertArray->push_back(osg::Vec3(0.0f, 0.0f, 10.1f));
      ++fgindex;

      for (iang = 0; iang < 20; ++iang)
      {
         float ang  = index / 20.0 * UtMath::cTWO_PI;
         float sang = sin(ang);
         float cang = cos(ang);
         bgVertArray->push_back(osg::Vec3(mSize * sang, mSize * cang, 10.0f));
         ++index;
         if (iang <= fgstop)
         {
            fgVertArray->push_back(osg::Vec3(mSize * 0.9f * sang, mSize * 0.9f * cang, 10.1f));
            ++fgindex;
         }
      }
      float stopang = (mCurrentTime - mStartTime) / mDuration * UtMath::cTWO_PI;
      float sang    = sin(stopang);
      float cang    = cos(stopang);
      fgVertArray->push_back(osg::Vec3(mSize * 0.9f * sang, mSize * 0.9f * cang, 10.1f));
      ++fgindex;

      bggeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, index));
      fggeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN, 0, fgindex));
      mGeode->addDrawable(bggeom.get());
      mGeode->addDrawable(fggeom.get());
   }
   else if (mStyle == UtoTimerShape::STYLE_HBAR1)
   {
      osg::ref_ptr<osg::Geometry> bggeom;
      bggeom = new osg::Geometry;
      bggeom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      osg::ref_ptr<osg::Geometry> fggeom;
      fggeom = new osg::Geometry;
      fggeom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

      osg::Vec3Array* bgVertArray  = new osg::Vec3Array;
      osg::Vec4Array* bgColorArray = new osg::Vec4Array;
      osg::Vec3Array* fgVertArray  = new osg::Vec3Array;
      osg::Vec4Array* fgColorArray = new osg::Vec4Array;

      bgColorArray->push_back(osg::Vec4(1.0, 1.0, 1.0, Color().AlphaF()));
      fgColorArray->push_back(osg::Vec4(Color().RedF(), Color().GreenF(), Color().BlueF(), Color().AlphaF()));
      bggeom->setVertexArray(bgVertArray);
      bggeom->setColorArray(bgColorArray);
      bggeom->setColorBinding(osg::Geometry::BIND_OVERALL);
      fggeom->setVertexArray(fgVertArray);
      fggeom->setColorArray(fgColorArray);
      fggeom->setColorBinding(osg::Geometry::BIND_OVERALL);

      float factor    = (mCurrentTime - mStartTime) / mDuration;
      float rightedge = factor * mSize * 0.9 + (1 - factor) * -mSize * 0.9;
      int   index     = 0;
      bgVertArray->push_back(osg::Vec3(-mSize, -0.2f * mSize, 10.0f));
      fgVertArray->push_back(osg::Vec3(-mSize * 0.9f, -0.2f * 0.8f * mSize, 10.1f));
      ++index;
      bgVertArray->push_back(osg::Vec3(-mSize, 0.2f * mSize, 10.0f));
      fgVertArray->push_back(osg::Vec3(-mSize * 0.9f, 0.2f * 0.8f * mSize, 10.1f));
      ++index;
      bgVertArray->push_back(osg::Vec3(mSize, 0.2f * mSize, 10.0f));
      fgVertArray->push_back(osg::Vec3(rightedge, 0.2f * 0.8f * mSize, 10.1f));
      ++index;
      bgVertArray->push_back(osg::Vec3(mSize, 0.2f * -mSize, 10.0f));
      fgVertArray->push_back(osg::Vec3(rightedge, -0.2f * 0.8f * mSize, 10.1f));
      ++index;

      bggeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, index));
      fggeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, index));
      mGeode->addDrawable(bggeom.get());
      mGeode->addDrawable(fggeom.get());
   }
}

// virtual
void UtoTimerShapeImp::SetColor(const UtoColor& aColor, unsigned int aClrMask, unsigned int aReserved /* = 0  */)
{
   UtoShapeImp::SetColor(aColor, aClrMask, aReserved);
   BuildGeometry();
}

void UtoTimerShapeImp::SetTiming(double aStartTime, double aDuration)
{
   mStartTime = aStartTime;
   mDuration  = aDuration;
   BuildGeometry();
}

void UtoTimerShapeImp::UpdateTime(double aTime)
{
   mCurrentTime = aTime;
   BuildGeometry();
}

void UtoTimerShapeImp::SetSize(float aSize)
{
   mSize = aSize;
   BuildGeometry();
}
