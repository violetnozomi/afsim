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

#include "WsfTrackProcessor.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <limits>
#include <list>

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "UtScriptRef.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackNotifyMessage.hpp"
#include "WsfTrackReportingStrategy.hpp"
#include "WsfTrackReportingStrategyTypes.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageHandler.hpp"

// ================================================================================================
//! The default constructor
WsfTrackProcessor::WsfTrackProcessor(const WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario)
   , mPurgeInterval(0.0)
   , mTrackHistoryRetentionInterval(3600.0)
   , // Default of one hour.
   mImagePurgeMultiplier(1.0)
   , mReportInterval(0.0)
   , mReportFusedTracks(false)
   , // We do not report fused tracks by default.
   mReportCandidateTracks(false)
   , // We do not report candidate tracks by default
   mReportRawTracks(true)
   , // We do report raw tracks by default.
   mReportUnchangedTracks(true)
   , // We do report redundant data by default.
   mReportPassThrough(false)
   , // We do not report raw sensor-based tracks by default.
   mMasterTrackProcessor(true)
   , // This is the mater track processor by default.
   mTrackManagerTypeSelected(false)
   , mRejectCircularReports(false)
   , mRejectNonSensorReports(false)
   , mReportOnUpdate(false)
   , mUpdateOnReport(false)
   , mUpdateOnReportSet(false)
   , mReportingStrategyName("batch")
   , mReportingStrategyPtr(nullptr)
   , mCallbacks()
   , mTrackManagerPtr(nullptr)
   , mIsTrackReportableScriptPtr(nullptr)
   , mDroppedRawTrackList()
   , mSourceId(nullptr)
   , mLastReportTime()
{
}

// ================================================================================================
// virtual
WsfTrackProcessor::~WsfTrackProcessor()
{
   delete mReportingStrategyPtr;
   if (!mMasterTrackProcessor)
   {
      delete mTrackManagerPtr;
   }
}

// ================================================================================================
WsfTrackProcessor::WsfTrackProcessor(const WsfTrackProcessor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mPurgeInterval(aSrc.mPurgeInterval)
   , mTrackHistoryRetentionInterval(aSrc.mTrackHistoryRetentionInterval)
   , mImagePurgeMultiplier(aSrc.mImagePurgeMultiplier)
   , mReportInterval(aSrc.mReportInterval)
   , mReportFusedTracks(aSrc.mReportFusedTracks)
   , mReportCandidateTracks(aSrc.mReportCandidateTracks)
   , mReportRawTracks(aSrc.mReportRawTracks)
   , mReportUnchangedTracks(aSrc.mReportUnchangedTracks)
   , mReportPassThrough(aSrc.mReportPassThrough)
   , mMasterTrackProcessor(aSrc.mMasterTrackProcessor)
   , mTrackManagerTypeSelected(aSrc.mTrackManagerTypeSelected)
   , mRejectCircularReports(aSrc.mRejectCircularReports)
   , mRejectNonSensorReports(aSrc.mRejectNonSensorReports)
   , mReportOnUpdate(aSrc.mReportOnUpdate)
   , mUpdateOnReport(aSrc.mUpdateOnReport)
   , mUpdateOnReportSet(aSrc.mUpdateOnReportSet)
   , mReportingStrategyName(aSrc.mReportingStrategyName)
   , mReportingStrategyPtr(nullptr)
   , mCallbacks()
   , mTrackManagerPtr(nullptr)
   , mIsTrackReportableScriptPtr(nullptr)
   , mDroppedRawTrackList()
   , mSourceId(nullptr)
   , mLastReportTime()
{
   if ((!mMasterTrackProcessor) && (aSrc.mTrackManagerPtr != nullptr))
   {
      mTrackManagerPtr = new WsfTrackManager(*aSrc.mTrackManagerPtr);
   }
   if (aSrc.mReportingStrategyPtr != nullptr)
   {
      mReportingStrategyPtr = aSrc.mReportingStrategyPtr->Clone();
      mReportingStrategyPtr->SetTrackProcessor(*this);
   }
}

// ================================================================================================
// virtual
WsfProcessor* WsfTrackProcessor::Clone() const
{
   return new WsfTrackProcessor(*this);
}

// ================================================================================================
// virtual
bool WsfTrackProcessor::Initialize(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize(aSimTime);

   if (mMasterTrackProcessor)
   {
      // Establish the link to the track manager, now that we have a platform pointer.
      mTrackManagerPtr = &(GetPlatform()->GetTrackManager());
   }
   else
   {
      // Initialize our non-master track manager.
      mTrackManagerPtr->Initialize(aSimTime, GetPlatform());
   }
   ok &= InitializeMemberData(aSimTime);
   return ok;
}

const char* WsfTrackProcessor::GetScriptClassName() const
{
   return "WsfTrackProcessor";
}

bool WsfTrackProcessor::InitializeMemberData(double aSimTime)
{
   bool ok = true;
   // If a purge_interval has been defined then inform the track manager that we will be performing that activity.
   if (mPurgeInterval > 0.0)
   {
      mTrackManagerPtr->SetWillPurgeInactiveTracks(true);
   }

   // If there are no external links then cancel the report interval as it would do nothing.
   if ((mReportInterval != 0.0) && (!mExternalLinks.HasLinks()))
   {
      // Issue a warning if the report interval was set with no external links.
      auto out =
         ut::log::warning() << "Track processor will not report tracks as it has no external communication links.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      mReportInterval = 0.0;
   }
   else if ((mReportInterval == 0.0) && (mExternalLinks.HasLinks()))
   {
      // Set a default report interval.
      mReportInterval = 10.0;
   }

   // If track reporting is to be performed, allocate the reporting strategy object if needed.
   if ((mReportInterval > 0.0) && (!mReportOnUpdate))
   {
      mReportingStrategyPtr = WsfTrackReportingStrategyTypes::Get(GetScenario()).Create(mReportingStrategyName, *this);
      if (mReportingStrategyPtr == nullptr)
      {
         ok = false;
      }
   }

   // Capture the pointer to the 'is_track_reportable' script and ensure it has the proper signature.
   mIsTrackReportableScriptPtr = mContextPtr->FindScript("is_track_reportable");
   if (mIsTrackReportableScriptPtr != nullptr)
   {
      ok &= mContextPtr->ValidateScript(mIsTrackReportableScriptPtr, "bool", "WsfTrack");
   }

   if (ok)
   {
      ConnectCallbacks();


      // Schedule events to perform track purging and track reporting.
      if (mPurgeInterval > 0.0)
      {
         double startTime = aSimTime + GetSimulation()->GetRandom().Uniform(0.0, mPurgeInterval);
         GetSimulation()->AddEvent(ut::make_unique<WsfTrackProcessor_PurgeTracksEvent>(startTime, this, mPurgeInterval));
      }

      if (mTrackHistoryRetentionInterval > 0.0)
      {
         double startTime = aSimTime + GetSimulation()->GetRandom().Uniform(0.0, mTrackHistoryRetentionInterval);
         GetSimulation()->AddEvent(
            ut::make_unique<WsfTrackProcessor_PurgeTrackHistoryEvent>(startTime, this, mTrackHistoryRetentionInterval));
      }

      if ((mReportInterval > 0.0) && (mReportingStrategyPtr != nullptr))
      {
         double startTime = aSimTime + GetSimulation()->GetRandom().Uniform(0.0, mReportInterval);
         GetSimulation()->AddEvent(ut::make_unique<WsfTrackProcessor_ReportTracksEvent>(startTime, this, mReportInterval));
      }
   }

   if (GetScenario().UseQuantitativeTrackQuality() && (!mUpdateOnReportSet))
   {
      mUpdateOnReport = true;
   }

   return ok;
}

// ================================================================================================
// virtual
bool WsfTrackProcessor::ProcessInput(UtInput& aInput)
{
   bool        wasProcessed = true;
   std::string command(aInput.GetCommand());
   if ((!mMasterTrackProcessor) && mTrackManagerPtr->ProcessInput(aInput))
   {
   }
   else if (command == "master_track_processor")
   {
      SelectMasterTrackProcessor(aInput);
   }
   else if (command == "non_master_track_processor")
   {
      SelectNonMasterTrackProcessor(aInput);
   }
   else if (command == "report_fused_tracks")
   {
      mReportFusedTracks = true;
      mReportRawTracks   = false;
   }
   else if (command == "report_candidate_tracks")
   {
      mReportCandidateTracks = true;
   }
   else if (command == "report_unchanged_tracks")
   {
      mReportUnchangedTracks = true;
   }
   else if (command == "report_raw_tracks")
   {
      mReportRawTracks   = true;
      mReportFusedTracks = false;
   }
   else if (command == "report_pass_through")
   {
      mReportPassThrough = true;
   }
   else if (command == "fused_track_reporting")
   {
      aInput.ReadValue(mReportFusedTracks);
      mReportRawTracks = !mReportFusedTracks;
   }
   else if (command == "unchanged_track_reporting")
   {
      aInput.ReadValue(mReportUnchangedTracks);
   }
   else if (command == "candidate_track_reporting")
   {
      aInput.ReadValue(mReportCandidateTracks);
   }
   else if (command == "raw_track_reporting")
   {
      aInput.ReadValue(mReportRawTracks);
      mReportFusedTracks = !mReportRawTracks;
   }
   else if (command == "pass_through_reporting")
   {
      aInput.ReadValue(mReportPassThrough);
   }
   else if (command == "circular_report_rejection")
   {
      aInput.ReadValue(mRejectCircularReports);
   }
   else if (command == "report_interval")
   {
      aInput.ReadValueOfType(mReportInterval, UtInput::cTIME);
      aInput.ValueGreater(mReportInterval, 0.0);
   }
   else if (command == "report_method")
   {
      std::string reportingStrategyString;
      aInput.ReadValue(reportingStrategyString);
      mReportingStrategyName = reportingStrategyString;

      mReportOnUpdate = false;
      if (reportingStrategyString == "on_update")
      {
         mReportOnUpdate        = true;
         mReportUnchangedTracks = false;
         mRejectCircularReports = true;
      }
      else if (reportingStrategyString == "on_update_fused")
      {
         mReportOnUpdate        = true;
         mReportFusedTracks     = true;
         mReportRawTracks       = false;
         mReportUnchangedTracks = false;
         mRejectCircularReports = true;
      }
      else if (WsfTrackReportingStrategyTypes::Get(GetScenario()).IsType(reportingStrategyString))
      {
      }
      else
      {
         throw UtInput::BadValue(aInput, "Reporting strategy type " + reportingStrategyString + " not recognized.");
      }
   }
   else if (command == "update_on_report")
   {
      aInput.ReadValue(mUpdateOnReport);
      mUpdateOnReportSet = true;
   }
   else if ((command == "purge_interval") || (command == "drop_after_inactive")) // Save for Initialize
   {
      aInput.ReadValueOfType(mPurgeInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mPurgeInterval, 0.0);
   }
   else if (command == "image_purge_multiplier")
   {
      aInput.ReadValue(mImagePurgeMultiplier);
      aInput.ValueGreaterOrEqual(mImagePurgeMultiplier, 1.0);
   }
   else if (command == "track_history_retention_interval")
   {
      aInput.ReadValueOfType(mTrackHistoryRetentionInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mTrackHistoryRetentionInterval, 0.0);
   }
   else if (command == "inbound_filter")
   {
      ProcessInboundFilterBlock(aInput);
   }
   else if (WsfScriptProcessor::ProcessInput(aInput))
   {
   }
   else if (command == "track_manager")
   {
      // Make sure this is configured as a non-master track processor.
      SelectNonMasterTrackProcessor(aInput);

      UtInputBlock block(aInput, "end_track_manager");
      while (block.ReadCommand())
      {
         wasProcessed = mTrackManagerPtr->ProcessInput(aInput);
         if (!wasProcessed)
         {
            break;
         }
      }
   }
   else
   {
      wasProcessed = false;
   }
   return wasProcessed;
}

// ================================================================================================
//! Process the 'inbound_filter' input block.
// protected
void WsfTrackProcessor::ProcessInboundFilterBlock(UtInput& aInput)
{
   // This is VERY simple for now. It only accepts the 'reject non_sensor_tracks' command.
   UtInputBlock inputBlock(aInput);
   std::string  command;

   while (inputBlock.ReadCommand(command))
   {
      if (command == "reject")
      {
         std::string what;
         aInput.ReadValue(what);
         if (what == "non_sensor_reports")
         {
            mRejectNonSensorReports = true;
         }
         else if (what == "nothing")
         {
            mRejectNonSensorReports = false;
         }
         else
         {
            throw UtInput::BadValue(aInput);
         }
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// ================================================================================================
//! This method is called by PurgeTracksEvent to perform the periodic track purging.
// virtual
void WsfTrackProcessor::PerformTrackHistoryPurging(double aSimTime)
{
   // Simply call the track manager's method.
   mTrackManagerPtr->PurgeLocalTrackHistory(aSimTime, mTrackHistoryRetentionInterval);
}

// ================================================================================================
//! This method is called by PurgeTracksEvent to perform the periodic track purging.
// virtual
void WsfTrackProcessor::PerformTrackPurging(double aSimTime)
{
   mTrackManagerPtr->PurgeInactiveRawTracks(aSimTime, mPurgeInterval, mPurgeInterval * mImagePurgeMultiplier);
   mTrackManagerPtr->PurgeInactiveTracks(aSimTime, mPurgeInterval);

   // Also clean up the dropped raw track list.
   DroppedRawTrackList::iterator rtli = mDroppedRawTrackList.begin();
   while (rtli != mDroppedRawTrackList.end())
   {
      if ((*rtli).second + mPurgeInterval < aSimTime)
      {
         mDroppedRawTrackList.erase(rtli++);
      }
      else
      {
         ++rtli;
      }
   }
}

// ================================================================================================
//! This method is called by ReportTracksEvent to perform the periodic track reporting.
//! It delegates track reporting responsibility to the reporting strategy.
// virtual
void WsfTrackProcessor::PerformTrackReporting(double aSimTime)
{
   if (mReportFusedTracks)
   {
      mReportingStrategyPtr->ReportFusedTracks(aSimTime);
   }
   if (mReportRawTracks)
   {
      mReportingStrategyPtr->ReportRawTracks(aSimTime);
   }
}

// ================================================================================================
// virtual public
//! Executes the 'is_track_reportable' script.
//! Allows analysts to check attributes of a track to determine if it is reported.
//! The track is considered reportable unless the script returns false.
//! @param aSimTime  The current simulation time.
//! @param aTrackPtr A pointer to a track.
//! @returns 'true' if the track is reportable, 'false' if not.
bool WsfTrackProcessor::IsTrackReportable(double aSimTime, const WsfTrack* aTrackPtr)
{
   bool isReportable = true;
   if (mIsTrackReportableScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;

      // There is one argument; the track
      UtScriptClass* trackClassPtr = mContextPtr->GetClass("WsfTrack");
      scriptArgs.push_back(UtScriptData(new UtScriptRef(const_cast<WsfTrack*>(aTrackPtr), trackClassPtr)));

      mContextPtr->ExecuteScript(aSimTime, mIsTrackReportableScriptPtr, scriptRetVal, scriptArgs);
      isReportable = scriptRetVal.GetBool();
   }

   return isReportable;
}

// ================================================================================================
// virtual protected
//! Send a message to other processors (a WsfTrackNotifyMessage) that a track in the track list has changed.
//! @param aSimTime         The current simulation time.
//! @param aChangedTrackPtr A pointer to the changed track.
//! @param aChangedReason   The reason code for the change.
void WsfTrackProcessor::NotifyPlatform(double aSimTime, const WsfTrack* aChangedTrackPtr, int aChangedReason)
{
   // Allow all internally linked observers to react to the change (but, make this call after master track list updates).
   WsfPlatformPart::SendMessage(
      aSimTime,
      WsfTrackNotifyMessage(GetPlatform(), aChangedTrackPtr->GetTrackId(), GetNameId(), aChangedReason));
}

// ================================================================================================
// virtual
bool WsfTrackProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool messageProcessed = true;
   // If this is a track message, then inform the track manager of the new track report.
   WsfStringId messageType = aMessage.GetType();
   if (messageType == WsfTrackMessage::GetTypeId())
   {
      const WsfTrackMessage& message        = static_cast<const WsfTrackMessage&>(aMessage);
      WsfTrack*              trackPtr       = message.GetTrack();
      bool                   isRemoteSender = (message.GetOriginator() != GetPlatform()->GetNameId());
      bool isLocalOrigin = (trackPtr->GetTrackId().GetOwningPlatformId() == GetPlatform()->GetNameId());

      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Track processor received track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
         out.AddNote() << "Sender: " << (isRemoteSender ? "remote" : "local");
         out.AddNote() << "Track ID: " << trackPtr->GetTrackId();
         out.AddNote() << "Target: " << trackPtr->GetTargetName();
      }

      if (isRemoteSender && isLocalOrigin)
      {
         // Do not accept tracks of local origin from a remote sender.
         // (i.e.: they have simply reflected a track that I originated).
      }
      else if (mRejectCircularReports && (message.GetReplyId() == GetPlatform()->GetNameId()))
      {
         // Do not accept tracks that were the recent product of a report from me.
      }
      else if ((mPurgeInterval > 0.0) && (trackPtr->GetUpdateTime() < (aSimTime - mPurgeInterval)))
      {
         // Data for this track are older than the purge interval.
         // We elect not to consider these data as they have probably
         // been deleted already as part of the track purging process.
      }
      else if (mDroppedRawTrackList.find(trackPtr->GetTrackId()) != mDroppedRawTrackList.end())
      {
         // We've already received a track drop for this track. Communication delays may have
         // caused the track report to come after the drop, hence we must ignore the report.
      }
      else if (ShouldProcessInboundReport(aSimTime, *trackPtr))
      {
         // Allow the track manager to fuse the raw track into a local track.

         mSourceId = message.GetSenderId();
         mTrackManagerPtr->AddTrackReport(aSimTime, *trackPtr);
         mSourceId = nullptr;
      }
   }
   else if (messageType == WsfTrackDropMessage::GetTypeId())
   {
      const WsfTrackDropMessage& message = static_cast<const WsfTrackDropMessage&>(aMessage);

      if (DebugEnabled())
      {
         WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(message.GetTargetIndex());
         auto         out       = ut::log::debug() << "Track processor received track drop.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
         if (message.GetOriginator() == GetPlatform()->GetNameId())
         {
            out.AddNote() << "Sender Location: remote";
         }
         else
         {
            out.AddNote() << "Sender Location: local";
         }
         out.AddNote() << "Track ID: " << message.GetTrackId();
         out.AddNote() << "Target: " << (targetPtr ? targetPtr->GetName() : "<unknown>");
      }

      // Inform the track manager that we wish to drop this track.
      mTrackManagerPtr->DropTrack(aSimTime, message.GetTrackId());

      // Maintain a short-term memory of what tracks have been dropped so that if we receive a track
      // update a short time later that we don't re-add it.
      mDroppedRawTrackList[message.GetTrackId()] = aSimTime;
   }
   else if (messageType == WsfTrackNotifyMessage::GetTypeId())
   {
      const WsfTrackNotifyMessage& message = static_cast<const WsfTrackNotifyMessage&>(aMessage);

      int reason = message.GetChangedReason();
      if ((reason == WsfTrackManager::cCREATED) || (reason == WsfTrackManager::cUPDATED) ||
          (reason == WsfTrackManager::cCANDIDATE_TRACK_PROMOTED))
      {
         WsfProcessor* processorPtr = GetPlatform()->GetComponent<WsfProcessor>(message.GetTrackProcessorNameId());
         if (processorPtr != nullptr)
         {
            WsfTrackProcessor* trackProcessorPtr(dynamic_cast<WsfTrackProcessor*>(processorPtr));
            if (trackProcessorPtr != nullptr)
            {
               WsfTrack* trackPtr = trackProcessorPtr->mTrackManagerPtr->FindTrack(message.GetTrackId());
               if (trackPtr != nullptr)
               {
                  mTrackManagerPtr->AddTrackReport(aSimTime, *trackPtr);
               }
            }
         }
      }
      else if (reason == WsfTrackManager::cDROPPED)
      {
         mTrackManagerPtr->DropTrack(aSimTime, message.GetTrackId());
      }
   }
   else
   {
      messageProcessed = false;
   }

   GetMessageHandler()->ProcessMessage(aSimTime, aMessage);

   return messageProcessed;
}

// ================================================================================================
//! Send a single track to externally connected entities.
//! Only reportable tracks are sent.
// virtual
void WsfTrackProcessor::SendTrack(double aSimTime, const WsfTrack& aTrack)
{
   // If the policy to report candidate tracks is set, report them; otherwise, don't.
   if (mReportCandidateTracks || (!aTrack.IsCandidate()))
   {
      SendTrackP(aSimTime, aTrack);
   }
}

// ================================================================================================
//! Send a single track to externally connected entities.
//! If the option is selected, do not report candidate tracks.
// virtual
void WsfTrackProcessor::SendTrack(double aSimTime, const WsfLocalTrack& aLocalTrack)
{
   // If the policy to report candidate tracks is set, report them; otherwise, don't.
   if (mReportCandidateTracks || (!aLocalTrack.IsCandidate()))
   {
      SendTrackP(aSimTime, aLocalTrack);
   }
}

// ================================================================================================
//! Immediately send a track over external links.
// protected virtual
void WsfTrackProcessor::SendTrackP(double aSimTime, const WsfTrack& aTrack)
{
   if (aTrack.IsReportable() &&                // Checks the track attribute
       IsTrackReportable(aSimTime, &aTrack) && // Checks for scripted condition
       (!aTrack.IsStale()))                    // Checks for non-stale data
   {
      // Check for sending unchanged tracks.
      double timeSinceLastUpdate = aSimTime - aTrack.GetUpdateTime();
      if (mReportUnchangedTracks || (timeSinceLastUpdate <= mReportInterval))
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Track processor reporting track.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Processor: " << GetName();
            out.AddNote() << "Track ID: " << aTrack.GetTrackId();
            out.AddNote() << "Truth Target: " << aTrack.GetTargetName();
            out.AddNote() << "Time Since Update: " << timeSinceLastUpdate;
            out.AddNote() << "Report Interval: " << mReportInterval;
         }
         WsfTrackMessage message(GetPlatform(), aTrack);
         if (mUpdateOnReport)
         {
            message.GetTrack()->KinematicUpdate(aSimTime);
         }
         message.SetSenderId(GetPlatform()->GetNameId());
         message.SetReplyId(aTrack.GetLastSourceName());
         ExecuteOnMessageCreate(aSimTime, message);
         GetExternalLinks().SendMessage(aSimTime, message, aTrack.GetLastSourceName());
      }
      else if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Track not reported due to unchanged data since last update.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Track: " << aTrack.GetTrackId();
         out.AddNote() << "Target: " << aTrack.GetTargetName();
         out.AddNote() << "Time Since Update: " << timeSinceLastUpdate;
         out.AddNote() << "Report Interval: " << mReportInterval;
      }
   }
}

// ================================================================================================
//! Should the supplied inbound report be processed?
//! This method is called when an incoming track report is received. It determines if the report should be forwarded
//! to the track manager for assimilation or 'false' if it should be ignored.
//! @returns 'true' if the report should processed or 'false' if it should be ignored.
// protected virtual
bool WsfTrackProcessor::ShouldProcessInboundReport(double aSimTime, const WsfTrack& aTrack)
{
   bool processReport = true;

   // This routine is pretty much a placeholder for now. Inbound filtering is not yet complex.

   WsfTrack::TrackType trackType = aTrack.GetTrackType();
   if ((trackType != WsfTrack::cUNFILTERED_SENSOR) && (trackType != WsfTrack::cFILTERED_SENSOR))
   {
      if (mRejectNonSensorReports)
      {
         processReport = false;
      }
   }

   if ((!processReport) && DebugEnabled())
   {
      ut::log::debug() << "Report rejected by inbound filter.";
   }
   return processReport;
}

// ================================================================================================
// Track manager observer callbacks
// ================================================================================================

// ================================================================================================
// private
void WsfTrackProcessor::CandidateTrackPromoted(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   WriteTrackDebugMessage(aSimTime, aLocalTrackPtr, "candidate track promoted");

   aLocalTrackPtr->SetLastSourceName(mSourceId); // For circular track reporting...

   if (mReportCandidateTracks)
   {
      // NOTE: If reporting candidate tracks only the notification message is sent.
      //       The actual track update will get reported by LocalTrackUpdated.
      SendTrackNotifyMessage(aSimTime, aLocalTrackPtr, WsfTrackManager::cCANDIDATE_TRACK_PROMOTED);
   }
   else if (!aLocalTrackPtr->IsCandidate())
   {
      // Candidate track reporting is NOT enabled and the track has been promoted from a 'candidate'
      // track to a full local track.
      //
      // Since candidate track reporting is NOT enabled then treat this like 'LocalTrackInitiated' as
      // others never seen the track before (i.e.: treat is as CREATED).

      SendTrackNotifyMessage(aSimTime, aLocalTrackPtr, WsfTrackManager::cCREATED);

      // Also send a track update message just like LocalTrackInitiated. LocalTrackUpdated may or may
      // not be called, so we have to make sure it is done...
      if (mReportFusedTracks)
      {
         SendTrackUpdateMessage(aSimTime, aLocalTrackPtr);
      }
   }
}

// ================================================================================================
// private
void WsfTrackProcessor::CurrentTargetChanged(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   WriteTrackDebugMessage(aSimTime, aLocalTrackPtr, "current target changed");

   if (mReportCandidateTracks || (!aLocalTrackPtr->IsCandidate()))
   {
      SendTrackNotifyMessage(aSimTime, aLocalTrackPtr, WsfTrackManager::cCURRENT_TARGET_CHANGED);
   }
}

// ================================================================================================
// private
void WsfTrackProcessor::LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   WriteTrackDebugMessage(aSimTime, aLocalTrackPtr, "local track dropped");

   if (mReportCandidateTracks || (!aLocalTrackPtr->IsCandidate()))
   {
      SendTrackNotifyMessage(aSimTime, aLocalTrackPtr, WsfTrackManager::cDROPPED);
      SendTrackDropMessage(aSimTime, aLocalTrackPtr);
   }
}

// ================================================================================================
// private
void WsfTrackProcessor::LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
{
   WriteTrackDebugMessage(aSimTime, aLocalTrackPtr, "local track initiated");

   aLocalTrackPtr->SetLastSourceName(mSourceId); // For circular track reporting...

   if (mReportCandidateTracks || (!aLocalTrackPtr->IsCandidate()))
   {
      SendTrackNotifyMessage(aSimTime, aLocalTrackPtr, WsfTrackManager::cCREATED);
      // A created track is reported immediately instead of waiting for the first periodic review.
      // This prevent the delay in seeing new information (Should this be an option?)
      if (mReportFusedTracks)
      {
         SendTrackUpdateMessage(aSimTime, aLocalTrackPtr);
      }
   }
}

// ================================================================================================
// private
void WsfTrackProcessor::LocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
{
   WriteTrackDebugMessage(aSimTime, aLocalTrackPtr, "local track updated");

   aLocalTrackPtr->SetLastSourceName(mSourceId); // For circular track reporting...

   if (mReportCandidateTracks || (!aLocalTrackPtr->IsCandidate()))
   {
      SendTrackNotifyMessage(aSimTime, aLocalTrackPtr, WsfTrackManager::cUPDATED);
      if (mReportFusedTracks && mReportOnUpdate)
      {
         SendTrackUpdateMessage(aSimTime, aLocalTrackPtr);
      }
   }
}

// ================================================================================================
// private
void WsfTrackProcessor::RawTrackDropped(double aSimTime, const WsfTrack* aRawTrackPtr)
{
   WriteTrackDebugMessage(aSimTime, aRawTrackPtr, "raw track dropped");

   // NOTE: Unlike local track changes, raw track changes are not reported via WsfTrackNotifyMessages.

   if (mReportCandidateTracks || (!aRawTrackPtr->IsCandidate()))
   {
      // Raw track drops when raw tracks are reported.
      if (mReportRawTracks)
      {
         SendTrackDropMessage(aSimTime, aRawTrackPtr);
      }
   }
}

// ================================================================================================
// private
void WsfTrackProcessor::RawTrackReceived(double aSimTime, const WsfTrack* aRawTrackPtr)
{
   WriteTrackDebugMessage(aSimTime, aRawTrackPtr, "raw track received");

   aRawTrackPtr->SetLastSourceName(mSourceId); // For circular track reporting...

   // NOTE: Unlike local track changes, raw track changes are not reported via WsfTrackNotifyMessages.

   if (mReportPassThrough && (aRawTrackPtr->GetSensorTypeId() != 0))
   {
      // Directly pass raw track reports to external subscribers if this is a "direct" report
      // (i.e., from a sensor, not a processor).
      SendTrackUpdateMessage(aSimTime, aRawTrackPtr);
   }
   else if (mReportCandidateTracks || (!aRawTrackPtr->IsCandidate()))
   {
      if (mReportRawTracks && mReportOnUpdate)
      {
         SendTrackUpdateMessage(aSimTime, aRawTrackPtr);
      }
   }
}

// ================================================================================================
void WsfTrackProcessor::TrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr)
{
   if (aNewManagerPtr)
   {
      mTrackManagerPtr = aNewManagerPtr;
   }
}

// ================================================================================================
// Helper methods for the track manager observer callbacks
// ================================================================================================

// ================================================================================================
// private
void WsfTrackProcessor::SendTrackDropMessage(double aSimTime, const WsfTrack* aTrackPtr)
{
   const WsfTrackId& trackId(aTrackPtr->GetTrackId());
   mLastReportTime.erase(trackId);

   // Only send track drop messages for locally produced tracks; otherwise, rely on periodic
   // review by other platforms to determine raw track drops.
   if (aTrackPtr->IsReportable() && (trackId.GetOwningPlatformId() == GetPlatform()->GetNameId()))
   {
      WsfTrackDropMessage message(GetPlatform(),
                                  aTrackPtr->GetUpdateTime(),
                                  aTrackPtr->GetTrackId(),
                                  aTrackPtr->GetTargetIndex());
      ExecuteOnMessageCreate(aSimTime, message);
      mExternalLinks.SendMessage(aSimTime, message);
   }
}

// ================================================================================================
//! Send a message to other processors (a WsfTrackNotifyMessage) that a track in the track list has changed.
//! @param aSimTime  The current simulation time.
//! @param aTrackPtr A pointer to the changed track.
//! @param aReason   The reason code for the change (from WsfTrackManager enumeration)
// private
void WsfTrackProcessor::SendTrackNotifyMessage(double aSimTime, const WsfTrack* aTrackPtr, int aReason)
{
   if (HasInternalLinks())
   {
      SendMessage(aSimTime, WsfTrackNotifyMessage(GetPlatform(), aTrackPtr->GetTrackId(), GetNameId(), aReason));
   }
}

// ================================================================================================
//! Send a track update message if it is time to do so.
// private
void WsfTrackProcessor::SendTrackUpdateMessage(double aSimTime, const WsfTrack* aTrackPtr)
{
   WsfTrackId                  trackId(aTrackPtr->GetTrackId());
   double                      lastReportTime = -9999.0;
   LastReportTimeMap::iterator i              = mLastReportTime.find(trackId);
   if (i != mLastReportTime.end())
   {
      lastReportTime = i->second;
   }
   if ((aTrackPtr->GetUpdateTime() - lastReportTime) >= mReportInterval)
   {
      SendTrackP(aSimTime, *aTrackPtr);
      if (mReportOnUpdate)
      {
         mLastReportTime[trackId] = aTrackPtr->GetUpdateTime();
      }
   }
}

// ================================================================================================
// private
void WsfTrackProcessor::WriteTrackDebugMessage(double aSimTime, const WsfTrack* aTrackPtr, const std::string& aReason)
{
   if (DebugEnabled())
   {
      std::string reason = aReason;
      if (!reason.empty())
      {
         reason[0] = std::toupper(reason[0]);
      }

      auto out = ut::log::debug() << reason << ".";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "Track ID: " << aTrackPtr->GetTrackId();
      out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
   }
}

// ================================================================================================
// private
void WsfTrackProcessor::SelectMasterTrackProcessor(UtInput& aInput)
{
   if (mTrackManagerTypeSelected)
   {
      if (!mMasterTrackProcessor)
      {
         throw UtInput::OutOfContext(aInput, "WSF_TRACK_PROCESSOR: Can't use a master_track_processor,\
                                             as you are already using a non_master_track_processor.");
      }
   }
   else
   {
      mMasterTrackProcessor     = true;
      mTrackManagerTypeSelected = true;
   }
}

// ================================================================================================
// private
void WsfTrackProcessor::SelectNonMasterTrackProcessor(UtInput& aInput)
{
   if (mTrackManagerTypeSelected)
   {
      if (mMasterTrackProcessor)
      {
         throw UtInput::OutOfContext(aInput, "WSF_TRACK_PROCESSOR: Can't use a non_master_track_processor,\
                                             as you are already using a master_track_processor.");
      }
   }
   else
   {
      mTrackManagerPtr          = new WsfTrackManager(GetScenario());
      mMasterTrackProcessor     = false;
      mTrackManagerTypeSelected = true;
   }
}

// ================================================================================================
// protected
void WsfTrackProcessor::SelectNonMasterTrackProcessor()
{
   mTrackManagerPtr          = new WsfTrackManager(GetScenario());
   mMasterTrackProcessor     = false;
   mTrackManagerTypeSelected = true;
}

// ================================================================================================
void WsfTrackProcessor::ConnectCallbacks()
{
   if (mTrackManagerPtr != nullptr)
   {
      // Subscribe to track manager events of interest
      mCallbacks.Add(mTrackManagerPtr->CandidateTrackPromoted.Connect(&WsfTrackProcessor::CandidateTrackPromoted, this));
      mCallbacks.Add(mTrackManagerPtr->LocalTrackDropped.Connect(&WsfTrackProcessor::LocalTrackDropped, this));
      mCallbacks.Add(mTrackManagerPtr->LocalTrackInitiated.Connect(&WsfTrackProcessor::LocalTrackInitiated, this));
      mCallbacks.Add(mTrackManagerPtr->LocalTrackUpdated.Connect(&WsfTrackProcessor::LocalTrackUpdated, this));
      mCallbacks.Add(mTrackManagerPtr->RawTrackDropped.Connect(&WsfTrackProcessor::RawTrackDropped, this));
      mCallbacks.Add(mTrackManagerPtr->RawTrackReceived.Connect(&WsfTrackProcessor::RawTrackReceived, this));
      mCallbacks.Add(mTrackManagerPtr->TrackManagerChanged.Connect(&WsfTrackProcessor::TrackManagerChanged, this));
   }
}

// ================================================================================================
WsfEvent::EventDisposition WsfTrackProcessor_PurgeTracksEvent::Execute()
{
   EventDisposition disposition = cDELETE;
   if (GetSimulation()->GetPlatformByIndex(mPlatformIndex) != nullptr)
   {
      mProcessorPtr->PerformTrackPurging(GetTime());
      SetTime(GetTime() + mPurgeInterval);
      disposition = cRESCHEDULE;
   }
   return disposition;
}

// ================================================================================================
WsfTrackProcessor_PurgeTracksEvent::WsfTrackProcessor_PurgeTracksEvent(double             aSimTime,
                                                                       WsfTrackProcessor* aProcessorPtr,
                                                                       double             aPurgeInterval)
   : WsfEvent(aSimTime)
   , mPurgeInterval(aPurgeInterval)
   , mProcessorPtr(aProcessorPtr)
{
   mPlatformIndex = mProcessorPtr->GetPlatform()->GetIndex();

   // Wait one cycle before purging.
   SetTime(aSimTime + aPurgeInterval);
}

// ================================================================================================
WsfEvent::EventDisposition WsfTrackProcessor_PurgeTrackHistoryEvent::Execute()
{
   EventDisposition disposition = cDELETE;
   if (GetSimulation()->GetPlatformByIndex(mPlatformIndex) != nullptr)
   {
      mProcessorPtr->PerformTrackHistoryPurging(GetTime());
      SetTime(GetTime() + mPurgeInterval);
      disposition = cRESCHEDULE;
   }
   return disposition;
}

// ================================================================================================
WsfTrackProcessor_PurgeTrackHistoryEvent::WsfTrackProcessor_PurgeTrackHistoryEvent(double             aSimTime,
                                                                                   WsfTrackProcessor* aProcessorPtr,
                                                                                   double             aPurgeInterval)
   : WsfTrackProcessor_PurgeTracksEvent(aSimTime, aProcessorPtr, aPurgeInterval)
{
}

// ================================================================================================
WsfEvent::EventDisposition WsfTrackProcessor_ReportTracksEvent::Execute()
{
   EventDisposition disposition = cDELETE;
   if (GetSimulation()->GetPlatformByIndex(mPlatformIndex) != nullptr)
   {
      mProcessorPtr->PerformTrackReporting(GetTime());
      SetTime(GetTime() + mReportInterval);
      disposition = cRESCHEDULE;
   }
   return disposition;
}

// ================================================================================================
WsfTrackProcessor_ReportTracksEvent::WsfTrackProcessor_ReportTracksEvent(double             aSimTime,
                                                                         WsfTrackProcessor* aProcessorPtr,
                                                                         double             aReportInterval)
   : WsfEvent(aSimTime)
   , mReportInterval(aReportInterval)
   , mProcessorPtr(aProcessorPtr)
{
   mPlatformIndex = mProcessorPtr->GetPlatform()->GetIndex();
}
