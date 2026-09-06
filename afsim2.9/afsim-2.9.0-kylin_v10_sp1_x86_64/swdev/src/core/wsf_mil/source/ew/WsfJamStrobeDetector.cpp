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

#include "WsfJamStrobeDetector.hpp"

#include <algorithm>
#include <iostream>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfFalseTargetScreener.hpp"
#include "WsfIFF_Manager.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"

const double WsfJamStrobeDetector::cSDURN = 1.0 / sqrt(3.0);

WsfJamStrobeDetector::WsfJamStrobeDetector()
   : mPerceiveJammingFirst(true)
   , mContJNR_PerceptionThreshold(UtMath::DB_ToLinear(380.0))
   , mPulsedJNR_PerceptionThreshold(UtMath::DB_ToLinear(380.0))
   , mCoherentJNR_PerceptionThreshold(UtMath::DB_ToLinear(380.0))
   , mReportsBearing(false)
   , mReportsElevation(false)
   , mTrackQuality(1.0)
   , mAzErrorStdDev(0.0)
   , mElErrorStdDev(0.0)
   , mGaussianAzError(true)
   , mGaussianElError(true)
   , mSensorModePtr(nullptr)
{
}

WsfJamStrobeDetector::WsfJamStrobeDetector(const WsfJamStrobeDetector& aSrc)
   : mPerceiveJammingFirst(aSrc.mPerceiveJammingFirst)
   , mContJNR_PerceptionThreshold(aSrc.mContJNR_PerceptionThreshold)
   , mPulsedJNR_PerceptionThreshold(aSrc.mPulsedJNR_PerceptionThreshold)
   , mCoherentJNR_PerceptionThreshold(aSrc.mCoherentJNR_PerceptionThreshold)
   , mReportsBearing(aSrc.mReportsBearing)
   , mReportsElevation(aSrc.mReportsElevation)
   , mTrackQuality(1.0)
   , mAzErrorStdDev(aSrc.mAzErrorStdDev)
   , mElErrorStdDev(aSrc.mElErrorStdDev)
   , mGaussianAzError(aSrc.mGaussianAzError)
   , mGaussianElError(aSrc.mGaussianElError)
   , mSensorModePtr(nullptr)
{
}

bool WsfJamStrobeDetector::Initialize(double aSimTime, WsfSensorMode* aSensorModePtr)
{
   // Assure that the jam strobe detector actually reports something (bearing or elevation)
   if (!mReportsBearing && !mReportsElevation)
   {
      ut::log::error() << "Jam strobe detector does not report anything.";
      return false;
   }

   mSensorModePtr = aSensorModePtr;
   return true;
}

double WsfJamStrobeDetector::ReadMeasurementError(UtInput& aInput)
{
   double      value;
   std::string units;
   aInput.ReadValue(value);
   aInput.ReadValue(units);
   if (units == "fraction_of_beamwidth")
   {
      aInput.ValueInClosedRange(value, 0.0, 1.0);
      value *= -1.0;
   }
   else
   {
      value = UtInput::ConvertValueFrom(value, units, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(value, 0.0);
   }

   return value;
}

bool WsfJamStrobeDetector::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if ((command == "jam_strobe_detector") || (command == "jam_strobe_indicator"))
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         if (command == "gaussian_azimuth_error_sigma")
         {
            mAzErrorStdDev   = ReadMeasurementError(aInput);
            mGaussianAzError = true;
         }
         else if (command == "gaussian_elevation_error_sigma")
         {
            mElErrorStdDev   = ReadMeasurementError(aInput);
            mGaussianElError = true;
         }
         else if (command == "uniform_azimuth_error_sigma")
         {
            mAzErrorStdDev   = ReadMeasurementError(aInput);
            mGaussianAzError = false;
         }
         else if (command == "uniform_elevation_error_sigma")
         {
            mElErrorStdDev   = ReadMeasurementError(aInput);
            mGaussianElError = false;
         }
         else if (command == "uniform_azimuth_error_bound")
         {
            mAzErrorStdDev   = ReadMeasurementError(aInput) * cSDURN;
            mGaussianAzError = false;
         }
         else if (command == "uniform_elevation_error_bound")
         {
            mElErrorStdDev   = ReadMeasurementError(aInput) * cSDURN;
            mGaussianElError = false;
         }
         else if (command == "reports_nothing")
         {
            mReportsBearing   = false;
            mReportsElevation = false;
         }
         else if (command == "reports_bearing")
         {
            mReportsBearing = true;
         }
         else if (command == "reports_elevation")
         {
            mReportsElevation = true;
         }
         else if (command == "track_quality")
         {
            aInput.ReadValue(mTrackQuality);
            aInput.ValueInClosedRange(mTrackQuality, 0.0, 1.0);
         }
         else if (command == "jamming_perception_threshold")
         {
            double jnrPerceptionThreshold;
            aInput.ReadValueOfType(jnrPerceptionThreshold, UtInput::cRATIO);
            mPulsedJNR_PerceptionThreshold = mContJNR_PerceptionThreshold = mCoherentJNR_PerceptionThreshold =
               jnrPerceptionThreshold;
         }
         else if (command == "continuous_jamming_perception_threshold")
         {
            aInput.ReadValueOfType(mContJNR_PerceptionThreshold, UtInput::cRATIO);
         }
         else if (command == "pulsed_jamming_perception_threshold")
         {
            aInput.ReadValueOfType(mPulsedJNR_PerceptionThreshold, UtInput::cRATIO);
         }
         else if (command == "coherent_jamming_perception_threshold")
         {
            aInput.ReadValueOfType(mCoherentJNR_PerceptionThreshold, UtInput::cRATIO);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// private
void WsfJamStrobeDetector::ApplyMeasurementErrors(WsfSensorResult& aResult)
{
   double azError(0.0);
   double elError(0.0);

   // Compute the standard deviation of the range error.
   WsfMeasurement& measurement = aResult.mMeasurement;
   measurement.SetUpdateTime(GetSimulation()->GetSimTime());

   ut::Random& random = GetSimulation()->GetRandom();

   measurement.SetSensorAzimuthError(0.0);
   if (mReportsBearing)
   {
      double sigma = mAzErrorStdDev;
      if (sigma < 0.0) // error sigma is a fraction of the beamwidth
      {
         sigma *= -aResult.GetReceiver()->GetAzimuthBeamwidth(0.0, 0.0);
      }
      measurement.SetSensorAzimuthError(sigma);
      if (sigma != 0.0)
      {
         azError = mGaussianAzError ? random.Gaussian() * sigma : random.Uniform(-sigma / cSDURN, sigma / cSDURN);
      }
   }

   measurement.SetSensorElevationError(0.0);
   if (mReportsElevation)
   {
      double sigma = mElErrorStdDev;
      if (sigma < 0.0) // error sigma is a fraction of the beamwidth
      {
         sigma *= -aResult.GetReceiver()->GetElevationBeamwidth(0.0, 0.0);
      }
      measurement.SetSensorElevationError(sigma);
      if (sigma != 0.0)
      {
         elError = mGaussianElError ? random.Gaussian() * sigma : random.Uniform(-sigma / cSDURN, sigma / cSDURN);
      }
   }

   // Apply the error
   // Use the true Az/El to avoid propagating wave bending effects to the position calculation.

   double az = 0.0;
   double el = 0.0;
   if (aResult.mRcvrToXmtr.mRange >= 0.0)
   {
      az = UtMath::NormalizeAngleMinusPi_Pi(aResult.mRcvrToXmtr.mTrueAz + azError);
      el = UtMath::Limit(aResult.mRcvrToXmtr.mTrueEl + elError, 0.9999 * UtMath::cPI_OVER_2);
   }
   measurement.SetSensorAzimuth(az);
   measurement.SetSensorAzimuthValid(true);
   measurement.SetSensorElevation(el);
   measurement.SetSensorElevationValid(true);
}

void WsfJamStrobeDetector::InitializeTrack(double aSimTime, WsfTrack*& aTrackPtr)
{
   if (aTrackPtr == nullptr)
   {
      aTrackPtr = new WsfTrack(WsfTrack::cUNFILTERED_SENSOR, mSensorModePtr->GetSensor(), mSensorModePtr->GetPlatform());
   }

   aTrackPtr->SetIsStrobe(true);
   aTrackPtr->Initialize(aSimTime, mSensorModePtr->GetPlatform()->GetNextTrackId(), *GetSimulation());
   aTrackPtr->SetMessageDataTag(GetSimulation()->CreateMessageDataTag());

   // Initialize the sensor mode used to produce the track.
   aTrackPtr->SetSensorModeId(mSensorModePtr->GetNameId());

   // NOTE: The following code to initialize the reporting flags in the track probably isn't necessary
   // anymore because UpdateTrack() always calls UpdateTrackReportingFlags() which can update the flags
   // based on operational conditions. It is being maintained JUST IN CASE somebody was expecting these
   // flags to be set upon exit from this method.

   aTrackPtr->SetBearingValid(mReportsBearing);
   aTrackPtr->SetElevationValid(mReportsElevation);

   // Set the baseline track quality.
   aTrackPtr->SetTrackQuality(mTrackQuality);
}

void WsfJamStrobeDetector::UpdateTrackReportingFlags(double aSimTime, WsfTrack*& aTrackPtr)
{
   aTrackPtr->SetBearingValid(mReportsBearing);
   aTrackPtr->SetElevationValid(mReportsElevation);
}

void WsfJamStrobeDetector::UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   // Update the track reporting flags (i.e.: <xxx>IsValid). This must be done before any part of the
   // track is updated as the flags are used to determine what data needs to be propagated from the
   // results of the detection attempt into the track.
   UpdateTrackReportingFlags(aSimTime, aTrackPtr);

   aTrackPtr->Update(aSimTime);
   aTrackPtr->SetMessageDataTag(GetSimulation()->CreateMessageDataTag());

   // Report the name of the mode used for this update.
   aTrackPtr->SetSensorModeId(mSensorModePtr->GetNameId());

   // Define the origin of the report as the location of the receiver, as the reported range and angles are
   // relative to that point. Note that all sensors *SHOULD* have a receiver as it is necessary to carry on
   // the interaction, but for some reason it doesn't then the platform location will be used.

   double snsrLocationWCS[3];
   UtVec3d::Set(snsrLocationWCS, aResult.mRcvrLoc.mLocWCS);
   if (!aResult.mRcvrLoc.mIsValid)
   {
      if ((aResult.GetReceiver() != nullptr) && (aResult.GetReceiver()->GetAntenna() != nullptr))
      {
         aResult.GetReceiver()->GetAntenna()->GetLocationWCS(snsrLocationWCS);
      }
      else
      {
         mSensorModePtr->GetPlatform()->GetLocationWCS(snsrLocationWCS);
      }
   }
   double platformLocationErrorWCS[3];
   mSensorModePtr->GetPlatform()->GetLocationErrorWCS(platformLocationErrorWCS);
   UtVec3d::Add(snsrLocationWCS, snsrLocationWCS, platformLocationErrorWCS);
   aTrackPtr->SetOriginatorLocationWCS(snsrLocationWCS);

   WsfMeasurement& measurement = aResult.mMeasurement;

   // Update the various fields...
   if (aTrackPtr->BearingValid() || aTrackPtr->ElevationValid())
   {
      double heading;
      double pitch;
      double roll;
      aResult.GetReceiver()->GetPlatform()->GetOrientationNED(heading, pitch, roll);
      double bearing   = UtMath::NormalizeAngleMinusPi_Pi(measurement.GetSensorAzimuth() + heading);
      double elevation = UtMath::NormalizeAngleMinusPi_Pi(measurement.GetSensorElevation() + pitch);

      if (aTrackPtr->BearingValid())
      {
         aTrackPtr->SetBearing(bearing);
      }
      if (aTrackPtr->ElevationValid())
      {
         aTrackPtr->SetElevation(elevation);
      }

      aTrackPtr->Set3D(false);
   }

   // Include the position errors with the track.
   aTrackPtr->SetBearingError(measurement.GetSensorAzimuthError());
   aTrackPtr->SetElevationError(measurement.GetSensorElevationError());

   // If the spatial domain hasn't been defined then simply copy the spatial domain of the target.
   if (aTrackPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_UNKNOWN)
   {
      aTrackPtr->SetSpatialDomain(aTargetPtr->GetSpatialDomain());
   }

   // TODO (MRG): reports jammer power or JNR?

   // This is not really 'track' information but is carried along for
   // non-simulation purposes or if somebody really needs to 'cheat'.
   aTrackPtr->SetTargetIndex(aTargetPtr->GetIndex());
   aTrackPtr->SetTargetName(aTargetPtr->GetNameId());
   aTrackPtr->SetTargetType(aTargetPtr->GetTypeId());

   // if (mSensorModePtr->GetScreener() != 0)
   //{
   //    aTrackPtr->IsReportable(mSensorModePtr->GetScreener()->AllowTrackReporting(aTrackPtr->GetTargetIndex(), aTrackPtr));
   // }

   // WSF did not have this because the tracker that had special logic for the jam strobes.
   // WSF generalized the tracker which use the quality from the sensor mode. We need to set it back here.
   aTrackPtr->SetTrackQuality(mTrackQuality);
}

WsfSimulation* WsfJamStrobeDetector::GetSimulation() const
{
   return (mSensorModePtr != nullptr) ? mSensorModePtr->GetSimulation() : nullptr;
}
