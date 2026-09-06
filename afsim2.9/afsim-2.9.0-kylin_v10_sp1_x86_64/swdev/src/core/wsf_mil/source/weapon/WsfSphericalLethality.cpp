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

#include "WsfSphericalLethality.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeaponEngagement.hpp"


WsfSphericalLethality::WsfSphericalLethality(WsfScenario& aScenario)
   : WsfExplicitWeaponEffects(aScenario)
   , mMinRadius(0.0)
   , mThreshold(0.0)
   , mMinDamage(0.0)
   , mMaxDamage(1.0)
   , mExponent(1.0)
{
}

// virtual
WsfWeaponEffects* WsfSphericalLethality::Clone() const
{
   return new WsfSphericalLethality(*this);
}

// virtual
bool WsfSphericalLethality::PreInitialize()
{
   bool result = WsfExplicitWeaponEffects::PreInitialize();

   // Check and adjust any bogus values:
   // Max and min values are forced to have correct relative values:

   if (MaximumEffectiveRadius() <= mMinRadius)
   {
      ut::log::error() << "WsfSphericalLethality Maximum radius must be greater than Minimum.";
      result = false;
   }
   if (mMaxDamage <= mMinDamage)
   {
      ut::log::error() << "WsfSphericalLethality Maximum damage must be greater than Minimum.";
      result = false;
   }
   if (mThreshold > mMaxDamage)
   {
      ut::log::error() << "WsfSphericalLethality damage Threshold must be less than Max Damage.";
      result = false;
   }

   return result;
}

// virtual
bool WsfSphericalLethality::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());

   if (command == "threshold_damage")
   {
      myCommand = true;
      aInput.ReadValue(mThreshold);
      aInput.ValueInClosedRange(mThreshold, 0.0, 1.0);
   }
   else if (command == "minimum_damage")
   {
      myCommand = true;
      aInput.ReadValue(mMinDamage);
      aInput.ValueInClosedRange(mMinDamage, 0.0, 1.0);
   }
   else if (command == "maximum_damage")
   {
      myCommand = true;
      aInput.ReadValue(mMaxDamage);
      aInput.ValueInClosedRange(mMaxDamage, 0.0, 1.0);
   }
   else if (command == "exponent")
   {
      myCommand = true;
      aInput.ReadValue(mExponent);
      aInput.ValueGreater(mExponent, 0.0);
   }
   else if (command == "minimum_radius")
   {
      myCommand = true;
      aInput.ReadValueOfType(mMinRadius, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMinRadius, 0.0);
   }
   else
   {
      myCommand = WsfExplicitWeaponEffects::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfSphericalLethality::ApplyEffectTo(double aSimTime, WsfPlatform* aOtherPlatformPtr)
{
   // Note:  Since this just passed a vulnerability radius test in the parent class,
   // just use the value here for calculation of Pk, if possible, to save time...

   double missDist = sqrt(LastTestRadiusSquared());

   double damageIncrement = 0.0;

   // It has already been determined (prior to this call)
   // that the detonation was within damaging blast radius...
   if (missDist < mMinRadius)
   {
      // Blast was closer than mMinRadius,
      // so inflict up to mMaxDamage, if possible.
      damageIncrement = mMaxDamage;
   }
   else
   {
      // Blast was in the "transfer function" range,
      // so intermediate level damage has occurred...

      double kR       = (missDist - mMinRadius) / (MaximumEffectiveRadius() - mMinRadius);
      damageIncrement = mMaxDamage - pow(kR, 1.0 / mExponent) * (mMaxDamage - mMinDamage);
   }

   // In order to further damage the target platform:
   // 1.  The platform must have already been damaged
   //       (threshhold previously met), or
   // 2.  DamageIncrement exceeds the mThreshold.

   bool canApplyDamage = (aOtherPlatformPtr->GetDamageFactor() != 0.0) || (damageIncrement > mThreshold);

   if (!canApplyDamage)
   {
      damageIncrement = 0.0;
   }

   ApplyEffectIncrement(aSimTime, aOtherPlatformPtr, damageIncrement);
}
