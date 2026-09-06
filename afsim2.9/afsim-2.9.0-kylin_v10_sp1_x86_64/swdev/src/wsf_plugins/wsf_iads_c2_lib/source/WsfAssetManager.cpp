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
*   File: WsfBMDisseminateC2.cpp
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
*   Abstract: This is a core class implementation for a HELIOS-like C2 dissemination
*    model. It may be used or extended to meet individual IADS needs.
*
*******************************************************************************/

#include "WsfAssetManager.hpp"

#include <string>
#include <memory>

#include <UtInputBlock.hpp>
#include <UtMemory.hpp>

#include "WsfScenario.hpp"

#include <script/WsfScriptManager.hpp>
#include <WsfBMPluginUtilities.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"
#include <WsfScriptAssetManagerClass.hpp>
#include <WsfTrackManager.hpp>
#include <WsfBMUtils.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

WsfAssetManager::WsfAssetManager(WsfScenario&       aScenario,
                                 const std::string& aPartClass,
                                 const std::string& aPartVarName)
   : WsfScriptProcessor(aScenario, aPartClass, aPartVarName)
   , WsfC2ComponentContainer(std::make_shared<il::AssetManagerInterface>(IADSC2ScenarioExtension::GetGlobalLogger(aScenario)))
   , WsfScriptOverridableProcessor(true) // by default, asset manager drives everything else
   , m_default_am_impl(IADSC2ScenarioExtension::GetGlobalLogger(aScenario))
{
   SetType(WsfStringId(WsfScriptAssetManagerClass::BASE_CLASS_NAME));
}

WsfAssetManager::WsfAssetManager(WsfScenario&       aScenario,
                                 const std::string& aPartClass,
                                 const std::string& aPartVarName,
                                 const std::shared_ptr<il::AssetManagerInterface>&  am_component)
   : WsfScriptProcessor(aScenario, aPartClass, aPartVarName)
   , WsfC2ComponentContainer(am_component)
   , WsfScriptOverridableProcessor(true) // by default, asset manager drives everything else
   , m_default_am_impl(IADSC2ScenarioExtension::GetGlobalLogger(aScenario))
{
   SetType(WsfStringId(WsfScriptAssetManagerClass::BASE_CLASS_NAME));
}

WsfAssetManager::WsfAssetManager(const WsfAssetManager& aSrc)
   : WsfScriptProcessor(aSrc)
   , WsfC2ComponentContainer(aSrc)
   , WsfScriptOverridableProcessor(aSrc)
   , m_drop_monitor()
   , m_initiated_monitor()
   , m_updated_monitor()
   , m_callbacks()
   , m_default_am_impl(aSrc.m_default_am_impl)
{
   m_am_component.reset(aSrc.m_am_component->Clone());
}

WsfAssetManager::~WsfAssetManager()
{
}


WsfProcessor* WsfAssetManager::Clone() const
{
   return new WsfAssetManager(*this);
}

// Plugin Interface
extern "C" WSF_IADS_C2_LIB_EXPORT void WsfAssetManager::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   // Register the type if it isn't already
   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptAssetManagerClass::SCRIPT_CLASS_NAME))
   {
      RegisterBaseTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptAssetManagerClass>(WsfScriptAssetManagerClass::SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfAssetManager::Create(WsfScenario& aScenario)
{
   std::shared_ptr<il::AssetManagerInterface> proc = std::shared_ptr<il::AssetManagerInterface>(new il::AssetManagerInterface(IADSC2ScenarioExtension::GetGlobalLogger(aScenario)));

   return new WsfAssetManager(aScenario, WsfScriptAssetManagerClass::SCRIPT_CLASS_NAME, "PROCESSOR", proc);
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfAssetManager> WsfAssetManager::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfAssetManager>(aScenario);
}

const char* WsfAssetManager::GetScriptClassName() const
{
   return WsfScriptAssetManagerClass::SCRIPT_CLASS_NAME;
}

const char* WsfAssetManager::GetBaseClassName()
{
   return WsfScriptAssetManagerClass::BASE_CLASS_NAME;
}

bool WsfAssetManager::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   // Attempt to parse scripting for common bm elements
   if (ProcessAssetManagementInput(aInput) ||
       m_default_am_impl.ProcessInput(aInput) ||
       WsfScriptOverridableProcessor::ProcessInput(aInput) ||
       WsfScriptProcessor::ProcessInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

bool WsfAssetManager::ProcessAssetManagementInput(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "status_settings")
   {
      UtInputBlock block(aInput, "end_" + command);

      while (block.ReadCommand(command) && processed)
      {
         if (command == "subord_yellow_timeout")
         {
            double timeout = 0;
            block.GetInput().ReadValueOfType(timeout, UtInput::cTIME);
            m_am_component->setAssetYellowAgeout(timeout);
         }
         else if (command == "subord_red_timeout")
         {
            double timeout = 0;
            block.GetInput().ReadValueOfType(timeout, UtInput::cTIME);
            m_am_component->setAssetRedAgeout(timeout);
         }
         else if (command == "report_position_every")
         {
            double distance_m = 0;
            block.GetInput().ReadValueOfType(distance_m, UtInput::cLENGTH);
            m_am_component->setReportPositionEveryMeters(distance_m);
         }
         else if (command == "or_every")
         {
            double time_s = 0;
            block.GetInput().ReadValueOfType(time_s, UtInput::cTIME);
            m_am_component->setReportPositionEverySeconds(time_s);
         }
         else if (command == "report_status_every")
         {
            double time_s = 0;
            block.GetInput().ReadValueOfType(time_s, UtInput::cTIME);
            m_am_component->setReportStatusEverySeconds(time_s);
         }
         else if (command == "aggregate_unit_status")
         {
            const bool aggregate = block.GetInput().ReadBool();
            m_am_component->setAggregateUnitStatus(aggregate);
         }
         else if (command == "stationary_opns_only")
         {
            const bool stationary = block.GetInput().ReadBool();
            m_am_component->setStationaryOpnsOnly(stationary);
         }
         else if (command == "weapon_required")
         {
            const bool weapons_req = block.GetInput().ReadBool();
            m_am_component->setWeaponRequired(weapons_req);
         }
         else if (command == "require_all_weapons")
         {
            const bool req_all_weapons = block.GetInput().ReadBool();
            m_am_component->setRequireAllWeapons(req_all_weapons);
         }
         else if (command == "ew_required")
         {
            const bool ew_req = block.GetInput().ReadBool();
            m_am_component->setEWRequired(ew_req);
         }
         else if (command == "tar_required")
         {
            const bool tar_req = block.GetInput().ReadBool();
            m_am_component->setTARRequired(tar_req);
         }
         else if (command == "ttr_required")
         {
            const bool ttr_req = block.GetInput().ReadBool();
            m_am_component->setTTRRequired(ttr_req);
         }
         else
         {
            processed = false;
         }
      }
   }
   else if (command == "max_assignments")
   {
      int max_assignments = 0;
      aInput.ReadValue(max_assignments);
      if (max_assignments >= 0)
      {
         m_am_component->setMaxAssignments(max_assignments);
         //processed = true;
      }
      else
      {
         HCL_FATAL_LOGGER(m_am_component->getGlobalLogger()) << "Invalid max assignments: " << max_assignments;
         processed = false;
      }
   }
   else if (command == "assignment_delay")
   {
      double delay = 0;
      aInput.ReadValueOfType(delay, UtInput::cTIME);
      m_am_component->setAssignmentDelay(delay);
   }
   else if (command == "decision_update_delay")
   {
      double delay = 0;
      aInput.ReadValueOfType(delay, UtInput::cTIME);
      m_am_component->setDecisionUpdateDelay(delay);
   }
   else if (command == "log_status")
   {
      bool log_status;
      log_status = aInput.ReadBool();
      m_am_component->setLogMyStatus(log_status);
   }
   else
      // unknown block
   {
      processed = false;
   }

   return processed;
}

WsfAssetMap* WsfAssetManager::GetAssetMapPtr()
{
   return new WsfAssetMap(&m_am_component->getAssets(), m_am_component->getGlobalLogger());
}

WsfBMAssetRecord* WsfAssetManager::GetAssetPtr()
{
   return new WsfBMAssetRecord(m_am_component->getThisAsset());
}

bool WsfAssetManager::Initialize(double aSimTime)
{
   bool success = true;

   if (!OverrideInternalInit())
   {
      m_default_am_impl.Initialize(this);
   }

   // base initialization - always happens
   success &= WsfScriptProcessor::Initialize(aSimTime);

   return success;
}

bool WsfAssetManager::Initialize2(double aSimTime)
{
   bool success = true;

   if (!OverrideInternalInit2())
   {
      // hook any other C2 processors that may be employed on this platform
      if (!SetComponent(WsfBMUtils::FindAttachedDisseminationManagerRef(this->GetPlatform(), false))) // not technically required
      {
         HCL_WARN_LOGGER(m_am_component->getGlobalLogger()) << "Employed an asset manager but no C2 Dissemination processor was found. Cannot send C2 messages.";
      }

      SetComponent(WsfBMUtils::FindAttachedBattleManagerRef(this->GetPlatform(), false)); // not required
      SetComponent(WsfBMUtils::FindAttachedSensorsManagerRef(this->GetPlatform(), false));

      m_default_am_impl.Initialize2(this);
   }

   // Register ourselves for the drop track callback of the platforms track manager
   auto track_mgr = &GetPlatform()->GetTrackManager();
   m_callbacks.Add(track_mgr->LocalTrackDropped.Connect(&WsfAssetManager::LocalTrackDropped, this));
   m_callbacks.Add(track_mgr->LocalTrackInitiated.Connect(&WsfAssetManager::LocalTrackInitiated, this));
   m_callbacks.Add(track_mgr->LocalTrackUpdated.Connect(&WsfAssetManager::LocalTrackUpdated, this));

   // base initialization - always happens
   success &= WsfScriptProcessor::Initialize2(aSimTime);

   return success;
}

bool WsfAssetManager::ConnectTrackDropCallback(const std::string& drop_handler_func)
{
   bool success = true;

   // Initialize the callback to script dispatcher
   if (!m_drop_monitor.Initialize(this, drop_handler_func))
   {
      success = false;
   }

   return success;
}

bool WsfAssetManager::ConnectTrackInitiatedCallback(const std::string& handler_func)
{
   bool success = true;

   // Initialize the callback to script dispatcher
   if (!m_initiated_monitor.Initialize(this, handler_func))
   {
      success = false;
   }

   return success;
}

bool WsfAssetManager::ConnectTrackUpdatedCallback(const std::string& handler_func)
{
   bool success = true;

   // Initialize the callback to script dispatcher
   if (!m_updated_monitor.Initialize(this, handler_func))
   {
      success = false;
   }

   return success;
}

void WsfAssetManager::Update(double aSimTime)
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

void WsfAssetManager::Run(const double aSimTime)
{
   m_default_am_impl.on_update(aSimTime, GetPlatform(), this);
   // base class updates always occur
   WsfScriptProcessor::Update(aSimTime);
}

bool WsfAssetManager::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool success = true;

   if (!OverrideInternalOnMessage())
   {
      m_default_am_impl.on_message(aSimTime, aMessage, this);
   }
   else
   {
      // nothing to do, let the script processor handle it
   }

   // base class processing always occur
   success &= WsfScriptProcessor::ProcessMessage(aSimTime, aMessage);

   return success;
}

bool WsfAssetManager::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool success = true;

   if (!OverrideInternalReceiveMessage())
   {
      // nothing to do
   }

   success &= WsfScriptProcessor::ReceiveMessage(aSimTime, aMessage);

   return success;
}

std::tuple<bool, double> WsfAssetManager::ShouldDelayIncomingMessage(double aSimTime, const WsfMessage& aMessage)
{
   double delay_amount_s = -1;
   auto should_delay = m_default_am_impl.ShouldDelayIncomingMessage(aSimTime, aMessage, this, delay_amount_s);

   return std::make_tuple(should_delay, delay_amount_s);
}

void WsfAssetManager::LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   if (!OverrideInternalLocalTrackDropped())
   {
      m_default_am_impl.LocalTrackDropped(aSimTime, aLocalTrackPtr, this);
   }

   if (m_drop_monitor.Initialized() && !m_drop_monitor.Invoke(aSimTime, aLocalTrackPtr))
   {
      HCL_ERROR_LOGGER(m_am_component->getGlobalLogger()) << "Error invoking drop track monitor LocalTrackDropped()";
   }
}

void WsfAssetManager::LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
{
   if (!OverrideInternalLocalTrackInitiated())
   {
      m_default_am_impl.LocalTrackInitiated(aSimTime, aLocalTrackPtr, aRawTrackPtr, this);
   }

   if (m_initiated_monitor.Initialized() && !m_initiated_monitor.Invoke(aSimTime, aLocalTrackPtr, aRawTrackPtr))
   {
      HCL_ERROR_LOGGER(m_am_component->getGlobalLogger()) << "Error invoking initiated track monitor LocalTrackInitiated()";
   }
}

void WsfAssetManager::LocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
{
   if (!OverrideInternalLocalTrackUpdated())
   {
      m_default_am_impl.LocalTrackUpdated(aSimTime, aLocalTrackPtr, aRawTrackPtr, this);
   }

   if (m_updated_monitor.Initialized() && !m_updated_monitor.Invoke(aSimTime, aLocalTrackPtr, aRawTrackPtr))
   {
      HCL_ERROR_LOGGER(m_am_component->getGlobalLogger()) << "Error invoking updated track monitor LocalTrackUpdated()";
   }
}

double WsfAssetManager::TimeLastReceivedTrackUpdate(const WsfLocalTrack* track) const
{
   double last_update_time = -1;

   // look up our internal asset if any and return the last update time (-1) if we haven't received this
   // asset yet
   const auto& last_update = m_am_component->getMasterTrack(WsfBMUtils::ConvertTrackId(track->GetTrackId()));
   if (last_update)
   {
      last_update_time = last_update->getTime();
   }

   return last_update_time;
}

int WsfAssetManager::GetMaxAssignments() const
{
   return m_am_component->getMaxAssignments();
}

int WsfAssetManager::GetCurrentNumAssignments() const
{
   return m_am_component->getNumAssignments();
}

double WsfAssetManager::GetAssignmentDelaySecs() const
{
   return m_am_component->getAssignmentDelay();
}
