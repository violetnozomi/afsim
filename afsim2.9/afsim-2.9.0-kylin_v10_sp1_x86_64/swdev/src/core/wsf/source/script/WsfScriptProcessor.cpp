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

#include "script/WsfScriptProcessor.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtStringUtil.hpp"
#include "WsfAdvancedBehaviorTree.hpp"
#include "WsfAdvancedBehaviorTreeNode.hpp"
#include "WsfBehaviorTree.hpp"
#include "WsfBehaviorTreeNode.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptEvent.hpp"
#include "script/WsfScriptMessageHandler.hpp"
#include "script/WsfScriptStateMachine.hpp"

// ================================================================================================
WsfScriptProcessor::WsfScriptProcessor(const WsfScenario& aScenario)
   : WsfProcessor(aScenario)
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), "WsfProcessor", "PROCESSOR"))
   , mMessageHandlerPtr(nullptr)
   , mOnMessageCreateScriptPtr(nullptr)
   , mExternalLinks()
   , mBehaviorTreePtr(nullptr)
   , mAdvancedBehaviorTreePtr(nullptr)
   , mStateIndex(0)
   , mSuppressMessage(false)
{
   // This is not done in the initializer list since it relies on mContextPtr.
   mMessageHandlerPtr = new WsfScriptMessageHandler(*mContextPtr);
   mMessageClassPtr   = mContextPtr->GetClass("WsfMessage");
}

// ================================================================================================
WsfScriptProcessor::WsfScriptProcessor(const WsfScenario& aScenario, WsfStringId aPartClass, const std::string& aPartVarName)
   : WsfProcessor(aScenario)
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), aPartClass, aPartVarName))
   , mMessageHandlerPtr(nullptr)
   , mOnMessageCreateScriptPtr(nullptr)
   , mExternalLinks()
   , mBehaviorTreePtr(nullptr)
   , mAdvancedBehaviorTreePtr(nullptr)
   , mStateIndex(0)
   , mSuppressMessage(false)
{
   // This is not done in the initializer list since it relies on mContextPtr.
   mMessageHandlerPtr = new WsfScriptMessageHandler(*mContextPtr);
   mMessageClassPtr   = mContextPtr->GetClass("WsfMessage");
}

// ================================================================================================
// virtual
WsfScriptProcessor::~WsfScriptProcessor()
{
   delete mContextPtr;
   delete mMessageHandlerPtr;
   delete mBehaviorTreePtr;
}

// ================================================================================================
// protected
WsfScriptProcessor::WsfScriptProcessor(const WsfScriptProcessor& aSrc)
   : WsfProcessor(aSrc)
   , mContextPtr(new WsfScriptContext(*(aSrc.mContextPtr)))
   , mMessageHandlerPtr(nullptr)
   , mOnMessageCreateScriptPtr(nullptr)
   , mExternalLinks(aSrc.mExternalLinks)
   , mBehaviorTreePtr(nullptr)
   , mAdvancedBehaviorTreePtr(aSrc.mAdvancedBehaviorTreePtr)
   , mStateIndex(aSrc.mStateIndex)
   , mSuppressMessage(false)
{
   mBehaviorTreePtr = (aSrc.mBehaviorTreePtr != nullptr) ? (aSrc.mBehaviorTreePtr->Clone()) : (nullptr);
   if (aSrc.mStateMachinePtr != nullptr)
   {
      mStateMachinePtr = ut::make_unique<WsfScriptStateMachine>(*aSrc.mStateMachinePtr, *mContextPtr, this);
   }
   // This is not done in the initializer list since it relies on mContextPtr.
   mMessageHandlerPtr = new WsfScriptMessageHandler(*aSrc.mMessageHandlerPtr, *mContextPtr);
   mMessageClassPtr   = mContextPtr->GetClass("WsfMessage");
}

// ================================================================================================
// virtual
WsfProcessor* WsfScriptProcessor::Clone() const
{
   return new WsfScriptProcessor(*this);
}

// ================================================================================================
// virtual
bool WsfScriptProcessor::Initialize(double aSimTime)
{
   bool ok = WsfProcessor::Initialize(aSimTime);
   ok &= mContextPtr->Initialize(aSimTime, GetPlatform(), this);
   ok &= mMessageHandlerPtr->Initialize(aSimTime, GetPlatform());

   // Capture the pointer to the 'on_message_create' script and ensure it has the proper signature.
   mOnMessageCreateScriptPtr = mContextPtr->FindScript("on_message_create");
   if (mOnMessageCreateScriptPtr != nullptr)
   {
      ok &= mContextPtr->ValidateScript(mOnMessageCreateScriptPtr, "void", "WsfMessage");
   }

   // Establish links to external (off-board) recipients
   ok &= mExternalLinks.Initialize(aSimTime, GetPlatform());

   if (mBehaviorTreePtr != nullptr)
   {
      ok &= mBehaviorTreePtr->Initialize(aSimTime, this);
   }

   if (mAdvancedBehaviorTreePtr != nullptr)
   {
      ok &= mAdvancedBehaviorTreePtr->Initialize(aSimTime, this);
   }

   if (mStateMachinePtr != nullptr)
   {
      ok &= mStateMachinePtr->Initialize(aSimTime);
      mStateIndex = mStateMachinePtr->GetInitialStateIndex();
      mStateMachinePtr->EnterState(mStateIndex); // this forces the "on_entry" block to be ran for the initial state
   }

   return ok;
}

// ================================================================================================
// virtual
bool WsfScriptProcessor::Initialize2(double aSimTime)
{
   bool ok = WsfProcessor::Initialize2(aSimTime);
   ok &= mContextPtr->Initialize2(aSimTime);
   return ok;
}

template<class BEHAVIOR_TREE>
void EditBehavior(BEHAVIOR_TREE* aBehaviorTreePtr, UtInput& aInput)
{
   // this is a local edit of an existing behavior(s) on the tree
   // only edit the node(s) of this name on this processor's behavior tree, NOT the global definition of the behavior
   if (aBehaviorTreePtr != nullptr)
   {
      // change all nodes of the name given
      UtInput::StoredLocation editStartLoc = aInput.StoreLocation();
      std::string             behaviorName;
      aInput.ReadValue(behaviorName);
      // don't forget: behavior names are object types, and behavior types are object names
      bool editedAtLeastOneBehavior = false;
      for (size_t i = 0; i < aBehaviorTreePtr->NodeCount(); ++i)
      {
         auto nodePtr = aBehaviorTreePtr->NodeEntry(i);
         if (nodePtr->GetType() == behaviorName)
         {
            aInput.RestoreLocation(editStartLoc);
            editedAtLeastOneBehavior |= nodePtr->ProcessInput(aInput);
         }
      }

      if (!editedAtLeastOneBehavior)
      {
         std::string msg = "error: local edit not possible, behavior leaf node not found: " + behaviorName;
         throw UtInput::BadValue(aInput, msg);
      }
   }
   else
   {
      throw UtInput::BadValue(aInput, "error: local behavior edit not possible, behavior tree not found!");
   }
}

// ================================================================================================
// virtual
bool WsfScriptProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (mMessageHandlerPtr->ProcessInput(aInput))
   {
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
   }
   else if (mExternalLinks.ProcessInput(aInput))
   {
   }
   else if (command == "behavior_tree")
   {
      delete mBehaviorTreePtr;
      mBehaviorTreePtr = new WsfBehaviorTree(GetScenario());
      mBehaviorTreePtr->ProcessInput(aInput);
   }
   else if (command == "clear_behavior_tree")
   {
      if (mBehaviorTreePtr != nullptr)
      {
         delete mBehaviorTreePtr;
      }
      mBehaviorTreePtr = nullptr;
   }
   else if (command == "advanced_behavior_tree")
   {
      mAdvancedBehaviorTreePtr = ut::make_unique<WsfAdvancedBehaviorTree>(GetScenario());
      mAdvancedBehaviorTreePtr->ProcessInput(aInput);
   }
   else if (command == "clear_advanced_behavior_tree")
   {
      mAdvancedBehaviorTreePtr = nullptr;
   }
   else if (command == "state" || command == "show_state_evaluations" || command == "show_state_transitions")
   {
      if (mStateMachinePtr == nullptr)
      {
         mStateMachinePtr = ut::make_unique<WsfScriptStateMachine>(*mContextPtr, this);
      }
      myCommand = mStateMachinePtr->ProcessInput(aInput);
   }
   else if (command == "edit")
   {
      std::string thing;
      aInput.ReadCommand(thing);
      if (thing == "behavior")
      {
         EditBehavior(mBehaviorTreePtr, aInput);
      }
      else if (thing == "advanced_behavior")
      {
         EditBehavior(mAdvancedBehaviorTreePtr.get(), aInput);
      }
      else if (thing == "state")
      {
         if (mStateMachinePtr == nullptr)
         {
            throw UtInput::BadValue(aInput, "error: local state edit not possible, no states have been defined!");
         }
         UtInputBlock inputBlock(aInput);
         std::string  stateName;
         aInput.ReadValue(stateName);
         WsfStringId                   stateNameId = stateName;
         WsfScriptStateMachine::State* statePtr    = mStateMachinePtr->FindState(stateNameId);
         if (statePtr == nullptr)
         {
            std::string msg = "error: local edit not possible, state not found: " + stateName;
            throw UtInput::BadValue(aInput, msg);
         }
         while (inputBlock.ReadCommand())
         {
            if (!statePtr->ProcessInput(aInput))
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
      }
      else
      {
         std::string msg = "error: local edit not valid, can not edit " + thing;
         throw UtInput::BadValue(aInput, msg);
      }
   }
   else
   {
      myCommand = WsfProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
// virtual
void WsfScriptProcessor::Update(double aSimTime)
{
   mContextPtr->Update(aSimTime);
   if (mBehaviorTreePtr != nullptr)
   {
      mBehaviorTreePtr->Execute(aSimTime);
   }
   if (mAdvancedBehaviorTreePtr != nullptr)
   {
      mAdvancedBehaviorTreePtr->Tick(aSimTime);
   }
   if (mStateMachinePtr != nullptr)
   {
      mStateIndex = mStateMachinePtr->EvaluateState(mStateIndex);
   }
}

// ================================================================================================
// virtual
bool WsfScriptProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   // Initialize the 'suppress message' flag. This can be set by the user scripts in their message
   // handler if they don't want the message forwarded.

   mSuppressMessage = false;

   // Perform any local processing
   bool processed = mMessageHandlerPtr->ProcessMessage(aSimTime, aMessage);

   if (mBehaviorTreePtr != nullptr)
   {
      for (size_t i = 0; i < mBehaviorTreePtr->NodeCount(); ++i)
      {
         WsfBehaviorTreeLeafNode* node = mBehaviorTreePtr->NodeEntry(i);
         processed |= node->ProcessMessage(aSimTime, aMessage);
      }
   }

   if (mAdvancedBehaviorTreePtr != nullptr)
   {
      for (size_t i = 0; i < mAdvancedBehaviorTreePtr->NodeCount(); ++i)
      {
         WsfAdvancedBehaviorTreeLeafNode* node = mAdvancedBehaviorTreePtr->NodeEntry(i);
         processed |= node->ProcessMessage(aSimTime, aMessage);
      }
   }

   // Forward the message to linked recipients
   if (!mSuppressMessage)
   {
#undef SendMessage
      SendMessage(aSimTime, aMessage);
   }
   return processed;
}

// ================================================================================================
//! A convenience for derived classes to send a message to linked objects.
//!
//! @param aSimTime The current simulation time.
//! @param aMessage The message to be sent.
// virtual
#undef SendMessage
void WsfScriptProcessor::SendMessage(double aSimTime, const WsfMessage& aMessage)
{
   WsfPlatformPart::SendMessage(aSimTime, aMessage); // Send message to onboard recipients
   if (mExternalLinks.HasLinks())
   {
      mExternalLinks.SendMessage(aSimTime, aMessage);
   }
}

// ================================================================================================
//! Let the user examine/modify the outbound message prior to it being sent.
//! This is primarily to let them set the message priority.
//! @param aSimTime The current simulation time.
//! @param aMessage The message to be transmitted.
//! @note Derived classes are responsible for invoking this at the point where they create the
//!       message for which
// virtual
void WsfScriptProcessor::ExecuteOnMessageCreate(double aSimTime, WsfMessage& aMessage)
{
   if (mOnMessageCreateScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;

      // There is one argument; the message
      scriptArgs.push_back(UtScriptData(new UtScriptRef(&aMessage, mMessageClassPtr)));
      mContextPtr->ExecuteScript(aSimTime, mOnMessageCreateScriptPtr, scriptRetVal, scriptArgs);
   }
}

// ================================================================================================
// virtual
bool WsfScriptProcessor::ExecuteScript(double aSimTime, const std::string& aScriptName)
{
   return mContextPtr->ExecuteScript(aSimTime, aScriptName);
}

// ================================================================================================
// virtual
bool WsfScriptProcessor::ExecuteScript(double                  aSimTime,
                                       WsfStringId             aScriptName,
                                       UtScriptData&           aScriptRetVal,
                                       const UtScriptDataList& aScriptArgs)
{
   return mContextPtr->ExecuteScript(aSimTime, aScriptName, aScriptRetVal, aScriptArgs);
}

// ================================================================================================
//! Execute the script with the specified string name ID at the specified time.
//! @param aTime The time at which to execute the script.
//! @param aScriptName The string name ID of the script to be executed.
//! @param aScriptArgs The list of arguments to pass to the script.
void WsfScriptProcessor::ExecuteScriptAtTime(double aTime, WsfStringId aScriptName, const UtScriptDataList& aScriptArgs)
{
   GetSimulation()->AddEvent(ut::make_unique<WsfScriptEvent>(aTime, this, aScriptName, aScriptArgs));
}

// ================================================================================================
//! Indicate if forwarding of the current message to linked recipients should be suppressed.
//!
//! This is invoked from scripts in the user message handler to declare if the message being processed
//! should be prevented from the normal forwarding to linked recipients. When each message is received
//! the suppression flag is set to false. The script message handler can then call SupressMessage() to
//! suppress the eventual forwarding of the message.
//! being processed should be forwarded to linked recipients. When each message is received,
//! the flag is cleared that
//!
//! @param aSuppressMessage If true, suppress the forwarding of the message to linked recipients.
void WsfScriptProcessor::SuppressMessage(bool aSuppressMessage)
{
   mSuppressMessage = aSuppressMessage;
}

// ================================================================================================
void WsfScriptProcessor::SetPlatform(WsfPlatform* aPlatformPtr)
{
   WsfProcessor::SetPlatform(aPlatformPtr);
   // Set the context's parent as soon as possible.
   // This allows scripts compiled later to use the parent's variables.
   if (aPlatformPtr != nullptr)
   {
      mContextPtr->SetParent(&aPlatformPtr->GetScriptContext());
   }
}

WsfAdvancedBehaviorTree* WsfScriptProcessor::AdvancedBehaviorTree() const
{
   if (mStateMachinePtr != nullptr)
   {
      return mStateMachinePtr->GetState(mStateIndex)->AdvancedBehaviorTree();
   }

   return mAdvancedBehaviorTreePtr.get();
}

// ================================================================================================
UtScriptContext* WsfScriptProcessor::GetScriptAccessibleContext() const
{
   return &mContextPtr->GetContext();
}

// ================================================================================================
std::string WsfScriptProcessor::State()
{
   if (mStateMachinePtr != nullptr && mStateIndex > 0) // state machine is 1-based indexing
   {
      return mStateMachinePtr->GetStateName(mStateIndex);
   }
   return "";
}

// ================================================================================================
std::string WsfScriptProcessor::StateAll()
{
   std::string            stateNameAll    = "";
   WsfScriptStateMachine* stateMachinePtr = mStateMachinePtr.get();
   int                    stateIndex      = mStateIndex;
   while (stateMachinePtr != nullptr && stateIndex > 0) // state machine is 1-based indexing
   {
      if (!stateNameAll.empty())
      {
         stateNameAll += "::";
      }
      stateNameAll += stateMachinePtr->GetStateName(stateIndex);

      int tempIndex   = stateMachinePtr->GetState(stateIndex)->ChildStateIndex();
      stateMachinePtr = stateMachinePtr->GetState(stateIndex)->ChildStateMachine();
      stateIndex      = tempIndex;
   }
   return stateNameAll;
}

// ================================================================================================
void WsfScriptProcessor::SetState(std::string& stateName)
{
   if (mStateMachinePtr != nullptr)
   {
      int index = mStateMachinePtr->GetStateIndex(stateName);
      if (index > 0 && index != mStateIndex)
      {
         mStateMachinePtr->ExitState(mStateIndex);
         mStateMachinePtr->EnterState(index);
         mStateIndex = index;
      }
   }
}

// ================================================================================================
void WsfScriptProcessor::SetStateAll(std::string& stateNameAll)
{
   std::vector<std::string> stateNames;
   UtStringUtil::Parse(stateNameAll, stateNames, "::");

   if (mStateMachinePtr != nullptr && mStateIndex > 0 && !stateNames.empty())
   {
      // handle the processor owned [parent] state machine first:
      int index = mStateMachinePtr->GetStateIndex(stateNames[0]);
      if (index > 0 && index != mStateIndex)
      {
         mStateMachinePtr->ExitState(mStateIndex);
         mStateMachinePtr->EnterState(index);
         mStateIndex = index;
      }

      // now handle the child state machines (if exist):
      int                           nameIndex       = 1; // start at 1, already processed 0 for parent state machine
      WsfScriptStateMachine::State* statePtr        = mStateMachinePtr->GetState(mStateIndex);
      WsfScriptStateMachine*        stateMachinePtr = statePtr->ChildStateMachine();
      int                           curStateIndex   = statePtr->ChildStateIndex();

      while (statePtr != nullptr && stateMachinePtr != nullptr && curStateIndex > 0 && nameIndex < (int)stateNames.size())
      {
         std::string stateName     = stateNames[nameIndex];
         int         newStateIndex = stateMachinePtr->GetStateIndex(stateName);
         if (newStateIndex <= 0)
         {
            break;
         }

         if (newStateIndex != curStateIndex)
         {
            stateMachinePtr->ExitState(curStateIndex);
            stateMachinePtr->EnterState(newStateIndex);
            statePtr->SetChildStateIndex(newStateIndex);
            curStateIndex = newStateIndex;
         }
         statePtr        = stateMachinePtr->GetState(curStateIndex);
         stateMachinePtr = statePtr->ChildStateMachine();
         curStateIndex   = statePtr->ChildStateIndex();
         ++nameIndex;
      }
   }
}
