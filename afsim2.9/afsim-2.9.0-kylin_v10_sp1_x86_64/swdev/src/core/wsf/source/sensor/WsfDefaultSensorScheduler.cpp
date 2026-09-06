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

#include "WsfDefaultSensorScheduler.hpp"

#include <algorithm>
#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "WsfEM_Util.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorTracker.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackList.hpp"

// The choice of what container to use for the sensing chance list and the request list was
// arrived at through the following process...
//
// The chance list was originally a list<>.  The first entry was simply popped off, processed and
// moved to the back (using front/pop_front/push_back).  This is probably the most inefficient way
// to do it, but it was very clean, and in the grand scheme of things it didn't really matter much.
// A small test program was written that used list::splice to move the entry, and that is twice
// as fast list::front/pop_front/push_back.  Using a deque<> with its front/pop_front/push_back
// sequence and it was even better - almost 10 times faster than the original method. The potential
// downside of deque<> is the way it allocates memory.
//
// Finally, I just decided on a vector<> with a rotating index. This is a little harder, but it is
// by far the fastest (about 60 times faster than the original list) and doesn't cause a constant
// reallocation of memory.
//
// As was indicated, the choice really didn't seem to affect performance because so much time is
// spent doing other things, but the choice of vectors seemed obvious because it eliminated any
// possibility of problems.
//
// A vector was also chosen for the request list.  The request list must be accessible in a
// time-ordered manner and a priority_queue would be an obvious choice, but we also have to
// be able to remove entries from the middle of the list.  Since this list is often very short,
// searching through a vector to find the next request is probably the easiest thing.

// =================================================================================================
WsfDefaultSensorScheduler::WsfDefaultSensorScheduler()
   : WsfSensorScheduler()
   , mModeList()
   , mDwellEndTime(0.0)
   , mNextSearchVisitTime(0.0)
   , mNextTrackVisitTime(0.0)
   , mSearchChanceInterval(0.0)
   , mSearchFrameTime(0.0)
   , mSearchIndex(0)
   , mRequestList()
   , mRequestIndex(0)
   , mLastExplicitModeIndex(0)
   , mSearchModeIndex(0)
   , mSearchAllowed(true)
   , mCheckSearchList(false)
   , mScanSchedulingMethod(cSSM_RANDOM)
{
}

// =================================================================================================
WsfDefaultSensorScheduler::WsfDefaultSensorScheduler(const WsfDefaultSensorScheduler& aSrc)
   : WsfSensorScheduler(aSrc)
   , mModeList()
   , mDwellEndTime(0.0)
   , mNextSearchVisitTime(0.0)
   , mNextTrackVisitTime(0.0)
   , mSearchChanceInterval(0.0)
   , mSearchFrameTime(0.0)
   , mSearchIndex(0)
   , mRequestList()
   , mRequestIndex(0)
   , mLastExplicitModeIndex(0)
   , mSearchModeIndex(0)
   , mSearchAllowed(true)
   , mCheckSearchList(aSrc.mCheckSearchList)
   , mScanSchedulingMethod(aSrc.mScanSchedulingMethod)
{
}

//! Factory method for WsfSensorSchedulerTypes to determine if a scheduler
//! represented by this class is being requested.
// static
//  =================================================================================================
std::unique_ptr<WsfSensorScheduler> WsfDefaultSensorScheduler::ObjectFactory(const std::string& aTypeName)
{
   if (aTypeName == "default")
   {
      return ut::make_unique<WsfDefaultSensorScheduler>();
   }
   return nullptr;
}

// =================================================================================================
// virtual
WsfSensorScheduler* WsfDefaultSensorScheduler::Clone() const
{
   return new WsfDefaultSensorScheduler(*this);
}

// =================================================================================================
// virtual
void WsfDefaultSensorScheduler::FrameTimeChanged(double aSimTime, WsfStringId aModeNameId)
{
   UpdateSearchFrameTime(aSimTime);
}

// =================================================================================================
// virtual
int WsfDefaultSensorScheduler::GetActiveRequestCount(WsfStringId aModeNameId) const
{
   size_t modeIndex = mSensorPtr->GetModeList()->GetCurrentMode();
   if (aModeNameId != 0)
   {
      modeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
   }
   int count = 0;
   if (modeIndex < mModeList.size())
   {
      for (const auto& i : mRequestList)
      {
         if (i.mModeIndex == modeIndex)
         {
            ++count;
         }
      }
   }
   return count;
}

// =================================================================================================
// virtual
int WsfDefaultSensorScheduler::GetMaximumRequestCount(WsfStringId aModeNameId) const
{
   size_t modeIndex = mSensorPtr->GetModeList()->GetCurrentMode();
   if (!aModeNameId.IsNull())
   {
      modeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
   }
   int count = 0;
   if (modeIndex < mModeList.size())
   {
      count = mModeList[modeIndex]->GetMaximumRequestCount();
   }
   return count;
}

// =================================================================================================
// virtual
bool WsfDefaultSensorScheduler::HaveRequestFor(const WsfTrackId& aRequestId) const
{
   return (std::find_if(mRequestList.begin(), mRequestList.end(), CompareRequestId(aRequestId)) != mRequestList.end());
}

// =================================================================================================
// virtual
bool WsfDefaultSensorScheduler::Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorTracker* aTrackerPtr)
{
   if (aSensorPtr->GetModeList() == nullptr)
   {
      ut::log::error() << "A mode list is required for WsfDefaultSensorScheduler.";
      assert(aSensorPtr->GetModeList() != nullptr);
      return false;
   }

   bool ok = WsfSensorScheduler::Initialize(aSimTime, aSensorPtr, aTrackerPtr);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   aSensorPtr->GetModeList()->GetDerivedModeList(mModeList);
   mLastExplicitModeIndex = static_cast<unsigned int>(mModeList.size());

   // Ensure that each mode has a frame time.
   for (WsfSensorMode* modePtr : mModeList)
   {
      if (modePtr->GetFrameTime() <= 0.0)
      {
         auto out = ut::log::error() << "WsfDefaultSensorScheduler: 'frame_time' not specified.";
         out.AddNote() << "Sensor: " << mSensorPtr->GetName();
         out.AddNote() << "Mode: " << modePtr->GetName();
         ok = false;
      }
   }

   return ok;
}

// =================================================================================================
// virtual
void WsfDefaultSensorScheduler::ModeDeselected(double aSimTime, WsfStringId aModeNameId)
{
   size_t modeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
   if (mLastExplicitModeIndex == modeIndex)
   {
      mLastExplicitModeIndex = static_cast<unsigned int>(mModeList.size());
   }

   // Check for a possible change in the search mode (either switching or becoming available).
   bool oldSearchAllowed = mSearchAllowed;
   CheckSearchModeAvailability();

   // Force a check of the search list if transitioning from search-allowed to search-not-allowed.
   if (oldSearchAllowed && (!mSearchAllowed))
   {
      mCheckSearchList = true;
   }
}

// =================================================================================================
// virtual
void WsfDefaultSensorScheduler::ModeSelected(double aSimTime, WsfStringId aModeNameId)
{
   // Before the multi-threading changes this was called only by explicit user calls to
   // WsfSensor::SelectMode (and not by the internal calls that originated from this class),
   // and those types of modes were probably only for search modes.
   //
   // mLastExplicitModeIndex was supposed to represent the last *explicitly* selected mode, which
   // should be a search-type mode. If the mode is a search mode then we will update the variable.

   size_t modeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
   if (modeIndex < mModeList.size() && mModeList[modeIndex]->CanSearchWhileTrack())
   {
      mLastExplicitModeIndex = modeIndex;
   }

   // Check for a possible change in the search mode (either switching or becoming available).
   bool oldSearchAllowed = mSearchAllowed;
   CheckSearchModeAvailability();

   // Update the search mode frame time and sensing chance interval (if necessary).
   UpdateSearchFrameTime(aSimTime);

   // Force a check of the search list if transitioning from search-allowed to search-not-allowed.
   if (oldSearchAllowed && (!mSearchAllowed))
   {
      mCheckSearchList = true;
   }
}

// =================================================================================================
// virtual
void WsfDefaultSensorScheduler::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool addToFront = true;
   switch (mScanSchedulingMethod)
   {
   case cSSM_RANDOM:
      addToFront = (aPlatformPtr->GetSimulation()->GetRandom().Uniform<double>() > 0.5);
      break;
   case cSSM_INPUT_ORDER:
      addToFront = true;
      break;
   case cSSM_REVERSE_INPUT_ORDER:
      addToFront = false;
      break;
   }

   // Put the platform on either the front or the back of the queue.

   if (addToFront)
   {
      mSearchList.push_back(aPlatformPtr->GetIndex());
   }
   else
   {
      mSearchList.insert(mSearchList.begin(), aPlatformPtr->GetIndex());

      // Because the list has been shifted we must also shift the index.  If it was pointed one-past
      // -the-end before then it will still be one-past-the-end.  This is OK because we check the
      // index in SelectTarget.
      ++mSearchIndex;
   }
   UpdateSearchChanceInterval();
}

// =================================================================================================
// virtual
void WsfDefaultSensorScheduler::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   // Tell 'SelectTarget' to check the search list for obsolete items if search is disabled.
   if ((!mSearchAllowed) &&
       (std::find(mSearchList.begin(), mSearchList.end(), aPlatformPtr->GetIndex()) != mSearchList.end()))
   {
      mCheckSearchList = true;
   }
}

// =================================================================================================
// virtual
bool WsfDefaultSensorScheduler::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "scan_scheduling")
   {
      aInput.ReadValue(command);
      if (command == "random")
      {
         mScanSchedulingMethod = cSSM_RANDOM;
      }
      else if (command == "input_order")
      {
         mScanSchedulingMethod = cSSM_INPUT_ORDER;
      }
      else if (command == "reverse_input_order")
      {
         mScanSchedulingMethod = cSSM_REVERSE_INPUT_ORDER;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Bad value for scan_scheduling: " + command);
      }
   }
   else
   {
      myCommand = WsfSensorScheduler::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
void WsfDefaultSensorScheduler::RemoveTarget(double aSimTime, size_t aTargetIndex)
{
   auto sli = std::find(mSearchList.begin(), mSearchList.end(), aTargetIndex);
   if (sli != mSearchList.end())
   {
      // The next part isn't strictly necessary, but it prevents us from missing a sensing
      // chance because the chance list has been shifted.
      //
      // We don't have to worry about the case where we delete the last entry and it is the
      // also the next sensing chance.  This is checked for in SelectTarget.

      // Should have used 'distance(mSearchList.begin(), sli)' here, but it didn't work on the Sun.
      std::vector<unsigned int>::size_type removedIndex = sli - mSearchList.begin();
      if (removedIndex < mSearchIndex)
      {
         --mSearchIndex;
      }

      mSearchList.erase(sli);
      UpdateSearchChanceInterval();
   }
}

// =================================================================================================
// virtual
const WsfTrack& WsfDefaultSensorScheduler::GetTrackForRequest(const WsfTrackId& aRequestId) const
{
   auto rli = std::find_if(mRequestList.begin(), mRequestList.end(), CompareRequestId(aRequestId));
   if (rli != mRequestList.end())
   {
      return (*rli).mTrack;
   }
   static WsfTrack track;
   return track;
}

// =================================================================================================
// virtual
bool WsfDefaultSensorScheduler::SelectTarget(double               aSimTime,
                                             double&              aNextSimTime,
                                             size_t&              aTargetIndex,
                                             WsfTrackId&          aRequestId,
                                             WsfSensor::Settings& aSettings)
{
   static const double cEPSILON = 1.0E-7;

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Sensor selecting target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
      out.AddNote() << "Next Sim Time: " << aNextSimTime;
      out.AddNote() << "Next Search Visit Time: " << mNextSearchVisitTime;
      out.AddNote() << "Next Track Visit Time: " << mNextTrackVisitTime;
   }
   WsfSimulation& sim = *mSensorPtr->GetSimulation();
   // If searching has been disabled some things are checked in the search list (upon request):
   //
   // 1) Targets that were being detected/tracked by the search mode and not the subject of a request need to
   //    have their tracks dropped.
   // 2) Targets that have been deleted and not the subject of a request need to be removed from the search list.
   //
   // NOTE: This check is performed only when requested AND when search is disabled. The request is made
   //       when mode change occurs or when a platform is deleted from the simulation.

   if (mCheckSearchList && (!mSearchAllowed))
   {
      WsfSensorTracker::Settings stSettings;
      bool                       platformsDeleted = false;
      for (size_t si = 0; si < mSearchList.size(); ++si)
      {
         size_t ti = mSearchList[si];
         if (!TargetHasActiveRequest(ti))
         {
            // Loop until any detection data is cleaned up and outstanding track is dropped.
            while (!mTrackerPtr->TargetSkipped(aSimTime, stSettings, WsfTrackId(), ti))
            {
            }

            // If the target platform no longer exists, mark it for removal from the search list.
            if (sim.GetPlatformByIndex(ti) == nullptr)
            {
               mSearchList[si]  = 0;
               platformsDeleted = true;
            }
         }
      }

      // Remove platforms from the search chance list that have been marked for removal.
      if (platformsDeleted)
      {
         mSearchList.erase(std::remove(mSearchList.begin(), mSearchList.end(), 0U), mSearchList.end());
      }
      // Start at the head of the list when a search mode is subsequently selected.
      mSearchIndex = 0;
   }
   mCheckSearchList = false;

   aRequestId.Null();
   size_t targetIndex = 0;
   if (mNextTrackVisitTime <= (aSimTime + cEPSILON))
   {
      // Select the next chance from the request queue.
      Request& request = mRequestList[mRequestIndex];
      aRequestId       = request.mRequestId;
      targetIndex      = SelectTargetForRequest(request);

      // Use the requested mode for the detection chance.
      aSettings.mModeIndex   = request.mModeIndex;
      WsfSensorMode* modePtr = mModeList[request.mModeIndex];

      // Get the position estimate for the cue.
      // If there is a tracker then attempt to get it from the there.
      // If there isn't a tracker or it doesn't have an estimate, then get it from the track supplied with the request.

      double targetLocWCS[3];
      bool   haveTargetState = false;
      bool   adjustTargetCue = false;
      if (mTrackerPtr != nullptr)
      {
         WsfSensorTracker::TargetState targetState;
         if (mTrackerPtr->GetTargetState(aSimTime, aRequestId, targetState))
         {
            haveTargetState = true;
            UtVec3d::Set(targetLocWCS, targetState.mLocationWCS.GetData());
            aSettings.mLockonTime = targetState.mLockonTime;
            adjustTargetCue       = true; // The target cue is only adjusted from internal cues
         }
      }

      if (!haveTargetState)
      {
         haveTargetState = request.mTrack.GetExtrapolatedLocationWCS(aSimTime, targetLocWCS);
      }

      if (haveTargetState)
      {
         // This is really, really ugly, but absolutely necessary for long-range tracking sensors.
         //
         // The cue location reported by the tracker is the 'geometric location' of the perceived target rather than the
         // 'apparent location' that accounts for atmospheric refraction. The difference typically isn't very much, but if
         // the beam is very narrow, the ranges are long and the geometric location is used, the losses from not being in
         // the center of the beam may be significant (a couple of dB). Thus, we must recover the 'apparent location'.
         //
         // Of course things don't work this way in real life (which uses ranges, angles, rates, etc.) But we really can't
         // do that here because our detection rates are probably not as high as a real system, so running tracking filters
         // and such is probably somewhat meaningless.
         //
         // At any rate, this takes care of the problem...
         //
         // For the current time, cuing for non-tracking sensors do not have this processing applied. They typically have wide
         // enough beams or are scanners (which will be adjusted to point at the target), so it is not as issue.

         if (adjustTargetCue)
         {
            if (mSensorPtr->GetEM_XmtrCount() > 0)
            {
               double earthRadiusScale = mSensorPtr->GetEM_Xmtr(0).GetEarthRadiusMultiplier();
               if (earthRadiusScale != 1.0)
               {
                  mSensorPtr->GetPlatform()->Update(aSimTime);
                  double sensorLocWCS[3];
                  mSensorPtr->GetLocationWCS(sensorLocWCS);
                  WsfEM_Util::ComputeApparentPosition(earthRadiusScale, sensorLocWCS, targetLocWCS, sensorLocWCS, targetLocWCS);
               }
            }
         }
         mSensorPtr->SetTransientCuedLocationWCS(targetLocWCS);
      }

      // Don't allow another detection chance until the dwell time has elapsed.

      mDwellEndTime        = request.mNextVisitTime + modePtr->GetDwellTime();
      mNextSearchVisitTime = std::max(mNextSearchVisitTime, mDwellEndTime);

      // Compute the next visit time for the request...

      request.mNextVisitTime += modePtr->GetRevisitTime();
      UpdateNextTrackVisitTime();
   }
   else if (mNextSearchVisitTime <= (aSimTime + cEPSILON))
   {
      // Select the next target for a search chance if search is allowed and there are items to search.
      if (mSearchAllowed && (!mSearchList.empty()))
      {
         aSettings.mModeIndex = mSearchModeIndex;
         if (mSearchIndex >= mSearchList.size())
         {
            mSearchIndex = 0;
         }
         targetIndex = mSearchList[mSearchIndex];
         ++mSearchIndex;

         // Bypass the search chance if there is an explicit request against the target
         if (TargetHasActiveRequest(targetIndex))
         {
            targetIndex = 0;
         }
      }
      mDwellEndTime = mNextSearchVisitTime; // Not really needed???
      mNextSearchVisitTime += mSearchChanceInterval;
   }

   aNextSimTime = std::min(mNextSearchVisitTime, mNextTrackVisitTime);
   aTargetIndex = targetIndex;
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Sensor has selected target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
      out.AddNote() << "Target: " << sim.GetPlatformNameId(aTargetIndex);
      out.AddNote() << "Mode: " << mModeList[aSettings.mModeIndex]->GetName();
      out.AddNote() << "Transient Cue: " << mSensorPtr->TransientCueActive();
      out.AddNote() << "Next Sim Time: " << aNextSimTime;
      out.AddNote() << "Next Search Visit Time: " << mNextSearchVisitTime;
      out.AddNote() << "Next Track Visit Time: " << mNextTrackVisitTime;
   }
   return ((aTargetIndex != 0) || mSensorPtr->TransientCueActive());
}

// =================================================================================================
// virtual
bool WsfDefaultSensorScheduler::StartTracking(double aSimTime, const WsfTrack& aTrack, WsfStringId aModeNameId)
{
   auto rli = std::find_if(mRequestList.begin(), mRequestList.end(), CompareRequestId(aTrack.GetTrackId()));

   // Validate the supplied mode name.

   size_t modeIndex = 0;
   if (!aModeNameId.IsNull())
   {
      modeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
      if (modeIndex >= mSensorPtr->GetModeList()->GetModeCount())
      {
         auto out = ut::log::error() << "Invalid mode requested for sensor.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << mSensorPtr->GetName();
         out.AddNote() << "Mode: " << aModeNameId;
         return false;
      }
   }
   else if (rli != mRequestList.end())
   {
      // A mode name was not specified. If a request already exists for this track then use the current mode.
      modeIndex = (*rli).mModeIndex;
   }

   WsfSensorMode* modePtr = mModeList[modeIndex];

   // Count the number of active requests for the desired mode.
   int activeRequestsInMode = 0;
   for (auto& i : mRequestList)
   {
      if (i.mModeIndex == modeIndex)
      {
         ++activeRequestsInMode;
      }
   }

   // Determine if the request is new or is an update of an existing request (updating track and mode).

   bool startedTracking = false;

   if (rli == mRequestList.end())
   {
      // Allow the request only if the maximum request count has not been exceeded.
      if (activeRequestsInMode < modePtr->GetMaximumRequestCount())
      {
         startedTracking = true;
         Request request(aSimTime, aTrack, modeIndex);

         // The first detection chance is set using a uniform random value within the frame time.
         request.mNextVisitTime =
            aSimTime + mSensorPtr->GetSimulation()->GetRandom().Uniform(0.0, modePtr->GetFrameTime());
         mRequestList.push_back(request);
         UpdateNextTrackVisitTime();

         SelectMode(aSimTime, request);
         WsfObserver::SensorRequestInitiated(mSensorPtr->GetSimulation())(aSimTime, mSensorPtr, modePtr, &aTrack);

         // This request is out-of-band, so we must change the update interval to force a sensing chance.
         double nextUpdateTime = std::min(mNextSearchVisitTime, mNextTrackVisitTime);
         mSensorPtr->SetNextUpdateTime(aSimTime, nextUpdateTime);
      }
   }
   else
   {
      // Updating an existing request.
      Request& request        = *rli;
      bool     requestUpdated = false;

      // The track is always updated even if the potential mode change request might fail. There doesn't
      // seem to be a good reason to at least not accept the updated track data.

      if (aTrack.GetUpdateTime() > request.mTrack.GetUpdateTime())
      {
         request.mTrack = aTrack; // Update the track
         requestUpdated = true;
      }

      // If the new mode is different that the old mode, de-select the old mode if no one else is using it
      // and select the new mode.
      startedTracking = true;
      if (request.mModeIndex != modeIndex)
      {
         // Make sure the maximum request count in the new mode is not exceeded.
         if (activeRequestsInMode < modePtr->GetMaximumRequestCount())
         {
            DeselectMode(aSimTime, request);
            request.mModeIndex = modeIndex;

            // The next detection chance is set to a uniform random value within the frame time of the new mode.
            request.mNextVisitTime =
               aSimTime + mSensorPtr->GetSimulation()->GetRandom().Uniform(0.0, modePtr->GetFrameTime());
            UpdateNextTrackVisitTime();

            SelectMode(aSimTime, request);
            requestUpdated = true;

            // This request is out-of-band, so we must change the update interval to force a sensing chance.
            double nextUpdateTime = std::min(mNextSearchVisitTime, mNextTrackVisitTime);
            mSensorPtr->SetNextUpdateTime(aSimTime, nextUpdateTime);
         }
      }
      if (requestUpdated)
      {
         WsfObserver::SensorRequestUpdated(mSensorPtr->GetSimulation())(aSimTime, mSensorPtr, modePtr, &aTrack);
      }
   }
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Sensor received start tracking request.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
      out.AddNote() << "Track ID: " << aTrack.GetTrackId();
      out.AddNote() << "Status: " << startedTracking;
   }
   return startedTracking;
}

// =================================================================================================
// virtual
bool WsfDefaultSensorScheduler::StopTracking(double aSimTime, const WsfTrackId& aRequestId)
{
   bool stoppedTracking = false;
   auto rli             = find_if(mRequestList.begin(), mRequestList.end(), CompareRequestId(aRequestId));
   if (rli != mRequestList.end())
   {
      Request& request = *rli;
      stoppedTracking  = true;
      DeselectMode(aSimTime, request);
      WsfObserver::SensorRequestCanceled(mSensorPtr->GetSimulation())(aSimTime, mSensorPtr, &request.mTrack);
      mRequestList.erase(rli);
      UpdateNextTrackVisitTime();
      // TODO-This may be overly aggressive because it currently causes the sensor track to be dropped.
      // TODO-If this sensor also has a simultaneous search capability then it will force the M/N criteria
      // TODO-to be re-satisfied.
      if (mTrackerPtr != nullptr)
      {
         mTrackerPtr->StopTracking(aSimTime, aRequestId);
      }
   }
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Sensor received stop tracking request.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
      out.AddNote() << "Track ID: " << aRequestId;
      out.AddNote() << "Status: " << stoppedTracking;
   }
   return stoppedTracking;
}

// =================================================================================================
// virtual
void WsfDefaultSensorScheduler::TrackerModeChange(double            aSimTime,
                                                  const WsfTrackId& aRequestId,
                                                  size_t            aTargetIndex,
                                                  WsfPlatform*      aTargetPtr,
                                                  const WsfTrack*   aTrackPtr,
                                                  WsfStringId       aModeNameId)
{
   size_t newModeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
   if (newModeIndex < mSensorPtr->GetModeList()->GetModeCount())
   {
      auto rli = std::find_if(mRequestList.begin(), mRequestList.end(), CompareRequestId(aRequestId));
      if (rli != mRequestList.end())
      {
         Request& request = *rli;
         if (request.mModeIndex != newModeIndex)
         {
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Sensor scheduler changing mode for track.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
               out.AddNote() << "Sensor: " << mSensorPtr->GetName();
               out.AddNote() << "Track ID: " << request.mRequestId;
               out.AddNote() << "Old Mode: " << mModeList[request.mModeIndex]->GetName();
               out.AddNote() << "New Mode: " << mModeList[newModeIndex]->GetName();
            }
            DeselectMode(aSimTime, request);
            request.mModeIndex = newModeIndex;
            SelectMode(aSimTime, request);

            // Update the request track with the most recent sensor track (if supplied).
            if (aTrackPtr != nullptr)
            {
               request.mTrack = *aTrackPtr;
            }

            // Update the next visit time for the request based on the new mode.
            WsfSensorMode* modePtr = mModeList[request.mModeIndex];
            request.mNextVisitTime = aSimTime + modePtr->GetFrameTime();
            UpdateNextTrackVisitTime();

            WsfObserver::SensorRequestUpdated(
               mSensorPtr->GetSimulation())(aSimTime, mSensorPtr, mModeList[request.mModeIndex], &(request.mTrack));
         }
      }
   }
}

// =================================================================================================
// virtual
void WsfDefaultSensorScheduler::TurnOff(double aSimTime)
{
   // When the sensor is turned off the sensing chance queue is cleared to save memory.
   ResetSearchList();
   ResetTrackList();
}

// =================================================================================================
// virtual
void WsfDefaultSensorScheduler::TurnOn(double aSimTime)
{
   // Evaluate the ability to perform search requests. This must be done before UpdateSearchChanceInterval()
   // because it uses the frame time of the search mode to determine the interval.
   CheckSearchModeAvailability();

   // When the sensor is turned off the sensing chance queue is cleared to save memory.
   // The queue must be rebuilt when the sensor is turned back on.

   ResetSearchList(); // This should be a no-op...
   WsfSimulation& sim           = *mSensorPtr->GetSimulation();
   size_t         platformCount = sim.GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* targetPtr = sim.GetPlatformEntry(platformEntry);
      if (targetPtr != mSensorPtr->GetPlatform())
      {
         mSensorPtr->PlatformAdded(aSimTime, targetPtr);
      }
   }
   UpdateSearchChanceInterval();

   // The track request queue is NOT cleared because even if one issues a request to
   // turn the sensor on, it is really an event that must be scheduled and dispatched.
   // Therefore one can't ensure that the 'StartTracking' occurs before the 'TurnOn'.

   // Reset the time for each of the requests so they start occurring.

   double nextVisitTime = aSimTime;
   for (auto& i : mRequestList)
   {
      i.mNextVisitTime = nextVisitTime;
      nextVisitTime += mModeList[i.mModeIndex]->GetDwellTime();
   }
   mDwellEndTime = aSimTime;
   UpdateNextTrackVisitTime();

   mNextSearchVisitTime = aSimTime;

   // Don't check the search list on the first update (no tracks have been formed, no targets have been deleted)
   mCheckSearchList = false;
}

// =================================================================================================
//! Check if the current mode selections provide for the ability to process search chances.
// private
void WsfDefaultSensorScheduler::CheckSearchModeAvailability()
{
   if (mSensorPtr->GetModeList()->CanMultiSelect())
   {
      // Multi-select sensor.  First check to see if any selected mode prevents searching.
      mSearchAllowed = true;
      unsigned int modeIndex;
      for (modeIndex = 0; modeIndex < mModeList.size(); ++modeIndex)
      {
         WsfSensorMode* modePtr = mModeList[modeIndex];
         if (modePtr->IsSelected() && modePtr->DisablesSearch())
         {
            mSearchAllowed = false;
            break;
         }
      }

      // If searching hasn't been disabled by now, determine which mode should be used for searching (if any).
      if (mSearchAllowed)
      {
         // If the last explicit mode is enabled and can search then it will be used.
         if ((mLastExplicitModeIndex < mModeList.size()) && mModeList[mLastExplicitModeIndex]->CanSearchWhileTrack() &&
             mModeList[mLastExplicitModeIndex]->IsSelected())
         {
            mSearchModeIndex = mLastExplicitModeIndex;
         }
         else
         {
            // A search mode has not been explicitly selected. Use the first selected mode that allows searching.
            mSearchAllowed = false;
            for (modeIndex = 0; modeIndex < mModeList.size(); ++modeIndex)
            {
               WsfSensorMode* modePtr = mModeList[modeIndex];
               if (modePtr->IsSelected() && modePtr->CanSearchWhileTrack())
               {
                  mSearchAllowed   = true;
                  mSearchModeIndex = modeIndex;
                  break;
               }
            }
         }
      }
   }
   else
   {
      // Single-select sensor. The ability to search is purely determined by the current mode.
      mSearchAllowed   = false;
      size_t modeIndex = mSensorPtr->GetModeList()->GetCurrentMode();
      if ((modeIndex < mModeList.size()) && (mModeList[modeIndex]->CanSearchWhileTrack()))
      {
         mSearchAllowed   = true;
         mSearchModeIndex = modeIndex;
      }
   }
}

// =================================================================================================
//! Deselect the mode associated with the current request if it is not in use by other requests.
// private
void WsfDefaultSensorScheduler::DeselectMode(double aSimTime, Request& aRequest)
{
   // The current mode is deselected if and only if the sensor has multi-select capability and there
   // are no other uses of the mode.
   if (mSensorPtr->GetModeList()->CanMultiSelect())
   {
      // Determine if anyone else is using the current mode.
      //
      // TODO What should be done if the mode being deselected is the mode being used for search.
      // TODO Should it be deactivated????

      bool shouldDeselect = true;
      for (auto& rli : mRequestList)
      {
         if ((aRequest.mModeIndex == rli.mModeIndex) && // same mode
             (aRequest.mRequestId != rli.mRequestId))   // not this request
         {
            shouldDeselect = false; // don't deselect because it has at least one active request
            break;
         }
      }

      if (shouldDeselect)
      {
         WsfSensorMode* modePtr = mModeList[aRequest.mModeIndex];
         mSensorPtr->ScheduleModeChange(aSimTime, false, modePtr->GetNameId());
      }
   }
}

// =================================================================================================
// private
void WsfDefaultSensorScheduler::SelectMode(double aSimTime, Request& aRequest)
{
   WsfSensorMode* modePtr = mModeList[aRequest.mModeIndex];
   if (!modePtr->IsSelected())
   {
      mSensorPtr->ScheduleModeChange(aSimTime, true, modePtr->GetNameId());
   }
}

// =================================================================================================
// private
void WsfDefaultSensorScheduler::ResetSearchList()
{
   // Delete the scan chances.
   mSearchList.clear();
   mSearchIndex         = 0;
   mNextSearchVisitTime = 1.0E+30;
}

// =================================================================================================
// private
void WsfDefaultSensorScheduler::ResetTrackList()
{
   // Delete the track requests.
   mRequestList.clear();
   mRequestIndex       = 0;
   mNextTrackVisitTime = 1.0E+30;
}

// =================================================================================================
// private
size_t WsfDefaultSensorScheduler::SelectTargetForRequest(Request& aRequest)
{
   size_t targetIndex = aRequest.mTargetIndex;
   if (targetIndex == 0)
   {
      // TODO Need to select a target index !!!
      targetIndex           = aRequest.mTrack.GetTargetIndex();
      aRequest.mTargetIndex = targetIndex;
   }
   return targetIndex;
}

// =================================================================================================
//! Returns true if the target has an active request against it
// private
bool WsfDefaultSensorScheduler::TargetHasActiveRequest(size_t aTargetIndex) const
{
   if (!mRequestList.empty())
   {
      for (const auto& i : mRequestList)
      {
         if (i.mTargetIndex == aTargetIndex)
         {
            return true;
         }
      }
   }
   return false;
}

// =================================================================================================
//! Update the time when the next track revisit should occur.
//! Note that the 'next visit time' is the time that would occur if there were no interference.
//! What we look for is the 'oldest' request (the one with the smallest next visit time).
//! The visit time is then adjusted as required.
// private
void WsfDefaultSensorScheduler::UpdateNextTrackVisitTime()
{
   mNextTrackVisitTime = 1.0E+30;
   mRequestIndex       = mRequestList.size();
   for (RequestListIndex i = 0; i < mRequestList.size(); ++i)
   {
      if (mRequestList[i].mNextVisitTime <= mNextTrackVisitTime)
      {
         mNextTrackVisitTime = mRequestList[i].mNextVisitTime;
         mRequestIndex       = i;
      }
   }

   // If a request was selected, don't allow it to interfere with any dwell that
   // is taking place for request
   if (mRequestIndex < mRequestList.size())
   {
      mNextTrackVisitTime = std::max(mNextTrackVisitTime, mDwellEndTime);
   }
}

// =================================================================================================
//! Update the interval between scan sensing chances.
//! This should be called whenever the scan sensing chance entry is added or deleted or whenever
//! the search mode is changed.
// private
void WsfDefaultSensorScheduler::UpdateSearchChanceInterval()
{
   if (mSearchAllowed)
   {
      mSearchChanceInterval = mModeList[mSearchModeIndex]->GetFrameTime();
      if (!mSearchList.empty())
      {
         mSearchChanceInterval = mSearchChanceInterval / mSearchList.size();
      }
   }
   else
   {
      // Search is not allowed... set the interval to a fairly low rate to force the check for
      // search track drop and deleted targets.
      mSearchChanceInterval = 10.0;
      mSearchFrameTime      = -1.0;
   }
   mSensorPtr->SetUpdateInterval(mSearchChanceInterval);
}

// =================================================================================================
void WsfDefaultSensorScheduler::UpdateSearchFrameTime(double aSimTime)
{
   // The time of the last search chance, if not interrupted by track requests.
   double lastSearchVisitTime = mNextSearchVisitTime - mSearchChanceInterval;

   // Update the search chance interval (based on the frame_time and the number of targets).
   UpdateSearchChanceInterval();

   // If the frame time for the active search mode changed then a forced change to 'next update time' must
   // be made because frame time changes are almost always out-of-band (i.e.: not during a sensing chance).
   if (mSearchAllowed && (mModeList[mSearchModeIndex]->GetFrameTime() != mSearchFrameTime))
   {
      mSearchFrameTime      = mModeList[mSearchModeIndex]->GetFrameTime();
      mNextSearchVisitTime  = lastSearchVisitTime + mSearchChanceInterval;
      mNextSearchVisitTime  = std::max(mNextSearchVisitTime, aSimTime);
      mNextSearchVisitTime  = std::max(mNextSearchVisitTime, mDwellEndTime);
      double nextUpdateTime = std::min(mNextSearchVisitTime, mNextTrackVisitTime);
      mSensorPtr->SetNextUpdateTime(aSimTime, nextUpdateTime);
   }
}
