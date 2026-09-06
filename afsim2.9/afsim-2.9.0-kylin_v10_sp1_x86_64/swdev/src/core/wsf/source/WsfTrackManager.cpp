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

#include "WsfTrackManager.hpp"

#include <algorithm>
#include <cassert>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfCorrelationStrategy.hpp"
#include "WsfCorrelationStrategyTypes.hpp"
#include "WsfDateTime.hpp"
#include "WsfFilter.hpp"
#include "WsfFilterTypes.hpp"
#include "WsfFusionStrategyTypes.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfUtil.hpp"
#include "script/WsfScriptContext.hpp"

// -------------------------------------------------------------------------------------------------
//! Constructor
WsfTrackManager::WsfTrackManager(const WsfScenario& aScenario)
   : WsfSimplePlatformComponent(cWSF_COMPONENT_TRACK_MANAGER, "#track_manager")
   , CandidateTrackPromoted()
   , CurrentTargetChanged()
   , LocalTrackCorrelation()
   , LocalTrackDecorrelation()
   , LocalTrackInitiated()
   , LocalTrackDropped()
   , LocalTrackUpdated()
   , RawTrackDropped()
   , RawTrackReceived()
   , TrackManagerChanged()
   , mScenario(aScenario)
   , mPlatformPtr(nullptr)
   , mIsInitialized(false)
   , mWillPurgeInactiveTracks(false)
   , mDropUncorrelatedTracks(true)
   , mRetainRawTracks(false)
   , mMaintainTrackHistory(false)
   , mIsFramed(false)
   , mFrameStarted(false)
   , mDebug(false)
   , mInitiateLocalTracks(true)
   , mPrototypeTrackPtr(new WsfLocalTrack())
   , mTrackList(new WsfLocalTrackList)
   , mFilteredRawTrackList(new WsfLocalTrackList)
   , mRawTrackList(new WsfTrackList)
   , mCurrentTargetId()
   , mInitialTracks()
   , mInitialTargetId()
   , mCorrelationStrategyPtr(nullptr)
   , mFusionStrategyPtr(nullptr)
   , mPrototypeFilterPtr(nullptr)
   , mAuxDataFusionRules()
{
   auto& correlationTypes(WsfCorrelationStrategyTypes::Get(aScenario));
   mCorrelationStrategyPtr = correlationTypes.Create(correlationTypes.GetDefaultStrategyName());
   auto& fusionTypes(WsfFusionStrategyTypes::Get(aScenario));
   mFusionStrategyPtr = fusionTypes.Create(fusionTypes.GetDefaultStrategyName());
}

// -------------------------------------------------------------------------------------------------
//! Copy constructor.
WsfTrackManager::WsfTrackManager(const WsfTrackManager& aSrc)
   : WsfSimplePlatformComponent(aSrc)
   , CandidateTrackPromoted()
   , CurrentTargetChanged()
   , LocalTrackCorrelation()
   , LocalTrackDecorrelation()
   , LocalTrackInitiated()
   , LocalTrackDropped()
   , LocalTrackUpdated()
   , RawTrackDropped()
   , RawTrackReceived()
   , TrackManagerChanged()
   , mScenario(aSrc.mScenario)
   , mPlatformPtr(nullptr)
   , mIsInitialized(false)
   , mWillPurgeInactiveTracks(false)
   , mDropUncorrelatedTracks(aSrc.mDropUncorrelatedTracks)
   , mRetainRawTracks(aSrc.mRetainRawTracks)
   , mMaintainTrackHistory(aSrc.mMaintainTrackHistory)
   , mIsFramed(aSrc.mIsFramed)
   , mFrameStarted(false)
   , mDebug(aSrc.mDebug)
   , mInitiateLocalTracks(aSrc.mInitiateLocalTracks)
   , mPrototypeTrackPtr(new WsfLocalTrack(*aSrc.mPrototypeTrackPtr))
   , mTrackList(new WsfLocalTrackList(*aSrc.mTrackList))
   , mFilteredRawTrackList(new WsfLocalTrackList(*aSrc.mFilteredRawTrackList))
   , mRawTrackList(new WsfTrackList(*aSrc.mRawTrackList))
   , mCurrentTargetId()
   , mInitialTracks()
   , mInitialTargetId(aSrc.mInitialTargetId)
   , mCorrelationStrategyPtr(aSrc.mCorrelationStrategyPtr->Clone())
   , mFusionStrategyPtr(aSrc.mFusionStrategyPtr->Clone())
   , mPrototypeFilterPtr(nullptr)
   , mAuxDataFusionRules(aSrc.mAuxDataFusionRules)
{
   for (auto initialTrack : aSrc.mInitialTracks)
   {
      mInitialTracks.push_back(initialTrack->Clone());
   }

   if (aSrc.mPrototypeFilterPtr)
   {
      mPrototypeFilterPtr = aSrc.mPrototypeFilterPtr->Clone();
   }
}

// -------------------------------------------------------------------------------------------------
// virtual
WsfTrackManager::~WsfTrackManager()
{
   delete mPrototypeTrackPtr;

   for (auto& initialTrack : mInitialTracks)
   {
      delete initialTrack;
   }
   delete mCorrelationStrategyPtr;
   delete mFusionStrategyPtr;
   delete mPrototypeFilterPtr;
   delete mTrackList;
   delete mFilteredRawTrackList;
   delete mRawTrackList;
}

// -------------------------------------------------------------------------------------------------
// virtual
//! Process input from a generic source.
//!    Examine the current input command.  If it is NOT one of the commands
//!    recognized by this class then it simply returns 'false'.  If it is
//!    one of the commands recognized by this class then it processes the
//!    command and returns 'true'.
//!    @param aInput a reference to a UtInput object.
//!    @returns 'true' if the command was one recognized by the class or 'false' if not.
bool WsfTrackManager::ProcessInput(UtInput& aInput)
{
   WsfTrack*               trackPtr               = nullptr;
   WsfFilter*              filterPtr              = nullptr;
   WsfCorrelationStrategy* correlationStrategyPtr = nullptr;
   WsfFusionStrategy*      fusionStrategyPtr      = nullptr;

   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "framed")
   {
      mIsFramed = true;
   }
   else if (command == "unframed")
   {
      mIsFramed = false;
   }
   else if (command == "debug")
   {
      mDebug = true;
   }
   else if (mPrototypeTrackPtr->WsfAuxDataEnabled::ProcessInput(aInput))
   {
      // Aux data input will be handled by the prototype track object.
   }
   else if (command == "tracker_type")
   {
      std::string type;
      aInput.ReadCommand(type);
      correlationStrategyPtr = WsfCorrelationStrategyTypes::Get(mScenario).Create(type);
      if (correlationStrategyPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown tracker type track correlation method: " + type);
      }
      delete mCorrelationStrategyPtr;
      mCorrelationStrategyPtr = correlationStrategyPtr;

      fusionStrategyPtr = WsfFusionStrategyTypes::Get(mScenario).Create(type);
      if (fusionStrategyPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown tracker type fusion method: " + type);
      }
      delete mFusionStrategyPtr;
      mFusionStrategyPtr = fusionStrategyPtr;

      // change from previous versions (< 2.0); "tracker_type" commands must be in a block
      UtInputBlock block(aInput, "end_tracker_type");
      while (block.ReadCommand(command))
      {
         if (mCorrelationStrategyPtr->ProcessInput(aInput))
         {
         }
         else if (mFusionStrategyPtr->ProcessInput(aInput))
         {
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (WsfCorrelationStrategyTypes::Get(mScenario).LoadInstance(aInput, correlationStrategyPtr))
   {
      delete mCorrelationStrategyPtr;
      mCorrelationStrategyPtr = correlationStrategyPtr;
   }
   else if (WsfFusionStrategyTypes::Get(mScenario).LoadInstance(aInput, fusionStrategyPtr))
   {
      delete mFusionStrategyPtr;
      mFusionStrategyPtr = fusionStrategyPtr;
   }
   else if (WsfFilterTypes::Get(mScenario).LoadInstance(aInput, filterPtr))
   {
      delete mPrototypeFilterPtr;
      mPrototypeFilterPtr = filterPtr;
   }
   else if (command == "uncorrelated_track_drops")
   {
      aInput.ReadValue(mDropUncorrelatedTracks);
   }
   else if (command == "retain_raw_tracks")
   {
      mRetainRawTracks = true;
   }
   else if (command == "retain_track_history")
   {
      mMaintainTrackHistory = true;
   }
   else if (WsfTrack::LoadTrackInstance(mScenario, aInput, nullptr, trackPtr))
   {
      if (trackPtr)
      {
         AddInitialTrack(*trackPtr);
         delete trackPtr;
      }
   }
   else if (command == "aux_data_fusion_rules")
   {
      ProcessAuxDataFusionInput(aInput);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// -------------------------------------------------------------------------------------------------
//! Initialize the object.
//! @param aSimTime The current simulation time.
//! @return 'true' if successful or 'false' if some sort of error occurred.
// virtual
bool WsfTrackManager::Initialize(double aSimTime)
{
   assert(mPlatformPtr != nullptr);
   bool success = mFusionStrategyPtr->Initialize(this);
   success &= mCorrelationStrategyPtr->Initialize(this);
   return success;
}

// -------------------------------------------------------------------------------------------------
//! Phase 2 Initialization.
//! This is called by WsfPlatform::Initialize() after phase 1 initialization has been performed for
//! all platform components. The functions in this method must be performed in phase 2 because they
//! require knowledge of the platform location, which is not known until after phase 1.
//! @param aSimTime The current simulation time.
//! @return 'true' if successful or 'false' if some sort of error occurred.
// virtual
bool WsfTrackManager::Initialize2(double aSimTime)
{
   bool success = true;

   // From this point forward we are considered initialized...
   mIsInitialized = true;

   // Load initial tracks into the track list.

   double locationWCS[3];
   mPlatformPtr->GetLocationWCS(locationWCS);
   for (WsfTrack* trackPtr : mInitialTracks)
   {
      trackPtr->SetSimulation(GetSimulation());

      // If this is the hand-off track from a platform spawn (e.g.: when a object performs a
      // SetCurrentTarget() using the track on the spawn command), then AddTrackReport is called
      // rather than AddTrack. The former creates a raw track from the hand-off track and creates
      // an associated local track, while the later simply creates a non-fusible local track. This
      // ensures than continuing uplink against the same target will get properly fused.

      if ((!mInitialTargetId.IsNull()) && (trackPtr->GetTrackId() == mInitialTargetId))
      {
         // This is the 'current target' from a platform spawn hand-off.
         // The input track has to be marked fusible so AddTrackReport alway creates a raw and local track.
         trackPtr->SetFusible(true);
         WsfLocalTrack* localTrackPtr = AddTrackReport(aSimTime, *trackPtr);
         if (localTrackPtr)
         {
            mCurrentTargetId = localTrackPtr->GetTrackId();
         }
         else
         {
            // This should never fail, but just in case.
            auto out = ut::log::warning() << "Could not initialize the hand-off track.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
      }
      else
      {
         // This is not the 'current target' from a platform spawn hand-off, so just create a non-fusible local track.
         if (trackPtr->GetTrackId().IsNull())
         {
            trackPtr->SetPlatformOriginator(mPlatformPtr);
            trackPtr->Initialize(aSimTime, mPlatformPtr->GetNextTrackId(), *GetSimulation());
            trackPtr->Update(aSimTime);
            trackPtr->SetOriginatorLocationWCS(locationWCS);
         }
         AddTrack(aSimTime, *trackPtr);
      }
      delete trackPtr;
   }
   mInitialTracks.clear();

   return success;
}

// -------------------------------------------------------------------------------------------------
//! Non-master track manager initialization.
//! Some objects create their own private track managers rather that use the one on the platform.
//! This form is used in those cases.
//! @param aSimTime    The current simulation time.
//! @param aPlatformPtr The platform which owns the object creating this track manager.
//! @returns true if successful or false if not.
//! @note In this case the component/track manager does not reside on the platform component list.
bool WsfTrackManager::Initialize(double aSimTime, WsfPlatform* aPlatformPtr)
{
   assert(aPlatformPtr);
   // NOTE: Inform the component it is attached to the platform. This must be done because it isn't
   //       part of a component list (which normally sets this attribute).
   SetComponentParent(aPlatformPtr);
   return Initialize(aSimTime) && Initialize2(aSimTime);
}

// -------------------------------------------------------------------------------------------------
//! Purge inactive tracks.
//! This method purges inactive local tracks.  A track is declared inactive after when it hasn't been
//! updated within the time interval prescribed by the parameter 'aInactivityTimeLimit'.
//! @param aSimTime             The current simulation time.
//! @param aInactivityTimeLimit The maximum amount of time that a track may go without having a track update
//! before it is declared inactive.
//! @note Observers are notified of any local track drops that are the result of purging.
// virtual
void WsfTrackManager::PurgeInactiveTracks(double aSimTime, double aInactivityTimeLimit)
{
   unsigned int trackIndex = 0;
   while (trackIndex < mTrackList->GetTrackCount())
   {
      WsfLocalTrack* trackPtr = mTrackList->GetTrackEntry(trackIndex);
      auto           out      = ut::log::debug(); // If nothing is written to out, noting is printed.
      if (mDebug)
      {
         out << "Considering purge of local track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Track ID: " << trackPtr->GetTrackId();
      }
      if (trackPtr->IsPurgeable() && (trackPtr->GetUpdateTime() + aInactivityTimeLimit < aSimTime))
      {
         if (trackPtr->GetUseCount() == 0)
         {
            NotifyOfLocalTrackDropped(aSimTime, trackPtr);
            if (!mRetainRawTracks)
            {
               DeleteCorrelatedRawTracks(aSimTime, *trackPtr);
            }
            if (mDebug)
            {
               out.AddNote() << "Dropping.";
            }
            trackIndex = mTrackList->DeleteTrackEntry(trackIndex);
         }
         else
         {
            if (mDebug)
            {
               out.AddNote() << "Not dropping. Non-zero use count.";
            }
            trackPtr->SetStale(true);
            ++trackIndex;
         }
      }
      else
      {
         if (mDebug)
         {
            if (trackPtr->IsPurgeable())
            {
               out.AddNote() << "Not dropping. Track still active.";
            }
            else
            {
               out << "Not dropping. Track is non-purgeable.";
            }
         }
         ++trackIndex;
      }
   }
}

// -------------------------------------------------------------------------------------------------
//! Purge inactive raw tracks.
//! This method purges inactive raw tracks.  A raw track is declared inactive after when it hasn't been
//! updated within the time interval prescribed by the parameter 'aInactivityTimeLimit'.
//! @param aSimTime             The current simulation time.
//! @param aInactivityTimeLimit The maximum amount of time that a track may go without having a track update
//! before it is declared inactive.
//! @param aStaticInactivityTimeLimit The maximum amount of time that a static track will be retained.
//! (i.e.: a track that is initially created, but then will probably never receive an update - e.g.: a spot SAR image).
// virtual
void WsfTrackManager::PurgeInactiveRawTracks(double aSimTime, double aInactivityTimeLimit, double aStaticInactivityTimeLimit)
{
   if (!mRetainRawTracks)
   {
      unsigned int trackIndex = 0;
      while (trackIndex < mRawTrackList->GetTrackCount())
      {
         WsfTrack* trackPtr  = mRawTrackList->GetTrackEntry(trackIndex);
         bool      dropTrack = false;
         if (trackPtr->IsPurgeable())
         {
            double timeSinceUpdated    = aSimTime - trackPtr->GetUpdateTime();
            double inactivityTimeLimit = aInactivityTimeLimit;
            if (trackPtr->GetTrackType() == WsfTrack::cSTATIC_IMAGE)
            {
               inactivityTimeLimit = aStaticInactivityTimeLimit;
            }
            dropTrack = (timeSinceUpdated > inactivityTimeLimit);
         }
         if (dropTrack)
         {
            if (mDebug)
            {
               auto out = ut::log::debug() << "Dropping raw track.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Track ID: " << trackPtr->GetTrackId();
               out.AddNote() << "Platform: " << mPlatformPtr->GetName();
            }
            // Remove any local track correlations, then delete the track.
            DropTrack(aSimTime, trackPtr->GetTrackId());
         }
         else
         {
            ++trackIndex;
         }
      }

      // Now, perform purging for filtered raw tracks.
      trackIndex = 0;
      while (trackIndex < mFilteredRawTrackList->GetTrackCount())
      {
         WsfTrack* trackPtr = mFilteredRawTrackList->GetTrackEntry(trackIndex);
         if ((trackPtr->GetUpdateTime() + aInactivityTimeLimit) < aSimTime)
         {
            trackIndex = mFilteredRawTrackList->DeleteTrackEntry(trackIndex);
         }
         else
         {
            ++trackIndex;
         }
      }
   }
}

void WsfTrackManager::PurgeLocalTrackHistory(double aSimTime, double aKeepTimeInterval)
{
   unsigned trackIndex = 0;
   while (trackIndex < mTrackList->GetTrackCount())
   {
      WsfLocalTrack* localTrackPtr = mTrackList->GetTrackEntry(trackIndex);
      if (localTrackPtr->HasHistory())
      {
         auto& history = localTrackPtr->GetHistory();
         history.erase(std::remove_if(history.begin(),
                                      history.end(),
                                      [=](const std::unique_ptr<WsfMeasurement>& m)
                                      { return m->GetUpdateTime() < aSimTime - aKeepTimeInterval; }),
                       history.end());
      }
      ++trackIndex;
   }
}

// -------------------------------------------------------------------------------------------------
// virtual
//! Lock a track to prevent it from being purged by the track manager.
//! @param aSimTime      The current simulation time.
//! @param aLocalTrackId The track ID of the local track to be locked.
void WsfTrackManager::LockTrack(double aSimTime, const WsfTrackId& aLocalTrackId)
{
   WsfLocalTrack* trackPtr = FindTrack(aLocalTrackId);
   if (trackPtr)
   {
      LockTrack(aSimTime, trackPtr);
   }
}

// -------------------------------------------------------------------------------------------------
// virtual
//! Lock a track to prevent it from being purged by the track manager.
//! Use this form if you already have a pointer to the local track to be locked.
//! It is faster because it doesn't have to locate the track in the track list.
//! @param aSimTime      The current simulation time.
//! @param aLocalTrackPtr The local track to be locked.
void WsfTrackManager::LockTrack(double aSimTime, WsfLocalTrack* aLocalTrackPtr)
{
   if ((aLocalTrackPtr) && (aLocalTrackPtr->GetTrackManager() == this))
   {
      aLocalTrackPtr->IncrementUseCount();
   }
}

// -------------------------------------------------------------------------------------------------
// virtual
//! Unlock a track that was previously locked.
//! @param aSimTime      The current simulation time.
//! @param aLocalTrackId The track ID of the local track to be unlocked.
void WsfTrackManager::UnlockTrack(double aSimTime, const WsfTrackId& aLocalTrackId)
{
   WsfLocalTrack* trackPtr = FindTrack(aLocalTrackId);
   if (trackPtr)
   {
      UnlockTrack(aSimTime, trackPtr);
   }
}

// -------------------------------------------------------------------------------------------------
// virtual
//! Unlock a track that was previously locked.
//! Use this form if you already have a pointer to the local track to be unlocked.
//! It is faster because it doesn't have to locate the track in the track list.
//! @param aSimTime       The current simulation time.
//! @param aLocalTrackPtr The local track to be unlocked.
void WsfTrackManager::UnlockTrack(double aSimTime, WsfLocalTrack* aLocalTrackPtr)
{
   if ((aLocalTrackPtr) && (aLocalTrackPtr->GetTrackManager() == this))
   {
      aLocalTrackPtr->DecrementUseCount();

      // If automatic purging is not being done then track must be dropped if:
      // -) It is not a pre-briefed track -and-
      // -) It is no longer locked -and-
      // -) It has no contributors

      if (aLocalTrackPtr->IsFusible() &&          // not a pre-briefed track
          mDropUncorrelatedTracks &&              // wants uncorrelated tracks to be dropped ...
          (!mWillPurgeInactiveTracks) &&          // ... unless automatic purging is being done
          (aLocalTrackPtr->GetUseCount() == 0) && // no longer locked
          (!aLocalTrackPtr->IsCorrelated()))      // no contributors...
      {
         if (mDebug)
         {
            auto logDebug = ut::log::debug() << "Dropping local Track as it is not correlated with any raw tracks.";
            logDebug.AddNote() << "T = " << aSimTime;
            logDebug.AddNote() << "Platform: " << mPlatformPtr->GetName();
            logDebug.AddNote() << "Track: " << aLocalTrackPtr->GetTrackId();
         }

         // Notify observers before actually deleting the track.
         NotifyOfLocalTrackDropped(aSimTime, aLocalTrackPtr);

         // As there are no correlated raw tracks, we can delete the local track without worrying
         // about culling the raw track list.
         if (!mTrackList->DeleteTrack(aLocalTrackPtr->GetTrackId()))
         {
            { // RAII block
               auto out = ut::log::error() << "Local track list is inconsistent.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << mPlatformPtr->GetName();
            }
            assert(false);
         }
      }
   }
}

// -------------------------------------------------------------------------------------------------
//! Remove the indicated track from the track list but don't delete the track object itself.
//! Caller assumes ownership of track object.
//!   @param aTrackId The track ID of the local track to be removed.
//!   @return Pointer to track or nullptr if the requested track could not be found.
std::unique_ptr<WsfLocalTrack> WsfTrackManager::RemoveTrack(const WsfTrackId& aTrackId)
{
   auto removedTrackPtr = mTrackList->RemoveTrack(aTrackId);
   if (removedTrackPtr)
   {
      removedTrackPtr->SetTrackManager(nullptr); // Detach the track from the track manager
      if (mDebug)
      {
         auto out = ut::log::debug() << "Removing local track.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Track ID: " << removedTrackPtr->GetTrackId();
      }
      NotifyOfLocalTrackDropped(GetSimulation()->GetSimTime(), removedTrackPtr.get());
   }
   return removedTrackPtr;
}

// -------------------------------------------------------------------------------------------------
// virtual
//! Start a frame for a frame based scan.
//! This method only needs to be called in conjunction with frame-based fusion strategies.
void WsfTrackManager::StartFrame()
{
   mFrameStarted = true;
}

// -------------------------------------------------------------------------------------------------
// protected virtual
//! Add a raw track report (default is add or replace).
//! @param aSimTime  The current simulation time.
//! @param aRawTrack The raw track to be added or replaced.
//! @return A pointer to the track that was added.  This is usually just the
//!  input raw track, but in this implementation, is an existing track if the
//!  saved copy is more recent than the one being added.
//!  This may be zero if the track report represented stale data
WsfTrack* WsfTrackManager::AddRawTrackReport(double aSimTime, const WsfTrack& aRawTrack)
{
   if (mDebug)
   {
      auto out = ut::log::debug() << "Adding raw track report.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Track ID: " << aRawTrack.GetTrackId();
      out.AddNote() << "Target: " << aRawTrack.GetTargetName();
      out.AddNote() << "Update Time: " << aRawTrack.GetUpdateTime();
      out.AddNote() << "Update Count: " << aRawTrack.GetUpdateCount();
   }

   WsfTrack* trackPtr = mRawTrackList->FindTrack(aRawTrack.GetTrackId());
   if (trackPtr)
   {
      // The last update time must be more recent than the stored raw track update time;
      // otherwise we may be receiving an older or identical copy of the track.
      // In that case, just use the track we have.
      if (aRawTrack.GetUpdateTime() > trackPtr->GetUpdateTime())
      {
         // Direct replacement in raw track list.
         trackPtr->ReplacementUpdate(aRawTrack);
         AssessTrackQuality(aSimTime, trackPtr);
         if (aRawTrack.HasAuxData())
         {
            trackPtr->MergeAuxData(aRawTrack);
         }
      }
      else
      {
         if (mDebug)
         {
            auto out = ut::log::debug() << "Ignoring new raw track report.";
            out.AddNote() << "New update time is before old update time.";
            out.AddNote() << "New Update Time: " << aRawTrack.GetUpdateTime();
            out.AddNote() << "Old Update Time: " << trackPtr->GetUpdateTime();
         }
         trackPtr = nullptr; // The track update is obsolete (the stored raw track is newer)
      }
   }
   else
   {
      trackPtr = aRawTrack.Clone();
      trackPtr->SetSimulation(GetSimulation());
      AssessTrackQuality(aSimTime, trackPtr);
      mRawTrackList->AddTrack(std::unique_ptr<WsfTrack>(trackPtr));
      if (mDebug)
      {
         ut::log::debug() << "Report is a new raw track.";
      }
   }

   return trackPtr;
}

// -------------------------------------------------------------------------------------------------
//! For tracks that are raw products of sensors, perform filtering based on the filter strategy.
WsfLocalTrack* WsfTrackManager::AddMeasurement(double aSimTime, const WsfTrack& aMeasurement)
{
   WsfLocalTrack* trackPtr = mFilteredRawTrackList->FindTrack(aMeasurement.GetTrackId());

   if (mDebug)
   {
      auto out = ut::log::debug() << "Adding measurement.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Originator: " << aMeasurement.GetOriginatorNameId();
      out.AddNote() << "Target: " << aMeasurement.GetTargetName();
      out.AddNote() << "Update Time: " << aMeasurement.GetUpdateTime();
   }

   if (trackPtr == nullptr)
   {
      // No existing filtered raw track.  Create a clone of the measurement and attach the filter.
      // NOTE: the new track is not reinitialized - the track ID and time of the original track is retained.
      auto trackUP = ut::make_unique<WsfLocalTrack>(aMeasurement);
      trackPtr     = trackUP.get();
      trackPtr->SetTrackManager(this);
      trackPtr->SetSimulation(GetSimulation());
      trackPtr->SetTrackType(WsfTrack::cFILTERED_SENSOR);
      trackPtr->SetFusible(true);
      trackPtr->SetIsCandidate(true);

      // Initiate a filter.
      // assert(trackPtr->GetFilter() == nullptr);
      trackPtr->SetFilter(mPrototypeFilterPtr->Clone());
      trackPtr->GetFilter()->Initialize(aSimTime, GetSimulation());
      mFilteredRawTrackList->AddTrack(std::move(trackUP));
   }

   // Update the track and the associated filter using the measurement data.
   trackPtr->UpdateFromMeasurement(aMeasurement.GetUpdateTime(), aMeasurement);

   // Fuse the updated filtered track into the corresponding local track. This must be done
   // BEFORE the promotion so any observer sees the most recent update applied.
   WsfLocalTrack* localTrackPtr = AddTrackReport(aSimTime, *trackPtr);

   // Update the track's "candidate" status.
   if (trackPtr->IsCandidate() && (trackPtr->GetFilter()->IsStable()))
   {
      trackPtr->SetIsCandidate(false);

      if ((localTrackPtr) && (localTrackPtr->IsCandidate()))
      {
         localTrackPtr->SetIsCandidate(false);

         // Notify observers that this track has been "promoted" to a full local track.
         NotifyOfCandidateTrackPromoted(aSimTime, localTrackPtr);
      }
   }

   return localTrackPtr;
}

// -------------------------------------------------------------------------------------------------
//! Assign a track quality to the raw track.
//! This is done on inbound raw tracks as they are moved to the raw track list.
//! Note that this does nothing to measurements. They have whatever quality they have.
//! This is mostly to prevent off-board tracks from preventing local track contributions
// private
void WsfTrackManager::AssessTrackQuality(double aSimTime, WsfTrack* aTrackPtr)
{
   if (mScenario.UseQuantitativeTrackQuality())
   {
      aTrackPtr->SetTrackQuality(aTrackPtr->GetQuality(aSimTime));
   }
   else // Using standard track quality.
   {
      WsfTrack::TrackType trackType = aTrackPtr->GetTrackType();
      if ((trackType == WsfTrack::cUNFILTERED_SENSOR) || (trackType == WsfTrack::cFILTERED_SENSOR) ||
          (trackType == WsfTrack::cSTATIC_IMAGE))
      {
         // Do nothing.
      }
      else
      {
         // Limit track quality to 0.5.
         double quality = std::min(aTrackPtr->GetTrackQuality(), 0.5);
         aTrackPtr->SetTrackQuality(quality);
      }
   }
}

// -------------------------------------------------------------------------------------------------
// virtual
//! Add a track report.  This report will be fused with any corresponding local
//! tracks.  If the track manager is running unframed, the merged track will
//! be returned immediately; otherwise, 0 will be returned, and a call to
//! FindCorrelatedTrack(...) will be required after EndFrame() is called.
//! The input raw track will be added to the raw track list if not present; otherwise,
//! it will be used to update the existing raw track.
//!   @param aSimTime The current simulation time.
//!   @param aTrackUpdate The raw track report to fuse into the local track list and add/replace
//!      in the raw track list.
//!   @return The local track into which the track report was fused. Note that this may be zero
//!           if the track was old or not immediately correlated.
WsfLocalTrack* WsfTrackManager::AddTrackReport(double aSimTime, const WsfTrack& aTrackUpdate)
{
   if (mDebug)
   {
      auto out = ut::log::debug() << "Adding track report.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Track ID: " << aTrackUpdate.GetTrackId();
      out.AddNote() << "Target: " << aTrackUpdate.GetTargetName();
   }
   WsfLocalTrack* localTrackPtr = nullptr;

   // Handle measurements separately.
   if ((aTrackUpdate.GetTrackType() == WsfTrack::cUNFILTERED_SENSOR) && mPrototypeFilterPtr)
   {
      localTrackPtr = AddMeasurement(aSimTime, aTrackUpdate);
   }
   else if (aTrackUpdate.IsFusible()) // Only fuse 'fusible' track reports.
   {
      // Notify interested observers of that the track update has been received.
      NotifyOfRawTrackReceived(aSimTime, &aTrackUpdate);

      // For fusible tracks the raw track is added first,
      // then fused into a local track.
      WsfTrack* addedTrackPtr = AddRawTrackReport(aSimTime, aTrackUpdate);

      if (addedTrackPtr)
      {
         // Determine whether there is an existing correlated local track.
         localTrackPtr = CorrelateTrack(aSimTime, *addedTrackPtr);

         if (localTrackPtr)
         {
            // Perform fusion of raw with correlated local track.
            FuseTrack(aSimTime, *addedTrackPtr, *localTrackPtr);
         }
         else if (mInitiateLocalTracks) // Not initially correlated.
         {
            localTrackPtr = InitiateTrack(aSimTime, *addedTrackPtr);
         }

         // If we are saving track history, do so now.
         if (mMaintainTrackHistory)
         {
            localTrackPtr->GetHistory().emplace_back(addedTrackPtr->Clone());
         }
      }
   }
   else
   {
      // Notify interested observers of that the track update has been received.
      NotifyOfRawTrackReceived(aSimTime, &aTrackUpdate);

      // For non-fusible tracks,
      // keep one, non-fusible copy as a local track.
      // Check whether this track already exists, and add it if not.
      localTrackPtr = FindCorrelatedTrack(aTrackUpdate.GetTrackId());
      if (localTrackPtr == nullptr)
      {
         localTrackPtr = AddTrack(aSimTime, aTrackUpdate);
      }
      else
      {
         assert(localTrackPtr->IsMirrorTrack(aTrackUpdate));

         // Perform direct replacement.
         localTrackPtr->ReplacementUpdate(aTrackUpdate);

         // Aux data is not copied by the direct replacement because we still want to allow the user
         // to have local aux data. This causes aux data to go through our 'fusion' process.
         UpdateAuxData(aSimTime, *localTrackPtr, aTrackUpdate);

         // Notify interested parties of the replacement update.
         NotifyOfLocalTrackUpdated(aSimTime, localTrackPtr, &aTrackUpdate);
      }
   }

   return localTrackPtr;
}

// -------------------------------------------------------------------------------------------------
// virtual
//! Add a track to the track manager's local track list.
//! The input raw track will not be fused or purged,
//!    and it will not be stored in the raw track list.
//!   @param aSimTime The time at which the track is added.
//!   @param aTrack The input raw track to add to the local track list.
//!   @return A pointer to the added track.
WsfLocalTrack* WsfTrackManager::AddTrack(double aSimTime, const WsfTrack& aTrack)
{
   size_t platformIndex = mPlatformPtr ? mPlatformPtr->GetIndex() : 0;
   auto   trackUP       = ut::make_unique<WsfLocalTrack>(aTrack);
   auto*  trackPtr      = trackUP.get();
   trackPtr->SetTrackManager(this);
   trackPtr->SetSimulation(GetSimulation());
   trackPtr->SetFusible(false);
   trackPtr->SetIsCandidate(false); // No filtering or fusion, so must be marked as a full track
   trackPtr->SetPurgeable(false);   // Default to the track's being non-purgeable.
   trackPtr->SetMirrorTrack(aTrack);

   // Make the track's aux data the union of the prototype's aux data and that of the added track.
   if (mPrototypeTrackPtr->HasAuxData())
   {
      trackPtr->SetAuxData(mPrototypeTrackPtr->GetAuxDataConst());
   }
   if (aTrack.HasAuxData())
   {
      trackPtr->MergeAuxData(aTrack);
   }
   mTrackList->AddTrack(std::move(trackUP));

   // This can get called via Initialize2 when initializing a track manager for platform that is
   // a 'deferred platform'. While not strictly necessary, this
   // suppresses messages for platforms that aren't a part of the simulation in the truest sense.

   if (platformIndex != 0)
   {
      NotifyOfLocalTrackInitiated(aSimTime, trackPtr, &aTrack);
   }

   mCorrelationStrategyPtr->NewTrackCorrelation(aTrack, *trackPtr);

   return trackPtr;
}

// -------------------------------------------------------------------------------------------------
//! Initiate a track and perform local track correlation, but do not inform any observers.
//! @param aSimTime The current simulation time.
//! @param aNonLocalTrack The track whose data are used to initiate the local track.
WsfLocalTrack* WsfTrackManager::InitiateTrackSilent(double aSimTime, const WsfTrack& aNonLocalTrack)
{
   // Create a new local track based on the raw track.
   WsfLocalTrack* trackPtr = InitiateTrackP(aSimTime, aNonLocalTrack);
   trackPtr->Correlate(aNonLocalTrack.GetTrackId());
   return trackPtr;
}

// -------------------------------------------------------------------------------------------------
//! Add track to the 'initial' track list.
//! An initial track is one that is created prior to actually initializing the platform that
//! contains the track manager. Examples are 'hand-off' tracks and
//! initial tracks defined in input files. The tracks are just put into a 'parking place'
//! until initialization, at which point they will be formally added to the track list.
//! @param aTrack The track to be added to the initial track list.
void WsfTrackManager::AddInitialTrack(const WsfTrack& aTrack)
{
   mInitialTracks.push_back(aTrack.Clone());
}

// -------------------------------------------------------------------------------------------------
//! Retrieve the current target track.
//!
//! @returns A Pointer to the current target. This will be 0 if there is no defined current target
//!          or if the track associated with the defined current target has been dropped.
//!
//! @note It is up to the caller to insure that the returned target track is
//! adequate (high enough fidelity) for the desired behavior (e.g., strike).
const WsfTrack* WsfTrackManager::GetCurrentTarget()
{
   const WsfLocalTrack* trackPtr = nullptr;
   if (!mCurrentTargetId.IsNull())
   {
      trackPtr = FindTrack(mCurrentTargetId);
      if (trackPtr == nullptr)
      {
         mCurrentTargetId.Null();
      }
   }
   return trackPtr;
}

// -------------------------------------------------------------------------------------------------
//! Set the current target to the indicated track.
//! @param aSimTime The current simulation time.
//! @param aTrack   The track indicating the current target.
//! @returns The track ID of the current target
//! If a track with the same ID already exists in the track list then it will be updated
//! with the new data. If the track does not exist then it will be added to the track list.
WsfTrackId WsfTrackManager::SetCurrentTarget(double aSimTime, const WsfTrack& aTrack)
{
   if (!mIsInitialized)
   {
      // Prior to initialization we simply capture the track ID and save the track in the 'initial track list'.
      AddInitialTrack(aTrack);
      mInitialTargetId = aTrack.GetTrackId();
   }
   else
   {
      WsfLocalTrack* trackPtr = FindTrack(aTrack.GetTrackId());
      if (trackPtr)
      {
         // The incoming track is already present as a local track. The incoming track will be used to update the
         // local track and the updated local track will be returned as the current target track.

         *static_cast<WsfTrack*>(trackPtr) = aTrack;
      }
      else
      {
         // The incoming track is not directly in the local track list. Add it as a track report, which will add
         // or update a corresponding raw track and then correlate it to a local track. It is the local track that
         // will become the current target track.

         trackPtr = AddTrackReport(aSimTime, aTrack);
         if (trackPtr == nullptr)
         {
            // AddTrackReport may reject the report if the incoming raw track it is already current in the raw track
            // list. If this is the case simply return the local track to which the raw track is already correlated.
            trackPtr = FindCorrelatedTrack(aTrack.GetTrackId());
         }
      }

      // Notify if track ID has changed
      if ((trackPtr) && (mCurrentTargetId != trackPtr->GetTrackId()))
      {
         mCurrentTargetId = trackPtr->GetTrackId();
         NotifyOfCurrentTargetChanged(aSimTime, trackPtr);
      }
   }
   return mCurrentTargetId;
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::NotifyOfCandidateTrackPromoted(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   CandidateTrackPromoted(aSimTime, aLocalTrackPtr);
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::NotifyOfLocalTrackCorrelation(double               aSimTime,
                                                    const WsfLocalTrack* aLocalTrackPtr,
                                                    const WsfTrackId&    aRawTrackId)
{
   WsfObserver::LocalTrackCorrelation(GetSimulation())(aSimTime, GetPlatform(), aLocalTrackPtr->GetTrackId(), aRawTrackId);
   LocalTrackCorrelation(aSimTime, aLocalTrackPtr, aRawTrackId);
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::NotifyOfLocalTrackDecorrelation(double               aSimTime,
                                                      const WsfLocalTrack* aLocalTrackPtr,
                                                      const WsfTrackId&    aRawTrackId)
{
   WsfObserver::LocalTrackDecorrelation(GetSimulation())(aSimTime, GetPlatform(), aLocalTrackPtr->GetTrackId(), aRawTrackId);
   LocalTrackDecorrelation(aSimTime, aLocalTrackPtr, aRawTrackId);
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::NotifyOfLocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   // WsfObserver::LocalTrackDropped(aSimTime, GetPlatform(), aLocalTrackPtr);
   WsfObserver::LocalTrackDropped(GetSimulation())(aSimTime, GetPlatform(), aLocalTrackPtr);
   LocalTrackDropped(aSimTime, aLocalTrackPtr);
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::NotifyOfLocalTrackInitiated(double               aSimTime,
                                                  const WsfLocalTrack* aLocalTrackPtr,
                                                  const WsfTrack*      aRawTrackPtr)
{
   WsfObserver::LocalTrackInitiated(GetSimulation())(aSimTime, GetPlatform(), aLocalTrackPtr, aRawTrackPtr);
   LocalTrackInitiated(aSimTime, aLocalTrackPtr, aRawTrackPtr);
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::NotifyOfLocalTrackUpdated(double               aSimTime,
                                                const WsfLocalTrack* aLocalTrackPtr,
                                                const WsfTrack*      aRawTrackPtr)
{
   WsfObserver::LocalTrackUpdated(GetSimulation())(aSimTime, GetPlatform(), aLocalTrackPtr, aRawTrackPtr);
   LocalTrackUpdated(aSimTime, aLocalTrackPtr, aRawTrackPtr);
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::NotifyOfRawTrackDropped(double aSimTime, const WsfTrack* aRawTrackPtr)
{
   RawTrackDropped(aSimTime, aRawTrackPtr);
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::NotifyOfRawTrackReceived(double aSimTime, const WsfTrack* aRawTrackPtr)
{
   RawTrackReceived(aSimTime, aRawTrackPtr);
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::NotifyOfCurrentTargetChanged(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   CurrentTargetChanged(aSimTime, aLocalTrackPtr);
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::NotifyOfTrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr)
{
   TrackManagerChanged(aSimTime, aNewManagerPtr);
}

// -------------------------------------------------------------------------------------------------
//! Update aux_data in the local track with aux_data from the non-local track.
//!
//! This is called from WsfDefaultFusion::UpdateNonKinematic data to update the aux_data in the local
//! track with aux_data from the non-local track. It is done within the context of the track manager
//! because it has access to the optional input data that is used to guide the process.
//!
//! @param aSimTime       The current simulation time.
//! @param aLocalTrack    The local track to be updated.
//! @param aNonLocalTrack The non-local track that is providing the update.
bool WsfTrackManager::UpdateAuxData(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   // Nothing to merge if the non-local track doesn't have any aux_data
   if (!aNonLocalTrack.HasAuxData())
   {
      return false;
   }

   bool localTrackUpdated = false;

   // The following code is more-or-less a copy of the code from UtAttributeContainer, with
   // additional code to conditionally prevent updating an attribute the local track.

   WsfAttributeContainer&                     localData     = aLocalTrack.GetAuxData();
   const WsfAttributeContainer&               nonLocalData  = aNonLocalTrack.GetAuxDataConst();
   const WsfAttributeContainer::AttributeMap& srcAttributes = nonLocalData.GetAttributeMap();
   for (const auto& srcAttribute : srcAttributes)
   {
      const UtAttributeBase* srcAttributePtr = srcAttribute.second.get();
      WsfStringId            nameId          = srcAttributePtr->GetNameId();
      if (CanUpdateAuxData(nameId, aLocalTrack, aNonLocalTrack))
      {
         const std::string& attribname = srcAttribute.first;
         assert(attribname == nameId);
         UtAttributeBase* attrib = localData.FindAttribute(attribname);
         if (attrib != nullptr)
         {
            attrib->SetAttribute(*srcAttributePtr);
         }
         else
         {
            localData.AddAttribute(ut::clone(srcAttributePtr));
         }
         localTrackUpdated = true;
      }
   }
   return localTrackUpdated;
}

// -------------------------------------------------------------------------------------------------
//! protected
//! Create a new local track based on the given non-local, 'raw' track.
//! Add a track as a correlated entry in the track list.
//!   @param aSimTime The current simulation time.
//!   @param aNonLocalTrack The input raw track to add to the local track list.
//!   @return A pointer to the added track.
//! @note This is the common code for both InitiateTrack and InitiateTrackSilent.
WsfLocalTrack* WsfTrackManager::InitiateTrackP(double aSimTime, const WsfTrack& aNonLocalTrack)
{
   auto localTrackPtr = ut::make_unique<WsfLocalTrack>(aNonLocalTrack);
   localTrackPtr->SetTrackManager(this);
   localTrackPtr->SetTrackType(WsfTrack::cPROCESSED);
   localTrackPtr->SetPlatformOriginator(mPlatformPtr);

   // Initializing the aux_data is a little tricky, because we want to honor any aux_data fusion rules.
   //
   // If the prototype track has aux_data, it becomes the 'initial' data, otherwise the 'initial' data is
   // set to empty. Note that this overwrites any aux_data that was copied from the non-local track by
   // the copy constructor (above). Then, any aux_data from the incoming non-local track is merged back in
   // using the UpdateAuxData method (which honors any fusion rules).

   if (mPrototypeTrackPtr->HasAuxData())
   {
      localTrackPtr->SetAuxData(mPrototypeTrackPtr->GetAuxDataConst());
   }
   else
   {
      localTrackPtr->DeleteAuxData();
   }
   UpdateAuxData(aSimTime, *localTrackPtr, aNonLocalTrack);

   // Range, el, and bearing not valid for off-board tracks.
   if (aNonLocalTrack.GetTrackId().GetOwningPlatformId() != mPlatformPtr->GetNameId())
   {
      localTrackPtr->SetRangeValid(false);
      localTrackPtr->SetBearingValid(false);
      localTrackPtr->SetElevationValid(false);
   }

   // Initialize the local track with a new track ID.
   //
   // The supplied time MUST be the time of the non-local track because the data contained in the
   // local track is from that time. Note that at one time WsfTrack::Initialize extrapolated to the
   // supplied time, but it was not consistent (position was extrapolated but covariance was not.
   //
   // A guiding premise is that for a single contributor, the local track should be an exact copy
   // of its contributor. Thus we don't want to be messing with it!
   localTrackPtr->Initialize(aNonLocalTrack.GetUpdateTime(), mPlatformPtr->GetNextTrackId(), *GetSimulation());

   // Indicate the non-local track will be initially acting as the mirror for this local track.
   localTrackPtr->SetMirrorTrack(aNonLocalTrack);

   if (mDebug)
   {
      auto out = ut::log::debug() << "Initiating local track from non-local track.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Local Track ID: " << localTrackPtr->GetTrackId();
      out.AddNote() << "From Track ID: " << aNonLocalTrack.GetTrackId();
   }

   localTrackPtr->ReplacementUpdate(aNonLocalTrack);

   // Initial fusion must take place before the track is announced as created.
   // Otherwise it is possible to broadcast a track with no valid data.
   mFusionStrategyPtr->InitializeLocalTrackFromNonLocalTrack(aSimTime, *localTrackPtr, aNonLocalTrack);

   // Reset the update count back to zero. Fusion may have caused the count to be updated, and we say that
   // a newly created track has an update count of zero (i.e.: it has never been updated...)
   localTrackPtr->SetUpdateCount(0);

   auto* tempPtr = localTrackPtr.get();
   mTrackList->AddTrack(std::move(localTrackPtr));
   return tempPtr;
}

// -------------------------------------------------------------------------------------------------
//! Notify the track manager that a raw track has been dropped.
//! This fact will be reflected in the track id list of the corresponding local track.
//! @param aSimTime The simulation time at which the track was dropped.
//! @param aRawTrackId The track id of the raw track that was dropped.
// virtual
void WsfTrackManager::DropTrack(double aSimTime, const WsfTrackId& aRawTrackId)
{
   // Remove any raw track correlations from the local track list.
   // Do this first, because it checks the raw track list.
   WsfLocalTrack* localTrackPtr = FindCorrelatedTrack(aRawTrackId);
   if (localTrackPtr)
   {
      DecorrelateLocalTrack(aSimTime, *localTrackPtr, aRawTrackId);
   }

   // Remove from the raw track list, unless we are retaining raw tracks.
   if (!mRetainRawTracks)
   {
      RemoveRawTrack(aSimTime, aRawTrackId);
   }
}

// -------------------------------------------------------------------------------------------------
void WsfTrackManager::DecorrelateLocalTrack(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrackId& aRawTrackId)
{
   if (mDebug)
   {
      auto out = ut::log::debug() << "Decorrelating raw track from local track.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Raw Track ID: " << aRawTrackId;
      out.AddNote() << "Local Track ID: " << aLocalTrack.GetTrackId();
   }

   // First, un-correlate from the local track.
   // This step is necessary to prevent problems during local track deletion, at which time
   // the associated local tracks might be deleted (and potentially we would be deleting this local track twice).
   if (aLocalTrack.Decorrelate(aRawTrackId))
   {
      if (mCorrelationStrategyPtr->GetCorrelatedTrackId(aRawTrackId) == aLocalTrack.GetTrackId())
      {
         // Inform the correlator of our action.
         mCorrelationStrategyPtr->Decorrelate(aRawTrackId);
      }

      // Inform observers
      NotifyOfLocalTrackDecorrelation(aSimTime, &aLocalTrack, aRawTrackId);

      // Certain parts of the track should be evaluated in the absence of the dropped component
      // (but only if the track is a fused track).
      if (aLocalTrack.IsFusible())
      {
         mFusionStrategyPtr->ReviewTrack(aLocalTrack);
      }
   }

   // As the perception of something may persist long after physical observations have ceased,
   // There is processing for option to drop uncorrelated tracks.
   // One may also *not* drop them, but elect to purge them after the next purge interval.
   //
   // If the use count is non-zero then this checked is deferred to UnlockTrack().

   if (mDropUncorrelatedTracks &&          // wants uncorrelated tracks to be dropped ...
       (!mWillPurgeInactiveTracks) &&      // ... unless automatic purging is being done
       (aLocalTrack.GetUseCount() == 0) && // track is not locked
       (!aLocalTrack.IsCorrelated()))      // no contributors...
   {
      if (mDebug)
      {
         auto out = ut::log::debug() << "Dropping local track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Track ID: " << aLocalTrack.GetTrackId();
         out.AddNote() << "Track is not correlated with any raw tracks.";
      }

      // Inform observers before deleting the track
      NotifyOfLocalTrackDropped(aSimTime, &aLocalTrack);

      // As there are no correlated raw tracks, we can delete the local track without worrying
      // about culling the raw track list.
      if (!mTrackList->DeleteTrack(aLocalTrack.GetTrackId()))
      {
         { // RAII block
            auto out = ut::log::error() << "Local track list is inconsistent.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         assert(false);
      }
   }
}

// -------------------------------------------------------------------------------------------------
//! Reflect a local track correlation in the local tracks raw track id list.
//! Then inform observers of the correlation.
//! @param aSimTime The current simulation time.
//! @param aLocalTrack The local track that has been correlated with an existing raw track report.
//! @param aRawTrackId The id of the raw track being correlated with the local track.
void WsfTrackManager::CorrelateLocalTrack(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrackId& aRawTrackId)
{
   if (aLocalTrack.Correlate(aRawTrackId)) // Reflect the correlation in the local track's raw track id list.
   {
      // Inform local observers.
      NotifyOfLocalTrackCorrelation(aSimTime, &aLocalTrack, aRawTrackId);
   }
}

// -------------------------------------------------------------------------------------------------
//! Remove all correlated raw track contributors to a local track.
// private
void WsfTrackManager::DeleteCorrelatedRawTracks(double aSimTime, WsfLocalTrack& aLocalTrack)
{
   const WsfLocalTrack::RawTrackIdList& idList = aLocalTrack.GetRawTrackIds();
   for (unsigned int trackNum = 0; trackNum < idList.GetCount(); ++trackNum)
   {
      const WsfTrackId* idPtr = idList.GetEntry(trackNum);
      if (!RemoveRawTrack(aSimTime, *idPtr))
      {
         { // RAII block
            auto out = ut::log::error() << "Raw track list inconsistent.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << mPlatformPtr->GetName();
         }
         assert(false);
      }
   }
}

// -------------------------------------------------------------------------------------------------
// virtual
//! Given a non-local (raw) track id, find a local track that has been correlated with it.
//!   @param aRawTrackId The non-local (raw) track id to use in correlating with local tracks.
//!   @return The local track, 0 if not found.
WsfLocalTrack* WsfTrackManager::FindCorrelatedTrack(const WsfTrackId& aRawTrackId)
{
   const WsfTrackId& localTrackId = mCorrelationStrategyPtr->GetCorrelatedTrackId(aRawTrackId);
   WsfLocalTrack*    trackPtr     = nullptr;
   if (!localTrackId.IsNull())
   {
      trackPtr = FindTrack(localTrackId);
   }
   if (mDebug)
   {
      auto out = ut::log::debug() << "Attempting to correlate track.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Track ID: " << aRawTrackId;
      if (trackPtr)
      {
         out.AddNote() << "Correlated with: " << trackPtr->GetTrackId();
      }
      else
      {
         out.AddNote() << "Not correlated.";
      }
   }

   return trackPtr;
}

// -------------------------------------------------------------------------------------------------
// static
//! Given a non-local (raw) track id, find a local track that has been correlated with it.
//!   @param aRawTrackId The non-local (raw) track id to use in correlating with local tracks.
//!   @param aLocalTrackList A local track list to use in finding the correlated track.
//!   @return The local track, 0 if not found.
WsfLocalTrack* WsfTrackManager::FindCorrelatedTrack(const WsfTrackId& aRawTrackId, WsfLocalTrackList& aLocalTrackList)
{
   // Once a correlation is made, we keep that correlation.
   // Find the previously correlated track, if any.
   WsfLocalTrack* trackPtr = nullptr;
   for (unsigned int trackNum = 0; trackNum < aLocalTrackList.GetTrackCount(); ++trackNum)
   {
      if (aLocalTrackList.GetTrackEntry(trackNum)->IsCorrelatedWith(aRawTrackId))
      {
         trackPtr = aLocalTrackList.GetTrackEntry(trackNum);
         break;
      }
   }
   return trackPtr;
}

// -------------------------------------------------------------------------------------------------
// virtual
//! For a framed input, signal end-of-frame.
//!   @note For framed input, this method should be called prior to
//!      calls to FindCorrelatedTrack.
void WsfTrackManager::EndFrame()
{
   mFrameStarted = false;
}

// -------------------------------------------------------------------------------------------------
// virtual
//! Create a new local track based on the given non-local, 'raw' track,
//! then inform local and global observers of this new track's creation.
//! Add a track as a correlated entry in the track list.
//!   @param aSimTime The current simulation time.
//!   @param aNonLocalTrack The input raw track to add to the local track list.
//!   @return A pointer to the added track.
//!   @note This method has a fundamentally different intent from the method,
//!         AddTrack(const WsfTrack& aTrack).  Whereas the AddTrack method
//!         inserts a "pre-briefed" track that cannot be purged or fused,
//!         this method inserts a new local track based on the input raw track.
WsfLocalTrack* WsfTrackManager::InitiateTrack(double aSimTime, const WsfTrack& aNonLocalTrack)
{
   WsfLocalTrack* localTrackPtr = nullptr;

   // Create a new local track based on the raw track.
   localTrackPtr = InitiateTrackP(aSimTime, aNonLocalTrack);
   mCorrelationStrategyPtr->NewTrackCorrelation(aNonLocalTrack, *localTrackPtr);
   CorrelateLocalTrack(aSimTime, *localTrackPtr, aNonLocalTrack.GetTrackId());

   // Notify interested parties of the new local track creation.
   NotifyOfLocalTrackInitiated(aSimTime, localTrackPtr, &aNonLocalTrack);

   // Debug-only sanity checks.
   // assert(localTrackPtr->IsCorrelatedWith(aNonLocalTrack.GetTrackId()));
   // assert(FindCorrelatedTrack(aNonLocalTrack.GetTrackId()) == localTrackPtr);

   return localTrackPtr;
}

// -------------------------------------------------------------------------------------------------
// protected
//! Invoke the correlation strategy to determine if a local track correlation exists for a
//! given non-local or 'raw' track.
//! Perform any housekeeping to ensure a consistent track list.
//! @param aSimTime The current simulation time.
//! @param aNonLocalTrack The non-local or 'raw' track to correlate against.
//! @return The corresponding local track, or 0 if the search fails.
WsfLocalTrack* WsfTrackManager::CorrelateTrack(double aSimTime, const WsfTrack& aNonLocalTrack)
{
   // Check existing correlation in case the track swaps.
   WsfLocalTrack* oldCorrelatedTrackPtr = FindCorrelatedTrack(aNonLocalTrack.GetTrackId());

   // Call the correlation strategy object to associate the raw track with an existing local track.
   WsfLocalTrack* correlatedLocalTrackPtr = mCorrelationStrategyPtr->Correlate(aSimTime, aNonLocalTrack, *mTrackList);

   if (correlatedLocalTrackPtr != oldCorrelatedTrackPtr) // Track swap, drop, or new local track.
   {
      // Decorrelate any old association.
      if (oldCorrelatedTrackPtr != nullptr)
      {
         DecorrelateLocalTrack(aSimTime, *oldCorrelatedTrackPtr, aNonLocalTrack.GetTrackId());
      }
      // Correlate any new association.
      // (new raw track with existing local track or existing raw track with existing local track).
      if (correlatedLocalTrackPtr != nullptr)
      {
         CorrelateLocalTrack(aSimTime, *correlatedLocalTrackPtr, aNonLocalTrack.GetTrackId());
      }
   }

   return correlatedLocalTrackPtr;
}

// -------------------------------------------------------------------------------------------------
// protected
//! Attempt to fuse a non-local track with a local track.
//!   @param aSimTime The current simulation time.
//!   @param aNonLocalTrack The non-local (raw) track to use for fusion with local tracks.
//!   @param aCorrelatedTrack The local track with which the non-local track was correlated.
void WsfTrackManager::FuseTrack(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrack& aCorrelatedTrack)
{
   if (mDebug)
   {
      auto out = ut::log::debug() << "Fusing non-local track with local track.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "External Track ID: " << aNonLocalTrack.GetTrackId();
      out.AddNote() << "Local Track ID: " << aCorrelatedTrack.GetTrackId();
   }

   // At this point, we have new data with which to update the local track.
   // Clear the 'stale' flag that may have been set during an earlier purge.
   if (aCorrelatedTrack.IsStale())
   {
      aCorrelatedTrack.SetStale(false);
   }

   if (mFusionStrategyPtr->UpdateLocalTrackFromNonLocalTrack(aSimTime, aCorrelatedTrack, aNonLocalTrack))
   {
      // If we receive a candidate track rather than having produced it directly, and a local track
      // was created from that track, then that local track will initially be marked as a candidate.
      // The only way to clear candidate status is then to track when a contributor is received that
      // is a not marked as such. ANY contributor will do - once candidate is cleared we never go back.
      if (aCorrelatedTrack.IsCandidate() && (!aNonLocalTrack.IsCandidate()))
      {
         aCorrelatedTrack.SetIsCandidate(false);
         // Notify observers that this track has been "promoted" to a full local track.
         NotifyOfCandidateTrackPromoted(aSimTime, &aCorrelatedTrack);
      }

      // Notify interested parties of the update.
      NotifyOfLocalTrackUpdated(aSimTime, &aCorrelatedTrack, &aNonLocalTrack);
   }
   // assert(FindCorrelatedTrack(aNonLocalTrack.GetTrackId()) != nullptr);
}

// -------------------------------------------------------------------------------------------------
//! Get the raw track corresponding with the given local track entry.
//!   @param aLocalTrackPtr The local track to use as the basis of the raw track search.
//!   @param aRawTrackEntry The entry containing the track id to search on.
//!   @return The corresponding raw track, or 0 if the search fails.
WsfTrack* WsfTrackManager::GetRawTrack(const WsfLocalTrack* aLocalTrackPtr, unsigned int aRawTrackEntry)
{
   WsfTrack*         trackPtr   = nullptr;
   const WsfTrackId* trackIdPtr = aLocalTrackPtr->GetRawTrackIds().GetEntry(aRawTrackEntry);
   if (trackIdPtr != nullptr)
   {
      trackPtr = mRawTrackList->FindTrack(*trackIdPtr);
   }
   return trackPtr;
}

// -------------------------------------------------------------------------------------------------
//! Removes the raw track corresponding to aTrackId
//! @return 'true' if the track was removed.
bool WsfTrackManager::RemoveRawTrack(double aSimTime, const WsfTrackId& aTrackId)
{
   bool removed(false);

   // Make a copy of the track id.  If the associated track ends up being deleted, and the track id
   // is part of the deleted track, it could be used both to delete from the raw track list,
   // then from the filtered raw track list, and for the second delete the parameter is potentially invalid.
   // *** DO NOT change to a WsfTrackId& as some static code analyzers will suggest.
   WsfTrackId dropTrackId(aTrackId);

   // If there are local observers, we must give observers access to the track before we delete it.
   WsfTrack* trackPtr = mRawTrackList->FindTrack(dropTrackId);
   if (trackPtr != nullptr)
   {
      // Notify observers before actually deleting the track.
      NotifyOfRawTrackDropped(aSimTime, trackPtr);
      removed = mRawTrackList->DeleteTrack(dropTrackId);
   }

   if (HasFilter())
   {
      mFilteredRawTrackList->DeleteTrack(dropTrackId);
   }

   return removed;
}

// -------------------------------------------------------------------------------------------------
//! Sets the correlation strategy used by this track manager.
void WsfTrackManager::SetCorrelationStrategy(WsfStringId aTypeName)
{
   auto& correlationTypes = WsfCorrelationStrategyTypes::Get(mScenario);
   // set the new strategy only if it is not equivalent to the current one,
   // and if the new strategy represents an existing strategy type.
   if ((mCorrelationStrategyPtr->GetTypeId() != aTypeName) && correlationTypes.Find(aTypeName))
   {
      delete mCorrelationStrategyPtr;
      mCorrelationStrategyPtr = nullptr;
      mCorrelationStrategyPtr = correlationTypes.Create(aTypeName.GetString());
      if (!mCorrelationStrategyPtr->Initialize(this))
      {
         assert(false);
      }
      WsfObserver::CorrelationStrategyChanged(GetSimulation())(GetSimulation()->GetSimTime(), mPlatformPtr);
   }
}

// -------------------------------------------------------------------------------------------------
//! Sets the fusion strategy used by this track manager.
void WsfTrackManager::SetFusionStrategy(WsfStringId aTypeName)
{
   auto& fusionTypes = WsfFusionStrategyTypes::Get(mScenario);
   // set the new strategy only if it is not equivalent to the current one,
   // and if the new strategy represents an existing strategy type.
   if ((mFusionStrategyPtr->GetTypeId() != aTypeName) && fusionTypes.Find(aTypeName))
   {
      delete mFusionStrategyPtr;
      mFusionStrategyPtr = nullptr;
      mFusionStrategyPtr = fusionTypes.Create(aTypeName.GetString());
      if (!mFusionStrategyPtr->Initialize(this))
      {
         assert(false);
      }
      WsfObserver::FusionStrategyChanged(GetSimulation())(GetSimulation()->GetSimTime(), mPlatformPtr);
   }
}

// -------------------------------------------------------------------------------------------------
//! Determine if an aux_data variable in a non-local (i.e.: raw) track will be allowed to update the local track.
// private
bool WsfTrackManager::CanUpdateAuxData(WsfStringId aNameId, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool        canUpdate          = true;
   WsfStringId thisPlatformNameId = GetPlatform()->GetNameId();

   AuxDataFusionRules::const_iterator fri = mAuxDataFusionRules.find(aNameId);
   if (fri != mAuxDataFusionRules.end())
   {
      AuxDataFusionRule rule = fri->second;
      if (rule == cADF_PRIVATE)
      {
         // Track updates cannot update this variable
         canUpdate = false;
      }
      else if (rule == cADF_ONLY_LOCAL)
      {
         // Only track updates from this platform can update this variable.
         canUpdate = (aLocalTrack.GetTrackId().GetOwningPlatformId() == thisPlatformNameId);
      }
      else if (rule == cADF_PREFER_LOCAL)
      {
         // The update will be allowed if the track is from this platform OR if it is from another platform
         // AND there are no other local contributors of this variable.
         if (aLocalTrack.GetTrackId().GetOwningPlatformId() != thisPlatformNameId)
         {
            const std::string& name(aNameId);

            // Search the associated raw track list and see if there are any local contributors with this variable
            const WsfLocalTrack::RawTrackIdList& rawTrackIds(aLocalTrack.GetRawTrackIds());
            unsigned int                         count = rawTrackIds.GetCount();
            for (unsigned int index = 0; index < count; ++index)
            {
               const WsfTrackId* rawTrackIdPtr = rawTrackIds.GetEntry(index);
               if (rawTrackIdPtr->GetOwningPlatformId() == thisPlatformNameId)
               {
                  // This raw track originated on this platform... see if it has this variable
                  const WsfTrack* rawTrackPtr = mRawTrackList->FindTrack(*rawTrackIdPtr);
                  if ((rawTrackPtr != nullptr) && rawTrackPtr->GetAuxDataConst().AttributeExists(name))
                  {
                     canUpdate = false;
                     break;
                  }
               }
            }
         }
      }
   }
   return canUpdate;
}

// -------------------------------------------------------------------------------------------------
// private
void WsfTrackManager::ProcessAuxDataFusionInput(UtInput& aInput)
{
   std::string command;

   mAuxDataFusionRules.clear();
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "variable")
      {
         std::string name;
         std::string method;
         aInput.ReadValue(name);
         aInput.ReadValue(method);
         AuxDataFusionRule rule;
         if (method == "private")
         {
            rule = cADF_PRIVATE;
         }
         else if (method == "only_local")
         {
            rule = cADF_ONLY_LOCAL;
         }
         else if (method == "prefer_local")
         {
            rule = cADF_PREFER_LOCAL;
         }
         else
         {
            throw UtInput::BadValue(inputBlock.GetInput(), "Invalid fusion method " + method);
         }
         // Note: The attribute containers use their own dictionary. We must use the same dictionary because the
         //       name id is a key in the attribute container, which uses its own dictionary...
         WsfStringId nameId          = name;
         mAuxDataFusionRules[nameId] = rule;
      }
      else
      {
         throw UtInput::UnknownCommand(inputBlock.GetInput());
      }
   }
}

// -------------------------------------------------------------------------------------------------
// The script interface 'class'
// -------------------------------------------------------------------------------------------------

class WSF_EXPORT WsfScriptTrackManager : public UtScriptClass
{
public:
   WsfScriptTrackManager(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(AddTrackReport);
   UT_DECLARE_SCRIPT_METHOD(RawTrackList);
   UT_DECLARE_SCRIPT_METHOD(FilteredRawTrackList);
   UT_DECLARE_SCRIPT_METHOD(LocalTrackList);
   UT_DECLARE_SCRIPT_METHOD(DropTrack);
   UT_DECLARE_SCRIPT_METHOD(Correlator);
};

// -------------------------------------------------------------------------------------------------
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfTrackManager::CreateScriptClass(const std::string& aClassName,
                                                                  UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptTrackManager>(aClassName, aScriptTypesPtr);
}

WsfSimulation* WsfTrackManager::GetSimulation() const
{
   return mPlatformPtr ? mPlatformPtr->GetSimulation() : nullptr;
}


// -------------------------------------------------------------------------------------------------
WsfScriptTrackManager::WsfScriptTrackManager(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTrackManager");

   AddMethod(ut::make_unique<AddTrackReport>());
   AddMethod(ut::make_unique<FilteredRawTrackList>());
   AddMethod(ut::make_unique<RawTrackList>());
   AddMethod(ut::make_unique<RawTrackList>("GetRawTrackList")); // NO_DOC | DEPRECATED (should not have the 'Get' prefix)
   AddMethod(ut::make_unique<LocalTrackList>());
   AddMethod(ut::make_unique<DropTrack>());
   AddMethod(ut::make_unique<Correlator>()); // NO_DOC | FOR_TEST_ONLY
}

// -------------------------------------------------------------------------------------------------
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackManager, WsfTrackManager, AddTrackReport, 1, "WsfLocalTrack", "WsfTrack")
{
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfTrack* trackPtr = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->AddTrackReport(simTime, *trackPtr), aReturnClassPtr));
}

// -------------------------------------------------------------------------------------------------
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackManager, WsfTrackManager, FilteredRawTrackList, 0, "WsfLocalTrackList", "")
{
   WsfLocalTrackList& filteredRawTrackList = aObjectPtr->GetFilteredRawTrackList();
   aReturnVal.SetPointer(new UtScriptRef(&filteredRawTrackList, aReturnClassPtr));
}

// -------------------------------------------------------------------------------------------------
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackManager, WsfTrackManager, RawTrackList, 0, "WsfTrackList", "")
{
   WsfTrackList& rawTrackList = aObjectPtr->GetRawTrackList();
   aReturnVal.SetPointer(new UtScriptRef(&rawTrackList, aReturnClassPtr));
}

// -------------------------------------------------------------------------------------------------
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackManager, WsfTrackManager, LocalTrackList, 0, "WsfLocalTrackList", "")
{
   WsfLocalTrackList& localTrackList = aObjectPtr->GetLocalTrackList();
   aReturnVal.SetPointer(new UtScriptRef(&localTrackList, aReturnClassPtr));
}

// -------------------------------------------------------------------------------------------------
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackManager, WsfTrackManager, DropTrack, 1, "void", "WsfTrackId")
{
   auto* trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->DropTrack(WsfScriptContext::GetTIME_NOW(aContext), *trackIdPtr);
}

// -------------------------------------------------------------------------------------------------
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackManager, WsfTrackManager, Correlator, 0, "WsfCorrelator", "")
{
   WsfCorrelationStrategy& correlator = aObjectPtr->GetCorrelationStrategy();
   aReturnVal.SetPointer(new UtScriptRef(&correlator, aReturnClassPtr));
}
