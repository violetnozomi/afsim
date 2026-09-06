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

#include "WsfCyberScriptExtensions.hpp"

#include <memory>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfCyberAttackParameters.hpp"
#include "WsfCyberConstraintTypes.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfCyberEventManager.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfCyberSimulationExtension.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "effects/WsfCyberMitmMessage.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"
#include "triggers/WsfCyberCompositeTrigger.hpp"
#include "triggers/WsfCyberTrigger.hpp"
#include "triggers/WsfCyberTriggerInterval.hpp"

namespace
{
int DelayStatus(bool               aAttack,
                const std::string& aAttackName,
                WsfPlatform*       aAttackerPtr,
                WsfPlatform*       aVictimPtr,
                WsfSimulation&     aSim)
{
   if (aAttackName.empty() || !aAttackerPtr || !aVictimPtr)
   {
      return static_cast<int>(wsf::cyber::Event::Type::cNONE);
   }

   auto engagementPtr =
      wsf::cyber::EngagementManager::Get(aSim).FindEngagement(aAttackName, aAttackerPtr->GetName(), aVictimPtr->GetName());
   if (!engagementPtr)
   {
      return static_cast<int>(wsf::cyber::Event::Type::cNONE);
   }

   auto key  = engagementPtr->GetKey();
   auto type = wsf::cyber::SimulationExtension::Get(aSim).GetCyberEventManager().GetEventType(aAttack, key);

   return static_cast<int>(type);
}

bool CancelEvent(bool               aAttack,
                 const std::string& aAttackName,
                 WsfPlatform*       aAttackerPtr,
                 WsfPlatform*       aVictimPtr,
                 WsfSimulation&     aSim)
{
   if (aAttackName.empty() || !aAttackerPtr || !aVictimPtr)
   {
      return false;
   }

   auto engagementPtr =
      wsf::cyber::EngagementManager::Get(aSim).FindEngagement(aAttackName, aAttackerPtr->GetName(), aVictimPtr->GetName());
   if (!engagementPtr)
   {
      return false;
   }

   auto key    = engagementPtr->GetKey();
   auto result = wsf::cyber::SimulationExtension::Get(aSim).GetCyberEventManager().CancelEvent(aAttack, key);

   return result;
}
} // namespace

namespace wsf
{
namespace cyber
{
namespace PlatformScriptMethods
{
enum CyberStatusReturn
{
   cUNKNOWN = -1,
   cFAILED,
   cSUCCEEDED
};


// ----------------------------------------------------------------------------
//                            Cyber Attack Platform Methods
// ----------------------------------------------------------------------------

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, CyberAttack_1, 2, "bool", "WsfPlatform, string")
{
   WsfPlatform* victimPlatform = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto& cyberSim      = wsf::cyber::SimulationExtension::Get(*SIMULATION);
   auto& cyberScenario = wsf::cyber::ScenarioExtension::Get(*SCENARIO);
   if (cyberScenario.GetAttackTypeInputRequirement(aVarArgs[1].GetString(), Effect::InputRequirement::cREQUIRED))
   {
      UT_SCRIPT_ABORT("Call made with too few arguments - associated effect(s) require user provided variable(s).");
   }

   bool returnVal = cyberSim.GetCyberEngagementManager().CyberAttack(aVarArgs[1].GetString(),
                                                                     aObjectPtr->GetNameId(),
                                                                     victimPlatform->GetNameId(),
                                                                     *SIMULATION);

   aReturnVal.SetBool(returnVal);
}


// This method performs a cyber attack with user defined variables passed as an argument to the script
// call. This data may be constructed programmatically, or via the script context. Regardless, this data
// will be available to the effects instantiated with this attack. For more details, see the cyber attack
// parameters object.
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, CyberAttack_2, 3, "bool", "WsfPlatform, string, WsfCyberAttackParameters")
{
   WsfPlatform* victimPlatform = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto         attackName     = aVarArgs[1].GetString();

   auto& cyberSim      = wsf::cyber::SimulationExtension::Get(*SIMULATION);
   auto& cyberScenario = wsf::cyber::ScenarioExtension::Get(*SCENARIO);
   if ((!cyberScenario.GetAttackTypeInputRequirement(attackName, Effect::InputRequirement::cREQUIRED)) &&
       (!cyberScenario.GetAttackTypeInputRequirement(attackName, Effect::InputRequirement::cOPTIONAL)))
   {
      UT_SCRIPT_ABORT(
         "Call made with too many arguments - associated effect(s) do not require user provided variable.");
   }

   auto parametersPtr = aVarArgs[2].GetPointer()->GetAppObject<AttackParameters>();
   bool returnVal     = cyberSim.GetCyberEngagementManager().CyberAttack(aVarArgs[1].GetString(),
                                                                     aObjectPtr->GetNameId(),
                                                                     victimPlatform->GetNameId(),
                                                                     *SIMULATION,
                                                                     parametersPtr);

   aReturnVal.SetBool(returnVal);
}

// ----------------------------------------------------------------------------
//                            Cyber Platform Component Methods
// ----------------------------------------------------------------------------

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, Constraint, 0, "WsfCyberConstraint", "")
{
   wsf::cyber::Constraint* lPtr = aObjectPtr->template GetComponent<wsf::cyber::Constraint>();
   aReturnVal.SetPointer(new UtScriptRef(lPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, CyberAttackStatus, 2, "int", "WsfPlatform, string")
{
   auto&        cyberSim       = wsf::cyber::SimulationExtension::Get(*SIMULATION);
   WsfPlatform* victimPlatform = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto* engagement = cyberSim.GetCyberEngagementManager().FindEngagement(aVarArgs[1].GetString(),
                                                                          aObjectPtr->GetNameId(),
                                                                          victimPlatform->GetNameId());

   if (!engagement)
   {
      aReturnVal.SetInt(cFAILED);
   }
   else
   {
      // Note that delivery delay time will prevent a draw for status reporting until the delay
      // is complete. We'll return the same status (cUNKNOWN) regardless of waiting for delivery
      // delay or if the status is not being reported to the attacker
      if (!engagement->GetStatusReportSuccess())
      {
         // A status report wasn't available to the attacker, and thus is unknown
         aReturnVal.SetInt(cUNKNOWN);
      }
      else
      {
         // A status report was made. Return attack success or failure
         if (engagement->GetAttackSuccess())
         {
            aReturnVal.SetInt(cSUCCEEDED);
         }
         else
         {
            aReturnVal.SetInt(cFAILED);
         }
      }
   }
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, LastCyberAttackTime, 2, "double", "WsfPlatform, string")
{
   auto&        cyberSim       = wsf::cyber::SimulationExtension::Get(*SIMULATION);
   WsfPlatform* victimPlatform = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto* engagement = cyberSim.GetCyberEngagementManager().FindEngagement(aVarArgs[1].GetString(),
                                                                          aObjectPtr->GetNameId(),
                                                                          victimPlatform->GetNameId());

   if (!engagement)
   {
      aReturnVal.SetDouble(UtMath::cDOUBLE_MAX);
   }
   else
   {
      aReturnVal.SetDouble(engagement->GetAttackStartTime());
   }
}

// ----------------------------------------------------------------------------
//                            Cyber Scan Platform Methods
// ----------------------------------------------------------------------------

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, CyberScan, 2, "bool", "WsfPlatform, string")
{
   WsfPlatform* victimPlatform = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto& cyberSim  = wsf::cyber::SimulationExtension::Get(*SIMULATION);
   bool  returnVal = cyberSim.GetCyberEngagementManager().CyberScan(aVarArgs[1].GetString(),
                                                                   aObjectPtr->GetNameId(),
                                                                   victimPlatform->GetNameId(),
                                                                   *SIMULATION);

   aReturnVal.SetBool(returnVal);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, CyberScanStatus, 2, "int", "WsfPlatform, string")
{
   auto&        cyberSim       = wsf::cyber::SimulationExtension::Get(*SIMULATION);
   WsfPlatform* victimPlatform = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto* engagement = cyberSim.GetCyberEngagementManager().FindEngagement(aVarArgs[1].GetString(),
                                                                          aObjectPtr->GetNameId(),
                                                                          victimPlatform->GetNameId());

   if (!engagement)
   {
      aReturnVal.SetInt(cFAILED);
   }
   else
   {
      // Check if the scan is still in progress
      double elapsedTime = SIMULATION->GetSimTime() - engagement->GetScanStartTime();
      if (elapsedTime < engagement->GetScanDelayTime())
      {
         aReturnVal.SetInt(cUNKNOWN);
      }
      else
      {
         // Scan was completed. Use scan success to know if detectable or not
         if (engagement->GetScanSuccess())
         {
            aReturnVal.SetInt(cSUCCEEDED);
         }
         else
         {
            aReturnVal.SetInt(cFAILED);
         }
      }
   }
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, LastCyberScanTime, 2, "double", "WsfPlatform, string")
{
   auto&        cyberSim       = wsf::cyber::SimulationExtension::Get(*SIMULATION);
   WsfPlatform* victimPlatform = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());

   auto* engagement = cyberSim.GetCyberEngagementManager().FindEngagement(aVarArgs[1].GetString(),
                                                                          aObjectPtr->GetNameId(),
                                                                          victimPlatform->GetNameId());

   if (!engagement)
   {
      aReturnVal.SetDouble(UtMath::cDOUBLE_MAX);
   }
   else
   {
      aReturnVal.SetDouble(engagement->GetScanStartTime());
   }
}
} // namespace PlatformScriptMethods

namespace SimulationScriptMethods
{
//! This version of cyber attack is a static method for attacks that may occur against a platform at various
//! scopes. The attacker and victim of this call are equivalent, as this attack is intended for embedded attacks
//! from a platform against itself.
UT_DEFINE_SCRIPT_METHOD_EXT(WsfSimulation, CyberAttack_1, 2, "bool", "WsfPlatform, string")
{
   auto targetPlatformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto platformNameId    = targetPlatformPtr->GetNameId();

   auto& cyberSim      = wsf::cyber::SimulationExtension::Get(*SIMULATION);
   auto& cyberScenario = wsf::cyber::ScenarioExtension::Get(*SCENARIO);
   if (cyberScenario.GetAttackTypeInputRequirement(aVarArgs[1].GetString(), Effect::InputRequirement::cREQUIRED))
   {
      UT_SCRIPT_ABORT("Call made with too few arguments - associated effect(s) require user provided variable(s).");
   }

   bool returnVal =
      cyberSim.GetCyberEngagementManager().CyberAttack(aVarArgs[1].GetString(), platformNameId, platformNameId, *SIMULATION);

   aReturnVal.SetBool(returnVal);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSimulation, CyberAttack_2, 3, "bool", "WsfPlatform, string, WsfCyberAttackParameters")
{
   auto targetPlatformPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   auto platformNameId    = targetPlatformPtr->GetNameId();
   auto attackName        = aVarArgs[1].GetString();

   auto& cyberSim      = wsf::cyber::SimulationExtension::Get(*SIMULATION);
   auto& cyberScenario = wsf::cyber::ScenarioExtension::Get(*SCENARIO);
   if ((!cyberScenario.GetAttackTypeInputRequirement(attackName, Effect::InputRequirement::cREQUIRED)) &&
       (!cyberScenario.GetAttackTypeInputRequirement(attackName, Effect::InputRequirement::cOPTIONAL)))
   {
      UT_SCRIPT_ABORT(
         "Call made with too many arguments - associated effect(s) do not require user provided variable.");
   }

   auto parametersPtr = aVarArgs[2].GetPointer()->GetAppObject<AttackParameters>();
   bool returnVal     = cyberSim.GetCyberEngagementManager().CyberAttack(aVarArgs[1].GetString(),
                                                                     platformNameId,
                                                                     platformNameId,
                                                                     *SIMULATION,
                                                                     parametersPtr);

   aReturnVal.SetBool(returnVal);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSimulation, CyberEngagementScanDelayStatus, 3, "int", "string, WsfPlatform, WsfPlatform")
{
   // Query the status (truth) of which delay phase any current cyber scan engagement is in. Provides users with a
   // script based capability to know the implications of canceling an ongoing engagement, which has implications based
   // on which delay is currently being processed.
   //
   // The return values are equivalent to the wsf::cyber::Event::Type enumerations.
   auto attackName  = aVarArgs[0].GetString();
   auto attackerPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfPlatform>();
   auto victimPtr   = aVarArgs[2].GetPointer()->GetAppObject<WsfPlatform>();

   auto type = DelayStatus(false, attackName, attackerPtr, victimPtr, *SIMULATION);

   aReturnVal.SetInt(type);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSimulation, CyberEngagementAttackDelayStatus, 3, "int", "string, WsfPlatform, WsfPlatform")
{
   // Query the status (truth) of which delay phase any current cyber attack engagement is in. Provides users with a
   // script based capability to know the implications of canceling an ongoing engagement, which has implications based
   // on which delay is currently being processed.
   //
   // The return values are equivalent to the wsf::cyber::Event::Type enumerations.
   auto attackName  = aVarArgs[0].GetString();
   auto attackerPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfPlatform>();
   auto victimPtr   = aVarArgs[2].GetPointer()->GetAppObject<WsfPlatform>();

   auto type = DelayStatus(true, attackName, attackerPtr, victimPtr, *SIMULATION);

   aReturnVal.SetInt(type);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSimulation, CyberScanCancel, 3, "bool", "string, WsfPlatform, WsfPlatform")
{
   // Cancel an ongoing cyber scan engagement. Returns false if the engagement does not have a valid ongoing delay,
   // or the variables provided by the user do not map to an existing engagement.
   auto attackName  = aVarArgs[0].GetString();
   auto attackerPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfPlatform>();
   auto victimPtr   = aVarArgs[2].GetPointer()->GetAppObject<WsfPlatform>();

   auto result = CancelEvent(false, attackName, attackerPtr, victimPtr, *SIMULATION);
   aReturnVal.SetBool(result);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSimulation, CyberAttackCancel, 3, "bool", "string, WsfPlatform, WsfPlatform")
{
   // Cancel an ongoing cyber attack engagement. Returns false if the engagement does not have a valid ongoing delay,
   // or the variables provided by the user do not map to an existing engagement.
   auto attackName  = aVarArgs[0].GetString();
   auto attackerPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfPlatform>();
   auto victimPtr   = aVarArgs[2].GetPointer()->GetAppObject<WsfPlatform>();

   auto result = CancelEvent(true, attackName, attackerPtr, victimPtr, *SIMULATION);
   aReturnVal.SetBool(result);
}
} // namespace SimulationScriptMethods

bool ScriptExtensions::AddExtClassMethods(const std::string& aClassName,
                                          const std::string& aBaseName,
                                          UtScriptTypes*     aScriptTypesPtr)
{
   if (aBaseName == aClassName)
   {
      //! Avoid registering twice. Accomplished via ExtendScriptClasses()
   }
   else if (aBaseName == "WsfPlatform")
   {
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<PlatformScriptMethods::Constraint>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<PlatformScriptMethods::CyberAttack_1>("CyberAttack"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<PlatformScriptMethods::CyberAttack_2>("CyberAttack"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<PlatformScriptMethods::CyberAttackStatus>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<PlatformScriptMethods::LastCyberAttackTime>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<PlatformScriptMethods::CyberScan>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<PlatformScriptMethods::CyberScanStatus>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<PlatformScriptMethods::LastCyberScanTime>());
   }
   else if (aBaseName == "WsfSimulation")
   {
      aScriptTypesPtr->AddStaticClassMethod(aClassName,
                                            ut::make_unique<SimulationScriptMethods::CyberAttack_1>("CyberAttack"));
      aScriptTypesPtr->AddStaticClassMethod(aClassName,
                                            ut::make_unique<SimulationScriptMethods::CyberAttack_2>("CyberAttack"));
      aScriptTypesPtr->AddStaticClassMethod(aClassName,
                                            ut::make_unique<SimulationScriptMethods::CyberEngagementScanDelayStatus>());
      aScriptTypesPtr->AddStaticClassMethod(aClassName,
                                            ut::make_unique<SimulationScriptMethods::CyberEngagementAttackDelayStatus>());
      aScriptTypesPtr->AddStaticClassMethod(aClassName, ut::make_unique<SimulationScriptMethods::CyberScanCancel>());
      aScriptTypesPtr->AddStaticClassMethod(aClassName, ut::make_unique<SimulationScriptMethods::CyberAttackCancel>());
   }
   else
   {
      return false;
   }

   return true;
}

void ScriptExtensions::ExtendScriptClasses(UtScriptTypes& aTypes)
{
   //! Register new script classes
   aTypes.Register(ut::make_unique<ScriptConstraintClass>("WsfCyberConstraint", &aTypes));
   aTypes.Register(ut::make_unique<ScriptAttackParameterObjectClass>("WsfCyberAttackParameterObject", &aTypes));
   aTypes.Register(ut::make_unique<ScriptAttackParametersClass>("WsfCyberAttackParameters", &aTypes));
   aTypes.Register(ut::make_unique<ScriptEngagement>("WsfCyberEngagement", &aTypes));
   aTypes.Register(ut::make_unique<ScriptMitmMessage>("WsfCyberMitmMessage", &aTypes));
   aTypes.Register(ut::make_unique<trigger::ScriptClass>("WsfCyberTrigger", &aTypes));
   aTypes.Register(ut::make_unique<trigger::CompositeScriptClass>("WsfCyberTriggerComposite", &aTypes));
   aTypes.Register(ut::make_unique<trigger::ScriptInterval>("WsfCyberTriggerInterval", &aTypes));

   //! Add new script methods to existing classes
   {
      using namespace PlatformScriptMethods;
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<PlatformScriptMethods::Constraint>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<CyberAttack_1>("CyberAttack"));
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<CyberAttack_2>("CyberAttack"));
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<CyberAttackStatus>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<LastCyberAttackTime>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<CyberScan>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<CyberScanStatus>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<LastCyberScanTime>());
   }

   {
      using namespace SimulationScriptMethods;
      aTypes.AddStaticClassMethod("WsfSimulation", ut::make_unique<CyberAttack_1>("CyberAttack"));
      aTypes.AddStaticClassMethod("WsfSimulation", ut::make_unique<CyberAttack_2>("CyberAttack"));
      aTypes.AddStaticClassMethod("WsfSimulation", ut::make_unique<CyberEngagementScanDelayStatus>());
      aTypes.AddStaticClassMethod("WsfSimulation", ut::make_unique<CyberEngagementAttackDelayStatus>());
      aTypes.AddStaticClassMethod("WsfSimulation", ut::make_unique<CyberScanCancel>());
      aTypes.AddStaticClassMethod("WsfSimulation", ut::make_unique<CyberAttackCancel>());
   }

   //! Register the extension
   aTypes.RegisterExtension(ut::make_unique<ScriptExtensions>());
}

} // namespace cyber
} // namespace wsf
