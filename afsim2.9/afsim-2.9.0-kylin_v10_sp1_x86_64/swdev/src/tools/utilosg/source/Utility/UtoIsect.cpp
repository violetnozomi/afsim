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

#include "UtoIsect.hpp"

namespace osgUtil2
{

///////////////////////////////////////////////////////////////////////////////
Hit::Hit()
   : _ratio(0.0f)
   ,
   //_originalLineSegment (),
   //_localLineSegment (),
   //_nodePath (),
   //_geode (),
   //_drawable (),
   //_matrix (),
   //_inverse (),
   //_vecIndexList (),
   _primitiveIndex(0)
//_intersectPoint (),
//_intersectNormal ()
{
}


Hit::Hit(const Hit& hit)
   : _ratio(hit._ratio)
   , _originalLineSegment(hit._originalLineSegment)
   , _localLineSegment(hit._localLineSegment)
   , _nodePath(hit._nodePath)
   , _geode(hit._geode)
   , _drawable(hit._drawable)
   , _matrix(hit._matrix)
   , _inverse(hit._inverse)
   , _vecIndexList(hit._vecIndexList)
   , _primitiveIndex(hit._primitiveIndex)
   , _intersectPoint(hit._intersectPoint)
   , _intersectNormal(hit._intersectNormal)
{
}


Hit::~Hit() {}


Hit& Hit::operator=(const Hit& hit)
{
   if (&hit == this)
      return *this;

   _matrix              = hit._matrix;
   _inverse             = hit._inverse;
   _originalLineSegment = hit._originalLineSegment;
   _localLineSegment    = hit._localLineSegment;

   // copy data across.
   _ratio               = hit._ratio;
   _originalLineSegment = hit._originalLineSegment;
   _localLineSegment    = hit._localLineSegment;
   _nodePath            = hit._nodePath;
   _geode               = hit._geode;
   _drawable            = hit._drawable;

   _vecIndexList    = hit._vecIndexList;
   _primitiveIndex  = hit._primitiveIndex;
   _intersectPoint  = hit._intersectPoint;
   _intersectNormal = hit._intersectNormal;

   return *this;
}

const osg::Vec3 Hit::getWorldIntersectNormal() const
{
   if (_inverse.valid())
   {
      osg::Vec3 norm = osg::Matrix::transform3x3(*_inverse, _intersectNormal);
      norm.normalize();
      return norm;
   }
   else
      return _intersectNormal;
}

///////////////////////////////////////////////////////////////////////////////
IntersectState::IntersectState()
{
   _segmentMaskStack.push_back(0xffffffff);
}


IntersectState::~IntersectState() {}


bool IntersectState::isCulled(const osg::BoundingSphere& bs, LineSegmentMask& segMaskOut)
{
   bool            hit       = false;
   LineSegmentMask mask      = 0x00000001;
   segMaskOut                = 0x00000000;
   LineSegmentMask segMaskIn = _segmentMaskStack.back();
   //    notify(INFO) << << "IntersectState::isCulled() mask in "<<segMaskIn<<"  ";
   for (IntersectState::LineSegmentList::iterator sitr = _segList.begin(); sitr != _segList.end(); ++sitr)
   {
      if ((segMaskIn & mask) && (sitr->second)->intersect(bs))
      {
         //            notify(INFO) << << "Hit ";
         segMaskOut = segMaskOut | mask;
         hit        = true;
      }
      mask = mask << 1;
   }
   //    notify(INFO) << << "mask = "<<segMaskOut<< std::endl;
   return !hit;
}


bool IntersectState::isCulled(const osg::BoundingBox& bb, LineSegmentMask& segMaskOut)
{
   bool            hit       = false;
   LineSegmentMask mask      = 0x00000001;
   segMaskOut                = 0x00000000;
   LineSegmentMask segMaskIn = _segmentMaskStack.back();
   for (IntersectState::LineSegmentList::iterator sitr = _segList.begin(); sitr != _segList.end(); ++sitr)
   {
      if ((segMaskIn & mask) && (sitr->second)->intersect(bb))
      {
         segMaskOut = segMaskOut | mask;
         hit        = true;
      }
      mask = mask << 1;
   }
   return !hit;
}

} // namespace osgUtil2
