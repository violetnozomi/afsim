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

#include "WsfCyberTrackManager.hpp"

#include "UtScript.hpp"
#include "WsfCorrelationStrategy.hpp"
#include "WsfFusionStrategy.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace cyber
{

// ================================================================================================
TrackManager::TrackManager(std::unique_ptr<WsfTrackManager>  aTrackManager,
                           std::shared_ptr<WsfScriptContext> aContextPtr,
                           UtScript*                         aOnEntry,
                           UtScript*                         aOnExit,
                           UtScript*                         aOnTrack,
                           UtScript*                         aOnRawTrack,
                           UtScript*                         aOnTrackDrop,
                           bool                              aTrackPurging,
                           bool                              aRawTrackPurging,
                           bool                              aLocalHistoryPurging)
   : WsfTrackManager(*aTrackManager)
   , mTrackManager(std::move(aTrackManager))
   , mContextPtr(aContextPtr)
   , mOnEntry(aOnEntry)
   , mOnExit(aOnExit)
   , mOnTrack(aOnTrack)
   , mOnRawTrack(aOnRawTrack)
   , mOnTrackDrop(aOnTrackDrop)
   , mActive(true)
   , mInactiveTrackPurging(aTrackPurging)
   , mInactiveRawTrackPurging(aRawTrackPurging)
   , mLocalTrackHistoryPurging(aLocalHistoryPurging)
{
   //! Any callbacks belonging to the WsfTrackManager we are decorating need to be
   //! 're-routed' directly to this instance instead, especially since these callbacks
   //! often pertain directly to track manipulation.
   SwapCallbacks();
}

// ================================================================================================
TrackManager* TrackManager::Clone() const
{
   return new TrackManager(*this);
}

// ================================================================================================
void TrackManager::ExecuteOnEntry(double aSimTime)
{
   if (mOnEntry && mActive)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;

      scriptArgs.push_back(new UtScriptRef(this, mContextPtr->GetClass("WsfTrackManager")));
      mContextPtr->ExecuteScript(aSimTime, mOnEntry, scriptRetVal, scriptArgs);
   }

   //! We don't need to reference nested and decorated track managers for this method call,
   //! as it will always be called and executed from the context of the effect, and only executed
   //! once at the beginning of the effect.
}

// ================================================================================================
void TrackManager::ExecuteOnExit(double aSimTime)
{
   if (mOnExit && mActive)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;

      scriptArgs.push_back(new UtScriptRef(this, mContextPtr->GetClass("WsfTrackManager")));
      mContextPtr->ExecuteScript(aSimTime, mOnExit, scriptRetVal, scriptArgs);
   }

   //! We don't need to reference nested and decorated track managers for this method call,
   //! as it will always be called and executed from the context of the effect, and only executed
   //! once at the end of the effect.
}

// ================================================================================================
WsfLocalTrack* TrackManager::AddTrackReport(double aSimTime, const WsfTrack& aTrackUpdate)
{
   WsfLocalTrack* returnTrack = nullptr;

   if (CyberAddTrack(aSimTime, aTrackUpdate))
   {
      returnTrack = WsfTrackManager::AddTrackReport(aSimTime, aTrackUpdate);
   }

   return returnTrack;
}

// ================================================================================================
bool TrackManager::CyberAddTrack(double aSimTime, const WsfTrack& aTrackUpdate)
{
   bool addTrack = true;

   //! Nested cyber managers function first before attempting locally.
   auto cyberTrackManager = dynamic_cast<TrackManager*>(mTrackManager.get());
   if (cyberTrackManager)
   {
      addTrack = cyberTrackManager->CyberAddTrack(aSimTime, aTrackUpdate);
   }

   if (addTrack && mActive && mOnTrack)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;

      scriptArgs.push_back(new UtScriptRef(aTrackUpdate.Clone(), mContextPtr->GetClass("WsfTrack"), UtScriptRef::cMANAGE));
      scriptArgs.push_back(new UtScriptRef(this, mContextPtr->GetClass("WsfTrackManager")));
      mContextPtr->ExecuteScript(aSimTime, mOnTrack, scriptRetVal, scriptArgs);
      addTrack = scriptRetVal.GetBool();
   }

   return addTrack;
}

// ================================================================================================
void TrackManager::DropTrack(double aSimTime, const WsfTrackId& aRawTrackId)
{
   if (CyberDropTrack(aSimTime, aRawTrackId))
   {
      WsfTrackManager::DropTrack(aSimTime, aRawTrackId);
   }
}

// ================================================================================================
bool TrackManager::CyberDropTrack(double aSimTime, const WsfTrackId& aRawTrackId)
{
   bool dropTrack = true;

   //! Nested cyber managers function first before attempting locally.
   auto cyberTrackManager = dynamic_cast<TrackManager*>(mTrackManager.get());
   if (cyberTrackManager)
   {
      dropTrack = cyberTrackManager->CyberDropTrack(aSimTime, aRawTrackId);
   }

   if (dropTrack && mActive && mOnTrackDrop)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      auto             copyTrackId = aRawTrackId;

      scriptArgs.push_back(new UtScriptRef(&copyTrackId, mContextPtr->GetClass("WsfTrackId")));
      scriptArgs.push_back(new UtScriptRef(this, mContextPtr->GetClass("WsfTrackManager")));
      mContextPtr->ExecuteScript(aSimTime, mOnTrackDrop, scriptRetVal, scriptArgs);
      dropTrack = scriptRetVal.GetBool();
   }

   return dropTrack;
}

// ================================================================================================
WsfTrack* TrackManager::AddRawTrackReport(double aSimTime, const WsfTrack& aRawTrack)
{
   WsfTrack* returnTrack = nullptr;

   if (CyberAddRawTrack(aSimTime, aRawTrack))
   {
      returnTrack = WsfTrackManager::AddRawTrackReport(aSimTime, aRawTrack);
   }

   return returnTrack;
}

// ================================================================================================
bool TrackManager::CyberAddRawTrack(double aSimTime, const WsfTrack& aRawTrack)
{
   bool addTrack = true;

   //! Nested cyber managers function first before attempting locally.
   auto cyberTrackManager = dynamic_cast<TrackManager*>(mTrackManager.get());
   if (cyberTrackManager)
   {
      addTrack = cyberTrackManager->CyberAddRawTrack(aSimTime, aRawTrack);
   }

   if (addTrack && mActive && mOnRawTrack)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;

      scriptArgs.push_back(new UtScriptRef(aRawTrack.Clone(), mContextPtr->GetClass("WsfTrack"), UtScriptRef::cMANAGE));
      scriptArgs.push_back(new UtScriptRef(this, mContextPtr->GetClass("WsfTrackManager")));
      mContextPtr->ExecuteScript(aSimTime, mOnRawTrack, scriptRetVal, scriptArgs);
      addTrack = scriptRetVal.GetBool();
   }

   return addTrack;
}

// ================================================================================================
void TrackManager::StartFrame()
{
   WsfTrackManager::StartFrame();
   mTrackManager->StartFrame();
}

// ================================================================================================
void TrackManager::EndFrame()
{
   WsfTrackManager::EndFrame();
   mTrackManager->EndFrame();
}

// ================================================================================================
TrackManager::TrackManager(const TrackManager& aSrc)
   : WsfTrackManager(aSrc)
   , mTrackManager(aSrc.mTrackManager->Clone())
   , mContextPtr(aSrc.mContextPtr)
   , mOnEntry(aSrc.mOnEntry)
   , mOnExit(aSrc.mOnExit)
   , mOnTrack(aSrc.mOnTrack)
   , mOnRawTrack(aSrc.mOnRawTrack)
   , mOnTrackDrop(aSrc.mOnTrackDrop)
   , mActive(aSrc.mActive)
   , mInactiveTrackPurging(aSrc.mInactiveTrackPurging)
   , mInactiveRawTrackPurging(aSrc.mInactiveRawTrackPurging)
   , mLocalTrackHistoryPurging(aSrc.mLocalTrackHistoryPurging)
{
}

// ================================================================================================
TrackManager::TrackManager(TrackManager&& aSrc)
   : WsfTrackManager(aSrc)
   , mTrackManager(std::move(aSrc.mTrackManager))
   , mContextPtr(aSrc.mContextPtr)
   , mOnEntry(aSrc.mOnEntry)
   , mOnExit(aSrc.mOnExit)
   , mOnTrack(aSrc.mOnTrack)
   , mOnRawTrack(aSrc.mOnRawTrack)
   , mOnTrackDrop(aSrc.mOnTrackDrop)
   , mActive(aSrc.mActive)
   , mInactiveTrackPurging(aSrc.mInactiveTrackPurging)
   , mInactiveRawTrackPurging(aSrc.mInactiveRawTrackPurging)
   , mLocalTrackHistoryPurging(aSrc.mLocalTrackHistoryPurging)
{
}

// ================================================================================================
void TrackManager::PurgeInactiveRawTracks(double aSimTime, double aInactivityTimeLimit, double aStaticInactivityTimeLimit)
{
   if (mInactiveRawTrackPurging)
   {
      mTrackManager->PurgeInactiveRawTracks(aSimTime, aInactivityTimeLimit, aStaticInactivityTimeLimit);
      WsfTrackManager::PurgeInactiveRawTracks(aSimTime, aInactivityTimeLimit, aStaticInactivityTimeLimit);
   }
}

// ================================================================================================
void TrackManager::PurgeLocalTrackHistory(double aSimTime, double aKeepTimeInterval)
{
   if (mLocalTrackHistoryPurging)
   {
      mTrackManager->PurgeLocalTrackHistory(aSimTime, aKeepTimeInterval);
      WsfTrackManager::PurgeLocalTrackHistory(aSimTime, aKeepTimeInterval);
   }
}

// ================================================================================================
void TrackManager::PurgeInactiveTracks(double aSimTime, double aInactivityTimeLimit)
{
   if (mInactiveTrackPurging)
   {
      mTrackManager->PurgeInactiveTracks(aSimTime, aInactivityTimeLimit);
      WsfTrackManager::PurgeInactiveTracks(aSimTime, aInactivityTimeLimit);
   }
}

// ================================================================================================
void TrackManager::SwapCallbacks()
{
   CandidateTrackPromoted.Merge(mTrackManager->CandidateTrackPromoted);
   CurrentTargetChanged.Merge(mTrackManager->CurrentTargetChanged);
   LocalTrackCorrelation.Merge(mTrackManager->LocalTrackCorrelation);
   LocalTrackDecorrelation.Merge(mTrackManager->LocalTrackDecorrelation);
   LocalTrackInitiated.Merge(mTrackManager->LocalTrackInitiated);
   LocalTrackDropped.Merge(mTrackManager->LocalTrackDropped);
   LocalTrackUpdated.Merge(mTrackManager->LocalTrackUpdated);
   RawTrackDropped.Merge(mTrackManager->RawTrackDropped);
   RawTrackReceived.Merge(mTrackManager->RawTrackReceived);
   TrackManagerChanged.Merge(mTrackManager->TrackManagerChanged);

   mTrackManager->CandidateTrackPromoted.DisconnectAll();
   mTrackManager->CurrentTargetChanged.DisconnectAll();
   mTrackManager->LocalTrackCorrelation.DisconnectAll();
   mTrackManager->LocalTrackDecorrelation.DisconnectAll();
   mTrackManager->LocalTrackInitiated.DisconnectAll();
   mTrackManager->LocalTrackDropped.DisconnectAll();
   mTrackManager->LocalTrackUpdated.DisconnectAll();
   mTrackManager->RawTrackDropped.DisconnectAll();
   mTrackManager->RawTrackReceived.DisconnectAll();
   mTrackManager->TrackManagerChanged.DisconnectAll();
}

} // namespace cyber
} // namespace wsf
