// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSOSM_Sensor.hpp"

#include "SOSM_Manager.hpp"
#include "SOSM_Sensor.hpp"
#include "SOSM_SensorTarget.hpp"
#include "SOSM_Target.hpp"
#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfPlatform.hpp"
#include "WsfSOSM_Interaction.hpp"
#include "WsfSOSM_Interface.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfSOSM_Sensor::WsfSOSM_Sensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mSOSM_ModeList()
   , mSOSM_InterfacePtr(nullptr)
{
   SetClass(cPASSIVE | cINFRARED); // This is a passive EO/IR sensor
   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new SOSM_Mode()));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

// =================================================================================================
WsfSOSM_Sensor::WsfSOSM_Sensor(const WsfSOSM_Sensor& aSrc)
   : WsfSensor(aSrc)
   , mSOSM_ModeList()
   , mSOSM_InterfacePtr(aSrc.mSOSM_InterfacePtr)
{
}

// =================================================================================================
// virtual
WsfSensor* WsfSOSM_Sensor::Clone() const
{
   return new WsfSOSM_Sensor(*this);
}

// =================================================================================================
// virtual
size_t WsfSOSM_Sensor::GetEM_RcvrCount() const
{
   // Returns 0 prior to initialization
   return mSOSM_ModeList.empty() ? 0U : 1U;
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfSOSM_Sensor::GetEM_Rcvr(size_t aIndex) const
{
   return mSOSM_ModeList[mModeListPtr->GetCurrentMode()]->mRcvr;
}

// =================================================================================================
// virtual
bool WsfSOSM_Sensor::Initialize(double aSimTime)
{
   mSOSM_InterfacePtr = static_cast<WsfSOSM_Interface*>(GetScenario().FindExtension("wsf_sosm"));

   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mSOSM_ModeList);

   return ok;
}

// =================================================================================================
// virtual
bool WsfSOSM_Sensor::ProcessInput(UtInput& aInput)
{
   return WsfSensor::ProcessInput(aInput);
}

// =================================================================================================
// virtual
void WsfSOSM_Sensor::Update(double aSimTime)
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
// virtual
void WsfSOSM_Sensor::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   // Inform each of the modes so they can clean up their target lists.
   for (size_t mi = 0; mi < mSOSM_ModeList.size(); ++mi)
   {
      mSOSM_ModeList[mi]->PlatformDeleted(aPlatformPtr);
   }
   WsfSensor::PlatformDeleted(aPlatformPtr);
}

// =================================================================================================
// Nested class SOSM_Mode
// =================================================================================================

// =================================================================================================
WsfSOSM_Sensor::SOSM_Mode::SOSM_Mode()
   : WsfSensorMode()
   , mSOSM_SensorType()
   , mAntenna()
   , mRcvr(WsfEM_Rcvr::cRF_SENSOR, &mAntenna)
   , mTargetMap()
   , mRangingTime(0.0)
   , mRangingTimeTrackQuality(0.0)
{
}

// =================================================================================================
WsfSOSM_Sensor::SOSM_Mode::SOSM_Mode(const SOSM_Mode& aSrc)
   : WsfSensorMode(aSrc)
   , mSOSM_SensorType(aSrc.mSOSM_SensorType)
   , mAntenna(aSrc.mAntenna)
   , mRcvr(aSrc.mRcvr, &mAntenna)
   , mTargetMap()
   , mRangingTime(aSrc.mRangingTime)
   , mRangingTimeTrackQuality(aSrc.mRangingTimeTrackQuality)
{
}

// =================================================================================================
// virtual
WsfSOSM_Sensor::SOSM_Mode::~SOSM_Mode()
{
   // Remove all entries from the target list
   for (TargetMap::iterator tmi = mTargetMap.begin(); tmi != mTargetMap.end(); ++tmi)
   {
      delete tmi->second;
   }
}

// =================================================================================================
// virtual
WsfMode* WsfSOSM_Sensor::SOSM_Mode::Clone() const
{
   return new SOSM_Mode(*this);
}

// =================================================================================================
// virtual
bool WsfSOSM_Sensor::SOSM_Mode::Initialize(double aSimTime)
{
   // Let the base class do its thing.
   bool ok = WsfSensorMode::Initialize(aSimTime);

   // If an SOSM sensor type was not provided, see if there is a mapping (explicit or implicit)
   if (mSOSM_SensorType.IsNull())
   {
      mSOSM_SensorType = GetSOSM_Interface()->GetSensorType(this);
   }

   // Make sure the specified SOSM sensor type exists.

   SOSM_Sensor* sensorPtr = GetSOSM_Interface()->GetManager().FindSensorType(mSOSM_SensorType.GetString());
   if (sensorPtr == nullptr)
   {
      auto logError = ut::log::error() << "Unable to locate SOSM sensor type.";
      logError.AddNote() << "SOSM_Sensor Type: " << mSOSM_SensorType;
      return false;
   }

   if (mSensorPtr->DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "WsfSOSM_Sensor::SOSM_Mode::Initialize Report.";
      logDebug.AddNote() << "T = " << aSimTime;
      logDebug.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
      logDebug.AddNote() << "Sensor: " << mSensorPtr->GetName();
      logDebug.AddNote() << "Mode: " << GetName();
      logDebug.AddNote() << "SOSM Sensor Type: " << sensorPtr->GetTypeName();
   }

   // Initialize the antenna/receiver objects.

   double lowerWavenumber = sensorPtr->Origin();
   double upperWavenumber = sensorPtr->Origin() + (sensorPtr->Count() * sensorPtr->Increment());
   double upperFrequency  = 1.0E+4 / lowerWavenumber;
   double lowerFrequency  = 1.0E+4 / upperWavenumber;
   mRcvr.SetFrequency(0.5 * (lowerFrequency + upperFrequency));
   mRcvr.SetBandwidth(upperFrequency - lowerFrequency);
   mRcvr.SetDetectionThreshold(sensorPtr->GetDetectionThresholdAboveHorizon());
   mRcvr.SetNoisePower(sensorPtr->GetNoiseEquivalentIrradiance());
   ok &= mAntenna.Initialize(mSensorPtr);
   ok &= mRcvr.Initialize(*GetSimulation());

   // Set the debug flag
   mRcvr.SetDebugEnabled(mSensorPtr->DebugEnabled());

   return ok;
}

// =================================================================================================
// virtual
bool WsfSOSM_Sensor::SOSM_Mode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "sosm_sensor_type")
   {
      std::string sensorType;
      aInput.ReadValue(sensorType);
      mSOSM_SensorType = sensorType;
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
   else if (mAntenna.ProcessInput(aInput) || mRcvr.ProcessInput(aInput) || mRcvr.ProcessInputBlock(aInput))
   {
   }
   else
   {
      myCommand = WsfSensorMode::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
bool WsfSOSM_Sensor::SOSM_Mode::AttemptToDetect(double           aSimTime,
                                                WsfPlatform*     aTargetPtr,
                                                Settings&        aSettings,
                                                WsfSensorResult& aResult)
{
   bool detected = false;
   aResult.Reset(aSettings);

   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current
   aTargetPtr->Update(aSimTime);          // Ensure the target position is current
   if (GetSensor()->DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "WsfSOSM_Sensor::SOSM_Mode::AttemptToDetect Report.";
      logDebug.AddNote() << "T = " << aSimTime;
      logDebug.AddNote() << "Platform: " << GetPlatform()->GetName();
      logDebug.AddNote() << "Sensor: " << GetSensor()->GetName();
      logDebug.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   // Determine if concealed (like in a building).
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= Result::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         aResult.mFailedStatus |= Result::cCONCEALMENT;
         // Must have object pointers so event_output and debug output show locations.
         aResult.BeginGenericInteraction(nullptr, aTargetPtr, &mRcvr);
      }
   }

   if ((aResult.mFailedStatus == 0) && (aResult.BeginOneWayInteraction(&mRcvr, aTargetPtr) == 0))
   {
      // Locate the sensor/target pair - allocate one if this is a new target.

      SOSM_SensorTarget*  pairPtr = nullptr;
      TargetMap::iterator tmi     = mTargetMap.find(aTargetPtr->GetIndex());
      if (tmi != mTargetMap.end())
      {
         pairPtr = tmi->second;
      }
      else
      {
         WsfSOSM_Interface* interfacePtr = GetSOSM_Interface();
         pairPtr = interfacePtr->GetManager().AllocateSensorTargetPair(mSOSM_SensorType.GetString(),
                                                                       interfacePtr->GetTargetType(aTargetPtr));
         mTargetMap[aTargetPtr->GetIndex()] = pairPtr;
      }

      // Set the position of the 'antenna'.
      aResult.SetReceiverBeamPosition();

      // Set the az/el for the infrared signature (aResult.mInfraredSigAz/El).
      aResult.ComputeInfraredSigAzEl();

      // Set the az/el for the optical signature (aResult.mOpticalSigAz/El).
      // This should be exactly the same as for the infrared case.
      aResult.ComputeOpticalSigAzEl();

      if ((pairPtr->GetSensor() != nullptr) && (pairPtr->GetTarget() != nullptr))
      {
         // Call the SOSM sensor model to compute the effect target irradiance.
         WsfSOSM_Interaction interaction(aResult);
         double              effectiveTargetIrradiance = pairPtr->ComputeTargetIrradiance(interaction);

         // Account for structural masking. Note that SOSM also provides an 'installation adjustment' that
         // does the same thing, but this also allows the WSF user to use its implementation.
         effectiveTargetIrradiance *= aResult.mMaskingFactor;

         // Call the SOSM sensor model to compute the probability of detection
         aResult.mPd = pairPtr->ComputeProbabilityOfDetection(static_cast<float>(effectiveTargetIrradiance), interaction);

         // Propagate values computed by SOSM into our 'Result' for use by observers.
         double targetIrradiance           = effectiveTargetIrradiance * interaction.mInstallationAdjustment; // W/cm^2
         float  targetArea                 = interaction.mBodyArea + interaction.mPlumeArea;
         aResult.mRcvdPower                = targetIrradiance * 1.0E+4; // W/m^2
         aResult.mSignalToNoise            = targetIrradiance / interaction.mNoiseEquivalentIrradiance;
         aResult.mOpticalSig               = interaction.mBodyArea * 1.0E-4; // cm^2 -> m^2
         double infraredSig                = interaction.mBodyIntensity + interaction.mPlumeIntensity;
         aResult.mInfraredSig              = infraredSig;                                    // W/sr
         aResult.mContrastRadiantIntensity = interaction.mContrastIntensity;                 // W/sr
         aResult.mBackgroundRadiantIntensity = interaction.mBackgroundRadiance * targetArea; // W/sr
         // TODO aResult.mForegroundRadiantIntensity = interaction.mForegroundRadiance * targetArea;     // W/sr
         aResult.mAbsorptionFactor   = interaction.mTransmittance;
         aResult.mDetectionThreshold = interaction.mDetectionThreshold;
      }

      // Determine if the Pd exceeds the detection threshold.
      aResult.mCheckedStatus |= Result::cSIGNAL_LEVEL;
      if (aResult.mPd >= aSettings.mRequiredPd)
      {
         // Check terrain masking only if all other checks succeed
         detected = !aResult.MaskedByTerrain();
      }
      else
      {
         aResult.mFailedStatus |= Result::cSIGNAL_LEVEL;
      }
   }

   detected &= WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);
   detected &= GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, aResult);
   if (GetSensor()->DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "Sensor Debug Info: ";
      aResult.Print(logDebug);
   }
   GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);
   return detected;
}

// =================================================================================================
// virtual
void WsfSOSM_Sensor::SOSM_Mode::Deselect(double aSimTime)
{
   // The mode is being deselected. Remove all the entries from the target list.
   for (TargetMap::iterator tmi = mTargetMap.begin(); tmi != mTargetMap.end(); ++tmi)
   {
      delete tmi->second;
   }
   mTargetMap.clear();
   mRcvr.Deactivate();
}

// =================================================================================================
// virtual
void WsfSOSM_Sensor::SOSM_Mode::Select(double aSimTime)
{
   mRcvr.Activate();
}

// =================================================================================================
// virtual
void WsfSOSM_Sensor::SOSM_Mode::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   // A platform has been deleted. Remove it from the target list.
   TargetMap::iterator tmi = mTargetMap.find(aPlatformPtr->GetIndex());
   if (tmi != mTargetMap.end())
   {
      delete tmi->second;
      mTargetMap.erase(tmi);
   }
}

// =================================================================================================
//! @copydoc WsfSensorMode::UpdateTrackReportingFlags(double aSimTime, WsfTrack* aTrackPtr)
//!
//! This function overrides the base class method to set the track validity flags as appropriate.
// virtual
void WsfSOSM_Sensor::SOSM_Mode::UpdateTrackReportingFlags(double aSimTime, WsfTrack* aTrackPtr)
{
   WsfSensorMode::UpdateTrackReportingFlags(aSimTime, aTrackPtr); // Call the base class first

   // If 'ranging_time' has been defined and if sufficient time has elapsed, then modify the environment
   // so that range will get reported.
   //
   // The ranging_time is a concept brought over from IWARS.  It basically simulates the fact that an ESM
   // system could triangulate and get the range after a sufficient period of time. Of course this really
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

WsfSOSM_Interface* WsfSOSM_Sensor::SOSM_Mode::GetSOSM_Interface() const
{
   return mSensorPtr ? dynamic_cast<WsfSOSM_Sensor*>(mSensorPtr)->GetSOSM_Interface() : nullptr;
}
