// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfPursueManeuver.hpp"

#include <cmath>
#include <memory>

#include "RelativeManeuverState.hpp"
#include "UtEarth.hpp"
#include "UtMemory.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace // {anonymous}
{

//! Duration between evaluations for this maneuver. [s]
constexpr double cEVALUATION_INTERVAL = 1.0;

//! Factor used to increase the lookahead on the target's motion.
constexpr double cLOOKAHEAD_FACTOR = 7.0;

//! How far to look ahead in time to avoid running into the surface. [s]
constexpr double cIMPACT_LOOKAHEAD_TIME = 15.0;

//! Altitude floor for avoiding surface impact. [m]
constexpr double cIMPACT_LOOKAHEAD_FLOOR = 100.0;

//! G load to use to avoid surface impact.
constexpr double cIMPACT_AVOIDANCE_GLOAD = 6.0;

//! Angle in delta roll that indicates an inversion event. [deg]
constexpr double cINVERTING_LIMIT_ENTER = 160.0;

//! Angle in delta roll that indicates the end of an inversion event. [deg]
constexpr double cINVERTING_LIMIT_EXIT = 30.0;

//! Cosine theta for determining if the chaser should turn harder.
constexpr double cHARD_TURN_LIMIT = 0.8;

//! Angle used to decide if the chaser is nearly vertical. [rad]
constexpr double cNEAR_VERTICAL_LIMIT = 80.0 * UtMath::cRAD_PER_DEG;

//! Relative maneuver state that will fly a platform to get onto the turn circle of the target.
class PursueManeuverState : public RelativeManeuverState
{
public:
   PursueManeuverState(const WsfPursueManeuver& aManeuver, WsfRelativeManeuver::Data& aData)
      : mManeuver(aManeuver)
      , mData(aData)
   {
   }
   ~PursueManeuverState() override = default;

   std::unique_ptr<RelativeManeuverState> Evaluate() override
   {
      GetTargetData();
      SolveAirLambertProblem();
      ComputeControls();
      return nullptr;
   }

   double GetEvaluationInterval() const override { return cEVALUATION_INTERVAL; }

private:
   void GetTargetData()
   {
      ManeuverTurnCircle targetCircle{mData.mKinematics.mLocWCS, mData.mKinematics.mVelWCS, mData.mKinematics.mAccWCS};
      // The target location is the position of the target, predicted forward by a factor of the
      // evaluation interval.
      double pursuitDistance = mManeuver.GetPursuitDistanceMeters() +
                               targetCircle.GetSpeedMetersPerSecond() * cLOOKAHEAD_FACTOR * cEVALUATION_INTERVAL;
      double theta       = pursuitDistance / targetCircle.GetRadiusMeters();
      mTargetLocationWCS = targetCircle.GetLocationOnCircle(theta);
      mTargetVelocityWCS = targetCircle.GetVelocityOnCircle(theta);
   }

   // By analogy with the Lambert problem for space-domain platforms, the 'Air Lambert'
   // problem gives the turn circle connecting the location of a chaser to the location
   // of a target. This circle will be in the plane defined by the separation of the two
   // points and the velocity of the chaser.
   void SolveAirLambertProblem()
   {
      mData.mChaserPlatformPtr->GetVelocityWCS(mChaserVelocityWCS.GetData());

      // Find the normal to the plane the solution will lie in.
      UtVec3d v1 = mChaserVelocityWCS * cEVALUATION_INTERVAL;
      UtVec3d v2 = mTargetLocationWCS - mData.mChaserLocWCS;
      mN_Hat.CrossProduct(v1, v2).Normalize();

      // The delta direction points from chaser to target in the solution frame.
      // The B direction completes the right-handed frame (Delta, B, N).
      UtVec3d delta          = mTargetLocationWCS - mData.mChaserLocWCS;
      double  halfSeparation = delta.Magnitude() / 2.0;
      UtVec3d deltaHat       = delta.GetNormal();
      UtVec3d bHat;
      bHat.CrossProduct(mN_Hat, deltaHat);

      // Compute the radius and half angle. In the (Delta, B) plane we have a separation
      // from the center to the chaser's location of (-halfSeparation, -C). And that
      // vector must be perpendicular to the velocity of the chaser, so the dot product is
      // set to zero to solve for C, and from that the radius.
      double velDeltaComp = mChaserVelocityWCS.DotProduct(deltaHat);
      double velB_Comp    = mChaserVelocityWCS.DotProduct(bHat);
      double C            = -halfSeparation * velDeltaComp / velB_Comp;
      mRadius             = sqrt(C * C + halfSeparation * halfSeparation);
      double halfAngle    = atan2(halfSeparation, C);

      // Compute center point
      UtVec3d origin    = (mTargetLocationWCS + mData.mChaserLocWCS) * 0.5;
      UtVec3d offset    = bHat * C;
      UtVec3d centerWCS = origin + offset;

      // Finish the X, Y, N frame. X points from the center to the starting location.
      // N is the plane normal, and Y (unused and so undefined) completes the right-handed frame.
      mX_Hat = (mData.mChaserLocWCS - centerWCS).GetNormal();

      // Compute the needed speed to traverse the solution arc in the time alloted.
      double speedRequired = 2.0 * halfAngle * mRadius / (cLOOKAHEAD_FACTOR * cEVALUATION_INTERVAL);
      speedRequired        = mManeuver.LimitSpeed(speedRequired);
      double ratio         = speedRequired / mChaserVelocityWCS.Magnitude();
      mChaserVelocityWCS *= ratio;
   }

   bool AboutToImpactGround() const
   {
      UtVec3d     prediction = mData.mChaserLocWCS + mChaserVelocityWCS * cIMPACT_LOOKAHEAD_TIME;
      WsfGeoPoint point{prediction.GetData()};
      return point.GetAlt() < cIMPACT_LOOKAHEAD_FLOOR;
   }

   double ComputeRollAngleDeg(double aFn_x, double aFn_n)
   {
      // The direction opposite to the normal force
      UtVec3d localDownWCS = mX_Hat * (-aFn_x) + mN_Hat * (-aFn_n);

      // Convert to ECS frame
      UtVec3d localDownECS;
      mData.mChaserPlatformPtr->ConvertWCSVectorToECS(localDownECS.GetData(), localDownWCS.GetData());

      // Compute roll angle control input
      mDeltaRollDeg = atan2(localDownECS.Get(1), localDownECS.Get(2)) * UtMath::cDEG_PER_RAD;
      if (!mInverting)
      {
         mInverting = fabs(mDeltaRollDeg) > cINVERTING_LIMIT_ENTER;
      }
      else
      {
         mInverting = fabs(mDeltaRollDeg) > cINVERTING_LIMIT_EXIT;
      }
      double heading;
      double pitch;
      double roll;
      mData.mChaserPlatformPtr->GetOrientationNED(heading, pitch, roll);
      roll *= UtMath::cDEG_PER_RAD;

      // If we are near to vertically oriented, we will avoid the roll singularity when
      // applying control.
      mIsNearVertical = fabs(pitch) > cNEAR_VERTICAL_LIMIT;

      return UtMath::NormalizeAngleMinus180_180(roll - mDeltaRollDeg);
   }

   double ComputeG_Load(double aFn_x, double aFn_n) const
   {
      double retval{0.0};
      // Unless the chaser is inverting, ...
      if (!mInverting)
      {
         // ... pull as hard as is needed to keep the chaser on the solution,
         retval = sqrt(aFn_x * aFn_x + aFn_n * aFn_n) / UtEarth::cACCEL_OF_GRAVITY;

         // ... or harder if the chaser needs to turn around more quickly.
         UtVec3d chaserVelHat = mChaserVelocityWCS.GetNormal();
         double  vDotV        = chaserVelHat.DotProduct(mTargetVelocityWCS.GetNormal());
         UtVec3d deltaPos     = (mTargetLocationWCS - mData.mChaserLocWCS).GetNormal();
         double  vDotX        = deltaPos.DotProduct(chaserVelHat);
         if (vDotV < cHARD_TURN_LIMIT && vDotX < cHARD_TURN_LIMIT)
         {
            // This branch is entered if the target and chaser are not moving in
            // roughly the same direction, and if the target is not ahead of the
            // chaser.
            retval *= 2.0;
         }
      }
      return mManeuver.LimitG_Load(retval);
   }

   double ComputeSpeedKTAS() const
   {
      // This value is limited in SolveAirLambertProblem above, because the limited
      // speed value is used when computing the normal force.
      return mChaserVelocityWCS.Magnitude() * UtMath::cNMPH_PER_MPS;
   }

   void ComputeControls()
   {
      if (AboutToImpactGround())
      {
         // Don't fly into the ground.
         mData.mChaserMoverPtr->SetPitchGLoad(cIMPACT_AVOIDANCE_GLOAD);
         mData.mChaserMoverPtr->SetAutopilotRollAngle(0.0);
         mData.mChaserMoverPtr->SetAutopilotSpeedKTAS(mChaserVelocityWCS.Magnitude() * UtMath::cNMPH_PER_MPS);
      }
      else
      {
         // Compute the normal acceleration (i.e., apply Newton's Second Law)
         UtVec3d gvec        = mData.mChaserLocWCS.GetNormal() * (-UtEarth::cACCEL_OF_GRAVITY);
         double  vMag        = mChaserVelocityWCS.Magnitude();
         double  Fn_over_m_x = -vMag * vMag / mRadius - gvec.DotProduct(mX_Hat);
         double  Fn_over_m_n = -gvec.DotProduct(mN_Hat);

         // Compute control inputs
         double rollAngleDeg = ComputeRollAngleDeg(Fn_over_m_x, Fn_over_m_n);
         double gLoad        = ComputeG_Load(Fn_over_m_x, Fn_over_m_n);
         double speedKTAS    = ComputeSpeedKTAS();

         // Apply controls
         mData.mChaserMoverPtr->SetPitchGLoad(gLoad);
         if (mIsNearVertical)
         {
            // When near vertical, set delta roll instead of roll to avoid the
            // singularity in the definition of roll.
            mData.mChaserMoverPtr->SetAutopilotDeltaRoll(-mDeltaRollDeg);
         }
         else
         {
            mData.mChaserMoverPtr->SetAutopilotRollAngle(rollAngleDeg);
         }
         mData.mChaserMoverPtr->SetAutopilotSpeedKTAS(speedKTAS);
      }
   }

   const WsfPursueManeuver&   mManeuver;
   WsfRelativeManeuver::Data& mData;

   // Target related
   UtVec3d mTargetLocationWCS{};
   UtVec3d mTargetVelocityWCS{};

   // Chaser related
   UtVec3d mChaserVelocityWCS{};

   // Solved motion related
   double mRadius{0.0};

   // Solution frame
   UtVec3d mX_Hat{};
   UtVec3d mN_Hat{};

   // Control related
   double mDeltaRollDeg{0.0};
   bool   mInverting{false};
   bool   mIsNearVertical{false};
};

} // namespace

WsfPursueManeuver::WsfPursueManeuver()
   : WsfRelativeManeuver{ut::make_unique<PursueManeuverState>(*this, mData)}
{
}

WsfPursueManeuver::WsfPursueManeuver(const WsfPursueManeuver& aOther)
   : WsfRelativeManeuver(aOther)
   , mTargetPlatformName{aOther.mTargetPlatformName}
   , mPursuitDistance_m{aOther.mPursuitDistance_m}
{
   mStatePtr = ut::make_unique<PursueManeuverState>(*this, mData);
}

bool WsfPursueManeuver::UpdateData()
{
   bool                 retval{false};
   const WsfSimulation* simulationPtr = GetSimulation();
   if (simulationPtr)
   {
      WsfPlatform* targetPtr = simulationPtr->GetPlatformByName(mTargetPlatformName);
      if (targetPtr)
      {
         // First get the target kinematics
         targetPtr->GetLocationWCS(mData.mKinematics.mLocWCS.GetData());
         targetPtr->GetVelocityWCS(mData.mKinematics.mVelWCS.GetData());
         targetPtr->GetAccelerationWCS(mData.mKinematics.mAccWCS.GetData());
         targetPtr->GetOrientationNED(mData.mKinematics.mAnglesNED[0],
                                      mData.mKinematics.mAnglesNED[1],
                                      mData.mKinematics.mAnglesNED[2]);
         mData.mKinematics.mG_Load = 0.0; // NOTE: The maneuver does not need the g-load

         // Then save some chaser information
         mData.mChaserMoverPtr    = GetMover();
         mData.mChaserPlatformPtr = mData.mChaserMoverPtr->GetPlatform();
         mData.mChaserPlatformPtr->GetLocationWCS(mData.mChaserLocWCS.GetData());
         mData.mSeparationWCS.Subtract(mData.mChaserLocWCS, mData.mKinematics.mLocWCS);

         retval = true;
      }
   }
   return retval;
}
