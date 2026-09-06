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

#ifndef WSFEXPLICITWEAPONEFFECTS_HPP
#define WSFEXPLICITWEAPONEFFECTS_HPP

#include "wsf_mil_export.h"

class WsfPlatform;
#include "WsfWeaponEffects.hpp"

//! A base class for explicit weapon effect determinations.
//! Produces damage to platforms when a weapon detonates in proximity.

class WSF_MIL_EXPORT WsfExplicitWeaponEffects : public WsfWeaponEffects
{
public:
   WsfExplicitWeaponEffects(WsfScenario& aScenario);
   ~WsfExplicitWeaponEffects() override = default;

   WsfWeaponEffects* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   //! Access the miss radius squared beyond which the weapon has no effect.
   virtual double MaximumEffectiveRadiusSquared() const;

   //! Access the miss radius beyond which the weapon has no effect.
   virtual double MaximumEffectiveRadius() const;

   //! Set the radius beyond which the weapon has no effect.
   void SetMaxiumEffectiveRadius(double aRadius) { mMaxRadius = aRadius; }

protected:
   WsfExplicitWeaponEffects(const WsfExplicitWeaponEffects& aSrc) = default;

   void CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr) override;

   void CalcPkParameters(WsfPlatform* aTargetPtr,
                         double&      aAzimuth,
                         double&      aElevation,
                         double&      aMissileSpeed,
                         double&      aTargetSpeed) override;

   // Overrides base class implementation, and adds a second range radius test.
   bool IsVulnerable(WsfPlatform* aTargetPtr) override;

   //! Get the miss radius squared for the most recent test.
   //! (Used in derived classes to prevent having to do the offset calculation a second time.)
   double LastTestRadiusSquared() const { return mRadiusSquared; }

   void CalcInterceptGeometry(WsfPlatform* aTargetPtr, double& aRangeRate, double& aApproachAngle);

private:
   //! Disable use.
   WsfExplicitWeaponEffects& operator=(const WsfExplicitWeaponEffects& aRhs) = delete;

   void CalcInterceptGeometry(const double aInterceptorVel[],
                              const double aTargetVel[],
                              double&      aRangeRate,
                              double&      aApproachAngle);

   // This is a time saver... derived weapon effect functions
   // will not be evaluated when target and weapon are more than this radius apart.
   double mMaxRadius;
   double mRadiusSquared;
};

#endif
