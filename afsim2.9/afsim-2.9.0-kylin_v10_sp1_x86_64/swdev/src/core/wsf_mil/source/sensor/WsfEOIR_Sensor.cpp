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

#include "WsfEOIR_Sensor.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfImage.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorScheduler.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfVideoMessage.hpp"

// ================================================================================================
WsfEOIR_Sensor::WsfEOIR_Sensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mEOIR_ModeList()
   , mChances()
   , mStreamNumber(0)
   , mFrameNumber(0)
   , mCallSensorTrackObservers(false)
{
   SetClass(cPASSIVE | cINFRARED | cVISUAL | cIMAGING); // This is a passive EO/IR sensor
   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new EOIR_Mode(aScenario)));

   // Assign the eoir sensor scheduler.
   SetScheduler(ut::make_unique<EOIR_SensorScheduler>());
}

// ================================================================================================
WsfEOIR_Sensor::WsfEOIR_Sensor(const WsfEOIR_Sensor& aSrc)
   : WsfSensor(aSrc)
   , mEOIR_ModeList()
   , mChances()
   , mStreamNumber(0)
   , mFrameNumber(0)
   , mCallSensorTrackObservers(aSrc.mCallSensorTrackObservers)
{
}

// ================================================================================================
// virtual
WsfEOIR_Sensor::~WsfEOIR_Sensor()
{
   ClearSensingChances();
}

// ================================================================================================
// virtual
WsfSensor* WsfEOIR_Sensor::Clone() const
{
   return new WsfEOIR_Sensor(*this);
}

// ================================================================================================
// virtual
bool WsfEOIR_Sensor::Initialize(double aSimTime)
{
   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mEOIR_ModeList);
   return ok;
}

// ================================================================================================
// virtual
void WsfEOIR_Sensor::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   WsfSensor::PlatformAdded(aSimTime, aPlatformPtr);

   if (IsTurnedOn() && AllowDetectionChancesFor(aPlatformPtr))
   {
      mChances.emplace(aPlatformPtr->GetIndex(), EOIR_Chance(aPlatformPtr));
   }
}

// ================================================================================================
// virtual
void WsfEOIR_Sensor::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   WsfSensor::PlatformDeleted(aPlatformPtr);

   if (IsTurnedOn())
   {
      auto chanceIter = mChances.find(aPlatformPtr->GetIndex());
      if ((chanceIter != mChances.end()) && chanceIter->second.mNowDetecting)
      {
         ProcessSensorDetectionChanged(GetSimulation()->GetSimTime(), chanceIter->second, WsfSensorResult::cTARGET_DELETED);
         mChances.erase(chanceIter);
      }
   }
}

// ================================================================================================
// virtual
bool WsfEOIR_Sensor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "call_sensor_track_observers")
   {
      aInput.ReadValue(mCallSensorTrackObservers);
   }
   else
   {
      myCommand = WsfSensor::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
// virtual
void WsfEOIR_Sensor::TurnOff(double aSimTime)
{
   WsfSensor::TurnOff(aSimTime);
   for (const auto& chance : mChances)
   {
      if (chance.second.mNowDetecting)
      {
         ProcessSensorDetectionChanged(aSimTime, chance.second, WsfSensorResult::cDETECTION_STOP);
      }
   }
   ClearSensingChances();
}

// ================================================================================================
// virtual
void WsfEOIR_Sensor::TurnOn(double aSimTime)
{
   WsfSensor::TurnOn(aSimTime); // Must be called prior to constructing the chance list
   ++mStreamNumber;
   mFrameNumber = 0;

   // Construct the sensing chance list for the active platforms.
   ClearSensingChances();
   size_t platformCount = GetSimulation()->GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* platformPtr = GetSimulation()->GetPlatformEntry(platformEntry);
      if (platformPtr != GetPlatform())
      {
         PlatformAdded(aSimTime, platformPtr);
      }
   }
}

// ================================================================================================
// virtual
void WsfEOIR_Sensor::Update(double aSimTime)
{
   // Exit quickly if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime > (aSimTime + 1.0E-5))
   {
      return;
   }
   mNextUpdateTime += GetUpdateInterval();

   WsfSensor::Update(aSimTime);

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Sensor update.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetName();
   }

   // Tell the observers that the 'scan' is being updated. At a minimum this will force
   // the DIS interface to publish a PDU as to where the sensor is pointed.

   WsfObserver::SensorScanUpdated(GetSimulation())(aSimTime, this);

   WsfSensorResult result;
   Settings        settings;
   settings.mModeIndex  = mModeListPtr->GetCurrentMode();
   result.mModeIndex    = settings.mModeIndex;
   EOIR_Mode* modePtr   = mEOIR_ModeList[settings.mModeIndex];
   settings.mRequiredPd = WsfSensor::GetRequiredPd(modePtr);

   // The sensor reporting origin and the reported locations must account for difference between
   // where the sensing platform *THINKS* it is located and where it is actually located.
   double platformLocationErrorWCS[3];
   GetPlatform()->GetLocationErrorWCS(platformLocationErrorWCS);

   WsfImage image(this);
   image.SetMessageDataTag(GetSimulation()->CreateMessageDataTag());
   image.SetMessageLength(mMessageLength);
   image.SetFrameTime(modePtr->GetFrameTime());
   image.SetImageTime(aSimTime);
   ++mFrameNumber;
   image.SetImageNumber(mFrameNumber);
   image.SetStreamNumber(mStreamNumber);
   double snrLocWCS[3];
   GetPlatform()->GetLocationWCS(snrLocWCS);
   double originLocWCS[3];
   UtVec3d::Add(originLocWCS, snrLocWCS, platformLocationErrorWCS);
   image.SetOriginatorLocationWCS(originLocWCS);

   // Determine the approximate location of the center of the image.
   //
   // If the user cued to a location then we use that. Otherwise we take the pointing vector and determine
   // approximately where it would hit a round smooth earth.

   double centerLocWCS[3] = {0.0, 0.0, 0.0};
   if (GetCueType() == cCUED_TO_LOCATION)
   {
      GetCuedLocationWCS(centerLocWCS);
   }
   else
   {
      // Get the pointing vector and determine approximately if and where it hits the ground.
      double pointingPCS[3] = {100000.0, 0.0, 0.0};
      double pointingWCS[3];
      double pointingNED[3];
      UtEntityPart::ConvertPCSVectorToWCS(pointingWCS, pointingPCS);
      // Convert the WCS vector to an NED vector.
      modePtr->GetAntenna().ConvertWCS_VectorToNED(pointingWCS, pointingNED);
      if (pointingNED[2] > 0.0) // Pointing down
      {
         double lat;
         double lon;
         double alt;
         modePtr->GetAntenna().GetLocationLLA(lat, lon, alt);
         double pointingNE = sqrt(pointingNED[0] * pointingNED[0] + pointingNED[1] * pointingNED[1]);
         if (pointingNE > 1.0)
         {
            // Determine the depression angle of the horizon. (A positive depression angle is down)
            // Continue the solution only if the view vector is below the horizon.

            double earthRadius            = UtSphericalEarth::cEARTH_RADIUS;
            double cosTheta               = earthRadius / (earthRadius + alt);
            double horizonDepressionAngle = acos(cosTheta);
            double depressionAngle        = atan2(pointingNED[2], pointingNE);
            if (depressionAngle > horizonDepressionAngle)
            {
               // Use law of cosines and quadratic equation to get the slant range.
               double a       = 1.0;
               double b       = -2.0 * (earthRadius + alt) * cos(UtMath::cPI_OVER_2 - depressionAngle);
               double c       = (2.0 * earthRadius * alt) + (alt * alt);
               double descrim = (b * b) - (4.0 * a * c);
               if (descrim > 0.0)
               {
                  double s  = sqrt(descrim);
                  double r1 = (-b + s) / (2.0 * a);
                  double r2 = (-b - s) / (2.0 * a);
                  // Choose the smallest positive value.
                  double r = r2;
                  if ((r1 > 0.0) && (r2 > 0.0))
                  {
                     r = std::min(r1, r2);
                  }
                  else if (r1 > 0.0)
                  {
                     r = r1;
                  }
                  UtVec3d::Normalize(pointingWCS);
                  UtVec3d::Multiply(pointingWCS, r);
                  UtVec3d::Add(centerLocWCS, snrLocWCS, pointingWCS);
               }
            }
         }
         else
         {
            // Pointing straight-down
            alt = GetPlatform()->GetTerrainHeight();
            UtEntity::ConvertLLAToWCS(lat, lon, alt, centerLocWCS);
         }
      }
   }

   // Compute the approximate width, height and resolution at the range of interest.

   image.SetCenterLocationWCS(centerLocWCS);
   image.SetWidth(modePtr->mPixelCountWidth);
   image.SetHeight(modePtr->mPixelCountHeight);
   if (!UtVec3d::Equals(centerLocWCS, 0.0))
   {
      double relLocWCS[3];
      UtVec3d::Subtract(relLocWCS, centerLocWCS, snrLocWCS);
      double range = UtVec3d::Magnitude(relLocWCS);
      double minAz;
      double maxAz;
      double minEl;
      double maxEl;
      modePtr->GetAntenna().GetAzimuthFieldOfView(minAz, maxAz);
      modePtr->GetAntenna().GetElevationFieldOfView(minEl, maxEl);
      if ((modePtr->mPixelCountWidth > 0) && (modePtr->mPixelCountHeight > 0))
      {
         image.SetWidthResolution(((maxAz - minAz) * range) / modePtr->mPixelCountWidth);
         image.SetHeightResolution(((maxEl - minEl) * range) / modePtr->mPixelCountHeight);
      }
      else if (modePtr->mAngularResolution > 0.0)
      {
         image.SetWidth((maxAz - minAz) / modePtr->mAngularResolution);
         image.SetHeight((maxEl - minEl) / modePtr->mAngularResolution);
         image.SetWidthResolution(range * modePtr->mAngularResolution);
         image.SetHeightResolution(range * modePtr->mAngularResolution);
      }

      // Set result range data for proper error calculations in case they are percentage of true range
      result.mRcvrToTgt.mRange = range;
   }

   double noiseLevel = modePtr->GetReceiver().GetNoisePower();
   image.SetNoiseLevel(noiseLevel);

   // Propagate the 'track quality' from the mode.
   image.SetTrackQuality(modePtr->GetTrackQuality());

   WsfTrackId requestId;
   size_t     targetIndex = 0;
   double     nextUpdateTime;

   // Loop over all platforms that are eligible to be detected and create image objects for each one detected.
   while (mSchedulerPtr->SelectTarget(aSimTime, nextUpdateTime, targetIndex, requestId, settings))
   {
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(targetIndex);
      if (targetPtr != nullptr)
      {
         auto chanceIter = mChances.find(targetIndex);
         if (chanceIter != mChances.end())
         {
            EOIR_Chance& chance   = chanceIter->second;
            bool         detected = false;

            if (modePtr->WithinDetectionRange(aSimTime, targetPtr))
            {
               // The following call dispatches to the base class' AttemptToDetect,
               // which calls the mode AttemptToDetect.  This mode AttemptToDetect
               // contains logic for the proper EM band (visual, IR).
               detected = AttemptToDetect(aSimTime, targetPtr, settings, result);
            }

            chance.mVisibleInImage = detected;
            if (detected)
            {
               modePtr->GetErrorModel()->ApplyMeasurementErrors(result);
               image.SetRangeError(result.mMeasurement.GetRangeError());
               image.SetBearingError(result.mMeasurement.GetSensorAzimuthError());
               image.SetElevationError(result.mMeasurement.GetSensorElevationError());
               image.SetRangeRateError(result.mMeasurement.GetRangeRateError());

               auto tgtLocWCS = result.mMeasurement.GetLocationWCS();
               UtVec3d::Set(chance.mLocationWCS, tgtLocWCS.GetData());

               // Create an object in the image.
               WsfImage::Object object(targetPtr);
               object.SetLocationWCS(tgtLocWCS.GetData());
               object.SetSignalLevel(result.mSignalToNoise * image.GetNoiseLevel());
               object.SetPixelCount(result.mPixelCount);
               object.SetPixelIntensity(result.mPd);
               image.AddObject(object);
            }
            NotifyObservers(aSimTime, modePtr, &chance, result);
         }
      }
   }

   // Notify observers of image creation
   WsfObserver::ImageCreated(GetSimulation())(aSimTime, this, &image);

   // Send the message to all attached processors.
   WsfVideoMessage message(GetPlatform(), image);
   SetMessageParameters(GetCurrentModeName(), message);
   SendMessage(aSimTime, message);
}

// ================================================================================================
// virtual
size_t WsfEOIR_Sensor::GetEM_RcvrCount() const
{
   // Returns 0 prior to initialization
   return mEOIR_ModeList.empty() ? 0U : 1U;
}

// ================================================================================================
// virtual
WsfEM_Rcvr& WsfEOIR_Sensor::GetEM_Rcvr(size_t aIndex) const
{
   return mEOIR_ModeList[mModeListPtr->GetCurrentMode()]->GetReceiver();
}

// ================================================================================================
//! Clear the sensing chance list.
// private
void WsfEOIR_Sensor::ClearSensingChances()
{
   mChances.clear();
}

// ================================================================================================
//! Notify observers of detection results.
//! This method simply notifies observers using the SensorDetectionChanged and SensorTrackXXX
//! events of the detection status against a particular object. It does NOT send out a WsfTrackMessage
//! as the product of this sensor type is an image, not a track. It is left to the image processor
//! to form tracks.
// private
void WsfEOIR_Sensor::NotifyObservers(double aSimTime, EOIR_Mode* aModePtr, EOIR_Chance* aChancePtr, WsfSensorResult& aResult)
{
   if (aChancePtr->mVisibleInImage)
   {
      if (!aChancePtr->mNowDetecting)
      {
         aChancePtr->mNowDetecting = true;
         WsfObserver::SensorDetectionChanged(GetSimulation())(aSimTime, this, aChancePtr->mTargetIndex, aResult);
      }

      // Produce pseudo-tracks for VESPA visualization
      if (mCallSensorTrackObservers)
      {
         WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(aChancePtr->mTargetIndex);
         if (targetPtr != nullptr)
         {
            if (aChancePtr->mTrackPtr == nullptr)
            {
               aModePtr->InitializeTrack(aSimTime, aChancePtr->mTrackPtr);
               // Mark as a pseudo-sensor track so observers can ignore or process as desired.
               aChancePtr->mTrackPtr->SetTrackType(WsfTrack::cPSEUDO_SENSOR);
               aModePtr->UpdateTrack(aSimTime, aChancePtr->mTrackPtr, targetPtr, aResult);
               WsfObserver::SensorTrackInitiated(GetSimulation())(aSimTime, this, aChancePtr->mTrackPtr);
            }
            else
            {
               aModePtr->UpdateTrack(aSimTime, aChancePtr->mTrackPtr, targetPtr, aResult);
               WsfObserver::SensorTrackUpdated(GetSimulation())(aSimTime, this, aChancePtr->mTrackPtr);
            }
         }
      }
   }
   else
   {
      // Target not detected
      if (aChancePtr->mNowDetecting)
      {
         aChancePtr->mNowDetecting = false;
         WsfObserver::SensorDetectionChanged(GetSimulation())(aSimTime, this, aChancePtr->mTargetIndex, aResult);
      }

      // Cleanup pseudo-tracks for VESPA visualization
      if (mCallSensorTrackObservers)
      {
         if (aChancePtr->mTrackPtr != nullptr)
         {
            WsfObserver::SensorTrackDropped(GetSimulation())(aSimTime, this, aChancePtr->mTrackPtr);
            delete aChancePtr->mTrackPtr;
            aChancePtr->mTrackPtr = nullptr;
         }
      }
   }
}

// =================================================================================================
//! Provide a minimal set of data in a SensorDetectionChanged event in the absence of
//! an active WsfSensor::Result.
//! @param aSimTime The current simulation time.
//! @param aChance  The sensing chance used to populate data provided in the event.
//! @param aStatus  The WsfSensorResult status flag (reason for detection change).
void WsfEOIR_Sensor::ProcessSensorDetectionChanged(double aSimTime, const EOIR_Chance& aChance, unsigned int aStatus)
{
   // Set up current interaction geometry and call SensorDetectionChanged.
   WsfSensorResult result;
   result.mCheckedStatus = aStatus;
   result.mFailedStatus  = aStatus;
   result.BeginOneWayInteraction(&(mEOIR_ModeList[aChance.mModeIndex]->GetReceiver()),
                                 GetSimulation()->GetPlatformByIndex(aChance.mTargetIndex));
   WsfObserver::SensorDetectionChanged(GetSimulation())(aSimTime, this, aChance.mTargetIndex, result);
}

// ================================================================================================
//                         Nested class WsfEOIR_Sensor::EOIR_Chance.
// ================================================================================================

// ================================================================================================
WsfEOIR_Sensor::EOIR_Chance::EOIR_Chance(WsfPlatform* aTargetPtr)
   : mTrackPtr(nullptr)
   , mLocationWCS{0.0, 0.0, 0.0}
   , mTargetIndex(aTargetPtr->GetIndex())
   , mVisibleInImage(true)
   , mNowDetecting(false)
{
}

// ================================================================================================
WsfEOIR_Sensor::EOIR_Chance::~EOIR_Chance()
{
   delete mTrackPtr;
}


// ================================================================================================
//                            Nested class WsfEOIR_Sensor::EOIR_Mode.
// ================================================================================================

// ================================================================================================
WsfEOIR_Sensor::EOIR_Mode::EOIR_Mode(WsfScenario& aScenario)
   : WsfEOIR_IRST_SensorMode(aScenario)
   , mPixelCountWidth(0)
   , mPixelCountHeight(0)
   , mAngularResolution(0.0)
   , mOmegaResolution(0.0)
{
   SetErrorModel(ut::make_unique<EOIR_ErrorModel>());
   SetVisualBandDefault(true); // 'band visual' is the default
}

// ================================================================================================
// virtual
WsfMode* WsfEOIR_Sensor::EOIR_Mode::Clone() const
{
   return new EOIR_Mode(*this);
}

// ================================================================================================
// virtual
bool WsfEOIR_Sensor::EOIR_Mode::Initialize(double aSimTime)
{
   bool ok = WsfEOIR_IRST_SensorMode::Initialize(aSimTime);

   if (GetFrameTime() <= 0.0)
   {
      ut::log::error() << "'frame_time' not specified.";
      ok = false;
   }
   if ((mAngularResolution == 0.0) && (mPixelCountWidth == 0))
   {
      ut::log::error() << "Either 'angular_resolution' or 'pixel_count' must be specified.";
      ok = false;
   }
   else if ((mAngularResolution) > 0 && (mOmegaResolution <= 0.0))
   {
      ut::log::error() << "'angular_resolution' too small. Please increase.";
      ok = false;
   }
   return ok;
}

// ================================================================================================
// virtual
bool WsfEOIR_Sensor::EOIR_Mode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "angular_resolution")
   {
      // replaces pixel_count
      mPixelCountWidth = mPixelCountHeight = 0;
      aInput.ReadValueOfType(mAngularResolution, UtInput::cANGLE);
      aInput.ValueGreater(mAngularResolution, 0.0);
      mOmegaResolution = UtMath::cTWO_PI * (1.0 - cos(0.5 * mAngularResolution));
   }
   else if (command == "pixel_count")
   {
      // replaces angular_resolution
      mAngularResolution = mOmegaResolution = 0.0;
      aInput.ReadValue(mPixelCountWidth);
      aInput.ValueGreater(mPixelCountWidth, 1);
      aInput.ReadValue(mPixelCountHeight);
      aInput.ValueGreater(mPixelCountHeight, 1);
   }
   else
   {
      myCommand = WsfEOIR_IRST_SensorMode::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
bool WsfEOIR_Sensor::EOIR_Mode::AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult)
{
   // Perform the detection attempt if not concealed and within range/angle limits
   if (BeginDetectionAttempt(aSimTime, aTargetPtr, aSettings, aResult))
   {
      // Determine the presented area.
      aResult.ComputeOpticalSigAzEl();
      aResult.mOpticalSig =
         WsfOpticalSignature::GetValue(aSimTime, aTargetPtr, aResult.mOpticalSigAz, aResult.mOpticalSigEl);

      // There are different calculations being done depending on if the user specified
      // an angular resolution or a pixel count.
      if (mAngularResolution != 0)
      {
         // Compute the solid angle subtended

         double range = aResult.mRcvrToTgt.mRange;
         double omega = aResult.mOpticalSig / std::max(range * range, 1.0);

         // Compute the approximate number of pixels occupied.  We first convert the
         // two dimensional angular resolution into a three dimensional solid angle.

         aResult.mPixelCount = omega / mOmegaResolution;
      }
      else
      {
         assert(mPixelCountWidth != 0);

         // Assume the target's signature is a square
         double targetLength        = sqrt(aResult.mOpticalSig);
         double range               = aResult.mRcvrToTgt.mRange;
         double targetAngularExtent = targetLength / range;

         double minAz;
         double maxAz;
         mAntenna.GetAzimuthFieldOfView(minAz, maxAz);
         double widthInPixels = mPixelCountWidth * targetAngularExtent / (maxAz - minAz);

         double minEl;
         double maxEl;
         mAntenna.GetElevationFieldOfView(minEl, maxEl);
         double heightInPixels = mPixelCountHeight * targetAngularExtent / (maxEl - minEl);

         aResult.mPixelCount = widthInPixels * heightInPixels;
      }

      if (mUseSimpleVisualDetector)
      {
         AttemptToDetectVisual(aSimTime, aTargetPtr, aSettings, aResult);
      }
      else
      {
         AttemptToDetectInfrared(aSimTime, aTargetPtr, aSettings, aResult);
      }
   }

   return EndDetectionAttempt(aSimTime, aTargetPtr, aSettings, aResult);
}

// ================================================================================================
// virtual
void WsfEOIR_Sensor::EOIR_Mode::Deselect(double aSimTime)
{
   mRcvr.Deactivate();
}

// ================================================================================================
// virtual
void WsfEOIR_Sensor::EOIR_Mode::Select(double aSimTime)
{
   // The new update interval does not take place until the next update.
   GetSensor()->SetUpdateInterval(GetFrameTime());
   mRcvr.Activate();
}

WsfEOIR_Sensor::EOIR_SensorScheduler::EOIR_SensorScheduler()
   : WsfSensorScheduler()
   , mTargetNumber(0)
{
}

// override
bool WsfEOIR_Sensor::EOIR_SensorScheduler::SelectTarget(double               aSimTime,
                                                        double&              aNextSimTime,
                                                        size_t&              aTargetIndex,
                                                        WsfTrackId&          aRequestId,
                                                        WsfSensor::Settings& aSettings)
{
   bool   haveTarget    = true;
   size_t platformCount = mSensorPtr->GetSimulation()->GetPlatformCount();
   if (mTargetNumber >= platformCount)
   {
      aNextSimTime  = aSimTime + mSensorPtr->GetUpdateInterval();
      mTargetNumber = 0;
      haveTarget    = false;
   }
   else
   {
      aNextSimTime             = aSimTime;
      WsfPlatform* platformPtr = mSensorPtr->GetSimulation()->GetPlatformEntry(mTargetNumber);
      if ((platformPtr != nullptr) && (mSensorPtr->GetPlatform() != platformPtr))
      {
         aTargetIndex = platformPtr->GetIndex();
         ++mTargetNumber;
      }
      else
      {
         // mTargetNumber is our platform index, or platform is invalid; select another.
         ++mTargetNumber;
         haveTarget = SelectTarget(aSimTime, aNextSimTime, aTargetIndex, aRequestId, aSettings);
      }
   }
   return haveTarget;
}

WsfSensorScheduler* WsfEOIR_Sensor::EOIR_SensorScheduler::Clone() const
{
   return new EOIR_SensorScheduler(*this);
}

// ================================================================================================
// override
void WsfEOIR_Sensor::EOIR_ErrorModel::ApplyMeasurementErrors(const wsf::SphericalMeasurementErrors& aErrors,
                                                             WsfSensorResult&                       aResult)
{
   // Apply the measurement errors to relative position and convert back to absolute
   // Use the true Az/El to avoid propagating wave bending effects to the position calculation.
   double tgtLocWCS[3];
   double rcvrToTgtLocWCS[3];
   auto   eoirModePtr = static_cast<WsfEOIR_Sensor::EOIR_Mode*>(GetSensorMode());

   eoirModePtr->GetAntenna().GetRelativeLocationWCS(aResult.mRcvrToTgt.mTrueAz + aErrors.mAzError,
                                                    aResult.mRcvrToTgt.mTrueEl + aErrors.mElError,
                                                    aResult.mRcvrToTgt.mRange + aErrors.mRangeError,
                                                    rcvrToTgtLocWCS);
   eoirModePtr->GetAntenna().GetLocationWCS(rcvrToTgtLocWCS, tgtLocWCS);

   // The location was computed using the truth location of the sensing platform.
   // Adjust the location to account for the perceived location of the platform.
   double platformLocationErrorWCS[3];
   eoirModePtr->GetSensor()->GetPlatform()->GetLocationErrorWCS(platformLocationErrorWCS);
   UtVec3d::Add(tgtLocWCS, tgtLocWCS, platformLocationErrorWCS);
   aResult.mMeasurement.SetLocationWCS(tgtLocWCS);

   // Set the sigmas in the measurement error fields.
   aResult.mMeasurement.SetSensorAzimuthError(GetSigmas().mAzErrorStdDev);
   aResult.mMeasurement.SetSensorElevationError(GetSigmas().mElErrorStdDev);
   aResult.mMeasurement.SetRangeError(GetSigmas().mRangeErrorStdDev);
   aResult.mMeasurement.SetRangeRateError(GetSigmas().mRangeRateErrorStdDev);
}
