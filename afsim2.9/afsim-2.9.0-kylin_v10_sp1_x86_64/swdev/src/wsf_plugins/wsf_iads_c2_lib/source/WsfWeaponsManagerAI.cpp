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
*   File: WsfWeaponsManagerAI.cpp
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

#include <WsfWeaponsManagerAI.hpp>
#include <WsfScriptWeaponsManagerAIClass.hpp>
#include <WsfBMPluginUtilities.hpp>
#include <iadsLib/weaponsManagerAI.hpp>
#include <UtInputBlock.hpp>
#include <UtMemory.hpp>
#include <iadsLib/util.hpp>
#include <iadsLib/NumericUtils.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"

WsfWeaponsManagerAI::WsfWeaponsManagerAI(WsfScenario& aScenario, const std::shared_ptr<il::weaponsManagerAI>& wm_component)
   : WsfBMWeaponsManager(aScenario, WsfScriptWeaponsManagerAIClass::SCRIPT_CLASS_NAME, "PROCESSOR", wm_component)
   , m_engagement_mod(new WsfWMAIEngagementMod(aScenario, this))
     //  , m_rwr_record()
{
   m_wm_component->setWeaponEngagementModule(this->m_engagement_mod);
}

WsfWeaponsManagerAI::WsfWeaponsManagerAI(const WsfWeaponsManagerAI& from)
   : WsfBMWeaponsManager(from)
   , m_engagement_mod(new WsfWMAIEngagementMod(from.GetScenario(), this))
     //, m_rwr_record(from.m_rwr_record)
{
   m_wm_component->setWeaponEngagementModule(this->m_engagement_mod);
}

WsfProcessor* WsfWeaponsManagerAI::Clone() const
{
#ifdef IC2DEBUG
   HCL_TRACE_LOGGER() << "WsfWeaponsManagerAI::Clone(before)";
#endif
   return new WsfWeaponsManagerAI(*this);
}

// Plugin Interface
extern "C" WSF_IADS_C2_LIB_EXPORT void WsfWeaponsManagerAI::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptWeaponsManagerAIClass::SCRIPT_CLASS_NAME))
   {
      // base class registration
      WsfBMWeaponsManager::RegisterScriptTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptWeaponsManagerAIClass>(WsfScriptWeaponsManagerAIClass::SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfWeaponsManagerAI::Create(WsfScenario& aScenario)
{
   std::shared_ptr<il::weaponsManagerAI> wm(new il::weaponsManagerAI());

#ifdef IC2DEBUG
   HCL_INFO_LOGGER() << "WsfWeaponsManagerAI::Create(): wm = " << wm;
#endif

   return new WsfWeaponsManagerAI(aScenario, wm);
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfWeaponsManagerAI> WsfWeaponsManagerAI::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfWeaponsManagerAI>(aScenario);
}
// End Plugin Interface

bool WsfWeaponsManagerAI::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   if (ProcessWeaponsManagerAIInput(aInput) ||
       WsfBMWeaponsManager::ProcessInput(aInput) ||
       m_engagement_mod->ProcessInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

const char* WsfWeaponsManagerAI::GetScriptClassName() const
{
   return WsfScriptWeaponsManagerAIClass::SCRIPT_CLASS_NAME;
}

bool WsfWeaponsManagerAI::Initialize(double aSimTime)
{
   bool ok = WsfBMWeaponsManager::Initialize(aSimTime);

   ok &= m_engagement_mod->Initialize(aSimTime);

   return ok;
}

bool WsfWeaponsManagerAI::Initialize2(double aSimTime)
{
   bool ok = WsfBMWeaponsManager::Initialize2(aSimTime);

   ok &= m_engagement_mod->Initialize2(aSimTime);

   auto wm = dynamic_cast<il::weaponsManagerAI*>(GetCoreWMRef().get());

   if (wm)
   {
      auto plat = GetPlatform();
      if (!plat)
      {
         return false;
      }

      // If home base position has the default unset altitude, it was not configured via scripting params, so set the lat & lon to starting position.
      if (NumericUtils::LessEqual(wm->getHomeBasePosition().getAltM(), il::C_DEFAULT_UNSET_HOME_BASE_POSITION_ALTITUDE))
      {
         double lat, lon, alt;
         plat->GetLocationLLA(lat, lon, alt);
         wm->setHomeBasePosition(lat * vm::DEGREES_TO_RADIANS, lon * vm::DEGREES_TO_RADIANS); // Sets altitude to 0
      }

      wm->setReturnToCAP(!NumericUtils::NearlyEqual(0.0, plat->GetSpeed()));
   }

   return ok;
}

void WsfWeaponsManagerAI::Update(double aSimTime)
{
   WsfBMWeaponsManager::Update(aSimTime);
}

bool WsfWeaponsManagerAI::ProcessWeaponsManagerAIInput(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "take_action")
   {
      UtInputBlock block(aInput, "end_" + command);
      processed = ProcessTakeActionBlock(block);
   }
   else if (command == "rwr_response")
   {
      UtInputBlock block(aInput, "end_" + command);
      processed = ProcessRWRResponseBlock(block);
   }
   else if (command == "alert_time")
   {
      double alert_time = 0;
      aInput.ReadValueOfType(alert_time, UtInput::cTIME);
      if (alert_time >= 0)
      {
         static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setAlertTime(alert_time);
      }
      else
      {
         HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid alert time: " << alert_time;
         processed = false;
      }
   }
   else if (command == "assess_engage_via")
   {
      std::string value;
      il::eAssessEngage assess;
      aInput.ReadValue(value);

      if (il::AssessEngageStringToEnum(value, assess, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
      {
         static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setAssessEngageVia(assess);
      }
      else
      {
         processed = false;
      }
   }
   else if (command == "escort")
   {
      bool enable;
      enable = aInput.ReadBool();
      static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEscort(enable);
   }
   else if (command == "debug")
   {
      static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setDebug(true);
   }
   else if (command == "intercept_speed")
   {
      double intercept_speed = 0;
      aInput.ReadValueOfType(intercept_speed, UtInput::cSPEED);
      if (intercept_speed >= 0)
      {
         static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setInterceptSpeed(intercept_speed);
      }
      else
      {
         HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid intercept speed: " << intercept_speed;
         processed = false;
      }
   }
   else if (command == "my_place_in_formation")
   {
      int place_in_formation = 0;
      aInput.ReadValue(place_in_formation);
      if (place_in_formation >= 0)
      {
         static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setPlaceInFormation(place_in_formation);
      }
      else
      {
         HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid place in formation: " << place_in_formation;
         processed = false;
      }
   }
   else if (command == "pursuit_range")
   {
      double range = 0;
      aInput.ReadValueOfType(range, UtInput::cLENGTH);
      if (range >= 0)
      {
         static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setPursuitRange(range);
      }
      else
      {
         HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid pursuit range: " << range;
         processed = false;
      }
   }
   else if (command == "home_base_position")
   {
      double lat, lon;
      aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
      static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setHomeBasePosition(lat, lon);
   }
   else if (command == "salvo_delay")
   {
      double delay = 0;
      aInput.ReadValueOfType(delay, UtInput::cTIME);
      if (delay >= 0)
      {
         static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setSalvoDelay(delay);
      }
      else
      {
         HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid salvo delay: " << delay;
         processed = false;
      }
   }

   else
      // unknown block
   {
      processed = false;
   }

   return processed;
}

//// Returns a WsfTrackList containing all threats detected by the RWR (i.e., all tracks in its active track list).
//WsfTrackList WsfWeaponsManagerAI::RWRDetectedThreatList()
//{
//	WsfTrackList trackList;
//	m_rwr_record.GetRWRActiveTrackList(trackList);
//	return trackList;
//}

bool WsfWeaponsManagerAI::ProcessTakeActionBlock(UtInputBlock& block)
{
   bool processed = true;

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      auto& aInput = block.GetInput();

      if (command == "enable")
      {
         static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionTakeAction(true);
      }
      else if (command == "disable")
      {
         static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionTakeAction(false);
      }
      else if (command == "action_distance")
      {
         double distance = 0;
         aInput.ReadValueOfType(distance, UtInput::cLENGTH);
         if (distance >= 0)
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionTakeActionDistance(distance);
         }
         else
         {
            HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid take action distance: " << distance;
            processed = false;
         }
      }
      else if (command == "ai_action_1")
      {
         std::string value;
         il::eTakeAction action;
         aInput.ReadValue(value);

         if (il::TakeActionStringToEnum(value, action, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionAction1(action);
         }
         else
         {
            processed = false;
         }
      }
      else if (command == "duration_1")
      {
         double duration = 0;
         aInput.ReadValueOfType(duration, UtInput::cTIME);
         if (duration >= 0)
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionAction1Duration(duration);
         }
         else
         {
            HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid take action duration 1: " << duration;
            processed = false;
         }
      }
      else if (command == "ai_action_2")
      {
         std::string value;
         il::eTakeAction action;
         aInput.ReadValue(value);

         if (il::TakeActionStringToEnum(value, action, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionAction2(action);
         }
         else
         {
            processed = false;
         }
      }
      else if (command == "duration_2")
      {
         double duration = 0;
         aInput.ReadValueOfType(duration, UtInput::cTIME);
         if (duration >= 0)
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionAction2Duration(duration);
         }
         else
         {
            HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid take action duration 2: " << duration;
            processed = false;
         }
      }
      else if (command == "ai_action_3")
      {
         std::string value;
         il::eTakeAction action;
         aInput.ReadValue(value);

         if (il::TakeActionStringToEnum(value, action, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionAction3(action);
         }
         else
         {
            processed = false;
         }
      }
      else if (command == "duration_3")
      {
         double duration = 0;
         aInput.ReadValueOfType(duration, UtInput::cTIME);
         if (duration >= 0)
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionAction3Duration(duration);
         }
         else
         {
            HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid take action duration 3: " << duration;
            processed = false;
         }
      }
      else if (command == "ai_action_4")
      {
         std::string value;
         il::eTakeAction action;
         aInput.ReadValue(value);

         if (il::TakeActionStringToEnum(value, action, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionAction4(action);
         }
         else
         {
            processed = false;
         }
      }
      else if (command == "duration_4")
      {
         double duration = 0;
         aInput.ReadValueOfType(duration, UtInput::cTIME);
         if (duration >= 0)
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionAction4Duration(duration);
         }
         else
         {
            HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid take action duration 4: " << duration;
            processed = false;
         }
      }
      else
      {
         processed = false;
      }
   }

   return processed;
}

bool WsfWeaponsManagerAI::ProcessRWRResponseBlock(UtInputBlock& block)
{
   bool processed = true;

   std::string command;
   while (block.ReadCommand(command) && processed)
   {
      auto& aInput = block.GetInput();

      if (command == "enable")
      {
         static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionRWRResponse(true);
      }
      else if (command == "disable")
      {
         static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionRWRResponse(false);
      }
      else if (command == "beam_duration")
      {
         double duration = 0;
         aInput.ReadValueOfType(duration, UtInput::cTIME);
         if (duration >= 0)
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionRWRResponseBeamDuration(duration);
         }
         else
         {
            HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid RWR response beam duration: " << duration;
            processed = false;
         }
      }
      else if (command == "sam_response_range")
      {
         double range = 0;
         aInput.ReadValueOfType(range, UtInput::cLENGTH);
         if (range >= 0)
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionRWRSAMResponseRange(range);
         }
         else
         {
            HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid RWR response SAM response range: " << range;
            processed = false;
         }
      }
      else if (command == "ai_response_range")
      {
         double range = 0;
         aInput.ReadValueOfType(range, UtInput::cLENGTH);
         if (range >= 0)
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionRWRAIResponseRange(range);
         }
         else
         {
            HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid RWR response AI response range: " << range;
            processed = false;
         }
      }
      else if (command == "restore_after_break_lock")
      {
         double duration = 0;
         aInput.ReadValueOfType(duration, UtInput::cTIME);
         if (duration >= 0)
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionRWRResponseRestoreAfterBreakLock(duration);
         }
         else
         {
            HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Invalid RWR response restore after break lock duration: " << duration;
            processed = false;
         }
      }
      else if (command == "priority")
      {
         std::string value;
         il::eRWRResponsePriority priority;
         aInput.ReadValue(value);

         if (il::RWRPriorityStringToEnum(value, priority, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
         {
            static_cast<il::weaponsManagerAI*>(m_wm_component.get())->setEvasionRWRResponsePriority(priority);
         }
         else
         {
            processed = false;
         }
      }

      else
      {
         processed = false;
      }
   }

   return processed;
}
