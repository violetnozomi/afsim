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

#ifndef WSFTRACKMANAGER_HPP
#define WSFTRACKMANAGER_HPP

#include "wsf_export.h"

#include <map>
#include <memory>
#include <string>

#include "UtCallback.hpp"
class UtInput;
class UtScriptTypes;

class WsfCorrelationStrategy;
class WsfFilter;
class WsfFusionStrategy;
class WsfLocalTrack;
class WsfPlatform;
class WsfScenario;
#include "WsfSimpleComponent.hpp"
class WsfSimulation;
#include "WsfTrackList.hpp"

//! The Track Manager maintains a 'local' track list.
//!
//! The track manager is used to maintain a list of 'local tracks' (as defined
//! by WsfLocalTrack).  The caller provides 'track reports' or 'raw tracks'
//! (as defined by WsfTrack) which are then either fused with existing local tracks
//! or used to start a new local track. This type of object is also known as a "tracker."
//!
//! The standard implementation of WsfTrackManager provides perfect correlation and
//! a default track fusion.  In order to provide an alternate capability,
//! one should define alternate correlation and fusion strategies through sub-classing
//! of WsfCorrelationStrategy and WsfFusionStrategy (see associated class documentation for
//! further details).  These strategies are then specified through the user input keywords
//! "correlation_method" and "fusion_method" (see the WSF User Input Guide for details).
//!
//! The local track contains a list of correlated track ids.  Also the
//! non-local or "raw" tracks themselves can be accessed through the raw track list,
//! if desired (see WsfTrackManager::GetRawTrackList).
//!
//! There are two mechanisms for purging local tracks from the track list
//!
//! <ul>
//! <li>The default method is to automatically drop the local track when all of its
//! constituent raw tracks have been dropped.
//! <li>Invoke the method SetDropUncorrelatedTracks(true) and periodically invoke the
//! method PurgeInactiveTracks or use some custom method for purging obsolete tracks.
//! </ul>
class WSF_EXPORT WsfTrackManager : public WsfSimplePlatformComponent
{
public:
   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   WsfTrackManager() = default;
   WsfTrackManager(const WsfScenario& aScenario);
   WsfTrackManager(const WsfTrackManager& aSrc);
   ~WsfTrackManager() override;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   void*         QueryInterface(int aRole) override { return (aRole == mRoles[0]) ? this : nullptr; }
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_TRACK_MANAGER; }
   void          ComponentParentChanged(WsfPlatform* aPlatformPtr) override { mPlatformPtr = aPlatformPtr; }
   //@}

   //! Create a copy, or clone, of "this" object.
   //! @return A pointer to the cloned object.
   virtual WsfTrackManager* Clone() const { return new WsfTrackManager(*this); }

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;

   virtual bool Initialize(double aSimTime, WsfPlatform* aPlatformPtr);

   //! Return the platform with which this track manager is associated.
   WsfPlatform* GetPlatform() const { return mPlatformPtr; }

   //! Return the correlation strategy used by this track manager.
   WsfCorrelationStrategy&       GetCorrelationStrategy() { return *mCorrelationStrategyPtr; }
   const WsfCorrelationStrategy& GetCorrelationStrategy() const { return *mCorrelationStrategyPtr; }

   //! Return the fusion strategy used by this track manager.
   WsfFusionStrategy&       GetFusionStrategy() { return *mFusionStrategyPtr; }
   const WsfFusionStrategy& GetFusionStrategy() const { return *mFusionStrategyPtr; }

   //! Sets the correlation strategy used by this track manager.
   void SetCorrelationStrategy(WsfStringId aTypeName);

   //! Sets the fusion strategy used by this track manager.
   void SetFusionStrategy(WsfStringId aTypeName);

   //! Indicate if local tracks should be dropped if they become uncorrelated.
   //! @param aDropUncorrelatedTracks If 'true' then local tracks will be dropped if they become uncorrelated.
   //! If 'false' then they will not be dropped when they become uncorrelated and must be dropped by some
   //! other mechanism (such as periodically calling PurgeInactiveTracks()).
   void SetDropUncorrelatedTracks(bool aDropUncorrelatedTracks) { mDropUncorrelatedTracks = aDropUncorrelatedTracks; }

   //! Set whether this instance will print diagnostic message to standard output.
   void SetDebug(bool aDebug) { mDebug = aDebug; }

   //! Return whether debug is enabled for this instance.
   bool DebugEnabled() const { return mDebug; }

   //! Set whether the tracker should initiate local tracks from a raw track,
   //! if no correlation is made by the correlation strategy (default is to initiate local tracks).
   void SetInitiateLocalTracks(bool aInitiateLocalTracks) { mInitiateLocalTracks = aInitiateLocalTracks; }

   //! Accessor to determine if local tracks are initiated (true by default).
   bool LocalTracksInitiated() const { return mInitiateLocalTracks; }

   //! @name External track purging.
   //@{
   virtual void PurgeInactiveTracks(double aSimTime, double aInactivityTimeLimit);

   virtual void PurgeInactiveRawTracks(double aSimTime, double aInactivityTimeLimit, double aStaticInactivityTimeLimit);

   virtual void PurgeLocalTrackHistory(double aSimTime, double aKeepTimeInterval);

   //! Indicate that some external process will be performing period purging of inactive tracks.
   //! This is simply a hint
   void SetWillPurgeInactiveTracks(bool aWillPurgeInactiveTracks)
   {
      mWillPurgeInactiveTracks = aWillPurgeInactiveTracks;
   }

   //! Will some external process perform period purging of inactive tracks.
   bool WillPurgeInactiveTracks() const { return mWillPurgeInactiveTracks; }

   //@}

   //! @name Track interlock methods.
   //! These are used by external controllers (e.g.: WsfTaskManager) to prevent a track from being
   //! purged at an inopportune time.
   //@{
   virtual void LockTrack(double aSimTime, const WsfTrackId& aLocalTrackId);

   virtual void LockTrack(double aSimTime, WsfLocalTrack* aLocalTrackPtr);

   virtual void UnlockTrack(double aSimTime, const WsfTrackId& aLocalTrackId);

   virtual void UnlockTrack(double aSimTime, WsfLocalTrack* aLocalTrackPtr);
   //@}

   //! @name Fusion methods:
   //{@
   virtual void StartFrame();

   virtual void EndFrame();

   virtual WsfLocalTrack* AddTrackReport(double aSimTime, const WsfTrack& aTrackUpdate);

   virtual WsfTrack* AddRawTrackReport(double aSimTime, const WsfTrack& aRawTrack);

   virtual void DropTrack(double aSimTime, const WsfTrackId& aRawTrackId);

   std::unique_ptr<WsfLocalTrack> RemoveTrack(const WsfTrackId& aTrackId);

   virtual WsfLocalTrack* FindCorrelatedTrack(const WsfTrackId& aRawTrackId);

   static WsfLocalTrack* FindCorrelatedTrack(const WsfTrackId& aRawTrackId, WsfLocalTrackList& aLocalTrackList);
   //@}

   //! @name 'Pre-brief' track methods.
   //@{
   virtual WsfLocalTrack* AddTrack(double aSimTime, const WsfTrack& aTrack);

   void AddInitialTrack(const WsfTrack& aTrack);

   virtual WsfLocalTrack* InitiateTrack(double aSimTime, const WsfTrack& aRawTrack);

   WsfLocalTrack* InitiateTrackSilent(double aSimTime, const WsfTrack& aRawTrack);

   //! Deprecated; use InitiateTrackSilent instead.
   WsfLocalTrack* InitiateAndCorrelateTrack(double aSimTime, const WsfTrack& aRawTrack)
   {
      return InitiateTrackSilent(aSimTime, aRawTrack);
   }
   //@}

   //! @name Methods that access the track lists directly:
   //@{
   //! Get the number of tracks in the track list.
   //!   This method, along with GetTrackEntry() can be used to iterate through
   //!   the track list.
   //!   @return The number of entries in the track list.
   unsigned int GetTrackCount() const { return mTrackList->GetTrackCount(); }

   //! Get a track from the track list by its entry index (offset) into the list.
   //!   This version returns a pointer to a track so the track can be modified.
   //!   @param aEntryIndex Index of the desired entry. The index must be in the range
   //!   0 <= aEntryIndex < GetTrackCount().
   WsfLocalTrack* GetTrackEntry(unsigned int aEntryIndex) { return mTrackList->GetTrackEntry(aEntryIndex); }

   //! Get the number of tracks in the raw track list.
   //!   This method, along with GetRawTrackEntry() can be used to iterate
   //!   through the track list.
   //!   @return The number of entries in the track list.
   unsigned int GetRawTrackCount() { return mRawTrackList->GetTrackCount(); }

   //! Get a raw track from the raw track list by its entry index (offset)
   //!   into the list. This version returns a pointer to a track so the
   //!   track can be modified.
   //!   @param aEntryIndex Index of the desired entry.
   //!   The index must be in the range  0 <= aEntryIndex < GetTrackCount()
   WsfTrack* GetRawTrackEntry(unsigned int aEntryIndex) { return mRawTrackList->GetTrackEntry(aEntryIndex); }

   //! Find the requested track.
   //!   This version will return a pointer to the track so it can be modified.
   //!   @param aLocalTrackId The track ID of the local track to be located.
   //!   @return Pointer to track or 0 if the requested track could not be found.
   WsfLocalTrack* FindTrack(const WsfTrackId& aLocalTrackId) { return mTrackList->FindTrack(aLocalTrackId); }

   //! Find the requested track report.
   //!   This version will return a pointer to the track so it can be modified.
   //!   @param aRawTrackId The track ID of the track report to be located.
   //!   @return Pointer to track or 0 if the requested track could not be found.
   WsfTrack* FindRawTrack(const WsfTrackId& aRawTrackId) { return mRawTrackList->FindTrack(aRawTrackId); }

   //! Return the aggregated local track list.
   //! @note Use this method with caution.  Direct use of the track list may have unexpected results.
   WsfLocalTrackList& GetTrackList() { return *mTrackList; }

   //! Return the sensor raw track list.
   //! @note Use this method with caution.  Direct use of the sensor track list may have unexpected results.
   WsfTrackList& GetRawTrackList() { return *mRawTrackList; }

   //! Return the sensor local track list.
   //! @note Use this method with caution.  Direct use of the sensor track list may have unexpected results.
   WsfLocalTrackList& GetLocalTrackList() { return *mTrackList; }

   //! Return the filtered sensor track list.
   //! @note Use this method with caution.  Direct use of the sensor track list may have unexpected results.
   WsfLocalTrackList& GetFilteredRawTrackList() { return *mFilteredRawTrackList; }

   //! Return the filtered sensor track list (const version).
   const WsfLocalTrackList& GetFilteredRawTrackList() const { return *mFilteredRawTrackList; }

   WsfTrack* GetRawTrack(const WsfLocalTrack* aLocalTrackPtr, unsigned int aRawTrackEntry);

   bool HasFilter() { return (mPrototypeFilterPtr != nullptr); }

   //@}

   //! @name 'current target' methods.
   //@{
   const WsfTrack* GetCurrentTarget();

   //! Get the track ID for the current target of interest.
   const WsfTrackId GetCurrentTargetTrackId() const { return mCurrentTargetId; }

   WsfTrackId SetCurrentTarget(double aSimTime, const WsfTrack& aTrack);

   //! Indicate the current target is no longer a target.
   //! @note This does not purge the track from the track list. It simply indicates that
   //! the current target is no longer a target.
   void ClearCurrentTarget() { mCurrentTargetId.Null(); }

   //@}

   //! @name Track status notification methods.
   //!
   //! External processors that implement track management functions (i.e.: directly maintain the local
   //! and raw track lists contained in the object) can call these methods to perform the standard
   //! notification processes would otherwise occur if this class were performing track maintenance.
   //@{
   void NotifyOfCandidateTrackPromoted(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   void NotifyOfCurrentTargetChanged(double aSimTime, const WsfLocalTrack* aRawTrackPtr);

   void NotifyOfLocalTrackCorrelation(double aSimTime, const WsfLocalTrack* aTrackPtr, const WsfTrackId& aRawTrackId);

   void NotifyOfLocalTrackDecorrelation(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrackId& aRawTrackId);

   void NotifyOfLocalTrackDropped(double aLocalSimTime, const WsfLocalTrack* aTrackPtr);

   void NotifyOfLocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);

   void NotifyOfLocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);

   void NotifyOfRawTrackDropped(double aSimTime, const WsfTrack* aTrack);

   void NotifyOfRawTrackReceived(double aSimTime, const WsfTrack* aRawTrackPtr);

   void NotifyOfTrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr);
   //@}

   //! @name Callback lists.
   //! Other objects may register callbacks that are to be invoked during track state changes.
   //!
   //@{
   UtCallbackListN<void(double, const WsfLocalTrack*)>                    CandidateTrackPromoted;
   UtCallbackListN<void(double, const WsfLocalTrack*)>                    CurrentTargetChanged;
   UtCallbackListN<void(double, const WsfLocalTrack*, const WsfTrackId&)> LocalTrackCorrelation;
   UtCallbackListN<void(double, const WsfLocalTrack*, const WsfTrackId&)> LocalTrackDecorrelation;
   UtCallbackListN<void(double, const WsfLocalTrack*, const WsfTrack*)>   LocalTrackInitiated;
   UtCallbackListN<void(double, const WsfLocalTrack*)>                    LocalTrackDropped;
   UtCallbackListN<void(double, const WsfLocalTrack*, const WsfTrack*)>   LocalTrackUpdated;
   UtCallbackListN<void(double, const WsfTrack*)>                         RawTrackDropped;
   UtCallbackListN<void(double, const WsfTrack*)>                         RawTrackReceived;
   UtCallbackListN<void(double, WsfTrackManager*)>                        TrackManagerChanged;
   //@}

   bool UpdateAuxData(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack);

   //! Common track status indications.
   //! @note: These MUST remain in this order because they appear in WsfTrackNotifyMessage and are
   //!        accessible to users via the script interface.
   enum TrackStatusIndication
   {
      cNONE,                     //!< There is no change, or the code is invalid.
      cCREATED,                  //!< A local track has been created.
      cUPDATED,                  //!< A local track has been updated.
      cDROPPED,                  //!< A local track has been dropped.
      cCANDIDATE_TRACK_PROMOTED, //!< A "candidate" local track has been promoted to a full local track.
      cRAW_TRACK_DROPPED,        //!< A raw track has been dropped.
      cRAW_TRACK_RECEIVED,       //!< An unprocessed raw track was received.
      cCURRENT_TARGET_CHANGED,   //!< The current target has changed.
      cCORRELATED,               //!< A local track has been correlated.
      cDECORRELATED              //!< A local track has been decorrelated.
   };

   enum AuxDataFusionRule
   {
      cADF_PRIVATE,
      cADF_ONLY_LOCAL,
      cADF_PREFER_LOCAL
   };

   WsfSimulation* GetSimulation() const;

protected:
   WsfLocalTrack* InitiateTrackP(double aSimTime, const WsfTrack& aTrack);

   void AssessTrackQuality(double aSimTime, WsfTrack* aTrackPtr);

   WsfLocalTrack* CorrelateTrack(double aSimTime, const WsfTrack& aNonLocalTrack);

   void FuseTrack(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrack& aLocalTrackPtr);

   WsfLocalTrack* AddMeasurement(double aSimTime, const WsfTrack& aMeasurement);

   WsfLocalTrack* GetPrototypeTrack() const { return mPrototypeTrackPtr; }

   const WsfScenario& GetScenario() const { return mScenario; }

private:
   WsfTrackManager& operator=(const WsfTrackManager&) = delete; // not implemented

   using AuxDataFusionRules = std::map<WsfStringId, AuxDataFusionRule>;

   void DecorrelateLocalTrack(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrackId& aRawTrack);

   void CorrelateLocalTrack(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrackId& aRawTrack);

   void DeleteCorrelatedRawTracks(double aSimTime, WsfLocalTrack& aLocalTrack);

   bool RemoveRawTrack(double aSimTime, const WsfTrackId& aTrackId);

   bool CanUpdateAuxData(WsfStringId aNameId, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack);

   void ProcessAuxDataFusionInput(UtInput& aInput);

   const WsfScenario& mScenario;

   //! The platform to which the track manager is attached.
   WsfPlatform* mPlatformPtr;

   //! 'true' if the track manager is initialized (used by SetCurrentTarget).
   bool mIsInitialized;

   //! If true, some external process has indicated that it will purge inactive tracks.
   bool mWillPurgeInactiveTracks;

   //! Drop local, uncorrelated tracks with no raw track contributors
   bool mDropUncorrelatedTracks;

   //! Keep all raw tracks, instead of deleting after one reporting interval
   bool mRetainRawTracks;

   //! Maintain a raw track history for all local tracks
   bool mMaintainTrackHistory;

   //! Frame-based variables (currently not used).
   bool mIsFramed;
   bool mFrameStarted;

   //! Configuration variables
   bool mDebug;
   bool mInitiateLocalTracks;

   //! The prototype track from which new local tracks are constructed.
   WsfLocalTrack* mPrototypeTrackPtr;

   //! The track lists maintained by this tracker.
   WsfLocalTrackList* mTrackList; // This is the 'master' or primary track list maintained.
   WsfLocalTrackList* mFilteredRawTrackList;
   WsfTrackList*      mRawTrackList;

   //! The track ID of the 'current target'.
   WsfTrackId mCurrentTargetId;

   //! 'Initial' tracks that are to be added to the track manager when it is initialized.
   //! These are typically read from the input file.
   using InitialTrackList = std::vector<WsfTrack*>;
   InitialTrackList mInitialTracks;
   WsfTrackId       mInitialTargetId;

   //! These are filtering and fusion strategies, and a prototype filter for all tracks.
   //! They are configured based on input values.
   WsfCorrelationStrategy* mCorrelationStrategyPtr;
   WsfFusionStrategy*      mFusionStrategyPtr;
   WsfFilter*              mPrototypeFilterPtr;

   AuxDataFusionRules mAuxDataFusionRules;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfTrackManager, cWSF_COMPONENT_TRACK_MANAGER)

#endif
