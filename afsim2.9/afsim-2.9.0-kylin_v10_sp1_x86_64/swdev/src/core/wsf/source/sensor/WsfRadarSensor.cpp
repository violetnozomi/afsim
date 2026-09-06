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

#include "WsfRadarSensor.hpp"

#include <algorithm>
#include <cmath>

#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "UtWallClock.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfEM_Clutter.hpp"
#include "WsfEM_ClutterTypes.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSensorErrorModel.hpp"
#include "WsfRadarSignature.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorComponent.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationInput.hpp"
#include "WsfStandardSensorErrorModel.hpp"
#include "WsfStringId.hpp"


namespace
{
std::string sLastImplicitBeamCommand;
}

// =================================================================================================
WsfRadarSensor::WsfRadarSensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mRadarModeList()
   , mXmtrList()
   , mRcvrList()
   , mAnyModeCanTransmit(true)
   , mAnyModeCanReceive(true)
   , mTempGeometryPtr(nullptr)
{
   SetClass(cACTIVE | cRADIO); // This is an active RF sensor.
   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new RadarMode()));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

// =================================================================================================
WsfRadarSensor::WsfRadarSensor(const WsfRadarSensor& aSrc)
   : WsfSensor(aSrc)
   , mRadarModeList()
   , mXmtrList()
   , mRcvrList()
   , mAnyModeCanTransmit(aSrc.mAnyModeCanTransmit)
   , mAnyModeCanReceive(aSrc.mAnyModeCanReceive)
   , mTempGeometryPtr(nullptr)
{
}

// =================================================================================================
// virtual
WsfSensor* WsfRadarSensor::Clone() const
{
   return new WsfRadarSensor(*this);
}

// =================================================================================================
// virtual
size_t WsfRadarSensor::GetEM_RcvrCount() const
{
   size_t count = 0;
   if (IsTurnedOn())
   {
      count = mRcvrList.size();
   }
   else if (!mRadarModeList.empty())
   {
      RadarMode* modePtr = mRadarModeList[mModeListPtr->GetCurrentMode()];
      count              = modePtr->mBeamList.size();
   }
   return count;
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfRadarSensor::GetEM_Rcvr(size_t aIndex) const
{
   if (IsTurnedOn())
   {
      if (aIndex < mRcvrList.size())
      {
         return *(mRcvrList[aIndex]);
      }
   }
   else if (!mRadarModeList.empty())
   {
      RadarMode* modePtr = mRadarModeList[mModeListPtr->GetCurrentMode()];
      if (aIndex < modePtr->mBeamList.size())
      {
         return *modePtr->mBeamList[aIndex]->mRcvrPtr;
      }
   }
   return WsfSensor::GetEM_Rcvr(aIndex);
}

// =================================================================================================
// virtual
size_t WsfRadarSensor::GetEM_RcvrCount(size_t aModeIndex) const
{
   size_t count = 0;
   if (aModeIndex < mRadarModeList.size())
   {
      RadarMode* modePtr = mRadarModeList[aModeIndex];
      count              = modePtr->mBeamList.size();
   }
   return count;
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfRadarSensor::GetEM_Rcvr(size_t aModeIndex, size_t aIndex) const
{
   if (aModeIndex < mRadarModeList.size())
   {
      RadarMode* modePtr = mRadarModeList[aModeIndex];
      if (aIndex < modePtr->mBeamList.size())
      {
         return *modePtr->mBeamList[aIndex]->mRcvrPtr;
      }
   }
   return WsfSensor::GetEM_Rcvr(aModeIndex, aIndex); // return dummy xmtr
}

// =================================================================================================
// virtual
size_t WsfRadarSensor::GetEM_XmtrCount() const
{
   size_t count = 0;
   if (IsTurnedOn())
   {
      count = mXmtrList.size();
   }
   else if (!mRadarModeList.empty())
   {
      RadarMode* modePtr = mRadarModeList[mModeListPtr->GetCurrentMode()];
      count              = modePtr->mBeamList.size();
   }
   return count;
}

// =================================================================================================
// virtual
WsfEM_Xmtr& WsfRadarSensor::GetEM_Xmtr(size_t aIndex) const
{
   if (IsTurnedOn())
   {
      if (aIndex < mXmtrList.size())
      {
         return *(mXmtrList[aIndex]);
      }
   }
   else if (!mRadarModeList.empty())
   {
      RadarMode* modePtr = mRadarModeList[mModeListPtr->GetCurrentMode()];
      if (aIndex < modePtr->mBeamList.size())
      {
         return *modePtr->mBeamList[aIndex]->mXmtrPtr;
      }
   }
   return WsfSensor::GetEM_Xmtr(aIndex);
}

// =================================================================================================
// virtual
size_t WsfRadarSensor::GetEM_XmtrCount(size_t aModeIndex) const
{
   return GetEM_RcvrCount(aModeIndex);
}

// =================================================================================================
// virtual
WsfEM_Xmtr& WsfRadarSensor::GetEM_Xmtr(size_t aModeIndex, size_t aIndex) const
{
   if (aModeIndex < mRadarModeList.size())
   {
      RadarMode* modePtr = mRadarModeList[aModeIndex];
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
void WsfRadarSensor::UpdateXmtrRcvrLists()
{
   mXmtrList.clear();
   mRcvrList.clear();
   for (RadarMode* modePtr : mRadarModeList)
   {
      if (modePtr->IsSelected())
      {
         for (size_t beamIndex = 0; beamIndex < modePtr->mBeamList.size(); ++beamIndex)
         {
            RadarBeam& beam = *modePtr->mBeamList[beamIndex];
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
bool WsfRadarSensor::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mRadarModeList);

   mAnyModeCanTransmit = false;
   mAnyModeCanReceive  = false;
   for (RadarMode* modePtr : mRadarModeList)
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
void WsfRadarSensor::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
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
bool WsfRadarSensor::ProcessInput(UtInput& aInput)
{
   return WsfSensor::ProcessInput(aInput);
}

// =================================================================================================
// virtual
void WsfRadarSensor::Update(double aSimTime)
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
void WsfRadarSensor::PerformScheduledDetections(double aSimTime)
{
   // Make sure that a scheduler and tracker are present.
   assert(mModeListPtr != nullptr);
   assert(mSchedulerPtr != nullptr);
   assert(mTrackerPtr != nullptr);
   assert(GetSimulation());

   // Let components do their thing...
   WsfSensorComponent::PrePerformScheduledDetections(*this, aSimTime);

   WsfTrackId                 requestId;
   Settings                   settings;
   WsfSensorResult            result;
   WsfSensorTracker::Settings stSettings;
   size_t                     targetIndex = 0;

   while (mSchedulerPtr->SelectTarget(aSimTime, mNextUpdateTime, targetIndex, requestId, settings))
   {
      WsfSensorMode* modePtr = mRadarModeList[settings.mModeIndex];
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
                  auto out = ut::log::debug() << "Radar sensor cueing to local track.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  out.AddNote() << "Sensor: " << GetName();
                  out.AddNote() << "Track #: " << requestId.GetLocalTrackNumber();
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
                  result.GetTransmitter()->NotifyListeners(aSimTime,
                                                           result); // Notify listeners for possible passive detection
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

   // Let components do their thing...
   WsfSensorComponent::PostPerformScheduledDetections(*this, aSimTime);

   // Set the update interval so the schedule is called at the desired time.
   double updateInterval = std::max(mNextUpdateTime - aSimTime, 1.0E-5);
   SetUpdateInterval(updateInterval);
}

// =================================================================================================
// Nested class WsfRadarSensor::RadarBeam.
// =================================================================================================

WsfRadarSensor::RadarBeam::RadarBeam()
   : WsfSensorBeam()
   , mAntennaPtr(ut::make_unique<WsfEM_Antenna>())
   , mXmtrPtr(ut::make_unique<WsfEM_Xmtr>(WsfEM_Xmtr::cXF_SENSOR, mAntennaPtr.get()))
   , mRcvrPtr(ut::make_unique<WsfEM_Rcvr>(WsfEM_Rcvr::cRF_SENSOR, mAntennaPtr.get()))
   , mClutterPtr(nullptr)
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
   , mClutterAttenuationFactor(1.0)
   , mClutterType()
{
   // Indicate the receiver is 'linked' with the transmitter.
   mXmtrPtr->SetLinkedReceiver(GetEM_Rcvr());
}

// =================================================================================================
WsfRadarSensor::RadarBeam::RadarBeam(const RadarBeam& aSrc)
   : WsfSensorBeam(aSrc)
   , mAntennaPtr(ut::make_unique<WsfEM_Antenna>(*aSrc.mAntennaPtr))
   , mXmtrPtr(ut::make_unique<WsfEM_Xmtr>(*aSrc.mXmtrPtr, mAntennaPtr.get()))
   , mRcvrPtr(ut::make_unique<WsfEM_Rcvr>(*aSrc.mRcvrPtr, mAntennaPtr.get()))
   , mClutterPtr(nullptr)
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
   , mClutterAttenuationFactor(aSrc.mClutterAttenuationFactor)
   , mClutterType(aSrc.mClutterType)
{
   if (aSrc.mClutterPtr != nullptr)
   {
      ut::log::error() << "Unexpected clone of clutter_model.";
   }

   // Indicate the receiver is 'linked' with the transmitter.
   mXmtrPtr->SetLinkedReceiver(GetEM_Rcvr());
}

WsfRadarSensor::RadarBeam& WsfRadarSensor::RadarBeam::operator=(const RadarBeam& aRhs)
{
   if (this != &aRhs)
   {
      WsfSensorBeam::operator                 =(aRhs);
      mAntennaPtr                             = ut::make_unique<WsfEM_Antenna>(*aRhs.mAntennaPtr);
      mXmtrPtr                                = ut::make_unique<WsfEM_Xmtr>(*aRhs.mXmtrPtr, mAntennaPtr.get());
      mRcvrPtr                                = ut::make_unique<WsfEM_Rcvr>(*aRhs.mRcvrPtr, mAntennaPtr.get());
      mClutterPtr                             = nullptr;
      mErrorModelAzBeamwidth                  = aRhs.mErrorModelAzBeamwidth;
      mErrorModelElBeamwidth                  = aRhs.mErrorModelElBeamwidth;
      mErrorModelPulseWidth                   = aRhs.mErrorModelPulseWidth;
      mErrorModelDopplerResolution            = aRhs.mErrorModelDopplerResolution;
      mCanTransmit                            = aRhs.mCanTransmit;
      mUseDetector                            = aRhs.mUseDetector;
      mDopplerResolution                      = aRhs.mDopplerResolution;
      mOneM2DetectRange                       = aRhs.mOneM2DetectRange;
      mLookDownFactor                         = aRhs.mLookDownFactor;
      mPRF_Factor                             = aRhs.mPRF_Factor;
      mPostLockonDetectionThresholdAdjustment = aRhs.mPostLockonDetectionThresholdAdjustment;
      mPostLockonAdjustmentDelayTime          = aRhs.mPostLockonAdjustmentDelayTime;
      mLoopGain                               = aRhs.mLoopGain;
      mAdjustmentFactor                       = aRhs.mAdjustmentFactor;
      mIntegrationGain                        = aRhs.mIntegrationGain;
      mNumberOfPulsesIntegrated               = aRhs.mNumberOfPulsesIntegrated;
      mDetector                               = aRhs.mDetector;
      mProbabilityTablePtr                    = aRhs.mProbabilityTablePtr;
      mClutterAttenuationFactor               = aRhs.mClutterAttenuationFactor;
      mClutterType                            = aRhs.mClutterType;

      if (aRhs.mClutterPtr != nullptr)
      {
         ut::log::error() << "Unexpected clone of clutter_model.";
      }

      // Indicate the receiver is 'linked' with the transmitter.
      mXmtrPtr->SetLinkedReceiver(GetEM_Rcvr());
   }
   return *this;
}

// =================================================================================================
// virtual
WsfRadarSensor::RadarBeam::~RadarBeam()
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
// virtual
void WsfRadarSensor::RadarBeam::AttemptToDetect(double           aSimTime,
                                                WsfPlatform*     aTargetPtr,
                                                Settings&        aSettings,
                                                WsfSensorResult& aResult)
{
   // Must have object pointers so event_output and debug output show locations.
   aResult.BeginGenericInteraction(GetEM_Xmtr(), aTargetPtr, GetEM_Rcvr());

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
void WsfRadarSensor::RadarBeam::AttemptToDetect(double           aSimTime,
                                                WsfPlatform*     aTargetPtr,
                                                Settings&        aSettings,
                                                WsfEM_Xmtr*      aXmtrPtr,
                                                WsfSensorResult& aResult)
{
   if (aResult.BeginTwoWayInteraction(aXmtrPtr, aTargetPtr, GetEM_Rcvr()) == 0)
   {
      // Set the position of the antenna beam(s).
      aResult.SetTransmitterBeamPosition();
      aResult.SetReceiverBeamPosition();

      // Determine the radar cross section of the target.

      aResult.mRadarSigAz = aResult.mTgtToRcvr.mAz;
      aResult.mRadarSigEl = aResult.mTgtToRcvr.mEl;
      aResult.mRadarSig   = WsfRadarSignature::GetValue(aTargetPtr,
                                                      aXmtrPtr,
                                                      GetEM_Rcvr(),
                                                      aResult.mTgtToXmtr.mAz,
                                                      aResult.mTgtToXmtr.mEl,
                                                      aResult.mTgtToRcvr.mAz,
                                                      aResult.mTgtToRcvr.mEl);

      // Calculate the signal return.
      aResult.ComputeRF_TwoWayPower(aResult.mRadarSig);

      // Account for the gain due to pulse compression.
      aResult.mRcvdPower *= aXmtrPtr->GetPulseCompressionRatio();

      // Account for integration gain
      aResult.mRcvdPower *= mIntegrationGain;

      // Allow for other general post-reception adjustments.
      aResult.mRcvdPower *= mAdjustmentFactor;

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

      // Apply mLookDownFactor if target is lower than receiver.
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

      // Compute the clutter power
      aResult.mClutterPower = 0.0;
      if (mClutterPtr != nullptr)
      {
         aResult.mClutterPower = mClutterPtr->ComputeClutterPower(aResult,
                                                                  GetPlatform()->GetSimulation()->GetEnvironment(),
                                                                  mClutterAttenuationFactor);
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
void WsfRadarSensor::RadarBeam::Calibrate(bool aPrint)
{
   double p_avg      = mXmtrPtr->GetAveragePower();
   double dutyCycle  = mXmtrPtr->GetPulseWidth() * mXmtrPtr->GetPulseRepetitionFrequency();
   double wavelength = UtMath::cLIGHT_SPEED / mXmtrPtr->GetFrequency();
   auto   out        = ut::log::info();
   if (aPrint)
   {
      out << "Radar beam calibrated.";

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
         for (unsigned int i = 0; i < (unsigned int)freqList.size(); ++i)
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

      // Allow for other general post-reception adjustments.
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

      // Allow for other general post-reception adjustments.
      temp1 *= mAdjustmentFactor;

      double r        = pow(temp1 / (mRcvrPtr->GetNoisePower() * detectionThreshold), 0.25);
      double loopGain = temp1 / mRcvrPtr->GetNoisePower();
      if (aPrint)
      {
         out.AddNote() << "1 m^2 Detection Range: " << r << " m (calibrated - free space)";
         out.AddNote() << "Loop Gain: " << UtMath::LinearToDB(loopGain) << " dB (calibrated)";
      }
   }

   if (aPrint && (mClutterPtr != nullptr) && (mClutterAttenuationFactor < 1.0))
   {
      out.AddNote() << "Clutter attenuation factor: " << UtMath::LinearToDB(mClutterAttenuationFactor) << " dB";
   }
}

// =================================================================================================
//! Compute the integrated pulse count.
//!
//! This is only used to compute the integrated pulse count for clutter calculations.
//!
//! From SalramMode::Initialize.
// public
double WsfRadarSensor::RadarBeam::ComputeIntegratedPulseCount(RadarMode& aMode)
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
            double minEl, maxEl;
            mAntennaPtr->GetElevationScanLimits(minEl, maxEl);
            timeOnTarget = aMode.GetFrameTime() * mXmtrPtr->GetElevationBeamwidth(0.0, 0.0) / (maxEl - minEl);
         }
         else
         {
            double minAz, maxAz;
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
   }
   return pulseCount;
}

// =================================================================================================
bool WsfRadarSensor::RadarBeam::Initialize(double       aSimTime,
                                           unsigned int aBeamIndex,
                                           WsfSensor*   aSensorPtr,
                                           RadarMode*   aModePtr,
                                           bool         aCanTransmit,
                                           bool         aCanReceive,
                                           bool         aShowCalibrationData,
                                           bool         aIsMultiBeam)
{
   // Propagate the debug flag
   if (aSensorPtr->GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG)
   {
      mRcvrPtr->SetDebugEnabled(aSensorPtr->DebugEnabled());
      mXmtrPtr->SetDebugEnabled(aSensorPtr->DebugEnabled());
   }

   mXmtrPtr->UseDefaultFrequency(aSensorPtr->GetScenario().GetSimulationInput().UseDefaultFrequency());

   mXmtrPtr->SetMode(aModePtr); // Identify the transmitter with the mode and beam index.
   mXmtrPtr->SetIndex(aBeamIndex);
   mRcvrPtr->SetMode(aModePtr); // Identify the transmitter with the mode and beam index.
   mRcvrPtr->SetIndex(aBeamIndex);
   mCanTransmit = aCanTransmit;

   bool ok = mAntennaPtr->Initialize(aSensorPtr);
   if (aCanTransmit)
   {
      ok &= mXmtrPtr->Initialize(*aSensorPtr->GetSimulation());
   }

   if (aCanReceive)
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

      if ((!GetSignalProcessors().Empty()) && ((mOneM2DetectRange > 0.0) || (mLoopGain > 0.0)))
      {
         ut::log::warning()
            << "'signal_processor' should not be used with 'one_m2_detect_range', 'range_product', or 'loop_gain'";
      }

      // Load and initialize the clutter model object.
      if (!mClutterType.IsNull())
      {
         mClutterPtr = WsfEM_ClutterTypes::Get(aSensorPtr->GetScenario()).Clone(mClutterType);
         if (mClutterPtr == nullptr)
         {
            auto out = ut::log::error() << "Undefined 'clutter_model'.";
            out.AddNote() << "Model: " << mClutterType;
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
            ut::log::error() << "Failed to initialize 'clutter_model'.";
            ok = false;
         }
      }
   }

   // Initialize after setting up transmitter, receiver and antenna
   ok &= WsfSensorBeam::Initialize(aSimTime);

   if (ok && aCanTransmit && aCanReceive)
   {
      if (aShowCalibrationData)
      {
         auto out = ut::log::info() << "Radar sensor initialized.";
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
bool WsfRadarSensor::RadarBeam::InitializeDetector(RadarMode& aMode)
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
bool WsfRadarSensor::RadarBeam::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (mAntennaPtr->ProcessInput(aInput) || mXmtrPtr->ProcessInputBlock(aInput) || mRcvrPtr->ProcessInputBlock(aInput))
   {
   }
   else if (WsfEM_ClutterTypes::Get(WsfScenario::FromInput(aInput)).LoadReference(aInput, mClutterType))
   {
   }
   else if (command == "doppler_resolution")
   {
      aInput.ReadValueOfType(mDopplerResolution, UtInput::cSPEED);
      aInput.ValueGreater(mDopplerResolution, 0.0);
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
      // Just another name for the reciprocal of the 'adjustment_factor'
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
   else if (command == "clutter_attenuation_factor")
   {
      aInput.ReadValueOfType(mClutterAttenuationFactor, UtInput::cRATIO);
      aInput.ValueInClosedRange(mClutterAttenuationFactor, 0.0, 1.0);
   }
   else
   {
      myCommand = WsfSensorBeam::ProcessInput(aInput);
   }
   return myCommand;
}

WsfPlatform* WsfRadarSensor::RadarBeam::GetPlatform()
{
   return mAntennaPtr->GetPlatform();
}

// =================================================================================================
// Nested class WsfRadarSensor::RadarMode.

WsfRadarSensor::RadarMode::RadarMode()
   : WsfSensorMode()
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
   SetErrorModel(ut::make_unique<wsf::RadarSensorErrorModel>());
   mBeamList.push_back(new RadarBeam());
   mBeamList[0]->SetBeamOwner(this);
}

// =================================================================================================
WsfRadarSensor::RadarMode::RadarMode(const RadarMode& aSrc)
   : WsfSensorMode(aSrc)
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
   for (size_t i = 0; i < mBeamList.size(); ++i)
   {
      mBeamList[i] = new RadarBeam(*aSrc.mBeamList[i]);
   }
}

// =================================================================================================
WsfRadarSensor::RadarMode& WsfRadarSensor::RadarMode::operator=(const RadarMode& aRhs)
{
   if (this != &aRhs)
   {
      WsfSensorMode::operator =(aRhs);
      mCanTransmit            = aRhs.mCanTransmit;
      mCanReceive             = aRhs.mCanReceive;
      mImplicitBeamUsed       = aRhs.mImplicitBeamUsed;
      mExplicitBeamUsed       = aRhs.mExplicitBeamUsed;
      mBeamList               = aRhs.mBeamList;
      mAltFreqSelectDelay     = aRhs.mAltFreqSelectDelay;
      mAltFreqChangeScheduled = aRhs.mAltFreqChangeScheduled;
      mLastAltFreqSelectTime  = aRhs.mLastAltFreqSelectTime;
      mIsFrequencyAgile       = aRhs.mIsFrequencyAgile;

      for (size_t i = 0; i < mBeamList.size(); ++i)
      {
         mBeamList[i] = new RadarBeam(*aRhs.mBeamList[i]);
      }
   }
   return *this;
}

// =================================================================================================
WsfRadarSensor::RadarMode::~RadarMode()
{
   for (auto& beamPtr : mBeamList)
   {
      delete beamPtr;
   }
}

// =================================================================================================
// virtual
WsfMode* WsfRadarSensor::RadarMode::Clone() const
{
   return new RadarMode(*this);
}

// =================================================================================================
// virtual
bool WsfRadarSensor::RadarMode::Initialize(double aSimTime)
{
   // If the mode is a transmit-only mode then force some sort of reporting to be present. Otherwise the base
   // class will indicate an error because the sensor doesn't report anything.
   if (!mCanReceive)
   {
      mReportingFlags.mReportsOther = true;
      if (mFrameTime == 0.0)
      {
         mFrameTime = 1000.0;
      }
   }

   bool ok = WsfSensorMode::Initialize(aSimTime);

   // Initialize each of the beams

   mMaximumRange     = 0.0;
   mIsFrequencyAgile = false; // reset for each instance
   bool isMultiBeam  = (mBeamList.size() > 1);
   for (unsigned int beamIndex = 0; beamIndex < mBeamList.size(); ++beamIndex)
   {
      RadarBeam& beam = *mBeamList[beamIndex];
      ok &= beam.Initialize(aSimTime,
                            beamIndex,
                            mSensorPtr,
                            this,
                            mCanTransmit,
                            mCanReceive,
                            mSensorPtr->ShowCalibrationData(),
                            isMultiBeam);

      mMaximumRange = std::max(mMaximumRange, beam.mAntennaPtr->GetMaximumRange());

      // TODO - may need method to determine how fast of switching determines agility.
      if ((!mIsFrequencyAgile) && mCanTransmit && (beam.mXmtrPtr->GetAlternateFrequencyCount() > 0))
      {
         mIsFrequencyAgile = true;
      }
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
bool WsfRadarSensor::RadarMode::ProcessInput(UtInput& aInput)
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
         mBeamList.push_back(new RadarBeam(*mBeamList[0]));
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

// =================================================================================================
bool WsfRadarSensor::RadarMode::AttemptToDetect(double           aSimTime,
                                                WsfPlatform*     aTargetPtr,
                                                Settings&        aSettings,
                                                WsfSensorResult& aResult)
{
   bool detected = false;
   aResult.Reset(aSettings);
   aResult.SetCategory(GetSensor()->GetZoneAttenuationModifier());
   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current
   aTargetPtr->Update(aSimTime);          // Ensure the target position is current

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Radar sensor attempting to detect target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Mode: " << GetName();
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   // Determine which beam returns the best results...
   if (!mCanReceive && mCanTransmit)
   {
      // TRANSMITTER only
   }
   else if (aResult.mFailedStatus == 0)
   {
      // Determine if concealed (like in a building).
      aResult.mCheckedStatus |= WsfSensorResult::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         aResult.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
         // Must have object pointers so event_output and debug output show locations.
         aResult.BeginGenericInteraction(mBeamList[0]->GetEM_Xmtr(), aTargetPtr, mBeamList[0]->GetEM_Rcvr());
      }

      mBeamList[0]->AttemptToDetect(aSimTime, aTargetPtr, aSettings, aResult);

      // Perform the terrain masking check if the detection was successful and if the masking check
      // was not performed internally as part of the detection processing.
      //
      // Also see NOTE in multi-beam processing.
      int terrainStatusMask    = (WsfSensorResult::cXMTR_TERRAIN_MASKING | WsfSensorResult::cRCVR_TERRAIN_MASKING);
      int terrainCheckedStatus = aResult.mCheckedStatus & terrainStatusMask;
      int terrainFailedStatus  = aResult.mFailedStatus & terrainStatusMask;
      if ((aResult.mFailedStatus == 0) && (terrainCheckedStatus == 0))
      {
         aResult.MaskedByTerrain();
         terrainCheckedStatus = aResult.mCheckedStatus & terrainStatusMask;
         terrainFailedStatus  = aResult.mFailedStatus & terrainStatusMask;
      }
      GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, aResult);
      GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);
      if (aResult.GetTransmitter() != nullptr)
      {
         aResult.GetTransmitter()->NotifyListeners(aSimTime, aResult); // Notify listeners for possible passive detection
      }
      if (GetSensor()->DebugEnabled())
      {
         auto out = ut::log::debug() << "Radar sensor attempting to detect target.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << GetSensor()->GetName();
         out.AddNote() << "Mode: " << GetName();
         out.AddNote() << "Target: " << aTargetPtr->GetName();
         if (mBeamList.size() > 1)
         {
            out.AddNote() << "Beam: 1";
         }
         auto note = out.AddNote() << "Result:";
         aResult.Print(note);
      }

      if (mBeamList.size() > 1)
      {
         // NOTE: Terrain masking used to be checked AFTER all beams had performed the basic detection
         // processing. Unfortunately this prevented the SensorDetectionAttempt observers from knowing
         // if terrain would mask target. Now terrain masking is now checked on first beam that passes
         // the all other detection criteria and is simply propagated to the subsequent beams.
         //
         // NOTE: This processing cannot be performed for multi-beam bistatic systems because each
         // beam could be receiving from a different transmitter. So the terrain masking will be checked
         // for EACH beam that successfully receives a detectable signal. I'm not sure if we'll ever
         // encounter multi-beam bistatic systems, but just in case...

         WsfSensorResult tempResult;
         for (unsigned int beamIndex = 1; beamIndex < mBeamList.size(); ++beamIndex)
         {
            tempResult.Reset(aSettings);
            tempResult.mBeamIndex = beamIndex;
            // Always force a terrain check for multi-beam bistatic (based on first beam)
            if (!aResult.mBistatic)
            {
               terrainCheckedStatus = 0;
               terrainFailedStatus  = 0;
            }
            tempResult.mCheckedStatus = ut::safe_cast<unsigned int, int>(terrainCheckedStatus);
            tempResult.mFailedStatus  = ut::safe_cast<unsigned int, int>(terrainFailedStatus);
            mBeamList[beamIndex]->AttemptToDetect(aSimTime, aTargetPtr, aSettings, tempResult);

            // Perform terrain masking check (or used the cached result) if the basic detection criteria passed.
            if (tempResult.mFailedStatus == 0)
            {
               if (terrainCheckedStatus == 0)
               {
                  tempResult.MaskedByTerrain();
                  terrainCheckedStatus = tempResult.mCheckedStatus & terrainStatusMask;
                  terrainFailedStatus  = tempResult.mFailedStatus & terrainStatusMask;
               }
               tempResult.mCheckedStatus |= terrainCheckedStatus;
               tempResult.mFailedStatus |= terrainFailedStatus;
            }
            GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, tempResult);
            GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, tempResult);
            if (tempResult.GetTransmitter() != nullptr)
            {
               tempResult.GetTransmitter()->NotifyListeners(aSimTime, tempResult); // Notify listeners for possible
                                                                                   // passive detection
            }
            if (GetSensor()->DebugEnabled())
            {
               auto out = ut::log::debug() << "Beam " << beamIndex + 1 << ":";
               tempResult.Print(out);
            }

            if (tempResult.mSignalToNoise > aResult.mSignalToNoise)
            {
               aResult = tempResult;
            }
         }
      }

      // Compute component effects.
      /*detected = */ WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);

      // A successful detection occurs only if the beam with the best signal-to-noise had no failing criteria.
      detected = (aResult.mFailedStatus == 0);
   }
   else
   {
      GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);
      if (GetSensor()->DebugEnabled())
      {
         auto out = ut::log::debug() << "Radar Sensor Detection Attempt Result:";
         aResult.Print(out);
      }
   }
   return detected;
}

// =================================================================================================
// virtual
void WsfRadarSensor::RadarMode::ScheduleAltFreqChange(double aSimTime, int aAltFreqId /*=-1*/)
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
void WsfRadarSensor::RadarMode::SelectAlternateFrequency(double aSimTime, int aAltFreqId /*=-1*/)
{
   for (auto& beam : mBeamList)
   {
      int id = aAltFreqId;
      if (id < 0)
      {
         id = beam->mXmtrPtr->GetCurrentAlternateFrequencyId() + 1;
      }
      beam->mXmtrPtr->SelectAlternateFrequency(id);
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
void WsfRadarSensor::RadarMode::Deselect(double /*aSimTime*/)
{
   for (auto& beam : mBeamList)
   {
      if (mCanReceive)
      {
         beam->mRcvrPtr->Deactivate();
      }
      if (mCanTransmit)
      {
         beam->mXmtrPtr->Deactivate();
      }
   }
   // Update the lists of active transmitters and receivers for GetEM_Xmtr and GetEM_Rcvr.
   WsfRadarSensor* sensorPtr = dynamic_cast<WsfRadarSensor*>(GetSensor());
   if (sensorPtr != nullptr)
   {
      sensorPtr->UpdateXmtrRcvrLists();
   }
}

// =================================================================================================
// virtual
void WsfRadarSensor::RadarMode::Select(double aSimTime)
{
   for (auto& beam : mBeamList)
   {
      if (mCanReceive)
      {
         beam->mRcvrPtr->Activate();
      }
      if (mCanTransmit)
      {
         beam->mXmtrPtr->Activate();
      }
   }
   // Update the lists of active transmitters and receivers for GetEM_Xmtr and GetEM_Rcvr.
   WsfRadarSensor* sensorPtr = dynamic_cast<WsfRadarSensor*>(GetSensor());
   if (sensorPtr != nullptr)
   {
      sensorPtr->UpdateXmtrRcvrLists();
   }

   if (mCanTransmit)
   {
      mBeamList[0]->mXmtrPtr->NotifyChangeListeners(aSimTime, GetPlatform()->GetIndex());
   }
}
