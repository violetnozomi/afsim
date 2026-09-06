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

#include "WsfLocalTrackStatus.hpp"

#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

WsfLocalTrackStatus::WsfLocalTrackStatus(double aSimTime, WsfSimulation& aSimulation, const WsfLocalTrack* aTrackPtr)
   : mLocalTrackId(aTrackPtr->GetTrackId())
   , mLastStateChangeTime(aSimTime)
   , mStateIndex(0)
   , mStateName(nullptr)
   , mTargetName(nullptr)
   , mEvaluationEpoch(1)
   , mTransitionPending(false)
{
   UpdateTargetName(aSimulation, aTrackPtr);
}

//! Update the truth target name that is associated with the specified track.
//! This should be called to maintain the name of the target that is associated with the track.
//! @param aSimulation The simulation
//! @param aTrackPtr Pointer to the local track.
void WsfLocalTrackStatus::UpdateTargetName(WsfSimulation& aSimulation, const WsfLocalTrack* aTrackPtr)
{
   mTargetName = aTrackPtr->GetTargetName();
   if (mTargetName == nullptr)
   {
      WsfPlatform* platformPtr = aSimulation.GetPlatformByIndex(aTrackPtr->GetTargetIndex());
      if (platformPtr != nullptr)
      {
         mTargetName = platformPtr->GetNameId();
      }
   }
}

// ***************************************************************************

//! The script interface 'class'
class WSF_EXPORT WsfScriptLocalTrackStatusClass : public UtScriptClass
{
public:
   WsfScriptLocalTrackStatusClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(LocalTrackId);
   UT_DECLARE_SCRIPT_METHOD(State);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceLastStateChange);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfLocalTrackStatus::CreateScriptClass(const std::string& aClassName,
                                                                      UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptLocalTrackStatusClass>(aClassName, aScriptTypesPtr);
}

WsfScriptLocalTrackStatusClass::WsfScriptLocalTrackStatusClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfLocalTrackStatus");

   AddMethod(ut::make_unique<LocalTrackId>());
   AddMethod(ut::make_unique<State>());
   AddMethod(ut::make_unique<TimeSinceLastStateChange>());
}

// Not really needed now because an use is via an unmanaged reference,
// but if we ever do return a managed reference then it is needed.
// virtual
void WsfScriptLocalTrackStatusClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfLocalTrackStatus*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackStatusClass, WsfLocalTrackStatus, LocalTrackId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetLocalTrackId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackStatusClass, WsfLocalTrackStatus, State, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetStateName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackStatusClass, WsfLocalTrackStatus, TimeSinceLastStateChange, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aReturnVal.SetDouble(simTime - aObjectPtr->GetLastStateChangeTime());
}
