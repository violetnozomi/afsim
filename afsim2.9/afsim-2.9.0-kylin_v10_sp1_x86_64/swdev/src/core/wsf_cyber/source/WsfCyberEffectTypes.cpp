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

#include "WsfCyberEffectTypes.hpp"

#include "UtMemory.hpp"
#include "WsfCyberScenarioExtension.hpp"

// include files for pre-defined cyber effect types
#include "effects/WsfCyberDetonateEffect.hpp"
#include "effects/WsfCyberMitmEffect.hpp"
#include "effects/WsfCyberScriptEffect.hpp"
#include "effects/WsfCyberScriptEffectEnhanced.hpp"
#include "effects/WsfCyberToggleEffect.hpp"
#include "effects/WsfCyberTrackManagerEffect.hpp"
#include "effects/WsfCyberTrackProcessorEffect.hpp"
#include "effects/WsfCyberWeaponsEffect.hpp"

namespace wsf
{
namespace cyber
{

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
EffectTypes& EffectTypes::Get(WsfScenario& aScenario)
{
   return ScenarioExtension::Get(aScenario).GetEffectTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const EffectTypes& EffectTypes::Get(const WsfScenario& aScenario)
{
   return ScenarioExtension::Get(aScenario).GetEffectTypes();
}

// =================================================================================================
EffectTypes::EffectTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<Effect>(aScenario, "cyber_effect")
{
   // Register pre-defined cyber_effect types.
   AddCoreType("WSF_CYBER_SCRIPT_EFFECT", ut::make_unique<ScriptEffect>(aScenario));
   AddCoreType("WSF_CYBER_DETONATE_EFFECT", ut::make_unique<DetonateEffect>());
   AddCoreType("WSF_CYBER_SCRIPT_EFFECT_ENHANCED", ut::make_unique<ScriptEffectEnhanced>(aScenario));
   AddCoreType("WSF_CYBER_TOGGLE_SENSORS_EFFECT", ut::make_unique<ToggleEffect<WsfSensor>>());
   AddCoreType("WSF_CYBER_TOGGLE_WEAPONS_EFFECT", ut::make_unique<ToggleEffect<WsfWeapon>>());
   AddCoreType("WSF_CYBER_TOGGLE_COMMS_EFFECT", ut::make_unique<ToggleEffect<wsf::comm::Comm>>());
   AddCoreType("WSF_CYBER_TOGGLE_PROCESSORS_EFFECT", ut::make_unique<ToggleEffect<WsfProcessor>>());
   AddCoreType("WSF_CYBER_MAN_IN_THE_MIDDLE_EFFECT", ut::make_unique<MitmEffect>(aScenario));
   AddCoreType("WSF_CYBER_WEAPONS_RETARGET_EFFECT", ut::make_unique<WeaponsEffect>(WeaponsEffect::cRETARGET));
   AddCoreType("WSF_CYBER_WEAPONS_UNTARGETED_EFFECT", ut::make_unique<WeaponsEffect>(WeaponsEffect::cUNTARGETED));
   AddCoreType("WSF_CYBER_TRACK_MANAGER_EFFECT", ut::make_unique<TrackManagerEffect>(aScenario));
   AddCoreType("WSF_CYBER_TRACK_PROCESSOR_EFFECT", ut::make_unique<TrackProcessorEffect>(aScenario));
}

} // namespace cyber
} // namespace wsf
