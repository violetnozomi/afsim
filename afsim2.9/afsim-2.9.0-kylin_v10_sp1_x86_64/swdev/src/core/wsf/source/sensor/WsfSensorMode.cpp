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

#include "WsfSensorMode.hpp"

#include <cassert>
#include <limits>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtRandom.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfIFF_Manager.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorErrorModel.hpp"
#include "WsfSensorErrorModelTypes.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSensorScheduler.hpp"
#include "WsfSimulation.hpp"
#include "WsfStandardSensorErrorModel.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfUtil.hpp"

// =================================================================================================
WsfSensorMode::WsfSensorMode()
   : mSensorPtr(nullptr)
   , mPlatformPtr(nullptr)
   , mMaximumRange(std::numeric_limits<double>::max())
   , mAzCueRate(1.01 * WsfArticulatedPart::cINFINITE_SLEW_RATE)
   , // Will get limited by the sensor rate
   mElCueRate(1.01 * WsfArticulatedPart::cINFINITE_SLEW_RATE)
   , // Will get limited by the sensor rate
   mMinAzCue(-UtMath::cPI)
   , // Will be limited by the sensor slew limits
   mMaxAzCue(UtMath::cPI)
   , // Will be limited by the sensor slew limits
   mMinElCue(-UtMath::cPI_OVER_2)
   , // Will be limited by the sensor slew limits
   mMaxElCue(UtMath::cPI_OVER_2)
   , // Will be limited by the sensor slew limits
   mCueMode(WsfSensor::cSLEW_AZ_EL)
   , // Will be limited by the sensor slew mode
   mMaximumRequestCount(0)
   , mCanSearchWhileTrack(false)
   , mDisablesSearch(false)
   , mFrameTime(0.0)
   , mRevisitTime(0.0)
   , mDwellTime(0.0)
   , mRequiredPd(0.5)
   , mEstablishTrackProbability(1.0)
   , mMaintainTrackProbability(1.0)
   , mHitsToEstablishTrack(1)
   , mEstablishTrackWindow(1)
   , mEstablishTrackMask(1)
   , mHitsToMaintainTrack(1)
   , mMaintainTrackWindow(1)
   , mMaintainTrackMask(1)
   , mTrackQuality(0.5)
   , mAzErrorStdDev(0.0)
   , mElErrorStdDev(0.0)
   , mRangeErrorStdDev(0.0)
   , mRangeRateErrorStdDev(0.0)
   , mErrorModelPtr(new wsf::StandardSensorErrorModel())
   , mOnSuccessModeName(nullptr)
   , mOnSuccessModeIndex(0)
   , mOnFailureModeName(nullptr)
   , mOnFailureModeIndex(0)
   , mMessageLength(0)
   , mMessagePriority(0)
   , mReportsSpatialDomain(WSF_SPATIAL_DOMAIN_UNKNOWN)
   , mReportingFlags(0)
   , mCapabilitiesFlags(cALL)
{
}

// =================================================================================================
WsfSensorMode::WsfSensorMode(const WsfSensorMode& aSrc)
   : WsfMode(aSrc)
   , mSensorPtr(nullptr)
   , mPlatformPtr(nullptr)
   , mMaximumRange(aSrc.mMaximumRange)
   , mAzCueRate(aSrc.mAzCueRate)
   , mElCueRate(aSrc.mElCueRate)
   , mMinAzCue(aSrc.mMinAzCue)
   , mMaxAzCue(aSrc.mMaxAzCue)
   , mMinElCue(aSrc.mMinElCue)
   , mMaxElCue(aSrc.mMaxElCue)
   , mCueMode(aSrc.mCueMode)
   , mMaximumRequestCount(aSrc.mMaximumRequestCount)
   , mCanSearchWhileTrack(aSrc.mCanSearchWhileTrack)
   , mDisablesSearch(aSrc.mDisablesSearch)
   , mFrameTime(aSrc.mFrameTime)
   , mRevisitTime(aSrc.mRevisitTime)
   , mDwellTime(aSrc.mDwellTime)
   , mRequiredPd(aSrc.mRequiredPd)
   , mEstablishTrackProbability(aSrc.mEstablishTrackProbability)
   , mMaintainTrackProbability(aSrc.mMaintainTrackProbability)
   , mHitsToEstablishTrack(aSrc.mHitsToEstablishTrack)
   , mEstablishTrackWindow(aSrc.mEstablishTrackWindow)
   , mEstablishTrackMask(aSrc.mEstablishTrackMask)
   , mHitsToMaintainTrack(aSrc.mHitsToMaintainTrack)
   , mMaintainTrackWindow(aSrc.mMaintainTrackWindow)
   , mMaintainTrackMask(aSrc.mMaintainTrackMask)
   , mTrackQuality(aSrc.mTrackQuality)
   , mAzErrorStdDev(aSrc.mAzErrorStdDev)
   , mElErrorStdDev(aSrc.mElErrorStdDev)
   , mRangeErrorStdDev(aSrc.mRangeErrorStdDev)
   , mRangeRateErrorStdDev(aSrc.mRangeRateErrorStdDev)
   , mErrorModelPtr(aSrc.mErrorModelPtr)
   , mOnSuccessModeName(aSrc.mOnSuccessModeName)
   , mOnSuccessModeIndex(0)
   , mOnFailureModeName(aSrc.mOnFailureModeName)
   , mOnFailureModeIndex(0)
   , mMessageLength(aSrc.mMessageLength)
   , mMessagePriority(aSrc.mMessagePriority)
   , mReportsSpatialDomain(aSrc.mReportsSpatialDomain)
   , mReportingFlags(aSrc.mReportingFlags)
   , mCapabilitiesFlags(aSrc.mCapabilitiesFlags)
{
}

WsfSensorMode& WsfSensorMode::operator=(const WsfSensorMode& aRhs)
{
   if (this != &aRhs)
   {
      WsfMode::operator          =(aRhs);
      mSensorPtr                 = nullptr;
      mPlatformPtr               = nullptr;
      mMaximumRange              = aRhs.mMaximumRange;
      mAzCueRate                 = aRhs.mAzCueRate;
      mElCueRate                 = aRhs.mElCueRate;
      mMinAzCue                  = aRhs.mMinAzCue;
      mMaxAzCue                  = aRhs.mMaxAzCue;
      mMinElCue                  = aRhs.mMinElCue;
      mMaxElCue                  = aRhs.mMaxElCue;
      mCueMode                   = aRhs.mCueMode;
      mMaximumRequestCount       = aRhs.mMaximumRequestCount;
      mCanSearchWhileTrack       = aRhs.mCanSearchWhileTrack;
      mDisablesSearch            = aRhs.mDisablesSearch;
      mFrameTime                 = aRhs.mFrameTime;
      mRevisitTime               = aRhs.mRevisitTime;
      mDwellTime                 = aRhs.mDwellTime;
      mRequiredPd                = aRhs.mRequiredPd;
      mEstablishTrackProbability = aRhs.mEstablishTrackProbability;
      mMaintainTrackProbability  = aRhs.mMaintainTrackProbability;
      mHitsToEstablishTrack      = aRhs.mHitsToEstablishTrack;
      mEstablishTrackWindow      = aRhs.mEstablishTrackWindow;
      mEstablishTrackMask        = aRhs.mEstablishTrackMask;
      mHitsToMaintainTrack       = aRhs.mHitsToMaintainTrack;
      mMaintainTrackWindow       = aRhs.mMaintainTrackWindow;
      mMaintainTrackMask         = aRhs.mMaintainTrackMask;
      mTrackQuality              = aRhs.mTrackQuality;
      mAzErrorStdDev             = aRhs.mAzErrorStdDev;
      mElErrorStdDev             = aRhs.mElErrorStdDev;
      mRangeErrorStdDev          = aRhs.mRangeErrorStdDev;
      mRangeRateErrorStdDev      = aRhs.mRangeRateErrorStdDev;
      mErrorModelPtr             = aRhs.mErrorModelPtr;
      mOnSuccessModeName         = aRhs.mOnSuccessModeName;
      mOnSuccessModeIndex        = 0;
      mOnFailureModeName         = aRhs.mOnFailureModeName;
      mOnFailureModeIndex        = 0;
      mMessageLength             = aRhs.mMessageLength;
      mMessagePriority           = aRhs.mMessagePriority;
      mReportsSpatialDomain      = aRhs.mReportsSpatialDomain;
      mReportingFlags            = aRhs.mReportingFlags;
      mCapabilitiesFlags         = aRhs.mCapabilitiesFlags;
   }
   return *this;
}

// =================================================================================================
WsfSensorMode::~WsfSensorMode() {}

// =================================================================================================
const WsfScenario& WsfSensorMode::GetScenario() const
{
   return mSensorPtr->GetScenario();
}

// =================================================================================================
WsfSimulation* WsfSensorMode::GetSimulation() const
{
   return mSensorPtr->GetSimulation();
}

// =================================================================================================
//! Initialize the sensor mode.
//! @param aSimTime   The current simulation time.
//! @returns 'true' if the mode was successfully initialize or 'false' if not.
// virtual
bool WsfSensorMode::Initialize(double aSimTime)
{
   bool ok = true;

   mPlatformPtr = mSensorPtr->GetPlatform();

   if (GetReportingFlags() == 0)
   {
      auto out = ut::log::warning() << "Sensor does not report anything.";
      out.AddNote() << "Platform: " << mPlatformPtr->GetName();
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
      out.AddNote() << "Mode: " << GetName();
   }

   if (!InitializeReporting())
   {
      ok = false;
   }

   if (mMaximumRequestCount == 0)
   {
      // If a pure search mode then set the appropriate values for search_while_track and disables_search.
      mCanSearchWhileTrack = true;
      mDisablesSearch      = false;
   }

   // Set the frame and revisit times equal if one is not defined.
   if ((mFrameTime <= 0.0) && (mRevisitTime > 0.0))
   {
      mFrameTime = mRevisitTime;
   }
   else if ((mRevisitTime <= 0.0) && (mFrameTime > 0.0))
   {
      mRevisitTime = mFrameTime;
   }

   // Validate the on_success/on_failure specifications.
   // If a given on_<> is not specified then its respective mode index is set to the current mode index.

   const WsfModeList* modeListPtr = mSensorPtr->GetModeList();

   mOnSuccessModeIndex = GetModeIndex();
   if (!mOnSuccessModeName.IsNull())
   {
      mOnSuccessModeIndex = modeListPtr->GetModeByName(mOnSuccessModeName);
      if (mOnSuccessModeIndex >= modeListPtr->GetModeCount())
      {
         auto out = ut::log::error() << "Unknown mode name for 'on_success'.";
         out.AddNote() << "Mode: " << mOnSuccessModeName;
         ok = false;
      }
   }

   mOnFailureModeIndex = GetModeIndex();
   if (!mOnFailureModeName.IsNull())
   {
      mOnFailureModeIndex = modeListPtr->GetModeByName(mOnFailureModeName);
      if (mOnFailureModeIndex >= modeListPtr->GetModeCount())
      {
         auto out = ut::log::error() << "Unknown mode name for 'on_failure'.";
         out.AddNote() << "Mode: " << mOnFailureModeName;
         ok = false;
      }
   }

   // Initialize the error model object.
   if (mErrorModelPtr != nullptr)
   {
      ok &= mErrorModelPtr->Initialize(this);

      // TODO remove this initialization when the sensor error variables are removed.
      auto stdErrorModelPtr = dynamic_cast<wsf::StandardSensorErrorModel*>(mErrorModelPtr.get());
      if (stdErrorModelPtr != nullptr)
      {
         const wsf::StandardSensorErrorModel::Sigmas& sigmas = stdErrorModelPtr->GetSigmas();
         mAzErrorStdDev                                      = sigmas.mAzErrorStdDev;
         mElErrorStdDev                                      = sigmas.mElErrorStdDev;
         mRangeErrorStdDev                                   = sigmas.mRangeErrorStdDev;
         mRangeRateErrorStdDev                               = sigmas.mRangeRateErrorStdDev;
      }
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfSensorMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "azimuth_cue_rate")
   {
      aInput.ReadValueOfType(mAzCueRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mAzCueRate, 0.0);
   }
   else if (command == "elevation_cue_rate")
   {
      aInput.ReadValueOfType(mElCueRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mElCueRate, 0.0);
   }
   else if (command == "azimuth_cue_limits")
   {
      aInput.ReadValueOfType(mMinAzCue, UtInput::cANGLE);
      aInput.ReadValueOfType(mMaxAzCue, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mMinAzCue, -UtMath::cPI);
      aInput.ValueLessOrEqual(mMaxAzCue, UtMath::cPI);
      aInput.ValueLessOrEqual(mMinAzCue, mMaxAzCue);
   }
   else if (command == "elevation_cue_limits")
   {
      aInput.ReadValueOfType(mMinElCue, UtInput::cANGLE);
      aInput.ReadValueOfType(mMaxElCue, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mMinElCue, -UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(mMaxElCue, UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(mMinElCue, mMaxElCue);
   }
   else if (command == "cue_mode")
   {
      std::string slewMode;
      aInput.ReadValue(slewMode);
      if (slewMode == "fixed")
      {
         mCueMode = WsfSensor::cSLEW_FIXED;
      }
      else if (slewMode == "azimuth")
      {
         mCueMode = WsfSensor::cSLEW_AZ;
      }
      else if (slewMode == "elevation")
      {
         mCueMode = WsfSensor::cSLEW_EL;
      }
      else if ((slewMode == "both") || (slewMode == "azimuth_and_elevation"))
      {
         mCueMode = WsfSensor::cSLEW_AZ_EL;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "maximum_request_count")
   {
      aInput.ReadValue(mMaximumRequestCount);
      aInput.ValueGreaterOrEqual(mMaximumRequestCount, 0);
   }
   else if (command == "search_while_track")
   {
      mCanSearchWhileTrack = true;
   }
   else if (command == "disables_search")
   {
      mDisablesSearch = true;
   }
   else if (command == "frame_time")
   {
      aInput.ReadValueOfType(mFrameTime, UtInput::cTIME);
      aInput.ValueGreater(mFrameTime, 0.0);
      if ((mSensorPtr != nullptr) && (mSensorPtr->GetScheduler() != nullptr) &&
          GetSimulation()) // TODO:REVIEW -- can't call FrameTimeChanged unless simulation is already started
      {
         mSensorPtr->GetScheduler()->FrameTimeChanged(GetSimulation()->GetSimTime(), GetNameId());
      }
   }
   else if (command == "revisit_time")
   {
      aInput.ReadValueOfType(mRevisitTime, UtInput::cTIME);
      aInput.ValueGreater(mRevisitTime, 0.0);
   }
   else if (command == "dwell_time")
   {
      aInput.ReadValueOfType(mDwellTime, UtInput::cTIME);
      aInput.ValueGreater(mDwellTime, 0.0);
   }
   else if (command == "required_pd")
   {
      aInput.ReadValue(mRequiredPd);
      aInput.ValueGreaterOrEqual(mRequiredPd, 0.0);
      aInput.ValueLess(mRequiredPd, 1.0);
   }
   else if (command == "track_quality")
   {
      aInput.ReadValue(mTrackQuality);
      aInput.ValueInClosedRange(mTrackQuality, 0.0, 1.0);
   }
   else if (command == "establish_track_probability")
   {
      aInput.ReadValue(mEstablishTrackProbability);
      aInput.ValueGreater(mEstablishTrackProbability, 0.0);
      aInput.ValueLessOrEqual(mEstablishTrackProbability, 1.0);
   }
   else if (command == "maintain_track_probability")
   {
      aInput.ReadValue(mMaintainTrackProbability);
      aInput.ValueGreater(mMaintainTrackProbability, 0.0);
      aInput.ValueLessOrEqual(mMaintainTrackProbability, 1.0);
   }
   else if (command == "hits_to_establish_track")
   {
      aInput.ReadValue(mHitsToEstablishTrack);
      aInput.ReadValue(mEstablishTrackWindow);
      aInput.ValueGreater(mHitsToEstablishTrack, 0);
      aInput.ValueLessOrEqual(mEstablishTrackWindow, 32);
      aInput.ValueGreaterOrEqual(mEstablishTrackWindow, mHitsToEstablishTrack);
      mEstablishTrackMask = WsfUtil::GenerateBitMask(mEstablishTrackWindow);
   }
   else if (command == "hits_to_maintain_track")
   {
      aInput.ReadValue(mHitsToMaintainTrack);
      aInput.ReadValue(mMaintainTrackWindow);
      aInput.ValueGreater(mHitsToMaintainTrack, 0);
      aInput.ValueLessOrEqual(mMaintainTrackWindow, 32);
      aInput.ValueGreaterOrEqual(mMaintainTrackWindow, mHitsToMaintainTrack);
      mMaintainTrackMask = WsfUtil::GenerateBitMask(mMaintainTrackWindow);
   }
   // 'on_success' and 'on_failure' are marked as deprecated because they are so restricted in their current
   // form that they aren't generally useful. They were a very early initial attempt to provide automatic mode
   // switching through search/acquire/track, but it was found to be severely lacking and needed to be handled
   // explicitly via scripting as the particular case required.
   else if (command == "on_success") // NO_DOC | DEPRECATED
   {
      // ut::log::warning() << "'on_success' sensor mode input will be deprecated in future release.";
      std::string modeName;
      aInput.ReadValue(modeName);
      mOnSuccessModeName = modeName;
   }
   else if (command == "on_failure") // NO_DOC | DEPRECATED
   {
      // ut::log::warning << "'on_failure' sensor mode input will be deprecated in future release.";
      std::string modeName;
      aInput.ReadValue(modeName);
      mOnFailureModeName = modeName;
   }
   else if (command == "message_length")
   {
      aInput.ReadValueOfType(mMessageLength, UtInput::cDATA_SIZE);
      aInput.ValueGreaterOrEqual(mMessageLength, 0);
   }
   else if (command == "message_priority")
   {
      aInput.ReadValue(mMessagePriority);
      aInput.ValueGreaterOrEqual(mMessagePriority, 0);
   }
   else if (command == "reports_nothing")
   {
      mReportsSpatialDomain = WSF_SPATIAL_DOMAIN_UNKNOWN;
      mReportingFlags.Set(cNONE);
   }
   else if (command == "reports_location")
   {
      mReportingFlags.mReportsLocation = true;
   }
   else if (command == "reports_velocity")
   {
      mReportingFlags.mReportsVelocity = true;
   }
   else if (command == "reports_range")
   {
      mReportingFlags.mReportsRange = true;
   }
   else if (command == "reports_bearing")
   {
      mReportingFlags.mReportsBearing = true;
   }
   else if (command == "reports_elevation")
   {
      mReportingFlags.mReportsElevation = true;
   }
   else if (command == "reports_range_rate")
   {
      mReportingFlags.mReportsRangeRate = true;
   }
   else if (command == "reports_side")
   {
      mReportingFlags.mReportsSide = true;
   }
   else if (command == "reports_type")
   {
      mReportingFlags.mReportsType = true;
   }
   else if (command == "reports_iff")
   {
      mReportingFlags.mReportsIFF_Status = true;
   }
   else if (command == "reports_signal_to_noise")
   {
      mReportingFlags.mReportsSignalToNoise = true;
   }
   else if (command == "reports_frequency")
   {
      mReportingFlags.mReportsFrequency = true;
   }
   else if ((command == "reports_pulsewidth") || (command == "reports_pw"))
   {
      mReportingFlags.mReportsPulseWidth = true;
      mReportingFlags.mReportsFrequency  = true;
   }
   else if ((command == "reports_pulse_repetition_interval") || (command == "reports_pri"))
   {
      mReportingFlags.mReportsPulseRepititionInterval = true;
      mReportingFlags.mReportsFrequency               = true;
   }
   else if (command == "reports_other")
   {
      mReportingFlags.mReportsOther = true;
   }
   else if (command == "error_model")
   {
      std::unique_ptr<wsf::SensorErrorModelBase> errorModelPtr{nullptr};
      if (WsfSensorErrorModelTypes::Get(WsfScenario::FromInput(aInput)).LoadInstance(aInput, errorModelPtr))
      {
         mErrorModelPtr = std::move(errorModelPtr);
      }
   }
   else if ((mErrorModelPtr != nullptr) && mErrorModelPtr->ProcessInput(aInput))
   {
   }
   else if (WsfMode::ProcessInput(aInput))
   {
   }
   else
   {
      // Loop over attached components to see if they can process the input.

      myCommand = false;
      WsfSensor* sensorPtr(GetSensor());
      assert(sensorPtr != nullptr);
      if (sensorPtr != nullptr)
      {
         const WsfSensor::ComponentList& components(sensorPtr->GetComponents());
         if (components.HasComponents())
         {
            for (auto component : components)
            {
               if (component->ProcessModeInput(aInput, *this))
               {
                  myCommand = true;
                  break;
               }
            }
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Return the number of beams in the mode.
//! Derived classes should return the number of beams that are defined.
//! @returns The number of beams defined. The result will be 0 if the derived class does not
//! support beams.
// virtual
size_t WsfSensorMode::GetBeamCount() const
{
   return 0;
}

// =================================================================================================
//! Return a pointer to the beam with the specified index.
//! A derived class will return a pointer
//! @param aIndex The index of the beam to be retrieved. The index is NOT checked for validity.
//!               It MUST be in the range [0..GetBeamCount()].
//! @returns A pointer to the beam. The results are undefined if an invalid index is provided or
//!          if the derived class does not support beams.
// virtual
WsfSensorBeam* WsfSensorMode::GetBeamEntry(size_t aIndex)
{
   return nullptr;
}

// =================================================================================================
//! Compute and apply measurement errors to the current detection attempt.
//!
//! @param aResult [input] The results of the sensing chance.
//!                [output] The mReportedLocWCS updated as appropriate.
// virtual
void WsfSensorMode::ApplyMeasurementErrors(WsfSensorResult& aResult)
{
   if (mErrorModelPtr != nullptr)
   {
      mErrorModelPtr->ApplyMeasurementErrors(aResult);
   }
}

// =================================================================================================
//! Compute the 'standard' measurement errors.
//!
//! @param aResult         [input]  The sensing chance result data.
//!                        [output] Measurement sigmas.
//! @param aAzError        [output] The computed azimuth error.
//! @param aElError        [output] The computed elevation error.
//! @param aRangeError     [output] The computed range error.
//! @param aRangeRateError [output] The computed range rate error.
// virtual
void WsfSensorMode::ComputeMeasurementErrors(WsfSensorResult& aResult,
                                             double&          aAzError,
                                             double&          aElError,
                                             double&          aRangeError,
                                             double&          aRangeRateError)
{
   if ((mErrorModelPtr != nullptr) && mErrorModelPtr->IsStandardModel())
   {
      // Bypass the sigma modifications above.

      // An error model may find the current sigmas, the default is to use the configured values.
      // DON'T overwrite the configured sigmas.  az-sig > az-sig' > report & blurring, but
      // the calculation will be repeated so don't set az-sig to az-sig'.

      // We give the error model a copy of the sigmas.  It can adjust them and
      // will set the reported errors on the Result object.  The modified reported
      // sigma("error") is in the Result object, while the applied error will
      // be in the error object, aka references to our input/output args.  The latter
      // values are used in ApplyErrrors, to re-place the reported position, so that any
      // following measure of az-el will be blurred by it.
      auto standardErrorModelPtr = static_cast<wsf::StandardSensorErrorModel*>(mErrorModelPtr.get());
      wsf::StandardSensorErrorModel::Sigmas sigmas(mAzErrorStdDev, mElErrorStdDev, mRangeErrorStdDev, mRangeRateErrorStdDev);
      standardErrorModelPtr->GetSphericalErrorStdDev(GetSensor()->GetRandom(), aResult, sigmas);
      aAzError        = sigmas.mAzErrorStdDev;
      aElError        = sigmas.mElErrorStdDev;
      aRangeError     = sigmas.mRangeErrorStdDev;
      aRangeRateError = sigmas.mRangeRateErrorStdDev;
   }

   // Invoke optional component models for this action.
   wsf::SphericalMeasurementErrors errors{aAzError, aElError, aRangeError, aRangeRateError};
   WsfSensorComponent::ComputeSphericalMeasurementErrors(*GetSensor(), aResult, errors);
   aAzError        = errors.mAzError;
   aElError        = errors.mElError;
   aRangeError     = errors.mRangeError;
   aRangeRateError = errors.mRangeRateError;
}

// =================================================================================================
//! Initialize a new track.
//!
//! This method is called to initialize a new track. A new track number
//! is assigned and all fields in the track are set to their initial state.
//!
//! @param aSimTime  The current simulation time.
//! @param aTrackPtr A reference to a pointer to the track to be initialized.
//!                  A track will be created if the pointer is null.
// virtual
void WsfSensorMode::InitializeTrack(double aSimTime, WsfTrack*& aTrackPtr)
{
   if (aTrackPtr == nullptr)
   {
      aTrackPtr = new WsfTrack(WsfTrack::cUNFILTERED_SENSOR, GetSensor(), GetPlatform());
   }
   aTrackPtr->Initialize(aSimTime, GetPlatform()->GetNextTrackId(), *GetSimulation());
   aTrackPtr->SetMessageDataTag(GetSimulation()->CreateMessageDataTag());

   // Initialize the sensor mode used to produce the track.
   aTrackPtr->SetSensorModeId(GetNameId());

   // NOTE: The following code to initialize the reporting flags in the track probably isn't necessary
   // anymore because UpdateTrack() always calls UpdateTrackReportingFlags() which can update the flags
   // based on operational conditions. It is being maintained JUST IN CASE somebody was expecting these
   // flags to be set upon exit from this method.

   aTrackPtr->SetSpatialDomain(mReportsSpatialDomain);
   aTrackPtr->SetWCS_LocationValid(ReportsLocation());
   aTrackPtr->SetRangeValid(ReportsRange());
   aTrackPtr->SetBearingValid(ReportsBearing());
   aTrackPtr->SetElevationValid(ReportsElevation());
   aTrackPtr->SetRangeRateValid(ReportsRangeRate());
   aTrackPtr->SetVelocityValid(ReportsVelocity());
   aTrackPtr->SetSideIdValid(ReportsSide());
   aTrackPtr->SetTypeIdValid(ReportsType());
   aTrackPtr->SetSignalToNoiseValid(ReportsSignalToNoise());

   // Set the baseline track quality.
   aTrackPtr->SetTrackQuality(mTrackQuality);
}

// =================================================================================================
//! Update the sensor cueing limits from the mode cueing limits.
//! This is called by WsfSensor::AttemptToDetect immediately before calling WsfSensorMode::AttempToDetect.
//! It modifies the sensor cueing limits to be the same as the mode cueing limits.
// virtual
void WsfSensorMode::UpdateSensorCueingLimits()
{
   WsfSensor* sPtr = GetSensor();
   sPtr->SetCueMode(mCueMode);
   sPtr->SetCueRates(mAzCueRate, mElCueRate);
   sPtr->SetAzCueLimits(mMinAzCue, mMaxAzCue);
   sPtr->SetElCueLimits(mMinElCue, mMaxElCue);
}

// =================================================================================================
//! Update a track with the results from the current detection.
//! @param aSimTime    The current simulation time.
//! @param aTrackPtr   A pointer to the track to be updated.
//! @param aTargetPtr  Pointer to the target.
//! @param aResult     The results of the current detection attempt
// virtual
void WsfSensorMode::UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult)
{
   // Update the track reporting flags (i.e.: <xxx>IsValid). This must be done before any part of the
   // track is updated as the flags are used to determine what data needs to be propagated from the
   // results of the detection attempt into the track.
   UpdateTrackReportingFlags(aSimTime, aTrackPtr);

   aTrackPtr->Update(aSimTime);
   aTrackPtr->SetMessageDataTag(GetSimulation()->CreateMessageDataTag());

   // Report the name of the mode used for this update.
   aTrackPtr->SetSensorModeId(GetNameId());

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
         GetSensor()->GetLocationWCS(snrLocationWCS);
      }
   }
   double platformLocationErrorWCS[3];
   GetPlatform()->GetLocationErrorWCS(platformLocationErrorWCS);
   UtVec3d::Add(snrLocationWCS, snrLocationWCS, platformLocationErrorWCS);
   aTrackPtr->SetOriginatorLocationWCS(snrLocationWCS);
   WsfMeasurement& measurement = aResult.mMeasurement;

   aTrackPtr->SetOriginatorTransformWCS(measurement.GetOriginatorTransformWCS());
   double reportedLocWCS[3];
   measurement.GetLocationWCS(reportedLocWCS);
   UpdateTrackPosition(aTrackPtr, aResult, reportedLocWCS);

   if (aTrackPtr->VelocityValid())
   {
      if (measurement.VelocityValid())
      {
         double velWCS[3];
         measurement.GetVelocityWCS(velWCS);
         aTrackPtr->SetVelocityWCS(velWCS);
      }
      else
      {
         double targetVelWCS[3];
         aTargetPtr->GetVelocityWCS(targetVelWCS);
         aTrackPtr->SetVelocityWCS(targetVelWCS);
      }
   }

   if (aTrackPtr->RangeRateValid())
   {
      if (measurement.RangeRateValid())
      {
         aTrackPtr->SetRangeRate(measurement.GetRangeRate());
      }
      else
      {
         if ((aResult.GetReceiver() != nullptr) && (aResult.GetReceiver()->GetPlatform() != nullptr))
         {
            // Get the target velocity.
            double relTgtVelWCS[3];
            aTargetPtr->GetVelocityWCS(relTgtVelWCS);

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
      else
      {
         aTrackPtr->SetSideId(aTargetPtr->GetSideId());
      }
   }

   if (aTrackPtr->TypeIdValid())
   {
      if (measurement.TypeIdValid())
      {
         aTrackPtr->SetTypeId(measurement.GetTypeId());
      }
      else
      {
         aTrackPtr->SetTypeId(aTargetPtr->GetTypeId());
      }
   }

   // There is no validity flag in the track for IFF. We simply check the sensor to decide if we should report.
   // For now assume IFF is simply done by checking to see if the target is on the same side as the sensing platform.

   if (ReportsIFF_Status())
   {
      WsfStringId targetSide = aTargetPtr->GetSideId();
      if (measurement.SideIdValid())
      {
         targetSide = measurement.GetSideId();
      }
      aTrackPtr->SetIFF_Status(
         mSensorPtr->GetScenario().GetIFF_Manager()->GetIFF_Status(GetPlatform()->GetSideId(),
                                                                   targetSide,
                                                                   aTargetPtr->GetCategories().GetCategoryList()));
   }

   if (aTrackPtr->SignalToNoiseValid())
   {
      aTrackPtr->SetSignalToNoise(aResult.mSignalToNoise);
   }

   // If the spatial domain hasn't been defined then simply copy the spatial domain of the target.
   if (aTrackPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_UNKNOWN)
   {
      aTrackPtr->SetSpatialDomain(aTargetPtr->GetSpatialDomain());
   }

   // The following provide information for higher-fidelity tracking operation.
   if (measurement.SensorAzimuthValid()) // Should always be true
   {
      aTrackPtr->SetSensorAzimuthValid(true);
      aTrackPtr->SetSensorAzimuth(measurement.GetSensorAzimuth());
      aTrackPtr->SetSensorAzimuthError(measurement.GetSensorAzimuthError());
   }

   if (measurement.SensorElevationValid()) // Should always be true
   {
      aTrackPtr->SetSensorElevationValid(true);
      aTrackPtr->SetSensorElevation(measurement.GetSensorElevation());
      aTrackPtr->SetSensorElevationError(measurement.GetSensorElevationError());
   }

   // This is not really 'track' information but is carried along for
   // non-simulation purposes or if somebody really needs to 'cheat'.
   aTrackPtr->SetTargetIndex(aTargetPtr->GetIndex());
   aTrackPtr->SetTargetName(aTargetPtr->GetNameId());
   aTrackPtr->SetTargetType(aTargetPtr->GetTypeId());

   // Invoke optional component models for this action.
   WsfSensorComponent::UpdateTrack(*GetSensor(), aSimTime, aTrackPtr, aTargetPtr, aResult);
}

// =================================================================================================
//! Update a track with new position information from the current detection.
//! @param aTrackPtr       A pointer to the track to be updated.
//! @param aResult [input] The results of the sensing chance.
//! @param aTgtLocationWCS New location specified for this track.
//! @note Errors for range, bearing, elevation, and measurement covariance will
//! also be set as appropriate.
// protected
void WsfSensorMode::UpdateTrackPosition(WsfTrack* aTrackPtr, WsfSensorResult& aResult, const double aTgtLocationWCS[3])
{
   // The incoming target location is the true location after being fuzzed up with sensor errors.
   // It must be biased by the sensing platform location error because the incoming location was
   // computed using the truth location of the sensor's receiver antenna (if attached) or platform
   // (if no receiver).
   double platformLocationErrorWCS[3];
   GetPlatform()->GetLocationErrorWCS(platformLocationErrorWCS);
   double tgtLocationWCS[3];
   UtVec3d::Add(tgtLocationWCS, aTgtLocationWCS, platformLocationErrorWCS);

   double range     = 0.0;
   double bearing   = 0.0;
   double elevation = 0.0;
   double WCS_ToBCS_Transform[3][3];
   UtQuaternion(aResult.mMeasurement.GetOriginatorTransformWCS()).Get(WCS_ToBCS_Transform);

   // Update the various fields...
   if (aTrackPtr->LocationValid())
   {
      aTrackPtr->SetLocationWCS(tgtLocationWCS);
      double originLocWCS[3];
      aTrackPtr->GetOriginatorLocationWCS(originLocWCS);
      UtMeasurementUtil::RangeBearingElevation(originLocWCS, tgtLocationWCS, range, bearing, elevation);
      aTrackPtr->Set3D(true);
   }

   WsfMeasurement& measurement = aResult.mMeasurement;

   if (aTrackPtr->RangeValid() || aTrackPtr->BearingValid() || aTrackPtr->ElevationValid())
   {
      double tgtLocationNED[3];
      if ((aResult.GetReceiver() != nullptr) && (aResult.GetReceiver()->GetAntenna() != nullptr))
      {
         aResult.GetReceiver()->GetAntenna()->ConvertWCS_ToNED(tgtLocationWCS, tgtLocationNED);
      }
      else
      {
         GetPlatform()->ConvertWCSToNED(tgtLocationWCS, tgtLocationNED);
      }
      bearing   = atan2(tgtLocationNED[1], tgtLocationNED[0]);
      double xy = sqrt(tgtLocationNED[0] * tgtLocationNED[0] + tgtLocationNED[1] * tgtLocationNED[1]);
      elevation = atan2(-tgtLocationNED[2], xy);

      if (aTrackPtr->RangeValid())
      {
         range = UtVec3d::Magnitude(tgtLocationNED);
         aTrackPtr->SetRange(range);
         aTrackPtr->SetRangeError(measurement.GetRangeError());
      }
      if (aTrackPtr->RangeRateValid())
      {
         aTrackPtr->SetRangeRate(measurement.GetRangeRate());
         aTrackPtr->SetRangeRateError(measurement.GetRangeRateError());
      }
      if (aTrackPtr->ElevationValid())
      {
         aTrackPtr->SetElevation(elevation);
         // Set the elevation error to be the el error.
         // It will be corrected if a valid covariance can be computed (below).
         aTrackPtr->SetElevationError(measurement.GetSensorElevationError());
      }
      if (aTrackPtr->BearingValid())
      {
         aTrackPtr->SetBearing(bearing);
         // Set the bearing error to be the az error.
         // It will be corrected if a valid covariance can be computed (below).
         aTrackPtr->SetBearingError(measurement.GetSensorAzimuthError());
      }
      if (aTrackPtr->BearingValid() && aTrackPtr->ElevationValid())
      {
         if ((aTrackPtr->GetBearingError() > 0.0) || (aTrackPtr->GetElevationError() > 0.0))
         {
            // Find the BCS to NED transform and extract the roll angle.
            double WCS_ToNED_Transform[3][3];
            GetPlatform()->GetWCSToNEDTransform(WCS_ToNED_Transform);
            double NED_ToBCS_Transform[3][3];
            double NED_ToWCS_Transform[3][3];
            UtMat3d::Transpose(NED_ToWCS_Transform, WCS_ToNED_Transform);
            UtMat3d::Multiply(NED_ToBCS_Transform, WCS_ToBCS_Transform, NED_ToWCS_Transform);
            double psi, theta, phi;
            UtEntity::ExtractEulerAngles(NED_ToBCS_Transform, psi, theta, phi);
            double sinPhi = sin(phi);
            double cosPhi = cos(phi);

            // Provide accurate bearing / elevation errors by rotating by the roll angle.
            double bearingError =
               fabs(measurement.GetSensorAzimuthError() * cosPhi + measurement.GetSensorElevationError() * sinPhi);
            double elevationError =
               fabs(measurement.GetSensorElevationError() * cosPhi - measurement.GetSensorAzimuthError() * sinPhi);
            aTrackPtr->SetBearingError(bearingError);
            aTrackPtr->SetElevationError(elevationError);
         }
      }
      if (!aTrackPtr->Is3D())
      {
         aTrackPtr->Set3D(aTrackPtr->RangeValid() && aTrackPtr->BearingValid() && aTrackPtr->ElevationValid());
      }
   }

   if (aTrackPtr->Is3D())
   {
      // Attempt to create a measurement covariance for this track.
      UtCovariance* measurementCovarPtr = nullptr;

      if ((measurement.GetRangeError() > 0.0) && (measurement.GetSensorAzimuthError() > 0.0) &&
          (measurement.GetSensorElevationError() > 0.0))
      {
         double originLocWCS[3];
         aTrackPtr->GetOriginatorLocationWCS(originLocWCS);

         double locationWCS[3];
         if (aTrackPtr->LocationValid())
         {
            aTrackPtr->GetLocationWCS(locationWCS);
            double targetVecWCS[3];
            UtVec3d::Subtract(targetVecWCS, locationWCS, originLocWCS);
            range = UtVec3d::Magnitude(targetVecWCS);
         }
         else // RBE valid
         {
            UtMeasurementUtil::LocationWCS(originLocWCS, range, bearing, elevation, locationWCS);
         }

         // Using the BCS coordinate system is the "Natural" coordinate system for computing the measurement covariance.
         measurementCovarPtr = UtCovariance::CreateMeasurementCovariance(range,
                                                                         measurement.GetRangeError(),
                                                                         measurement.GetSensorAzimuthError(),
                                                                         measurement.GetSensorElevationError());
         measurementCovarPtr->SetTargetLocationWCS(locationWCS);
         measurementCovarPtr->SetOriginLocationWCS(originLocWCS);

         // Here frame A is BCS and frame B is WCS; parameter is the B to A transform.
         UtCovariance::ConvertFromFrameA_ToFrameB(*measurementCovarPtr, WCS_ToBCS_Transform);
      }

      if (measurementCovarPtr != nullptr)
      {
         aTrackPtr->SetMeasurementCovariance(aTrackPtr->GetUpdateTime(), measurementCovarPtr);
      }
   }
}

// =================================================================================================
//! Update the reported data flags in a track.
//!
//! This is called by UpdateTrack() prior to applying the sensor detection results to the track.
//! The function of the method is to set the flags in the track that indicate what data is 'valid'.
//! UpdateTrack() uses the flags to decide what data from the sensor detection results is to be pushed
//! into the track.
//!
//! A base class may override this method to provide the ability to dynamically change the reporting
//! abilities. The default method simply sets the reporting flags from the mode.
//!
//! @param aSimTime The current simulation time.
//! @param aTrackPtr The track whose reporting flags are to be updated.
//!
//! @note In general, the base class should call this method.
// virtual
void WsfSensorMode::UpdateTrackReportingFlags(double aSimTime, WsfTrack* aTrackPtr)
{
   aTrackPtr->SetSpatialDomain(mReportsSpatialDomain);
   aTrackPtr->SetWCS_LocationValid(ReportsLocation());
   aTrackPtr->SetRangeValid(ReportsRange());
   aTrackPtr->SetBearingValid(ReportsBearing());
   aTrackPtr->SetElevationValid(ReportsElevation());
   aTrackPtr->SetRangeRateValid(ReportsRangeRate());
   aTrackPtr->SetVelocityValid(ReportsVelocity());
   aTrackPtr->SetSideIdValid(ReportsSide());
   aTrackPtr->SetTypeIdValid(ReportsType());
   aTrackPtr->SetSignalToNoiseValid(ReportsSignalToNoise());
}

// =================================================================================================
//! Perform a very conservative check to see if the target is within the detection range of the sensor.
//!
//! This method performs a very conservative check to see if the target is within the detection range of the sensor.
//! It should can be called by the before attempting to update the targets position to the current time.
//! It will make a quick estimate of the targets current location based on the last known location and then check to
//! see if that location is somewhat close to the detection range of the sensor.
//!
//! Judicious use of this method can reduce computation time.
//!
//! @param aSimTime   The current simulation time.
//! @param aTargetPtr The target in question.
//! @returns true if there is even a remote possibility that the target is within range of the sensor.
// virtual
bool WsfSensorMode::WithinDetectionRange(double aSimTime, WsfPlatform* aTargetPtr)
{
   return WsfUtil::PotentiallyWithinRange(aSimTime, mPlatformPtr, aTargetPtr, mMaximumRange);
}

// =================================================================================================
void WsfSensorMode::SetAzErrorStdDev(double aAzErrorStdDev)
{
   auto stdErrorModelPtr = dynamic_cast<wsf::StandardSensorErrorModel*>(mErrorModelPtr.get());
   if (stdErrorModelPtr != nullptr)
   {
      stdErrorModelPtr->SetAzErrorStdDev(aAzErrorStdDev);
   }
   mAzErrorStdDev = aAzErrorStdDev;
}

// =================================================================================================
void WsfSensorMode::SetElErrorStdDev(double aElErrorStdDev)
{
   auto stdErrorModelPtr = dynamic_cast<wsf::StandardSensorErrorModel*>(mErrorModelPtr.get());
   if (stdErrorModelPtr != nullptr)
   {
      stdErrorModelPtr->SetElErrorStdDev(aElErrorStdDev);
   }
   mElErrorStdDev = aElErrorStdDev;
}

// =================================================================================================
void WsfSensorMode::SetRangeErrorStdDev(double aRangeErrorStdDev)
{
   auto stdErrorModelPtr = dynamic_cast<wsf::StandardSensorErrorModel*>(mErrorModelPtr.get());
   if (stdErrorModelPtr != nullptr)
   {
      stdErrorModelPtr->SetRangeErrorStdDev(aRangeErrorStdDev);
   }
   mRangeErrorStdDev = aRangeErrorStdDev;
}

// =================================================================================================
void WsfSensorMode::SetRangeRateErrorStdDev(double aRangeRateErrorStdDev)
{
   auto stdErrorModelPtr = dynamic_cast<wsf::StandardSensorErrorModel*>(mErrorModelPtr.get());
   if (stdErrorModelPtr != nullptr)
   {
      stdErrorModelPtr->SetRangeRateErrorStdDev(aRangeRateErrorStdDev);
   }
   mRangeRateErrorStdDev = aRangeRateErrorStdDev;
}

// =================================================================================================
//! Alternate frequency selection/change event that changes a sensor's frequency.
WsfSensorMode::AltFrequencyChangeEvent::AltFrequencyChangeEvent(double         aSimTime,
                                                                WsfSensorMode* aSensorModePtr,
                                                                int            aAltFrequencyId /*= -1*/)
   : WsfEvent(aSimTime)
   , mSensorModePtr(aSensorModePtr)
   , mAltFrequencyId(aAltFrequencyId)
{
   mPlatformIndex = mSensorModePtr->GetPlatform()->GetIndex();
}

// =================================================================================================
WsfEvent::EventDisposition WsfSensorMode::AltFrequencyChangeEvent::Execute()
{
   WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if (platformPtr != nullptr)
   {
      if (mSensorModePtr != nullptr)
      {
         mSensorModePtr->SelectAlternateFrequency(GetTime(), mAltFrequencyId);
      }
   }
   return cDELETE;
}

// =================================================================================================
bool WsfSensorMode::InitializeReporting()
{
   // Flag what we want to report but can't.
   ReportingFlags willReport   = mReportingFlags.Get() & mCapabilitiesFlags.Get();
   ReportingFlags cannotReport = (mReportingFlags.Get() ^ mCapabilitiesFlags.Get()) & mReportingFlags.Get();

   bool ok = true;

   // If there is anything that cannot be reported, print a warning message.
   // In the case that nothing can be reported, return false.
   if (cannotReport.Get() != 0) // quick return; no further processing required
   {
      if (willReport.Get() == 0)
      {
         ok = false; // we must be able to report something.
      }
      else // Notify the user of the inconsistency.
      {
         auto out = ut::log::warning() << "Platform's Sensor in Mode is unable to report all requested fields.";
         out.AddNote() << "Platform: " << GetSensor()->GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << GetSensor()->GetName();
         out.AddNote() << "Mode: " << GetName();
         auto note = out.AddNote() << "Cannot Report:";
         if (cannotReport.mReportsBearing)
         {
            note.AddNote() << "bearing";
         }
         if (cannotReport.mReportsElevation)
         {
            note.AddNote() << "elevation";
         }
         if (cannotReport.mReportsFrequency)
         {
            note.AddNote() << "frequency";
         }
         if (cannotReport.mReportsIFF_Status)
         {
            note.AddNote() << "iff_status";
         }
         if (cannotReport.mReportsLocation)
         {
            note.AddNote() << "location";
         }
         if (cannotReport.mReportsOther)
         {
            note.AddNote() << "other";
         }
         if (cannotReport.mReportsPulseRepititionInterval)
         {
            note.AddNote() << "pulse_repetition_interval";
         }
         if (cannotReport.mReportsPulseWidth)
         {
            note.AddNote() << "pulse_width";
         }
         if (cannotReport.mReportsRange)
         {
            note.AddNote() << "range";
         }
         if (cannotReport.mReportsRangeRate)
         {
            note.AddNote() << "range_rate";
         }
         if (cannotReport.mReportsSide)
         {
            note.AddNote() << "side";
         }
         if (cannotReport.mReportsSignalToNoise)
         {
            note.AddNote() << "signal_to_noise";
         }
         if (cannotReport.mReportsType)
         {
            note.AddNote() << "type";
         }
         if (cannotReport.mReportsVelocity)
         {
            note.AddNote() << "velocity";
         }

         mReportingFlags = willReport;
      }
   }

   return ok;
}
