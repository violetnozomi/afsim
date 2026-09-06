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

#include "script/WsfScriptStateMachine.hpp"

#include <algorithm>
#include <memory>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptContext.hpp"
#include "WsfAdvancedBehaviorTree.hpp"
#include "WsfAdvancedBehaviorTreeNode.hpp"
#include "WsfBehaviorTree.hpp"
#include "WsfBehaviorTreeNode.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptContext.hpp"

// Set an initial value for the static id counter
size_t WsfScriptStateMachine::mNextUniqueId = 0;

//! Constructor.
//! @param aContext The script context in which scripts are to be compiled and executed.
//! @param aParentProcPtr Pointer to the parent processor of the state machine
//! @param aParentNameSpace The string name ID of the parent processor namespace
WsfScriptStateMachine::WsfScriptStateMachine(WsfScriptContext&   aContext,
                                             WsfScriptProcessor* aParentProcPtr,
                                             WsfStringId         aParentNameSpace)
   : mContextPtr(&aContext)
   , mParentNameSpace(aParentNameSpace)
   , mStateList()
   , mShowStateEvaluations(false)
   , mShowStateTransitions(false)
   , mParentProcessorPtr(aParentProcPtr)
   , mId(GetUniqueId())
{
}

//! Copy constructor.
//! @param aSrc     The source state machine.
//! @param aContext The script context in which scripts are to be compiled and executed.
//! @param aParentProcPtr Pointer to the parent processor of the state machine
WsfScriptStateMachine::WsfScriptStateMachine(const WsfScriptStateMachine& aSrc,
                                             WsfScriptContext&            aContext,
                                             WsfScriptProcessor*          aParentProcPtr)
   : mContextPtr(&aContext)
   , mParentNameSpace(aSrc.mParentNameSpace)
   , mStateList()
   , mShowStateEvaluations(aSrc.mShowStateEvaluations)
   , mShowStateTransitions(aSrc.mShowStateTransitions)
   , mParentProcessorPtr(aParentProcPtr)
   , mId(GetUniqueId()) // For some reason when this copies over I can't use aSrc.mId, it always returns 1?
{
   // Clone each of the states.
   for (const auto& state : aSrc.mStateList)
   {
      mStateList.emplace_back(state->Clone(*mContextPtr));
   }
}

//! Create an empty State object.
//! A derived class must override this method to create their specific type of State object.
// virtual
std::unique_ptr<WsfScriptStateMachine::State> WsfScriptStateMachine::CreateState(WsfStringId       aStateName,
                                                                                 WsfScriptContext& aContext)
{
   return ut::make_unique<State>(aStateName, aContext);
}

//! Evaluate transition rules.
//! This method applies the transition rules for the current state to determine if a state transition
//! should occur. If a transition is to occur, the follow steps occur:
//! - the 'on_exit' script for the current state (if defined) is executed.
//! - the 'on_entry' script for the new state (if defined) is executed.
//! - the transition rules for the new state are executed.
//! If any transition rule in the new state then the exit/entry/evaluate process is repeated. This
//! continues until no transition occurs.
//! @param aCurrentStateIndex The current state (index) within the machine. This should be zero on the
//! initial call.
//! @return The resulting state within in the machine. This may be the same as aCurrentStateIndex
//! if no transitions occur.
// virtual
int WsfScriptStateMachine::Evaluate(int aCurrentStateIndex)
{
   // On the first time though we implicitly enter the first state...
   State* statePtr = nullptr;
   if (aCurrentStateIndex <= 0)
   {
      statePtr = GetState(GetInitialStateIndex());

      if (mContextPtr != nullptr)
      {
         WsfObserver::StateMachineState(
            mContextPtr->GetSimulation())(mContextPtr->GetSimulation()->GetSimTime(), this, 0, GetInitialStateIndex());
      }

      EnterState(statePtr);
   }
   else
   {
      // Find the selected state
      statePtr = GetState(aCurrentStateIndex);
      if (statePtr == nullptr)
      {
         auto out = ut::log::error() << "WsfScriptStateMachine: Invalid entry state index.";
         out.AddNote() << "Index: " << aCurrentStateIndex;
         out.AddNote() << "Limit: " << mStateList.size();
         return 0; // Restart in the initial state...
      }
   }

   // Perform transitions until no more can be performed.
   int  nextStateIndex;
   bool hasTransitioned = false;
   while ((nextStateIndex = statePtr->Evaluate(mShowStateEvaluations)) != statePtr->GetStateIndex())
   {
      ExitState(statePtr); // Exit the current state

      // Find the pointer to the state in to which we are to transition.
      State* nextStatePtr = GetState(nextStateIndex);
      if (nextStatePtr == nullptr)
      {
         auto out = ut::log::error() << "WsfScriptStateMachine: Invalid transition state index.";
         out.AddNote() << "Index: " << nextStateIndex;
         out.AddNote() << "Limit: " << mStateList.size();
         break;
      }

      if (mContextPtr != nullptr)
      {
         mTransitionToSelfMessage = false;
         WsfObserver::StateMachineState(mContextPtr->GetSimulation())(mContextPtr->GetSimulation()->GetSimTime(),
                                                                      this,
                                                                      statePtr->GetStateIndex(),
                                                                      nextStateIndex);
      }

      statePtr = nextStatePtr;
      EnterState(statePtr); // Enter the new state
      hasTransitioned = true;
   }

   // no transition
   if (!hasTransitioned)
   {
      // Check to see if a one time state message transitioning to self needs to be sent
      if (!mTransitionToSelfMessage && mContextPtr != nullptr)
      {
         WsfObserver::StateMachineState(mContextPtr->GetSimulation())(mContextPtr->GetSimulation()->GetSimTime(),
                                                                      this,
                                                                      statePtr->GetStateIndex(),
                                                                      statePtr->GetStateIndex());
         mTransitionToSelfMessage = true;
      }
   }

   return statePtr->GetStateIndex();
}

//! Evaluate the transition rules for the current state only.
//! This is like Evaluate() except that it will not cascade through multiple states. This method allows
//! the caller to detect each and every state change and maintain any information they deem necessary.
//!
//! If evaluating the rules for the current state causes a transition then the 'on_exit' for the current state
//! will be executed (if it is defined). If 'aDoNewStateEntry' is true and the 'on_entry' for the new state is
//! defined, it will be executed but no transition rules will be executed in the new state.
//!
//! @param aCurrentStateIndex The current state (index) within the machine. This should be zero on the
//! initial call.
//!
//! @param aDoNewStateEntry This is used only if the evaluation indicates a transition should occur. If 'true',
//! the OnEntry method will be executed for the new state. If 'false' it will be the responsibility of the
//! caller to invoke the OnEntry method at the appropriate time.
//!
//! @return The resulting state within in the machine. This may be the same as aCurrentStateIndex
//! if a transition does not occur.
int WsfScriptStateMachine::EvaluateState(int aCurrentStateIndex, bool aDoNewStateEntry /* = true */)
{
   // If the supplied index was 0 then we immediately transition into the first state.  We count this as a
   // state transition and immediately exit. The caller then has the option to call us again to force the
   // transition rules to be evaluated.

   State* statePtr = nullptr;
   if (aCurrentStateIndex <= 0)
   {
      statePtr = GetState(GetInitialStateIndex());
      if (aDoNewStateEntry)
      {
         if (mContextPtr != nullptr)
         {
            WsfObserver::StateMachineState(
               mContextPtr->GetSimulation())(mContextPtr->GetSimulation()->GetSimTime(), this, 0, GetInitialStateIndex());
         }

         EnterState(statePtr);
      }
      return statePtr->GetStateIndex();
   }

   // Find the selected state
   statePtr = GetState(aCurrentStateIndex);
   if (statePtr == nullptr)
   {
      auto out = ut::log::error() << "WsfScriptStateMachine: Invalid entry state index.";
      out.AddNote() << "Index: " << aCurrentStateIndex;
      out.AddNote() << "Limit: " << mStateList.size();
      return 0; // Restart in the initial state...
   }

   // Evaluate the state and determine if a transition should occur.
   int nextStateIndex;
   if ((nextStateIndex = statePtr->Evaluate(mShowStateEvaluations)) != statePtr->GetStateIndex())
   {
      ExitState(statePtr); // Exit the current state

      // Find the pointer to the state in to which we are to transition.
      State* nextStatePtr = GetState(nextStateIndex);
      if (nextStatePtr == nullptr)
      {
         auto out = ut::log::error() << "WsfScriptStateMachine: Invalid transition state index.";
         out.AddNote() << "Index: " << nextStateIndex;
         out.AddNote() << "Limit: " << mStateList.size();
         return statePtr->GetStateIndex(); // Stay in the current state
      }

      if (mContextPtr != nullptr)
      {
         mTransitionToSelfMessage = false;
         WsfObserver::StateMachineState(mContextPtr->GetSimulation())(mContextPtr->GetSimulation()->GetSimTime(),
                                                                      this,
                                                                      statePtr->GetStateIndex(),
                                                                      nextStateIndex);
      }

      statePtr = nextStatePtr;
      if (aDoNewStateEntry)
      {
         EnterState(statePtr); // Enter the new state
      }
   }
   else // no state transition
   {
      // Don't need the hasTransitioned bool here since it's an if not a while
      // Check to see if a one time state message transitioning to self needs to be sent
      if (!mTransitionToSelfMessage && mContextPtr != nullptr)
      {
         WsfObserver::StateMachineState(mContextPtr->GetSimulation())(mContextPtr->GetSimulation()->GetSimTime(),
                                                                      this,
                                                                      statePtr->GetStateIndex(),
                                                                      statePtr->GetStateIndex());
         mTransitionToSelfMessage = true;
      }
   }

   return statePtr->GetStateIndex();
}

//! An external method to perform the actions associated with entering a state.
//! This method is provided to allow the caller to control state transitions explicitly.
//! @param aStateIndex The index of the state that is being entered.
// virtual
void WsfScriptStateMachine::EnterState(int aStateIndex)
{
   // Find and exit the selected state
   State* statePtr = GetState(aStateIndex);
   if (statePtr != nullptr)
   {
      EnterState(statePtr);
   }
}

//! An internal method to perform the operations associated with entering a state.
//! @param aStatePtr The state that is being entered.
// protected virtual
void WsfScriptStateMachine::EnterState(State* aStatePtr)
{
   if (mShowStateTransitions)
   {
      auto out = ut::log::debug() << "WsfScriptStateMachine: Entering state.";
      out.AddNote() << "State: " << aStatePtr->GetStateName();
   }
   EnterStateNotify(aStatePtr->GetStateIndex());
   aStatePtr->EnterState();
}

//! An external method to perform the actions associated with exiting a state.
//! This method is provided to allow the caller to control state transitions explicitly.
//! @param aStateIndex The index of the state that is being left.
// virtual
void WsfScriptStateMachine::ExitState(int aStateIndex)
{
   // Find and exit the selected state
   State* statePtr = GetState(aStateIndex);
   if (statePtr != nullptr)
   {
      ExitState(statePtr);
   }
}

//! An internal method to perform the operations associated with exiting a state.
//! @param aStatePtr The state that is being exited.
// protected virtual
void WsfScriptStateMachine::ExitState(State* aStatePtr)
{
   if (mShowStateTransitions)
   {
      auto out = ut::log::debug() << "WsfScriptStateMachine: Exiting state.";
      out.AddNote() << "State: " << aStatePtr->GetStateName();
   }
   aStatePtr->ExitState();
   ExitStateNotify(aStatePtr->GetStateIndex());
}

//! Find a State object given its state name (ID).
//! @param aStateName The desired state name (ID).
//! @returns A pointer to the State object or 0 if the requested state does not exist.
WsfScriptStateMachine::State* WsfScriptStateMachine::FindState(WsfStringId aStateName) const
{
   auto it = std::find_if(std::begin(mStateList),
                          std::end(mStateList),
                          [aStateName](const std::unique_ptr<State>& aState)
                          { return (aState->GetStateName() == aStateName); });

   if (it != std::end(mStateList))
   {
      return it->get();
   }
   return nullptr; // The requested state was not found
}

//! Get the state index of the first state in the machine.
//! For now the return value will be 1, but this allows the provision to define a machine where the
//! initial state is something other than the first state.
int WsfScriptStateMachine::GetInitialStateIndex() const
{
   return mStateList[0]->GetStateIndex();
}

//! Return a State object given its state index.
//! @param aStateIndex The desired state index.
//! @returns A pointer to the State object or nullptr if the requested state does not exist.
WsfScriptStateMachine::State* WsfScriptStateMachine::GetState(int aStateIndex) const
{
   if ((aStateIndex > 0) && (aStateIndex <= static_cast<int>(mStateList.size())))
   {
      return mStateList[aStateIndex - 1].get();
   }
   else if (aStateIndex == 0)
   {
      return mStateList[0].get();
   }
   return nullptr;
}

//! Return the index of the state with the specified name (ID).
//! @param aStateName The string ID of the state whose index is to be returned.
//! @returns The index of the state with the specified name or 0 if the name does not
//! represent a valid state.
int WsfScriptStateMachine::GetStateIndex(WsfStringId aStateName) const
{
   int    stateIndex = 0;
   State* statePtr   = FindState(aStateName);
   if (statePtr != nullptr)
   {
      stateIndex = statePtr->GetStateIndex();
   }
   return stateIndex;
}

//! Return the name (ID) of a state given its state index.
WsfStringId WsfScriptStateMachine::GetStateName(int aStateIndex) const
{
   WsfStringId stateName = nullptr;
   State*      statePtr  = GetState(aStateIndex);
   if (statePtr != nullptr)
   {
      stateName = statePtr->GetStateName();
   }
   return stateName;
}

//! Initialize the state machine.
//! @param aSimTime The current simulation time.
//! @return 'true' if successful or 'false' if the state machine could not be initialized.
// virtual
bool WsfScriptStateMachine::Initialize(double aSimTime)
{
   if (mStateList.empty())
   {
      ut::log::error() << "WsfScriptStateMachine: Empty state machine.";
      return false;
   }

   bool ok = true;

   auto out = ut::log::error() << "WsfScriptStateMachine: Unable to initialize states.";
   for (const auto& statePtr : mStateList)
   {
      if (!statePtr->Initialize(aSimTime, *this, *mContextPtr))
      {
         ok = false;
         out.AddNote() << "State: " << statePtr->GetStateName();
      }
   }
   if (ok)
   {
      out.Clear();
   }

   if (mContextPtr != nullptr)
   {
      WsfObserver::StateMachine(mContextPtr->GetSimulation())(mContextPtr->GetSimulation()->GetSimTime(), this);
   }

   return ok;
}

// virtual
bool WsfScriptStateMachine::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "state")
   {
      UtInputBlock inputBlock(aInput);
      std::string  stateName;
      aInput.ReadValue(stateName);
      WsfStringId stateNameId = stateName;
      if (FindState(stateNameId) != nullptr)
      {
         throw UtInput::BadValue(aInput, "Duplicate state name");
      }
      auto statePtr = CreateState(stateNameId, *mContextPtr);

      while (inputBlock.ReadCommand())
      {
         if (!statePtr->ProcessInput(aInput, mParentNameSpace))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      statePtr->SetStateIndex(static_cast<int>(mStateList.size() + 1));
      mStateList.push_back(std::move(statePtr));
   }
   else if (command == "show_state_evaluations")
   {
      mShowStateEvaluations = true;
   }
   else if (command == "show_state_transitions")
   {
      mShowStateTransitions = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// ***********************************************************************************************
// Nested class WsfScriptStateMachine::State
// ***********************************************************************************************

WsfScriptStateMachine::State::State(WsfStringId aStateName, WsfScriptContext& aContext)
   : mStateName(aStateName)
   , mStateIndex(0)
   , mContextPtr(&aContext)
   , mOnEntryScriptPtr(nullptr)
   , mOnEntryScriptName(nullptr)
   , mOnExitScriptPtr(nullptr)
   , mOnExitScriptName(nullptr)
   , mTransitionList()
   , mBehaviorTreePtr(nullptr)
   , mChildFiniteStateMachinePtr(nullptr)
   , mChildStateIndex(0)
{
}

WsfScriptStateMachine::State::State(const State& aSrc, WsfScriptContext& aContext)
   : mStateName(aSrc.mStateName)
   , mStateIndex(aSrc.mStateIndex)
   , mContextPtr(&aContext)
   , mOnEntryScriptPtr(nullptr)
   , mOnEntryScriptName(aSrc.mOnEntryScriptName)
   , mOnExitScriptPtr(nullptr)
   , mOnExitScriptName(aSrc.mOnExitScriptName)
   , mTransitionList()
   , mBehaviorTreePtr(nullptr)
   , // set below if aSrc has one to clone
   mAdvancedBehaviorTreePtr(aSrc.mAdvancedBehaviorTreePtr)
   , // clone the behavior tree in this state (if one exists)
   mChildFiniteStateMachinePtr(nullptr)
   , // set below if aSrc has one to clone
   mChildStateIndex(aSrc.mChildStateIndex)
{
   // clone the behavior tree in this state (if one exists)
   mBehaviorTreePtr = (aSrc.mBehaviorTreePtr != nullptr) ? (aSrc.mBehaviorTreePtr->Clone()) : (nullptr);

   // clone the child state machine in this state (if one exists)
   if (aSrc.mChildFiniteStateMachinePtr != nullptr)
   {
      mChildFiniteStateMachinePtr = new WsfScriptStateMachine(*aSrc.mChildFiniteStateMachinePtr, *mContextPtr);
   }

   // Clone each of the state transition objects
   for (const auto& transition : aSrc.mTransitionList)
   {
      mTransitionList.emplace_back(transition->Clone());
   }
}

// virtual
WsfScriptStateMachine::State::~State()
{
   if (mBehaviorTreePtr != nullptr)
   {
      delete mBehaviorTreePtr;
   }
   if (mChildFiniteStateMachinePtr != nullptr)
   {
      delete mChildFiniteStateMachinePtr;
   }
}

//! Create a clone of the state.
//! This must be provided by a derived class if they extend State.
// virtual
WsfScriptStateMachine::State* WsfScriptStateMachine::State::Clone(WsfScriptContext& aContext) const
{
   return new State(*this, aContext);
}

//! A virtual constructor for Transition objects.
//! This must be provided by derived classes if they extend Transition.
// virtual
std::unique_ptr<WsfScriptStateMachine::Transition> WsfScriptStateMachine::State::CreateTransition(WsfStringId aNextStateName) const
{
   return ut::make_unique<Transition>(aNextStateName);
}

//! Removes the specified transition from the state
void WsfScriptStateMachine::State::RemoveTransition(WsfStringId aNextStateName)
{
   auto tli    = mTransitionList.begin();
   auto tliEnd = mTransitionList.end();
   for (; tli != tliEnd; ++tli)
   {
      if ((*tli)->GetNextStateName() == aNextStateName)
      {
         mTransitionList.erase(tli);
         break;
      }
   }
}

//! Execute the script associated with entering the state.
// virtual
void WsfScriptStateMachine::State::EnterState()
{
   if (mOnEntryScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      mContextPtr->ExecuteScript(mOnEntryScriptPtr, scriptRetVal, scriptArgs);
   }

   if (mChildFiniteStateMachinePtr != nullptr)
   {
      mChildStateIndex = mChildFiniteStateMachinePtr->GetInitialStateIndex();

      if (mContextPtr != nullptr)
      {
         WsfObserver::StateMachineState(mContextPtr->GetSimulation())(mContextPtr->GetSimulation()->GetSimTime(),
                                                                      mChildFiniteStateMachinePtr,
                                                                      0,
                                                                      mChildStateIndex);
      }

      mChildFiniteStateMachinePtr->EnterState(
         mChildStateIndex); // enter the initial child state when a parent state is entered
   }
}

//! Evaluate the transition rules associated with the state.
//! The rules are evaluated in the order in which they are defined.
//! Control returns to the caller when the first rule that would cause a transition is detected.
//! @return The state index of the next state to be executed.  If no transition occurs then
//! it simply returns the state index of the current state.
// virtual
int WsfScriptStateMachine::State::Evaluate(bool aShowStateEvaluations)
{
   if (aShowStateEvaluations)
   {
      auto out = ut::log::debug() << "WsfScriptStateMachine: Evaluating state.";
      out.AddNote() << "State: " << GetStateName();
   }

   if (mBehaviorTreePtr != nullptr)
   {
      mBehaviorTreePtr->Execute(mBehaviorTreePtr->GetSimulation()->GetSimTime());
   }

   if (mAdvancedBehaviorTreePtr != nullptr)
   {
      mAdvancedBehaviorTreePtr->Tick(mAdvancedBehaviorTreePtr->GetSimulation()->GetSimTime());
   }

   // Evaluate each transition rule in order...
   for (const auto& transitionPtr : mTransitionList)
   {
      if (transitionPtr->Evaluate(*mContextPtr))
      {
         if (aShowStateEvaluations)
         {
            auto out = ut::log::debug() << "WsfScriptStateMachine: Rule returned true.";
            out.AddNote() << "Rule: " << transitionPtr->GetNextStateName();
         }

         int nextIndex = transitionPtr->GetNextStateIndex();
         if (nextIndex != mStateIndex)
         {
            return nextIndex;
         }
         else
         {
            break; // allow the Child FSM to run below
         }
      }

      if (aShowStateEvaluations)
      {
         auto out = ut::log::debug() << "WsfScriptStateMachine: Rule returned false.";
         out.AddNote() << "Rule: " << transitionPtr->GetNextStateName();
      }
   }

   // only process child states if we remain in this parent state
   if (mChildFiniteStateMachinePtr != nullptr)
   {
      mChildStateIndex =
         mChildFiniteStateMachinePtr->EvaluateState(mChildStateIndex); // only does one state transition (if necessary)
   }

   return mStateIndex; // None of the rules would cause a transition.
}

//! Execute the script associated with exiting the state.
// virtual
void WsfScriptStateMachine::State::ExitState()
{
   if (mOnExitScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      mContextPtr->ExecuteScript(mOnExitScriptPtr, scriptRetVal, scriptArgs);
   }
}

//! Initialize the state.
//! @param aSimTime The current simulation time.
//! @param aStateMachine The state machine with which the state is associated.
//! @param aParentContext The script context to which this state context is to be attached.
//! @returns true if successful or false if not.
// virtual
bool WsfScriptStateMachine::State::Initialize(double                 aSimTime,
                                              WsfScriptStateMachine& aStateMachine,
                                              WsfScriptContext&      aParentContext)
{
   bool ok = true;

   if (mBehaviorTreePtr != nullptr)
   {
      // attempt to initialize the behavior tree, then let the state finish its initialize
      ok = mBehaviorTreePtr->Initialize(aSimTime, aStateMachine.Processor(), mContextPtr);
   }

   if (mAdvancedBehaviorTreePtr != nullptr)
   {
      // attempt to initialize the advanced behavior tree, then let the state finish its initialize
      ok &= mAdvancedBehaviorTreePtr->Initialize(aSimTime, aStateMachine.Processor(), mContextPtr);
   }

   if (mChildFiniteStateMachinePtr != nullptr)
   {
      mChildFiniteStateMachinePtr->SetProcessor(aStateMachine.Processor());
      mChildFiniteStateMachinePtr->Initialize(aSimTime);
   }

   // Ensure that each transition refers to a defined state.
   for (const auto& transitionPtr : mTransitionList)
   {
      if (!transitionPtr->Initialize(aSimTime, aParentContext))
      {
         ok       = false;
         auto out = ut::log::error() << "WsfScriptStateMachine: Unable to initialize 'next_state'.";
         out.AddNote() << "Current State: " << mStateName;
         out.AddNote() << "Next State: " << transitionPtr->GetNextStateName();
      }

      // Ensure the desired transition state exists in the machine.
      State* nextStatePtr = aStateMachine.FindState(transitionPtr->GetNextStateName());
      if (nextStatePtr != nullptr)
      {
         transitionPtr->SetNextStateIndex(nextStatePtr->GetStateIndex());
      }
      else
      {
         ok       = false;
         auto out = ut::log::error() << "WsfScriptStateMachine: 'next_state' does not exist.";
         out.AddNote() << "Current State: " << mStateName;
         out.AddNote() << "Next State: " << transitionPtr->GetNextStateName();
      }
   }

   // Capture the pointers to the on_entry and on_exit scripts for faster access at execution time.
   if (mOnEntryScriptName != 0)
   {
      mOnEntryScriptPtr = mContextPtr->FindScript(mOnEntryScriptName);
   }
   if (mOnExitScriptName != 0)
   {
      mOnExitScriptPtr = mContextPtr->FindScript(mOnExitScriptName);
   }
   return ok;
}

//! Process input for the state.
//! @param aInput The input stream.
//! @param aNameSpace The string name ID of the state namespace
//! @returns true if a command was recognized (and processed) or false if the command was not
//! one recognized.
// virtual
bool WsfScriptStateMachine::State::ProcessInput(UtInput& aInput, WsfStringId aNameSpace)
{
   std::string nameSpace =
      (!aNameSpace.IsNull()) ? (aNameSpace.GetString() + "::" + mStateName.GetString()) : (mStateName.GetString());
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "on_entry")
   {
      std::string endBlockToken("end_script");
      std::string nextWord;
      aInput.ReadValue(nextWord);
      if (nextWord != "script")
      {
         aInput.PushBack(nextWord);
         endBlockToken = "end_on_entry";
      }
      UtScript* scriptPtr = mContextPtr->Compile(command, "void", aInput, endBlockToken, nameSpace);
      mOnEntryScriptName  = scriptPtr->GetName();
   }
   else if (command == "on_exit")
   {
      std::string endBlockToken("end_script");
      std::string nextWord;
      aInput.ReadValue(nextWord);
      if (nextWord != "script")
      {
         aInput.PushBack(nextWord);
         endBlockToken = "end_on_exit";
      }
      UtScript* scriptPtr = mContextPtr->Compile(command, "void", aInput, endBlockToken, nameSpace);
      mOnExitScriptName   = scriptPtr->GetName();
   }
   else if (command == "next_state")
   {
      UtInputBlock inputBlock(aInput);
      std::string  nextStateName;
      aInput.ReadValue(nextStateName);
      WsfStringId nextStateNameId = nextStateName;
      // Make sure the transition name is not a duplicate
      for (TransitionList::const_iterator tli = mTransitionList.begin(); tli != mTransitionList.end(); ++tli)
      {
         if (nextStateNameId == (*tli)->GetNextStateName())
         {
            throw UtInput::BadValue(aInput, "Duplicate next_state " + nextStateName + " in state " + mStateName);
         }
      }
      auto transitionPtr = CreateTransition(nextStateNameId);
      while (inputBlock.ReadCommand())
      {
         if (!transitionPtr->ProcessInput(aInput, *mContextPtr, nameSpace))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mTransitionList.push_back(std::move(transitionPtr));
   }
   else if (command == "behavior_tree")
   {
      if (mBehaviorTreePtr != nullptr)
      {
         delete mBehaviorTreePtr;
      }
      mBehaviorTreePtr = new WsfBehaviorTree(WsfScenario::FromInput(aInput));
      mBehaviorTreePtr->ProcessInput(aInput);
   }
   else if (command == "advanced_behavior_tree")
   {
      mAdvancedBehaviorTreePtr = ut::make_unique<WsfAdvancedBehaviorTree>(WsfScenario::FromInput(aInput));
      mAdvancedBehaviorTreePtr->ProcessInput(aInput);
   }
   else if (command == "edit")
   {
      std::string thing;
      aInput.ReadCommand(thing);
      if (thing == "behavior")
      {
         // this is a local edit of an existing behavior(s) on the tree
         // only edit the nodes(s) of this name on this processor's behavior tree, NOT the global definition of the behavior
         if (mBehaviorTreePtr != nullptr)
         {
            // change all nodes of the name given
            UtInput::StoredLocation editStartLoc = aInput.StoreLocation();
            std::string             behaviorName;
            aInput.ReadValue(behaviorName);
            // don't forget: behavior names are object types, and behavior types are object names
            bool editedAtLeastOneBehavior = false;
            for (size_t i = 0; i < mBehaviorTreePtr->NodeCount(); ++i)
            {
               WsfBehaviorTreeLeafNode* nodePtr = mBehaviorTreePtr->NodeEntry(i);
               if (nodePtr->GetType() == behaviorName)
               {
                  aInput.RestoreLocation(editStartLoc);
                  editedAtLeastOneBehavior |= nodePtr->ProcessInput(aInput);
               }
            }

            if (!editedAtLeastOneBehavior)
            {
               std::string msg = "error: local state edit not possible, behavior leaf node not found: " + behaviorName;
               throw UtInput::BadValue(aInput, msg);
            }
         }
         else
         {
            throw UtInput::BadValue(aInput, "error: local state edit not possible, behavior tree not found!");
         }
      }
      else if (thing == "advanced_behavior")
      {
         // this is a local edit of an existing advanced_behavior(s) on the tree
         // only edit the nodes(s) of this name on this processor's advanced_behavior tree, NOT the global definition of
         // the advanced_behavior
         if (mAdvancedBehaviorTreePtr != nullptr)
         {
            // change all nodes of the name given
            UtInput::StoredLocation editStartLoc = aInput.StoreLocation();
            std::string             behaviorName;
            aInput.ReadValue(behaviorName);
            // don't forget: behavior names are object types, and behavior types are object names
            bool editedAtLeastOneBehavior = false;
            for (size_t i = 0; i < mAdvancedBehaviorTreePtr->NodeCount(); ++i)
            {
               WsfAdvancedBehaviorTreeLeafNode* nodePtr = mAdvancedBehaviorTreePtr->NodeEntry(i);
               if (nodePtr->GetType() == behaviorName)
               {
                  aInput.RestoreLocation(editStartLoc);
                  editedAtLeastOneBehavior |= nodePtr->ProcessInput(aInput);
               }
            }

            if (!editedAtLeastOneBehavior)
            {
               std::string msg =
                  "error: local state edit not possible, advanced_behavior leaf node not found: " + behaviorName;
               throw UtInput::BadValue(aInput, msg);
            }
         }
         else
         {
            throw UtInput::BadValue(aInput, "error: local state edit not possible, advanced behavior tree not found!");
         }
      }
      else
      {
         std::string msg = "error: local edit not valid, can not edit " + thing;
         throw UtInput::BadValue(aInput, msg);
      }
   }
   else if (command == "state" || command == "show_state_evaluations" || command == "show_state_transitions")
   {
      if (mChildFiniteStateMachinePtr == nullptr)
      {
         mChildFiniteStateMachinePtr = new WsfScriptStateMachine(*mContextPtr, nullptr, nameSpace);
      }
      myCommand = mChildFiniteStateMachinePtr->ProcessInput(aInput);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

const WsfScenario* WsfScriptStateMachine::State::GetScenario() const
{
   return mContextPtr->GetScenario();
}

void WsfScriptStateMachine::State::SetChildStateIndex(int stateIndex)
{
   if ((mChildFiniteStateMachinePtr != nullptr) && (mChildFiniteStateMachinePtr->GetState(stateIndex) != nullptr))
   {
      mChildStateIndex = stateIndex;
   }
}


// ***********************************************************************************************
// Nested class WsfScriptStateMachine::Transition
// ***********************************************************************************************

WsfScriptStateMachine::Transition::Transition(WsfStringId aNextStateName)
   : mNextStateName(aNextStateName)
   , mNextStateIndex(0)
   , mScriptName(nullptr)
   , mScriptPtr(nullptr)
{
}

WsfScriptStateMachine::Transition::Transition(const Transition& aSrc)
   : mNextStateName(aSrc.mNextStateName)
   , mNextStateIndex(aSrc.mNextStateIndex)
   , mScriptName(aSrc.mScriptName)
   , mScriptPtr(nullptr)
{
}

//! A virtual 'copy constructor'.
//! This must be provided by derived classes if they extend Transition.
// virtual
WsfScriptStateMachine::Transition* WsfScriptStateMachine::Transition::Clone() const
{
   return new Transition(*this);
}

//! Evaluate the rule to determine if a transition should occur.
//! @param aContext The context for script execution.
//! @returns true if a transition should occur or false if no transition should occur.
// virtual
bool WsfScriptStateMachine::Transition::Evaluate(WsfScriptContext& aContext)
{
   bool doTransition = false;
   if (mScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal(false);
      UtScriptDataList scriptArgs;
      aContext.ExecuteScript(mScriptPtr, scriptRetVal, scriptArgs);
      doTransition = scriptRetVal.GetBool();
   }
   return doTransition;
}

//! Initialize the transition rule.
//! @param aSimTime The current simulation time.
//! @param aContext The context for script execution.
//! @returns true if successful or false if not.
// virtual
bool WsfScriptStateMachine::Transition::Initialize(double aSimTime, WsfScriptContext& aContext)
{
   bool ok = true;
   if (mScriptName != 0)
   {
      // Capture the pointer to the script for faster access at execution time.
      mScriptPtr = aContext.FindScript(mScriptName);
      if (mScriptPtr == nullptr)
      {
         ok       = false;
         auto out = ut::log::error() << "Unable to find script.";
         out.AddNote() << "Script: " << mScriptName;
      }
   }
   return ok;
}

//! Process input for the transition rule.
//! @param aInput The input stream.
//! @param aContext The script context into which the scripts are to be entered.
//! @param aNameSpace The string ID of the current state name.
//! @returns true if a command was recognized (and processed) or false if the command was not
//! one recognized.
// virtual
bool WsfScriptStateMachine::Transition::ProcessInput(UtInput& aInput, WsfScriptContext& aContext, WsfStringId aNameSpace)

{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "script")
   {
      myCommand           = true;
      UtScript* scriptPtr = aContext.Compile(mNextStateName, "bool", aInput, "end_script", aNameSpace);
      mScriptName         = scriptPtr->GetName();
   }
   else
   {
      // This base class implementation assumes that if the command isn't recognized then it is the
      // start of the script and what terminates the script is 'end_next_state'.
      aInput.PushBack(command); // push the command back on to the input stream so it can be compiled
      UtScript* scriptPtr = aContext.Compile(mNextStateName, "bool", aInput, "end_next_state", aNameSpace);
      mScriptName         = scriptPtr->GetName();
      // We must push an 'end_next_state' back on to the input so the enclosing input block can be closed.
      aInput.PushBack("end_next_state");
   }
   return myCommand;
}

// ****************************************************************************

//! Defines the methods in WsfScriptStateMachine for use in script.
class WSF_EXPORT WsfScriptStateMachineClass : public UtScriptClass
{
public:
   WsfScriptStateMachineClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Evaluate);
   UT_DECLARE_SCRIPT_METHOD(EvaluateState);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfScriptStateMachine::CreateScriptClass(const std::string& aClassName,
                                                                        UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptStateMachineClass>(aClassName, aScriptTypesPtr);
}

WsfScriptStateMachineClass::WsfScriptStateMachineClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfStateMachine");

   AddMethod(ut::make_unique<Evaluate>());
   AddMethod(ut::make_unique<EvaluateState>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptStateMachineClass, WsfScriptStateMachine, Evaluate, 1, "string", "string")
{
   // Argument 1: Current state.
   // Return value: updated state.
   int currentStateIndex = aObjectPtr->GetStateIndex(aVarArgs[0].GetString());
   int nextStateIndex    = aObjectPtr->Evaluate(currentStateIndex);
   aReturnVal.SetString(aObjectPtr->GetStateName(nextStateIndex));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptStateMachineClass, WsfScriptStateMachine, EvaluateState, 1, "string", "string")
{
   // Argument 1: Current state.
   // Return value: updated state.
   int currentStateIndex = aObjectPtr->GetStateIndex(aVarArgs[0].GetString());
   int nextStateIndex    = aObjectPtr->EvaluateState(currentStateIndex);
   aReturnVal.SetString(aObjectPtr->GetStateName(nextStateIndex));
}
