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

#include "WsfSAR_Sensor.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>

#include "UtAzElLookup.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Attenuation.hpp"
#include "WsfEM_Util.hpp"
#include "WsfImage.hpp"
#include "WsfImageMessage.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSignature.hpp"
#include "WsfSensorComponent.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackObserver.hpp"

// References:
//
// 1) "Spotlight Synthetic Aperture Radar Signal Processing Algorithms", Walter G. Carrara,
//    Ron S. Goodman and Ronald M. Majewski, Artech House, 1995.
// 2) "Synthetic Aperture Radar Mode Constraints", Matthew J. Renaud, The Boeing Company,
//    Revision C, 25-April-2007.
// 3) "Fundamentals of Radar" class notes, section "Air-To-Ground Radar Modes", The Boeing
//    Electronic Systems Engineering.

// NOTE:
//
// The term 'squint angle' had conflicting definitions in the references. References 1 and 3
// define 'squint angle' to be the angle in X-Y plane (local horizontal) between the projection
// of the aircraft velocity vector onto the X-Y plane and the projection of the line-of-sight
// vector to the scene center on the X-Y plane.
//
// Reference 2 defined the 'squint angle' to be the TOTAL angle between the aircraft velocity
// vector and the line-of-sight vector to the scene center.
//
// For the purposes of this code, 'squint angle' will follow the FIRST definition. The term
// 'total angle' will replace 'squint angle' in formulas from reference 2.

namespace
{

//!  An internal event scheduled to complete an automatic turn off.

// NOTE: The automatic turn off process used to be accomplished by having TurnOn() simply schedule
// a WsfSensorEvent(...,cTURN_ON). Unfortunately, that only worked if the automatic turn off was the
// only way the sensor was ever turned off. If the sensor was turned off manually before the event
// dispatched, the event was still left out there and would still dispatch. If a new image collection
// task was initiated prior to the dispatch of the orphaned event, the event would dispatch and stop
// the new collection task prematurely!
class AutoTurnOffEvent : public WsfEvent
{
public:
   AutoTurnOffEvent(double aSimTime, WsfSAR_Sensor* aSensorPtr)
      : WsfEvent(aSimTime)
      , mSensorPtr(aSensorPtr)
      , mPlatformIndex(aSensorPtr->GetPlatform()->GetIndex())
      , mTurnOffEventNumber(aSensorPtr->GetTurnOffEventNumber())
   {
   }

   EventDisposition Execute() override
   {
      WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
      if (platformPtr != nullptr)
      {
         if (mTurnOffEventNumber == mSensorPtr->GetTurnOffEventNumber())
         {
            GetSimulation()->TurnPartOff(GetTime(), mSensorPtr);
         }
      }
      return cDELETE;
   }

private:
   WsfSAR_Sensor* mSensorPtr;
   size_t         mPlatformIndex;
   unsigned int   mTurnOffEventNumber;
};

} // namespace

// ================================================================================================
WsfSAR_Sensor::WsfSAR_Sensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mSAR_ModeList()
   , mChances()
   , mStreamNumber(0)
   , mFrameNumber(0)
   , mBackscatterCoefficient(0.0)
   , mIntensityMin(0.0)
   , mIntensityMax(0.0)
   , mIntensityRange(0.0)
   , mLastUpdateTime(0.0)
   , mModeStartTime(0.0)
   , mLastModeIndex(0)
   , mTurnOffEventNumber(0)
   , mRescaleSubstandardImage(true)
   , mAutomaticTurnOff(false)
   , mSAR_ConstraintPlotting(false)
   , mShowStatus(false)
   , mCallSensorTrackObservers(false)
   , mAchievedCNR(0.0)
   , mAchievedAzimuthResolution(0.0)
   , mAchievedGroundRangeResolution(0.0)
   , mTempPlatform(aScenario)
{
   SetClass(cACTIVE | cRADIO | cIMAGING); // This is an active RF sensor that produces an image.
   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new SAR_Mode()));
}

// ================================================================================================
WsfSAR_Sensor::WsfSAR_Sensor(const WsfSAR_Sensor& aSrc)
   : WsfSensor(aSrc)
   , mSAR_ModeList()
   , mChances()
   , mStreamNumber(0)
   , mFrameNumber(0)
   , mBackscatterCoefficient(aSrc.mBackscatterCoefficient)
   , mIntensityMin(aSrc.mIntensityMin)
   , mIntensityMax(aSrc.mIntensityMax)
   , mIntensityRange(aSrc.mIntensityRange)
   , mLastUpdateTime(0.0)
   , mModeStartTime(0.0)
   , mLastModeIndex(0)
   , mTurnOffEventNumber(0)
   , mRescaleSubstandardImage(aSrc.mRescaleSubstandardImage)
   , mAutomaticTurnOff(aSrc.mAutomaticTurnOff)
   , mSAR_ConstraintPlotting(aSrc.mSAR_ConstraintPlotting)
   , mShowStatus(aSrc.mShowStatus)
   , mCallSensorTrackObservers(aSrc.mCallSensorTrackObservers)
   , mAchievedCNR(0.0)
   , mAchievedAzimuthResolution(0.0)
   , mAchievedGroundRangeResolution(0.0)
   , mTempPlatform(aSrc.GetScenario())
{
}

// ================================================================================================
// virtual
WsfSAR_Sensor::~WsfSAR_Sensor()
{
   ClearSensingChances();
}

// ================================================================================================
// This is a pass-through to the base class implementation. The PRF and dwell time are computed
// (if required) using the geometry at sensor turn-on or mode switch. When called from sensor_plot
// the geometry is constantly changing without a corresponding mode change.
//
// This method simply computes the PRF and dwell time (if needed), and calls the base class method.
// virtual
bool WsfSAR_Sensor::AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult)
{
   SAR_Mode* modePtr = mSAR_ModeList[mModeListPtr->GetCurrentMode()];
   UpdatePosition(aSimTime);
   modePtr->ComputeGeometry(aSimTime, mGeometry);
   if (modePtr->mComputeDwellTime)
   {
      modePtr->mDwellTime                = modePtr->ComputeDwellTime(mGeometry, modePtr->mResolution);
      modePtr->mDwellTime                = std::min(modePtr->mDwellTime, modePtr->mMaximumDwellTime);
      modePtr->mCurrentAzimuthResolution = modePtr->mResolution;
   }
   else
   {
      modePtr->mCurrentAzimuthResolution = modePtr->ComputeAzimuthResolution(mGeometry, modePtr->mDwellTime);
   }
   modePtr->mCurrentGroundRangeResolution = modePtr->ComputeGroundRangeResolution(mGeometry);

   if (modePtr->mComputePRF)
   {
      double prf = modePtr->ComputePRF(mGeometry);
      modePtr->mXmtrPtr->SetPulseRepetitionFrequency(prf);
   }
   return WsfSensor::AttemptToDetect(aSimTime, aTargetPtr, aSettings, aResult);
}

// ================================================================================================
// virtual
WsfSensor* WsfSAR_Sensor::Clone() const
{
   return new WsfSAR_Sensor(*this);
}

// ================================================================================================
//! Can a spot image request be scheduled at the current time.
//!
//! Given the current platform status and currently selected sensor mode and cued location,
//! determine if a an imaging request can satisfy the operating requirements.
//!
//! @param aSimTime        [input] The current simulation time.
//! @param aDwellTime      [output] The dwell time required. (seconds)
//! @param aResolution     [output] The resolution a 'pixel' (assumed square). (meters)
//! @param aCNR            [output] The clutter-to-noise ratio (absolute, not DB).
//! @param aDopplerMargin  [output] The margin between the determined PRF and the minimum PRF.
//!                                 A negative value indicates a Doppler ambiguity.
//! @param aFailureReasons [output] If the return value is false, this is a bit mask of FailureReasons
//!                                 that indicate why the imaging operation should not occur.
//! @returns 'true' if imaging is allowed or 'false' if not.
bool WsfSAR_Sensor::CanImageNow(double  aSimTime,
                                double& aDwellTime,
                                double& aResolution,
                                double& aCNR,
                                double& aDopplerMargin,
                                int&    aFailureReasons)
{
   aFailureReasons = 0;      // Assume no failures
   UpdatePosition(aSimTime); // Ensure my position is current

   SAR_Mode* modePtr = mSAR_ModeList[mModeListPtr->GetCurrentMode()];
   modePtr->ComputeGeometry(aSimTime, mGeometry);
   double prf;
   double minimumPRF;
   modePtr->PredictPerformance(mGeometry, prf, aDwellTime, aResolution, aCNR, minimumPRF);

   // double ratio = aDwellTime / modePtr->mMaximumDwellTime;
   // if (ratio >= 1.01)
   if (aDwellTime > modePtr->mMaximumDwellTime)
   {
      aFailureReasons |= cFR_DWELL_TIME_LIMIT_EXCEEDED;
   }

   if (aCNR < modePtr->mMinimumCNR)
   {
      aFailureReasons |= cFR_INSUFFICIENT_CLUTTER_TO_NOISE;
   }

   aDopplerMargin = 0.0;
   if (prf > 0.0)
   {
      aDopplerMargin = prf - minimumPRF;
      if (aDopplerMargin < 0.0)
      {
         aFailureReasons |= cFR_DOPPLER_FOLDOVER;
      }
   }

   // If we're looking backward into the array then we cannot image.
   if (mGeometry.mScanAngle >= UtMath::cPI_OVER_2)
   {
      aFailureReasons |= cFR_FIELD_OF_VIEW_EXCEEDED;
   }
   return (aFailureReasons == 0);
}

// ================================================================================================
//! Get the dwell time for spot mode.
//! @note This is effective only if the currently selected mode is a 'spot' mode.
double WsfSAR_Sensor::GetDwellTime() const
{
   SAR_Mode* modePtr = mSAR_ModeList[mModeListPtr->GetCurrentMode()];
   return modePtr->mDwellTime;
}

// ================================================================================================
//! Set the dwell time for spot mode.
//! @note This is effective only if the currently selected mode is a 'spot' mode.
void WsfSAR_Sensor::SetDwellTime(double aDwellTime)
{
   SAR_Mode* modePtr          = mSAR_ModeList[mModeListPtr->GetCurrentMode()];
   modePtr->mDwellTime        = aDwellTime;
   modePtr->mComputeDwellTime = (aDwellTime <= 0.0);
}

// ================================================================================================
//! Get the desired image resolution for a spot SAR request.
//! @note This is effective only if the currently selected mode is a 'spot' mode.
double WsfSAR_Sensor::GetResolution() const
{
   SAR_Mode* modePtr = mSAR_ModeList[mModeListPtr->GetCurrentMode()];
   return modePtr->mResolution;
}

// ================================================================================================
//! Set the desired resolution for a spot SAR mode request.
//! @note This is effective only if the currently selected mode is a 'spot' mode.
void WsfSAR_Sensor::SetResolution(double aResolution)
{
   SAR_Mode* modePtr          = mSAR_ModeList[mModeListPtr->GetCurrentMode()];
   modePtr->mResolution       = aResolution;
   modePtr->mComputeDwellTime = (aResolution > 0.0);
}

// ================================================================================================
// virtual
bool WsfSAR_Sensor::Initialize(double aSimTime)
{
   // If 'sar_constraint_plotting true', disable the slewing/cueing limits.
   if (mSAR_ConstraintPlotting)
   {
      SetAzSlewLimits(-UtMath::cPI, UtMath::cPI);
      SetElSlewLimits(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      SetAzCueLimits(-UtMath::cPI, UtMath::cPI);
      SetElCueLimits(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }

   bool ok = WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mSAR_ModeList);
   mLastModeIndex = mModeListPtr->GetModeCount();
   mModeStartTime = aSimTime;

   return ok;
}

// =================================================================================================
// virtual
void WsfSAR_Sensor::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   SAR_Mode* modePtr = mSAR_ModeList[mModeListPtr->GetCurrentMode()];
   AddSensingChance(aSimTime, *modePtr, mGeometry, aPlatformPtr);
   WsfSensor::PlatformAdded(aSimTime, aPlatformPtr);
}

// =================================================================================================
// virtual
void WsfSAR_Sensor::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   auto it = std::find_if(std::begin(mChances),
                          std::end(mChances),
                          [aPlatformPtr](const std::unique_ptr<SAR_Chance>& aChance)
                          { return aChance->mTargetIndex == aPlatformPtr->GetIndex(); });

   if (it != std::end(mChances))
   {
      mChances.erase(it);
   }

   WsfSensor::PlatformDeleted(aPlatformPtr);
}

// ================================================================================================
// virtual
bool WsfSAR_Sensor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "rescale_substandard_image")
   {
      aInput.ReadValue(mRescaleSubstandardImage);
   }
   else if (command == "automatic_turn_off")
   {
      mAutomaticTurnOff = true;
   }
   else if (command == "sar_constraint_plotting")
   {
      aInput.ReadValue(mSAR_ConstraintPlotting);
   }
   else if (command == "show_status")
   {
      mShowStatus = true;
   }
   else if (command == "call_sensor_track_observers")
   {
      aInput.ReadValue(mCallSensorTrackObservers);
   }
   else if (command == "backscatter_coefficient")
   {
      aInput.ReadValueOfType(mBackscatterCoefficient, UtInput::cRATIO);
   }
   else if (command == "intensity_limits")
   {
      aInput.ReadValueOfType(mIntensityMin, UtInput::cPOWER_DB);
      aInput.ReadValueOfType(mIntensityMax, UtInput::cPOWER_DB);
      aInput.ValueGreater(mIntensityMin, 0.0);
      aInput.ValueGreater(mIntensityMax, mIntensityMin);
      mIntensityRange = 0.0;
   }
   else if (command == "intensity_range")
   {
      aInput.ReadValueOfType(mIntensityRange, UtInput::cPOWER_DB);
      aInput.ValueGreater(mIntensityRange, 0.0);
      mIntensityMin = 0.0;
      mIntensityMax = 0.0;
   }
   else
   {
      myCommand = WsfSensor::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
// virtual
void WsfSAR_Sensor::TurnOff(double aSimTime)
{
   ++mTurnOffEventNumber; // Invalidate any outstanding automatic turn off events

   // Process turn-off logic only if Update actually got called.
   if (mLastModeIndex < mModeListPtr->GetModeCount())
   {
      // If turning off while in spot mode, send the image.
      SAR_Mode* modePtr = mSAR_ModeList[mLastModeIndex];
      if (modePtr->mOperatingMode == SAR_Mode::cOM_SPOT)
      {
         SpotModeEnd(aSimTime, *modePtr);
      }
   }

   ClearSensingChances();
   WsfSensor::TurnOff(aSimTime); // Let the base class do its thing
}

// ================================================================================================
// virtual
void WsfSAR_Sensor::TurnOn(double aSimTime)
{
   ++mTurnOffEventNumber; // Invalidate any outstanding automatic turn off events

   // Set the 'last mode index' to an invalid value. This is a sign to Update/ProcessModeChange that the call
   // is due to the sensor being turned on.

   mLastModeIndex  = mModeListPtr->GetModeCount();
   mLastUpdateTime = aSimTime;

   mAchievedAzimuthResolution     = 0.0;
   mAchievedGroundRangeResolution = 0.0;
   mAchievedCNR                   = 0.0;

   ClearSensingChances();

   WsfSensor::TurnOn(aSimTime); // Let the base class do its thing
}

// ================================================================================================
// virtual
void WsfSAR_Sensor::Update(double aSimTime)
{
   // Exit quickly if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime > (aSimTime + 1.0E-5))
   {
      return;
   }

   WsfSensor::Update(aSimTime);

   // Determine if the mode switch has occurred, or if this is the first call since the sensor was turned-on.

   if (mModeListPtr->GetCurrentMode() != mLastModeIndex)
   {
      ProcessModeChange(aSimTime, mLastModeIndex, mModeListPtr->GetCurrentMode());
      mLastModeIndex = mModeListPtr->GetCurrentMode();
      mModeStartTime = aSimTime;
   }

   // Tell the observers that the 'scan' is being updated. At a minimum this will force
   // the DIS interface to publish a PDU as to where the sensor is pointed.

   WsfObserver::SensorScanUpdated(GetSimulation())(aSimTime, this);

   // Call the appropriate 'update' method depending on the operating mode.

   Settings settings;
   settings.mModeIndex = mModeListPtr->GetCurrentMode();
   SAR_Mode* modePtr   = mSAR_ModeList[settings.mModeIndex];
   switch (modePtr->mOperatingMode)
   {
   case SAR_Mode::cOM_SPOT:
      SpotModeUpdate(aSimTime, settings);
      break;
   case SAR_Mode::cOM_STRIP:
      StripModeUpdate(aSimTime, settings);
      break;
   default:
      // should never happen.
      break;
   }

   mLastUpdateTime = aSimTime;
   mNextUpdateTime += GetUpdateInterval(); // Set the time of the next update
}

// ================================================================================================
// virtual
size_t WsfSAR_Sensor::GetEM_RcvrCount() const
{
   // Returns 0 prior to initialization
   return mSAR_ModeList.empty() ? 0U : 1U;
}

// ================================================================================================
// virtual
WsfEM_Rcvr& WsfSAR_Sensor::GetEM_Rcvr(size_t aIndex) const
{
   if (aIndex > 0)
   {
      static WsfEM_Rcvr dummyRcvr(WsfEM_Rcvr::cRF_SENSOR);
      return dummyRcvr;
   }
   return *mSAR_ModeList[mModeListPtr->GetCurrentMode()]->mRcvrPtr;
}

// ================================================================================================
size_t WsfSAR_Sensor::GetEM_XmtrCount() const
{
   // Returns 0 prior to initialization
   return mSAR_ModeList.empty() ? 0U : 1U;
}

// ================================================================================================
// virtual
WsfEM_Xmtr& WsfSAR_Sensor::GetEM_Xmtr(size_t aIndex) const
{
   if (aIndex > 0)
   {
      static WsfEM_Xmtr dummyXmtr(WsfEM_Xmtr::cXF_SENSOR);
      return dummyXmtr;
   }
   return *mSAR_ModeList[mModeListPtr->GetCurrentMode()]->mXmtrPtr;
}

std::unique_ptr<UtScriptClass> WsfSAR_Sensor::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptSAR_SensorClass>(aClassName, aScriptTypesPtr);
}

// ================================================================================================
// Private methods.

// ================================================================================================
//! Build and send the image message.
//! @param aSimTime The current simulation time.
//! @param aMode    The mode that created the image.
// private
void WsfSAR_Sensor::BuildAndSendImage(double aSimTime, SAR_Mode& aMode)
{
   WsfSensorResult result;
   result.mModeIndex = aMode.GetModeIndex();

   // The sensor reporting origin and the reported locations must account for difference between
   // where the sensing platform *THINKS* it is located and where it is actually located.
   double platformLocationErrorWCS[3];
   GetPlatform()->GetLocationErrorWCS(platformLocationErrorWCS);

   // Compute the scaling used to convert signal strength to pixel intensity.
   //
   // If the user did not specify scaling then the floor is set to the noise power and the range is
   // set so the clutter return will have a value of 0.3

   double clutterDBW = UtMath::LinearToDB(mAchievedCNR * aMode.mRcvrPtr->GetNoisePower()); // C = C/N * N
   double floorDBW   = UtMath::LinearToDB(aMode.mRcvrPtr->GetNoisePower());
   double rangeDBW   = (clutterDBW - floorDBW) / 0.3;
   auto   out        = ut::log::debug();
   if (mIntensityRange > 0.0)
   {
      // User specify a range above the noise floor.
      rangeDBW = UtMath::LinearToDB(mIntensityRange);
   }
   else if (mIntensityMin > 0.0)
   {
      // User specified a minimum and maximum.
      floorDBW = UtMath::LinearToDB(mIntensityMin);
      rangeDBW = UtMath::LinearToDB(mIntensityMax) - floorDBW;
   }
   if (DebugEnabled())
   {
      out << "Building and sending image.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetName();
      out.AddNote() << "Noise: " << UtMath::LinearToDB(aMode.mRcvrPtr->GetNoisePower()) << " dBw";
      out.AddNote() << "Clutter: " << clutterDBW << " dBw";
      out.AddNote() << "Floor: " << floorDBW << " dBw";
      out.AddNote() << "Range: " << rangeDBW << " dBw";
   }

   // Create the image and fill in the basic attributes

   WsfEM_Antenna& antenna = *aMode.mAntennaPtr;

   WsfImage image(this);
   image.SetMessageDataTag(GetSimulation()->CreateMessageDataTag());
   image.SetMessageLength(mMessageLength);
   double frameTime = (aMode.mOperatingMode == SAR_Mode::cOM_STRIP) ? aMode.GetFrameTime() : 0.0;
   image.SetFrameTime(frameTime);
   image.SetImageTime(aSimTime);
   image.SetImageNumber(mFrameNumber);
   image.SetStreamNumber(mStreamNumber);
   double snrLocWCS[3];
   GetPlatform()->GetLocationWCS(snrLocWCS);
   UtVec3d::Add(snrLocWCS, snrLocWCS, platformLocationErrorWCS);
   image.SetOriginatorLocationWCS(snrLocWCS);
   image.SetCenterLocationWCS(aMode.mImageCenterWCS);
   double imageWidth  = aMode.mImageWidth;
   double imageHeight = aMode.mImageHeight;
   if ((aMode.mImageWidth <= 0.0) || (aMode.mImageHeight <= 0.0))
   {
      // image_width and image_height were not specified. Compute an equivalent from the field-of-view values.
      double minAz;
      double maxAz;
      double minEl;
      double maxEl;
      antenna.GetAzimuthFieldOfView(minAz, maxAz);
      antenna.GetElevationFieldOfView(minEl, maxEl);
      if ((minEl < 0.0) && (minEl > -(UtMath::cPI_OVER_2 - mGeometry.mGrazingAngle)) && (maxEl > 0.0) &&
          (maxEl < mGeometry.mGrazingAngle))
      {
         // Using equation for chord length (not arc length)
         imageWidth         = 2.0 * mGeometry.mSlantRange * sin(0.5 * (maxAz - minAz));
         minEl              = fabs(minEl);
         double lowerHeight = mGeometry.mSlantRange * sin(minEl) / sin(UtMath::cPI - mGeometry.mGrazingAngle - minEl);
         double upperHeight = mGeometry.mSlantRange * sin(maxEl) / sin(mGeometry.mGrazingAngle - maxEl);
         imageHeight        = lowerHeight + upperHeight;
      }
   }

   image.SetWidth(imageWidth / mAchievedAzimuthResolution);
   image.SetHeight(imageHeight / mAchievedGroundRangeResolution);
   image.SetWidthResolution(mAchievedAzimuthResolution);
   image.SetHeightResolution(mAchievedGroundRangeResolution);
   image.SetNoiseLevel(aMode.mRcvrPtr->GetNoisePower());
   image.SetBackgroundLevel(mAchievedCNR * aMode.mRcvrPtr->GetNoisePower());
   image.SetMinimumLevel(UtMath::DB_ToLinear(floorDBW));
   image.SetMaximumLevel(UtMath::DB_ToLinear(floorDBW + rangeDBW));
   image.SetCollectionFactor(aMode.mCurrentAzimuthResolution / mAchievedAzimuthResolution);

   // If there are sigmas associated with sensor accuracy given in the sensor definition file,
   // compute the errors to be applied to all of the targets in the image.
   // If the mComputeRangeDopplerErrors is set to 'true' however, the Range and Doppler errors
   // will be used and will override the az, el, and range errors computed.
   result.mRcvrToTgt.mRange = mGeometry.mSlantRange;
   result.mMeasurement.SetUpdateTime(aSimTime);
   result.mMeasurement.SetLocationWCS(aMode.mImageCenterWCS);
   auto sphericalErrorsPtr                       = aMode.mSAR_ErrorModelPtr->ComputeMeasurementErrors(result);
   const wsf::SphericalMeasurementErrors& errors = *sphericalErrorsPtr;
   aMode.mSAR_ErrorModelPtr->ApplyMeasurementErrors(errors, result);

   image.SetRangeError(result.mMeasurement.GetRangeError());
   image.SetBearingError(result.mMeasurement.GetSensorAzimuthError());
   image.SetElevationError(result.mMeasurement.GetSensorElevationError());
   image.SetRangeRateError(result.mMeasurement.GetRangeRateError());

   // Propagate the 'track quality' from the mode.
   image.SetTrackQuality(aMode.GetTrackQuality());

   // Add an object to the image for each detected platform.

   for (const auto& chancePtr : mChances)
   {
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(chancePtr->mTargetIndex);
      if ((targetPtr != nullptr) && (chancePtr->mSignalSum > 0.0))
      {
         aMode.mSAR_ErrorModelPtr->ApplySAR_MeasurementErrors(errors, *chancePtr.get());

         // The location was computed using the truth location of the sensing platform.
         // Adjust the location to account for the perceived location of the platform.
         double tgtLocWCS[3];
         UtVec3d::Add(tgtLocWCS, chancePtr->mLocationWCS, platformLocationErrorWCS);

         // Put the location and pixel count in the image.
         WsfImage::Object object(targetPtr);
         object.SetLocationWCS(tgtLocWCS);

         // Compute the accumulated signal strength.
         object.SetSignalLevel(chancePtr->mSignalSum);

         // Determine the approximate size of the target in the image.
         double area       = chancePtr->mAreaSum / static_cast<double>(chancePtr->mNumberOfDetections);
         double pixelCount = area / (mAchievedAzimuthResolution * mAchievedGroundRangeResolution);
         object.SetPixelCount(pixelCount);

         // Compute the approximate intensity of pixels.
         //
         // NOTE: The signal in the 'Chance' object is the return from the ENTIRE object. We are computing the
         // intensity of a pixel, so we must divide by the number of pixels.

         double signalDBW = UtMath::LinearToDB(chancePtr->mSignalSum / std::max(pixelCount, 0.1));
         double intensity = UtMath::Limit((signalDBW - floorDBW) / rangeDBW, 0.0, 1.0);
         object.SetPixelIntensity(intensity);

         image.AddObject(object);
         if (DebugEnabled())
         {
            auto note = out.AddNote() << "Target: " << targetPtr->GetName();
            note.AddNote() << "Pixel count: " << object.GetPixelCount();
            note.AddNote() << "Intensity: " << object.GetPixelIntensity();
         }
      }
      else if ((targetPtr != nullptr) && DebugEnabled())
      {
         auto note = out.AddNote() << "Target: " << targetPtr->GetName();
         note.AddNote() << "Target not detected.";
      }
   }

   // Notify observers of image creation
   WsfObserver::ImageCreated(GetSimulation())(aSimTime, this, &image);

   // Send the message to all attached processors.

   WsfImageMessage message(GetPlatform(), image);
   SetMessageParameters(GetCurrentModeName(), message);
   SendMessage(aSimTime, message);
}

// ================================================================================================
//! Clear the sensing chance list.
// private
void WsfSAR_Sensor::ClearSensingChances()
{
   mChances.clear();
}

// ================================================================================================
//! Add a target to the sensing chances list if it meets the mode criteria.
//!
//! @param aSimTime  The current simulation time.
//! @param aTargetPtr The platform to be added.
//! @param aMode     The mode to be used.
//! @param aGeometry The current imaging geometry.
// private
void WsfSAR_Sensor::AddSensingChance(double aSimTime, const SAR_Mode& aMode, const Geometry& aGeometry, WsfPlatform* aTargetPtr)

{
   bool addTarget = false;
   if ((aTargetPtr != GetPlatform()) && AllowDetectionChancesFor(aTargetPtr))
   {
      if (aMode.mOperatingMode == SAR_Mode::cOM_SPOT)
      {
         // Select the platforms that could conceivably be in the image.
         //
         // A target that may be 'slightly' outside the area of the image is included in the list
         // in order to allow it to move into the image area while the image is being formed.
         //
         // (JAJ) I don't know how this would appear in a real system, but it was requested.

         double snrLocWCS[3];
         aMode.mAntennaPtr->GetLocationWCS(snrLocWCS);
         double snrLat;
         double snrLon;
         double snrAlt;
         aMode.mAntennaPtr->GetLocationLLA(snrLat, snrLon, snrAlt);

         // Compute the FOV limits to be used for building the potential target list.
         //
         // If the FOV limits were specified and are 'reasonably small', the FOV limits used for building
         // the potential target list will be increased to include targets that would otherwise be
         // outside the image. This allows a target to move into the image while the image is being formed.

         double minAzFOV;
         double maxAzFOV;
         aMode.mAntennaPtr->GetAzimuthFieldOfView(minAzFOV, maxAzFOV);
         double minElFOV;
         double maxElFOV;
         aMode.mAntennaPtr->GetElevationFieldOfView(minElFOV, maxElFOV);

         double azFOV = maxAzFOV - minAzFOV;
         double elFOV = maxElFOV - minElFOV;
         if ((aGeometry.mGrazingAngle > (0.01 * UtMath::cRAD_PER_DEG)) && (azFOV > (0.001 * UtMath::cRAD_PER_DEG)) &&
             (azFOV < (90.0 * UtMath::cRAD_PER_DEG)) && (elFOV > (0.001 * UtMath::cRAD_PER_DEG)) &&
             (elFOV < (90.0 * UtMath::cRAD_PER_DEG)))
         {
            // Extend the FOV to allow a target moving 50 m/s (~ 111 mi/hr) to move into the
            // image with the dwell time. Some upper and lower bound limits are applied to ensure
            // some reasonable border...

            double deltaWidth = std::max(50.0 * aMode.mDwellTime, 1000.0);
            deltaWidth        = std::min(deltaWidth, std::min(5000.0, 0.2 * aGeometry.mSlantRange));

            double azInc = deltaWidth / aGeometry.mSlantRange;
            minAzFOV -= azInc;
            maxAzFOV += azInc;

            // Extending the elevation limits is a little tricker, but we can be sloppy.

            double deltaHeight        = deltaWidth;
            double sinGraze           = sin(aGeometry.mGrazingAngle);
            double spotHeight         = elFOV * aGeometry.mSlantRange / sinGraze;
            double expandedSpotHeight = spotHeight + (2.0 * deltaHeight);
            double newElFOV           = expandedSpotHeight * sinGraze / aGeometry.mSlantRange;
            double deltaElFOV         = newElFOV - elFOV;
            minElFOV -= 0.5 * deltaElFOV;
            maxElFOV += 0.5 * deltaElFOV;
         }

         aTargetPtr->Update(aSimTime);
         double tgtLocWCS[3];
         aTargetPtr->GetLocationWCS(tgtLocWCS);
         double tgtLat;
         double tgtLon;
         double tgtAlt;
         aTargetPtr->GetLocationLLA(tgtLat, tgtLon, tgtAlt);
         double snrToTgtLocWCS[3];
         UtVec3d::Subtract(snrToTgtLocWCS, tgtLocWCS, snrLocWCS);
         double snrToTgtRange = UtVec3d::Normalize(snrToTgtLocWCS);
         if (aMode.mAntennaPtr->WithinRange(snrToTgtRange))
         {
            // Target is within range. Determine if it is within the FOV.
            double apparentSnrLocWCS[3];
            double apparentTgtLocWCS[3];
            WsfEM_Util::ComputeApparentPosition(aMode.mXmtrPtr->GetEarthRadiusMultiplier(),
                                                snrLocWCS,
                                                tgtLocWCS,
                                                snrAlt,
                                                tgtAlt,
                                                apparentSnrLocWCS,
                                                apparentTgtLocWCS);
            double snrToTgtUnitVecWCS[3];
            UtVec3d::Subtract(snrToTgtUnitVecWCS, apparentTgtLocWCS, snrLocWCS);
            UtVec3d::Normalize(snrToTgtUnitVecWCS);
            double snrToTgtAz;
            double snrToTgtEl;
            aMode.mAntennaPtr->ComputeAspect(snrToTgtUnitVecWCS, snrToTgtAz, snrToTgtEl);
            addTarget = ((snrToTgtAz >= minAzFOV) && (snrToTgtAz <= maxAzFOV) && (snrToTgtEl >= minElFOV) &&
                         (snrToTgtEl <= maxElFOV));
         }
      }
      else
      {
         addTarget = true;
      }
   }

   if (addTarget)
   {
      mChances.push_back(ut::make_unique<SAR_Chance>(aTargetPtr));
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Added target.";
         out.AddNote() << "Target: " << aTargetPtr->GetName();
      }
   }
   else if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Omitted target.";
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }
}

// ================================================================================================
//! Notify observers of detection results.
//! This method simply notifies observers using the SensorDetectionChanged and SensorTrackXXX
//! events of the detection status against a particular object. It does NOT send out a WsfTrackMessage
//! as the product of this sensor type is an image, not a track. It is left to the image processor
//! to form tracks.
// private
void WsfSAR_Sensor::NotifyObservers(double           aSimTime,
                                    SAR_Mode*        aModePtr,
                                    SAR_Chance*      aChancePtr,
                                    WsfSensorResult& aResult,
                                    bool             aDetected)
{
   if (aDetected)
   {
      aResult.mMeasurement.SetLocationWCS(aChancePtr->mLocationWCS);
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

// ================================================================================================
//! A mode change or sensor turn-on has occurred.
//!
//! This is called by 'Update' when a mode change has been detected or it is the first update following
//! a the turning on of the sensor.
//!
//! @param aSimTime  The current simulation time.
//! @param aOldModeIndex The mode index of the previously selected mode. This will be greater than the maximum
//!                      mode index if there was no previous mode.
//! @param aNewModeIndex The mode index of the newly selected mode.
// private
void WsfSAR_Sensor::ProcessModeChange(double aSimTime, size_t aOldModeIndex, size_t aNewModeIndex)
{
   // Perform any processing necessary to complete the previous mode.
   // If the mode index is invalid then this is first call following a TurnOn.

   SAR_Mode* oldModePtr = nullptr;
   SAR_Mode* newModePtr = mSAR_ModeList[aNewModeIndex];
   if (aOldModeIndex < mModeListPtr->GetCurrentMode())
   {
      oldModePtr = mSAR_ModeList[aOldModeIndex];
      switch (oldModePtr->mOperatingMode)
      {
      case SAR_Mode::cOM_SPOT:
         SpotModeEnd(aSimTime, *oldModePtr);
         break;
      case SAR_Mode::cOM_STRIP:
         StripModeEnd(aSimTime, *oldModePtr);
         break;
      default:
         break;
      }
   }

   // Perform any processing required to initiate the new mode.

   ++mTurnOffEventNumber; // Invalidate any outstanding automatic turn off events

   newModePtr->ComputeGeometry(aSimTime, mGeometry);
   if (mShowStatus)
   {
      auto out = ut::log::info() << "Show Status:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetName();
      out.AddNote() << "Selecting Mode: " << newModePtr->GetName();
      out.AddNote() << "Speed: " << GetPlatform()->GetSpeed() << " m/s";
      out.AddNote() << "Slant Range: " << mGeometry.mSlantRange << " m";
      out.AddNote() << "Squint Angle: " << mGeometry.mSquintAngle * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Depression Angle: " << mGeometry.mDepressionAngle * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Total Angle: " << mGeometry.mTotalAngle * UtMath::cDEG_PER_RAD << " deg";
   }

   switch (newModePtr->mOperatingMode)
   {
   case SAR_Mode::cOM_SPOT:
      ++mStreamNumber;
      mFrameNumber = 0;
      SpotModeBegin(aSimTime, *newModePtr, mGeometry);
      break;
   case SAR_Mode::cOM_STRIP:
      // The stream/frame number are incremented/reset only if the sensor is being turned on or if there
      // is a change from spot -> strip mode.
      //
      // It is NOT changed when changing from one strip mode to another strip mode. A change of one strip-mode to
      // another strip-mode retains the sequencing so the image/video processor can retain a filter or a track.
      if ((oldModePtr == nullptr) || (oldModePtr->mOperatingMode != SAR_Mode::cOM_STRIP))
      {
         ++mStreamNumber;
         mFrameNumber = 0;
      }
      StripModeBegin(aSimTime, *newModePtr, mGeometry);
      break;
   default:
      break;
   }

   if (mShowStatus)
   {
      double pulseWidth = newModePtr->mXmtrPtr->GetPulseWidth();
      if (newModePtr->mXmtrPtr->GetPulseWidth() > 0.0)
      {
         pulseWidth = newModePtr->mXmtrPtr->GetPulseWidth();
      }
      else if (newModePtr->mRcvrPtr->GetBandwidth() > 0.0)
      {
         pulseWidth = 1.0 / newModePtr->mRcvrPtr->GetBandwidth();
      }

      if (mShowStatus)
      {
         double dwellTime = newModePtr->mDwellTime;
         double prf       = newModePtr->mXmtrPtr->GetPulseRepetitionFrequency();
         auto   out       = ut::log::info() << "Azimuth Resolution:";
         out.AddNote() << "PRF: " << prf << " Hz";
         out.AddNote() << "Dwell Time: " << dwellTime;
         out.AddNote() << "Pulses: " << std::max(1, static_cast<int>(dwellTime * prf));
         out.AddNote() << "CNR: " << UtMath::LinearToDB(mAchievedCNR) << " dBw";
         out.AddNote() << "Range Resolution: " << mAchievedGroundRangeResolution << " m";
         out.AddNote() << "Pulse Width: " << pulseWidth * 1E+6 << " us";
         out.AddNote() << "Pulse Compression Ratio: " << newModePtr->mXmtrPtr->GetPulseCompressionRatio();
      }
   }
}

// ================================================================================================
//! Begin spot mode processing.
//!
//! @param aSimTime   The current simulation time.
//! @param aModePtr   The mode to be used.
//! @param aGeometry  The current imaging geometry.
// private
void WsfSAR_Sensor::SpotModeBegin(double aSimTime, SAR_Mode& aMode, Geometry& aGeometry)
{
   // Call the routine to predict SAR performance. This also gives us the dwell time and the PRF.

   double prf;
   double dwellTime;
   double azimuthResolution;
   double cnr;
   double minimumPRF;
   aMode.PredictPerformance(aGeometry, prf, dwellTime, azimuthResolution, cnr, minimumPRF);
   if (aMode.mComputePRF)
   {
      aMode.mXmtrPtr->SetPulseRepetitionFrequency(prf);
   }

   bool inhibitCollection = false;
   if (dwellTime > aMode.mMaximumDwellTime)
   {
      if (aMode.mInhibitSubstandardCollection)
      {
         inhibitCollection = true;
         auto out          = ut::log::info() << "Collection Inhibited: Maximum dwell time exceeded.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Sensor: " << GetName();
      }
      dwellTime         = aMode.mMaximumDwellTime;
      azimuthResolution = aMode.ComputeAzimuthResolution(aGeometry, dwellTime);
   }
   mAchievedAzimuthResolution     = azimuthResolution;
   mAchievedGroundRangeResolution = aMode.ComputeGroundRangeResolution(aGeometry);
   mAchievedCNR                   = cnr;

   aMode.mDwellTime                    = dwellTime;
   aMode.mCurrentAzimuthResolution     = mAchievedAzimuthResolution;
   aMode.mCurrentGroundRangeResolution = mAchievedGroundRangeResolution;
   if (inhibitCollection)
   {
      // Collection was inhibited due to conditions that would cause a substandard image.
      GetSimulation()->AddEvent(ut::make_unique<AutoTurnOffEvent>(aSimTime, this));
   }
   else if (aMode.mAutomaticTurnOff)
   {
      // If automatic turn-off was requested, schedule it for the end of the dwell.
      GetSimulation()->AddEvent(ut::make_unique<AutoTurnOffEvent>(aSimTime + dwellTime, this));
   }

   WsfSimulation& simulation    = *GetSimulation();
   size_t         platformCount = simulation.GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      AddSensingChance(aSimTime, aMode, mGeometry, simulation.GetPlatformEntry(platformEntry));
   }
}

// ================================================================================================
//! End spot mode processing.
//!
//! The main function is to produce the 'image' and send it to the intended recipients.
//!
//! @param aSimTime The current simulation time.
//! @param aMode    The mode being ended.
// private
void WsfSAR_Sensor::SpotModeEnd(double aSimTime, SAR_Mode& aMode)
{
   // Complete the last sample.
   Settings settings;
   settings.mModeIndex = aMode.GetModeIndex();
   SpotModeUpdate(aSimTime, settings);

   // Compute the actual dwell time
   double dwellTime = std::max(0.1, aSimTime - mModeStartTime);

   // Compute the Achieved Image Resolution & CNR.
   //
   // Note that we use the geometry from the start of the image so consistent results are generated.
   mAchievedAzimuthResolution     = aMode.ComputeAzimuthResolution(mGeometry, dwellTime);
   mAchievedGroundRangeResolution = aMode.ComputeGroundRangeResolution(mGeometry);
   mAchievedCNR =
      aMode.ComputeCNR(mGeometry, aMode.mXmtrPtr->GetPulseRepetitionFrequency(), dwellTime, mAchievedAzimuthResolution);

   // If the image is 'substandard' (wasn't collected for a long enough time) and the user requested rescaling of
   // substandard images, the ground range resolution will be adjusted so as to maintain the same pixel aspect
   // ratio as the requested aspect ratio.

   if ((mAchievedAzimuthResolution > aMode.mCurrentAzimuthResolution) && mRescaleSubstandardImage)
   {
      double scaleFactor             = mAchievedAzimuthResolution / aMode.mCurrentAzimuthResolution;
      mAchievedGroundRangeResolution = aMode.mCurrentGroundRangeResolution * scaleFactor;
   }

   if (mShowStatus)
   {
      auto out = ut::log::info() << "Spot SAR complete.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetName();
      out.AddNote() << "Mode: " << aMode.GetName();
      out.AddNote() << "Dwell Time: " << dwellTime;
      out.AddNote() << "CNR: " << UtMath::LinearToDB(mAchievedCNR) << " dBw";
      out.AddNote() << "Achieved Range Resolution: " << mAchievedGroundRangeResolution << " m";
   }

   // Send the image...
   mFrameNumber = 0;
   BuildAndSendImage(aSimTime, aMode);
}

// ================================================================================================
//! Perform the 'Update' operation for spot mode.
//!
//! @param aSimTime  The current simulation time.
//! @param aSettings The desired sensor settings.
// private
void WsfSAR_Sensor::SpotModeUpdate(double aSimTime, Settings& aSettings)
{
   double dt = aSimTime - mLastUpdateTime;
   // Check all the potential targets to see if they are still in the image

   SAR_Mode* modePtr     = mSAR_ModeList[aSettings.mModeIndex];
   aSettings.mRequiredPd = WsfSensor::GetRequiredPd(modePtr);
   WsfSensorResult result;
   for (const auto& chancePtr : mChances)
   {
      bool         detected  = false;
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(chancePtr->mTargetIndex);
      if (targetPtr != nullptr)
      {
         if (modePtr->WithinDetectionRange(aSimTime, targetPtr))
         {
            // Bypass our derived class implementation of AttemptToDetect when called from Update.
            // This directs the request to the proper mode.
            detected = WsfSensor::AttemptToDetect(aSimTime, targetPtr, aSettings, result);
         }
      }

      if (detected && (dt > 1.0E-6))
      {
         targetPtr->GetLocationWCS(chancePtr->mLocationWCS);
         double signal = result.mSignalToNoise * modePtr->mRcvrPtr->GetNoisePower(); // S/N * N = S
         double prf    = modePtr->mXmtrPtr->GetPulseRepetitionFrequency();
         if (prf > 0.0)
         {
            // The signal computed above considers the number of pulses collected as being the number collected
            // during the PROJECTED time required to collect an image of the desired resolution. We want the
            // signal received only during the current interval.
            signal *= (dt / modePtr->mDwellTime);
         }
         chancePtr->mSignalSum += signal;
         chancePtr->mAreaSum += result.mOpticalSig;
         chancePtr->mComputedRangeError   = result.mMeasurement.GetRangeError();
         chancePtr->mComputedDopplerError = result.mMeasurement.GetRangeRateError();
         ++chancePtr->mNumberOfDetections;
      }
      NotifyObservers(aSimTime, modePtr, chancePtr.get(), result, detected);
   }
}

// ================================================================================================
//! Begin strip mode processing.
//!
//! @param aSimTime  The current simulation time.
//! @param aMode     The mode to be used.
//! @param aGeometry The current imaging geometry.
// private
void WsfSAR_Sensor::StripModeBegin(double aSimTime, SAR_Mode& aMode, Geometry& aGeometry)
{
   ClearSensingChances();
   WsfSimulation& simulation = *GetSimulation();
   size_t         pCount     = simulation.GetPlatformCount();
   for (size_t pEntry = 0; pEntry < pCount; ++pEntry)
   {
      AddSensingChance(aSimTime, aMode, aGeometry, simulation.GetPlatformEntry(pEntry));
   }

   // Predict the performance, which will get the PRF, dwell time and CNR
   double prf;
   double dwellTime;
   double azimuthResolution;
   double cnr;
   double minimumPRF;
   aMode.PredictPerformance(aGeometry, prf, dwellTime, azimuthResolution, cnr, minimumPRF);

   if (aMode.mComputePRF)
   {
      aMode.mXmtrPtr->SetPulseRepetitionFrequency(prf);
   }
   mAchievedAzimuthResolution     = azimuthResolution;
   mAchievedGroundRangeResolution = aMode.ComputeGroundRangeResolution(aGeometry);
   mAchievedCNR                   = cnr;

   aMode.mDwellTime                    = std::min(dwellTime, aMode.mMaximumDwellTime);
   aMode.mCurrentAzimuthResolution     = mAchievedAzimuthResolution;
   aMode.mCurrentGroundRangeResolution = mAchievedGroundRangeResolution;

   // Issue a warning if the resolution is less than the theoretical minimum of the physical
   // aperture size / 2 (Stimson, pg 415, d_a_min). A very conservative estimate of aperture size
   // is used based on the beamwidth. (The 0.8 term is for added safety).

   double wavelength = UtMath::cLIGHT_SPEED / aMode.mXmtrPtr->GetFrequency();
   double beamwidth =
      std::min(aMode.mXmtrPtr->GetAzimuthBeamwidth(0.0, 0.0), aMode.mXmtrPtr->GetElevationBeamwidth(0.0, 0.0));
   double d_a_min = 0.8 * wavelength / beamwidth;
   if (aMode.mCurrentAzimuthResolution < d_a_min)
   {
      auto out = ut::log::warning() << "Requested/computed resolution is less than approximate minimum.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetName();
      out.AddNote() << "Minimum: " << d_a_min << " m";
   }
}

// ================================================================================================
//! End strip mode processing.
//! @param aSimTime   The current simulation time.
//! @param aMode    The mode being terminated.
//! @param aModeIndex The mode index of the strip mode.
// private
void WsfSAR_Sensor::StripModeEnd(double aSimTime, SAR_Mode& aMode)
{
   ClearSensingChances();
}

// ================================================================================================
//! Perform the 'Update' operation for strip mode.
//! @param aSimTime  The current simulation time.
//! @param aSettings The desired sensor settings.
// private
void WsfSAR_Sensor::StripModeUpdate(double aSimTime, Settings& aSettings)
{
   SAR_Mode* modePtr     = mSAR_ModeList[aSettings.mModeIndex];
   aSettings.mRequiredPd = WsfSensor::GetRequiredPd(modePtr);

   double tgtLocWCS[3];
   double tgtVelWCS[3];
   double snrLocWCS[3];
   GetLocationWCS(snrLocWCS);
   double maxRangeSq = modePtr->mAntennaPtr->GetMaximumRange();
   if (maxRangeSq < 1.0E+16)
   {
      maxRangeSq = maxRangeSq * maxRangeSq;
   }

   WsfSensorResult result;
   for (const auto& chancePtr : mChances)
   {
      chancePtr->mSignalSum          = 0.0;
      chancePtr->mAreaSum            = 0.0;
      chancePtr->mNumberOfDetections = 0;
      bool         detected          = false;
      WsfPlatform* targetPtr         = GetSimulation()->GetPlatformByIndex(chancePtr->mTargetIndex);
      if (targetPtr != nullptr)
      {
         // Perform a quick culling check to see if the target is even potentially within range...
         double deltaT = std::max(0.0, aSimTime - targetPtr->GetLastUpdateTime());
         if (deltaT > 5.0)
         {
            targetPtr->Update(aSimTime);
         }

         targetPtr->GetVelocityWCS(tgtVelWCS);
         targetPtr->GetLocationWCS(tgtLocWCS);
         UtVec3d::AddProduct(tgtLocWCS, tgtLocWCS, deltaT, tgtVelWCS);

         double relTgtLocWCS[3];
         UtVec3d::Subtract(relTgtLocWCS, tgtLocWCS, snrLocWCS);
         double rangeSq = UtVec3d::MagnitudeSquared(relTgtLocWCS);
         if (rangeSq <= maxRangeSq)
         {
            // Bypass our derived class implementation of AttemptToDetect when called from Update.
            detected = WsfSensor::AttemptToDetect(aSimTime, targetPtr, aSettings, result);
            if (detected)
            {
               targetPtr->GetLocationWCS(chancePtr->mLocationWCS);                         // TODO check this
               double signal = result.mSignalToNoise * modePtr->mRcvrPtr->GetNoisePower(); // S/N * N = S
               chancePtr->mSignalSum += signal;
               chancePtr->mAreaSum += result.mOpticalSig;
               chancePtr->mComputedRangeError   = result.mMeasurement.GetRangeError();
               chancePtr->mComputedDopplerError = result.mMeasurement.GetRangeRateError();
               ++chancePtr->mNumberOfDetections;
            }
         }
      }
      NotifyObservers(aSimTime, modePtr, chancePtr.get(), result, detected);
   }

   ++mFrameNumber;
   BuildAndSendImage(aSimTime, *modePtr);
}

// ================================================================================================
//                         Nested class WsfSAR_Sensor::SAR_Chance.
// ================================================================================================

// ================================================================================================
WsfSAR_Sensor::SAR_Chance::SAR_Chance(WsfPlatform* aTargetPtr)
   : mTrackPtr(nullptr)
   , mSignalSum(0.0)
   , mAreaSum(0.0)
   , mComputedRangeError(0.0)
   , mComputedDopplerError(0.0)
   , mTargetIndex(aTargetPtr->GetIndex())
   , mNumberOfDetections(0U)
   , mNowDetecting(false)
{
   UtVec3d::Set(mLocationWCS, 0.0);
}

// ================================================================================================
WsfSAR_Sensor::SAR_Chance::~SAR_Chance()
{
   delete mTrackPtr;
}

// ================================================================================================
//                            Nested class WsfSAR_Sensor::SAR_Mode.
// ================================================================================================

// ================================================================================================
WsfSAR_Sensor::SAR_Mode::SAR_Mode()
   : WsfSensorMode()
   , WsfSensorBeam()
   , mSAR_SensorPtr(nullptr)
   , mOperatingMode(cOM_SPOT)
   , mAntennaPtr(new WsfEM_Antenna)
   , mXmtrPtr(new WsfEM_Xmtr(WsfEM_Xmtr::cXF_SENSOR, mAntennaPtr.get()))
   , mRcvrPtr(new WsfEM_Rcvr(WsfEM_Rcvr::cRF_SENSOR, mAntennaPtr.get()))
   , mKa(1.0)
   , mKd(1.0)
   , mResolution(0.0)
   , mDwellTime(0.0)
   , mMaximumDwellTime(999.0)
   , mMinimumCNR(1.0)
   , mMaxDetectableSpeed(2.0)
   , mCurrentAzimuthResolution(0.0)
   , mCurrentGroundRangeResolution(0.0)
   , mAngularResolution(0.0)
   , mIntegrationGain(1.0)
   , mAdjustmentFactor(1.0)
   , mOneM2DetectRange(0.0)
   , mComputeDwellTime(false)
   , mComputePRF(false)
   , mAutomaticTurnOff(false)
   , mInhibitSubstandardCollection(false)
   , mImageHeight(0.0)
   , mImageWidth(0.0)
{
   SetBeamOwner(this);
   SetErrorModel(ut::make_unique<SAR_ErrorModel>());
   mSAR_ErrorModelPtr = static_cast<SAR_ErrorModel*>(mErrorModelPtr.get());

   UtVec3d::Set(mImageCenterWCS, 0.0);

   // Indicate the receiver is 'linked' with the transmitter.
   mXmtrPtr->SetLinkedReceiver(GetEM_Rcvr());
}

// ================================================================================================
WsfSAR_Sensor::SAR_Mode::SAR_Mode(const SAR_Mode& aSrc)
   : WsfSensorMode(aSrc)
   , WsfSensorBeam(aSrc)
   , mSAR_SensorPtr(nullptr)
   , mOperatingMode(aSrc.mOperatingMode)
   , mAntennaPtr(new WsfEM_Antenna(*aSrc.mAntennaPtr))
   , mXmtrPtr(new WsfEM_Xmtr(*aSrc.mXmtrPtr, mAntennaPtr.get()))
   , mRcvrPtr(new WsfEM_Rcvr(*aSrc.mRcvrPtr, mAntennaPtr.get()))
   , mKa(aSrc.mKa)
   , mKd(aSrc.mKd)
   , mResolution(aSrc.mResolution)
   , mDwellTime(aSrc.mDwellTime)
   , mMaximumDwellTime(aSrc.mMaximumDwellTime)
   , mMinimumCNR(aSrc.mMinimumCNR)
   , mMaxDetectableSpeed(aSrc.mMaxDetectableSpeed)
   , mCurrentAzimuthResolution(aSrc.mCurrentAzimuthResolution)
   , mCurrentGroundRangeResolution(aSrc.mCurrentGroundRangeResolution)
   , mAngularResolution(aSrc.mAngularResolution)
   , mIntegrationGain(aSrc.mIntegrationGain)
   , mAdjustmentFactor(aSrc.mAdjustmentFactor)
   , mOneM2DetectRange(aSrc.mOneM2DetectRange)
   , mComputeDwellTime(aSrc.mComputeDwellTime)
   , mComputePRF(aSrc.mComputePRF)
   , mAutomaticTurnOff(aSrc.mAutomaticTurnOff)
   , mInhibitSubstandardCollection(aSrc.mInhibitSubstandardCollection)
   , mImageHeight(aSrc.mImageHeight)
   , mImageWidth(aSrc.mImageWidth)
{
   UtVec3d::Set(mImageCenterWCS, aSrc.mImageCenterWCS);
   mSAR_ErrorModelPtr = static_cast<SAR_ErrorModel*>(mErrorModelPtr.get());

   // Indicate the receiver is 'linked' with the transmitter.
   mXmtrPtr->SetLinkedReceiver(GetEM_Rcvr());
}

// ================================================================================================
// virtual
WsfMode* WsfSAR_Sensor::SAR_Mode::Clone() const
{
   return new SAR_Mode(*this);
}

// ================================================================================================
// virtual
void WsfSAR_Sensor::SAR_Mode::Deselect(double aSimTime)
{
   mRcvrPtr->Deactivate();
   mXmtrPtr->Deactivate();
}

// ================================================================================================
// virtual
void WsfSAR_Sensor::SAR_Mode::Select(double aSimTime)
{
   mRcvrPtr->Activate();
   mXmtrPtr->Activate();
   ComputeUpdateInterval(aSimTime);
}

// ================================================================================================
// virtual
bool WsfSAR_Sensor::SAR_Mode::Initialize(double aSimTime)
{
   mSAR_SensorPtr = static_cast<WsfSAR_Sensor*>(mSensorPtr);

   // If 'sar_constraint_plotting true', disable the field-of-view limits.
   if (mSAR_SensorPtr->mSAR_ConstraintPlotting)
   {
      mAntennaPtr->SetRangeLimits(0.0, 1.0E+10);
      mAntennaPtr->SetAltitudeLimits(-1.0E+10, 1.0E+10);
      mAntennaPtr->SetAzimuthFieldOfView(-UtMath::cPI, UtMath::cPI);
      mAntennaPtr->SetElevationFieldOfView(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      mAntennaPtr->SetEBS_AzCosSteeringLimit(0.0); // cos(pi/2)
      mAntennaPtr->SetEBS_ElCosSteeringLimit(0.0); // cos(pi/2)
   }

   // For compatibility, if automatic turnoff was selected at the sensor level, propagate it to the mode.
   if (mSAR_SensorPtr->mAutomaticTurnOff)
   {
      mAutomaticTurnOff = true;
   }

   bool ok = WsfSensorMode::Initialize(aSimTime);

   // If a pulse width wasn't specified, we'll try to compute one when the sensor is used.
   mComputePRF = false;
   if (mXmtrPtr->GetPulseRepetitionFrequency() == 0.0)
   {
      // Force a PRF to prevent initialization failure if pulse_width is specified
      mXmtrPtr->SetPulseRepetitionFrequency(1000.0);
      mComputePRF = true;
   }

   // If a frame time was not specified, use the sensor update_interval.
   // If that is zero, a default will get selected in ComputeUpdateInterval.
   if (mFrameTime <= 0.0)
   {
      mFrameTime = mSensorPtr->GetUpdateInterval();
   }

   mXmtrPtr->SetMode(this); // Identify the xmtr with the mode
   mRcvrPtr->SetMode(this); // Identify the rcvr with the mode
   mXmtrPtr->SetAntenna(mAntennaPtr.get());
   mRcvrPtr->SetAntenna(mAntennaPtr.get());
   ok &= mAntennaPtr->Initialize(mSensorPtr);
   ok &= mXmtrPtr->Initialize(*GetSimulation());
   ok &= mRcvrPtr->Initialize(*GetSimulation());

   // In case the bandwidth was not set for the noise calculation,
   // set the bandwidth using the pulsewidth of the linked transmitter.
   mRcvrPtr->UpdateNoisePower(mXmtrPtr->GetPulseWidth());

   if (mComputeDwellTime)
   {
      // Compute dwell time from desired resolution
      if (mResolution <= 0.0)
      {
         auto out = ut::log::error() << "'resolution' must be specified if 'dwell_time' is not specified.";
         out.AddNote() << "Sensor: " << GetSensor()->GetType();
         out.AddNote() << "Sensor Type: " << GetSensor()->GetType();
         out.AddNote() << "Mode: " << GetName();
         ok = false;
      }
   }
   else
   {
      // Compute resolution from dwell time
      if (mDwellTime <= 0.0)
      {
         auto out = ut::log::error() << "'dwell_time' must be specified if 'resolution' is not specified.";
         out.AddNote() << "Sensor: " << GetSensor()->GetType();
         out.AddNote() << "Sensor Type: " << GetSensor()->GetType();
         out.AddNote() << "Mode: " << GetName();
         ok = false;
      }
   }

   if (ok)
   {
      if (mSensorPtr->ShowCalibrationData())
      {
         auto out = ut::log::info() << "Calibration Data:";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Senor: " << mSensorPtr->GetName();
         out.AddNote() << "Mode: " << GetName();
      }
      Calibrate();
   }

   // Set the debug flag
   mRcvrPtr->SetDebugEnabled(mSensorPtr->DebugEnabled());
   mXmtrPtr->SetDebugEnabled(mSensorPtr->DebugEnabled());

   return ok;
}

// ================================================================================================
// virtual
bool WsfSAR_Sensor::SAR_Mode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (mAntennaPtr->ProcessInput(aInput) || mXmtrPtr->ProcessInputBlock(aInput) || mRcvrPtr->ProcessInputBlock(aInput))
   {
   }
   else if (command == "operating_mode")
   {
      std::string operatingMode;
      aInput.ReadValue(operatingMode);
      if ((operatingMode == "spot") || (operatingMode == "spotlight"))
      {
         mOperatingMode = cOM_SPOT;
      }
      else if ((operatingMode == "strip") || (operatingMode == "stripmap"))
      {
         mOperatingMode = cOM_STRIP;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid value for operating_mode: " + operatingMode);
      }
   }
   else if (command == "automatic_turn_off")
   {
      mAutomaticTurnOff = true;
   }
   else if (command == "doppler_filter_broadening_factor")
   {
      aInput.ReadValue(mKa);
      aInput.ValueGreaterOrEqual(mKa, 1.0);
   }
   else if ((command == "doppler_foldover_margin_factor") || (command == "doppler_overcollect_ratio"))
   {
      aInput.ReadValue(mKd);
      aInput.ValueGreaterOrEqual(mKd, 1.0);
   }
   else if ((command == "dwell_time") || (command == "image_formation_time")) // DEPRECATED
   {
      aInput.ReadValueOfType(mDwellTime, UtInput::cTIME);
      aInput.ValueGreater(mDwellTime, 0.0);
      mComputeDwellTime = false;
   }
   else if ((command == "resolution") || (command == "desired_resolution") || // DEPRECATED
            (command == "desired_image_resolution"))                          // DEPRECATED
   {
      aInput.ReadValueOfType(mResolution, UtInput::cLENGTH);
      aInput.ValueGreater(mResolution, 0.0);
      mComputeDwellTime = true;
   }
   else if ((command == "maximum_dwell_time") || (command == "maximum_image_formation_time")) // DEPRECATED
   {
      aInput.ReadValueOfType(mMaximumDwellTime, UtInput::cTIME);
      aInput.ValueGreater(mMaximumDwellTime, 0.0);
   }
   else if (command == "minimum_clutter_to_noise_ratio")
   {
      aInput.ReadValueOfType(mMinimumCNR, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(mMinimumCNR, 1.0);
   }
   else if (command == "maximum_detectable_speed")
   {
      aInput.ReadValueOfType(mMaxDetectableSpeed, UtInput::cSPEED);
      aInput.ValueGreater(mMaxDetectableSpeed, 0.0);
   }
   else if (command == "inhibit_substandard_collection")
   {
      aInput.ReadValue(mInhibitSubstandardCollection);
   }
   else if (command == "angular_resolution") // DEPRECATED
   {
      auto out = ut::log::warning() << "WSF_SAR_SENSOR: angular_resolution has been deprecated.";
      out.AddNote() << "Use the 'resolution' or 'dwell_time' commands.";
      aInput.ReadValueOfType(mAngularResolution, UtInput::cANGLE);
      aInput.ValueGreater(mAngularResolution, 0.0);
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
   }
   else if (command == "integration_gain")
   {
      aInput.ReadValueOfType(mIntegrationGain, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(mIntegrationGain, 1.0);
   }
   else if (command == "adjustment_factor")
   {
      aInput.ReadValueOfType(mAdjustmentFactor, UtInput::cRATIO);
      aInput.ValueGreater(mAdjustmentFactor, 0.0);
   }
   else if (command == "one_m2_detect_range")
   {
      aInput.ReadValueOfType(mOneM2DetectRange, UtInput::cLENGTH);
      aInput.ValueGreater(mOneM2DetectRange, 0.0);
   }
   else if (command == "image_height")
   {
      aInput.ReadValueOfType(mImageHeight, UtInput::cLENGTH);
      aInput.ValueGreater(mImageHeight, 0.0);
      aInput.ValueLessOrEqual(mImageHeight, 9999.0 * 1000.0);
   }
   else if (command == "image_width")
   {
      aInput.ReadValueOfType(mImageWidth, UtInput::cLENGTH);
      aInput.ValueGreater(mImageWidth, 0.0);
      aInput.ValueLessOrEqual(mImageWidth, 9999.0 * 1000.0);
   }
   else if (command == "image_size")
   {
      aInput.ReadValueOfType(mImageWidth, UtInput::cLENGTH);
      aInput.ValueGreater(mImageWidth, 0.0);
      aInput.ValueLessOrEqual(mImageWidth, 9999.0 * 1000.0);
      mImageHeight = mImageWidth;
   }
   else
   {
      myCommand = WsfSensorMode::ProcessInput(aInput) || WsfSensorBeam::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
// virtual
bool WsfSAR_Sensor::SAR_Mode::AttemptToDetect(double           aSimTime,
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
      auto out = ut::log::debug() << "Attempting to detect target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   // Determine if concealed (like in a building).
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= WsfSensorResult::cCONCEALMENT;
      if (aTargetPtr->GetConcealmentFactor() > 0.99F)
      {
         // We can't detect if it's in a building (or something like that)
         aResult.mFailedStatus |= WsfSensorResult::cCONCEALMENT;
         // Must have object pointers so event_output and debug output show locations.
         aResult.BeginGenericInteraction(mXmtrPtr.get(), aTargetPtr, mRcvrPtr.get());
      }
   }

   // We can't detect things that are moving...
   if (aResult.mFailedStatus == 0)
   {
      aResult.mCheckedStatus |= WsfSensorResult::cVELOCITY_LIMITS;
      double velocity[3];
      aTargetPtr->GetVelocityWCS(velocity);
      double speed = UtVec3d::Magnitude(velocity);
      if (speed > mMaxDetectableSpeed)
      {
         aResult.mFailedStatus |= WsfSensorResult::cVELOCITY_LIMITS;
         aResult.BeginGenericInteraction(mXmtrPtr.get(), aTargetPtr, mRcvrPtr.get());
      }
   }

   // If we're good up to this point then compute signal-to-noise and check for detection.

   if ((aResult.mFailedStatus == 0) && (aResult.BeginTwoWayInteraction(mXmtrPtr.get(), aTargetPtr, mRcvrPtr.get()) == 0))
   {
      // Set the position of the antenna beam(s).
      aResult.SetTransmitterBeamPosition();
      aResult.SetReceiverBeamPosition();

      // Determine the radar cross section of the target.
      aResult.ComputeRadarSigAzEl();
      aResult.mRadarSig = WsfRadarSignature::GetValue(aTargetPtr,
                                                      mXmtrPtr.get(),
                                                      mRcvrPtr.get(),
                                                      aResult.mRadarSigAz,
                                                      aResult.mRadarSigEl,
                                                      aResult.mRadarSigAz,
                                                      aResult.mRadarSigEl);

      // Calculate the signal return.
      aResult.ComputeRF_TwoWayPower(aResult.mRadarSig);

      // Account for the gain due to pulse compression.
      aResult.mRcvdPower *= mXmtrPtr->GetPulseCompressionRatio();

      // Integrate the pulses over the desired dwell time.
      int numberOfPulsesIntegrated = std::max(1, static_cast<int>(mDwellTime * mXmtrPtr->GetPulseRepetitionFrequency()));
      aResult.mRcvdPower *= numberOfPulsesIntegrated;

      // Account for integration gain
      aResult.mRcvdPower *= mIntegrationGain;

      // Allow for other general post-reception adjustments (for compatibility with SUPPRESSOR OPERATING-LOSSES).
      aResult.mRcvdPower *= mAdjustmentFactor;

      // Compute component effects.
      WsfSensorComponent::AttemptToDetect(*GetSensor(), aSimTime, aResult);

      // Compute the total effective signal-to-noise ratio at the output of the receiver.
      aResult.mSignalToNoise =
         mRcvrPtr->ComputeSignalToNoise(aResult.mRcvdPower, aResult.mClutterPower, aResult.mInterferencePower);

      aResult.mPd = 1.0;

      // Apply component effects
      aResult.mPd *= (1.0 - aResult.mInterferenceFactor);

      if (aResult.mSignalToNoise < mRcvrPtr->GetDetectionThreshold())
      {
         aResult.mPd = 0.0;
      }

      // If the required Pd is exceeded then compute the APPROXIMATE size of the object in the image.
      if (aResult.mPd >= aSettings.mRequiredPd)
      {
         // Get the projected area when viewed from the zenith.

         // aResult.ComputeOpticalSigAzEl();                // Old mechanism used the projected area along the sight line
         aResult.mOpticalSigAz = 0.0;
         aResult.mOpticalSigEl = UtMath::cPI_OVER_2;
         aResult.mOpticalSig =
            WsfOpticalSignature::GetValue(aSimTime, aTargetPtr, aResult.mOpticalSigAz, aResult.mOpticalSigEl);
         aResult.mPixelCount = aResult.mOpticalSig / (mCurrentAzimuthResolution * mCurrentGroundRangeResolution);

         // Compute the standard measurement errors. The range/Doppler errors may get overridden below.
         mErrorModelPtr->ApplyMeasurementErrors(aResult);

         // Terrain masking is checked only when we have a detection.
         // Check for range/doppler foldover
         // If that is OK, check for terrain masking
         if (!WithinUnambiguousRange(aResult))
         {
            detected = false;
            aResult.mFailedStatus |= WsfSensorResult::cXMTR_RANGE_LIMITS;
         }
         else
         {
            detected = !aResult.MaskedByTerrain();
         }
      }
   }

   detected &= WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);
   detected &= GetSensor()->ScriptAllowDetection(aSimTime, aTargetPtr, aResult);
   if (GetSensor()->DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "Sensor Data:";
      aResult.Print(logDebug);
   }
   GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);
   return detected;
}

// ================================================================================================
//! SAR Target Location Error Computations
//! Equations taken from "Air-to-Ground Radar Modes" presentation that is part of
//! Fundamentals of Radar course taught internally at Boeing
void WsfSAR_Sensor::SAR_Mode::SAR_ErrorModel::ApplyRangeDopplerErrors(WsfEM_Antenna* aAntennaPtr,
                                                                      WsfPlatform*   aTargetPtr,
                                                                      SAR_Chance*    aChancePtr)
{
   // If the errors are zero, return quickly
   if ((aChancePtr->mComputedRangeError == 0.0) && (aChancePtr->mComputedDopplerError == 0.0))
   {
      return;
   }

   // Compute the SAR TLEs
   double rangeDopplerErrors[2] = {aChancePtr->mComputedRangeError, aChancePtr->mComputedDopplerError};

   // Compute Squint Angle
   // Angle between the platform ground track vector and the target being imaged
   double slantRange;
   double squintAngle = static_cast<WsfSAR_Sensor::SAR_Mode*>(GetSensorMode())->ComputeSquintAngle(aTargetPtr, slantRange);

   // Compute ground-range & grazing angle wrt the antenna
   double tgtLocWCS[3];
   aTargetPtr->GetLocationWCS(tgtLocWCS);

   double tgtLocNED[3];
   aAntennaPtr->ConvertWCS_ToNED(tgtLocWCS, tgtLocNED);
   GetSensorMode()->GetPlatform()->ConvertWCSToNED(tgtLocWCS, tgtLocNED);

   double hRange = sqrt(tgtLocNED[0] * tgtLocNED[0] + tgtLocNED[1] * tgtLocNED[1]);

   // Compute the grazing angle
   double grazingAngle = 0.0;
   if (hRange != 0.0)
   {
      grazingAngle = atan2(tgtLocNED[2], hRange);
   }
   else
   {
      grazingAngle = UtMath::cPI_OVER_2;
      if (tgtLocNED[2] > 0.0)
      {
         grazingAngle = -UtMath::cPI_OVER_2;
      }
   }

   double platVel[3];
   GetSensorMode()->GetPlatform()->GetVelocityNED(platVel);
   double velMag = UtVec3d::Magnitude(platVel);

   double term1XRV = pow((hRange * mSAR_Errors[3] / (tan(squintAngle) * velMag)), 2);
   double term2XRV = pow(hRange * mSAR_Errors[4] / velMag, 2);
   double term3XRV = pow(tgtLocNED[2] * mSAR_Errors[5] / (sin(squintAngle) * velMag), 2);
   double term4XRV = pow(-tan(grazingAngle) / tan(squintAngle) * mSAR_Errors[2], 2);
   double term5XRV = pow(sin(grazingAngle) * tan(grazingAngle) / tan(squintAngle) * rangeDopplerErrors[0], 2);
   double term6XRV = pow(rangeDopplerErrors[1], 2) + pow(mSAR_Errors[0], 2) +
                     pow(mSAR_Errors[6], 2) * pow(velMag, 2) * pow(sin(squintAngle), 2);

   double stdDevXR = sqrt(term1XRV + term2XRV + term3XRV + term4XRV + term5XRV + term6XRV);

   double term1DRV = pow(rangeDopplerErrors[0] / cos(grazingAngle), 2);
   double term2DRV = pow(tan(grazingAngle) * mSAR_Errors[2], 2);
   double term3DRV = pow(mSAR_Errors[1], 2) + pow(mSAR_Errors[6], 2) * pow(velMag, 2) * pow(cos(squintAngle), 2);

   double stdDevDR = sqrt(term1DRV + term2DRV + term3DRV);

   // Compute an error for downrange and cross-range and add that to the NED Vector
   ut::Random& random  = GetSensorMode()->GetSensor()->GetRandom();
   double      drError = stdDevDR * random.Gaussian();
   double      xrError = stdDevXR * random.Gaussian();

   // Rotate the NED Vector to a Local cross-range, downrange, up vector space
   // This is done by rotating through the squintAngle
   double JRU[3] = {0.0, 0.0, 0.0};
   JRU[0]        = tgtLocNED[0] * sin(squintAngle) - tgtLocNED[1] * cos(squintAngle);
   JRU[1]        = tgtLocNED[0] * cos(squintAngle) + tgtLocNED[1] * sin(squintAngle);
   JRU[2]        = tgtLocNED[2];

   // Add cross-range error
   JRU[0] += xrError;

   // Create a tempDR - Used to make sure we retain the proper sign
   double tempDR = fabs(JRU[1]);

   // Add the downrange error to the temp
   tempDR += drError;

   // Multiply against the original/abs(original) to get the sign correct
   tempDR *= JRU[1] / fabs(JRU[1]);
   JRU[1] = tempDR;

   // Rotate Back through the squintAngle to get back into NED
   tgtLocNED[0] = -sin(squintAngle) * JRU[0] + cos(squintAngle) * JRU[1];
   tgtLocNED[1] = cos(squintAngle) * JRU[0] + sin(squintAngle) * JRU[1];
   tgtLocNED[2] = JRU[2];

   // Convert back to WCS and overwrite the chancePtr
   aAntennaPtr->ConvertNED_ToWCS(tgtLocNED, aChancePtr->mLocationWCS);
}

// ================================================================================================
//! Compute the geometry parameters for a imaging or prediction request.
void WsfSAR_Sensor::SAR_Mode::ComputeGeometry(double aSimTime, Geometry& aGeometry)
{
   // Compute the sensor pointing angles relative to the velocity vector.
   WsfSensor*   sensorPtr   = GetSensor();
   WsfPlatform* platformPtr = sensorPtr->GetPlatform();

   double snrToTgtLocPCS[3] = {1.0, 0.0, 0.0};
   double snrToTgtLocWCS[3];
   sensorPtr->ConvertPCSVectorToWCS(snrToTgtLocWCS, snrToTgtLocPCS);

   // If the sensor is cued (as it normally is) and if the cue is outside the slew limits, then the
   // PCS location will be clipped by the cue limits. In this routine is desirable to have the
   // unclipped limits. In the case were clipping occurs, a more compute intensive process is used.
   // This should really only affect sensor_plot because a real simulation would probably not be
   // setting the cue outside the limits.

   if ((sensorPtr->GetSlewState() & WsfSensor::cAT_LIMIT_MASK) != 0)
   {
      // Capture the current cue/slew limits
      double minAzSlew = sensorPtr->GetMinAzSlew();
      double maxAzSlew = sensorPtr->GetMaxAzSlew();
      double minElSlew = sensorPtr->GetMinElSlew();
      double maxElSlew = sensorPtr->GetMaxElSlew();
      double minAzCue  = sensorPtr->GetMinAzCue();
      double maxAzCue  = sensorPtr->GetMaxAzCue();
      double minElCue  = sensorPtr->GetMinElCue();
      double maxElCue  = sensorPtr->GetMaxElCue();

      // Set the cue/slew limits to unlimited and recompute the pointing vector to WCS
      sensorPtr->SetAzSlewLimits(-UtMath::cPI, UtMath::cPI);
      sensorPtr->SetElSlewLimits(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      sensorPtr->SetAzCueLimits(-UtMath::cPI, UtMath::cPI);
      sensorPtr->SetElCueLimits(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      sensorPtr->UpdatePosition(aSimTime);
      sensorPtr->ConvertPCSVectorToWCS(snrToTgtLocWCS, snrToTgtLocPCS);

      // Reset the cue/slew limits to the saved values and reset the orientation.
      sensorPtr->SetAzSlewLimits(minAzSlew, maxAzSlew);
      sensorPtr->SetElSlewLimits(minElSlew, maxElSlew);
      sensorPtr->SetAzCueLimits(minAzCue, maxAzCue);
      sensorPtr->SetElCueLimits(minElCue, maxElCue);
      sensorPtr->UpdatePosition(aSimTime);
   }

   double snrToTgtLocNED[3];
   platformPtr->ConvertWCSVectorToNED(snrToTgtLocNED, snrToTgtLocWCS);

   // Compute the depression angle (positive down!!!)
   double snrToTgtLocNE       = sqrt((snrToTgtLocNED[0] * snrToTgtLocNED[0]) + (snrToTgtLocNED[1] * snrToTgtLocNED[1]));
   aGeometry.mDepressionAngle = atan2(snrToTgtLocNED[2], snrToTgtLocNE);

   // Compute the slant range to the center of interest and the grazing angle at the center of interest.
   double lat;
   double lon;
   sensorPtr->GetLocationLLA(lat, lon, aGeometry.mAltitude);
   aGeometry.mSlantRange = ComputeSlantRange(aGeometry.mAltitude, aGeometry.mDepressionAngle, aGeometry.mGrazingAngle);

   // Calculate antenna FOV if image size was specified
   ComputeFOV(aGeometry);

   // Compute the total angle between the platform velocity vector and the beam pointing vector.
   double snrVelNED[3];
   platformPtr->GetVelocityNED(snrVelNED);
   double dot             = UtVec3d::DotProduct(snrVelNED, snrToTgtLocNED);
   double velMag          = UtVec3d::Magnitude(snrVelNED);
   double locMag          = UtVec3d::Magnitude(snrToTgtLocNED);
   double cosAngle        = dot / (velMag * locMag);
   double totalAngle      = acos(std::min(std::max(cosAngle, -1.0), 1.0));
   aGeometry.mGroundSpeed = velMag;
   aGeometry.mTotalAngle  = fabs(totalAngle);

   // Compute the squint angle.
   aGeometry.mSquintAngle = 0.0;
   if (snrToTgtLocNE > 1.0E-6) // Not 'directly' below or above.
   {
      double v1[3] = {snrVelNED[0], snrVelNED[1], 0.0};
      double v2[3] = {snrToTgtLocNED[0], snrToTgtLocNED[1], 0.0};
      UtVec3d::Normalize(v1);
      UtVec3d::Normalize(v2);
      cosAngle               = UtVec3d::DotProduct(v1, v2);
      aGeometry.mSquintAngle = acos(std::min(std::max(cosAngle, -1.0), 1.0));
   }

   // Compute the scan angle for electronically scanned systems.
   // Compute the aspect of the target with respect to the current cue.
   // The target should be at (0,0) az/el, but we do this just for consistency...
   double snrToTgtAz;
   double snrToTgtEl;
   mAntennaPtr->ComputeAspect(snrToTgtLocWCS, snrToTgtAz, snrToTgtEl);

   // Compute the position of the beam with respect to the antenna coordinate system.
   double WCS_ToBeamTransform[3][3];
   double ebsAz;
   double ebsEl;
   mAntennaPtr->ComputeBeamPosition(mXmtrPtr.get(), snrToTgtAz, snrToTgtEl, WCS_ToBeamTransform, ebsAz, ebsEl);

   aGeometry.mScanAngle            = 0.0;
   WsfEM_Antenna::EBS_Mode ebsMode = mAntennaPtr->GetEBS_Mode();
   if (ebsMode != WsfEM_Antenna::cEBS_NONE)
   {
      if (ebsMode == WsfEM_Antenna::cEBS_AZIMUTH)
      {
         aGeometry.mScanAngle = fabs(ebsAz);
      }
      else if (ebsMode == WsfEM_Antenna::cEBS_ELEVATION)
      {
         aGeometry.mScanAngle = fabs(ebsEl);
      }
      else
      {
         double snrToTgtLocACS[3];
         mAntennaPtr->ConvertWCS_VectorToACS(snrToTgtLocWCS, snrToTgtLocACS);
         UtVec3d::Normalize(snrToTgtLocACS);
         aGeometry.mScanAngle = acos(snrToTgtLocACS[0]);
      }
   }

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Compute Geometry:";
      out.AddNote() << "Slant Range: " << aGeometry.mSlantRange;
      out.AddNote() << "Squint: " << aGeometry.mScanAngle * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Depression: " << aGeometry.mDepressionAngle * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Total: " << aGeometry.mTotalAngle * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Scan: " << aGeometry.mScanAngle * UtMath::cDEG_PER_RAD << " deg";
   }
}

// ================================================================================================
//! Compute the Clutter-to-Noise Ratio
//! This assumes UpdatePosition and ComputeGeometry have been called.
double WsfSAR_Sensor::SAR_Mode::ComputeCNR(const Geometry& aGeometry, double aPRF, double aDwellTime, double aResolution)
{
   // This is basically the mono-static radar equation with the target being the ground patch.

   if (aGeometry.mScanAngle >= UtMath::cPI_OVER_2)
   {
      // We can't look backwards into the antenna face...
      return 1.0E-37; // -370 db
   }

   double frequency  = mXmtrPtr->GetFrequency();
   double wavelength = UtMath::cLIGHT_SPEED / frequency;

   // Determine the effective RCS of the ground patch.
   double sigma0 = mSAR_SensorPtr->mBackscatterCoefficient;
   if (sigma0 <= 0.0)
   {
      // TODO - Where did this come from??? Look in Skolnik, Sec 12.6, pg 12.29-31
      double sigma0_DB = 15.0 * (log10(3.0) - 1.0 - log10(wavelength * 100.0));
      sigma0           = UtMath::DB_ToLinear(sigma0_DB);
   }

   double rcs          = sigma0 * aResolution * aResolution;
   double grazingAngle = std::max(fabs(aGeometry.mGrazingAngle), 5.0 * UtMath::cRAD_PER_DEG);
   rcs *= sin(grazingAngle);

   // The 'target' is going to be declared to be directly along the cue and on the ground.

   double tgtRange           = aGeometry.mSlantRange;
   double thisToTgtLocPCS[3] = {tgtRange, 0.0, 0.0};
   double thisToTgtLocWCS[3];
   GetSensor()->ConvertPCSVectorToWCS(thisToTgtLocWCS, thisToTgtLocPCS);

   double tgtLocWCS[3];
   GetSensor()->GetLocationWCS(thisToTgtLocWCS, tgtLocWCS);
   mSAR_SensorPtr->mTempPlatform.SetLocationWCS(tgtLocWCS);

   WsfSensorResult result;

   // This sets the pointers to the transmitter, target and receiver.
   result.BeginGenericInteraction(mXmtrPtr.get(), &(mSAR_SensorPtr->mTempPlatform), mRcvrPtr.get());

   // This computes all the relative geometry (as it is left undefined by the above call).
   result.ComputeUndefinedGeometry();

   // Set the transmitter and receiver beam positions (which will be the same).
   result.SetTransmitterBeamPosition();
   result.SetReceiverBeamPosition();

   // And compute the received power (two-way: transmitter->ground patch->receiver)
   double p_received = result.ComputeRF_TwoWayPower(rcs);

   // Account for the gain due to pulse compression.
   p_received *= mXmtrPtr->GetPulseCompressionRatio();

   // Integrate the pulses over the desired dwell time.
   int numberOfPulsesIntegrated = std::max(1, static_cast<int>(aDwellTime * aPRF));
   p_received *= numberOfPulsesIntegrated;

   // Account for integration gain
   p_received *= mIntegrationGain;

   // Allow for other general post-reception adjustments (for compatibility with SUPPRESSOR OPERATING-LOSSES).
   p_received *= mAdjustmentFactor;

   return (p_received / mRcvrPtr->GetNoisePower());
}

// ================================================================================================
//! Compute the dwell time necessary to achieve a specified resolution.
double WsfSAR_Sensor::SAR_Mode::ComputeDwellTime(const Geometry& aGeometry, double aResolution)
{
   double dwellTime = std::max(mMaximumDwellTime + 1.0, 1000.0);
   if (aGeometry.mScanAngle >= UtMath::cPI_OVER_2)
   {
      // We can't look backwards into the antenna face...
      return dwellTime;
   }

   // Reference 2, equation 5:
   //
   //               lambda * Ka * R
   // t_D = --------------------------------
   //       2 * V * delta_cr * sin(theta_sq)
   //
   // Remember: theta_sq in reference 2 is the 'total angle', not the standard 'squint angle'...

   // NOTE: The sin(theta_sq) term was replaced by fabs(sin(squint_angle)) * cos(grazing_angle).

   double wavelength = UtMath::cLIGHT_SPEED / mXmtrPtr->GetFrequency();
   double numerator  = wavelength * mKa * aGeometry.mSlantRange;
   double denominator =
      2.0 * aGeometry.mGroundSpeed * aResolution * fabs(sin(aGeometry.mSquintAngle)) * cos(aGeometry.mGrazingAngle);
   if (denominator > 0.0)
   {
      dwellTime = numerator / denominator;
   }
   // The 'min' is used to prevent ridiculous dwell times.
   return std::min(dwellTime, 1000.0);
}

// ================================================================================================
double WsfSAR_Sensor::SAR_Mode::ComputePRF(const Geometry& aGeometry)
{
   double prf = 1000.0;
   if (mSAR_SensorPtr->mSAR_ConstraintPlotting)
   {
      // If executing a SAR constraint plot, set the PRF equal to the value where the current slant
      // range is at the leading edge of the boundary of the ambiguous region.
      prf = UtMath::cLIGHT_SPEED / (2.0 * aGeometry.mSlantRange + 1.0);
   }
   else
   {
#if 1
      prf = UtMath::cLIGHT_SPEED / (2.0 * aGeometry.mSlantRange + 1.0);
      prf *= 0.9;
#else
      // Compute the PRF for the upper edge of the beam.
      double beamwidth       = mXmtr.GetElevationBeamwidth();
      double depressionAngle = aGeometry.mDepressionAngle - (0.5 * beamwidth);
      double grazingAngle;
      double slantRange = ComputeSlantRange(aGeometry.mAltitude, depressionAngle, grazingAngle);
      prf               = UtMath::cLIGHT_SPEED / (2.0 * slantRange);
#endif
   }
   return prf;
}

// ================================================================================================
double WsfSAR_Sensor::SAR_Mode::ComputeAzimuthResolution(const Geometry& aGeometry, double aDwellTime)
{
   double resolution = 1000.0;
   if (mAngularResolution > 0.0)
   {
      // Compute using the old mechanism
      resolution = mAngularResolution * aGeometry.mSlantRange;
   }
   else if (aGeometry.mScanAngle > UtMath::cPI_OVER_2)
   {
      // We can't look backwards into the antenna face...
   }
   else
   {
      // Reference 2, equation 5:
      //
      //               lambda * Ka * R
      // t_D = --------------------------------
      //       2 * V * delta_cr * sin(theta_sq)
      //
      // solving for delta_cr:
      //
      //                lambda * Ka * R
      // delta_cr = ---------------------------
      //            2 * V * t_D * sin(theta_sq)
      //
      // Remember: theta_sq in reference 2 is the 'total angle', not the standard 'squint angle'...

      // NOTE: The sin(theta_sq) term was replaced by fabs(sin(squint_angle)) * cos(grazing_angle).

      double wavelength = UtMath::cLIGHT_SPEED / mXmtrPtr->GetFrequency();
      double numerator  = wavelength * mKa * aGeometry.mSlantRange;
      double denominator =
         2.0 * aGeometry.mGroundSpeed * aDwellTime * fabs(sin(aGeometry.mSquintAngle)) * cos(aGeometry.mGrazingAngle);
      if (denominator > 0.0)
      {
         resolution = numerator / denominator;
      }
   }
   return resolution;
}

// ================================================================================================
double WsfSAR_Sensor::SAR_Mode::ComputeGroundRangeResolution(const Geometry& aGeometry)
{
   double resolution = mResolution;
   if (mAngularResolution > 0.0)
   {
      // Compute using the old mechanism
      resolution = mAngularResolution * aGeometry.mSlantRange;
   }
   else
   {
      double pulseWidth = mXmtrPtr->GetPulseWidth();
      if (mXmtrPtr->GetPulseWidth() > 0.0)
      {
         pulseWidth = mXmtrPtr->GetPulseWidth();
      }
      else if (mRcvrPtr->GetBandwidth() > 0.0)
      {
         pulseWidth = 1.0 / mRcvrPtr->GetBandwidth();
      }
      if (pulseWidth > 0.0)
      {
         resolution = 0.5 * UtMath::cLIGHT_SPEED * pulseWidth / std::max(1.0, mXmtrPtr->GetPulseCompressionRatio());
      }
   }
   double grazingAngle = std::min(fabs(aGeometry.mGrazingAngle), 45.0 * UtMath::cRAD_PER_DEG);
   return resolution / cos(grazingAngle);
}

// ================================================================================================
//! Compute the slant range to the 'center-of-interest' for a given altitude and depression angle.
//!
//! @param aAltitude        [input]  The altitude of the sensor. (meters above MSL)
//! @param aDepressionAngle [input]  The depression angle to the center-of-interest. (radians).
//! This is the POSITIVE angle that between the pointing vector to the center-of-interest and the
//! local horizontal plane at the sensor.
//! @param aGrazingAngle    [output] The angle between the pointing vector to the center-of-interest
//! and the local horizontal plane at the center of interest. (radians)
//! @returns The slant range to the center of interest. (meters)
double WsfSAR_Sensor::SAR_Mode::ComputeSlantRange(double aAltitude, double aDepressionAngle, double& aGrazingAngle)
{
   // Return immediately if negative altitude.
   if (aAltitude < 0.0)
   {
      aGrazingAngle = 0.0;
      return 0.0;
   }

   // Return immediately if pointing straight down.
   if (aDepressionAngle >= UtMath::cPI_OVER_2)
   {
      aGrazingAngle = UtMath::cPI_OVER_2;
      return aAltitude;
   }

   double re = UtSphericalEarth::cEARTH_RADIUS * mXmtrPtr->GetEarthRadiusMultiplier();

   // sideA  is the side from the Earth center to the source (sensor).
   // sideB  is the side from the Earth center to the target (center-of-interest)
   // sideC  is the side from the source location to the target location.
   // angleA is the included angle between sides B and C. (90 deg + aGrazingAngle)
   // angleB is the included angle between sides A and C. (90 deg - aDepressionAngle)
   // angleC is the included angle between sides A and B.

   // Compute the depression angle of the horizon.

   double sideA                  = re + aAltitude;
   double sideB                  = re;
   double sin_angleB             = sideB / sideA;
   double horizonDepressionAngle = UtMath::cPI_OVER_2 - asin(sin_angleB);

   // If the supplied depression angle is less than the depression angle of the horizon then
   // we can proceed with a solution. If the angle is greater than the horizon depression angle
   // then we simply return the horizon point as the center-of-interest.

   double slantRange = 0.0;
   if (aDepressionAngle > horizonDepressionAngle) // Pointing below the horizon
   {
      // Use the law of sines to determine angle A.
      double angleB     = UtMath::cPI_OVER_2 - aDepressionAngle;
      double sin_angleA = (sideA / sideB) * sin(angleB);

      // One would think angleA could be gotten directly using asin(sin_angleA), but NOOOO. The return values are
      // in the range [-pi/2, pi/2] and we KNOW our angle is GREATER than pi/2.  (This is known because we have
      // already tested to make sure we are below the horizon where angleA is pi/2). In fact, we know the return
      // value will be in the range [0, pi/2] because sin_angleA will always be positive.

      double angleA = UtMath::cPI - asin(sin_angleA); // This is what we need
      // double angleA = acos(sin_angleA) + UtMath::cPI_OVER_2;
      // double angleA = asin(sin_angleA);

      aGrazingAngle = angleA - UtMath::cPI_OVER_2; // The grazing angle is just angleA - 90 deg

      // Use the law of cosines to the slant range (side C).
      double angleC = UtMath::cPI - angleA - angleB;
      double sideC  = sqrt((sideA * sideA) + (sideB * sideB) - (2.0 * sideA * sideB * cos(angleC)));
      slantRange    = sideC;
   }
   else // Pointing at or above the horizon.
   {
      slantRange    = sqrt((sideA * sideA) - (sideB * sideB));
      aGrazingAngle = 0.0;
   }
   return slantRange;
}

// ================================================================================================
//! Compute the angle between the sensing platform's velocity vector and the target
double WsfSAR_Sensor::SAR_Mode::ComputeSquintAngle(WsfPlatform* aTargetPtr, double& aSlantRange)
{
   WsfPlatform* platformPtr = GetPlatform();

   double platformToTgt[3];
   platformPtr->GetRelativeLocationWCS(aTargetPtr, platformToTgt);
   aSlantRange = UtVec3d::Normalize(platformToTgt);
   double squintAngle;
   double platformToTgtEl;
   platformPtr->ComputeAspect(platformToTgt, squintAngle, platformToTgtEl);

   return squintAngle;
}

// ================================================================================================
//! ComputeRangeDopplerErrors
//! Taken and translated from SUPPRESSOR (mtsar.f) which got it from TRIMSIM
//! Computes a range and Doppler error used in the SAR Target Location Error Code
void WsfSAR_Sensor::SAR_Mode::SAR_ErrorModel::ComputeRangeDopplerErrors(double             aSimTime,
                                                                        const WsfPlatform* aTargetPtr,
                                                                        double&            aRangeError,
                                                                        double&            aDopplerError) const
{
   double relativePosition[3];
   double platformVelocity[3];

   // Get Target Position
   WsfPlatform* platformPtr = GetSensorMode()->GetPlatform();

   // Relative platform/tgt position vector
   platformPtr->GetRelativeLocationNED(aTargetPtr, relativePosition);

   // Compute the range and unitize the relative position
   UtVec3d::Normalize(relativePosition);

   double xmitFreq = static_cast<WsfSAR_Sensor::SAR_Mode*>(GetSensorMode())->mXmtrPtr->GetFrequency();

   platformPtr->GetVelocityNED(platformVelocity);

   // Compute the Doppler frequency
   double doppler = UtVec3d::DotProduct(relativePosition, platformVelocity) * 2.0 * xmitFreq / UtMath::cLIGHT_SPEED;

   // Compute Angle Error and then Normalize
   double unitAngleErr[3];
   unitAngleErr[0] = -relativePosition[0] * relativePosition[2];
   unitAngleErr[1] = -relativePosition[1] * relativePosition[2];
   unitAngleErr[2] = relativePosition[0] * relativePosition[0] + relativePosition[1] * relativePosition[1];

   UtVec3d::Normalize(unitAngleErr);

   // SAR Range Observation
   // For now leave these effects of Atmospheric Refraction at ZERO
   double rangeError = 0.0;
   double angleError = 0.0;

   // Load the Observation Matrix
   UtMatrixd Observation_H(2, 3);
   Observation_H(0, 0) = relativePosition[0];
   Observation_H(0, 1) = relativePosition[1];
   Observation_H(0, 2) = relativePosition[2];

   UtMatrixd rangeDopplerErrors(2, 15);

   // Weighted Errors for Range
   rangeDopplerErrors(0, 0) = -Observation_H(0, 0);
   rangeDopplerErrors(0, 1) = -Observation_H(0, 1);
   rangeDopplerErrors(0, 2) = -Observation_H(0, 2);
   rangeDopplerErrors(0, 3) = 0.0;
   rangeDopplerErrors(0, 4) = 0.0;
   rangeDopplerErrors(0, 5) = 0.0;
   rangeDopplerErrors(0, 6) = Observation_H(0, 0) * platformVelocity[0] + Observation_H(0, 1) * platformVelocity[1] +
                              Observation_H(0, 2) * platformVelocity[2];
   rangeDopplerErrors(0, 7)  = -1.0;
   rangeDopplerErrors(0, 8)  = 0.0;
   rangeDopplerErrors(0, 9)  = 0.0;
   rangeDopplerErrors(0, 10) = 1.0;
   rangeDopplerErrors(0, 11) = 1.0;
   rangeDopplerErrors(0, 12) = 0.0;
   rangeDopplerErrors(0, 13) = 0.0;
   rangeDopplerErrors(0, 14) = -rangeError;

   // SAR Doppler Observation
   UtMatrixd unitPos(3, 1);
   UtMatrixd unitPosTranspose(1, 3);

   unitPos(0, 0) = relativePosition[0];
   unitPos(1, 0) = relativePosition[1];
   unitPos(2, 0) = relativePosition[2];

   unitPosTranspose.Transpose(unitPos);

   UtMatrixd temp1(3, 3);
   UtMatrixd temp2(3, 3);
   temp1.Multiply(unitPos, unitPosTranspose);

   UtMatrixd identity3x3(3, 3);
   identity3x3.LoadIdentity();

   temp2.Subtract(identity3x3, temp1);

   UtMatrixd velSARTranspose(1, 3);
   velSARTranspose(0, 0) = platformVelocity[0];
   velSARTranspose(0, 1) = platformVelocity[1];
   velSARTranspose(0, 2) = platformVelocity[2];

   UtMatrixd arow(1, 3);
   arow.Multiply(velSARTranspose, temp2);

   double temp[3] = {arow(0, 0), arow(0, 1), arow(0, 2)};
   double scale   = UtVec3d::Normalize(temp);

   Observation_H(1, 0) = temp[0];
   Observation_H(1, 1) = temp[1];
   Observation_H(1, 2) = temp[2];

   rangeDopplerErrors(1, 0)  = -Observation_H(1, 0);
   rangeDopplerErrors(1, 1)  = -Observation_H(1, 1);
   rangeDopplerErrors(1, 2)  = -Observation_H(1, 2);
   rangeDopplerErrors(1, 3)  = scale * unitPos(0, 0);
   rangeDopplerErrors(1, 4)  = scale * unitPos(1, 0);
   rangeDopplerErrors(1, 5)  = scale * unitPos(2, 0);
   rangeDopplerErrors(1, 6)  = scale * UtVec3d::DotProduct(temp, platformVelocity);
   rangeDopplerErrors(1, 7)  = 0.0;
   rangeDopplerErrors(1, 8)  = -scale * UtMath::cLIGHT_SPEED / (2.0 * xmitFreq);
   rangeDopplerErrors(1, 9)  = -scale * UtMath::cLIGHT_SPEED * doppler / (2.0 * xmitFreq * xmitFreq);
   rangeDopplerErrors(1, 10) = 0.0;
   rangeDopplerErrors(1, 11) = 0.0;
   rangeDopplerErrors(1, 12) = 1.0;
   rangeDopplerErrors(1, 13) = 1.0;
   rangeDopplerErrors(1, 14) = -scale * (UtVec3d::DotProduct(platformVelocity, unitAngleErr)) * fabs(angleError);

   UtMatrixd Temp2x15(2, 15);
   Temp2x15.Multiply(rangeDopplerErrors, mErrorsSqrd);

   UtMatrixd ErrorsTranspose(15, 2);
   ErrorsTranspose.Transpose(rangeDopplerErrors);

   mCovarianceMatrix.Multiply(Temp2x15, ErrorsTranspose);

   aRangeError   = sqrt(mCovarianceMatrix(0, 0));
   aDopplerError = sqrt(mCovarianceMatrix(1, 1));
}

// ================================================================================================
//! Compute the simulation update interval for this mode.
void WsfSAR_Sensor::SAR_Mode::ComputeUpdateInterval(double aSimTime)
{
   double updateInterval = GetFrameTime();
   if ((mOperatingMode == cOM_STRIP) && (updateInterval <= 0.0))
   {
      // If an update interval in strip mode, calculate one based on the velocity and the beamwidth.
      // We'll try to take samples every half beamwidth on the ground.

      Geometry geometry;
      GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current
      ComputeGeometry(aSimTime, geometry);

      double beamwidth = std::min(mXmtrPtr->GetAzimuthBeamwidth(0.0, 0.0), mXmtrPtr->GetElevationBeamwidth(0.0, 0.0));
      double distance  = 0.5 * beamwidth * geometry.mSlantRange;
      if (geometry.mGroundSpeed > 1.0)
      {
         updateInterval = distance / geometry.mGroundSpeed;
      }
   }
   if (updateInterval <= 0.0)
   {
      updateInterval = 1.0;
   }

   if (mSAR_SensorPtr->mShowStatus)
   {
      auto out = ut::log::info() << "Using simulation update interval.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Interval: " << updateInterval << " seconds";
   }
   GetSensor()->SetUpdateInterval(updateInterval);
}

// ================================================================================================
void WsfSAR_Sensor::SAR_Mode::PredictPerformance(const Geometry& aGeometry,
                                                 double&         aPRF,
                                                 double&         aDwellTime,
                                                 double&         aAzimuthResolution,
                                                 double&         aCNR,
                                                 double&         aMinimumPRF)
{
   // Compute the dwell time if requested, otherwise use the dwell_time as declared in input.
   // If the resolution has been specified, then the dwell_time needs to be computed.
   if (mComputeDwellTime)
   {
      aAzimuthResolution = mResolution;
      aDwellTime         = ComputeDwellTime(aGeometry, aAzimuthResolution);
   }
   else
   {
      // A desired dwell time has been specified, so let's see what the resolution will be
      aDwellTime         = mDwellTime;
      aAzimuthResolution = ComputeAzimuthResolution(aGeometry, aDwellTime);
   }

   // Compute the PRF needed to get an image of the desired resolution in the desired time.
   aPRF = mXmtrPtr->GetPulseRepetitionFrequency();
   if (mComputePRF)
   {
      aPRF = ComputePRF(aGeometry);
   }

   // Using the resolution and dwell time predict the CNR
   aCNR = ComputeCNR(aGeometry, aPRF, aDwellTime, aAzimuthResolution);

   // Compute the minimum PRF allowed...
   // Reference 2, equation 13.

   aMinimumPRF = aPRF + 1.0;                      // Assume Doppler fold-over...
   if (aGeometry.mScanAngle < UtMath::cPI_OVER_2) // Not looking back into the antenna face
   {
      double wavelength  = UtMath::cLIGHT_SPEED / mXmtrPtr->GetFrequency();
      double theta_BW    = mXmtrPtr->GetAzimuthBeamwidth(0.0, 0.0);
      double numerator   = 2.0 * aGeometry.mGroundSpeed * mKd * theta_BW * sin(aGeometry.mTotalAngle);
      double denominator = wavelength * fabs(cos(aGeometry.mScanAngle));
      if (denominator != 0.0)
      {
         aMinimumPRF = numerator / denominator;
      }
   }
}

// ================================================================================================
//! Computes the center of the image. (Recursive)
//! @param aLocationWCS_1 WCS location of point 1
//! @param aLocationWCS_2 WCS location of point 2
//! @param aFirstPass     Set to true if first pass (default = false)
//! @return 'true' if image center calculation is complete or 'false' if not.
bool WsfSAR_Sensor::SAR_Mode::ComputeImageCenter(double aLocationWCS_1[3], double aLocationWCS_2[3], bool aFirstPass)
{
   // Get slant range squared
   double deltaLocWCS[3] = {0.0};
   UtVec3d::Subtract(deltaLocWCS, aLocationWCS_2, aLocationWCS_1);
   double slantRangeSquared = UtVec3d::MagnitudeSquared(deltaLocWCS);

   // Check for termination
   if (slantRangeSquared < 100.0)
   {
      return true;
   }

   // Masked by terrain check
   double lat1;
   double lon1;
   double alt1;
   UtEntity::ConvertWCSToLLA(aLocationWCS_1, lat1, lon1, alt1);
   double lat2;
   double lon2;
   double alt2;
   UtEntity::ConvertWCSToLLA(aLocationWCS_2, lat2, lon2, alt2);

   if (GetSimulation()->GetTerrainInterface()->MaskedByTerrain(lat1, lon1, alt1, lat2, lon2, alt2, 0.0))
   {
      // Move b = midpoint
      double temp[3] = {0.0};
      UtVec3d::Multiply(temp, deltaLocWCS, 0.5);
      UtVec3d::Add(aLocationWCS_2, aLocationWCS_1, temp);
   }
   else
   {
      if (aFirstPass)
      {
         return true;
      }

      // b
      double temp[3] = {0.0};
      UtVec3d::Multiply(temp, deltaLocWCS, 1.5);
      UtVec3d::Add(aLocationWCS_2, aLocationWCS_1, temp);

      // a
      UtVec3d::Add(aLocationWCS_1, aLocationWCS_1, deltaLocWCS);
   }
   return ComputeImageCenter(aLocationWCS_1, aLocationWCS_2);
}

// ================================================================================================
//! Computes the sensor FOV if image height and width constraints are given.
//! @param aGeometry  Current sensor geometry
void WsfSAR_Sensor::SAR_Mode::ComputeFOV(const Geometry& aGeometry)
{
   // Quick return if no cue
   if (GetSensor()->GetCueType() == cCUED_TO_NOTHING)
   {
      return;
   }

   if ((mImageHeight > 0.0) && (mImageWidth > 0.0))
   {
      // Get the sensor location in WCS
      double snrLocWCS[3] = {0.0};
      GetPlatform()->GetLocationWCS(snrLocWCS);

      // Get the cue location in WCS
      double cueWCS[3] = {0.0};
      if (GetSensor()->GetCueType() == cCUED_TO_LOCATION)
      {
         GetSensor()->GetCuedLocationWCS(cueWCS);
      }
      else if (GetSensor()->GetCueType() == cCUED_TO_ANGLE)
      {
         double snrToCueLocPCS[3] = {aGeometry.mSlantRange, 0.0, 0.0};
         double thisToCueLocWCS[3];
         GetSensor()->ConvertPCSVectorToWCS(thisToCueLocWCS, snrToCueLocPCS);
         UtVec3d::Add(cueWCS, snrLocWCS, thisToCueLocWCS);
      }

      // Find the center of the image; takes terrain into account
      ComputeImageCenter(snrLocWCS, cueWCS, true);
      UtVec3d::Set(mImageCenterWCS, cueWCS);

      // Get slant range
      double snrToCueLocWCS[3] = {0.0};
      GetPlatform()->GetRelativeLocationWCS(cueWCS, snrToCueLocWCS);
      double slantRange = UtVec3d::Magnitude(snrToCueLocWCS);

      // image width
      double azAngle = atan2(mImageWidth / 2.0, slantRange);
      mAntennaPtr->SetAzimuthFieldOfView(-azAngle, azAngle);

      // image height
      // Depression angle remains the same we just march up the LOS vector
      double snrToCueLocNED[3] = {0.0};
      GetPlatform()->ConvertWCSVectorToNED(snrToCueLocNED, snrToCueLocWCS);

      double temp1        = snrToCueLocNED[2] * tan(aGeometry.mDepressionAngle);
      double temp2        = mImageHeight / 2.0 * tan(aGeometry.mDepressionAngle);
      double elAngleUpper = atan2(temp1, (snrToCueLocNED[2] + temp2));
      double elAngleLower = atan2(temp1, (snrToCueLocNED[2] - temp2));

      // Find the difference between depression angle
      elAngleUpper = fabs(aGeometry.mDepressionAngle - elAngleUpper);
      elAngleLower = fabs(aGeometry.mDepressionAngle - elAngleLower);

      // Elevation FOV is between +90 or -90 degrees
      elAngleUpper = UtMath::Limit(elAngleUpper, 0.0, UtMath::cPI_OVER_2);
      elAngleLower = UtMath::Limit(elAngleLower, 0.0, UtMath::cPI_OVER_2);
      mAntennaPtr->SetElevationFieldOfView(-elAngleLower, elAngleUpper);
   }
}

// ================================================================================================
bool WsfSAR_Sensor::SAR_Mode::WithinUnambiguousRange(WsfSensorResult& aResult)
{
   double prf = mXmtrPtr->GetPulseRepetitionFrequency();
   if (prf <= 0.0)
   {
      return true;
   }

   double maxRange = UtMath::cLIGHT_SPEED / (2.0 * prf);
   return (aResult.mXmtrToTgt.mRange <= maxRange);
}

// ================================================================================================
void WsfSAR_Sensor::SAR_Mode::Calibrate()
{
   WsfEM_Xmtr& xmtr = *GetEM_Xmtr();
   WsfEM_Rcvr& rcvr = *GetEM_Rcvr();

   double p_avg      = xmtr.GetAveragePower();
   double dutyCycle  = xmtr.GetPulseWidth() * xmtr.GetPulseRepetitionFrequency();
   double wavelength = UtMath::cLIGHT_SPEED / xmtr.GetFrequency();

   bool print = mSensorPtr->ShowCalibrationData();
   auto out   = ut::log::info();
   if (print)
   {
      out << "WsfSAR_Sensor::Calibrate():";
      out.AddNote() << "Peak Power Output: " << UtMath::LinearToDB(xmtr.GetPeakPower()) << " dbW ("
                    << xmtr.GetPeakPower() << " W)";
      out.AddNote() << "Pulse Repetition Frequency: " << xmtr.GetPulseRepetitionFrequency() << " Hz";
      out.AddNote() << "Pulse Width: " << xmtr.GetPulseWidth() << " sec";
      out.AddNote() << "Duty Cycle: " << dutyCycle;
      double pcr = xmtr.GetPulseCompressionRatio();
      out.AddNote() << "Pulse Compression Ratio: " << UtMath::LinearToDB(pcr) << " dB (" << pcr << ")";
      out.AddNote() << "Average Power Output: " << UtMath::LinearToDB(p_avg) << " dBW (" << p_avg << " W)";
      out.AddNote() << "Frequency: " << xmtr.GetFrequency() << " Hz";
      out.AddNote() << "Wavelength: " << wavelength << " m";

      double peakGain;
      double azBeamwidth;
      double elBeamwidth;
      peakGain    = xmtr.GetAntennaGain(xmtr.GetPolarization(), xmtr.GetFrequency(), 0.0, 0.0, 0.0, 0.0);
      azBeamwidth = xmtr.GetAzimuthBeamwidth(0.0, 0.0);
      elBeamwidth = xmtr.GetElevationBeamwidth(0.0, 0.0);
      out.AddNote() << "Transmitter Antenna Gain: " << UtMath::LinearToDB(peakGain) << " dB (" << peakGain << ")";
      out.AddNote() << "Azimuth Beamwidth: " << azBeamwidth * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Elevation Beamwidth: " << elBeamwidth * UtMath::cDEG_PER_RAD << " deg";
      peakGain    = rcvr.GetAntennaGain(xmtr.GetPolarization(), xmtr.GetFrequency(), 0.0, 0.0, 0.0, 0.0);
      azBeamwidth = rcvr.GetAzimuthBeamwidth(0.0, 0.0);
      elBeamwidth = rcvr.GetElevationBeamwidth(0.0, 0.0);
      out.AddNote() << "Receiver Antenna Gain: " << UtMath::LinearToDB(peakGain) << " dB (" << peakGain << ")";
      out.AddNote() << "Azimuth Beamwidth: " << azBeamwidth * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Elevation Beamwidth: " << elBeamwidth * UtMath::cDEG_PER_RAD << " deg";

      double internalLoss;
      internalLoss = xmtr.GetInternalLoss();
      out.AddNote() << "Transmitter Internal Loss: " << UtMath::LinearToDB(internalLoss) << " dB (" << internalLoss << ")";
      internalLoss = rcvr.GetInternalLoss();
      out.AddNote() << "Receiver Internal Loss: " << UtMath::LinearToDB(internalLoss) << " dB (" << internalLoss << ")";
      if (mIntegrationGain != 1.0)
      {
         out.AddNote() << "Integration Gain: " << UtMath::LinearToDB(mIntegrationGain) << " dB (" << mIntegrationGain
                       << ")";
      }
      if (mAdjustmentFactor != 1.0)
      {
         out.AddNote() << "Adjustment Factor: " << UtMath::LinearToDB(mAdjustmentFactor) << " dB (" << mAdjustmentFactor
                       << ")";
      }

      if (mOneM2DetectRange > 0.0)
      {
         out.AddNote() << "1 m^2 Detection Range: " << mOneM2DetectRange << " m";
      }
      else
      {
         out.AddNote() << "Receiver Noise Power: " << UtMath::LinearToDB(rcvr.GetNoisePower()) << " dBW ("
                       << rcvr.GetNoisePower() << " W)";
      }
   }

   double detectionThreshold = rcvr.GetDetectionThreshold();
   if (print)
   {
      double thresholdWatts = detectionThreshold * rcvr.GetNoisePower();
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
      double p_radiated = xmtr.GetPower() * xmtr.GetPeakAntennaGain() / xmtr.GetInternalLoss();

      // Compute the power per unit area on at the target
      double p_tgt_area = p_radiated * distanceFactor;

      // Compute the reflected power
      double rcs         = 1.0;
      double p_reflected = p_tgt_area * rcs;

      // Compute the incident power per unit area at the receiving antenna.
      double p_rcvr_area = p_reflected * distanceFactor;

      // Compute the effective received power.
      double temp1      = wavelength * wavelength / (4.0 * UtMath::cPI);
      double p_received = p_rcvr_area * temp1 * rcvr.GetPeakAntennaGain() / rcvr.GetInternalLoss();

      // Account for the gain due to pulse compression.
      p_received *= xmtr.GetPulseCompressionRatio();

      // Account for integration gain
      p_received *= mIntegrationGain;

      // Allow for other general post-reception adjustments (for compatibility with SUPPRESSOR OPERATING-LOSSES).
      p_received *= mAdjustmentFactor;

      double rcvrNoise = p_received / detectionThreshold;
      rcvr.SetNoisePower(rcvrNoise);
      if (print)
      {
         out.AddNote() << "Receiver Noise: " << UtMath::LinearToDB(rcvrNoise) << " dbW (" << rcvrNoise
                       << " W) (calibrated)";
      }
   }

   {
      double temp1 = (wavelength * wavelength) / (UtMath::cFOUR_PI * UtMath::cFOUR_PI * UtMath::cFOUR_PI);
      temp1 *= xmtr.GetPower() * xmtr.GetPeakAntennaGain() * rcvr.GetPeakAntennaGain();
      temp1 /= (xmtr.GetInternalLoss() * rcvr.GetInternalLoss());

      // Account for the gain due to pulse compression.
      temp1 *= xmtr.GetPulseCompressionRatio();

      // Account for integration gain
      temp1 *= mIntegrationGain;

      // Allow for other general post-reception adjustments (for compatibility with SUPPRESSOR OPERATING-LOSSES).
      temp1 *= mAdjustmentFactor;

      double r = pow(temp1 / (rcvr.GetNoisePower() * detectionThreshold), 0.25);
      if (print)
      {
         out.AddNote() << "1 m^2 Detection Range: " << r << " m (calibrated - free space)";
      }
   }
}

WsfScriptSAR_SensorClass::WsfScriptSAR_SensorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptSensorClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSAR_Sensor");

   AddMethod(ut::make_unique<GetDwellTime_1>("GetDwellTime"));
   AddMethod(ut::make_unique<GetDwellTime_2>("GetDwellTime"));
   AddMethod(ut::make_unique<SetDwellTime_1>("SetDwellTime"));
   AddMethod(ut::make_unique<SetDwellTime_2>("SetDwellTime"));

   AddMethod(ut::make_unique<GetResolution_1>("GetResolution"));
   AddMethod(ut::make_unique<GetResolution_2>("GetResolution"));
   AddMethod(ut::make_unique<SetResolution_1>("SetResolution"));
   AddMethod(ut::make_unique<SetResolution_2>("SetResolution"));
}

//! Returns the dwell time of currently selected mode
UT_DEFINE_SCRIPT_METHOD(WsfScriptSAR_SensorClass, WsfSAR_Sensor, GetDwellTime_1, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDwellTime());
}

//! Returns the dwell time of the specified mode
UT_DEFINE_SCRIPT_METHOD(WsfScriptSAR_SensorClass, WsfSAR_Sensor, GetDwellTime_2, 1, "double", "string")
{
   auto modeName = aVarArgs[0].GetString();
   auto index    = aObjectPtr->GetModeIndex(modeName);
   if (index >= 0 && index < aObjectPtr->GetModeCount())
   {
      auto sarMode = static_cast<WsfSAR_Sensor::SAR_Mode*>(aObjectPtr->GetModeEntry(index));
      aReturnVal.SetDouble(sarMode->mDwellTime);
   }
   else
   {
      UT_SCRIPT_ABORT("Invalid mode name");
   }
}

//! Returns the resolution of currently selected mode
UT_DEFINE_SCRIPT_METHOD(WsfScriptSAR_SensorClass, WsfSAR_Sensor, GetResolution_1, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetResolution());
}

//! Returns the resolution of the specified mode
UT_DEFINE_SCRIPT_METHOD(WsfScriptSAR_SensorClass, WsfSAR_Sensor, GetResolution_2, 1, "double", "string")
{
   auto modeName = aVarArgs[0].GetString();
   auto index    = aObjectPtr->GetModeIndex(modeName);
   if (index >= 0 && index < aObjectPtr->GetModeCount())
   {
      auto sarMode = static_cast<WsfSAR_Sensor::SAR_Mode*>(aObjectPtr->GetModeEntry(index));
      aReturnVal.SetDouble(sarMode->mResolution);
   }
   else
   {
      UT_SCRIPT_ABORT("Invalid mode name");
   }
}

//! Sets the dwell time of currently selected mode
UT_DEFINE_SCRIPT_METHOD(WsfScriptSAR_SensorClass, WsfSAR_Sensor, SetDwellTime_1, 1, "void", "double")
{
   aObjectPtr->SetDwellTime(aVarArgs[0].GetDouble());
}

//! Sets the dwell time of the specified mode
UT_DEFINE_SCRIPT_METHOD(WsfScriptSAR_SensorClass, WsfSAR_Sensor, SetDwellTime_2, 2, "void", "double, string")
{
   auto modeName = aVarArgs[1].GetString();
   auto index    = aObjectPtr->GetModeIndex(modeName);
   if (index >= 0 && index < aObjectPtr->GetModeCount())
   {
      auto sarMode        = static_cast<WsfSAR_Sensor::SAR_Mode*>(aObjectPtr->GetModeEntry(index));
      sarMode->mDwellTime = aVarArgs[0].GetDouble();
   }
   else
   {
      UT_SCRIPT_ABORT("Invalid mode name");
   }
}

//! Sets the resolution of currently selected mode
UT_DEFINE_SCRIPT_METHOD(WsfScriptSAR_SensorClass, WsfSAR_Sensor, SetResolution_1, 1, "void", "double")
{
   aObjectPtr->SetResolution(aVarArgs[0].GetDouble());
}

//! Sets the resolution of the specified mode
UT_DEFINE_SCRIPT_METHOD(WsfScriptSAR_SensorClass, WsfSAR_Sensor, SetResolution_2, 2, "void", "double, string")
{
   auto modeName = aVarArgs[1].GetString();
   auto index    = aObjectPtr->GetModeIndex(modeName);
   if (index >= 0 && index < aObjectPtr->GetModeCount())
   {
      auto sarMode         = static_cast<WsfSAR_Sensor::SAR_Mode*>(aObjectPtr->GetModeEntry(index));
      sarMode->mResolution = aVarArgs[0].GetDouble();
   }
   else
   {
      UT_SCRIPT_ABORT("Invalid mode name");
   }
}

std::unique_ptr<wsf::SphericalMeasurementErrors>
WsfSAR_Sensor::SAR_Mode::SAR_ErrorModel::ComputeMeasurementErrors(WsfSensorResult& aResult) const
{
   mTempPlatformPtr->SetLocationWCS(aResult.mMeasurement.GetLocationWCS().GetData());
   wsf::SphericalMeasurementErrors errors;

   // Compute Range and Doppler Errors
   if (mComputeRangeDopplerErrors)
   {
      ComputeRangeDopplerErrors(aResult.mMeasurement.GetUpdateTime(),
                                mTempPlatformPtr.get(),
                                errors.mRangeError,
                                errors.mRangeRateError);
   }

   return ut::make_unique<wsf::SphericalMeasurementErrors>(errors);
}

void WsfSAR_Sensor::SAR_Mode::SAR_ErrorModel::ApplyMeasurementErrors(const wsf::SphericalMeasurementErrors& aErrors,
                                                                     WsfSensorResult&                       aResult)
{
   // Compute Range and Doppler Errors
   if (mComputeRangeDopplerErrors)
   {
      aResult.mMeasurement.SetRangeError(aErrors.mRangeError);
      aResult.mMeasurement.SetRangeRateError(aErrors.mRangeRateError);
   }
}

//! Apply spherical measurement errors to a SAR_Chance result.
//! @note Because SAR_Chance does not derive from WsfSensorResult, this special implementation needed to be created,
//! instead of implementing an override of ApplyMeasurementErrors.
void WsfSAR_Sensor::SAR_Mode::SAR_ErrorModel::ApplySAR_MeasurementErrors(const wsf::SphericalMeasurementErrors& aErrors,
                                                                         SAR_Chance&                            aChance)
{
   // If the range and Doppler errors were computed, they will be used for Target Location Error Generation
   // Otherwise the standard WsfSensor error computations will be used.
   if (mComputeRangeDopplerErrors)
   {
      ApplyRangeDopplerErrors(mAntennaPtr, mTempPlatformPtr.get(), &aChance);
   }
   else
   {
      // Determine the relative location of the target with respect to the sensor, apply the
      // measurement errors and convert the apparent relative location back to WCS.

      double snrToTgtLocWCS[3];
      double snrToTgtRange;
      double snrToTgtAz;
      double snrToTgtEl;
      mAntennaPtr->GetRelativeLocationWCS(aChance.mLocationWCS, snrToTgtLocWCS);
      mAntennaPtr->ComputeAspect(snrToTgtLocWCS, snrToTgtAz, snrToTgtEl);
      snrToTgtRange = UtVec3d::Magnitude(snrToTgtLocWCS);
      mAntennaPtr->GetRelativeLocationWCS(snrToTgtAz + aErrors.mAzError,
                                          snrToTgtEl + aErrors.mElError,
                                          snrToTgtRange + aErrors.mRangeError,
                                          snrToTgtLocWCS);
      mAntennaPtr->GetLocationWCS(snrToTgtLocWCS, aChance.mLocationWCS);
   }
}

WsfSAR_Sensor::SAR_Mode::SAR_ErrorModel::SAR_ErrorModel()
{
   for (size_t i = 0; i < 15; ++i)
   {
      mSAR_Errors[i]    = 0.0;
      mSAR_ErrorsSet[i] = false;
   }
}

WsfSAR_Sensor::SAR_Mode::SAR_ErrorModel::SAR_ErrorModel(const SAR_ErrorModel& aSrc)
{
   for (size_t i = 0; i < 15; ++i)
   {
      mSAR_Errors[i]    = aSrc.mSAR_Errors[i];
      mSAR_ErrorsSet[i] = aSrc.mSAR_ErrorsSet[i];
   }
   if (aSrc.mTempPlatformPtr != nullptr)
   {
      mTempPlatformPtr = aSrc.mTempPlatformPtr->Clone();
   }
}

bool WsfSAR_Sensor::SAR_Mode::SAR_ErrorModel::ProcessInput(UtInput& aInput)
{
   mComputeRangeDopplerErrors = true;

   // Make sure the Error Array and associated Boolean Array is initialized
   for (size_t i = 0; i < 15; ++i)
   {
      mSAR_Errors[i]    = 0.0;
      mSAR_ErrorsSet[i] = false;
   }

   std::string command{aInput.GetCommand()};
   bool        myCommand = false;
   if (command == "sar_error_model_parameters")
   {
      UtInputBlock block(aInput, "end_sar_error_model_parameters");
      while (block.ReadCommand(command))
      {
         if ((command == "platform_position_error_sigmas") || (command == "platform_position_error_sigmas_NED")) // DEPRECATED
         {
            double northErr;
            double eastErr;
            double downErr;
            block.GetInput().ReadValueOfType(northErr, UtInput::cLENGTH);
            block.GetInput().ReadValueOfType(eastErr, UtInput::cLENGTH);
            block.GetInput().ReadValueOfType(downErr, UtInput::cLENGTH);

            // Put this data into the mSAR_Error array in NED
            mSAR_Errors[0] = northErr;
            mSAR_Errors[1] = eastErr;
            mSAR_Errors[2] = downErr;

            // Keep track of which data has been set
            mSAR_ErrorsSet[0] = true;
            mSAR_ErrorsSet[1] = true;
            mSAR_ErrorsSet[2] = true;
         }
         else if ((command == "platform_velocity_error_sigmas") || (command == "platform_velocity_error_sigmas_NED")) // DEPRECATED
         {
            double nDotErr;
            double eDotErr;
            double dDotErr;
            block.GetInput().ReadValueOfType(nDotErr, UtInput::cSPEED);
            block.GetInput().ReadValueOfType(eDotErr, UtInput::cSPEED);
            block.GetInput().ReadValueOfType(dDotErr, UtInput::cSPEED);

            // Put this data into the mSAR_Error array in NED
            mSAR_Errors[3] = nDotErr;
            mSAR_Errors[4] = eDotErr;
            mSAR_Errors[5] = dDotErr;

            // Keep track of which data has been set
            mSAR_ErrorsSet[3] = true;
            mSAR_ErrorsSet[4] = true;
            mSAR_ErrorsSet[5] = true;
         }
         else if (command == "inter_system_time_delay")
         {
            double timeDelay;
            block.GetInput().ReadValueOfType(timeDelay, UtInput::cTIME);

            mSAR_Errors[6]    = timeDelay;
            mSAR_ErrorsSet[6] = true;
         }
         else if (command == "range_error_sigma")
         {
            double rangeSigma;
            block.GetInput().ReadValueOfType(rangeSigma, UtInput::cLENGTH);

            mSAR_Errors[7]    = rangeSigma;
            mSAR_ErrorsSet[7] = true;
         }
         else if (command == "doppler_frequency_error_sigma")
         {
            double dopplerSigma;
            block.GetInput().ReadValueOfType(dopplerSigma, UtInput::cFREQUENCY);

            mSAR_Errors[8]    = dopplerSigma;
            mSAR_ErrorsSet[8] = true;
         }
         else if (command == "transmit_frequency_error_sigma")
         {
            double xmitSigma;
            block.GetInput().ReadValueOfType(xmitSigma, UtInput::cFREQUENCY);

            mSAR_Errors[9]    = xmitSigma;
            mSAR_ErrorsSet[9] = true;
         }
         else if (command == "range_cursor_location_error_sigma")
         {
            double cursorRange;
            block.GetInput().ReadValueOfType(cursorRange, UtInput::cLENGTH);

            mSAR_Errors[10]    = cursorRange;
            mSAR_ErrorsSet[10] = true;
         }
         else if (command == "range_target_selection_error_sigma")
         {
            double rangeSelection;
            block.GetInput().ReadValueOfType(rangeSelection, UtInput::cLENGTH);

            mSAR_Errors[11]    = rangeSelection;
            mSAR_ErrorsSet[11] = true;
         }
         else if (command == "doppler_cursor_location_error_sigma")
         {
            double cursorDoppler;
            block.GetInput().ReadValueOfType(cursorDoppler, UtInput::cLENGTH);

            mSAR_Errors[12]    = cursorDoppler;
            mSAR_ErrorsSet[12] = true;
         }
         else if (command == "doppler_target_selection_error_sigma")
         {
            double dopplerSelection;
            block.GetInput().ReadValueOfType(dopplerSelection, UtInput::cLENGTH);

            mSAR_Errors[13]    = dopplerSelection;
            mSAR_ErrorsSet[13] = true;
         }
         else if (command == "atmospheric_refraction_residual")
         {
            double atmosResidual;
            block.GetInput().ReadValue(atmosResidual);

            mSAR_Errors[14]    = atmosResidual;
            mSAR_ErrorsSet[14] = true;
         }
         else
         {
            throw UtInput::UnknownCommand(block.GetInput());
         }
      }
      myCommand = true;
   }
   else
   {
      myCommand = wsf::StandardSensorErrorModel::ProcessInput(aInput);
   }

   return myCommand;
}

bool WsfSAR_Sensor::SAR_Mode::SAR_ErrorModel::Initialize(WsfSensorMode* aSensorModePtr)
{
   // If computing an Error Covariance Matrix check to see if everything has been set.
   if (mComputeRangeDopplerErrors)
   {
      for (bool error : mSAR_ErrorsSet)
      {
         if (!error)
         {
            auto out = ut::log::error() << "SAR Sensor Error Statistics were NOT initialized.";
            out.AddNote() << "Sensor: " << GetSensorMode()->GetSensor()->GetName();
            out.AddNote() << "Sensor Type: " << GetSensorMode()->GetSensor()->GetType();
            out.AddNote() << "Mode: " << GetName();
            break;
         }
      }

      // Create the Error Matrix used in the Covariance Computation
      for (unsigned int j = 0; j < 15; ++j)
      {
         mErrorsSqrd(j, j) = pow(mSAR_Errors[j], 2.0);
      }
   }

   mTempPlatformPtr = ut::make_unique<WsfPlatform>(aSensorModePtr->GetScenario());
   mAntennaPtr      = static_cast<SAR_Mode*>(aSensorModePtr)->mAntennaPtr.get();

   return wsf::StandardSensorErrorModel::Initialize(aSensorModePtr);
}
