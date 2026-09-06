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

#ifndef WKFPARTICLESYSTEMUPDATER_HPP
#define WKFPARTICLESYSTEMUPDATER_HPP

#include "wkf_common_export.h"

#include <osgParticle/ParticleSystemUpdater>

namespace wkf
{
// By default, osg::ParticleSystemUpdater updates all of its particle systems based on the elapsed time between frames.
// We override the traversal method to update the particle systems based on the elapsed real time,
// as well as provide a way of pausing the effects.
class WKF_COMMON_EXPORT ParticleSystemUpdater : public osgParticle::ParticleSystemUpdater
{
public:
   ParticleSystemUpdater();

   void traverse(osg::NodeVisitor& nv) override;

   void Pause(bool aPause) { mPaused = aPause; }
   bool IsPaused() const { return mPaused; }

private:
   bool   mPaused;
   double mRealTimeSinceLastUpdate;
};
} // namespace wkf

#endif // !WKFPARTICLESYSTEMUPDATER_HPP
