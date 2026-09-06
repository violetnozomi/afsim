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

#ifndef WSFCARLTONLETHALITY_HPP
#define WSFCARLTONLETHALITY_HPP

#include "wsf_mil_export.h"

#include <map>

#include "WsfExplicitWeaponEffects.hpp"

//! A specialization of the WsfLethality class which implements the Carlton Damage Equation.
//!
//!   The Carlton damage equation is:
//!      - PD  = D0 * exp[ -(D0*Pi/LA) * (x^2*(RDR) + y^2*(1/RDR)) ]
//!        - Where:
//!          - PD  = Probability of damage
//!          - D0  = Input parameter for the [Munition, Target] pairing. (default = 0.5)
//!          - LA  = Lethal Area, input for each [munition, target, kill criteria] combo.
//!                  (Munition type and kill criteria is known at instantiation, so then
//!                   Lethal Area is a function of target type.)
//!          - x   = range direction miss distance
//!          - y   = deflection (lateral) miss distance
//!          - RDR = Deflection to Range lethality ratio = 1 / (1 - 0.8 cos (AOF))
//!          - AOF = Angle of fall of projectile (measured from horizontal).
//!
//!   @note
//!   This damage equation is primarily used to assess artillery effectiveness
//!   against fixed ground targets.  Miss Range and Miss Deflection are assumed
//!   to occur in a more-or-less horizontal plane containing the target, and
//!   the weapon's velocity vector is used to orient the values in a near-far,
//!   right-left direction.  Any Z offsets are ignored.  Orientation of either
//!   the weapon or the target do not matter.
//!
//!   If there is no lethal area defined for the target type,
//!   the munition is assumed to have NO effect on the target.

class WSF_MIL_EXPORT WsfCarltonLethality : public WsfExplicitWeaponEffects
{
public:
   WsfCarltonLethality(WsfScenario& aScenario);
   ~WsfCarltonLethality() override = default;
   WsfWeaponEffects* Clone() const override;
   bool              ProcessInput(UtInput& aInput) override;

   //! Set D0 = Input the parameter for the [Munition, Target] pair. (default = 0.5)
   void SetD0(double aD0) { mD0 = aD0; }

   //! Get D0 = Access the parameter used for the [Munition, Target] pair.
   double GetD0() const { return mD0; }

   bool AddPlatformType(WsfStringId aPlatformTypeID, double aLethalArea);

protected:
   WsfCarltonLethality(const WsfCarltonLethality& aSrc) = default;

   // Prevent use of operator= by declaring, but not defining.
   WsfCarltonLethality& operator=(const WsfCarltonLethality& aRhs) = delete;

   void CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr) override;

private:
   // Map of possible platform types, and the Lethal Area for each.
   std::map<WsfStringId, double> mLAMap;

   // D0 (default = 0.5).
   double mD0;
};

#endif
