// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfAdvancedBehaviorTreeNode.hpp"

#include <numeric>
#include <string>

#include <sys/stat.h>

#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtRandom.hpp"
#include "WsfAdvancedBehaviorTree.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfEventPipe.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMessage.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageHandler.hpp"
#include "script/WsfScriptProcessor.hpp"
#include "script/WsfScriptStateMachine.hpp"

unsigned int WsfAdvancedBehaviorTreeNode::mNextUniqueId = 0;

WsfScriptAdvancedBehaviorTreeNodeClass::WsfScriptAdvancedBehaviorTreeNodeClass(const std::string& aClassName,
                                                                               UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfAdvancedBehaviorTreeNode");
   AddMethod(ut::make_unique<Type>());
   AddMethod(ut::make_unique<Id>());
   AddMethod(ut::make_unique<Running_1>("Running"));
   AddMethod(ut::make_unique<Running_2>("Running"));
   AddMethod(ut::make_unique<Success_1>("Success"));
   AddMethod(ut::make_unique<Success_2>("Success"));
   AddMethod(ut::make_unique<Failure_1>("Failure"));
   AddMethod(ut::make_unique<Failure_2>("Failure"));
   AddMethod(ut::make_unique<TurnOff>());
   AddMethod(ut::make_unique<TurnOn>());
   AddMethod(ut::make_unique<State>());
   AddMethod(ut::make_unique<Tree>());

   // Blackboard methods
   AddMethod(ut::make_unique<GetBlackboardVar>("GetBlackboardVar"));

   AddMethod(ut::make_unique<SetBlackboardVar>("SetBlackboardVar"));
   AddMethod(ut::make_unique<SetBlackboardTrackVar>("SetBlackboardVar"));
   AddMethod(ut::make_unique<SetBlackboardPlatformVar>("SetBlackboardVar"));

   AddMethod(ut::make_unique<DeleteBlackboardVar>("DeleteBlackboardVar"));
   AddMethod(ut::make_unique<BlackboardVarExists>("BlackboardVarExists"));

   // Shared Blackboard methods
   AddMethod(ut::make_unique<ShareBlackboard>("ShareBlackboard"));
   AddMethod(ut::make_unique<ShareBlackboardWithName>("ShareBlackboard"));
   AddMethod(ut::make_unique<CreateSharedBlackboard>("CreateSharedBlackboard"));

   AddMethod(ut::make_unique<GetSharedBlackboardVar>("GetSharedBlackboardVar"));

   AddMethod(ut::make_unique<SetSharedBlackboardVar>("SetSharedBlackboardVar"));
   AddMethod(ut::make_unique<SetSharedBlackboardTrackVar>("SetSharedBlackboardVar"));
   AddMethod(ut::make_unique<SetSharedBlackboardPlatformVar>("SetSharedBlackboardVar"));

   AddMethod(ut::make_unique<DeleteSharedBlackboardVar>("DeleteSharedBlackboardVar"));
   AddMethod(ut::make_unique<SharedBlackboardExists>("SharedBlackboardExists"));
   AddMethod(ut::make_unique<SharedBlackboardVarExists>("SharedBlackboardVarExists"));
}

UtScriptContext* WsfScriptAdvancedBehaviorTreeNodeClass::GetContext(void* aObjectPtr)
{
   UtScriptContext* contextPtr = nullptr;
   auto             nodePtr    = static_cast<WsfAdvancedBehaviorTreeNode*>(aObjectPtr);
   if (nodePtr != nullptr)
   {
      contextPtr = &(nodePtr->GetScriptContext().GetContext());
   }
   return contextPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, BlackboardVarExists, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->GetBlackboard()->AttributeExists(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        CreateSharedBlackboard,
                        1,
                        "void",
                        "string")
{
   if (!aObjectPtr->GetTopRootNode()->GetOwningTree()->SharedBlackboardExists(aVarArgs[0].GetString()))
   {
      aObjectPtr->GetTopRootNode()->GetOwningTree()->AddBlackboard(aVarArgs[0].GetString(),
                                                                   std::make_shared<WsfAttributeContainer>());
   }
   else
   {
      ut::log::warning() << "CreateSharedBlackboard: Could not create the shared blackboard '"
                         << aVarArgs[0].GetString() << "', it already exists!";
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, DeleteBlackboardVar, 1, "void", "string")
{
   if (!aObjectPtr->GetBlackboard()->Delete(aVarArgs[0].GetString()))
   {
      ut::log::warning() << "DeleteBlackboardVar: Could not delete the blackboard var '" << aVarArgs[1].GetString()
                         << "', it doesn't exist!";
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        DeleteSharedBlackboardVar,
                        2,
                        "void",
                        "string, string")
{
   auto sharedBB = aObjectPtr->GetTopRootNode()->GetOwningTree()->GetSharedBlackboard(aVarArgs[0].GetString());
   if (sharedBB != nullptr)
   {
      if (!sharedBB->Delete(aVarArgs[1].GetString()))
      {
         ut::log::warning() << "DeleteSharedBlackboardVar: Could not delete var '" << aVarArgs[1].GetString()
                            << "' from shared blackboard '" << aVarArgs[0].GetString() << "', it doesn't exist!";
      }
   }
   else
   {
      ut::log::warning() << "DeleteSharedBlackboardVar: Could not delete var '" << aVarArgs[1].GetString()
                         << "', the shared blackboard '" << aVarArgs[0].GetString() << "' doesn't exist!";
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, Failure_1, 0, "int", "")
{
   aObjectPtr->SetTooltip("");
   aReturnVal.SetInt(3);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, Failure_2, 1, "int", "string")
{
   std::string tooltipText = aVarArgs[0].GetString();
   aObjectPtr->SetTooltip(tooltipText);
   aReturnVal.SetInt(3);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, GetBlackboardVar, 1, "Object", "string")
{
   if (aObjectPtr->GetBlackboard()->AttributeExists(aVarArgs[0].GetString()))
   {
      UtAttributeBase& attribute = aObjectPtr->GetBlackboard()->GetAttribute(aVarArgs[0].GetString());
      // Check primitive types first
      std::string valueType = attribute.GetType();
      if (valueType == "bool")
      {
         aReturnVal.SetBool(attribute.GetBool());
      }
      else if (valueType == "int")
      {
         aReturnVal.SetInt(attribute.GetInt());
      }
      else if (valueType == "string")
      {
         aReturnVal.SetString(attribute.GetString());
      }
      else if (valueType == "double")
      {
         aReturnVal.SetDouble(attribute.GetDouble());
      }
      else // Check pointer types
      {
         // Types we check for
         WsfTrack*    t = nullptr;
         WsfPlatform* p = nullptr;

         // If TryGet returns true t will not be nullptr
         if (attribute.TryGet<WsfTrack*>(t))
         {
            aReturnVal.SetPointer(new UtScriptRef(t, aContext.GetTypes()->GetClass("WsfTrack")));
         }
         else if (attribute.TryGet<WsfPlatform*>(p))
         {
            aReturnVal.SetPointer(new UtScriptRef(p, aContext.GetTypes()->GetClass("WsfPlatform")));
         }
         // Check more pointer types here...
      }
   }
   else
   {
      ut::log::warning() << "GetBlackboardVar: The blackboard var '" << aVarArgs[0].GetString() << "' doesn't exist!";
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        GetSharedBlackboardVar,
                        2,
                        "Object",
                        "string, string")
{
   auto sharedBB = aObjectPtr->GetTopRootNode()->GetOwningTree()->GetSharedBlackboard(aVarArgs[0].GetString());
   if (sharedBB != nullptr)
   {
      if (sharedBB->AttributeExists(aVarArgs[1].GetString()))
      {
         UtAttributeBase& attribute = sharedBB->GetAttribute(aVarArgs[1].GetString());

         // Check primitive types first
         std::string valueType = attribute.GetType();
         if (valueType == "bool")
         {
            aReturnVal.SetBool(attribute.GetBool());
         }
         else if (valueType == "int")
         {
            aReturnVal.SetInt(attribute.GetInt());
         }
         else if (valueType == "string")
         {
            aReturnVal.SetString(attribute.GetString());
         }
         else if (valueType == "double")
         {
            aReturnVal.SetDouble(attribute.GetDouble());
         }
         else // Check pointer types
         {
            // Types we check for
            WsfTrack*    t = nullptr;
            WsfPlatform* p = nullptr;

            // If TryGet returns true t will not be nullptr
            if (attribute.TryGet<WsfTrack*>(t))
            {
               aReturnVal.SetPointer(new UtScriptRef(t, aContext.GetTypes()->GetClass("WsfTrack")));
            }
            else if (attribute.TryGet<WsfPlatform*>(p))
            {
               aReturnVal.SetPointer(new UtScriptRef(p, aContext.GetTypes()->GetClass("WsfPlatform")));
            }
            // Check more pointer types here...
         }
      }
      else
      {
         ut::log::warning() << "GetSharedBlackboardVar: Could not find the var '" << aVarArgs[1].GetString()
                            << "' on the shared blackboard '" << aVarArgs[0].GetString() << "'.";
         aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
      }
   }
   else
   {
      ut::log::warning() << "GetSharedBlackboardVar: Could not find the var '" << aVarArgs[1].GetString()
                         << "', the shared blackboard '" << aVarArgs[0].GetString() << "' doesn't exist!";
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, Type, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, Id, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->Id());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, Running_1, 0, "int", "")
{
   aObjectPtr->SetTooltip("");
   aReturnVal.SetInt(1);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, Running_2, 1, "int", "string")
{
   std::string tooltipText = aVarArgs[0].GetString();
   aObjectPtr->SetTooltip(tooltipText);
   aReturnVal.SetInt(1);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, SetBlackboardVar, 2, "void", "string, Object")
{
   auto valueType = aVarArgs[1].GetType();
   switch (valueType)
   {
   case ut::script::Data::Type::cBOOL:
   {
      aObjectPtr->GetBlackboard()->Assign(aVarArgs[0].GetString(), aVarArgs[1].GetBool());
      break;
   }
   case ut::script::Data::Type::cINT:
   {
      aObjectPtr->GetBlackboard()->Assign(aVarArgs[0].GetString(), aVarArgs[1].GetInt());
      break;
   }
   case ut::script::Data::Type::cSTRING:
   {
      aObjectPtr->GetBlackboard()->Assign(aVarArgs[0].GetString(), aVarArgs[1].GetString());
      break;
   }
   case ut::script::Data::Type::cDOUBLE:
   {
      aObjectPtr->GetBlackboard()->Assign(aVarArgs[0].GetString(), aVarArgs[1].GetDouble());
      break;
   }
   default:
      break;
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        SetBlackboardTrackVar,
                        2,
                        "void",
                        "string, WsfTrack")
{
   auto trackPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfTrack>();
   aObjectPtr->GetBlackboard()->Assign(aVarArgs[0].GetString(), trackPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        SetBlackboardPlatformVar,
                        2,
                        "void",
                        "string, WsfPlatform")
{
   auto platformPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfPlatform>();
   aObjectPtr->GetBlackboard()->Assign(aVarArgs[0].GetString(), platformPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        SetSharedBlackboardVar,
                        3,
                        "void",
                        "string, string, Object")
{
   if (!aObjectPtr->GetTopRootNode()->GetOwningTree()->SharedBlackboardExists(aVarArgs[0].GetString()))
   {
      ut::log::warning() << "SetSharedBlackboardVar: Shared blackboard '" << aVarArgs[0].GetString()
                         << "' doesn't exist, creating it now...";

      aObjectPtr->GetTopRootNode()->GetOwningTree()->AddBlackboard(aVarArgs[0].GetString(),
                                                                   std::make_shared<WsfAttributeContainer>());
   }

   auto valueType = aVarArgs[2].GetType();
   switch (valueType)
   {
   case ut::script::Data::Type::cBOOL:
   {
      aObjectPtr->GetTopRootNode()
         ->GetOwningTree()
         ->GetSharedBlackboard(aVarArgs[0].GetString())
         ->Assign(aVarArgs[1].GetString(), aVarArgs[2].GetBool());
      break;
   }
   case ut::script::Data::Type::cINT:
   {
      aObjectPtr->GetTopRootNode()
         ->GetOwningTree()
         ->GetSharedBlackboard(aVarArgs[0].GetString())
         ->Assign(aVarArgs[1].GetString(), aVarArgs[2].GetInt());
      break;
   }
   case ut::script::Data::Type::cSTRING:
   {
      aObjectPtr->GetTopRootNode()
         ->GetOwningTree()
         ->GetSharedBlackboard(aVarArgs[0].GetString())
         ->Assign(aVarArgs[1].GetString(), aVarArgs[2].GetString());
      break;
   }
   case ut::script::Data::Type::cDOUBLE:
   {
      aObjectPtr->GetTopRootNode()
         ->GetOwningTree()
         ->GetSharedBlackboard(aVarArgs[0].GetString())
         ->Assign(aVarArgs[1].GetString(), aVarArgs[2].GetDouble());
      break;
   }
   default:
      break;
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        SetSharedBlackboardTrackVar,
                        3,
                        "void",
                        "string, string, WsfTrack")
{
   if (!aObjectPtr->GetTopRootNode()->GetOwningTree()->SharedBlackboardExists(aVarArgs[0].GetString()))
   {
      ut::log::warning() << "SetSharedBlackboardTrackVar: Shared blackboard '" << aVarArgs[0].GetString()
                         << "' doesn't exist, creating it now...";

      aObjectPtr->GetTopRootNode()->GetOwningTree()->AddBlackboard(aVarArgs[0].GetString(),
                                                                   std::make_shared<WsfAttributeContainer>());
   }

   auto trackPtr = aVarArgs[2].GetPointer()->GetAppObject<WsfTrack>();
   aObjectPtr->GetTopRootNode()
      ->GetOwningTree()
      ->GetSharedBlackboard(aVarArgs[0].GetString())
      ->Assign(aVarArgs[1].GetString(), trackPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        SetSharedBlackboardPlatformVar,
                        3,
                        "void",
                        "string, string, WsfPlatform")
{
   if (!aObjectPtr->GetTopRootNode()->GetOwningTree()->SharedBlackboardExists(aVarArgs[0].GetString()))
   {
      ut::log::warning() << "SetSharedBlackboardPlatformVar: Shared blackboard '" << aVarArgs[0].GetString()
                         << "' doesn't exist, creating it now...";

      aObjectPtr->GetTopRootNode()->GetOwningTree()->AddBlackboard(aVarArgs[0].GetString(),
                                                                   std::make_shared<WsfAttributeContainer>());
   }

   auto platformPtr = aVarArgs[2].GetPointer()->GetAppObject<WsfPlatform>();
   aObjectPtr->GetTopRootNode()
      ->GetOwningTree()
      ->GetSharedBlackboard(aVarArgs[0].GetString())
      ->Assign(aVarArgs[1].GetString(), platformPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, ShareBlackboard, 0, "void", "")
{
   auto nodeName = aObjectPtr->GetName();
   if (!aObjectPtr->GetTopRootNode()->GetOwningTree()->SharedBlackboardExists(nodeName))
   {
      aObjectPtr->GetTopRootNode()->GetOwningTree()->AddBlackboard(nodeName, std::move(aObjectPtr->GetBlackboard()));
   }
   else
   {
      ut::log::warning() << "ShareBlackboard: Could not share the blackboard '" << nodeName << "', it already exists!";
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        ShareBlackboardWithName,
                        1,
                        "void",
                        "string")
{
   if (!aObjectPtr->GetTopRootNode()->GetOwningTree()->SharedBlackboardExists(aVarArgs[0].GetString()))
   {
      aObjectPtr->GetTopRootNode()->GetOwningTree()->AddBlackboard(aVarArgs[0].GetString(),
                                                                   std::move(aObjectPtr->GetBlackboard()));
   }
   else
   {
      ut::log::warning() << "ShareBlackboard: Could not share the blackboard '" << aVarArgs[0].GetString()
                         << "', it already exists!";
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        SharedBlackboardExists,
                        1,
                        "bool",
                        "string")
{
   aReturnVal.SetBool(aObjectPtr->GetTopRootNode()->GetOwningTree()->SharedBlackboardExists(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass,
                        WsfAdvancedBehaviorTreeNode,
                        SharedBlackboardVarExists,
                        2,
                        "bool",
                        "string, string")
{
   auto sharedBB = aObjectPtr->GetTopRootNode()->GetOwningTree()->GetSharedBlackboard(aVarArgs[0].GetString());
   if (sharedBB != nullptr)
   {
      aReturnVal.SetBool(sharedBB->AttributeExists(aVarArgs[1].GetString()));
   }
   else
   {
      ut::log::warning() << "SharedBlackboardVarExists: Could not find shared blackboard var '" << aVarArgs[1].GetString()
                         << "', the shared blackboard '" << aVarArgs[0].GetString() << "' doesn't exist!";
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, Success_1, 0, "int", "")
{
   aObjectPtr->SetTooltip("");
   aReturnVal.SetInt(2);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, Success_2, 1, "int", "string")
{
   std::string tooltipText = aVarArgs[0].GetString();
   aObjectPtr->SetTooltip(tooltipText);
   aReturnVal.SetInt(2);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, TurnOff, 0, "void", "")
{
   aObjectPtr->TurnOff();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, TurnOn, 0, "void", "")
{
   aObjectPtr->TurnOn();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, State, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetNodeStatus()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeNodeClass, WsfAdvancedBehaviorTreeNode, Tree, 0, "WsfAdvancedBehaviorTree", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetOwningTree(), aReturnClassPtr));
}

WsfAdvancedBehaviorTreeNodeTypes::WsfAdvancedBehaviorTreeNodeTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfAdvancedBehaviorTreeNode>(aScenario, cSINGULAR_BASE_TYPE, "advanced_behavior")
{
   AddCoreType("WSF_ADVANCED_BEHAVIOR", ut::make_unique<WsfAdvancedBehaviorTreeActionNode>(aScenario));
   AddCoreType("WSF_ADVANCED_BEHAVIOR_CONDITION", ut::make_unique<WsfAdvancedBehaviorTreeConditionNode>(aScenario));
}

/** Load a new 'type object' from an input stream. Process the current command if it
 * is one that defines a new 'type object' of this class.
 */
WsfAdvancedBehaviorTreeNodeTypes::LoadResult WsfAdvancedBehaviorTreeNodeTypes::LoadType(UtInput& aInput)
{
   LoadResult result;
   if (aInput.GetCommand() == "advanced_behavior")
   {
      result.mIsCommandProcessed = true;

      std::string behaviorName;
      std::string behaviorType;
      aInput.ReadValue(behaviorName);
      aInput.ReadValue(behaviorType);

      aInput.PushBack(behaviorName);

      // Specifying the type is optional. If it isn't specified, WSF_BEHAVIOR is used.
      if (!IsType(behaviorType))
      {
         // In this case the behavior is a behavior command, so push it back on to be handled in 'ProcessInput'
         aInput.PushBack(behaviorType);

         // Use the default behavior type
         behaviorType = "WSF_ADVANCED_BEHAVIOR";
      }

      std::unique_ptr<WsfAdvancedBehaviorTreeNode> behaviorPtr(Clone(behaviorType));
      if (behaviorPtr != nullptr)
      {
         behaviorPtr->RegisterInput(aInput);
         behaviorPtr->SetType(behaviorType);
         behaviorPtr->ProcessInput(aInput);
         if (!Add(behaviorName, std::move(behaviorPtr)))
         {
            throw UtInput::BadValue(aInput, "Duplicate behavior name: " + behaviorName);
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown behavior type: " + behaviorType);
      }
   } // (aInput.GetCommand() == "advanced_behavior")
   else if (aInput.GetCommand() == "condition")
   {
      result.mIsCommandProcessed = true;

      std::string behaviorName;
      std::string behaviorType;
      aInput.ReadValue(behaviorName);
      aInput.ReadValue(behaviorType);

      aInput.PushBack(behaviorName);

      // Specifying the type is optional. If it isn't specified, WSF_BEHAVIOR is used.
      if (!IsType(behaviorType))
      {
         // In this case the behavior is a behavior command, so push it back on to be handled in 'ProcessInput'
         aInput.PushBack(behaviorType);

         // Use the default behavior type
         behaviorType = "WSF_ADVANCED_BEHAVIOR_CONDITION";
      }

      std::unique_ptr<WsfAdvancedBehaviorTreeNode> behaviorPtr(Clone(behaviorType));
      if (behaviorPtr != nullptr)
      {
         behaviorPtr->RegisterInput(aInput);
         behaviorPtr->SetType(behaviorType);
         behaviorPtr->ProcessInput(aInput);
         if (!Add(behaviorName, std::move(behaviorPtr)))
         {
            throw UtInput::BadValue(aInput, "Duplicate behavior name: " + behaviorName);
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown behavior type: " + behaviorType);
      }
   } // else if (aInput.GetCommand() == "condition")
   return result;
}


WsfAdvancedBehaviorTreeNode::WsfAdvancedBehaviorTreeNode(const WsfScenario& aScenario)
   : mScenario(aScenario)
   , mBlackboard(new WsfAttributeContainer())
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), "WsfAdvancedBehaviorTreeNode"))
   , mMessageHandlerPtr(nullptr)
   , mParentPtr(nullptr)
   , mParentTreePtr(nullptr)
   , mFiniteStateMachinePtr(nullptr)
   , mStateIndex(0)
   , mFileLastModTime()
   , mDebug(false)
   , mInitialized(false)
   , mTurnedOn(true)
   , mIsTreeRootNode(false)
   , mHasParent(false)
   , mExecutedLastTick(false)
   , mInPrecondition(false)
   , mReturnStatus(BT::cIDLE)
   , mFullPathFileName("")
   , mName("")
   , mDescription("")
   , mPreconditionTooltip()
   , mExecuteTooltip()
   , mPreconditionTooltipTimestamp(-1.0)
   , mExecuteTooltipTimestamp(-1.0)
   , mOnInitScriptPtr(nullptr)
   , mNewExecuteScriptPtr(nullptr)
   , mNewFailScriptPtr(nullptr)
   , mPreconditionScriptPtr(nullptr)
   , mExecuteScriptPtr(nullptr)
   , mColor(0.f, 0.f, 0.f, 0.f)
   , mHasDefinedColor(false)
{
   mId = GetUniqueNodeId();
   WsfObject::SetType(WsfStringId("undefined"));
   WsfObject::SetName(WsfStringId("undefined"));

   RegisterExternVariable("WsfPlatform", "PLATFORM");
   RegisterExternVariable("WsfProcessor", "PROCESSOR");
}


WsfAdvancedBehaviorTreeNode::WsfAdvancedBehaviorTreeNode(const WsfAdvancedBehaviorTreeNode& aSrc)
   : WsfObject(aSrc)
   , mScenario(aSrc.mScenario)
   , mBlackboard(new WsfAttributeContainer())
   , mContextPtr(new WsfScriptContext(*(aSrc.mContextPtr)))
   , mMessageHandlerPtr(nullptr)
   , mParentPtr(nullptr)
   , mParentTreePtr(nullptr)
   , mFiniteStateMachinePtr(nullptr)
   , // set below if aSrc has one to clone
   mStateIndex(aSrc.mStateIndex)
   , mFileLastModTime(aSrc.mFileLastModTime)
   , mDebug(aSrc.mDebug)
   , mInitialized(aSrc.mInitialized)
   , mTurnedOn(aSrc.mTurnedOn)
   , mIsTreeRootNode(aSrc.mIsTreeRootNode)
   , mHasParent(false)
   , mExecutedLastTick(false)
   , mInPrecondition(false)
   , mReturnStatus(aSrc.mReturnStatus)
   , mFullPathFileName(aSrc.mFullPathFileName)
   , mName(aSrc.mName)
   , mDescription(aSrc.mDescription)
   , mPreconditionTooltip(aSrc.mPreconditionTooltip)
   , mExecuteTooltip(aSrc.mExecuteTooltip)
   , mPreconditionTooltipTimestamp(aSrc.mPreconditionTooltipTimestamp)
   , mExecuteTooltipTimestamp(aSrc.mExecuteTooltipTimestamp)
   , mOnInitScriptPtr(nullptr)
   , mNewExecuteScriptPtr(nullptr)
   , mNewFailScriptPtr(nullptr)
   , mPreconditionScriptPtr(nullptr)
   , mExecuteScriptPtr(nullptr)
   , mColor(aSrc.mColor)
   , mHasDefinedColor(aSrc.mHasDefinedColor)
{
   mId = GetUniqueNodeId(); // each instance (copy) of a behavior node is unique

   mPreconditionScriptPtr = mContextPtr->FindScript("precondition");
   mNewExecuteScriptPtr   = mContextPtr->FindScript("on_new_execute");
   mNewFailScriptPtr      = mContextPtr->FindScript("on_new_fail");
   mOnInitScriptPtr       = mContextPtr->FindScript("on_init");
   mExecuteScriptPtr      = mContextPtr->FindScript("execute");

   // Copy state machine
   if (aSrc.mFiniteStateMachinePtr != nullptr)
   {
      mFiniteStateMachinePtr = new WsfScriptStateMachine(*aSrc.mFiniteStateMachinePtr, *mContextPtr);
   }

   // Copy message handler
   if (aSrc.mMessageHandlerPtr != nullptr)
   {
      mMessageHandlerPtr = new WsfScriptMessageHandler(*aSrc.mMessageHandlerPtr, *mContextPtr);
   }
}


WsfAdvancedBehaviorTreeNode::~WsfAdvancedBehaviorTreeNode()
{
   delete mFiniteStateMachinePtr;

   delete mContextPtr; // deleting this handles all script blocks compiled with it (init, execute, precondition, etc...)

   delete mMessageHandlerPtr; // TODO - delete before or after the context?
}

WsfAdvancedBehaviorTreeNode* WsfAdvancedBehaviorTreeNode::FindNode(const std::string& aNodeName)
{
   if (aNodeName == WsfObject::GetName())
   {
      return this;
   }

   if (HasChildrenContainer())
   {
      auto compNode = dynamic_cast<WsfAdvancedBehaviorTreeCompositeNode*>(this);
      if (compNode != nullptr)
      {
         for (auto i : compNode->GetChildren())
         {
            auto nodePtr = i->FindNode(aNodeName);
            if (nodePtr != nullptr)
            {
               return nodePtr;
            }
         }
      }
   }
   return nullptr;
}

void WsfAdvancedBehaviorTreeNode::FindLastExecuted(std::vector<WsfAdvancedBehaviorTreeNode*>& aLastExecuted)
{
   if (ExecutedLastTick() && !HasChildrenContainer())
   {
      auto leafNode = dynamic_cast<WsfAdvancedBehaviorTreeNode*>(this);
      if (leafNode != nullptr)
      {
         aLastExecuted.push_back(leafNode);
      }
   }

   if (HasChildrenContainer())
   {
      auto compNode = dynamic_cast<WsfAdvancedBehaviorTreeCompositeNode*>(this);
      if (compNode != nullptr)
      {
         for (auto i : compNode->GetChildren())
         {
            i->FindLastExecuted(aLastExecuted);
         }
      }
   }
}

WsfAdvancedBehaviorTreeNode* WsfAdvancedBehaviorTreeNode::GetTopRootNode()
{
   auto parentPtr = this;
   while (parentPtr->GetParent() != nullptr)
   {
      parentPtr = parentPtr->GetParent();
   }
   return parentPtr;
}

void WsfAdvancedBehaviorTreeNode::SetInitScript(UtScript* aScriptPtr)
{
   mOnInitScriptPtr = aScriptPtr;
}

bool WsfAdvancedBehaviorTreeNode::RegisterExternVariable(const std::string& aVariableType, const std::string& aVariableName)
{
   bool registered = false;
   if (mContextPtr != nullptr)
   {
      registered =
         (mContextPtr->GetContext().ModifyScope()->RegisterExternVariable(aVariableName, aVariableType) != nullptr);
   }
   return registered;
}

UtScriptContext* WsfAdvancedBehaviorTreeNode::GetScriptAccessibleContext() const
{
   return &mContextPtr->GetContext();
}

const char* WsfAdvancedBehaviorTreeNode::GetScriptClassName() const
{
   return "WsfAdvancedBehaviorTreeNode";
}

WsfSimulation* WsfAdvancedBehaviorTreeNode::GetSimulation() const
{
   const WsfPlatform* platformPtr = GetOwningPlatform();
   return (platformPtr != nullptr) ? platformPtr->GetSimulation() : mContextPtr->GetSimulation();
}

bool WsfAdvancedBehaviorTreeNode::Initialize(double aSimTime, WsfScriptContext* aParentContextPtr)
{
   mInitialized = true;
   mContextPtr->SetParent(aParentContextPtr); // set my parent (either the processor context, or another behavior context)

   // this class member context can get at "PROCESSOR" and "PLATFORM" variables from parent contexts
   mInitialized &= mContextPtr->Initialize(aSimTime, *aParentContextPtr, this); // initialize before all my children try to
   if (GetOwningPlatform() != nullptr && mMessageHandlerPtr != nullptr)
   {
      mInitialized &= mMessageHandlerPtr->Initialize(aSimTime, GetOwningPlatform());
   }

   if (mOnInitScriptPtr != nullptr)
   {
      mContextPtr->ExecuteScript(aSimTime, mOnInitScriptPtr);
   }

   if (mFiniteStateMachinePtr != nullptr)
   {
      mFiniteStateMachinePtr->SetProcessor(GetOwningProcessor());
      mFiniteStateMachinePtr->Initialize(aSimTime);
      mStateIndex = mFiniteStateMachinePtr->GetInitialStateIndex();
      mFiniteStateMachinePtr->EnterState(mStateIndex); // this forces the "on_entry" block to be ran for the initial state
   }

   if (!mTurnedOn && mReturnStatus != BT::cDISABLED)
   {
      TurnOff();
   }

   return mInitialized;
}

WsfPlatform* WsfAdvancedBehaviorTreeNode::GetOwningPlatform() const
{
   if (GetOwningTree() != nullptr)
   {
      return GetOwningTree()->GetOwningPlatform();
   }
   return nullptr;
}

WsfScriptProcessor* WsfAdvancedBehaviorTreeNode::GetOwningProcessor() const
{
   if (GetOwningTree() != nullptr)
   {
      return GetOwningTree()->GetParentProcessor();
   }
   return nullptr;
}

WsfAdvancedBehaviorTree* WsfAdvancedBehaviorTreeNode::GetOwningTree() const
{
   return mParentTreePtr;
}

const bool WsfAdvancedBehaviorTreeNode::PreconditionsMet() const
{
   mInPrecondition       = true;
   bool preconditionsMet = true;
   if (mPreconditionScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal(preconditionsMet);
      UtScriptDataList scriptArgs;
      mContextPtr->ExecuteScript(mPreconditionScriptPtr, scriptRetVal, scriptArgs);
      if (scriptRetVal.GetType() == ut::script::Data::Type::cDOUBLE)
      {
         preconditionsMet = (scriptRetVal.GetDouble() > 0);
      }
      else if (scriptRetVal.GetType() == ut::script::Data::Type::cINT)
      {
         auto returnStatus = static_cast<BT::ReturnStatus>(scriptRetVal.GetInt());
         if (returnStatus == BT::cSUCCESS)
         {
            preconditionsMet = true;
         }
         else
         {
            preconditionsMet = false;
         }
      }
      else
      {
         preconditionsMet = scriptRetVal.GetBool(); // mPreconditionScriptPtr is compiled with return type of Object,
                                                    // anything could be returned, hopefully they used a primitive type
      }
      mPreconditionRan = true;
      mPreconditionMet = preconditionsMet;
   }
   mInPrecondition = false;
   return preconditionsMet;
}

bool WsfAdvancedBehaviorTreeNode::ProcessInput(UtInput& aInput)
{
   bool  myCommand = true;
   auto& cmd       = aInput.GetCommand();
   if (cmd == "debug")
   {
      mDebug = true;
   }
   else if (cmd == "enabled")
   {
      mTurnedOn = aInput.ReadBool();
   }
   else if (cmd == "precondition")
   {
      // use return type "Object" because some preconditions can return double values, and others return Boolean values
      mPreconditionScriptPtr = mContextPtr->Compile("precondition", "Object", aInput, "end_precondition");
   }
   else if (cmd == "on_new_execute")
   {
      mNewExecuteScriptPtr = mContextPtr->Compile("on_new_execute", "void", aInput, "end_on_new_execute");
   }
   else if (cmd == "on_new_fail")
   {
      mNewFailScriptPtr = mContextPtr->Compile("on_new_fail", "void", aInput, "end_on_new_fail");
   }
   else if (cmd == "execute")
   {
      mExecuteScriptPtr = mContextPtr->Compile("execute", "int", aInput, "end_execute");
   }
   else if (cmd == "on_init")
   {
      mOnInitScriptPtr = mContextPtr->Compile("on_init", "void", aInput, "end_on_init");
   }
   else if (cmd == "state" || cmd == "show_state_evaluations" || cmd == "show_state_transitions")
   {
      if (mFiniteStateMachinePtr == nullptr)
      {
         mFiniteStateMachinePtr = new WsfScriptStateMachine(*mContextPtr);
      }
      myCommand = mFiniteStateMachinePtr->ProcessInput(aInput);
   }
   else if (cmd == "on_message")
   {
      if (mMessageHandlerPtr == nullptr)
      {
         mMessageHandlerPtr = new WsfScriptMessageHandler(*mContextPtr);
      }
      myCommand = mMessageHandlerPtr->ProcessInput(aInput); // on_message script handling
   }
   else if (cmd == "color")
   {
      aInput.ReadValue(mColor);
      mHasDefinedColor = true;
   }
   else if (cmd == "name")
   {
      std::string name;
      aInput.ReadValueQuoted(name);
      SetName(name);
   }
   else if (cmd == "desc" || cmd == "description")
   {
      // Get the description
      aInput.ReadValueQuoted(mDescription);
   }
   else if (aInput.GetCommand() == "success_policy")
   {
      // Skip here, only parallel will use this
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
      // still good
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool WsfAdvancedBehaviorTreeNode::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   if (mMessageHandlerPtr != nullptr)
   {
      return mMessageHandlerPtr->ProcessMessage(aSimTime, aMessage);
   }
   return false;
}

void WsfAdvancedBehaviorTreeNode::SetFileInfo(const std::string& aPath, const time_t& aModTime)
{
   mFullPathFileName = aPath;
   mFileLastModTime  = aModTime;
}

void WsfAdvancedBehaviorTreeNode::SetFilePath(const std::string& aPath)
{
   // get last file modified time, and set it along with the new name
   struct stat results;
   stat(aPath.c_str(), &results); // if fails, then "results.st_mtime" should hold default time_t value...
   SetFileInfo(aPath, results.st_mtime);
}

void WsfAdvancedBehaviorTreeNode::SetHasParent(const bool aValue)
{
   mHasParent = aValue;
}

void WsfAdvancedBehaviorTreeNode::SetNodeName(const std::string& aName)
{
   mName = aName;
}

void WsfAdvancedBehaviorTreeNode::SetNodeStatus(const BT::ReturnStatus aStatus)
{
   // Send an exec event if the node state changes.
   if (mReturnStatus != aStatus)
   {
      // Set the status before we send the message
      mReturnStatus = aStatus;

      GetTopRootNode()->GetOwningTree()->SetShouldOutputNextTick(true);
   }
   else
   {
      mReturnStatus = aStatus;
   }
}

void WsfAdvancedBehaviorTreeNode::SetTooltip(const std::string& aText, const bool aSendEvent)
{
   if (mInPrecondition)
   {
      SetPreconditionTooltip(aText, aSendEvent);
   }
   else
   {
      SetExecuteTooltip(aText, aSendEvent);
   }
}

void WsfAdvancedBehaviorTreeNode::SetPreconditionTooltip(const std::string& aText, const bool aSendEvent)
{
   double simTime = mContextPtr->GetTIME_NOW(mContextPtr->GetContext());
   if (mPreconditionTooltip != aText && simTime >= mPreconditionTooltipTimestamp)
   {
      mPreconditionTooltip = aText;
      if (aSendEvent)
      {
         WsfObserver::AdvancedBehaviorTreeState(GetSimulation())(simTime, GetOwningTree()->RootNode().get());
      }
      mPreconditionTooltipTimestamp = simTime;
   }
}

void WsfAdvancedBehaviorTreeNode::SetExecuteTooltip(const std::string& aText, const bool aSendEvent)
{
   double simTime = mContextPtr->GetTIME_NOW(mContextPtr->GetContext());
   // Check if our current simtime is greater than the last time the tooltip text was set
   if (mExecuteTooltip != aText && simTime >= mExecuteTooltipTimestamp)
   {
      mExecuteTooltip = aText;
      if (aSendEvent)
      {
         WsfObserver::AdvancedBehaviorTreeState(GetSimulation())(simTime, GetOwningTree()->RootNode().get());
      }
      mExecuteTooltipTimestamp = simTime;
   }
}

BT::ReturnStatus WsfAdvancedBehaviorTreeNode::Tick()
{
   // If the node is not turned on, return disabled
   if (!mTurnedOn)
   {
      // Check if the node has been set to disabled yet or not
      if (mReturnStatus != BT::cDISABLED)
      {
         // Recursively turn this and all of its children off
         TurnOff();
      }
      return BT::cDISABLED;
   }

   // If the preconditions are met for this node, execute the script
   if ((mPreconditionRan || PreconditionsMet()) && mPreconditionMet)
   {
      // If we didn't hit the execute script last tick, but did this tick, run the on_new_execute block
      if (!mExecutedLastTick && mNewExecuteScriptPtr != nullptr)
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;
         mContextPtr->ExecuteScript(GetSimulation()->GetSimTime(), mNewExecuteScriptPtr, scriptRetVal, scriptArgs);
      }
      mExecutedLastTick = true;

      // If the execute script exists, run it
      if (mExecuteScriptPtr != nullptr)
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;
         mContextPtr->ExecuteScript(GetSimulation()->GetSimTime(), mExecuteScriptPtr, scriptRetVal, scriptArgs);
      }

      if (mFiniteStateMachinePtr != nullptr)
      {
         mStateIndex = mFiniteStateMachinePtr->EvaluateState(mStateIndex); // only does one state transition (if necessary)
      }

      // Run the individual node tick function here
      return TickFunction();
   }
   else
   {
      // If we ran last tick, but didn't hit the execute this tick, run on_new_failure block
      if (mExecutedLastTick && mNewFailScriptPtr != nullptr)
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;
         mContextPtr->ExecuteScript(GetSimulation()->GetSimTime(), mNewFailScriptPtr, scriptRetVal, scriptArgs);
      }
      mExecutedLastTick = false;
      if (HasChildrenContainer())
      {
         HaltChildren(0u, "Node not considered");
      }
      SetNodeStatus(BT::cFAILURE);
      return BT::cFAILURE;
   }
}

WsfAdvancedBehaviorTreeLeafNode::WsfAdvancedBehaviorTreeLeafNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeNode(aScenario)
{
   WsfObject::SetType(WsfStringId("WSF_ADVANCED_BEHAVIOR"));
   WsfObject::SetName(WsfStringId("advanced_behavior"));
}

WsfAdvancedBehaviorTreeLeafNode::WsfAdvancedBehaviorTreeLeafNode(const WsfAdvancedBehaviorTreeLeafNode& aSrc)
   : WsfAdvancedBehaviorTreeNode(aSrc)
{
}

WsfAdvancedBehaviorTreeLeafNode* WsfAdvancedBehaviorTreeLeafNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeLeafNode(*this);
}

void WsfAdvancedBehaviorTreeLeafNode::Halt(const std::string& aReason)
{
   SetPreconditionTooltip(aReason);
   SetExecuteTooltip("");
   SetNodeStatus(BT::cHALTED);
}

const double WsfAdvancedBehaviorTreeLeafNode::PreconditionValue() const
{
   double retVal = 0.0;
   if (mPreconditionScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal(retVal);
      UtScriptDataList scriptArgs;
      mContextPtr->ExecuteScript(mPreconditionScriptPtr, scriptRetVal, scriptArgs);
      if (scriptRetVal.GetType() == ut::script::Data::Type::cBOOL)
      {
         retVal = (scriptRetVal.GetBool()) ? (1.0) : (-std::numeric_limits<double>::max());
      }
      else if (scriptRetVal.GetType() == ut::script::Data::Type::cINT)
      {
         auto returnStatus = static_cast<BT::ReturnStatus>(scriptRetVal.GetInt());
         retVal            = (returnStatus == BT::cSUCCESS) ? (1.0) : (-std::numeric_limits<double>::max());
      }
      else
      {
         retVal = scriptRetVal.GetDouble(); // mPreconditionScriptPtr is compiled with return type of Object, anything
                                            // could be returned, hopefully they used a primitive type
      }
      mPreconditionRan = true;
      mPreconditionMet = (retVal > 0.0);
   }
   return retVal;
}

bool WsfAdvancedBehaviorTreeLeafNode::ProcessInput(UtInput& aInput)
{
   if (aInput.GetCommand() == "advanced_behavior" || aInput.GetCommand() == "condition")
   {
      UtInputBlock block(aInput);
      std::string  name;
      if (block.ReadCommand(name)) // reads next command, should be the behavior name
      {
         WsfObject::SetName(name);

         std::string cmd;
         while (block.ReadCommand(cmd))
         {
            if (WsfAdvancedBehaviorTreeNode::ProcessInput(aInput))
            {
               // precondition, on_new_execute, or on_new_fail
            }
            else if (cmd == "sequence" || cmd == "selector" || cmd == "selector_with_memory" ||
                     cmd == "sequence_with_memory" || cmd == "parallel" || cmd == "priority_selector" ||
                     cmd == "weighted_random" || cmd == "behavior_node" || cmd == "decorator" || cmd == "condition")
            {
               if (mParentTreePtr == nullptr)
               {
                  // create a temporary tree for creating child types
                  mParentTreePtr = new WsfAdvancedBehaviorTree(GetScenario());
               }
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
      } // if (block.ReadCommand(name))
      return true;
   } // if (aInput.GetCommand() == "advanced_behavior" || aInput.GetCommand() == "condition")
   return false;
}

void WsfAdvancedBehaviorTreeLeafNode::RegisterInput(UtInput& aInput)
{
   std::string fileName = aInput.GetCurrentFileName();
   if (fileName[0] == '.' && (fileName[1] == '/' || fileName[1] == '\\'))
   {
      fileName = fileName.substr(2); // strip off first two characters of filename
   }
   std::string rootPath     = UtPath::WorkingDirectory().GetNormalizedPath();
   std::string fullPathName = rootPath + "/" + fileName;
   SetFilePath(fullPathName); // this sets the last modified time too
}

void WsfAdvancedBehaviorTreeLeafNode::SetTree(WsfAdvancedBehaviorTree* aTreePtr)
{
   mParentTreePtr = aTreePtr;
}

void WsfAdvancedBehaviorTreeLeafNode::TurnOff()
{
   mTurnedOn            = false;
   mPreconditionTooltip = std::string("Node turned off");
   mExecuteTooltip.clear();
   SetNodeStatus(BT::cDISABLED);
}

void WsfAdvancedBehaviorTreeLeafNode::TurnOn()
{
   mTurnedOn = true;
   mPreconditionTooltip.clear();
   mExecuteTooltip.clear();
   SetNodeStatus(BT::cIDLE);
}

void WsfAdvancedBehaviorTreeLeafNode::ResetPreconditionVars()
{
   if (mPreconditionScriptPtr != nullptr)
   {
      mPreconditionRan = false;
      mPreconditionMet = false;
   }
   else // If we don't have a script we want these true to just run the execute
   {
      mPreconditionRan = true;
      mPreconditionMet = true;
   }
}

WsfAdvancedBehaviorTreeActionNode::WsfAdvancedBehaviorTreeActionNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeLeafNode(aScenario)
{
}

WsfAdvancedBehaviorTreeActionNode* WsfAdvancedBehaviorTreeActionNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeActionNode(*this);
}

void WsfAdvancedBehaviorTreeActionNode::Halt(const std::string& aReason)
{
   WsfAdvancedBehaviorTreeLeafNode::Halt(aReason);
}

void WsfAdvancedBehaviorTreeActionNode::SetTree(WsfAdvancedBehaviorTree* aTreePtr)
{
   mParentTreePtr = aTreePtr;
   mParentTreePtr->RegisterActionNode(this);
}

BT::ReturnStatus WsfAdvancedBehaviorTreeActionNode::Tick()
{
   // If the node is not turned on, return disabled
   if (!mTurnedOn)
   {
      // Check if the node has been set to disabled yet or not
      if (mReturnStatus != BT::cDISABLED)
      {
         // Turn this node off
         TurnOff();
      }
      return BT::cDISABLED;
   }

   // If the preconditions are met for this node, execute the script
   if ((mPreconditionRan || PreconditionsMet()) && mPreconditionMet)
   {
      // If we didn't hit the execute script last tick, but did this tick, run the on_new_execute block
      if (!mExecutedLastTick && mNewExecuteScriptPtr != nullptr)
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;
         mContextPtr->ExecuteScript(GetSimulation()->GetSimTime(), mNewExecuteScriptPtr, scriptRetVal, scriptArgs);
      }
      mExecutedLastTick = true;

      BT::ReturnStatus nodeStatus = GetNodeStatus();
      // If the execute script exists, run it
      if (mExecuteScriptPtr != nullptr)
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;
         mContextPtr->ExecuteScript(GetSimulation()->GetSimTime(), mExecuteScriptPtr, scriptRetVal, scriptArgs);
         nodeStatus = static_cast<BT::ReturnStatus>(scriptRetVal.GetInt());
      }

      if (mFiniteStateMachinePtr != nullptr)
      {
         mStateIndex = mFiniteStateMachinePtr->EvaluateState(mStateIndex); // only does one state transition (if necessary)
      }

      // Check if this node was turned off during it's tick
      // If the node is not turned on, return disabled
      if (!mTurnedOn)
      {
         // Check if the node has been set to disabled yet or not
         if (mReturnStatus != BT::cDISABLED)
         {
            // Turn this node off
            TurnOff();
         }
         return BT::cDISABLED;
      }
      else
      {
         SetNodeStatus(nodeStatus);
         return nodeStatus;
      }
   }
   // If the precondition failed, return failure
   else
   {
      // If we ran last tick, but didn't hit the execute this tick, run on_new_failure block
      if (mExecutedLastTick && mNewFailScriptPtr != nullptr)
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;
         mContextPtr->ExecuteScript(GetSimulation()->GetSimTime(), mNewFailScriptPtr, scriptRetVal, scriptArgs);
      }
      mExecutedLastTick = false;
      SetNodeStatus(BT::cFAILURE);
      return BT::cFAILURE;
   }
}

WsfAdvancedBehaviorTreeConditionNode::WsfAdvancedBehaviorTreeConditionNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeLeafNode(aScenario)
{
}

WsfAdvancedBehaviorTreeConditionNode* WsfAdvancedBehaviorTreeConditionNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeConditionNode(*this);
}

void WsfAdvancedBehaviorTreeConditionNode::Halt(const std::string& aReason)
{
   WsfAdvancedBehaviorTreeLeafNode::Halt(aReason);
}

BT::ReturnStatus WsfAdvancedBehaviorTreeConditionNode::Tick()
{
   // If the node is not turned on, return disabled
   if (!mTurnedOn)
   {
      // Check if the node has been set to disabled yet or not
      if (mReturnStatus != BT::cDISABLED)
      {
         // Recursively turn this and all of its children off
         TurnOff();
      }
      return BT::cDISABLED;
   }

   if ((mPreconditionRan || PreconditionsMet()) && mPreconditionMet)
   {
      SetNodeStatus(BT::cSUCCESS);
      return BT::cSUCCESS;
   }
   else
   {
      SetNodeStatus(BT::cFAILURE);
      return BT::cFAILURE;
   }
}

WsfAdvancedBehaviorTreeCompositeNode::WsfAdvancedBehaviorTreeCompositeNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeNode(aScenario)
   , mChildren()
   , mTrees()
{
}

WsfAdvancedBehaviorTreeCompositeNode::WsfAdvancedBehaviorTreeCompositeNode(const WsfAdvancedBehaviorTreeCompositeNode& aSrc)
   : WsfAdvancedBehaviorTreeNode(aSrc)
   , mTrees(aSrc.mTrees)
{
   // Clone the children from the source
   for (auto& nodePtr : aSrc.mChildren)
   {
      AddChild(ut::clone(nodePtr));
   }

   // Clone the children from the source
   for (auto& tree : mTrees)
   {
      // Update each tree and replace the new root node from it in this root node's children.
      // Set the new root node for the tree
      tree.second->SetRootNodeClass();
      // Get a reference to the root node at the position
      // of the tree in this trees children
      auto& nodeToSet = mChildren.at(tree.first);
      // Set that node equal to the new root node,
      nodeToSet = tree.second->RootNode();
      // and set the parent to this root node
      nodeToSet->SetParent(this);
   }
}

void WsfAdvancedBehaviorTreeCompositeNode::AddChild(std::shared_ptr<WsfAdvancedBehaviorTreeNode> aChildPtr)
{
   // If the child doesn't have a parent, we are okay to set it's parent to this node.
   if (!aChildPtr->HasParent())
   {
      aChildPtr->SetHasParent(true);
      aChildPtr->SetParent(this);
      mChildren.push_back(aChildPtr);
   }
}

void WsfAdvancedBehaviorTreeCompositeNode::AddTree(ut::CloneablePtr<WsfAdvancedBehaviorTree>& aTreePtr)
{
   AddChild(aTreePtr->RootNode());
   mTrees.emplace(mChildren.size() - 1, std::move(aTreePtr));
}

void WsfAdvancedBehaviorTreeCompositeNode::ResetPreconditionVars()
{
   if (mPreconditionScriptPtr != nullptr)
   {
      mPreconditionRan = false;
      mPreconditionMet = false;
   }
   else // If we don't have a script we want these true to just run the execute
   {
      mPreconditionRan = true;
      mPreconditionMet = true;
   }

   for (auto i : mChildren)
   {
      i->ResetPreconditionVars();
   }
}

const bool WsfAdvancedBehaviorTreeCompositeNode::AreChildrenRunning() const
{
   for (auto i : mChildren)
   {
      if (i->GetNodeStatus() == BT::cRUNNING)
      {
         return true;
      }
   }
   return false;
}

WsfAdvancedBehaviorTreeCompositeNode* WsfAdvancedBehaviorTreeCompositeNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeCompositeNode(*this);
}

void WsfAdvancedBehaviorTreeCompositeNode::OutputTreeStructures(const double aSimTime)
{
   for (auto& tree : mTrees)
   {
      tree.second->OutputTreeStructure(aSimTime);
   }

   for (auto& node : mChildren)
   {
      node->OutputTreeStructures(aSimTime);
   }
}

void WsfAdvancedBehaviorTreeCompositeNode::OutputTreeStates(const double aSimTime)
{
   if (mIsTreeRootNode && mParentTreePtr != nullptr)
   {
      mParentTreePtr->OutputTreeState(aSimTime);
   }

   for (auto& node : mChildren)
   {
      node->OutputTreeStates(aSimTime);
   }
}

const int WsfAdvancedBehaviorTreeCompositeNode::GetDepth() const
{
   int depthMax = 0;
   int depth    = 0;
   for (auto i : mChildren)
   {
      depth = i->GetDepth();
      if (depth > depthMax)
      {
         depthMax = depth;
      }
   }
   return 1 + depthMax;
}

const unsigned int WsfAdvancedBehaviorTreeCompositeNode::GetLastEnabledChild() const
{
   unsigned int lastChild = 0;
   for (unsigned int i = 0; i < mChildren.size(); i++)
   {
      if (mChildren[i]->GetTurnedOn())
      {
         lastChild = i;
      }
   }
   return lastChild;
}

const unsigned int WsfAdvancedBehaviorTreeCompositeNode::GetNumEnabledChildren() const
{
   unsigned int numEnabled = 0;
   for (auto i : mChildren)
   {
      if (i->GetTurnedOn())
      {
         numEnabled++;
      }
   }
   return numEnabled;
}

void WsfAdvancedBehaviorTreeCompositeNode::Halt(const std::string& aReason)
{
   SetPreconditionTooltip(aReason);
   SetExecuteTooltip("");
   HaltChildren(0u, aReason);
   SetNodeStatus(BT::cHALTED);
}

void WsfAdvancedBehaviorTreeCompositeNode::HaltChildren(unsigned int aIndex, const std::string& aReason)
{
   // For each child from aIndex to mChildren.size(), halt each child.
   for (unsigned int i = aIndex; i < mChildren.size(); i++)
   {
      if (!mChildren[i]->GetTurnedOn())
      {
         continue;
      }

      mChildren[i]->Halt(aReason);
   }
}

void WsfAdvancedBehaviorTreeCompositeNode::HaltRunning(const std::string& aReason)
{
   // For each child from aIndex to mChildren.size(), halt each child.
   for (auto i : mChildren)
   {
      if (!i->GetTurnedOn())
      {
         continue;
      }
      else if (i->GetNodeStatus() == BT::cRUNNING)
      {
         i->Halt(aReason);
      }
   }
}

bool WsfAdvancedBehaviorTreeCompositeNode::Initialize(double aSimTime, WsfScriptContext* aParentContextPtr)
{
   WsfAdvancedBehaviorTreeNode::Initialize(aSimTime, aParentContextPtr);

   for (auto& child : mChildren)
   {
      mInitialized &= child->Initialize(aSimTime, this->mContextPtr);
   }

   return mInitialized;
}

const double WsfAdvancedBehaviorTreeCompositeNode::PreconditionValue() const
{
   double retVal = 0.0;
   if (!mChildren.empty())
   {
      retVal = mChildren[0]->PreconditionValue();
   }
   return retVal;
}

void WsfAdvancedBehaviorTreeCompositeNode::SetTree(WsfAdvancedBehaviorTree* aTreePtr)
{
   mParentTreePtr = aTreePtr;
   for (auto& child : mChildren)
   {
      if (!child->GetIsTreeRootNode())
      {
         child->SetTree(mParentTreePtr);
      }
   }
}

void WsfAdvancedBehaviorTreeCompositeNode::TurnOff()
{
   mTurnedOn            = false;
   mPreconditionTooltip = std::string("Node turned off");
   mExecuteTooltip.clear();
   SetNodeStatus(BT::cDISABLED);

   // Turn off children as well
   for (auto i : mChildren)
   {
      i->TurnOff();
   }
}

void WsfAdvancedBehaviorTreeCompositeNode::TurnOn()
{
   mTurnedOn = true;
   mPreconditionTooltip.clear();
   mExecuteTooltip.clear();
   SetNodeStatus(BT::cIDLE);

   // Turn on children as well
   for (auto i : mChildren)
   {
      i->TurnOn();
   }
}

WsfAdvancedBehaviorTreeDecoratorNode::WsfAdvancedBehaviorTreeDecoratorNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeCompositeNode(aScenario)
{
}

void WsfAdvancedBehaviorTreeDecoratorNode::AddChild(std::shared_ptr<WsfAdvancedBehaviorTreeNode> aChildPtr)
{
   // Check to make sure the decorator doesn't have a child (it can only have one)
   if (mChildren.empty())
   {
      aChildPtr->SetHasParent(true);
      aChildPtr->SetParent(this);
      mChildren.push_back(aChildPtr);
   }
}

void WsfAdvancedBehaviorTreeDecoratorNode::AddTree(ut::CloneablePtr<WsfAdvancedBehaviorTree>& aTreePtr)
{
   AddChild(aTreePtr->RootNode());
   mTrees.emplace(0, std::move(aTreePtr)); // Decorator only has one child, this index will always be 0
}

void WsfAdvancedBehaviorTreeDecoratorNode::OutputTreeStructures(const double aSimTime)
{
   for (auto& tree : mTrees)
   {
      tree.second->OutputTreeStructure(aSimTime);
   }

   for (auto& node : mChildren)
   {
      node->OutputTreeStructures(aSimTime);
   }
}

void WsfAdvancedBehaviorTreeDecoratorNode::OutputTreeStates(const double aSimTime)
{
   if (mIsTreeRootNode && mParentTreePtr != nullptr)
   {
      mParentTreePtr->OutputTreeState(aSimTime);
   }

   for (auto& node : mChildren)
   {
      node->OutputTreeStates(aSimTime);
   }
}

WsfAdvancedBehaviorTreeDecoratorNode* WsfAdvancedBehaviorTreeDecoratorNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeDecoratorNode(*this);
}

BT::ReturnStatus WsfAdvancedBehaviorTreeDecoratorNode::TickFunction()
{
   // If this decorator has a child, tick it.
   if (!mChildren.empty())
   {
      BT::ReturnStatus childStatus = mChildren[0]->Tick();

      // After ticking the child, If the child has not failed,
      // halt all of its children except the first.
      if (childStatus != BT::cFAILURE)
      {
         HaltChildren(1u, "");
      }

      SetNodeStatus(childStatus);
      return childStatus;
   }
   return BT::cIDLE;
}

WsfAdvancedBehaviorTreeInverterNode::WsfAdvancedBehaviorTreeInverterNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeDecoratorNode(aScenario)
{
   mName = "inverter";
   WsfObject::SetType(WsfStringId("inverter"));
   WsfObject::SetName(WsfStringId("inverter"));
}

WsfAdvancedBehaviorTreeInverterNode* WsfAdvancedBehaviorTreeInverterNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeInverterNode(*this);
}

BT::ReturnStatus WsfAdvancedBehaviorTreeInverterNode::Convert(BT::ReturnStatus aStatus)
{
   switch (aStatus)
   {
   case BT::cSUCCESS:
      return BT::cFAILURE;
      break;
   case BT::cFAILURE:
      return BT::cSUCCESS;
      break;
   default:
      return aStatus;
   }
}

BT::ReturnStatus WsfAdvancedBehaviorTreeInverterNode::TickFunction()
{
   // If this decorator has a child, tick it.
   if (!mChildren.empty())
   {
      // If our child is disabled, we should be too.
      if (!mChildren[0]->GetTurnedOn())
      {
         TurnOff();
         return BT::cDISABLED;
      }
      else
      {
         BT::ReturnStatus childStatus = mChildren[0]->Tick();

         // Invert the state here
         SetNodeStatus(Convert(childStatus));
         return Convert(childStatus);
      }
   }
   return BT::cIDLE;
}

WsfAdvancedBehaviorTreeSucceederNode::WsfAdvancedBehaviorTreeSucceederNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeDecoratorNode(aScenario)
{
   mName = "succeeder";
   WsfObject::SetType(WsfStringId("succeeder"));
   WsfObject::SetName(WsfStringId("succeeder"));
}

WsfAdvancedBehaviorTreeSucceederNode* WsfAdvancedBehaviorTreeSucceederNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeSucceederNode(*this);
}

BT::ReturnStatus WsfAdvancedBehaviorTreeSucceederNode::TickFunction()
{
   // If this decorator has a child, tick it.
   if (!mChildren.empty())
   {
      // If our child is disabled, we should be too.
      if (!mChildren[0]->GetTurnedOn())
      {
         TurnOff();
         return BT::cDISABLED;
      }
      else
      {
         HaltChildren(0u, "This node is being succeeded, and does not need to run.");
      }
   }
   SetNodeStatus(BT::cSUCCESS);
   return BT::cSUCCESS;
}

WsfAdvancedBehaviorTreeNegatorNode::WsfAdvancedBehaviorTreeNegatorNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeDecoratorNode(aScenario)
{
   mName = "negator";
   WsfObject::SetType(WsfStringId("negator"));
   WsfObject::SetName(WsfStringId("negator"));
}

WsfAdvancedBehaviorTreeNegatorNode* WsfAdvancedBehaviorTreeNegatorNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeNegatorNode(*this);
}

BT::ReturnStatus WsfAdvancedBehaviorTreeNegatorNode::TickFunction()
{
   // If this decorator has a child, tick it.
   if (!mChildren.empty())
   {
      // If our child is disabled, we should be too.
      if (!mChildren[0]->GetTurnedOn())
      {
         TurnOff();
         return BT::cDISABLED;
      }
      else
      {
         HaltChildren(0u, "This node is being negated, and does not need to run.");
      }
   }
   SetNodeStatus(BT::cFAILURE);
   return BT::cFAILURE;
}

WsfAdvancedBehaviorTreeRepeaterNode::WsfAdvancedBehaviorTreeRepeaterNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeDecoratorNode(aScenario)
   , mRepeats(1)
   , mRepeatsLeft(1)
   , mRunDuration(-1)
   , mRunStartTime(-1)
   , mRunType(Repeat)
{
   mName = "repeater";
   WsfObject::SetType(WsfStringId("repeater"));
   WsfObject::SetName(WsfStringId("repeater"));
}

WsfAdvancedBehaviorTreeRepeaterNode* WsfAdvancedBehaviorTreeRepeaterNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeRepeaterNode(*this);
}

void WsfAdvancedBehaviorTreeRepeaterNode::SetRepeats(int aRepeatCount)
{
   mRepeats     = aRepeatCount;
   mRepeatsLeft = mRepeats;
}

void WsfAdvancedBehaviorTreeRepeaterNode::SetRunType(RunType aRunType)
{
   mRunType = aRunType;
}

BT::ReturnStatus WsfAdvancedBehaviorTreeRepeaterNode::TickFunction()
{
   // If this decorator has a child, tick it.
   if (!mChildren.empty())
   {
      // If our child is disabled, we should be too.
      if (!mChildren[0]->GetTurnedOn())
      {
         TurnOff();
         return BT::cDISABLED;
      }

      switch (mRunType)
      {
      case ForTime:
      {
         // If we are just starting the run for the first time, set the starting run time
         if (mReturnStatus == BT::cIDLE || mReturnStatus == BT::cHALTED)
         {
            mRunStartTime = GetSimulation()->GetSimTime();
         }

         // If we haven't gone over the run duration, tick the node again
         double timeRunning = GetSimulation()->GetSimTime() - mRunStartTime;
         if (timeRunning < mRunDuration)
         {
            BT::ReturnStatus childStatus = mChildren[0]->Tick();

            // If our child turned off during its tick, return failure
            if (childStatus == BT::cDISABLED)
            {
               SetPreconditionTooltip("Child node turned off during its tick!");
               SetExecuteTooltip("");
               SetNodeStatus(BT::cFAILURE);
               return BT::cFAILURE;
            }
            else
            {
               SetNodeStatus(BT::cRUNNING);
               return BT::cRUNNING;
            }
         }
         else // Once the repeater is done, return true. (Reset it to idle after one success tick?)
         {
            SetNodeStatus(BT::cSUCCESS);
            return BT::cSUCCESS;
         }
      }
      break;
      case UntilDone:
      {
         BT::ReturnStatus childStatus = mChildren[0]->GetNodeStatus();
         // If this child hasn't finished (hasn't returned success or failure), tick it.
         if (childStatus != BT::cSUCCESS && childStatus != BT::cFAILURE)
         {
            childStatus = mChildren[0]->Tick();

            // Handle child tick response here
            // If our child turned off during its tick, return failure
            if (childStatus == BT::cDISABLED)
            {
               SetPreconditionTooltip("Child node turned off during its tick!");
               SetExecuteTooltip("");
               SetNodeStatus(BT::cFAILURE);
               return BT::cFAILURE;
            }
            else if (childStatus == BT::cSUCCESS || childStatus == BT::cFAILURE)
            {
               SetNodeStatus(BT::cSUCCESS);
               return BT::cSUCCESS;
            }
            else if (childStatus == BT::cRUNNING)
            {
               SetNodeStatus(BT::cRUNNING);
               return BT::cRUNNING;
            }
         }
         else // Once the repeater is done, return true. (Reset it to idle after one success tick?)
         {
            SetNodeStatus(BT::cSUCCESS);
            return BT::cSUCCESS;
         }
      }
      break;
      case Repeat:
      default:
      {
         // If we have repeats left, tick the node
         if (mRepeatsLeft > 0)
         {
            mRepeatsLeft--;
            BT::ReturnStatus childStatus = mChildren[0]->Tick();

            // If our child turned off during its tick, return failure
            if (childStatus == BT::cDISABLED)
            {
               SetPreconditionTooltip("Child node turned off during its tick!");
               SetExecuteTooltip("");
               SetNodeStatus(BT::cFAILURE);
               return BT::cFAILURE;
            }
            else
            {
               SetNodeStatus(BT::cRUNNING);
               return BT::cRUNNING;
            }
         }
         else // Once the repeater is done, return true. (Reset it to idle after one success tick?)
         {
            if (mChildren[0]->GetNodeStatus() == BT::cRUNNING)
            {
               mChildren[0]->Halt("Repeater has finished running.");
            }
            SetNodeStatus(BT::cSUCCESS);
            return BT::cSUCCESS;
         }
      }
      break;
      }
   }
   SetNodeStatus(BT::cFAILURE);
   return BT::cFAILURE;
}

WsfAdvancedBehaviorTreeParallelNode::WsfAdvancedBehaviorTreeParallelNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeCompositeNode(aScenario)
{
   mName = "parallel";
   WsfObject::SetType(WsfStringId("parallel"));
   WsfObject::SetName(WsfStringId("parallel"));
}

WsfAdvancedBehaviorTreeParallelNode::WsfAdvancedBehaviorTreeParallelNode(const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode,
                                                                         BT::SuccessPolicy  aSuccessPolicy,
                                                                         const unsigned int aThreshold)
   : WsfAdvancedBehaviorTreeCompositeNode(aCompositeNode)
   , mPolicy(aSuccessPolicy)
   , mThreshold(aThreshold)
{
   mName = "parallel";
}

WsfAdvancedBehaviorTreeParallelNode* WsfAdvancedBehaviorTreeParallelNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeParallelNode(*this);
}

void WsfAdvancedBehaviorTreeParallelNode::Halt(const std::string& aReason)
{
   mNumChildrenFailure = 0;
   mNumChildrenSuccess = 0;
   SetPreconditionTooltip(aReason);
   SetExecuteTooltip("");
   WsfAdvancedBehaviorTreeCompositeNode::Halt(aReason);
}

const double WsfAdvancedBehaviorTreeParallelNode::PreconditionValue() const
{
   double retVal = 0.0;
   for (auto i : mChildren)
   {
      double childVal = i->PreconditionValue();
      if (childVal > 0.0)
      {
         retVal += childVal;
      }
   }
   return retVal;
}

void WsfAdvancedBehaviorTreeParallelNode::SetSuccessPolicy(BT::SuccessPolicy aPolicy)
{
   mPolicy = aPolicy;
   mParentTreePtr->SetSuccessPolicy(aPolicy);
}

void WsfAdvancedBehaviorTreeParallelNode::SetThreshold(unsigned int aThreshold)
{
   mThreshold = aThreshold;
   mParentTreePtr->SetThreshold(aThreshold);
}

bool WsfAdvancedBehaviorTreeParallelNode::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (aInput.GetCommand() == "success_policy")
   {
      std::string successPolicy;
      aInput.ReadValue(successPolicy);
      if (successPolicy == "succeed_on_one")
      {
         SetSuccessPolicy(BT::cSUCCEED_ON_ONE);
      }
      else if (successPolicy == "succeed_on_all")
      {
         SetSuccessPolicy(BT::cSUCCEED_ON_ALL);
      }
      else if (successPolicy == "threshold")
      {
         unsigned int threshold;
         aInput.ReadValue(threshold);
         SetThreshold(threshold);
         SetSuccessPolicy(BT::cTHRESHOLD);
      }
   }
   else if (WsfAdvancedBehaviorTreeNode::ProcessInput(aInput))
   {
      // sets any generic node definitions
      // see WsfAdvancedBehaviorTreeNode::ProcessInput()
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

BT::ReturnStatus WsfAdvancedBehaviorTreeParallelNode::TickFunction()
{
   mNumChildrenSuccess = 0;
   mNumChildrenFailure = 0;

   // Tick each child
   for (auto i : mChildren)
   {
      BT::ReturnStatus childStatus = i->Tick();

      // Handle the child's status
      switch (childStatus)
      {
         // If our child has succeeded, check our threshold
         // and reset the needed values if it has been reached
      case BT::cSUCCESS:
         mNumChildrenSuccess++;
         // If our child has failed, check our threshold
         // and reset the needed values if it cannot be reached
         break;
      case BT::cFAILURE:
         mNumChildrenFailure++;
         break;
      case BT::cDISABLED:
      default:
         break;
      } // switch (childStatus)
   }    // for (int i = 0; i < GetNumChildren(); i++)

   switch (mPolicy)
   {
   case BT::cSUCCEED_ON_ALL:
   {
      // If any children fail, this node fails
      if (mNumChildrenFailure > 0)
      {
         mNumChildrenFailure = 0;
         mNumChildrenSuccess = 0;
         SetNodeStatus(BT::cFAILURE);
         return BT::cFAILURE;
      }
      else if (AreChildrenRunning())
      {
         SetNodeStatus(BT::cRUNNING);
         return BT::cRUNNING;
      }
      else // If we make it here, all children have succeeded
      {
         mNumChildrenFailure = 0;
         mNumChildrenSuccess = 0;
         SetNodeStatus(BT::cSUCCESS);
         return BT::cSUCCESS;
      }
   }
   break;
   case BT::cTHRESHOLD:
   {
      // Check if it is possible for us to hit the threshold.
      // If we can't return failure immediately, even if children are running.
      if (mNumChildrenFailure > (GetNumEnabledChildren() - mThreshold))
      {
         mNumChildrenFailure = 0;
         mNumChildrenSuccess = 0;
         SetNodeStatus(BT::cFAILURE);
         return BT::cFAILURE;
      }
      // After all children have ran, check and set the parallel node's state
      // according to its threshold
      else if (mNumChildrenSuccess >= mThreshold)
      {
         mNumChildrenFailure = 0;
         mNumChildrenSuccess = 0;
         SetNodeStatus(BT::cSUCCESS);
         return BT::cSUCCESS;
      }
      else if (mNumChildrenFailure > mChildren.size() - mThreshold)
      {
         mNumChildrenFailure = 0;
         mNumChildrenSuccess = 0;
         SetNodeStatus(BT::cFAILURE);
         return BT::cFAILURE;
      }
      else if (AreChildrenRunning())
      {
         SetNodeStatus(BT::cRUNNING);
         return BT::cRUNNING;
      }
      else // There are no children running, and we haven't hit the success threshold.
      {
         mNumChildrenFailure = 0;
         mNumChildrenSuccess = 0;
         SetNodeStatus(BT::cFAILURE);
         return BT::cFAILURE;
      }
   }
   break;
   case BT::cSUCCEED_ON_ONE:
   default:
   {
      // If any children succeed, this node succeeds
      if (mNumChildrenSuccess > 0)
      {
         mNumChildrenFailure = 0;
         mNumChildrenSuccess = 0;
         SetNodeStatus(BT::cSUCCESS);
         return BT::cSUCCESS;
      }
      else if (AreChildrenRunning())
      {
         SetNodeStatus(BT::cRUNNING);
         return BT::cRUNNING;
      }
      else // If we make it here, all children have failed
      {
         mNumChildrenFailure = 0;
         mNumChildrenSuccess = 0;
         SetNodeStatus(BT::cFAILURE);
         return BT::cFAILURE;
      }
   }
   break;
   }
   return BT::cIDLE;
}

WsfAdvancedBehaviorTreeSelectorNode::WsfAdvancedBehaviorTreeSelectorNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeCompositeNode(aScenario)
{
   mName = "selector";
   WsfObject::SetType(WsfStringId("selector"));
   WsfObject::SetName(WsfStringId("selector"));
}

WsfAdvancedBehaviorTreeSelectorNode::WsfAdvancedBehaviorTreeSelectorNode(const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode)
   : WsfAdvancedBehaviorTreeCompositeNode(aCompositeNode)
{
   mName = "selector";
}

WsfAdvancedBehaviorTreeSelectorNode* WsfAdvancedBehaviorTreeSelectorNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeSelectorNode(*this);
}

const double WsfAdvancedBehaviorTreeSelectorNode::PreconditionValue() const
{
   double retVal = 0.0;
   for (auto i : mChildren)
   {
      double childVal = i->PreconditionValue();
      if (childVal > 0.0)
      {
         return childVal;
      }
   }
   return retVal;
}

BT::ReturnStatus WsfAdvancedBehaviorTreeSelectorNode::TickFunction()
{
   // Tick each child
   for (unsigned int i = 0; i < mChildren.size(); i++)
   {
      // If the child is turned off, skip it
      if (!mChildren[i]->GetTurnedOn())
      {
         continue;
      }

      BT::ReturnStatus childStatus = mChildren[i]->Tick();

      // Handle the tick response
      // If the child turned off during its tick
      if (childStatus == BT::cDISABLED)
      {
         continue;
      }
      // If the child is running
      else if (childStatus == BT::cRUNNING)
      {
         HaltChildren(i + 1, "Node not considered");
         SetNodeStatus(BT::cRUNNING);
         return BT::cRUNNING;
      }
      // If the child has not failed yet
      else if (childStatus != BT::cFAILURE)
      {
         // Make sure every child after the one we are looking at is halted
         // if the current child has not failed
         HaltChildren(i + 1, "Node not considered");
         SetNodeStatus(childStatus);
         return childStatus;
      }
      // If the child has failed
      else
      {
         // Reset the child to idle, and check if it is the last child in the selector.
         // If it is, then the selector has failed.
         if (i == GetLastEnabledChild())
         {
            SetNodeStatus(childStatus);
            return childStatus;
         }
      }
   } // for (unsigned int i = 0; i < mChildren.size(); i++)
   return BT::cIDLE;
}

WsfAdvancedBehaviorTreeSelectorNodeWithMemory::WsfAdvancedBehaviorTreeSelectorNodeWithMemory(const WsfScenario& aScenario,
                                                                                             BT::ResetPolicy aResetPolicy)
   : WsfAdvancedBehaviorTreeCompositeNode(aScenario)
   , mCurrentChildIndex(0)
   , mResetPolicy(aResetPolicy)
{
   mName = "selector*";
   WsfObject::SetType(WsfStringId("selector*"));
   WsfObject::SetName(WsfStringId("selector*"));
}

WsfAdvancedBehaviorTreeSelectorNodeWithMemory::WsfAdvancedBehaviorTreeSelectorNodeWithMemory(
   const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode,
   BT::ResetPolicy                             aResetPolicy)
   : WsfAdvancedBehaviorTreeCompositeNode(aCompositeNode)
   , mCurrentChildIndex(0)
   , mResetPolicy(aResetPolicy)
{
   mName = "selector*";
}

WsfAdvancedBehaviorTreeSelectorNodeWithMemory* WsfAdvancedBehaviorTreeSelectorNodeWithMemory::Clone() const
{
   return new WsfAdvancedBehaviorTreeSelectorNodeWithMemory(*this);
}

void WsfAdvancedBehaviorTreeSelectorNodeWithMemory::Halt(const std::string& aReason)
{
   mCurrentChildIndex = 0;
   SetPreconditionTooltip(aReason);
   SetExecuteTooltip("");
   WsfAdvancedBehaviorTreeCompositeNode::Halt(aReason);
}

const double WsfAdvancedBehaviorTreeSelectorNodeWithMemory::PreconditionValue() const
{
   double retVal = 0.0;
   for (auto i : mChildren)
   {
      double childVal = i->PreconditionValue();
      if (childVal > 0.0)
      {
         return childVal;
      }
   }
   return retVal;
}

BT::ReturnStatus WsfAdvancedBehaviorTreeSelectorNodeWithMemory::TickFunction()
{
   // Tick each child
   while (mCurrentChildIndex < mChildren.size())
   {
      // If the child is turned off, skip it
      if (!mChildren[mCurrentChildIndex]->GetTurnedOn())
      {
         mCurrentChildIndex++;
         continue;
      }

      BT::ReturnStatus childStatus = mChildren[mCurrentChildIndex]->Tick();

      // Handle the tick response
      // If the node was turned off during the tick
      if (childStatus == BT::cDISABLED)
      {
         continue;
      }
      // If the child is running
      else if (childStatus == BT::cRUNNING)
      {
         HaltChildren(mCurrentChildIndex + 1, "Node not considered");
         SetNodeStatus(childStatus);
         return childStatus;
      }
      // If the child has not failed yet
      else if (childStatus != BT::cFAILURE)
      {
         // If our child has succeeded and matches the reset policy, reset our memory
         if (childStatus == BT::cSUCCESS && (mResetPolicy == BT::cON_SUCCESS || mResetPolicy == BT::cON_SUCCESS_OR_FAILURE))
         {
            HaltChildren(mCurrentChildIndex + 1, "Node not considered");
            mCurrentChildIndex = 0;
            SetNodeStatus(childStatus);
            return childStatus;
         }
      }
      // If the child failed
      else
      {
         // Check if it isn't the last node and increment our memory index
         if (mCurrentChildIndex != GetLastEnabledChild())
         {
            mCurrentChildIndex++;
            SetNodeStatus(BT::cRUNNING);
            continue;
            // return BT::cRUNNING;
         }
         // If the final child failed, reset the memory
         else
         {
            mCurrentChildIndex = 0;
         }

         SetNodeStatus(childStatus);
         return childStatus;
      }
   } // while (mCurrentChildIndex < mChildren.size())
   return BT::cIDLE;
}

WsfAdvancedBehaviorTreeSequenceNode::WsfAdvancedBehaviorTreeSequenceNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeCompositeNode(aScenario)
{
   mName = "sequence";
   WsfObject::SetType(WsfStringId("sequence"));
   WsfObject::SetName(WsfStringId("sequence"));
}

WsfAdvancedBehaviorTreeSequenceNode::WsfAdvancedBehaviorTreeSequenceNode(const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode)
   : WsfAdvancedBehaviorTreeCompositeNode(aCompositeNode)
{
   mName = "sequence";
}

WsfAdvancedBehaviorTreeSequenceNode* WsfAdvancedBehaviorTreeSequenceNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeSequenceNode(*this);
}

BT::ReturnStatus WsfAdvancedBehaviorTreeSequenceNode::TickFunction()
{
   // Tick each child
   for (unsigned int i = 0; i < mChildren.size(); i++)
   {
      // If the child is turned off, skip it
      if (!mChildren[i]->GetTurnedOn())
      {
         continue;
      }

      BT::ReturnStatus childStatus = mChildren[i]->Tick();

      // Handle the tick response
      // If the node was turned off during the tick
      if (childStatus == BT::cDISABLED)
      {
         continue;
      }
      // If the child is running
      else if (childStatus == BT::cRUNNING)
      {
         HaltChildren(i + 1, "Node not considered");
         SetNodeStatus(BT::cRUNNING);
         return BT::cRUNNING;
      }
      // If the child has not succeeded yet
      else if (childStatus != BT::cSUCCESS)
      {
         // Make sure every child after the one we are looking at is halted
         // if the current child has not failed
         HaltChildren(i + 1, "Node not considered");
         SetNodeStatus(childStatus);
         return childStatus;
      }
      // If the child has succeeded
      else
      {
         // Check if this child is the last child in the sequence.
         // If so, then the sequence has succeeded.
         if (i == GetLastEnabledChild())
         {
            SetNodeStatus(childStatus);
            return childStatus;
         }
      }
   } // for (unsigned int i = 0; i < mChildren.size(); i++)
   return BT::cIDLE;
}

WsfAdvancedBehaviorTreeSequenceNodeWithMemory::WsfAdvancedBehaviorTreeSequenceNodeWithMemory(const WsfScenario& aScenario,
                                                                                             BT::ResetPolicy aResetPolicy)
   : WsfAdvancedBehaviorTreeCompositeNode(aScenario)
   , mCurrentChildIndex(0)
   , mResetPolicy(aResetPolicy)
{
   mName = "sequence*";
   WsfObject::SetType(WsfStringId("sequence*"));
   WsfObject::SetName(WsfStringId("sequence*"));
}

WsfAdvancedBehaviorTreeSequenceNodeWithMemory::WsfAdvancedBehaviorTreeSequenceNodeWithMemory(
   const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode,
   BT::ResetPolicy                             aResetPolicy)
   : WsfAdvancedBehaviorTreeCompositeNode(aCompositeNode)
   , mCurrentChildIndex(0)
   , mResetPolicy(aResetPolicy)
{
   mName = "sequence*";
}

WsfAdvancedBehaviorTreeSequenceNodeWithMemory* WsfAdvancedBehaviorTreeSequenceNodeWithMemory::Clone() const
{
   return new WsfAdvancedBehaviorTreeSequenceNodeWithMemory(*this);
}

void WsfAdvancedBehaviorTreeSequenceNodeWithMemory::Halt(const std::string& aReason)
{
   mCurrentChildIndex = 0;
   SetPreconditionTooltip(aReason);
   SetExecuteTooltip("");
   WsfAdvancedBehaviorTreeCompositeNode::Halt(aReason);
}

BT::ReturnStatus WsfAdvancedBehaviorTreeSequenceNodeWithMemory::TickFunction()
{
   // Tick each child
   while (mCurrentChildIndex < mChildren.size())
   {
      // If the child is turned off, skip it
      if (!mChildren[mCurrentChildIndex]->GetTurnedOn())
      {
         mCurrentChildIndex++;
         continue;
      }

      BT::ReturnStatus childStatus = mChildren[mCurrentChildIndex]->Tick();

      // Handle the tick response
      // If the node was turned off during the tick
      if (childStatus == BT::cDISABLED)
      {
         continue;
      }
      // If the child is running
      else if (childStatus == BT::cRUNNING)
      {
         HaltChildren(mCurrentChildIndex + 1, "Node not considered");
         SetNodeStatus(BT::cRUNNING);
         return BT::cRUNNING;
      }
      // If the child has not succeeded yet
      else if (childStatus != BT::cSUCCESS)
      {
         // If our child has failed and matches the reset policy, reset our memory
         if (childStatus == BT::cFAILURE && (mResetPolicy == BT::cON_FAILURE || mResetPolicy == BT::cON_SUCCESS_OR_FAILURE))
         {
            HaltChildren(mCurrentChildIndex + 1, "Node not considered");
            mCurrentChildIndex = 0;
            SetNodeStatus(childStatus);
            return childStatus;
         }
      }
      // If our node was successful
      else
      {
         // Check if it isn't the last node and increment our memory index
         if (mCurrentChildIndex != GetLastEnabledChild())
         {
            mCurrentChildIndex++;
            SetNodeStatus(BT::cRUNNING);
            continue;
            // return BT::cRUNNING;
         }
         // If the final child failed, reset the memory
         else
         {
            mCurrentChildIndex = 0;
            SetNodeStatus(childStatus);
            return childStatus;
         }
      }
   } // while (mCurrentChildIndex < mChildren.size())
   return BT::cIDLE;
}

WsfAdvancedBehaviorTreePrioritySelectorNode::WsfAdvancedBehaviorTreePrioritySelectorNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeSelectorNode(aScenario)
   , mCurrentSelectionPtr(nullptr)
{
   mName = "priority_selector";
   WsfObject::SetType(WsfStringId("priority_selector"));
   WsfObject::SetName(WsfStringId("priority_selector"));
}

WsfAdvancedBehaviorTreePrioritySelectorNode::WsfAdvancedBehaviorTreePrioritySelectorNode(
   const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode)
   : WsfAdvancedBehaviorTreeSelectorNode(aCompositeNode)
   , mCurrentSelectionPtr(nullptr)
{
   mName = "priority_selector";
}

WsfAdvancedBehaviorTreePrioritySelectorNode* WsfAdvancedBehaviorTreePrioritySelectorNode::Clone() const
{
   return new WsfAdvancedBehaviorTreePrioritySelectorNode(*this);
}

const double WsfAdvancedBehaviorTreePrioritySelectorNode::PreconditionValue() const
{
   std::pair<double, WsfAdvancedBehaviorTreeNode*> retVal(0.0, nullptr);
   for (auto i : mChildren)
   {
      double childVal = i->PreconditionValue();
      if (childVal > retVal.first)
      {
         retVal.first  = childVal;
         retVal.second = i.get();
      }
   }

   mCurrentSelectionPtr = retVal.second;

   return retVal.first;
}

BT::ReturnStatus WsfAdvancedBehaviorTreePrioritySelectorNode::TickFunction()
{
   // If we don't have a node selected, evaluate weights and run that child
   if (GetCurrentSelection() == nullptr)
   {
      // Check each child for a precondition value and then execute the highest one.
      double                       highestPriority      = 0.0;
      WsfAdvancedBehaviorTreeNode* highestPriorityChild = nullptr;

      // Check all children for the highest priority
      for (auto i : mChildren)
      {
         if (!i->GetTurnedOn())
         {
            continue;
         }

         double preconditionValue = i->PreconditionValue();

         // If this child has the highest priority, set the new value and pointer
         // Note: If all children are equal the first will get picked repeatedly
         if (preconditionValue > highestPriority)
         {
            highestPriority      = preconditionValue;
            highestPriorityChild = i.get();
         }
      }

      SetCurrentSelection(highestPriorityChild);

      // If we evaluate all children and still don't get a selection, we have no children to execute.
      if (GetCurrentSelection() == nullptr)
      {
         SetExecuteTooltip("No children to execute!");
         HaltChildren(0u, "Node not considered");
         SetNodeStatus(BT::cFAILURE);
         return BT::cFAILURE;
      }
      else
      {
         // Halt all children that aren't the current selection
         for (auto i : mChildren)
         {
            if (!i->GetTurnedOn())
            {
               continue;
            }

            if (i.get() != GetCurrentSelection())
            {
               i->Halt("Node not considered");
            }
         }

         // Then execute the highest priority child.  Any node whose precondition has already
         // run will not execute the precondition again this tick
         BT::ReturnStatus childStatus = GetCurrentSelection()->Tick();

         // If our selection turned off during the tick, return failure
         if (childStatus == BT::cDISABLED)
         {
            SetPreconditionTooltip("Selected node turned off during its tick!");
            SetExecuteTooltip("");
            SetNodeStatus(BT::cFAILURE);
            return BT::cFAILURE;
         }
         else
         {
            SetNodeStatus(childStatus);
         }

         // If the selected child is done running, reset the selected node.
         if (childStatus != BT::cRUNNING)
         {
            SetCurrentSelection(nullptr);
         }

         return childStatus;
      }
   }
   else // If we have a currently selected child, tick it and return
   {
      // Halt all children that aren't the current selection
      for (auto i : mChildren)
      {
         if (!i->GetTurnedOn())
         {
            continue;
         }

         if (i.get() != GetCurrentSelection())
         {
            i->Halt("Node not considered");
         }
      }

      // Then execute the highest priority child
      BT::ReturnStatus childStatus = GetCurrentSelection()->Tick();
      SetNodeStatus(childStatus);

      // If the selected child is done running, reset the selected node.
      if (childStatus != BT::cRUNNING)
      {
         SetCurrentSelection(nullptr);
      }

      return childStatus;
   }
}

WsfAdvancedBehaviorTreeWeightedRandomNode::WsfAdvancedBehaviorTreeWeightedRandomNode(const WsfScenario& aScenario)
   : WsfAdvancedBehaviorTreeCompositeNode(aScenario)
   , mCurrentSelectionPtr(nullptr)
{
   mName = "weighted_random";
   WsfObject::SetType(WsfStringId("weighted_random"));
   WsfObject::SetName(WsfStringId("weighted_random"));
}

WsfAdvancedBehaviorTreeWeightedRandomNode::WsfAdvancedBehaviorTreeWeightedRandomNode(
   const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode)
   : WsfAdvancedBehaviorTreeCompositeNode(aCompositeNode)
   , mCurrentSelectionPtr(nullptr)
{
   mName = "weighted_random";
}

WsfAdvancedBehaviorTreeWeightedRandomNode* WsfAdvancedBehaviorTreeWeightedRandomNode::Clone() const
{
   return new WsfAdvancedBehaviorTreeWeightedRandomNode(*this);
}

const std::pair<double, WsfAdvancedBehaviorTreeNode*> WsfAdvancedBehaviorTreeWeightedRandomNode::GetRandomNode() const
{
   std::pair<double, WsfAdvancedBehaviorTreeNode*> retVal(0.0, nullptr);
   std::vector<double>                             weights;

   for (auto i : mChildren)
   {
      if (!i->GetTurnedOn())
      {
         weights.push_back(0.0);
         continue;
      }

      double preconditionValue = i->PreconditionValue();
      weights.push_back((preconditionValue < 0.0) ? (0.0) : (preconditionValue));
   }

   double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
   if (sum > 0.0)
   {
      // can make a choice, at least one child returned a value
      double cumulative = 0.0;
      double R          = GetSimulation()->GetRandom().Uniform<double>();

      for (unsigned int i = 0; i < mChildren.size(); ++i)
      {
         cumulative += (weights[i] / sum);
         if (cumulative >= R)
         {
            retVal.first  = weights[i];
            retVal.second = mChildren[i].get();
            break;
         }
      }
   }

   return retVal;
}

const double WsfAdvancedBehaviorTreeWeightedRandomNode::PreconditionValue() const
{
   std::pair<double, WsfAdvancedBehaviorTreeNode*> randomDraw = GetRandomNode();

   mCurrentSelectionPtr = randomDraw.second;

   return randomDraw.first;
}

BT::ReturnStatus WsfAdvancedBehaviorTreeWeightedRandomNode::TickFunction()
{
   // If we have a preselected node, tick it and return, resetting the preset node pointer.
   if (GetParent()->GetType() == "priority_selector" && mCurrentSelectionPtr != nullptr)
   {
      // Halt all children that aren't the chosen child
      for (auto i : mChildren)
      {
         if (!i->GetTurnedOn())
         {
            continue;
         }

         if (i.get() != mCurrentSelectionPtr)
         {
            i->Halt("node not chosen");
         }
      }

      BT::ReturnStatus childStatus = mCurrentSelectionPtr->Tick();

      // If our selection turned off during its tick, return failure
      if (childStatus == BT::cDISABLED)
      {
         SetPreconditionTooltip("Selected node turned off during its tick!");
         SetExecuteTooltip("");
         SetNodeStatus(BT::cFAILURE);
         return BT::cFAILURE;
      }
      else
      {
         SetNodeStatus(childStatus);
      }
      // Reset the current weighted random so that the priority selector will get a new draw weight on this weighted
      // random next time If the selected child is done running, reset the selected node.
      if (childStatus != BT::cRUNNING)
      {
         mCurrentSelectionPtr = nullptr;
      }

      return childStatus;
   }
   else // If we didnt have a node preselected for us, check if we have a node running, and if we do, run it
   {
      WsfAdvancedBehaviorTreeNode* runningNode = nullptr;
      for (auto i : mChildren)
      {
         if (i->GetNodeStatus() == BT::cRUNNING)
         {
            runningNode = i.get();
            break;
         }
      }
      if (runningNode != nullptr)
      {
         // Halt all children that aren't the chosen child
         for (auto i : mChildren)
         {
            if (!i->GetTurnedOn())
            {
               continue;
            }

            if (i.get() != runningNode)
            {
               i->Halt("node not chosen");
            }
         }
         BT::ReturnStatus childStatus = runningNode->Tick();

         // If our running node turned off during its tick, return failure
         if (childStatus == BT::cDISABLED)
         {
            SetPreconditionTooltip("Selected node turned off during its tick!");
            SetExecuteTooltip("");
            SetNodeStatus(BT::cFAILURE);
            return BT::cFAILURE;
         }
         else
         {
            SetNodeStatus(childStatus);
         }

         return childStatus;
      }
      else // Otherwise make a random draw and tick the selected node
      {
         std::vector<double> weights;

         for (auto i : mChildren)
         {
            if (!i->GetTurnedOn())
            {
               weights.push_back(0.0);
            }
            else
            {
               double preconditionValue = i->PreconditionValue();
               weights.push_back((preconditionValue < 0.0) ? (0.0) : (preconditionValue));
            }
         }

         double                       sum     = std::accumulate(weights.begin(), weights.end(), 0.0);
         WsfAdvancedBehaviorTreeNode* nodePtr = nullptr;
         if (sum > 0.0)
         {
            // can make a choice, at least one child returned a value
            double cumulative = 0.0;
            double R          = GetSimulation()->GetRandom().Uniform<double>();

            for (unsigned int i = 0; i < mChildren.size(); ++i)
            {
               cumulative += (weights[i] / sum);
               if (cumulative >= R)
               {
                  nodePtr = mChildren[i].get();
                  break;
               }
            }

            // Halt all children that aren't the chosen child
            for (auto i : mChildren)
            {
               if (!i->GetTurnedOn())
               {
                  continue;
               }

               if (i.get() != nodePtr)
               {
                  i->Halt("node not chosen");
               }
            }
            BT::ReturnStatus childStatus = BT::cFAILURE;
            if (nodePtr)
            {
               // Tick the node, if its precondition has already run it won't run again this tick
               childStatus = nodePtr->Tick();
            }

            // If our child turned off during its tick, return failure
            if (childStatus == BT::cDISABLED)
            {
               SetPreconditionTooltip("Selected node turned off during its tick!");
               SetExecuteTooltip("");
               SetNodeStatus(BT::cFAILURE);
               return BT::cFAILURE;
            }
            else
            {
               SetNodeStatus(childStatus);
            }

            return childStatus;
         }
         // If the sum is <= 0, then we don't have any nodes to chose from
         else
         {
            SetPreconditionTooltip("No nodes to chose from!");
            SetExecuteTooltip("");
            HaltChildren(0u, "Node not considered");
            SetNodeStatus(BT::cFAILURE);
            return BT::cFAILURE;
         }
      }
   }
   return BT::cIDLE;
}
