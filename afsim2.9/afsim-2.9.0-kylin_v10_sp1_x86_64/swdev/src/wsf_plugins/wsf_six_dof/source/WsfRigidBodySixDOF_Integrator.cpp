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

#include "WsfRigidBodySixDOF_Integrator.hpp"

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtQuaternion.hpp"
#include "WsfRigidBodySixDOF_AeroCoreObject.hpp"
#include "WsfRigidBodySixDOF_LandingGear.hpp"
#include "WsfRigidBodySixDOF_Mover.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_ForceAndMomentsObject.hpp"
#include "WsfSixDOF_KinematicState.hpp"

wsf::six_dof::RigidBodyIntegrator::RigidBodyIntegrator(RigidBodyMover& aVehicle)
   : mVehicle(&aVehicle)
{
}

wsf::six_dof::RigidBodyIntegrator* wsf::six_dof::RigidBodyIntegrator::Clone() const
{
   return new RigidBodyIntegrator(*this);
}

wsf::six_dof::Mover* wsf::six_dof::RigidBodyIntegrator::GetParentVehicle() const
{
   return mVehicle;
}

void wsf::six_dof::RigidBodyIntegrator::SetParentVehicle(RigidBodyMover* aVehicle)
{
   mVehicle = aVehicle;
}

void wsf::six_dof::RigidBodyIntegrator::Update(int64_t aSimTime_nanosec, double aDeltaT_sec) const

{
   if (mVehicle == nullptr)
   {
      return;
   }

   // Start by calculating the current center of mass (CM)
   mVehicle->CalculateCurrentMassProperties();

   // Get the mass properties
   MassProperties massProperties = mVehicle->GetMassProperties();

   // -------------------------------------------------------------------

   // Store a const copy of the current kinematic state
   const KinematicState initialState = *mVehicle->GetKinematicState();

   // Copy state over to a temp state, modified during integration
   KinematicState tempState = initialState;

   // -------------------------------------------------------------------

   // Using Heun's modified Euler method

   // The delta-t passed to CalculateFM is mostly arbitrary; we need a large enough value to avoid producing
   // large derivatives, but nothing so large that we miss detail. Using the mover's dt is reasonable.

   // Calculate the F&M at the start. Here, we use the "last update" for the evaluation time.
   ForceAndMomentsObject tempForcesMomentsAtRP_T0;
   ForceAndMomentsObject tempForcesMomentsAtCM_T0;
   CalculateFM(tempState,
               utils::TimeToTime(mVehicle->GetLastUpdateTime_sec()),
               aDeltaT_sec,
               tempForcesMomentsAtRP_T0,
               tempForcesMomentsAtCM_T0);

   // Propagate to initial-guess temp state using the calculated F&M
   PropagateUsingFM(tempState, massProperties, aDeltaT_sec, tempForcesMomentsAtRP_T0, tempForcesMomentsAtCM_T0);

   // Calculate the F&M at the other endpoint. Evaluate at the full timestep.
   ForceAndMomentsObject tempForcesMomentsAtRP_T1;
   ForceAndMomentsObject tempForcesMomentsAtCM_T1;
   CalculateFM(tempState, aSimTime_nanosec, aDeltaT_sec, tempForcesMomentsAtRP_T1, tempForcesMomentsAtCM_T1);

   // Propagate to new temp state using the calculated F&M
   PropagateUsingFM(tempState, massProperties, aDeltaT_sec, tempForcesMomentsAtRP_T1, tempForcesMomentsAtCM_T1);

   // Average the F&M
   ForceAndMomentsObject averageForcesMomentsAtRP = tempForcesMomentsAtRP_T0.CalcAverageWith(tempForcesMomentsAtRP_T1);
   ForceAndMomentsObject averageForcesMomentsAtCM = tempForcesMomentsAtCM_T0.CalcAverageWith(tempForcesMomentsAtCM_T1);

   // -------------------------------------------------------------------

   // Get a reference to the kinematic state
   KinematicState& kinematicState = *mVehicle->GetKinematicState();

   // Set the following values from tempState from the last CalculateFM into kinematicState
   double   lift_lbs      = tempState.GetLift();
   double   drag_lbs      = tempState.GetDrag();
   double   sideForce_lbs = tempState.GetSideForce();
   double   thrust_lbs    = tempState.GetThrust();
   double   wgt_lbs       = tempState.GetWeight();
   UtVec3dX moment_ftlbs  = tempState.GetMoment();
   kinematicState.SetLiftDragSideForceThrustWeight(lift_lbs, drag_lbs, sideForce_lbs, thrust_lbs, wgt_lbs);
   kinematicState.SetMomentAtCG(moment_ftlbs);

   // -------------------------------------------------------------------

   // Special landing gear handling to avoid jitter on the ground: If we shouldn't
   // be moving, don't update state.
   RigidBodyLandingGear* gear = mVehicle->GetLandingGear();
   if (gear != nullptr && gear->FrictionHoldingStill())
   {
      return;
   }

   // -------------------------------------------------------------------

   // We now "Update" rather than "Propagate" to the final state,
   // using the average F&M along with the original state. Here we
   // use the current sim time (end of frame) and now we use the
   // full frame delta-T, rather than zero/epsilon.
   UpdateUsingFM(kinematicState, massProperties, aSimTime_nanosec, aDeltaT_sec, averageForcesMomentsAtRP, averageForcesMomentsAtCM);

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

void wsf::six_dof::RigidBodyIntegrator::CalculateFM(KinematicState&        aState,
                                                    int64_t                aSimTime_nanosec,
                                                    double                 aDeltaT_sec,
                                                    ForceAndMomentsObject& aForcesMomentsAtRP,
                                                    ForceAndMomentsObject& aForcesMomentsAtCM) const
{
   if (mVehicle == nullptr)
   {
      return;
   }

   // Get references to mass properties and kinematic state
   MassProperties massProperties = mVehicle->GetMassProperties();

   // Calculate various F&M contributors, including aero, propulsion, and
   // landing gear. These F&M are all calculated relative to the reference
   // point (RP) of the object, not the CM. The gravity force will also be
   // calculated, but it is calculated relative to the center of mass (CM).

   // Begin by setting up F&M objects for F&M acting at the RP and CM
   ForceAndMomentsObject ForcesMomentsAtRP;
   ForceAndMomentsObject ForcesMomentsAtCM;

   // Set the CM location for the CM-based F&M object
   UtVec3dX cmRef_ft = massProperties.GetCmPosRelToRef_ft();
   ForcesMomentsAtCM.MoveRefPoint_ft(cmRef_ft);

   // -------------------------------------------------------------------

   // Calc aero F&M

   // Update the aero state parameters (alpha, beta, mach, rho, etc)
   aState.UpdateAeroState(aSimTime_nanosec);

   UtVec3dX aeroLiftBodyVector_lbs;
   UtVec3dX aeroDragBodyVector_lbs;
   UtVec3dX aeroSideBodyVector_lbs;
   UtVec3dX aeroMoment_ftlbs;
   UtVec3dX referencePt_ft;

   // Calculate the aero forces and moments
   mVehicle->CalculateAeroBodyForceAndMoments(aeroLiftBodyVector_lbs,
                                              aeroDragBodyVector_lbs,
                                              aeroSideBodyVector_lbs,
                                              aeroMoment_ftlbs,
                                              referencePt_ft);

   // Sum lift, drag, and side forces into "Total" force
   UtVec3dX aeroTotalBodyForce_lbs = aeroLiftBodyVector_lbs + aeroDragBodyVector_lbs + aeroSideBodyVector_lbs;

   // Use a special aero F&M object -- set the ref pt and calculated F&M
   ForceAndMomentsObject    AeroForcesMomentsAtAeroRP;
   RigidBodyAeroCoreObject* aeroObj = mVehicle->GetActiveAeroObject();
   if (aeroObj != nullptr)
   {
      UtVec3dX aeroCenter_ft = aeroObj->GetAeroCenter_ft();
      referencePt_ft         = referencePt_ft + aeroCenter_ft;
   }
   AeroForcesMomentsAtAeroRP.MoveRefPoint_ft(referencePt_ft);
   AeroForcesMomentsAtAeroRP.AddForceAndMomentAtReferencePoint(aeroTotalBodyForce_lbs, aeroMoment_ftlbs);

   // Add to the reference point F&M (the aero RP will be moved to std RP)
   ForcesMomentsAtRP += AeroForcesMomentsAtAeroRP;

   // -------------------------------------------------------------------

   // Calc propulsion F&M

   UtVec3dX inertialPropulsionForce_lbs;
   UtVec3dX propulsionMoment_ftlbs;

   // Calculate the propulsion forces and moments for this object and subobjects
   mVehicle->CalculatePropulsionFM(aSimTime_nanosec, aDeltaT_sec, aState, inertialPropulsionForce_lbs, propulsionMoment_ftlbs);
   // TODO ---------------------> add ref point

   // Convert propulsion inertial F&M to body coords
   UtVec3dX propulsionTotalBodyForce_lbs = aState.CalcBodyVecFromWCSVec(inertialPropulsionForce_lbs);

   // Add to the reference point F&M
   ForcesMomentsAtRP.AddForceAndMomentAtReferencePoint(propulsionTotalBodyForce_lbs, propulsionMoment_ftlbs);

   // -------------------------------------------------------------------

   // Calc landing gear F&M

   // These are for landing gear (if present). We set these to zero here, so that if
   // no gear are present, zero forces and moments will be added (below)
   UtVec3dX inertialTotalGearVector_lbs;
   UtVec3dX totalGearMoment_ftlbs;

   UtVec3dX aeroTotalInertialForce_lbs = aState.CalcWCSVecFromBodyVec(aeroTotalBodyForce_lbs);

   // The F&M calculation for landing gear needs to know what other forces (non-gravity)
   // are already acting on the object. These include aero and propulsion forces.
   UtVec3dX nonLandingGearTotalForceVecInertial_lbs = aeroTotalInertialForce_lbs + inertialPropulsionForce_lbs;

   // Calculate the landing gear forces and moments
   mVehicle->CalculateLandingGearFM(aSimTime_nanosec,
                                    nonLandingGearTotalForceVecInertial_lbs,
                                    inertialTotalGearVector_lbs,
                                    totalGearMoment_ftlbs);

   // Convert landing gear inertial F&M to body coords
   UtVec3dX landingGearTotalBodyForce_lbs = aState.CalcBodyVecFromWCSVec(inertialTotalGearVector_lbs);

   // Add to the reference point F&M
   ForcesMomentsAtRP.AddForceAndMomentAtReferencePoint(landingGearTotalBodyForce_lbs, totalGearMoment_ftlbs);

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

   // Now, add the gravity force, which acts at the CM instead of
   // the RP like the other F&M above
   ForcesMomentsAtCM.AddForceAtReferencePoint(gravityBodyForce_lbs);

   // -------------------------------------------------------------------

   // Set outgoing data
   aForcesMomentsAtRP = ForcesMomentsAtRP;
   aForcesMomentsAtCM = ForcesMomentsAtCM;

   // The total moment is the combination of moments at RP and CM
   // We start with CM
   ForceAndMomentsObject tempForcesMomentsAtCM = aForcesMomentsAtCM;
   // We then add RP, and the result will automatically be at the CM
   tempForcesMomentsAtCM += aForcesMomentsAtRP;
   // Get the moment at the ref pt, which is the CM
   UtVec3dX moment_ftlbs = tempForcesMomentsAtCM.GetMomentAtRefPoint_ftlbs();
   aState.SetMomentAtCG(moment_ftlbs);
}

void wsf::six_dof::RigidBodyIntegrator::PropagateUsingFM(KinematicState&       aState,
                                                         const MassProperties& aMassProperties,
                                                         double                aDeltaT_sec,
                                                         ForceAndMomentsObject aForcesMomentsAtRP,
                                                         ForceAndMomentsObject aForcesMomentsAtCM) const
{
   if (mVehicle == nullptr)
   {
      return;
   }

   // Get the mass
   double currentMass_lbm = aMassProperties.GetMass_lbs();

   if (currentMass_lbm <= 0)
   {
      return;
   }

   // We want to limit the max g (this should be a settable parameter)
   // to prevent acceleration spikes that can occur.
   const double cMaxG        = 1000.0;
   double       maxForce_lbs = currentMass_lbm * cMaxG;
   aForcesMomentsAtRP.LimitMaxForceMagnitude_lbs(maxForce_lbs);
   aForcesMomentsAtCM.LimitMaxForceMagnitude_lbs(maxForce_lbs);

   // Convert body coords to inertial coords
   UtVec3dX totalNonGravityBodyRelForce_lbs;
   UtVec3dX totalNonGravityMoment_ftlbs;
   aForcesMomentsAtRP.GetForceAndMomentAtCurrentRefPoint(totalNonGravityBodyRelForce_lbs, totalNonGravityMoment_ftlbs);

   double bodyFx = 0.0;
   double bodyFy = 0.0;
   double bodyFz = 0.0;
   totalNonGravityBodyRelForce_lbs.Get(bodyFx, bodyFy, bodyFz);

   // Calculate/set body-rel g-loads (Nx/Ny/Nz)
   double nx_g = bodyFx / currentMass_lbm;
   double ny_g = bodyFy / currentMass_lbm;
   double nz_g = bodyFz / currentMass_lbm;
   aState.SetBodyAccel(nx_g, ny_g, nz_g);

   // -------------------------------------------------------------------

   // At this point, we know the total F&M, which is composed of two
   // sets -- one at the RP and the other at the CM. We want the sum
   // of the F&M acting at the CM. To get this, we merely add the
   // RP F&M to the CM F&M. The ForceAndMomentsObject automatically
   // handles the different reference points for the RP and CM.
   aForcesMomentsAtCM += aForcesMomentsAtRP;

   // ----------------------------------------------------------------------

   // We also want to limit angular acceleration (this should be a settable parameter)
   // to prevent acceleration spikes that can occur.
   const double cMaxOmegaDot_rps = 100.0 * 360.0 * UtMath::cDEG_PER_RAD; // 100 revs/sec2 max

   // Moment = I * accel
   double momentX_ftlbs = aMassProperties.GetIxx_slugft2() * cMaxOmegaDot_rps;
   double momentY_ftlbs = aMassProperties.GetIyy_slugft2() * cMaxOmegaDot_rps;
   double momentZ_ftlbs = aMassProperties.GetIzz_slugft2() * cMaxOmegaDot_rps;

   // The limit is the largest moment
   double maxMoment_ftlbs = momentX_ftlbs;
   if (momentY_ftlbs > maxMoment_ftlbs)
   {
      maxMoment_ftlbs = momentY_ftlbs;
   }
   if (momentZ_ftlbs > maxMoment_ftlbs)
   {
      maxMoment_ftlbs = momentZ_ftlbs;
   }

   aForcesMomentsAtCM.LimitMomentMagnitude_ftlbs(maxMoment_ftlbs);

   // ----------------------------------------------------------------------

   // Convert body coords to inertial coords
   UtVec3dX totalBodyRelForce_lbs;
   UtVec3dX totalMoment_ftlbs;
   aForcesMomentsAtCM.GetForceAndMomentAtCurrentRefPoint(totalBodyRelForce_lbs, totalMoment_ftlbs);

   // Determine the inertial translational acceleration in m/sec^2
   // Note: g is actually 32.1740 not 32.2 (9.80665 m/sec2 or 32.1740 ft/sec2)
   const double cGravitationAccel_mps2 = 9.80665;
   UtVec3dX     bodyAccel_mps2         = totalBodyRelForce_lbs * cGravitationAccel_mps2 / currentMass_lbm;

   // Next, propagate in translation, using the acceleration
   PropagateTranslation(aState, bodyAccel_mps2, aDeltaT_sec);

   // ----------------------------------------------------------------------

   // Determine the rotational acceleration (M = I*a -> a = M/I)
   UtVec3dX rotationalAccel_rps2;
   rotationalAccel_rps2.Set(0, totalMoment_ftlbs.X() / aMassProperties.GetIxx_slugft2());
   rotationalAccel_rps2.Set(1, totalMoment_ftlbs.Y() / aMassProperties.GetIyy_slugft2());
   rotationalAccel_rps2.Set(2, totalMoment_ftlbs.Z() / aMassProperties.GetIzz_slugft2());

   // Now, propagate in rotation
   PropagateRotation(aState, rotationalAccel_rps2, aDeltaT_sec);
}

void wsf::six_dof::RigidBodyIntegrator::UpdateUsingFM(KinematicState&              aState,
                                                      const MassProperties&        aMassProperties,
                                                      int64_t                      aSimTime_nanosec,
                                                      double                       aDeltaT_sec,
                                                      const ForceAndMomentsObject& aForcesMomentsAtRP,
                                                      const ForceAndMomentsObject& aForcesMomentsAtCM) const
{
   if (mVehicle == nullptr)
   {
      return;
   }

   UpdateFuelBurn(aState, aSimTime_nanosec, aDeltaT_sec);

   PropagateUsingFM(aState, aMassProperties, aDeltaT_sec, aForcesMomentsAtRP, aForcesMomentsAtCM);
}


void wsf::six_dof::RigidBodyIntegrator::PropagateRotation(KinematicState& aState,
                                                          UtVec3dX        aRotationalAccel_rps2,
                                                          double          aDeltaT_sec) const
{
   if (mVehicle == nullptr)
   {
      return;
   }

   // If a simple yaw damper is used, we will yaw by the current beta angle, but
   // only if the vehicle is off of the ground

   if (mVehicle->UseSimpleYawDamper() && !mVehicle->GetWeightOnWheels())
   {
      double deltaAngle_rad = aState.GetBeta_rad();

      double yawRate_rps = 0.0;
      if (deltaAngle_rad != 0.0 && aDeltaT_sec > utils::cEPSILON_SIMTIME_SEC)
      {
         // Guard against tiny deltaT
         yawRate_rps = deltaAngle_rad / aDeltaT_sec;
      }

      // Reset omega with a new z-component
      UtVec3dX omega_rps = aState.GetOmegaBody();
      omega_rps.Set(2, yawRate_rps);
      aState.SetOmegaBody(omega_rps);

      // Reset omega dot with the zeroed omegaZdot
      aRotationalAccel_rps2.Set(2, 0.0);
   }

   Integrator::PropagateRotation(aState, aRotationalAccel_rps2, aDeltaT_sec);
}
