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

#ifndef WSFSPHERICALLETHALITY_HPP
#define WSFSPHERICALLETHALITY_HPP

#include "wsf_mil_export.h"

#include "WsfExplicitWeaponEffects.hpp"

//! A WsfLethality class that causes incremental damage inversely proportional to miss distance.
//! A specialization WsfLethality that causes incremental damage to other platforms, varying
//! inversely proportional to miss distance.  No random draws are used.  The amount of damage
//! imposed is constrained between minimum and maximum levels as specified by input files.
//! The implementation does not account for hardness of the target platform.

class WSF_MIL_EXPORT WsfSphericalLethality : public WsfExplicitWeaponEffects
{
public:
   WsfSphericalLethality(WsfScenario& aScenario);
   ~WsfSphericalLethality() override = default;
   WsfWeaponEffects* Clone() const override;
   bool              PreInitialize() override;
   bool              ProcessInput(UtInput& aInput) override;
   void              SetMinRadius(double aRadius) { mMinRadius = aRadius; }

protected:
   // Copy constructor (for Clone())
   WsfSphericalLethality(const WsfSphericalLethality& aSrc) = default;

   // Prevent use of operator= by declaring, but not defining.
   WsfSphericalLethality& operator=(const WsfSphericalLethality& aRhs) = delete;

   void ApplyEffectTo(double aSimTime, WsfPlatform* aOtherPlatformPtr) override;

private:
   // The SMALLEST radius at which the MINIMUM amount of damage
   // is sustained is kept in the base class.
   double mMinRadius;

   // Indicates a minimum damage that must occur before damage begins accumulating
   // to the target.  After the threshold damage is achieved, the value is ignored.
   double mThreshold;

   // The smallest amount of damage that can be applied to the target.
   double mMinDamage;

   // The largest amount of damage ever applied to a target.
   double mMaxDamage;

   // Models a non-linear damage effect with miss distance:
   // Default of mExponent = 1 ==> linear
   double mExponent;
};

#endif
