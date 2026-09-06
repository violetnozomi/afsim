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
/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
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

/* I modified this code to support line picking.  I'll try to mark the places where I make changes (outside of naming),
   so that it should simplify merging in OSG changes. - Jonathon Marjamaa (Boeing) */

#ifndef UTOCME_INTERSECTVISITOR
#define UTOCME_INTERSECTVISITOR 1

#include <map>
#include <set>
#include <vector>

#include <osg/Geode>
#include <osg/LineSegment>
#include <osg/Matrix>
#include <osg/NodeVisitor>
#include <osg/Transform>
#include <osgUtil/Export>

#include "UtoHitInfo.hpp"

namespace osgUtil
{


/** Deprecated - use IntersectionVisitor instead.*/
class /*OSGUTIL_EXPORT*/ UtoCmeIntersectVisitor : public osg::NodeVisitor
{
public:
   UtoCmeIntersectVisitor();
   ~UtoCmeIntersectVisitor() override;

   META_NodeVisitor("osgUtil", "UtoCmeIntersectVisitor")

      void reset() override;

   /** Add a line segment to use for intersection testing during scene traversal.
    * Note, a maximum of 32 line segments can be added to a IntersectVistor,
    * adding more than this will result in warning being emitted to the console
    * and the excess segments being ignored.*/
   void addLineSegment(osg::LineSegment* seg);

   typedef std::vector<UtoHitInfo>                          UtoCmeHitList;
   typedef std::map<const osg::LineSegment*, UtoCmeHitList> CmeLineSegmentHitListMap;

   UtoCmeHitList& getHitList(const osg::LineSegment* seg) { return _segHitList[seg]; }

   int getNumHits(const osg::LineSegment* seg) { return _segHitList[seg].size(); }

   CmeLineSegmentHitListMap& getSegHitList() { return _segHitList; }

   bool hits();

   enum LODSelectionMode
   {
      USE_HIGHEST_LEVEL_OF_DETAIL,
      USE_SEGMENT_START_POINT_AS_EYE_POINT_FOR_LOD_LEVEL_SELECTION
   };

   void             setLODSelectionMode(LODSelectionMode mode) { _lodSelectionMode = mode; }
   LODSelectionMode getLODSelectionMode() const { return _lodSelectionMode; }

   /** Set the eye point in local coordinates.
    * This is a pseudo-EyePoint for billboarding and LOD purposes.
    * It is copied from the Start point of the most-recently-added segment
    * of the intersection ray set (IntersectState::_segList). */
   void setEyePoint(const osg::Vec3& eye) { _pseudoEyePoint = eye; }

   osg::Vec3 getEyePoint() const override;


   /** Get the distance from a point to the eye point, distance value in local coordinate system.
    * This is calculated using the pseudo-EyePoint (above) when doing LOD calculcations. */
   float getDistanceToEyePoint(const osg::Vec3& pos, bool withLODScale) const override;

   void apply(osg::Node&) override;
   void apply(osg::Geode& node) override;
   void apply(osg::Billboard& node) override;

   void apply(osg::Group& node) override;
   void apply(osg::Transform& node) override;
   void apply(osg::Switch& node) override;
   void apply(osg::LOD& node) override;

protected:
   class CmeIntersectState : public osg::Referenced
   {
   public:
      CmeIntersectState();

      osg::ref_ptr<osg::RefMatrix> _MVPW_matrix;
      osg::ref_ptr<osg::RefMatrix> _MVPW_inverse;
      osg::ref_ptr<osg::RefMatrix> _view_matrix;
      osg::ref_ptr<osg::RefMatrix> _view_inverse;
      osg::ref_ptr<osg::RefMatrix> _model_matrix;
      osg::ref_ptr<osg::RefMatrix> _model_inverse;
      osg::ref_ptr<osg::StateSet>  _stateSet;
      int                          _orthoScaled;

      typedef std::pair<osg::ref_ptr<osg::LineSegment>, osg::ref_ptr<osg::LineSegment>> CmeLineSegmentPair;
      typedef std::vector<CmeLineSegmentPair>                                           CmeLineSegmentList;
      CmeLineSegmentList                                                                _segList;

      typedef unsigned int                    CmeLineSegmentMask;
      typedef std::vector<CmeLineSegmentMask> CmeLineSegmentMaskStack;
      CmeLineSegmentMaskStack                 _segmentMaskStack;

      bool isCulled(const osg::BoundingSphere& bs, CmeLineSegmentMask& segMaskOut);
      bool isCulled(const osg::BoundingBox& bb, CmeLineSegmentMask& segMaskOut);

      void addLineSegment(osg::LineSegment* seg);


   protected:
      ~CmeIntersectState() override;
   };
   bool HandleScreenScaled(osg::Node& aNode);
   bool intersect(osg::Drawable& gset);

   void pushMatrix(osg::RefMatrix* matrix, osg::Transform::ReferenceFrame rf, bool aIncrementOrthoScale = false);
   void popMatrix();

   bool enterNode(osg::Node& node);
   void leaveNode();

   void PushState(osg::StateSet* ss);

   typedef std::vector<osg::ref_ptr<CmeIntersectState>> CmeIntersectStateStack;

   CmeIntersectStateStack _intersectStateStack;

   std::vector<float> _lineWidthStack;
   std::vector<float> _pointWidthStack;

   CmeLineSegmentHitListMap _segHitList;

   LODSelectionMode _lodSelectionMode;
   osg::Vec3        _pseudoEyePoint;
   int              _verticalResolution;
   double           _fovy;
   bool             mScreenScaled;
   float            mOrthoScale;
   float            mIconScale;
};

/** Picking intersection visitor specialises the IntersectVistor to allow more convinient handling of mouse picking.*/
class /*OSGUTIL_EXPORT*/ UtoCmePickVisitor : public osgUtil::UtoCmeIntersectVisitor
{
public:
   UtoCmePickVisitor(const osg::Viewport* viewport,
                     const osg::Matrixd&  proj,
                     const osg::Matrixd&  view,
                     float                mx,
                     float                my,
                     bool                 aScreenScaled,
                     float                mOrthoScale,
                     float                mIconScale);

   void runNestedPickVisitor(osg::Node&           node,
                             const osg::Viewport* viewport,
                             const osg::Matrix&   proj,
                             const osg::Matrix&   view,
                             float                mx,
                             float                my);

   void apply(osg::Projection& projection) override;

   void apply(osg::Camera& camera) override;

protected:
   float _mx;
   float _my;

   osg::ref_ptr<const osg::Viewport> _lastViewport;
   osg::Matrixd                      _lastProjectionMatrix;
   osg::Matrixd                      _lastViewMatrix;
};

} // namespace osgUtil

#endif
