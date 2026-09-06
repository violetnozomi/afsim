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
// UtoHittestTraverser.hpp : header file

#if !defined(_UTOHITTESTTRAVERSER_HPP_)
#define _UTOHITTESTTRAVERSER_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <list>

#include <osg/LineSegment>
#include <osg/Matrix>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/ref_ptr>

#include "UtoGeometryUtil.hpp"

class UtoHittestTraverser : public osg::NodeVisitor
{
public:
   friend class osg::NodeVisitor;

   struct HitInfo
   {
      osg::Vec3  point;
      osg::Node* node;
      HitInfo(const osg::Vec3& pt, osg::Node* n)
         : point(pt)
         , node(n)
      {
      }
   };

   typedef std::list<HitInfo>     NodeList;
   typedef std::list<osg::Matrix> GMatrixStack;

   UtoHittestTraverser(osg::Node* n, osg::LineSegment& ls, int maxEnt);

   bool      AnyHit() { return !m_HitNodes.empty(); }
   void      Clear();
   NodeList& Nodes() { return m_HitNodes; }

protected:
private:
   ~UtoHittestTraverser() override;

   UtoHittestTraverser();
   UtoHittestTraverser(const UtoHittestTraverser&);
   UtoHittestTraverser& operator=(const UtoHittestTraverser&);

   virtual bool PreNodeCallback();
   virtual bool PostNodeCallback();
   bool         ProcessHit(const osg::LineSegment& ray, UtoGeometryUtil::PolyIterator& iter, osg::Vec3& hit);

   static const double m_AlphaThreshold;

   osg::ref_ptr<osg::LineSegment> m_LineSegment;
   int                            m_nMax;
   NodeList                       m_HitNodes;
   GMatrixStack                   m_MatrixStack;
};


#endif // !defined(_UTOHITTESTTRAVERSER_HPP_)
