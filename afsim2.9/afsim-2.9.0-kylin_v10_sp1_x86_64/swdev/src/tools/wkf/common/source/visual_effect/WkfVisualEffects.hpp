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

#ifndef VISUALEFFECT_HPP
#define VISUALEFFECT_HPP

#include "wkf_common_export.h"

#include <osgParticle/ModularEmitter>
#include <osgParticle/Particle>
#include <osgParticle/SectorPlacer>

#include "VaEntity.hpp"
#include "VaUtils.hpp"
#include "WkfPlatform.hpp"
#include "particle_system/WkfParticleSystem.hpp"
#include "particle_system/WkfParticleSystemUpdater.hpp"

namespace wkf
{
class WKF_COMMON_EXPORT VisualEffect
{
public:
   VisualEffect();

   enum ViewerDisplayOption
   {
      cMAP_VIEW,
      cIMMERSIVE_VIEWS,
      cALL_VIEWS
   };

   void Initialize();

   // Should be overridden if an effect has any necessary logic in addition to simply
   // creating particles (i.e. Trailing Effects need to compensate for platform movement).
   virtual void Update(double aTime){};

   void Attach(osg::Group* aGroup, vespa::VaEntity& aAnchor);

   void SetPaused(bool aPause);
   void SetEnabled(bool aEnabled);

   void ScaleParticles(double aScalar);
   void SetParticleLifetime(double aLifetime);

   bool IsDead();
   bool IsEndless() { return mEndless; }

   void KillGradually();
   void KillAllParticles();

   void   SetDuration(double aDuration);
   double GetDuration() const { return mDuration; }

   void   SetCreationTime(double aTime) { mCreationTime = aTime; }
   double GetCreationTime() const { return mCreationTime; }

   void SetOffset(const osg::Vec3& aOffset) { mOffset = aOffset; }

   ViewerDisplayOption GetDisplayOption() const { return mDisplayOption; }

protected:
   bool mInitialized;
   bool mEndless;
   bool mDying;

   double           mDuration;
   double           mCreationTime;
   vespa::VaEntity* mAnchorEntity;
   osg::Vec3        mOffset;

   osgParticle::Particle                     mParticleTemplate;
   osg::ref_ptr<osgParticle::ModularEmitter> mEmitter;
   osg::ref_ptr<ParticleSystem>              mSystem;
   osg::ref_ptr<ParticleSystemUpdater>       mUpdater;

   ViewerDisplayOption mDisplayOption;

private:
   // All visual effects should implement this function to set up their
   // particle emitter, system and updater.
   virtual void PrivateInitialize() = 0;
};

// Trailing effects emanate from a platform and typically last indefinitely.
class WKF_COMMON_EXPORT TrailingEffect : public VisualEffect
{
public:
   TrailingEffect();

   // Creates additional particles to compensate for the movement of the source platform,
   // as well as updates the shooting angles to reflect the platform's orientation.
   void Update(double aTime) override;

   void           SetSourcePlatform(wkf::Platform& aSourcePlatform);
   wkf::Platform* GetSourcePlatform() const { return mSourcePlatform; }

   bool IsFromEngine() const { return mEmanatesFromEngine; }
   bool IsShort() const { return mShort; }

   void   SetAngleVariance(double aVariance) { mAngleVariance = aVariance; }
   double GetAngleVariance() const { return mAngleVariance; }

   void SetInitialParticleSize(double aSize);

protected:
   bool mEmanatesFromEngine;
   bool mShort; // Short effects are attached directly to the source platform.

   double         mParticleCountMultiplier;
   double         mAngleVariance;
   wkf::Platform* mSourcePlatform;

private:
   void AdjustShootingAngles();
   void CreateParticlesForMovementCompensation();

   vespa::VaPosition mPreviousPosition;
   double            mPreviousSimTime;
};

class WKF_COMMON_EXPORT ExplosionEffect : public VisualEffect
{
public:
   ExplosionEffect();

private:
   void PrivateInitialize() override;
};

class WKF_COMMON_EXPORT ExplosionDebrisEffect : public VisualEffect
{
public:
   ExplosionDebrisEffect();

private:
   void PrivateInitialize() override;
};

// Iconified explosions are less realistic and intended to be shown on the Map Display only.
class WKF_COMMON_EXPORT IconifiedExplosionEffect : public VisualEffect
{
public:
   IconifiedExplosionEffect();

private:
   void PrivateInitialize() override;
};

class WKF_COMMON_EXPORT FireEffect : public TrailingEffect
{
public:
   FireEffect();

private:
   void PrivateInitialize() override;
};

class WKF_COMMON_EXPORT AfterburnerEffect : public TrailingEffect
{
public:
   AfterburnerEffect();

private:
   void PrivateInitialize() override;
};

class WKF_COMMON_EXPORT LaunchFlashEffect : public TrailingEffect
{
public:
   LaunchFlashEffect();

private:
   void PrivateInitialize() override;
};

class WKF_COMMON_EXPORT EngineOperatingEffect : public TrailingEffect
{
public:
   EngineOperatingEffect();

private:
   void PrivateInitialize() override;
};

class WKF_COMMON_EXPORT ContrailEffect : public TrailingEffect
{
public:
   ContrailEffect();

private:
   void PrivateInitialize() override;
};

class WKF_COMMON_EXPORT RocketSmokeEffect : public TrailingEffect
{
public:
   RocketSmokeEffect();

private:
   void PrivateInitialize() override;
};

class WKF_COMMON_EXPORT DamageSmokeEffect : public TrailingEffect
{
public:
   DamageSmokeEffect();

private:
   void PrivateInitialize() override;
};

class WKF_COMMON_EXPORT EngineSmokeEffect : public TrailingEffect
{
public:
   EngineSmokeEffect();

private:
   void PrivateInitialize() override;
};

class WKF_COMMON_EXPORT SmokePlumeEffect : public TrailingEffect
{
public:
   SmokePlumeEffect();

private:
   void PrivateInitialize() override;
};
} // namespace wkf

#endif // !WKFVISUALEFFECT_HPP
