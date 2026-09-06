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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

// WsfOTH_RadarSensor is a simple OTH radar sensor implementation.
// Basically a copy of the WsfRadarSensor class with added in functionality

#include "WsfOTH_RadarSensor.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iostream>

#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "UtWallClock.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfEM_Clutter.hpp"
#include "WsfEM_ClutterTypes.hpp"
#include "WsfEnvironment.hpp"
#include "WsfOTH_RadarSensorErrorModel.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSignature.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationInput.hpp"
#include "WsfStandardSensorErrorModel.hpp"
#include "WsfStringId.hpp"

namespace
{
using MTI_AdjustmentTable = WsfMTI_TableManager::MTI_AdjustmentTable;
std::string sLastImplicitBeamCommand;
} // namespace

// =================================================================================================
WsfOTH_RadarSensor::WsfOTH_RadarSensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mOTH_ModeList()
   , mXmtrList()
   , mRcvrList()
   , mAnyModeCanTransmit(true)
   , mAnyModeCanReceive(true)
   , mTempGeometryPtr(nullptr)
{
   SetClass(cACTIVE | cRADIO); // This is an active RF sensor.
   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new OTH_Mode()));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

// =================================================================================================
WsfOTH_RadarSensor::WsfOTH_RadarSensor(const WsfOTH_RadarSensor& aSrc)
   : WsfSensor(aSrc)
   , mOTH_ModeList()
   , mXmtrList()
   , mRcvrList()
   , mAnyModeCanTransmit(aSrc.mAnyModeCanTransmit)
   , mAnyModeCanReceive(aSrc.mAnyModeCanReceive)
   , mTempGeometryPtr(nullptr)
{
}

// =================================================================================================
// virtual
WsfSensor* WsfOTH_RadarSensor::Clone() const
{
   return new WsfOTH_RadarSensor(*this);
}

// =================================================================================================
// virtual
size_t WsfOTH_RadarSensor::GetEM_RcvrCount() const
{
   size_t count = 0;
   if (IsTurnedOn())
   {
      count = mRcvrList.size();
   }
   else if (!mOTH_ModeList.empty())
   {
      OTH_Mode* modePtr = mOTH_ModeList[mModeListPtr->GetCurrentMode()];
      count             = modePtr->mBeamList.size();
   }
   return count;
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfOTH_RadarSensor::GetEM_Rcvr(size_t aIndex) const
{
   if (IsTurnedOn())
   {
      if (aIndex < mRcvrList.size())
      {
         return *(mRcvrList[aIndex]);
      }
   }
   else if (!mOTH_ModeList.empty())
   {
      OTH_Mode* modePtr = mOTH_ModeList[mModeListPtr->GetCurrentMode()];
      if (aIndex < modePtr->mBeamList.size())
      {
         return *modePtr->mBeamList[aIndex]->GetEM_Rcvr();
      }
   }
   return WsfSensor::GetEM_Rcvr(aIndex);
}

// =================================================================================================
// virtual
size_t WsfOTH_RadarSensor::GetEM_RcvrCount(size_t aModeIndex) const
{
   size_t count = 0;
   if (aModeIndex < mOTH_ModeList.size())
   {
      OTH_Mode* modePtr = mOTH_ModeList[aModeIndex];
      count             = modePtr->mBeamList.size();
   }
   return count;
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfOTH_RadarSensor::GetEM_Rcvr(size_t aModeIndex, size_t aIndex) const
{
   if (aModeIndex < mOTH_ModeList.size())
   {
      OTH_Mode* modePtr = mOTH_ModeList[aModeIndex];
      if (aIndex < modePtr->mBeamList.size())
      {
         return *modePtr->mBeamList[aIndex]->mRcvrPtr;
      }
   }
   return WsfSensor::GetEM_Rcvr(aModeIndex, aIndex); // return dummy xmtr
}

// =================================================================================================
// virtual
size_t WsfOTH_RadarSensor::GetEM_XmtrCount() const
{
   size_t count = 0;
   if (IsTurnedOn())
   {
      count = mXmtrList.size();
   }
   else if (!mOTH_ModeList.empty())
   {
      OTH_Mode* modePtr = mOTH_ModeList[mModeListPtr->GetCurrentMode()];
      count             = modePtr->mBeamList.size();
   }
   return count;
}

// =================================================================================================
// virtual
WsfEM_Xmtr& WsfOTH_RadarSensor::GetEM_Xmtr(size_t aIndex) const
{
   if (IsTurnedOn())
   {
      if (aIndex < mXmtrList.size())
      {
         return *(mXmtrList[aIndex]);
      }
   }
   else if (!mOTH_ModeList.empty())
   {
      OTH_Mode* modePtr = mOTH_ModeList[mModeListPtr->GetCurrentMode()];
      if (aIndex < modePtr->mBeamList.size())
      {
         return *modePtr->mBeamList[aIndex]->GetEM_Xmtr();
      }
   }
   return WsfSensor::GetEM_Xmtr(aIndex);
}

// =================================================================================================
// virtual
size_t WsfOTH_RadarSensor::GetEM_XmtrCount(size_t aModeIndex) const
{
   return GetEM_RcvrCount(aModeIndex);
}

// =================================================================================================
// virtual
WsfEM_Xmtr& WsfOTH_RadarSensor::GetEM_Xmtr(size_t aModeIndex, size_t aIndex) const
{
   if (aModeIndex < mOTH_ModeList.size())
   {
      OTH_Mode* modePtr = mOTH_ModeList[aModeIndex];
      if (aIndex < modePtr->mBeamList.size())
      {
         return *modePtr->mBeamList[aIndex]->mXmtrPtr;
      }
   }
   return WsfSensor::GetEM_Xmtr(aModeIndex, aIndex); // return dummy xmtr
}

// =================================================================================================
//! An internal method that is called to update the lists for GetEM_Xmtr/Rcvr.
//! This is called whenever modes are selected or deselected. It updates the lists
//! of active transmitters and receivers that are accessed by the GetEM_Rcvr and
//! GetEM_Xmtr methods.
// virtual
void WsfOTH_RadarSensor::UpdateXmtrRcvrLists()
{
   mXmtrList.clear();
   mRcvrList.clear();
   for (OTH_Mode* modePtr : mOTH_ModeList)
   {
      if (modePtr->IsSelected())
      {
         for (unsigned int beamIndex = 0; beamIndex < modePtr->mBeamList.size(); ++beamIndex)
         {
            OTH_Beam& beam = *modePtr->mBeamList[beamIndex];
            if (modePtr->mCanTransmit)
            {
               mXmtrList.push_back(beam.GetEM_Xmtr());
            }
            if (modePtr->mCanReceive)
            {
               mRcvrList.push_back(beam.GetEM_Rcvr());
            }
         }
      }
   }
}

// =================================================================================================
// virtual
bool WsfOTH_RadarSensor::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mOTH_ModeList);

   mAnyModeCanTransmit = false;
   mAnyModeCanReceive  = false;
   for (OTH_Mode* modePtr : mOTH_ModeList)
   {
      mAnyModeCanTransmit |= modePtr->mCanTransmit;
      mAnyModeCanReceive |= modePtr->mCanReceive;
      // When randomize_radar_frequencies is enabled, notify SensorFrequencyChanged observers of the
      // initial frequencies selected (e.g. via SENSOR_FREQUENCY_CHANGED event) when relevant.
      if (GetSimulation()->RandomizeFrequency() && modePtr->mCanTransmit)
      {
         WsfObserver::SensorFrequencyChanged(GetSimulation())(aSimTime, this, modePtr);
      }
   }
   if (!mAnyModeCanTransmit)
   {
      SetClass(cSEMI_ACTIVE | cRADIO); // This is a semi-active RF sensor
   }
   return ok;
}

// =================================================================================================
// virtual
void WsfOTH_RadarSensor::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // NOTE: A transmit-only device does not perform sensing chances.
   //       All detections are done by the receiver.
   if (mAnyModeCanReceive)
   {
      WsfSensor::PlatformAdded(aSimTime, aPlatformPtr);
   }
}

// =================================================================================================
// virtual
bool WsfOTH_RadarSensor::ProcessInput(UtInput& aInput)
{
   return WsfSensor::ProcessInput(aInput);
}

// =================================================================================================
// virtual
void WsfOTH_RadarSensor::Update(double aSimTime)
{
   // Bypass updates if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime <= (aSimTime + 1.0E-5))
   {
      WsfSensor::Update(aSimTime);
      PerformScheduledDetections(aSimTime); // Perform any required detection attempts
   }
}

// =================================================================================================
//! Process detections attempts under the control of a scheduler.
//!
//! This is a convenience method to process any required detections as directed by a sensor
//! scheduler.
//!
//! @param aSimTime The current simulation time.
// virtual
void WsfOTH_RadarSensor::PerformScheduledDetections(double aSimTime)
{
   // Make sure that a scheduler and tracker are present.
   assert(mModeListPtr != nullptr);
   assert(mSchedulerPtr != nullptr);
   assert(mTrackerPtr != nullptr);
   assert(GetSimulation());

   WsfTrackId                 requestId;
   Settings                   settings;
   WsfSensorResult            result;
   WsfSensorTracker::Settings stSettings;
   size_t                     targetIndex = 0;

   while (mSchedulerPtr->SelectTarget(aSimTime, mNextUpdateTime, targetIndex, requestId, settings))
   {
      WsfSensorMode* modePtr = mOTH_ModeList[settings.mModeIndex];
      assert(modePtr != nullptr);

      // Perform the sensing chance if the target still exists.
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(targetIndex);
      if (targetPtr != nullptr)
      {
         if (targetPtr->IsFalseTarget())
         {
            continue;
         }

         settings.mRequiredPd = GetRequiredPd(modePtr);
         if (modePtr->WithinDetectionRange(aSimTime, targetPtr))
         {
            if (AttemptToDetect(aSimTime, targetPtr, settings, result))
            {
               // Apply errors and indicate target is detected
               modePtr->ApplyMeasurementErrors(result);
               mTrackerPtr->TargetDetected(aSimTime, stSettings, requestId, targetIndex, targetPtr, result);
            }
            else
            {
               mTrackerPtr->TargetUndetected(aSimTime, stSettings, requestId, targetIndex, targetPtr, result);
            }
            NotifyTargetUpdated(aSimTime, targetPtr, result);
         }
         else
         {
            // Detection chance was culled because it is out of range. We must still report to the tracker
            // because it possible it may be coasting the target.
            result.Reset();
            result.mModeIndex     = settings.mModeIndex;
            result.mCheckedStatus = WsfSensorResult::cRCVR_RANGE_LIMITS;
            result.mFailedStatus  = WsfSensorResult::cRCVR_RANGE_LIMITS;
            mTrackerPtr->TargetUndetected(aSimTime, stSettings, requestId, targetIndex, targetPtr, result);
         }
      }
      else if (TransientCueActive() && (targetIndex == 0))
      {
         if (mTempGeometryPtr == nullptr)
         {
            mTempGeometryPtr = new WsfPlatform(GetScenario());
         }

         double locationWCS[3];
         GetTransientCuedLocationWCS(locationWCS);
         mTempGeometryPtr->SetLocationWCS(locationWCS);

         settings.mRequiredPd = GetRequiredPd(modePtr);
         if (modePtr->WithinDetectionRange(aSimTime, mTempGeometryPtr))
         {
            for (unsigned int xmtrIndex = 0; xmtrIndex < GetEM_XmtrCount(); ++xmtrIndex)
            {
               if (DebugEnabled())
               {
                  auto out = ut::log::debug() << "Curing to local track.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  out.AddNote() << "Sensor: " << GetName();
                  out.AddNote() << "Local Track Number: " << requestId.GetLocalTrackNumber();
               }
               result.Reset(settings);
               UpdatePosition(aSimTime); // Ensure position is current
               result.BeginGenericInteraction(&GetEM_Xmtr(xmtrIndex),
                                              mTempGeometryPtr,
                                              GetEM_Xmtr(xmtrIndex).GetLinkedReceiver());
               result.ComputeUndefinedGeometry();
               result.SetTransmitterBeamPosition();
               if (result.GetTransmitter() != nullptr)
               {
                  result.GetTransmitter()->NotifyListeners(aSimTime, result); // Notify listeners for possible ESM detection
               }
            }
         }
      }
      else if (mTrackerPtr->TargetDeleted(aSimTime, stSettings, requestId, targetIndex))
      {
         // Target no longer physically exists and is not being tracked.
         mSchedulerPtr->RemoveTarget(aSimTime, targetIndex);
      }
      WsfArticulatedPart::ClearTransientCue(); // Release any transient cue created by the scheduler.
   }                                           // while (mSchedulerPtr->SelectTarget())

   // Set the update interval so the schedule is called at the desired time.
   double updateInterval = std::max(mNextUpdateTime - aSimTime, 1.0E-5);
   SetUpdateInterval(updateInterval);
}

// =================================================================================================
// Nested class WsfOTH_RadarSensor::OTH_Beam:.
// =================================================================================================

WsfOTH_RadarSensor::OTH_Beam::OTH_Beam()
   : WsfSensorBeam()
   , mAntennaPtr(new WsfEM_Antenna)
   , mXmtrPtr(new WsfEM_Xmtr(WsfEM_Xmtr::cXF_SENSOR, mAntennaPtr.get()))
   , mRcvrPtr(new WsfEM_Rcvr(WsfEM_Rcvr::cRF_SENSOR, mAntennaPtr.get()))
   , mClutterPtr(nullptr)
   , mNoise()
   , mErrorModelAzBeamwidth(-1.0)
   , mErrorModelElBeamwidth(-1.0)
   , mErrorModelPulseWidth(-1.0)
   , mErrorModelDopplerResolution(-1.0)
   ,
   // Start of private data
   mCanTransmit(true)
   , mUseDetector(false)
   , mDopplerResolution(0.0)
   , mOneM2DetectRange(0.0)
   , mLookDownFactor(1.0)
   , mPRF_Factor(1.0)
   , mPostLockonDetectionThresholdAdjustment(1.0)
   , mPostLockonAdjustmentDelayTime(0.0)
   , mLoopGain(0.0)
   , mAdjustmentFactor(1.0)
   , mIntegrationGain(1.0)
   , mNumberOfPulsesIntegrated(1)
   , mDetector()
   , mProbabilityTablePtr(nullptr)
   , mMTI_AdjustmentTablePtr(nullptr)
   , mMTI_TableMaxRange(1.0e38)
   , mClutterAttenuationFactor(0.0)
   , mClutterType()
   , mMTI_MaxRange(0.0)
   , mMTI_InterpulsePeriod_1(0.0)
   , mMTI_InterpulsePeriod_2(0.0)
   , mMTI_StabilityConstant(0.0)
   , mMTI_Weights{0.0, 0.0, 0.0}
   , mMTI_NumberOfStages(1)
   , mPerformMTI_Processing(false)
   , mMTI_ClutterLock(true)
   , mMTI_UpWind(true)
   , mSolarCharacteristics()
   , mIonosphericCharacteristics()
   , mOTH_Geometry()
   , mIonosphereChanged(false)
   , mComputeAtmosphericNoise(false)
   , mDebug(false)
{
   // Indicate the receiver is 'linked' with the transmitter.
   mXmtrPtr->SetLinkedReceiver(GetEM_Rcvr());
}

// =================================================================================================
WsfOTH_RadarSensor::OTH_Beam::OTH_Beam(const OTH_Beam& aSrc)
   : WsfSensorBeam(aSrc)
   , mAntennaPtr(new WsfEM_Antenna(*aSrc.mAntennaPtr))
   , mXmtrPtr(new WsfEM_Xmtr(*aSrc.mXmtrPtr, mAntennaPtr.get()))
   , mRcvrPtr(new WsfEM_Rcvr(*aSrc.mRcvrPtr, mAntennaPtr.get()))
   , mClutterPtr(nullptr)
   , mNoise(aSrc.mNoise)
   , mErrorModelAzBeamwidth(aSrc.mErrorModelAzBeamwidth)
   , mErrorModelElBeamwidth(aSrc.mErrorModelElBeamwidth)
   , mErrorModelPulseWidth(aSrc.mErrorModelPulseWidth)
   , mErrorModelDopplerResolution(aSrc.mErrorModelDopplerResolution)
   , mCanTransmit(aSrc.mCanTransmit)
   , mUseDetector(aSrc.mUseDetector)
   , mDopplerResolution(aSrc.mDopplerResolution)
   , mOneM2DetectRange(aSrc.mOneM2DetectRange)
   , mLookDownFactor(aSrc.mLookDownFactor)
   , mPRF_Factor(aSrc.mPRF_Factor)
   , mPostLockonDetectionThresholdAdjustment(aSrc.mPostLockonDetectionThresholdAdjustment)
   , mPostLockonAdjustmentDelayTime(aSrc.mPostLockonAdjustmentDelayTime)
   , mLoopGain(aSrc.mLoopGain)
   , mAdjustmentFactor(aSrc.mAdjustmentFactor)
   , mIntegrationGain(aSrc.mIntegrationGain)
   , mNumberOfPulsesIntegrated(aSrc.mNumberOfPulsesIntegrated)
   , mDetector(aSrc.mDetector)
   , mProbabilityTablePtr(aSrc.mProbabilityTablePtr)
   , mMTI_AdjustmentTablePtr(aSrc.mMTI_AdjustmentTablePtr)
   , mMTI_TableMaxRange(aSrc.mMTI_TableMaxRange)
   , mClutterAttenuationFactor(aSrc.mClutterAttenuationFactor)
   , mClutterType(aSrc.mClutterType)
   , mMTI_MaxRange(aSrc.mMTI_MaxRange)
   , mMTI_InterpulsePeriod_1(aSrc.mMTI_InterpulsePeriod_1)
   , mMTI_InterpulsePeriod_2(aSrc.mMTI_InterpulsePeriod_2)
   , mMTI_StabilityConstant(aSrc.mMTI_StabilityConstant)
   , mMTI_Weights{0.0, 0.0, 0.0}
   , mMTI_NumberOfStages(aSrc.mMTI_NumberOfStages)
   , mPerformMTI_Processing(aSrc.mPerformMTI_Processing)
   , mMTI_ClutterLock(aSrc.mMTI_ClutterLock)
   , mMTI_UpWind(aSrc.mMTI_UpWind)
   , mSolarCharacteristics(aSrc.mSolarCharacteristics)
   , mIonosphericCharacteristics(aSrc.mIonosphericCharacteristics)
   , mOTH_Geometry(aSrc.mOTH_Geometry)
   , mIonosphereChanged(aSrc.mIonosphereChanged)
   , mComputeAtmosphericNoise(aSrc.mComputeAtmosphericNoise)
   , mDebug(aSrc.mDebug)
{
   if (aSrc.mClutterPtr != nullptr)
   {
      ut::log::error() << "Unexpected clone of clutter_model.";
   }

   // Indicate the receiver is 'linked' with the transmitter.
   mXmtrPtr->SetLinkedReceiver(GetEM_Rcvr());
}

// =================================================================================================
// virtual
WsfOTH_RadarSensor::OTH_Beam::~OTH_Beam()
{
   delete mClutterPtr;
}

// =================================================================================================
//! Attempt to detect the specified target.
//!
//! @param aSimTime     [input]   The current simulation time.
//! @param aTargetPtr   [input]   The target that is trying to be detected.
//! @param aSettings    [input]   The settings of the sensor which is attempting to detect the aTargetPtr.
//! @param aResult      [updated] On input this contains the current relative geometry information.
//! On output this it will be updated to contain the RCS, signal-to-noise and Pd.

void WsfOTH_RadarSensor::OTH_Beam::AttemptToDetect(double           aSimTime,
                                                   WsfPlatform*     aTargetPtr,
                                                   Settings&        aSettings,
                                                   WsfSensorResult& aResult)
{
   // Should not happen at this point.
   if (mIonosphereChanged)
   {
      UpdateIonosphericCharacteristics();
   }

   if (!CanBounceToTarget(aTargetPtr, aResult))
   {
      aResult.mFailedStatus |= WsfSensorResult::cOTH_LIMITS;
   }

   if (aResult.mFailedStatus == 0)
   {
      if (mCanTransmit)
      {
         // Mono-static interaction.
         AttemptToDetect(aSimTime, aTargetPtr, aSettings, GetEM_Xmtr(), aResult);
      }
      else
      {
         // Bistatic interaction.  Attempt to receive from any transmitter that is marked
         // as a sensor and take the one that gives the best result.

         bool            firstInteraction = true;
         WsfSensorResult tempResult;
         for (unsigned int i = 0; i < mRcvrPtr->GetSensorInteractorCount(); ++i)
         {
            WsfEM_Xmtr* xmtrPtr = mRcvrPtr->GetSensorInteractorEntry(i);
            // Only attempt the detection if the illuminator platform is not the same as the target platform.
            // (The illuminator does not illuminate itself!)
            if (xmtrPtr->GetPlatform() != aTargetPtr)
            {
               if (firstInteraction)
               {
                  firstInteraction = false;
                  AttemptToDetect(aSimTime, aTargetPtr, aSettings, xmtrPtr, aResult);
               }
               else
               {
                  tempResult.Reset(aResult);
                  tempResult.mBeamIndex = aResult.mBeamIndex;
                  AttemptToDetect(aSimTime, aTargetPtr, aSettings, xmtrPtr, tempResult);
                  if (tempResult.mSignalToNoise > aResult.mSignalToNoise)
                  {
                     aResult = tempResult;
                  }
               }
            }
         }
         if (firstInteraction)
         {
            aResult.mFailedStatus |= WsfEM_Interaction::cSIGNAL_LEVEL;
         }
      }
   }
}

// =================================================================================================
// virtual
void WsfOTH_RadarSensor::OTH_Beam::AttemptToDetect(double           aSimTime,
                                                   WsfPlatform*     aTargetPtr,
                                                   Settings&        aSettings,
                                                   WsfEM_Xmtr*      aXmtrPtr,
                                                   WsfSensorResult& aResult)
{
   if (aResult.BeginTwoWayOTH_Interaction(aXmtrPtr, aTargetPtr, GetEM_Rcvr(), mOTH_Geometry.mReflectionLocWCS) == 0)
   {
      // Set the position of the antenna beam(s).
      aResult.SetTransmitterBeamPosition();
      aResult.SetReceiverBeamPosition();

      // Determine the radar cross section of the target.

      aResult.ComputeRadarSigAzEl();
      aResult.mRadarSig = WsfRadarSignature::GetValue(aTargetPtr,
                                                      aXmtrPtr,
                                                      GetEM_Rcvr(),
                                                      aResult.mRadarSigAz,
                                                      aResult.mRadarSigEl,
                                                      aResult.mRadarSigAz,
                                                      aResult.mRadarSigEl);

      // Calculate the signal return.
      aResult.ComputeRF_TwoWayPower(aResult.mRadarSig);

      // Override the power computation just performed using the ranges from the
      // Sensor -> Reflection Point -> Target
      // Multiply the R^4 term
      double xmtrRange = std::max(aResult.mXmtrToTgt.mRange, 1.0);
      double rcvrRange = xmtrRange;
      if (aResult.mBistatic)
      {
         rcvrRange = std::max(aResult.mRcvrToTgt.mRange, 1.0);
      }
      aResult.mRcvdPower *= (UtMath::cFOUR_PI * xmtrRange * xmtrRange * UtMath::cFOUR_PI * rcvrRange * rcvrRange);

      // Compute the Ranges Between the reflection point and the target, xmtr, and rcvr, respectively.
      double xmtrToTgt = 0.0;
      double rcvrToTgt = 0.0;
      ComputeBouncePathRanges(aResult, xmtrToTgt, rcvrToTgt);

      xmtrToTgt = std::max(1.0, xmtrToTgt);
      rcvrToTgt = std::max(1.0, rcvrToTgt);

      // Apply the OTH Ranges to the Power
      aResult.mRcvdPower /= (UtMath::cFOUR_PI * xmtrToTgt * xmtrToTgt * UtMath::cFOUR_PI * rcvrToTgt * rcvrToTgt);

      // Account for the gain due to pulse compression.
      aResult.mRcvdPower *= aXmtrPtr->GetPulseCompressionRatio();

      // Account for integration gain
      aResult.mRcvdPower *= mIntegrationGain;

      // Allow for other general post-reception adjustments (for compatibility with SUPPRESSOR OPERATING-LOSSES).
      aResult.mRcvdPower *= mAdjustmentFactor;

      // Apply the MTI adjustment if it was provided
      if (mMTI_AdjustmentTablePtr)
      {
         // Approximation to ground range.
         double cosTheta    = cos(aResult.mRcvrToTgt.mTrueEl);
         double groundRange = aResult.mRcvrToTgt.mRange * cosTheta;
         if (groundRange < mMTI_TableMaxRange)
         {
            // The return value from GetDopplerSpeed is negative if the target is closing. We want it the other way around!
            double closingSpeed = -aResult.ComputeTargetDopplerSpeed(true);
            double MTI_AdjustmentFactor =
               mMTI_AdjustmentTablePtr->ComputeAdjustmentFactor(closingSpeed, aXmtrPtr->GetFrequency());
            aResult.mRcvdPower *= MTI_AdjustmentFactor;
         }
      }

      // Apply mPRF_Factor if abs(closing speed) < platform speed.  Included for IWARS compatibility.
      if (mPRF_Factor != 1.0)
      {
         double toTargetWCS[3];
         mAntennaPtr->GetPlatform()->GetRelativeLocationWCS(aTargetPtr, toTargetWCS);
         UtVec3d::Normalize(toTargetWCS);
         double thisVelocityWCS[3];
         mAntennaPtr->GetPlatform()->GetVelocityWCS(thisVelocityWCS);
         double targetVelocityWCS[3];
         aTargetPtr->GetVelocityWCS(targetVelocityWCS);

         double closingVelocity =
            UtVec3d::DotProduct(thisVelocityWCS, toTargetWCS) - UtVec3d::DotProduct(targetVelocityWCS, toTargetWCS);
         double thisSpeed = UtVec3d::Magnitude(thisVelocityWCS);

         if ((closingVelocity < thisSpeed) && (closingVelocity > -thisSpeed))
         {
            aResult.mRcvdPower *= mPRF_Factor;
         }
      }

      // Apply mLookDownFactor if target is lower than receiver.  Included for IWARS compatibility.
      if (mLookDownFactor != 1.0)
      {
         double antennaLat;
         double antennaLon;
         double antennaAlt;
         mAntennaPtr->GetLocationLLA(antennaLat, antennaLon, antennaAlt);
         if (antennaAlt >= aTargetPtr->GetAltitude())
         {
            aResult.mRcvdPower *= mLookDownFactor;
         }
      }

      // Compute the effects of MTI on clutter, received power and noise power.
      // This also will return the clutter attenuation factor for non-MTI systems.
      double clutterAttenuationFactor;
      ComputeMTI_Effects(aResult, clutterAttenuationFactor);

      // Compute the clutter power
      aResult.mClutterPower = 0.0;
      if ((mClutterPtr != nullptr) && (clutterAttenuationFactor > 0.0))
      {
         aResult.mClutterPower = mClutterPtr->ComputeClutterPower(aResult,
                                                                  GetPlatform()->GetSimulation()->GetEnvironment(),
                                                                  clutterAttenuationFactor);
         // cout << "WSF  clutter: Unfiltered: "
         //      << UtMath::SafeLinearToDB(aResult.mClutterPower / clutterAttenuationFactor)
         //      << " Atten: " << UtMath::LinearToDB(1.0 / clutterAttenuationFactor)
         //      << " Filtered: "
         //      << UtMath::SafeLinearToDB(aResult.mClutterPower) << endl;
      }

      // Add galactic, atmospherics, and man made noise to the clutter power
      if (mComputeAtmosphericNoise)
      {
         aResult.mClutterPower += mNoise.ComputeNoise(aResult);
      }

      // Compute component effects.
      WsfSensor* sensorPtr = GetSensorMode()->GetSensor();
      WsfSensorComponent::AttemptToDetect(*sensorPtr, aSimTime, aResult);

      // Adjust for the effects of any signal processing.
      GetSignalProcessors().Execute(aSimTime, aResult);

      // Ensure Signal processing didn't have a failure code.
      if (aResult.mFailedStatus == 0)
      {
         // Compute the total effective signal-to-interference ratio at the output of the receiver.
         aResult.mSignalToNoise =
            mRcvrPtr->ComputeSignalToNoise(aResult.mRcvdPower, aResult.mClutterPower, aResult.mInterferencePower);

         // If a 'time-locked-on' was supplied (aSettings.mLockonTime >= 0.0) then adjust the detection threshold
         // by the post_lockon_detection_threshold_adjustment (default 1.0). Note that the 'time-locked-on' simply
         // indicates when WsfSensorTracker has declared a that detection is 'stable' (e.g.: M/N criteria met for
         // the mode). It doesn't mean the sensor is a 'tracker'. That's OK, however, because the default adjustment
         // is 1.0. So even if it is applied to a tracking mode it will still work

         double detectionThresholdAdjustment = 1.0;
         if ((aSettings.mLockonTime >= 0.0) && ((aSettings.mLockonTime + mPostLockonAdjustmentDelayTime) <= aSimTime))
         {
            detectionThresholdAdjustment = mPostLockonDetectionThresholdAdjustment;
            aResult.mDetectionThreshold *= detectionThresholdAdjustment;
         }

         // Compute the probability of detection.
         if (mProbabilityTablePtr)
         {
            // detection_probability table selected
            aResult.mPd =
               mProbabilityTablePtr->ComputeProbabilityOfDetection(aResult.mSignalToNoise / detectionThresholdAdjustment);
         }
         else if (mUseDetector)
         {
            // Marcum-Swerling detector selected
            aResult.mPd = mDetector.ComputeProbabilityOfDetection(aResult.mSignalToNoise / detectionThresholdAdjustment);
         }
         else
         {
            // Simple binary detector selected
            aResult.mPd = 1.0;
            if (aResult.mSignalToNoise < (mRcvrPtr->GetDetectionThreshold() * detectionThresholdAdjustment))
            {
               aResult.mPd = 0.0;
            }
         }

         // Adjust the Pd by optional component effects.
         aResult.mPd *= (1.0 - aResult.mInterferenceFactor);

         // Check the signal level
         aResult.mCheckedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
         if (aResult.mPd < aResult.mRequiredPd)
         {
            aResult.mFailedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
         }
      }
   }
}

// =================================================================================================
// private
void WsfOTH_RadarSensor::OTH_Beam::Calibrate(bool aPrint)
{
   double p_avg      = mXmtrPtr->GetAveragePower();
   double dutyCycle  = mXmtrPtr->GetPulseWidth() * mXmtrPtr->GetPulseRepetitionFrequency();
   double wavelength = UtMath::cLIGHT_SPEED / mXmtrPtr->GetFrequency();
   auto   out        = ut::log::info();
   if (aPrint)
   {
      out << "WsfOTH_RadarSensor::Calibrate():";
      out.AddNote() << "Peak Power Output: " << UtMath::LinearToDB(mXmtrPtr->GetPeakPower()) << " dbW ("
                    << mXmtrPtr->GetPeakPower() << " W)";
      out.AddNote() << "Pulse Repetition Frequency: " << mXmtrPtr->GetPulseRepetitionFrequency() << " Hz";
      out.AddNote() << "Pulse Width: " << mXmtrPtr->GetPulseWidth() << " sec";
      out.AddNote() << "Duty Cycle (input:PW/PRI): " << mXmtrPtr->GetDutyCycle() << ":" << dutyCycle;
      double pcr = mXmtrPtr->GetPulseCompressionRatio();
      out.AddNote() << "Pulse Compression Ratio: " << UtMath::LinearToDB(pcr) << " dB (" << pcr << ")";
      out.AddNote() << "Average Power Output: " << UtMath::LinearToDB(p_avg) << " dBW (" << p_avg << " W)";

      if (mXmtrPtr->ExplicitFrequencyList() /* && (!mXmtrPtr->UseDefaultFrequency())*/)
      {
         out.AddNote() << "Frequency (default): " << mXmtrPtr->GetFrequency() << " Hz";
         out.AddNote() << "Wavelength (default): " << wavelength << " m";
         WsfEM_Xmtr::FrequencyList freqList = mXmtrPtr->GetAlternateFrequencies();
         for (size_t i = 0; i < freqList.size(); ++i)
         {
            out.AddNote() << "Frequency " << i + 1 << ": " << freqList[i] << " Hz";
         }
      }
      else
      {
         out.AddNote() << "Frequency: " << mXmtrPtr->GetFrequency() << " Hz";
         out.AddNote() << "Wavelength: " << wavelength << " m";
      }

      double peakGain;
      peakGain = mXmtrPtr->GetPeakAntennaGain();
      out.AddNote() << "Transmitter Antenna Gain: " << UtMath::LinearToDB(peakGain) << " dB (" << peakGain << ")";
      peakGain = mRcvrPtr->GetPeakAntennaGain();
      out.AddNote() << "Receiver Antenna Gain: " << UtMath::LinearToDB(peakGain) << " dB (" << peakGain << ")";

      double internalLoss;
      internalLoss = mXmtrPtr->GetInternalLoss();
      out.AddNote() << "Transmitter Internal Loss: " << UtMath::LinearToDB(internalLoss) << " dB (" << internalLoss << ")";
      internalLoss = mRcvrPtr->GetInternalLoss();
      out.AddNote() << "Receiver Internal Loss: " << UtMath::LinearToDB(internalLoss) << " dB (" << internalLoss << ")";

      double integrationGain = mIntegrationGain;
      if (integrationGain != 1.0)
      {
         out.AddNote() << "Integration Gain: " << UtMath::LinearToDB(integrationGain) << " dB (" << integrationGain << ")";
      }

      double adjustmentFactor = mAdjustmentFactor;
      if (adjustmentFactor != 1.0)
      {
         out.AddNote() << "Adjustment Factor: " << UtMath::LinearToDB(adjustmentFactor) << " dB (" << adjustmentFactor
                       << ")";
      }

      if (mOneM2DetectRange > 0.0)
      {
         out.AddNote() << "1 m^2 Detection Range: " << mOneM2DetectRange << " m";
      }
      else if (mLoopGain > 0.0)
      {
         out.AddNote() << "Loop Gain: " << UtMath::LinearToDB(mLoopGain) << " dB";
      }
      else
      {
         out.AddNote() << "Receiver Noise Power: " << UtMath::LinearToDB(mRcvrPtr->GetNoisePower()) << " dBW ("
                       << mRcvrPtr->GetNoisePower() << " W)";
      }
   }

   double detectionThreshold = mRcvrPtr->GetDetectionThreshold();
   if (aPrint)
   {
      double thresholdWatts = detectionThreshold * mRcvrPtr->GetNoisePower();
      out.AddNote() << "Minimum Detectable Signal: " << UtMath::LinearToDB(thresholdWatts) << " dBW (" << thresholdWatts
                    << " W)";
      out.AddNote() << "Minimum Detectable S/N: " << UtMath::LinearToDB(detectionThreshold) << " dB ("
                    << detectionThreshold << ")";
   }

   if (mOneM2DetectRange > 0.0)
   {
      double r = mOneM2DetectRange;

      double distanceFactor = 1.0 / (4.0 * UtMath::cPI * r * r);

      // Compute the effective radiated power from the antenna.
      double p_radiated = mXmtrPtr->GetPower() * mXmtrPtr->GetPeakAntennaGain() / mXmtrPtr->GetInternalLoss();

      // Compute the power per unit area on at the target
      double p_tgt_area = p_radiated * distanceFactor;

      // Compute the reflected power
      double rcs         = 1.0;
      double p_reflected = p_tgt_area * rcs;

      // Compute the incident power per unit area at the receiving antenna.
      double p_rcvr_area = p_reflected * distanceFactor;

      // Compute the effective received power.
      double temp1      = wavelength * wavelength / (4.0 * UtMath::cPI);
      double p_received = p_rcvr_area * temp1 * mRcvrPtr->GetPeakAntennaGain() / mRcvrPtr->GetInternalLoss();

      // Account for the gain due to pulse compression.
      p_received *= mXmtrPtr->GetPulseCompressionRatio();

      // Account for integration gain
      p_received *= mIntegrationGain;

      // Allow for other general post-reception adjustments (for compatibility with SUPPRESSOR OPERATING-LOSSES).
      p_received *= mAdjustmentFactor;

      double rcvrNoise = p_received / detectionThreshold;
      mRcvrPtr->SetNoisePower(rcvrNoise);
      if (aPrint)
      {
         out.AddNote() << "Receiver Noise: " << UtMath::LinearToDB(rcvrNoise) << " dbW (" << rcvrNoise
                       << " W) (calibrated)";
      }
   }
   else if (mLoopGain > 0.0)
   {
      // Solve for the receiver noise
      double temp1 = (wavelength * wavelength) / (UtMath::cFOUR_PI * UtMath::cFOUR_PI * UtMath::cFOUR_PI);
      temp1 *= mXmtrPtr->GetPower() * mXmtrPtr->GetPeakAntennaGain() * mRcvrPtr->GetPeakAntennaGain();
      temp1 /= (mXmtrPtr->GetInternalLoss() * mRcvrPtr->GetInternalLoss());
      double rcvrNoise = temp1 / mLoopGain;
      mRcvrPtr->SetNoisePower(rcvrNoise);
      if (aPrint)
      {
         out.AddNote() << "Receiver Noise: " << UtMath::LinearToDB(rcvrNoise) << " dbW (" << rcvrNoise
                       << " W) (calibrated)";
      }
   }

   {
      double temp1 = (wavelength * wavelength) / (UtMath::cFOUR_PI * UtMath::cFOUR_PI * UtMath::cFOUR_PI);
      temp1 *= mXmtrPtr->GetPower() * mXmtrPtr->GetPeakAntennaGain() * mRcvrPtr->GetPeakAntennaGain();
      temp1 /= (mXmtrPtr->GetInternalLoss() * mRcvrPtr->GetInternalLoss());

      // Account for the gain due to pulse compression.
      temp1 *= mXmtrPtr->GetPulseCompressionRatio();

      // Account for integration gain
      temp1 *= mIntegrationGain;

      // Allow for other general post-reception adjustments (for compatibility with SUPPRESSOR OPERATING-LOSSES).
      temp1 *= mAdjustmentFactor;

      double r        = pow(temp1 / (mRcvrPtr->GetNoisePower() * detectionThreshold), 0.25);
      double loopGain = temp1 / mRcvrPtr->GetNoisePower();
      if (aPrint)
      {
         out.AddNote() << "1 m^2 Detection Range: " << r << " m (calibrated - free space)";
         out.AddNote() << "Loop Gain: " << UtMath::LinearToDB(loopGain) << " dB (calibrated)";
      }
   }
}

// =================================================================================================
bool WsfOTH_RadarSensor::OTH_Beam::AreIonosphericParametersValid()
{
   bool ok = mIonosphericCharacteristics.mElectronDensityAtMax > 0;
   ok &= mIonosphericCharacteristics.mElectronHeightAtMax > 0;
   ok &= mIonosphericCharacteristics.mReflectionHeight > 0;
   ok &= mIonosphericCharacteristics.mTemperatureAtMax > 0;

   if (!ok)
   {
      ut::log::warning() << "Invalid ionospheric characterization in WsfOTH_RadarSensor.";
   }

   return ok;
}

// =================================================================================================
void WsfOTH_RadarSensor::OTH_Beam::GetIonosphericRangeLimits(double& aMin, double& aMax)
{
   aMin = mIonosphericCharacteristics.mMinRange;
   aMax = mIonosphericCharacteristics.mMaxRange;
}

void WsfOTH_RadarSensor::OTH_Beam::ComputeIonosphericCharacteristics()
{
   double radarLat;
   double radarLon;
   double radarAlt;

   // radar location
   mXmtrPtr->GetPlatform()->GetLocationLLA(radarLat, radarLon, radarAlt);

   double radarLatRads = radarLat * UtMath::cRAD_PER_DEG;

   // define and determine some solar characteristics

   double dayOfYear = mSolarCharacteristics.mDayOfYear;
   double hourOfDay = mSolarCharacteristics.mHourOfDay;

   double solarAngleRads               = ((hourOfDay - 12.00) * 15) * UtMath::cRAD_PER_DEG;
   mSolarCharacteristics.mSolarAngRads = solarAngleRads;

   double solarDeclinationAngle     = 23.44 * sin(0.9856 * UtMath::cRAD_PER_DEG * (dayOfYear - 80.7));
   double solarDeclinationAngleRads = solarDeclinationAngle * UtMath::cRAD_PER_DEG;

   mSolarCharacteristics.mSolarDeclinationAngRads = solarDeclinationAngleRads;

   double solarZenithAngleRads = acos(sin(radarLatRads) * sin(solarDeclinationAngleRads) +
                                      cos(radarLatRads) * cos(solarDeclinationAngleRads) * cos(solarAngleRads));

   mSolarCharacteristics.mSolarZenithAngRads = solarZenithAngleRads;

   // define some ionospheric characteristics
   double nM = mIonosphericCharacteristics.mElectronDensityAtMax; // electron density at maximum
   double hMax_km =
      mIonosphericCharacteristics.mElectronHeightAtMax / 1000.0;         // height of electron density maximum in meters
   double h_km = mIonosphericCharacteristics.mReflectionHeight / 1000.0; // height of ionospheric reflection
   double T    = mIonosphericCharacteristics.mTemperatureAtMax;          // temperature of the reflection layer

   // computations to determine critical frequency and critical angle
   double hNormalized    = (34.11 / T) * (h_km - hMax_km); // distances converted to km
   double cosZenithAngle = cos(solarZenithAngleRads);
   double secZenithAngle = (cosZenithAngle <= 0) ? UtMath::cDOUBLE_MAX : 1.0 / cosZenithAngle;

   double n0 = nM * sqrt(secZenithAngle);
   double nE = n0 * exp(0.5 * (1 - hNormalized - (secZenithAngle * exp(-hNormalized))));

   mIonosphericCharacteristics.mCriticalFrequency = 8.98 * sqrt(nE);

   // Add a little buffer to the min frequency so that the sensor isn't capable of looking straight up.
   mIonosphericCharacteristics.mMinUsableFrequency = 1.03 * mIonosphericCharacteristics.mCriticalFrequency;

   // compute min range based on highest possible incidence angle for the current radar frequency
   // compute the maximum incidence angle for the given frequency
   double radarFrequency = mXmtrPtr->GetFrequency();
   if (radarFrequency <= mIonosphericCharacteristics.mMinUsableFrequency)
   {
      auto out = ut::log::warning() << "Current Transmitter Frequency is LESS than OTH Minimum Usable Frequency for "
                                       "the chosen Ionospheric Conditions, transmit Signal will not propagate.";
      out.AddNote() << "Defined Transmitter Frequency: " << radarFrequency / 1000000.0 << " MHz";
      out.AddNote() << "Computed Minimum Usable Frequency: " << mIonosphericCharacteristics.mMinUsableFrequency / 1000000.0
                    << " MHz";

      // Clamp the MaxIncidence Angle so that no AttemptsToDetect can occur.
      mIonosphericCharacteristics.mMaxIncidenceAngleDegrees = 0.0;
   }
   else
   {
      mIonosphericCharacteristics.mMaxIncidenceAngleDegrees =
         asin(mIonosphericCharacteristics.mCriticalFrequency / radarFrequency) / UtMath::cRAD_PER_DEG;
   }

   // Get the complement of the MaxIncidenceAngle and then apply the Law of Sines to compute the greatCircleHalfAngle
   double A     = UtSphericalEarth::cEARTH_RADIUS + h_km * 1000.0;
   double B     = UtSphericalEarth::cEARTH_RADIUS;
   double b     = UtMath::cPI_OVER_2 - mIonosphericCharacteristics.mMaxIncidenceAngleDegrees / UtMath::cDEG_PER_RAD;
   double sin_b = sin(b);

   double a                     = UtMath::cPI - asin(A * sin_b / B);
   double gammaGreatCircleAngle = 2.0 * (UtMath::cPI - a - b);
   // compute the minimum range due to the maximum incidence angle
   mIonosphericCharacteristics.mMinRange = gammaGreatCircleAngle * UtSphericalEarth::cEARTH_RADIUS;

   // compute max range based on great circle tangent
   double cSquared   = pow(UtSphericalEarth::cEARTH_RADIUS * 0.001 + h_km, 2.0);
   double aSquared   = pow(UtSphericalEarth::cEARTH_RADIUS * 0.001, 2.0);
   double bSquared   = cSquared - aSquared;
   double gammaPrime = asin(sqrt(bSquared) / sqrt(cSquared));

   mIonosphericCharacteristics.mMaxRange = UtSphericalEarth::cEARTH_RADIUS * (2.0 * gammaPrime);

   mIonosphereChanged = false;
}

void WsfOTH_RadarSensor::OTH_Beam::UpdateIonosphericCharacteristics()
{
   ComputeIonosphericCharacteristics();

   // Update the min/max Ranges in the Antenna if commanded to override user input in the Antenna
   if (mIonosphericCharacteristics.mRangeConstrained)
   {
      double min;
      double max;
      GetIonosphericRangeLimits(min, max);
      mAntennaPtr->SetRangeLimits(min, max);
   }
}

bool WsfOTH_RadarSensor::OTH_Beam::CanBounceToTarget(WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   bool canAttemptToDetect = false;

   double radarLat;
   double radarLon;
   double radarAlt;
   double targetLat;
   double targetLon;
   double targetAlt;

   // radar location
   mXmtrPtr->GetPlatform()->GetLocationLLA(radarLat, radarLon, radarAlt);

   // target location
   aTargetPtr->GetLocationLLA(targetLat, targetLon, targetAlt);

   double heading;

   UtSphericalEarth::GreatCircleHeadingAndDistance(radarLat,
                                                   radarLon,
                                                   targetLat,
                                                   targetLon,
                                                   heading,
                                                   mOTH_Geometry.mGreatCircleDistance);

   double radarLatRads = radarLat * UtMath::cRAD_PER_DEG;

   double h_km = mIonosphericCharacteristics.mReflectionHeight / 1000.0; // height of ionospheric reflection

   // determine if target location supports detection
   // compute heading and great circle distance to target
   double radarLonRads  = radarLon * UtMath::cRAD_PER_DEG;
   double targetLatRads = targetLat * UtMath::cRAD_PER_DEG;
   double targetLonRads = targetLon * UtMath::cRAD_PER_DEG;
   ;

   double deltaLonRads = targetLonRads - radarLonRads;

   mOTH_Geometry.mAngularExtent = mOTH_Geometry.mGreatCircleDistance / UtSphericalEarth::cEARTH_RADIUS;

   // Compute midpoint (i.e. reflection point)
   double temp1 = cos(targetLatRads) * cos(deltaLonRads);
   double temp2 = cos(targetLatRads) * sin(deltaLonRads);
   double temp3 = cos(radarLatRads) + temp1;
   double temp4 = atan2(temp2, temp3);

   double arg1 = sin(radarLatRads) + sin(targetLatRads);
   double arg2 = sqrt(pow(temp3, 2.0) + pow(temp2, 2.0));

   double reflectionLat = atan2(arg1, arg2) * UtMath::cDEG_PER_RAD;
   double reflectionLon = (radarLonRads + temp4) * UtMath::cDEG_PER_RAD;
   double reflectionAlt = h_km * 1000.0;
   // Save the reflection Lat, Lon, Alt

   UtEntity::ConvertLLAToWCS(reflectionLat, reflectionLon, reflectionAlt, mOTH_Geometry.mReflectionLocWCS);
   mOTH_Geometry.mReflectionHeight = reflectionAlt;

   // apply law of cosines to determine distance from the radar to the ionosphere at the appropriate incidence angle
   //   c^2 = a^2 + b^2 - 2*a*b*cos(C)
   double C = 0.5 * (mOTH_Geometry.mAngularExtent);

   double phiIncidence = C / UtMath::cRAD_PER_DEG;

   if (phiIncidence <= mIonosphericCharacteristics.mMaxIncidenceAngleDegrees)
   {
      canAttemptToDetect = true;
   }
   else if (mDebug)
   {
      auto out = ut::log::debug() << "Detection being prevented due to ionospheric conditions.";
      out.AddNote() << "Max Incidence Angle: " << mIonosphericCharacteristics.mMaxIncidenceAngleDegrees;
      out.AddNote() << "Required Incidence Angle: " << phiIncidence;
   }
   return canAttemptToDetect;
}

void WsfOTH_RadarSensor::OTH_Beam::ComputeBouncePathRanges(WsfSensorResult& aResult, double& aXmtrToTgt, double& aRcvrToTgt)
{
   double relVec[3];

   // Xmtr to Bounce Point
   UtVec3d::Subtract(relVec, aResult.mXmtrLoc.mLocWCS, mOTH_Geometry.mReflectionLocWCS);
   double xmtrToBouncePointRange = UtVec3d::Magnitude(relVec);

   // Rcvr to Bounce Point...Assume mono-static
   double rcvrToBouncePointRange = xmtrToBouncePointRange;

   if (aResult.mBistatic)
   {
      UtVec3d::Subtract(relVec, aResult.mRcvrLoc.mLocWCS, mOTH_Geometry.mReflectionLocWCS);
      rcvrToBouncePointRange = UtVec3d::Magnitude(relVec);
   }

   // Bounce Point to Target
   UtVec3d::Subtract(relVec, aResult.mTgtLoc.mLocWCS, mOTH_Geometry.mReflectionLocWCS);
   double bouncePointToTgtRange = UtVec3d::Magnitude(relVec);

   aXmtrToTgt = xmtrToBouncePointRange + bouncePointToTgtRange;
   aRcvrToTgt = rcvrToBouncePointRange + bouncePointToTgtRange;
}

void WsfOTH_RadarSensor::OTH_Beam::GetReflectionLocationWCS(double aReflectionLocationWCS[])
{
   UtVec3d::Set(aReflectionLocationWCS, mOTH_Geometry.mReflectionLocWCS);
}

// =================================================================================================
//! Computes clutter attenuation for radar with double delay line canceler.
//!
//! Routine is valid for radars both with and without a clutter lock feature.
//! Equations are take from Nathanson's "Radar Design Principles" Chapter 9.
//!
//! Taken from SalramClutter::ClutterAttenuation.
// private
void WsfOTH_RadarSensor::OTH_Beam::ComputeClutterAttenuationFactor(OTH_Mode& aMode)
{
   double              sigma;
   double              sigmav;
   static const double v0iss[2][7] = {{0.06, 0.36, 0.58, 0.95, 1.34, 1.65, 1.95},
                                      {0.06, 1.22, 1.80, 2.20, 2.46, 2.70, 2.92}};
   static const double sigvis[7]   = {0.01, 0.30, 0.62, 0.94, 1.25, 1.56, 1.80};

   WsfEnvironment& environment = GetPlatform()->GetScenario().GetEnvironment();
   int             seaState    = environment.GetSeaState();
   seaState                    = std::min(std::max(seaState, 0), 6);
   double integratedPulseCount = ComputeIntegratedPulseCount(aMode);

   if (mXmtrPtr->GetPlatform()->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SURFACE)
   {
      // Use Sea State 1 for now
      sigma = sigvis[seaState];
   }
   else
   {
      double windSpeed = environment.GetWindSpeed();
      double temp      = 1.29 * log10(windSpeed / 0.01);
      sigma            = 0.000018 * pow(10.0, temp);
   }

   double wavelength = UtMath::cLIGHT_SPEED / mXmtrPtr->GetFrequency();
   if (mAntennaPtr->GetScanMode() != WsfEM_Antenna::cSCAN_FIXED)
   {
      // Antenna scanning (not applicable in track mode)

      // Assume antenna scans in elevation and azimuth, assume 360 deg az scan sector.
      double sector = UtMath::cTWO_PI;
      if (mAntennaPtr->GetScanMode() == WsfEM_Antenna::cSCAN_AZ)
      {
         // Azimuth scans in azimuth only.
         double azMin;
         double azMax;
         mAntennaPtr->GetAzimuthScanLimits(azMin, azMax);
         sector = azMax - azMin;
      }
      else
      {
      }
      // cout << "sector=" << sector * UtMath::cDEG_PER_RAD << endl;
      double sigsca =
         ((1.0 / aMode.GetFrameTime()) * sector * wavelength) / (10.7 * mRcvrPtr->GetAzimuthBeamwidth(0.0, 0.0));
      sigmav = sqrt((sigma * sigma) + (sigsca * sigsca));
   }
   else
   {
      // Antenna Tracking
      sigmav = sigma;
   }

   double v0 = 0.0;
   if (mMTI_ClutterLock)
   {
      v0 = sigmav / sqrt(integratedPulseCount);
   }
   else if (mMTI_UpWind)
   {
      int ipol = (mXmtrPtr->GetPolarization() == WsfEM_Types::cPOL_HORIZONTAL) ? 1 : 0;
      v0       = v0iss[ipol][seaState];
   }

   // Compute limit on clutter attenuation resulting from clutter
   // motion and antenna scanning for a 2-stage canceler.
   // Use maximally flat weights (which reduce to binomial weights for
   // unstaggered systems

   // Weights not calculated here, using R. Hackett's results,
   // Weights are calculated in REFRANG, passed into program

   double temp;
   double temp1;
   double x1 = UtMath::cTWO_PI * sigmav / wavelength;
   double x2 = UtMath::cFOUR_PI * v0 / wavelength;

   temp         = x1 * mMTI_InterpulsePeriod_1;
   temp1        = cos(x2 * mMTI_InterpulsePeriod_1);
   double rhot1 = exp(-2.0 * temp * temp) * temp1;

   temp         = x1 * mMTI_InterpulsePeriod_2;
   temp1        = cos(x2 * mMTI_InterpulsePeriod_2);
   double rhot2 = exp(-2.0 * temp * temp) * temp1;

   temp           = x1 * (mMTI_InterpulsePeriod_1 + mMTI_InterpulsePeriod_2);
   temp1          = cos(x2 * (mMTI_InterpulsePeriod_1 + mMTI_InterpulsePeriod_2));
   double rhot1t2 = exp(-2.0 * temp * temp) * temp1;

   // ca is in units of power (volts^2)
   temp         = mMTI_Weights[0] * mMTI_Weights[0];
   temp1        = mMTI_Weights[1] * mMTI_Weights[1];
   double temp2 = mMTI_Weights[2] * mMTI_Weights[2];
   double ca =
      1.0 / ((temp + temp1 + temp2) + (2.0 * mMTI_Weights[0] * mMTI_Weights[1] * rhot1) +
             (2.0 * mMTI_Weights[1] * mMTI_Weights[2] * rhot2) + (2.0 * mMTI_Weights[0] * mMTI_Weights[2] * rhot1t2));

   // Compute limit on clutter attenuation resulting from  transmitter instability - stabconst
   // *****NOTE: Value is hardwired into present simulation*****
   // Determine overall clutter attenuation - cat & catdb
   //
   // The above NOTE was not correct. The user had to provide it in the input file...
   double stabilityConstant = (mMTI_StabilityConstant > 0.0) ? mMTI_StabilityConstant : 100.0;

   double cati               = (1.0 / sqrt(ca)) + (1.0 / sqrt(stabilityConstant));
   double cat                = 1.0 / cati;
   mClutterAttenuationFactor = 1.0 / (cat * cat);

   // cout << "WSF  clutter initialization" << endl;
   // cout << "sigma=" << sigma << endl;
   // cout << "sigmav=" << sigmav << endl;
   // cout << "npi=" << integratedPulseCount << endl;
   // cout << "v0=" << v0 << endl;
   // cout << "ca=" << ca << endl;
   // double catdb = UtMath::LinearToDB(cat * cat);           // 20.0 * log10(cat)
   // cout << "catdb=" << catdb << endl;
}

// =================================================================================================
//! Compute the integrated pulse count.
//!
//! This is only used to compute the integrated pulse count for clutter calculations.
//!
//! From SalramMode::Initialize.
// public
double WsfOTH_RadarSensor::OTH_Beam::ComputeIntegratedPulseCount(OTH_Mode& aMode)
{
   double pulseCount = 1.0; // Assuming continuous wave...
   if (mXmtrPtr->GetPulseRepetitionFrequency() != 0.0)
   {
      // Pulsed radar...

      double timeOnTarget = 0.0;
      if (aMode.GetDwellTime() == 0.0)
      {
         if (mAntennaPtr->GetScanMode() == WsfEM_Antenna::cSCAN_EL)
         {
            double minEl;
            double maxEl;
            mAntennaPtr->GetElevationScanLimits(minEl, maxEl);
            timeOnTarget = aMode.GetFrameTime() * mXmtrPtr->GetElevationBeamwidth(0.0, 0.0) / (maxEl - minEl);
         }
         else
         {
            double minAz;
            double maxAz;
            mAntennaPtr->GetAzimuthScanLimits(minAz, maxAz);
            timeOnTarget = aMode.GetFrameTime() * mXmtrPtr->GetAzimuthBeamwidth(0.0, 0.0) / (maxAz - minAz);
         }
      }
      else
      {
         // Tracker
         timeOnTarget = aMode.GetDwellTime();
      }
      pulseCount = timeOnTarget * mXmtrPtr->GetPulseRepetitionFrequency();

      // Calculate number of clutter pulses
      // TODO mXni = ComputeEffectivePulseIntegration(mNumPulseInt, GetPulseRepetitionFrequency(), timeOnTarget);
   }
#ifdef TODO
   else if (GetPulseDoppler())
   {
      // Pulse Doppler radar
      mExp = 1.0;

      // FIND TIME ON TARGET AND NUMBER OF SIGNAL PULSES INTEGRATED
      // New change, to allow for noncoherent integration of pulses in a Doppler radar.

      if (GetNumberPulseDopplerPulses() > 0.0)
      {
         mExp = exp1;

         // Pass number of pulses from namelist and sampling frequency to
         // calculate number of independent clutter pulses, note receiver
         // bandwidth is in MHz. convert to Hz to pass to CLUT_PULSES_INT
         // (later converted to Hz).

         timeOnTarget = GetNumberPulseDopplerPulses() / GetBurstRepetitionFrequency();
         mNumPulseInt = GetNumberPulseDopplerPulses();

         // Now get number of clutter pulses
         mXni = ComputeEffectivePulseIntegration(mNumPulseInt, GetReceiverBandWidth(), timeOnTarget);
      }
      else
      {
         // For a Doppler radar, don't call if only on pulse, use single pulse
         // for number of clutter pulses.
         timeOnTarget = 1.0 / GetBurstRepetitionFrequency();
         mNumPulseInt = 1.0;
         mXni         = 1.0;
      }
   }
#endif
   return pulseCount;
}

// =================================================================================================
//! Compute the clutter attenuation to be applied to the current interaction
//! @param aResult The current interaction.
//! @param aClutterAttenuationFactor The attenuation factor for the clutter signal. 0 for complete
//! attenuation and 1 for no attenuation.
//! Taken from SalramMode::ComputeProbabilityOfDetection and SalramMode::Initialize.
// private
void WsfOTH_RadarSensor::OTH_Beam::ComputeMTI_Effects(WsfSensorResult& aResult, double& aClutterAttenuationFactor)
{
   // TODO-CHECK should this be done ONLY if a clutter model is present???
   if (mClutterPtr == nullptr)
   {
      aClutterAttenuationFactor = 1.0;
      return;
   }

   // Select the clutter attenuation factor.
   // This can be the result of MTI, Doppler, clutter maps or other means.

   aClutterAttenuationFactor  = mClutterAttenuationFactor;
   bool performMTI_Processing = mPerformMTI_Processing;
   if (mPerformMTI_Processing)
   {
      // Approximation to ground range.
      double cosTheta    = cos(aResult.mRcvrToTgt.mTrueEl);
      double groundRange = aResult.mRcvrToTgt.mRange * cosTheta;
      if (groundRange > mMTI_MaxRange)
      {
         aClutterAttenuationFactor = 1.0; // No attenuation
         performMTI_Processing     = false;
      }
   }

   // Perform MTI processing only if MTI radar and within the range limits for MTI processing.

   if (performMTI_Processing) // MTI radar and within MTI maximum_range
   {
      // Compute the Doppler frequency.
      double relWCS[3];
      UtVec3d::Subtract(relWCS, aResult.mTgtLoc.mLocWCS, aResult.mRcvrLoc.mLocWCS);
      double targetVelWCS[3];
      aResult.GetTarget()->GetVelocityWCS(targetVelWCS);
      double radialVelocity = UtVec3d::DotProduct(targetVelWCS, relWCS);
      radialVelocity /= std::max(aResult.mXmtrToTgt.mRange, 1.0);
      radialVelocity = fabs(radialVelocity);
      double fc      = (2.0 * radialVelocity) / (UtMath::cLIGHT_SPEED / mXmtrPtr->GetFrequency());

      double prf[2];
      int    numPRFs = 1;

      if (mMTI_InterpulsePeriod_1 != mMTI_InterpulsePeriod_2)
      {
         numPRFs = 2;
         prf[0]  = 1.0 / mMTI_InterpulsePeriod_1;
         prf[1]  = 1.0 / mMTI_InterpulsePeriod_2;
      }

      // Find target response (TARGRES) for pulse radar with MTI turned on
      double targetResponse = 0.0;
      if (numPRFs == 1)
      {
         // Use Nathanson method to compute MTI response for nonstaggered multiple delay line system
         double pulse_rep_freq = mXmtrPtr->GetPulseRepetitionFrequency() != 0.0 ? mXmtrPtr->GetPulseRepetitionFrequency() :
                                                                                  1.0 / mMTI_InterpulsePeriod_1;
         double ratio          = fc / pulse_rep_freq;
         double temp1          = 2.0 * mMTI_NumberOfStages;
         double temp2          = sin(UtMath::cPI * ratio);
         targetResponse        = pow(2.0, temp1) * pow(temp2, temp1);
      }
      else
      {
         // Weights not calculated here, using R. Hackett's results, weights
         // are calculated in ReferenceRange
         double omegaD = UtMath::cTWO_PI * fc;
         double sum1   = mMTI_Weights[0];
         double sum2   = 0.0;
         double delay  = 0.0;
         for (int j = 0; j < numPRFs; ++j)
         {
            delay           = delay + (1.0 / prf[j]);
            double angle    = omegaD * delay;
            double sinAngle = sin(angle);
            double cosAngle = cos(angle);
            sum1            = sum1 + mMTI_Weights[j + 1] * cosAngle;
            sum2            = sum2 + mMTI_Weights[j + 1] * sinAngle;
         }
         targetResponse = (sum1 * sum1) + (sum2 * sum2);
      }
      targetResponse = std::max(targetResponse, 1.0E-6);

      // Adjust the received power by the 'MTI improvement'
      aResult.mRcvdPower *= targetResponse;

      // Determine average power gain of the MTI system to account for the system noise
      // passing through the MTI Dual Delay Line Canceler.

      // Calculate noise power gain at output of canceler.
      // NO/NI = W1**2 + W2**2 + W3**2 + ...

      double receiverNoiseGain =
         mMTI_Weights[0] * mMTI_Weights[0] + mMTI_Weights[1] * mMTI_Weights[1] + mMTI_Weights[2] * mMTI_Weights[2];
      aResult.mRcvrNoisePower *= receiverNoiseGain;
   }
}

// =================================================================================================
bool WsfOTH_RadarSensor::OTH_Beam::Initialize(double       aSimTime,
                                              unsigned int aBeamIndex,
                                              WsfSensor*   aSensorPtr,
                                              OTH_Mode*    aModePtr,
                                              bool         aCanTransmit,
                                              bool         aCanReceive,
                                              bool         aShowCalibrationData,
                                              bool         aIsMultiBeam)
{
   bool ok = true;

   // Propagate the debug flag
   if ((aSensorPtr->GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG) != 0u)
   {
      mRcvrPtr->SetDebugEnabled(aSensorPtr->DebugEnabled());
      mXmtrPtr->SetDebugEnabled(aSensorPtr->DebugEnabled());
   }

   mXmtrPtr->UseDefaultFrequency(aSensorPtr->GetScenario().GetSimulationInput().UseDefaultFrequency());

   mXmtrPtr->SetMode(aModePtr); // Identify the xmtr with the mode and beam index.
   mXmtrPtr->SetIndex(aBeamIndex);
   mRcvrPtr->SetMode(aModePtr); // Identify the rcvr with the mode and beam index.
   mRcvrPtr->SetIndex(aBeamIndex);
   mCanTransmit = aCanTransmit;

   ok &= mAntennaPtr->Initialize(aSensorPtr);
   if (aCanTransmit)
   {
      ok &= mXmtrPtr->Initialize(*aSensorPtr->GetSimulation());
   }

   if (ok && aCanReceive)
   {
      ok &= mRcvrPtr->Initialize(*aSensorPtr->GetSimulation());

      // In case the bandwidth was not set for the noise calculation,
      // set the bandwidth using the pulsewidth of the linked transmitter.
      mRcvrPtr->UpdateNoisePower(mXmtrPtr->GetPulseWidth());

      if (mUseDetector)
      {
         mDetector.Initialize(0.0, aModePtr, aBeamIndex);
      }
      else if (mProbabilityTablePtr)
      {
         mProbabilityTablePtr->Initialize(0.0, aModePtr, aBeamIndex);
      }

      ok &= InitializeMTI(*aModePtr);

      if ((!GetSignalProcessors().Empty()) && ((mOneM2DetectRange > 0.0) || (mLoopGain > 0.0)))
      {
         ut::log::warning()
            << "Signal_processor should not be used with one_m2_detect_range, range_product or loop_gain.";
      }


      // Load and initialize the clutter model object.

      if (!mClutterType.IsNull())
      {
         mClutterPtr = WsfEM_ClutterTypes::Get(aSensorPtr->GetScenario()).Clone(mClutterType);
         if (mClutterPtr == nullptr)
         {
            auto out = ut::log::error() << "clutter_model has not been defined.";
            out.AddNote() << "clutter_model: " << mClutterType;
            ok = false;
         }
         else if (mClutterPtr->IsNullModel())
         {
            // The referenced model is a 'null' (no-effect) model - delete it to save processing time.
            delete mClutterPtr;
            mClutterPtr = nullptr;
         }
         else if (!mClutterPtr->Initialize(GetEM_Rcvr()))
         {
            ut::log::error() << "Failed to initialize clutter_model.";
            ok = false;
         }
      }
   }


   // Initialize the Noise
   ok &= mNoise.Initialize();

   // Check to see if the ionospheric inputs are valid
   ok &= AreIonosphericParametersValid();

   if (ok)
   {
      // Compute the initial ionospheric conditions and limits
      ComputeIonosphericCharacteristics();

      if (GetIonosphericRangeConstrained())
      {
         // Check to see if user defined range limits exist. If so, print a warning that they are about to be
         // reset to computed values.
         if (mAntennaPtr->GetMinimumRange() != 0.0 || mAntennaPtr->GetMaximumRange() != DBL_MAX)
         {
            auto out = ut::log::warning()
                       << "OTH Ionospheric Constrained Range Computation will override defined min/max Range Limits.";
            out.AddNote() << "Sensor: " << aSensorPtr->GetName();
            out.AddNote() << "Sensor Type: " << aSensorPtr->GetType();
         }

         double min;
         double max;
         GetIonosphericRangeLimits(min, max);
         mAntennaPtr->SetRangeLimits(min, max);
      }
   }

   if (ok && aCanTransmit && aCanReceive)
   {
      if (aShowCalibrationData)
      {
         auto out = ut::log::info() << "Calibration Data:";
         out.AddNote() << "Platform: " << aSensorPtr->GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << aSensorPtr->GetName();
         out.AddNote() << "Mode: " << aModePtr->GetName();
         if (aIsMultiBeam)
         {
            out.AddNote() << "Beam: " << aBeamIndex + 1;
         }
      }
      Calibrate(aShowCalibrationData);
   }
   return ok;
}

// =================================================================================================
// private
bool WsfOTH_RadarSensor::OTH_Beam::InitializeDetector(OTH_Mode& aMode)
{
   // If a statistical detection is being used, compute the nominal detection threshold and the integration gain.
   if (mProbabilityTablePtr || mUseDetector)
   {
      double requiredPd = aMode.GetRequiredPd(); // Get the required Pd as entered.
      if ((requiredPd <= 0.0) || (requiredPd > 1.0))
      {
         requiredPd = 0.5;
      }
      requiredPd = std::min(std::max(requiredPd, 0.002), 0.998);

      // Make two passes through the following loop. The first pass uses the 'number_of_pulses_integrated'
      // as defined by the user. This produces the pre-integration signal-to-noise for the desired Pd. The
      // second pass uses the 'number_of_pulses_integrated' equal to one. This produces the post-integration
      // signal-to-noise for the desired Pd. From these two numbers we compute the effective integration gain.
      //
      // Note that the detector is left with the number_of_pulses_integrated set to one. The computed integration
      // gain is stored in the beam and used to adjust the non-integrated signal-to-noise to explicitly produce an
      // integrated signal-to-noise which is then fed to the detector (which does no integration).
      //
      // The reason for all of this is that the error model should use post-integration signal levels rather than
      // pre-integration signal levels. If this isn't done, the smaller pre-integration signal will be used by
      // the error model and will result in larger errors in the track, and very erratic tracks from the filters.

      double singlePulseThreshold     = 0.0;
      double multiPulseThreshold      = 0.0;
      int    numberOfPulsesIntegrated = mNumberOfPulsesIntegrated;
      mDetector.SetNumberOfPulsesIntegrated(numberOfPulsesIntegrated);
      for (int pass = 1; pass <= 2; ++pass)
      {
         // Find the signal to noise the generates a Pd equal to the required Pd.
         double loSignalToNoise    = 0.0;
         double hiSignalToNoise    = 1000.0;
         double detectionThreshold = 0.0;
         while (fabs(hiSignalToNoise - loSignalToNoise) > 0.001)
         {
            detectionThreshold = 0.5 * (loSignalToNoise + hiSignalToNoise);
            double pd;
            if (mProbabilityTablePtr)
            {
               pd = mProbabilityTablePtr->ComputeProbabilityOfDetection(detectionThreshold);
            }
            else
            {
               pd = mDetector.ComputeProbabilityOfDetection(detectionThreshold);
            }
            if (fabs(pd - requiredPd) < 0.001)
            {
               break;
            }
            else if (pd < requiredPd)
            {
               loSignalToNoise = detectionThreshold;
            }
            else
            {
               hiSignalToNoise = detectionThreshold;
            }
         }
         // Assume this is the final pass - capture the single pulse threshold.
         singlePulseThreshold = detectionThreshold;
         if (pass == 1)
         {
            // On first pass capture the multiple-pulse threshold (which is the same as
            // the single-pulse threshold if multiple pulses are not being integrated).
            multiPulseThreshold = detectionThreshold;
            mDetector.SetNumberOfPulsesIntegrated(1);
            if ((numberOfPulsesIntegrated == 1) || mProbabilityTablePtr)
            {
               break; // Bypass second loop if not integrating multiple pulses
            }
         }
      }
      mRcvrPtr->SetDetectionThreshold(singlePulseThreshold);
      mIntegrationGain = singlePulseThreshold / multiPulseThreshold;
   }
   return true;
}

// =================================================================================================
// private
bool WsfOTH_RadarSensor::OTH_Beam::InitializeMTI(OTH_Mode& aMode)
{
   // Override the attenuation factor with a computed one if commanded
   if (mPerformMTI_Processing)
   {
      // Ensure a PRF was specified.
      if (mXmtrPtr->GetPulseRepetitionFrequency() <= 0.0)
      {
         ut::log::error() << "'pulse_repetition_frequency' must be specified with 'mti_processor'.";
         return false;
      }

      // Ensure the maximum MTI range was specified
      if (mMTI_MaxRange <= 0.0)
      {
         ut::log::error() << "'maximum_range' must be specified in 'mti_processor'.";
         return false;
      }

      // Compute the weights based upon the interpulse_periods entered into the block.
      // If no interpulse_periods have been entered, then the use the inverse of the PRF.

      if ((mMTI_InterpulsePeriod_1 == 0.0) && (mMTI_InterpulsePeriod_2 == 0.0))
      {
         // Set the interpulse periods to the inverse of the PRF (Non-staggered MTI system).
         mMTI_InterpulsePeriod_1 = 1.0 / mXmtrPtr->GetPulseRepetitionFrequency();
         mMTI_InterpulsePeriod_2 = mMTI_InterpulsePeriod_1;
      }
      else if ((mMTI_InterpulsePeriod_1 == 0.0) || (mMTI_InterpulsePeriod_2 == 0.0))
      {
         ut::log::error() << "'interpulse_period_1 and 2' must both be specified in 'mti_processor'.";
         return false;
      }

      mMTI_Weights[0] = 1.0;
      mMTI_Weights[1] = -(mMTI_InterpulsePeriod_1 + mMTI_InterpulsePeriod_2) / mMTI_InterpulsePeriod_2;
      mMTI_Weights[2] = mMTI_InterpulsePeriod_1 / mMTI_InterpulsePeriod_2;

      // Compute the clutter attenuation factor if not explicitly specified.
      if (mClutterAttenuationFactor == 0.0)
      {
         ComputeClutterAttenuationFactor(aMode);
      }
   }
   else
   {
      // Non-MTI radar. If clutter_attenuation_factor was not specified then set the default.
      if (mClutterAttenuationFactor == 0.0)
      {
         mClutterAttenuationFactor = 1.0;
      }
   }
   return true;
}

// =================================================================================================
bool WsfOTH_RadarSensor::OTH_Beam::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (mAntennaPtr->ProcessInput(aInput) || mXmtrPtr->ProcessInputBlock(aInput) || mRcvrPtr->ProcessInputBlock(aInput))
   {
   }
   else if (WsfEM_ClutterTypes::Get(WsfScenario::FromInput(aInput)).LoadReference(aInput, mClutterType))
   {
   }
   else if (command == "noise_environment")
   {
      mNoise.ProcessInput(aInput);
      mComputeAtmosphericNoise = true;
   }
   else if (command == "integration_gain")
   {
      aInput.ReadValueOfType(mIntegrationGain, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(mIntegrationGain, 1.0);
      mUseDetector = false;
   }
   else if (command == "adjustment_factor")
   {
      aInput.ReadValueOfType(mAdjustmentFactor, UtInput::cRATIO);
      aInput.ValueGreater(mAdjustmentFactor, 0.0);
   }
   else if (command == "operating_loss")
   {
      // For compatibility with SUPPRESSOR. Just another name for the reciprocal of the 'adjustment_factor'
      double operatingLoss;
      aInput.ReadValueOfType(operatingLoss, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(operatingLoss, 1.0);
      mAdjustmentFactor = 1.0 / operatingLoss;
   }
   else if (command == "detection_threshold")
   {
      // The receiver also has a detection threshold. It is also put here so the
      // 'integration_gain' and 'detection_threshold' can be grouped together by the user
      // in a logical fashion.
      double detectionThreshold;
      aInput.ReadValueOfType(detectionThreshold, UtInput::cRATIO);
      aInput.ValueGreater(detectionThreshold, 0.0);
      mRcvrPtr->SetDetectionThreshold(detectionThreshold);
      mUseDetector         = false;
      mProbabilityTablePtr = std::shared_ptr<wsf::DetectionProbabilityTable>(nullptr);
   }
   else if (command == "post_lockon_detection_threshold_adjustment")
   {
      aInput.ReadValueOfType(mPostLockonDetectionThresholdAdjustment, UtInput::cRATIO);
      aInput.ValueGreater(mPostLockonDetectionThresholdAdjustment, 0.0);
   }
   else if (command == "post_lockon_adjustment_delay_time")
   {
      aInput.ReadValueOfType(mPostLockonAdjustmentDelayTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mPostLockonAdjustmentDelayTime, 0.0);
   }
   else if (command == "number_of_pulses_integrated")
   {
      aInput.ReadValue(mNumberOfPulsesIntegrated);
      aInput.ValueGreater(mNumberOfPulsesIntegrated, 0);
   }
   else if (command == "detection_probability")
   {
      mProbabilityTablePtr = std::make_shared<wsf::DetectionProbabilityTable>();
      mProbabilityTablePtr->ReadTable(aInput);
      mUseDetector = false;
   }
   else if (command == "detector_law")
   {
      std::string word;
      aInput.ReadValue(word);
      if (word == "linear")
      {
         mDetector.SetDetectorLaw(wsf::MarcumSwerling::cDL_LINEAR);
      }
      else if (word == "square")
      {
         mDetector.SetDetectorLaw(wsf::MarcumSwerling::cDL_SQUARE);
      }
      else if (word == "log")
      {
         mDetector.SetDetectorLaw(wsf::MarcumSwerling::cDL_LOG);
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "probability_of_false_alarm")
   {
      double probabilityOfFalseAlarm;
      aInput.ReadValue(probabilityOfFalseAlarm);
      aInput.ValueInClosedRange(probabilityOfFalseAlarm, 0.0, 1.0);
      mDetector.SetProbabilityOfFalseAlarm(probabilityOfFalseAlarm);
   }
   else if (command == "swerling_case")
   {
      int swerlingCase;
      aInput.ReadValue(swerlingCase);
      aInput.ValueInClosedRange(swerlingCase, 0, 4);
      mDetector.SetCase(swerlingCase);
      mUseDetector         = true;
      mProbabilityTablePtr = std::shared_ptr<wsf::DetectionProbabilityTable>(nullptr);
   }
   else if (command == "no_swerling_case")
   {
      mUseDetector         = false;
      mProbabilityTablePtr = std::shared_ptr<wsf::DetectionProbabilityTable>(nullptr);
   }
   else if (command == "error_model_parameters")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         if (command == "azimuth_beamwidth")
         {
            aInput.ReadValueOfType(mErrorModelAzBeamwidth, UtInput::cANGLE);
            aInput.ValueGreater(mErrorModelAzBeamwidth, 0.0);
         }
         else if (command == "elevation_beamwidth")
         {
            aInput.ReadValueOfType(mErrorModelElBeamwidth, UtInput::cANGLE);
            aInput.ValueGreater(mErrorModelElBeamwidth, 0.0);
         }
         else if (command == "pulse_width")
         {
            aInput.ReadValueOfType(mErrorModelPulseWidth, UtInput::cTIME);
            aInput.ValueGreater(mErrorModelPulseWidth, 0.0);
         }
         else if (command == "receiver_bandwidth")
         {
            // Derive the pulse width assuming a matched filter.
            double bandwidth;
            aInput.ReadValueOfType(bandwidth, UtInput::cFREQUENCY);
            aInput.ValueGreater(bandwidth, 0.0);
            mErrorModelPulseWidth = 1.0 / bandwidth;
         }
         else if (command == "doppler_resolution")
         {
            aInput.ReadValueOfType(mErrorModelDopplerResolution, UtInput::cSPEED);
            aInput.ValueGreater(mErrorModelDopplerResolution, 0.0);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if ((command == "azimuth_error_factor") || (command == "elevation_error_factor") ||
            (command == "range_error_factor") || (command == "angle_error_factor"))
   {
      ut::log::warning() << "'" << command << "' has been deprecated.";
   }
   else if (command == "one_m2_detect_range")
   {
      aInput.ReadValueOfType(mOneM2DetectRange, UtInput::cLENGTH);
      aInput.ValueGreater(mOneM2DetectRange, 0.0);
      mLoopGain = 0.0; // This overrides loop_gain
   }
   else if (command == "range_product")
   {
      // range_product = rcs * detect_range^4
      double rangeProduct;
      aInput.ReadValueOfType(rangeProduct, UtInput::cAREA_DB);
      aInput.ValueGreater(rangeProduct, 0.0);
      mOneM2DetectRange = pow(rangeProduct, 0.25);
      mLoopGain         = 0.0; // This overrides loop_gain
   }
   else if (command == "loop_gain")
   {
      aInput.ReadValueOfType(mLoopGain, UtInput::cRATIO);
      aInput.ValueGreater(mLoopGain, 0.0);
      mOneM2DetectRange = 0.0; // This overrides one_m2_detect_range
   }
   else if (command == "look_down_factor")
   {
      aInput.ReadValueOfType(mLookDownFactor, UtInput::cRATIO);
   }
   else if (command == "prf_factor")
   {
      aInput.ReadValueOfType(mPRF_Factor, UtInput::cRATIO);
   }
   else if (command == "mti_processor")
   {
      if (mMTI_AdjustmentTablePtr)
      {
         ut::log::error()
            << "WsfOTH_RadarSensor::RadarBeam::ProcessInput: An MTI adjustment table has already been specified.";
         throw UtInput::BadValue(aInput, "3: An MTI adjustment table has already been specified!");
      }

      mPerformMTI_Processing = true;
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         if (command == "maximum_range")
         {
            aInput.ReadValueOfType(mMTI_MaxRange, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(mMTI_MaxRange, 0.0);
         }
         else if (command == "number_of_stages")
         {
            aInput.ReadValue(mMTI_NumberOfStages);
            aInput.ValueGreaterOrEqual(mMTI_NumberOfStages, 1);
         }
         else if (command == "interpulse_period_1")
         {
            aInput.ReadValueOfType(mMTI_InterpulsePeriod_1, UtInput::cTIME);
            aInput.ValueGreaterOrEqual(mMTI_InterpulsePeriod_1, 0.0);
         }
         else if (command == "interpulse_period_2")
         {
            aInput.ReadValueOfType(mMTI_InterpulsePeriod_2, UtInput::cTIME);
            aInput.ValueGreaterOrEqual(mMTI_InterpulsePeriod_2, 0.0);
         }
         else if (command == "clutter_lock")
         {
            aInput.ReadValue(mMTI_ClutterLock);
         }
         else if (command == "stability_constant")
         {
            aInput.ReadValue(mMTI_StabilityConstant);
            aInput.ValueGreater(mMTI_StabilityConstant, 0.0);
         }
         else if (command == "upwind")
         {
            aInput.ReadValue(mMTI_UpWind);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "solar_characteristics")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         if (command == "hour_of_day")
         {
            aInput.ReadValue(mSolarCharacteristics.mHourOfDay);
            aInput.ValueInClosedRange(mSolarCharacteristics.mHourOfDay, 1, 24);
         }
         else if (command == "day_of_year")
         {
            aInput.ReadValue(mSolarCharacteristics.mDayOfYear);
            aInput.ValueInClosedRange(mSolarCharacteristics.mDayOfYear, 1, 365);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "ionospheric_characteristics")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         if (command == "electron_temperature")
         {
            aInput.ReadValue(mIonosphericCharacteristics.mTemperatureAtMax);
         }
         else if (command == "electron_density_at_max")
         {
            aInput.ReadValue(mIonosphericCharacteristics.mElectronDensityAtMax);
         }
         else if (command == "electron_height_at_max")
         {
            aInput.ReadValueOfType(mIonosphericCharacteristics.mElectronHeightAtMax, UtInput::cLENGTH);
         }
         else if (command == "reflection_height")
         {
            aInput.ReadValueOfType(mIonosphericCharacteristics.mReflectionHeight, UtInput::cLENGTH);
         }
         else if (command == "ionosphere_constrains_minimum_range")
         {
            aInput.ReadValue(mIonosphericCharacteristics.mRangeConstrained);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else
   {
      myCommand = WsfSensorBeam::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
void WsfOTH_RadarSensor::OTH_Beam::ReadTable(UtInput& aInput, WsfMTI_TableManager::MTI_AdjustmentTable& aMTI_AdjustmentTable)
{
   UtInputBlock inputBlock(aInput);

   std::string         command;
   std::vector<double> speedFrequencyValues;
   std::vector<double> adjustmentValues;
   bool                usedClosingSpeed     = false;
   bool                usedDopplerFrequency = false;
   while (inputBlock.ReadCommand(command))
   {
      double closingValue = 0.0;
      if (command == "closing_speed")
      {
         if (usedDopplerFrequency)
         {
            throw UtInput::OutOfContext(aInput, "Cannot use closing_speed and doppler_frequency in the same table");
         }
         usedClosingSpeed = true;
         aInput.ReadValueOfType(closingValue, UtInput::cSPEED);
         if ((!speedFrequencyValues.empty()) && (closingValue <= speedFrequencyValues.back()))
         {
            throw UtInput::BadValue(aInput, "closing_speed values must be monotonically ascending");
         }
      }
      else if (command == "doppler_frequency")
      {
         if (usedClosingSpeed)
         {
            throw UtInput::OutOfContext(aInput, "Cannot use closing_speed and doppler_frequency in the same table");
         }
         usedDopplerFrequency = true;
         aInput.ReadValueOfType(closingValue, UtInput::cFREQUENCY);
         if ((!speedFrequencyValues.empty()) && (closingValue <= speedFrequencyValues.back()))
         {
            throw UtInput::BadValue(aInput, "doppler_frequency values must be monotonically ascending");
         }
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }

      // Process the part after 'closing_speed' or 'doppler_frequency'

      std::string adjustmentStr;
      aInput.ReadValue(adjustmentStr);
      aInput.StringEqual(adjustmentStr, "adjustment");

      // UtInput will take the text dB value and by unit returns the linear equivilan,
      // which needs to be converted back to dB before storage.
      double adjustment;
      aInput.ReadValueOfType(adjustment, UtInput::cRATIO);
      aInput.ValueGreater(adjustment, 0.0);
      adjustment = UtMath::LinearToDB(adjustment);

      speedFrequencyValues.push_back(closingValue);
      adjustmentValues.push_back(adjustment);
   }

   if (speedFrequencyValues.size() < 2)
   {
      throw UtInput::BadValue(aInput, "mti_adjustment table must have at least two entries");
   }

   // aMTI_AdjustmentTable.SetDopplerFrequency(usedDopplerFrequency);
   // mSpeedFrequencyTable.SetValues(speedFrequencyValues);
   // mAdjustmentTable.SetValues(adjustmentValues);
   aMTI_AdjustmentTable.SetDopplerFrequency(usedDopplerFrequency);
   aMTI_AdjustmentTable.GetSpeedFrequencyTable().SetValues(speedFrequencyValues);
   aMTI_AdjustmentTable.GetAdjustmentTable().SetValues(adjustmentValues);
}

WsfPlatform* WsfOTH_RadarSensor::OTH_Beam::GetPlatform()
{
   return mAntennaPtr->GetPlatform();
}


// =================================================================================================
// Nested class WsfOTH_RadarSensor::RadarMode.

WsfOTH_RadarSensor::OTH_Mode::OTH_Mode()
   : WsfSensorMode()
   , mOverrideMeasurementWithTruth(false)
   , mCanTransmit(true)
   , mCanReceive(true)
   , mImplicitBeamUsed(false)
   , mExplicitBeamUsed(false)
   , mBeamList()
   , mAltFreqSelectDelay(0.0)
   , mAltFreqChangeScheduled(false)
   , mLastAltFreqSelectTime(0.0)
   , mIsFrequencyAgile(false)
{
   SetErrorModel(ut::make_unique<wsf::OTH_RadarSensorErrorModel>());
   mBeamList.push_back(new OTH_Beam());
   mBeamList.back()->SetBeamOwner(this);

   SetCapabilities(cALL ^ cPULSEWIDTH ^ cFREQUENCY ^ cPULSE_REPITITION_INTERVAL);
}

// =================================================================================================
WsfOTH_RadarSensor::OTH_Mode::OTH_Mode(const OTH_Mode& aSrc)
   : WsfSensorMode(aSrc)
   , mOverrideMeasurementWithTruth(aSrc.mOverrideMeasurementWithTruth)
   , mCanTransmit(aSrc.mCanTransmit)
   , mCanReceive(aSrc.mCanReceive)
   , mImplicitBeamUsed(aSrc.mImplicitBeamUsed)
   , mExplicitBeamUsed(aSrc.mExplicitBeamUsed)
   , mBeamList(aSrc.mBeamList)
   , mAltFreqSelectDelay(aSrc.mAltFreqSelectDelay)
   , mAltFreqChangeScheduled(aSrc.mAltFreqChangeScheduled)
   , mLastAltFreqSelectTime(aSrc.mLastAltFreqSelectTime)
   , mIsFrequencyAgile(aSrc.mIsFrequencyAgile)
{
   for (auto& beam : mBeamList)
   {
      beam = new OTH_Beam(*beam);
   }
}

// =================================================================================================
WsfOTH_RadarSensor::OTH_Mode::~OTH_Mode()
{
   for (auto& beamPtr : mBeamList)
   {
      delete beamPtr;
   }
}

// =================================================================================================
// virtual
WsfMode* WsfOTH_RadarSensor::OTH_Mode::Clone() const
{
   return new OTH_Mode(*this);
}

// =================================================================================================
// virtual
bool WsfOTH_RadarSensor::OTH_Mode::Initialize(double aSimTime)
{
   // If the mode is a transmit-only mode then force some sort of reporting to be present. Otherwise the base
   // class will indicate an error because the sensor doesn't report anything.
   if (!mCanTransmit)
   {
      mReportingFlags.mReportsOther = true;
   }

   bool ok = WsfSensorMode::Initialize(aSimTime);

   // Initialize each of the beams
   mMaximumRange     = 0.0;
   mIsFrequencyAgile = false; // reset for each instance
   bool isMultiBeam  = (mBeamList.size() > 1);
   for (unsigned int beamIndex = 0; beamIndex < mBeamList.size(); ++beamIndex)
   {
      OTH_Beam& beam = *mBeamList[beamIndex];
      ok &= beam.Initialize(aSimTime,
                            beamIndex,
                            mSensorPtr,
                            this,
                            mCanTransmit,
                            mCanReceive,
                            mSensorPtr->ShowCalibrationData(),
                            isMultiBeam);

      mMaximumRange = std::max(mMaximumRange, beam.mAntennaPtr->GetMaximumRange());

#ifdef TODO_JAJ
      // TODO - May not need to check if beam can perceive jamming to set agility flag.
      WsfEW_EA_EP* epPtr = WsfEW_EP::GetElectronicProtect(*(beam.mRcvr));
      if ((!mIsFrequencyAgile) && mCanTransmit && (beam.mXmtrPtr->GetAlternateFrequencyCount() > 0) &&
          beam.CanPerceiveJamming() && (epPtr != 0))
      {
         mIsFrequencyAgile = epPtr->HasFrequencyAgilityEffect();
      }
#endif
   }

   if (ok)
   {
      WsfSimulation& sim = *mSensorPtr->GetSimulation();
      if (mCanTransmit && sim.RandomizeFrequency())
      {
         int id =
            mSensorPtr->GetRandom().Uniform(0, std::max(0, mBeamList[0]->mXmtrPtr->GetAlternateFrequencyCount() - 1));
         SelectAlternateFrequency(aSimTime, id);
      }
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfOTH_RadarSensor::OTH_Mode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   // An explicit 'beam' command can be used only if commands have not been applied to the implicit beam.
   // Commands for the implicit 'beam' cannot be used if an explicit beam has been defined.
   if (command == "beam")
   {
      if (mImplicitBeamUsed)
      {
         std::string msg = "'beam' cannot be used in this context; command " + sLastImplicitBeamCommand +
                           " must be moved inside the 'beam' definition.";
         throw UtInput::BadValue(aInput, msg);
      }
      mExplicitBeamUsed = true;
      UtInputBlock inputBlock(aInput);

      // Beam 1 is implicitly created.  Subsequent beams are created by using a beam number
      // that is one higher than the last beam number. The initial values for the beam
      // are copied from the first beam.

      int beamNumber;
      aInput.ReadValue(beamNumber);
      aInput.ValueInClosedRange(beamNumber, 1, static_cast<int>(mBeamList.size() + 2));
      unsigned int beamIndex = static_cast<unsigned int>(beamNumber - 1);
      if (beamIndex == mBeamList.size())
      {
         mBeamList.push_back(new OTH_Beam(*mBeamList[0]));
         mBeamList.back()->SetBeamIndex(beamIndex);
      }

      while (inputBlock.ReadCommand())
      {
         if (!mBeamList[beamIndex]->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (mBeamList[0]->ProcessInput(aInput))
   {
      if (mExplicitBeamUsed)
      {
         throw UtInput::BadValue(aInput, "Implicit 'beam' commands cannot be used if an explicit 'beam' has been defined");
      }
      mImplicitBeamUsed        = true;
      sLastImplicitBeamCommand = aInput.GetCommand();
   }
   else if (command == "override_measurement_with_truth")
   {
      aInput.ReadValue(mOverrideMeasurementWithTruth);
   }
   else if (command == "receive_only")
   {
      mCanReceive  = true;
      mCanTransmit = false;
   }
   else if (command == "transmit_only")
   {
      mCanReceive  = false;
      mCanTransmit = true;
   }
   else if (command == "frequency_select_delay")
   {
      aInput.ReadValueOfType(mAltFreqSelectDelay, UtInput::cTIME);
   }
   else
   {
      myCommand = WsfSensorMode::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOTH_RadarSensor::OTH_Mode::AttemptToDetect(double           aSimTime,
                                                   WsfPlatform*     aTargetPtr,
                                                   Settings&        aSettings,
                                                   WsfSensorResult& aResult)
{
   bool            detected = false;
   WsfSensorResult result;
   result.Reset(aSettings);
   result.SetCategory(GetSensor()->GetZoneAttenuationModifier());
   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current
   aTargetPtr->Update(aSimTime);          // Ensure the target position is current

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Attempting to detect target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Mode: " << GetName();
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   // Determine which beam returns the best results...
   if (result.mFailedStatus == 0)
   {
      // Determine if concealed (like in a building).
      result.mCheckedStatus |= WsfSensorResult::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         result.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
         // Must have object pointers so event_output and debug output show locations.
         result.BeginGenericInteraction(mBeamList[0]->GetEM_Xmtr(), aTargetPtr, mBeamList[0]->GetEM_Rcvr());
      }

      mBeamList[0]->AttemptToDetect(aSimTime, aTargetPtr, aSettings, result);
      GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, result);
      GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, result);
      if (result.GetTransmitter() != nullptr)
      {
         result.GetTransmitter()->NotifyListeners(aSimTime, result); // Notify listeners for possible ESM detection
      }
      auto out = ut::log::debug();
      if (GetSensor()->DebugEnabled())
      {
         out << "Attempt to detect interaction.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << GetSensor()->GetName();
         out.AddNote() << "Mode: " << GetName();
         out.AddNote() << "Target: " << aTargetPtr->GetName();
         if (mBeamList.size() > 1)
         {
            auto note = out.AddNote() << "Beam: 1";
            result.Print(note);
         }
         else
         {
            result.Print(out);
         }
      }
      if (mBeamList.size() > 1)
      {
         WsfSensorResult tempResult;
         for (unsigned int beamIndex = 1; beamIndex < mBeamList.size(); ++beamIndex)
         {
            tempResult.Reset(aSettings);
            tempResult.mBeamIndex = beamIndex;
            mBeamList[beamIndex]->AttemptToDetect(aSimTime, aTargetPtr, aSettings, tempResult);
            GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, tempResult);
            GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, tempResult);
            if (tempResult.GetTransmitter() != nullptr)
            {
               tempResult.GetTransmitter()->NotifyListeners(aSimTime,
                                                            tempResult); // Notify listeners for possible ESM detection
            }
            if (GetSensor()->DebugEnabled())
            {
               auto note = out.AddNote() << "Beam: " << beamIndex + 1;
               tempResult.Print(note);
            }

            if (tempResult.mSignalToNoise > result.mSignalToNoise)
            {
               // If any of the previous beams detected jamming set the temp result to perceive jamming.
               result = tempResult;
            }
         }
      }

      // A successful detection occurs only if the beam with the best signal-to-noise had no failing criteria.
      detected = (result.mFailedStatus == 0);
   }
   else
   {
      GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, result);
      if (GetSensor()->DebugEnabled())
      {
         auto logDebug = ut::log::debug() << "Sensor Debug Info.";
         result.Print(logDebug);
      }
   }
   aResult = result;
   detected &= WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);
   return detected;
}

// =================================================================================================
// virtual
void WsfOTH_RadarSensor::OTH_Mode::ScheduleAltFreqChange(double aSimTime, int aAltFreqId /*=-1*/)
{
   double changeTime = std::max(aSimTime, mLastAltFreqSelectTime) + mAltFreqSelectDelay;
   GetSimulation()->AddEvent(ut::make_unique<AltFrequencyChangeEvent>(changeTime, this, aAltFreqId));
   mAltFreqChangeScheduled = true;
}

// =================================================================================================
//! Changes to the next alternate frequency set for the current mode for all beams.
//! @param aSimTime   The current simulation time.
//! @param aAltFreqId The ID of the frequency set to change to.
// virtual
void WsfOTH_RadarSensor::OTH_Mode::SelectAlternateFrequency(double aSimTime, int aAltFreqId /*=-1*/)
{
   for (auto& beamPtr : mBeamList)
   {
      int id = aAltFreqId;
      if (id < 0)
      {
         id = beamPtr->mXmtrPtr->GetCurrentAlternateFrequencyId() + 1;
      }
      beamPtr->mXmtrPtr->SelectAlternateFrequency(id);
   }
   mBeamList[0]->mXmtrPtr->NotifyChangeListeners(aSimTime, GetPlatform()->GetIndex());
   // Limit frequency change monitoring to sim runtime.  When randomize_radar_frequencies is enabled,
   // this method is also called during sim load/init as each mode is being parsed/initialized, but
   // frequency details are not yet available.  Note that checking sim->IsActive() works for mission
   // but not necessarily for all applications, hence the use of aSimTime here.
   if (aSimTime > 0.0)
   {
      WsfObserver::SensorFrequencyChanged(GetSimulation())(aSimTime, GetSensor(), this);
   }
   mLastAltFreqSelectTime  = aSimTime;
   mAltFreqChangeScheduled = false;
}

// =================================================================================================
// virtual
void WsfOTH_RadarSensor::OTH_Mode::Deselect(double /*aSimTime*/)
{
   for (auto& beamPtr : mBeamList)
   {
      if (mCanReceive)
      {
         beamPtr->mRcvrPtr->Deactivate();
      }
      if (mCanTransmit)
      {
         beamPtr->mXmtrPtr->Deactivate();
      }
   }
   // Update the lists of active transmitters and receivers for GetEM_Xmtr and GetEM_Rcvr.
   WsfOTH_RadarSensor* sensorPtr = dynamic_cast<WsfOTH_RadarSensor*>(GetSensor());
   if (sensorPtr != nullptr)
   {
      sensorPtr->UpdateXmtrRcvrLists();
   }
}

// =================================================================================================
// virtual
void WsfOTH_RadarSensor::OTH_Mode::Select(double aSimTime)
{
   for (auto& beamPtr : mBeamList)
   {
      if (mCanReceive)
      {
         beamPtr->mRcvrPtr->Activate();
      }
      if (mCanTransmit)
      {
         beamPtr->mXmtrPtr->Activate();
      }
   }
   // Update the lists of active transmitters and receivers for GetEM_Xmtr and GetEM_Rcvr.
   WsfOTH_RadarSensor* sensorPtr = dynamic_cast<WsfOTH_RadarSensor*>(GetSensor());
   if (sensorPtr != nullptr)
   {
      sensorPtr->UpdateXmtrRcvrLists();
   }

   if (mCanTransmit)
   {
      mBeamList[0]->mXmtrPtr->NotifyChangeListeners(aSimTime, GetPlatform()->GetIndex());
   }
}

// =================================================================================================
//! Update a track with the results from the current detection.
//! @param aSimTime    The current simulation time.
//! @param aTrackPtr   A pointer to the track to be updated.
//! @param aTargetPtr  Pointer to the target.
//! @param aResult     The results of the current detection attempt
// virtual
void WsfOTH_RadarSensor::OTH_Mode::UpdateTrack(double           aSimTime,
                                               WsfTrack*        aTrackPtr,
                                               WsfPlatform*     aTargetPtr,
                                               WsfSensorResult& aResult)
{
   // Update the track reporting flags (i.e.: <xxx>IsValid). This must be done before any part of the
   // track is updated as the flags are used to determine what data needs to be propagated from the
   // results of the detection attempt into the track.

   WsfSensorMode::UpdateTrack(aSimTime, aTrackPtr, aTargetPtr, aResult);

   // Update the reflection point in the track's aux data
   OTH_Beam beam = *mBeamList[aResult.mBeamIndex];
   double   reflectionLoc[3];
   beam.GetReflectionLocationWCS(reflectionLoc);

   double lat;
   double lon;
   double alt;
   UtEntity::ConvertWCSToLLA(reflectionLoc, lat, lon, alt);
   aTrackPtr->GetAuxData().Assign("oth_reflection_point_lat", lat);
   aTrackPtr->GetAuxData().Assign("oth_reflection_point_lon", lon);
   aTrackPtr->GetAuxData().Assign("oth_reflection_point_alt", alt);
}
