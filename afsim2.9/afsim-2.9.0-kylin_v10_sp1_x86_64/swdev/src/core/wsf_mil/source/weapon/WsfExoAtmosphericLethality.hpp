// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFEXOATMOSPHERICLETHALITY_HPP
#define WSFEXOATMOSPHERICLETHALITY_HPP

#include "wsf_mil_export.h"

#include <vector>

class UtInputBlock;
#include "UtRandom.hpp"
#include "UtVec3.hpp"
#include "WsfExplicitWeaponEffects.hpp"

//! A weapon effect that maps Probability of Kill (Pk) into terminal velocity conditions.
//!
//! This class is a specialization of WsfExplicitWeaponEffects that implements linearly varying
//! target kill probabilities onto the terminal velocities of an engagement.  The independent
//! variable choices are:  Impact Velocity and Impact Angle.  A Pk contribution of each of these
//! may be combined into a final Pk, against which a random draw is made for kill determination.

class WSF_MIL_EXPORT WsfExoAtmosphericLethality : public WsfExplicitWeaponEffects
{
public:
   WsfExoAtmosphericLethality(WsfScenario& aScenario);
   ~WsfExoAtmosphericLethality() override = default;

   WsfWeaponEffects* Clone() const override;
   bool              PreInitialize() override;
   bool              Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr) override;
   bool              ProcessInput(UtInput& aInput) override;
   void              CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr) override;
   void              Terminate(double aSimTime) override;

protected:
   // The "pair" below is (Independent Value, resulting Pk);
   using IvPkPair = std::pair<double, double>;
   using Points   = std::vector<IvPkPair>;

   WsfExoAtmosphericLethality(const WsfExoAtmosphericLethality& aSrc) = default;

   //! Prevent use of operator= by declaring, but not defining.
   WsfExoAtmosphericLethality& operator=(const WsfExoAtmosphericLethality& aRhs) = delete;

   // void ProcessDebrisBlock(UtInputBlock& aInputBlock);
   double InterpPkArray(double aIV_Value, const Points& aArray) const;
   double GetVelocityPk(double aVelocityValue);
   double GetAnglePk(double aAngleValue);

   /*class DebrisType
   {
      public:
         double mProjectileDispersion;
         double mTangentialVelocitySigma;
         double mDetectableMassFraction;
         double mMinDetectableParticleMass;
         //double mMaxParticleMass;
         double mCDA;
         bool ProcessInput(UtInput& aInput);
         void ProcessBlock(UtInputBlock& aInputBlock);
         DebrisType();
   };

   struct PlatformData {
   public:
      PlatformData(double aMass, const double aVelWCS[]) : mMass(aMass)
      {
         UtVec3d::Set(mVelWCS, aVelWCS);
      }
      double mMass;
      double mVelWCS[3];
   };

   typedef std::vector<DebrisType> DebrisTypes;*/

private:
   bool   mEnableDebris;
   Points mImpactVelocityPoints;
   Points mImpactAnglePoints;
   // DebrisTypes mDebrisTypes;
   ut::Random mRandom;
};

#endif
