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

#include "UtMath6D_Motion.hpp"

//#define ASSERT_TRANSFORMS 1

#include <assert.h>
#include <iostream>
#include <string.h>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace
{
const double c6D_MOTION_TOO_SLOW = 0.05;

const UtVec3dX cFORWARD(1.0, 0.0, 0.0);
const UtVec3dX cRIGHT(0.0, 1.0, 0.0);
const UtVec3dX cDOWN(0.0, 0.0, 1.0);

const double cDEFAULT_CL_ALPHA  = 2.0 * UtMath::cPI;
const double cDEFAULT_MAX_ALPHA = 30.0 * UtMath::cRAD_PER_DEG;
const double cDEFAULT_CL_MAX    = cDEFAULT_MAX_ALPHA * cDEFAULT_CL_ALPHA;
} // namespace

// =================================================================
UtMath6D_Motion::UtMath6D_Motion(const UtAtmosphere& aAtmosphere)
   : mEntityPtr(nullptr)
   , mGuidance()
   , mLastUpdateTime(0.0)
   , mRefArea(30.0)
   , mCL_Alpha(cDEFAULT_CL_ALPHA)
   , mMaxAlpha(cDEFAULT_MAX_ALPHA)
   , mMaxCL(cDEFAULT_CL_MAX)
   , mMass(1000)
   , mInitSpeed(2.0)
   , mInitFPA(0.0)
   , mTargetAirspeed()
   , mTargetLinearSpeed(0.0)
   , mSpeed(0.0)
   , mGroundSpeed(0.0)
   , mMaxLinearAccel(0.25 * UtEarth::cACCEL_OF_GRAVITY)
   , mMaxNormalAccel(4.00 * UtEarth::cACCEL_OF_GRAVITY)
   , mMaxTangentialOmega(0.5)
   , mDistToPoint(0.0)
   , mPrevAlt(0.0)
   , mLocWCS()
   , mVelWCS()
   , mDesiredAccelWCS()
   , mVelNED()
   , mGammaNED(0.0)
   , mCourseNED(0.0)
   , mAltitude(0.0)
   , mAltitudeRate(0.0)
   , mHeadingNED(0.0)
   , mPitchNED(0.0)
   , mRollNED(0.0)
   , mLatitudeDeg(0.0)
   , mLongitudeDeg(0.0)
   , mSpeedGain(1.0)
   , mAccelNED_WOTFG()
   , mDetailDebug(false)
   , mDidSwitchFTP(false)
   , mStabDot()
   , mDcmWCStoNED()
   , mDcmNEDtoStab()
   , mDcmWCStoStab()
   , mQuatWCStoStab()
   , mQuatRateWCStoStab()
   , mPrevDeltaWCS()
   , mDeltaWCS()
   , mDesiredLocWCS()
   , mDesiredAccelFrame(cNED)
   , mFilteredUnitFwdNED()
   , mFilteredUnitDwnNED()
   , mAtm(aAtmosphere)
{
}

// =================================================================
UtMath6D_Motion::UtMath6D_Motion(const UtMath6D_Motion& aSrc)
   : mEntityPtr(nullptr)
   , mGuidance(aSrc.mGuidance)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mRefArea(aSrc.mRefArea)
   , mCL_Alpha(aSrc.mCL_Alpha)
   , mMaxAlpha(aSrc.mMaxAlpha)
   , mMaxCL(aSrc.mMaxCL)
   , mMass(aSrc.mMass)
   , mInitSpeed(aSrc.mInitSpeed)
   , mInitFPA(aSrc.mInitFPA)
   , mTargetAirspeed(aSrc.mTargetAirspeed)
   , mTargetLinearSpeed(aSrc.mTargetLinearSpeed)
   , mSpeed(aSrc.mSpeed)
   , mMaxLinearAccel(aSrc.mMaxLinearAccel)
   , mMaxNormalAccel(aSrc.mMaxNormalAccel)
   , mMaxTangentialOmega(aSrc.mMaxTangentialOmega)
   , mDistToPoint(aSrc.mDistToPoint)
   , mPrevAlt(aSrc.mPrevAlt)
   , mLocWCS(aSrc.mLocWCS)
   , mVelWCS(aSrc.mVelWCS)
   , mDesiredAccelWCS(aSrc.mDesiredAccelWCS)
   , mVelNED(aSrc.mVelNED)
   , mGammaNED(aSrc.mGammaNED)
   , mCourseNED(aSrc.mCourseNED)
   , mAltitude(aSrc.mAltitude)
   , mAltitudeRate(aSrc.mAltitudeRate)
   , mHeadingNED(aSrc.mHeadingNED)
   , mPitchNED(aSrc.mPitchNED)
   , mRollNED(aSrc.mRollNED)
   , mLatitudeDeg(aSrc.mLatitudeDeg)
   , mLongitudeDeg(aSrc.mLongitudeDeg)
   , mSpeedGain(aSrc.mSpeedGain)
   , mAccelNED_WOTFG()
   , mDetailDebug(aSrc.mDetailDebug)
   , mDidSwitchFTP(aSrc.mDidSwitchFTP)
   , mStabDot(aSrc.mStabDot)
   , mDcmWCStoNED(aSrc.mDcmWCStoNED)
   , mDcmNEDtoStab(aSrc.mDcmNEDtoStab)
   , mDcmWCStoStab(aSrc.mDcmWCStoStab)
   , mQuatWCStoStab(aSrc.mQuatWCStoStab)
   , mQuatRateWCStoStab(aSrc.mQuatRateWCStoStab)
   , mPrevDeltaWCS(aSrc.mPrevDeltaWCS)
   , mDeltaWCS(aSrc.mDeltaWCS)
   , mDesiredLocWCS(aSrc.mDesiredLocWCS)
   , mDesiredAccelFrame(aSrc.mDesiredAccelFrame)
   , mFilteredUnitFwdNED()
   , mFilteredUnitDwnNED()
   , mAtm(aSrc.mAtm)
{
}

// =================================================================
bool UtMath6D_Motion::LiftValuesAreOK()
{
   // Resolve max CL, max alpha, CL_Alpha
   if ((mMaxCL == cDEFAULT_CL_MAX) && (mCL_Alpha == cDEFAULT_CL_ALPHA) && (mMaxAlpha == cDEFAULT_MAX_ALPHA))
   {
      // do nothing
      return true;
   }
   else if ((mMaxCL != cDEFAULT_CL_MAX) && (mCL_Alpha != cDEFAULT_CL_ALPHA) && (mMaxAlpha != cDEFAULT_MAX_ALPHA))
   {
      // Assure that all are consistent:
      double epsilon = fabs(mMaxCL - mMaxAlpha * mCL_Alpha);
      if (epsilon > 0.001)
      {
         ut::log::error() << "Inconsistency in values for Motion maximum CL, Alpha, and CL_Alpha.";
         return false;
      }
      return true;
   }
   else
   {
      // Use the values that are NOT defaulted to set the ones that ARE defaulted.
      if ((mMaxCL != cDEFAULT_CL_MAX) && (mCL_Alpha != cDEFAULT_CL_ALPHA))
      {
         mMaxAlpha = mMaxCL / mCL_Alpha;
         return true;
      }
      else if ((mMaxCL != cDEFAULT_CL_MAX) && (mMaxAlpha != cDEFAULT_MAX_ALPHA))
      {
         mCL_Alpha = mMaxCL / mMaxAlpha;
         return true;
      }
      else if ((mCL_Alpha != cDEFAULT_CL_ALPHA) && (mMaxAlpha != cDEFAULT_MAX_ALPHA))
      {
         mMaxCL = mCL_Alpha * mMaxAlpha;
         return true;
      }
      else
      {
         if ((mCL_Alpha != cDEFAULT_CL_ALPHA) || (mMaxAlpha != cDEFAULT_MAX_ALPHA))
         {
            mMaxCL = mCL_Alpha * mMaxAlpha;
            return true;
         }
         else if (mMaxCL != cDEFAULT_CL_MAX)
         {
            mMaxAlpha = mMaxCL / mCL_Alpha;
            return true;
         }
      }
   }

   return false;
}

// =================================================================
// virtual
bool UtMath6D_Motion::Initialize(double aSimTime, UtEntity* aEntityPtr)
{
   bool success = true;

   // Initialize our motion state from our host platform's values.
   mEntityPtr = aEntityPtr;
   if (mEntityPtr == nullptr)
   {
      success = false;
   }

   if (success)
   {
      success = LiftValuesAreOK();
   }

   mLastUpdateTime = aSimTime;
   mTargetAirspeed.SetAtmosphere(&mAtm);

   mEntityPtr->GetLocationWCS(mLocWCS.GetData());

   // Always assume our angular velocity starts out zero.
   mStabDot.Null();
   mPrevDeltaWCS.Null();
   mDeltaWCS.Null();
   mDesiredAccelWCS.Null();

   mGuidance.Reset();

   // Use the platform initial heading, and augment with initial speed and flight path angle.
   // (Flight path angle or speed cannot be specified within a platform's initial velocity.)
   double heading;
   double pitch;
   double roll;
   mEntityPtr->GetOrientationNED(heading, pitch, roll);
   UtVec3dX velNED;
   velNED[0] = mInitSpeed * cos(heading) * cos(mInitFPA);
   velNED[1] = mInitSpeed * sin(heading) * sin(mInitFPA);
   velNED[2] = mInitSpeed * -sin(mInitFPA);

   mSpeed       = velNED.Magnitude();
   mGroundSpeed = sqrt(velNED[1] * velNED[1] + velNED[2] * velNED[2]);
   mCourseNED   = atan2(velNED[1], velNED[0]);
   mGammaNED    = -asin(velNED[2] / mSpeed);

   mEntityPtr->SetVelocityNED(velNED.GetData());
   mEntityPtr->GetLocationLLA(mLatitudeDeg, mLongitudeDeg, mAltitude);
   mAltitudeRate = -velNED[2];

   double tempDCM[3][3];
   mEntityPtr->GetWCSToNEDTransform(tempDCM);
   mDcmWCStoNED.Set(tempDCM);

   double qSref = 0.50 * mAtm.Density(mAltitude) * mSpeed * mSpeed * mRefArea;

   double liftNeeded = mMass * UtEarth::cACCEL_OF_GRAVITY * cos(mGammaNED);
   double alpha      = liftNeeded / (mCL_Alpha * qSref);
   if (alpha > mMaxAlpha)
   {
      // Do not allow alpha to exceed max.
      alpha = mMaxAlpha;
   }

   mEntityPtr->SetOrientationNED(mCourseNED, mGammaNED + alpha, 0.0);
   mEntityPtr->GetOrientationNED(mHeadingNED, mPitchNED, mRollNED);

   mDcmNEDtoStab.Set(mCourseNED, mGammaNED, 0.0);
   mEntityPtr->GetVelocityWCS(mVelWCS.GetData());

   mDcmWCStoStab = mDcmNEDtoStab * mDcmWCStoNED;
   mQuatWCStoStab.Set(mDcmWCStoStab);

   mAccelNED_WOTFG.Set(0.0, 0.0, -UtEarth::cACCEL_OF_GRAVITY);

   SetAttitudeFrom(alpha, true);
   // The call above is to set the two values below:
   mFilteredUnitFwdNED.Reset(UtVec3dX()); // TO-DO
   mFilteredUnitDwnNED.Reset(UtVec3dX()); // TO-DO

   return success;
}

// =================================================================
double UtMath6D_Motion::NeededTangentialAccel()
{
   double accelRate = 0.0;

   if (mTargetAirspeed.IsSet())
   {
      mTargetAirspeed.Get(mAltitude, mAltitudeRate, mTargetLinearSpeed, accelRate);
   }

   double tangentialAccel = (mTargetLinearSpeed - mSpeed) * mSpeedGain + accelRate;

   if (tangentialAccel > mMaxLinearAccel)
   {
      tangentialAccel = mMaxLinearAccel;
   }

   if (tangentialAccel < -mMaxLinearAccel)
   {
      tangentialAccel = -mMaxLinearAccel;
   }

   return tangentialAccel;
}

// =================================================================
double UtMath6D_Motion::TurnRadius()
{
   double groundRadius = mGroundSpeed / mMaxTangentialOmega;
   double airRadius    = mSpeed / mMaxTangentialOmega;
   double radius       = std::max(std::min(groundRadius, airRadius), 10.0);
   return radius;
}

// =================================================================
bool UtMath6D_Motion::RecommendSwitchToNewWaypoint()
{
   if ((mLocWCS - mDesiredLocWCS).Magnitude() <= TurnRadius())
   {
      return true;
   }
   return false;
}

// =================================================================
//! This function sets a new fly-to-point, and will use pro nav
//! and/or velocity pursuit to intercept it.
void UtMath6D_Motion::SetFlyToPoint(const UtVec3dX& aLocWCS)
{
   mGuidance.Reset();
   mDistToPoint       = DBL_MAX;
   mDesiredLocWCS     = aLocWCS;
   mDidSwitchFTP      = true;
   mDesiredAccelFrame = cFLY_TO_POINT;
}

// =================================================================
void UtMath6D_Motion::SetFlyToPoint(double aLatDeg, double aLonDeg, double aAlt)
{
   UtVec3dX newPointWCS;
   UtEntity::ConvertLLAToWCS(aLatDeg, aLonDeg, aAlt, newPointWCS.GetData());
   SetFlyToPoint(newPointWCS);
}

// =================================================================
void UtMath6D_Motion::ComputeAndSetNED_Attitude(double /*aSimTime*/)
{
   // Now compute and set a "pretty-looking" attitude, completely manufactured
   // from thin air.  (Compute "forward" and "down" ECS unit vectors, and
   // filter them to smooth the body angular rates.  Then recover the filtered
   // values to determine current WCS orientation.)

   // if lift = qSref * Cla * alpha, then alpha = lift / (qSref * Cla)
   double qSref = 0.50 * mAtm.Density(mAltitude) * mSpeed * mSpeed * mRefArea;
   // UtVec3dX aoaVectorStab = mAccelNED_WOTFG * mDcmNEDtoStab * mMass / (qSref * mCL_Alpha);
   double alphaStab = mAccelNED_WOTFG.Magnitude() * mMass / (qSref * mCL_Alpha);
   if (alphaStab > mMaxAlpha)
   {
      alphaStab = mMaxAlpha;
   }

   SetAttitudeFrom(alphaStab);
}

// =================================================================
void UtMath6D_Motion::SetAttitudeFrom(double aAlphaStab, bool aStartFilters)
{
   UtVec3dX accelStab        = mAccelNED_WOTFG * mDcmNEDtoStab;
   double   alphaRotationPhi = 0.0;
   if (accelStab.MagYZ() > 0.00001)
   {
      alphaRotationPhi = atan2(accelStab[1], -accelStab[2]);
   }

   double stabPhi;
   double stabTheta;
   double stabPsi;
   mDcmWCStoStab.Get(stabPsi, stabTheta, stabPhi);

   UtDCM stabToECS(0.0, aAlphaStab, alphaRotationPhi);

   // Now convert stab orientation into NED coordinates:
   UtDCM ecsToNED = (stabToECS * mDcmNEDtoStab).Transpose();

   // Filter "n" past values.
   UtVec3dX fwdNED  = cFORWARD * ecsToNED;
   UtVec3dX downNED = cDOWN * ecsToNED;

   if (aStartFilters)
   {
      mFilteredUnitFwdNED.Reset(fwdNED);
      mFilteredUnitDwnNED.Reset(downNED);
   }
   else
   {
      fwdNED  = mFilteredUnitFwdNED.Update(fwdNED);
      downNED = mFilteredUnitDwnNED.Update(downNED);
   }

   fwdNED.Normalize();
   downNED.Normalize();

   double hdg   = atan2(fwdNED[1], fwdNED[0]);
   double pitch = -asin(fwdNED[2]);
   // while debugging, guard against NAN issues.
   assert(hdg == hdg);
   assert(pitch == pitch);
   UtDCM    hdgPitch(hdg, pitch, 0.0);
   UtVec3dX rollInYZ_Plane = downNED * hdgPitch;
   double   roll           = atan2(-rollInYZ_Plane[1], rollInYZ_Plane[2]);
   mEntityPtr->SetOrientationNED(hdg, pitch, roll);
   double psi;
   double theta;
   double phi;
   mEntityPtr->GetOrientationWCS(psi, theta, phi);
   mDcmWCStoECS.Set(psi, theta, phi);
}

// =================================================================
// virtual
void UtMath6D_Motion::Update(double aSimTime)
{
   if (aSimTime > mLastUpdateTime)
   {
      double dt = aSimTime - mLastUpdateTime;

      // Obtain the displacement we would see this time step if
      // there was no acceleration from the previous time step.
      // dS = v * dt
      mPrevDeltaWCS = dt * mSpeed * cFORWARD * mDcmWCStoStab.Transpose();

      UtVec3dX desiredAccelVel;
      if (mDesiredAccelFrame == cFLY_TO_POINT)
      {
         desiredAccelVel =
            mGuidance.AccelerationVel(aSimTime, mLocWCS, mVelWCS, mDesiredLocWCS, mDcmWCStoStab, mDcmWCStoECS, mDcmWCStoNED);
         desiredAccelVel[0] = 0.0; // remove speed-variation component, as it is handled separately
      }
      else
      {
         assert(false);
      }

      mDesiredAccelWCS = desiredAccelVel * mDcmWCStoStab.Transpose();

      // A special value for acceleration is computed to use for attitude determination.
      // Acceleration in the NED frame, without thrust, but including opposing (fighting) gravity
      mAccelNED_WOTFG = mDesiredAccelWCS * mDcmWCStoNED - UtEarth::cACCEL_OF_GRAVITY * cDOWN;

      ComputeAndSetNED_Attitude(aSimTime); // uses mAccelNED_WOTFG
      ConstrainStabRates();                // uses mDesiredAccelWCS to discern mStabDot

      // Compute resulting quaternion rate (inertial frame) from body rates (ECS frame).
      mQuatRateWCStoStab.SetRate(mQuatWCStoStab, mStabDot);

      // Now propagate the quaternion forward to a new orientation.
      mQuatWCStoStab.Update(mQuatRateWCStoStab, dt);
      // mQuatNEDtoStab.Normalize(); // Placed in the UtQuaternion class, directly.

      // Transform Quaternion into equivalent DCM.
      mQuatWCStoStab.Get(mDcmWCStoStab);

      // Add in linear/axial acceleration
      mSpeed += NeededTangentialAccel() * dt;
      mVelWCS = mSpeed * cFORWARD * mDcmWCStoStab.Transpose();

      // Obtain new delta vector for this state.
      mDeltaWCS = dt * mSpeed * cFORWARD * mDcmWCStoStab.Transpose();

      // Propagate the average between prev and current extrapolated velocities.
      mLocWCS += 0.5 * (mPrevDeltaWCS + mDeltaWCS);

      mEntityPtr->SetLocationWCS(mLocWCS.GetData());
      mEntityPtr->SetVelocityWCS(mVelWCS.GetData());

      // Attitude will be set in NED terms in the call below.
      UpdateNED_AfterMove();

      mLastUpdateTime = aSimTime;
   }
}

// =================================================================================================
void UtMath6D_Motion::UpdateNED_AfterMove()
{
   // ==================== START
   // Get the attributes that we cannot directly control in this class:

   // Get new mWCStoNED from UtEntity.
   double tempDCM[3][3];
   mEntityPtr->GetWCSToNEDTransform(tempDCM);
   mDcmWCStoNED.Set(tempDCM);

   // Get new LLA.
   mEntityPtr->GetLocationLLA(mLatitudeDeg, mLongitudeDeg, mAltitude);
   // ==================== END

   // Since we now have a DCM, can transform our velocity into NED:
   mVelNED       = mVelWCS * mDcmWCStoNED;
   mAltitudeRate = -mVelNED[2];
   mGroundSpeed  = sqrt(mVelNED[0] * mVelNED[0] + mVelNED[1] * mVelNED[1]);

   if (mSpeed > c6D_MOTION_TOO_SLOW)
   {
      mGammaNED = -asin(mVelNED[2] / mSpeed);
   }
   if (mGroundSpeed > c6D_MOTION_TOO_SLOW)
   {
      mCourseNED = atan2(mVelNED[1], mVelNED[0]);
   }

   mDcmNEDtoStab = mDcmWCStoStab * mDcmWCStoNED.Transpose();

#ifdef ASSERT_TRANSFORMS
   assert(mDcmNEDtoStab.IsValid(true));
   assert(mDcmWCStoNED.IsValid(true));
   assert(mDcmWCStoStab.IsValid(true));
#endif
}

// =================================================================================================
void UtMath6D_Motion::ConstrainStabRates()
{
   // Continuously roll the stability axes to point down to center of earth.
   // So if our stab "right" unit vector, converted into NED has a non-zero
   // "down" value, then roll left if positive (down).
   UtVec3dX stabRightNED = mDcmNEDtoStab.Transpose().GetRow(1);
   mStabDot[0]           = -0.00001 * stabRightNED[1];

   // Compute stability rates from desired accel WCS
   UtVec3dX stabRates = (1.0 / mSpeed) * mDesiredAccelWCS * mDcmWCStoStab;
   // Axis commutation.  A desired lateral acceleration (+Y) gives rise to a velocity vector yaw rate (+Z).
   mStabDot[2] = stabRates[1];
   // Axis commutation.  A desired vertical acceleration (-Z) gives rise to a velocity vector pitch rate (+Y).
   mStabDot[1] = -stabRates[2];

   // Assure maximum mMaxTangentialOmega limit does not get exceeded:
   double requTangentialRate = sqrt(mStabDot[1] * mStabDot[1] + mStabDot[2] * mStabDot[2]);
   if (requTangentialRate > mMaxTangentialOmega)
   {
      // Limit the axial/tangential rate to the max.
      double attenuation = mMaxTangentialOmega / requTangentialRate;
      mStabDot[1] *= attenuation;
      mStabDot[2] *= attenuation;
   }

   // assert(mStabDot[0] <= mMaxRollRate);

#ifdef ASSERT_TRANSFORMS
   double tangentialOmega = sqrt(mStabDot[1] * mStabDot[1] + mStabDot[2] * mStabDot[2]);
   assert(tangentialOmega <= mMaxTangentialOmega + 0.00001);
#endif
}

// =================================================================
// virtual
bool UtMath6D_Motion::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "motion_debug")
   {
      aInput.ReadValue(mDetailDebug);
   }
   else if (command == "target_speed")
   {
      aInput.ReadValueOfType(mTargetLinearSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mTargetLinearSpeed, 0.0);
   }
   else if (command == "maximum_linear_acceleration")
   {
      aInput.ReadValueOfType(mMaxLinearAccel, UtInput::cACCELERATION);
      aInput.ValueGreater(mMaxLinearAccel, 0.0);
   }
   else if (command == "maximum_lateral_acceleration")
   {
      aInput.ReadValueOfType(mMaxNormalAccel, UtInput::cACCELERATION);
      aInput.ValueGreater(mMaxNormalAccel, 0.0);
   }
   else if (command == "maximum_axial_body_turn_rate")
   {
      aInput.ReadValueOfType(mMaxTangentialOmega, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mMaxTangentialOmega, 0.0);
   }
   else if (command == "maximum_alpha")
   {
      aInput.ReadValueOfType(mMaxAlpha, UtInput::cANGLE);
      aInput.ValueGreater(mMaxAlpha, 0.0);
   }
   else if ((command == "maximum_cl") || (command == "cl_max"))
   {
      aInput.ReadValue(mMaxCL);
      aInput.ValueGreater(mMaxCL, 0.0);
   }
   else if (command == "initial_speed")
   {
      aInput.ReadValueOfType(mInitSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mInitSpeed, 0.0);
   }
   else if (command == "initial_flight_path_angle")
   {
      aInput.ReadValueOfType(mInitFPA, UtInput::cANGLE);
   }
   else if (command == "mass")
   {
      aInput.ReadValueOfType(mMass, UtInput::cMASS);
      aInput.ValueGreater(mMass, 0.0);
   }
   else if (command == "weight")
   {
      double wt;
      aInput.ReadValueOfType(wt, UtInput::cFORCE);
      aInput.ValueGreater(wt, 0.0);
      mMass = wt / UtEarth::cACCEL_OF_GRAVITY;
   }
   else if (command == "reference_area")
   {
      aInput.ReadValueOfType(mRefArea, UtInput::cAREA);
      aInput.ValueGreater(mRefArea, 0.0);
   }
   else if (command == "cl_alpha")
   {
      aInput.ReadValue(mCL_Alpha);
      aInput.ValueGreater(mCL_Alpha, 0.0);
   }
   else if (command == "speed_gain")
   {
      aInput.ReadValue(mSpeedGain);
      aInput.ValueGreater(mSpeedGain, 0.0);
   }
   else if (mTargetAirspeed.ProcessInput(aInput))
   {
   }
   else if (mAtm.ProcessInput(aInput))
   {
   }
   else if (mGuidance.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================
bool UtMath6D_Motion::FlyToPointReached()
{
   // Consider horizontal turns only in considering whether a waypoint is reached.
   UtVec3dX diffWCS     = mLocWCS - mDesiredLocWCS;
   UtVec3dX diffNED     = diffWCS * mDcmWCStoNED.Transpose();
   double   groundRange = sqrt(diffNED[0] * diffNED[0] + diffNED[1] * diffNED[1]);
   return groundRange <= TurnRadius();
}

// ========================= Sub-Class Filter =====================================================
// ================================================================================================
UtMath6D_Motion::Filter::Filter(size_t aSize)
   : mBuffer()
{
   mBuffer.resize(aSize);
}

// ================================================================================================
void UtMath6D_Motion::Filter::Reset(const UtVec3dX& aValue)
{
   for (size_t i = 0; i != mBuffer.size(); ++i)
   {
      mBuffer[i] = aValue;
   }
}

// ================================================================================================
UtVec3dX UtMath6D_Motion::Filter::Update(const UtVec3dX& aInput)
{
   mBuffer[mModulus % mBuffer.size()] = aInput;
   ++mModulus;

   UtVec3dX rv;
   for (size_t i = 0; i != mBuffer.size(); ++i)
   {
      for (size_t j = 0; j != 3; ++j)
      {
         rv[j] += mBuffer[i][j];
      }
   }

   return rv / (double)mBuffer.size();
}
