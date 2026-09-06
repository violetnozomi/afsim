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

#include "WsfCyberSimulationExtension.hpp"

#include "UtMemory.hpp"
#include "WsfCSV_EventOutput.hpp"
#include "WsfCyberAttackTypes.hpp"
#include "WsfCyberEventPipe.hpp"
#include "WsfCyberEventResults.hpp"
#include "WsfCyberObserver.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfEventOutput.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "observer/WsfPlatformObserver.hpp"
#include "script/WsfScriptObserver.hpp"

namespace
{
void Register_event_output(wsf::event::output::SimulationExtension& aEventOutput)
{
   WsfSimulation* simPtr = &aEventOutput.GetSimulation();

   using namespace wsf::event;
   aEventOutput.AddEvent<CyberAttackInitiated>("CYBER_ATTACK_INITIATED", WsfObserver::CyberAttackInitiated(simPtr));
   aEventOutput.AddEvent<CyberAttackSucceeded>("CYBER_ATTACK_SUCCEEDED", WsfObserver::CyberAttackSucceeded(simPtr));
   aEventOutput.AddEvent<CyberAttackFailed>("CYBER_ATTACK_FAILED", WsfObserver::CyberAttackFailed(simPtr));
   aEventOutput.AddEvent<CyberAttackDetected>("CYBER_ATTACK_DETECTED", WsfObserver::CyberAttackDetected(simPtr));
   aEventOutput.AddEvent<CyberAttackAttributed>("CYBER_ATTACK_ATTRIBUTED", WsfObserver::CyberAttackAttributed(simPtr));
   aEventOutput.AddEvent<CyberAttackRecovery>("CYBER_ATTACK_RECOVERY", WsfObserver::CyberAttackRecovery(simPtr));
   aEventOutput.AddEvent<CyberScanInitiated>("CYBER_SCAN_INITIATED", WsfObserver::CyberScanInitiated(simPtr));
   aEventOutput.AddEvent<CyberScanSucceeded>("CYBER_SCAN_SUCCEEDED", WsfObserver::CyberScanSucceeded(simPtr));
   aEventOutput.AddEvent<CyberScanFailed>("CYBER_SCAN_FAILED", WsfObserver::CyberScanFailed(simPtr));
   aEventOutput.AddEvent<CyberScanDetected>("CYBER_SCAN_DETECTED", WsfObserver::CyberScanDetected(simPtr));
   aEventOutput.AddEvent<CyberScanAttributed>("CYBER_SCAN_ATTRIBUTED", WsfObserver::CyberScanAttributed(simPtr));
   aEventOutput.AddEvent<CyberTriggerEvaluation>("CYBER_TRIGGER_EVALUATION", WsfObserver::CyberTriggerEvaluation(simPtr));
   aEventOutput.AddEvent<CyberTriggerExecution>("CYBER_TRIGGER_EXECUTION", WsfObserver::CyberTriggerExecution(simPtr));
}

void Register_script_observer(WsfScriptObserver& aScriptObserver)
{
   auto& sim = aScriptObserver.GetSimulation();
   aScriptObserver.AddEvent("CYBER_ATTACK_INITIATED",
                            WsfObserver::CyberAttackInitiated(&sim),
                            "CyberAttackInitiated",
                            "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_ATTACK_SUCCEEDED",
                            WsfObserver::CyberAttackSucceeded(&sim),
                            "CyberAttackSucceeded",
                            "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_ATTACK_FAILED",
                            WsfObserver::CyberAttackFailed(&sim),
                            "CyberAttackFailed",
                            "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_ATTACK_DETECTED",
                            WsfObserver::CyberAttackDetected(&sim),
                            "CyberAttackDetected",
                            "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_ATTACK_ATTRIBUTED",
                            WsfObserver::CyberAttackAttributed(&sim),
                            "CyberAttackAttributed",
                            "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_ATTACK_RECOVERY",
                            WsfObserver::CyberAttackRecovery(&sim),
                            "CyberAttackRecovery",
                            "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_SCAN_INITIATED",
                            WsfObserver::CyberScanInitiated(&sim),
                            "CyberScanInitiated",
                            "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_SCAN_SUCCEEDED",
                            WsfObserver::CyberScanSucceeded(&sim),
                            "CyberScanSucceeded",
                            "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_SCAN_FAILED", WsfObserver::CyberScanFailed(&sim), "CyberScanFailed", "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_SCAN_DETECTED",
                            WsfObserver::CyberScanDetected(&sim),
                            "CyberScanDetected",
                            "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_SCAN_ATTRIBUTED",
                            WsfObserver::CyberScanAttributed(&sim),
                            "CyberScanAttributed",
                            "WsfCyberEngagement");
   aScriptObserver.AddEvent("CYBER_TRIGGER_EVALUATION",
                            WsfObserver::CyberTriggerEvaluation(&sim),
                            "CyberTriggerEvaluation",
                            "WsfCyberTrigger, bool");
   aScriptObserver.AddEvent("CYBER_TRIGGER_EXECUTION",
                            WsfObserver::CyberTriggerExecution(&sim),
                            "CyberTriggerExecution",
                            "WsfCyberTrigger");
}
} // namespace

namespace wsf
{
namespace cyber
{
// =================================================================================================
//! Return a reference to the cyber simulation extension.
// static
SimulationExtension& SimulationExtension::Get(const WsfSimulation& aSimulation)
{
   WsfSimulationExtension* extensionPtr = aSimulation.FindExtension("wsf_cyber");
   return static_cast<SimulationExtension&>(*extensionPtr);
}

// =================================================================================================
void SimulationExtension::AddedToSimulation()
{
   // Hook in the script observer handlers for our events...
   auto scriptObserverPtr = WsfScriptObserver::Find(GetSimulation());
   if (scriptObserverPtr)
   {
      Register_script_observer(*scriptObserverPtr);
   }

   // If the event_output extension is available, hook in the handlers for our events.
   WsfEventOutput* eventOutputPtr = WsfEventOutput::Find(GetSimulation());
   if (eventOutputPtr != nullptr)
   {
      Register_event_output(*eventOutputPtr);
   }

   // If the csv_event_output extension is available, hook in the handlers for our events.
   WsfCSV_EventOutput* csvEventOutputPtr = WsfCSV_EventOutput::Find(GetSimulation());
   if (csvEventOutputPtr != nullptr)
   {
      WsfCSV_EventOutputData::AddDataTags("CYBER_ATTACK_INITIATED",
                                          {"time<time>",
                                           "event<string>",
                                           "platform<string>",
                                           /*"attacking_platform",*/ "victim_platform<string>",
                                           "attack_type<string>"});
      WsfCSV_EventOutputData::AddDataTags("CYBER_ATTACK_SUCCEEDED",
                                          {"time<time>",
                                           "event<string>",
                                           "platform<string>",
                                           /*"attacking_platform",*/ "victim_platform<string>",
                                           "attack_type<string>",
                                           "success_threshold<double>",
                                           "success_draw<double>",
                                           "report_status<string>",
                                           "report_threshold<double>",
                                           "report_draw<double>",
                                           "detect_threshold<double>",
                                           "detect_draw<double>"});
      WsfCSV_EventOutputData::AddDataTags("CYBER_ATTACK_FAILED",
                                          {"time<time>",
                                           "event<string>",
                                           "platform<string>",
                                           /*"attacking_platform",*/ "victim_platform<string>",
                                           "attack_type<string>",
                                           "reason<string>",
                                           "success_threshold<double>",
                                           "success_draw<double>",
                                           "report_status<string>",
                                           "report_threshold<double>",
                                           "report_draw<double>"});
      WsfCSV_EventOutputData::AddDataTags("CYBER_ATTACK_DETECTED",
                                          {"time<time>",
                                           "event<string>",
                                           "platform<string>",
                                           /*"attacking_platform",*/ "victim_platform<string>",
                                           "attack_type<string>"});
      WsfCSV_EventOutputData::AddDataTags("CYBER_ATTACK_ATTRIBUTED",
                                          {"time<time>",
                                           "event<string>",
                                           "platform<string>",
                                           /*"attacking_platform",*/ "victim_platform<string>",
                                           "attack_type<string>"});
      WsfCSV_EventOutputData::AddDataTags("CYBER_ATTACK_RECOVERY",
                                          {"time<time>",
                                           "event<string>",
                                           "platform<string>",
                                           /*"attacking_platform",*/ "victim_platform<string>",
                                           "attack_type<string>",
                                           "immunity_status<string>",
                                           "immunity_threshold<double>",
                                           "immunity_draw<double>"});
      WsfCSV_EventOutputData::AddDataTags("CYBER_SCAN_INITIATED",
                                          {"time<time>",
                                           "event<string>",
                                           "platform<string>",
                                           /*"attacking_platform",*/ "victim_platform<string>",
                                           "attack_type<string>"});
      WsfCSV_EventOutputData::AddDataTags("CYBER_SCAN_SUCCEEDED",
                                          {"time<time>",
                                           "event<string>",
                                           "platform<string>",
                                           /*"attacking_platform",*/ "victim_platform<string>",
                                           "attack_type<string>",
                                           "detect_threshold<double>",
                                           "detect_draw<double>"});
      WsfCSV_EventOutputData::AddDataTags("CYBER_SCAN_FAILED",
                                          {"time<time>",
                                           "event<string>",
                                           "platform<string>",
                                           /*"attacking_platform",*/ "victim_platform<string>",
                                           "attack_type<string>",
                                           "reason<string>",
                                           "detect_threshold<double>",
                                           "detect_draw<double>"});
      WsfCSV_EventOutputData::AddDataTags(
         "CYBER_SCAN_DETECTED",
         {"time<time>", "event<string>", /*"attacking_platform",*/ "victim_platform<string>", "attack_type<string>"});
      WsfCSV_EventOutputData::AddDataTags(
         "CYBER_SCAN_ATTRIBUTED",
         {"time<time>", "event<string>", /*"attacking_platform",*/ "victim_platform<string>", "attack_type<string>"});
      WsfCSV_EventOutputData::AddDataTags(
         "CYBER_TRIGGER_EXECUTION",
         {"time<time>", "event<string>", "trigger_scope<string>", "trigger_name<string>", "platform<string>"});
      WsfCSV_EventOutputData::AddDataTags("CYBER_TRIGGER_EVALUATION",
                                          {"time<time>",
                                           "event<string>",
                                           "trigger_scope<string>",
                                           "trigger_name<string>",
                                           "platform<string>",
                                           "evaluation_result<bool>"});
      Register_event_output(*csvEventOutputPtr);
   }

   // If the event_pipe extension is available, hook in the handers for our events.
   WsfEventPipeInterface* eventPipePtr = WsfEventPipeInterface::Find(GetSimulation());
   if (eventPipePtr != nullptr)
   {
      mEventPipePtr = ut::make_unique<EventPipe>(eventPipePtr);
      eventPipePtr->AddLogger(mEventPipePtr.get());
   }

   // Initialize the cyber event manager
   mEventManagerPtr = ut::make_unique<EventManager>(GetSimulation());

   // Initialize the cyber draw manager
   mDrawManagerPtr = ut::make_unique<random::RandomManager>(&GetSimulation());

   // Initialize the visualization manager
   if (ScenarioExtension::Get(GetScenario()).IsDrawEnabled())
   {
      mVisualizationManager.AddVisualization(ut::make_unique<VisualizationDraw>(&GetSimulation()));
   }

   // Initialize the trigger manager
   mTriggerManagerPtr = ut::make_unique<trigger::Manager>(GetSimulation());

   //! Attach the platform deletion callback, to aid in maintaining engagements
   WsfSimulation* simPtr = &GetSimulation();
   mCallbacks.Add(WsfObserver::PlatformDeleted(simPtr).Connect(&SimulationExtension::PlatformDeleted, this));
}

// =================================================================================================
bool SimulationExtension::Initialize()
{
   //! Initialize the random variables for attack types. Because of the way that WsfRandomVariable
   //! is initialized, this must be done here to have access to the simulation random seed, as
   //! attack types are not platform components and have no reasonable means of access to the simulation.
   //! Therefore, the sim will "push" this data to the attack when it becomes available here via
   //! initialization.
   AttackTypes& attackListRef = ScenarioExtension::Get(GetScenario()).GetAttackTypes();

   std::vector<Attack*> attackPtrList;
   attackListRef.GetCurrentTypes(attackPtrList);

   bool ok = true;
   for (const auto& attackPtr : attackPtrList)
   {
      ok &= attackPtr->Initialize(GetSimulation().GetSimTime(), GetSimulation());

      //! Check each attack for valid effects.
      auto& effects = attackPtr->GetEffects();
      for (auto& effect : effects)
      {
         if (!ScenarioExtension::Get(GetScenario()).GetEffectTypeExists(effect))
         {
            auto logWarning = ut::log::warning() << "Invalid or unrecognized Effect. ";
            logWarning.AddNote() << "Effect: " << effect;
            ok = false;
         }
      }
   }

   ok &= mTriggerManagerPtr->Initialize();

   return ok;
}

// =================================================================================================
void SimulationExtension::PlatformDeleted(double, WsfPlatform* aPlatformPtr)
{
   mEngagementManager.CullVictimEngagements(aPlatformPtr->GetName());
}

} // namespace cyber
} // namespace wsf
