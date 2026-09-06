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
// UtoHittestUtils.hpp : header file

#if !defined(_UTOHITTESTUTIL_HPP_)
#define _UTOHITTESTUTIL_HPP_

#include <algorithm>
#include <list>

#include <osg/LineSegment>
#include <osg/TriangleFunctor>

#include "UtoIsect.hpp"
#include "UtoTypes.hpp"

using osg::LineSegment;
using osg::TriangleFunctor;
using osg::Vec2;
using osg::Vec3;
using osgUtil2::TriangleHit;

struct TriangleIntersect
{
   osg::ref_ptr<LineSegment> _seg;

   Vec3  _s;
   Vec3  _d;
   float _length;

   int   _index;
   float _ratio;
   bool  _hit;


   typedef std::multimap<float, TriangleHit> TriangleHitList;

   TriangleHitList _thl;

   TriangleIntersect() {}

   TriangleIntersect(const LineSegment& seg, float ratio = FLT_MAX) { set(seg, ratio); }

   void set(const LineSegment& seg, float ratio = FLT_MAX)
   {
      _thl.clear();

      _seg   = new LineSegment(seg);
      _hit   = false;
      _index = 0;
      _ratio = ratio;

      _s      = _seg->start();
      _d      = _seg->end() - _seg->start();
      _length = _d.length();
      _d /= _length;
   }

   //   bool intersect(const Vec3& v1,const Vec3& v2,const Vec3& v3,float& r)
   inline void operator()(const Vec3& v1, const Vec3& v2, const Vec3& v3, bool treatVertexDataAsTemporary = true)
   {
      ++_index;

      if (v1 == v2 || v2 == v3 || v1 == v3)
         return;

      Vec3  v12  = v2 - v1;
      Vec3  n12  = v12 ^ _d;
      float ds12 = (_s - v1) * n12;
      float d312 = (v3 - v1) * n12;
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

      Vec3  v23  = v3 - v2;
      Vec3  n23  = v23 ^ _d;
      float ds23 = (_s - v2) * n23;
      float d123 = (v1 - v2) * n23;
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

      Vec3  v31  = v1 - v3;
      Vec3  n31  = v31 ^ _d;
      float ds31 = (_s - v3) * n31;
      float d231 = (v2 - v3) * n31;
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

      Vec3 in = v1 * r1 + v2 * r2 + v3 * r3;

      float d = (in - _s) * _d;

      if (d < 0.0f)
         return;
      if (d > _length)
         return;

      osg::Vec3 normal = v12 ^ v23;
      normal.normalize();

      float r = d / _length;

      if (!in.valid())
      {
         osg::notify(osg::WARN) << "Warning:: Picked up error in TriangleIntersect" << std::endl;
         // osg::notify(osg::WARN)<<"   ("<<v1<<",\t"<<v2<<",\t"<<v3<<")"<<std::endl;
         // osg::notify(osg::WARN)<<"   ("<<r1<<",\t"<<r2<<",\t"<<r3<<")"<<std::endl;
         return;
      }

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
};

class TriangleIntersectPolicy
{
public:
   bool process_hit(const osg::LineSegment& ls, const osg::Drawable& drawable)
   {
      m_ti.set(ls);
      drawable.accept(m_ti);
      return m_ti._hit;
   }

   void first_hit() { m_thitr = m_ti._thl.begin(); }
   bool done_hit() { return m_thitr == m_ti._thl.end(); }
   void next_hit() { ++m_thitr; }

   osgUtil2::TriangleHit& getHit() const { return m_thitr->second; }
   float                  getRatio() const { return m_thitr->first; }

   void onEnterNode(osg::Node& node) {}
   void onLeaveNode(osg::Node& node) {}

protected:
   TriangleFunctor<TriangleIntersect>           m_ti;
   TriangleIntersect::TriangleHitList::iterator m_thitr;
};


class TexelIntersectPolicy
{
public:
   class TexelIntersectAttributeFunctor : public osg::Drawable::ConstAttributeFunctor
   {
   public:
      TexelIntersectAttributeFunctor()
         : _index(0)
      {
      }
      ~TexelIntersectAttributeFunctor() override {}

      void apply(osg::Drawable::AttributeType attr, unsigned int n, const Vec3* pt) override
      {
         if (osg::Drawable::VERTICES == attr)
         {
            _vert.reserve(n);
            _vert.assign(pt, pt + n);
         }
      }

      void apply(osg::Drawable::AttributeType attr, unsigned int n, const Vec2* pt) override
      {
         if (osg::Drawable::TEXTURE_COORDS_0 == attr)
         {
            _tex.reserve(n);
            _tex.assign(pt, pt + n);
         }
      }

      void reset(int idx)
      {
         _tex.resize(0);
         _vert.resize(0);
      }

      unsigned int              _index;
      typedef std::vector<Vec2> Vec2Vec;
      typedef std::vector<Vec3> Vec3Vec;
      Vec2Vec                   _tex;
      Vec3Vec                   _vert;
   };

   TexelIntersectPolicy();
   ~TexelIntersectPolicy();

   inline void maxVec3(osg::Vec3& o, const osg::Vec3& a)
   {
      o._v[0] = std::max(o._v[0], a._v[0]);
      o._v[1] = std::max(o._v[1], a._v[1]);
      o._v[2] = std::max(o._v[2], a._v[2]);
   }
   inline void minVec3(osg::Vec3& o, const osg::Vec3& a)
   {
      o._v[0] = std::min(o._v[0], a._v[0]);
      o._v[1] = std::min(o._v[1], a._v[1]);
      o._v[2] = std::min(o._v[2], a._v[2]);
   }
   inline void maxVec2(osg::Vec2& o, const osg::Vec2& a)
   {
      o._v[0] = std::max(o._v[0], a._v[0]);
      o._v[1] = std::max(o._v[1], a._v[1]);
   }
   inline void minVec2(osg::Vec2& o, const osg::Vec2& a)
   {
      o._v[0] = std::min(o._v[0], a._v[0]);
      o._v[1] = std::min(o._v[1], a._v[1]);
   }

   bool process_hit(const osg::LineSegment& ls, const osg::Drawable& drawable);

   void first_hit() { m_thitr = m_ti._thl.begin(); }
   bool done_hit() { return m_thitr == m_ti._thl.end(); }
   void next_hit() { ++m_thitr; }

   osgUtil2::TriangleHit& getHit() const { return m_thitr->second; }
   float                  getRatio() const { return m_thitr->first; }

   void onEnterNode(osg::Node& node);
   void onLeaveNode(osg::Node& node);

   // virtual osg::Matrix& getCurMatrix() = 0;

   void setAlphaThreshhold(float alpha) { m_AlphaThreshold = alpha; }

private:
   typedef std::list<osg::ref_ptr<osg::Image>> ImageStack;

   TriangleFunctor<TriangleIntersect>           m_ti;
   TriangleIntersect::TriangleHitList::iterator m_thitr;
   TexelIntersectAttributeFunctor               m_texel;
   // ImageStack m_imageStack;
   float m_AlphaThreshold;
};

#endif
