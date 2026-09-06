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

#include "WkfParticleSystemUpdater.hpp"

#include "VaTimeManager.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::ParticleSystemUpdater::ParticleSystemUpdater()
   : osgParticle::ParticleSystemUpdater()
   , mPaused(false)
   , mRealTimeSinceLastUpdate(0)
{
}

void wkf::ParticleSystemUpdater::traverse(osg::NodeVisitor& nv)
{
   osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(&nv);
   if (cv != nullptr)
   {
      for (unsigned int i = 0; i < getNumParticleSystems(); ++i)
      {
         osgParticle::ParticleSystem* ps = getParticleSystem(i);
         double dt = std::max(vaEnv.GetTime()->GetWallClockTime() - mRealTimeSinceLastUpdate, 0.0);

         // Don't update the particle system if we're paused.
         if (!IsPaused())
         {
            ps->update(dt, nv);
         }
      }
      mRealTimeSinceLastUpdate = vaEnv.GetTime()->GetWallClockTime();
   }
}
