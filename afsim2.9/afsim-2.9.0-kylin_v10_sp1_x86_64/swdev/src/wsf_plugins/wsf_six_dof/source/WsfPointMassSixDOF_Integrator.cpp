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

#include "WsfPointMassSixDOF_Integrator.hpp"

#include <stdio.h>

#include "UtMath.hpp"
#include "UtQuaternion.hpp"
#include "WsfPointMassSixDOF_AeroCoreObject.hpp"
#include "WsfPointMassSixDOF_Mover.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_ForceAndRotationObject.hpp"
#include "WsfSixDOF_KinematicState.hpp"
#include "WsfSixDOF_MassProperties.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::PointMassIntegrator::PointMassIntegrator(PointMassMover& aVehicle)
   : mVehicle(&aVehicle)
{
}

wsf::six_dof::PointMassIntegrator* wsf::six_dof::PointMassIntegrator::Clone() const
{
   return new PointMassIntegrator(*this);
}

wsf::six_dof::Mover* wsf::six_dof::PointMassIntegrator::GetParentVehicle() const
{
   return mVehicle;
}

void wsf::six_dof::PointMassIntegrator::SetParentVehicle(PointMassMover* aVehicle)
{
   mVehicle = aVehicle;
}

void wsf::six_dof::PointMassIntegrator::Update(int64_t aSimTime_nanosec, double aDeltaT_sec) const

{
   if (mVehicle == nullptr)
   {
      return;
   }

   // Start by calculating the current center of mass (CM)
   mVehicle->CalculateCurrentMassProperties();

   // -------------------------------------------------------------------

   // Store a const copy of the current kinematic state
   const KinematicState initialState = *mVehicle->GetKinematicState();

   // Copy state over to a temp state, modified during integration
   KinematicState tempState = initialState;

   // -------------------------------------------------------------------

   // Using Heun's modified Euler method

   // The delta-t passed to CalculateAcceleration is mostly arbitrary; we need a large enough value to avoid
   // producing large derivatives, but nothing so large that we miss detail. Using the mover's dt is reasonable.

   // Calculate the accelerations at the start. Here, we use the "last update" for the evaluation time.
   UtVec3dX tempTranslationalAcceleration_mps2_T0;
   UtVec3dX tempRotationalAcceleration_rps2_T0;
   UtVec3dX tempGravitationalAcceleration_g_T0;
   CalculateAcceleration(tempState,
                         utils::TimeToTime(mVehicle->GetLastUpdateTime_sec()),
                         aDeltaT_sec,
                         tempGravitationalAcceleration_g_T0,
                         tempTranslationalAcceleration_mps2_T0,
                         tempRotationalAcceleration_rps2_T0);

   // Propagate to initial-guess temp state using the calculated accelerations
   PropagateUsingAcceleration(tempState,
                              aDeltaT_sec,
                              tempGravitationalAcceleration_g_T0,
                              tempTranslationalAcceleration_mps2_T0,
                              tempRotationalAcceleration_rps2_T0);

   // Calculate the accelerations at the other endpoint. Evaluate at the full timestep.
   UtVec3dX tempTranslationalAcceleration_mps2_T1;
   UtVec3dX tempRotationalAcceleration_rps2_T1;
   UtVec3dX tempGravitationalAcceleration_g_T1;
   CalculateAcceleration(tempState,
                         aSimTime_nanosec,
                         aDeltaT_sec,
                         tempGravitationalAcceleration_g_T1,
                         tempTranslationalAcceleration_mps2_T1,
                         tempRotationalAcceleration_rps2_T1);

   // Propagate to new temp state using the calculated F&M
   PropagateUsingAcceleration(tempState,
                              aDeltaT_sec,
                              tempGravitationalAcceleration_g_T1,
                              tempTranslationalAcceleration_mps2_T1,
                              tempRotationalAcceleration_rps2_T1);

   // Average the F&M
   UtVec3dX averageTranslationalAcceleration_mps2 =
      (tempTranslationalAcceleration_mps2_T0 + tempTranslationalAcceleration_mps2_T1) * 0.5;
   UtVec3dX averageRotationalAcceleration_rps2 =
      (tempRotationalAcceleration_rps2_T0 + tempRotationalAcceleration_rps2_T1) * 0.5;
   UtVec3dX averageGravitationalAcceleration_g =
      (tempGravitationalAcceleration_g_T0 + tempGravitationalAcceleration_g_T1) * 0.5;

   // -------------------------------------------------------------------

   // Get a reference to the kinematic state
   KinematicState& kinematicState = *mVehicle->GetKinematicState();

   // Set the following values from tempState from the last CalculateFM into kinematicState
   double lift_lbs      = tempState.GetLift();
   double drag_lbs      = tempState.GetDrag();
   double sideForce_lbs = tempState.GetSideForce();
   double thrust_lbs    = tempState.GetThrust();
   double wgt_lbs       = tempState.GetWeight();
   kinematicState.SetLiftDragSideForceThrustWeight(lift_lbs, drag_lbs, sideForce_lbs, thrust_lbs, wgt_lbs);

   // We now "Update" rather than "Propagate" to the final state,
   // using the average F&M along with the original state. Here we
   // use the current sim time (end of frame) and now we use the
   // full frame delta-T, rather than zero/epsilon.
   UpdateUsingAcceleration(kinematicState,
                           aSimTime_nanosec,
                           aDeltaT_sec,
                           averageGravitationalAcceleration_g,
                           averageTranslationalAcceleration_mps2,
                           averageRotationalAcceleration_rps2);

   if (mVehicle->GetFreezeFlags()->testingNoAlpha)
   {
      // Remove any alpha
      kinematicState.RemoveAlphaForTesting();
   }

   // Calc rates (especially alpha-dot and beta-dot)
   kinematicState.UpdateAeroState(aSimTime_nanosec);

   // Calculate parameters that are useful on the "outside"
   kinematicState.CalculateSecondaryParameters();
}

void wsf::six_dof::PointMassIntegrator::CalculateAcceleration(KinematicState& aState,
                                                              int64_t         aSimTime_nanosec,
                                                              double          aDeltaT_sec,
                                                              UtVec3dX&       aGravitationalAccel_g,
                                                              UtVec3dX&       aTranslationalAccel_mps2,
                                                              UtVec3dX&       aRotationalAccel_mps2) const
{
   if (mVehicle == nullptr)
   {
      return;
   }

   // Get the mass properties
   MassProperties massProperties = mVehicle->GetMassProperties();
   if (massProperties.GetMass_lbs() <= 0.0)
   {
      return;
   }

   // Calculate various F&M contributors, including aero, propulsion, and
   // landing gear. These F&M are all calculated relative to the reference
   // point (RP) of the object, not the CM. The gravity force will also be
   // calculated, but it is calculated relative to the center of mass (CM).

   // Begin by setting up F&M objects for F&M acting at the RP and CM
   ForceAndRotationObject totalForceBody;

   // -------------------------------------------------------------------

   // Calc aero F&M

   // Update the aero state parameters (alpha, beta, mach, rho, etc)
   aState.UpdateAeroState(aSimTime_nanosec);

   UtVec3dX aeroLiftBodyVector_lbs;
   UtVec3dX aeroDragBodyVector_lbs;
   UtVec3dX aeroSideBodyVector_lbs;
   UtVec3dX rotationalAccelerationLimits_rps2;
   UtVec3dX stabilizingFrequency_rps;

   // Calculate the aero forces and moments
   mVehicle->CalculateAeroBodyForceAndRotation(aeroLiftBodyVector_lbs,
                                               aeroDragBodyVector_lbs,
                                               aeroSideBodyVector_lbs,
                                               rotationalAccelerationLimits_rps2,
                                               stabilizingFrequency_rps);

   // Sum lift, drag, and side forces into "Total" force
   UtVec3dX aeroTotalBodyForce_lbs = aeroLiftBodyVector_lbs + aeroDragBodyVector_lbs + aeroSideBodyVector_lbs;

   totalForceBody.AddForce(aeroTotalBodyForce_lbs);
   totalForceBody.AddMaximumAngularAcceleration_rps2(rotationalAccelerationLimits_rps2);
   totalForceBody.AddStabilizingFrequency_rps(stabilizingFrequency_rps);

   // -------------------------------------------------------------------

   // Calc propulsion F&M

   UtVec3dX inertialPropulsionForce_lbs;
   UtVec3dX propulsionRotationAccel_rps2;

   // Calculate the propulsion forces and moments for this object and subobjects
   mVehicle->CalculatePropulsionFM(aSimTime_nanosec,
                                   aDeltaT_sec,
                                   aState,
                                   inertialPropulsionForce_lbs,
                                   propulsionRotationAccel_rps2);

   // Convert propulsion inertial F&M to body coords
   UtVec3dX propulsionTotalBodyForce_lbs = aState.CalcBodyVecFromWCSVec(inertialPropulsionForce_lbs);

   // Add to the reference point F&M
   totalForceBody.AddForce(propulsionTotalBodyForce_lbs);
   totalForceBody.AddMaximumAngularAcceleration_rps2(propulsionRotationAccel_rps2);

   // -------------------------------------------------------------------

   // Set lift, drag, thrust, and weight
   double lift_lbs      = aeroLiftBodyVector_lbs.Magnitude();
   double drag_lbs      = aeroDragBodyVector_lbs.Magnitude();
   double sideforce_lbs = aeroSideBodyVector_lbs.Magnitude();
   double thrust_lbs    = inertialPropulsionForce_lbs.Magnitude();
   double wgt_lbs       = massProperties.GetMass_lbs();
   aState.SetLiftDragSideForceThrustWeight(lift_lbs, drag_lbs, sideforce_lbs, thrust_lbs, wgt_lbs);

   // -------------------------------------------------------------------

   // Calc gravity F&M

   // First, calc the gravity direction vector
   UtVec3dX gravityVec = aState.NormalizedGravitationalAccelVec();

   // Next, calc the gravity force vector
   double   currentMass_lbm          = massProperties.GetMass_lbs();
   UtVec3dX gravityInertialForce_lbs = gravityVec * currentMass_lbm;

   // Convert gravity inertial F&M to body coords
   UtVec3dX gravityBodyForce_lbs = aState.CalcBodyVecFromWCSVec(gravityInertialForce_lbs);

   // Now, add the gravity force
   totalForceBody.AddForce(gravityBodyForce_lbs);

   aGravitationalAccel_g = gravityBodyForce_lbs / massProperties.GetMass_lbs();
   aTranslationalAccel_mps2 =
      totalForceBody.GetForce_lbs() / massProperties.GetMass_lbs() * utils::cREFERENCE_GRAV_ACCEL_MPS2;

   // -------------------------------------------------------------------

   // Rotational acceleration is provided by a combination of the controller (first term) and the plant (second term)
   // Trim is assumed to be 0 degrees in alpha and beta but could be changed per vehicle type in future

   double massFraction      = massProperties.GetMass_lbs() / massProperties.GetBaseMass_lbs();
   double moverTimestep_sec = GetParentVehicle()->GetStepSize_sec();

   // Control effects
   UtVec3dX rotationalAccelControls_rps2;

   PointMassFlightControlSystem* flightControls = mVehicle->GetFlightControls();
   if (flightControls)
   {
      UtVec3dX commandedRotationRates_dps = flightControls->GetBodyRateCommands_dps();

      UtVec3dX commandedRotationRates_rps = commandedRotationRates_dps * UtMath::cRAD_PER_DEG;
      UtVec3dX currentRotationRates_rps   = aState.GetOmegaBody();

      // This control system is intended to be reasonably foolproof, and so should avoid overshooting the goal.
      // An overshoot in this context results in a "buzzing" effect in the controller, which muddles telemetry traces.

      rotationalAccelControls_rps2 = (commandedRotationRates_rps - currentRotationRates_rps) / moverTimestep_sec;

      UtVec3dX rotationalAccelLimitBase_rps2 = totalForceBody.GetMaximumAngularAcceleration_rps2();
      UtVec3dX rotationalAccelLimit_rps2     = rotationalAccelLimitBase_rps2 / massFraction;

      rotationalAccelControls_rps2.Set(0,
                                       UtMath::Limit(rotationalAccelControls_rps2.X(),
                                                     std::abs(rotationalAccelLimit_rps2.X())));
      rotationalAccelControls_rps2.Set(1,
                                       UtMath::Limit(rotationalAccelControls_rps2.Y(),
                                                     std::abs(rotationalAccelLimit_rps2.Y())));
      rotationalAccelControls_rps2.Set(2,
                                       UtMath::Limit(rotationalAccelControls_rps2.Z(),
                                                     std::abs(rotationalAccelLimit_rps2.Z())));
   }

   // Plant effects to approximate stability/instability
   UtVec3dX rotationalAccelStability_rps2;

   double alpha_rad    = aState.GetAlpha_rad();
   double beta_rad     = aState.GetBeta_rad();
   double rollRate_rps = aState.GetRollRate_rps();
   double alphaDot_rps = aState.GetAlphaDot_rps();
   double betaDot_rps  = aState.GetBetaDot_rps();

   double rollStabilizingFrequency  = 0.0;
   double alphaStabilizingFrequency = 0.0;
   double betaStabilizingFrequency  = 0.0;
   totalForceBody.GetStabilizingFrequency_rps().Get(rollStabilizingFrequency,
                                                    alphaStabilizingFrequency,
                                                    betaStabilizingFrequency);

   rollStabilizingFrequency /= massFraction;
   alphaStabilizingFrequency /= massFraction;
   betaStabilizingFrequency /= massFraction;

   // This is a second order "control" with damping of 1, to allow us to directly affect the second-order system
   double pitchAccelerationStability = (0.0 - alpha_rad) * alphaStabilizingFrequency * alphaStabilizingFrequency -
                                       2.0 * alphaStabilizingFrequency * alphaDot_rps;
   double yawAccelerationStability = (0.0 - beta_rad) * betaStabilizingFrequency * betaStabilizingFrequency -
                                     2.0 * betaStabilizingFrequency * betaDot_rps;

   // This is a first-order "control," using the natural frequency as the inverse of a control lag
   double rollAlphaFactor =
      rollStabilizingFrequency * moverTimestep_sec / (1.0 + rollStabilizingFrequency * moverTimestep_sec);
   double expectedRollRate_rps      = (1.0 - rollAlphaFactor) * rollRate_rps;
   double deltaRollRate_rps         = expectedRollRate_rps - rollRate_rps;
   double rollAccelerationStability = deltaRollRate_rps / moverTimestep_sec;

   // Limit the stabilizing acceleration to avoid numerical divergence
   double maxRollAccelerationStability = std::abs(-rollRate_rps / moverTimestep_sec);
   double maxPitchAccelerationStability =
      std::abs(2.0 / (moverTimestep_sec * moverTimestep_sec) * (-alpha_rad - alphaDot_rps * moverTimestep_sec));
   double maxYawAccelerationStability =
      std::abs(2.0 / (moverTimestep_sec * moverTimestep_sec) * (-beta_rad - betaDot_rps * moverTimestep_sec));
   rollAccelerationStability  = UtMath::Limit(rollAccelerationStability, maxRollAccelerationStability);
   pitchAccelerationStability = UtMath::Limit(pitchAccelerationStability, maxPitchAccelerationStability);
   yawAccelerationStability   = UtMath::Limit(yawAccelerationStability, maxYawAccelerationStability);

   rotationalAccelStability_rps2.Set(rollAccelerationStability, pitchAccelerationStability, -yawAccelerationStability);

   // Sum effects together
   aRotationalAccel_mps2 = rotationalAccelControls_rps2 + rotationalAccelStability_rps2;
}

void wsf::six_dof::PointMassIntegrator::PropagateUsingAcceleration(KinematicState& aState,
                                                                   double          aDeltaT_sec,
                                                                   const UtVec3dX& aGravitationalAccel_g,
                                                                   const UtVec3dX& aTranslationalAccel_mps2,
                                                                   const UtVec3dX& aRotationalAccel_rps2) const
{
   if (mVehicle == nullptr)
   {
      return;
   }

   // ----------------------------------------------------------------------

   // Limit F&M (optional)

   // Note: These two tests (limitMaxG and limitRotAccel) are currently being
   // enforced at all times, but we may want the choice of whether or not to
   // use these tests be be selectable through data files.

   // We want to limit the max g (this should be a settable parameter)
   // to prevent acceleration spikes that can occur.
   const double cMaxG         = 1000.0;
   double       maxAccel_mps2 = utils::cREFERENCE_GRAV_ACCEL_MPS2 * cMaxG;

   UtVec3dX translationalAccelerationBody_mps2 = aTranslationalAccel_mps2;

   double accelerationMagnitude_mps = translationalAccelerationBody_mps2.Magnitude();
   if (accelerationMagnitude_mps > maxAccel_mps2)
   {
      translationalAccelerationBody_mps2 *= maxAccel_mps2 / accelerationMagnitude_mps;
   }

   // Calculate/set body-rel g-loads, minus gravitational acceleration (Nx/Ny/Nz)
   UtVec3dX translationalAccelerationBody_g = translationalAccelerationBody_mps2 / utils::cREFERENCE_GRAV_ACCEL_MPS2;

   aState.SetBodyAccel(translationalAccelerationBody_g.X() - aGravitationalAccel_g.X(),
                       translationalAccelerationBody_g.Y() - aGravitationalAccel_g.Y(),
                       translationalAccelerationBody_g.Z() - aGravitationalAccel_g.Z());

   // ----------------------------------------------------------------------

   // Next, propagate in translation, using the body-frame acceleration
   PropagateTranslation(aState, translationalAccelerationBody_mps2, aDeltaT_sec);

   // ----------------------------------------------------------------------

   // Now, propagate in rotation
   PropagateRotation(aState, aRotationalAccel_rps2, aDeltaT_sec);
}

void wsf::six_dof::PointMassIntegrator::UpdateUsingAcceleration(KinematicState& aState,
                                                                int64_t         aSimTime_nanosec,
                                                                double          aDeltaT_sec,
                                                                const UtVec3dX& aGravitationalAccel_g,
                                                                const UtVec3dX& aTranslationalAccel_mps2,
                                                                const UtVec3dX& aRotationalAccel_rps2) const
{
   if (mVehicle == nullptr)
   {
      return;
   }

   UpdateFuelBurn(aState, aSimTime_nanosec, aDeltaT_sec);

   PropagateUsingAcceleration(aState, aDeltaT_sec, aGravitationalAccel_g, aTranslationalAccel_mps2, aRotationalAccel_rps2);
}
