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

#include "WkfVisualEffects.hpp"

#include "UtEllipsoidalEarth.hpp"
#include "UtoEntity.hpp"
#include "UtoMemory.hpp"
#include "UtoShape.hpp"
#include "WkfEnvironment.hpp"
#include "WkfVisualEffectsPrefObject.hpp"

namespace
{
// Moves a position towards another position by a fixed distance, where the distance is in meters.
void MovePosition(vespa::VaPosition& aPosition, const vespa::VaPosition& aTargetPosition, double aDistance)
{
   double ecef[3];

   // First, normalize the vector between the target position and the given position.
   ecef[0] = aTargetPosition.GetECEF_X() - aPosition.GetECEF_X();
   ecef[1] = aTargetPosition.GetECEF_Y() - aPosition.GetECEF_Y();
   ecef[2] = aTargetPosition.GetECEF_Z() - aPosition.GetECEF_Z();

   double magnitude = std::sqrt(std::pow(ecef[0], 2) + std::pow(ecef[1], 2) + std::pow(ecef[2], 2));
   ecef[0] /= magnitude;
   ecef[1] /= magnitude;
   ecef[2] /= magnitude;

   // Next, advance the ECEF coordinates by the given distance, then convert them to LLA coordinates
   // and move mPreviousPosition to the new location.
   double newEcef[3];
   newEcef[0] = aPosition.GetECEF_X();
   newEcef[1] = aPosition.GetECEF_Y();
   newEcef[2] = aPosition.GetECEF_Z();

   newEcef[0] += aDistance * ecef[0];
   newEcef[1] += aDistance * ecef[1];
   newEcef[2] += aDistance * ecef[2];

   // Finally, convert the new ECEF coordinates to latitude/longitude and set the given position
   // to its new location.
   double lat, lon, alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(newEcef, lat, lon, alt);

   aPosition.SetLat(lat);
   aPosition.SetLon(lon);
   aPosition.SetAlt(alt);
}
} // namespace

wkf::VisualEffect::VisualEffect()
   : mInitialized(false)
   , mEndless(false)
   , mDying(false)
   , mDuration(0)
   , mCreationTime(0)
   , mAnchorEntity(nullptr)
   , mEmitter(new osgParticle::ModularEmitter())
   , mSystem(new wkf::ParticleSystem())
   , mUpdater(new wkf::ParticleSystemUpdater())
   , mDisplayOption(cALL_VIEWS)
{
}

void wkf::VisualEffect::Initialize()
{
   PrivateInitialize();
   mInitialized = true;
}

void wkf::VisualEffect::Attach(osg::Group* aGroup, vespa::VaEntity& aAnchor)
{
   if (mInitialized)
   {
      osg::ref_ptr<osg::Geode> geode = new osg::Geode();
      aGroup->addChild(geode);
      geode->addDrawable(mSystem);

      aGroup->addChild(mEmitter);
      aGroup->addChild(mUpdater);

      mAnchorEntity = &aAnchor;
   }
}

void wkf::VisualEffect::SetPaused(bool aPause)
{
   if (mInitialized)
   {
      mUpdater->Pause(aPause); // Stops current particles from moving.

      // Don't re-enable the emitter if the effect is dying.
      if (!mDying)
      {
         mEmitter->setEnabled(!aPause); // Prevents the creation of new particles.
      }
   }
}

void wkf::VisualEffect::SetEnabled(bool aEnabled)
{
   if (mInitialized)
   {
      mEmitter->setEnabled(aEnabled);
   }
}

void wkf::VisualEffect::ScaleParticles(double aScalar)
{
   if (mInitialized)
   {
      for (int i = 0; i < mSystem->numParticles(); ++i)
      {
         osgParticle::rangef sizeRange = mSystem->getDefaultParticleTemplate().getSizeRange();
         sizeRange.set(sizeRange.minimum * aScalar, sizeRange.maximum * aScalar);
         mSystem->getParticle(i)->setSizeRange(sizeRange);
      }

      osg::NodeVisitor nv;
      mSystem->update(0, nv);
   }
}

void wkf::VisualEffect::SetParticleLifetime(double aLifetime)
{
   if (mInitialized)
   {
      for (int i = 0; i < mSystem->numParticles(); ++i)
      {
         mSystem->getParticle(i)->setLifeTime(aLifetime);
      }

      osg::NodeVisitor nv;
      mSystem->update(0, nv);
   }
}

bool wkf::VisualEffect::IsDead()
{
   if (mInitialized)
   {
      return !(mEmitter->getEnabled()) && mSystem->areAllParticlesDead();
   }
   else
   {
      return false;
   }
}

void wkf::VisualEffect::KillGradually()
{
   SetEnabled(false);
   SetDuration(0);

   mDying = true;
}

void wkf::VisualEffect::KillAllParticles()
{
   if (mInitialized)
   {
      for (int i = 0; i < mSystem->numParticles(); ++i)
      {
         mSystem->destroyParticle(i);
      }

      osg::NodeVisitor nv;
      mSystem->update(0, nv);
   }
}

void wkf::VisualEffect::SetDuration(double aDuration)
{
   mDuration = aDuration;
   mEndless  = false;
}

wkf::TrailingEffect::TrailingEffect()
   : VisualEffect()
   , mEmanatesFromEngine(true)
   , mShort(false)
   , mParticleCountMultiplier(1.0)
   , mAngleVariance(osg::PI / 64)
   , mSourcePlatform(nullptr)
{
   mEndless       = true;
   mDisplayOption = cIMMERSIVE_VIEWS;
}

void wkf::TrailingEffect::Update(double aTime)
{
   if (mInitialized && mSourcePlatform != nullptr && !mUpdater->IsPaused() && mEmitter->isEnabled())
   {
      CreateParticlesForMovementCompensation();
      AdjustShootingAngles();
   }

   mPreviousPosition = mSourcePlatform->GetPosition();
}

void wkf::TrailingEffect::SetSourcePlatform(wkf::Platform& aSourcePlatform)
{
   mSourcePlatform   = &aSourcePlatform;
   mPreviousPosition = mSourcePlatform->GetPosition();
}

void wkf::TrailingEffect::SetInitialParticleSize(double aSize)
{
   if (mInitialized)
   {
      double max = mParticleTemplate.getSizeRange().maximum;
      mParticleTemplate.setSizeRange(osgParticle::rangef(aSize, max));

      mSystem->setDefaultParticleTemplate(mParticleTemplate);
   }
}

void wkf::TrailingEffect::AdjustShootingAngles()
{
   osgParticle::RadialShooter* radialShooter = dynamic_cast<osgParticle::RadialShooter*>(mEmitter->getShooter());
   if (radialShooter != nullptr)
   {
      if (mSourcePlatform->GetVelocityWCS().Magnitude() > 0)
      {
         UtVec3d exhaustUnitVector =
            ((mSourcePlatform->GetVelocityWCS() / mSourcePlatform->GetVelocityWCS().Magnitude()) * -1);

         double newTheta = std::acos(exhaustUnitVector[2]) * (exhaustUnitVector[0] < 0 ? -1 : 1);
         double newPhi   = std::atan(exhaustUnitVector[1] / exhaustUnitVector[0]);

         radialShooter->setThetaRange(newTheta - mAngleVariance, newTheta + mAngleVariance);
         radialShooter->setPhiRange(newPhi - mAngleVariance, newPhi + mAngleVariance);
      }
   }
}

void wkf::TrailingEffect::CreateParticlesForMovementCompensation()
{
   // Account for "skipped" distance between previous update and this current one.
   // First, determine the amount of skipped distance (in meters).
   vespa::VaPosition currentPosition = mSourcePlatform->GetPosition();

   double heading, distance = 0;
   mPreviousPosition.CalculateGreatCircleHeadingAndDistance(currentPosition, heading, distance);
   distance +=
      std::fabs(currentPosition.GetAlt() - mPreviousPosition.GetAlt()); // Take the difference in altitude into account.

   // If the effect is short, move the previous position much closer to the
   // current position before creating any particles.
   if (mShort)
   {
      MovePosition(mPreviousPosition, currentPosition, distance / 1.05);
   }

   // osgParticle stores particle sizes in terms of meters, so we can calculate how many particles
   // should be created in order to fill the gap.
   double minParticleSize = mSystem->getDefaultParticleTemplate().getSizeRange().minimum;
   minParticleSize *= 1.5; // Increase the minimum size to help prevent overdraw.

   int numParticlesToCreate = distance / minParticleSize;
   numParticlesToCreate     = std::min(200, numParticlesToCreate); // Enforce a maximum of 200 particles to create.

   // For each new particle to create, set the placer at the previous position, place the particle,
   // then update the previous position by moving it closer to the new position by a value equal to
   // the particle's size.
   double speed = mSourcePlatform->GetVelocityWCS().Magnitude(); // Determine the platform's speed for use in manually
                                                                 // updating particles.
   double ecef[3];
   for (int i = 0; i <= numParticlesToCreate; ++i)
   {
      // Update the particle placer's location.
      if (mShort)
      {
         ecef[0] = 0;
         ecef[1] = 0;
         ecef[2] = 0;
      }
      else
      {
         ecef[0] = mPreviousPosition.GetECEF_X() - mAnchorEntity->GetPosition().GetECEF_X();
         ecef[1] = mPreviousPosition.GetECEF_Y() - mAnchorEntity->GetPosition().GetECEF_Y();
         ecef[2] = mPreviousPosition.GetECEF_Z() - mAnchorEntity->GetPosition().GetECEF_Z();
      }

      // Take the engine offset into account.
      double hpr[3];
      mSourcePlatform->GetUtoEntity()->GetOrientationWCS(hpr[0], hpr[1], hpr[2]);
      auto         q = osg::Quat(hpr[2], osg::Vec3(1, 0, 0), hpr[1], osg::Vec3(0, 1, 0), hpr[0], osg::Vec3(0, 0, 1));
      osg::Matrixf rotation;
      q.get(rotation);

      osg::Vec3 offset = mOffset;
      offset           = rotation.preMult(offset);
      ecef[0] += offset.x();
      ecef[1] += offset.y();
      ecef[2] += offset.z();

      osgParticle::CenteredPlacer* placer = dynamic_cast<osgParticle::CenteredPlacer*>(mEmitter->getPlacer());
      if (placer != nullptr)
      {
         placer->setCenter(ecef[0], ecef[1], ecef[2]);

         // Create and place a particle, then manually update it based on how long it *should*
         // have been around already.
         osgParticle::Particle* p = mSystem->createParticle(&(mSystem->getDefaultParticleTemplate()));
         if (p != nullptr)
         {
            placer->place(p);

            double dt = distance / speed;
            p->update(dt, false);

            distance -= minParticleSize;

            osgParticle::RadialShooter* radShooter = dynamic_cast<osgParticle::RadialShooter*>(mEmitter->getShooter());
            if (radShooter != nullptr)
            {
               radShooter->shoot(p);
            }
         }
      }

      // Move the previous position closer to the current position.
      MovePosition(mPreviousPosition, currentPosition, minParticleSize);

      // On our last particle creation, set the previous position to the current position;
      // this will prevent the distance we need to cover from growing progressively larger.
      if (i == numParticlesToCreate)
      {
         mPreviousPosition = currentPosition;
      }
   }
}

wkf::ExplosionEffect::ExplosionEffect()
   : VisualEffect()
{
   mDuration      = 0.2;
   mDisplayOption = cIMMERSIVE_VIEWS;
}

void wkf::ExplosionEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("explosion.png", fullFilePath);

   // The first argument is the texture file; the second argument enables/disables additive blending.
   // For now, we turn off additive blending so darker colors are displayed correctly.
   mSystem->setDefaultAttributes(fullFilePath, false);

   // Set up the particle template.
   if (mAnchorEntity != nullptr)
   {
      if (mAnchorEntity->GetPosition().GetAlt() > 0)
      {
         mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(1, 1, 1, 1), osg::Vec4(0.0, 0.0, 0.0, 1)));
         mParticleTemplate.setSizeRange(osgParticle::rangef(70, 25));
      }
      else
      {
         mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(1, 1, 1, 1), osg::Vec4(0.0, 0.0, 0.0, 1)));
         mParticleTemplate.setSizeRange(osgParticle::rangef(150, 100));
      }
   }
   else
   {
      mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(1, 1, 1, 1), osg::Vec4(0.0, 0.0, 0.0, 1)));
      mParticleTemplate.setSizeRange(osgParticle::rangef(70, 25));
   }

   mParticleTemplate.setLifeTime(5);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(500, 500));
   mEmitter->setCounter(randomCounter);

   // Ground explosions shoot their particles upwards, rather than in all directions.
   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();

   if (mAnchorEntity != nullptr)
   {
      if (mAnchorEntity->GetPosition().GetAlt() > 0)
      {
         radialShooter->setThetaRange(0, 2 * osg::PI);
         radialShooter->setInitialSpeedRange(osgParticle::rangef(25, 25));
      }
      else
      {
         radialShooter->setThetaRange((-1 * osg::PI) / 4, osg::PI / 4);
         radialShooter->setPhiRange(0, osg::PI);
         radialShooter->setInitialSpeedRange(150, 250);
      }
   }
   else
   {
      radialShooter->setThetaRange(0, 2 * osg::PI);
      radialShooter->setInitialSpeedRange(osgParticle::rangef(25, 25));
   }

   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::ExplosionDebrisEffect::ExplosionDebrisEffect()
   : VisualEffect()
{
   mDuration      = 0.2;
   mDisplayOption = cIMMERSIVE_VIEWS;
}

void wkf::ExplosionDebrisEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("debris.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath, false); // Turn off additive blending.

   // Set up the particle template.
   if (mAnchorEntity != nullptr)
   {
      if (mAnchorEntity->GetPosition().GetAlt() > 0)
      {
         mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.0, 0.0, 0.0, 1), osg::Vec4(0.2, 0.2, 0.2, 1)));
      }
      else
      {
         mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.4, 0.4, 0.4, 1), osg::Vec4(0.2, 0.2, 0.2, 1)));
      }
   }
   else
   {
      mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.0, 0.0, 0.0, 1), osg::Vec4(0.2, 0.2, 0.2, 1)));
   }
   mParticleTemplate.setSizeRange(osgParticle::rangef(10, 5));
   mParticleTemplate.setLifeTime(5);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(150, 250));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   if (mAnchorEntity != nullptr)
   {
      if (mAnchorEntity->GetPosition().GetAlt() > 0)
      {
         radialShooter->setThetaRange(0, 2 * osg::PI);
      }
      else
      {
         radialShooter->setPhiRange(0, osg::PI);
      }
   }
   else
   {
      radialShooter->setThetaRange(0, 2 * osg::PI);
   }
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   radialShooter->setInitialSpeedRange(osgParticle::rangef(50, 75));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::IconifiedExplosionEffect::IconifiedExplosionEffect()
   : VisualEffect()
{
   mDuration      = 0.2;
   mDisplayOption = cMAP_VIEW;
}

void wkf::IconifiedExplosionEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("explosion.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath, false); // Turn off additive blending.
   mSystem->SetScaling(true);

   double sizeScalar = wkfEnv.GetPreferenceObject<wkf::VisualEffectsPrefObject>()->GetPreferences().mMapExplosionScalar;
   double lifetime = wkfEnv.GetPreferenceObject<wkf::VisualEffectsPrefObject>()->GetPreferences().mMapExplosionLifetime;

   // Set up the particle template.
   mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(1, 1, 1, 1), osg::Vec4(0.0, 0.0, 0.0, 1)));
   mParticleTemplate.setSizeRange(osgParticle::rangef(25 * sizeScalar, 15 * sizeScalar));
   mParticleTemplate.setLifeTime(lifetime);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(100, 100));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   radialShooter->setThetaRange(0, 2 * osg::PI);
   radialShooter->setInitialSpeedRange(osgParticle::rangef(25 * sizeScalar, 40 * sizeScalar));
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::FireEffect::FireEffect()
   : TrailingEffect()
{
   mEmanatesFromEngine = false;
   mShort              = true;
}

void wkf::FireEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("smoke.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath);

   // Set up the particle template.
   mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.8, 0.0, 0.0, 1), osg::Vec4(0.5, 0.5, 0.0, 1)));
   mParticleTemplate.setSizeRange(osgParticle::rangef(5, 0));
   mParticleTemplate.setLifeTime(2);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);
   mEmitter->setEndless(true);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(0, 0));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   radialShooter->setInitialSpeedRange(osgParticle::rangef(10, 25));
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::AfterburnerEffect::AfterburnerEffect()
   : TrailingEffect()
{
   mAngleVariance = osg::PI / 128;
   mShort         = true;
}

void wkf::AfterburnerEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("smoke.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath);

   // Set up the particle template.
   mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.8, 0.2, 0.0, 0.5), osg::Vec4(0.5, 0.5, 0.0, 0.1)));
   mParticleTemplate.setAlphaRange(osgParticle::rangef(0.9, 0.0));
   mParticleTemplate.setSizeRange(osgParticle::rangef(0.5, 0.2));
   mParticleTemplate.setLifeTime(0.15);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);
   mEmitter->setEndless(true);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(0, 0));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   radialShooter->setInitialSpeedRange(osgParticle::rangef(0, 25));
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::LaunchFlashEffect::LaunchFlashEffect()
   : TrailingEffect()
{
}

void wkf::LaunchFlashEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("smoke.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath);

   // Set up the particle template.
   mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(1, 1, 1, 1), osg::Vec4(1, 1, 1, 1)));
   mParticleTemplate.setSizeRange(osgParticle::rangef(0.5, 30));
   mParticleTemplate.setLifeTime(15);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(200, 200));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   radialShooter->setInitialSpeedRange(osgParticle::rangef(85, 150));
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::EngineOperatingEffect::EngineOperatingEffect()
   : TrailingEffect()
{
   mShort = true;
}

void wkf::EngineOperatingEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("smoke.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath);

   // Set up the particle template.
   mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(1, 1, 1, 0.1), osg::Vec4(1, 1, 1, 0.1)));
   mParticleTemplate.setAlphaRange(osgParticle::rangef(0.1, 0.0));
   mParticleTemplate.setSizeRange(osgParticle::rangef(0.5, 0));
   mParticleTemplate.setLifeTime(0.1);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);
   mEmitter->setEndless(true);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(0, 0));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   radialShooter->setInitialSpeedRange(osgParticle::rangef(0, 25));
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::ContrailEffect::ContrailEffect()
   : TrailingEffect()
{
   mAngleVariance           = osg::PI / 32;
   mParticleCountMultiplier = 4.0;
}

void wkf::ContrailEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("smoke.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath);

   // Set up the particle template.
   mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(1, 1, 1, 0.01), osg::Vec4(1, 1, 1, 0.4)));
   mParticleTemplate.setSizeRange(osgParticle::rangef(0.5, 10));
   mParticleTemplate.setLifeTime(10);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);
   mEmitter->setEndless(true);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(20, 25));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   radialShooter->setInitialSpeedRange(osgParticle::rangef(5, 15));
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::RocketSmokeEffect::RocketSmokeEffect()
   : TrailingEffect()
{
}

void wkf::RocketSmokeEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("smoke.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath);

   // Set up the particle template.
   mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.9, 0.9, 0.9, 1), osg::Vec4(0.8, 0.8, 0.8, 1)));
   mParticleTemplate.setAlphaRange(osgParticle::rangef(0.8, 0.0));
   mParticleTemplate.setSizeRange(osgParticle::rangef(0.5, 2));
   mParticleTemplate.setLifeTime(1);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);
   mEmitter->setEndless(true);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(50, 55));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   radialShooter->setInitialSpeedRange(osgParticle::rangef(5, 15));
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::DamageSmokeEffect::DamageSmokeEffect()
   : TrailingEffect()
{
   mEmanatesFromEngine = false;
}

void wkf::DamageSmokeEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("smoke.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath, false); // Turn off additive blending.

   // Set up the particle template.
   mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.1, 0.0, 0.0, 1), osg::Vec4(0.0, 0.0, 0.0, 1)));
   mParticleTemplate.setSizeRange(osgParticle::rangef(0.5, 2));
   mParticleTemplate.setLifeTime(0.3);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);
   mEmitter->setEndless(true);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(20, 50));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   radialShooter->setInitialSpeedRange(osgParticle::rangef(5, 15));
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::EngineSmokeEffect::EngineSmokeEffect()
   : TrailingEffect()
{
   mAngleVariance = osg::PI / 128;
   mShort         = true;
}

void wkf::EngineSmokeEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("smoke.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath, false); // Turn off additive blending.

   // Set up the particle template.
   mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.0, 0.0, 0.0, 0.05), osg::Vec4(0.2, 0.2, 0.2, 0.05)));
   mParticleTemplate.setAlphaRange(osgParticle::rangef(0.5, 0.0));
   mParticleTemplate.setSizeRange(osgParticle::rangef(0.5, 2));
   mParticleTemplate.setLifeTime(2);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);
   mEmitter->setEndless(true);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(0, 0));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   radialShooter->setInitialSpeedRange(osgParticle::rangef(5, 15));
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}

wkf::SmokePlumeEffect::SmokePlumeEffect()
   : TrailingEffect()
{
   mAngleVariance = osg::PI / 32;
}

void wkf::SmokePlumeEffect::PrivateInitialize()
{
   std::string fullFilePath;
   vespa::VaUtils::GetPathPrioritizedFilename("smoke.png", fullFilePath);

   mSystem->setDefaultAttributes(fullFilePath, false); // Turn off additive blending.

   // Set up the particle template.
   mParticleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.2, 0.2, 0.2, 1), osg::Vec4(0.0, 0.0, 0.0, 1)));
   mParticleTemplate.setSizeRange(osgParticle::rangef(30, 60));
   mParticleTemplate.setLifeTime(20);
   mSystem->setDefaultParticleTemplate(mParticleTemplate);

   // Set up the emitter.
   mEmitter->setParticleSystem(mSystem);
   mEmitter->setEndless(true);

   auto randomCounter = ut::osg::make_ref<osgParticle::RandomRateCounter>();
   randomCounter->setRateRange(osgParticle::rangef(50, 50));
   mEmitter->setCounter(randomCounter);

   auto radialShooter = ut::osg::make_ref<osgParticle::RadialShooter>();
   radialShooter->setInitialSpeedRange(osgParticle::rangef(50, 100));
   radialShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, -1), osg::Vec3(0, 0, 1));
   mEmitter->setShooter(radialShooter);

   auto sectorPlacer = ut::osg::make_ref<osgParticle::SectorPlacer>();
   sectorPlacer->setRadiusRange(0, 0);
   sectorPlacer->setPhiRange(0, 0);
   mEmitter->setPlacer(sectorPlacer);

   mUpdater->addParticleSystem(mSystem);
}
