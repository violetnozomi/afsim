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
// UtoHittestUtil.cpp : implementation file

#include "UtoHittestUtils.hpp"

#include <osg/Texture2D>


TexelIntersectPolicy::TexelIntersectPolicy()
   : m_AlphaThreshold(1.0f / 255.0f)
{
}

TexelIntersectPolicy::~TexelIntersectPolicy() {}

/* UTO Function Block **********************************************************

Function: TexelIntersectPolicy::process_hit

Description:

*******************************************************************************/

bool TexelIntersectPolicy::process_hit(const osg::LineSegment& ls, const osg::Drawable& drawable)
{
   m_ti.set(ls);
   drawable.accept(m_ti);
   if (m_ti._hit)
   {
      // take the first hit out of the hit list
      TriangleHit& hit = m_ti._thl.begin()->second;
      Vec3         in  = *hit._v1 * hit._r1 + *hit._v2 * hit._r2 + *hit._v3 * hit._r3;

      const osg::Image*    img = nullptr;
      const osg::StateSet* ss  = drawable.getStateSet();
      if (ss)
      {
         const osg::StateAttribute* sa = ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
         if (sa)
         {
            const osg::Texture2D* tex = dynamic_cast<const osg::Texture2D*>(sa);
            if (tex)
            {
               img = tex->getImage();
            }
         }
      }

      if (img && img->isImageTranslucent() && GL_RGBA == img->getPixelFormat())
      {
         // extract the texture & vertex coordinates from the drawable
         m_texel.reset(hit._index);
         drawable.accept(m_texel);

         Vec3   vmin, vmax;
         size_t i, max(m_texel._vert.size());
         for (i = 0; i < max; ++i)
         {
            if (i == 0)
            {
               vmin = vmax = m_texel._vert[0];
            }
            else
            {
               maxVec3(vmax, m_texel._vert[i]);
               minVec3(vmin, m_texel._vert[i]);
            }
         }

         Vec2 tmin, tmax;
         max = m_texel._tex.size();
         for (i = 0; i < max; ++i)
         {
            if (i == 0)
            {
               tmin = tmax = m_texel._tex[0];
            }
            else
            {
               maxVec2(tmax, m_texel._tex[i]);
               minVec2(tmin, m_texel._tex[i]);
            }
         }

         // we now linearly interpolate to get the texel
         // coordinate (using x and y)
         double s = tmin._v[0] + (tmax._v[0] - tmin._v[0]) * (in._v[0] - vmin._v[0]) / (vmax._v[0] - vmin._v[0]);
         double t = tmin._v[1] + (tmax._v[1] - tmin._v[1]) * (in._v[1] - vmin._v[1]) / (vmax._v[1] - vmin._v[1]);

         // normalize result
         s = fmodf(s, 1.0f);
         t = fmodf(t, 1.0f);

         unsigned int u = (unsigned int)(s * img->s() + 0.5f);
         unsigned int v = (unsigned int)(t * img->t() + 0.5f);

         const unsigned char* clr = img->data(u, v);

         // count an alpha < threshold to be a "miss"
         if ((float)clr[3] / 255.0f < m_AlphaThreshold)
         {
            m_ti._hit = false;
            m_ti._thl.clear();
         }
      }
   }
   return m_ti._hit;
}


/* UTO Function Block **********************************************************

Function: TexelIntersectPolicy::onEnterNode

Description:

*******************************************************************************/

void TexelIntersectPolicy::onEnterNode(osg::Node& node)
{
   // by default push the last image on the stack to simplify onLeaveNode()
   /*
   osg::Image* img = imageStack.empty() ? 0 : imageStack.back().get();

   osg::StateSet* ss = node.getStateSet();
   if (ss)
   {
      osg::StateAttribute* sa = ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
      if (sa)
      {

         osg::Texture2D* tex = dynamic_cast<osg::Texture2D*>(sa);
         if (tex)
         {
            img = tex->getImage();
         }
      }
   }
   imageStack.push_back(img);
   */
}

/* UTO Function Block **********************************************************

Function: TexelIntersectPolicy::onLeaveNode

Description:

*******************************************************************************/

void TexelIntersectPolicy::onLeaveNode(osg::Node& node)
{
   // imageStack.pop_back();
}
