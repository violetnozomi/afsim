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
// UtoAttrPolyLineShapeImp.cpp : implementation file

#include "UtoAttrPolyLineShapeImp.hpp"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PrimitiveSet>
#include <osg/Texture1D>

#include "UtoAttrPolyLineShape.hpp"
#include "UtoShaderUtilities.hpp"
#include "UtoShaders.hpp"

UtoAttrPolyLineShapeImp::UtoAttrPolyLineShapeImp()
{
   m_PrimitiveSet->setMode(osg::PrimitiveSet::LINE_STRIP);
   m_Geode->setName("UtoAttrPolyLineShape");

   // setup an empty color array
   m_Color = new osg::Vec4Array;

   m_Geometry->setColorArray(m_Color.get());
   m_Geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
}

UtoAttrPolyLineShapeImp::UtoAttrPolyLineShapeImp(const UtoPosition pts[], const UtoColor cls[], int num)
{
   m_PrimitiveSet->setMode(osg::PrimitiveSet::LINE_STRIP);
   m_Geode->setName("UtoAttrPolyLineShape");

   // setup an empty color array
   m_Color = new osg::Vec4Array;

   Insert(0, pts, cls, num);

   m_Geometry->setColorArray(m_Color.get());
   m_Geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
}

UtoAttrPolyLineShapeImp::UtoAttrPolyLineShapeImp(const UtoAttrPolyLineShapeImp& rhs)
   : UtoPolyLineShapeImp(rhs)
{
   m_Color = new osg::Vec4Array(*rhs.m_Color.get());
   m_Geometry->setColorArray(m_Color.get());
   m_Geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
}

UtoAttrPolyLineShapeImp::~UtoAttrPolyLineShapeImp() {}

int UtoAttrPolyLineShapeImp::Remove(int pos)
{
   int size = m_Color->size();
   // check for user errors
   if (pos >= size || pos < 0)
      return -1;

   m_Color->erase(m_Color->begin() + pos);

   return UtoPolyLineShapeImp::Remove(pos);
}

void UtoAttrPolyLineShapeImp::Clear()
{
   int index = m_Color->size();
   // Call remove until all elements have been removed.
   while (Remove(--index) != -1)
   {
   }
}

UtoColor UtoAttrPolyLineShapeImp::ColorAt(int pos)
{
   osg::Vec4Array::iterator color = m_Color->begin() + pos;
   return UtoColor((unsigned char)(color->x() * 255),
                   (unsigned char)(color->y() * 255),
                   (unsigned char)(color->z() * 255),
                   (unsigned char)(color->w() * 255));
}

int UtoAttrPolyLineShapeImp::Insert(int pos, const UtoPosition pts[], const UtoColor col[], int num)
{
   // insert the new points
   for (int i = 0; i < num; ++i)
   {
      m_Color->insert(m_Color->begin() + pos + i,
                      1,
                      osg::Vec4(col[i].RedF(), col[i].GreenF(), col[i].BlueF(), col[i].AlphaF()));
   }
   // the first element is already in place

   return UtoPolyLineShapeImp::Insert(pos, pts, num);
}

int UtoAttrPolyLineShapeImp::SetAt(int pos, const UtoPosition pts, const UtoColor col)
{
   osg::Vec4Array::iterator color = m_Color->begin() + pos;
   color->x()                     = col.RedF();
   color->y()                     = col.GreenF();
   color->z()                     = col.BlueF();
   color->w()                     = col.AlphaF();

   return UtoPolyLineShapeImp::SetAt(pos, pts);
}

bool UtoAttrPolyLineShapeImp::SetShaders(const char* aVShader, const char* aFShader, std::set<std::string>& aDefList)
{
   UnloadShaders();
   bool shaders = UtoShaderUtilities::ShadersEnabled();
   if (shaders)
   {
      m_RootNode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
      m_RootNode->getOrCreateStateSet()->setAttributeAndModes(UtoShaders::UseProgram(aVShader, aFShader, aDefList));
   }
   return shaders;
}

void UtoAttrPolyLineShapeImp::UnloadShaders()
{
   // remove program from mGeode
   // remove bounding sphere callback

   m_RootNode->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::PROGRAM);
   m_RootNode->setComputeBoundingSphereCallback(nullptr);
   mBoundingSphereCallbackPtr = nullptr;
}

UtoAttrPolyLineShapeImp::PolyLineBoundingSphereCallback::PolyLineBoundingSphereCallback()
   : osg::Node::ComputeBoundingSphereCallback()
{
}


osg::BoundingSphere UtoAttrPolyLineShapeImp::PolyLineBoundingSphereCallback::computeBound(const osg::Node&)
{
   double              range = 3704.0 * 5000;
   osg::BoundingSphere temp;
   temp.set(osg::Vec3(0, 0, 0), range);
   return temp;
}
