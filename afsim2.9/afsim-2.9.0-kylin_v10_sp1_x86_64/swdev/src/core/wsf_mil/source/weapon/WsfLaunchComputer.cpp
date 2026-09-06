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

#include "WsfLaunchComputer.hpp"

#include <iomanip>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfPlatform.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"

// static
const double WsfLaunchComputer::cFOREVER = 1.0E8;
// static
const double WsfLaunchComputer::cOUT_OF_REACH = 1.0E+30;
// static
const double WsfLaunchComputer::cUNDEFINED_DOUBLE = 1.0E+30;

namespace
{
const double cNULL_VEC[3]{0.0, 0.0, 0.0};
}

// =================================================================================================
WsfLaunchComputer::WsfLaunchComputer()
   : WsfObject()
   , mWeaponPtr(nullptr)
   , mDebugEnabled(false)
   , mShowResults(false)
   , mComputerGenerationMode(false)
   , mPredictionLockinTime(0.25)
   , mMaxDeltaAltitude(0.0)
   , mMinDeltaAltitude(0.0)
   , mMinOpeningSpeed(0.0)
   , mMaxOpeningSpeed(0.0)
   , mMaxSlantRange(0.0)
   , mMinSlantRange(0.0)
   , mCosineMaxBoresight(-1.0)
   , mMaximumTOF(0.0)
   , mAvgThrustingAccel(0.0)
   , mBurnoutSpeed(0.0)
   , mBurnDuration(0.0)
   , mCoastTime(0.0)
   , mAvgCoastingAccel(0.0)
   , mMinTerminalSpeed(0.0)
   , mLastPredictionTime(1.0E+30)
   , mLastPredictionTrackId()
   , mValidityFlags(0)
   , mLaunchTime(-1.0)
   , mLauncherBearing(0.0)
   , mLauncherElevation(0.0)
   , mLoftAngle(0.0)
   , mBurnTime(0.0)
   , mTimeOfFlight(-1.0)
   , mInterceptTime(-1.0)
   , mInterceptPoint()
   , mTargetImpactTime(-1.0)
   , mTargetImpactPoint()
   , mEndTime(-1.0)
   , mEndPoint()
   , mCommandedLauncherBearing(cUNDEFINED_DOUBLE)
   , mCommandedLauncherElevation(cUNDEFINED_DOUBLE)
   , mCommandedLoftAngle(cUNDEFINED_DOUBLE)
   , mCommandedBurnTime(cUNDEFINED_DOUBLE)
   , mCommandedInterceptPoint(cNULL_VEC)
   , mExpectedFlightDistance(0.0)
   , mCurrentOpeningSpeed(0.0)
   , mThrustingDistance(0.0)
   , mCoastingDistance(0.0)
{
   ClearConstraints();
}

// =================================================================================================
//! Copy constructor.
WsfLaunchComputer::WsfLaunchComputer(const WsfLaunchComputer& aSrc)
   : WsfObject(aSrc)
   , mWeaponPtr(aSrc.mWeaponPtr)
   , mDebugEnabled(aSrc.mDebugEnabled)
   , mShowResults(aSrc.mShowResults)
   , mComputerGenerationMode(aSrc.mComputerGenerationMode)
   , mPredictionLockinTime(aSrc.mPredictionLockinTime)
   , mMaxDeltaAltitude(aSrc.mMaxDeltaAltitude)
   , mMinDeltaAltitude(aSrc.mMinDeltaAltitude)
   , mMinOpeningSpeed(aSrc.mMinOpeningSpeed)
   , mMaxOpeningSpeed(aSrc.mMaxOpeningSpeed)
   , mMaxSlantRange(aSrc.mMaxSlantRange)
   , mMinSlantRange(aSrc.mMinSlantRange)
   , mCosineMaxBoresight(aSrc.mCosineMaxBoresight)
   , mMaximumTOF(aSrc.mMaximumTOF)
   , mAvgThrustingAccel(aSrc.mAvgThrustingAccel)
   , mBurnoutSpeed(aSrc.mBurnoutSpeed)
   , mBurnDuration(aSrc.mBurnDuration)
   , mCoastTime(aSrc.mCoastTime)
   , mAvgCoastingAccel(aSrc.mAvgCoastingAccel)
   , mMinTerminalSpeed(aSrc.mMinTerminalSpeed)
   , mLastPredictionTime(aSrc.mLastPredictionTime)
   , mLastPredictionTrackId(aSrc.mLastPredictionTrackId)
   , mValidityFlags(aSrc.mValidityFlags)
   , mLaunchTime(aSrc.mLaunchTime)
   , mLauncherBearing(aSrc.mLauncherBearing)
   , mLauncherElevation(aSrc.mLauncherElevation)
   , mLoftAngle(aSrc.mLoftAngle)
   , mBurnTime(aSrc.mBurnTime)
   , mTimeOfFlight(aSrc.mTimeOfFlight)
   , mInterceptTime(aSrc.mInterceptTime)
   , mInterceptPoint(aSrc.mInterceptPoint)
   , mTargetImpactTime(aSrc.mTargetImpactTime)
   , mTargetImpactPoint(aSrc.mTargetImpactPoint)
   , mEndTime(aSrc.mEndTime)
   , mEndPoint(aSrc.mEndPoint)
   , mCommandedLauncherBearing(aSrc.mCommandedLauncherBearing)
   , mCommandedLauncherElevation(aSrc.mCommandedLauncherElevation)
   , mCommandedLoftAngle(aSrc.mCommandedLoftAngle)
   , mCommandedBurnTime(aSrc.mCommandedBurnTime)
   , mCommandedInterceptPoint(aSrc.mCommandedInterceptPoint)
   , mExpectedFlightDistance(0.0)
   , mCurrentOpeningSpeed(0.0)
   , mThrustingDistance(aSrc.mThrustingDistance)
   , mCoastingDistance(aSrc.mCoastingDistance)
{
   CopyConstraints(aSrc.mConstraintEnabled);
}

// =================================================================================================
void WsfLaunchComputer::ClearConstraints()
{
   for (int i = 0; i < cNUMBER_OF_CONSTRAINTS; ++i)
   {
      mConstraintEnabled[i] = false;
   }
}

// =================================================================================================
void WsfLaunchComputer::CopyConstraints(const bool aSrc[])
{
   for (int i = 0; i < cNUMBER_OF_CONSTRAINTS; ++i)
   {
      mConstraintEnabled[i] = aSrc[i];
   }
}

// =================================================================================================
// virtual
//! Process input from a generic source.
//!    Examine the current input command.  If it is NOT one of the commands
//!    recognized by this class then it simply returns 'false'.  If it is
//!    one of the commands recognized by this class then it processes the
//!    command and returns 'true'.
//!    @param aInput a reference to a UtInput object.
//!    @returns 'true' if the command was one recognized by the class or 'false' if not.
bool WsfLaunchComputer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   double      value = 0.0;

   if (command == "prediction_lockin_time")
   {
      aInput.ReadValueOfType(mPredictionLockinTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mPredictionLockinTime, 0.0);
   }
   else if (command == "maximum_delta_altitude")
   {
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      SetMaximumDeltaAltitude(value);

      if ((mConstraintEnabled[cMIN_DELTA_ALTITUDE]) && (mMinDeltaAltitude >= mMaxDeltaAltitude))
      {
         throw UtInput::BadValue(aInput, "maximum_delta_altitude must be > minimum_delta_altitude.");
      }
   }
   else if (command == "minimum_delta_altitude")
   {
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      SetMinimumDeltaAltitude(value);

      if ((mConstraintEnabled[cMAX_DELTA_ALTITUDE]) && (mMinDeltaAltitude >= mMaxDeltaAltitude))
      {
         throw UtInput::BadValue(aInput, "minimum_delta_altitude must be < maximum_delta_altitude.");
      }
   }
   else if (command == "maximum_closing_speed")
   {
      aInput.ReadValueOfType(value, UtInput::cSPEED);
      SetMaximumClosingSpeed(value);

      if ((mConstraintEnabled[cMAX_OPENING_SPEED]) && (mMinOpeningSpeed >= mMaxOpeningSpeed))
      {
         throw UtInput::BadValue(aInput, "maximum_closing_speed must be greater than minimum_closing_speed.");
      }
   }
   else if (command == "maximum_opening_speed")
   {
      aInput.ReadValueOfType(value, UtInput::cSPEED);
      SetMaximumOpeningSpeed(value);

      if ((mConstraintEnabled[cMIN_OPENING_SPEED]) && (mMaxOpeningSpeed <= mMinOpeningSpeed))
      {
         throw UtInput::BadValue(aInput, "maximum_opening_speed must be greater than minimum_opening_speed.");
      }
   }
   else if (command == "minimum_closing_speed")
   {
      aInput.ReadValueOfType(value, UtInput::cSPEED);
      SetMinimumClosingSpeed(value);

      if ((mConstraintEnabled[cMIN_OPENING_SPEED]) && (mMaxOpeningSpeed <= mMinOpeningSpeed))
      {
         throw UtInput::BadValue(aInput, "minimum_closing_speed must be less than maximum_closing_speed.");
      }
   }
   else if (command == "minimum_opening_speed")
   {
      aInput.ReadValueOfType(value, UtInput::cSPEED);
      SetMinimumOpeningSpeed(value);

      if ((mConstraintEnabled[cMAX_OPENING_SPEED]) && (mMinOpeningSpeed >= mMaxOpeningSpeed))
      {
         throw UtInput::BadValue(aInput, "maximum_opening_speed must be greater than minimum_opening_speed.");
      }
   }
   else if (command == "maximum_slant_range")
   {
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(value, 1.0);
      SetMaximumSlantRange(value);

      if ((mConstraintEnabled[cMIN_SLANT_RANGE]) && (mMinSlantRange >= mMaxSlantRange))
      {
         throw UtInput::BadValue(aInput, "maximum_slant_range must be > minimum_slant_range.");
      }
   }
   else if (command == "minimum_slant_range")
   {
      aInput.ReadValueOfType(value, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(value, 0.0);
      SetMinimumSlantRange(value);

      if ((mConstraintEnabled[cMAX_SLANT_RANGE]) && (mMinSlantRange >= mMaxSlantRange))
      {
         throw UtInput::BadValue(aInput, "minumum_slant_range must be < maximum_slant_range.");
      }
   }
   else if (command == "maximum_time_of_flight")
   {
      aInput.ReadValueOfType(value, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(value, 0.0);
      SetMaximumTimeOfFlight(value);

      if ((mBurnDuration != 0.0) && (mCoastTime != 0.0))
      {
         throw UtInput::BadValue(aInput, "thrust_duration, coast_duration, maximum_time_of_flight -- pick any two.");
      }
   }
   else if (command == "maximum_boresight_angle")
   {
      aInput.ReadValueOfType(value, UtInput::cANGLE);
      aInput.ValueGreater(value, 0.0);
      SetMaximumOffBoresight(value);
   }
   else if (command == "thrust_duration")
   {
      aInput.ReadValueOfType(value, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(value, 0.0);
      SetThrustDuration(value);

      if ((mMaximumTOF != 0.0) && (mCoastTime != 0.0))
      {
         throw UtInput::BadValue(aInput, "thrust_duration, coast_duration, maximum_time_of_flight -- pick any two.");
      }
   }
   else if (command == "coast_duration")
   {
      aInput.ReadValueOfType(value, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(value, 0.0);
      SetCoastDuration(value);

      if ((mBurnDuration != 0.0) && (mMaximumTOF != 0.0))
      {
         throw UtInput::BadValue(aInput, "thrust_duration, coast_duration, maximum_time_of_flight -- pick any two.");
      }
   }
   else if (command == "burnout_speed")
   {
      aInput.ReadValueOfType(value, UtInput::cSPEED);
      aInput.ValueGreater(value, 0.0);
      SetBurnoutSpeed(value);
   }
   else if (command == "minimum_terminal_speed")
   {
      aInput.ReadValueOfType(value, UtInput::cSPEED);
      aInput.ValueGreater(value, 0.0);
      SetMinTerminalSpeed(value);
   }
   else if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else if (command == "no_debug")
   {
      mDebugEnabled = false;
   }
   else if (command == "commanded_intercept_point")
   {
      double lat;
      double lon;
      double alt;
      aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
      aInput.ReadValueOfType(alt, UtInput::cLENGTH);
      SetCommandedInterceptPoint(WsfGeoPoint(lat, lon, alt));
      // If a low level 'Fire' is called without a track, UpdateResults never gets called to
      // propagate commanded values to the active result. The following is done so low-level
      // tests can be created that want to pass simulated launch computer results to the weapon.
      // This allows one to easily create a test case that replicates a particular condition.
      SetInterceptPoint(WsfGeoPoint(lat, lon, alt));
   }
   else if (command == "commanded_launcher_bearing")
   {
      double bearing;
      aInput.ReadValueOfType(bearing, UtInput::cANGLE);
      aInput.ValueInClosedRange(bearing, -UtMath::cTWO_PI, UtMath::cTWO_PI);
      bearing = UtMath::NormalizeAngleMinusPi_Pi(bearing);
      SetCommandedLauncherBearing(bearing);
      SetLauncherBearing(bearing); // Set note in commanded_intercept_point
   }
   else if (command == "commanded_launcher_elevation")
   {
      double elevation;
      aInput.ReadValueOfType(elevation, UtInput::cANGLE);
      aInput.ValueInClosedRange(elevation, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      SetCommandedLauncherElevation(elevation);
      SetLauncherElevation(elevation); // Set note in commanded_intercept_point
   }
   else if (command == "commanded_loft_angle")
   {
      double loftAngle;
      aInput.ReadValueOfType(loftAngle, UtInput::cANGLE);
      aInput.ValueInClosedRange(loftAngle, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      SetCommandedLoftAngle(loftAngle);
      SetLoftAngle(loftAngle); // Set note in commanded_intercept_point
   }
   else if (command == "commanded_burn_time")
   {
      double burnTime;
      aInput.ReadValueOfType(burnTime, UtInput::cTIME);
      aInput.ValueGreater(burnTime, 0.0);
      SetCommandedBurnTime(burnTime);
      SetBurnTime(burnTime); // Set note in commanded_intercept_point
   }
   else if (command == "computer_generation_mode")
   {
      aInput.ReadValue(mComputerGenerationMode);
   }
   else if (command == "show_results")
   {
      mShowResults = true;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
// virtual
//! Initialize the object.
//! @param aSimTime   The current simulation time.
//! @param aWeaponPtr The reference platform to use with default launch calculations.
//! @return 'true' if successful or 'false' if some sort of error occurred.
bool WsfLaunchComputer::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   bool success = true;
   mWeaponPtr   = aWeaponPtr;
   if (mWeaponPtr == nullptr)
   {
      ut::log::error() << "Weapon pointer was null.";
      success = false;
   }
   if (success && (!ComputerGenerationMode()))
   {
      // If running in simple command mode (e.g.: one or more commanded_xxx commands specified),
      // ensure the base class InitializeTTIData works by filling in the any missing values with
      // something that will allow it to pass. This allows one to easily create a launch computer
      // where the launch data is specified via input rather than by specifying a target (if there
      // is no target then EstimatedTimeToIntercept is not called, so these values are not needed).
      // Otherwise the user is left with having to provide dummy values. This is all due to a
      // poorly constructed base class.

      if (mValidityFlags != 0) // one or more commanded values specified
      {
         // Make sure the base class InitializeTTIData has all the required input
         if ((mMaximumTOF == 0.0) && (mCoastTime == 0.0) && (mBurnDuration == 0.0))
         {
            mBurnDuration = 1.0;
            mMaximumTOF   = 1.1;
         }
         if (mBurnoutSpeed == 0.0)
         {
            mBurnoutSpeed = 10.0;
         }
         if (mMinTerminalSpeed == 0.0)
         {
            mMinTerminalSpeed = 1.0;
         }
      }
      success = InitializeTTIData();
   }
   mShowResults |= DebugEnabled();
   return success;
}

// =================================================================================================
// virtual
//! Initialize Time To Intercept data for the object.
//! If no TTI data is needed, then override method and simply return true.
//! @return 'true' if successful or 'false' if some sort of error occurred.
bool WsfLaunchComputer::InitializeTTIData()
{
   bool success = true;

   if ((mMaximumTOF == 0.0) && (mCoastTime == 0.0) && (mBurnDuration == 0.0))
   {
      auto out = ut::log::error() << "Must specify any 2 of the following:";
      out.AddNote() << "maximum_time_of_flight";
      out.AddNote() << "coast_duration";
      out.AddNote() << "thrust_duration";
      success = false;
   }
   else if ((mMaximumTOF != 0.0) && (mCoastTime != 0.0) && (mBurnDuration == 0.0))
   {
      mBurnDuration = mMaximumTOF - mCoastTime;
      if (mBurnDuration <= 0.0)
      {
         ut::log::error() << "A negative thrust duration was calculated.";
         success = false;
      }
   }
   else if ((mMaximumTOF != 0.0) && (mCoastTime == 0.0) && (mBurnDuration != 0.0))
   {
      mCoastTime = mMaximumTOF - mBurnDuration;
      if (mCoastTime <= 0.0)
      {
         ut::log::error() << "A negative coast duration was calculated.";
         success = false;
      }
   }
   else if ((mMaximumTOF == 0.0) && (mCoastTime != 0.0) && (mBurnDuration != 0.0))
   {
      mMaximumTOF = mBurnDuration + mCoastTime;
   }

   if (mBurnoutSpeed == 0.0)
   {
      ut::log::error() << "Burnout speed was not provided.";
      success = false;
   }
   else if (mMinTerminalSpeed == 0.0)
   {
      ut::log::error() << "Minimum terminal speed was not provided.";
      success = false;
   }
   else if (success)
   {
      mAvgThrustingAccel = mBurnoutSpeed / mBurnDuration;
      mAvgCoastingAccel  = (mBurnoutSpeed - mMinTerminalSpeed) / mCoastTime;

      mThrustingDistance = 0.5 * mBurnDuration * mBurnoutSpeed;
      mCoastingDistance  = 0.5 * mCoastTime * (mBurnoutSpeed + mMinTerminalSpeed);

      if (mMaxSlantRange != 0.0)
      {
         if (DebugEnabled())
         {
            double sTotal = mThrustingDistance + mCoastingDistance;
            auto   out    = ut::log::debug()
                       << "FYI: WsfLaunchComputer has calculated max range versus a stated maximum_slant_range.";
            out.AddNote() << "Max Range: " << sTotal;
            out.AddNote() << "maximum_slant_range: " << mMaxSlantRange;
            out.AddNote() << "Is this OK?";
         }
      }
   }

   return success;
}

// =================================================================================================
// virtual
void WsfLaunchComputer::Update(double /*aSimTime*/) {}

// =================================================================================================
//! Create a copy, or clone, of "this" object.
//! @return A pointer to the cloned object.
// virtual
WsfLaunchComputer* WsfLaunchComputer::Clone() const
{
   return new WsfLaunchComputer(*this);
}

// =================================================================================================
// Required script upcasting works
const char* WsfLaunchComputer::GetScriptClassName() const
{
   return "WsfLaunchComputer";
}

// =================================================================================================
void WsfLaunchComputer::SetMaximumDeltaAltitude(double aMaxDeltaAltitude)
{
   mMaxDeltaAltitude                       = aMaxDeltaAltitude;
   mConstraintEnabled[cMAX_DELTA_ALTITUDE] = true;
}

// =================================================================================================
void WsfLaunchComputer::SetMinimumDeltaAltitude(double aMinDeltaAltitude)
{
   mMinDeltaAltitude                       = aMinDeltaAltitude;
   mConstraintEnabled[cMIN_DELTA_ALTITUDE] = true;
}

// =================================================================================================
void WsfLaunchComputer::SetMaximumClosingSpeed(double aMaxClosingSpeed)
{
   mMinOpeningSpeed                       = -aMaxClosingSpeed;
   mConstraintEnabled[cMIN_OPENING_SPEED] = true;
}

// =================================================================================================
void WsfLaunchComputer::SetMaximumOpeningSpeed(double aMaxOpeningSpeed)
{
   mMaxOpeningSpeed                       = aMaxOpeningSpeed;
   mConstraintEnabled[cMAX_OPENING_SPEED] = true;
}

// =================================================================================================
void WsfLaunchComputer::SetMinimumClosingSpeed(double aMinClosingSpeed)
{
   mMaxOpeningSpeed                       = -aMinClosingSpeed;
   mConstraintEnabled[cMAX_OPENING_SPEED] = true;
}

// =================================================================================================
void WsfLaunchComputer::SetMinimumOpeningSpeed(double aMinOpeningSpeed)
{
   mMinOpeningSpeed                       = aMinOpeningSpeed;
   mConstraintEnabled[cMIN_OPENING_SPEED] = true;
}

// =================================================================================================
void WsfLaunchComputer::SetMaximumSlantRange(double aMaxRange)
{
   mMaxSlantRange                       = fabs(aMaxRange);
   mConstraintEnabled[cMAX_SLANT_RANGE] = true;
}

// =================================================================================================
void WsfLaunchComputer::SetMinimumSlantRange(double aMinRange)
{
   mMinSlantRange                       = fabs(aMinRange);
   mConstraintEnabled[cMIN_SLANT_RANGE] = true;
}

// =================================================================================================
void WsfLaunchComputer::SetMaximumOffBoresight(double aMaxAngle)
{
   mCosineMaxBoresight                      = cos(aMaxAngle);
   mConstraintEnabled[cMAX_BORESIGHT_ANGLE] = true;
}

// =================================================================================================
//! Check the target altitude relative to the launch altitude for constraints.
//! @return True indicates either the constraint was not set, or the constraint was passed.
bool WsfLaunchComputer::DeltaAltitudeCheck(double aDeltaAltitude) const
{
   bool pass = true;
   if ((mConstraintEnabled[cMAX_DELTA_ALTITUDE]) && (aDeltaAltitude > mMaxDeltaAltitude))
   {
      pass = false;
   }
   else if ((mConstraintEnabled[cMIN_DELTA_ALTITUDE]) && (aDeltaAltitude < mMinDeltaAltitude))
   {
      pass = false;
   }
   return pass;
}

// =================================================================================================
//! Check the target range relative to min and max constraints.
//! @return True indicates either the constraint was not set, or the constraint was passed.
bool WsfLaunchComputer::SlantRangeCheck(double aSlantRange) const
{
   bool pass = true;
   if ((mConstraintEnabled[cMAX_SLANT_RANGE]) && (aSlantRange > mMaxSlantRange))
   {
      pass = false;
   }
   else if ((mConstraintEnabled[cMIN_SLANT_RANGE]) && (aSlantRange < mMinSlantRange))
   {
      pass = false;
   }
   return pass;
}

// =================================================================================================
//! Check the target boresight angle relative to the launcher.
//! @return True indicates either the constraint was not set, or the constraint passed.
bool WsfLaunchComputer::BoresightAngleCheck(WsfPlatform* aPlatPtr, double aLocWCS[3]) const
{
   bool pass = true;
   if (mConstraintEnabled[cMAX_BORESIGHT_ANGLE])
   {
      double boresight[3];
      aPlatPtr->ConvertWCSToECS(aLocWCS, boresight);
      UtVec3d::Normalize(boresight);
      if (boresight[0] < mCosineMaxBoresight)
      {
         pass = false;
      }
   }
   return pass;
}

// =================================================================================================
//! Evaluate whether the supplied WsfTrack currently passes the supplied constraints.
//! @param aSimTime             Current time
//! @param aFutureInterceptTime Predicted future time of intercept (aSimTime plus predicted
//!                             time of flight of the weapon).
//! @param aTrack               Track to be extrapolated for intercept
//! @return true Indicates within constraints (intercept believed attainable)
// virtual
bool WsfLaunchComputer::WithinConstraintsAt(double aSimTime, double aFutureInterceptTime, const WsfTrack& aTrack)
{
   // There are a host of things that must be true before a weapon
   // launch can take place.  To save time, check the easiest items
   // first, then continue on to the more computationally intense tests.

   GetPlatform()->Update(aSimTime);

   double launchHeading;
   double launchPitch;
   double launchRoll;
   GetPlatform()->GetOrientationNED(launchHeading, launchPitch, launchRoll);

   double launchLat;
   double launchLon;
   double launchAlt;
   GetPlatform()->GetLocationLLA(launchLat, launchLon, launchAlt);

   double launchLocWCS[3];
   GetPlatform()->GetLocationWCS(launchLocWCS);

   double futureTgtLocWCS[3];

   mCurrentOpeningSpeed    = 0.0;
   mExpectedFlightDistance = 0.0;

   if (!aTrack.GetExtrapolatedLocationWCS(aFutureInterceptTime, futureTgtLocWCS))
   {
      return false;
   }

   double targetLocNED[3];
   GetPlatform()->ConvertWCSToNED(futureTgtLocWCS, targetLocNED);

   double futureTgtOffsetWCS[3];
   double tgtLat;
   double tgtLon;
   double tgtAlt;
   UtEntity::ConvertWCSToLLA(futureTgtLocWCS, tgtLat, tgtLon, tgtAlt);

   if (!DeltaAltitudeCheck(tgtAlt - launchAlt))
   {
      return false;
   }

   UtVec3d::Subtract(futureTgtOffsetWCS, futureTgtLocWCS, launchLocWCS);
   mExpectedFlightDistance = UtVec3d::Magnitude(futureTgtOffsetWCS);

   if (!SlantRangeCheck(mExpectedFlightDistance))
   {
      return false;
   }

   if (!BoresightAngleCheck(GetPlatform(), futureTgtLocWCS))
   {
      return false;
   }

   //       Calculate a closing speed if possible.
   // (Ignore the constraint if the Velocity is unknown.)
   if ((aTrack.VelocityValid()) && ((mConstraintEnabled[cMIN_OPENING_SPEED]) || (mConstraintEnabled[cMAX_OPENING_SPEED])))
   {
      double myVelWCS[3];
      GetPlatform()->GetVelocityWCS(myVelWCS);

      double trackVelWCS[3];
      aTrack.GetVelocityWCS(trackVelWCS);

      double relativeVelWCS[3];
      UtVec3d::Subtract(relativeVelWCS, myVelWCS, trackVelWCS);

      double mag           = UtVec3d::Magnitude(futureTgtOffsetWCS);
      mCurrentOpeningSpeed = UtVec3d::DotProduct(relativeVelWCS, futureTgtOffsetWCS) / mag;

      if ((mConstraintEnabled[cMIN_OPENING_SPEED]) && (mCurrentOpeningSpeed < mMinOpeningSpeed))
      {
         return false;
      }

      if ((mConstraintEnabled[cMAX_OPENING_SPEED]) && (mCurrentOpeningSpeed > mMaxOpeningSpeed))
      {
         return false;
      }
   }

   return true;
}

// =================================================================================================
//! Determine the estimated time for the weapon to intercept a target.
//! @param aSimTime        The current simulation time.
//! @param aTrack          The track that represents the target.
//! @param aLaunchDelayTime The time it takes until the weapon will be launched.
//! @return The estimated time to intercept the target, or cFOREVER if no intercept is possible.
// virtual
double WsfLaunchComputer::EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime)
{
   // If launch computer generation mode, simply use the commanded values.
   if (ComputerGenerationMode())
   {
      InitializeResults(aSimTime, aTrack);
      UpdateResults(aSimTime, aLaunchDelayTime, aTrack);
      return aLaunchDelayTime;
   }

   InitializeResults(aSimTime, aTrack);

   // Note that WithinConstraintsAt() will populate mExpectedFlightDistance if possible.

   // FIRST ITERATION (ignore return value):
   double estTTI = 0.0;
   WithinConstraintsAt(aSimTime, aSimTime + aLaunchDelayTime + estTTI, aTrack);

   if (mExpectedFlightDistance <= mThrustingDistance)
   {
      estTTI = sqrt(2.0 * mExpectedFlightDistance / mAvgThrustingAccel);
   }
   else if (mExpectedFlightDistance <= (mThrustingDistance + mCoastingDistance))
   {
      double coastingRange = mExpectedFlightDistance - mThrustingDistance;
      estTTI               = mBurnDuration + sqrt(2.0 * coastingRange / mAvgCoastingAccel);
   }

   // Now go through the calculations AGAIN, accounting for track
   // extrapolation by the estimated time to intercept.
   // This will improve the accuracy of the estimate:
   double timeToIntercept = cFOREVER;
   if (WithinConstraintsAt(aSimTime, aSimTime + aLaunchDelayTime + estTTI, aTrack))
   {
      if (mExpectedFlightDistance <= mThrustingDistance)
      {
         timeToIntercept = sqrt(2.0 * mExpectedFlightDistance / mAvgThrustingAccel);
      }
      else if (mExpectedFlightDistance <= (mThrustingDistance + mCoastingDistance))
      {
         double coastingRange = mExpectedFlightDistance - mThrustingDistance;
         timeToIntercept      = mBurnDuration + sqrt(2.0 * coastingRange / mAvgThrustingAccel);
      }
   }

   UpdateResults(aSimTime, timeToIntercept, aTrack);
   return timeToIntercept;
}

// =================================================================================================
//! Copy intercept prediction results to the specified track.
//! This is a helper method that can be used to copy results to a track where they can be picked up
//! by a weapon (either being launched or inflight). The static Get() methods can be used to extract
//! the values from the track.
void WsfLaunchComputer::CopyResultsToTrack(WsfTrack& aTrack)
{
   if ((!HaveResultsFor(aTrack.GetTrackId())) && (!ComputerGenerationMode()))
   {
      return;
   }

   WsfAttributeContainer& auxData = aTrack.GetAuxData();
   if (LauncherBearingIsValid())
   {
      auxData.Assign("LC_LAUNCHER_AZIMUTH", mLauncherBearing * UtMath::cDEG_PER_RAD);
   }
   if (LauncherElevationIsValid())
   {
      auxData.Assign("LC_LAUNCHER_ELEVATION", mLauncherElevation * UtMath::cDEG_PER_RAD);
   }
   if (LoftAngleIsValid())
   {
      auxData.Assign("LC_LOFT_ANGLE", mLoftAngle * UtMath::cDEG_PER_RAD);
   }
   if (BurnTimeIsValid())
   {
      auxData.Assign("LC_BURN_TIME", mBurnTime);
   }
   if (InterceptPointIsValid())
   {
      auxData.Assign("LC_INTERCEPT_LAT", mInterceptPoint.GetLat());
      auxData.Assign("LC_INTERCEPT_LON", mInterceptPoint.GetLon());
      auxData.Assign("LC_INTERCEPT_ALT", mInterceptPoint.GetAlt());
   }
   if (InterceptTimeIsValid())
   {
      auxData.Assign("LC_INTERCEPT_TIME", mInterceptTime);
   }
   if (EndPointIsValid())
   {
      auxData.Assign("LC_END_LAT", mEndPoint.GetLat());
      auxData.Assign("LC_END_LON", mEndPoint.GetLon());
      auxData.Assign("LC_END_ALT", mEndPoint.GetAlt());
   }
   if (EndTimeIsValid())
   {
      auxData.Assign("LC_END_TIME", mEndTime);
   }
}

// =================================================================================================
bool WsfLaunchComputer::HaveResultsFor(double aSimTime, const WsfTrackId& aTrackId, double& aTimeToIntercept) const
{
   aTimeToIntercept = cFOREVER;
   bool haveResults = false;
   if ((mValidityFlags != 0) && ((aSimTime - mLastPredictionTime) < mPredictionLockinTime) &&
       (aTrackId == mLastPredictionTrackId))
   {
      aTimeToIntercept = std::max(0.0, mInterceptTime - aSimTime);
      haveResults      = true;
   }
   return haveResults;
}

// =================================================================================================
bool WsfLaunchComputer::GetLauncherElevation(const WsfTrack& aTrack, double& aElevation)
{
   return GetDataValue(aTrack, "LC_LAUNCHER_ELEVATION", aElevation, UtMath::cRAD_PER_DEG);
}

// =================================================================================================
bool WsfLaunchComputer::GetLauncherBearing(const WsfTrack& aTrack, double& aAzimuth)
{
   return GetDataValue(aTrack, "LC_LAUNCHER_AZIMUTH", aAzimuth, UtMath::cRAD_PER_DEG);
}

// =================================================================================================
bool WsfLaunchComputer::GetLoftAngle(const WsfTrack& aTrack, double& aLoftAngle)
{
   return GetDataValue(aTrack, "LC_LOFT_ANGLE", aLoftAngle, UtMath::cRAD_PER_DEG);
}

// =================================================================================================
bool WsfLaunchComputer::GetBurnTime(const WsfTrack& aTrack, double& aBurnTime)
{
   return GetDataValue(aTrack, "LC_BURN_TIME", aBurnTime, 1.0);
}

// =================================================================================================
bool WsfLaunchComputer::GetInterceptTime(const WsfTrack& aTrack, double& aInterceptTime)
{
   return GetDataValue(aTrack, "LC_INTERCEPT_TIME", aInterceptTime, 1.0);
}

// =================================================================================================
bool WsfLaunchComputer::GetInterceptPoint(const WsfTrack& aTrack, WsfGeoPoint& aPoint)
{
   double lat     = 0.0;
   double lon     = 0.0;
   double alt     = 0.0;
   bool   isValid = GetDataValue(aTrack, "LC_INTERCEPT_LAT", lat, 1.0) &&
                  GetDataValue(aTrack, "LC_INTERCEPT_LON", lon, 1.0) &&
                  GetDataValue(aTrack, "LC_INTERCEPT_ALT", alt, 1.0);
   if (isValid)
   {
      aPoint.SetLocationLLA(lat, lon, alt);
   }
   return isValid;
}

// =================================================================================================
// protected static
bool WsfLaunchComputer::GetDataValue(const WsfTrack& aTrack, const std::string& aName, double& aValue, double aMultiplier)
{
   bool isValid = aTrack.HasAuxData();
   if (isValid)
   {
      isValid = aTrack.GetAuxDataConst().AttributeExists(aName);
      if (isValid)
      {
         aValue = aTrack.GetAuxDataConst().GetDouble(aName) * aMultiplier;
      }
   }
   return isValid;
}

// =================================================================================================
// protected
void WsfLaunchComputer::InitializeResults(double aSimTime, const WsfTrack& aTrack)
{
   mLastPredictionTime    = aSimTime;
   mLastPredictionTrackId = aTrack.GetTrackId(); // The track for whom the intercept data is being computed
   mValidityFlags         = 0;                   // No data is valid
}

// =================================================================================================
// protected
void WsfLaunchComputer::UpdateResults(double aSimTime, double aTimeToIntercept, const WsfTrack& aTrack)
{
   // If an intercept has been computed or if in computer generation mode,
   // override or provide results with the commanded values.

   if ((aTimeToIntercept < cFOREVER) || ComputerGenerationMode())
   {
      if (!mComputerGenerationMode)
      {
         mInterceptTime = aSimTime + aTimeToIntercept;
      }

      double interceptLocWCS[3];
      mCommandedInterceptPoint.GetLocationWCS(interceptLocWCS);
      if (UtVec3d::MagnitudeSquared(interceptLocWCS) != 0.0)
      {
         mInterceptPoint = mCommandedInterceptPoint;
         mValidityFlags |= cVF_INTERCEPT_POINT;
      }
      if (mCommandedLauncherBearing != cUNDEFINED_DOUBLE)
      {
         mLauncherBearing = mCommandedLauncherBearing;
         mValidityFlags |= cVF_LAUNCHER_BEARING;
      }
      if (mCommandedLauncherElevation != cUNDEFINED_DOUBLE)
      {
         mLauncherElevation = mCommandedLauncherElevation;
         mValidityFlags |= cVF_LAUNCHER_ELEVATION;
      }
      if (mCommandedLoftAngle != cUNDEFINED_DOUBLE)
      {
         mLoftAngle = mCommandedLoftAngle;
         mValidityFlags |= cVF_LOFT_ANGLE;
      }
      if (mCommandedBurnTime != cUNDEFINED_DOUBLE)
      {
         mBurnTime = mCommandedBurnTime;
         mValidityFlags |= cVF_BURN_TIME;
      }

      if (ShowResults())
      {
         auto out = ut::log::info() << std::fixed << "Launch computer solution:";

         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         if (LaunchTimeIsValid())
         {
            out.AddNote() << "Launch Time: " << std::setprecision(4) << mLaunchTime << " sec ("
                          << mLaunchTime - aSimTime << " sec. from now)";
         }
         if (LauncherBearingIsValid())
         {
            out.AddNote() << "Launcher Bearing: " << std::setprecision(4) << mLauncherBearing * UtMath::cDEG_PER_RAD
                          << " deg";
         }
         if (LauncherElevationIsValid())
         {
            out.AddNote() << "Launcher Elevation: " << std::setprecision(4) << mLauncherElevation * UtMath::cDEG_PER_RAD
                          << " deg";
         }
         if (LoftAngleIsValid())
         {
            out.AddNote() << "Loft: " << std::setprecision(4) << mLoftAngle * UtMath::cDEG_PER_RAD << " deg";
         }
         if (BurnTimeIsValid())
         {
            out.AddNote() << "Burn Time: " << std::setprecision(4) << mBurnTime << " sec";
         }
         if (TimeOfFlightIsValid())
         {
            out.AddNote() << "Time of Flight: " << std::setprecision(4) << mTimeOfFlight << " sec";
         }
         if (InterceptTimeIsValid())
         {
            out.AddNote() << "Predicted Intercept Time: " << std::setprecision(4) << mInterceptTime << " sec ("
                          << mInterceptTime - aSimTime << " sec. from now)";
         }
         if (InterceptPointIsValid())
         {
            auto note = out.AddNote() << "Predicted Intercept Point:";
            note.AddNote() << "Lat: " << UtLatPos(mInterceptPoint.GetLat(), UtAngle::FmtDMS + 2);
            note.AddNote() << "Log: " << UtLonPos(mInterceptPoint.GetLon(), UtAngle::FmtDMS + 2);
            note.AddNote() << "Alt: " << mInterceptPoint.GetAlt() << " m msl";
         }
         if (TargetImpactTimeIsValid())
         {
            out.AddNote() << "Predicted Target Impact Time: " << std::setprecision(4) << mTargetImpactTime << " sec ("
                          << mInterceptTime - aSimTime << " sec. from now)";
         }
         if (TargetImpactPointIsValid())
         {
            auto note = out.AddNote() << "Predict Target Impact Point:";
            note.AddNote() << "Lat: " << UtLatPos(mTargetImpactPoint.GetLat(), UtAngle::FmtDMS + 2);
            note.AddNote() << "Lon: " << UtLonPos(mTargetImpactPoint.GetLon(), UtAngle::FmtDMS + 2);
            note.AddNote() << "Alt: " << mTargetImpactPoint.GetAlt() << " m msl";
         }
         if (EndTimeIsValid())
         {
            out.AddNote() << "Predicted End Time: " << std::setprecision(4) << mEndTime << " sec ("
                          << mInterceptTime - aSimTime << " sec. from now)";
         }
         if (EndPointIsValid())
         {
            auto note = out.AddNote() << "Predicted End Point:";
            note.AddNote() << "Lat: " << UtLatPos(mEndPoint.GetLat(), UtAngle::FmtDMS + 2);
            note.AddNote() << "Lon: " << UtLonPos(mEndPoint.GetLon(), UtAngle::FmtDMS + 2);
            note.AddNote() << "Alt: " << mEndPoint.GetAlt() << " m msl";
         }
      }
   }
}

WsfSimulation* WsfLaunchComputer::GetSimulation() const
{
   return mWeaponPtr != nullptr ? mWeaponPtr->GetSimulation() : nullptr;
}
