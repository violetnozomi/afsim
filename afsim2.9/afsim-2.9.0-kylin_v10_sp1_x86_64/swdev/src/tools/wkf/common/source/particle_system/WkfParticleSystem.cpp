// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfParticleSystem.hpp"

wkf::ParticleSystem::ParticleSystem()
   : osgParticle::ParticleSystem()
   , mScaling(false)
{
}

wkf::ParticleSystem::ParticleSystem(const ParticleSystem& copy, const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/)
   : osgParticle::ParticleSystem(copy, copyop)
{
}

// this is nearly identical to the osgParticle::ParticleSystem drawImplementation, but we call our SinglePassRenderer
// as opposed to the parent classes single_pass_renderer.  Our method requires the projection matrix in addition to the
// modelview matrix
void wkf::ParticleSystem::drawImplementation(osg::RenderInfo& renderInfo) const
{
   ScopedReadLock lock(_readWriteMutex);

   osg::State& state = *renderInfo.getState();

   _last_frame = state.getFrameStamp()->getFrameNumber();

   if (_particles.size() <= 0)
      return;

   _dirty_dt = true;

   osg::Matrix modelview  = state.getModelViewMatrix();
   osg::Matrix projection = state.getProjectionMatrix();

   osgParticle::ParticleSystem::ArrayData& ad = _bufferedArrayData[state.getContextID()];

   if (_useVertexArray)
   {
      RenderVertexArray(ad);
   }
   else
   {
      RenderSinglePass(ad, modelview, projection);
   }

   // set up depth mask for first rendering pass
#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE) && !defined(OSG_GLES3_AVAILABLE) && \
   !defined(OSG_GL3_AVAILABLE)
   glPushAttrib(GL_DEPTH_BUFFER_BIT);
#endif

   glDepthMask(GL_FALSE);

   ad.dispatchArrays(state);
   ad.dispatchPrimitives();

#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE) && !defined(OSG_GLES3_AVAILABLE) && \
   !defined(OSG_GL3_AVAILABLE)
   // restore depth mask settings
   glPopAttrib();
#endif

   // render, second pass
   if (_doublepass)
   {
      // set up color mask for second rendering pass
#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE) && !defined(OSG_GLES3_AVAILABLE) && \
   !defined(OSG_GL3_AVAILABLE)
      glPushAttrib(GL_COLOR_BUFFER_BIT);
#endif
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

      ad.dispatchPrimitives();

#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE) && !defined(OSG_GLES3_AVAILABLE) && \
   !defined(OSG_GL3_AVAILABLE)
      // restore color mask settings
      glPopAttrib();
#endif
   }
}

void wkf::ParticleSystem::RenderVertexArray(osgParticle::ParticleSystem::ArrayData& aArrayData) const
{
   if (!aArrayData.vertices.valid())
   {
      aArrayData.init3();
      aArrayData.reserve(_particles.capacity());
   }

   aArrayData.clear();
   aArrayData.dirty();

   osg::Vec3Array&        vertices   = *aArrayData.vertices;
   osg::Vec3Array&        normals    = *aArrayData.normals;
   osg::Vec4Array&        colors     = *aArrayData.colors;
   osg::Vec3Array&        texcoords  = *aArrayData.texcoords3;
   ArrayData::Primitives& primitives = aArrayData.primitives;

   for (unsigned int i = 0; i < _particles.size(); i += _detail)
   {
      const osgParticle::Particle* particle = &_particles[i];
      const osg::Vec4&             color    = particle->getCurrentColor();
      const osg::Vec3&             pos      = particle->getPosition();
      const osg::Vec3&             vel      = particle->getVelocity();
      colors.push_back(color);
      texcoords.push_back(osg::Vec3(particle->isAlive(), particle->getCurrentSize(), particle->getCurrentAlpha()));
      normals.push_back(vel);
      vertices.push_back(pos);
   }

   primitives.push_back(ArrayData::ModeCount(GL_POINTS, vertices.size()));
}

void wkf::ParticleSystem::RenderSinglePass(osgParticle::ParticleSystem::ArrayData& aArrayData,
                                           const osg::Matrix&                      aModelView,
                                           const osg::Matrix&                      aProjection) const
{
   // set up arrays and primitives ready to fill in
   if (!aArrayData.vertices.valid())
   {
      aArrayData.init();
      aArrayData.reserve(_particles.capacity() * 4);
   }

   aArrayData.clear();
   aArrayData.dirty();

   osg::Vec3Array&        vertices   = *aArrayData.vertices;
   osg::Vec4Array&        colors     = *aArrayData.colors;
   osg::Vec2Array&        texcoords  = *aArrayData.texcoords2;
   ArrayData::Primitives& primitives = aArrayData.primitives;


   float scale = sqrtf(static_cast<float>(_detail));

   osg::Vec3 xAxis = _align_X_axis;
   osg::Vec3 yAxis = _align_Y_axis;

   osg::Vec3 scaled_aligned_xAxis = _align_X_axis;
   osg::Vec3 scaled_aligned_yAxis = _align_Y_axis;

   float xScale = 1.0f;
   float yScale = 1.0f;

   if (_alignment == BILLBOARD)
   {
      xAxis = osg::Matrix::transform3x3(aModelView, _align_X_axis);
      yAxis = osg::Matrix::transform3x3(aModelView, _align_Y_axis);

      float lengthX2 = xAxis.length2();
      float lengthY2 = yAxis.length2();

      if (_particleScaleReferenceFrame == LOCAL_COORDINATES)
      {
         xScale = 1.0f / sqrtf(lengthX2);
         yScale = 1.0f / sqrtf(lengthY2);
      }
      else
      {
         xScale = 1.0f / lengthX2;
         yScale = 1.0f / lengthY2;
      }

      scaled_aligned_xAxis *= xScale;
      scaled_aligned_yAxis *= yScale;

      xAxis *= xScale;
      yAxis *= yScale;
   }

   for (unsigned int i = 0; i < _particles.size(); i += _detail)
   {
      const osgParticle::Particle* currentParticle = &_particles[i];
      double                       screenScale     = 1.0;
      if (mScaling)
      {
         // this is the custom screen scaling code, it gets applied in the calls to osgParticle::Particle::render
         osg::Vec4 projP = aProjection.preMult(aModelView.preMult(osg::Vec4(currentParticle->getPosition(), 1.0)));
         screenScale     = projP[3] * 0.001;
      }

      bool insideDistance = true;
      if (_sortMode != NO_SORT && _visibilityDistance > 0.0)
      {
         insideDistance = (currentParticle->getDepth() >= 0.0 && currentParticle->getDepth() <= _visibilityDistance);
      }

      if (currentParticle->isAlive() && insideDistance)
      {
         const osg::Vec3& angle            = currentParticle->getAngle();
         bool             requiresRotation = (angle.x() != 0.0f || angle.y() != 0.0f || angle.z() != 0.0f);
         if (requiresRotation)
         {
            osg::Matrix R;
            R.makeRotate(angle.x(), osg::Vec3(1, 0, 0), angle.y(), osg::Vec3(0, 1, 0), angle.z(), osg::Vec3(0, 0, 1));

            if (_alignment == BILLBOARD)
            {
               xAxis = osg::Matrix::transform3x3(R, scaled_aligned_xAxis);
               xAxis = osg::Matrix::transform3x3(aModelView, xAxis);

               yAxis = osg::Matrix::transform3x3(R, scaled_aligned_yAxis);
               yAxis = osg::Matrix::transform3x3(aModelView, yAxis);
            }
            else
            {
               xAxis = osg::Matrix::transform3x3(R, scaled_aligned_xAxis);
               yAxis = osg::Matrix::transform3x3(R, scaled_aligned_yAxis);
            }
         }

         osg::Vec4 color = currentParticle->getCurrentColor();
         color.a() *= currentParticle->getCurrentAlpha();

         float currentSize = currentParticle->getCurrentSize();

         const osg::Vec3& xpos    = currentParticle->getPosition();
         const float      s_coord = currentParticle->getSTexCoord();
         const float      t_coord = currentParticle->getTTexCoord();
         const float      s_tile  = currentParticle->getSTexTile();
         const float      t_tile  = currentParticle->getTTexTile();

         osg::Vec3 p1(xAxis * currentSize * scale * screenScale);
         osg::Vec3 p2(yAxis * currentSize * scale * screenScale);

         switch (currentParticle->getShape())
         {
         case osgParticle::Particle::POINT:
         {
            vertices.push_back(currentParticle->getPosition());
            colors.push_back(color);
            texcoords.push_back(osg::Vec2(0.5f, 0.5f));

            if (!primitives.empty() && primitives.back().first == GL_POINTS)
            {
               primitives.back().second++;
            }
            else
            {
               primitives.push_back(ArrayData::ModeCount(GL_POINTS, 1));
            }

            break;
         }
         case osgParticle::Particle::USER:
         case osgParticle::Particle::QUAD_TRIANGLESTRIP:
         case osgParticle::Particle::HEXAGON:
         case osgParticle::Particle::QUAD:
         {
            const osg::Vec3 c0(xpos - p1 - p2);
            const osg::Vec2 t0(s_coord, t_coord);
            const osg::Vec3 c1(xpos + p1 - p2);
            const osg::Vec2 t1(s_coord + s_tile, t_coord);
            const osg::Vec3 c2(xpos + p1 + p2);
            const osg::Vec2 t2(s_coord + s_tile, t_coord + t_tile);
            const osg::Vec3 c3(xpos - p1 + p2);
            const osg::Vec2 t3(s_coord, t_coord + t_tile);

            // First 3 points (and texcoords) of quad or triangle
            vertices.push_back(c0);
            vertices.push_back(c1);
            vertices.push_back(c2);
            texcoords.push_back(t0);
            texcoords.push_back(t1);
            texcoords.push_back(t2);

#if !defined(OSG_GLES2_AVAILABLE)
            const unsigned int count = 4;
            const GLenum       mode  = GL_QUADS;

            // Last point (and texcoord) of quad
            vertices.push_back(c3);
            texcoords.push_back(t3);
#else
            // No GL_QUADS mode on GLES2 and upper
            const unsigned int count = 6;
            const GLenum       mode  = GL_TRIANGLES;

            // Second triangle
            vertices.push_back(c2);
            vertices.push_back(c3);
            vertices.push_back(c0);
            texcoords.push_back(t2);
            texcoords.push_back(t3);
            texcoords.push_back(t0);
#endif
            for (unsigned int j = 0; j < count; ++j)
               colors.push_back(color);

            if (!primitives.empty() && primitives.back().first == mode)
            {
               primitives.back().second += count;
            }
            else
            {
               primitives.push_back(ArrayData::ModeCount(mode, count));
            }

            break;
         }
         case osgParticle::Particle::LINE:
         {
            // Get the normalized direction of the particle, to be used in the
            // calculation of one of the linesegment endpoints.
            const osg::Vec3& velocity = currentParticle->getVelocity();
            float            vl       = velocity.length();
            if (vl != 0)
            {
               osg::Vec3 v = velocity * currentSize * scale / vl;

               vertices.push_back(currentParticle->getPosition());
               colors.push_back(color);
               texcoords.push_back(osg::Vec2(0.0f, 0.0f));

               vertices.push_back(currentParticle->getPosition() + v);
               colors.push_back(color);
               texcoords.push_back(osg::Vec2(1.0f, 1.0f));

               if (!primitives.empty() && primitives.back().first == GL_LINES)
               {
                  primitives.back().second += 2;
               }
               else
               {
                  primitives.push_back(ArrayData::ModeCount(GL_LINES, 2));
               }
            }
            break;
         }

         default:
            OSG_WARN << "Invalid shape for particles\n";
         }
      }
   }
}
