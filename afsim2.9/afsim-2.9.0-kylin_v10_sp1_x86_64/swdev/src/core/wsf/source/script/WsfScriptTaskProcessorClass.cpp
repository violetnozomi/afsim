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

#include "script/WsfScriptTaskProcessorClass.hpp"

#include "UtMemory.hpp"
#include "WsfTaskProcessor.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackStateController.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptTaskManagerClass.hpp"

WsfScriptTaskProcessorClass::WsfScriptTaskProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptTaskManagerClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTaskProcessor");
   AddMethod(ut::make_unique<CountInState>());
   AddMethod(ut::make_unique<EnterState>());
   AddMethod(ut::make_unique<TracksInState>());
   AddMethod(ut::make_unique<StateOfTrack>());
   AddMethod(ut::make_unique<SetEvaluationInterval>());
   AddMethod(ut::make_unique<SetTransitionTime>());
}

//! int count = \<x\>.CountInState(string aStateName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskProcessorClass, WsfTaskProcessor, CountInState, 1, "int", "string")
{
   aReturnVal.SetInt(aObjectPtr->GetTrackStateController().TracksInState((WsfStringId)aVarArgs[0].GetString()));
}

//! \<x\>.EnterState(WsfTrackId aTrackId, string aStateName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskProcessorClass, WsfTaskProcessor, EnterState, 2, "void", "WsfTrackId, string")
{
   WsfTrackId* trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   double      simTime    = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->GetTrackStateController().EnterState(simTime, *trackIdPtr, (WsfStringId)aVarArgs[1].GetString());
}

//! Array<WsfLocalTrack> trackList = \<x\>.TracksInState(string aStateName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskProcessorClass, WsfTaskProcessor, TracksInState, 1, "Array<WsfLocalTrack>", "string")
{
   UtScriptClass*              classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   std::vector<WsfLocalTrack*> trackList;
   aObjectPtr->GetTrackStateController().TracksInState((WsfStringId)aVarArgs[0].GetString(), trackList);
   std::vector<UtScriptData>* arrayPtr = new std::vector<UtScriptData>();
   for (unsigned int i = 0; i < trackList.size(); ++i)
   {
      arrayPtr->push_back(UtScriptData(new UtScriptRef(trackList[i], classPtr)));
   }
   aReturnVal.SetPointer(new UtScriptRef(arrayPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! \<x\>.StateOfTrack(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskProcessorClass, WsfTaskProcessor, StateOfTrack, 1, "string", "WsfTrackId")
{
   WsfTrackId* trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetString(aObjectPtr->GetTrackStateController().StateOfTrack(*trackIdPtr));
}

//! Dynamically redefine the evaluation interval for determine when the next evaluation should occur.
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskProcessorClass, WsfTaskProcessor, SetEvaluationInterval, 1, "void", "double")
{
   aObjectPtr->GetTrackStateController().SetEvaluationInterval(aVarArgs[0].GetDouble());
}

//! Set the time to transition into the next state (if a transition occurs)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskProcessorClass, WsfTaskProcessor, SetTransitionTime, 1, "void", "double")
{
   aObjectPtr->GetTrackStateController().SetTransitionTime(aVarArgs[0].GetDouble());
}
