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

//! A specialization of WsfSensor that provides the baseline passive RF sensor.

#include "WsfPassiveSensor.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfComm.hpp"
#include "WsfCommComponentHW.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorComponent.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfUtil.hpp"
#include "script/WsfScriptContext.hpp"

#define DEBUG_PASSIVE_SENSOR false

// =================================================================================================
WsfPassiveSensor::WsfPassiveSensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mPassiveModeList()
   , mTargetTypeReporting()
   , mEmitterTypeReporting()
   , mSignalList()
   , mUpdateActive(false)
{
   SetClass(cPASSIVE | cRADIO); // This is a passive RF sensor

   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new PassiveMode()));

   // Assign the default sensor scheduler
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());

   // Assign the default sensor tracker
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));

   mThreadSafe = false;
}

// =================================================================================================
//! Copy constructor (for Clone()).
WsfPassiveSensor::WsfPassiveSensor(const WsfPassiveSensor& aSrc)
   : WsfSensor(aSrc)
   , mPassiveModeList()
   , mTargetTypeReporting(aSrc.mTargetTypeReporting)
   , mEmitterTypeReporting(aSrc.mEmitterTypeReporting)
   , mSignalList()
   , mUpdateActive(aSrc.mUpdateActive)
{
}

// =================================================================================================
//! @copydoc WsfSensor::AttemptToDetect
//!
//! This does the same as the base class method except when called for the 'master' in a TDOA system.
//! In that case a controller is invoked which invokes the detection processing for the master and
//! each of the slaves and then performs TDOA processing on the collected results.
//!
//! See the base class for documentation of the arguments and return value.
// virtual
bool WsfPassiveSensor::AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult)
{
   assert(aSettings.mModeIndex < mPassiveModeList.size());
   if (aSettings.mModeIndex >= mPassiveModeList.size())
   {
      return false;
   }

   // Modify the base slewing limits with the mode-specific slewing limits.
   WsfSensorMode* modePtr = mPassiveModeList[aSettings.mModeIndex];
   modePtr->UpdateSensorCueingLimits();

   bool detected = modePtr->AttemptToDetect(aSimTime, aTargetPtr, aSettings, aResult);

   if (detected)
   {
      mTargetTypeReporting.AddTargetReportState(aSimTime, aTargetPtr);
   }
   else
   {
      mTargetTypeReporting.RemoveTargetReportState(aTargetPtr->GetIndex());
   }

   return detected;
}

// =================================================================================================
// virtual
WsfSensor* WsfPassiveSensor::Clone() const
{
   return new WsfPassiveSensor(*this);
}

// =================================================================================================
// virtual
bool WsfPassiveSensor::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mPassiveModeList);

   return ok;
}

// =================================================================================================
// virtual
bool WsfPassiveSensor::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mEmitterTypeReporting.ProcessInput(aInput))
   {
   }
   else if (mTargetTypeReporting.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfSensor::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
void WsfPassiveSensor::TurnOff(double aSimTime)
{
   WsfSensor::TurnOff(aSimTime);
   mTargetTypeReporting.RemoveAllTargetReportStates();
   mEmitterTypeReporting.RemoveAllEmitterReportStates();
}

// =================================================================================================
// virtual
void WsfPassiveSensor::Update(double aSimTime)
{
   // Exit quickly if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime <= (aSimTime + 1.0E-5))
   {
      mUpdateActive = true;
      WsfSensor::Update(aSimTime);
      PerformScheduledDetections(aSimTime); // Perform any required detection attempts
      mUpdateActive = false;
   }
}

// =================================================================================================
// virtual
size_t WsfPassiveSensor::GetEM_RcvrCount() const
{
   size_t count = 0;
   if (!mPassiveModeList.empty())
   {
      PassiveMode* modePtr = mPassiveModeList[mModeListPtr->GetCurrentMode()];
      count                = modePtr->mBeamList.size();
   }
   return count;
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfPassiveSensor::GetEM_Rcvr(size_t aIndex) const
{
   PassiveMode* modePtr = mPassiveModeList[mModeListPtr->GetCurrentMode()];
   return *modePtr->mBeamList[aIndex].mRcvrPtr;
}

// =================================================================================================
// This is called because we've registered as a change listener and updates to a
// target's emitters is needed.
// This implementation will cause the listener (i.e. not the change listeners) to
// be reset  if they are used also for this target's emitters.
// virtual
void WsfPassiveSensor::SignalChangeCallback(double aSimTime, size_t aTargetIndex)
{
   if (IsSlave())
   {
      return;
   }

   // Make sure that a scheduler and tracker are present.
   assert(mModeListPtr != nullptr);
   assert(mSchedulerPtr != nullptr);
   assert(mTrackerPtr != nullptr);

   if (mTrackerPtr->GetActiveTrackCount() == 0)
   {
      return;
   }

   // Perform the sensing chance if the target still exists.
   WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(aTargetIndex);
   if (targetPtr != nullptr)
   {
      if (targetPtr->IsFalseTarget())
      {
         return;
      }

      WsfTrackId                 requestId;
      Settings                   settings;
      WsfSensorResult            result;
      WsfSensorTracker::Settings stSettings;
      WsfTrackId                 trackId;
      mTrackerPtr->GetRequestDataForTarget(aTargetIndex, requestId, settings.mModeIndex, trackId);

      if (trackId.IsNull())
      {
         return;
      }

      PassiveMode* modePtr = mPassiveModeList[settings.mModeIndex];
      assert(modePtr != nullptr);

      settings.mRequiredPd = WsfSensor::GetRequiredPd(modePtr);
      if (modePtr->WithinDetectionRange(aSimTime, targetPtr))
      {
         if (AttemptToDetect(aSimTime, targetPtr, settings, result))
         {
            // Apply errors and indicate target is detected
            modePtr->ApplyMeasurementErrors(result);
            mTrackerPtr->TargetDetected(aSimTime, stSettings, requestId, aTargetIndex, targetPtr, result);
         }
         else
         {
            mTrackerPtr->TargetUndetected(aSimTime, stSettings, requestId, aTargetIndex, targetPtr, result);
         }
      }
      else
      {
         // Detection chance was culled because it is out of range. We must still report to the tracker
         // because it possible it may be coasting the target.
         result.Reset();
         result.mModeIndex     = settings.mModeIndex;
         result.mCheckedStatus = WsfSensorResult::cRCVR_RANGE_LIMITS;
         result.mFailedStatus  = WsfSensorResult::cRCVR_RANGE_LIMITS;
         mTrackerPtr->TargetUndetected(aSimTime, stSettings, requestId, aTargetIndex, targetPtr, result);
      }
   }
}

// =================================================================================================
// Function developed to be called from script that would allow a user to change a Frequency Band's
// parameters or to add/set a new Frequency Band to an existing mode->beam->rcvr
bool WsfPassiveSensor::TuneFrequencyBand(double       aSimTime,
                                         bool         aReset,
                                         WsfStringId  aModeNameId,
                                         size_t       aBeamNumber,
                                         unsigned int aFrequencyBandNumber,
                                         double       aLowerFreq,
                                         double       aUpperFreq,
                                         double       aDwellTime,
                                         double       aRevisitTime)
{
   if (aLowerFreq >= aUpperFreq)
   {
      auto out = ut::log::error() << "Passive sensor cannot set frequency band.";
      out.AddNote() << "Low frequency is greater than high frequency.";
      out.AddNote() << "Low: " << aLowerFreq;
      out.AddNote() << "High: " << aUpperFreq;
      return false;
   }

   size_t modeIndex = mModeListPtr->GetCurrentMode();
   if (aModeNameId != 0)
   {
      modeIndex = mModeListPtr->GetModeByName(aModeNameId);
   }
   if (modeIndex < mPassiveModeList.size())
   {
      return mPassiveModeList[modeIndex]->TuneFrequencyBand(aSimTime,
                                                            aReset,
                                                            aBeamNumber,
                                                            aFrequencyBandNumber,
                                                            aLowerFreq,
                                                            aUpperFreq,
                                                            aDwellTime,
                                                            aRevisitTime);
   }
   else
   {
      auto out = ut::log::error() << "Invalid mode set for passive sensor.";
      out.AddNote() << "Mode: " << aModeNameId;
   }
   return false;
}

unsigned int WsfPassiveSensor::GetNumberofFrequencyBands(WsfStringId aModeId, size_t aBeam)
{
   size_t modeIndex = mModeListPtr->GetCurrentMode();
   if (aModeId != 0)
   {
      modeIndex = mModeListPtr->GetModeByName(aModeId);
   }
   PassiveMode* modePtr = mPassiveModeList[modeIndex];

   unsigned int bandCount = 0;
   if (modePtr)
   {
      if ((aBeam <= modePtr->mBeamList.size()) && (aBeam > 0))
      {
         PassiveBeam& beam = modePtr->mBeamList[aBeam - 1];
         bandCount         = static_cast<unsigned int>(beam.mRcvrPtr->mFrequencyBands.size());
      }
      else
      {
         auto out = ut::log::error() << "Invalid beam index used to request the number of frequency bands.";
         out.AddNote() << "Actual: " << aBeam;
         out.AddNote() << "Expected: 1 <= N <= Beam Count";
         out.AddNote() << "Beam Count = " << modePtr->mBeamList.size(); // Using equals instead of colon
      }
   }
   return bandCount;
}

// =================================================================================================
// Nested class WsfPassiveSensor::PassiveRcvr.
// =================================================================================================

// =================================================================================================
WsfPassiveSensor::PassiveRcvr::PassiveRcvr(WsfEM_Antenna* aAntennaPtr)
   : WsfEM_Rcvr(WsfEM_Rcvr::cRF_PASSIVE_SENSOR, aAntennaPtr)
   , mFrequencyBands()
   , mModePtr(nullptr)
   , mPSOS_Enabled(false)
   , mHavePSOS_FrequencyData(false)
   , mPSOS_DebugEnabled(false)
{
}

// =================================================================================================
WsfPassiveSensor::PassiveRcvr::PassiveRcvr(const PassiveRcvr& aSrc, WsfEM_Antenna* aAntennaPtr)
   : WsfEM_Rcvr(aSrc, aAntennaPtr)
   , mFrequencyBands(aSrc.mFrequencyBands)
   , mModePtr(nullptr)
   , mPSOS_Enabled(aSrc.mPSOS_Enabled)
   , mHavePSOS_FrequencyData(aSrc.mHavePSOS_FrequencyData)
   , mPSOS_DebugEnabled(aSrc.mPSOS_DebugEnabled)
{
}

// =================================================================================================
// virtual
bool WsfPassiveSensor::PassiveRcvr::Initialize(WsfSimulation& aSimulation)
{
   mModePtr = static_cast<PassiveMode*>(GetMode());

   if (mFrequencyBands.empty())
   {
      auto out = ut::log::error() << "No frequency bands defined for passive sensor.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetArticulatedPart()->GetName();
      out.AddNote() << "Mode: " << mModePtr->GetName();
      return false;
   }
   else if (GetFrequency() == 0.0)
   {
      // If the receiver frequency was not specified, use the limits of all bands.
      // We need something to prevent an error while initializing the receiver.
      double minFreq = mFrequencyBands.at(0).mLowerFrequency;
      double maxFreq = mFrequencyBands.at(0).mUpperFrequency;
      for (size_t i = 1; i < mFrequencyBands.size(); ++i)
      {
         minFreq = std::min(minFreq, mFrequencyBands.at(i).mLowerFrequency);
         maxFreq = std::max(maxFreq, mFrequencyBands.at(i).mUpperFrequency);
      }
      double frequency = 0.5 * (minFreq + maxFreq);
      SetFrequency(frequency);

      // set receiver bandwidth to frequency band range
      double bandwidth = maxFreq - minFreq;
      SetBandwidth(bandwidth);
   }

   // Determine if the frequency scanning characteristics are defined.

   mHavePSOS_FrequencyData = ((mFrequencyBands.at(0).mDwellTime != 0.0) && (mFrequencyBands.at(0).mRevisitTime != 0.0));
   for (const FrequencyBand& band : mFrequencyBands)
   {
      if ((band.mDwellTime == 0.0) && (band.mRevisitTime == 0.0))
      {
         if (mHavePSOS_FrequencyData)
         {
            ut::log::error() << "Must specify 'dwell_time' and 'revisit_time' on all bands if used on any band.";
            return false;
         }
      }
      else if ((band.mDwellTime != 0.0) && (band.mRevisitTime != 0.0))
      {
         if (!mHavePSOS_FrequencyData)
         {
            ut::log::error() << "Must specify 'dwell_time' and 'revisit_time' on all bands if used on any band.";
         }
      }
      else
      {
         ut::log::error() << "Must specify both 'dwell_time' and 'revisit_time' if either is used.";
         return false;
      }
   }

   return WsfEM_Rcvr::Initialize(*mModePtr->GetSensor()->GetSimulation());
}

// =================================================================================================
// virtual
bool WsfPassiveSensor::PassiveRcvr::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "frequency_band")
   {
      FrequencyBand frequencyBand;
      aInput.ReadValueOfType(frequencyBand.mLowerFrequency, UtInput::cFREQUENCY);
      aInput.ReadValueOfType(frequencyBand.mUpperFrequency, UtInput::cFREQUENCY);
      aInput.ValueGreaterOrEqual(frequencyBand.mLowerFrequency, 0.0);
      aInput.ValueGreaterOrEqual(frequencyBand.mUpperFrequency, frequencyBand.mLowerFrequency);
      mFrequencyBands.push_back(frequencyBand);
      // The dwell_time and revisit_time, if present, must follow immediately after.
      bool done = false;
      while (!done)
      {
         aInput.ReadCommand(command);
         if (command == "dwell_time")
         {
            FrequencyBand& band = mFrequencyBands.back();
            aInput.ReadValueOfType(band.mDwellTime, UtInput::cTIME);
            aInput.ValueGreater(band.mDwellTime, 0.0);
         }
         else if (command == "revisit_time")
         {
            FrequencyBand& band = mFrequencyBands.back();
            aInput.ReadValueOfType(band.mRevisitTime, UtInput::cTIME);
            aInput.ValueGreater(band.mRevisitTime, 0.0);
         }
         else
         {
            aInput.PushBack(command);
            done = true;
         }
      }
   }
   else if (command == "scan_on_scan_model")
   {
      aInput.ReadValue(mPSOS_Enabled);
   }
   else if (command == "debug_scan_on_scan")
   {
      mPSOS_DebugEnabled = true;
   }
   else
   {
      myCommand = WsfEM_Rcvr::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
//! Can this passive sensor potentially interact with (detect) the specified emitter?
//!
//! This is invoked by WsfEM_Manager to maintain potential interaction lists.
//!
//! @param aXmtrPtr [input] The transmitter to be evaluated.
//! @returns 'true' if the potentially interact with the transmitter or 'false' if not.
// virtual
bool WsfPassiveSensor::PassiveRcvr::CanInteractWith(WsfEM_Xmtr* aXmtrPtr)
{
   bool canInteractWith = mModePtr->GetSensor()->AllowDetectionChancesFor(aXmtrPtr->GetPlatform());

   // For now we won't interact with a transmitter on our own platform.
   //
   // In theory, you could detect a transmitter on your own platform, but in reality
   // we hope that this isn't the case (e.g.: a interferer must turn off for a very brief
   // time to allow a passive sensor to detect frequency changes).  We really don't model these
   // short-duration on/off cycles (yet).

   if (canInteractWith && (GetPlatform() != aXmtrPtr->GetPlatform()))
   {
      // This method looks at the frequency of the transmitter and determines if it can
      // be detected by any of the frequency bands of this receiver.
      canInteractWith      = false;
      double xmtrLowerFreq = aXmtrPtr->GetFrequency() - (0.5 * aXmtrPtr->GetBandwidth());
      double xmtrUpperFreq = xmtrLowerFreq + aXmtrPtr->GetBandwidth();
      for (auto& band : mFrequencyBands)
      {
         double rcvrLowerFreq = band.mLowerFrequency;
         double rcvrUpperFreq = band.mUpperFrequency;
         if ((xmtrLowerFreq <= rcvrUpperFreq) && (xmtrUpperFreq >= rcvrLowerFreq))
         {
            canInteractWith = true;
            break;
         }
      }
   }
   return canInteractWith;
}

// =================================================================================================
// This specialized implementation that completely replaces the base class version.
// It calls the owning modes' implementation which maintains its own form of interactor list, which also
// includes determining the method used to detect the transmitter. The base class method, which maintains
// a list in the receiver of the interacting transmitters, is not used because we need additional data.
// This eliminates duplication and wasted memory.
// virtual
bool WsfPassiveSensor::PassiveRcvr::AddInteractor(WsfEM_Xmtr* aXmtrPtr)
{
   return mModePtr->AddInteractor(aXmtrPtr, this);
}

// =================================================================================================
// This specialized implementation completely replaces the base class version. It calls the owning
// modes' implementation which un-does the work of AddInteractor.
// virtual
bool WsfPassiveSensor::PassiveRcvr::RemoveInteractor(WsfEM_Xmtr* aXmtrPtr)
{
   return mModePtr->RemoveInteractor(aXmtrPtr, this);
}

// =================================================================================================
// This is called because we've registered as a listener to a particular transmitter.
// virtual
void WsfPassiveSensor::PassiveRcvr::EmitterActiveCallback(double aSimTime, WsfEM_Interaction& aResult)
{
   mModePtr->EmitterActiveCallback(aSimTime, aResult, this);
}

// =================================================================================================
// This is called because we've registered as a listener to a particular transmitter.
// virtual
void WsfPassiveSensor::PassiveRcvr::SignalChangeCallback(double aSimTime, size_t aTargetIndex)
{
   dynamic_cast<WsfPassiveSensor*>(mModePtr->GetSensor())->SignalChangeCallback(aSimTime, aTargetIndex);
}

// =================================================================================================
// Set, add or modify the frequency band parameters.
bool WsfPassiveSensor::PassiveRcvr::TuneFrequencyBand(double       aSimTime,
                                                      bool         aReset,
                                                      unsigned int aFrequencyBandNumber,
                                                      double       aLowerFreq,
                                                      double       aUpperFreq,
                                                      double       aDwellTime,
                                                      double       aRevisitTime)
{
   if (aReset)
   {
      if (aFrequencyBandNumber == 0)
      {
         if (((aDwellTime == 0.0) && (aRevisitTime != 0.0)) || ((aDwellTime != 0.0) && (aRevisitTime == 0.0)))
         {
            ut::log::error() << "Must specify both 'dwell_time' and 'revisit_time' if either is used.";
            return false;
         }

         mFrequencyBands.clear();

         FrequencyBand band;
         band.mLowerFrequency = aLowerFreq;
         band.mUpperFrequency = aUpperFreq;
         band.mDwellTime      = aDwellTime;
         band.mRevisitTime    = aRevisitTime;
         mFrequencyBands.push_back(band);
      }
      else if (aFrequencyBandNumber <= mFrequencyBands.size())
      {
         if (mFrequencyBands.size() == 1)
         {
            if (((aDwellTime == 0.0) && (aRevisitTime != 0.0)) || ((aDwellTime != 0.0) && (aRevisitTime == 0.0)))
            {
               ut::log::error() << "Must specify both 'dwell_time' and 'revisit_time' if either is used.";
               return false;
            }
         }
         else
         {
            if (mHavePSOS_FrequencyData)
            {
               if ((aDwellTime == 0.0) || (aRevisitTime == 0.0))
               {
                  auto out = ut::log::error() << "Band could not be returned.";
                  out.AddNote() << "The passive sensor was initialized to use the PSOS model.";
                  out.AddNote() << "When returning, both dwell time and revisit time must be non-zero.";
                  out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  out.AddNote() << "Sensor: " << GetArticulatedPart()->GetName();
                  out.AddNote() << "Band Number: " << aFrequencyBandNumber;
                  return false;
               }
            }
            else if ((aDwellTime != 0.0) || (aRevisitTime != 0.0))
            {
               {
                  auto out = ut::log::error() << "Band could not be returned.";
                  out.AddNote() << "The passive sensor was initialized to NOT use the PSOS model.";
                  out.AddNote() << "When returning, both dwell time and revisit time must be zero.";
                  out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  out.AddNote() << "Sensor: " << GetArticulatedPart()->GetName();
                  out.AddNote() << "Band Number: " << aFrequencyBandNumber;
                  return false;
               }
            }
         }
         mFrequencyBands[aFrequencyBandNumber - 1].mLowerFrequency = aLowerFreq;
         mFrequencyBands[aFrequencyBandNumber - 1].mUpperFrequency = aUpperFreq;
         mFrequencyBands[aFrequencyBandNumber - 1].mDwellTime      = aDwellTime;
         mFrequencyBands[aFrequencyBandNumber - 1].mRevisitTime    = aRevisitTime;
      }
      else
      {
         auto out = ut::log::error() << "Frequency band does not exist in passive sensor.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << GetArticulatedPart()->GetName();
         out.AddNote() << "Band Number: " << aFrequencyBandNumber;
      }
   }
   else // ADD
   {
      if (mHavePSOS_FrequencyData)
      {
         if ((aDwellTime == 0.0) || (aRevisitTime == 0.0))
         {
            auto out = ut::log::error() << "New passive sensor band could not be added.";
            out.AddNote() << "The passive sensor was initialized to use the PSOS model.";
            out.AddNote() << "When returning, both dwell time and revisit time must be non-zero.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Sensor: " << GetArticulatedPart()->GetName();
            out.AddNote() << "Band Number: " << aFrequencyBandNumber;
            return false;
         }
      }
      else if ((aDwellTime != 0.0) || (aRevisitTime != 0.0))
      {
         auto out = ut::log::error() << "New passive sensor band could not be added.";
         out.AddNote() << "The passive sensor was initialized to NOT use the PSOS model.";
         out.AddNote() << "When returning, both dwell time and revisit time must be zero.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << GetArticulatedPart()->GetName();
         out.AddNote() << "Band Number: " << aFrequencyBandNumber;
         return false;
      }

      FrequencyBand band;
      band.mLowerFrequency = aLowerFreq;
      band.mUpperFrequency = aUpperFreq;
      band.mDwellTime      = aDwellTime;
      band.mRevisitTime    = aRevisitTime;
      mFrequencyBands.push_back(band);
   }


   // If the receiver frequency was not specified, use the limits of all bands.
   // We need something to prevent an error while initializing the receiver.
   double minFreq = mFrequencyBands.at(0).mLowerFrequency;
   double maxFreq = mFrequencyBands.at(0).mUpperFrequency;
   for (size_t i = 1; i < mFrequencyBands.size(); ++i)
   {
      minFreq = std::min(minFreq, mFrequencyBands.at(i).mLowerFrequency);
      maxFreq = std::max(maxFreq, mFrequencyBands.at(i).mUpperFrequency);
   }
   double frequency = 0.5 * (minFreq + maxFreq);
   SetFrequency(frequency);

   // set receiver bandwidth to frequency band range
   double bandwidth = maxFreq - minFreq;
   SetBandwidth(bandwidth);

   mHavePSOS_FrequencyData = ((mFrequencyBands.at(0).mDwellTime != 0.0) && (mFrequencyBands.at(0).mRevisitTime != 0.0));
   for (const FrequencyBand& band : mFrequencyBands)
   {
      if ((band.mDwellTime == 0.0) && (band.mRevisitTime == 0.0))
      {
         if (mHavePSOS_FrequencyData)
         {
            ut::log::error() << "Must specify 'dwell_time' and 'revisit_time' on all bands if used on any band.";
            return false;
         }
      }
      else if ((band.mDwellTime != 0.0) && (band.mRevisitTime != 0.0))
      {
         if (!mHavePSOS_FrequencyData)
         {
            ut::log::error() << "Must specify 'dwell_time' and 'revisit_time' on all bands if used on any band.";
         }
      }
      else
      {
         ut::log::error() << "Must specify both 'dwell_time' and 'revisit_time' if either is used.";
         return false;
      }
   }
   return true;
}

// =================================================================================================
//! Compute the received power from an emission direction, taking into account the polarization and
//! frequency of the incoming signal for antenna gain. Does NOT take into account bandwidth mismatch
//! between the receiver and signal.
//! @param aSourceAz             [input] Source azimuth with respect to the beam (radians)
//! @param aSourceEl             [input] Source elevation with respect to the beam (radians)
//! @param aEBS_Az               [input] Beam azimuth with respect to the antenna (radians)
//! @param aEBS_El               [input] Beam elevation with respect to the antenna (radians)
//! @param aPolarization         [input] Polarization of the signal being received.
//! @param aFrequency            [input] Frequency of the signal being received (Hertz).
//! @param aReceivedPowerDensity [input] Power density of the signal at the face of the antenna (w/m^2).
//! @param aAntennaGain          [output] Computed antenna gain (absolute - not dB).
//! @return The received power in watts.
//!
//! @note This method uses the transmitter center frequency as the center frequency as opposed
//!       to the base class implementation which uses the receiver center frequency.
//!
//! @note This method does not apply polarization effects because the polarization of the signal
//!       may have changed due to reflection. The application of these effects is left
//!       to the caller.
//! @note This method does not apply bandwidth effects because the bandwidth of the signal
//!       may have changed due to reflection. The application of these effects is left
//!       to the caller.
// virtual
double WsfPassiveSensor::PassiveRcvr::ComputeReceivedPower(double                    aSourceAz,
                                                           double                    aSourceEl,
                                                           double                    aEBS_Az,
                                                           double                    aEBS_El,
                                                           double                    aReceivedPowerDensity,
                                                           WsfEM_Types::Polarization aPolarization,
                                                           double                    aFrequency,
                                                           double&                   aAntennaGain)
{
   // Compute the antenna gain.
   aAntennaGain = GetAntennaGain(aPolarization, aFrequency, aSourceAz, aSourceEl, aEBS_Az, aEBS_El);

   double wavelength    = UtMath::cLIGHT_SPEED / aFrequency;
   double temp1         = wavelength * wavelength / UtMath::cFOUR_PI;
   double receivedPower = aReceivedPowerDensity * temp1 * aAntennaGain / mInternalLoss;
   return receivedPower;
}

// =================================================================================================
// Nested class WsfPassiveSensor::PassiveBeam.
// =================================================================================================

// =================================================================================================
WsfPassiveSensor::PassiveBeam::PassiveBeam()
   : WsfSensorBeam()
   , mAntennaPtr(new WsfEM_Antenna)
   , mRcvrPtr(new PassiveRcvr(mAntennaPtr))
   , mDetector()
{
   mRcvrPtr->SetDetectionThreshold(0.0);
}

// =================================================================================================
WsfPassiveSensor::PassiveBeam::PassiveBeam(const PassiveBeam& aSrc)
   : WsfSensorBeam(aSrc)
   , mAntennaPtr(new WsfEM_Antenna(*aSrc.mAntennaPtr))
   , mRcvrPtr(new PassiveRcvr(*aSrc.mRcvrPtr, mAntennaPtr))
   , mDetector(aSrc.mDetector)
{
}

WsfPassiveSensor::PassiveBeam& WsfPassiveSensor::PassiveBeam::operator=(const PassiveBeam& aRhs)
{
   if (this != &aRhs)
   {
      WsfSensorBeam::operator=(aRhs);
      mAntennaPtr            = new WsfEM_Antenna(*aRhs.mAntennaPtr);
      mRcvrPtr               = new PassiveRcvr(*aRhs.mRcvrPtr, mAntennaPtr);
      mDetector              = aRhs.mDetector;
   }
   return *this;
}

// =================================================================================================
bool WsfPassiveSensor::PassiveBeam::ProcessInput(UtInput& aInput)
{
   // Note: The base class receiver has a 'detection_threshold' specification, but we also have one.
   //       If the command occurs outside a 'receiver/end_receiver' block we see it directly, but if
   //       it appears inside the block we won't see it. We could flat-out disable it in the receiver
   //       but this could cause errors in user input files. For now we'll detect if it gets set in
   //       the block and then propagate it to our local values.

   bool myCommand = true;

   if (mDetector.ProcessInput(aInput)) // Must be first - see note above
   {
   }
   else if (mAntennaPtr->ProcessInput(aInput))
   {
   }
   else if (mRcvrPtr->ProcessInput(aInput) || mRcvrPtr->ProcessInputBlock(aInput))
   {
      // See comment at the start of the method...
      if (mRcvrPtr->GetDetectionThreshold() != 0.0) // Receiver detection threshold changed
      {
         mDetector.SetDetectionThreshold(WsfPassiveDetector::cST_CONTINUOUS, 0.0, mRcvrPtr->GetDetectionThreshold());
         mDetector.SetDetectionThreshold(WsfPassiveDetector::cST_PULSED, 0.0, mRcvrPtr->GetDetectionThreshold());
         mRcvrPtr->SetDetectionThreshold(0.0); // Reset to detect another possible change
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
bool WsfPassiveSensor::PassiveBeam::Initialize(WsfSensor* aSensorPtr, PassiveMode* aModePtr)
{
   bool ok = true;

   ok &= mAntennaPtr->Initialize(aSensorPtr);
   ok &= mRcvrPtr->Initialize(*aSensorPtr->GetSimulation());
   ok &= mDetector.Initialize();

   mRcvrPtr->SetDebugEnabled(aSensorPtr->DebugEnabled()); // Propagate the debug flag

   return ok;
}

// =================================================================================================
// Helper function for mapping the PassiveSensor SignalType to the PassiveDetector SignalType.
// (Provided so External Services wouldn't have to be changed).
namespace
{

inline WsfPassiveDetector::SignalType MapSignalType(WsfPassiveSensor::SignalType aSignalType)
{
   WsfPassiveDetector::SignalType signalType = WsfPassiveDetector::cST_CONTINUOUS;
   if (aSignalType == WsfPassiveSensor::cST_PULSED)
   {
      signalType = WsfPassiveDetector::cST_PULSED;
   }
   return signalType;
}
} // namespace

// =================================================================================================
//! Gets the count of frequency-threshold entries.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @returns the count of frequency-threshold entries.
size_t WsfPassiveSensor::PassiveBeam::GetDetectionThresholdCount(SignalType aSignalType) const
{
   return mDetector.GetDetectionThresholdCount(MapSignalType(aSignalType));
}

// =================================================================================================
//! Gets the data in a frequency-threshold entry.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @param aIndex The index of the entry.
//! @param aFrequency The frequency in the entry.
//! @param aThreshold The threshold value in the entry.
void WsfPassiveSensor::PassiveBeam::GetDetectionThresholdEntry(SignalType aSignalType,
                                                               size_t     aIndex,
                                                               double&    aFrequency,
                                                               double&    aThreshold) const
{
   mDetector.GetDetectionThresholdEntry(MapSignalType(aSignalType), aIndex, aFrequency, aThreshold);
}

// =================================================================================================
//! Gets the count of frequency-sensitivity entries.
//!
//! @param aSignalType Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @returns the count of frequency-threshold entries.
size_t WsfPassiveSensor::PassiveBeam::GetDetectionSensitivityCount(SignalType aSignalType) const
{
   return mDetector.GetDetectionSensitivityCount(MapSignalType(aSignalType));
}

// =================================================================================================
//! Gets the data in a frequency-sensitivity entry.
//!
//! @param aSignalType  Type of signal (continuous (cST_CONTINUOUS) or pulsed (cST_PULSED)).
//! @param aIndex       The index of the entry.
//! @param aFrequency   The frequency in the entry.
//! @param aSensitivity The sensitivity value in the entry.
void WsfPassiveSensor::PassiveBeam::GetDetectionSensitivityEntry(SignalType aSignalType,
                                                                 size_t     aIndex,
                                                                 double&    aFrequency,
                                                                 double&    aSensitivity) const
{
   mDetector.GetDetectionSensitivityEntry(MapSignalType(aSignalType), aIndex, aFrequency, aSensitivity);
}

// =================================================================================================
// Function developed to be called from script that would allow a user to change a Frequency Band's
// information or to add a new Frequency Band to an existing mode->Beam->rcvr
bool WsfPassiveSensor::PassiveBeam::TuneFrequencyBand(double       aSimTime,
                                                      bool         aReset,
                                                      unsigned int aFrequencyBandNumber,
                                                      double       aLowerFreq,
                                                      double       aUpperFreq,
                                                      double       aDwellTime,
                                                      double       aRevisitTime)
{
   return mRcvrPtr->TuneFrequencyBand(aSimTime, aReset, aFrequencyBandNumber, aLowerFreq, aUpperFreq, aDwellTime, aRevisitTime);
}

// =================================================================================================
//! @param aSimTime           The current simulation time.
//! @param aXmtrPtr           Pointer to the transmitter that is attempting to be detected.
//! @param aResult            The structure containing the results of the interaction.
//! @param aAuxInteractionPtr An auxiliary interaction that is attempting to be detected.
//!                           This will be zero if there is not an auxiliary interaction.
//! @param aPSOS_Enabled      'true' if 'Probabilistic Scan-On-Scan' processing is enabled.
//! @param aInteractorPtr     Pointer to the 'Interactor' object. This is only used if PSOS is enabled.
bool WsfPassiveSensor::PassiveBeam::AttemptToDetect(double             aSimTime,
                                                    WsfEM_Xmtr*        aXmtrPtr,
                                                    WsfSensorResult&   aResult,
                                                    WsfEM_Interaction* aAuxInteractionPtr,
                                                    bool               aPSOS_Enabled,
                                                    Interactor*        aInteractorPtr)
{
   // Perform 'Probabilistic Scan-On-Scan' (PSOS) processing if it is 'globally' enabled
   // (i.e.: running a normal time-based simulation vs. something like sensor_plot) AND
   // the receiver has PSOS enabled.

   bool computePSOS_Effects = aPSOS_Enabled && mRcvrPtr->mPSOS_Enabled;

   // NOTE: For the time being, 'listener' type interactions are not processed. For comm interactions
   // especially, it may be difficult to accumulate enough Pd for detection.

   if ((aInteractorPtr != nullptr) && (aInteractorPtr->mUseListener))
   {
      computePSOS_Effects = false;
   }

   bool detected = false;

   if (aResult.BeginOneWayInteraction(aXmtrPtr, mRcvrPtr, false, true) == 0)
   {
      aResult.SetReceiverBeamPosition();
      if (aAuxInteractionPtr == nullptr)
      {
         aResult.SetTransmitterBeamPosition();
      }
      else
      {
         // The geometry for the transmitter beam may not have been computed (which would be the case if
         // the Begin<xxx>Interaction was never performed (Doppler constraints) or not fully completed
         // (geometry constraints).  The call to ComputeUndefinedGeometry and SetTransmitterBeamPosition
         // addresses this problem.
         if (aAuxInteractionPtr->mXmtrBeam.mGain < 0.0)
         {
            aAuxInteractionPtr->ComputeUndefinedGeometry();
            aAuxInteractionPtr->SetTransmitterBeamPosition();
         }
         aResult.SetTransmitterBeamPosition(aAuxInteractionPtr->mXmtrBeam);
      }

      // Determine the amount of received power from the specified transmitter.
      aResult.ComputeRF_OneWayPower();

      // The effect of interferers on passive sensors is are not really clear. On one hand, a interferer is just another
      // source of energy to be detected by the passive sensor (a passive sensor is used to detect the frequency of a
      // interferer so the operator can decide which frequency to interfere), but on the other hand, a interferer
      // could inject energy that would affect the ability of the passive sensor to detect some other transmitter
      // (think of somebody shouting in your ear while you are trying to listen to somebody else).

      // TODO - compute interfere effects and apply

      // Compute the total effective signal-to-noise ratio at the output of the receiver.
      aResult.mSignalToNoise =
         mRcvrPtr->ComputeSignalToNoise(aResult.mRcvdPower, aResult.mClutterPower, aResult.mInterferencePower);

      // Determine if the signal can be detected.
      // This updates aResult.mPd and aResult.mDetectionThreshold.
      mDetector.ComputeDetectability(aResult);

      // Perform 'Probabilistic Scan-On-Scan' (PSOS) processing if it is 'globally' enabled
      // (i.e.: running a normal time-based simulation vs. something like sensor_plot) AND
      // the receiver has PSOS enabled.
      if (computePSOS_Effects)
      {
         detected = ComputePSOS_Effects(aSimTime, aXmtrPtr, aResult, aInteractorPtr);
      }
      else
      {
         // Check Pd vs. required Pd. Terrain masking is checked only if the Pd check is satisfied.
         aResult.mCheckedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
         if (aResult.mPd >= aResult.mRequiredPd)
         {
            detected = (!aResult.MaskedByTerrain());
         }
         else
         {
            aResult.mFailedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
            detected = false;
         }
      }

      if (DEBUG_PASSIVE_SENSOR)
      {
         auto out = ut::log::debug() << "Passive Sensor Detection Data:";
         out.AddNote() << "Transmitter Frequency: " << aXmtrPtr->GetFrequency() << " Hz";
         out.AddNote() << "Received Power: " << UtMath::LinearToDB(aResult.mRcvdPower) + 30 << " dBm";
         out.AddNote() << "Noise power: " << UtMath::LinearToDB(mRcvrPtr->GetNoisePower() + 30) << " dBm";
         out.AddNote() << "S/N: " << UtMath::LinearToDB(aResult.mSignalToNoise) << " dB";
         out.AddNote() << "Detection Threshold: " << UtMath::LinearToDB(aResult.mDetectionThreshold) << " dB";
         out.AddNote() << "Detection Sensitivity: "
                       << UtMath::LinearToDB(aResult.mDetectionThreshold * mRcvrPtr->GetNoisePower()) + 30 << " dBm";
      }
   }
   else if (computePSOS_Effects && (aInteractorPtr != nullptr))
   {
      ProcessFailedPSOS_Detection(aSimTime, aInteractorPtr);
   }
   return detected;
}

// =================================================================================================
bool WsfPassiveSensor::PassiveBeam::ComputePSOS_Effects(double           aSimTime,
                                                        WsfEM_Xmtr*      aXmtrPtr,
                                                        WsfSensorResult& aResult,
                                                        Interactor*      aInteractorPtr)
{
   if (mRcvrPtr->mPSOS_DebugEnabled)
   {
      auto out = ut::log::debug() << "Computing passive beam PSOS effects.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mRcvrPtr->GetPlatform()->GetName();
      out.AddNote() << "EM_Xmtr: " << aXmtrPtr->GetPlatform()->GetName();
   }

   double frequency = aXmtrPtr->GetFrequency();

   // Get the raw detection sensitivity.
   double S_req = aResult.mDetectionThreshold * mRcvrPtr->GetNoisePower();

   // Compute the received signal strength as if it had been transmitted through perfectly
   // isotropic antenna (i.e.: a gain of 1 everywhere).

   double S_iso = aResult.mRcvdPower / aResult.mXmtrBeam.mGain;

   // Determine the transmit antenna gain required GIVEN THE CURRENT GEOMETRY that produces a
   // received signal strength that reaches the sensitivity threshold.

   double G_req = S_req / S_iso;

   // Compute the probability of overlap in azimuth angle.
   //
   // This computation is bypassed if the transmit antenna does NOT scan in azimuth.
   // It also ignores the fact that the passive sensor antenna does NOT scan at all.

   double PA = 1.0;

   WsfEM_Antenna*          antennaPtr = aXmtrPtr->GetAntenna();
   WsfEM_Antenna::ScanMode scanMode   = antennaPtr->GetScanMode();
   WsfAntennaPattern* patternPtr = aXmtrPtr->GetAntennaPattern(aXmtrPtr->GetPolarization(), aXmtrPtr->GetFrequency());
   if ((patternPtr != nullptr) && ((scanMode == WsfEM_Antenna::cSCAN_AZ) || (scanMode == WsfEM_Antenna::cSCAN_AZ_EL)))
   {
      // Get the peak gain from the transmit antenna and get determine the fraction of the pattern
      // that exceeds the gain requirement.
      //
      // Note: The peak gain is the peak gain AT THE CURRENT ELEVATION ANGLE (not the peak gain of
      //       the whole pattern. This is assumed to be at az=0.

      double G_max = aXmtrPtr->GetAntennaGain(aXmtrPtr->GetPolarization(),
                                              frequency,
                                              0.0,
                                              aResult.mXmtrBeam.mEl,
                                              0.0,
                                              aResult.mXmtrBeam.mEBS_El);

      // Determine the extents of the antenna pattern to be processed.
      // Note: There may be wrap-around if the receiver is outside the scanning limits
      //       of the transmitter.
      double minAz1;
      double maxAz1;
      double minAz2 = 1.0; // Assume no wrap-around
      double maxAz2 = -1.0;
      antennaPtr->GetAzimuthScanLimits(minAz1, maxAz1);
      if ((aResult.mXmtrBeam.mGain > 0.0) && // gain has been determined so angle is valid
          (aResult.mXmtrBeam.mAz != 0.0))    // couldn't point at target, so must be outside scan volume
      {
         // aResult.mXmtrBeam.mAz represent the best the positioning logic can do (it is positioned at
         // either the minimum or maximum stop). Compute the part of the scan pattern that would be
         // exposed as the transmitter antenna was performing its scan.

         double azScan = maxAz1 - minAz1;
         if (aResult.mXmtrBeam.mAz > 0.0)
         {
            // The receiver is to the right of the transmitter.
            minAz1 = aResult.mXmtrBeam.mAz; // Position from the right limit
            maxAz1 = minAz1 + azScan;
            if (maxAz1 > (UtMath::cPI + 0.00001))
            {
               minAz2 = -UtMath::cPI;
               maxAz2 = maxAz1 - UtMath::cTWO_PI;
               maxAz1 = UtMath::cPI;
            }
         }
         else
         {
            // The receiver is to the left of the transmitter
            maxAz1 = aResult.mXmtrBeam.mAz; // Position from the left limit
            minAz1 = maxAz1 - azScan;
            if (minAz1 < -(UtMath::cPI + 0.00001))
            {
               minAz2 = minAz1 + UtMath::cTWO_PI;
               maxAz2 = UtMath::cPI;
               minAz1 = -UtMath::cPI;
            }
         }
      }

      PA = patternPtr->GetGainThresholdFraction(G_req, G_max, minAz1, maxAz1, frequency);
      if (minAz2 < maxAz2) // Process wrap-around
      {
         double PA2 = patternPtr->GetGainThresholdFraction(G_req, G_max, minAz2, maxAz2, frequency);
         PA += PA2;
      }
   }

   // Now compute the probability that we have an overlap in frequency.

   double PF         = 1.0; // Probability of frequency match
   double dwellCount = 1.0;
   if (mRcvrPtr->mHavePSOS_FrequencyData)
   {
      size_t i;
      for (i = 0; i < mRcvrPtr->mFrequencyBands.size(); ++i)
      {
         // Bands are sorted in increasing order of frequency.
         if (frequency < mRcvrPtr->mFrequencyBands.at(i).mUpperFrequency)
         {
            break;
         }
      }

      // This would only happen if the frequency was equal to the last frequency. The frequency should
      // never be greater because a transmitter would never be added as an interactor.
      if (i == mRcvrPtr->mFrequencyBands.size())
      {
         --i;
      }

      // PF = (T_d + T_cpi) / T_r, but we don't use T_cpi for now.
      PF = std::min(mRcvrPtr->mFrequencyBands.at(i).mDwellTime / mRcvrPtr->mFrequencyBands.at(i).mRevisitTime, 1.0);

      // Determine the average number of dwells that would occur in this band during a sampling frame.
      dwellCount = mRcvrPtr->mModePtr->GetFrameTime() / mRcvrPtr->mFrequencyBands.at(i).mRevisitTime;
   }

   // Compute the single-scan probability of detection ignoring terrain.
   double PSS = PA * PF;

   // If there is any probability of detection then terrain masking must be checked, even
   // though the cumulative Pd may not yet be sufficient. If it masked by terrain then it
   // is treated as though no probability exists.
   if ((PSS > 0.0) && (aResult.MaskedByTerrain()))
   {
      PSS = 0.0;
   }

   // Update the cumulative Pd.

   bool detected = false;
   aResult.mCheckedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
   if (PSS > 0.0)
   {
      // We may get several dwells within the sample (frame) time.
      double PSS_frame = 1.0 - pow(1.0 - PSS, dwellCount);

      // Merge the cumulative probability for this frame into the running cumulative probability
      double PD_cum                      = aInteractorPtr->mPSOS_CumulativePd;
      PD_cum                             = 1.0 - ((1.0 - PD_cum) * (1.0 - PSS_frame));
      aInteractorPtr->mPSOS_CumulativePd = PD_cum;

      // The PD for the detection attempt is the current cumulative Pd.
      // A detection will be declared if the Pd is above the required Pd.
      aResult.mPd = PD_cum;
      detected    = (aResult.mPd >= aResult.mRequiredPd);
   }
   else
   {
      // If there was no possibility for detection then the cumulative Pd is reduced.
      // If it falls below 0.5 then the cumulative Pd is reset
      aResult.mFailedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
      aResult.mPd = 0.0;
      ProcessFailedPSOS_Detection(aSimTime, aInteractorPtr);
   }

   if (mRcvrPtr->mPSOS_DebugEnabled)
   {
      auto out = ut::log::debug() << "PSOS Data:";
      out.AddNote() << "PA: " << PA;
      out.AddNote() << "PF: " << PF;
      out.AddNote() << "PSS: " << PSS;
      out.AddNote() << "DC: " << dwellCount;
      out.AddNote() << "PSS(frame): " << 1.0 - pow(1.0 - PSS, dwellCount);
      out.AddNote() << "PD_CumalativePd: " << aInteractorPtr->mPSOS_CumulativePd;
      out.AddNote() << "PD: " << aResult.mPd;
   }
   return detected;
}

// =================================================================================================
//! Process a failed PSOS detection attempt.
//! This routine is called whenever PSOS is enabled and a detection attempt determines there is
//! absolutely no probability of a detection.
void WsfPassiveSensor::PassiveBeam::ProcessFailedPSOS_Detection(double aSimTime, Interactor* aInteractorPtr)
{
   // Degrade the cumulative Pd.
   aInteractorPtr->mPSOS_CumulativePd *= 0.8;

   // Reset the cumulative Pd if it drops below 0.5, forcing it to re-accumulate.
   if (aInteractorPtr->mPSOS_CumulativePd < 0.5)
   {
      aInteractorPtr->mPSOS_CumulativePd = 0.0;
   }
}

WsfPassiveSensor::PassiveBeam::~PassiveBeam()
{
   delete mAntennaPtr;
   delete mRcvrPtr;
}

// =================================================================================================
// Nested class WsfPassiveSensor::PassiveMode.
// =================================================================================================

// =================================================================================================
WsfPassiveSensor::PassiveMode::PassiveMode()
   : WsfSensorMode()
   , mUnframedDetectionCoastTime(2.0)
   , mRangingTime(0.0)
   , mRangingTimeTrackQuality(0.0)
   , mImplicitBeamUsed(false)
   , mExplicitBeamUsed(false)
   , mUnframedDetectionOptimization(true)
   , mPSOS_Enabled(true)
   , mBeamList()
   , mTargetInteractorMap()
   , mMutex()
{
   SetErrorModel(ut::make_unique<PassiveErrorModel>());
   mBeamList.emplace_back();
   mBeamList.back().SetBeamOwner(this);
}

// =================================================================================================
WsfPassiveSensor::PassiveMode::PassiveMode(const PassiveMode& aSrc)
   : WsfSensorMode(aSrc)
   , mUnframedDetectionCoastTime(aSrc.mUnframedDetectionCoastTime)
   , mRangingTime(aSrc.mRangingTime)
   , mRangingTimeTrackQuality(aSrc.mRangingTimeTrackQuality)
   , mImplicitBeamUsed(aSrc.mImplicitBeamUsed)
   , mExplicitBeamUsed(aSrc.mExplicitBeamUsed)
   , mUnframedDetectionOptimization(aSrc.mUnframedDetectionOptimization)
   , mPSOS_Enabled(aSrc.mPSOS_Enabled)
   , mBeamList(aSrc.mBeamList)
   , mTargetInteractorMap(aSrc.mTargetInteractorMap)
   , mMutex()
{
}

WsfPassiveSensor::PassiveMode& WsfPassiveSensor::PassiveMode::operator=(const PassiveMode& aRhs)
{
   if (this != &aRhs)
   {
      WsfSensorMode::operator        =(aRhs);
      mUnframedDetectionCoastTime    = aRhs.mUnframedDetectionCoastTime;
      mRangingTime                   = aRhs.mRangingTime;
      mRangingTimeTrackQuality       = aRhs.mRangingTimeTrackQuality;
      mImplicitBeamUsed              = aRhs.mImplicitBeamUsed;
      mExplicitBeamUsed              = aRhs.mExplicitBeamUsed;
      mUnframedDetectionOptimization = aRhs.mUnframedDetectionOptimization;
      mPSOS_Enabled                  = aRhs.mPSOS_Enabled;
      mBeamList                      = aRhs.mBeamList;
      mTargetInteractorMap           = aRhs.mTargetInteractorMap;
      // mMutex
   }
   return *this;
}

// =================================================================================================
WsfPassiveSensor::PassiveMode::~PassiveMode()
{
   // Clean up the contents of the interactor entries.
   //
   // A simple destructor for the objects because the object is just a simple container that maintains
   // the item. The contents are maintained by external calls.

   for (auto& timi : mTargetInteractorMap)
   {
      InteractorList& interactorList = timi.second;
      for (Interactor& interactor : interactorList)
      {
         if (interactor.mUseListener)
         {
            interactor.mXmtrPtr->RemoveListener(interactor.mRcvrPtr);
         }

         if (interactor.mUseChangeListener)
         {
            interactor.mXmtrPtr->RemoveChangeListener(interactor.mRcvrPtr);
         }
         delete interactor.mResultPtr;
      }
   }
}

// =================================================================================================
// virtual
WsfMode* WsfPassiveSensor::PassiveMode::Clone() const
{
   return new PassiveMode(*this);
}

// =================================================================================================
// virtual
bool WsfPassiveSensor::PassiveMode::Initialize(double aSimTime)
{
   bool ok = WsfSensorMode::Initialize(aSimTime);

   if (GetFrameTime() <= 0.0)
   {
      ut::log::error() << "Unspecified frame_time.";
      ok = false;
   }

   for (unsigned int beamIndex = 0; beamIndex < mBeamList.size(); ++beamIndex)
   {
      PassiveBeam& beam = mBeamList[beamIndex];
      beam.mRcvrPtr->SetMode(this); // Identify the rcvr with the mode and beam index.
      beam.mRcvrPtr->SetIndex(beamIndex);
      ok &= beam.Initialize(mSensorPtr, this);
      mMaximumRange = beam.mAntennaPtr->GetMaximumRange();
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfPassiveSensor::PassiveMode::ProcessInput(UtInput& aInput)
{
   // Note: The base class receiver has a 'detection_threshold' specification, but we also have one.
   //       If the command occurs outside a 'receiver/end_receiver' block we see it directly, but if
   //       it appears inside the block we won't see it. We could flat-out disable it in the receiver
   //       but this could cause errors in user input files. For now we'll detect if it gets set in
   //       the block and then propagate it to our local values.

   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "beam")
   {
      if (mImplicitBeamUsed)
      {
         throw UtInput::BadValue(aInput, "'beam' cannot be used in this context");
      }
      mExplicitBeamUsed = true;
      UtInputBlock inputBlock(aInput);

      // Beam 1 is implicitly created.  Subsequent beams are created by using a beam number
      // that is one higher than the last beam number. The initial values for the beam
      // are copied from the first beam.

      int beamNumber;
      aInput.ReadValue(beamNumber);
      aInput.ValueInClosedRange(beamNumber, 1, static_cast<int>(mBeamList.size() + 2));
      auto beamIndex = static_cast<size_t>(beamNumber - 1);
      if (beamIndex == mBeamList.size())
      {
         PassiveBeam beam(mBeamList.at(0));
         beam.SetBeamIndex(beamIndex);
         beam.mRcvrPtr->mFrequencyBands.clear();
         mBeamList.push_back(beam);
      }

      while (inputBlock.ReadCommand())
      {
         if (!mBeamList[beamIndex].ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (mBeamList.at(0).ProcessInput(aInput))
   {
      if (mExplicitBeamUsed)
      {
         throw UtInput::BadValue(aInput, "Implicit 'beam' commands cannot be used if an explicit 'beam' has been defined");
      }
      mImplicitBeamUsed = true;
   }
   else if (command == "unframed_detection_coast_time")
   {
      aInput.ReadValueOfType(mUnframedDetectionCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mUnframedDetectionCoastTime, 0.0);
   }
   else if (command == "unframed_detection_optimization")
   {
      aInput.ReadValue(mUnframedDetectionOptimization);
   }
   else if (command == "ranging_time")
   {
      aInput.ReadValueOfType(mRangingTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mRangingTime, 0.0);
   }
   else if (command == "ranging_time_track_quality")
   {
      aInput.ReadValue(mRangingTimeTrackQuality);
      aInput.ValueGreaterOrEqual(mRangingTimeTrackQuality, 0.0);
   }
   else
   {
      myCommand = WsfSensorMode::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfPassiveSensor::PassiveMode::AttemptToDetect(double           aSimTime,
                                                    WsfPlatform*     aTargetPtr,
                                                    Settings&        aSettings,
                                                    WsfSensorResult& aResult)
{
   bool targetDetected = false;

   aResult.Reset(aSettings);
   aResult.SetCategory(GetSensor()->GetZoneAttenuationModifier());
   auto* sensorPtr = static_cast<WsfPassiveSensor*>(GetSensor());

   // Probabilistic Scan-On-Scan (PSOS) processing is enabled if this is called when 'Update()' is in control
   // (as is the case when a typical simulation is running). PSOS is disabled for direct calls (e.g.: from
   // sensor_plot).

   mPSOS_Enabled = sensorPtr->mUpdateActive;

   // Locate the list of transmitters on the target platform that can potentially be detected by this mode.
   auto timi = mTargetInteractorMap.find(aTargetPtr->GetIndex());

   // Exit quickly if no potential interactions exist.
   if (timi == mTargetInteractorMap.end())
   {
      return false;
   }

   if (sensorPtr->DebugEnabled())
   {
      auto out = ut::log::debug() << "Attempting to detect target via polling.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Mode: " << GetName();
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   sensorPtr->UpdatePosition(aSimTime); // Ensure my position is current

   // Clear the list of signals detected. This list is used in UpdateTrack();
   sensorPtr->mSignalList.clear();

   WsfSensorResult        beamResult;
   WsfSensorResult        xmtrResult;
   bool                   xmtrDetected         = false;
   bool                   firstResultForXmtr   = true;
   bool                   firstResultForTarget = true;
   InteractorList&        interactorList       = timi->second;
   auto                   ili                  = interactorList.begin();
   auto                   interactorListEnd    = interactorList.end();
   std::set<unsigned int> detectedXmtrPartIds;
   while (ili != interactorListEnd)
   {
      Interactor&  interactor = *ili;
      WsfEM_Xmtr*  xmtrPtr    = interactor.mXmtrPtr;
      PassiveRcvr* rcvrPtr    = interactor.mRcvrPtr;
      size_t       beamIndex  = rcvrPtr->GetIndex();

      bool beamDetected = false;
      if (interactor.mUseListener)
      {
         // Listener detection method is being used for this xmtr-rcvr pair. Use results captured by the
         // listener interface if they are available, otherwise report no detection.

         if (xmtrPtr->GetFunction() == WsfEM_Xmtr::cXF_INTERFERER)
         {
            ComputeInterferenceEffect(aSimTime, xmtrPtr, rcvrPtr);
         }

         // Another little quirk to allow a passive sensor to be plotted for detections against a tracking radar.
         // In this case the tracking radar will never have it's attempt to detect method called because it is
         // the target. If we get to this point and no attempt has been made and the sensor is cued, then
         // simulate a detection by position them beam and invoking the listeners.

         if ((xmtrPtr->GetFunction() == WsfEM_Xmtr::cXF_SENSOR) &&
             (interactor.mDetectStatus == Interactor::cDS_NOT_ATTEMPTED))
         {
            if (xmtrPtr->GetArticulatedPart()->IsCued())
            {
               xmtrPtr->UpdatePosition(aSimTime);
               WsfEM_Interaction targetInteraction;
               if (targetInteraction.BeginOneWayInteraction(xmtrPtr, rcvrPtr, false, true) == 0)
               {
                  targetInteraction.SetReceiverBeamPosition();
                  targetInteraction.SetTransmitterBeamPosition();
                  xmtrPtr->NotifyListeners(aSimTime, targetInteraction);
               }
            }
         }

         // Assume no detection this frame.
         beamResult.Reset(aSettings);
         beamResult.mBeamIndex = beamIndex;
         if (interactor.mResultPtr != nullptr)
         {
            if (interactor.mDetectStatus == Interactor::cDS_DETECTED)
            {
               // A listener detection method picked-up this xmtr-rcvr pair during the current frame.
               beamDetected = true;
               beamResult   = *(interactor.mResultPtr);
            }
            else if (interactor.mDetectStatus == Interactor::cDS_NOT_ATTEMPTED)
            {
               // If an attempt was not performed during the current frame, we still may want to use the last result
               // if it isn't too old. The main reason for this is that if the passive sensor is running at a faster
               // frame time than the sensor it is trying to detect, the passive sensor may see it one frame and not the
               // next. This may cause the track to continuously get dropped and initiated.
               //
               // OR... If a comm device is transmitting a long message, the listener is only called once at
               // the start of the transmission. In such a case we want to report the result as long as the
               // transmission is active.

               bool useLastResult = false;
               if (xmtrPtr->GetFunction() == WsfEM_Xmtr::cXF_COMM)
               {
                  // For comm we want to see if the message was detectable AT ANY TIME SINCE THE LAST SAMPLE.
                  // If the transmission was active at the last sample but ended before this sample, we still
                  useLastResult = ((aSimTime - GetFrameTime()) <= xmtrPtr->GetTransmissionEndTime());
               }
               else
               {
                  useLastResult = ((aSimTime - interactor.mLastAttemptTime) <= mUnframedDetectionCoastTime);
               }
               if (useLastResult)
               {
                  if ((interactor.mResultPtr->mCheckedStatus != 0) && (interactor.mResultPtr->mFailedStatus == 0))
                  {
                     beamDetected = true;
                     beamResult   = *(interactor.mResultPtr);
                  }
               }
            }
            else
            {
               // Failed signal or geometry
               beamResult   = *(interactor.mResultPtr);
               beamDetected = (beamResult.mCheckedStatus != 0) && (beamResult.mFailedStatus == 0);
            }
         }
         // Reset the detection flag for the start of a new frame
         interactor.mDetectStatus = Interactor::cDS_NOT_ATTEMPTED;
      }
      else
      {
         // The polling detection method is being used for this xmtr-rcvr pair. Use the normal detection attempt.
         xmtrPtr->UpdatePosition(aSimTime); // Ensure the target's transmitter position is current
         beamResult.Reset(aSettings);
         beamResult.mBeamIndex = beamIndex;
         beamDetected =
            mBeamList[beamIndex].AttemptToDetect(aSimTime, xmtrPtr, beamResult, nullptr, mPSOS_Enabled, &interactor);
         beamDetected &= GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, beamResult);
         GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, beamResult);
         if (sensorPtr->DebugEnabled())
         {
            auto out = ut::log::debug() << "Xmtr-Rcvr Pair Data Dump:";
            out.AddNote() << "Beam: " << beamIndex + 1;
            out.AddNote() << "Target System: " << xmtrPtr->GetArticulatedPart()->GetName();
            if (xmtrPtr->GetMode() != nullptr)
            {
               out.AddNote() << "Mode: " << xmtrPtr->GetMode()->GetName();
            }
            out.AddNote() << "Index: " << xmtrPtr->GetIndex() + 1;
            beamResult.Print(out);
         }
      }

      if (firstResultForXmtr)
      {
         // This is the first detection attempt in this call for this transmitter - so it becomes the baseline.
         xmtrResult         = beamResult;
         xmtrDetected       = beamDetected;
         firstResultForXmtr = false;
      }
      else if (beamDetected)
      {
         // This is a second or subsequent receiver that has detected this transmitter in this call.
         // Overwrite the previous results if the new signal-to-noise is better.
         xmtrDetected = true;
         if (beamResult.mSignalToNoise > xmtrResult.mSignalToNoise)
         {
            xmtrResult = beamResult;
         }
      }

      // If signal data is being reported, save off the frequency limits of the signal,
      // and any other data that is also being reported.

      if (beamDetected && ReportsFrequency())
      {
         std::vector<WsfTrack::Signal>& signalList    = sensorPtr->mSignalList;
         double                         frequency     = xmtrPtr->GetFrequency();
         double                         halfBandwidth = 0.5 * xmtrPtr->GetBandwidth();
         WsfTrack::Signal               signal(frequency - halfBandwidth, frequency + halfBandwidth);

         signal.mEmitterTruthId = xmtrPtr->GetArticulatedPart()->GetTypeId();

         if (ReportsPulseWidth())
         {
            signal.mPulseWidth = xmtrPtr->GetPulseWidth();
         }

         if (ReportsPulseRepititionInterval())
         {
            signal.mPulseRepetitionInterval = xmtrPtr->GetPulseRepetitionInterval();
         }

         if (std::find(signalList.begin(), signalList.end(), signal) == signalList.end())
         {
            signalList.push_back(signal);
         }
      }

      // Check if the next interaction involves the same transmitter. If not, finish this one off...

      ++ili;
      if ((ili == interactorListEnd) || (ili->mXmtrPtr != xmtrPtr))
      {
         // If the transmitter was detected...update the EmitterReportState if the platform was not already reported
         unsigned int xmtrPartId = xmtrPtr->GetAntenna()->GetArticulatedPart()->GetUniqueId();
         if (xmtrDetected)
         {
            sensorPtr->mEmitterTypeReporting.AddEmitterReportState(aSimTime, xmtrPtr);
            detectedXmtrPartIds.insert(xmtrPartId);
         }
         else if (detectedXmtrPartIds.find(xmtrPartId) == detectedXmtrPartIds.end())
         {
            sensorPtr->mEmitterTypeReporting.RemoveEmitterReportState(aTargetPtr->GetIndex(), xmtrPartId);
         }

         // Update the total status of the detection attempt for this target.
         if (firstResultForTarget)
         {
            aResult              = xmtrResult;
            firstResultForTarget = false;
         }
         else if (xmtrDetected)
         {
            if (xmtrResult.mSignalToNoise > aResult.mSignalToNoise)
            {
               aResult = xmtrResult;
            }
         }
         xmtrDetected       = false;
         firstResultForXmtr = true;
      }
   }

   // Compute component effects.
   targetDetected = WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);

   mPSOS_Enabled = true; // Revert back to the default.
   return targetDetected;
}

// =================================================================================================
//! This is called from WsfRF_Repeater to determine if the repeater can detect the signal that
//! it is attempting to interfere.
bool WsfPassiveSensor::PassiveMode::CanDetect(double             aSimTime,
                                              WsfEM_Xmtr*        aXmtrPtr,
                                              WsfSensorResult&   aResult,
                                              WsfEM_Interaction& aAuxInteraction)
{
   bool canDetect = false;

   // The incoming interaction contains the transmitter we want to try to detect.
   WsfEM_Xmtr* xmtrPtr = aAuxInteraction.GetTransmitter();
   if (xmtrPtr == nullptr)
   {
      return false; // shouldn't happen, but just in case
   }

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Checking if sensor can detect transmitter.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Sensor Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Sensor Mode: " << GetName();
      out.AddNote() << "Transmitter Platform: " << xmtrPtr->GetPlatform()->GetName();
      out.AddNote() << "Transmitter Art. Part: " << xmtrPtr->GetArticulatedPart()->GetName();
      out.AddNote() << "Transmitter Mode: " << xmtrPtr->GetMode()->GetName();
   }

   // Modify the base slewing limits with the mode-specific slewing limits.
   UpdateSensorCueingLimits();

   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current

   Settings settings;
   settings.mModeIndex  = GetModeIndex();
   settings.mRequiredPd = WsfSensor::GetRequiredPd(this);

   aResult.Reset(settings);
   aResult.SetCategory(GetSensor()->GetZoneAttenuationModifier());
   aResult.mBeamIndex = 0;

   canDetect = mBeamList.at(0).AttemptToDetect(aSimTime, xmtrPtr, aResult, &aAuxInteraction, false, nullptr);

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Beam 1";
      out.AddNote() << "Target System: " << aXmtrPtr->GetArticulatedPart()->GetName();
      if (aXmtrPtr->GetMode() != nullptr)
      {
         out.AddNote() << "Mode: " << aXmtrPtr->GetMode()->GetName();
      }
      out.AddNote() << "Index: " << aXmtrPtr->GetIndex() + 1;
      aResult.Print(out);
   }

   if (mBeamList.size() > 1)
   {
      WsfSensorResult beamResult{aResult};
      for (unsigned int beamIndex = 1; beamIndex < mBeamList.size(); ++beamIndex)
      {
         // Perform the detection attempt
         beamResult.Reset(settings);
         beamResult.SetCategory(GetSensor()->GetZoneAttenuationModifier());
         beamResult.mBeamIndex = beamIndex;

         canDetect = mBeamList[beamIndex].AttemptToDetect(aSimTime, xmtrPtr, beamResult, &aAuxInteraction, false, nullptr);

         if (GetSensor()->DebugEnabled())
         {
            auto out = ut::log::debug() << "Beam " << (beamIndex + 1) << ":";
            out.AddNote() << "Target System: " << aXmtrPtr->GetArticulatedPart()->GetName();
            if (aXmtrPtr->GetMode() != nullptr)
            {
               out.AddNote() << "Mode: " << aXmtrPtr->GetMode()->GetName();
            }
            out.AddNote() << "Index: " << (aXmtrPtr->GetIndex() + 1);
            beamResult.Print(out);
         }

         if (beamResult.mSignalToNoise > aResult.mSignalToNoise)
         {
            aResult = beamResult;
         }
      }
   }
   return canDetect;
}

// =================================================================================================
// virtual
void WsfPassiveSensor::PassiveMode::Deselect(double aSimTime)
{
   for (auto& beam : mBeamList)
   {
      beam.mRcvrPtr->Deactivate();
   }
}

// =================================================================================================
// virtual
void WsfPassiveSensor::PassiveMode::Select(double aSimTime)
{
   // The new update interval does not take place until the next update.
   GetSensor()->SetUpdateInterval(GetFrameTime());

   for (auto& beam : mBeamList)
   {
      beam.mRcvrPtr->Activate();
   }
}

// =================================================================================================
//! @copydoc WsfSensorMode::UpdateTrack
//!
//! The overrides the base class method in order to populate the track with the additional data
//! provided by this sensor.
// virtual
void WsfPassiveSensor::PassiveMode::UpdateTrack(double           aSimTime,
                                                WsfTrack*        aTrackPtr,
                                                WsfPlatform*     aTargetPtr,
                                                WsfSensorResult& aResult)
{
   // This overrides the base class method.

   auto* sensorPtr = static_cast<WsfPassiveSensor*>(GetSensor());

   WsfSensorMode::UpdateTrack(aSimTime, aTrackPtr, aTargetPtr, aResult);

   // If 'reports_frequency', set the signal list for the detected signals.
   if (ReportsFrequency())
   {
      aTrackPtr->SetSignalList(sensorPtr->mSignalList);
   }

   // UpdateReportedEmitterType MUST come after WsfSensorMode::UpdateTrack(...) as this call updates the track
   // reporting flags for TypeIdValid as well as the track TypeId per the user inputs and will cause conflicts
   // if not in this order.

   // Determine the reported emitter type
   sensorPtr->mEmitterTypeReporting.UpdateReportedEmitterType(aSimTime, ReportsType(), aResult, aTargetPtr, aTrackPtr);

   // UpdateReportedTargetType MUST come after WsfSensorMode::UpdateTrack(...) and UpdateReportedEmitterType(...)
   // as this call updates the track reporting flags for TypeIdValid as well as the track TypeId per the user inputs
   // and will cause conflicts if not in this order.

   // Determine the reported target type
   sensorPtr->mTargetTypeReporting.UpdateReportedTargetType(aSimTime, ReportsType(), aResult, aTargetPtr, aTrackPtr);

   // The truth type is the truth type of the emitter and not the truth type of the platform containing the emitter.
   // This must be done AFTER the call to the base class method because it sets the default.

   if (aResult.GetTransmitter() != nullptr)
   {
      aTrackPtr->SetTargetType(aResult.GetTransmitter()->GetArticulatedPart()->GetTypeId());
   }
}

// =================================================================================================
//! @copydoc WsfSensorMode::UpdateTrackReportingFlags
//!
//! The overrides the base class method to set the track validity flags as appropriate.
// virtual
void WsfPassiveSensor::PassiveMode::UpdateTrackReportingFlags(double aSimTime, WsfTrack* aTrackPtr)
{
   WsfSensorMode::UpdateTrackReportingFlags(aSimTime, aTrackPtr); // Call the base class first

   aTrackPtr->SetTypeIdValid(false); // Set to false since this is updated by the UpdateTrack in the PassiveMode

   // If 'ranging_time' has been defined and if sufficient time has elapsed, then modify the environment
   // so that range will get reported.
   //
   // The ranging_time is a concept brought over from IWARS.  It basically simulates the fact that a passive
   // sensor system could triangulate and get the range after a sufficient period of time. Of course this really
   // depends on geometry, but this was good enough for them.  This will eventually be superseded.

   if (mRangingTime > 0.0)
   {
      bool rangeValid = (aSimTime - aTrackPtr->GetStartTime()) >= mRangingTime;
      aTrackPtr->SetRangeValid(rangeValid);
      if (rangeValid && (mRangingTimeTrackQuality > 0.0))
      {
         aTrackPtr->SetTrackQuality(mRangingTimeTrackQuality);
      }
   }
}

// =================================================================================================
//! Pass-through from WsfPassiveSensor::PassiveRcvr::AddInteractor.
//!
//! Each target platform has a interactor list, which is just a collection of pairs of transmitters
//! on the target that can be potentially seen by the receiver(s) that are contained in this mode.
//! (Note that most of the time there is only one receiver in the mode, but the 'multi-beam' feature
//! lets one define total separate antennas/receivers, much like the multi-beam radar.)
//!
//! The pairs in the list are maintained so that all pairs involving a given transmitter are kept
//! together. This is needed by the PassiveMode::AttemptToDetect method.
bool WsfPassiveSensor::PassiveMode::AddInteractor(WsfEM_Xmtr* aXmtrPtr, PassiveRcvr* aRcvrPtr)
{
   // Utilize the 'listener' detection method for comm devices that transmit intermittently.
   // For all other devices the 'polling' detection method will be used.

   bool useListener       = false;
   bool useChangeListener = false;
   if (aXmtrPtr->GetFunction() == WsfEM_Xmtr::cXF_COMM)
   {
      auto* commPtr = dynamic_cast<wsf::comm::Comm*>(aXmtrPtr->GetArticulatedPart());
      if (commPtr)
      {
         auto* commHW_Ptr = wsf::comm::ComponentHW::Find(*commPtr);
         if ((commHW_Ptr) && (!commHW_Ptr->TransmitsContinuously()))
         {
            useListener = true;
         }
      }

      if (aXmtrPtr->GetAlternateFrequencyCount() > 0)
      {
         useChangeListener = true;
      }
   }
   else if (aXmtrPtr->GetFunction() == WsfEM_Xmtr::cXF_SENSOR)
   {
      auto* sensorPtr = dynamic_cast<WsfSensor*>(aXmtrPtr->GetArticulatedPart());
      auto* modePtr   = dynamic_cast<WsfSensorMode*>(aXmtrPtr->GetMode());
      if ((sensorPtr != nullptr) && (modePtr != nullptr))
      {
         if (modePtr->GetMaximumRequestCount() != 0)
         {
            useListener = true;
         }

         if ((aXmtrPtr->GetAlternateFrequencyCount() > 0) || (sensorPtr->GetModeCount() > 1))
         {
            useChangeListener = true;
         }
      }
   }
   else if (aXmtrPtr->GetFunction() == WsfEM_Xmtr::cXF_INTERFERER)
   {
      useListener = UseListenerForInterferer(aXmtrPtr);
   }

   size_t targetIndex = aXmtrPtr->GetPlatform()->GetIndex();
   auto   timi        = mTargetInteractorMap.find(targetIndex);
   if (timi != mTargetInteractorMap.end())
   {
      InteractorList& interactorList    = timi->second;
      auto            interactorListEnd = interactorList.end();
      auto            ili               = interactorList.begin();

      // We want to keep all the entries involving a given transmitter together (this is needed in
      // PassiveMode::AttemptToDetect so it can make classification decisions). So search through the
      // list to find the first entry with a matching transmitter (or the end of the list).

      while ((ili != interactorListEnd) && (ili->mXmtrPtr != aXmtrPtr))
      {
         ++ili;
      }

      // We've either an entry with a matching transmitter (or we've hit the end of the list).
      // Continue searching entries until a different transmitter is detected (or we hit the end of the list).
      // If an exact match is found we exit immediately so there are no duplicates.

      while ((ili != interactorListEnd) && (ili->mXmtrPtr == aXmtrPtr))
      {
         if (ili->mRcvrPtr == aRcvrPtr)
         {
            return false; // Exact match - already in the list
         }
         ++ili;
      }

      // Insert the new entry in the correct position.
      interactorList.insert(ili, Interactor(aXmtrPtr, aRcvrPtr, useListener, useChangeListener));
   }
   else
   {
      // Target does not yet exist in the map.
      InteractorList interactorList;
      interactorList.emplace_back(aXmtrPtr, aRcvrPtr, useListener, useChangeListener);
      mTargetInteractorMap.insert(std::make_pair(targetIndex, interactorList));
   }

   // If the listener interface should be used to detect intermittent signals then attach this
   // receiver to the transmitter as a listener.
   if (useListener)
   {
      aXmtrPtr->AddListener(aRcvrPtr);
   }

   // If the listener interface should be used to detect signal changes then attach this
   // receiver to the transmitter as a change listener.
   if (useChangeListener)
   {
      aXmtrPtr->AddChangeListener(aRcvrPtr);
   }

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Adding interactor between receiver and transmitter.";
      out.AddNote() << "T = " << GetSimulation()->GetSimTime();
      out.AddNote() << "Platform: " << GetSensor()->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Mode: " << GetName();
      out.AddNote() << "Receiver: " << aRcvrPtr->GetName();
      out.AddNote() << "Transmitter: " << aXmtrPtr->GetName();
      out.AddNote() << "Transmitter Platform: " << aXmtrPtr->GetPlatform()->GetName();
      out.AddNote() << "Transmitter Art. Part: " << aXmtrPtr->GetArticulatedPart()->GetName();
      out.AddNote() << "Use Listener: " << useListener;
      out.AddNote() << "Use Change Listener: " << useChangeListener;
   }
   return true; // Indicate entry was added.
}

// =================================================================================================
//! Pass-through from WsfPassiveSensor::PassiveRcvr::RemoveInteractor.
bool WsfPassiveSensor::PassiveMode::RemoveInteractor(WsfEM_Xmtr* aXmtrPtr, PassiveRcvr* aRcvrPtr)
{
   bool removed = false;

   size_t targetIndex = aXmtrPtr->GetPlatform()->GetIndex();
   auto   timi        = mTargetInteractorMap.find(targetIndex);
   if (timi != mTargetInteractorMap.end())
   {
      InteractorList& interactorList = timi->second;
      for (auto ili = interactorList.begin(); ili != interactorList.end(); ++ili)
      {
         if ((ili->mXmtrPtr == aXmtrPtr) && (ili->mRcvrPtr == aRcvrPtr))
         {
            // Found the right entry.

            delete ili->mResultPtr; // Delete any stored detection results

            // If the 'listener' interface was being used to detect intermittent transmissions,
            // indicate we are no longer listening.

            if (ili->mUseListener)
            {
               aXmtrPtr->RemoveListener(aRcvrPtr);
            }

            if (ili->mUseChangeListener)
            {
               aXmtrPtr->RemoveChangeListener(aRcvrPtr);
            }

            // Remove the transmitter/receiver pair from the interactor list. If the target has no
            // more interactors then remove the target entry from the map.

            auto sensorPtr = static_cast<WsfPassiveSensor*>(mSensorPtr);
            sensorPtr->mEmitterTypeReporting.RemoveEmitterReportState(ili->mXmtrPtr->GetPlatform()->GetIndex(),
                                                                      ili->mXmtrPtr->GetArticulatedPart()->GetUniqueId());
            interactorList.erase(ili);
            if (interactorList.empty())
            {
               mTargetInteractorMap.erase(timi);
            }
            removed = true;
            break;
         }
      }
   }

   if (removed && GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Removing interactor between receiver and transmitter.";
      out.AddNote() << "T = " << GetSimulation()->GetSimTime();
      out.AddNote() << "Platform: " << GetSensor()->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Mode: " << GetName();
      out.AddNote() << "Receiver: " << aRcvrPtr->GetName();
      out.AddNote() << "Transmitter: " << aXmtrPtr->GetName();
      out.AddNote() << "Transmitter Platform: " << aXmtrPtr->GetPlatform()->GetName();
      out.AddNote() << "Transmitter Art. Part:" << aXmtrPtr->GetArticulatedPart()->GetName();
   }
   return removed;
}

// =================================================================================================
//! Pass-through from WsfPassiveSensor::PassiveRcvr::EmitterActiveCallback
void WsfPassiveSensor::PassiveMode::EmitterActiveCallback(double aSimTime, WsfEM_Interaction& aResult, PassiveRcvr* aRcvrPtr)
{
   size_t beamIndex = aRcvrPtr->GetIndex();
   if (beamIndex > mBeamList.size())
   {
      return; // shouldn't happen, but just in case.
   }

   // The incoming interaction contains the transmitter we want to try to detect.
   WsfEM_Xmtr* xmtrPtr = aResult.GetTransmitter();
   if (xmtrPtr == nullptr)
   {
      return; // shouldn't happen, but just in case
   }

   std::lock_guard<std::recursive_mutex> lock(mMutex);

   // Locate the target containing the list of transmitters.
   size_t targetIndex = xmtrPtr->GetPlatform()->GetIndex();
   auto   timi        = mTargetInteractorMap.find(targetIndex);
   if (timi == mTargetInteractorMap.end())
   {
      return; // shouldn't happen, but just in case
   }

   // Locate the transmitter/receiver interaction state.
   InteractorList& interactorList = timi->second;
   Interactor*     interactorPtr  = nullptr;
   for (Interactor& interactor : interactorList)
   {
      if ((xmtrPtr == interactor.mXmtrPtr) && (aRcvrPtr == interactor.mRcvrPtr))
      {
         interactorPtr = &interactor;
         break;
      }
   }

   if (interactorPtr == nullptr)
   {
      return; // shouldn't happen, but just in case.
   }

   if (GetSensor()->DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "Sensor attempting to detect transmitter via listener";
      logDebug.AddNote() << "T = " << aSimTime;
      logDebug.AddNote() << "Platform: " << GetPlatform()->GetName();
      logDebug.AddNote() << "Mode: " << GetName();
      logDebug.AddNote() << "Transmitter Platform: " << xmtrPtr->GetPlatform()->GetName();
   }

   // If unframed detection optimization is enabled (the default), determine if a detection attempt needs
   // to be performed. The attempt will be suppressed if:
   //
   // -) The target transmitter has already been detected in the current frame.
   // -) A previous detection in the frame failed because there was no possibility of detection
   //    (i.e.: failed geometry limits, horizon or terrain masking).
   //
   // Detection optimization is a recognition that in most cases one is only interested if the transmitter
   // is detected, not by how much.

   if (mUnframedDetectionOptimization)
   {
      if ((interactorPtr->mDetectStatus == Interactor::cDS_DETECTED) ||
          (interactorPtr->mDetectStatus == Interactor::cDS_FAILED_GEOMETRY))
      {
         if (GetSensor()->DebugEnabled())
         {
            auto out = ut::log::debug() << "Using previous status for cycle: ";
            if (interactorPtr->mDetectStatus == Interactor::cDS_DETECTED)
            {
               out << "DETECTED";
            }
            else if (interactorPtr->mDetectStatus == Interactor::cDS_FAILED_GEOMETRY)
            {
               out << "FAILED_GEOMETRY";
            }
            else
            {
               out << "UNKNOWN";
            }
         }
         return;
      }
   }

   // Perform the detection attempt

   // Modify the base slewing limits with the mode-specific slewing limits.
   UpdateSensorCueingLimits();

   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current

   Settings        settings;
   WsfSensorResult result;
   settings.mModeIndex  = GetModeIndex();
   settings.mRequiredPd = WsfSensor::GetRequiredPd(this);

   result.Reset(settings);
   result.SetCategory(GetSensor()->GetZoneAttenuationModifier());
   result.mBeamIndex = beamIndex;

   bool detected = mBeamList[beamIndex].AttemptToDetect(aSimTime, xmtrPtr, result, &aResult, mPSOS_Enabled, interactorPtr);
   detected &= GetSensor()->ScriptAllowDetection(aSimTime, xmtrPtr->GetPlatform(), result);
   GetSensor()->NotifySensorDetectionAttempted(aSimTime, xmtrPtr->GetPlatform(), result);
   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Beam " << beamIndex + 1 << ":";
      out.AddNote() << "Target System: " << xmtrPtr->GetArticulatedPart()->GetName();
      if (xmtrPtr->GetMode() != nullptr)
      {
         out.AddNote() << "Mode: " << xmtrPtr->GetMode()->GetName();
      }
      out.AddNote() << "Index: " << xmtrPtr->GetIndex() + 1;
      result.Print(out);
   }

   if (detected)
   {
      // See WsfPassiveSensor::PassiveMode::ApplyMeasurementErrors for why it is called here...

      ApplyMeasurementErrors(result);
      if (interactorPtr->mResultPtr == nullptr)
      {
         // First detection ever for this xmtr/rcvr pair.
         interactorPtr->mResultPtr = new WsfSensorResult(result);
      }
      else if (interactorPtr->mDetectStatus != Interactor::cDS_DETECTED)
      {
         // First detection by this xmtr/rcvr pair during the current frame.
         *(interactorPtr->mResultPtr) = result;
      }
      else if (result.mSignalToNoise > interactorPtr->mResultPtr->mSignalToNoise)
      {
         // Another detection has occurred for this xmtr/rcvr pair during the current frame, and
         // the new results are 'better' (have a higher signal-to-noise) than the previous results.
         *(interactorPtr->mResultPtr) = result;
      }
      interactorPtr->mDetectStatus = Interactor::cDS_DETECTED;
   }
   else if (interactorPtr->mDetectStatus != Interactor::cDS_DETECTED)
   {
      // This section is a very important optimization. If the signal was not detected due to geometry or masking
      // constraints, then for a reasonably short time we can assume that the geometry of the xmtr/rcvr will
      // not change and that all subsequent attempts in the same frame can be eliminated because they also
      // would fail the check.
      //
      // For example, an interferer would end up calling this routine for EVERY interaction with a the receiver that it
      // is trying to interfere with. If us and the interferer are 'out-of-sight/out-of-mind' there is no reason for
      // each interference interaction to be checked. If, however, that constraint is not met, we must be called because
      // the interference transmitters could be pointed anywhere, including at us!
      //
      // This same argument applied to tracking radars.
      //
      // NOTE: The detection results will be stored if the interactor has a place to store them. This is useful
      // if SENSOR_DETECTION_ATTEMPT is enabled and one wants to see why the target became undetectable.

      // Default to capturing results if there is a place to store them.
      bool updateResults = (interactorPtr->mResultPtr != nullptr);

      if (((result.mCheckedStatus & WsfSensorResult::cSIGNAL_LEVEL) == 0) ||
          ((result.mFailedStatus & WsfSensorResult::cRCVR_TERRAIN_MASKING) != 0))
      {
         // We never made it to the signal level test or the xmtr/rcvr are masked by the terrain.
         // No further detection attempts for this pair during this frame.
         interactorPtr->mDetectStatus = Interactor::cDS_FAILED_GEOMETRY;
      }
      else
      {
         // The detector made it to the signal level check AND it didn't fail terrain masking (which is checked last).
         // This means that even though this attempt failed, another attempt within the same frame may succeed.
         //
         // By default the results are captured (if there is a place to store them), but this is suppressed if this
         // result is worse than the previous failed signal attempt IN THE SAME FRAME.
         if ((interactorPtr->mDetectStatus == Interactor::cDS_FAILED_SIGNAL) && (interactorPtr->mResultPtr != nullptr) &&
             (interactorPtr->mResultPtr->mSignalToNoise > result.mSignalToNoise))
         {
            updateResults = false;
         }
         interactorPtr->mDetectStatus = Interactor::cDS_FAILED_SIGNAL;
      }

      if (updateResults)
      {
         *(interactorPtr->mResultPtr) = result;
      }
   }
   interactorPtr->mLastAttemptTime = aSimTime;
}

// =================================================================================================
// Read 'azimuth/elevation/range _error_sigma_table
void WsfPassiveSensor::PassiveMode::PassiveErrorModel::ReadErrorTable(UtInput&           aInput,
                                                                      ErrorTable&        aErrorTable,
                                                                      UtInput::ValueType aValueType)
{
   std::string         tableType(aInput.GetCommand());
   std::string         command;
   std::vector<double> frequencies;
   std::vector<double> errorSigmas;
   UtInputBlock        inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "frequency")
      {
         double frequency;
         aInput.ReadValueOfType(frequency, UtInput::cFREQUENCY);
         aInput.ValueGreater(frequency, 0.0);
         frequency = log10(frequency);
         if ((!frequencies.empty()) && (frequency <= frequencies.back()))
         {
            throw UtInput::BadValue(aInput, "Entries must be in order of ascending frequency");
         }
         double      errorSigma;
         std::string units;
         aInput.ReadValue(errorSigma);
         aInput.ReadValue(units);
         if (units == "percent_of_true_range")
         {
            aInput.ValueGreater(errorSigma, 0.0);
            aInput.ValueLessOrEqual(errorSigma, 100.0);
            errorSigma *= -0.01;
         }
         else
         {
            errorSigma = aInput.ConvertValueFrom(errorSigma, units, aValueType);
            aInput.ValueGreater(errorSigma, 0.0);
         }
         if (!errorSigmas.empty())
         {
            if (((errorSigma < 0.0) && (errorSigmas.back() > 0.0)) || ((errorSigma > 0.0) && (errorSigmas.back() < 0.0)))
            {
               throw UtInput::BadValue(aInput, "Cannot intermix percent_of_true_range with other entries");
            }
         }
         frequencies.push_back(frequency);
         errorSigmas.push_back(errorSigma);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   if (frequencies.size() == 1)
   {
      throw UtInput::BadValue(aInput, "At least two entries must be given for " + tableType);
   }
   aErrorTable.mFrequency.SetValues(frequencies);
   aErrorTable.mErrorSigma.SetValues(errorSigmas);
}

// =================================================================================================
//! Select the frequency dependent error sigma if available.
//! @param aErrorTable The frequency-dependent error table (az, el or range).
//! @param aFrequency  The frequency of the received signal (Hz)
//! @param aErrorSigma On input this should be the reference to the mode base class error sigma,
//! which will be used if no table was defined. If a table was defined then the value will be updated
//! with the interpolated value from the table.
void WsfPassiveSensor::PassiveMode::PassiveErrorModel::SelectErrorSigma(const ErrorTable& aErrorTable,
                                                                        double            aFrequency,
                                                                        double&           aErrorSigma)
{
   if (aErrorTable.mFrequency.GetSize() >= 2)
   {
      // The table is errorStdDev .vs. log10(frequency)
      TblLookupLU<double> frequency;
      frequency.Lookup(aErrorTable.mFrequency, log10(std::max(aFrequency, 1.0E-37)));
      aErrorSigma = TblEvaluate(aErrorTable.mErrorSigma, frequency);
   }
}

// =================================================================================================
// Set, add or modify the frequency band parameters for a given beam or all beams.
bool WsfPassiveSensor::PassiveMode::TuneFrequencyBand(double       aSimTime,
                                                      bool         aReset,
                                                      size_t       aBeamNumber,
                                                      unsigned int aFrequencyBandNumber,
                                                      double       aLowerFreq,
                                                      double       aUpperFreq,
                                                      double       aDwellTime,
                                                      double       aRevisitTime)
{
   if (aBeamNumber == 0)
   {
      bool success(false);
      for (auto& beam : mBeamList)
      {
         success =
            beam.TuneFrequencyBand(aSimTime, aReset, aFrequencyBandNumber, aLowerFreq, aUpperFreq, aDwellTime, aRevisitTime);
      }
      return success;
   }
   else if (aBeamNumber <= mBeamList.size())
   {
      return mBeamList[aBeamNumber - 1]
         .TuneFrequencyBand(aSimTime, aReset, aFrequencyBandNumber, aLowerFreq, aUpperFreq, aDwellTime, aRevisitTime);
   }
   else
   {
      auto out = ut::log::error() << "Invalid beam number.";
      out.AddNote() << "Beam: " << aBeamNumber;
   }
   return false;
}

// =================================================================================================
// Nested class WsfPassiveSensor::Interactor
// =================================================================================================
//
// The use of this class may seem a bit redundant because the WsfEM_Rcvr already contains a list of
// transmitters with which the receiver could interact. If we didn't support multi-beam (receiver)
// systems and didn't support detection of transient signals then that would be sufficient. But this
// construct reduces some loops and facilitates the detection of transient signals.

// =================================================================================================
WsfPassiveSensor::Interactor::Interactor(WsfEM_Xmtr* aXmtrPtr, PassiveRcvr* aRcvrPtr, bool aUseListener, bool aUseChangeListener)
   : mXmtrPtr(aXmtrPtr)
   , mRcvrPtr(aRcvrPtr)
   , mResultPtr(nullptr)
   , mLastAttemptTime(0.0)
   , mPSOS_CumulativePd(0.0)
   , mDetectStatus(cDS_NOT_ATTEMPTED)
   , mUseListener(aUseListener)
   , mUseChangeListener(aUseChangeListener)
{
}

std::unique_ptr<UtScriptClass> WsfPassiveSensor::CreateScriptClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptPassiveSensorClass>(aClassName, aScriptTypesPtr);
}

WsfScriptPassiveSensorClass::WsfScriptPassiveSensorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptSensorClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfPassiveSensor");

   AddMethod(ut::make_unique<FrequencyBandCount_1>("FrequencyBandCount"));
   AddMethod(ut::make_unique<FrequencyBandCount_2>("FrequencyBandCount"));
   AddMethod(ut::make_unique<TuneFrequencyBand>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPassiveSensorClass, WsfPassiveSensor, FrequencyBandCount_1, 1, "int", "int")
{
   // Argument 0: Beam Number
   WsfStringId modeNameId("");
   aReturnVal.SetInt(aObjectPtr->GetNumberofFrequencyBands(modeNameId, aVarArgs[0].GetInt()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPassiveSensorClass, WsfPassiveSensor, FrequencyBandCount_2, 2, "int", "string, int")
{
   // Argument 0: Mode Name
   // Argument 1: Beam Number
   aReturnVal.SetInt(aObjectPtr->GetNumberofFrequencyBands(aVarArgs[0].GetString(), aVarArgs[1].GetInt()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPassiveSensorClass,
                        WsfPassiveSensor,
                        TuneFrequencyBand,
                        8,
                        "bool",
                        "bool, string, int, int, double, double, double, double")
{
   // Argument 0: Add/Reset Command
   // Argument 1: Mode Name
   // Argument 2: Beam Number
   // Argument 3: Freq Band Index
   // Argument 4: Lower Frequency Limit
   // Argument 5: Upper Frequency Limit
   // Argument 6: Dwell Time
   // Argument 7: Revisit

   double       simTime        = WsfScriptContext::GetTIME_NOW(aContext);
   bool         resetFlag      = aVarArgs[0].GetBool();
   WsfStringId  modeNameId     = aVarArgs[1].GetString();
   unsigned int beamNumber     = aVarArgs[2].GetInt();
   unsigned int bandNumber     = aVarArgs[3].GetInt();
   double       lowerFreqLimit = aVarArgs[4].GetDouble();
   double       upperFreqLimit = aVarArgs[5].GetDouble();
   double       dwellTime      = aVarArgs[6].GetDouble();
   double       revisitTime    = aVarArgs[7].GetDouble();

   bool ok = aObjectPtr->TuneFrequencyBand(simTime,
                                           resetFlag,
                                           modeNameId,
                                           beamNumber,
                                           bandNumber,
                                           lowerFreqLimit,
                                           upperFreqLimit,
                                           dwellTime,
                                           revisitTime);
   aReturnVal.SetBool(ok);
}

void WsfPassiveSensor::PassiveMode::PassiveErrorModel::ApplyMeasurementErrors(const wsf::SphericalMeasurementErrors& aErrors,
                                                                              WsfSensorResult& aResult)
{
   // This is just a front-end to WsfSensorMode::ApplyMeasurementErrors. It will bypass applying the
   // errors in the case where they have already been computed.
   //
   // In order to work properly, ApplyMeasurementErrors must be called at the same simulation time
   // as the detection attempt (this is because it uses the sensor orientation at the time of the
   // call to convert the sensor-relative az/el/range to a WCS location.
   //
   // Normally ApplyMeasurementErrors is called by WsfSensor::PerformScheduledDetections. This works
   // fine where the detection chance ACTUALLY occurs at the same time as the application of the
   // measurement errors (i.e.: for polling detections). But detections that occur in the listener
   // interface are cached and then processed during the NEXT call to PerformScheduledDetections. In
   // the mean time, the platform may have move or changed orientation (the later causes a bigger
   // problem).
   //
   // The solution is to apply the measurement errors in the listener interface and use those, and
   // prevent any attempt to reapply them when called from PerformScheduledDetections.

   if (!(aResult.mMeasurement.RangeValid() || aResult.mMeasurement.SensorAzimuthValid() ||
         aResult.mMeasurement.SensorElevationValid()))
   {
      StandardSensorErrorModel::ApplyMeasurementErrors(aErrors, aResult);
   }
}

std::unique_ptr<wsf::SphericalMeasurementErrors>
WsfPassiveSensor::PassiveMode::PassiveErrorModel::ComputeMeasurementErrors(WsfSensorResult& aResult) const
{
   if (!(aResult.mMeasurement.RangeValid() || aResult.mMeasurement.SensorAzimuthValid() ||
         aResult.mMeasurement.SensorElevationValid()))
   {
      Sigmas sigmas = GetSigmas();
      if (aResult.GetTransmitter() != nullptr)
      {
         double frequency = aResult.GetTransmitter()->GetFrequency();
         SelectErrorSigma(mAzErrorTable, frequency, sigmas.mAzErrorStdDev);
         SelectErrorSigma(mElErrorTable, frequency, sigmas.mElErrorStdDev);
         SelectErrorSigma(mRangeErrorTable, frequency, sigmas.mRangeErrorStdDev);
      }
      auto errors = GetSphericalErrorStdDev(GetSensorMode()->GetSensor()->GetRandom(), aResult, sigmas);

      // Invoke optional component models for this action.
      WsfSensorComponent::ComputeSphericalMeasurementErrors(*GetSensorMode()->GetSensor(), aResult, errors);
      return ut::make_unique<wsf::SphericalMeasurementErrors>(errors);
   }

   return ut::make_unique<wsf::SphericalMeasurementErrors>();
}

bool WsfPassiveSensor::PassiveMode::PassiveErrorModel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "azimuth_error_sigma_table")
   {
      ReadErrorTable(aInput, mAzErrorTable, UtInput::cANGLE);
   }
   else if (command == "elevation_error_sigma_table")
   {
      ReadErrorTable(aInput, mElErrorTable, UtInput::cANGLE);
   }
   else if (command == "range_error_sigma_table")
   {
      ReadErrorTable(aInput, mRangeErrorTable, UtInput::cLENGTH);
   }
   else
   {
      myCommand = StandardSensorErrorModel::ProcessInput(aInput);
   }
   return myCommand;
}
