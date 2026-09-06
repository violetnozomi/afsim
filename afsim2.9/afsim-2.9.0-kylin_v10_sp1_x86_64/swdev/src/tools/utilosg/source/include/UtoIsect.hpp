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

/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2003 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
 */

#ifndef OSGUTIL2_INTERSECTVISITOR
#define OSGUTIL2_INTERSECTVISITOR 1

#include <algorithm>
#include <float.h>
#include <map>
#include <vector>

#include <osg/Billboard>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LOD>
#include <osg/LineSegment>
#include <osg/Matrix>
#include <osg/NodeVisitor>
#include <osg/Notify>
#include <osg/Transform>
#include <osg/TriangleFunctor>

namespace osgUtil2
{

///////////////////////////////////////////////////////////////////////////////
class Hit
{
public:
   Hit();
   Hit(const Hit& hit);
   ~Hit();

   Hit& operator=(const Hit& hit);

   typedef std::vector<int> VecIndexList;

   bool operator<(const Hit& hit) const
   {
      if (_originalLineSegment < hit._originalLineSegment)
         return true;
      if (hit._originalLineSegment < _originalLineSegment)
         return false;
      return _ratio < hit._ratio;
   }


   const osg::Vec3& getLocalIntersectPoint() const { return _intersectPoint; }
   const osg::Vec3& getLocalIntersectNormal() const { return _intersectNormal; }

   const osg::Vec3 getWorldIntersectPoint() const
   {
      if (_matrix.valid())
         return _intersectPoint * (*_matrix);
      else
         return _intersectPoint;
   }
   const osg::Vec3 getWorldIntersectNormal() const;

   float                          _ratio;
   osg::ref_ptr<osg::LineSegment> _originalLineSegment;
   osg::ref_ptr<osg::LineSegment> _localLineSegment;
   osg::NodePath                  _nodePath;
   osg::ref_ptr<osg::Geode>       _geode;
   osg::ref_ptr<osg::Drawable>    _drawable;
   osg::ref_ptr<osg::RefMatrix>   _matrix;
   osg::ref_ptr<osg::RefMatrix>   _inverse;

   VecIndexList _vecIndexList;
   int          _primitiveIndex;
   osg::Vec3    _intersectPoint;
   osg::Vec3    _intersectNormal;
};

///////////////////////////////////////////////////////////////////////////////
struct TriangleHit
{
   TriangleHit(unsigned int     index,
               const osg::Vec3& normal,
               float            r1,
               const osg::Vec3* v1,
               float            r2,
               const osg::Vec3* v2,
               float            r3,
               const osg::Vec3* v3)
      : _index(index)
      , _normal(normal)
      , _r1(r1)
      , _v1(v1)
      , _r2(r2)
      , _v2(v2)
      , _r3(r3)
      , _v3(v3)
   {
   }

   unsigned int     _index;
   const osg::Vec3  _normal;
   float            _r1;
   const osg::Vec3* _v1;
   float            _r2;
   const osg::Vec3* _v2;
   float            _r3;
   const osg::Vec3* _v3;
};

///////////////////////////////////////////////////////////////////////////////
class IntersectState : public osg::Referenced
{
public:
   IntersectState();

   osg::ref_ptr<osg::RefMatrix> _matrix;
   osg::ref_ptr<osg::RefMatrix> _inverse;

   typedef std::pair<osg::ref_ptr<osg::LineSegment>, osg::ref_ptr<osg::LineSegment>> LineSegmentPair;
   typedef std::vector<LineSegmentPair>                                              LineSegmentList;
   LineSegmentList                                                                   _segList;

   typedef unsigned int                 LineSegmentMask;
   typedef std::vector<LineSegmentMask> LineSegmentMaskStack;
   LineSegmentMaskStack                 _segmentMaskStack;

   bool isCulled(const osg::BoundingSphere& bs, LineSegmentMask& segMaskOut);
   bool isCulled(const osg::BoundingBox& bb, LineSegmentMask& segMaskOut);

   void addLineSegmentPair(osg::LineSegment* first, osg::LineSegment* second)
   {
      _segList.push_back(LineSegmentPair(first, second));
   }

protected:
   ~IntersectState() override;
};


///////////////////////////////////////////////////////////////////////////////

/*
class IntersectPolicy
{
public:
   bool process_hit(const osg::LineSegment& ls, const osg::Drawable& drawable);
   void first_hit();
   bool done_hit();
   void next_hit();
   TriangleHit& getHit() const;
   float getRatio() const;
   void onEnterNode(osg::Node& node);
   void onLeaveNode(osg::Node& node);
   virtual osg::Matrix& getCurMatrix() = 0;
};*/


/* Basic visitor for ray based collisions of a scene.*/
template<class T>
class IntersectVisitor : public osg::NodeVisitor, public T
{
public:
   IntersectVisitor();
   ~IntersectVisitor() override;

   void reset() override;

   /* Add a line segment to use for intersection testing during scene traversal.*/
   void addLineSegment(osg::LineSegment* seg);

   // typedef std::multiset<Hit> HitList;
   typedef std::vector<Hit>                     HitList;
   typedef std::map<osg::LineSegment*, HitList> LineSegmentHitListMap;
   HitList&                                     getHitList(osg::LineSegment* seg) { return _segHitList[seg]; }
   int                                          getNumHits(osg::LineSegment* seg) { return _segHitList[seg].size(); }

   bool hits();

   void apply(osg::Node&) override;
   void apply(osg::Geode& node) override;
   void apply(osg::Billboard& node) override;

   void apply(osg::Group& node) override;
   void apply(osg::Transform& node) override;
   void apply(osg::Switch& node) override;
   void apply(osg::LOD& node) override;

protected:
   // Force the names to be visible and dependent on the template paramter.
   using T::done_hit;
   using T::first_hit;
   using T::getHit;
   using T::getRatio;
   using T::next_hit;
   using T::onEnterNode;
   using T::onLeaveNode;
   using T::process_hit;

   bool intersect(osg::Drawable& gset);

   void pushMatrix(const osg::Matrix& matrix);
   void popMatrix();

   bool enterNode(osg::Node& node);
   void leaveNode();

   virtual osg::Matrix& getCurMatrix() { return *_intersectStateStack.back().get()->_matrix.get(); }

   typedef std::vector<osg::ref_ptr<IntersectState>> IntersectStateStack;

   IntersectStateStack _intersectStateStack;

   osg::NodePath         _nodePath;
   LineSegmentHitListMap _segHitList;
};

template<class T>
IntersectVisitor<T>::IntersectVisitor()
{
   // overide the default node visitor mode.
   setTraversalMode(NodeVisitor::TRAVERSE_ACTIVE_CHILDREN);

   reset();
}


template<class T>
IntersectVisitor<T>::~IntersectVisitor()
{
}


template<class T>
void IntersectVisitor<T>::reset()
{
   _intersectStateStack.clear();

   // create a empty IntersectState on the intersectStateStack.
   _intersectStateStack.push_back(new IntersectState);

   _nodePath.clear();
   _segHitList.clear();
}


template<class T>
bool IntersectVisitor<T>::hits()
{
   for (LineSegmentHitListMap::iterator itr = _segHitList.begin(); itr != _segHitList.end(); ++itr)
   {
      if (!(itr->second.empty()))
         return true;
   }
   return false;
}


template<class T>
void IntersectVisitor<T>::addLineSegment(osg::LineSegment* seg)
{
   if (!seg)
      return;

   if (!seg->valid())
   {
      osg::notify(osg::WARN) << "Warning: invalid line segment passed to IntersectVisitor::addLineSegment(..)" << std::endl;
      // osg::notify(osg::WARN)<<"         "<<seg->start()<<" "<<seg->end()<<" segment ignored.."<< std::endl;
      return;
   }

   // first check to see if segment has already been added.
   for (LineSegmentHitListMap::iterator itr = _segHitList.begin(); itr != _segHitList.end(); ++itr)
   {
      if (itr->first == seg)
         return;
   }

   // create a new segment transformed to local coordintes.
   IntersectState*   cis = _intersectStateStack.back().get();
   osg::LineSegment* ns  = new osg::LineSegment;

   if (cis->_inverse.valid())
      ns->mult(*seg, *(cis->_inverse));
   else
      *ns = *seg;

   cis->addLineSegmentPair(seg, ns);
}


template<class T>
void IntersectVisitor<T>::pushMatrix(const osg::Matrix& matrix)
{
   IntersectState* nis = new IntersectState;

   IntersectState* cis = _intersectStateStack.back().get();

   if (cis->_matrix.valid())
   {
      nis->_matrix = new osg::RefMatrix;
      nis->_matrix->mult(matrix, *(cis->_matrix));
   }
   else
   {
      nis->_matrix = new osg::RefMatrix(matrix);
   }

   osg::RefMatrix* inverse_world = new osg::RefMatrix;
   inverse_world->invert(*(nis->_matrix));
   nis->_inverse = inverse_world;

   IntersectState::LineSegmentMask segMaskIn = cis->_segmentMaskStack.back();
   IntersectState::LineSegmentMask mask      = 0x00000001;
   for (IntersectState::LineSegmentList::iterator sitr = cis->_segList.begin(); sitr != cis->_segList.end(); ++sitr)
   {
      if ((segMaskIn & mask))
      {
         osg::LineSegment* seg = new osg::LineSegment;
         seg->mult(*(sitr->first), *inverse_world);
         nis->addLineSegmentPair(sitr->first.get(), seg);
      }
      mask = mask << 1;
   }

   _intersectStateStack.push_back(nis);
}


template<class T>
void IntersectVisitor<T>::popMatrix()
{
   if (!_intersectStateStack.empty())
   {
      _intersectStateStack.pop_back();
   }
}


template<class T>
bool IntersectVisitor<T>::enterNode(osg::Node& node)
{
   const osg::BoundingSphere& bs = node.getBound();
   if (bs.valid())
   {
      IntersectState*                 cis = _intersectStateStack.back().get();
      IntersectState::LineSegmentMask sm  = 0xffffffff;
      if (cis->isCulled(bs, sm))
         return false;
      cis->_segmentMaskStack.push_back(sm);
      _nodePath.push_back(&node);

      onEnterNode(node);

      return true;
   }
   else
   {
      return false;
   }
}


template<class T>
void IntersectVisitor<T>::leaveNode()
{
   onLeaveNode(*_nodePath.back());

   IntersectState* cis = _intersectStateStack.back().get();
   cis->_segmentMaskStack.pop_back();
   _nodePath.pop_back();
}


template<class T>
void IntersectVisitor<T>::apply(osg::Node& node)
{
   if (!enterNode(node))
      return;

   traverse(node);

   leaveNode();
}

template<class T>
bool IntersectVisitor<T>::intersect(osg::Drawable& drawable)
{
   bool hitFlag = false;

   IntersectState* cis = _intersectStateStack.back().get();

   const auto& bb = drawable.getBound();

   for (IntersectState::LineSegmentList::iterator sitr = cis->_segList.begin(); sitr != cis->_segList.end(); ++sitr)
   {
      if (sitr->second->intersect(bb))
      {
         if (process_hit(*sitr->second, drawable))
         {
            osg::Geometry* geometry = drawable.asGeometry();

            for (first_hit(); !done_hit(); next_hit())
            {
               Hit hit;
               hit._nodePath = _nodePath;
               hit._matrix   = cis->_matrix;
               hit._inverse  = cis->_inverse;
               hit._drawable = &drawable;
               if (_nodePath.empty())
                  hit._geode = nullptr;
               else
                  hit._geode = dynamic_cast<osg::Geode*>(_nodePath.back());

               TriangleHit& triHit = getHit();

               hit._ratio               = getRatio();
               hit._primitiveIndex      = triHit._index;
               hit._originalLineSegment = sitr->first;
               hit._localLineSegment    = sitr->second;

               hit._intersectPoint = sitr->second->start() * (1.0f - hit._ratio) + sitr->second->end() * hit._ratio;

               hit._intersectNormal = triHit._normal;

               if (geometry)
               {
                  osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
                  if (vertices)
                  {
                     osg::Vec3* first = &(vertices->front());
                     if (triHit._v1)
                        hit._vecIndexList.push_back(triHit._v1 - first);
                     if (triHit._v2)
                        hit._vecIndexList.push_back(triHit._v2 - first);
                     if (triHit._v3)
                        hit._vecIndexList.push_back(triHit._v3 - first);
                  }
               }


               _segHitList[sitr->first.get()].push_back(hit);

               std::sort(_segHitList[sitr->first.get()].begin(), _segHitList[sitr->first.get()].end());

               hitFlag = true;
            }
         }
      }
   }

   return hitFlag;
}

template<class T>
void IntersectVisitor<T>::apply(osg::Geode& geode)
{
   if (!enterNode(geode))
      return;

   for (unsigned int i = 0; i < geode.getNumDrawables(); i++)
   {
      intersect(*geode.getDrawable(i));
   }

   leaveNode();
}


template<class T>
void IntersectVisitor<T>::apply(osg::Billboard& node)
{
   if (!enterNode(node))
      return;

   leaveNode();
}


template<class T>
void IntersectVisitor<T>::apply(osg::Group& node)
{
   if (!enterNode(node))
      return;

   traverse(node);

   leaveNode();
}


template<class T>
void IntersectVisitor<T>::apply(osg::Transform& node)
{
   if (!enterNode(node))
      return;

   osg::ref_ptr<osg::RefMatrix> matrix = new osg::RefMatrix;
   node.computeLocalToWorldMatrix(*matrix, this);

   pushMatrix(*matrix);

   traverse(node);

   popMatrix();

   leaveNode();
}


template<class T>
void IntersectVisitor<T>::apply(osg::Switch& node)
{
   apply((osg::Group&)node);
}


template<class T>
void IntersectVisitor<T>::apply(osg::LOD& node)
{
   apply((osg::Group&)node);
}

} // namespace osgUtil2

#endif
