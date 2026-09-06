// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "script/WsfScriptProcessorClass.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "WsfAdvancedBehaviorTree.hpp"
#include "WsfAdvancedBehaviorTreeNode.hpp"
#include "WsfBehaviorTree.hpp"
#include "WsfBehaviorTreeNode.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptProcessor.hpp"
#include "script/WsfScriptStateMachineProcessor.hpp"

WsfScriptProcessorClass::WsfScriptProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptPlatformPartClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfProcessor");

   AddStaticMethod(ut::make_unique<IsA_TypeOf>());

   AddMethod(ut::make_unique<TurnOff>());
   AddMethod(ut::make_unique<TurnOn>());
   AddMethod(ut::make_unique<UpdateInterval>());
   AddMethod(ut::make_unique<SetUpdateInterval>());

   AddMethod(ut::make_unique<ScriptExists>());
   AddMethod(ut::make_unique<Execute_1>("Execute"));
   AddMethod(ut::make_unique<Execute_2>("Execute"));
   AddMethod(ut::make_unique<ExecuteAtTime_1>("ExecuteAtTime"));
   AddMethod(ut::make_unique<ExecuteAtTime_1>("ExecuteScriptAtTime")); // Old name
   AddMethod(ut::make_unique<ExecuteAtTime_2>("ExecuteAtTime"));
   AddMethod(ut::make_unique<ExecuteScript>());
   AddMethod(ut::make_unique<ExecuteScriptWithArgs>());
   AddMethod(ut::make_unique<SuppressMessage>());

   AddMethod(ut::make_unique<FindBehavior>());
   AddMethod(ut::make_unique<FindBehavior>("Behavior"));
   AddMethod(ut::make_unique<BehaviorsExecuted>());
   AddMethod(ut::make_unique<BehaviorCount>());
   AddMethod(ut::make_unique<BehaviorEntry>());
   AddMethod(ut::make_unique<BehaviorTreeRootNode>());

   AddMethod(ut::make_unique<FindAdvancedBehavior>());
   AddMethod(ut::make_unique<FindAdvancedBehavior>("AdvancedBehavior"));
   AddMethod(ut::make_unique<AdvancedBehaviorsExecuted>());
   AddMethod(ut::make_unique<AdvancedBehaviorCount>());
   AddMethod(ut::make_unique<AdvancedBehaviorEntry>());
   AddMethod(ut::make_unique<AdvancedBehaviorTree>());

   AddMethod(ut::make_unique<State>());
   AddMethod(ut::make_unique<StateAll>());
   AddMethod(ut::make_unique<SetState>());
   AddMethod(ut::make_unique<SetStateAll>());
   AddMethod(ut::make_unique<GetStates>());
   AddMethod(ut::make_unique<GetStateAdvancedBehaviorTree>());
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfScriptProcessorClass, IsA_TypeOf, 2, "bool", "string, string")
{
   bool          isA_TypeOf = false;
   WsfProcessor* objectPtr  = WsfProcessorTypes::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (objectPtr != nullptr)
   {
      isA_TypeOf = objectPtr->IsA_TypeOf(aVarArgs[1].GetString());
   }
   aReturnVal.SetBool(isA_TypeOf);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptProcessorClass, WsfProcessor, TurnOff, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      ok = SIMULATION->TurnPartOff(TIME_NOW, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptProcessorClass, WsfProcessor, TurnOn, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      ok = SIMULATION->TurnPartOn(TIME_NOW, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, UpdateInterval, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetUpdateInterval());
}

//! SetUpdateInterval(double aUpdateInterval);
UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, SetUpdateInterval, 1, "void", "double")
{
   SIMULATION->SetPartUpdateInterval(TIME_NOW, aObjectPtr, aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, ScriptExists, 1, "bool", "string")
{
   // Argument 1: script name
   bool                exists       = false;
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      exists = (processorPtr->GetScriptContext().FindScript(aVarArgs[0].GetString()) != nullptr);
   }
   aReturnVal.SetBool(exists);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, Execute_1, 1, "Object", "string")
{
   // Argument 1: script name
   UtScriptData        rv(0);
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      if (!processorPtr->ExecuteScript(TIME_NOW, aVarArgs[0].GetString(), rv, UtScriptDataList()))
      {
         auto out = ut::log::error() << "Could not execute script.";
         out.AddNote() << "Script: " << aVarArgs[0].GetString();
      }
   }
   aReturnVal = rv;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, Execute_2, 2, "Object", "string, Array<Object>")
{
   // Argument 1: script name
   // Argument 2: the argument array.
   UtScriptData        rv(0);
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      UtScriptDataList* argsPtr = (UtScriptDataList*)aVarArgs[1].GetPointer()->GetAppObject();
      if (!processorPtr->ExecuteScript(TIME_NOW, aVarArgs[0].GetString(), rv, *argsPtr))
      {
         auto out = ut::log::error() << "Could not execute script.";
         out.AddNote() << "Script: " << aVarArgs[0].GetString();
      }
   }
   aReturnVal = rv;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, ExecuteAtTime_1, 2, "bool", "double, string")
{
   // Argument 1: time to execute script
   // Argument 2: script name
   bool ok           = false;
   auto processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      if (processorPtr->GetScriptContext().FindScript(aVarArgs[1].GetString()) != nullptr)
      {
         double time = aVarArgs[0].GetDouble();
         processorPtr->ExecuteScriptAtTime(time, aVarArgs[1].GetString(), UtScriptDataList());
         ok = true;
      }
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, ExecuteAtTime_2, 3, "bool", "double, string, Array<Object>")
{
   // Argument 1: time to execute script
   // Argument 2: script name
   // Argument 3: the argument array.
   bool ok           = false;
   auto processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      if (processorPtr->GetScriptContext().FindScript(aVarArgs[1].GetString()) != nullptr)
      {
         double time    = aVarArgs[0].GetDouble();
         auto   argsPtr = aVarArgs[2].GetPointer()->GetAppObject<UtScriptDataList>();
         processorPtr->ExecuteScriptAtTime(time, aVarArgs[1].GetString(), *argsPtr);
         ok = true;
      }
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, ExecuteScript, 1, "bool", "string")
{
   // Argument 1: script name
   bool                ok           = false;
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      double simTime = WsfScriptContext::GetTIME_NOW(aContext);
      ok             = processorPtr->ExecuteScript(simTime, aVarArgs[0].GetString());
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, ExecuteScriptWithArgs, 2, "bool", "string, Array<Object>")
{
   // Argument 1: script name
   // Argument 2: the argument array.
   bool                ok           = false;
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      UtScriptDataList* argsPtr = (UtScriptDataList*)aVarArgs[1].GetPointer()->GetAppObject();
      double            simTime = WsfScriptContext::GetTIME_NOW(aContext);
      UtScriptData      ret;
      ok = processorPtr->ExecuteScript(simTime, aVarArgs[0].GetString(), ret, *argsPtr);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, SuppressMessage, 0, "void", "")
{
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      processorPtr->SuppressMessage(true);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, FindBehavior, 1, "WsfBehaviorTreeNode", "string")
{
   WsfBehaviorTreeNode* nodePtr      = nullptr;
   WsfScriptProcessor*  processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      std::string            behaviorName = aVarArgs[0].GetString();
      const WsfBehaviorTree* treePtr      = processorPtr->BehaviorTree();
      if (treePtr != nullptr)
      {
         nodePtr = treePtr->FindNode(behaviorName);
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(nodePtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, BehaviorsExecuted, 0, "Array<WsfBehaviorTreeNode>", "")
{
   std::vector<WsfBehaviorTreeNode*> behaviors;
   WsfScriptProcessor*               processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      const WsfBehaviorTree* treePtr = processorPtr->BehaviorTree();
      if (treePtr != nullptr)
      {
         treePtr->FindLastExecuted(behaviors);
      }
   }
   UtScriptClass*             classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   std::vector<UtScriptData>* arrayPtr = new std::vector<UtScriptData>();
   for (unsigned int i = 0; i < behaviors.size(); ++i)
   {
      WsfBehaviorTreeNode* nodePtr = behaviors[i];
      arrayPtr->emplace_back(new UtScriptRef(nodePtr, classPtr, UtScriptRef::cDONT_MANAGE));
   }
   aReturnVal.SetPointer(new UtScriptRef(arrayPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, BehaviorCount, 0, "int", "")
{
   size_t              count        = 0;
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      const WsfBehaviorTree* treePtr = processorPtr->BehaviorTree();
      if (treePtr != nullptr)
      {
         count = treePtr->NodeCount();
      }
   }
   aReturnVal.SetInt(static_cast<int>(count));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, BehaviorEntry, 1, "WsfBehaviorTreeNode", "int")
{
   WsfBehaviorTreeNode* nodePtr      = nullptr;
   WsfScriptProcessor*  processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      const WsfBehaviorTree* treePtr = processorPtr->BehaviorTree();
      if (treePtr != nullptr)
      {
         size_t behaviorIndex = aVarArgs[0].GetInt();
         nodePtr              = treePtr->NodeEntry(behaviorIndex);
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(nodePtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, BehaviorTreeRootNode, 0, "WsfBehaviorTreeNode", "")
{
   WsfBehaviorTreeNode* nodePtr      = nullptr;
   WsfScriptProcessor*  processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      const WsfBehaviorTree* treePtr = processorPtr->BehaviorTree();
      if (treePtr != nullptr)
      {
         nodePtr = treePtr->RootNode();
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(nodePtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, FindAdvancedBehavior, 1, "WsfAdvancedBehaviorTreeNode", "string")
{
   WsfAdvancedBehaviorTreeNode* nodePtr      = nullptr;
   WsfScriptProcessor*          processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      const WsfAdvancedBehaviorTree* treePtr = processorPtr->AdvancedBehaviorTree();
      if (treePtr != nullptr)
      {
         std::string behaviorName = aVarArgs[0].GetString();
         nodePtr                  = treePtr->FindNode(behaviorName);
      }
   }
   aReturnVal.SetPointer(UtScriptRef::Ref(nodePtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass,
                        WsfProcessor,
                        AdvancedBehaviorsExecuted,
                        0,
                        "Array<WsfAdvancedBehaviorTreeNode>",
                        "")
{
   std::vector<WsfAdvancedBehaviorTreeNode*> behaviors;
   WsfScriptProcessor*                       processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      const WsfAdvancedBehaviorTree* treePtr = processorPtr->AdvancedBehaviorTree();
      if (treePtr != nullptr)
      {
         treePtr->FindLastExecuted(behaviors);
      }
   }
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   auto           arrayPtr = ut::make_unique<ut::script::DataList>();
   for (auto nodePtr : behaviors)
   {
      arrayPtr->emplace_back(UtScriptRef::Ref(nodePtr, classPtr, UtScriptRef::cDONT_MANAGE));
   }
   aReturnVal.SetPointer(new UtScriptRef(arrayPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, AdvancedBehaviorCount, 0, "int", "")
{
   size_t              count        = 0;
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      const WsfAdvancedBehaviorTree* treePtr = processorPtr->AdvancedBehaviorTree();
      if (treePtr != nullptr)
      {
         count = treePtr->NodeCount();
      }
   }
   aReturnVal.SetInt(static_cast<int>(count));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, AdvancedBehaviorEntry, 1, "WsfAdvancedBehaviorTreeNode", "int")
{
   WsfAdvancedBehaviorTreeNode*    nodePtr                  = nullptr;
   WsfScriptStateMachineProcessor* stateMachineProcessorPtr = dynamic_cast<WsfScriptStateMachineProcessor*>(aObjectPtr);
   if (stateMachineProcessorPtr != nullptr)
   {
      const WsfAdvancedBehaviorTree* treePtr = stateMachineProcessorPtr->AdvancedBehaviorTree();
      if (treePtr != nullptr)
      {
         auto behaviorIndex = ut::cast_to_size_t(aVarArgs[0].GetInt());
         nodePtr            = treePtr->NodeEntry(behaviorIndex);
      }
   }
   else
   {
      WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
      if (processorPtr != nullptr)
      {
         const WsfAdvancedBehaviorTree* treePtr = processorPtr->AdvancedBehaviorTree();
         if (treePtr != nullptr)
         {
            auto behaviorIndex = ut::cast_to_size_t(aVarArgs[0].GetInt());
            nodePtr            = treePtr->NodeEntry(behaviorIndex);
         }
      }
   }

   aReturnVal.SetPointer(UtScriptRef::Ref(nodePtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, AdvancedBehaviorTree, 0, "WsfAdvancedBehaviorTree", "")
{
   WsfAdvancedBehaviorTree* treePtr      = nullptr;
   WsfScriptProcessor*      processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      treePtr = processorPtr->AdvancedBehaviorTree();
   }
   aReturnVal.SetPointer(UtScriptRef::Ref(treePtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, State, 0, "string", "")
{
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   std::string         stateName    = processorPtr->State();
   aReturnVal.SetString(stateName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, StateAll, 0, "string", "")
{
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   std::string         stateNameAll = processorPtr->StateAll();
   aReturnVal.SetString(stateNameAll);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, SetState, 1, "void", "string")
{
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   std::string         stateName    = aVarArgs[0].GetString();
   processorPtr->SetState(stateName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, SetStateAll, 1, "void", "string")
{
   WsfScriptProcessor* processorPtr = dynamic_cast<WsfScriptProcessor*>(aObjectPtr);
   std::string         stateNameAll = aVarArgs[0].GetString();
   processorPtr->SetStateAll(stateNameAll);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, GetStates, 0, "Array<string>", "")
{
   auto                            stateNames               = ut::make_unique<std::vector<UtScriptData>>();
   WsfScriptStateMachineProcessor* stateMachineProcessorPtr = dynamic_cast<WsfScriptStateMachineProcessor*>(aObjectPtr);
   if (stateMachineProcessorPtr != nullptr)
   {
      const WsfScriptStateMachine* stateMachinePtr = stateMachineProcessorPtr->GetStateMachine();
      if (stateMachinePtr != nullptr)
      {
         for (int i = stateMachinePtr->GetInitialStateIndex(); i <= stateMachinePtr->GetStateCount(); ++i)
         {
            stateNames->emplace_back(stateMachinePtr->GetState(i)->GetStateName().GetString());
         }
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(stateNames.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptProcessorClass, WsfProcessor, GetStateAdvancedBehaviorTree, 1, "WsfAdvancedBehaviorTree", "string")
{
   WsfAdvancedBehaviorTree*        treePtr                  = nullptr;
   WsfScriptStateMachineProcessor* stateMachineProcessorPtr = dynamic_cast<WsfScriptStateMachineProcessor*>(aObjectPtr);
   if (stateMachineProcessorPtr != nullptr)
   {
      const WsfScriptStateMachine* stateMachinePtr = stateMachineProcessorPtr->GetStateMachine();
      if (stateMachinePtr != nullptr)
      {
         WsfScriptStateMachine::State* state =
            stateMachinePtr->GetState(stateMachinePtr->GetStateIndex(aVarArgs[0].GetString()));
         if (state != nullptr)
         {
            WsfAdvancedBehaviorTree* abt = state->AdvancedBehaviorTree();
            if (abt != nullptr)
            {
               treePtr = abt;
            }
         }
      }
   }
   aReturnVal.SetPointer(UtScriptRef::Ref(treePtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}
