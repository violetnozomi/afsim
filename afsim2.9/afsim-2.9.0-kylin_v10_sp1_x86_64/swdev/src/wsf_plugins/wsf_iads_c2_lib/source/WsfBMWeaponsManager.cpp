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
*   File: WsfBMWeaponsManager.cpp
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
*   Abstract: This is a core class implementation for a HELIOS-like weapons
*    manager base class.
*
*******************************************************************************/

#include <WsfBMWeaponsManager.hpp>

#include <UtInputBlock.hpp>
#include <UtMemory.hpp>
#include <WsfApplication.hpp>

#include <WsfBMPluginUtilities.hpp>
#include <WsfScriptWeaponsManagerClass.hpp>
#include <WsfBMUtils.hpp>

#include <iadsLib/util.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"

WsfBMWeaponsManager::WsfBMWeaponsManager(WsfScenario&       aScenario,
                                         const std::string& aPartClass,
                                         const std::string& aPartVarName,
                                         const std::shared_ptr<il::WeaponsManagerInterface>&  wm_component)
   : WsfScriptProcessor(aScenario, aPartClass, aPartVarName)
   , WsfC2ComponentContainer(wm_component)
   , WsfScriptOverridableProcessor(false) // by default, asset manager drives everything else
{
   SetType(WsfStringId(WsfScriptWeaponsManagerClass::BASE_CLASS_NAME));
}

WsfBMWeaponsManager::WsfBMWeaponsManager(const WsfBMWeaponsManager& aSrc)
   : WsfScriptProcessor(aSrc)
   , WsfC2ComponentContainer(aSrc)
   , WsfScriptOverridableProcessor(aSrc)
   , m_default_weapons_manager_impl(aSrc.m_default_weapons_manager_impl)
{
   m_wm_component.reset(aSrc.m_wm_component->Clone());
}

WsfBMWeaponsManager::~WsfBMWeaponsManager()
{
}

WsfProcessor* WsfBMWeaponsManager::Clone() const
{
   return new WsfBMWeaponsManager(*this);
}

// Plugin Interface
extern "C" WSF_IADS_C2_LIB_EXPORT void WsfBMWeaponsManager::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   // Register the type if it isn't already
   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptWeaponsManagerClass::SCRIPT_CLASS_NAME))
   {
      RegisterBaseTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptWeaponsManagerClass>(WsfScriptWeaponsManagerClass::SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfBMWeaponsManager::Create(WsfScenario& aScenario)
{
   std::shared_ptr<il::WeaponsManagerInterface> wm = std::shared_ptr<il::WeaponsManagerInterface>(new il::WeaponsManagerInterface);

   return new WsfBMWeaponsManager(aScenario, WsfScriptWeaponsManagerClass::SCRIPT_CLASS_NAME, "PROCESSOR", wm);
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfBMWeaponsManager> WsfBMWeaponsManager::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfBMWeaponsManager>(aScenario);
}

// End Plugin Interface

const char* WsfBMWeaponsManager::GetScriptClassName() const
{
   return WsfScriptWeaponsManagerClass::SCRIPT_CLASS_NAME;
}

const char* WsfBMWeaponsManager::GetBaseClassName()
{
   return WsfScriptWeaponsManagerClass::BASE_CLASS_NAME;
}

bool WsfBMWeaponsManager::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   // Attempt to parse scripting for common bm elements
   if (ProcessWeaponsManagerInput(aInput) ||
       m_default_weapons_manager_impl.ProcessInput(aInput) ||
       WsfScriptOverridableProcessor::ProcessInput(aInput) ||
       WsfScriptProcessor::ProcessInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

bool WsfBMWeaponsManager::Initialize(double aSimTime)
{
   bool success = true;

   if (!OverrideInternalInit())
   {
      m_default_weapons_manager_impl.Initialize(this);
   }

   // base initialization - always happen
   success &= WsfScriptProcessor::Initialize(aSimTime);

   return success;
}

bool WsfBMWeaponsManager::Initialize2(double aSimTime)
{
   bool success = true;

   if (!OverrideInternalInit2())
   {
      if (!SetComponent(WsfBMUtils::FindAttachedAssetManagerRef(this->GetPlatform())))
      {
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "A weapons manager processor requires a processor of type WSF_ASSET_MANAGER to be employed on the same platform";
         return success &= false;
      }

      m_default_weapons_manager_impl.Initialize2(this);
   }

   // base initialization - always happen
   // run the script processor initialize2 after we've done processor hooking to allow the script to override it.
   success &= WsfScriptProcessor::Initialize2(aSimTime);

   return success;
}

void WsfBMWeaponsManager::Update(double aSimTime)
{
   if (!OverrideInternalOnUpdate() && AllowAutonomousUpdates())
   {
      Run(aSimTime);
   }
   else
   {
      // base class updates always occur
      WsfScriptProcessor::Update(aSimTime);
   }
}

void WsfBMWeaponsManager::Run(const double aSimTime)
{
   m_default_weapons_manager_impl.on_update(aSimTime, GetPlatform(), this);
   // base class updates always occur
   WsfScriptProcessor::Update(aSimTime);
}

bool WsfBMWeaponsManager::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool success = true;

   if (!OverrideInternalOnMessage())
   {
      m_default_weapons_manager_impl.on_message(aSimTime, aMessage, this);
   }

   // base class processing always occur
   success &= WsfScriptProcessor::ProcessMessage(aSimTime, aMessage);

   return success;
}

bool WsfBMWeaponsManager::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool success = true;

   if (!OverrideInternalReceiveMessage())
   {
      // nothing to do
   }

   success &= WsfScriptProcessor::ReceiveMessage(aSimTime, aMessage);

   return success;
}

bool WsfBMWeaponsManager::ProcessWeaponsManagerInput(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "self_defense")
   {
      UtInputBlock block(aInput);
      processed = ProcessSelfDefenseBlock(block);
   }
   else if (command == "engagement_settings")
   {
      UtInputBlock block(aInput);
      processed = ProcessEngagementSettingsBlock(block);
   }
   else if (command == "delays")
   {
      UtInputBlock block(aInput);
      processed = ProcessDelaysBlock(block);
   }
   else if (command == "wez")
   {
      UtInputBlock block(aInput);
      processed = ProcessWEZBlock(block);
   }
   else if (command == "haveco_reporting_strategy")
   {
      std::string strategy;
      aInput.ReadValue(strategy);
      try
      {
         m_wm_component->setHavecoReportingStyle(il::WeaponsManagerInterface::HavecoReportStyleFromString(strategy));
      }
      catch (std::runtime_error& error)
      {
         processed = false;
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << GetName() << ": WsfBMWeaponsManager::ProcessWeaponsManagerInput(): " << error.what() << std::endl;
      }
   }
   else
   {
      processed = false;
   }

   return processed;
}

bool WsfBMWeaponsManager::ProcessSelfDefenseBlock(UtInputBlock& block)
{
   bool processed = true;

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      auto& aInput = block.GetInput();
      if (command == "enable")
      {
         m_wm_component->setSelfDefense(true);
      }
      else if (command == "disable")
      {
         m_wm_component->setSelfDefense(false);
      }
      else if (command == "range")
      {
         double range_m = 0;
         aInput.ReadValueOfType(range_m, UtInput::cLENGTH);
         m_wm_component->setSelfDefenseRange(range_m);
      }
      else if (command == "shot_doctrine")
      {
         std::string value;
         il::eShotDoctrine doctrine;
         aInput.ReadValue(value);

         if (il::ShotDoctrineStringToEnum(value, doctrine, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
         {
            m_wm_component->setSelfDefenseShotDoctrine(doctrine);
         }
         else
         {
            processed = false;
         }
      }
      else if (command == "shot_expiry")
      {
         double value_s;
         aInput.ReadValueOfType(value_s, UtInput::cTIME);
         m_wm_component->setSelfDefenseShotExpiry(value_s);
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}

bool WsfBMWeaponsManager::ProcessEngagementSettingsBlock(UtInputBlock& block)
{
   bool processed = true;

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      auto& aInput = block.GetInput();
      if (command == "ew_targets")
      {
         m_wm_component->setEngageEWTargets(aInput.ReadBool());
      }
      else if (command == "tar_targets")
      {
         m_wm_component->setEngageTARTargets(aInput.ReadBool());
      }
      else if (command == "ttr_targets")
      {
         m_wm_component->setEngageTTRTargets(aInput.ReadBool());
      }
      else if (command == "engage_local_ttr_targets_only")
      {
         m_wm_component->setEngageLocalTTRTargetsOnly(aInput.ReadBool());
      }
      else if (command == "track_quality")
      {
         double quality = 0.0;
         aInput.ReadValue(quality);
         if (quality >= 0.0 && quality <= 1.0)
         {
            m_wm_component->setEngageTrackQuality(quality);
         }
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}

bool WsfBMWeaponsManager::ProcessDelaysBlock(UtInputBlock& block)
{
   bool processed = true;

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      auto& aInput = block.GetInput();
      // TTWA
      // time_to_launch is on the weapon as GetTimeBetweenShots()
      // salvo_shot is on the weapon as GetTimeToFire()
      if (command == "time_between_engagements")
      {
         double time_between_engagements_s = 0;
         aInput.ReadValueOfType(time_between_engagements_s, UtInput::cTIME);
         m_wm_component->setTimeBetweenEngagementsDelay(time_between_engagements_s);
      }
      else if (command == "expected_sensor_acquisition")
      {
         double sensor_acquisition_s = 0;
         aInput.ReadValueOfType(sensor_acquisition_s, UtInput::cTIME);
         m_wm_component->setExpectedExecutionDelay(sensor_acquisition_s);
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}

bool WsfBMWeaponsManager::ProcessWEZBlock(UtInputBlock& block)
{
   bool processed = true;

   HCL_INFO_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Note: WEZ calculator scripting here is only intended to be input into intercept calculators. "
                     "Unless the weapon intercept calculations make use of this data, it will not be used.";

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      auto& aInput = block.GetInput();
      if (command == "library")
      {
         std::string library_name;
         aInput.ReadValue(library_name);
         if (library_name != "none")
         {
            m_wm_component->setWEZLibraryName(library_name);
            m_wm_component->setUseWEZCalculator(true);
         }
      }
      else if (command == "tactical_range")
      {
         std::string rng_str;
         aInput.ReadValue(rng_str);
         try
         {
            m_wm_component->setWEZTacticalRange(il::WeaponsManagerInterface::WEZTacticalRangeFromString(rng_str));
         }
         catch (std::runtime_error& error)
         {
            processed = false;
            HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << GetName() << ": WsfBMWeaponsManager::ProcessWEZBlock(): " << error.what() << std::endl;
         }
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}
