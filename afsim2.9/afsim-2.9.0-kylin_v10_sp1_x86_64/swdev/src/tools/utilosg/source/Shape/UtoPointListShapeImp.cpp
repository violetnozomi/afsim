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
// UtoPointListShapeImp.cpp : implementation file

#include "UtoPointListShapeImp.hpp"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PrimitiveSet>

#include "UtoPointListShape.hpp"

UtoPointListShapeImp::UtoPointListShapeImp()
{
   m_PrimitiveSet->setMode(osg::PrimitiveSet::POINTS);
   m_Geode->setName("UtoPointListShape");
   m_Geode->setCullingActive(false);
}

UtoPointListShapeImp::UtoPointListShapeImp(const UtoPosition pts[], int num)
{
   m_PrimitiveSet->setMode(osg::PrimitiveSet::POINTS);
   m_Geode->setName("UtoPointListShape");
   m_Geode->setCullingActive(false);
   Insert(0, pts, num);
}

UtoPointListShapeImp::UtoPointListShapeImp(const UtoPointListShapeImp& rhs)
   : UtoPolyShapeImp(rhs)
{
   m_Geode->setName("UtoPointListShape");
   m_Geode->setCullingActive(false);

   m_Geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

UtoPointListShapeImp::~UtoPointListShapeImp() {}
