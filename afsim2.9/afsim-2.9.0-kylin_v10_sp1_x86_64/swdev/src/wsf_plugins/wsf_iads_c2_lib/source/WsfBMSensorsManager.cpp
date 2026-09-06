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
*   File: WsfBMSensorsManager.cpp
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
*   Abstract: This is a core class implementation for a HELIOS-like sensors
*    manager base class.
*
*******************************************************************************/

#include <WsfBMSensorsManager.hpp>

#include <UtInputBlock.hpp>
#include <UtMemory.hpp>

#include <WsfBMPluginUtilities.hpp>
#include <WsfScriptSensorsManagerClass.hpp>
#include <WsfBMUtils.hpp>
#include <WsfBMSensorInfoRecord.hpp>

#include <iadsLib/util.hpp>
#include <WsfBMMOELogger.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

WsfBMSensorsManager::WsfBMSensorsManager(WsfScenario&       aScenario,
                                         const std::string& aPartClass,
                                         const std::string& aPartVarName,
                                         const std::shared_ptr<il::SensorsManagerInterface>&  sm_component)
   : WsfScriptProcessor(aScenario, aPartClass, aPartVarName)
   , WsfC2ComponentContainer(sm_component)
   , WsfScriptOverridableProcessor(false) // by default, asset manager drives everything else
{
   SetType(WsfStringId(WsfScriptSensorsManagerClass::BASE_CLASS_NAME));
}

WsfBMSensorsManager::WsfBMSensorsManager(const WsfBMSensorsManager& aSrc)
   : WsfScriptProcessor(aSrc)
   , WsfC2ComponentContainer(aSrc)
   , WsfScriptOverridableProcessor(aSrc)
   , m_ttr_tracking_mode_name(aSrc.m_ttr_tracking_mode_name)
   , m_default_sensors_manager_impl(aSrc.m_default_sensors_manager_impl)
{
   m_sm_component.reset(aSrc.m_sm_component->Clone());
}

WsfBMSensorsManager::~WsfBMSensorsManager()
{
}

WsfProcessor* WsfBMSensorsManager::Clone() const
{
   return new WsfBMSensorsManager(*this);
}

// Plugin Interface
extern "C" WSF_IADS_C2_LIB_EXPORT void WsfBMSensorsManager::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   // Register the type if it isn't already
   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptSensorsManagerClass::SCRIPT_CLASS_NAME))
   {
      RegisterBaseTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptSensorsManagerClass>(WsfScriptSensorsManagerClass::SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfBMSensorsManager::Create(WsfScenario& aScenario)
{
   std::shared_ptr<il::SensorsManagerInterface> sm = std::shared_ptr<il::SensorsManagerInterface>(new il::SensorsManagerInterface);

   return new WsfBMSensorsManager(aScenario, WsfScriptSensorsManagerClass::SCRIPT_CLASS_NAME, "PROCESSOR", sm);
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfBMSensorsManager> WsfBMSensorsManager::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfBMSensorsManager>(aScenario);
}

// End Plugin Interface

const char* WsfBMSensorsManager::GetScriptClassName() const
{
   return WsfScriptSensorsManagerClass::SCRIPT_CLASS_NAME;
}

const char* WsfBMSensorsManager::GetBaseClassName()
{
   return WsfScriptSensorsManagerClass::BASE_CLASS_NAME;
}

bool WsfBMSensorsManager::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   // Attempt to parse scripting for common bm elements
   if (ProcessSensorsManagerInput(aInput) ||
       m_default_sensors_manager_impl.ProcessInput(aInput) ||
       WsfScriptOverridableProcessor::ProcessInput(aInput) ||
       WsfScriptProcessor::ProcessInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

bool WsfBMSensorsManager::Initialize(double aSimTime)
{
   bool success = true;

   if (!OverrideInternalInit())
   {
      m_default_sensors_manager_impl.Initialize(this);
   }

   // base initialization - always happen
   success &= WsfScriptProcessor::Initialize(aSimTime);

   return success;
}

bool WsfBMSensorsManager::Initialize2(double aSimTime)
{
   bool success = true;

   if (!OverrideInternalInit2())
   {
      if (!SetComponent(WsfBMUtils::FindAttachedAssetManagerRef(this->GetPlatform())))
      {
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "A sensors manager processor requires a processor of type WSF_ASSET_MANAGER to be employed on the same platform";
         return success &= false;
      }

      m_default_sensors_manager_impl.Initialize2(this);
   }

   // base initialization - always happen
   // run the script processor initialize2 after we've done processor hooking to allow the script to override it.
   success &= WsfScriptProcessor::Initialize2(aSimTime);

   return success;
}

void WsfBMSensorsManager::Update(double aSimTime)
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

void WsfBMSensorsManager::Run(const double aSimTime)
{
   m_default_sensors_manager_impl.on_update(aSimTime, GetPlatform(), this);
   // base class updates always occur
   WsfScriptProcessor::Update(aSimTime);
}

bool WsfBMSensorsManager::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool success = true;

   if (!OverrideInternalOnMessage())
   {
      m_default_sensors_manager_impl.on_message(aSimTime, aMessage, this);
   }

   // base class processing always occur
   success &= WsfScriptProcessor::ProcessMessage(aSimTime, aMessage);

   return success;
}

bool WsfBMSensorsManager::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool success = true;

   if (!OverrideInternalReceiveMessage())
   {
      // nothing to do
   }

   success &= WsfScriptProcessor::ReceiveMessage(aSimTime, aMessage);

   return success;
}

bool WsfBMSensorsManager::ProcessSensorsManagerInput(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "ttr_tracking_mode_name")
   {
      aInput.ReadValue(m_ttr_tracking_mode_name);
   }
   else if (command == "max_tar_acquisition_time")
   {
      double acq_time = 0;
      aInput.ReadValueOfType(acq_time, UtInput::cTIME);
      GetCoreSMRef()->SetMaxTARAcqTime(acq_time);
   }
   else if (command == "max_ttr_acquisition_time")
   {
      double acq_time = 0;
      aInput.ReadValueOfType(acq_time, UtInput::cTIME);
      GetCoreSMRef()->SetMaxTTRAcqTime(acq_time);
   }
   else if (command == "turn_off_tar_if_no_tracks")
   {
      bool turn_off_flag = false;
      aInput.ReadValue(turn_off_flag);
      GetCoreSMRef()->SetTurnOffTARNoTracks(turn_off_flag);
   }
   else if (command == "turn_off_ttr_if_no_tracks")
   {
      bool turn_off_flag = false;
      aInput.ReadValue(turn_off_flag);
      GetCoreSMRef()->SetTurnOffTTRNoTracks(turn_off_flag);
   }
   else // unknown block
   {
      processed = false;
   }

   return processed;
}

// Note: at the AFSIM level, the way the sensor manager works is that all cues are remotely executed by
// the sensor manager. To ensure perception of the sensor cues is accurate, we resynch our perception
// based on sensor truth of the cue counts before we perform the cueing main loop
void WsfBMSensorsManager::UpdateSensorCuePerception()
{
   auto sm_proc = GetCoreSMRef();

   auto resynch_sensor_cues = [&](std::shared_ptr<il::assetRecord> subordinate) -> void
   {
      for (std::size_t sidx = 1; sidx < subordinate->getNumSensors() + 1; ++sidx)
      {
         auto raw_sensor = subordinate->getNthSensor(sidx);
         if (raw_sensor->IsTTR())
         {
            auto afsim_sensor = dynamic_cast<WsfBMSensorInfoRecord*>(raw_sensor.get());
            if (afsim_sensor)
            {
               afsim_sensor->SetTrackingMode(GetTTRTrackingModeName());
            }
         }
         if (!raw_sensor->IsRWR())
         {
            raw_sensor->SetEstimatedActiveCues(raw_sensor->GetTruthActiveCues(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())));
            raw_sensor->SetMaxActiveCues(raw_sensor->GetMaxActiveCues());
         }
      }
   };

   auto am = sm_proc->GetAssetManager();
   am->getThisAsset()->foreachSubordinateBFS(am->getAssets(), resynch_sensor_cues);
}

void WsfBMSensorsManager::SetTTRTrackingModeName(const std::string& opt_mode_name)
{
   m_ttr_tracking_mode_name = opt_mode_name;
}

std::string WsfBMSensorsManager::GetTTRTrackingModeName() const
{
   return m_ttr_tracking_mode_name;
}
