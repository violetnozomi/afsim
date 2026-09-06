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

#include "WsfCorrelationStrategy.hpp"

#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfTrackId.hpp"
#include "WsfTrackManager.hpp"

// =================================================================================================
WsfCorrelationStrategy::WsfCorrelationStrategy()
   : mCorrelationMap()
   , mTrackManagerPtr(nullptr)
{
}

// =================================================================================================
WsfCorrelationStrategy::WsfCorrelationStrategy(const WsfCorrelationStrategy& aSrc)
   : mCorrelationMap(aSrc.mCorrelationMap)
   , mTrackManagerPtr(nullptr) // Set during Initialize.
{
}

WsfCorrelationStrategy& WsfCorrelationStrategy::operator=(const WsfCorrelationStrategy& aRhs)
{
   if (this != &aRhs)
   {
      WsfObject::operator=(aRhs);
      mCorrelationMap    = aRhs.mCorrelationMap;
      mTrackManagerPtr   = nullptr; // Set during Initialize.
   }
   return *this;
}

// =================================================================================================
//! Notifies the correlation strategy that a new track correlation has been made
//! This is called by the track manager for new tracks.
void WsfCorrelationStrategy::NewTrackCorrelation(const WsfTrack& aRawTrack, const WsfLocalTrack& aLocalTrack)
{
   mCorrelationMap[aRawTrack.GetTrackId()] = aLocalTrack.GetTrackId();
}

// =================================================================================================
//! Given a non-local track update (or measurement), find the track in the track list
//! that correlates with the given track or measurement.
//! @note This is a template method that defers execution to CorrelateImpl
WsfLocalTrack* WsfCorrelationStrategy::Correlate(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList)
{
   WsfLocalTrack* localTrackPtr = CorrelateImpl(aSimTime, aNonLocalTrack, aTrackList);
   if (localTrackPtr != nullptr)
   {
      mCorrelationMap[aNonLocalTrack.GetTrackId()] = localTrackPtr->GetTrackId();
   }
   return localTrackPtr;
}


// =================================================================================================
//! Given a raw track id, return the (local) track id previously correlated with
//! this track id.  If there is no correlation, the track id will be "null",
//! (i.e., the function WsfTrackId::IsNull will return true for the returned value of this method.
const WsfTrackId& WsfCorrelationStrategy::GetCorrelatedTrackId(const WsfTrackId& aRawTrackId) const
{
   static const WsfTrackId                          nullTrackId;
   std::map<WsfTrackId, WsfTrackId>::const_iterator trackIdIter = mCorrelationMap.find(aRawTrackId);
   if (trackIdIter != mCorrelationMap.end())
   {
      return trackIdIter->second;
   }
   else
   {
      return nullTrackId;
   }
}

// =================================================================================================
//! Allow the user to break an existing correlation
// virtual
void WsfCorrelationStrategy::Decorrelate(const WsfTrackId& aRawTrackId)
{
   std::map<WsfTrackId, WsfTrackId>::iterator trackIdIter = mCorrelationMap.find(aRawTrackId);
   if (trackIdIter != mCorrelationMap.end())
   {
      mCorrelationMap.erase(trackIdIter);
   }
}

// =================================================================================================
//! Notifies the track manager and observers that a track has been correlated
//! by the correlation strategy
void WsfCorrelationStrategy::NotifyOfLocalTrackCorrelation(double               aSimTime,
                                                           const WsfLocalTrack& aLocalTrack,
                                                           const WsfTrackId&    aRawTrackId)
{
   mTrackManagerPtr->NotifyOfLocalTrackCorrelation(aSimTime, &aLocalTrack, aRawTrackId);
}

WsfSimulation* WsfCorrelationStrategy::GetSimulation() const
{
   assert(mTrackManagerPtr);
   return mTrackManagerPtr ? mTrackManagerPtr->GetSimulation() : nullptr;
}

//! Method to facilitate unit testing.
bool WsfCorrelationStrategy::TestCorrelation(const WsfTrack& aTrack1, const WsfTrack& aTrack2)
{
   WsfLocalTrackList list;
   list.AddTrack(ut::make_unique<WsfLocalTrack>(aTrack2));
   WsfLocalTrack* correlatedTrackPtr = Correlate(aTrack1.GetUpdateTime(), aTrack1, list);
   return (correlatedTrackPtr != nullptr);
}

#include "script/WsfScriptContext.hpp"
// ================================================================================================
WsfScriptCorrelationClass::WsfScriptCorrelationClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCorrelator");

   AddMethod(ut::make_unique<TestCorrelation>());
   AddMethod(ut::make_unique<Correlate>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCorrelationClass, WsfCorrelationStrategy, TestCorrelation, 2, "bool", "WsfTrack, WsfTrack")
{
   WsfTrack* track1Ptr  = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfTrack* track2Ptr  = static_cast<WsfTrack*>(aVarArgs[1].GetPointer()->GetAppObject());
   bool      correlated = aObjectPtr->TestCorrelation(*track1Ptr, *track2Ptr);
   aReturnVal.SetBool(correlated);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCorrelationClass, WsfCorrelationStrategy, Correlate, 2, "WsfTrackId", "WsfTrack, WsfLocalTrackList")
{
   WsfTrack*            track1Ptr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfLocalTrackList*   trackListPtr = static_cast<WsfLocalTrackList*>(aVarArgs[1].GetPointer()->GetAppObject());
   const WsfLocalTrack* ctPtr = aObjectPtr->Correlate(WsfScriptContext::GetTIME_NOW(aContext), *track1Ptr, *trackListPtr);
   WsfTrackId*          trackIdPtr = new WsfTrackId();
   if (ctPtr != nullptr)
   {
      *trackIdPtr = ctPtr->GetTrackId();
   }
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
