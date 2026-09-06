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
// UtoPolygonShapeImp.cpp : implementation file

#include "UtoPolygonShapeImp.hpp"

#include <osg/Geode>

UtoPolygonShapeImp::UtoPolygonShapeImp()
{
   m_PrimitiveSet->setMode(osg::PrimitiveSet::POLYGON);
   m_Geode->setName("UtoPolygonShape");
}

UtoPolygonShapeImp::UtoPolygonShapeImp(const UtoPosition pts[], int num)
{
   m_PrimitiveSet->setMode(osg::PrimitiveSet::POLYGON);
   m_Geode->setName("UtoPolygonShape");
   Insert(0, pts, num);
   NeedCallback();
}

UtoPolygonShapeImp::UtoPolygonShapeImp(const UtoPolygonShapeImp& rhs)
   : UtoPolyShapeImp(rhs)
{
   m_Geode->setName("UtoPolygonShape");
   NeedCallback();
}

UtoPolygonShapeImp::~UtoPolygonShapeImp() {}

// Normal update callback.
void UtoPolygonShapeImp::Update(osg::NodeVisitor* nv, osg::Drawable* drawable)
{
   osg::Vec3Array& vect = *m_Vertex;
   // Must have three vertices to calculate a normal.
   if (vect.size() >= 3)
   {
      osg::Vec3 v0 = vect[0], v1 = vect[1], v2 = vect[2];
      osg::Vec3 n = (v0 - v1) ^ (v1 - v2);
      n.normalize();
      m_Normal->front() = n;
   }
   UtoGeometryShapeImp::Update(nv, drawable);
}
