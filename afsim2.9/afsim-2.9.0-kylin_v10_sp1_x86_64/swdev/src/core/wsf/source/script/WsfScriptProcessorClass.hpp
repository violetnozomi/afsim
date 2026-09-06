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

#ifndef WSFSCRIPTPROCESSORCLASS_HPP
#define WSFSCRIPTPROCESSORCLASS_HPP

#include "wsf_export.h"

#include "script/WsfScriptPlatformPartClass.hpp"

//! Define script methods for WsfProcessor.
class WSF_EXPORT WsfScriptProcessorClass : public WsfScriptPlatformPartClass
{
public:
   WsfScriptProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(IsA_TypeOf); // static IsA_TypeOf("derived-type", "base-type")

   UT_DECLARE_SCRIPT_METHOD(TurnOff);
   UT_DECLARE_SCRIPT_METHOD(TurnOn);
   UT_DECLARE_SCRIPT_METHOD(UpdateInterval);
   UT_DECLARE_SCRIPT_METHOD(SetUpdateInterval);

   // Script methods

   UT_DECLARE_SCRIPT_METHOD(ScriptExists);
   UT_DECLARE_SCRIPT_METHOD(Execute_1);
   UT_DECLARE_SCRIPT_METHOD(Execute_2);
   UT_DECLARE_SCRIPT_METHOD(ExecuteAtTime_1);
   UT_DECLARE_SCRIPT_METHOD(ExecuteAtTime_2);

   UT_DECLARE_SCRIPT_METHOD(ExecuteScript);
   UT_DECLARE_SCRIPT_METHOD(ExecuteScriptWithArgs);
   UT_DECLARE_SCRIPT_METHOD(SuppressMessage);

   // Behavior Tree methods

   UT_DECLARE_SCRIPT_METHOD(Behavior);
   UT_DECLARE_SCRIPT_METHOD(FindBehavior);
   UT_DECLARE_SCRIPT_METHOD(BehaviorsExecuted);
   UT_DECLARE_SCRIPT_METHOD(BehaviorCount);
   UT_DECLARE_SCRIPT_METHOD(BehaviorEntry);
   UT_DECLARE_SCRIPT_METHOD(BehaviorTreeRootNode);

   UT_DECLARE_SCRIPT_METHOD(AdvancedBehavior);
   UT_DECLARE_SCRIPT_METHOD(FindAdvancedBehavior);
   UT_DECLARE_SCRIPT_METHOD(AdvancedBehaviorsExecuted);
   UT_DECLARE_SCRIPT_METHOD(AdvancedBehaviorCount);
   UT_DECLARE_SCRIPT_METHOD(AdvancedBehaviorEntry);
   UT_DECLARE_SCRIPT_METHOD(AdvancedBehaviorTree);

   // State Machine methods
   UT_DECLARE_SCRIPT_METHOD(State);
   UT_DECLARE_SCRIPT_METHOD(StateAll); // if they exist, includes child states in string - parent[:child:child...]
   UT_DECLARE_SCRIPT_METHOD(SetState);
   UT_DECLARE_SCRIPT_METHOD(SetStateAll); // parses string for child states to set those too - parent:child[:child...]
   UT_DECLARE_SCRIPT_METHOD(GetStates);   ///< Get all top level states
   UT_DECLARE_SCRIPT_METHOD(GetStateAdvancedBehaviorTree); ///< Get the ABT for a state
};

#endif
