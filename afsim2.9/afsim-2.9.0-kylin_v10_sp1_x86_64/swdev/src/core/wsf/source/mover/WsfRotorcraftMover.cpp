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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfRotorcraftMover.hpp"

#include <assert.h>
#include <string>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3dX.hpp"
#include "WsfEnvironment.hpp"
#include "WsfModeList.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"

namespace
{
double Sign(double aValue)
{
   return (aValue >= 0.0) ? 1.0 : -1.0;
}
const double cGRAVITY                         = -UtEarth::cACCEL_OF_GRAVITY;
const double cWAYPOINT_CAPTURE_RADIUS         = 30.0;
const double cPOSITION_HOLD_CAPTURE_RADIUS    = 30.0;
const double cPOSITION_HOLD_CAPTURE_RADIUS_SQ = cPOSITION_HOLD_CAPTURE_RADIUS * cPOSITION_HOLD_CAPTURE_RADIUS;
} // namespace

// =================================================================
WsfRotorcraftMover::WsfRotorcraftMover(WsfScenario& aScenario)
   : WsfIterativeRouteMover(aScenario)
   , mFlightIndex(0)
   , mStartModeId()
   , mIsWeatherCocking(false)
   , mNeedToClimb(true)
   , mIsAltitudeHold(false)
   , mIsPositionHold(false)
   , mRotorcraftModeList()
   , mLocalGuidance()
   , mLocalUpdateTime(0.0)
   , mCourseRad(0.0)
   , mAltErrToROC_Gain(1.0)
   , mFabsROC_Limit(0.0)
   , mHeadingRad(0.0)
   , mHeadingErrorRad(0.0)
   , mSouthAndEastTipTimeConstant(1.0)
   , mPosHoldCaptureRadius(200.0)
   , mDesiredAccelUp(0.0)
   , mDesiredAccelNED()
   , mDesiredVelNED()
   , mVelNED()
   , mAccelNED()
   , mNetAccelNED()
   , mNetAccelWCS()
   , mPrevNetAccelWCS()
   , mVelWCS()
   , mPrevVelWCS()
   , mLocWCS()
   , mPrevLocWCS()
   , mNED_Error()
   , mVelNED_Error()
   , mSouthTipFilter()
   , mEastTipFilter()
   , mDcmWCStoNED()
   , mDcmNEDtoDisk()
   , mDcmDiskToECS()
   , mDcmNEDtoECS()
   , mNorthAccelValuePID()
   , mEastAccelValuePID()
   , mVerticalAccelValuePID()
   , mNorthAccelRatePID()
   , mEastAccelRatePID()
   , mVerticalAccelRatePID()
   , mWPP_GuidancePtr(nullptr)
   , mModePtr(nullptr)
   , mLatitudeDeg(0.0)
   , mLongitudeDeg(0.0)
   , mAltitude(0.0)
   , mAltitudeRate(0.0)
   , mGroundSpeed(0.0)
   , mAltitudeError(0.0)
   , mTimeToTargetAlt(0.0)
   , mDesiredROC(0.0)
   , mReductionRatio(0.0)
   , mDistToTgtLL(0.0)
   , mTimeToTgtLL(0.0)
{
   // Create the mode list with the mover-specific mode template.
   SetModeList(new WsfModeList(new RotorcraftMode()));
   mWPP_GuidancePtr = new WsfWaypointPathGuidance();
   SetGuidance(mWPP_GuidancePtr);

   // Provide defaults for PIDs, in case none
   // are specified in stream input file:
   UtPID_Controller temp;

   // Lateral RATE PID:
   temp.SetProportionalGain(2.0);
   temp.SetDerivativeGain(0.5);
   mNorthAccelRatePID = temp;
   mEastAccelRatePID  = temp;

   // Lateral VALUE PID:
   temp.SetProportionalGain(0.02);
   temp.SetDerivativeGain(20.0);
   mNorthAccelValuePID = temp;
   mEastAccelValuePID  = temp;

   // Vertical RATE PID:
   temp.SetProportionalGain(0.025);
   temp.SetDerivativeGain(2.0);
   mVerticalAccelRatePID = temp;

   // Vertical VALUE PID:
   temp.SetProportionalGain(0.025);
   temp.SetDerivativeGain(12.0);
   mVerticalAccelValuePID = temp;
}

// =================================================================
WsfRotorcraftMover::WsfRotorcraftMover(const WsfRotorcraftMover& aSrc)
   : WsfIterativeRouteMover(aSrc)
   , mFlightIndex(aSrc.mFlightIndex)
   , mStartModeId(aSrc.mStartModeId)
   , mIsWeatherCocking(aSrc.mIsWeatherCocking)
   , mNeedToClimb(aSrc.mNeedToClimb)
   , mIsAltitudeHold(aSrc.mIsAltitudeHold)
   , mIsPositionHold(aSrc.mIsPositionHold)
   , mRotorcraftModeList()
   , mLocalUpdateTime(aSrc.mLocalUpdateTime)
   , mCourseRad(aSrc.mCourseRad)
   , mAltErrToROC_Gain(aSrc.mAltErrToROC_Gain)
   , mFabsROC_Limit(aSrc.mFabsROC_Limit)
   , mHeadingRad(aSrc.mHeadingRad)
   , mHeadingErrorRad(aSrc.mHeadingErrorRad)
   , mSouthAndEastTipTimeConstant(aSrc.mSouthAndEastTipTimeConstant)
   , mPosHoldCaptureRadius(aSrc.mPosHoldCaptureRadius)
   , mDesiredAccelUp(aSrc.mDesiredAccelUp)
   , mDesiredAccelNED(aSrc.mDesiredAccelNED)
   , mDesiredVelNED(aSrc.mDesiredVelNED)
   , mVelNED(aSrc.mVelNED)
   , mAccelNED()
   , mNetAccelNED()
   , mNetAccelWCS(aSrc.mNetAccelWCS)
   , mPrevNetAccelWCS(aSrc.mPrevNetAccelWCS)
   , mVelWCS(aSrc.mVelWCS)
   , mPrevVelWCS(aSrc.mPrevVelWCS)
   , mLocWCS(aSrc.mLocWCS)
   , mPrevLocWCS()
   , mNED_Error()
   , mVelNED_Error()
   , mSouthTipFilter(aSrc.mSouthTipFilter)
   , mEastTipFilter(aSrc.mEastTipFilter)
   , mDcmWCStoNED(aSrc.mDcmWCStoNED)
   , mDcmNEDtoDisk(aSrc.mDcmNEDtoDisk)
   , mDcmDiskToECS(aSrc.mDcmDiskToECS)
   , mDcmNEDtoECS(aSrc.mDcmNEDtoECS)
   , mNorthAccelValuePID(aSrc.mNorthAccelValuePID)
   , mEastAccelValuePID(aSrc.mEastAccelValuePID)
   , mVerticalAccelValuePID(aSrc.mVerticalAccelValuePID)
   , mNorthAccelRatePID(aSrc.mNorthAccelRatePID)
   , mEastAccelRatePID(aSrc.mEastAccelRatePID)
   , mVerticalAccelRatePID(aSrc.mVerticalAccelRatePID)
   , mWPP_GuidancePtr(nullptr)
   , mModePtr(nullptr)
   , mLatitudeDeg(0.0)
   , mLongitudeDeg(0.0)
   , mAltitude(0.0)
   , mAltitudeRate(0.0)
   , mGroundSpeed(0.0)
   , mAltitudeError(0.0)
   , mTimeToTargetAlt(0.0)
   , mDesiredROC(0.0)
   , mReductionRatio(0.0)
   , mDistToTgtLL(0.0)
   , mTimeToTgtLL(0.0)
{
   mWPP_GuidancePtr = dynamic_cast<WsfWaypointPathGuidance*>(GetGuidance());
}

// =================================================================
WsfRotorcraftMover::~WsfRotorcraftMover() {}

// =================================================================
// == PROVIDE REASONABLE DEFAULTS FOR A ROTORCRAFT ==
double WsfRotorcraftMover::MaximumGroundSpeed() const
{
   if (mModePtr == nullptr)
   {
      return 100.0;
   }
   else
   {
      return mModePtr->mMaximumGroundSpeed;
   }
}

// =================================================================
double WsfRotorcraftMover::MaximumAcceleration() const
{
   if (mModePtr == nullptr)
   {
      return 3.0 * UtEarth::cACCEL_OF_GRAVITY;
   }
   else
   {
      return mModePtr->mMaximumTotalAccel;
   }
}

// =================================================================
double WsfRotorcraftMover::MaximumClimbRate() const
{
   if (mModePtr == nullptr)
   {
      return 50;
   }
   else
   {
      return mModePtr->mMaximumRateOfClimb;
   }
}

// =================================================================
double WsfRotorcraftMover::MaximumDescentRate() const
{
   if (mModePtr == nullptr)
   {
      return 40;
   }
   else
   {
      return mModePtr->mMaximumRateOfDescent;
   }
}

// =================================================================
double WsfRotorcraftMover::MinimumUpwardAccel() const
{
   if (mModePtr == nullptr)
   {
      return 0.0;
   }
   else
   {
      return mModePtr->mMinimumUpwardAccel;
   }
}

// =================================================================
double WsfRotorcraftMover::BodyRatesGain() const
{
   if (mModePtr == nullptr)
   {
      return 2.0;
   }
   else
   {
      return mModePtr->mBodyRatesGain;
   }
}

// =================================================================
double WsfRotorcraftMover::WeathercockSpeed() const
{
   if (mModePtr == nullptr)
   {
      return 7.0;
   }
   else
   {
      return mModePtr->mWeathercockSpeed;
   }
}

// =================================================================
double WsfRotorcraftMover::MaximumBodyRate() const
{
   if (mModePtr == nullptr)
   {
      return 3.0;
   }
   else
   {
      return mModePtr->mMaximumAttitudeRate;
   }
}

// =================================================================
// virtual
bool WsfRotorcraftMover::GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute)
{
   double targetSpeed = fabs(aSpeed);
   if (mModePtr != nullptr)
   {
      double constraint = mModePtr->mMaximumGroundSpeed;
      if (constraint < targetSpeed)
      {
         targetSpeed = constraint;
      }
   }
   mLocalGuidance.mTargetGroundSpeed = targetSpeed;
   return WsfIterativeRouteMover::GoToSpeed(aSimTime, targetSpeed, aLinearAccel, aKeepRoute);
}

// =================================================================
// virtual
bool WsfRotorcraftMover::GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute)
{
   mLocalGuidance.mTargetAltitude = aAltitude;
   return WsfIterativeRouteMover::GoToAltitude(aSimTime, aAltitude, aClimbDiveRate, aKeepRoute);
}

// =================================================================
bool WsfRotorcraftMover::Unpause(double aSimTime, bool aInterrupt)
{
   if (IsPaused())
   {
      if (WsfIterativeRouteMover::Unpause(aSimTime, aInterrupt))
      {
         mLocalUpdateTime = aSimTime;
         return true;
      }
   }
   return false;
}

// =================================================================
void WsfRotorcraftMover::SetPositionHold(double aSimTime, bool aValue)
{
   if (aValue == mIsPositionHold)
   {
      return;
   }

   // If we change from Off to On or vice versa, reset the Filter
   mSouthTipFilter.Reset(aSimTime);
   mEastTipFilter.Reset(aSimTime);

   mIsPositionHold = aValue;
}

// =================================================================
void WsfRotorcraftMover::SetAltitudeHold(double aSimTime, bool aValue)
{
   if (aValue == mIsAltitudeHold)
   {
      return;
   }
   mIsAltitudeHold = aValue;
}

// =================================================================
// virtual
bool WsfRotorcraftMover::Initialize(double aSimTime)
{
   bool success = WsfIterativeRouteMover::Initialize(aSimTime);

   // === Initialize all State values: ===
   GetPlatform()->GetLocationLLA(mLatitudeDeg, mLongitudeDeg, mAltitude);

   double tempDCM[3][3];
   UtEllipsoidalEarth::ComputeNEDTransform(mLatitudeDeg, mLongitudeDeg, mAltitude, tempDCM, mLocWCS.GetData());
   mDcmWCStoNED.Set(tempDCM);

   double pitch, roll;
   GetPlatform()->GetOrientationNED(mHeadingRad, pitch, roll);
   mDcmDiskToECS.Set(mHeadingRad, 0.0, 0.0);

   // The Platform does not accept initial velocity information.
   // Always use Zero as starting velocity.
   mVelNED.Set(0.0, 0.0, 0.0);
   mVelWCS = mDcmWCStoNED.InverseTransform(mVelNED);

   mGroundSpeed     = 0.0;
   mCourseRad       = 0.0;
   mAltitudeRate    = 0;
   mLocalUpdateTime = aSimTime;

   mSouthTipFilter.SetTimeConstant(mSouthAndEastTipTimeConstant);
   mEastTipFilter.SetTimeConstant(mSouthAndEastTipTimeConstant);

   mSouthTipFilter.Reset(aSimTime, 0.0);
   mEastTipFilter.Reset(aSimTime, 0.0);

   if (success)
   {
      success = mVerticalAccelRatePID.Initialize(aSimTime, GetUpdateInterval()) &&
                mNorthAccelRatePID.Initialize(aSimTime, GetUpdateInterval()) &&
                mEastAccelRatePID.Initialize(aSimTime, GetUpdateInterval()) &&
                mVerticalAccelValuePID.Initialize(aSimTime, GetUpdateInterval()) &&
                mNorthAccelValuePID.Initialize(aSimTime, GetUpdateInterval()) &&
                mEastAccelValuePID.Initialize(aSimTime, GetUpdateInterval());
   }

   // Reduce future dynamic casting by extracting derived class mode pointers.
   GetModeList()->GetDerivedModeList(mRotorcraftModeList);

   SetMode(mStartModeId);

   return success;
}

// =================================================================
// virtual
bool WsfRotorcraftMover::Initialize2(double aSimTime)
{
   bool success = WsfIterativeRouteMover::Initialize2(aSimTime);

   return success;
}

// =================================================================
// virtual
WsfMover* WsfRotorcraftMover::Clone() const
{
   return new WsfRotorcraftMover(*this);
}

// =================================================================
void WsfRotorcraftMover::SetModeId_P(WsfStringId aModeId)
{
   auto it = std::find_if(mRotorcraftModeList.begin(),
                          mRotorcraftModeList.end(),
                          [aModeId](const RotorcraftMode* aModePtr) { return aModePtr->GetNameId() == aModeId; });
   if (it != std::end(mRotorcraftModeList))
   {
      mModePtr = *it;
   }

   if (mModePtr != nullptr)
   {
      // Set motion attributes that are consistent with a named mode.
      if (mModePtr->mMaximumGroundSpeed != RotorcraftMode::cNOT_SET)
      {
         // Debatable... do we always want to go as fast as possible???
         mLocalGuidance.mTargetGroundSpeed = mModePtr->mMaximumGroundSpeed;
      }

      if (mModePtr->mMaximumAttitudeRate != RotorcraftMode::cNOT_SET)
      {
         mWPP_GuidancePtr->SetRollRateLimit(mModePtr->mMaximumAttitudeRate);
      }

      if (mModePtr->mBodyRatesGain != RotorcraftMode::cNOT_SET)
      {
         mWPP_GuidancePtr->SetHeadingPursuitGain(mModePtr->mBodyRatesGain);
      }
   }
}

// =================================================================
// virtual
void WsfRotorcraftMover::SetMode(WsfStringId aModeId)
{
   SetModeId_P(aModeId);

   // Throw it up the chain...
   WsfIterativeRouteMover::SetMode(aModeId);
}

// =================================================================
// virtual
bool WsfRotorcraftMover::TurnToHeading(double aSimTime, double aHeading, double aRadialAccel, WsfPath::TurnDirection aTurnDirection)
{
   SetHeading(aHeading);
   return WsfIterativeRouteMover::TurnToHeading(aSimTime, aHeading, aRadialAccel, aTurnDirection);
}

// =================================================================
// virtual
void WsfRotorcraftMover::SetHeading(double aHeading)
{
   mLocalGuidance.mTargetHeadingRad = aHeading;
}

// =================================================================
// virtual
void WsfRotorcraftMover::SetOrientationNED(double aHeading, double aPitch, double aRoll)
{
   // Here we do not care about the pitch and roll, we only consider desired heading:
   mLocalGuidance.mTargetHeadingRad = aHeading;
}

// =================================================================
// virtual
bool WsfRotorcraftMover::GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude)
{
   if (aAltitude != mLocalGuidance.mTargetAltitude)
   {
      SetAltitudeHold(aSimTime, false);
   }

   if ((aLatitude != mLocalGuidance.mTargetLatitude) && (aLongitude != mLocalGuidance.mTargetLongitude))
   {
      SetPositionHold(aSimTime, false);
   }

   mLocalGuidance.mTargetAltitude  = aAltitude;
   mLocalGuidance.mTargetLatitude  = aLatitude;
   mLocalGuidance.mTargetLongitude = aLongitude;

   double targetCourseDeg;
   double distNotUsed;
   UtSphericalEarth::GreatCircleHeadingAndDistance(mLatitudeDeg,
                                                   mLongitudeDeg,
                                                   mLocalGuidance.mTargetLatitude,
                                                   mLocalGuidance.mTargetLongitude,
                                                   targetCourseDeg,
                                                   distNotUsed);

   mLocalGuidance.mTargetCourseRad = UtMath::cRAD_PER_DEG * targetCourseDeg;

   mLocalGuidance.mLatLonIsValid = true;

   return WsfIterativeRouteMover::GoToLocation(aSimTime, aLatitude, aLongitude, aAltitude);
}

// =================================================================
bool WsfRotorcraftMover::SetLocalGuidance(double aSimTime)
{
   // Set guidance, as currently provided by our route via waypoints, etc.
   if (mWPP_GuidancePtr->AltIsSet())
   {
      if (mLocalGuidance.mTargetAltitude != mWPP_GuidancePtr->GetAltitude())
      {
         SetAltitudeHold(aSimTime, false);
      }
      mLocalGuidance.mTargetAltitude = mWPP_GuidancePtr->GetAltitude();
   }

   if (mWPP_GuidancePtr->LatLonIsSet())
   {
      double newLat = mWPP_GuidancePtr->GetLatitude();
      double newLon = mWPP_GuidancePtr->GetLongitude();

      if ((newLat != mLocalGuidance.mTargetLatitude) || (newLon != mLocalGuidance.mTargetLongitude))
      {
         SetPositionHold(aSimTime, false);
      }

      mLocalGuidance.mLatLonIsValid   = true;
      mLocalGuidance.mTargetLatitude  = newLat;
      mLocalGuidance.mTargetLongitude = newLon;
   }
   else if (!mIsPositionHold)
   {
      mLocalGuidance.mLatLonIsValid = false;
   }

   // NOTE:  Here we map "heading" into "course".
   if (mWPP_GuidancePtr->HeadingIsSet())
   {
      mLocalGuidance.mTargetCourseRad = mWPP_GuidancePtr->GetRawHeading();
   }

   if (mWPP_GuidancePtr->SpeedIsSet())
   {
      double setting = mWPP_GuidancePtr->GetSpeed();
      if (setting < 0.1)
      {
         mLocalGuidance.mCmdSpeedIsZero = true;
      }
      else
      {
         mLocalGuidance.mTargetGroundSpeed = mWPP_GuidancePtr->GetSpeed();
         mLocalGuidance.mCmdSpeedIsZero    = false;
      }

      if (mModePtr != nullptr)
      {
         double limit = MaximumGroundSpeed();
         if (limit < mLocalGuidance.mTargetGroundSpeed)
         {
            mLocalGuidance.mTargetGroundSpeed = limit;
         }
      }
   }

   // Select a Heading Value don not change the target
   // heading if the waypoint is too close:
   if (mLocalGuidance.mLatLonIsValid)
   {
      double targetHdgDeg;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mLatitudeDeg,
                                                      mLongitudeDeg,
                                                      mLocalGuidance.mTargetLatitude,
                                                      mLocalGuidance.mTargetLongitude,
                                                      targetHdgDeg,
                                                      mDistToTgtLL);

      mTimeToTgtLL = 1000.0;
      if (mGroundSpeed > 0.0)
      {
         mTimeToTgtLL = mDistToTgtLL / mGroundSpeed;
      }

      // Do not readjust the target heading if we get too close to the target LL,
      // as heading angles could go haywire.
      if (mDistToTgtLL > cWAYPOINT_CAPTURE_RADIUS)
      {
         mLocalGuidance.mTargetCourseRad = targetHdgDeg * UtMath::cRAD_PER_DEG;
      }

      if (mDistToTgtLL <= cWAYPOINT_CAPTURE_RADIUS)
      {
         // We are close enough to the waypoint to Switch to the next one.
         return true;
      }
   }

   return false;
}

// =================================================================
// Compute desired constant acceleration to achieve a target value.
// Assume the target rate is zero, just achieving the target value in equilibrium.
bool WsfRotorcraftMover::ComputeAccel(double  aCurrentRate,
                                      double  aCurrentValue,
                                      double  aTargetValue,
                                      double  aCriticalAccel,
                                      double& aCriticalTime)
{
   // Guess an accel and time values, and iterate to closure:
   // Equation a) 0 = V0 + a * t, so a = V0 / t
   // Equation b) S = S0 + V0 * t + 1/2 * a * t * t;
   // Solve using the quadratic formula, putting a into b
   // c) S = S0 + V0 * t + 1/2 * [(-V0) / a] * t * t

   // Apply quadratic formula, with t as the IV:
   assert(aCriticalAccel != 0.0);
   double A            = 0.5 * (-aCurrentRate) / aCriticalAccel;
   double B            = aCurrentRate;
   double C            = aCurrentValue - aTargetValue;
   double discriminant = B * B - 4 * A * C;
   if (discriminant <= 0.0)
   {
      return false;
   }

   double sqrt_discriminant = sqrt(discriminant);

   assert(A != 0.0);
   double tta_A = (-B + sqrt_discriminant) / (2.0 * A);
   double tta_B = (-B - sqrt_discriminant) / (2.0 * A);

   // Choose the smallest non-negative value:
   // B will be the smallest positive value if it IS positive...
   aCriticalTime = (tta_B > 0.0) ? tta_B : tta_A;
   return aCriticalTime > 0.0;
}

// =================================================================
// virtual
void WsfRotorcraftMover::LocalUpdate(double aSimTime, bool& aMoved, bool& aAdvanceToNextWaypoint)
{
   static const UtVec3dX cGRAVITY_VECT = UtVec3dX(0.0, 0.0, cGRAVITY);

   // Platform motion is the double-integration of acceleration with the
   // WCS frame as inertial.  Gravity will always pull downward at 1 G
   double dT = aSimTime - mLocalUpdateTime;

   if (dT > 0.0)
   {
      aMoved = true;
   }
   else
   {
      aMoved = false;
      return;
   }

   if (SetLocalGuidance(aSimTime))
   {
      if (!mIsPositionHold)
      {
         aAdvanceToNextWaypoint = true;
      }
   }

   // Allow the airframe to weathercock into the wind if above weathercock speed.
   mIsWeatherCocking = mGroundSpeed >= WeathercockSpeed();
   mNeedToClimb      = mLocalGuidance.mTargetAltitude >= mAltitude;

   if (GetDamageFactor() >= 1.0)
   {
      // Handle an immediate forced landing if Mobility broken:
      double lat, lon, alt;
      GetPlatform()->GetLocationLLA(lat, lon, alt);
      float height;
      GetPlatform()->GetTerrain().GetElevInterp(lat, lon, height);

      mLocalGuidance.mTargetGroundSpeed = 0.0;
      mLocalGuidance.mTargetAltitude    = static_cast<double>(height);
   }

   // The rate limits may be different for climb versus descent
   mFabsROC_Limit = 100.0;
   if (mModePtr != nullptr)
   {
      mFabsROC_Limit = (mNeedToClimb) ? MaximumClimbRate() : MaximumDescentRate();
   }

   SetSwitches(aSimTime);

   mDesiredROC = -mAltErrToROC_Gain * mAltitudeError;

   if (fabs(mDesiredROC) > mFabsROC_Limit)
   {
      // Clamp to a maximum absolute value:
      mDesiredROC = Sign(mDesiredROC) * mFabsROC_Limit;
   }

   // Further attenuate the desired rate of climb as we are approaching the
   // desired target altitude, or we will overshoot it.  Use the minimum allowed
   // G value to set what is a reasonable ROC limit, given current closure rate.
   mTimeToTargetAlt = 30.0;
   if (fabs(mAltitudeRate) > 0.01)
   {
      assert(mAltitudeRate != 0.0);
      double ttt = fabs(mAltitudeError / mAltitudeRate);
      if (ttt < mTimeToTargetAlt)
      {
         mTimeToTargetAlt = ttt;
      }
   }

   // When nearing the target altitude, strive for zero vertical velocity.
   if (mTimeToTargetAlt < 30.0)
   {
      // Linear attenuation of desired rate of climb from
      // 1.0 at 30 seconds to 0.1 at zero seconds.
      mReductionRatio = 0.1 + 0.03 * mTimeToTargetAlt;
      mDesiredROC *= mReductionRatio;
   }
   else
   {
      mReductionRatio = 1.0;
   }

   double criticalTime;
   if (ComputeAccel(mAltitudeRate, mAltitude, mLocalGuidance.mTargetAltitude, cGRAVITY + MinimumUpwardAccel(), criticalTime))
   {
      if (criticalTime < 10)
      {
         mDesiredROC = 0.0;
      }
   }

   // Calculate desired velocity NED, which will drive lateral accelerations:
   mDesiredVelNED[0] = cos(mLocalGuidance.mTargetCourseRad) * mLocalGuidance.mTargetGroundSpeed;
   mDesiredVelNED[1] = sin(mLocalGuidance.mTargetCourseRad) * mLocalGuidance.mTargetGroundSpeed;
   mDesiredVelNED[2] = -mDesiredROC;

   mVelNED_Error = mVelNED - mDesiredVelNED;

   if (mIsPositionHold)
   {
      // These two (lateral accel) PIDS track on an error between Desired and Actual Positions
      mDesiredAccelNED[0] = mNorthAccelValuePID.Update(aSimTime, -mNED_Error[0]); // was cmdNorth
      mDesiredAccelNED[1] = mEastAccelValuePID.Update(aSimTime, -mNED_Error[1]);  // was cmdEast
   }
   else
   {
      // These two (lateral accel) PIDS track on an error between Desired and Actual Velocities
      mDesiredAccelNED[0] = mNorthAccelRatePID.Update(aSimTime, -mVelNED_Error[0]);
      mDesiredAccelNED[1] = mEastAccelRatePID.Update(aSimTime, -mVelNED_Error[1]);
   }

   if (mIsAltitudeHold)
   {
      // This PID tracks on an error between Desired and Actual Altitude:
      mDesiredAccelUp = mVerticalAccelValuePID.Update(aSimTime, -mAltitudeError);
   }
   else
   {
      // This PID tracks on an error between Desired and Actual Rate of Climb:
      mDesiredAccelUp = mVerticalAccelRatePID.Update(aSimTime, mVelNED_Error[2]);
   }

   mDesiredAccelNED[2] = -mDesiredAccelUp + cGRAVITY;

   // Now set the actual acceleration to the desired
   // (but we will impose constraints on top of this value, too).
   mAccelNED = mDesiredAccelNED;

   double lim = MaximumAcceleration();
   double mag = mAccelNED.Magnitude();

   if (mag > lim)
   {
      // We have exceeded our available acceleration limit.  Give the "up" direction
      // acceleration higher priority, so that we do not fall when turning hard to
      // intercept a heading.
      double verticalAccel = -mAccelNED[2];
      if (verticalAccel > lim)
      {
         mAccelNED.Set(0.0, 0.0, -lim);
      }
      else if (verticalAccel < -lim)
      {
         mAccelNED.Set(0.0, 0.0, lim);
      }
      else
      {
         double lateral = sqrt(mAccelNED[0] * mAccelNED[0] + mAccelNED[1] * mAccelNED[1]);
         assert(lim > verticalAccel);
         double available = sqrt(lim * lim - verticalAccel * verticalAccel);
         assert(lateral != 0.0);
         double attenuation = available / lateral;
         mAccelNED[0] *= attenuation;
         mAccelNED[1] *= attenuation;
      }
      // TODO_UNUSED mag = lim;
   }

   // Impose a vertical drop accel limit:
   double minUp = MinimumUpwardAccel();
   if (mAccelNED[2] > -minUp)
   {
      mAccelNED[2] = -minUp;
   }

   // Now rotate the NED-facing rotor disk to slant east-west or north-south:
   // If an acceleration component acts in the negative x direction (south),
   // tip rotor disk southward (pitch up).  If a component acts in positive
   // y direction (east), then tip rotor eastward (roll right).
   double tipSouth = atan2(-mAccelNED[0], -mAccelNED[2]);
   double tipEast  = atan2(mAccelNED[1], -mAccelNED[2]);

   // Impose a first-order time lag to smooth the response to transients.
   double tipSouth_Filtered = mSouthTipFilter.Update(aSimTime, tipSouth);
   double tipEast_Filtered  = mEastTipFilter.Update(aSimTime, tipEast);
   mDcmNEDtoDisk.Set(0.0, tipSouth_Filtered, tipEast_Filtered);

   // Rotate heading toward the target at a rate proportional to the error, clamped to a maximum
   double desiredHdgRad = (mIsWeatherCocking) ? mCourseRad : mLocalGuidance.mTargetHeadingRad;
   mHeadingErrorRad     = UtMath::NormalizeAngleMinusPi_Pi(desiredHdgRad - mHeadingRad);
   double slewRate      = mHeadingErrorRad * BodyRatesGain();
   double slewRateLimit = MaximumBodyRate();
   if (fabs(slewRate) > slewRateLimit)
   {
      slewRate = (slewRate >= 0.0) ? slewRateLimit : -slewRateLimit;
   }
   mHeadingRad += slewRate * dT;
   mDcmDiskToECS.Set(mHeadingRad, 0.0, 0.0);

   mNetAccelNED = mAccelNED - cGRAVITY_VECT;
   mNetAccelWCS = mDcmWCStoNED.InverseTransform(mNetAccelNED);

   // Propagate linearly to a new location in 3-D space.
   // ====== TRAPEZOIDAL (?) INTEGRATION SCHEME ======
   mVelWCS += 0.5 * (mNetAccelWCS + mPrevNetAccelWCS) * dT;
   mLocWCS += 0.5 * (mVelWCS + mPrevVelWCS) * dT;

   mPrevNetAccelWCS = mNetAccelWCS;
   mPrevVelWCS      = mVelWCS;
   mPrevLocWCS      = mLocWCS;

   // We just moved.  So refresh LLA
   UtEllipsoidalEarth::ConvertECEFToLLA(mLocWCS.GetData(), mLatitudeDeg, mLongitudeDeg, mAltitude);

   // Now refresh the mDcmWCStoNED DCM.
   double locNotUsed[3], tempDCM[3][3];
   UtEllipsoidalEarth::ComputeNEDTransform(mLatitudeDeg, mLongitudeDeg, mAltitude, tempDCM, locNotUsed);
   mDcmWCStoNED.Set(tempDCM);
   mDcmNEDtoECS = mDcmDiskToECS * mDcmNEDtoDisk;

   WsfIterativeRouteMover::Update(aSimTime);

   if (IsWaypointSwitchTime(aSimTime))
   {
      if (!mIsPositionHold)
      {
         aAdvanceToNextWaypoint = true;
      }
   }

   ExportMotionState();

   // Re-orient NED velocity
   mVelNED = mDcmWCStoNED.Transform(mVelWCS);

   // === Refresh other exported or plotted data: ===
   mGroundSpeed = sqrt(mVelNED[0] * mVelNED[0] + mVelNED[1] * mVelNED[1]);
   if (mGroundSpeed > 0.000001)
   {
      mCourseRad = atan2(mVelNED[1], mVelNED[0]);
   }

   mAltitudeRate = -mVelNED[2];

   double hdg, pitch, roll;
   mDcmNEDtoECS.Get(hdg, pitch, roll);

   mLocalUpdateTime = aSimTime;
}

// =================================================================
void WsfRotorcraftMover::SetSwitches(double aSimTime)
{
   const double cALTITUDE_CLOSE_ENOUGH = 10.0;

   // Compute the _desired_ rate of climb.
   mAltitudeError = mAltitude - mLocalGuidance.mTargetAltitude;

   if (!mIsAltitudeHold && fabs(mAltitudeError) <= cALTITUDE_CLOSE_ENOUGH)
   {
      SetAltitudeHold(aSimTime, true);
   }

   if (mLocalGuidance.mLatLonIsValid)
   {
      UtVec3dX targetLocWCS;
      UtEntity::ConvertLLAToWCS(mLocalGuidance.mTargetLatitude,
                                mLocalGuidance.mTargetLongitude,
                                mLocalGuidance.mTargetAltitude,
                                targetLocWCS.GetData());
      // Due to curvature of earth issues, the NED error is not
      // computed correctly here.  Override below with Altitude Error:
      mNED_Error = mDcmWCStoNED.Transform(mLocWCS - targetLocWCS);
   }
   else
   {
      mNED_Error[0] = 0.0;
      mNED_Error[1] = 0.0;
      mDistToTgtLL  = 0.0;
      mTimeToTgtLL  = 10000.;
   }
   mNED_Error[2] = -mAltitudeError;

   if (!mIsPositionHold && mLocalGuidance.mLatLonIsValid && mLocalGuidance.mCmdSpeedIsZero)
   {
      double lateralOffsetSq = (mNED_Error[0] * mNED_Error[0] + mNED_Error[1] * mNED_Error[1]);

      if (lateralOffsetSq <= cPOSITION_HOLD_CAPTURE_RADIUS_SQ)
      {
         SetPositionHold(aSimTime, true);
      }
   }
}

// =================================================================
void WsfRotorcraftMover::ExportMotionState()
{
   double hdg, pitch, roll;
   mDcmNEDtoECS.Get(hdg, pitch, roll);

   // Now refresh external indications of our motion state:
   GetPlatform()->SetLocationWCS(mLocWCS.GetData());
   GetPlatform()->SetVelocityWCS(mVelWCS.GetData());
   GetPlatform()->SetAccelerationWCS(mNetAccelWCS.GetData());
   GetPlatform()->SetOrientationNED(hdg, pitch, roll);
}

// =================================================================
// virtual
bool WsfRotorcraftMover::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "desired_heading")
   {
      aInput.ReadValueOfType(mLocalGuidance.mTargetHeadingRad, UtInput::cANGLE);
   }
   else if (command == "position_hold_capture_radius")
   {
      aInput.ReadValueOfType(mPosHoldCaptureRadius, UtInput::cLENGTH);
      aInput.ValueGreater(mPosHoldCaptureRadius, 0.0);
   }
   else if (command == "start_mode")
   {
      std::string modeName;
      aInput.ReadValue(modeName);
      mStartModeId = modeName;
   }
   else if (command == "vertical_acceleration_rate_pid")
   {
      mVerticalAccelRatePID.ProcessInputBlock(aInput);
   }
   else if (command == "lateral_acceleration_rate_pid")
   {
      // Applies to both north and east PIDs.
      mNorthAccelRatePID.ProcessInputBlock(aInput);
      mEastAccelRatePID = mNorthAccelRatePID;
   }
   else if (command == "vertical_acceleration_value_pid")
   {
      mVerticalAccelValuePID.ProcessInputBlock(aInput);
   }
   else if (command == "lateral_acceleration_value_pid")
   {
      // Applies to both north and east PIDs.
      mNorthAccelValuePID.ProcessInputBlock(aInput);
      mEastAccelValuePID = mNorthAccelValuePID;
   }
   else if (command == "disk_tilt_filter_time_constant")
   {
      aInput.ReadValueOfType(mSouthAndEastTipTimeConstant, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mSouthAndEastTipTimeConstant, 0.0);
   }
   else if (command == "altitude_error_to_rate_of_climb_gain")
   {
      aInput.ReadValue(mAltErrToROC_Gain);
      aInput.ValueGreaterOrEqual(mAltErrToROC_Gain, 0.0);
   }
   else
   {
      myCommand = WsfIterativeRouteMover::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================
// ========================= Sub-Class Guidance ==========================
WsfRotorcraftMover::LocalGuidance::LocalGuidance()
   : mTargetGroundSpeed(0.0)
   , mTargetCourseRad(0.0)
   , mTargetLatitude(0.0)
   , mTargetLongitude(0.0)
   , mTargetAltitude(0.0)
   , mTargetHeadingRad(0.0)
   , mLatLonIsValid(false)
   , mCmdSpeedIsZero(false)
{
}

// =================================================================
// ========================= Sub-Class RotorcraftMode ==========================
// static
const double WsfRotorcraftMover::RotorcraftMode::cNOT_SET = -1.23456E-78;

WsfRotorcraftMover::RotorcraftMode::RotorcraftMode()
   : WsfMoverMode()
   , mWeathercockSpeed(10.0)
   , mMaximumAttitudeRate(2.0)
   , mMinimumUpwardAccel(0.5 * UtEarth::cACCEL_OF_GRAVITY)
   , mBodyRatesGain(2.0)
   , mMaximumGroundSpeed(30.0)
   , mMaximumRateOfClimb(3.0)
   , mMaximumRateOfDescent(4.0)
   , mMaximumTotalAccel(3.0 * UtEarth::cACCEL_OF_GRAVITY)
{
}

// =================================================================
WsfRotorcraftMover::RotorcraftMode::RotorcraftMode(const RotorcraftMode& aSrc)
   : WsfMoverMode(aSrc)
   , mWeathercockSpeed(aSrc.mWeathercockSpeed)
   , mMaximumAttitudeRate(aSrc.mMaximumAttitudeRate)
   , mMinimumUpwardAccel(aSrc.mMinimumUpwardAccel)
   , mBodyRatesGain(aSrc.mBodyRatesGain)
   , mMaximumGroundSpeed(aSrc.mMaximumGroundSpeed)
   , mMaximumRateOfClimb(aSrc.mMaximumRateOfClimb)
   , mMaximumRateOfDescent(aSrc.mMaximumRateOfDescent)
   , mMaximumTotalAccel(aSrc.mMaximumTotalAccel)
{
}

// =================================================================
// virtual
WsfMode* WsfRotorcraftMover::RotorcraftMode::Clone() const
{
   return new RotorcraftMode(*this);
}

// =================================================================
// virtual
bool WsfRotorcraftMover::RotorcraftMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "weathercock_speed")
   {
      aInput.ReadValueOfType(mWeathercockSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mWeathercockSpeed, 0.0);
   }
   else if (command == "maximum_attitude_rate")
   {
      aInput.ReadValueOfType(mMaximumAttitudeRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreaterOrEqual(mMaximumAttitudeRate, 0.0);
   }
   else if (command == "minimum_upward_acceleration")
   {
      aInput.ReadValueOfType(mMinimumUpwardAccel, UtInput::cACCELERATION);
      aInput.ValueGreaterOrEqual(mMinimumUpwardAccel, 0.01);
   }
   else if (command == "body_rates_gain")
   {
      aInput.ReadValue(mBodyRatesGain);
      aInput.ValueGreaterOrEqual(mBodyRatesGain, 0.0);
   }
   else if (command == "maximum_ground_speed")
   {
      aInput.ReadValueOfType(mMaximumGroundSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMaximumGroundSpeed, 0.0);
   }
   else if (command == "maximum_rate_of_climb")
   {
      aInput.ReadValueOfType(mMaximumRateOfClimb, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMaximumRateOfClimb, 0.0);
   }
   else if (command == "maximum_rate_of_descent")
   {
      aInput.ReadValueOfType(mMaximumRateOfDescent, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMaximumRateOfDescent, 0.0);
   }
   else if (command == "maximum_total_acceleration")
   {
      aInput.ReadValueOfType(mMaximumTotalAccel, UtInput::cACCELERATION);
      aInput.ValueGreaterOrEqual(mMaximumTotalAccel, 1.1 * UtEarth::cACCEL_OF_GRAVITY);
   }
   else
   {
      myCommand = WsfMoverMode::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================
// virtual
void WsfRotorcraftMover::RotorcraftMode::Deselect(double aSimTime)
{
   // Do not know what to do here.  assert(false);
}

// =================================================================
// virtual
void WsfRotorcraftMover::RotorcraftMode::Select(double aSimTime)
{
   // Do not know what to do here.  assert(false);
}
