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

#ifndef WKFPARTICLESYSTEM
#define WKFPARTICLESYSTEM

#include "wkf_common_export.h"

#include <osgParticle/ParticleSystem>

namespace wkf
{
// this class overrides osgParticle::ParticleSystem so that we can screen-scale the particles
class WKF_COMMON_EXPORT ParticleSystem : public osgParticle::ParticleSystem
{
public:
   ParticleSystem();
   ParticleSystem(const ParticleSystem& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

   void drawImplementation(osg::RenderInfo& renderInfo) const override;
   void RenderVertexArray(osgParticle::ParticleSystem::ArrayData& aArrayData) const;
   void RenderSinglePass(osgParticle::ParticleSystem::ArrayData& aArrayData,
                         const osg::Matrix&                      aModelView,
                         const osg::Matrix&                      aProjection) const;

   void SetScaling(bool aScaling) { mScaling = aScaling; }

private:
   bool mScaling;
};

} // namespace wkf

#endif
