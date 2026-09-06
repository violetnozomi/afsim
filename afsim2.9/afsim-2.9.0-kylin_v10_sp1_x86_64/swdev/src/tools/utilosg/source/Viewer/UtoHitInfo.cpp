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
#include "UtoHitInfo.hpp"

#include <map>
#include <set>
#include <vector>

#include <osg/Geode>
#include <osg/LineSegment>
#include <osg/Matrix>
#include <osg/NodeVisitor>
#include <osg/Transform>


UtoHitInfo::UtoHitInfo() {}


UtoHitInfo::UtoHitInfo(const UtoHitInfo& hit)
{
   // copy data across.
   _lineHit             = false;
   _ratio               = hit._ratio;
   _originalLineSegment = hit._originalLineSegment;
   _localLineSegment    = hit._localLineSegment;
   _nodePath            = hit._nodePath;
   _geode               = hit._geode;
   _drawable            = hit._drawable;
   _matrix              = hit._matrix;
   _inverse             = hit._inverse;

   _vecIndexList    = hit._vecIndexList;
   _primitiveIndex  = hit._primitiveIndex;
   _intersectPoint  = hit._intersectPoint;
   _intersectNormal = hit._intersectNormal;
}


UtoHitInfo::~UtoHitInfo() {}


UtoHitInfo& UtoHitInfo::operator=(const UtoHitInfo& hit)
{
   if (&hit == this)
      return *this;

   _lineHit             = hit._lineHit;
   _matrix              = hit._matrix;
   _inverse             = hit._inverse;
   _originalLineSegment = hit._originalLineSegment;
   _localLineSegment    = hit._localLineSegment;

   // copy data across.
   _ratio    = hit._ratio;
   _nodePath = hit._nodePath;
   _geode    = hit._geode;
   _drawable = hit._drawable;

   _vecIndexList    = hit._vecIndexList;
   _primitiveIndex  = hit._primitiveIndex;
   _intersectPoint  = hit._intersectPoint;
   _intersectNormal = hit._intersectNormal;

   return *this;
}

const osg::Vec3 UtoHitInfo::getWorldIntersectNormal() const
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

int UtoHitInfo::getPrimitiveIndex() const
{
   return _primitiveIndex;
}

const UtoHitInfo::CmeVecIndexList& UtoHitInfo::getVecIndexList() const
{
   return _vecIndexList;
}

const osg::RefMatrix* UtoHitInfo::getInverseMatrix() const
{
   return _inverse.get();
}

const osg::RefMatrix* UtoHitInfo::getMatrix() const
{
   return _matrix.get();
}

const osg::Drawable* UtoHitInfo::getDrawable() const
{
   return _drawable.get();
}

osg::Drawable* UtoHitInfo::getDrawable()
{
   return _drawable.get();
}

const osg::Geode* UtoHitInfo::getGeode() const
{
   return _geode.get();
}

osg::Geode* UtoHitInfo::getGeode()
{
   return _geode.get();
}

const osg::NodePath& UtoHitInfo::getNodePath() const
{
   return _nodePath;
}

osg::NodePath& UtoHitInfo::getNodePath()
{
   return _nodePath;
}

const osg::LineSegment* UtoHitInfo::getLocalLineSegment() const
{
   return _localLineSegment.get();
}

const osg::LineSegment* UtoHitInfo::getOriginalLineSegment() const
{
   return _originalLineSegment.get();
}

float UtoHitInfo::getRatio() const
{
   return _ratio;
}

bool UtoHitInfo::IsLineHit() const
{
   return _lineHit;
}

const osg::Vec3 UtoHitInfo::getWorldIntersectPoint() const
{
   if (_matrix.valid())
      return _intersectPoint * (*_matrix);
   else
      return _intersectPoint;
}

const osg::Vec3& UtoHitInfo::getLocalIntersectNormal() const
{
   return _intersectNormal;
}

const osg::Vec3& UtoHitInfo::getLocalIntersectPoint() const
{
   return _intersectPoint;
}

bool UtoHitInfo::operator<(const UtoHitInfo& hit) const
{
   if (_originalLineSegment < hit._originalLineSegment)
      return true;
   if (hit._originalLineSegment < _originalLineSegment)
      return false;
   return _ratio < hit._ratio;
}

UtoHitInfoList::~UtoHitInfoList()
{
   //   for (size_t i = 0; i < _hits.size(); ++i) delete _hits[i];
}
