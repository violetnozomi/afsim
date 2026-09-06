// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfRateLimitedAttitudeController.hpp"

#include "UtInput.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "WsfPlatform.hpp"
#include "WsfSpaceMoverBase.hpp"

namespace
{
const double cONE_DEGREE_PER_SEC = 0.0174532925199; // Converted to rad/s
};

WsfRateLimitedAttitudeController::WsfRateLimitedAttitudeController()
   : WsfAttitudeController()
   , mTargetPsiECI(0.0)
   , mTargetThetaECI(0.0)
   , mTargetPhiECI(0.0)
   , mYawRate(cONE_DEGREE_PER_SEC)
   , mPitchRate(cONE_DEGREE_PER_SEC)
   , mRollRate(cONE_DEGREE_PER_SEC)
{
}

void WsfRateLimitedAttitudeController::SetTargetOrientationECI(double aPsi, double aTheta, double aPhi)
{
   mTargetPsiECI   = aPsi;
   mTargetThetaECI = aTheta;
   mTargetPhiECI   = aPhi;
}

void WsfRateLimitedAttitudeController::GetTargetOrientationECI(double& aPsi, double& aTheta, double& aPhi) const
{
   aPsi   = mTargetPsiECI;
   aTheta = mTargetThetaECI;
   aPhi   = mTargetPhiECI;
}

void WsfRateLimitedAttitudeController::UpdateOrientation(double aDeltaT)
{
   auto   platform = GetMover()->GetPlatform();
   double psi, theta, phi;
   platform->GetOrientationECI(psi, theta, phi);

   // C is current transform from ECS->ECI
   double Cinv[3][3];
   UtEntity::ComputeRotationalTransform(psi, theta, phi, Cinv);
   double C[3][3];
   UtMat3d::Transpose(C, Cinv);

   // T is target transform from ECS->ECI
   double Tinv[3][3];
   UtEntity::ComputeRotationalTransform(mTargetPsiECI, mTargetThetaECI, mTargetPhiECI, Tinv);
   double T[3][3];
   UtMat3d::Transpose(T, Tinv);

   // R = T^{-1} C
   double R[3][3];
   UtMat3d::Multiply(R, Tinv, C);

   // Compute angles for R and limit them by the rates
   double bodyPsi, bodyTheta, bodyPhi;
   UtEntity::ExtractEulerAngles(R, bodyPsi, bodyTheta, bodyPhi);
   bodyPsi   = UpdateAngle(0.0, bodyPsi, mYawRate * aDeltaT);
   bodyTheta = UpdateAngle(0.0, bodyTheta, mPitchRate * aDeltaT);
   bodyPhi   = UpdateAngle(0.0, bodyPhi, mRollRate * aDeltaT);

   // R_c is the rate limited version of R
   double Rc[3][3];
   UtEntity::ComputeRotationalTransform(bodyPsi, bodyTheta, bodyPhi, Rc);

   // T_c = C R_c^{-1} is rate limited target transform from ECS->ECI
   double Rcinv[3][3];
   UtMat3d::Transpose(Rcinv, Rc);
   double Tc[3][3];
   UtMat3d::Multiply(Tc, C, Rcinv);

   // Update platform position with angles from T_c^{-1} (which is an ECI->ECS transform)
   double Tcinv[3][3];
   UtMat3d::Transpose(Tcinv, Tc);
   double updatePsi, updateTheta, updatePhi;
   UtEntity::ExtractEulerAngles(Tcinv, updatePsi, updateTheta, updatePhi);
   platform->SetOrientationECI(updatePsi, updateTheta, updatePhi);
}

bool WsfRateLimitedAttitudeController::AtTargetOrientation() const
{
   double psi, theta, phi;
   GetMover()->GetPlatform()->GetOrientationECI(psi, theta, phi);

   // use a tolerance of one thousandth of a degree [this is arbitrary, and better choices likely exist]
   static const double cTOLERANCE = UtMath::cRAD_PER_DEG * 1.0e-3;

   return UtMath::AngleWithinTolerance(UtMath::AngleDifference(psi, mTargetPsiECI), cTOLERANCE) &&
          UtMath::AngleWithinTolerance(UtMath::AngleDifference(theta, mTargetThetaECI), cTOLERANCE) &&
          UtMath::AngleWithinTolerance(UtMath::AngleDifference(phi, mTargetPhiECI), cTOLERANCE);
}

bool WsfRateLimitedAttitudeController::ProcessInput(UtInput& aInput)
{
   bool myCommand{true};

   std::string command{aInput.GetCommand()};
   if (command == "maximum_yaw_rate")
   {
      aInput.ReadValueOfType(mYawRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreaterOrEqual(mYawRate, 0.0);
   }
   else if (command == "maximum_pitch_rate")
   {
      aInput.ReadValueOfType(mPitchRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreaterOrEqual(mPitchRate, 0.0);
   }
   else if (command == "maximum_roll_rate")
   {
      aInput.ReadValueOfType(mRollRate, UtInput::cANGULAR_RATE);
      aInput.ValueGreaterOrEqual(mRollRate, 0.0);
   }
   else
   {
      myCommand = WsfAttitudeController::ProcessInput(aInput);
   }

   return myCommand;
}

//! This returns the updated angle given the inputs.
//! This computes a new angle that is either @p aMaxChange away from @p aCurr
//! in the direction of @p aTarget, or if @p aMaxChange would overshoot this
//! will return @p aTarget.
//! @param aCurr      - the current angle
//! @param aTarget    - the target angle
//! @param aMaxChange - the maximum change in angle
//! @returns          - the updated angle
double WsfRateLimitedAttitudeController::UpdateAngle(double aCurr, double aTarget, double aMaxChange)
{
   // aCurr = UtMath::NormalizeAngle0_TwoPi(aCurr);
   // aTarget = UtMath::NormalizeAngle0_TwoPi(aTarget);
   double tMinusC = aTarget - aCurr;
   double cMinusT = aCurr - aTarget;
   double delta{tMinusC};
   if (aCurr < aTarget)
   {
      if (tMinusC < UtMath::cPI)
      {
         delta = std::min(tMinusC, aMaxChange);
      }
      else
      {
         delta = -std::min(cMinusT + UtMath::cTWO_PI, aMaxChange);
      }
   }
   else
   {
      if (cMinusT < UtMath::cPI)
      {
         delta = -std::min(cMinusT, aMaxChange);
      }
      else
      {
         delta = std::min(tMinusC + UtMath::cTWO_PI, aMaxChange);
      }
   }

   return aCurr + delta;
}
