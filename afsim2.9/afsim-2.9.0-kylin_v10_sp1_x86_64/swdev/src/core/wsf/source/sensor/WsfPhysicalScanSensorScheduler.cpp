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

#include "WsfPhysicalScanSensorScheduler.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSimulation.hpp"

// This class was developed in order to schedule targets to be searched based on their physical location
// relative to the sensor.

WsfPhysicalScanSensorScheduler::WsfPhysicalScanSensorScheduler(std::string      aTypeName,
                                                               bool             aRandomizeInitialHeading,
                                                               SearchListSortFn aSearchListSort)
   : WsfSensorScheduler()
   , mTypeName(std::move(aTypeName))
   , mSearchListSort(std::move(aSearchListSort))
{
   if (!aRandomizeInitialHeading)
   {
      mInitialHeading = 0.0;
   }
}

WsfPhysicalScanSensorScheduler::WsfPhysicalScanSensorScheduler(const WsfPhysicalScanSensorScheduler& aSrc)
   : WsfSensorScheduler(aSrc)
   , mTypeName(aSrc.mTypeName)
   , mInitialHeading(aSrc.mInitialHeading)
   , mSearchListSort(aSrc.mSearchListSort)
{
}

//! Factory method for WsfSensorSchedulerTypes to determine if a scheduler
//! represented by this class is being requested.
// static
//  =================================================================================================
std::unique_ptr<WsfSensorScheduler> WsfPhysicalScanSensorScheduler::ObjectFactory(const std::string& aTypeName)
{
   if (aTypeName == "physical_scan")
   {
      return ut::make_unique<WsfPhysicalScanSensorScheduler>();
   }
   return nullptr;
}

// virtual
WsfSensorScheduler* WsfPhysicalScanSensorScheduler::Clone() const
{
   return new WsfPhysicalScanSensorScheduler(*this);
}

bool WsfPhysicalScanSensorScheduler::ProcessInput(UtInput& aInput)
{
   std::string command{aInput.GetCommand()};
   if (command == "initial_heading")
   {
      aInput.ReadValueOfType(mInitialHeading.emplace(), UtInput::cANGLE);
      mInitialHeading = UtMath::NormalizeAngle0_TwoPi(mInitialHeading.value());
      return true;
   }
   return WsfSensorScheduler::ProcessInput(aInput);
}

// virtual
bool WsfPhysicalScanSensorScheduler::Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorTracker* aTrackerPtr)
{
   if (aSensorPtr->GetModeList() == nullptr)
   {
      ut::log::error() << "A mode list is required for " << mTypeName << ".";
      assert(aSensorPtr->GetModeList() != nullptr);
      return false;
   }

   // Initialize Sensor Scheduler with no Tracker
   bool ok = WsfSensorScheduler::Initialize(aSimTime, aSensorPtr, nullptr);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   aSensorPtr->GetModeList()->GetDerivedModeList(mModeList);
   mLastExplicitModeIndex = mModeList.size();

   // Must have an update interval in order to determine size of "pie"
   mSensorUpdateInterval = aSensorPtr->GetUpdateInterval();
   if (mSensorUpdateInterval == 0.0)
   {
      auto out = ut::log::error() << "An 'update_interval' is required for " << mTypeName << " scheduler.";
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
      return false;
   }

   // If initial heading is not specified, use a random direction to start pointing the sensor "pie"
   if (!mInitialHeading)
   {
      mInitialHeading = GetSimulation()->GetRandom().Uniform(0.0, UtMath::cTWO_PI);
   }
   double currentHeading = mInitialHeading.value();

   for (size_t modeIndex = 0; modeIndex < mModeList.size(); ++modeIndex)
   {
      WsfSensorMode* modePtr = mModeList[modeIndex];

      // Ensure that each mode has a frame time.
      double modeFrameTime = modePtr->GetFrameTime();
      if (modeFrameTime <= 0.0)
      {
         auto out = ut::log::error() << "Unspecified 'frame_time'.";
         out.AddNote() << "Sensor: " << mSensorPtr->GetName();
         out.AddNote() << "Mode: " << modePtr->GetName();
         ok = false;
      }

      // Determine size of sensor "pie" slice that is checked each update
      double numTimesCalledPerUpdate                      = modeFrameTime / mSensorUpdateInterval;
      mSearchModeDataMap[modeIndex].mSensorPieSize        = UtMath::cTWO_PI / numTimesCalledPerUpdate;
      mSearchModeDataMap[modeIndex].mCurrentSensorHeading = currentHeading;

      // Initialize past time
      mSearchModeDataMap[modeIndex].mLastUpdate = aSimTime;

      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Scheduling initialization of sensor.";
         out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << mSensorPtr->GetName();
         out.AddNote() << "Type: " << mSensorPtr->GetType();
         out.AddNote() << "Mode: " << mModeList[modeIndex]->GetName();
         out.AddNote() << "Update_Interval: " << mSensorUpdateInterval << " sec";
         out.AddNote() << "Mode Frame Time: " << modeFrameTime << " sec";
         out.AddNote() << "SweepAngle Per Update: " << mSearchModeDataMap[modeIndex].mSensorPieSize * UtMath::cDEG_PER_RAD
                       << " degrees";
         out.AddNote() << "Initial Heading: " << mSearchModeDataMap[modeIndex].mCurrentSensorHeading * UtMath::cDEG_PER_RAD
                       << " degrees";
      }
   }

   return ok;
}

// virtual
void WsfPhysicalScanSensorScheduler::ModeDeselected(double aSimTime, WsfStringId aModeNameId)
{
   size_t modeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
   if (mLastExplicitModeIndex == modeIndex)
   {
      mLastExplicitModeIndex = mModeList.size();
   }
   CheckSearchModeAvailability();
}

// virtual
void WsfPhysicalScanSensorScheduler::ModeSelected(double aSimTime, WsfStringId aModeNameId)
{
   mLastExplicitModeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
   CheckSearchModeAvailability();
}

// virtual
void WsfPhysicalScanSensorScheduler::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Put the platform on the back of the queue.  The entries are processed by their physical location
   // in relation to the sensor not where they are in the queue.
   if (mSearchListSort)
   {
      size_t index = aPlatformPtr->GetIndex();
      mSearchList.insert(std::upper_bound(mSearchList.begin(), mSearchList.end(), index, mSearchListSort), index);
      return;
   }
   mSearchList.push_back(aPlatformPtr->GetIndex());
}

// virtual
void WsfPhysicalScanSensorScheduler::RemoveTarget(double aSimTime, size_t aTargetIndex)
{
   auto sli = find(mSearchList.begin(), mSearchList.end(), aTargetIndex);
   if (sli != mSearchList.end())
   {
      mSearchList.erase(sli);
   }
}

// virtual
bool WsfPhysicalScanSensorScheduler::SelectTarget(double               aSimTime,
                                                  double&              aNextSimTime,
                                                  size_t&              aTargetIndex,
                                                  WsfTrackId&          aRequestId,
                                                  WsfSensor::Settings& aSettings)
{
   aRequestId.Null();
   aTargetIndex = 0;

   if ((aSimTime - mLastUpdate) >= 0.001)
   {
      for (size_t modeIndex : mSearchModeIndex)
      {
         assert(modeIndex < mModeList.size());
         if (modeIndex >= mModeList.size())
         {
            return false;
         }

         if (mSearchModeDataMap[modeIndex].mLastUpdate != aSimTime)
         {
            // This is a new update pass, move the sensor angle and determine which tracks are in this FOV

            // Increase the current Sensor heading by size of the Sensor pie slice
            double Te         = aSimTime - mSearchModeDataMap[modeIndex].mLastUpdate;
            double numUpdates = Te / mSensorUpdateInterval;
            mSearchModeDataMap[modeIndex].mCurrentSensorHeading =
               UtMath::NormalizeAngle0_TwoPi(mSearchModeDataMap[modeIndex].mCurrentSensorHeading +
                                             mSearchModeDataMap[modeIndex].mSensorPieSize * numUpdates);

            // Determine azimuth boundaries
            double az1 = mSearchModeDataMap[modeIndex].mCurrentSensorHeading -
                         (mSearchModeDataMap[modeIndex].mSensorPieSize / 2.0);
            double az2 = mSearchModeDataMap[modeIndex].mCurrentSensorHeading +
                         (mSearchModeDataMap[modeIndex].mSensorPieSize / 2.0);

            double az3 = az1;
            double az4 = az2;
            if (az2 > UtMath::cTWO_PI)
            {
               // Set up to handle condition when the sensor pie goes all the way around
               az4 = az2 - UtMath::cTWO_PI;
               az3 = 0.0;
               az2 = UtMath::cTWO_PI;
            }

            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Azimuth search area report:";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
               out.AddNote() << "Sensor: " << mSensorPtr->GetName();
               out.AddNote() << "Type: " << mSensorPtr->GetType();
               out.AddNote() << "Mode: " << mModeList[modeIndex]->GetName();
               out.AddNote() << "Current Sensor Azimuth: "
                             << mSearchModeDataMap[modeIndex].mCurrentSensorHeading * UtMath::cDEG_PER_RAD << " degrees";
               out.AddNote() << "Azimuth search area : " << az1 * UtMath::cDEG_PER_RAD << " to "
                             << az4 * UtMath::cDEG_PER_RAD << " degrees from location";
            }

            // Go through the list of allowed platforms and see which are in this slice of the FOV
            if ((!mSearchList.empty()) && mSearchAllowed)
            {
               // Loop through list of platforms to search
               for (size_t searchIndex : mSearchList)
               {
                  // If this target falls within the Azimuth FOV values for this update interval

                  WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(searchIndex);
                  if (targetPtr)
                  {
                     // Find location of this target relative to sensor
                     targetPtr->Update(aSimTime);
                     double targetLocWCS[3];
                     targetPtr->GetLocationWCS(targetLocWCS);

                     // Compute bearing in degrees
                     double bearing = UtMath::NormalizeAngle0_TwoPi(mSensorPtr->GetPlatform()->TrueBearing(targetLocWCS));

                     if (((bearing >= az1) && (bearing < az2)) || ((bearing >= az3) && (bearing < az4)))
                     {
                        if (DebugEnabled())
                        {
                           // double relTgtLocNED[3];
                           // mSensorPtr->GetPlatform()->ConvertWCSToNED(targetLocWCS, relTgtLocNED);
                           auto out = ut::log::debug() << "Adding target to search interval.";
                           out.AddNote() << "Target: " << targetPtr->GetName();
                           out.AddNote() << "Bearing: " << bearing * UtMath::cDEG_PER_RAD;
                        }

                        // Add to list of targets to return
                        SensorTarget sensorTarget(searchIndex, modeIndex);
                        mSensorTargetQueue.push(sensorTarget);
                     }
                  }
               }
            }
         }
         // save off current value of sim time for next iteration
         mSearchModeDataMap[modeIndex].mLastUpdate = aSimTime;
      }
      mLastUpdate = aSimTime;
   }

   // Return next target in list if queue is not empty
   aNextSimTime = aSimTime + mSensorPtr->GetUpdateInterval();
   if (!mSensorTargetQueue.empty())
   {
      // Process target
      aTargetIndex         = mSensorTargetQueue.front().mTargetIndex;
      aSettings.mModeIndex = mSensorTargetQueue.front().mModeIndex;
      mSensorTargetQueue.pop();
      aNextSimTime = aSimTime; // Keep running...
   }

   return (aTargetIndex != 0);
}

// virtual
void WsfPhysicalScanSensorScheduler::TurnOff(double aSimTime)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Scheduler turned off.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
   }

   // When the sensor is turned off the platform search list is cleared to save memory.
   ResetSearchList();
}

// virtual
void WsfPhysicalScanSensorScheduler::TurnOn(double aSimTime)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Scheduler turned on.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
   }

   // When the sensor is turned off the platform search queue is cleared to save memory.
   // The queue must be rebuilt when the sensor is turned back on.

   ResetSearchList(); // This should be a no-op...
   WsfSimulation& sim           = *GetSimulation();
   size_t         platformCount = sim.GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* targetPtr = sim.GetPlatformEntry(platformEntry);

      if (targetPtr != mSensorPtr->GetPlatform())
      {
         mSensorPtr->PlatformAdded(aSimTime, targetPtr);
      }
   }

   // Evaluate the ability to perform search requests.
   CheckSearchModeAvailability();
}

//! Check if the current mode selections provide for the ability to process search chances.
// private
void WsfPhysicalScanSensorScheduler::CheckSearchModeAvailability()
{
   // Reset the search modes list and flag
   mSearchModeIndex.clear();
   mSearchAllowed = false;

   // Loop over and add any search capable modes (single and multi-select mode sensors)
   for (size_t modeIndex = 0; modeIndex < mModeList.size(); ++modeIndex)
   {
      WsfSensorMode* modePtr = mModeList[modeIndex];
      if (modePtr->IsSelected())
      {
         if (modePtr->DisablesSearch())
         {
            mSearchAllowed = false;
            mSearchModeIndex.clear();
            break;
         }
         else if (modePtr->CanSearchWhileTrack())
         {
            mSearchAllowed = true;
            mSearchModeIndex.push_back(modeIndex);
         }
      }
   }

   /*
   if (mSensorPtr->GetModeList()->CanMultiSelect())
   {
      // Multi-select sensor.  First check to see if any selected mode prevents searching.
      mSearchAllowed = true;
      unsigned int modeIndex;
      for (modeIndex = 0; modeIndex < mModeList.size(); ++modeIndex)
      {
         WsfSensorMode* modePtr = mModeList[modeIndex];
         if (modePtr->IsSelected() &&
             modePtr->DisablesSearch())
         {
            mSearchAllowed = false;
            break;
         }
      }

      // If searching hasn't been disabled by now, determine which mode should be used for searching (if any).
      if (mSearchAllowed)
      {
         // If the last explicit mode is enabled and can search then it will be used.
         if ((mLastExplicitModeIndex < mModeList.size()) &&
             mModeList[mLastExplicitModeIndex]->CanSearchWhileTrack() &&
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
               if (modePtr->IsSelected() &&
                   modePtr->CanSearchWhileTrack())
               {
                  mSearchAllowed = true;
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
      mSearchModeIndex = mSensorPtr->GetModeList()->GetCurrentMode();
      if ((mSearchModeIndex < mModeList.size()) &&
          (mModeList[mSearchModeIndex]->CanSearchWhileTrack()))
      {
         mSearchAllowed = true;
      }
   }
   */
}

// private
void WsfPhysicalScanSensorScheduler::ResetSearchList()
{
   // Delete the scan chances.
   mSearchList.clear();
}
