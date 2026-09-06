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

#include "WsfSixDOF_StationKeepingState.hpp"

#include "WsfPlatform.hpp"
#include "WsfSixDOF_Formation.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "maneuvers/WsfSixDOF_RelativeManeuver.hpp"

namespace
{
constexpr double cKEEP_STATION_TO_FORM_UP_DISTANCE = 500.0;
constexpr double cKEEP_STATION_DELTA_T             = 0.1;
constexpr double cKEEP_STATION_GLOAD_ALPHA         = 0.05;
constexpr double cKEEP_STATION_GLOAD_BETA          = 0.1;
constexpr double cKEEP_STATION_ROLL_ALPHA          = 0.7;
constexpr double cKEEP_STATION_ROLL_BETA           = 3.0;
constexpr double cKEEP_STATION_ROLL_GAMMA          = 6.0;
constexpr double cKEEP_STATION_SPEED_ALPHA         = 0.5;
constexpr double cKEEP_STATION_SPEED_BETA          = 1.0;
constexpr double cKEEP_STATION_SPEED_GAMMA         = 5.0;
constexpr int    cKEEP_STATION_HITS_TO_PURSUE      = 3;

constexpr double cFORM_UP_TO_KEEP_STATION_DISTANCE = 200.0;
constexpr double cFORM_UP_CLOSING_DISTANCE         = 3000.0;
constexpr double cFORM_UP_CLOSING_COSTHETA         = 0.4;
constexpr double cFORM_UP_DELTA_T                  = 1.0;
constexpr double cFORM_UP_ROLL_MAX_ALPHA           = 1.5;
constexpr double cFORM_UP_ROLL_ALPHA_SCALING       = 0.05 / 100.0;
constexpr double cFORM_UP_CLOSING_ALPHA_FACTOR     = 1.0e-3;
constexpr double cFORM_UP_CLOSING_BETA_FACTOR      = 5.0e-3;
constexpr double cFORM_UP_SPEED_COSTHETA           = 0.3;

constexpr double cPURSUE_DELTA_T              = 1.0;
constexpr double cPURSUE_GLOAD_ALPHA          = 0.002;
constexpr double cPURSUE_GLOAD_BETA           = 0.04;
constexpr double cPURSUE_ROLL_ALPHA           = 0.2;
constexpr double cPURSUE_ROLL_BETA            = 3.0;
constexpr double cPURSUE_SPEED_RANGE          = 5.0;
constexpr double cPURSUE_SPEED_FACTOR         = 100.0;
constexpr int    cPURSUE_HITS_TO_KEEP_STATION = 3;
} // namespace

std::unique_ptr<wsf::six_dof::RelativeManeuverState> wsf::six_dof::FormUpState::Evaluate()
{
   std::unique_ptr<RelativeManeuverState> retvalPtr{nullptr};

   if (mData.mSeparationWCS.Magnitude() < cFORM_UP_TO_KEEP_STATION_DISTANCE)
   {
      // Once the chaser gets close enough, transition to KeepStationState.
      retvalPtr = ut::make_unique<KeepStationState>(mData, mManeuver);
   }
   else
   {
      UtEntity utilEntity = mData.mKinematics.GetEntity();

      double rollAngle_deg = ComputeRollAngleDeg(utilEntity);
      double speed_ktas    = ComputeSpeedKTAS();
      double altitude_ft   = utilEntity.GetAltitude() * UtMath::cFT_PER_M;

      mData.mChaserMoverPtr->SetAutopilotRollAngle(rollAngle_deg);
      mData.mChaserMoverPtr->SetAutopilotAltitude(altitude_ft);
      mData.mChaserMoverPtr->SetAutopilotSpeedKTAS(speed_ktas);
   }

   return retvalPtr;
}

double wsf::six_dof::FormUpState::GetEvaluationInterval() const
{
   return cFORM_UP_DELTA_T;
}

double wsf::six_dof::FormUpState::ComputeRollAngleDeg(const UtEntity& aEntity) const
{
   // Depending on if the velocity of the chaser and the target are aligned,
   // or anti-aligned, the computed roll would need to be swapped.
   UtVec3d velChaser;
   mData.mChaserPlatformPtr->GetVelocityWCS(velChaser.GetData());
   double vDotV = UtVec3d::DotProduct(mData.mKinematics.mVelWCS, velChaser) /
                  (mData.mKinematics.mVelWCS.Magnitude() * velChaser.Magnitude());
   double rollAngle = mData.mKinematics.mAnglesNED[2] * vDotV;

   double sep = mData.mSeparationWCS.Magnitude();
   if (vDotV > cFORM_UP_CLOSING_COSTHETA && sep < cFORM_UP_CLOSING_DISTANCE)
   {
      // If the velocities are roughly aligned, and the chaser is not too far from
      // the target, then a more direct control scheme is used that is based on the ECS frame
      // of the target point.

      // delta position in ECS frame
      UtVec3d deltaLoc;
      aEntity.ConvertWCSToECS(mData.mChaserLocWCS.GetData(), deltaLoc.GetData());

      // delta velocity in ECS frame
      UtVec3d chaserVelWCS;
      mData.mChaserPlatformPtr->GetVelocityWCS(chaserVelWCS.GetData());
      UtVec3d deltaVelWCS = chaserVelWCS - mData.mKinematics.mVelWCS;
      UtVec3d deltaVel;
      aEntity.ConvertWCSVectorToECS(deltaVel.GetData(), deltaVelWCS.GetData());

      // We use similar control gains as in station keeping, but reduced in scale
      // given the much larger typical separation.
      constexpr double alpha{cKEEP_STATION_ROLL_ALPHA * cFORM_UP_CLOSING_ALPHA_FACTOR};
      constexpr double beta{cKEEP_STATION_ROLL_BETA * cFORM_UP_CLOSING_BETA_FACTOR};
      rollAngle += -alpha * deltaLoc[1] - beta * deltaVel[1];
   }
   else
   {
      double bearingToTarget = mData.mChaserPlatformPtr->RelativeBearing(mData.mKinematics.mLocWCS.GetData());
      double alpha           = std::min(cFORM_UP_ROLL_ALPHA_SCALING * sep, cFORM_UP_ROLL_MAX_ALPHA);
      rollAngle += alpha * bearingToTarget;
   }

   // Limit the roll angle according to the allowed g-load limit; allow for level turns.
   double maxRollAngle = acos(1.0 / mManeuver.GetG_LoadMax());
   rollAngle           = UtMath::Limit(rollAngle, maxRollAngle);

   return rollAngle * UtMath::cDEG_PER_RAD;
}

double wsf::six_dof::FormUpState::ComputeSpeedKTAS() const
{
   UtVec3d velChaser;
   mData.mChaserPlatformPtr->GetVelocityWCS(velChaser.GetData());
   double dotProd =
      -UtVec3d::DotProduct(mData.mSeparationWCS, velChaser) / (mData.mSeparationWCS.Magnitude() * velChaser.Magnitude());

   double targetVel = mData.mKinematics.mVelWCS.Magnitude();
   double minSpeed  = mManeuver.GetSpeedMpsMin();
   double maxSpeed  = mManeuver.GetSpeedMpsMax();

   double retval;
   if (dotProd < -cFORM_UP_SPEED_COSTHETA)
   {
      retval = minSpeed;
   }
   else if (dotProd < 0.0)
   {
      retval = minSpeed + (targetVel - minSpeed) * (dotProd + cFORM_UP_SPEED_COSTHETA) / cFORM_UP_SPEED_COSTHETA;
   }
   else if (dotProd < cFORM_UP_SPEED_COSTHETA)
   {
      retval = targetVel + (maxSpeed - targetVel) * dotProd / cFORM_UP_SPEED_COSTHETA;
   }
   else
   {
      retval = maxSpeed;
   }
   return retval * UtMath::cNMPH_PER_MPS;
}

std::unique_ptr<wsf::six_dof::RelativeManeuverState> wsf::six_dof::KeepStationState::Evaluate()
{
   std::unique_ptr<RelativeManeuverState> retvalPtr{nullptr};

   bool leaving{false};
   if (!mManeuver.IsSpeedInsideAllowedRange(mData.mKinematics.mVelWCS.Magnitude()))
   {
      // If we are outside the allowed speed range, we transition, but only if we have
      // broken the limit on three consecutive evaluations.
      ++mExitCount;
      if (mExitCount >= cKEEP_STATION_HITS_TO_PURSUE)
      {
         retvalPtr = ut::make_unique<FormationPursueState>(mData, mManeuver);
         leaving   = true;
      }
   }
   else
   {
      // Otherwise, transition out of station keeping if the chaser is too far
      // from its station.
      mExitCount = 0;
      if (mData.mSeparationWCS.Magnitude() > cKEEP_STATION_TO_FORM_UP_DISTANCE)
      {
         retvalPtr = ut::make_unique<FormUpState>(mData, mManeuver);
         leaving   = true;
      }
   }

   if (!leaving)
   {
      UtVec3d deltaLoc;
      UtVec3d deltaVel;
      UtVec3d deltaAcc;
      ComputeCoordinates(deltaLoc, deltaVel, deltaAcc);

      double rollAngle_deg = ComputeRollAngleDeg(deltaLoc, deltaVel, deltaAcc);
      double gLoad         = ComputeG_Load(deltaLoc, deltaVel);
      double speed_ktas    = ComputeSpeedKTAS(deltaLoc, deltaVel, deltaAcc);

      mData.mChaserMoverPtr->SetAutopilotRollAngle(rollAngle_deg);
      mData.mChaserMoverPtr->SetPitchGLoad(gLoad);
      mData.mChaserMoverPtr->SetAutopilotSpeedKTAS(speed_ktas);
   }

   return retvalPtr;
}

double wsf::six_dof::KeepStationState::GetEvaluationInterval() const
{
   return cKEEP_STATION_DELTA_T;
}

void wsf::six_dof::KeepStationState::ComputeCoordinates(UtVec3d& aDeltaLoc, UtVec3d& aDeltaVel, UtVec3d& aDeltaAcc) const
{
   // The controls produced in the KeepStationState derive from difference in the ECS frame an entity
   // would have if it existed with the kinematics saves in mData. In general, each control, roll, g-load
   // and speed, all use offsets in position, velocity and acceleration from the target point to determining
   // the computed control. In addition to the position offsets, which tend to drive the chaser to the
   // target location, the velocity offset acts to reduce overshoot, and the acceleration offset acts to
   // dampen the effects of accelerating target locations.

   UtEntity utilEntity = mData.mKinematics.GetEntity();

   // delta position in ECS frame
   utilEntity.ConvertWCSToECS(mData.mChaserLocWCS.GetData(), aDeltaLoc.GetData());

   // delta velocity in ECS frame
   UtVec3d chaserVelWCS;
   mData.mChaserPlatformPtr->GetVelocityWCS(chaserVelWCS.GetData());
   UtVec3d deltaVelWCS = chaserVelWCS - mData.mKinematics.mVelWCS;
   utilEntity.ConvertWCSVectorToECS(aDeltaVel.GetData(), deltaVelWCS.GetData());

   // delta acceleration in the ECS frame
   UtVec3d chaserAccWCS;
   mData.mChaserPlatformPtr->GetAccelerationWCS(chaserAccWCS.GetData());
   UtVec3d deltaAccWCS = chaserAccWCS - mData.mKinematics.mAccWCS;
   utilEntity.ConvertWCSVectorToECS(aDeltaAcc.GetData(), deltaAccWCS.GetData());
}

double wsf::six_dof::KeepStationState::ComputeRollAngleDeg(const UtVec3d& aDeltaLoc,
                                                           const UtVec3d& aDeltaVel,
                                                           const UtVec3d& aDeltaAcc) const
{
   // Using the roll of the target as a baseline, compute offsets based on position, velocity
   // and acceleration differences.
   double retval = mData.mKinematics.mAnglesNED[2] * UtMath::cDEG_PER_RAD - cKEEP_STATION_ROLL_ALPHA * aDeltaLoc[1] -
                   cKEEP_STATION_ROLL_BETA * aDeltaVel[1] - cKEEP_STATION_ROLL_GAMMA * aDeltaAcc[1];
   return retval;
}

double wsf::six_dof::KeepStationState::ComputeG_Load(const UtVec3d& aDeltaLoc, const UtVec3d& aDeltaVel) const
{
   // Using the g-load of the target as a baseline, compute offsets based on position and velocity
   // differences.
   double retval =
      mData.mKinematics.mG_Load + cKEEP_STATION_GLOAD_ALPHA * aDeltaLoc[2] + cKEEP_STATION_GLOAD_BETA * aDeltaVel[2];
   return mManeuver.LimitG_Load(retval);
}

double wsf::six_dof::KeepStationState::ComputeSpeedKTAS(const UtVec3d& aDeltaLoc,
                                                        const UtVec3d& aDeltaVel,
                                                        const UtVec3d& aDeltaAcc) const
{
   // Using the speed of the target as a baseline, compute offsets based on the position, velocity
   // and acceleration differences.
   double retval = mData.mKinematics.mVelWCS.Magnitude() - cKEEP_STATION_SPEED_ALPHA * aDeltaLoc[0] -
                   cKEEP_STATION_SPEED_BETA * aDeltaVel[0] - cKEEP_STATION_SPEED_GAMMA * aDeltaAcc[0];
   return mManeuver.LimitSpeed(retval) * UtMath::cNMPH_PER_MPS;
}

std::unique_ptr<wsf::six_dof::RelativeManeuverState> wsf::six_dof::FormationPursueState::Evaluate()
{
   std::unique_ptr<RelativeManeuverState> retvalPtr{nullptr};

   if (mManeuver.IsSpeedInsideAllowedRange(mData.mKinematics.mVelWCS.Magnitude()))
   {
      // If in pursuit, leave pursuit once the target kinematics are inside the allowed speed,
      // but only after a few iterations.
      ++mExitCount;
   }
   else
   {
      mExitCount = 0;
   }

   // check if we should drop back out of Pursue
   if (mExitCount >= cPURSUE_HITS_TO_KEEP_STATION)
   {
      retvalPtr = ut::make_unique<KeepStationState>(mData, mManeuver);
   }
   else
   {
      ComputeOffsets();

      double rollAngle_deg = ComputeRollAngle();
      double gLoad         = ComputeG_Load();
      double speedKTAS     = ComputeSpeedKTAS();

      mData.mChaserMoverPtr->SetAutopilotRollAngle(rollAngle_deg);
      mData.mChaserMoverPtr->SetAutopilotSpeedKTAS(speedKTAS);
      mData.mChaserMoverPtr->SetPitchGLoad(gLoad);
   }

   return retvalPtr;
}

double wsf::six_dof::FormationPursueState::GetEvaluationInterval() const
{
   return cPURSUE_DELTA_T;
}

double wsf::six_dof::FormationPursueState::ComputeRollAngle() const
{
   // Using the roll of the target as a baseline, compute offsets based on position and velocity
   // differences.
   return mData.mKinematics.mAnglesNED[2] * UtMath::cDEG_PER_RAD + cPURSUE_ROLL_ALPHA * mDeltaLoc[1] +
          cPURSUE_ROLL_BETA * mDeltaVel[1];
}

double wsf::six_dof::FormationPursueState::ComputeG_Load() const
{
   // Using the g-load of the target as a baseline, compute offsets based on position and velocity
   // differences.
   double retval = mData.mKinematics.mG_Load - cPURSUE_GLOAD_ALPHA * mDeltaLoc[2] - cPURSUE_GLOAD_BETA * mDeltaVel[2];
   return mManeuver.LimitG_Load(retval);
}

double wsf::six_dof::FormationPursueState::ComputeSpeedKTAS() const
{
   // The speed during pursue is only allowed to vary from the target's speed by a small amount.
   double retval = mTargetSpeed + cPURSUE_SPEED_RANGE * atan2(mDeltaLoc[0], cPURSUE_SPEED_FACTOR);
   return mManeuver.LimitSpeed(retval) * UtMath::cNMPH_PER_MPS;
}

void wsf::six_dof::FormationPursueState::ComputeOffsets()
{
   // The offsets used to compute the controls for this state are in the ECS frame of
   // the chaser.

   Formation* formationPtr = mManeuver.GetFormation();
   auto       kinematics   = formationPtr->GetKeepStationRoot()->GetFormationLeader()->GetMemberKinematicState();
   auto       turnCircle   = kinematics.GetTurnCircle();

   UtVec3d diff      = kinematics.mLocWCS - mData.mChaserLocWCS;
   mTrailingDistance = diff.Magnitude();

   mTargetSpeed = turnCircle.GetSpeedMetersPerSecond();
   double phase = -mTrailingDistance / turnCircle.GetRadiusMeters();

   UtVec3d targetPoint = turnCircle.GetLocationOnCircle(phase);
   mData.mChaserPlatformPtr->ConvertWCSToECS(targetPoint.GetData(), mDeltaLoc.GetData());

   UtVec3d targetVelocity = turnCircle.GetVelocityOnCircle(phase);
   UtVec3d chaserVelocity;
   mData.mChaserPlatformPtr->GetVelocityWCS(chaserVelocity.GetData());
   UtVec3d deltaVelWCS = targetVelocity - chaserVelocity;
   mData.mChaserPlatformPtr->ConvertWCSVectorToECS(mDeltaVel.GetData(), deltaVelWCS.GetData());
}
