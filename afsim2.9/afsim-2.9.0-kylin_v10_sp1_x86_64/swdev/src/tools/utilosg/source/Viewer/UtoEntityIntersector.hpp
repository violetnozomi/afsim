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
// UtoEntityIntersector.hpp : header file

#if !defined(_UTOENTITYINTERSECTOR_HPP_)
#define _UTOENTITYINTERSECTOR_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

#include <list>
#include <vector>

#include <osg/LineSegment>
#include <osg/ref_ptr>
#include <osgUtil/SceneView>

namespace osg
{
class Node;
// class Vec3;
} // namespace osg

class UtoEntityIntersector
{
public:
   struct HitInfo
   {
      double dist;
      // vtEntity* entity;
      osg::Node* entity;
      // HitInfo (double d, vtEntity* e) : dist (d), entity (e) {}
      HitInfo(double d, osg::Node* e)
         : dist(d)
         , entity(e)
      {
      }
   };

   typedef std::list<osg::Node*> EntityList;
   typedef std::vector<HitInfo>  HitVec;

   explicit UtoEntityIntersector(osgUtil::SceneView* s);
   ~UtoEntityIntersector();

   void    Test(const osg::Vec3& Start, const osg::Vec3& End, int num);
   void    SetScene(osgUtil::SceneView* s);
   HitVec& Entities() { return m_HitEntities; }

protected:
private:
   UtoEntityIntersector();
   UtoEntityIntersector(const UtoEntityIntersector&);
   UtoEntityIntersector& operator=(const UtoEntityIntersector&);

   static bool enum_proc(const int num, osg::Node* ent, void* udata);

   void Clear();

   osg::ref_ptr<osgUtil::SceneView> m_pScene;
   osg::ref_ptr<osg::LineSegment>   m_Line;
   EntityList                       m_IsectEntities;
   HitVec                           m_HitEntities;
};


#endif // !defined(_UTOENTITYINTERSECTOR_HPP_)
