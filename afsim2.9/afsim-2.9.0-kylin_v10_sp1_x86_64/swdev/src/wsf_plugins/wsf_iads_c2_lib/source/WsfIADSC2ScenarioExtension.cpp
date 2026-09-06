// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfIADSC2ScenarioExtension.hpp"

#include <memory>

#include "WsfProcessorTypes.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptContext.hpp"

#include "WsfAssetManager.hpp"
#include "WsfBattleManager.hpp"
#include "WsfBMDisseminateC2.hpp"
#include "WsfBMSensorsManager.hpp"
#include "WsfBMWeaponsManager.hpp"
#include "WsfSensorsManagerFov.hpp"
#include "WsfSimpleSensorsManager.hpp"
#include "WsfUnclassAssetManager.hpp"
#include "WsfUnclassBattleManager.hpp"
#include "WsfUnclassDisseminateC2.hpp"
#include "WsfWeaponsManagerAI.hpp"
#include "WsfWeaponsManagerSAM.hpp"

#include "WsfScriptBattleManagerClass.hpp"
#include "WsfScriptDisseminateC2Class.hpp"
#include "WsfScriptSensorsManagerClass.hpp"
#include "WsfScriptSensorsManagerFovClass.hpp"
#include "WsfScriptSimpleSensorsManagerClass.hpp"
#include "WsfScriptUnclassAssetManagerClass.hpp"
#include "WsfScriptUnclassBattleManagerClass.hpp"
#include "WsfScriptUnclassDisseminateC2Class.hpp"
#include "WsfScriptWeaponsManagerAIClass.hpp"
#include "WsfScriptWeaponsManagerClass.hpp"
#include "WsfScriptWeaponsManagerSAMClass.hpp"

const std::string IADSC2ScenarioExtension::cNAME = "wsf_iads_c2";

// virtual
void IADSC2ScenarioExtension::AddedToScenario()
{
   WsfScenario& scenario = GetScenario();

   mGlobalLogger = std::make_shared<il::logger::GlobalLogger>();

   // Must register the base type first
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptAssetManagerClass::GetBaseClassName(), WsfAssetManager::CreateUnique(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptBattleManagerClass::GetBaseClassName(), WsfBattleManager::CreateUnique(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptDisseminateC2Class::GetBaseClassName(), WsfBMDisseminateC2::CreateUnique(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptSensorsManagerClass::GetBaseClassName(), WsfBMSensorsManager::CreateUnique(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptWeaponsManagerClass::GetBaseClassName(), WsfBMWeaponsManager::CreateUnique(scenario));

   // Now register the derived types
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptUnclassAssetManagerClass::GetBaseClassName(), WsfUnclassifiedAssetManager::CreateUnique(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptUnclassBattleManagerClass::GetBaseClassName(), WsfUnclassifiedBattleManager::CreateUnique(scenario));

   auto scenario_context = scenario.GetScriptContext();
   scenario_context->DeclareVariable("WsfBMUtils", "BMUTILS");
   if (!scenario_context->GetContext().FindGlobalVar("BMUTILS"))
   {
      auto script_class = scenario_context->GetClass("WsfBMUtils");
      scenario_context->GetContext().RegisterVariable("BMUTILS", "WsfBMUtils")->Set(ut::script::Data(script_class, script_class, UtScriptRef::cDONT_MANAGE));
   }

   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptUnclassDisseminateC2Class::GetBaseClassName(), WsfUnclassifiedDisseminateC2::CreateUnique(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptSimpleSensorsManagerClass::GetBaseClassName(), WsfSimpleSensorsManager::CreateUnique(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptWeaponsManagerSAMClass::GetBaseClassName(), WsfWeaponsManagerSAM::CreateUnique(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptWeaponsManagerAIClass::GetBaseClassName(), WsfWeaponsManagerAI::CreateUnique(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType(WsfScriptSensorsManagerFovClass::GetBaseClassName(), WsfSensorsManagerFov::CreateUnique(scenario));
}

// virtual
void IADSC2ScenarioExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension("wsf_iads_c2", ut::make_unique<IADSC2SimulationExtension>());
}
