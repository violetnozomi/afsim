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

#ifndef WSFTRACKPROCESSOR_HPP
#define WSFTRACKPROCESSOR_HPP

#include "wsf_export.h"

#include <unordered_map>

#include "UtCallbackHolder.hpp"
#include "WsfEvent.hpp"
class WsfLocalTrack;
#include "script/WsfScriptProcessor.hpp"
class WsfTrack;
#include "WsfTrackId.hpp"
class WsfTrackManager;
class WsfTrackReportingStrategy;
class WsfScriptMessageHandler;

//! This is a processor incorporating track management and fusion capabilities.
//! This processor utilizes a track manager (see WsfTrackManager) to process non-local,
//! "raw" track reports (see WsfTrack) and into local, fused tracks (see WsfLocalTrack).
//!
//! This processor, as a WsfScriptProcessor, maintains internal and external links. Any
//! internally connected processors are notified of any event relevant to tracks
//! (creation, deletion, update, and drop) through internal links. Also, track drops and
//! periodic local track reports are sent over external links to command chain recipients.
//!
//! This processor may be configured as either the master track processor or as an
//! independent track processor.  If it is the master track processor (the default, also
//! triggered by user input "master_track_processor"), it maintains local tracks in the
//! platform's master track list (maintained by the platform's track manager
//! [see WsfTrackManager]).  If this processor is not configured as the master track
//! processor (user input "non_master_track_processor"), this object will create its own
//! track manager.  This separate track manager will maintain its own local tracks
//! separate from the master track list.
class WSF_EXPORT WsfTrackProcessor : public WsfScriptProcessor
{
public:
   WsfTrackProcessor(const WsfScenario& aScenario);
   WsfTrackProcessor& operator=(const WsfTrackProcessor&) = delete;
   ~WsfTrackProcessor() override;

   WsfProcessor* Clone() const override;
   bool          Initialize(double aSimTime) override;
   bool          ProcessInput(UtInput& aInput) override;
   bool          ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
   const char*   GetScriptClassName() const override;

   //! Return the track manager object referenced by this processor.
   WsfTrackManager& GetTrackManager() const { return *mTrackManagerPtr; }

   //! Allow track reporting strategies to access the external links object.
   wsf::ExternalLinks& GetExternalLinks() { return mExternalLinks; }

   bool InitializeMemberData(double aSimTime);

   void ConnectCallbacks();

   bool IsMasterTrackManager() const { return mMasterTrackProcessor; }

   virtual void SendTrack(double aSimTime, const WsfTrack& aTrack);

   virtual void SendTrack(double aSimTime, const WsfLocalTrack& aLocalTrack);

   //! @name Internal methods.
   //! The following methods are not to be considered part of the public interface.
   //@{
   virtual void PerformTrackPurging(double aSimTime);
   virtual void PerformTrackHistoryPurging(double aSimTime);
   virtual void PerformTrackReporting(double aSimTime);
   double       GetReportingInterval() const { return mReportInterval; }    // TODO-AWK HACK for Scenario Analyzer
   double       GetPurgeInterval() const { return mPurgeInterval; }         // TODO-AWK HACK for Scenario Analyzer
   bool         GetReportFusedTracks() const { return mReportFusedTracks; } // TODO-AWK HACK for Scenario Analyzer
   bool         GetReportRawTracks() const { return mReportRawTracks; }     // TODO-AWK HACK for Scenario Analyzer
   //@}

protected:
   WsfTrackProcessor(const WsfTrackProcessor& aSrc);

   virtual bool IsTrackReportable(double aSimTime, const WsfTrack* aTrackPtr);

   virtual void NotifyPlatform(double aSimTime, const WsfTrack* aChangedTrackPtr, int aChangedReason);

   void ProcessInboundFilterBlock(UtInput& aInput);

   void SelectNonMasterTrackProcessor();

   virtual void SendTrackP(double aSimTime, const WsfTrack& aTrack);

   virtual bool ShouldProcessInboundReport(double aSimTime, const WsfTrack& aTrack);

private:
   //! @name Track manager observer callbacks, and helper methods.
   //@{
   void CandidateTrackPromoted(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   void CurrentTargetChanged(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   void LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   void LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);

   void LocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);

   void RawTrackDropped(double aSimTime, const WsfTrack* aRawTrackPtr);

   void RawTrackReceived(double aSimTime, const WsfTrack* aRawTrackPtr);

   void SendTrackDropMessage(double aSimTime, const WsfTrack* aTrackPtr);

   void SendTrackNotifyMessage(double aSimTime, const WsfTrack* aTrackPtr, int aReason);

   void SendTrackUpdateMessage(double aSimTime, const WsfTrack* aTrackPtr);

   void TrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr);

   void WriteTrackDebugMessage(double aSimTime, const WsfTrack* aTrackPtr, const std::string& aReason);

   void SelectMasterTrackProcessor(UtInput& aInput);

   void SelectNonMasterTrackProcessor(UtInput& aInput);

   //@}

   //! The time between calls to purge inactive tracks.
   double mPurgeInterval;
   double mTrackHistoryRetentionInterval;

   //! The multiplier of the purge_interval used to define the purge interval for image tracks.
   //! This must be greater than or equal to 1.0.
   double mImagePurgeMultiplier;

   //! The time between track reports.
   double mReportInterval;

   bool mReportFusedTracks;
   bool mReportCandidateTracks;
   bool mReportRawTracks;
   bool mReportUnchangedTracks;
   bool mReportPassThrough;
   bool mMasterTrackProcessor;
   bool mTrackManagerTypeSelected;

   bool mRejectCircularReports;
   bool mRejectNonSensorReports;

   bool mReportOnUpdate;
   bool mUpdateOnReport;
   bool mUpdateOnReportSet;

   WsfStringId                mReportingStrategyName;
   WsfTrackReportingStrategy* mReportingStrategyPtr;

   UtCallbackHolder mCallbacks;

   //! A pointer to the track manager (master or alternate).
   WsfTrackManager* mTrackManagerPtr;

   //! A pointer to the 'is_track_reportable' script.
   UtScript* mIsTrackReportableScriptPtr;

   //! Maintains a record of the time when a raw track was explicitly dropped.
   //! This is used to discard updates for the same track that may be received later
   //! due to comm/processing delays. Entries are purged during track purging.
   using DroppedRawTrackList = std::unordered_map<WsfTrackId, double, WsfTrackId>;
   DroppedRawTrackList mDroppedRawTrackList;

   //! The name ID of the source of the current message being processed.
   //! This is the value of WsfTrackMessage::GetSenderId while processing the current message.
   //! It is zero at other times.
   WsfStringId mSourceId;

   using LastReportTimeMap = std::unordered_map<WsfTrackId, double, WsfTrackId>;
   LastReportTimeMap mLastReportTime;
};

//! A nested class for the event that performs periodic purging of inactive tracks.
class WsfTrackProcessor_PurgeTracksEvent : public WsfEvent
{
public:
   WsfTrackProcessor_PurgeTracksEvent() = default;
   WsfTrackProcessor_PurgeTracksEvent(double aSimTime, WsfTrackProcessor* aProcessorPtr, double aPurgeInterval);

   EventDisposition Execute() override;

protected:
   double             mPurgeInterval;
   WsfTrackProcessor* mProcessorPtr;
   size_t             mPlatformIndex;
};

//! The same idea is used to purge track history.
class WsfTrackProcessor_PurgeTrackHistoryEvent : public WsfTrackProcessor_PurgeTracksEvent
{
public:
   WsfTrackProcessor_PurgeTrackHistoryEvent() = default;
   WsfTrackProcessor_PurgeTrackHistoryEvent(double aSimTime, WsfTrackProcessor* aProcessorPtr, double aPurgeInterval);

   EventDisposition Execute() override;
};

//! A nested class for the event that performs periodic reporting of tracks.
class WsfTrackProcessor_ReportTracksEvent : public WsfEvent
{
public:
   WsfTrackProcessor_ReportTracksEvent() = default;
   WsfTrackProcessor_ReportTracksEvent(double aSimTime, WsfTrackProcessor* aProcessorPtr, double aReportInterval);

   EventDisposition Execute() override;

private:
   double             mReportInterval;
   WsfTrackProcessor* mProcessorPtr;
   size_t             mPlatformIndex;
};

#endif
