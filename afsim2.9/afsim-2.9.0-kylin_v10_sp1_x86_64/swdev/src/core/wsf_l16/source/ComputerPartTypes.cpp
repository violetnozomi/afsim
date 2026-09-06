// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "ComputerPartTypes.hpp"

#include "NetworkEnabledWeaponPart.hpp"
#include "PPLI_Part.hpp"
#include "ScriptedPart.hpp"
#include "SurveillancePart.hpp"
#include "SystemStatusPart.hpp"
#include "UtMemory.hpp"
#include "WeaponsCoordinationPart.hpp"

namespace WsfL16
{

ComputerPartTypes::ComputerPartTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<ComputerPart>(aScenario, "computer_part")
{
   Add("PPLI", ut::make_unique<PPLI_Part>(aScenario));
   Add("SYSTEM_STATUS", ut::make_unique<SystemStatusPart>(aScenario));
   Add("SURVEILLANCE", ut::make_unique<SurveillancePart>(aScenario));
   Add("SCRIPTED", ut::make_unique<ScriptedPart>(aScenario));
   Add("WEAPONS_COORDINATION", ut::make_unique<WeaponsCoordinationPart>(aScenario));
   Add("NETWORK_ENABLED_WEAPON", ut::make_unique<NetworkEnabledWeaponPart>(aScenario));
}

} // namespace WsfL16
