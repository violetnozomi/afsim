// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfAirCombatTypeManager.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfAirCombatEventPipe.hpp"
#include "WsfAirCombatObserver.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfEventPipe.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfPlugin.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

WsfAirCombatTypeManager::WsfAirCombatTypeManager()
   : WsfScenarioExtension()
{
}

WsfAirCombatTypeManager::~WsfAirCombatTypeManager() {}

void WsfAirCombatTypeManager::AddedToScenario()
{
   WsfScenario& scenario(GetScenario());
   WsfProcessorTypes::Get(scenario).Add("WSF_SA_PROCESSOR", ut::make_unique<WsfSA_Processor>(scenario));

   WsfEventPipeExtension* eventPipePtr = WsfEventPipeExtension::Find(GetScenario());
   if (eventPipePtr != nullptr)
   {
      wsf::AirCombat::EventPipe::RegisterEvents(*eventPipePtr);
   }
}

void WsfAirCombatTypeManager::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension("air_combat_observer", ut::make_unique<WsfAirCombatObserver>());
   aSimulation.RegisterExtension("wsf_air_combat_eventpipe", ut::make_unique<wsf::AirCombat::EventPipeInterface>());
}
