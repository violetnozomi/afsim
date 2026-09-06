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

#ifndef WSFSCRIPTSTATEMACHINE_HPP
#define WSFSCRIPTSTATEMACHINE_HPP

#include "wsf_export.h"

#include <memory>
#include <string>
#include <vector>

#include "UtCloneablePtr.hpp"
#include "WsfAdvancedBehaviorTree.hpp"
#include "WsfStringId.hpp"

class UtInput;
class UtScript;
class UtScriptClass;
class UtScriptTypes;
class WsfBehaviorTree;
class WsfScriptContext;
class WsfScriptProcessor;
class WsfScenario;

//! A finite state machine that can be controlled via the scripting language.
//! A object of this class can be used to provide a scriptable finite state machine.
//!
//! This class and the nested classes State and Transition can be extended. If you
//! extend State then you must:
//! - provide a Clone method in your extended State class.
//! - provide a CreateState method in your extended WsfScriptStateMachine class.
//!
//! If you are going to extend Transition then you must also do the above AND
//! - provide a Clone method in your extended Transition class.
//! - provide a CreateTransition method in your extended State class.

class WSF_EXPORT WsfScriptStateMachine
{
public:
   //! A Transition represents a rule that can cause a transition into a new state.
   //! If the Evaluate() method returns true then the state machine should transition
   //! to the new state.
   class WSF_EXPORT Transition
   {
   public:
      Transition() = default;
      explicit Transition(WsfStringId aNextStateName);
      Transition(const Transition& aSrc);
      virtual ~Transition() = default;

      virtual Transition* Clone() const;

      virtual bool Evaluate(WsfScriptContext& aContext);

      int GetNextStateIndex() const { return mNextStateIndex; }

      void SetNextStateIndex(int aNextStateIndex) { mNextStateIndex = aNextStateIndex; }

      WsfStringId GetNextStateName() const { return mNextStateName; }

      virtual bool Initialize(double aSimTime, WsfScriptContext& aContext);

      virtual bool ProcessInput(UtInput& aInput, WsfScriptContext& aContext, WsfStringId aNameSpace);

   protected:
      //! The name (ID) of next state.
      WsfStringId mNextStateName;

      //! The index of the next state in the state machine.
      int mNextStateIndex{0};

      //! The string ID of the name of the script that defines the transition rule.
      WsfStringId mScriptName;

      //! The script that defines the transition rule.
      //! Note that this script is 'owned' by the script context under which it was compiled.
      UtScript* mScriptPtr{nullptr};
   };

   //! A list of transitions.
   using TransitionList = std::vector<std::unique_ptr<Transition>>;

   //! A 'State' represents an allowable state within the machine.
   //! It consists of a set of transitions as well as methods that are called
   //! on entry and exit from the state.
   class WSF_EXPORT State
   {
   public:
      State() = default;
      explicit State(WsfStringId aStateName, WsfScriptContext& aContext);
      State(const State& aSrc) = delete;
      const State& operator=(const State& aRhs) = delete;
      virtual ~State();

      virtual State* Clone(WsfScriptContext& aContext) const;

      virtual std::unique_ptr<Transition> CreateTransition(WsfStringId aNextStateName) const;
      virtual void                        RemoveTransition(WsfStringId aNextStateName);

      TransitionList& GetTransitions() { return mTransitionList; }

      virtual void EnterState();

      virtual int Evaluate(bool aShowStateEvaluations);

      virtual void ExitState();

      //! Return the index of this state within the state machine.
      //! @returns the state index in the range [1..N].
      int GetStateIndex() const { return mStateIndex; }

      //! Set the state index of this state within the state machine.
      void SetStateIndex(int aStateIndex) { mStateIndex = aStateIndex; }

      //! Return the name (ID) of this state.
      WsfStringId GetStateName() const { return mStateName; }

      virtual bool Initialize(double aSimTime, WsfScriptStateMachine& aStateMachine, WsfScriptContext& aContext);

      virtual bool       ProcessInput(UtInput& aInput, WsfStringId aNameSpace = nullptr);
      const WsfScenario* GetScenario() const;

      WsfScriptStateMachine* ChildStateMachine() { return mChildFiniteStateMachinePtr; }
      int                    ChildStateIndex() { return mChildStateIndex; }
      void                   SetChildStateIndex(int stateIndex);

      WsfAdvancedBehaviorTree* AdvancedBehaviorTree() const { return mAdvancedBehaviorTreePtr.get(); }

   protected:
      State(const State& aSrc, WsfScriptContext& aContext);

      //! The name (ID) of this state.
      WsfStringId mStateName;

      //! The index of this state within the state machine.
      int mStateIndex{0};

      //! The script context.
      WsfScriptContext* mContextPtr{nullptr};

      //! The script to be executed when entering the state.
      //! Note that this script is 'owned' by the script context under which it was compiled.
      UtScript*   mOnEntryScriptPtr{nullptr};
      WsfStringId mOnEntryScriptName;

      //! The script to be executed when exiting the state.
      //! Note that this script is 'owned' by the script context under which it was compiled.
      UtScript*   mOnExitScriptPtr{nullptr};
      WsfStringId mOnExitScriptName;

      //! The rules that provide for transitions out of this state.
      TransitionList mTransitionList;

      WsfBehaviorTree* mBehaviorTreePtr{nullptr};

      ut::CloneablePtr<WsfAdvancedBehaviorTree> mAdvancedBehaviorTreePtr{nullptr};

      // child state machine
      WsfScriptStateMachine* mChildFiniteStateMachinePtr{nullptr};
      int                    mChildStateIndex{0};
   };

   //! A collection of states.
   using StateList = std::vector<std::unique_ptr<State>>;

   WsfScriptStateMachine() = default;
   WsfScriptStateMachine(WsfScriptContext&   aContext,
                         WsfScriptProcessor* aParentProcPtr   = nullptr,
                         WsfStringId         aParentNameSpace = nullptr);
   WsfScriptStateMachine(const WsfScriptStateMachine& aSrc,
                         WsfScriptContext&            aContext,
                         WsfScriptProcessor*          aParentProcPtr = nullptr);
   virtual ~WsfScriptStateMachine()     = default;
   const WsfScriptStateMachine& operator=(const WsfScriptStateMachine& aRhs) = delete;
   WsfScriptStateMachine(const WsfScriptStateMachine& aSrc)                  = delete;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   /** Return the id for this state machine. */
   const size_t Id() const { return mId; }

   //! This method is invoked by the base class whenever a state is about to be entered.
   //! @param aStateIndex The index of the state [1..N] that is about to be entered.
   virtual void EnterStateNotify(int aStateIndex) {}

   //! This method is invoked by the base class whenever a state is about to be exited.
   //! @param aStateIndex The index of the state [1..N] that is about to be exit.
   virtual void ExitStateNotify(int aStateIndex) {}

   virtual int Evaluate(int aCurrentStateIndex);

   virtual int EvaluateState(int aCurrentStateIndex, bool aDoNewStateEntry = true);

   virtual void EnterState(int aStateIndex);

   virtual void ExitState(int aStateIndex);

   State* FindState(WsfStringId aStateName) const;

   int GetInitialStateIndex() const;

   State* GetState(int aStateIndex) const;

   //! Return the number of states in the state machine.
   int GetStateCount() const { return static_cast<int>(mStateList.size()); }

   int GetStateIndex(WsfStringId aStateName) const;

   WsfStringId GetStateName(int aStateIndex) const;

   virtual bool Initialize(double aSimTime);

   virtual bool ProcessInput(UtInput& aInput);

   void ShowStateEvaluations(bool aShowStateEvaluations) { mShowStateEvaluations = aShowStateEvaluations; }
   bool ShowStateEvaluations() const { return mShowStateEvaluations; }

   void ShowStateTransitions(bool aShowStateTransitions) { mShowStateTransitions = aShowStateTransitions; }
   bool ShowStateTransitions() const { return mShowStateTransitions; }

   void                SetProcessor(WsfScriptProcessor* aProcessor) { mParentProcessorPtr = aProcessor; }
   WsfScriptProcessor* Processor() { return mParentProcessorPtr; }

protected:
   virtual void EnterState(State* aStatePtr);
   virtual void ExitState(State* aStatePtr);

   virtual std::unique_ptr<State> CreateState(WsfStringId aStateName, WsfScriptContext& aContext);

   //! The context used for compiling and executing scripts.
   WsfScriptContext* mContextPtr{nullptr};
   WsfStringId       mParentNameSpace;

   //! The list of states.
   StateList mStateList;

   bool mShowStateEvaluations{false};
   bool mShowStateTransitions{false};

   WsfScriptProcessor* mParentProcessorPtr{nullptr};
   size_t              mId; ///< Unique integer id of this state machine
private:
   /** Get a unique int id for this state machine. */
   static size_t GetUniqueId() { return ++mNextUniqueId; }

   static size_t mNextUniqueId;          ///< Static counter for unique state machine ids
   bool mTransitionToSelfMessage{false}; ///< Flag that will send a state message when the state transitions to itself
};

#endif
