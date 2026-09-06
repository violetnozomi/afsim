// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_Utils.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>

#include "UtAtmosphere.hpp"
#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

// -------------------------------------------------------------------------------

// This outputs a double into a string.
// aWidth is the total width of the string and aPrecision is the number of
// digits to the right of the decimal.
std::string wsf::six_dof::utils::OutputDoubleString(double aValue, int aWidth, int aPrecision)
{
   std::stringstream ss;

   ss.setf(std::ios::fixed);
   ss.width(aWidth);
   ss.precision(aPrecision);
   ss << aValue;

   return ss.str();
}

int64_t wsf::six_dof::utils::TimeToTime(double aSimTime_sec)
{
   double  localTime  = aSimTime_sec * 1000000000.0;
   int64_t local_nano = static_cast<int64_t>(localTime);

   return local_nano;
}

double wsf::six_dof::utils::TimeToTime(int64_t aSimTime_nanosec)
{
   return (0.000000001 * static_cast<double>(aSimTime_nanosec));
}

double wsf::six_dof::utils::NextGuessFromFalsePosition(double aGuessA, double aOutputA, double aGuessB, double aOutputB)
{
   return (aGuessA * aOutputB - aGuessB * aOutputA) / (aOutputB - aOutputA);
}

void wsf::six_dof::utils::UpdateFalsePositionBounds(double& aInputA,
                                                    double& aOutputA,
                                                    double& aInputB,
                                                    double& aOutputB,
                                                    double  aInputC,
                                                    double  aOutputC)
{
   if (UtMath::Sign(aOutputA) == UtMath::Sign(aOutputC))
   {
      aInputA  = aInputC;
      aOutputA = aOutputC;
   }
   else
   {
      aInputB  = aInputC;
      aOutputB = aOutputC;
   }
}

void wsf::six_dof::utils::CalcUnitVectors(const UtDCM& aDCM, UtVec3dX& aUnitVecX, UtVec3dX& aUnitVecY, UtVec3dX& aUnitVecZ)
{
   aUnitVecX.Set(1.0, 0.0, 0.0);
   aUnitVecY.Set(0.0, 1.0, 0.0);
   aUnitVecZ.Set(0.0, 0.0, 1.0);

   aUnitVecX = aDCM.InverseTransform(aUnitVecX);
   aUnitVecY = aDCM.InverseTransform(aUnitVecY);
   aUnitVecZ = aDCM.InverseTransform(aUnitVecZ);

   aUnitVecX.Normalize();
   aUnitVecY.Normalize();
   aUnitVecZ.Normalize();
}

void wsf::six_dof::utils::CalcLocalNEDVectors(const UtVec3dX& aPos,
                                              UtVec3dX&       aLocalNorth,
                                              UtVec3dX&       aLocalEast,
                                              UtVec3dX&       aLocalDown)
{
   aLocalDown.Set(aPos);
   aLocalDown = aLocalDown * -1.0;
   aLocalDown.Normalize();

   UtVec3dX tempVec(0.0, 0.0, 10.0); // z is north
   aLocalEast = aLocalDown.Cross(tempVec);
   aLocalEast.Normalize();

   aLocalNorth = aLocalEast.Cross(aLocalDown);
   aLocalNorth.Normalize();
}

void wsf::six_dof::utils::CalcLocalAngles(const UtVec3dX& aLocalNorth,
                                          const UtVec3dX& aLocalEast,
                                          const UtVec3dX& aLocalDown,
                                          const UtVec3dX& aUnitVecX,
                                          const UtVec3dX& aUnitVecZ,
                                          double&         aLocalYaw_rad,
                                          double&         aLocalPitch_rad,
                                          double&         aLocalRoll_rad)
{
   // Determine if near vertical
   UtVec3dX localUp = -1.0 * aLocalDown;

   double dotProduct = aUnitVecX.Dot(localUp);
   if (dotProduct > 1.0)
   {
      dotProduct = 1.0;
   }
   if (dotProduct < -1.0)
   {
      dotProduct = -1.0;
   }
   double angleFromUp_rad = acos(dotProduct);

   if (fabs(angleFromUp_rad) < 1.75E-6) // 0.01 deg (0.0001/57.3 = 1.75e-6)
   {
      // A vertical or near-vertical situation
      aLocalPitch_rad = UtMath::cPI_OVER_2;
      aLocalRoll_rad  = 0.0;

      double northComp = aLocalNorth.Dot(aUnitVecZ);
      double eastComp  = aLocalEast.Dot(aUnitVecZ);

      aLocalYaw_rad = atan2(eastComp, northComp);

      return;
   }

   UtVec3dX tempRight = aLocalDown.Cross(aUnitVecX);
   tempRight.Normalize();
   UtVec3dX tempForward = tempRight.Cross(aLocalDown);
   tempForward.Normalize();

   double dot = tempForward.Dot(aUnitVecX);
   if (dot > 1.0)
   {
      dot = 1.0;
   }
   if (dot < -1.0)
   {
      dot = -1.0;
   }
   aLocalPitch_rad     = acos(dot);
   double downFraction = aLocalDown.Dot(aUnitVecX);
   if (downFraction > 0.0)
   {
      aLocalPitch_rad *= -1.0;
   }

   double northFraction = aLocalNorth.Dot(aUnitVecX);
   double eastFraction  = aLocalEast.Dot(aUnitVecX);
   aLocalYaw_rad        = atan2(eastFraction, northFraction);

   UtVec3dX tempDown = aUnitVecX.Cross(tempRight);
   double   tan_x    = tempRight.Dot(aUnitVecZ);
   double   tan_y    = tempDown.Dot(aUnitVecZ);
   aLocalRoll_rad    = atan2(tan_y, tan_x) - UtMath::cPI_OVER_2;
}

void wsf::six_dof::utils::InertialToBodyRel(const UtDCM& aMatrix, const UtVec3dX& aInertial, UtVec3dX& aBodyRel)
{
   UtVec3dX vec;
   UtVec3dX result;
   UtVec3dX body;

   vec.Set(1.0, 0.0, 0.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyX = result.DotProduct(aInertial);

   vec.Set(0.0, 1.0, 0.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyY = result.DotProduct(aInertial);

   vec.Set(0.0, 0.0, 1.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyZ = result.DotProduct(aInertial);

   aBodyRel.Set(bodyX, bodyY, bodyZ);
}

void wsf::six_dof::utils::DCM_InertialToBodyRel(const UtDCM& aMatrix, const UtVec3dX& aInertial, UtVec3dX& aBodyRel)
{
   UtVec3dX vec;
   UtVec3dX result;
   UtVec3dX body;

   vec.Set(1.0, 0.0, 0.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyX = result.DotProduct(aInertial);

   vec.Set(0.0, 1.0, 0.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyY = result.DotProduct(aInertial);

   vec.Set(0.0, 0.0, 1.0);
   result       = aMatrix.InverseTransform(vec);
   double bodyZ = result.DotProduct(aInertial);

   aBodyRel.Set(bodyX, bodyY, bodyZ);
}

void wsf::six_dof::utils::AlphaBetaFromInertialVel(const UtDCM&    aMatrix,
                                                   const UtVec3dX& aInertialVel_mps,
                                                   double&         aSpeed_fps,
                                                   double&         aAlpha_deg,
                                                   double&         aBeta_deg)
{
   double vx, vy, vz;
   aInertialVel_mps.Get(vx, vy, vz);
   UtVec3dX inertial_mps(vx, vy, vz);
   UtVec3dX bodyRel_mps;
   DCM_InertialToBodyRel(aMatrix, inertial_mps, bodyRel_mps);

   UtVec3dX bodyRel_fps = bodyRel_mps * UtMath::cFT_PER_M;
   AlphaBetaFromBodyRelVel(bodyRel_fps, aAlpha_deg, aBeta_deg, aSpeed_fps);

   // Limit alpha/beta when velocity is small, to reduce "noise" when
   // stationary or nearly stationary
   if (aSpeed_fps < 0.1)
   {
      aAlpha_deg = 0.0;
      aBeta_deg  = 0.0;
   }
}

void wsf::six_dof::utils::AlphaBetaFromBodyRelVel(const UtVec3dX& aVel_fps,
                                                  double&         aAlpha_deg,
                                                  double&         aBeta_deg,
                                                  double&         aSpeed_fps)
{
   AlphaBetaFromBodyRelVel(aVel_fps, aAlpha_deg, aBeta_deg);
   aSpeed_fps = aVel_fps.Magnitude();
}

void wsf::six_dof::utils::AlphaBetaFromBodyRelVel(const UtVec3dX& aVel_fps, double& aAlpha_deg, double& aBeta_deg)
{
   double alpha_rad = 0.0;
   double beta_rad  = 0.0;

   double vx = 0.0;
   double vy = 0.0;
   double vz = 0.0;

   aVel_fps.Get(vx, vy, vz);

   if (aVel_fps.Magnitude() < std::numeric_limits<double>::epsilon())
   {
      aAlpha_deg = 0.0;
      aBeta_deg  = 0.0;
      return;
   }

   beta_rad = std::asin(vy / aVel_fps.Magnitude());
   // Special case: Velocity is only in Y direction. We don't have enough information
   // to determine alpha, so we set to -pi/2 or pi/2
   if (std::fabs(vx) < std::numeric_limits<double>::epsilon() && std::fabs(vz) < std::numeric_limits<double>::epsilon())
   {
      if (vy > 0)
      {
         alpha_rad = UtMath::cPI_OVER_2;
      }
      else
      {
         alpha_rad = -UtMath::cPI_OVER_2;
      }
   }
   // Special case: Velocity is only in Z direction. We don't have enough information
   // to determine alpha, so we set to -pi/2 or pi/2
   else if (std::fabs(vx) < std::numeric_limits<double>::epsilon() &&
            std::fabs(vy) < std::numeric_limits<double>::epsilon())
   {
      if (vz < 0)
      {
         alpha_rad = -UtMath::cPI_OVER_2;
      }
      else
      {
         alpha_rad = UtMath::cPI_OVER_2;
      }
   }
   else
   {
      alpha_rad = std::atan2(vz, vx);
   }

   aAlpha_deg = UtMath::cDEG_PER_RAD * alpha_rad;
   aBeta_deg  = UtMath::cDEG_PER_RAD * beta_rad;
}

void wsf::six_dof::utils::LimitThrottleNormalized(double& aThrottle)
{
   aThrottle = UtMath::Limit(aThrottle, 0.0, 1.0);
}

void wsf::six_dof::utils::LimitThrottleAfterburner(double& aThrottle)
{
   aThrottle = UtMath::Limit(aThrottle, 0.0, 2.0);
}

const wsf::six_dof::SinglePidValueData& wsf::six_dof::utils::GetSinglePidData(Pid::Type                         aPid,
                                                                              const AutopilotPidGroupValueData& aPID_Values)
{
   switch (aPid)
   {
   case Pid::Alpha:
      return aPID_Values.alphaPID;
   case Pid::VerticalSpeed:
      return aPID_Values.verticalSpeedPID;
   case Pid::PitchAngle:
      return aPID_Values.pitchAnglePID;
   case Pid::PitchRate:
      return aPID_Values.pitchRatePID;
   case Pid::FlightPathAngle:
      return aPID_Values.flightPathAnglePID;
   case Pid::DeltaPitch:
      return aPID_Values.deltaPitchPID;
   case Pid::Altitude:
      return aPID_Values.altitudePID;
   case Pid::Beta:
      return aPID_Values.betaPID;
   case Pid::YawRate:
      return aPID_Values.yawRatePID;
   case Pid::YawHeading:
      return aPID_Values.yawheadingPID;
   case Pid::TaxiHeading:
      return aPID_Values.taxiHeadingPID;
   case Pid::RollRate:
      return aPID_Values.rollRatePID;
   case Pid::DeltaRoll:
      return aPID_Values.deltaRollPID;
   case Pid::BankAngle:
      return aPID_Values.bankAnglePID;
   case Pid::RollHeading:
      return aPID_Values.rollHeadingPID;
   case Pid::ForwardAccel:
      return aPID_Values.forwardAccelPID;
   case Pid::Speed:
      return aPID_Values.speedPID;
   case Pid::TaxiSpeed:
      return aPID_Values.taxiSpeedPID;
   case Pid::TaxiYawRate:
      return aPID_Values.taxiYawRatePID;
   default:
      // Use alpha PID as default
      return aPID_Values.alphaPID;
   }
}
