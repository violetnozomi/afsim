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
// UtoHittestTraverser.cpp : implementation file

#include "UtoHittestTraverser.hpp"

#include <algorithm>

#include "UtoOptions.hpp"

const double UtoHittestTraverser::m_AlphaThreshold = 1.0 / 255.0;

UtoHittestTraverser::UtoHittestTraverser(osg::Node* n, osg::LineSegment& ls, int maxEnt)
   : // osg::NodeVisitor(*n),
   m_LineSegment(&ls)
   , m_nMax(maxEnt)
   , m_MatrixStack(1)
{
}

UtoHittestTraverser::~UtoHittestTraverser() {}

bool UtoHittestTraverser::PreNodeCallback()
{
   return false;
}

bool UtoHittestTraverser::PostNodeCallback()
{
   return false;
}

bool UtoHittestTraverser::ProcessHit(const osg::LineSegment& ray, UtoGeometryUtil::PolyIterator& iter, osg::Vec3& hit)
{
   return false;
}
