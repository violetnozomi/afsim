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

#include "WsfCyberCompositeTrigger.hpp"

#include <iostream>
#include <numeric>

#include "UtInputBlock.hpp"
#include "WsfCyberSimulationExtension.hpp"
#include "WsfCyberTriggerManager.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace cyber
{
namespace trigger
{

// =================================================================================================
CompositeTrigger::CompositeTrigger(WsfScenario& aScenario, TriggerFactory aFactory)
   : Trigger(aScenario)
   , mFactory{std::move(aFactory)}
{
}

// =================================================================================================
CompositeTrigger::CompositeTrigger(const CompositeTrigger& aSrc)
   : Trigger(aSrc)
   , mPolicy(aSrc.mPolicy)
   , mFactory{aSrc.mFactory}
{
   for (const auto& trigger : aSrc.mTriggers)
   {
      mTriggers.emplace_back(trigger);
   }
}

// =================================================================================================
CompositeTrigger* CompositeTrigger::Clone() const
{
   return new CompositeTrigger(*this);
}

// =================================================================================================
CompositeTrigger* CompositeTrigger::CloneComponent() const
{
   return Clone();
}

namespace
{
template<class Iterable, class T, class BinaryOperation>
T accumulate(const Iterable& aIterable, T aInitVal, BinaryOperation aOp)
{
   return std::accumulate(std::begin(aIterable), std::end(aIterable), aInitVal, aOp);
}
} // namespace

// =================================================================================================
bool CompositeTrigger::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   auto command   = aInput.GetCommand();

   if (command == "triggers")
   {
      UtInputBlock triggersBlock(aInput, "end_triggers");
      while (triggersBlock.ReadCommand())
      {
         auto triggerCommand = aInput.GetCommand();
         if (triggerCommand == "not")
         {
            std::string triggerName;
            aInput.ReadValue(triggerName);
            mTriggers.emplace_back(triggerName, true);
         }
         else
         {
            mTriggers.emplace_back(triggerCommand, false);
         }
      }
   }
   else if (command == "policy")
   {
      std::string policy;
      aInput.ReadValue(policy);
      if (policy == "and")
      {
         mPolicy = Policy::cAND;
      }
      else if (policy == "or")
      {
         mPolicy = Policy::cOR;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown policy type in cyber trigger");
      }
   }
   else
   {
      myCommand = Trigger::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
bool CompositeTrigger::Initialize(double aSimTime)
{
   bool ok = Trigger::Initialize(aSimTime);
   for (auto& triggerInput : mTriggers)
   {
      auto trigger = mFactory(triggerInput.GetTriggerInput());

      if (!trigger)
      {
         auto logError = ut::log::error() << "Undefined cyber trigger type passed to WSF_CYBER_COMPOSITE_TRIGGER.";
         logError.AddNote() << "WSF_CYBER_COMPOSITE_TRIGGER: " << triggerInput.GetTriggerInput();
         ok = false;
      }

      triggerInput.SetTrigger(std::move(trigger));
      ok &= triggerInput->InitializeGlobal(aSimTime, *(GetPlatform()->GetSimulation()));
   }
   return ok;
}

// =================================================================================================
bool CompositeTrigger::InitializeGlobal(double aSimTime, WsfSimulation& aSim)
{
   bool ok = Trigger::InitializeGlobal(aSimTime, aSim);
   for (auto& triggerInput : mTriggers)
   {
      auto trigger = mFactory(triggerInput.GetTriggerInput());
      if (!trigger)
      {
         auto logError = ut::log::error() << "Undefined cyber trigger type passed to WSF_CYBER_COMPOSITE_TRIGGER.";
         logError.AddNote() << "WSF_CYBER_COMPOSITE_TRIGGER: " << triggerInput.GetTriggerInput();
         ok = false;
      }

      triggerInput.SetTrigger(std::move(trigger));
      ok &= triggerInput->InitializeGlobal(aSimTime, aSim);
   }
   return ok;
}

// =================================================================================================
bool CompositeTrigger::Evaluate(double aSimTime)
{
   auto result = false;
   auto init   = Trigger::Evaluate(aSimTime);

   if (mPolicy == Policy::cAND)
   {
      // To avoid short-circuiting, ignore the evaluate result if it has not been specified by
      // the user in the composite trigger itself. We don't need to do this for the OR policy,
      // as the return value when the user has not specified the evaluate method in the
      // composite trigger will be false (and the triggers will evaluate if appropriate).
      if (!HasEvaluate() && !init)
      {
         init = !mTriggers.empty();
      }

      result = accumulate(mTriggers,
                          init,
                          [aSimTime](bool aVal, const TriggerWrapper& aTrigger)
                          { return aVal && aTrigger.Evaluate(aSimTime); });
   }
   if (mPolicy == Policy::cOR)
   {
      result = accumulate(mTriggers,
                          init,
                          [aSimTime](bool aVal, const TriggerWrapper& aTrigger)
                          { return aVal || aTrigger.Evaluate(aSimTime); });
   }

   // If this composite trigger has any evaluating triggers, or there was
   // an evaluation method on the composite trigger, notify.
   if (!mTriggers.empty() || HasEvaluate())
   {
      WsfObserver::CyberTriggerEvaluation(mContext.GetSimulation())(aSimTime, *this, result);
   }

   return result;
}

// =================================================================================================
bool CompositeTrigger::AddTrigger(double             aSimTime,
                                  const std::string& aTriggerName,
                                  WsfSimulation&     aSim,
                                  bool               aNOT) // = false
{
   if (!aTriggerName.empty())
   {
      auto trigger = mFactory(aTriggerName);
      if (trigger && trigger->InitializeGlobal(aSimTime, aSim))
      {
         mTriggers.emplace_back(aTriggerName, aNOT, std::move(trigger));
         return true;
      }
   }
   return false;
}

// =================================================================================================
bool CompositeTrigger::AddTrigger(double                   aSimTime,
                                  std::unique_ptr<Trigger> aTrigger,
                                  WsfSimulation&           aSim,
                                  bool                     aNOT) // = false
{
   // If the name or type have already been set, then this is a managed trigger.
   // This method does not support managed triggers.
   if (aTrigger && aTrigger->GetName().empty() && aTrigger->GetType().empty())
   {
      // It is required that the user provided an identifier to name the trigger.
      auto name = aTrigger->GetIdentifier();
      if (!name.empty())
      {
         aTrigger->SetType(name);
         if (aTrigger->InitializeGlobal(aSimTime, aSim))
         {
            mTriggers.emplace_back(name, aNOT, std::move(aTrigger));
            return true;
         }
      }
   }

   return false;
}

// =================================================================================================
bool CompositeTrigger::RemoveTrigger(size_t aIndex)
{
   if (aIndex < mTriggers.size())
   {
      auto it = std::begin(mTriggers);
      std::advance(it, aIndex);
      mTriggers.erase(it);
      return true;
   }

   return false;
}

// =================================================================================================
bool CompositeTrigger::RemoveTrigger(const std::string& aTriggerName,
                                     bool               aNOT) // = false
{
   if (!aTriggerName.empty())
   {
      auto triggerIt =
         std::find_if(std::begin(mTriggers),
                      std::end(mTriggers),
                      [aTriggerName, aNOT](const TriggerWrapper& aTriggerWrapper)
                      { return ((aTriggerName == aTriggerWrapper.GetName()) && (aNOT == aTriggerWrapper.IsNOT())); });

      if (triggerIt != std::end(mTriggers))
      {
         mTriggers.erase(triggerIt);
         return true;
      }
   }
   return false;
}

// =================================================================================================
void CompositeTrigger::ClearTriggers()
{
   mTriggers.clear();
}

// =================================================================================================
std::string CompositeTrigger::TriggerWrapper::GetName() const
{
   std::string name = mTrigger->GetName();
   if (name.empty())
   {
      name = mTrigger->GetType();
   }

   return name;
}

// =================================================================================================
CompositeScriptClass::CompositeScriptClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : ScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCyberTriggerComposite");

   AddMethod(ut::make_unique<IsPolicyAND>());
   AddMethod(ut::make_unique<IsPolicyOR>());
   AddMethod(ut::make_unique<SetPolicyAND>());
   AddMethod(ut::make_unique<SetPolicyOR>());

   AddMethod(ut::make_unique<GetTriggersSize>());
   AddMethod(ut::make_unique<GetTriggerEntry>());
   AddMethod(ut::make_unique<IsTriggerEntryNOT>());

   AddMethod(ut::make_unique<AddTrigger_1>("AddTrigger"));
   AddMethod(ut::make_unique<AddTrigger_2>("AddTrigger"));
   AddMethod(ut::make_unique<RemoveTriggerEntry>());
   AddMethod(ut::make_unique<EraseTriggers>());
   AddMethod(ut::make_unique<SetTriggerEntryNOT>());
}

// =================================================================================================
void* CompositeScriptClass::Create(const UtScriptContext& aInstance)
{
   auto triggerPtr =
      ut::make_unique<CompositeTrigger>(*WsfScriptContext::GetSCENARIO(aInstance),
                                        [&](const std::string& aTriggerName) -> std::unique_ptr<trigger::Trigger>
                                        {
                                           // Since this is the script context, attempt to find the trigger from the
                                           // Trigger Manager.
                                           auto& cyberSim =
                                              SimulationExtension::Get(*WsfScriptContext::GetSIMULATION(aInstance));
                                           auto& triggerManager = cyberSim.GetCyberTriggerManager();
                                           auto* trigger        = triggerManager.GetTrigger(aTriggerName);
                                           if (trigger)
                                           {
                                              return ut::clone(trigger);
                                           }
                                           return nullptr;
                                        });

   // Triggers created in the script context are always executing.
   triggerPtr->SetInstanced(true);
   return triggerPtr.release();
}

// =================================================================================================
void* CompositeScriptClass::Clone(void* aObjectPtr)
{
   return static_cast<CompositeTrigger*>(aObjectPtr)->Clone();
}

// =================================================================================================
void CompositeScriptClass::Destroy(void* aObjectPtr)
{
   delete static_cast<CompositeTrigger*>(aObjectPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, IsPolicyAND, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsPolicyAND());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, IsPolicyOR, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsPolicyOR());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, SetPolicyAND, 0, "void", "")
{
   aObjectPtr->SetPolicy(CompositeTrigger::Policy::cAND);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, SetPolicyOR, 0, "void", "")
{
   aObjectPtr->SetPolicy(CompositeTrigger::Policy::cOR);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, GetTriggersSize, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->NumTriggers()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, GetTriggerEntry, 1, "WsfCyberTrigger", "int")
{
   auto index = ut::cast_to_size_t(aVarArgs[0].GetInt());
   aReturnVal.SetPointer(UtScriptRef::Ref(aObjectPtr->GetTrigger(index), aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, IsTriggerEntryNOT, 1, "bool", "int")
{
   auto index = ut::cast_to_size_t(aVarArgs[0].GetInt());
   aReturnVal.SetBool(aObjectPtr->IsTriggerNOT(index));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, AddTrigger_1, 2, "bool", "string, bool")
{
   auto triggerName = aVarArgs[0].GetString();
   auto isNOT       = aVarArgs[1].GetBool();
   bool ok          = false;

   if (!triggerName.empty())
   {
      ok = aObjectPtr->AddTrigger(TIME_NOW, triggerName, *SIMULATION, isNOT);
   }

   aReturnVal.SetBool(ok);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, AddTrigger_2, 2, "bool", "WsfCyberTrigger, bool")
{
   auto added   = false;
   auto trigger = aVarArgs[0].GetPointer()->GetAppObject<Trigger>();
   auto isNOT   = aVarArgs[1].GetBool();

   if (trigger)
   {
      added = aObjectPtr->AddTrigger(TIME_NOW, ut::clone(trigger), *SIMULATION, isNOT);
   }

   aReturnVal.SetBool(added);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, RemoveTriggerEntry, 1, "bool", "int")
{
   auto index = ut::cast_to_size_t(aVarArgs[0].GetInt());
   aReturnVal.SetBool(aObjectPtr->RemoveTrigger(index));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, EraseTriggers, 0, "void", "")
{
   aObjectPtr->ClearTriggers();
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(CompositeScriptClass, CompositeTrigger, SetTriggerEntryNOT, 2, "void", "int, bool")
{
   auto index = ut::cast_to_size_t(aVarArgs[0].GetInt());
   auto isNOT = aVarArgs[1].GetBool();
   aObjectPtr->SetTriggerNOT(index, isNOT);
}

} // namespace trigger
} // namespace cyber
} // namespace wsf
