// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERSCENARIOEXTENSION_HPP
#define WSFCYBERSCENARIOEXTENSION_HPP

#include "wsf_cyber_export.h"

#include <string>

#include "WsfCyberConstraintTypes.hpp"
#include "WsfScenarioExtension.hpp"
#include "effects/WsfCyberEffect.hpp"

namespace wsf
{
namespace cyber
{
class AttackTypes;
class EffectTypes;
class ProtectTypes;
class TriggerTypes;

//! The WSF scenario extension for cyber.
class WSF_CYBER_EXPORT ScenarioExtension : public WsfScenarioExtension
{
public:
   static ScenarioExtension& Get(const WsfScenario& aScenario);

   void AddedToScenario() override;
   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;

   AttackTypes&     GetAttackTypes() const { return *mAttackTypesPtr; }
   EffectTypes&     GetEffectTypes() const { return *mEffectTypesPtr; }
   ProtectTypes&    GetProtectTypes() const { return *mProtectTypesPtr; }
   ConstraintTypes& GetConstraintTypes() const { return *mConstraintTypesPtr; }
   TriggerTypes&    GetTriggerTypes() const { return *mTriggerTypesPtr; }

   //! @name Type Existence Queries
   //! These methods return if a particular type exists without required knowledge
   //! of the types themselves (no needed header)
   //@{
   bool GetAttackTypeExists(const std::string& aNamedAttack) const;
   bool GetEffectTypeExists(const std::string& aNamedEffect) const;
   bool GetProtectTypeExists(const std::string& aNamedProtection) const;
   bool GetTriggerTypeExists(const std::string& aNamedTrigger) const;
   bool GetConstraintTypesExists(const std::string& aNamedConstraint) const;
   //@}

   //! @name GetAttackTypeInputRequirement
   //! Given the name of the attack and the input requirement being tested,
   //! returns a boolean value if any of the effects associated with the
   //! attack have the specified input requirement.
   bool GetAttackTypeInputRequirement(const std::string& aNamedAttack, Effect::InputRequirement aRequirement) const;

   bool IsDrawEnabled() const { return mDrawEnabled; }

   AttackTypes*     mAttackTypesPtr;
   EffectTypes*     mEffectTypesPtr;
   ProtectTypes*    mProtectTypesPtr;
   ConstraintTypes* mConstraintTypesPtr;
   TriggerTypes*    mTriggerTypesPtr;
   bool             mDrawEnabled{false};
};

} // namespace cyber
} // namespace wsf

#endif
