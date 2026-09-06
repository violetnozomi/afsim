// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfUnclassBattleManager.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

namespace il
{
   class unclassifiedAssetManager;
   class unclassifiedBattleManager;
}

#include <string>

#include <UtTime.hpp>
#include <UtInputBlock.hpp>
#include <UtMemory.hpp>

#include "WsfUnclassBattleManager.hpp"

#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/BattleManagerIface.hpp>
#include <iadsLib/unclassifiedBattleManager.hpp>
#include <iadsLib/unclassifiedAssetManager.hpp>
#include <iadsLib/util.hpp>

#include <script/WsfScriptManager.hpp>
#include "WsfScriptUnclassBattleManagerClass.hpp"
#include <WsfBMPluginUtilities.hpp>
#include <WsfBMWeaponTable.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

WsfProcessor* UninitializedWsfUnclassifiedBattleManager(WsfScenario& aScenario, const std::string& calling_context)
{
   std::shared_ptr<il::unclassifiedBattleManager> bm(new il::unclassifiedBattleManager(IADSC2ScenarioExtension::GetGlobalLogger(aScenario)));

#ifdef IC2DEBUG
   HCL_INFO_LOGGER() << "WsfUnclassifiedBattleManager::Create(): bm = " << bm;
#endif

   return new WsfUnclassifiedBattleManager(aScenario, bm);
}

WsfUnclassifiedBattleManager::WsfUnclassifiedBattleManager(WsfScenario& aScenario)
   : WsfBattleManager(aScenario, UNCLASS_BM_SCRIPT_CLASS_NAME, "PROCESSOR", std::make_shared<il::unclassifiedBattleManager>(IADSC2ScenarioExtension::GetGlobalLogger(aScenario)))
{
}

WsfUnclassifiedBattleManager::WsfUnclassifiedBattleManager(WsfScenario&       aScenario,
                                                           const std::shared_ptr<il::BattleManagerInterface>& bm)
   : WsfBattleManager(aScenario, UNCLASS_BM_SCRIPT_CLASS_NAME, "PROCESSOR", bm)
{
}

// See the notes in the header file about the copy constructor and it's role with
// how AFSIM initialization works
WsfUnclassifiedBattleManager::WsfUnclassifiedBattleManager(const WsfUnclassifiedBattleManager& from)
   : WsfBattleManager(from)
{
#ifdef IC2DEBUG
   HCL_TRACE_LOGGER() << "WsfUnclassifiedBattleManager::(copy constructor): am = " << m_am_component << " bm = " << m_bm_component;
#endif
}

WsfProcessor* WsfUnclassifiedBattleManager::Clone() const
{
   // for the clone, we need to use the copy constructor since the base WsfObjects are already initialized
   return new WsfUnclassifiedBattleManager(*this);
}

// Plugin Interface
extern "C" WSF_IADS_C2_LIB_EXPORT void WsfUnclassifiedBattleManager::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   // Register the type if it isn't already
   if (!IsClassTypeRegistered(aApplicationPtr, UNCLASS_BM_SCRIPT_CLASS_NAME))
   {
      // base class registration
      WsfBattleManager::RegisterScriptTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptUnclassBattleManagerClass>(UNCLASS_BM_SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfUnclassifiedBattleManager::Create(WsfScenario& aScenario)
{
   return UninitializedWsfUnclassifiedBattleManager(aScenario, "WsfUnclassifiedBattleManager::Create()");
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfUnclassifiedBattleManager> WsfUnclassifiedBattleManager::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfUnclassifiedBattleManager>(aScenario);
}

bool WsfUnclassifiedBattleManager::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   if (WsfBattleManager::ProcessInput(aInput) ||
       ProcessCoreUnclassBMInput(aInput) ||
       ProcessPrintSettings(aInput) ||
       ProcessWeaponTable(aInput) ||
       ProcessScoringfactors(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

const char* WsfUnclassifiedBattleManager::GetScriptClassName() const
{
   return UNCLASS_BM_SCRIPT_CLASS_NAME;
}

bool WsfUnclassifiedBattleManager::ProcessCoreUnclassBMInput(UtInput& aInput)
{
   bool processed = true;
   auto bm_ptr = dynamic_cast<il::unclassifiedBattleManager*>(m_bm_component.get());

   std::string command = aInput.GetCommand();

   if (command == "stale_asset_time")
   {
      double value = 0;
      aInput.ReadValueOfType(value, UtInput::cTIME);

      bm_ptr->setStaleAssetTime(value);
   }
   else if (command == "shot_doctrine")
   {
      std::string value;
      il::eShotDoctrine doctrine;
      aInput.ReadValue(value);

      if (il::ShotDoctrineStringToEnum(value, doctrine, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
      {
         bm_ptr->setShotDoctrine(doctrine);
      }
      else
      {
         processed = false;
      }
   }
   else if (command == "max_firing_time")
   {
      double value = 0;
      aInput.ReadValueOfType(value, UtInput::cTIME);

      bm_ptr->setMaxFiringTime(value);
   }
   else
   {
      processed = false;
   }

   return processed;
}

bool WsfUnclassifiedBattleManager::ProcessPrintSettings(UtInput& aInput)
{
   bool processed = true;
   auto bm_ptr = dynamic_cast<il::unclassifiedBattleManager*>(m_bm_component.get());

   std::string command = aInput.GetCommand();

   if (command == "print_settings")
   {
      UtInputBlock block(aInput, "end_" + command);

      while (block.ReadCommand(command) && processed)
      {
         if (command == "valid_units")
         {
            bool value = true;
            block.GetInput().ReadValue(value);
            bm_ptr->setPrintValidUnits(value);
         }
         else if (command == "valid_unit_details")
         {
            bool value = true;
            block.GetInput().ReadValue(value);
            bm_ptr->setPrintValidUnitDetails(value);
         }
         else if (command == "invalid_unit_details")
         {
            bool value = true;
            block.GetInput().ReadValue(value);
            bm_ptr->setPrintInvalidUnitDetails(value);
         }
         else
         {
            processed = false;
         }
      }
   }
   else
   {
      processed = false;
   }

   return processed;
}

bool WsfUnclassifiedBattleManager::ProcessWeaponTable(UtInput& aInput)
{
   bool my_command = false;
   auto bm_ptr = dynamic_cast<il::unclassifiedBattleManager*>(m_bm_component.get());

   WsfBMWeaponTable wt(&bm_ptr->getWeaponTable());
   if (wt.ProcessInput(aInput, true, true))
   {
      my_command = true;
   }

   return my_command;
}

bool WsfUnclassifiedBattleManager::ProcessScoringfactors(UtInput& aInput)
{
   bool processed = true;
   auto bm_ptr = dynamic_cast<il::unclassifiedBattleManager*>(m_bm_component.get());

   std::string command = aInput.GetCommand();

   if (command == "scoring_factors")
   {
      UtInputBlock block(aInput, "end_" + command);

      while (block.ReadCommand(command) && processed)
      {
         if (command == "weapon_type")
         {
            int value = 0;
            block.GetInput().ReadValue(value);
            bm_ptr->setWeaponTypeScoringWeight(value);
         }
         else if (command == "weapon_subtype")
         {
            int value = 0;
            block.GetInput().ReadValue(value);
            bm_ptr->setWeaponSubtypeScoringWeight(value);
         }
         else if (command == "intercept_range")
         {
            int value = 0;
            block.GetInput().ReadValue(value);
            bm_ptr->setInterceptRangeScoringWeight(value);
         }
         else if (command == "intercept_time")
         {
            int value = 0;
            block.GetInput().ReadValue(value);
            bm_ptr->setInterceptTimeScoringWeight(value);
         }
         else if (command == "pk")
         {
            int value = 0;
            block.GetInput().ReadValue(value);
            bm_ptr->setProbabilityKillScoringWeight(value);
         }
         else if (command == "workload")
         {
            int value = 0;
            block.GetInput().ReadValue(value);
            bm_ptr->setWorkloadScoringWeight(value);
         }
         else
         {
            processed = false;
         }
      }
   }
   else
   {
      processed = false;
   }

   return processed;
}
