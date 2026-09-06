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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "WsfEW_SensorComponent.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <utility>

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_FalseTargetEffect.hpp"
#include "WsfEW_Result.hpp"
#include "WsfFalseTarget.hpp"
#include "WsfFalseTargetScreener.hpp"
#include "WsfFalseTargetScreenerTypes.hpp"
#include "WsfIFF_Manager.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSensorScheduler.hpp"
#include "WsfSensorTracker.hpp"
#include "WsfSimulation.hpp"

namespace
{
class EWSensorComponentFactory : public WsfComponentFactory<WsfSensor>
{
public:
   void PreInput(WsfSensor& aParent) override
   {
      // This must always present on RF sensors.
      // It isn't needed on acoustic or optical sensors.
      if ((aParent.GetClass() & WsfSensor::cRADIO) != 0)
      {
         WsfEW_SensorComponent::FindOrCreate(aParent);
      }
   }
};

// These tracker settings are for calls to the tracker from within this component. The third argument
// is a function code to indicate why the call is happening. That way when the call comes back to
// this component (in the form of 'Tracker...' calls) we will know the special processing that may
// need to be applied.

enum
{
   cFALSE_TARGET_UPDATES = 1, //!< False target detected, undetected or deleted
   cJAM_STROBE_UPDATES   = 2, //!< Jam strobe detected, undetected or deleted
   cBLOCKING_TARGET      = 3  //!< Target being blocked by false targets
};

// Tracker settings for false target updates
WsfSensorTrackerSettings ftTrackerSettings(WsfSensorTrackerSettings::cSUPPRESS_DETECTION_CHANGE_MESSAGES |
                                              WsfSensorTrackerSettings::cSUPPRESS_ALLOW_TRACKING_CHECK,
                                           cWSF_COMPONENT_EW_SENSOR,
                                           cFALSE_TARGET_UPDATES);

// Tracker settings for jam strobe target updates
WsfSensorTrackerSettings jsTrackerSettings(WsfSensorTrackerSettings::cSUPPRESS_TRACKING_CRITERIA |
                                              WsfSensorTrackerSettings::cSUPPRESS_ALLOW_TRACKING_CHECK,
                                           cWSF_COMPONENT_EW_SENSOR,
                                           cJAM_STROBE_UPDATES);

// Tracker settings to block a target (Needed by TrackerDropTrack).
// Used only in call to 'TargetBlocked'.
WsfSensorTrackerSettings tbTrackerSettings(WsfSensorTrackerSettings::cSUPPRESS_DETECTION_CHANGE_MESSAGES,
                                           cWSF_COMPONENT_EW_SENSOR,
                                           cBLOCKING_TARGET);
} // namespace

// =================================================================================================
//! Register the component factory that handles input for this component.
void WsfEW_SensorComponent::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<EWSensorComponentFactory>());
}

// =================================================================================================
//! Find the instance of this component attached to the specified sensor.
WsfEW_SensorComponent* WsfEW_SensorComponent::Find(const WsfSensor& aParent)
{
   WsfEW_SensorComponent* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<WsfEW_SensorComponent>(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the instance of this component attached to the specified processor,
//! and create it if it doesn't exist.
WsfEW_SensorComponent* WsfEW_SensorComponent::FindOrCreate(WsfSensor& aParent)
{
   WsfEW_SensorComponent* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      componentPtr = new WsfEW_SensorComponent;
      // Determine if the parent is of type WsfRadarSensor. This may be needed during input processing
      // and it needs to be done here. It cannot be done in 'ComponentParentChanged' because the RTTI
      // information is not fully available at that time. (JAJ)
      WsfSensor* sensorPtr            = &aParent;
      componentPtr->mIsWsfRadarSensor = ((dynamic_cast<WsfRadarSensor*>(sensorPtr) != nullptr) ? true : false);
      aParent.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// =================================================================================================
WsfEW_SensorComponent::WsfEW_SensorComponent()
   : mModeList()
   , mTemplate()
   , mLastSimTime(-1.0)
   , mLastRcvrPtr(nullptr)
   , mLastEW_Result()
   , mSuppressedTrackingSimTime(-1.0)
   , mSuppressedTrackingObjectId(0)
   , mIsWsfRadarSensor(false)
   , mHasFalseTargetScreeners(false)
   , mHasJamStrobeDetectors(false)
{
}

// =================================================================================================
WsfEW_SensorComponent::WsfEW_SensorComponent(const WsfEW_SensorComponent& aSrc)
   : mModeList(aSrc.mModeList)
   , mTemplate(aSrc.mTemplate)
   , mLastSimTime(-1.0)
   , mLastRcvrPtr(nullptr)
   , mLastEW_Result()
   , mSuppressedTrackingSimTime(-1.0)
   , mSuppressedTrackingObjectId(0)
   , mIsWsfRadarSensor(aSrc.mIsWsfRadarSensor)
   , mHasFalseTargetScreeners(aSrc.mHasFalseTargetScreeners)
   , mHasJamStrobeDetectors(aSrc.mHasJamStrobeDetectors)
{
}

// =================================================================================================
WsfComponent* WsfEW_SensorComponent::CloneComponent() const
{
   return new WsfEW_SensorComponent(*this);
}

// =================================================================================================
WsfStringId WsfEW_SensorComponent::GetComponentName() const
{
   static WsfStringId nullStringId;
   return nullStringId;
}

// =================================================================================================
const int* WsfEW_SensorComponent::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_EW_SENSOR, cWSF_COMPONENT_SENSOR_COMPONENT, cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* WsfEW_SensorComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_EW_SENSOR)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_SENSOR_COMPONENT)
   {
      return (WsfSensorComponent*)this;
   }
   return nullptr;
}

// =================================================================================================
bool WsfEW_SensorComponent::Initialize(double aSimTime)
{
   bool ok(true);

   // If the mode list is empty then the single implicit mode defined by the template is used.
   if (mModeList.empty())
   {
      mModeList.push_back(mTemplate);
   }

   // Expand the local mode list to match the sensor mode list size. In cases where the modes are generated
   // from user input then they will be the same. In the case of WsfCompositeSensor that isn't true as its
   // mode list is derived from the constituent sensors.
   while (GetSensor()->GetModeCount() > mModeList.size())
   {
      mModeList.push_back(mTemplate);
   }

   // Initialize each of the modes.
   mHasFalseTargetScreeners = false;
   mHasJamStrobeDetectors   = false;
   for (unsigned int modeIndex = 0; modeIndex < mModeList.size(); ++modeIndex)
   {
      EW_Mode&       ewMode(mModeList[modeIndex]);
      WsfSensorMode* modePtr = GetSensor()->GetModeEntry(modeIndex);

      // Validate the mode index and the mode->sensor pointer
      if (modeIndex != modePtr->GetModeIndex())
      {
         ut::log::error() << "WsfEW_SensorComponent mode index mismatch.";
         ok = false;
      }
      if (modePtr->GetSensor() != GetSensor())
      {
         ut::log::error() << "WsfEW_SensorComponent mode->sensor pointer mismatch.";
         ok = false;
      }

      size_t beamCount = modePtr->GetBeamCount();

      // For the same reason as the mode list size, expand the beam list size.
      while (beamCount > ewMode.mBeamList.size())
      {
         ewMode.mBeamList.push_back(ewMode.mBeamList[0]);
      }

      // Validate the beam index and the beam->mode pointer
      for (size_t beamIndex = 0; beamIndex < ewMode.mBeamList.size(); ++beamIndex)
      {
         if (beamCount > 0)
         {
            WsfSensorBeam* beamPtr = modePtr->GetBeamEntry(beamIndex);
            if (beamPtr->GetBeamIndex() != beamIndex)
            {
               ut::log::error() << "WsfEW_SensorComponent beam index mismatch.";
               ok = false;
            }
            if (beamPtr->GetSensorMode() != modePtr)
            {
               ut::log::error() << "WsfEW_SensorComponent beam->mode pointer mismatch.";
               ok = false;
            }
         }
         if (ok)
         {
            ewMode.mBeamList[beamIndex].mBeamIndex = beamIndex;
         }
      }

      if (ewMode.mScreenerPtr != nullptr)
      {
         mHasFalseTargetScreeners = true;
         if (!ewMode.mScreenerPtr->Initialize(aSimTime, modePtr, modePtr->ReportsSignalToNoise()))
         {
            auto out = ut::log::error() << "EW failed to initialize screener for mode.";
            out.AddNote() << "Mode: " << modePtr->GetName();
            ok = false;
         }
      }

      if (ewMode.mJammingPerceptionResetTime < 0.0)
      {
         double resetTime = std::max(modePtr->GetFrameTime(), GetSensor()->GetModeSelectDelay());
         resetTime        = std::max(resetTime, modePtr->GetAltFreqSelectDelay());
         // Fudge factor so jam strobes detections aren't reset immediately by regular detects on the next frame.
         ewMode.mJammingPerceptionResetTime = resetTime * 1.05;
      }

      if (ewMode.mUseJamStrobeDetector)
      {
         mHasJamStrobeDetectors = true;
         if (!ewMode.mJamStrobeDetector.Initialize(aSimTime, modePtr))
         {
            auto out = ut::log::error() << "EW failed to initialize jam strobe detector for mode.";
            out.AddNote() << "Mode: " << modePtr->GetName();
            ok = false;
         }
      }
   }

   // For the time being, certain capabilities are allowed only on WsfRadarSensor
   mHasFalseTargetScreeners &= mIsWsfRadarSensor;
   mHasJamStrobeDetectors &= mIsWsfRadarSensor;

   return ok;
}

// =================================================================================================
bool WsfEW_SensorComponent::ProcessModeInput(UtInput& aInput, WsfSensorMode& aSensorMode)
{
   EW_Mode& ewMode(GetOrCreateEW_Mode(aSensorMode));

   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "jamming_perception_timeout")
   {
      aInput.ReadValueOfType(ewMode.mJammingPerceptionResetTime, UtInput::cTIME);
   }
   else if (ewMode.mJamStrobeDetector.ProcessInput(aInput))
   {
      ewMode.mUseJamStrobeDetector = true;
   }
   else if (command == "maintain_track_measurement_history")
   {
      aInput.ReadValue(ewMode.mMaintainTrackMeasurementHistory);
   }
   else
   {
      myCommand = false;
      auto&                   scenario(aSensorMode.GetSensor()->GetScenario());
      WsfFalseTargetScreener* screenerPtr(nullptr);
      if (WsfFalseTargetScreenerTypes::Get(scenario).LoadInstance(aInput, screenerPtr))
      {
         delete ewMode.mScreenerPtr;
         ewMode.mScreenerPtr = screenerPtr;
         myCommand           = true;
      }
   }
   return myCommand;
}

// =================================================================================================
bool WsfEW_SensorComponent::ProcessBeamInput(UtInput& aInput, WsfSensorBeam& aSensorBeam)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "jamming_perception_threshold")
   {
      EW_Beam& beam(GetOrCreateEW_Beam(aSensorBeam));
      double   jnrPerceptionThreshold;
      aInput.ReadValueOfType(jnrPerceptionThreshold, UtInput::cRATIO);
      beam.mContJNR_PerceptionThreshold     = jnrPerceptionThreshold;
      beam.mPulsedJNR_PerceptionThreshold   = jnrPerceptionThreshold;
      beam.mCoherentJNR_PerceptionThreshold = jnrPerceptionThreshold;
   }
   else if (command == "continuous_jamming_perception_threshold")
   {
      EW_Beam& beam(GetOrCreateEW_Beam(aSensorBeam));
      aInput.ReadValueOfType(beam.mContJNR_PerceptionThreshold, UtInput::cRATIO);
   }
   else if (command == "pulsed_jamming_perception_threshold")
   {
      EW_Beam& beam(GetOrCreateEW_Beam(aSensorBeam));
      aInput.ReadValueOfType(beam.mPulsedJNR_PerceptionThreshold, UtInput::cRATIO);
   }
   else if (command == "coherent_jamming_perception_threshold")
   {
      EW_Beam& beam(GetOrCreateEW_Beam(aSensorBeam));
      aInput.ReadValueOfType(beam.mCoherentJNR_PerceptionThreshold, UtInput::cRATIO);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// virtual
void WsfEW_SensorComponent::SelectMode(double aSimTime, WsfStringId aModeNameId)
{
   WsfSensorMode* modePtr = GetMode(aModeNameId);
   if (modePtr != nullptr)
   {
      EW_Mode& mode(mModeList[modePtr->GetModeIndex()]);
      mode.mContJammingFirstPerceived     = -1.0;
      mode.mContJammingLastPerceived      = -1.0;
      mode.mPulseJammingFirstPerceived    = -1.0;
      mode.mPulseJammingLastPerceived     = -1.0;
      mode.mCoherentJammingFirstPerceived = -1.0;
      mode.mCoherentJammingLastPerceived  = -1.0;
   }
}

// =================================================================================================
// virtual
void WsfEW_SensorComponent::DeselectMode(double aSimTime, WsfStringId aModeNameId)
{
   WsfSensorMode* modePtr(GetMode(aModeNameId));
   if (modePtr != nullptr)
   {
      EW_Mode& mode(mModeList[modePtr->GetModeIndex()]);
      mode.mContJammingFirstPerceived     = -1.0;
      mode.mContJammingLastPerceived      = -1.0;
      mode.mPulseJammingFirstPerceived    = -1.0;
      mode.mPulseJammingLastPerceived     = -1.0;
      mode.mCoherentJammingFirstPerceived = -1.0;
      mode.mCoherentJammingLastPerceived  = -1.0;
   }
}

// =================================================================================================
void WsfEW_SensorComponent::AttemptToDetect(double aSimTime, WsfSensorResult& aResult)
{
   WsfEW_Result& ewResult = WsfEW_Result::FindOrCreate(aResult);

   // When performing bistatic detections the sensor will have an outer loop over the beams in the sensor
   // and an inner loop that calls AttemptToDetect for any potentially interacting sensor transmitter.
   //
   // The jamming calculations only need to be done ONCE for each outer loop.

   if ((aResult.mBistatic) && (aSimTime == mLastSimTime) && (aResult.GetReceiver() == mLastRcvrPtr))
   {
      ewResult = mLastEW_Result;
   }
   else
   {
      WsfRF_Jammer::ComputeTotalJammerEffects(aSimTime, aResult, true);

      // See if jamming is perceived by the sensor
      WsfEM_Rcvr*    rcvrPtr = aResult.GetReceiver();
      WsfSensorMode* modePtr = dynamic_cast<WsfSensorMode*>(rcvrPtr->GetMode());
      if (modePtr != nullptr)
      {
         EW_Mode& ewMode(mModeList[modePtr->GetModeIndex()]);
         EW_Beam& ewBeam(ewMode.mBeamList[aResult.mBeamIndex]);
         ewResult.mPerceivesContJamming  = PerceivesContinuousJamming(aResult, ewBeam.GetContJNR_PerceptionThreshold());
         ewResult.mPerceivesPulseJamming = PerceivesPulseJamming(aResult, ewBeam.GetPulsedJNR_PerceptionThreshold());
         ewResult.mPerceivesCoherentJamming =
            PerceivesCoherentJamming(aResult, ewBeam.GetCoherentJNR_PerceptionThreshold());

         // If this is the first beam of a mode then being the accumulation of the jamming perception status
         // for the detection result of this mode vs. this target.
         if (aResult.mBeamIndex == 0)
         {
            ewMode.mAccumPerceivesContJamming     = ewResult.mPerceivesContJamming;
            ewMode.mAccumPerceivesPulseJamming    = ewResult.mPerceivesPulseJamming;
            ewMode.mAccumPerceivesCoherentJamming = ewResult.mPerceivesCoherentJamming;
         }

         // Continue accumulation of the accumulated status for the mode.
         ewMode.mAccumPerceivesContJamming |= ewResult.mPerceivesContJamming;
         ewMode.mAccumPerceivesPulseJamming |= ewResult.mPerceivesPulseJamming;
         ewMode.mAccumPerceivesCoherentJamming |= ewResult.mPerceivesCoherentJamming;

         // If this is the last beam of the mode then update the perception status for the mode.
         if ((aResult.mBeamIndex + 1) == ewMode.mBeamList.size())
         {
            // Continuous jamming
            if (ewMode.mAccumPerceivesContJamming)
            {
               if (ewMode.mContJammingFirstPerceived < 0.0)
               {
                  ewMode.mContJammingFirstPerceived = aSimTime;
               }
               ewMode.mContJammingLastPerceived = aSimTime;
            }
            else if ((ewMode.mContJammingLastPerceived >= 0.0) &&
                     ((aSimTime - ewMode.mContJammingLastPerceived) > ewMode.mJammingPerceptionResetTime))
            {
               ewMode.mContJammingFirstPerceived = -1.0;
               ewMode.mContJammingLastPerceived  = -1.0;
            }

            // Pulse jamming
            if (ewMode.mAccumPerceivesPulseJamming)
            {
               if (ewMode.mPulseJammingFirstPerceived < 0.0)
               {
                  ewMode.mPulseJammingFirstPerceived = aSimTime;
               }
               ewMode.mPulseJammingLastPerceived = aSimTime;
            }
            else if ((ewMode.mPulseJammingLastPerceived >= 0.0) &&
                     ((aSimTime - ewMode.mPulseJammingLastPerceived) > ewMode.mJammingPerceptionResetTime))
            {
               ewMode.mPulseJammingFirstPerceived = -1.0;
               ewMode.mPulseJammingLastPerceived  = -1.0;
            }

            // Coherent jamming
            if (ewMode.mAccumPerceivesCoherentJamming)
            {
               if (ewMode.mCoherentJammingFirstPerceived < 0.0)
               {
                  ewMode.mCoherentJammingFirstPerceived = aSimTime;
               }
               ewMode.mCoherentJammingLastPerceived = aSimTime;
            }
            else if ((ewMode.mCoherentJammingLastPerceived >= 0.0) &&
                     ((aSimTime - ewMode.mCoherentJammingLastPerceived) > ewMode.mJammingPerceptionResetTime))
            {
               ewMode.mCoherentJammingFirstPerceived = -1.0;
               ewMode.mCoherentJammingLastPerceived  = -1.0;
            }
         }
      }

      // Save results for bistatic operations.
      if (aResult.mBistatic)
      {
         mLastSimTime   = aSimTime;
         mLastRcvrPtr   = aResult.GetReceiver();
         mLastEW_Result = ewResult;
      }
   }

   // Check EW Effects to see if the track is dropped.
   if (((ewResult.mEW_Effects.mMask & WsfEW_Effect::cEB_DROP_TRACK) != 0u) ||
       (((ewResult.mEW_Effects.mMask & WsfEW_Effect::cEB_FLOODED_BLOCKED)) != 0u))
   {
      aResult.mInterferenceFactor = 1.0;
   }
}

// =================================================================================================
// Extends WsfSensorMode::ComputeMeasurementErrors
void WsfEW_SensorComponent::ComputeSphericalMeasurementErrors(const WsfSensorResult&           aResult,
                                                              wsf::SphericalMeasurementErrors& aErrors) const
{
   const WsfEW_Result* resultPtr = WsfEW_Result::Find(aResult);
   if ((resultPtr != nullptr) && ((resultPtr->mEW_Effects.mMask & WsfEW_Effect::cEB_TRACK_ERROR) != 0u))
   {
      aErrors.mAzError += resultPtr->mEW_Effects.mEW_Error.mAzimuth;
      aErrors.mElError += resultPtr->mEW_Effects.mEW_Error.mElevation;
      aErrors.mRangeError += resultPtr->mEW_Effects.mEW_Error.mRange;
      // aErrors.mRangeRateError += resultPtr->mEW_Effects.mEW_Error.mRangeRate; // TODO
   }
}

// =================================================================================================
// Extends WsfSensor::PrePerformScheduledDetections
void WsfEW_SensorComponent::PrePerformScheduledDetections(double aSimTime)
{
   // Return immediately if the sensor does not have any false target screeners.
   if (!mHasFalseTargetScreeners)
   {
      return;
   }

   WsfTrackId          requestId;
   WsfSensor::Settings settings;
   WsfSensorResult     result;

   // Attempt to detect false targets in all active modes.
   size_t modeListSize(GetSensor()->GetModeCount());
   for (size_t modeIndex = 0; modeIndex < modeListSize; ++modeIndex)
   {
      WsfSensorMode* modePtr = GetSensor()->GetModeEntry(modeIndex);
      if (modePtr->IsSelected())
      {
         WsfFalseTargetScreener* screenerPtr = mModeList[modeIndex].mScreenerPtr;
         if ((screenerPtr != nullptr) && (!screenerPtr->IsRequestBasedTracking()) &&
             (!screenerPtr->OnlyScreenFT_TrackRequests()))
         {
            requestId.Null();
            settings.mModeIndex = modeIndex;
            PerformFalseTargetDetections(aSimTime, settings, requestId);
         }
      }
   }
}

// =================================================================================================
// Extends WsfSensor::PostPerformScheduledDetections
void WsfEW_SensorComponent::PostPerformScheduledDetections(double aSimTime)
{
   if (mHasJamStrobeDetectors)
   {
      PerformJamStrobeDetections(aSimTime);
   }
}

// =================================================================================================
// Extends WsfSensorMode::UpdateTrack
void WsfEW_SensorComponent::UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   EW_Mode& ewMode(mModeList[aResult.mModeIndex]);
   if (ewMode.mScreenerPtr != nullptr)
   {
      aTrackPtr->IsReportable(ewMode.mScreenerPtr->AllowTrackReporting(aTrackPtr->GetTargetIndex(), aTrackPtr));
   }
}

// =================================================================================================
bool WsfEW_SensorComponent::TrackerAllowTracking(double                 aSimTime,
                                                 const TrackerSettings& aSettings,
                                                 const WsfTrackId&      aRequestId,
                                                 size_t                 aObjectId,
                                                 WsfTrack*              aTrackPtr,
                                                 WsfSensorResult&       aResult)
{
   bool                    allowTracking(true);
   EW_Mode&                ewMode(mModeList[aResult.mModeIndex]);
   WsfFalseTargetScreener* screenerPtr = ewMode.mScreenerPtr;
   if (screenerPtr != nullptr)
   {
      allowTracking = screenerPtr->AllowTargetTrack(aSimTime, aObjectId, aTrackPtr, aResult);
      if (!allowTracking)
      {
         // HACK!!! (JAJ) See TrackerDropTrack for reason.
         mSuppressedTrackingSimTime  = aSimTime;
         mSuppressedTrackingObjectId = aObjectId;
         if (screenerPtr->GetDebugEnabled())
         {
            WsfPlatform* targetPtr  = aResult.GetTarget();
            std::string  targetName = (targetPtr != nullptr) ? targetPtr->GetName() : "<unknown>";
            auto         out        = ut::log::debug() << "Sensor was prevented from tracking target.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << screenerPtr->GetSensorModePtr()->GetPlatform()->GetName();
            out.AddNote() << "Sensor: " << screenerPtr->GetSensorModePtr()->GetName();
            out.AddNote() << "Sensor Mode: " << screenerPtr->GetSensorModePtr()->GetName();
            out.AddNote() << "Target: " << targetName;
         }
      }
      else if (screenerPtr->GetDebugEnabled())
      {
         WsfPlatform* targetPtr  = aResult.GetTarget();
         std::string  targetName = (targetPtr != nullptr) ? targetPtr->GetName() : "<unknown>";
         auto         out        = ut::log::debug() << "Sensor is tracking target.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << screenerPtr->GetSensorModePtr()->GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << screenerPtr->GetSensorModePtr()->GetSensor()->GetName();
         out.AddNote() << "Sensor Mode: " << screenerPtr->GetSensorModePtr()->GetName();
         out.AddNote() << "Target: " << targetName;
      }
   }
   return allowTracking;
}

// =================================================================================================
void WsfEW_SensorComponent::TrackerInitializeTrack(double                 aSimTime,
                                                   const TrackerSettings& aSettings,
                                                   const WsfTrackId&      aRequestId,
                                                   size_t                 aObjectId,
                                                   WsfSensorMode*         aModePtr,
                                                   WsfTrack*&             aTrackPtr)
{
   // This is invoked indirectly via WsfSensorTracker::TargetDetected
   if (aSettings.mComponentRole == cCOMPONENT_ROLE<WsfEW_SensorComponent>())
   {
      if (aSettings.mComponentFlags == cFALSE_TARGET_UPDATES)
      {
         // Called to initialize a false target track.
         EW_Mode&                ewMode(mModeList[aModePtr->GetModeIndex()]);
         WsfFalseTargetScreener* screenerPtr = ewMode.mScreenerPtr;
         if (screenerPtr != nullptr)
         {
            aModePtr->InitializeTrack(aSimTime, aTrackPtr);
            screenerPtr->AddFalseTargetTrack(aObjectId, aTrackPtr->GetTrackId());
         }
      }
      else if (aSettings.mComponentFlags == cJAM_STROBE_UPDATES)
      {
         // Called to initialize a jam strobe track
         EW_Mode& ewMode(mModeList[aModePtr->GetModeIndex()]);
         ewMode.mJamStrobeDetector.InitializeTrack(aSimTime, aTrackPtr);
      }
   }
}

// =================================================================================================
void WsfEW_SensorComponent::TrackerUpdateTrack(double                 aSimTime,
                                               const TrackerSettings& aSettings,
                                               const WsfTrackId&      aRequestId,
                                               size_t                 aObjectId,
                                               WsfSensorMode*         aModePtr,
                                               WsfTrack*              aTrackPtr,
                                               WsfPlatform*           aTargetPtr,
                                               WsfSensorResult&       aResult)
{
   EW_Mode& ewMode(mModeList[aModePtr->GetModeIndex()]);
   if (ewMode.mMaintainTrackMeasurementHistory && aResult.Detected())
   {
      aTrackPtr->GetHistory().emplace_back(aResult.mMeasurement.Clone());
   }

   // This is invoked indirectly via WsfSensorTracker::TargetDetected and WsfSensorTracker::TargetUndetected
   // We only care when it is explicitly directed to me...
   if (aSettings.mComponentRole == cCOMPONENT_ROLE<WsfEW_SensorComponent>())
   {
      if (aSettings.mComponentFlags == cFALSE_TARGET_UPDATES)
      {
         // Called to update a false target track
         WsfFalseTargetScreener* screenerPtr = ewMode.mScreenerPtr;
         if (screenerPtr != nullptr)
         {
            WsfFalseTargetBlip* blipPtr = screenerPtr->GetTrackableBlip(aObjectId);
            // TODO_JAJ will trip in test_FalseTargetSLC_SLB.txt assert(blipPtr != nullptr);
            if (blipPtr != nullptr)
            {
               UpdateFalseTargetTrack(aSimTime, aTrackPtr, blipPtr, aResult);
            }
            else
            {
               auto out = ut::log::info() << "blipPtr is null.";
               out.AddNote() << "T = " << aSimTime;
            }
         }
      }
      else if (aSettings.mComponentFlags == cJAM_STROBE_UPDATES)
      {
         // Called to update a jam strobe track
         ewMode.mJamStrobeDetector.UpdateTrack(aSimTime, aTrackPtr, aTargetPtr, aResult);
      }
   }
}

// =================================================================================================
void WsfEW_SensorComponent::TrackerDropTrack(double                 aSimTime,
                                             const TrackerSettings& aSettings,
                                             const WsfTrackId&      aRequestId,
                                             size_t                 aObjectId,
                                             WsfSensorMode*         aModePtr,
                                             WsfTrack*              aTrackPtr)
{
   // This is invoked directly or indirectly via WsfSensorTracker::TargetUndetected and TargetDeleted
   if (aSettings.mComponentRole == 0)
   {
      // Not a redirect explicitly to this component.
      // If there is a screener defined then tell it a normal target track is being dropped.
      EW_Mode&                ewMode(mModeList[aModePtr->GetModeIndex()]);
      WsfFalseTargetScreener* screenerPtr = ewMode.mScreenerPtr;
      if (screenerPtr != nullptr)
      {
         // HACK!!! (JAJ) If TrackerAllowTracking suppresses tracking the target, we cannot tell the screener
         // HACK!!! to DropTargetTrack because it causes problems. So only drop the target track under 'normal'
         // HACK!!! circumstances. TrackerAllowTracking sets the variables that will tell us to not drop
         //
         // This assumes, of course, that the sensor tracker is dropping the track when it gets the flags.
         if ((aSimTime != mSuppressedTrackingSimTime) || (aObjectId != mSuppressedTrackingObjectId))
         {
            screenerPtr->DropTargetTrack(aObjectId);
         }
      }
   }
   else if (aSettings.mComponentRole == cCOMPONENT_ROLE<WsfEW_SensorComponent>())
   {
      // Explicitly directed back to me from a false target track or jam strobe track drop.
      if (aSettings.mComponentFlags == cFALSE_TARGET_UPDATES)
      {
         // Called to delete a false target track.
         EW_Mode&                ewMode(mModeList[aModePtr->GetModeIndex()]);
         WsfFalseTargetScreener* screenerPtr = ewMode.mScreenerPtr;
         if (screenerPtr != nullptr)
         {
            screenerPtr->DropFalseTargetTrack(aObjectId);
         }
      }
      // cJAM_STROBE_UPDATES and cBLOCKING_TARGET don't need special processing
   }
   // HACK!!! Reset suppressed tracking flags.
   mSuppressedTrackingSimTime  = -1.0;
   mSuppressedTrackingObjectId = 0;
}

// =================================================================================================
//! Return the pointer to the false target screener associated with the specified mode.
//! This can be null.
WsfFalseTargetScreener* WsfEW_SensorComponent::GetScreener(const WsfSensorMode& aSensorMode) const
{
   return mModeList[aSensorMode.GetModeIndex()].mScreenerPtr;
}

// =================================================================================================
//! Returns true if the specified technique is active for the specified mode.
bool WsfEW_SensorComponent::IsEP_TechniqueActive(double /* aSimTime */,
                                                 WsfStringId aTechniqueId,
                                                 WsfStringId aModeNameId /* = WsfStringId() */)
{
   bool           active(false);
   WsfSensorMode* modePtr = GetMode(aModeNameId);
   if (modePtr != nullptr)
   {
      for (unsigned int i = 0; i < modePtr->GetBeamCount(); ++i)
      {
         auto beamEntry = modePtr->GetBeamEntry(i);
         if (beamEntry)
         {
            WsfEM_Rcvr* rcvrPtr = beamEntry->GetEM_Rcvr();
            if (rcvrPtr != nullptr)
            {
               WsfEW_EA_EP* epPtr = WsfEW_EP::GetElectronicProtect(*rcvrPtr);
               if (epPtr != nullptr)
               {
                  active |= epPtr->TechniqueIsInUse(aTechniqueId);
               }
            }
         }
      }
   }
   return active;
}

// =================================================================================================
bool WsfEW_SensorComponent::SelectEP_Technique(double /* aSimTime */,
                                               WsfStringId aTechniqueId,
                                               WsfStringId aModeNameId /* = WsfStringId() */)
{
   bool           selected(false);
   WsfSensorMode* modePtr = GetMode(aModeNameId);
   if (modePtr != nullptr)
   {
      for (unsigned int i = 0; i < modePtr->GetBeamCount(); ++i)
      {
         auto beamEntry = modePtr->GetBeamEntry(i);
         if (beamEntry)
         {
            WsfEM_Rcvr* rcvrPtr = beamEntry->GetEM_Rcvr();
            if (rcvrPtr != nullptr)
            {
               WsfEW_EA_EP* epPtr = WsfEW_EP::GetElectronicProtect(*rcvrPtr);
               if (epPtr != nullptr)
               {
                  selected |= epPtr->SelectTechnique(aTechniqueId);
               }
            }
         }
      }
   }
   return selected;
}

// =================================================================================================
bool WsfEW_SensorComponent::DeselectEP_Technique(double /* aSimTime */,
                                                 WsfStringId aTechniqueId,
                                                 WsfStringId aModeNameId /* = WsfStringId() */)
{
   bool           deselected(false);
   WsfSensorMode* modePtr = GetMode(aModeNameId);
   if (modePtr != nullptr)
   {
      for (unsigned int i = 0; i < modePtr->GetBeamCount(); ++i)
      {
         auto beamEntry = modePtr->GetBeamEntry(i);
         if (beamEntry)
         {
            WsfEM_Rcvr* rcvrPtr = beamEntry->GetEM_Rcvr();
            if (rcvrPtr != nullptr)
            {
               WsfEW_EA_EP* epPtr = WsfEW_EP::GetElectronicProtect(*rcvrPtr);
               if (epPtr != nullptr)
               {
                  deselected |= epPtr->DeselectTechnique(aTechniqueId);
               }
            }
         }
      }
   }
   return deselected;
}

// =================================================================================================
bool WsfEW_SensorComponent::JammingPerceived(const WsfSensorMode& aSensorMode)
{
   return mModeList[aSensorMode.GetModeIndex()].JammingPerceived();
}

// =================================================================================================
bool WsfEW_SensorComponent::JammingPerceived(WsfStringId aModeNameId)
{
   WsfSensorMode* modePtr(GetMode(aModeNameId));
   return ((modePtr != nullptr) ? mModeList[modePtr->GetModeIndex()].JammingPerceived() : false);
}

// =================================================================================================
bool WsfEW_SensorComponent::ContinuousJammingPerceived(const WsfSensorMode& aSensorMode)
{
   return mModeList[aSensorMode.GetModeIndex()].ContinuousJammingPerceived();
}

// =================================================================================================
bool WsfEW_SensorComponent::ContinuousJammingPerceived(WsfStringId aModeNameId)
{
   WsfSensorMode* modePtr(GetMode(aModeNameId));
   return ((modePtr != nullptr) ? mModeList[modePtr->GetModeIndex()].ContinuousJammingPerceived() : false);
}

// =================================================================================================
bool WsfEW_SensorComponent::PulseJammingPerceived(const WsfSensorMode& aSensorMode)
{
   return mModeList[aSensorMode.GetModeIndex()].PulseJammingPerceived();
}

// =================================================================================================
bool WsfEW_SensorComponent::PulseJammingPerceived(WsfStringId aModeNameId)
{
   WsfSensorMode* modePtr(GetMode(aModeNameId));
   return ((modePtr != nullptr) ? mModeList[modePtr->GetModeIndex()].PulseJammingPerceived() : false);
}

// =================================================================================================
double WsfEW_SensorComponent::GetJammingFirstPerceived(const WsfSensorMode& aSensorMode)
{
   return mModeList[aSensorMode.GetModeIndex()].GetJammingFirstPerceived();
}

// =================================================================================================
double WsfEW_SensorComponent::GetJammingLastPerceived(const WsfSensorMode& aSensorMode)
{
   return mModeList[aSensorMode.GetModeIndex()].GetJammingLastPerceived();
}

// =================================================================================================
//! Return the pointer to the mode with the specified name.
//! @param aModeNameId The name of the mode to be returned.
//! @returns Pointer to the requested mode, or nullptr if the mode does not exist.
// private
WsfSensorMode* WsfEW_SensorComponent::GetMode(WsfStringId aModeNameId) const
{
   WsfSensorMode* modePtr(nullptr);
   size_t         modeIndex = GetSensor()->GetModeIndex(aModeNameId);
   if (modeIndex < GetSensor()->GetModeCount())
   {
      modePtr = GetSensor()->GetModeEntry(modeIndex);
   }
   return modePtr;
}

// =================================================================================================
//! Return the pointer to the local EW mode with the specified name.
//! @param aModeNameId The name of the mode to be returned.
//! @returns Pointer to the requested mode. If the mode does not exist it will be created.
//! @note This function is only needed when the component is not yet initialized (i.e.: during
//! ProcessInput). During ProcessInput it will create mode list entries as needed. After initialization
//! one can use aSensorMode.GetModeIndex() and directly access the mode list.
// private
WsfEW_SensorComponent::EW_Mode& WsfEW_SensorComponent::GetOrCreateEW_Mode(const WsfSensorMode& aSensorMode)
{
   // The local EW modes are maintained in the same order as the real mode list. If the
   // new mode index is larger than the current list size then expand our list as needed.

   // See if a mode_template is being used and return the mTemplate if so.
   if (&aSensorMode == aSensorMode.GetSensor()->GetModeList()->GetModeTemplate())
   {
      return mTemplate;
   }

   // Newly created modes are copies of the template.
   size_t modeIndex(aSensorMode.GetModeIndex());
   while (modeIndex >= mModeList.size())
   {
      mModeList.push_back(mTemplate);
   }
   return mModeList[modeIndex];
}

// =================================================================================================
//! Return the pointer to the local EW beam that corresponds to the real sensor beam..
//! @param aSensorBeam The sensor beam whose EW_Beam is to be returned.
//! @returns Pointer to the requested beam. If the beam does not exist it will be created.
//! @note This is only use during input processing.
// private
WsfEW_SensorComponent::EW_Beam& WsfEW_SensorComponent::GetOrCreateEW_Beam(const WsfSensorBeam& aSensorBeam)
{
   WsfSensorMode* modePtr = aSensorBeam.GetSensorMode();
   assert(modePtr != nullptr);
   EW_Mode& ewMode(GetOrCreateEW_Mode(*modePtr));
   assert(!ewMode.mBeamList.empty()); // A mode is always created with a beam

   // The local EW beams are maintained in the same order as the real beam list. If the
   // new beam index is larger than the current list size then expand our list as needed.
   // Newly created beams are copies of the first beam.
   size_t beamIndex(aSensorBeam.GetBeamIndex());
   while (beamIndex >= ewMode.mBeamList.size())
   {
      ewMode.mBeamList.push_back(ewMode.mBeamList[0]);
   }
   return ewMode.mBeamList[beamIndex];
}

// =================================================================================================
// Refactored from RadarMode::AttemptToDetectFalseTarget in WSF WsfRadarSensor/AFSIM WsfEW_RadarSensor.
bool WsfEW_SensorComponent::AttemptToDetectFalseTarget(double               aSimTime,
                                                       WsfFalseTargetBlip*  aBlipPtr,
                                                       WsfSensor::Settings& aSettings,
                                                       WsfSensorResult&     aResult)
{
   bool detected(false);
   WsfEW_Result::FindOrCreate(aResult);
   aResult.Reset(aSettings);
   aResult.SetCategory(GetSensor()->GetZoneAttenuationModifier());
   GetSensor()->UpdatePosition(aSimTime); // Ensure position is current

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Sensor is attempting to detect Blip.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Mode: " << GetSensor()->GetModeName(aSettings.mModeIndex);
      out.AddNote() << "Blip: " << aBlipPtr->GetNameId();
   }

   WsfEM_Interaction jammerInteraction;
   WsfEW_Result::FindOrCreate(jammerInteraction);

   WsfFalseTargetScreener* screenerPtr(mModeList[aSettings.mModeIndex].mScreenerPtr);
   AttemptToDetectFalseTarget(aSimTime, aBlipPtr, screenerPtr->GetTempPlatform(), aSettings, jammerInteraction, aResult);

   if (aResult.GetTransmitter() != nullptr)
   {
      aResult.GetTransmitter()->NotifyListeners(aSimTime, aResult); // Notify listeners for possible ESM detection
   }

   // if (GetSensor()->DebugEnabled())
   //{
   //    cout << "AttemptToDetect interaction: " << GetPlatform()->GetName() << '.' << GetSensor()->GetName()
   //       << ", mode " << GetName() << "; attempting to detect " << aBlipPtr->GetNameId() << '\n';
   //    if (mBeamList.size() > 1)
   //    {
   //       cout << " Beam: 1\n";
   //    }
   //    aResult.Print();
   // }

   WsfSensorMode* modePtr(GetSensor()->GetModeEntry(aSettings.mModeIndex));
   size_t         beamCount(modePtr->GetBeamCount());
   if (beamCount > 1)
   {
      WsfSensorResult tempResult;
      WsfEW_Result::FindOrCreate(tempResult);
      for (size_t beamIndex = 1; beamIndex < beamCount; ++beamIndex)
      {
         tempResult.Reset(aSettings);
         tempResult.mBeamIndex = beamIndex;
         AttemptToDetectFalseTarget(aSimTime, aBlipPtr, screenerPtr->GetTempPlatform(), aSettings, jammerInteraction, tempResult);

         if (tempResult.GetTransmitter() != nullptr)
         {
            tempResult.GetTransmitter()->NotifyListeners(aSimTime,
                                                         tempResult); // Notify listeners for possible ESM detection
         }
         // if (GetSensor()->DebugEnabled())
         //{
         //   cout << " Beam: " << beamIndex + 1 << '\n';
         //    tempResult.Print();
         // }

         if (tempResult.mSignalToNoise > aResult.mSignalToNoise)
         {
            aResult = tempResult;
         }
      }
   }

   // A successful detection occurs only if the beam with the best signal-to-noise had no failing criteria.
   detected =
      (((aResult.mFailedStatus & WsfSensorResult::cSIGNAL_LEVEL) == 0) &&
       ((aResult.mFailedStatus & WsfSensorResult::cDOPPLER_LIMITS) == 0) && (jammerInteraction.mFailedStatus == 0));

   if (detected)
   {
      // get current blip location and velocity and update the result
      // double blipLoc_WCS[3];
      // aBlipPtr->GetLocationWCS(blipLoc_WCS);
      // UtVec3d::Set(aResult.mReportedLocWCS, blipLoc_WCS);

      double azAngle = aResult.mRcvrToTgt.mAz - aResult.mRcvrBeam.mAz;
      double elAngle = aResult.mRcvrToTgt.mEl - aResult.mRcvrBeam.mEl;

      double relLocationWCS[3];
      aResult.GetReceiver()->GetAntenna()->GetRelativeLocationWCS(azAngle, elAngle, aResult.mRcvrToTgt.mRange, relLocationWCS);
      double locationWCS[3];
      aResult.GetReceiver()->GetAntenna()->GetLocationWCS(locationWCS);
      UtVec3d::Add(locationWCS, locationWCS, relLocationWCS);

      // if (aBlipPtr->GetFalseTarget()->GetRadarBehavior() == WsfFalseTarget::cCONSISTENT_TO_MULTIPLE_RADARS)
      //{
      //    double blipLoc_WCS[3];
      //    aBlipPtr->GetLocationWCS(blipLoc_WCS);
      //    double lat1, lon1, alt1;
      //    UtEntity::ConvertWCSToLLA(blipLoc_WCS, lat1, lon1, alt1);
      //    double lat2, lon2, alt2;
      //    UtEntity::ConvertWCSToLLA(locationWCS, lat2, lon2, alt2);
      //    UtEntity::ConvertLLAToWCS(lat1, lon1, alt2, locationWCS);
      // }

      aResult.mMeasurement.SetLocationWCS(locationWCS);

      // Set the target location in the result so it can be used to compute the equivalent RCS later.
      // Will not update the result as it will have the desired location and reported locations.
      aResult.GetTarget()->SetLocationWCS(locationWCS);
      aResult.mTgtLoc.mIsValid = false;
      aResult.mCheckedStatus   = 0;
      aResult.ComputeUndefinedGeometry();
      modePtr->ApplyMeasurementErrors(aResult);

      double blipVelWCS[3];
      aBlipPtr->GetVelocityWCS(blipVelWCS);
      aResult.mMeasurement.SetVelocityWCS(blipVelWCS);
   }

   WsfFalseTarget::RadarSnsrPair radarIdKey = std::make_pair(GetPlatform()->GetNameId(), GetSensor()->GetNameId());
   aBlipPtr->UpdateBlipData(radarIdKey, aResult, jammerInteraction);
   aResult.mRadarSig = aBlipPtr->GetEquivalentRCS(radarIdKey);
   detected &= (aBlipPtr->GetStrength(radarIdKey) != WsfFalseTargetBlip::cUndetected);

   return detected;
}

// =================================================================================================
// NOTE:
//
// This routine is predicated on this being attached to a WsfRadarSensor. If it is really desired that
// false targets can be injected into other models then a more general interface needs to be developed
// to compute the detection agains a blip.
//
// Refactored from RadarBeam::AttemptToDetectFalseTarget in WSF 1.x WsfRadarSensor
void WsfEW_SensorComponent::AttemptToDetectFalseTarget(double                aSimTime,
                                                       WsfFalseTarget::Blip* aBlipPtr,
                                                       WsfPlatform*          aTempPlatformPtr,
                                                       WsfSensor::Settings&  aSettings,
                                                       WsfEM_Interaction&    aJammerInteraction,
                                                       WsfSensorResult&      aResult)
{
   assert(mIsWsfRadarSensor);

   WsfSensorMode*             tempModePtr = GetSensor()->GetModeEntry(aSettings.mModeIndex);
   WsfRadarSensor::RadarMode* modePtr     = dynamic_cast<WsfRadarSensor::RadarMode*>(tempModePtr);
   assert(modePtr != nullptr);
   WsfRadarSensor::RadarBeam* beamPtr = modePtr->mBeamList[aResult.mBeamIndex];
   assert(beamPtr != nullptr);
   WsfRadarSensor::RadarBeam& beam    = *beamPtr;
   WsfEM_Rcvr*                rcvrPtr = beam.GetEM_Rcvr();
   WsfEM_Xmtr*                xmtrPtr = beam.GetEM_Xmtr();

   // compute power required by the jammer to generate the appropriate amplitude
   //  determined by mimicking the false target's RCS at the blip location
   //  and computing the power which would be reflected back to the radar.
   // Even if the two-way interaction fails go ahead and get the scaling SNR.
   if (aResult.BeginTwoWayInteraction(xmtrPtr, aTempPlatformPtr, rcvrPtr) != 0)
   {
      aResult.ComputeUndefinedGeometry();
   }
   aResult.SetTransmitterBeamPosition();
   aResult.SetReceiverBeamPosition();

   // The following call takes into account the PCR, Integration Gain and adjustment factor
   aBlipPtr->ComputeRcvdPower(aSimTime, aResult, aJammerInteraction);

   // Adjust for the effects of any signal processing.
   beam.GetSignalProcessors().Execute(aSimTime, aResult);

   // Compute the total effective signal-to-interference ratio at the output of the receiver.
   aResult.mSignalToNoise =
      rcvrPtr->ComputeSignalToNoise(aResult.mRcvdPower, aResult.mClutterPower, aResult.mInterferencePower);

   // If a 'time-locked-on' was supplied (aSettings.mLockonTime >= 0.0) then adjust the detection threshold
   // by the post_lockon_detection_threshold_adjustment (default 1.0). Note that the 'time-locked-on' simply
   // indicates when WsfSensorTracker has declared a that detection is 'stable' (e.g.: M/N criteria met for
   // the mode). It doesn't mean the sensor is a 'tracker'. That's OK, however, because the default adjustment
   // is 1.0. So even if it is applied to a tracking mode it will still work

   double detectionThresholdAdjustment = 1.0;
#ifdef TODO_JAJ
   if ((aSettings.mLockonTime >= 0.0) && ((aSettings.mLockonTime + mPostLockonAdjustmentDelayTime) <= aSimTime))
   {
      detectionThresholdAdjustment = mPostLockonDetectionThresholdAdjustment;
      aResult.mDetectionThreshold *= detectionThresholdAdjustment;
   }
#endif

   // Compute the probability of detection.
   if (beam.GetProbabilityTable())
   {
      // detection_probability table selected
      aResult.mPd =
         beam.GetProbabilityTable()->ComputeProbabilityOfDetection(aResult.mSignalToNoise / detectionThresholdAdjustment);
   }
   else if (beam.UsingDetector())
   {
      // Marcum-Swerling detector selected
      aResult.mPd =
         beam.GetDetector().ComputeProbabilityOfDetection(aResult.mSignalToNoise / detectionThresholdAdjustment);
   }
   else
   {
      // Simple binary detector selected
      aResult.mPd = 1.0;
      if (aResult.mSignalToNoise < (rcvrPtr->GetDetectionThreshold() * detectionThresholdAdjustment))
      {
         aResult.mPd = 0.0;
      }
   }

   // Check the signal level
   aResult.mCheckedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
   if (aResult.mPd < aResult.mRequiredPd)
   {
      aResult.mFailedStatus |= WsfSensorResult::cSIGNAL_LEVEL;
   }
}

// =================================================================================================
void WsfEW_SensorComponent::UpdateFalseTargetTrack(double              aSimTime,
                                                   WsfTrack*           aTrackPtr,
                                                   WsfFalseTargetBlip* aBlipPtr,
                                                   WsfSensorResult&    aResult)
{
   WsfSensorMode* modePtr = GetSensor()->GetModeEntry(aResult.mModeIndex);

   // Update the track reporting flags (i.e.: <xxx>IsValid). This must be done before any part of the
   // track is updated as the flags are used to determine what data needs to be propagated from the
   // results of the detection attempt into the track.
   modePtr->UpdateTrackReportingFlags(aSimTime, aTrackPtr);

   aTrackPtr->Update(aSimTime);
   aTrackPtr->SetMessageDataTag(GetSimulation()->CreateMessageDataTag());

   // Report the name of the mode used for this update.
   aTrackPtr->SetSensorModeId(modePtr->GetNameId());

   // Define the origin of the report as the location of the receiver, as the reported range and angles are
   // relative to that point. Note that all sensors *SHOULD* have a receiver as it is necessary to carry on
   // the interaction, but for some reason it doesn't then the platform location will be used.

   double snrLocationWCS[3];
   UtVec3d::Set(snrLocationWCS, aResult.mRcvrLoc.mLocWCS);
   if (!aResult.mRcvrLoc.mIsValid)
   {
      if ((aResult.GetReceiver() != nullptr) && (aResult.GetReceiver()->GetAntenna() != nullptr))
      {
         aResult.GetReceiver()->GetAntenna()->GetLocationWCS(snrLocationWCS);
      }
      else
      {
         GetPlatform()->GetLocationWCS(snrLocationWCS);
      }
   }
   double platformLocationErrorWCS[3];
   GetPlatform()->GetLocationErrorWCS(platformLocationErrorWCS);
   UtVec3d::Add(snrLocationWCS, snrLocationWCS, platformLocationErrorWCS);
   aTrackPtr->SetOriginatorLocationWCS(snrLocationWCS);

   WsfMeasurement& measurement = aResult.mMeasurement;
   double          reportedLocWCS[3];
   measurement.GetLocationWCS(reportedLocWCS);
   modePtr->UpdateTrackPosition(aTrackPtr, aResult, reportedLocWCS);

   if (aTrackPtr->VelocityValid())
   {
      if (measurement.VelocityValid())
      {
         double velWCS[3];
         measurement.GetVelocityWCS(velWCS);
         aTrackPtr->SetVelocityWCS(velWCS);
      }
      else if (aBlipPtr != nullptr)
      {
         double targetVelWCS[3];
         aBlipPtr->GetVelocityWCS(targetVelWCS);
         aTrackPtr->SetVelocityWCS(targetVelWCS);
      }
   }

   if (aTrackPtr->RangeRateValid())
   {
      aTrackPtr->SetRangeRateError(measurement.GetRangeRateError());
      if (measurement.RangeRateValid())
      {
         aTrackPtr->SetRangeRate(measurement.GetRangeRate());
      }
      else if (aBlipPtr != nullptr)
      {
         if ((aResult.GetReceiver() != nullptr) && (aResult.GetReceiver()->GetPlatform() != nullptr))
         {
            // Get the target velocity.
            double relTgtVelWCS[3];
            aBlipPtr->GetVelocityWCS(relTgtVelWCS);

            // Get the sensor velocity.
            double snrVelWCS[3];
            aResult.GetReceiver()->GetPlatform()->GetVelocityWCS(snrVelWCS);

            // Compute the relative WCS velocity (V_t - V_s)
            UtVec3d::Subtract(relTgtVelWCS, relTgtVelWCS, snrVelWCS);

            // Compute the dot product of the velocity vector along the relative position unit vector.
            // This is the range rate.
            double rangeRate = UtVec3d::DotProduct(relTgtVelWCS, aResult.mRcvrToTgt.mTrueUnitVecWCS);

            aTrackPtr->SetRangeRate(rangeRate + measurement.GetRangeRateError());
         }
      }
   }

   if (aTrackPtr->SideIdValid())
   {
      if (measurement.SideIdValid())
      {
         aTrackPtr->SetSideId(measurement.GetSideId());
      }
      else if (aBlipPtr != nullptr)
      {
         //         aTrackPtr->SetSideId(aBlipPtr->GetSideId());
      }
   }

   if (aTrackPtr->TypeIdValid())
   {
      if (measurement.TypeIdValid())
      {
         aTrackPtr->SetTypeId(measurement.GetTypeId());
      }
      else if (aBlipPtr != nullptr)
      {
         //         aTrackPtr->SetTypeId(aBlipPtr->GetTypeId());
      }
   }

   // There is no validity flag in the track for IFF. We simply check the sensor to decide if we should report.
   // For now assume IFF is simply done by checking to see if the target is on the same side as the sensing platform.

   if (modePtr->ReportsIFF_Status())
   {
      WsfStringId targetSide; // = aBlipPtr->GetSideId();
      if (measurement.SideIdValid())
      {
         targetSide = measurement.GetSideId();
      }
      if ((aBlipPtr != nullptr) && (aBlipPtr->GetPlatform() != nullptr))
      {
         auto& scenario(modePtr->GetSensor()->GetScenario());
         aTrackPtr->SetIFF_Status(
            scenario.GetIFF_Manager()->GetIFF_Status(GetPlatform()->GetSideId(),
                                                     targetSide,
                                                     aBlipPtr->GetPlatform()->GetCategories().GetCategoryList()));
      }
   }

   if (aTrackPtr->SignalToNoiseValid())
   {
      aTrackPtr->SetSignalToNoise(aResult.mSignalToNoise);
   }

   // If the spatial domain hasn't been defined then simply copy the spatial domain of the target.
   // if (aTrackPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_UNKNOWN)
   //{
   //   aTrackPtr->SetSpatialDomain(WsfSpatialDomain::WSF_SPATIAL_DOMAIN_UNKNOWN);
   //}

   if (aBlipPtr != nullptr)
   {
      // Commented out below in lieu of using the blip unique id instead...may cause issues later in simulation
      // if (aBlipPtr->GetPlatform() != nullptr)
      //{
      //   aTrackPtr->SetTargetIndex(aBlipPtr->GetPlatform()->GetIndex());
      //}
      aTrackPtr->SetTargetIndex(aBlipPtr->GetUniqueId());
      aTrackPtr->SetTargetName(aBlipPtr->GetNameId());
      aTrackPtr->SetTargetType(aBlipPtr->GetTypeId());
   }
   aTrackPtr->SetIsFalseTarget(true);

   WsfFalseTargetScreener* screenerPtr(mModeList[aResult.mModeIndex].mScreenerPtr);
   if (screenerPtr != nullptr)
   {
      aTrackPtr->IsReportable(screenerPtr->AllowTrackReporting(aTrackPtr->GetTargetIndex(), aTrackPtr));
   }
}

// =================================================================================================
//! Process false target detection blips
//!
//! @param aSimTime The current simulation time.
//! @param aRequestId The request ID (track ID) for which this is for.
//! @param aSettings  [input] A Settings object that indicates the
//! operational settings to be used for the false target detections.
//!
//! @note Entering a request ID that is 'null' will cause all false targets to be evaluated for tracking.
void WsfEW_SensorComponent::PerformFalseTargetDetections(double aSimTime, WsfSensor::Settings& aSettings, WsfTrackId& aRequestId)
{
   // NOTE: This should not be called unless the sensor is of type WsfRadarSensor.

   WsfSensorMode*          modePtr = GetSensor()->GetModeEntry(aSettings.mModeIndex);
   EW_Mode&                ewMode(mModeList[aSettings.mModeIndex]);
   WsfFalseTargetScreener* screenerPtr = mModeList[aSettings.mModeIndex].mScreenerPtr;

   // Modify the base slewing limits with the mode-specific slewing limits.
   modePtr->UpdateSensorCueingLimits();

   aSettings.mRequiredPd = modePtr->GetRequiredPd();

   // Update the interactors first to set screener invalid if new interactors are present.
   bool detected    = ewMode.UpdateFalseTargetInteractors(aSimTime, *this, aSettings);
   bool needsUpdate = screenerPtr->NeedsUpdated(aSimTime) ||
                      (!aRequestId.IsNull()); // store as it is reset in the mScreener->Update(aSimTime) below

   // Check to see if an update is required, if not return
   if (detected && needsUpdate)
   {
      screenerPtr->Update(aSimTime, aSettings); // This will reset NeedsUpdate
   }

   if ((detected && needsUpdate) || (!detected))
   {
      if (screenerPtr->GetScreenerValid())
      {
         WsfFalseTargetScreener::TrackIdMap blockedTracks = screenerPtr->GetBlockedTargets();
         for (auto& blockedTrack : blockedTracks)
         {
            size_t     objectId = blockedTrack.first;
            WsfTrackId trackId  = blockedTrack.second;
            GetSensor()->GetTracker()->TargetBlocked(aSimTime, tbTrackerSettings, trackId, objectId);
         }
      }

      WsfFalseTarget::Blip* currentBlipPtr;
      // WsfStringInt objectId;
      // WsfStringId blipNameId;
      WsfFalseTarget::RadarSnsrPair radarIdKey = std::make_pair(GetPlatform()->GetNameId(), GetSensor()->GetNameId());
      WsfSensorTracker*             trackerPtr = GetSensor()->GetTracker();

      WsfFalseTargetScreener::BlipIdFTJammerList& blipIdList = screenerPtr->GetTrackableBlipIds();

      for (auto& blipIdtoFTXmtrPair : blipIdList)
      {
         currentBlipPtr = blipIdtoFTXmtrPair.second.first->GetBlipPtr(blipIdtoFTXmtrPair.first,
                                                                      blipIdtoFTXmtrPair.second.second->GetUniqueId());

         if (currentBlipPtr != nullptr)
         {
            if (currentBlipPtr->GetStrength(radarIdKey) == WsfFalseTargetBlip::cUndetected)
            {
               auto out = ut::log::warning()
                          << "DefaultSensorTracker::FalseTargetDetected has gotten an undetected blip.";
               out.AddNote()
                  << "Check the WsfFalseTargetScreener for issues in mTrackableBlipIds and mBloclkedBlipIds.";
               continue;
            }

            // Note: The use of the name id could collide in the tracker statelist
            //       so a special unique id that uses the upper half of the size_t
            //       was set instead.
            // objectId = blipIdtoFTXmtrPair.first;  // The blip name Id for now
            size_t objectId = currentBlipPtr->GetUniqueId(); // The blip name Id for now

            // if ((GetSensor()->GetModeList()->CanMultiSelect()) && aRequestId.IsNull())
            //{
            //    string objectName = objectId + "." + modePtr->GetName();
            //    objectId = WsfStringInt(objectName);
            // }

            WsfSensorResult*    resultPtr = dynamic_cast<WsfSensorResult*>(currentBlipPtr->GetInteraction(radarIdKey));
            WsfSensorScheduler* schedulerPtr(GetSensor()->GetScheduler());
            if (((aRequestId.IsNull())) || (schedulerPtr->GetTrackForRequest(aRequestId).GetTargetIndex() ==
                                            objectId)) // TODO-AWK Needs tested to see if this still works yet.
            {
               WsfPlatform* targetPtr = currentBlipPtr->GetPlatform();
               trackerPtr->TargetDetected(aSimTime, ftTrackerSettings, aRequestId, objectId, targetPtr, *resultPtr);
            }
         }
      }

      int                                droppedTracks(0);
      WsfFalseTargetScreener::TrackIdMap ftTrackIds = screenerPtr->GetFalseTargetTracks();
      for (auto& ftTrackId : ftTrackIds)
      {
         size_t     objectId = ftTrackId.first;
         WsfTrackId trackId  = ftTrackId.second;

         // blipNameId = objectId;
         // if ((GetSensor()->GetModeList()->CanMultiSelect()) && aRequestId.IsNull())
         //{
         //    string blipName = blipNameId;
         //    string searchName = "." + modePtr->GetName();
         //    blipName.erase(blipName.find_last_not_of(searchName) + 1);
         //    blipNameId = blipName;
         // }

         bool dropBlip(false);
         currentBlipPtr = nullptr;
         if (screenerPtr->FindTrackableBlip(objectId))
         {
            currentBlipPtr = screenerPtr->GetTrackableBlip(objectId);
            if (currentBlipPtr == nullptr)
            {
               dropBlip = true;
            }
            else if (currentBlipPtr->GetStrength(radarIdKey) == WsfFalseTargetBlip::cUndetected)
            {
               dropBlip = true;
            }
         }
         else if (screenerPtr->FindBlockedBlip(objectId))
         {
            currentBlipPtr = screenerPtr->GetBlockedBlip(objectId);
            dropBlip       = true;
         }
         else if (screenerPtr->FindUndetectedBlip(objectId))
         {
            currentBlipPtr = screenerPtr->GetUndetectedBlip(objectId);
            dropBlip       = true;
         }

         if (dropBlip && (currentBlipPtr != nullptr))
         {
            WsfSensorResult* resultPtr = dynamic_cast<WsfSensorResult*>(currentBlipPtr->GetInteraction(radarIdKey));
            WsfPlatform*     targetPtr = currentBlipPtr->GetPlatform();
            trackerPtr->TargetUndetected(aSimTime, ftTrackerSettings, trackId, objectId, targetPtr, *resultPtr);
            ++droppedTracks;
         }

         if (currentBlipPtr == nullptr)
         {
            trackerPtr->TargetDeleted(aSimTime, ftTrackerSettings, trackId, objectId);
            ++droppedTracks;
         }
      }

      if (screenerPtr->GetDebugEnabled() && droppedTracks > 0)
      {
         auto out = ut::log::debug() << "Dropped false target tracks.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Dropped Tracks: " << droppedTracks;
      }
   }
}

// =================================================================================================
//! Process jammer strobe detections attempts under the control of a scheduler.
//! @param aSimTime The current simulation time.
void WsfEW_SensorComponent::PerformJamStrobeDetections(double aSimTime)
{
   WsfSensor::Settings settings;
   JamStrobeResultsMap jamStrobeResults;
   WsfSensorTracker*   trackerPtr = GetSensor()->GetTracker();

   for (size_t modeIndex = 0; modeIndex < mModeList.size(); ++modeIndex)
   {
      EW_Mode& ewMode(mModeList[modeIndex]);
      if (ewMode.mUseJamStrobeDetector)
      {
         jamStrobeResults.clear();
         settings.mModeIndex = modeIndex;
         DetectJamStrobes(aSimTime, settings, jamStrobeResults);

         for (auto& jamStrobeResult : jamStrobeResults)
         {
            // If it can be perceived then a track needs injected into the sensor's tracker
            // that is 1D (i.e. bearing only) with the associated error in bearing.
            WsfTrackId requestId;
            requestId.Null();
            WsfEW_Result* ewResultPtr = WsfEW_Result::Find(jamStrobeResult.second);
            if ((ewResultPtr != nullptr) && (ewResultPtr->mPerceivesContJamming || ewResultPtr->mPerceivesPulseJamming))
            {
               ewMode.mJamStrobeDetector.ApplyMeasurementErrors(jamStrobeResult.second);
               trackerPtr->TargetDetected(aSimTime,
                                          jsTrackerSettings,
                                          requestId,
                                          jamStrobeResult.first->GetUniqueId(),
                                          jamStrobeResult.first->GetPlatform(),
                                          jamStrobeResult.second);
            }
            else
            {
               jamStrobeResult.second.mCheckedStatus = WsfSensorResult::cSIGNAL_LEVEL;
               jamStrobeResult.second.mFailedStatus  = WsfSensorResult::cSIGNAL_LEVEL;
               trackerPtr->TargetUndetected(aSimTime,
                                            jsTrackerSettings,
                                            requestId,
                                            jamStrobeResult.first->GetUniqueId(),
                                            jamStrobeResult.first->GetPlatform(),
                                            jamStrobeResult.second);
            }
         }
      }
   }
}

// =================================================================================================
//! Attempt to detect jam strobes.
//!
//! This provides the common interface used to determine if a sensor can detect a jammer strobe.
//! The base class implementation simply invokes the mode-specific DetectJamStrobes method for
//! the currently selected mode.
//!
//! @param aSimTime  [input] The current simulation time.
//! @param aSettings [input] A Settings object that indicates the operational settings to be used for the detection attempt.
//! @param aResults  [input] The results map for the jamming interaction with the sensor.
//!
//! @returns true if jam strobes are detected.
bool WsfEW_SensorComponent::DetectJamStrobes(double aSimTime, WsfSensor::Settings& aSettings, JamStrobeResultsMap& aResults)
{
   WsfSensorMode* modePtr(GetSensor()->GetModeEntry(aSettings.mModeIndex));
   EW_Mode&       ewMode(mModeList[aSettings.mModeIndex]);
   // Modify the base slewing limits with the mode-specific slewing limits.
   modePtr->UpdateSensorCueingLimits();
   return ewMode.DetectJamStrobes(*this, aSimTime, aSettings, aResults);
}

// =================================================================================================
// Nested class EW_Beam
// =================================================================================================

// =================================================================================================
WsfEW_SensorComponent::EW_Beam::EW_Beam()
   : mBeamIndex(0)
   , mContJNR_PerceptionThreshold(1.0e38)
   , mPulsedJNR_PerceptionThreshold(1.0e38)
   , mCoherentJNR_PerceptionThreshold(1.0e38)
{
}

// =================================================================================================
//! Checks to see if jamming can be perceived by the sensor beam.
//! @returns 'true' if jamming can be perceived by the sensor beam, 'false' otherwise.
bool WsfEW_SensorComponent::EW_Beam::CanPerceiveJamming() const
{
   return (((mPulsedJNR_PerceptionThreshold - 1.0e38) < 0.001) || ((mContJNR_PerceptionThreshold - 1.0e38) < 0.001) ||
           ((mCoherentJNR_PerceptionThreshold - 1.0e38) < 0.001));
}

// =================================================================================================
bool WsfEW_SensorComponent::EW_Beam::DetectJamStrobes(WsfEW_SensorComponent& aEW_Sensor,
                                                      double                 aSimTime,
                                                      WsfSensor::Settings&   aSettings,
                                                      JamStrobeResultsMap&   aResults)
{
   bool detected(false);

   WsfSensorMode* modePtr   = aEW_Sensor.GetSensor()->GetModeEntry(aSettings.mModeIndex);
   auto           beamEntry = modePtr->GetBeamEntry(mBeamIndex);
   if (beamEntry)
   {
      WsfEM_Rcvr* rcvrPtr = beamEntry->GetEM_Rcvr();
      size_t      interactorCount(rcvrPtr->GetInterferenceInteractorCount());
      if (interactorCount > 0)
      {
         WsfSensorResult jammerResult;
         WsfEW_Result&   ewJammerResult = WsfEW_Result::FindOrCreate(jammerResult);
         for (size_t xmtrIndex = 0; xmtrIndex < interactorCount; ++xmtrIndex)
         {
            WsfEM_Xmtr* jammerXmtrPtr = rcvrPtr->GetInterferenceInteractorEntry(xmtrIndex);
            jammerResult.Reset(aSettings);
            jammerResult.BeginGenericInteraction(jammerXmtrPtr, nullptr, rcvrPtr);
            jammerResult.ComputeUndefinedGeometry();
            jammerResult.SetReceiverBeamPosition();
            WsfSensorResult targetInteraction(jammerResult);
            WsfRF_Jammer::ComputeSingleJammerEffect(aSimTime, jammerXmtrPtr, targetInteraction, jammerResult, false);

            // TODO - doesn't account for coherent receiver EP gains (i.e. Signal effects defined in receiver EP blocks);
            //         ComputeJammerPowers(...) does not add in the rcvr side gains right now.
            ewJammerResult.mCoherentJammerPower *= ewJammerResult.mEW_Effects.mEW_SignalEffect.mSignalPowerGain;

            // Compute the jammer-to-noise ratios for the perceives jamming flag.
            EW_Mode& ewMode(aEW_Sensor.GetOrCreateEW_Mode(*modePtr));
            if (PerceivesJamming(jammerResult,
                                 ewMode.mJamStrobeDetector.GetContJNR_PerceptionThreshold(),
                                 ewMode.mJamStrobeDetector.GetPulsedJNR_PerceptionThreshold(),
                                 ewMode.mJamStrobeDetector.GetCoherentJNR_PerceptionThreshold(),
                                 ewJammerResult.mPerceivesContJamming,
                                 ewJammerResult.mPerceivesPulseJamming,
                                 ewJammerResult.mPerceivesCoherentJamming))
            {
               detected = true;
               jammerResult.mRcvdPower =
                  std::max(std::max(ewJammerResult.mNoiseJammerPower, ewJammerResult.mPulseJammerPower),
                           ewJammerResult.mCoherentJammerPower);
               jammerResult.mSignalToNoise = rcvrPtr->ComputeSignalToNoise(jammerResult.mRcvdPower, 0.0, 0.0);
            }

            if ((aResults.find(jammerXmtrPtr) == aResults.end()) ||
                (jammerResult.mSignalToNoise > aResults[jammerXmtrPtr].mSignalToNoise))
            {
               aResults[jammerXmtrPtr] = jammerResult;
            }
         }
      }
   }
   return detected;
}

// =================================================================================================
// Nested class SensorMode
// =================================================================================================

// =================================================================================================
WsfEW_SensorComponent::EW_Mode::EW_Mode()
   : mBeamList()
   , mScreenerPtr(nullptr)
   , mJammingPerceptionResetTime(-1.0)
   , mContJammingFirstPerceived(-1.0)
   , mContJammingLastPerceived(-1.0)
   , mPulseJammingFirstPerceived(-1.0)
   , mPulseJammingLastPerceived(-1.0)
   , mCoherentJammingFirstPerceived(-1.0)
   , mCoherentJammingLastPerceived(-1.0)
   , mJamStrobeDetector()
   , mUseJamStrobeDetector(false)
   , mAccumPerceivesContJamming(false)
   , mAccumPerceivesPulseJamming(false)
   , mAccumPerceivesCoherentJamming(false)
   , mMaintainTrackMeasurementHistory(false)
{
   mBeamList.emplace_back();
}

// =================================================================================================
WsfEW_SensorComponent::EW_Mode::EW_Mode(const EW_Mode& aSrc)
   : mBeamList(aSrc.mBeamList)
   , mScreenerPtr(nullptr)
   , mJammingPerceptionResetTime(aSrc.mJammingPerceptionResetTime)
   , mContJammingFirstPerceived(aSrc.mContJammingFirstPerceived)
   , mContJammingLastPerceived(aSrc.mContJammingLastPerceived)
   , mPulseJammingFirstPerceived(aSrc.mPulseJammingFirstPerceived)
   , mPulseJammingLastPerceived(aSrc.mPulseJammingLastPerceived)
   , mCoherentJammingFirstPerceived(aSrc.mCoherentJammingFirstPerceived)
   , mCoherentJammingLastPerceived(aSrc.mCoherentJammingLastPerceived)
   , mJamStrobeDetector(aSrc.mJamStrobeDetector)
   , mUseJamStrobeDetector(aSrc.mUseJamStrobeDetector)
   , mAccumPerceivesContJamming(false)
   , mAccumPerceivesPulseJamming(false)
   , mAccumPerceivesCoherentJamming(false)
   , mMaintainTrackMeasurementHistory(aSrc.mMaintainTrackMeasurementHistory)
{
   if (aSrc.mScreenerPtr != nullptr)
   {
      mScreenerPtr = aSrc.mScreenerPtr->Clone();
   }
}

// =================================================================================================
WsfEW_SensorComponent::EW_Mode::~EW_Mode()
{
   delete mScreenerPtr;
}

// =================================================================================================
//! Checks to see if jamming is being perceived by the sensor.
//! @returns 'true' if jamming is perceived by the sensor, 'false' otherwise.
bool WsfEW_SensorComponent::EW_Mode::JammingPerceived()
{
   return ((mContJammingLastPerceived >= 0.0) || (mPulseJammingLastPerceived >= 0.0) ||
           (mCoherentJammingLastPerceived >= 0.0));
}

// =================================================================================================
//! Returns the time in which continuous, pulse, or coherent jamming was first perceived by the sensor.
//! @returns the minimum simulation time when continuous/pulse/coherent jamming was first perceived.
double WsfEW_SensorComponent::EW_Mode::GetJammingFirstPerceived() const
{
   double contFirstPerceived     = (mContJammingFirstPerceived == -1.0) ? DBL_MAX : mContJammingFirstPerceived;
   double pulseFirstPerceived    = (mPulseJammingFirstPerceived == -1.0) ? DBL_MAX : mPulseJammingFirstPerceived;
   double coherentFirstPerceived = (mCoherentJammingFirstPerceived == -1.0) ? DBL_MAX : mCoherentJammingFirstPerceived;

   double perTime = std::min(contFirstPerceived, std::min(pulseFirstPerceived, coherentFirstPerceived));

   return (perTime == DBL_MAX ? -1.0 : perTime);
}

// =================================================================================================
//! Returns the time in which continuous, pulse, or coherent jamming was last perceived by the sensor.
//! @returns the maximum simulation time when continuous/pulse/coherent jamming was last perceived.
double WsfEW_SensorComponent::EW_Mode::GetJammingLastPerceived() const
{
   return std::max(mContJammingLastPerceived, std::max(mPulseJammingLastPerceived, mCoherentJammingLastPerceived));
}

// =================================================================================================
//! Checks to see if continuous jamming is being perceived by the sensor.
//! @returns 'true' if jamming is perceived by the sensor, 'false' otherwise.
bool WsfEW_SensorComponent::EW_Mode::ContinuousJammingPerceived()
{
   return (mContJammingLastPerceived >= 0.0);
}

// =================================================================================================
//! Checks to see if jamming is being perceived by the sensor.
//! @returns 'true' if jamming is perceived by the sensor, 'false' otherwise.
bool WsfEW_SensorComponent::EW_Mode::PulseJammingPerceived()
{
   return (mPulseJammingLastPerceived >= 0.0);
}

// =================================================================================================
bool WsfEW_SensorComponent::EW_Mode::DetectJamStrobes(WsfEW_SensorComponent& aEW_Sensor,
                                                      double                 aSimTime,
                                                      WsfSensor::Settings&   aSettings,
                                                      JamStrobeResultsMap&   aResults)
{
   bool detected(false);

   for (auto& beam : mBeamList)
   {
      detected |= beam.DetectJamStrobes(aEW_Sensor, aSimTime, aSettings, aResults);
   }

   if (detected)
   {
      bool jammingPreviouslyPerceived = JammingPerceived();

      // Update jamming perception times.
      if (mContJammingFirstPerceived < 0.0)
      {
         mContJammingFirstPerceived = aSimTime;
      }
      mContJammingLastPerceived = aSimTime;

      if (mPulseJammingFirstPerceived < 0.0)
      {
         mPulseJammingFirstPerceived = aSimTime;
      }
      mPulseJammingLastPerceived = aSimTime;

      if (mCoherentJammingFirstPerceived < 0.0)
      {
         mCoherentJammingFirstPerceived = aSimTime;
      }
      mCoherentJammingLastPerceived = aSimTime;

      // If jamming wasn't PREVIOUSLY perceived then this is the first time we've seen it.
      // If 'Perceive-Jamming-First' then we delay returning any results until the next iteration.
      if ((!jammingPreviouslyPerceived) && mJamStrobeDetector.PerceiveJammingFirst())
      {
         aResults.clear();
      }
   }
   else
   {
      // Jam strobe not detected. Determine if the perception of jamming should be reset.
      if ((mContJammingLastPerceived >= 0.0) && ((aSimTime - mContJammingLastPerceived) > mJammingPerceptionResetTime))
      {
         mContJammingFirstPerceived = -1.0;
         mContJammingLastPerceived  = -1.0;
      }
      if ((mPulseJammingLastPerceived >= 0.0) && ((aSimTime - mPulseJammingLastPerceived) > mJammingPerceptionResetTime))
      {
         mPulseJammingFirstPerceived = -1.0;
         mPulseJammingLastPerceived  = -1.0;
      }
      if ((mCoherentJammingLastPerceived >= 0.0) &&
          ((aSimTime - mCoherentJammingLastPerceived) > mJammingPerceptionResetTime))
      {
         mCoherentJammingFirstPerceived = -1.0;
         mCoherentJammingLastPerceived  = -1.0;
      }
   }
   return detected;
}

// =================================================================================================
bool WsfEW_SensorComponent::EW_Mode::UpdateFalseTargetInteractors(double                 aSimTime,
                                                                  WsfEW_SensorComponent& aEW_Sensor,
                                                                  WsfSensor::Settings&   aSettings)
{
   WsfSensorMode* modePtr = aEW_Sensor.GetSensor()->GetModeEntry(aSettings.mModeIndex);

   std::map<WsfFalseTargetScreener::FT_JammerIdPair, size_t> ftJamIdToBeamMap; // use ids for consistency/order in
                                                                               // regression testing
   std::map<WsfFalseTargetScreener::FT_JammerIdPair, WsfEW_FalseTargetEffect*>              ftJamIdToEffectMap;
   std::map<WsfFalseTargetScreener::FT_JammerIdPair, WsfFalseTargetScreener::FT_JammerPair> ftJamIdsToFtJamMap;
   WsfFalseTargetScreener::FT_JammerPair                                                    ftJamPair;
   WsfFalseTargetScreener::FT_JammerIdPair                                                  ftJamIdPair;

   WsfEW_FalseTargetEffect* ftEffectPtr = nullptr;

   for (size_t beamIndex = 0; beamIndex < modePtr->GetBeamCount(); ++beamIndex)
   {
      WsfSensorBeam* beamPtr = modePtr->GetBeamEntry(beamIndex);
      if (beamPtr)
      {
         WsfEM_Rcvr* rcvrPtr = beamPtr->GetEM_Rcvr();
         WsfEM_Xmtr* xmtrPtr = beamPtr->GetEM_Xmtr();

         // step through possible jammer/receiver interactions
         for (size_t i = 0; i < rcvrPtr->GetInterferenceInteractorCount(); ++i)
         {
            WsfEM_Xmtr*  jammerXmtrPtr = rcvrPtr->GetInterferenceInteractorEntry(i);
            WsfEW_EA_EP* eaPtr         = WsfEW_EA::GetElectronicAttack(*jammerXmtrPtr);
            if (eaPtr == nullptr)
            {
               continue;
            }

            // Find false target techniques and its associated false target effect
            WsfEW_Types::TechniqueIdVec eaTechniquesVector = eaPtr->GetTechniquesInUse();
            for (const auto& techIndex : eaTechniquesVector)
            {
               WsfEW_Technique*              techPtr      = eaPtr->GetTechnique(techIndex);
               WsfEW_Technique::EffectPtrVec eaEffectsVec = techPtr->GetEffects();

               for (auto& eaEffect : eaEffectsVec)
               {
                  ftEffectPtr = dynamic_cast<WsfEW_FalseTargetEffect*>(eaEffect);
                  if (ftEffectPtr == nullptr)
                  {
                     continue;
                  }

                  WsfFalseTarget* falseTargetPtr = ftEffectPtr->GetFT_Ptr(rcvrPtr->GetArticulatedPart()->GetTypeId());
                  if (falseTargetPtr == nullptr)
                  {
                     continue;
                  }

                  ftJamPair   = std::make_pair(falseTargetPtr, jammerXmtrPtr);
                  ftJamIdPair = std::make_pair(falseTargetPtr->GetNameId(), jammerXmtrPtr->GetUniqueId());
                  if (ftJamIdToBeamMap.find(ftJamIdPair) == ftJamIdToBeamMap.end())
                  {
                     ftJamIdToBeamMap[ftJamIdPair]   = static_cast<size_t>(-1); // use special size_t number
                     ftJamIdToEffectMap[ftJamIdPair] = nullptr;
                     ftJamIdsToFtJamMap[ftJamIdPair] = ftJamPair;
                  }

                  // Check the power and setup the effect if successful..
                  if (falseTargetPtr->PowerSufficiencyCheck(aSimTime, jammerXmtrPtr, xmtrPtr, rcvrPtr))
                  {
                     ftJamIdToBeamMap[ftJamIdPair]   = beamIndex;
                     ftJamIdToEffectMap[ftJamIdPair] = ftEffectPtr;
                  }
               }
            }
         }
      }
   }

   bool falseTargetsPresent(false);
   for (auto& ftJamIdBeamMapIter : ftJamIdToBeamMap)
   {
      if (ftJamIdBeamMapIter.second != static_cast<size_t>(-1))
      {
         falseTargetsPresent    = true;
         ftJamPair              = ftJamIdsToFtJamMap.find(ftJamIdBeamMapIter.first)->second;
         ftEffectPtr            = ftJamIdToEffectMap.find(ftJamIdBeamMapIter.first)->second;
         WsfSensorBeam* beamPtr = modePtr->GetBeamEntry(ftJamIdBeamMapIter.second);
         if (beamPtr)
         {
            WsfEM_Rcvr* rcvrPtr = beamPtr->GetEM_Rcvr();
            WsfEM_Xmtr* xmtrPtr = beamPtr->GetEM_Xmtr();
            if (mScreenerPtr->FindFalseTarget(ftJamPair))
            {
               ftEffectPtr->UpdateFT(*rcvrPtr, *xmtrPtr);
            }
            else if (ftEffectPtr->InitializeFT(*rcvrPtr, *xmtrPtr))
            {
               ftEffectPtr->UpdateFT(*rcvrPtr, *xmtrPtr);
               mScreenerPtr->AddFalseTarget(ftJamPair); // add it to the screener's list of fts
            }
         }
      }
      else if (mScreenerPtr->FindFalseTarget(ftJamPair)) // if not, remove it if it exists
      {
         mScreenerPtr->RemoveFalseTarget(ftJamPair);
      }
   }
   return falseTargetsPresent;
}

// =================================================================================================
//! Checks to see if jamming is being perceived by the sensor.
//! @note  Currently assumes that the receiver clutter power was previously calculated.
//! @param aSimTime The current simulation time.
//! @param aResult  On input this contains the current information for the interaction.
//! @returns 'true' if jamming is perceived by the sensor, 'false' otherwise.
bool WsfEW_SensorComponent::PerceivesJamming(const WsfSensorResult& aResult,
                                             double                 aContJNR_Threshold,
                                             double                 aPulsedJNR_Threshold,
                                             double                 aCoherentJNR_Threshold,
                                             bool&                  aPerceivesContJamming,
                                             bool&                  aPerceivesPulseJamming,
                                             bool&                  aPerceivesCoherentJamming)
{
   aPerceivesContJamming     = PerceivesContinuousJamming(aResult, aContJNR_Threshold);
   aPerceivesPulseJamming    = PerceivesPulseJamming(aResult, aPulsedJNR_Threshold);
   aPerceivesCoherentJamming = PerceivesCoherentJamming(aResult, aCoherentJNR_Threshold);
   return aPerceivesContJamming || aPerceivesPulseJamming || aPerceivesCoherentJamming;
}

// =================================================================================================
//! Checks to see if continuous jamming is being perceived by the sensor.
//! @note  Currently assumes that the receiver clutter power was previously calculated.
//! @param aSimTime The current simulation time.
//! @param aResult  On input this contains the current information for the interaction.
//! @returns 'true' if jamming is perceived by the sensor, 'false' otherwise.
bool WsfEW_SensorComponent::PerceivesContinuousJamming(const WsfSensorResult& aResult, double aContJNR_Threshold)
{
   WsfEM_Rcvr*   rcvrPtr     = aResult.GetReceiver();
   WsfEW_Result* ewResultPtr = WsfEW_Result::Find(aResult);
   if ((rcvrPtr == nullptr) || (ewResultPtr == nullptr))
   {
      return false;
   }
   WsfEW_Result& ewResult = *ewResultPtr;

   // Compute the jammer-to-noise ratios for the perceives jamming flag.
   double noiseJammerPower = ewResult.mNoiseJammerPower /
                             (ewResult.mEW_Effects.mEW_NoiseJammingEffect.mBlanking *
                              ewResult.mEW_Effects.mEW_NoiseJammingEffect.mCancelation *
                              ewResult.mEW_Effects.mEW_NoiseJammingEffect.mJammingPowerGain *
                              ewResult.mEW_Effects.mEW_NoiseJammingEffect.mJtoXGain/* *
                                                            ewResult.mEW_Effects.mEW_NoiseJammingEffect.mPulseSuppression *
                                                            ewResult.mEW_Effects.mEW_NoiseJammingEffect.mRPJ_Gain*/);

   double contJammerToNoise = rcvrPtr->ComputeSignalToNoise(noiseJammerPower, aResult.mClutterPower, 0.0);

   return (contJammerToNoise >= aContJNR_Threshold);
}

// =================================================================================================
//! Checks to see if pulse jamming is being perceived by the sensor.
//! @note  Currently assumes that the receiver clutter power was previously calculated.
//! @param aSimTime The current simulation time.
//! @param aResult  On input this contains the current information for the interaction.
//! @returns 'true' if jamming is perceived by the sensor, 'false' otherwise.
bool WsfEW_SensorComponent::PerceivesPulseJamming(const WsfSensorResult& aResult, double aPulsedJNR_Threshold)
{
   WsfEM_Rcvr*   rcvrPtr     = aResult.GetReceiver();
   WsfEW_Result* ewResultPtr = WsfEW_Result::Find(aResult);
   if ((rcvrPtr == nullptr) || (ewResultPtr == nullptr))
   {
      return false;
   }
   WsfEW_Result& ewResult = *ewResultPtr;

   // Compute the pulse and coherent jammer-to-noise ratios for the perceives jamming flag.
   double pulseJammerPower    = ewResult.mPulseJammerPower / (ewResult.mEW_Effects.mEW_PulseJammingEffect.mBlanking *
                                                           ewResult.mEW_Effects.mEW_PulseJammingEffect.mCancelation *
                                                           ewResult.mEW_Effects.mEW_PulseJammingEffect.mJammingPowerGain *
                                                           ewResult.mEW_Effects.mEW_PulseJammingEffect.mJtoXGain *
                                                           ewResult.mEW_Effects.mEW_PulseJammingEffect.mPulseSuppression *
                                                           ewResult.mEW_Effects.mEW_PulseJammingEffect.mRPJ_Gain);
   double pulsedJammerToNoise = rcvrPtr->ComputeSignalToNoise(pulseJammerPower, aResult.mClutterPower, 0.0);
   return (pulsedJammerToNoise >= aPulsedJNR_Threshold);
}

bool WsfEW_SensorComponent::PerceivesCoherentJamming(const WsfSensorResult& aResult, double aCoherentJNR_Threshold)
{
   WsfEM_Rcvr*   rcvrPtr     = aResult.GetReceiver();
   WsfEW_Result* ewResultPtr = WsfEW_Result::Find(aResult);
   if ((rcvrPtr == nullptr) || (ewResultPtr == nullptr))
   {
      return false;
   }
   WsfEW_Result& ewResult = *ewResultPtr;

   double coherentJammerPower =
      ewResult.mCoherentJammerPower / (ewResult.mEW_Effects.mEW_CoherentJammingEffect.mBlanking *
                                       ewResult.mEW_Effects.mEW_CoherentJammingEffect.mCancelation *
                                       ewResult.mEW_Effects.mEW_CoherentJammingEffect.mJammingPowerGain *
                                       ewResult.mEW_Effects.mEW_CoherentJammingEffect.mJtoXGain *
                                       ewResult.mEW_Effects.mEW_CoherentJammingEffect.mPulseSuppression *
                                       ewResult.mEW_Effects.mEW_CoherentJammingEffect.mRPJ_Gain);
   double coherentJammerToNoise = rcvrPtr->ComputeSignalToNoise(coherentJammerPower, aResult.mClutterPower, 0.0);
   return (coherentJammerToNoise >= aCoherentJNR_Threshold);
}
