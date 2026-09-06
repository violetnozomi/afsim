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

#include "UtMath3D_Motion.hpp"

//#define ASSERT_TRANSFORMS 1

#include <assert.h>
#include <iostream>
#include <string.h>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace
{
const double c3D_MOTION_TOO_SLOW = 0.05;
}

// =================================================================
UtMath3D_Motion::UtMath3D_Motion(bool aBroachAtSeaLevel)
   : mEntityPtr(nullptr)
   , mStartSimTime(0.0)
   , mLocalLastUpdateTime(0.0)
   , mPathDistanceTravelled(0.0)
   , mGroundDistanceTravelled(0.0)
   , mTimeTravelled(0.0)
   , mInitialSpeed(0.0)
   , mInitialFPA(0.0)
   , mTargetLinearSpeed(15.0)
   , mCurrentLinearSpeed(0.0)
   , mCurrentGroundSpeed(0.0)
   , mMaxLinearAccel(0.25 * UtEarth::cACCEL_OF_GRAVITY)
   , mMaxNormalAccel(8.00 * UtEarth::cACCEL_OF_GRAVITY)
   , mMaxRollRate(180.0 * UtMath::cRAD_PER_DEG)
   , mMaxTangentialOmega(45.0 * UtMath::cRAD_PER_DEG)
   , mLocWCS()
   , mVelWCS()
   , mAccWCS()
   , mVelNED()
   , mAccNED()
   , mGammaNED(0.0)
   , mCourseNED(0.0)
   , mAltitude(0.0)
   , mAltitudeRate(0.0)
   , mRollNED(0.0)
   , mLatitudeDeg(0.0)
   , mLongitudeDeg(0.0)
   , mDetailDebug(false)
   , mPreferCanopyUp(false) // default = bank-to-turn
   , mBroachAtSeaLevel(aBroachAtSeaLevel)
   , mIsFlyingToPoint(false)
   , mUseGrndTrnRadius(true)
   , mPQR()
   , mWCStoNED()
   , mNEDtoECS()
   , mWCStoVelAndECS()
   , mQuatWCStoVelAndECS()
   , mQuatRate()
   , mPrevDeltaWCS()
   , mCurrentDeltaWCS()
   , mDesiredAccel()
   , mDesiredLocWCS()
   , mDesiredAccelIsWCS(false)
   , mDesiredBankNED(0.0)
   , mGuidance()
{
}

// =================================================================
UtMath3D_Motion::UtMath3D_Motion(const UtMath3D_Motion& aSrc)
   : mEntityPtr(nullptr)
   , mStartSimTime(aSrc.mStartSimTime)
   , mLocalLastUpdateTime(aSrc.mLocalLastUpdateTime)
   , mPathDistanceTravelled(aSrc.mPathDistanceTravelled)
   , mGroundDistanceTravelled(aSrc.mGroundDistanceTravelled)
   , mTimeTravelled(aSrc.mTimeTravelled)
   , mInitialSpeed(aSrc.mInitialSpeed)
   , mInitialFPA(aSrc.mInitialFPA)
   , mTargetLinearSpeed(aSrc.mTargetLinearSpeed)
   , mCurrentLinearSpeed(aSrc.mCurrentLinearSpeed)
   , mCurrentGroundSpeed(aSrc.mCurrentGroundSpeed)
   , mMaxLinearAccel(aSrc.mMaxLinearAccel)
   , mMaxNormalAccel(aSrc.mMaxNormalAccel)
   , mMaxRollRate(aSrc.mMaxRollRate)
   , mMaxTangentialOmega(aSrc.mMaxTangentialOmega)
   , mLocWCS(aSrc.mLocWCS)
   , mVelWCS(aSrc.mVelWCS)
   , mAccWCS(aSrc.mAccWCS)
   , mVelNED(aSrc.mVelNED)
   , mAccNED(aSrc.mAccNED)
   , mGammaNED(aSrc.mGammaNED)
   , mCourseNED(aSrc.mCourseNED)
   , mAltitude(aSrc.mAltitude)
   , mAltitudeRate(aSrc.mAltitudeRate)
   , mRollNED(aSrc.mRollNED)
   , mLatitudeDeg(aSrc.mLatitudeDeg)
   , mLongitudeDeg(aSrc.mLongitudeDeg)
   , mDetailDebug(aSrc.mDetailDebug)
   , mPreferCanopyUp(aSrc.mPreferCanopyUp)
   , mBroachAtSeaLevel(aSrc.mBroachAtSeaLevel)
   , mIsFlyingToPoint(aSrc.mIsFlyingToPoint)
   , mUseGrndTrnRadius(aSrc.mUseGrndTrnRadius)
   , mPQR(aSrc.mPQR)
   , mWCStoNED(aSrc.mWCStoNED)
   , mNEDtoECS(aSrc.mNEDtoECS)
   , mWCStoVelAndECS(aSrc.mWCStoVelAndECS)
   , mQuatWCStoVelAndECS(aSrc.mQuatWCStoVelAndECS)
   , mQuatRate(aSrc.mQuatRate)
   , mPrevDeltaWCS(aSrc.mPrevDeltaWCS)
   , mCurrentDeltaWCS(aSrc.mCurrentDeltaWCS)
   , mDesiredAccel(aSrc.mDesiredAccel)
   , mDesiredLocWCS(aSrc.mDesiredLocWCS)
   , mDesiredAccelIsWCS(aSrc.mDesiredAccelIsWCS)
   , mDesiredBankNED(aSrc.mDesiredBankNED)
   , mGuidance(aSrc.mGuidance)
{
}

// =================================================================
// virtual
bool UtMath3D_Motion::Initialize(double aSimTime, UtEntity* aEntityPtr)
{
   // Initialize our motion state from our host platform's values.
   // Because we are 3-DOF not 6-DOF, we will generally ignore his
   // orientation and replace it with our own, in the direction
   // of the velocity vector.  Two exceptions:  If stationary, we
   // will accept his heading and pitch angles as our desired ones,
   // and we will always accept his roll angle as our initial value.

   mEntityPtr = aEntityPtr;
   assert(mEntityPtr != nullptr);
   mStartSimTime            = aSimTime;
   mLocalLastUpdateTime     = aSimTime;
   mPathDistanceTravelled   = 0.0;
   mGroundDistanceTravelled = 0.0;
   mTimeTravelled           = 0.0;

   GetAllMotionFromEntity();

   // Now if we were given initial conditions, set them, otherwise, leave alone.
   SetMotionToInitConditions();

   double   lat;
   double   lon;
   double   alt;
   UtVec3dX locWCS;
   mEntityPtr->GetLocationWCS(locWCS.GetData());
   UtEntity::ConvertWCSToLLA(locWCS.GetData(), lat, lon, alt);

   // Always assume our angular velocity starts out zero.
   mPQR.Null();
   mPrevDeltaWCS.Null();

   if (mDetailDebug)
   {
      PrintState(aSimTime);
   }

   return mEntityPtr != nullptr;
}

// =================================================================
void UtMath3D_Motion::GetAllMotionFromEntity()
{
   mEntityPtr->GetOrientationNED(mCourseNED, mGammaNED, mRollNED);
   // double courseDeg = mCourseNED * UtMath::cDEG_PER_RAD; // Currently unused
   mEntityPtr->GetVelocityNED(mVelNED.GetData());
   mEntityPtr->GetLocationWCS(mLocWCS.GetData());
   mEntityPtr->GetVelocityWCS(mVelWCS.GetData());
   mEntityPtr->GetLocationLLA(mLatitudeDeg, mLongitudeDeg, mAltitude);

   mAltitudeRate       = -mVelNED[2];
   mCurrentLinearSpeed = mVelNED.Magnitude();
   mCurrentGroundSpeed = std::sqrt(mVelNED[0] * mVelNED[0] + mVelNED[1] * mVelNED[1]);

   // Here is where we re-orient the body into the velocity vector,
   // which may modify the motion state of the initial entity!

   // This object will re-orient the attitude to the velocity vector,
   // but preserve the roll angle.  May change heading and pitch!!!
   if (mCurrentLinearSpeed > c3D_MOTION_TOO_SLOW)
   {
      mGammaNED = -std::asin(mVelNED[2] / mCurrentLinearSpeed);
   }
   if (mCurrentGroundSpeed > c3D_MOTION_TOO_SLOW)
   {
      mCourseNED = std::atan2(mVelNED[1], mVelNED[0]);
   }

   // EVEN THOUGH THIS IS A "GET" FUNCTION, HERE WE HAVE TO DO A RE-"SET":
   // Reorient the body the way our 3-DOF motion dictates:
   mEntityPtr->SetOrientationNED(mCourseNED, mGammaNED, mRollNED);

   GetDCMsAndQuatFromEntity();
}

// =================================================================
void UtMath3D_Motion::SetMotionToInitConditions()
{
   // Now, if needed, modify the initial conditions according to input requests:
   if ((mInitialSpeed == 0.0) && (mInitialFPA == 0))
      return;

   mCurrentLinearSpeed = mInitialSpeed;
   if (mInitialFPA != 0)
   {
      mGammaNED = mInitialFPA;
   }
   UtVec3dX newVelNED(mCourseNED, mGammaNED, mCurrentLinearSpeed, true);
   mVelNED             = newVelNED;
   mAltitudeRate       = -mVelNED[2];
   mCurrentGroundSpeed = std::sqrt(mVelNED[0] * mVelNED[0] + mVelNED[1] * mVelNED[1]);

   // EVEN THOUGH THIS IS A "GET" FUNCTION, HERE WE HAVE TO DO A RE-"SET":
   // Reorient the body the way our 3-DOF motion dictates:
   mEntityPtr->SetOrientationNED(mCourseNED, mGammaNED, mRollNED);

   GetDCMsAndQuatFromEntity();
}

// =================================================================
void UtMath3D_Motion::GetDCMsAndQuatFromEntity()
{
   // Accept the WCS to NED DCM from UtEntity.
   mWCStoNED = mEntityPtr->GetDcmWCStoNED(); // = f(Lat, Lon, Alt)
#ifdef ASSERT_TRANSFORMS
   assert(mWCStoNED.IsValid(true));
#endif

   mWCStoVelAndECS = mEntityPtr->GetDcmWCStoECS();
#ifdef ASSERT_TRANSFORMS
   assert(mWCStoVelAndECS.IsValid(true));
#endif
   mQuatWCStoVelAndECS.Set(mWCStoVelAndECS);

   // Now confirm that my matrix manipulations come out the same:
   // if mWCStoVelAndECS = mNEDtoECS * mWCStoNED,
   // then mNEDtoECS = mWCStoVelAndECS * mNEDtoWCS;
   // and mNEDtoECS = mWCStoVelAndECS * mWCStoNED.Transpose();
   mNEDtoECS = mWCStoVelAndECS * mWCStoNED.Transpose();
#ifdef ASSERT_TRANSFORMS
   assert(mNEDtoECS.IsValid(true));
#endif
   // double myHdg, myGamma, myRoll;
   // mNEDtoECS.Get(myHdg, myGamma, myRoll); // Checked out okay.
}

// =================================================================
UtVec3dX UtMath3D_Motion::VelNED() const
{
   return mVelWCS * mWCStoNED;
}

// =================================================================
void UtMath3D_Motion::NED_Orientation(double& aHeading, double& aPitch, double& aRoll)
{
   aHeading = mCourseNED;
   aPitch   = mGammaNED;
   aRoll    = mRollNED;
}

// =================================================================
// virtual
bool UtMath3D_Motion::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "detailed_debug")
   {
      aInput.ReadValue(mDetailDebug);
   }
   else if (command == "prefer_canopy_up")
   {
      aInput.ReadValue(mPreferCanopyUp);
   }
   else if (command == "bank_to_turn")
   {
      bool bankToTurn;
      aInput.ReadValue(bankToTurn);
      mPreferCanopyUp = !bankToTurn;
   }
   else if (command == "broach_at_sea_level")
   {
      aInput.ReadValue(mBroachAtSeaLevel);
   }
   else if (command == "waypoint_switch_on_ground_turning_radius")
   {
      aInput.ReadValue(mUseGrndTrnRadius);
   }
   else if ((command == "target_speed") || (command == "desired_speed"))
   {
      aInput.ReadValueOfType(mTargetLinearSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mTargetLinearSpeed, 0.0);
   }
   else if (command == "initial_speed")
   {
      aInput.ReadValueOfType(mInitialSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mInitialSpeed, 0.0);
   }
   else if (command == "initial_flight_path_angle")
   {
      aInput.ReadValueOfType(mInitialFPA, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mInitialFPA, 0.0);
   }
   else if ((command == "maximum_linear_acceleration") ||
            (command == "default_linear_acceleration")) // backward compatibility
   {
      aInput.ReadValueOfType(mMaxLinearAccel, UtInput::cACCELERATION);
      aInput.ValueGreater(mMaxLinearAccel, 0.0);
   }
   else if ((command == "maximum_radial_acceleration") ||
            (command == "default_radial_acceleration")) // backward compatibility
   {
      aInput.ReadValueOfType(mMaxNormalAccel, UtInput::cACCELERATION);
      aInput.ValueGreater(mMaxNormalAccel, 0.0);
   }
   else if (command == "maximum_body_turn_rate")
   {
      aInput.ReadValueOfType(mMaxTangentialOmega, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mMaxTangentialOmega, 0.0);
   }
   else if (command == "maximum_body_roll_rate")
   {
      aInput.ReadValueOfType(mMaxRollRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(mMaxRollRate, 0.0);
   }
   else if (command == "default_climb_rate")
   {
      double valueNotUsed;
      aInput.ReadValueOfType(valueNotUsed, UtInput::cSPEED);
      ut::log::info() << "WSF_KINEMATIC_MOVER will disregard the supplied value for 'default_climb_rate'.";
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
void UtMath3D_Motion::SetDesiredAccelWCS(const UtVec3dX& aAccelWCS)
{
   mDesiredAccel      = aAccelWCS;
   mDesiredAccelIsWCS = true;
   mIsFlyingToPoint   = false;
#ifdef ASSERT_TRANSFORMS
   assert(aAccelWCS[0] == aAccelWCS[0]);
   assert(aAccelWCS[1] == aAccelWCS[1]);
   assert(aAccelWCS[2] == aAccelWCS[2]);
#endif
}

// =================================================================
void UtMath3D_Motion::SetDesiredAccelECS(const UtVec3dX& aAccelECS)
{
   mDesiredAccel      = aAccelECS;
   mDesiredAccelIsWCS = false;
   mIsFlyingToPoint   = false;
#ifdef ASSERT_TRANSFORMS
   assert(aAccelECS[0] == aAccelECS[0]);
   assert(aAccelECS[1] == aAccelECS[1]);
   assert(aAccelECS[2] == aAccelECS[2]);
#endif
}

// =================================================================
void UtMath3D_Motion::SetFlyToPoint(const UtVec3dX& aAimpointWCS)
{
   mIsFlyingToPoint = true;
   mDesiredLocWCS   = aAimpointWCS;
}

// =================================================================
void UtMath3D_Motion::SetFlyToPoint(double aLat, double aLon, double aAlt)
{
   UtVec3dX flyToWCS;
   UtEntity::ConvertLLAToWCS(aLat, aLon, aAlt, flyToWCS.GetData());
   SetFlyToPoint(flyToWCS);
}

// =================================================================
double UtMath3D_Motion::TurnRadius() const
{
   return mCurrentLinearSpeed / mMaxTangentialOmega;
}

// =================================================================
double UtMath3D_Motion::GroundTurnRadius() const
{
   return mCurrentGroundSpeed / mMaxTangentialOmega;
}

// =================================================================
bool UtMath3D_Motion::FlyToPointReached(double /*aSimTime*/) const
{
   bool     isReached = false;
   UtVec3dX diffWCS   = mLocWCS - mDesiredLocWCS;
   double   kFactor   = 1.0 / std::cos(mGammaNED);
   double   rToGo;
   double   currentR;

   if (mUseGrndTrnRadius)
   {
      UtVec3dX diffNED = diffWCS * mWCStoNED;
      currentR         = std::sqrt(diffNED[0] * diffNED[0] + diffNED[1] * diffNED[1]);
      rToGo            = kFactor * GroundTurnRadius();
   }
   else
   {
      currentR = diffWCS.Magnitude();
      rToGo    = kFactor * TurnRadius();
   }

   isReached = currentR < rToGo;
   return isReached;
}

// =================================================================
// virtual
void UtMath3D_Motion::Update(double aSimTime)
{
   const UtVec3dX cECS_UNIT_FORWARD(1.0, 0.0, 0.0);

   if (aSimTime > mLocalLastUpdateTime)
   {
      double   dt              = aSimTime - mLocalLastUpdateTime;
      double   prevLinearSpeed = mCurrentLinearSpeed;
      double   prevGroundSpeed = mCurrentGroundSpeed;
      UtVec3dX prevVelWCS      = mVelWCS;

      // Obtain "old" extrapolated velocity for motion integration
      mPrevDeltaWCS = ((mCurrentLinearSpeed * dt) * cECS_UNIT_FORWARD) * mWCStoVelAndECS.Transpose();

      // Compute a new speed if needed.
      UpdateLinearSpeed(dt);

      // Transform the desired guidance into pitch/yaw/roll rates.
      UpdateGuidance(aSimTime);

      // Compute resulting quaternion rate (inertial frame) from body rates (ECS frame).
      mQuatRate.SetRate(mQuatWCStoVelAndECS, mPQR);

      // Now re-orient the quaternion (which directs the velocity vector) to a new direction.
      mQuatWCStoVelAndECS.Update(mQuatRate, dt);
      // mQuatWCStoVelAndECS.Normalize(); // Placed in the UtQuaternion class, directly.

      // Transform Quaternion into equivalent DCM.
      mQuatWCStoVelAndECS.Get(mWCStoVelAndECS);

      // Obtain new delta vector for this state.
      mCurrentDeltaWCS = ((mCurrentLinearSpeed * dt) * cECS_UNIT_FORWARD) * mWCStoVelAndECS.Transpose();

      // Propagate the average between prev and current extrapolated velocities.
      mLocWCS += 0.5 * (mPrevDeltaWCS + mCurrentDeltaWCS);
      mVelWCS = (mCurrentLinearSpeed * cECS_UNIT_FORWARD) * mWCStoVelAndECS.Transpose();
      mAccWCS = (mVelWCS - prevVelWCS) / dt;

      mEntityPtr->SetLocationWCS(mLocWCS.GetData());
      mEntityPtr->SetVelocityWCS(mVelWCS.GetData());
      mEntityPtr->SetAccelerationWCS(mAccWCS.GetData());

      // Attitude will be set in NED terms in the call below.
      UpdateNED_AfterWCS_Move(aSimTime);

      mPathDistanceTravelled += 0.5 * (prevLinearSpeed + mCurrentLinearSpeed) * dt;
      mGroundDistanceTravelled += 0.5 * (prevGroundSpeed + mCurrentGroundSpeed) * dt;
      mTimeTravelled += dt;
      mLocalLastUpdateTime = aSimTime;

      if (mDetailDebug)
      {
         PrintState(aSimTime);
      }
   }
}

// =================================================================
void UtMath3D_Motion::UpdateGuidance(double aSimTime)
{
   const double cROLL_RATE_GAIN = 0.5;

   UtVec3dX desiredAccelECS;
   // double dt = aSimTime - mLocalLastUpdateTime;  // Currently unused

   if (mIsFlyingToPoint)
   {
      desiredAccelECS =
         mGuidance.AccelerationVel(aSimTime, mLocWCS, mVelWCS, mDesiredLocWCS, mWCStoVelAndECS, mWCStoVelAndECS, mWCStoNED);
   }
   else
   {
      // Transform guidance (if any) into ECS frame:
      if (mDesiredAccelIsWCS)
      {
         desiredAccelECS = mDesiredAccel * mWCStoVelAndECS;
      }
      else
      {
         desiredAccelECS = mDesiredAccel;
      }
   }

   // Dimensional analysis - angular rates are acceleration / velocity
   if (mCurrentLinearSpeed > c3D_MOTION_TOO_SLOW)
   {
      UtVec3dX desiredECSRates = desiredAccelECS / mCurrentLinearSpeed;

      // Ignore inertial roll rate, as will be overridden below anyway.
      // mPQR[0] = 0.0;

      // A positive side acceleration (in Y direction) denotes a
      // desire for a yaw rate (Z) !!!
      mPQR[2] = desiredECSRates[1];

      // A positive downward acceleration (in Z direction) denotes a
      // desire for a negative pitch rate (Y) !!!
      mPQR[1] = -desiredECSRates[2];

#ifdef ASSERT_TRANSFORMS
      assert(mPQR[0] == mPQR[0]);
      assert(mPQR[1] == mPQR[1]);
      assert(mPQR[2] == mPQR[2]);
#endif
   }

   // Compute desired roll rate to hit target bank angle:
   if (mPreferCanopyUp)
   {
      mDesiredBankNED = 0.0;
   }
   else
   {
      mDesiredBankNED = std::atan2(desiredAccelECS[1], UtEarth::cACCEL_OF_GRAVITY);
   }

   mPQR[0] = cROLL_RATE_GAIN * UtMath::NormalizeAngleMinusPi_Pi(mDesiredBankNED - mRollNED);

   ConstrainBodyRates();
}

// =================================================================================================
void UtMath3D_Motion::UpdateNED_AfterWCS_Move(double aSimTime)
{
   double prevAlt = mAltitude;

   // ==================== START
   // Get the attributes that we cannot directly control in this class:

   // Get new mWCStoNED from UtEntity.
   double tempDCM[3][3];
   mEntityPtr->GetWCSToNEDTransform(tempDCM);
   mWCStoNED.Set(tempDCM);

#ifdef ASSERT_TRANSFORMS
   assert(mWCStoNED.IsValid(true));
#endif

   // Get new LLA.
   mEntityPtr->GetLocationLLA(mLatitudeDeg, mLongitudeDeg, mAltitude);
   // ==================== END

   // Since we now have a DCM, can transform our velocity into NED:
   mVelNED             = mVelWCS * mWCStoNED;
   mAccNED             = mAccWCS * mWCStoNED;
   mAltitudeRate       = -mVelNED[2];
   mCurrentGroundSpeed = std::sqrt(mVelNED[0] * mVelNED[0] + mVelNED[1] * mVelNED[1]);

   if (mCurrentLinearSpeed > c3D_MOTION_TOO_SLOW)
   {
      mGammaNED = -std::asin(mVelNED[2] / mCurrentLinearSpeed);
   }
   if (mCurrentGroundSpeed > c3D_MOTION_TOO_SLOW)
   {
      mCourseNED = std::atan2(mVelNED[1], mVelNED[0]);
   }

   mNEDtoECS = mWCStoVelAndECS * mWCStoNED.Transpose();

#ifdef ASSERT_TRANSFORMS
   assert(mWCStoVelAndECS.IsValid(true));
#endif

#ifdef ASSERT_TRANSFORMS
   assert(mWCStoNED.IsValid(true));
#endif

#ifdef ASSERT_TRANSFORMS
   assert(mNEDtoECS.IsValid(true));
#endif

   // Get New NED orientation:
   double heading;
   double pitch;
   mNEDtoECS.Get(heading, pitch, mRollNED);

   // EVEN THOUGH THIS IS A "GET" FUNCTION, HERE WE HAVE TO DO A RE-"SET":
   // Reorient the body the way our 3-DOF motion dictates:
   mEntityPtr->SetOrientationNED(mCourseNED, mGammaNED, mRollNED);

   if (mBroachAtSeaLevel && prevAlt < 0.0 && mAltitude > 0.0)
   {
      Broach(aSimTime);
   }
}

// =================================================================================================
void UtMath3D_Motion::Broach(double aSimTime)
{
   // We crossed over sea-level, and for an immersed torpedo (for instance),
   // that is a very bad thing.  Fake the dynamics of a torpedo hitting the
   // surface and porpoising back, with an instantaneous kludge:

   double   dt         = aSimTime - mLocalLastUpdateTime;
   UtVec3dX prevVelNED = mVelNED;

   // A.  Reposition the body 1 meter below sea level, at the same Lat, Lon as before.
   UtEntity::ConvertLLAToWCS(mLatitudeDeg, mLongitudeDeg, -1.0, mLocWCS.GetData());

   // B.  Cut the linear speed in half.
   mCurrentLinearSpeed *= 0.5;

   // C.  Take the (upward) vertical velocity, cut by half, and negate it (Porpoise).
   mAltitudeRate = -0.5 * mVelNED[2];

   // D.  Set the Flight Path Angle, from known values for vertical speed and total speed.
   mGammaNED = asin(mAltitudeRate / mCurrentLinearSpeed);

   // E.  Keep the NED course the same.
   mVelNED = UtVec3dX(mCourseNED, mGammaNED, mCurrentLinearSpeed, true);
   mAccNED = (mVelNED - prevVelNED) / dt;

   // F.  Force pitch, yaw, and roll rates to zero.
   mPQR.Null();

   // G.  Keep the NED bank angle the same.
   // mRollNED = mRollNED;

   // Now "Set" both internal and external attributes so that all is consistent.
   mEntityPtr->SetLocationWCS(mLocWCS.GetData());
   mEntityPtr->SetVelocityNED(mVelNED.GetData());
   mEntityPtr->GetVelocityWCS(mVelWCS.GetData());
   mEntityPtr->SetAccelerationNED(mAccNED.GetData());
   mEntityPtr->GetAccelerationWCS(mAccWCS.GetData());
   mEntityPtr->SetOrientationNED(mCourseNED, mGammaNED, mRollNED);

   GetDCMsAndQuatFromEntity();
}

// =================================================================================================
void UtMath3D_Motion::ConstrainBodyRates()
{
   // Assure roll rate limit within constraints.
   if (mPQR[0] > mMaxRollRate)
   {
      mPQR[0] = mMaxRollRate;
   }

   if (mPQR[0] < -mMaxRollRate)
   {
      mPQR[0] = -mMaxRollRate;
   }

   // Assure maximum mMaxTangentialOmega limit does not get exceeded:
   double requTangentialRate = std::sqrt(mPQR[1] * mPQR[1] + mPQR[2] * mPQR[2]);
   double limitRate          = mMaxTangentialOmega;
   double otherLimitRate     = mMaxNormalAccel / mCurrentLinearSpeed;
   if (otherLimitRate < limitRate)
   {
      limitRate = otherLimitRate;
   }
   if (requTangentialRate > limitRate)
   {
      // Limit the axial/tangential rate to the max.
      double attenuation = limitRate / requTangentialRate;
      mPQR[1] *= attenuation;
      mPQR[2] *= attenuation;
   }

#ifdef ASSERT_TRANSFORMS
   // assert(mPQR[0] <= mMaxRollRate);
   double tangentialOmega = std::sqrt(mPQR[1] * mPQR[1] + mPQR[2] * mPQR[2]);
   assert(tangentialOmega <= mMaxTangentialOmega + 0.00001);
#endif
}

// =================================================================================================
void UtMath3D_Motion::UpdateLinearSpeed(double aDeltaTime)
{
   double velDiff = mTargetLinearSpeed - mCurrentLinearSpeed;
   if (velDiff == 0.0)
      return;

   double dV = mMaxLinearAccel * aDeltaTime;
   if (velDiff > 0)
   {
      mCurrentLinearSpeed += dV;
      if (mCurrentLinearSpeed > mTargetLinearSpeed)
      {
         mCurrentLinearSpeed = mTargetLinearSpeed;
      }
   }
   else
   {
      mCurrentLinearSpeed -= dV;
      if (mCurrentLinearSpeed < mTargetLinearSpeed)
      {
         mCurrentLinearSpeed = mTargetLinearSpeed;
      }
   }
}

// =================================================================================================
void UtMath3D_Motion::PrintState(double aSimTime)
{
   auto out = ut::log::info() << "Motion State:";
   out.AddNote() << "T = " << aSimTime;
   out.AddNote() << "Loc XYZ: " << int(mLocWCS[0]) << ", " << int(mLocWCS[1]) << ", " << int(mLocWCS[2]);
   out.AddNote() << "Vel XYZ: " << mVelWCS[0] << ", " << mVelWCS[1] << ", " << mVelWCS[2];
   out.AddNote() << "Vel NED: " << mVelNED[0] << ", " << mVelNED[1] << ", " << mVelNED[2];
   out.AddNote() << "Acc XYZ: " << mAccWCS[0] << ", " << mAccWCS[1] << ", " << mAccWCS[2];
   out.AddNote() << "Acc NED: " << mAccNED[0] << ", " << mAccNED[1] << ", " << mAccNED[2];
   out.AddNote() << "Alt: " << mAltitude;
   out.AddNote() << "AltRt: " << int(mAltitudeRate);
   out.AddNote() << "Speed: " << mCurrentLinearSpeed;
   out.AddNote() << "HdgD: " << int(mCourseNED * UtMath::cDEG_PER_RAD);
   out.AddNote() << "GammaD: " << int(mGammaNED * UtMath::cDEG_PER_RAD);
   out.AddNote() << "RollD: " << int(mRollNED * UtMath::cDEG_PER_RAD);
}
