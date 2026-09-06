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
// UtoEntityIntersector.cpp : implementation file

#include "UtoEntityIntersector.hpp"

#include <algorithm>

#include <osg/Vec3>
#include <osgUtil/SceneView>

#include "UtoHittestTraverser.hpp"

struct DistanceSorter
{
   typedef UtoEntityIntersector::HitInfo HitInfo;
   DistanceSorter(const osg::Vec3& pt)
      : m_Start(pt)
   {
   }
   bool      operator()(HitInfo& e1, HitInfo& e2) { return (e1.dist < e2.dist); }
   osg::Vec3 m_Start;
};

UtoEntityIntersector::UtoEntityIntersector(osgUtil::SceneView* s)
   : m_pScene(s)
{
}

UtoEntityIntersector::~UtoEntityIntersector() {}

void UtoEntityIntersector::Test(const osg::Vec3& Start, const osg::Vec3& End, int num) {}

void UtoEntityIntersector::Clear()
{
   m_HitEntities.clear();
   m_IsectEntities.clear();
}

void UtoEntityIntersector::SetScene(osgUtil::SceneView* s)
{
   m_pScene = s;
}

bool UtoEntityIntersector::enum_proc(const int num, osg::Node* ent, void* udata)
{
   return false;
}
