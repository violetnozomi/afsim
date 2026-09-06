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
#ifndef HITINFO_HPP
#define HITINFO_HPP

#include "utilosg_export.h"

#include <vector>

#include <osg/Matrix>
#include <osg/Node>
#include <osg/Vec3>
#include <osg/ref_ptr>

class UtoShape;
namespace osg
{
class LineSegment;
class Drawable;
class Geode;
} // namespace osg

class UTILOSG_EXPORT UtoHitInfo
{
   /** Deprecated */
public:
   UtoHitInfo();
   UtoHitInfo(const UtoHitInfo& hit);
   ~UtoHitInfo();

   UtoHitInfo& operator=(const UtoHitInfo& hit);

   typedef std::vector<int> CmeVecIndexList;

   bool operator<(const UtoHitInfo& hit) const;


   const osg::Vec3& getLocalIntersectPoint() const;
   const osg::Vec3& getLocalIntersectNormal() const;

   const osg::Vec3 getWorldIntersectPoint() const;
   const osg::Vec3 getWorldIntersectNormal() const;

   bool                    IsLineHit() const;
   float                   getRatio() const;
   const osg::LineSegment* getOriginalLineSegment() const;
   const osg::LineSegment* getLocalLineSegment() const;
   osg::NodePath&          getNodePath();
   const osg::NodePath&    getNodePath() const;
   osg::Geode*             getGeode();
   const osg::Geode*       getGeode() const;
   osg::Drawable*          getDrawable();
   const osg::Drawable*    getDrawable() const;
   const osg::RefMatrix*   getMatrix() const;
   const osg::RefMatrix*   getInverseMatrix() const;
   const CmeVecIndexList&  getVecIndexList() const;
   int                     getPrimitiveIndex() const;

   bool                           _lineHit;
   float                          _ratio;
   osg::ref_ptr<osg::LineSegment> _originalLineSegment;
   osg::ref_ptr<osg::LineSegment> _localLineSegment;
   osg::NodePath                  _nodePath;
   osg::ref_ptr<osg::Geode>       _geode;
   osg::ref_ptr<osg::Drawable>    _drawable;
   osg::ref_ptr<osg::RefMatrix>   _matrix;
   osg::ref_ptr<osg::RefMatrix>   _inverse;

   CmeVecIndexList _vecIndexList;
   int             _primitiveIndex;
   osg::Vec3       _intersectPoint;
   osg::Vec3       _intersectNormal;
};

class UTILOSG_EXPORT UtoHitInfoList
{
public:
   UtoHitInfoList() {}
   ~UtoHitInfoList();
   std::vector<UtoHitInfo> _hits;
   // If these hits are associated with a shape, this is a pointer to the shape
   UtoShape* mShapePtr;
};
#endif
