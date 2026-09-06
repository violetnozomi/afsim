// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "triggers/WsfCyberTrigger.hpp"

#include <cmath>
#include <iostream>

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfCyberSimulationExtension.hpp"
#include "WsfCyberTriggerEvent.hpp"
#include "WsfCyberTriggerManager.hpp"
#include "WsfCyberTriggerTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace cyber
{
namespace trigger
{

// =================================================================================================
Trigger::Trigger(WsfScenario& aScenario)
   : WsfObject()
   , WsfPlatformComponent()
   , mPlatformPtr(nullptr)
   , mDebug(false)
   , mExtrapolatedTrigger(false)
   , mTriggerActivated(false)
   , mShouldInstance(false)
   , mIntervals()
   , mContext(*aScenario.GetScriptContext(), "WsfCyberTrigger")
   , mScenarioPtr(&aScenario)
   , mOnEvaluate(nullptr)
   , mOnEvaluateGlobalName()
   , mOnExecute(nullptr)
   , mOnExecuteGlobalName()
   , mIdentifier()
{
}

// =================================================================================================
Trigger::Trigger(const Trigger& aSrc)
   : WsfObject(aSrc)
   , WsfPlatformComponent(aSrc)
   , mPlatformPtr(nullptr)
   , mDebug(aSrc.mDebug)
   , mExtrapolatedTrigger(aSrc.mExtrapolatedTrigger)
   , mTriggerActivated(aSrc.mTriggerActivated)
   , mShouldInstance(aSrc.mShouldInstance)
   , mIntervals(aSrc.mIntervals)
   , mContext(aSrc.mContext)
   , mScenarioPtr(aSrc.mScenarioPtr)
   , mOnEvaluate(aSrc.mOnEvaluate)
   , mOnEvaluateGlobalName(aSrc.mOnEvaluateGlobalName)
   , mOnExecute(aSrc.mOnExecute)
   , mOnExecuteGlobalName(aSrc.mOnExecuteGlobalName)
   , mIdentifier(aSrc.mIdentifier)
{
}

// =================================================================================================
Trigger& Trigger::operator=(const Trigger& aRhs)
{
   if (this != &aRhs)
   {
      WsfObject::           operator=(aRhs);
      WsfPlatformComponent::operator=(aRhs);
      mDebug                        = aRhs.mDebug;
      mExtrapolatedTrigger          = aRhs.mExtrapolatedTrigger;
      mTriggerActivated             = aRhs.mTriggerActivated;
      mShouldInstance               = aRhs.mShouldInstance;
      mIntervals                    = aRhs.mIntervals;
      mScenarioPtr                  = aRhs.mScenarioPtr;
      mContext                      = aRhs.mContext;
      mOnEvaluate                   = aRhs.mOnEvaluate;
      mOnEvaluateGlobalName         = aRhs.mOnEvaluateGlobalName;
      mOnExecute                    = aRhs.mOnExecute;
      mOnExecuteGlobalName          = aRhs.mOnExecuteGlobalName;
      mIdentifier                   = aRhs.mIdentifier;
   }

   return *this;
}

// =================================================================================================
Trigger* Trigger::Clone() const
{
   return new Trigger(*this);
}

// =================================================================================================
const int* Trigger::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_CYBER_TRIGGER, cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* Trigger::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_CYBER_TRIGGER)
   {
      return this;
   }

   return nullptr;
}

// =================================================================================================
bool Trigger::Initialize(double aSimTime)
{
   bool ok = true;

   ok &= mContext.Initialize(aSimTime, GetPlatform()->GetScriptContext(), this);

   for (auto& interval : mIntervals)
   {
      interval.Initialize();
   }

   //! Error if a platform not provided.
   if (!mPlatformPtr)
   {
      ok = false;
   }

   //! Local platform events are the only triggers initialized in this way.
   //! Thus, schedule these via an event if initialization is good.
   if (ok)
   {
      auto timeResult = NextIntervalTime(0.0);
      if (timeResult.first)
      {
         mPlatformPtr->GetSimulation()->AddEvent(
            ut::make_unique<PlatformEvent>(timeResult.second, GetPlatform()->GetName(), GetName()));
      }
   }

   return ok;
}

// =================================================================================================
bool Trigger::InitializeGlobal(double, WsfSimulation& aSim)
{
   bool ok = true;

   //! This is a global trigger. Use the global context.
   ok &= mContext.Initialize(aSim, this);

   for (auto& interval : mIntervals)
   {
      interval.Initialize();
   }

   return ok;
}

// =================================================================================================
bool Trigger::ProcessInput(UtInput& aInput)
{
   auto myCommand = true;
   auto command   = aInput.GetCommand();

   if (command == "debug")
   {
      mDebug = true;
   }
   else if (command == "extrapolated_trigger")
   {
      aInput.ReadValue(mExtrapolatedTrigger);
   }
   else if (command == "execute_trigger")
   {
      aInput.ReadValue(mShouldInstance);
   }
   else if (command == "update_interval")
   {
      UtInputBlock intervalBlock(aInput, "end_update_interval");
      Interval     interval;
      while (intervalBlock.ReadCommand())
      {
         if (!interval.ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      mIntervals.emplace_back(interval);
   }
   else if (command == "script")
   {
      UtScript* scriptPtr = mContext.Compile(aInput, "end_script", GetName());

      std::string scriptName     = scriptPtr->GetName();
      bool        validSignature = true;

      if (scriptName == "OnEvaluate" || scriptName == (GetName() + "::OnEvaluate"))
      {
         validSignature = mContext.ValidateScript(scriptPtr, "bool", "");
         mOnEvaluate    = scriptPtr;
      }
      else if (scriptName == "OnExecute" || scriptName == (GetName() + "::OnExecute"))
      {
         validSignature = mContext.ValidateScript(scriptPtr, "void", "");
         mOnExecute     = scriptPtr;
      }
      else
      {
         throw UtException("Unrecognized script method in " + GetName());
      }

      if (!validSignature)
      {
         throw UtException("Invalid script return type, argument type, or argument count in Trigger: " + GetName());
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
bool Trigger::SetOnEvaluate(const std::string& aScriptName)
{
   if (!mOnEvaluate)
   {
      mOnEvaluateGlobalName = aScriptName;
      return true;
   }

   return false;
}

// =================================================================================================
bool Trigger::SetOnExecute(const std::string& aScriptName)
{
   if (!mOnExecute)
   {
      mOnExecuteGlobalName = aScriptName;
      return true;
   }

   return false;
}

// =================================================================================================
std::pair<bool, double> Trigger::NextIntervalTime(double aSimTime) const
{
   auto returnPair = std::make_pair(false, std::numeric_limits<double>::max());

   for (auto& interval : mIntervals)
   {
      auto pairVal = interval.NextEvaluationTime(aSimTime);
      if (pairVal.first && (pairVal.second < returnPair.second))
      {
         returnPair.first  = true;
         returnPair.second = pairVal.second;
      }
   }

   if (!returnPair.first)
   {
      returnPair.second = 0.0;
   }

   return returnPair;
}

// =================================================================================================
bool Trigger::Evaluate(double aSimTime)
{
   if (mOnEvaluate || !mOnEvaluateGlobalName.empty())
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;

      if (mOnEvaluate)
      {
         mContext.ExecuteScript(aSimTime, mOnEvaluate, scriptRetVal, scriptArgs);
      }
      else
      {
         mContext.GetSimulation()->GetScriptContext().ExecuteScript(aSimTime, mOnEvaluateGlobalName, scriptRetVal, scriptArgs);
      }

      auto result = scriptRetVal.GetBool();
      WsfObserver::CyberTriggerEvaluation(mContext.GetSimulation())(aSimTime, *this, result);
      return result;
   }

   return false;
}

// =================================================================================================
void Trigger::Execute(double aSimTime)
{
   if (mOnExecute)
   {
      mContext.ExecuteScript(aSimTime, mOnExecute);
   }
   else if (!mOnExecuteGlobalName.empty())
   {
      mContext.GetSimulation()->GetScriptContext().ExecuteScript(aSimTime, mOnExecuteGlobalName);
   }

   WsfObserver::CyberTriggerExecution(mContext.GetSimulation())(aSimTime, *this);
   mTriggerActivated = true;
}

// =================================================================================================
bool Trigger::HasEvaluate() const
{
   return (mOnEvaluate || !mOnEvaluateGlobalName.empty());
}

// =================================================================================================
bool Trigger::HasExecute() const
{
   return (mOnExecute || !mOnExecuteGlobalName.empty());
}

// =================================================================================================
ScriptClass::ScriptClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptObjectClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCyberTrigger");

   mIsScriptAccessible = true;
   mConstructible      = true;
   mCloneable          = true;

   AddStaticMethod(ut::make_unique<GetTrigger_1>("GetTrigger"));
   AddStaticMethod(ut::make_unique<GetTrigger_2>("GetTrigger"));

   AddMethod(ut::make_unique<CreateDerivedTrigger>());
   AddMethod(ut::make_unique<AddGlobalTrigger>());
   AddMethod(ut::make_unique<AddPlatformTrigger>());

   AddMethod(ut::make_unique<IsExtrapolated>());
   AddMethod(ut::make_unique<HasTriggered>());
   AddMethod(ut::make_unique<IsExecutingTrigger>());
   AddMethod(ut::make_unique<NextIntervalTime_1>("NextIntervalTime"));
   AddMethod(ut::make_unique<NextIntervalTime_2>("NextIntervalTime"));
   AddMethod(ut::make_unique<GetIntervals>());
   AddMethod(ut::make_unique<GetIdentifier>());

   AddMethod(ut::make_unique<SetExtrapolated>());
   AddMethod(ut::make_unique<AddInterval>());
   AddMethod(ut::make_unique<ClearIntervals>());
   AddMethod(ut::make_unique<SetIdentifier>());

   AddMethod(ut::make_unique<SetOnEvaluate>());
   AddMethod(ut::make_unique<SetOnExecute>());
}

// =================================================================================================
void* ScriptClass::Create(const UtScriptContext& aInstance)
{
   auto triggerPtr = ut::make_unique<Trigger>(*WsfScriptContext::GetSCENARIO(aInstance));

   // Triggers created in the script context are always executing.
   triggerPtr->SetInstanced(true);
   return triggerPtr.release();
}

// =================================================================================================
void* ScriptClass::Clone(void* aObjectPtr)
{
   return static_cast<Trigger*>(aObjectPtr)->Clone();
}

// =================================================================================================
void ScriptClass::Destroy(void* aObjectPtr)
{
   delete static_cast<Trigger*>(aObjectPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, GetTrigger_1, 2, "WsfCyberTrigger", "string, string")
{
   Trigger* triggerPtr   = nullptr;
   auto     platformName = aVarArgs[0].GetString();
   auto     triggerName  = aVarArgs[1].GetString();
   auto     platformPtr  = SIMULATION->GetPlatformByName(platformName);

   if (platformPtr)
   {
      triggerPtr = platformPtr->GetComponent<Trigger>(triggerName);
      if (triggerPtr)
      {
         triggerPtr->SetIdentifier(triggerPtr->GetName());
      }
   }

   aReturnVal.SetPointer(UtScriptRef::Ref(triggerPtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, GetTrigger_2, 1, "WsfCyberTrigger", "string")
{
   auto  triggerName    = aVarArgs[0].GetString();
   auto& cyberSim       = SimulationExtension::Get(*SIMULATION);
   auto& triggerManager = cyberSim.GetCyberTriggerManager();

   auto triggerPtr = triggerManager.GetTrigger(triggerName);
   if (triggerPtr)
   {
      triggerPtr->SetIdentifier(triggerPtr->GetType());
   }
   aReturnVal.SetPointer(UtScriptRef::Ref(triggerPtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, CreateDerivedTrigger, 1, "WsfCyberTrigger", "string")
{
   auto triggerIdentifier = aVarArgs[0].GetString();
   auto trigger           = ut::clone(aObjectPtr);

   if (!triggerIdentifier.empty())
   {
      trigger->SetInstanced(true);
      trigger->SetIdentifier(triggerIdentifier);
   }

   aReturnVal.SetPointer(UtScriptRef::Ref(trigger.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, AddGlobalTrigger, 0, "bool", "")
{
   bool added = false;

   if (!aObjectPtr->GetIdentifier().empty())
   {
      auto& cyberSim       = SimulationExtension::Get(*SIMULATION);
      auto& triggerManager = cyberSim.GetCyberTriggerManager();
      auto  trigger        = ut::clone(aObjectPtr);
      trigger->SetType(aObjectPtr->GetIdentifier());

      added = triggerManager.AddManagedTrigger(TIME_NOW, std::move(trigger));
   }
   aReturnVal.SetBool(added);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, AddPlatformTrigger, 1, "bool", "string")
{
   bool added = false;

   if (!aObjectPtr->GetIdentifier().empty())
   {
      auto platformName = aVarArgs[0].GetString();
      auto platformPtr  = SIMULATION->GetPlatformByName(platformName);

      if (platformPtr)
      {
         auto trigger = ut::clone(aObjectPtr);
         trigger->SetType(trigger->GetName());
         trigger->SetName(aObjectPtr->GetIdentifier());

         added = platformPtr->AddComponent(trigger.get());
         if (added && trigger->Initialize(TIME_NOW))
         {
            trigger.release();
         }
         else
         {
            added = false;
            platformPtr->RemoveComponent(trigger.get());
         }
      }
   }
   aReturnVal.SetBool(added);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, IsExtrapolated, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsExtrapolated());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, HasTriggered, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasTriggered());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, IsExecutingTrigger, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->ShouldInstance());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, NextIntervalTime_1, 0, "double", "")
{
   auto result = aObjectPtr->NextIntervalTime(TIME_NOW);
   auto time   = -1.0;

   if (result.first)
   {
      time = result.second;
   }

   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, NextIntervalTime_2, 1, "double", "double")
{
   auto userTime = aVarArgs[0].GetDouble();
   auto result   = aObjectPtr->NextIntervalTime(userTime);
   auto time     = -1.0;

   if (result.first)
   {
      time = result.second;
   }

   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, GetIntervals, 0, "Array<WsfCyberTriggerInterval>", "")
{
   const auto& intervals = aObjectPtr->GetIntervals();
   auto        data      = ut::make_unique<std::vector<UtScriptData>>();

   for (const auto& interval : intervals)
   {
      data->emplace_back(ScriptInterval::Create(interval));
   }

   auto scriptRefPtr = ut::make_unique<UtScriptRef>(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(scriptRefPtr.release());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, GetIdentifier, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetIdentifier());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, SetExtrapolated, 1, "void", "bool")
{
   auto extrapolate = aVarArgs[0].GetBool();
   aObjectPtr->SetExtrapolated(extrapolate);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, AddInterval, 1, "void", "WsfCyberTriggerInterval")
{
   auto intervalPtr = aVarArgs[0].GetPointer()->GetAppObject<Interval>();
   aObjectPtr->AddInterval(*intervalPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, ClearIntervals, 0, "void", "")
{
   aObjectPtr->ClearIntervals();
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, SetIdentifier, 1, "void", "string")
{
   aObjectPtr->SetIdentifier(aVarArgs[0].GetString());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, SetOnEvaluate, 1, "bool", "string")
{
   auto methodName = aVarArgs[0].GetString();
   auto result     = aObjectPtr->SetOnEvaluate(methodName);

   aReturnVal.SetBool(result);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptClass, Trigger, SetOnExecute, 1, "bool", "string")
{
   auto methodName = aVarArgs[0].GetString();
   auto result     = aObjectPtr->SetOnExecute(methodName);

   aReturnVal.SetBool(result);
}

} // namespace trigger
} // namespace cyber
} // namespace wsf
