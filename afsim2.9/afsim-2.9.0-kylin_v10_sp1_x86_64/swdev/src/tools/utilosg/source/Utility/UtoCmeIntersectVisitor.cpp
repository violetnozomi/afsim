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
   so that it should simplify merging in OSG changes.  Do a search for JMNOTE - Jonathon Marjamaa (Boeing) */

#include <algorithm>
#include <float.h>
#include <map>

#include <UtoCmeIntersectVisitor.hpp>
#include <UtoCmeLineFunctor.hpp>
#include <UtoShaderUtilities.hpp>
#include <osg/Billboard>
#include <osg/Camera>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LOD>
#include <osg/LineWidth>
#include <osg/Notify>
#include <osg/Point>
#include <osg/Projection>
#include <osg/Transform>
#include <osg/io_utils>

namespace osgUtil
{

UtoCmeIntersectVisitor::CmeIntersectState::CmeIntersectState()
{
   _orthoScaled = 0;
   _segmentMaskStack.push_back(0xffffffff);
}


UtoCmeIntersectVisitor::CmeIntersectState::~CmeIntersectState() {}


bool UtoCmeIntersectVisitor::CmeIntersectState::isCulled(const osg::BoundingSphere& bs, CmeLineSegmentMask& segMaskOut)
{
   bool               hit       = false;
   CmeLineSegmentMask mask      = 0x00000001;
   segMaskOut                   = 0x00000000;
   CmeLineSegmentMask segMaskIn = _segmentMaskStack.back();
   for (CmeIntersectState::CmeLineSegmentList::iterator sitr = _segList.begin(); sitr != _segList.end(); ++sitr)
   {
      if ((segMaskIn & mask) && (sitr->second)->intersect(bs))
      {
         segMaskOut = segMaskOut | mask;
         hit        = true;
      }
      mask = mask << 1;
   }
   return !hit;
}

bool UtoCmeIntersectVisitor::CmeIntersectState::isCulled(const osg::BoundingBox& bb, CmeLineSegmentMask& segMaskOut)
{
   bool               hit       = false;
   CmeLineSegmentMask mask      = 0x00000001;
   segMaskOut                   = 0x00000000;
   CmeLineSegmentMask segMaskIn = _segmentMaskStack.back();
   for (CmeIntersectState::CmeLineSegmentList::iterator sitr = _segList.begin(); sitr != _segList.end(); ++sitr)
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

void UtoCmeIntersectVisitor::CmeIntersectState::addLineSegment(osg::LineSegment* seg)
{
   // create a new segment transformed to local coordintes.
   osg::LineSegment* ns = new osg::LineSegment;

   if (_model_inverse.valid())
   {
      if (_view_inverse.valid())
      {
         osg::Matrix matrix = (*(_view_inverse)) * (*(_model_inverse));
         ns->mult(*seg, matrix);
      }
      else
      {
         ns->mult(*seg, *(_model_inverse));
      }
   }
   else if (_view_inverse.valid())
   {
      ns->mult(*seg, *(_view_inverse));
   }
   else
   {
      *ns = *seg;
   }

   _segList.push_back(CmeLineSegmentPair(seg, ns));
}


UtoCmeIntersectVisitor::UtoCmeIntersectVisitor()
{
   // override the default node visitor mode.
   setTraversalMode(NodeVisitor::TRAVERSE_ACTIVE_CHILDREN);

   // Initialize eyepoint to 0,0,0
   setEyePoint(osg::Vec3(0.0f, 0.0f, 0.0f));

   setLODSelectionMode(USE_HIGHEST_LEVEL_OF_DETAIL); // original IntersectVisitor behavior
   // setLODSelectionMode(USE_SEGMENT_START_POINT_AS_EYE_POINT_FOR_LOD_LEVEL_SELECTION);

   reset();
}


UtoCmeIntersectVisitor::~UtoCmeIntersectVisitor() {}


void UtoCmeIntersectVisitor::reset()
{
   _intersectStateStack.clear();

   // create a empty IntersectState on the intersectStateStack.
   _intersectStateStack.push_back(new CmeIntersectState);

   _segHitList.clear();
}

float UtoCmeIntersectVisitor::getDistanceToEyePoint(const osg::Vec3& pos, bool /*withLODScale*/) const
{
   if (_lodSelectionMode == USE_SEGMENT_START_POINT_AS_EYE_POINT_FOR_LOD_LEVEL_SELECTION)
   {
      // osg::notify(osg::NOTICE)<<"IntersectVisitor::getDistanceToEyePoint)"<<(pos-getEyePoint()).length()<<std::endl;
      // LODScale is not available to IntersectVisitor, so we ignore the withLODScale argument
      // if (withLODScale) return (pos-getEyePoint()).length()*getLODScale();
      // else return (pos-getEyePoint()).length();
      return (pos - getEyePoint()).length();
   }
   else
   {
      return 0.0f;
   }
}


bool UtoCmeIntersectVisitor::hits()
{
   for (CmeLineSegmentHitListMap::iterator itr = _segHitList.begin(); itr != _segHitList.end(); ++itr)
   {
      if (!(itr->second.empty()))
         return true;
   }
   return false;
}

osg::Vec3 UtoCmeIntersectVisitor::getEyePoint() const
{
   const CmeIntersectState* cis = _intersectStateStack.empty() ? nullptr : _intersectStateStack.back().get();
   if (cis && (cis->_model_inverse.valid() || cis->_view_inverse.valid()))
   {
      osg::Vec3 eyePoint = _pseudoEyePoint;
      if (cis->_view_inverse.valid())
         eyePoint = eyePoint * (*(cis->_view_inverse));
      if (cis->_model_inverse.valid())
         eyePoint = eyePoint * (*(cis->_model_inverse));

      // osg::notify(osg::NOTICE)<<"IntersectVisitor::getEyePoint()"<<eyePoint<<std::endl;

      return eyePoint;
   }
   else
   {
      return _pseudoEyePoint;
   }
}

void UtoCmeIntersectVisitor::addLineSegment(osg::LineSegment* seg)
{
   if (!seg)
      return;

   if (!seg->valid())
   {
      osg::notify(osg::WARN) << "Warning: invalid line segment passed to IntersectVisitor::addLineSegment(..)" << std::endl;
      osg::notify(osg::WARN) << "         " << seg->start() << " " << seg->end() << " segment ignored.." << std::endl;
      return;
   }

   CmeIntersectState* cis = _intersectStateStack.back().get();

   if (cis->_segList.size() >= 32)
   {
      osg::notify(osg::WARN) << "Warning: excessive number of line segmenets passed to "
                                "IntersectVisitor::addLineSegment(..), maximum permitted is 32 line segments."
                             << std::endl;
      osg::notify(osg::WARN) << "         " << seg->start() << " " << seg->end() << " segment ignored.." << std::endl;
      return;
   }

   setEyePoint(seg->start()); // set start of line segment to be pseudo EyePoint for billboarding and LOD purposes

   // first check to see if segment has already been added.
   for (CmeIntersectState::CmeLineSegmentList::iterator itr = cis->_segList.begin(); itr != cis->_segList.end(); ++itr)
   {
      if (itr->first == seg)
         return;
   }

   cis->addLineSegment(seg);
}


void UtoCmeIntersectVisitor::pushMatrix(osg::RefMatrix* matrix, osg::Transform::ReferenceFrame rf, bool aIncrementOrthoScale)
{
   CmeIntersectState* nis = new CmeIntersectState;

   CmeIntersectState* cis = _intersectStateStack.back().get();

   nis->_orthoScaled = cis->_orthoScaled;

   if (rf == osg::Transform::RELATIVE_RF)
   {
      // share the original view matrix
      nis->_view_matrix  = cis->_view_matrix;
      nis->_view_inverse = cis->_view_inverse;
      nis->_MVPW_matrix  = cis->_MVPW_matrix;
      nis->_MVPW_inverse = cis->_MVPW_inverse;

      // set up new model matrix
      nis->_model_matrix = matrix;
      if (cis->_model_matrix.valid())
      {
         nis->_model_matrix->postMult(*(cis->_model_matrix));
      }

      osg::RefMatrix* inverse_world = new osg::RefMatrix;
      inverse_world->invert(*(nis->_model_matrix));
      nis->_model_inverse = inverse_world;
   }
   else
   {
      // set a new view matrix
      nis->_view_matrix = matrix;

      osg::RefMatrix* inverse_world = new osg::RefMatrix;
      inverse_world->invert(*(nis->_view_matrix));
      nis->_view_inverse = inverse_world;

      // model matrix now blank.
      nis->_model_matrix  = nullptr;
      nis->_model_inverse = nullptr;
   }


   CmeIntersectState::CmeLineSegmentMask segMaskIn = cis->_segmentMaskStack.back();
   CmeIntersectState::CmeLineSegmentMask mask      = 0x00000001;
   for (CmeIntersectState::CmeLineSegmentList::iterator sitr = cis->_segList.begin(); sitr != cis->_segList.end(); ++sitr)
   {
      if ((segMaskIn & mask))
      {
         nis->addLineSegment(sitr->first.get());
      }
      mask = mask << 1;
   }

   if (aIncrementOrthoScale)
   {
      nis->_orthoScaled++;
   }

   _intersectStateStack.push_back(nis);
}


void UtoCmeIntersectVisitor::popMatrix()
{
   if (!_intersectStateStack.empty())
   {
      _intersectStateStack.pop_back();
   }
}


bool UtoCmeIntersectVisitor::enterNode(osg::Node& node)
{
   osg::BoundingSphere bs = node.getBound();
   if (0 == (node.getNodeMask() & 0x00000001))
   {
      return false;
   }
   if (bs.valid() && node.isCullingActive())
   {
      CmeIntersectState*                    cis = _intersectStateStack.back().get();
      CmeIntersectState::CmeLineSegmentMask sm  = 0xffffffff;
      //        if (cis->isCulled(bs,sm)) return false;
      cis->_segmentMaskStack.push_back(sm);

      osg::StateSet* ss = node.getStateSet();
      PushState(ss);

      return true;
   }
   else
   {
      CmeIntersectState* cis = _intersectStateStack.back().get();
      if (!cis->_segmentMaskStack.empty())
         cis->_segmentMaskStack.push_back(cis->_segmentMaskStack.back());
      else
         cis->_segmentMaskStack.push_back(0xffffffff);

      osg::StateSet* ss = node.getStateSet();
      PushState(ss);
      return true;
   }
}

void UtoCmeIntersectVisitor::PushState(osg::StateSet* ss)
{
   float           lineWidth = 1.0f;
   osg::LineWidth* lw        = nullptr;
   if (ss != nullptr)
   {
      lw = (osg::LineWidth*)(ss->getAttribute(osg::StateAttribute::LINEWIDTH));
   }
   if (nullptr != lw)
   {
      lineWidth = lw->getWidth();
   }
   else if (!_lineWidthStack.empty())
   {
      lineWidth = _lineWidthStack.back();
   }
   _lineWidthStack.push_back(lineWidth);
   osg::Point* ps = nullptr;
   if (ss != nullptr)
   {
      ps = (osg::Point*)(ss->getAttribute(osg::StateAttribute::POINT));
   }
   float pointSize = 1.0f;
   if (nullptr != ps)
   {
      pointSize = ps->getSize();
   }
   else if (!_pointWidthStack.empty())
   {
      pointSize = _pointWidthStack.back();
   }
   _pointWidthStack.push_back(pointSize);
}

void UtoCmeIntersectVisitor::leaveNode()
{
   CmeIntersectState* cis = _intersectStateStack.back().get();
   cis->_segmentMaskStack.pop_back();
   if (!_lineWidthStack.empty())
   {
      _lineWidthStack.pop_back();
   }
   if (!_pointWidthStack.empty())
   {
      _pointWidthStack.pop_back();
   }
}


void UtoCmeIntersectVisitor::apply(osg::Node& node)
{
   if (!enterNode(node))
      return;

   traverse(node);

   leaveNode();
}

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

protected:
   TriangleHit& operator=(const TriangleHit&) { return *this; }
};

/* JMNOTE: this is the major source of my changes.  I am adding a line segment intersection apart from the existing
 * triangle intersection */
struct LineHit
{
   LineHit(unsigned int index, float r1, const osg::Vec3* v1, const osg::Vec3* v2)
      : _index(index)
      , _r(r1)
      , _v1(v1)
      , _v2(v2)
   {
   }

   unsigned int     _index;
   float            _r;
   const osg::Vec3* _v1;
   const osg::Vec3* _v2;

protected:
   LineHit& operator=(const LineHit&) { return *this; }
};

struct PointHit
{
   PointHit(unsigned int index, const osg::Vec3* v1)
      : _index(index)
      , _v1(v1)
   {
   }

   unsigned int     _index;
   const osg::Vec3* _v1;

protected:
   PointHit& operator=(const LineHit&) { return *this; }
};


struct TriangledAndLineIntersect
{
   osg::ref_ptr<osg::LineSegment> _seg;

   osg::Vec3 _s;
   osg::Vec3 _d;
   float     _length;

   int    _index;
   float  _ratio;
   bool   _hit;
   int    _verticalResolution;
   double _fovy;
   double _orthoScale;
   float  _lineWidth;
   float  _pointSize;

   typedef std::multimap<float, PointHit> PointHitList;

   PointHitList _phl;

   typedef std::multimap<float, LineHit> LineHitList;

   LineHitList _lhl;

   typedef std::multimap<float, TriangleHit> TriangleHitList;

   TriangleHitList _thl;

   TriangledAndLineIntersect()
      : _lineWidth(1.0f)
      , _pointSize(1.0f)
   {
   }

   TriangledAndLineIntersect(const osg::LineSegment& seg,
                             int                     verticalResolution,
                             double                  fovy,
                             double                  orthoScale,
                             float                   ratio = FLT_MAX)
      : _lineWidth(1.0f)
      , _pointSize(1.0f)
   {
      set(seg, verticalResolution, fovy, orthoScale, ratio);
   }

   void set(const osg::LineSegment& seg, int verticalResolution, double fovy, double orthoScale, float ratio = FLT_MAX)
   {
      _verticalResolution = verticalResolution;
      _fovy               = fovy;
      _orthoScale         = orthoScale;
      _seg                = new osg::LineSegment(seg);
      _hit                = false;
      _index              = 0;
      _ratio              = ratio;

      _s      = _seg->start();
      _d      = _seg->end() - _seg->start();
      _length = _d.length();
      _d /= _length;
   }

   void setLineWidth(const float aW) { _lineWidth = aW; }

   void setPointSize(const float aS) { _pointSize = aS; }

   float PointToSegmentDistance(osg::Vec3 line_startPoint,
                                osg::Vec3 line_endPoint,
                                osg::Vec3 point,
                                double&   fractionalDistanceOnLine,
                                double&   closestDist)
   {
      osg::Vec3 ld   = line_endPoint - line_startPoint;
      double    pld  = point * ld;
      double    l1ld = line_startPoint * ld;
      double    ldld = ld * ld;

      fractionalDistanceOnLine = (pld - l1ld) / ldld;

      double fd     = fractionalDistanceOnLine;
      fd            = (fd < 0) ? 0 : fd;
      fd            = (fd > 1) ? 1 : fd;
      osg::Vec3 lp  = line_startPoint + (line_endPoint - line_startPoint) * fd;
      osg::Vec3 l2p = lp - point;
      closestDist   = l2p.length();
      return closestDist;
   }

   float SegmentToSegmentDistance(osg::Vec3 line1_startPoint,
                                  osg::Vec3 line1_endPoint,
                                  osg::Vec3 line2_startPoint,
                                  osg::Vec3 line2_endPoint,
                                  osg::Vec3 closestePointOnLine1,
                                  osg::Vec3 closestePointOnLine2,
                                  double&   fractionalDistanceOnLine1,
                                  double&   fractionalDistanceOnLine2,
                                  double&   closestDist)
   {
      // From http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline3d/

      osg::Vec3 p13;
      osg::Vec3 p43;
      osg::Vec3 p21;
      double    d1343, d4321, d1321, d4343, d2121;
      double    numer, denom;
      double    EPS = 0.000000001;

      p13 = line1_startPoint - line2_startPoint;
      p43 = line2_endPoint - line2_startPoint;
      if ((fabs(p43[0]) < EPS) && (fabs(p43[1]) < EPS) && (fabs(p43[2]) < EPS))
      {
         return false;
      }
      p21 = line1_endPoint - line1_startPoint;
      if ((fabs(p21[0]) < EPS) && (fabs(p21[1]) < EPS) && (fabs(p21[2]) < EPS))
      {
         return false;
      }

      d1343 = (p13[0] * p43[0]) + (p13[1] * p43[1]) + (p13[2] * p43[2]);
      d4321 = (p43[0] * p21[0]) + (p43[1] * p21[1]) + (p43[2] * p21[2]);
      d1321 = (p13[0] * p21[0]) + (p13[1] * p21[1]) + (p13[2] * p21[2]);
      d4343 = (p43[0] * p43[0]) + (p43[1] * p43[1]) + (p43[2] * p43[2]);
      d2121 = (p21[0] * p21[0]) + (p21[1] * p21[1]) + (p21[2] * p21[2]);

      denom = d2121 * d4343 - d4321 * d4321;
      if (fabs(denom) < EPS)
      {
         return false;
      }
      numer = d1343 * d4321 - d1321 * d4343;

      fractionalDistanceOnLine1 = numer / denom;
      fractionalDistanceOnLine2 = (d1343 + d4321 * fractionalDistanceOnLine1) / d4343;

      closestePointOnLine1 = line1_startPoint + p21 * fractionalDistanceOnLine1;
      closestePointOnLine2 = line2_startPoint + p43 * fractionalDistanceOnLine2;

      osg::Vec3 del_line1 = line1_endPoint - line1_startPoint;
      osg::Vec3 del_line2 = line2_endPoint - line2_startPoint;

      double limited_mua = fractionalDistanceOnLine1;
      if (limited_mua > 1.0)
         limited_mua = 1.0;
      if (limited_mua < 0.0)
         limited_mua = 0.0;
      double limited_mub = fractionalDistanceOnLine2;
      if (limited_mub > 1.0)
         limited_mub = 1.0;
      if (limited_mub < 0.0)
         limited_mub = 0.0;

      osg::Vec3 tempDist;
      osg::Vec3 close_line1;
      tempDist    = del_line1 * limited_mua;
      close_line1 = line1_startPoint + tempDist;

      osg::Vec3 close_line2;
      tempDist    = del_line2 * limited_mub;
      close_line2 = line2_startPoint + tempDist;

      osg::Vec3 distBetween;
      distBetween = close_line1 - close_line2;
      closestDist = distBetween.length();

      return true;
   }

   //   bool intersect(const osg::Vec3& v1,const osg::Vec3& v2,float& r)
   inline void operator()(const osg::Vec3& v1, const osg::Vec3& v2, bool treatVertexDataAsTemporary)
   {
      ++_index;

      if (v1 == v2)
         return;

      double    s2s;
      double    fd1;
      double    fd2;
      osg::Vec3 cp1;
      osg::Vec3 cp2;
      if (!SegmentToSegmentDistance(_s, _s + _d * _length, v1, v2, cp1, cp2, fd1, fd2, s2s))
      {
         return;
      }
      if (fd1 < 0) // if it is behind the camera, we don't want it.
      {
         return;
      }
      double pixelDist = 10;
      if (_fovy > 0) // indicates a perspective projection (I encoded orthographic when negative)
      {
         double d     = _length * fd1; // how far is the closest point from the eye
         double theta = s2s / d;       // approximation of the view angle from the selection point to the closest point
         pixelDist    = theta * _verticalResolution / _fovy;
      }
      else
      {
         double theta = s2s; // pretty much the same for orthographic, but range doesn't matter, and we deal with
                             // distance instead of angles
         pixelDist =
            _orthoScale * theta * _verticalResolution / -_fovy; // and the fovy needs to correct for the encoding I did
      }

      if (pixelDist > 1 + _lineWidth)
      {
         return;
      }

      if (treatVertexDataAsTemporary)
      {
         _lhl.insert(std::pair<const float, LineHit>(fd1, LineHit(_index - 1, fd2, nullptr, nullptr)));
      }
      else
      {
         _lhl.insert(std::pair<const float, LineHit>(fd1, LineHit(_index - 1, fd2, &v1, &v2)));
      }
      _hit = true;
   }

   //   bool intersect(const osg::Vec3& v1,const osg::Vec3& v2,const osg::Vec3& v3,float& r)
   inline void operator()(const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3, bool treatVertexDataAsTemporary)
   {
      ++_index;

      if (v1 == v2 || v2 == v3 || v1 == v3)
         return;

      osg::Vec3 v12  = v2 - v1;
      osg::Vec3 n12  = v12 ^ _d;
      float     ds12 = (_s - v1) * n12;
      float     d312 = (v3 - v1) * n12;
      if (d312 >= 0.0f)
      {
         if (ds12 < 0.0f)
            return;
         if (ds12 > d312)
            return;
      }
      else // d312 < 0
      {
         if (ds12 > 0.0f)
            return;
         if (ds12 < d312)
            return;
      }

      osg::Vec3 v23  = v3 - v2;
      osg::Vec3 n23  = v23 ^ _d;
      float     ds23 = (_s - v2) * n23;
      float     d123 = (v1 - v2) * n23;
      if (d123 >= 0.0f)
      {
         if (ds23 < 0.0f)
            return;
         if (ds23 > d123)
            return;
      }
      else // d123 < 0
      {
         if (ds23 > 0.0f)
            return;
         if (ds23 < d123)
            return;
      }

      osg::Vec3 v31  = v1 - v3;
      osg::Vec3 n31  = v31 ^ _d;
      float     ds31 = (_s - v3) * n31;
      float     d231 = (v2 - v3) * n31;
      if (d231 >= 0.0f)
      {
         if (ds31 < 0.0f)
            return;
         if (ds31 > d231)
            return;
      }
      else // d231 < 0
      {
         if (ds31 > 0.0f)
            return;
         if (ds31 < d231)
            return;
      }


      float r3;
      if (ds12 == 0.0f)
         r3 = 0.0f;
      else if (d312 != 0.0f)
         r3 = ds12 / d312;
      else
         return; // the triangle and the line must be parallel intersection.

      float r1;
      if (ds23 == 0.0f)
         r1 = 0.0f;
      else if (d123 != 0.0f)
         r1 = ds23 / d123;
      else
         return; // the triangle and the line must be parallel intersection.

      float r2;
      if (ds31 == 0.0f)
         r2 = 0.0f;
      else if (d231 != 0.0f)
         r2 = ds31 / d231;
      else
         return; // the triangle and the line must be parallel intersection.

      float total_r = (r1 + r2 + r3);
      if (total_r != 1.0f)
      {
         if (total_r == 0.0f)
            return; // the triangle and the line must be parallel intersection.
         float inv_total_r = 1.0f / total_r;
         r1 *= inv_total_r;
         r2 *= inv_total_r;
         r3 *= inv_total_r;
      }

      osg::Vec3 in = v1 * r1 + v2 * r2 + v3 * r3;
      if (!in.valid())
      {
         osg::notify(osg::WARN) << "Warning:: Picked up error in TriangleIntersect" << std::endl;
         osg::notify(osg::WARN) << "   (" << v1 << ",\t" << v2 << ",\t" << v3 << ")" << std::endl;
         osg::notify(osg::WARN) << "   (" << r1 << ",\t" << r2 << ",\t" << r3 << ")" << std::endl;
         return;
      }

      float d = (in - _s) * _d;

      if (d < 0.0f)
         return;
      if (d > _length)
         return;

      osg::Vec3 normal = v12 ^ v23;
      normal.normalize();

      float r = d / _length;


      if (treatVertexDataAsTemporary)
      {
         _thl.insert(
            std::pair<const float, TriangleHit>(r, TriangleHit(_index - 1, normal, r1, nullptr, r2, nullptr, r3, nullptr)));
      }
      else
      {
         _thl.insert(std::pair<const float, TriangleHit>(r, TriangleHit(_index - 1, normal, r1, &v1, r2, &v2, r3, &v3)));
      }
      _hit = true;
   }

   inline void operator()(const osg::Vec3& v1, bool treatVertexDataAsTemporary)
   {
      ++_index;

      double p2s;
      double fd1;
      PointToSegmentDistance(_s, _s + _d * _length, v1, fd1, p2s);

      if (fd1 < 0) // if it is behind the camera, we don't want it.
      {
         return;
      }
      double pixelDist = 10;
      if (_fovy > 0) // indicates a perspective projection (I encoded orthographic when negative)
      {
         double d     = _length * fd1; // how far is the closest point from the eye
         double theta = p2s / d;       // approximation of the view angle from the selection point to the closest point
         pixelDist    = theta * _verticalResolution / _fovy;
      }
      else
      {
         double theta = p2s; // pretty much the same for orthographic, but range doesn't matter, and we deal with
                             // distance instead of angles
         pixelDist = theta * _verticalResolution / -_fovy; // and the fovy needs to correct for the encoding I did
      }

      if (pixelDist > 4 + _pointSize)
      {
         return;
      }

      if (treatVertexDataAsTemporary)
      {
         _phl.insert(std::pair<const float, PointHit>(fd1, PointHit(_index - 1, nullptr)));
      }
      else
      {
         _phl.insert(std::pair<const float, PointHit>(fd1, PointHit(_index - 1, &v1)));
      }
      _hit = true;
   }
};
/* JMNOTE: Ok I should be done here, apart from name changed. -Jonathon Marjamaa*/

#include <iostream>
bool UtoCmeIntersectVisitor::intersect(osg::Drawable& drawable)
{
   bool hitFlag = false;

   CmeIntersectState* cis = _intersectStateStack.back().get();

   osg::BoundingBox bb = drawable.getBoundingBox();
   osg::Vec4        ctr(bb.center()[0], bb.center()[1], bb.center()[2], 1.0);
   /////////////
   float          lineWidth = 1;
   float          pointSize = 1;
   osg::StateSet* nss       = drawable.getStateSet();

   osg::LineWidth* lw = nullptr;
   if (nss != nullptr)
   {
      lw = (osg::LineWidth*)(nss->getAttribute(osg::StateAttribute::LINEWIDTH));
   }
   if (nullptr != lw)
   {
      lineWidth = lw->getWidth();
   }
   else if (!_lineWidthStack.empty())
   {
      lineWidth = _lineWidthStack.back();
   }
   osg::Point* ps = nullptr;
   if (nss != nullptr)
   {
      ps = (osg::Point*)(nss->getAttribute(osg::StateAttribute::POINT));
   }
   if (nullptr != ps)
   {
      pointSize = ps->getSize();
   }
   else if (!_pointWidthStack.empty())
   {
      pointSize = _pointWidthStack.back();
   }

   float psize = (lineWidth > pointSize) ? lineWidth : pointSize;

   if (cis != nullptr && cis->_view_matrix.valid() && cis->_model_matrix.valid() && cis->_MVPW_matrix.valid())
   {
      osg::Matrix objToScr = (*(cis->_model_matrix)) * (*(cis->_view_matrix)) * (*(cis->_MVPW_matrix));
      osg::Matrix scrToObj = (*(cis->_MVPW_inverse)) * (*(cis->_view_inverse)) * (*(cis->_model_inverse));
      osg::Vec4   ctrXFrm  = ctr * objToScr; // convert the center into screen space
      ctrXFrm /= ctrXFrm[3];
      ctrXFrm += osg::Vec4(psize * 0.5, 0, 0, 0); // offset by one pixel length
      ctrXFrm = ctrXFrm * scrToObj;
      ctrXFrm /= ctrXFrm[3];
      osg::Vec4 delta =
         ctr - ctrXFrm; // convert to object space (center + 1 pixel), and get difference vector from orig ctr
      double halflen = delta.length(); // get the length of the difference
      bb.expandBy(bb.xMin() - halflen, bb.yMin() - halflen, bb.zMin() - halflen);
      bb.expandBy(bb.xMax() + halflen, bb.yMax() + halflen, bb.zMax() + halflen); // grow bounding box, as necessary
   }
   /////////////////
   for (CmeIntersectState::CmeLineSegmentList::iterator sitr = cis->_segList.begin(); sitr != cis->_segList.end(); ++sitr)
   {
      if (sitr->second->intersect(bb))
      {
         /* JMNOTE: Here I use the line intersect in place of the triangle intersect functor */
         osg::UtoCmeLineFunctor<TriangledAndLineIntersect> li;
         li.set(*sitr->second, _verticalResolution, _fovy, cis->_orthoScaled > 0 ? mOrthoScale : 1.0);

         li.setLineWidth(lineWidth);
         li.setPointSize(pointSize);

         drawable.accept(li);
         if (li._hit)
         {
            osg::Geometry* geometry = drawable.asGeometry();


            for (TriangledAndLineIntersect::PointHitList::iterator phitr = li._phl.begin(); phitr != li._phl.end(); ++phitr)
            {
               UtoHitInfo hit;
               hit._lineHit  = false;
               hit._nodePath = _nodePath;
               hit._matrix   = cis->_model_matrix;
               hit._inverse  = cis->_model_inverse;
               hit._drawable = &drawable;
               if (_nodePath.empty())
                  hit._geode = nullptr;
               else
                  hit._geode = dynamic_cast<osg::Geode*>(_nodePath.back());

               PointHit& pointHit = phitr->second;

               hit._ratio               = phitr->first;
               hit._primitiveIndex      = pointHit._index;
               hit._originalLineSegment = sitr->first;
               hit._localLineSegment    = sitr->second;

               hit._intersectPoint = sitr->second->start() * (1.0f - hit._ratio) + sitr->second->end() * hit._ratio;

               if (geometry)
               {
                  osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
                  if (vertices)
                  {
                     osg::Vec3* first = &(vertices->front());
                     if (pointHit._v1)
                        hit._vecIndexList.push_back(pointHit._v1 - first);
                  }
               }

               _segHitList[sitr->first.get()].push_back(hit);

               std::sort(_segHitList[sitr->first.get()].begin(), _segHitList[sitr->first.get()].end());

               hitFlag = true;
            }
            for (TriangledAndLineIntersect::LineHitList::iterator lhitr = li._lhl.begin(); lhitr != li._lhl.end(); ++lhitr)
            {
               UtoHitInfo hit;
               hit._lineHit  = true;
               hit._nodePath = _nodePath;
               hit._matrix   = cis->_model_matrix;
               hit._inverse  = cis->_model_inverse;
               hit._drawable = &drawable;
               if (_nodePath.empty())
                  hit._geode = nullptr;
               else
                  hit._geode = dynamic_cast<osg::Geode*>(_nodePath.back());

               LineHit& lineHit = lhitr->second;

               hit._ratio               = lhitr->first;
               hit._primitiveIndex      = lineHit._index;
               hit._originalLineSegment = sitr->first;
               hit._localLineSegment    = sitr->second;

               hit._intersectPoint = sitr->second->start() * (1.0f - hit._ratio) + sitr->second->end() * hit._ratio;

               if (geometry)
               {
                  osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
                  if (vertices)
                  {
                     osg::Vec3* first = &(vertices->front());
                     if (lineHit._v1)
                        hit._vecIndexList.push_back(lineHit._v1 - first);
                     if (lineHit._v2)
                        hit._vecIndexList.push_back(lineHit._v2 - first);
                  }
               }

               _segHitList[sitr->first.get()].push_back(hit);

               std::sort(_segHitList[sitr->first.get()].begin(), _segHitList[sitr->first.get()].end());

               hitFlag = true;
            }
            for (TriangledAndLineIntersect::TriangleHitList::iterator thitr = li._thl.begin(); thitr != li._thl.end();
                 ++thitr)
            {
               UtoHitInfo hit;
               hit._lineHit  = false;
               hit._nodePath = _nodePath;
               hit._matrix   = cis->_model_matrix;
               hit._inverse  = cis->_model_inverse;
               hit._drawable = &drawable;
               if (_nodePath.empty())
                  hit._geode = nullptr;
               else
                  hit._geode = dynamic_cast<osg::Geode*>(_nodePath.back());

               TriangleHit& triHit = thitr->second;

               hit._ratio               = thitr->first;
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
         /* JMNOTE: All done here */
      }
   }

   return hitFlag;
}

void UtoCmeIntersectVisitor::apply(osg::Geode& geode)
{
   if (!enterNode(geode))
      return;

   bool autoScale = HandleScreenScaled(geode);

   for (unsigned int i = 0; i < geode.getNumDrawables(); i++)
   {
      intersect(*geode.getDrawable(i));
   }

   if (autoScale)
   {
      popMatrix();
   }


   leaveNode();
}


void UtoCmeIntersectVisitor::apply(osg::Billboard& node)
{
   if (!enterNode(node))
      return;

   // IntersectVisitor doesn't have getEyeLocal(), can we use NodeVisitor::getEyePoint()?
   const osg::Vec3& eye_local = getEyePoint();

   for (unsigned int i = 0; i < node.getNumDrawables(); i++)
   {
      const osg::Vec3&             pos              = node.getPosition(i);
      osg::ref_ptr<osg::RefMatrix> billboard_matrix = new osg::RefMatrix;
      node.computeMatrix(*billboard_matrix, eye_local, pos);

      pushMatrix(billboard_matrix.get(), osg::Transform::RELATIVE_RF);

      intersect(*node.getDrawable(i));

      popMatrix();
   }

   leaveNode();
}


void UtoCmeIntersectVisitor::apply(osg::Group& node)
{
   if (!enterNode(node))
      return;

   bool autoScale = HandleScreenScaled(node);

   traverse(node);

   if (autoScale)
   {
      popMatrix();
   }

   leaveNode();
}


void UtoCmeIntersectVisitor::apply(osg::Transform& node)
{
   if (!enterNode(node))
      return;

   osg::ref_ptr<osg::RefMatrix> matrix = new osg::RefMatrix;
   node.computeLocalToWorldMatrix(*matrix, this);

   pushMatrix(matrix.get(), node.getReferenceFrame());

   traverse(node);

   popMatrix();

   leaveNode();
}


void UtoCmeIntersectVisitor::apply(osg::Switch& node)
{
   apply((osg::Group&)node);
}


void UtoCmeIntersectVisitor::apply(osg::LOD& node)
{
   apply((osg::Group&)node);
}

bool UtoCmeIntersectVisitor::HandleScreenScaled(osg::Node& aNode)
{
   if ((aNode.getNumDescriptions() > 0) && (aNode.getDescription(0) == "VA_ScreenScaled"))
   {
      osg::Vec4 zero(0.0, 0.0, 0.0, 1.0);

      if (mScreenScaled)
      {
         if (mOrthoScale != 0)
         {
            float                VA_ScreenScale = 1.0;
            const osg::StateSet* ss             = aNode.getStateSet();
            if (nullptr != ss)
            {
               const osg::Uniform* ssUni = ss->getUniform("VA_ScreenScale");
               if ((nullptr == ssUni) || (!ssUni->get(VA_ScreenScale)))
               {
                  VA_ScreenScale = 1.0;
               }
            }
            float                        scalar       = mOrthoScale / VA_ScreenScale * 10.0f * mIconScale;
            osg::ref_ptr<osg::RefMatrix> scalarMatrix = new osg::RefMatrix(osg::Matrix::scale(scalar, scalar, scalar));
            pushMatrix(scalarMatrix, osg::Transform::RELATIVE_RF, true);
         }
         else
         {
            CmeIntersectState* cis = !_intersectStateStack.empty() ? _intersectStateStack.back().get() : nullptr;
            if ((cis != nullptr) && (cis->_view_matrix.valid()) && (cis->_model_matrix.valid()) &&
                (cis->_MVPW_matrix.valid()))
            {
               osg::Matrix objToScr       = (*(cis->_model_matrix)) * (*(cis->_view_matrix)) * (*(cis->_MVPW_matrix));
               osg::Vec4   ctrXFrm        = zero * objToScr;
               float       VA_ScreenScale = 1.0;
               const osg::StateSet* ss    = aNode.getStateSet();
               if (nullptr != ss)
               {
                  const osg::Uniform* ssUni = ss->getUniform("VA_ScreenScale");
                  if ((nullptr == ssUni) || (!ssUni->get(VA_ScreenScale)))
                  {
                     VA_ScreenScale = 1.0;
                  }
               }
               float scalar = _verticalResolution / ctrXFrm.w() * VA_ScreenScale * 0.1 / mIconScale;
               scalar       = scalar < 1.0 ? 1.0f / scalar : 1.0;

               osg::ref_ptr<osg::RefMatrix> scalarMatrix = new osg::RefMatrix(osg::Matrix::scale(scalar, scalar, scalar));
               pushMatrix(scalarMatrix, osg::Transform::RELATIVE_RF, true);
            }
         }
         return true;
      }
   }
   return false;
}

UtoCmePickVisitor::UtoCmePickVisitor(const osg::Viewport* viewport,
                                     const osg::Matrixd&  proj,
                                     const osg::Matrixd&  view,
                                     float                mx,
                                     float                my,
                                     bool                 aScreenScaled,
                                     float                aOrthoScale,
                                     float                aIconScale)
   : _mx(mx)
   , _my(my)
   , _lastViewport(viewport)
   , _lastProjectionMatrix(proj)
   , _lastViewMatrix(view)
{
   mScreenScaled = aScreenScaled;
   mOrthoScale   = aOrthoScale;
   double zNear  = 0;
   double zFar   = 0;
   double fovy   = 0;
   double aspect = 0;
   proj.getPerspective(fovy, aspect, zNear, zFar);
   _verticalResolution = (int)(viewport->height());
   mIconScale          = aIconScale;
   _fovy               = fovy * osg::PI / 180.0;
   if (_fovy == 0)
   {
      double xLeft   = 0;
      double xRight  = 0;
      double yTop    = 0;
      double yBottom = 0;
      proj.getOrtho(xLeft, xRight, yBottom, yTop, zNear, zFar);
      _fovy = yBottom - yTop;
   }
   setLODSelectionMode(USE_SEGMENT_START_POINT_AS_EYE_POINT_FOR_LOD_LEVEL_SELECTION);

   if (viewport && mx >= static_cast<float>(viewport->x()) && my >= static_cast<float>(viewport->y()) &&
       mx < static_cast<float>(viewport->x() + viewport->width()) &&
       my < static_cast<float>(viewport->y() + viewport->height()))
   {
      // mouse pointer intersect viewport so we can proceed to set up a line segment
      osg::Matrix  MVPW = proj * viewport->computeWindowMatrix();
      osg::Matrixd inverseMVPW;
      inverseMVPW.invert(MVPW);

      osg::Vec3         nearPoint   = osg::Vec3(mx, my, 0.0f) * inverseMVPW;
      osg::Vec3         farPoint    = osg::Vec3(mx, my, 1.0f) * inverseMVPW;
      osg::LineSegment* lineSegment = new osg::LineSegment;
      lineSegment->set(nearPoint, farPoint);

      CmeIntersectState* cis = !_intersectStateStack.empty() ? _intersectStateStack.back().get() : nullptr;
      if (cis)
      {
         cis->_MVPW_matrix  = new osg::RefMatrix(MVPW);
         cis->_MVPW_inverse = new osg::RefMatrix(inverseMVPW);
         cis->_view_matrix  = new osg::RefMatrix(view);
         cis->_view_inverse = new osg::RefMatrix;
         cis->_view_inverse->invert(*(cis->_view_matrix));

         cis->_model_matrix  = nullptr;
         cis->_model_inverse = nullptr;
      }
      else
      {
         osg::notify(osg::NOTICE) << "Warning: PickVisitor not set up correctly, picking errors likely" << std::endl;
      }


      addLineSegment(lineSegment);
   }
}

void UtoCmePickVisitor::runNestedPickVisitor(osg::Node&           node,
                                             const osg::Viewport* viewport,
                                             const osg::Matrix&   proj,
                                             const osg::Matrix&   view,
                                             float                mx,
                                             float                my)
{
   UtoCmePickVisitor newPickVisitor(viewport, proj, view, mx, my, mScreenScaled, mOrthoScale, mIconScale);
   newPickVisitor.setTraversalMask(getTraversalMask());

   newPickVisitor.getNodePath() = getNodePath();

   // the new pickvisitor over the nodes children.
   node.traverse(newPickVisitor);

   for (CmeLineSegmentHitListMap::iterator itr = newPickVisitor._segHitList.begin();
        itr != newPickVisitor._segHitList.end();
        ++itr)
   {
      _segHitList.insert(*itr);
   }
}

void UtoCmePickVisitor::apply(osg::Projection& projection)
{
   runNestedPickVisitor(projection, _lastViewport.get(), projection.getMatrix(), _lastViewMatrix, _mx, _my);
}

void UtoCmePickVisitor::apply(osg::Camera& camera)
{
   if (!camera.isRenderToTextureCamera())
   {
      if (camera.getReferenceFrame() == osg::Camera::RELATIVE_RF)
      {
         if (camera.getTransformOrder() == osg::Camera::POST_MULTIPLY)
         {
            runNestedPickVisitor(camera,
                                 camera.getViewport() ? camera.getViewport() : _lastViewport.get(),
                                 _lastProjectionMatrix * camera.getProjectionMatrix(),
                                 _lastViewMatrix * camera.getViewMatrix(),
                                 _mx,
                                 _my);
         }
         else // PRE_MULTIPLY
         {
            runNestedPickVisitor(camera,
                                 camera.getViewport() ? camera.getViewport() : _lastViewport.get(),
                                 camera.getProjectionMatrix() * _lastProjectionMatrix,
                                 camera.getViewMatrix() * _lastViewMatrix,
                                 _mx,
                                 _my);
         }
      }
      else
      {
         runNestedPickVisitor(camera,
                              camera.getViewport() ? camera.getViewport() : _lastViewport.get(),
                              camera.getProjectionMatrix(),
                              camera.getViewMatrix(),
                              _mx,
                              _my);
      }
   }
}

} // namespace osgUtil
