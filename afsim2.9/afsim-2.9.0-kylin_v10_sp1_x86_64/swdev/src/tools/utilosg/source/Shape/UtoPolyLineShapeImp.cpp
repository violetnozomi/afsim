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
// UtoPolyLineShapeImp.cpp : implementation file

#include "UtoPolyLineShapeImp.hpp"

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PrimitiveSet>

#include "UtCast.hpp"
#include "UtoPolyLineShape.hpp"

UtoPolyLineShapeImp::UtoPolyLineShapeImp()
   : m_AntiAliasLines(false)
   , mDepthBin(-1000)
   , mUseNestedBins(false)
   , mUsingSides(false)
   , m_DepthTest(true)
{
   m_PrimitiveSet->setMode(osg::PrimitiveSet::LINE_STRIP);
   m_Geode->setName("UtoPolyLineShape");

   // Register the heading transform at 0 deg
   mOrientationTransform = new osg::MatrixTransform;
   mOrientationTransform->addChild(m_Geode.get());
   RemoveShapeChildNode(m_Geode.get());
   AddShapeChildNode(mOrientationTransform);
}

UtoPolyLineShapeImp::UtoPolyLineShapeImp(const UtoPosition pts[], int num)
   : m_AntiAliasLines(false)
   , mDepthBin(-1000)
   , mUseNestedBins(false)
   , mUsingSides(false)
   , m_DepthTest(true)
{
   m_PrimitiveSet->setMode(osg::PrimitiveSet::LINE_STRIP);
   m_Geode->setName("UtoPolyLineShape");
   Insert(0, pts, num);

   // Register the heading transform at 0 deg
   mOrientationTransform = new osg::MatrixTransform;
   mOrientationTransform->addChild(m_Geode.get());
   RemoveShapeChildNode(m_Geode.get());
   AddShapeChildNode(mOrientationTransform);
}

UtoPolyLineShapeImp::UtoPolyLineShapeImp(const UtoPolyLineShapeImp& rhs)
   : UtoPolyShapeImp(rhs)
   , m_AntiAliasLines(rhs.m_AntiAliasLines)
   , mDepthBin(rhs.mDepthBin)
   , mUseNestedBins(rhs.mUseNestedBins)
   , mUsingSides(rhs.mUsingSides)
   , m_DepthTest(rhs.m_DepthTest)
{
   m_Geode->setName("UtoPolyLineShape");

   // Register the heading transform at 0 deg
   mOrientationTransform = new osg::MatrixTransform;
   mOrientationTransform->addChild(m_Geode.get());
   RemoveShapeChildNode(m_Geode.get());
   AddShapeChildNode(mOrientationTransform);
}

UtoPolyLineShapeImp::~UtoPolyLineShapeImp() {}

osg::ref_ptr<osg::StateSet> g_pPolyLineShapeStateSet;

void UtoPolyLineShapeImp::Update(osg::NodeVisitor* nv, osg::Drawable* drawable)
{
   mDrawTangential = true;
   UtoGeometryShapeImp::Update(nv, drawable);

   UtoPosition s          = Sides();
   bool        wantsSides = ((s.x() != 0) || (s.y() != 0) || (s.z() != 0));

   if (mDepthBin != -1000 || m_AntiAliasLines || wantsSides != mUsingSides || !m_DepthTest)
   {
      // Make the shading be flat.  If Sides are enabled the state set will
      // be overridden in the base Update as the lighting will be required.
      // This just allows the lighting to be again disabled if the sides are
      // disabled.
      osg::StateSet* ss = new osg::StateSet;
      m_Geometry->setStateSet(ss);
      if (!wantsSides)
      {
         ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      }
      if (m_AntiAliasLines)
      {
         ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
         ss->setAttributeAndModes(new osg::BlendFunc(), osg::StateAttribute::ON);
         // Depth writes will interfere with smooth line drawing
         osg::Depth* depthPtr = new osg::Depth();
         depthPtr->setWriteMask(false);
         ss->setAttributeAndModes(depthPtr, osg::StateAttribute::ON);
      }
      else
      {
         ss->removeAttribute(osg::StateAttribute::DEPTH);
         ss->removeAttribute(osg::StateAttribute::BLENDFUNC);
         ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::OFF);

         if (!m_Geometry->getStateSet())
         {
            m_Geometry->setStateSet(GetDefaultStateset());
         }
      }
      if (mDepthBin != -1000)
      {
         if (mDepthBin == 12)
            ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL), osg::StateAttribute::ON);
         ss->setRenderBinDetails(mDepthBin, "DepthSortedBin");
         ss->setNestRenderBins(mUseNestedBins);
      }
      ss->setMode(GL_DEPTH_TEST, m_DepthTest ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
   }
   else
   {
      m_Geometry->setStateSet(GetDefaultStateset());
   }
}

unsigned int UtoPolyLineShapeImp::Aggregation() const
{
   unsigned int mode = m_PrimitiveSet->getMode();
   unsigned int aggregationType;
   switch (mode)
   {
   case osg::PrimitiveSet::LINE_STRIP:
      aggregationType = UtoPolyLineShape::LINE_STRIP;
      break;
   case osg::PrimitiveSet::LINES:
      aggregationType = UtoPolyLineShape::LINES;
      break;
   case osg::PrimitiveSet::LINE_LOOP:
      aggregationType = UtoPolyLineShape::LINE_LOOP;
      break;
   default:
      aggregationType = UtoPolyLineShape::LINE_STRIP;
      break;
   }
   return aggregationType;
}

void UtoPolyLineShapeImp::SetAggregation(unsigned int aggregation)
{
   switch (aggregation)
   {
   case UtoPolyLineShape::LINE_STRIP:
      m_PrimitiveSet->setMode(osg::PrimitiveSet::LINE_STRIP);
      break;
   case UtoPolyLineShape::LINES:
      m_PrimitiveSet->setMode(osg::PrimitiveSet::LINES);
      break;
   case UtoPolyLineShape::LINE_LOOP:
      m_PrimitiveSet->setMode(osg::PrimitiveSet::LINE_LOOP);
      break;
   default:
      m_PrimitiveSet->setMode(osg::PrimitiveSet::LINE_STRIP);
      break;
   }
}

void UtoPolyLineShapeImp::SetOrientationTransform(const UtoOrientation& aOrientation)
{
   mOrientationTransform->setMatrix(osg::Matrix::rotate(aOrientation.yaw,
                                                        osg::Vec3d(0, 0, 1),
                                                        aOrientation.pitch,
                                                        osg::Vec3d(1, 0, 0),
                                                        aOrientation.roll,
                                                        osg::Vec3d(0, 1, 0)));
}

void UtoPolyLineShapeImp::EnableAntiAliasLines(bool enable)
{
   if (m_AntiAliasLines != enable)
   {
      m_AntiAliasLines = enable;
      NeedCallback();
   }
}

void UtoPolyLineShapeImp::SetDepthBin(int depthBin, bool useNestedBins)
{
   mDepthBin      = depthBin;
   mUseNestedBins = useNestedBins;
   NeedCallback();
}

void UtoPolyLineShapeImp::UnsetDepthBin()
{
   mDepthBin = -1000;
   NeedCallback();
}

void UtoPolyLineShapeImp::AddSidesPostOffset(bool close)
{
   if (!m_Vertex->empty())
   {
      // Add a primtive set that draws vertical lines from each vertex to the corresponding offset vertex.
      const unsigned int       count = m_Vertex->size() / 2U;
      osg::DrawElementsUShort* elem  = new osg::DrawElementsUShort(osg::PrimitiveSet::LINES, 2U * count);
      m_Geometry->addPrimitiveSet(elem);
      int                      o = 0;
      osg::DrawElementsUShort& e = *elem;
      for (unsigned int i = 0U; i < count; ++i)
      {
         e[o++] = ut::safe_cast<GLushort, unsigned int>(i);
         e[o++] = ut::safe_cast<GLushort, unsigned int>(i + count);
      }
   }
}


osg::StateSet* UtoPolyLineShapeImp::GetDefaultStateset()
{
   if (!g_pPolyLineShapeStateSet.get())
   {
      g_pPolyLineShapeStateSet = new osg::StateSet;
      g_pPolyLineShapeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   }
   return g_pPolyLineShapeStateSet.get();
}

void UtoPolyLineShapeImp::SetDepthTesting(bool bDepthTest)
{
   if (bDepthTest != m_DepthTest)
   {
      m_DepthTest = bDepthTest;
      NeedCallback();
   }
}
