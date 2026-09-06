// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERADDDETONATOREFFECT_HPP
#define WSFCYBERADDDETONATOREFFECT_HPP

#include "wsf_cyber_export.h"

#include <iostream>
#include <unordered_set>

#include "WsfCyberEffect.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfTrackProcessor.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEffects.hpp"

namespace wsf
{
namespace cyber
{
//! This cyber effect allows an attacking platform to remotely detonate specified
//! WSF_EXPLICIT_WEAPONs on the victim platform. All weapons specified will continue
//! to detonate until either all the specified weapons are detonated, or the victim
//! is destroyed.
class WSF_CYBER_EXPORT DetonateEffect : public Effect
{
public:
   DetonateEffect() = default;

   DetonateEffect* Clone() const override;
   bool            ProcessInput(UtInput& aInput) override;

   void Attack(double aSimTime, Engagement& aEngagement) override;
   void Restore(double aSimTime, Engagement& aEngagement) override;

   // TODO: follow on work could make this an input.
   InputRequirement RequiredInput() const override { return InputRequirement::cNOT_ALLOWED; }
   size_t           RequiredInputCount() const override { return 0U; }

   //! Casts the weapon to an explicit weapon and checks to see if it is on the list of weapons to detonate.
   WsfExplicitWeapon* CheckForValidWeapon(WsfWeapon* aWpnPtr);

protected:
   DetonateEffect(const DetonateEffect& aSrc) = default;

   std::unordered_set<UtStringId> mWeaponNames{};
   std::unordered_set<UtStringId> mWeaponTypes{};
   size_t                         mTotalDetonates = 0;
};

} // namespace cyber
} // namespace wsf

#endif
