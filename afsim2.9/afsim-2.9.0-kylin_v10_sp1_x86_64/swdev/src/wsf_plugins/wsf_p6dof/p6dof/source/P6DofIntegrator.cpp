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

#include "P6DofIntegrator.hpp"

#include "P6DofAeroCoreObject.hpp"
#include "P6DofForceAndMomentsObject.hpp"
#include "P6DofFreezeFlags.hpp"
#include "P6DofKinematicState.hpp"
#include "P6DofLandingGear.hpp"
#include "P6DofMassProperties.hpp"
#include "P6DofScenario.hpp"
#include "P6DofVehicle.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtQuaternion.hpp"

P6DofIntegrator* P6DofIntegrator::Clone() const
{
   return new P6DofIntegrator(*this);
}

void P6DofIntegrator::Update(P6DofVehicle* aObject, int64_t aSimTime_nanosec, double aDeltaT_sec)

{
   // Start by calculating the current center of mass (CM)
   aObject->CalculateCurrentMassProperties();

   // Get a reference to the current kinematic state
   P6DofKinematicState& kinematicState = aObject->GetKinematicState();

   // Get the mass properties
   const P6DofMassProperties& massProperties = aObject->GetMassProperties();

   // Get the atmosphere
   P6DofAtmosphere* atm = aObject->GetScenario()->GetAtmosphere();

   // -------------------------------------------------------------------

   // Copy the kinematic state to a temp state
   P6DofKinematicState tempState = kinematicState;

   // Calculate the F&M at the start. Here, we use the "last update"
   // for the sim time, and want a delta-T of zero. We use epsilon
   // instead of zero to avoid divide by zero issues.
   P6DofForceAndMomentsObject tempForcesMomentsAtRP1;
   P6DofForceAndMomentsObject tempForcesMomentsAtCM1;
   CalculateFM(aObject,
               tempState,
               P6DofUtils::TimeToTime(aObject->GetLastUpdateTime_sec()),
               P6DofUtils::cEPSILON_SIMTIME_SEC,
               tempForcesMomentsAtRP1,
               tempForcesMomentsAtCM1);

   // -------------------------------------------------------------------

   // Propagate to new temp state using the calculated F&M
   PropagateUsingFM(aObject, tempState, massProperties, aDeltaT_sec, tempForcesMomentsAtRP1, tempForcesMomentsAtCM1);

   // Determine the new F&M with the new temp state. We now use
   // the current sim time (end of frame) and we again want a
   // delta-T of zero, but use epsilon.
   P6DofForceAndMomentsObject tempForcesMomentsAtRP2;
   P6DofForceAndMomentsObject tempForcesMomentsAtCM2;
   CalculateFM(aObject, tempState, aSimTime_nanosec, P6DofUtils::cEPSILON_SIMTIME_SEC, tempForcesMomentsAtRP2, tempForcesMomentsAtCM2);

   // Average the F&M
   P6DofForceAndMomentsObject averageForcesMomentsAtRP = tempForcesMomentsAtRP1.CalcAverageWith(tempForcesMomentsAtRP2);

   P6DofForceAndMomentsObject averageForcesMomentsAtCM = tempForcesMomentsAtCM1.CalcAverageWith(tempForcesMomentsAtCM2);

   // -------------------------------------------------------------------

   // Set the following values from tempState from the last CalculateFM into kinematicState
   double   lift_lbs      = tempState.GetLift();
   double   drag_lbs      = tempState.GetDrag();
   double   sideForce_lbs = tempState.GetSideForce();
   double   thrust_lbs    = tempState.GetThrust();
   double   wgt_lbs       = tempState.GetWeight();
   UtVec3dX moment_ftlbs  = tempState.GetMoment();
   kinematicState.SetLiftDragSideForceThrustWeight(lift_lbs, drag_lbs, sideForce_lbs, thrust_lbs, wgt_lbs);
   kinematicState.SetMomentAtCG(moment_ftlbs);

   // We now "Update" rather than "Propagate" to the final state,
   // using the average F&M along with the original state. Here we
   // use the current sim time (end of frame) and now we use the
   // full frame delta-T, rather than zero/epsilon.
   UpdateUsingFM(aObject,
                 kinematicState,
                 massProperties,
                 aSimTime_nanosec,
                 aDeltaT_sec,
                 averageForcesMomentsAtRP,
                 averageForcesMomentsAtCM);

   if (aObject->GetScenario()->GetFreezeFlags()->GetMasterNoAlphaTesting())
   {
      // Remove any alpha
      kinematicState.RemoveAlphaForTesting(atm);
   }

   // Calc rates (especially alpha-dot and beta-dot)
   kinematicState.CalculateRates(aSimTime_nanosec);

   // Calculate parameters that are useful on the "outside"
   kinematicState.CalculateSecondaryParameters(aObject->GetScenario()->GetAtmosphere());
}

void P6DofIntegrator::CalculateFM(P6DofVehicle*               aObject,
                                  P6DofKinematicState&        aState,
                                  int64_t                     aSimTime_nanosec,
                                  double                      aDeltaT_sec,
                                  P6DofForceAndMomentsObject& aForcesMomentsAtRP,
                                  P6DofForceAndMomentsObject& aForcesMomentsAtCM)
{
   // Get references to mass properties and kinematic state
   const P6DofMassProperties& massProperties = aObject->GetMassProperties();

   // Calculate various F&M contributors, including aero, propulsion, and
   // landing gear. These F&M are all calculated relative to the reference
   // point (RP) of the object, not the CM. The gravity force will also be
   // calculated, but it is calculated relative to the center of mass (CM).

   // Begin by setting up F&M objects for F&M acting at the RP and CM
   P6DofForceAndMomentsObject ForcesMomentsAtRP;
   P6DofForceAndMomentsObject ForcesMomentsAtCM;

   // Set the CM location for the CM-based F&M object
   UtVec3dX cmRef_ft = massProperties.GetCmPosRelToRef_ft();
   ForcesMomentsAtCM.MoveRefPoint_ft(cmRef_ft);

   // -------------------------------------------------------------------

   // Calc aero F&M

   // Update the aero state parameters (alpha, beta, mach, rho, etc)
   aState.UpdateAeroState(aObject->GetScenario()->GetAtmosphere(),
                          aObject->GetScenario()->GetWind(),
                          aSimTime_nanosec,
                          aDeltaT_sec);

   UtVec3dX aeroLiftBodyVector_lbs(0.0, 0.0, 0.0);
   UtVec3dX aeroDragBodyVector_lbs(0.0, 0.0, 0.0);
   UtVec3dX aeroSideBodyVector_lbs(0.0, 0.0, 0.0);
   UtVec3dX aeroMoment_ftlbs(0.0, 0.0, 0.0);
   UtVec3dX referencePt_ft(0.0, 0.0, 0.0);

   // Calculate the aero forces and moments
   aObject->CalculateAeroBodyFM(aeroLiftBodyVector_lbs,
                                aeroDragBodyVector_lbs,
                                aeroSideBodyVector_lbs,
                                aeroMoment_ftlbs,
                                referencePt_ft);

   // Sum lift, drag, and side forces into "Total" force
   UtVec3dX aeroTotalBodyForce_lbs = aeroLiftBodyVector_lbs + aeroDragBodyVector_lbs + aeroSideBodyVector_lbs;


   // Use a special aero F&M object -- set the ref pt and calculated F&M
   P6DofForceAndMomentsObject AeroForcesMomentsAtAeroRP;
   P6DofAeroCoreObject*       aeroObj = aObject->GetActiveAeroObject();
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

   UtVec3dX inertialPropulsionForce_lbs(0., 0., 0.);
   UtVec3dX propulsionMoment_ftlbs(0., 0., 0.);

   // Calculate the propulsion forces and moments for this object and subobjects
   aObject->CalculatePropulsionFM(aSimTime_nanosec, aDeltaT_sec, aState, inertialPropulsionForce_lbs, propulsionMoment_ftlbs);
   // TODO ---------------------> add ref point

   // Convert propulsion inertial F&M to body coords
   UtVec3dX propulsionTotalBodyForce_lbs = aState.CalcBodyVecFromInertialVec(inertialPropulsionForce_lbs);

   // Add to the reference point F&M
   ForcesMomentsAtRP.AddForceAndMomentAtReferencePoint(propulsionTotalBodyForce_lbs, propulsionMoment_ftlbs);

   // -------------------------------------------------------------------

   // Calc landing gear F&M

   // These are for landing gear (if present). We set these to zero here, so that if
   // no gear are present, zero forces and moments will be added (below)
   UtVec3dX inertialTotalGearVector_lbs(0., 0., 0.);
   UtVec3dX totalGearMoment_ftlbs(0., 0., 0.);

   UtVec3dX aeroTotalInertialForce_lbs = aState.CalcInertialVecFromBodyVec(aeroTotalBodyForce_lbs);

   // The F&M calculation for landing gear needs to know what other forces (non-gravity)
   // are already acting on the object. These include aero and propulsion forces.
   UtVec3dX nonLandingGearTotalForceVecInertial_lbs = aeroTotalInertialForce_lbs + inertialPropulsionForce_lbs;

   // Calculate the landing gear forces and moments
   aObject->CalculateLandingGearFM(aSimTime_nanosec,
                                   nonLandingGearTotalForceVecInertial_lbs,
                                   inertialTotalGearVector_lbs,
                                   totalGearMoment_ftlbs);

   // Convert landing gear inertial F&M to body coords
   UtVec3dX landingGearTotalBodyForce_lbs = aState.CalcBodyVecFromInertialVec(inertialTotalGearVector_lbs);

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
   UtVec3dX gravityVec = P6DofKinematicState::NormalizedGravitationalAccelVec(aObject->GetScenario()->GetGravity(),
                                                                              aState.GetLat(),
                                                                              aState.GetLon(),
                                                                              aState.GetAlt_m(),
                                                                              aState.UseSphericalEarth());
   // Next, calc the gravity force vector
   double   currentMass_lbm          = massProperties.GetMass_lbs();
   UtVec3dX gravityInertialForce_lbs = gravityVec * currentMass_lbm;

   // Convert gravity inertial F&M to body coords
   UtVec3dX gravityBodyForce_lbs = aState.CalcBodyVecFromInertialVec(gravityInertialForce_lbs);

   // Now, add the gravity force, which acts at the CM instead of
   // the RP like the other F&M above
   ForcesMomentsAtCM.AddForceAtReferencePoint(gravityBodyForce_lbs);

   // -------------------------------------------------------------------

   // Set outgoing data
   aForcesMomentsAtRP = ForcesMomentsAtRP;
   aForcesMomentsAtCM = ForcesMomentsAtCM;

   // The total moment is the combination of moments at RP and CM
   // We start with CM
   P6DofForceAndMomentsObject tempForcesMomentsAtCM = aForcesMomentsAtCM;
   // We then add RP, and the result will automatically be at the CM
   tempForcesMomentsAtCM += aForcesMomentsAtRP;
   // Get the moment at the ref pt, which is the CM
   UtVec3dX moment_ftlbs = tempForcesMomentsAtCM.GetMomentAtRefPoint_ftlbs();
   aState.SetMomentAtCG(moment_ftlbs);
}

void P6DofIntegrator::UpdateFuelBurn(P6DofVehicle*        aObject,
                                     P6DofKinematicState& aState,
                                     int64_t              aSimTime_nanosec,
                                     double               aDeltaT_sec)
{
   // Update the fuel/propellant burn for engines and fuel tanks
   aObject->UpdateFuelBurn(aSimTime_nanosec, aDeltaT_sec, aState);
}

void P6DofIntegrator::PropagateUsingFM(P6DofVehicle*               aObject,
                                       P6DofKinematicState&        aState,
                                       const P6DofMassProperties&  aMassProperties,
                                       double                      aDeltaT_sec,
                                       P6DofForceAndMomentsObject& aForcesMomentsAtRP,
                                       P6DofForceAndMomentsObject& aForcesMomentsAtCM)
{
   // Convert body coords to inertial coords
   UtVec3dX totalNonGravityBodyRelForce_lbs(0., 0., 0.);
   UtVec3dX totalNonGravityMoment_ftlbs(0., 0., 0.);
   aForcesMomentsAtRP.GetForceAndMomentAtCurrentRefPoint(totalNonGravityBodyRelForce_lbs, totalNonGravityMoment_ftlbs);

   double bodyFx = 0.0;
   double bodyFy = 0.0;
   double bodyFz = 0.0;
   totalNonGravityBodyRelForce_lbs.Get(bodyFx, bodyFy, bodyFz);

   // Calculate/set body-rel g-loads (Nx/Ny/Nz)
   double currentMass_lbm = aMassProperties.GetMass_lbs();
   double nx_g            = 0;
   double ny_g            = 0;
   double nz_g            = 0;
   if (currentMass_lbm > 0)
   {
      nx_g = bodyFx / currentMass_lbm;
      ny_g = bodyFy / currentMass_lbm;
      nz_g = bodyFz / currentMass_lbm;
   }
   aState.SetBodyAccel(nx_g, ny_g, nz_g);

   // -------------------------------------------------------------------

   // At this point, we know the total F&M, which is composed of two
   // sets -- one at the RP and the other at the CM. We want the sum
   // of the F&M acting at the CM. To get this, we merely add the
   // RP F&M to the CM F&M. The P6DofForceAndMomentsObject automatically
   // handles the different reference points for the RP and CM.
   P6DofForceAndMomentsObject totalForcesMomentsAtCM = aForcesMomentsAtCM;
   totalForcesMomentsAtCM += aForcesMomentsAtRP;

   // Get the mass
   double currentMass_lbs = aMassProperties.GetMass_lbs();

   // ----------------------------------------------------------------------

   // Limit F&M (optional)

   // Note: These two tests (limitMaxG and limitRotAccel) are currently being
   // enforced at all times, but we may want the choice of whether or not to
   // use these tests be be selectable through data files.

   // We want to limit the max g (this should be a settable parameter)
   // to prevent acceleration spikes that can occur. This is optional.
   const bool   limitMaxG = true;
   const double cMaxG     = 1000.0;
   if (limitMaxG)
   {
      double maxForce_lbs = currentMass_lbs * cMaxG;
      totalForcesMomentsAtCM.LimitMaxForceMagnitude_lbs(maxForce_lbs);
   }

   // We also want to limit angular acceleration (this should be a settable parameter)
   // to prevent acceleration spikes that can occur. This is optional.
   bool         limitRotAccel    = true;
   const double cMaxOmegaDot_rps = 100.0 * 360.0 * UtMath::cDEG_PER_RAD; // 100 revs/sec2 max
   if (limitRotAccel)
   {
      // Mom = I * accel
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

      totalForcesMomentsAtCM.LimitMomentMagnitude_ftlbs(maxMoment_ftlbs);
   }

   // ----------------------------------------------------------------------

   // Convert body coords to inertial coords
   UtVec3dX totalBodyRelForce_lbs(0., 0., 0.);
   UtVec3dX totalMoment_ftlbs(0., 0., 0.);
   totalForcesMomentsAtCM.GetForceAndMomentAtCurrentRefPoint(totalBodyRelForce_lbs, totalMoment_ftlbs);
   UtVec3dX totalInertialForce_lbs = aState.CalcInertialVecFromBodyVec(totalBodyRelForce_lbs);

   // Determine the inertial translational acceleration in m/sec^2
   // Note: g is actually 32.1740 not 32.2 (9.80665 m/sec2 or 32.1740 ft/sec2)
   const double cGravitationAccel_mps2 = 9.80665;
   UtVec3dX     inertialAccel_mps2;
   inertialAccel_mps2.Set(0, cGravitationAccel_mps2 * (totalInertialForce_lbs.X() / currentMass_lbs));
   inertialAccel_mps2.Set(1, cGravitationAccel_mps2 * (totalInertialForce_lbs.Y() / currentMass_lbs));
   inertialAccel_mps2.Set(2, cGravitationAccel_mps2 * (totalInertialForce_lbs.Z() / currentMass_lbs));

   // Next, propagate in translation, using the acceleration
   PropagateTranslation(aObject, aState, inertialAccel_mps2, aDeltaT_sec);

   // ----------------------------------------------------------------------

   // Determine the rotational acceleration (M = I*a -> a = M/I)
   UtVec3dX rotationalAccel_rps2;
   rotationalAccel_rps2.Set(0, totalMoment_ftlbs.X() / aMassProperties.GetIxx_slugft2());
   rotationalAccel_rps2.Set(1, totalMoment_ftlbs.Y() / aMassProperties.GetIyy_slugft2());
   rotationalAccel_rps2.Set(2, totalMoment_ftlbs.Z() / aMassProperties.GetIzz_slugft2());

   // Now, propagate in rotation
   PropagateRotation(aObject, aState, rotationalAccel_rps2, aDeltaT_sec);
}

void P6DofIntegrator::UpdateUsingFM(P6DofVehicle*               aObject,
                                    P6DofKinematicState&        aState,
                                    const P6DofMassProperties&  aMassProperties,
                                    int64_t                     aSimTime_nanosec,
                                    double                      aDeltaT_sec,
                                    P6DofForceAndMomentsObject& aForcesMomentsAtRP,
                                    P6DofForceAndMomentsObject& aForcesMomentsAtCM)
{
   UpdateFuelBurn(aObject, aState, aSimTime_nanosec, aDeltaT_sec);

   PropagateUsingFM(aObject, aState, aMassProperties, aDeltaT_sec, aForcesMomentsAtRP, aForcesMomentsAtCM);
}


void P6DofIntegrator::PropagateTranslation(P6DofVehicle*        aObject,
                                           P6DofKinematicState& aState,
                                           const UtVec3dX&      aInertialAccel_mps2,
                                           double               aDeltaT_sec)
{
   if (aState.UseSphericalEarth())
   {
      PropagateTranslationSphericalEarth(aObject, aState, aInertialAccel_mps2, aDeltaT_sec);
   }
   else
   {
      PropagateTranslationWGSEarth(aObject, aState, aInertialAccel_mps2, aDeltaT_sec);
   }
}

void P6DofIntegrator::PropagateTranslationSphericalEarth(P6DofVehicle*        aObject,
                                                         P6DofKinematicState& aState,
                                                         UtVec3dX             aInertialAccel_mps2,
                                                         double               aDeltaT_sec)
{
   P6DofFreezeFlags* manager = aObject->GetScenario()->GetFreezeFlags();
   if (manager == nullptr)
   {
      ut::log::error() << "Null Freeze Flags in P6DofIntegrator::PropagateTranslationSphericalEarth().";
      return;
   }

   // Set the inertial acceleration
   aState.SetInertialAccel(aInertialAccel_mps2);

   if (manager->GetMasterFreezeLocation() && manager->GetMasterFreezeAltitude())
   {
      // We're in a full 3D frozen state, do nothing
   }
   else if (manager->GetMasterFreezeLocation())
   {
      // Remember the current Lat/Lon
      double savedLat = aState.GetLat();
      double savedLon = aState.GetLon();

      // Update position
      aState.SetLocationSpherical(aState.GetLocationSpherical() + aState.GetVelocitySpherical() * aDeltaT_sec +
                                  0.5 * (aDeltaT_sec * aDeltaT_sec) * aState.GetInertialAccel());

      // Get LLA
      UtVec3dX pos = aState.GetLocationSpherical();
      double   lat, lon, alt;
      P6DofUtils::CalcSphericalLatLonAlt(pos, lat, lon, alt);

      // Revert to saved values for lat/lon
      lat = savedLat;
      lon = savedLon;

      // Get position
      P6DofUtils::CalcSphericalPositionFromLatLonAlt(lat, lon, alt, pos);

      // Set position (which has unmoved lat/lon)
      aState.SetLocationSpherical(pos);
   }
   else if (manager->GetMasterFreezeAltitude())
   {
      // Remember the current alt
      double savedAlt_m = aState.GetAlt_m();

      // Update position
      aState.SetLocationSpherical(aState.GetLocationSpherical() + aState.GetVelocitySpherical() * aDeltaT_sec +
                                  0.5 * (aDeltaT_sec * aDeltaT_sec) * aState.GetInertialAccel());

      // Get LLA
      UtVec3dX pos = aState.GetLocationSpherical();
      double   lat, lon, alt;
      P6DofUtils::CalcSphericalLatLonAlt(pos, lat, lon, alt);

      // Revert to saved values for alt
      alt = savedAlt_m;

      // Get position
      P6DofUtils::CalcSphericalPositionFromLatLonAlt(lat, lon, alt, pos);

      // Set position (which has unmoved alt)
      aState.SetLocationSpherical(pos);
   }
   else
   {
      // We are not frozen, so perform the standard calculation
      aState.SetLocationSpherical(aState.GetLocationSpherical() + aState.GetVelocitySpherical() * aDeltaT_sec +
                                  0.5 * (aDeltaT_sec * aDeltaT_sec) * aState.GetInertialAccel());
   }

   // Get the current speed
   double current_speed = aState.GetVelocitySpherical().Magnitude();

   // Update velocity
   if (manager->GetMasterFreezeSpeed() && manager->GetMasterFreezeVerticalSpeed())
   {
      aState.SetVelocitySpherical(aState.GetVelocitySpherical() + aState.GetInertialAccel() * aDeltaT_sec);

      // Get the new inertial velocity
      UtVec3dX inertialVel_mps = aState.GetVelocitySpherical();

      // Convert to NED velocity
      UtVec3dX velNED_mps = aState.CalcNEDVecFromInertialVec(inertialVel_mps);
      double   vecVelNED_mps[3];
      velNED_mps.Get(vecVelNED_mps);

      // Zero vertical speed
      vecVelNED_mps[2] = 0;
      velNED_mps.Set(vecVelNED_mps);

      // Now freeze the speed to what it was
      velNED_mps.Normalize();
      velNED_mps = velNED_mps * current_speed;

      // Set spherical vel using NED
      UtVec3dX velSpherical_mps = aState.CalcInertialVecFromNEDVec(velNED_mps);
      aState.SetVelocitySpherical(velSpherical_mps);
   }
   else if (manager->GetMasterFreezeSpeed())
   {
      UtVec3dX velocity = aState.GetVelocitySpherical() + aState.GetInertialAccel() * aDeltaT_sec;
      // Now freeze the speed to what it was
      velocity.Normalize();
      aState.SetVelocitySpherical(velocity * current_speed);
   }
   else if (manager->GetMasterFreezeVerticalSpeed())
   {
      aState.SetVelocitySpherical(aState.GetVelocitySpherical() + aState.GetInertialAccel() * aDeltaT_sec);

      // Get the new inertial velocity
      UtVec3dX inertialVel_mps = aState.GetVelocitySpherical();

      // Convert to NED velocity
      UtVec3dX velNED_mps = aState.CalcNEDVecFromInertialVec(inertialVel_mps);
      double   vecVelNED_mps[3];
      velNED_mps.Get(vecVelNED_mps);

      // Zero vertical speed and set the "frozen" value
      vecVelNED_mps[2] = 0;

      // Set spherical vel using NED
      UtVec3dX velSpherical_mps = aState.CalcInertialVecFromNEDVec(vecVelNED_mps);
      aState.SetVelocitySpherical(velSpherical_mps);
   }
   else
   {
      aState.SetVelocitySpherical(aState.GetVelocitySpherical() + aState.GetInertialAccel() * aDeltaT_sec);
   }
}

void P6DofIntegrator::PropagateTranslationWGSEarth(P6DofVehicle*        aObject,
                                                   P6DofKinematicState& aState,
                                                   UtVec3dX             aInertialAccel_mps2,
                                                   double               aDeltaT_sec)
{
   P6DofFreezeFlags* manager = aObject->GetScenario()->GetFreezeFlags();
   if (manager == nullptr)
   {
      ut::log::error() << "Null Freeze Flags in P6DofIntegrator::PropagateTranslationWGSEarth().";
      return;
   }

   // Set the inertial acceleration
   aState.SetInertialAccel(aInertialAccel_mps2);

   if (manager->GetMasterFreezeLocation() && manager->GetMasterFreezeAltitude())
   {
      // We're in a full 3D frozen state, do nothing
   }
   else if (manager->GetMasterFreezeLocation())
   {
      // Remember the current lat/lon
      double savedLat = aState.GetLat();
      double savedLon = aState.GetLon();

      // Update position
      aState.SetLocationWGS(aState.GetLocationWGS() + aState.GetVelocityWGS() * aDeltaT_sec +
                            0.5 * (aDeltaT_sec * aDeltaT_sec) * aState.GetInertialAccel());

      // Get LLA
      UtVec3dX pos = aState.GetLocationWGS();
      double   lat, lon, alt;
      UtEntity tempEntity;
      double   posVec[3];
      pos.Get(posVec);
      tempEntity.SetLocationWCS(posVec);
      tempEntity.GetLocationLLA(lat, lon, alt);

      // Revert to saved values for lat/lon
      lat = savedLat;
      lon = savedLon;

      // Get position
      tempEntity.SetLocationLLA(lat, lon, alt);
      tempEntity.GetLocationWCS(posVec);

      // Set position (which has unmoved lat/lon)
      aState.SetLocationWGS(posVec);
   }
   else if (manager->GetMasterFreezeAltitude())
   {
      // Remember the current alt
      double savedAlt_m = aState.GetAlt_m();

      // Update position
      aState.SetLocationWGS(aState.GetLocationWGS() + aState.GetVelocityWGS() * aDeltaT_sec +
                            0.5 * (aDeltaT_sec * aDeltaT_sec) * aState.GetInertialAccel());

      // Get LLA
      UtVec3dX pos = aState.GetLocationWGS();
      double   lat, lon, alt;
      UtEntity tempEntity;
      double   posVec[3];
      pos.Get(posVec);
      tempEntity.SetLocationWCS(posVec);
      tempEntity.GetLocationLLA(lat, lon, alt);

      // Revert to saved values for alt
      alt = savedAlt_m;

      // Get position
      tempEntity.SetLocationLLA(lat, lon, alt);
      tempEntity.GetLocationWCS(posVec);

      // Set position (which has unmoved lat/lon)
      aState.SetLocationWGS(posVec);
   }
   else
   {
      // We are not frozen, so perform the standard calculation
      aState.SetLocationWGS(aState.GetLocationWGS() + aState.GetVelocityWGS() * aDeltaT_sec +
                            0.5 * (aDeltaT_sec * aDeltaT_sec) * aState.GetInertialAccel());
   }

   // Get current speed
   double current_speed_mps = aState.GetVelocityWGS().Magnitude();

   // Update velocity
   if (manager->GetMasterFreezeSpeed() && manager->GetMasterFreezeVerticalSpeed())
   {
      aState.SetVelocityWGS(aState.GetVelocityWGS() + aState.GetInertialAccel() * aDeltaT_sec);

      // Get the new inertial velocity
      UtVec3dX inertialVel_mps = aState.GetVelocityWGS();

      // Convert to NED velocity
      UtVec3dX velNED_mps = aState.CalcNEDVecFromInertialVec(inertialVel_mps);
      double   vecVelNED_mps[3];
      velNED_mps.Get(vecVelNED_mps);

      // Zero vertical speed
      vecVelNED_mps[2] = 0;
      velNED_mps.Set(vecVelNED_mps);

      // Now freeze the speed to what it was
      velNED_mps.Normalize();
      velNED_mps = velNED_mps * current_speed_mps;

      // Set WGS vel using NED
      UtVec3dX velWGS_mps = aState.CalcInertialVecFromNEDVec(velNED_mps);
      aState.SetVelocityWGS(velWGS_mps);
   }
   else if (manager->GetMasterFreezeSpeed())
   {
      UtVec3dX velocity = aState.GetVelocityWGS() + aState.GetInertialAccel() * aDeltaT_sec;
      // Now freeze the speed to what it was
      velocity.Normalize();
      aState.SetVelocityWGS(velocity * current_speed_mps);
   }
   else if (manager->GetMasterFreezeVerticalSpeed())
   {
      aState.SetVelocityWGS(aState.GetVelocityWGS() + aState.GetInertialAccel() * aDeltaT_sec);

      // Get the new inertial velocity
      UtVec3dX inertialVel_mps = aState.GetVelocityWGS();

      // Convert to NED velocity
      UtVec3dX velNED_mps = aState.CalcNEDVecFromInertialVec(inertialVel_mps);
      double   vecVelNED_mps[3];
      velNED_mps.Get(vecVelNED_mps);

      // Zero vertical speed
      vecVelNED_mps[2] = 0;

      // Set WGS vel using NED
      UtVec3dX velWGS_mps = aState.CalcInertialVecFromNEDVec(vecVelNED_mps);
      aState.SetVelocityWGS(velWGS_mps);
   }
   else
   {
      aState.SetVelocityWGS(aState.GetVelocityWGS() + aState.GetInertialAccel() * aDeltaT_sec);
   }
}

void P6DofIntegrator::PropagateRotation(P6DofVehicle*        aObject,
                                        P6DofKinematicState& aState,
                                        UtVec3dX             aRotationalAccel_rps2,
                                        double               aDeltaT_sec)
{
   double omegaX_dot = aRotationalAccel_rps2.X();
   double omegaY_dot = aRotationalAccel_rps2.Y();
   double omegaZ_dot = aRotationalAccel_rps2.Z();

   // Freeze angular rates, if appropriate
   P6DofFreezeFlags* manager = aObject->GetScenario()->GetFreezeFlags();
   if (manager->GetMasterFreezeYaw())
   {
      omegaZ_dot    = 0; // Freeze the accel
      UtVec3dX temp = aState.GetOmegaBody();
      temp.Set(2, 0.0);
      aState.SetOmegaBody(temp); // Freeze the rate
   }
   if (manager->GetMasterFreezePitch())
   {
      omegaY_dot    = 0; // Freeze the accel
      UtVec3dX temp = aState.GetOmegaBody();
      temp.Set(1, 0.0);
      aState.SetOmegaBody(temp); // Freeze the rate
   }
   if (manager->GetMasterFreezeRoll())
   {
      omegaX_dot    = 0; // Freeze the accel
      UtVec3dX temp = aState.GetOmegaBody();
      temp.Set(0, 0.0);
      aState.SetOmegaBody(temp); // Freeze the rate
   }

   aState.SetOmegaBodyDot(UtVec3dX(omegaX_dot, omegaY_dot, omegaZ_dot));

   UtVec3dX delAng(0., 0., 0.);
   delAng = aState.GetOmegaBody() * aDeltaT_sec + aState.GetOmegaBodyDot() * (0.5 * aDeltaT_sec * aDeltaT_sec);

   // Set new omega (body rates)
   aState.SetOmegaBody(aState.GetOmegaBody() + aState.GetOmegaBodyDot() * aDeltaT_sec);

   // Special landing gear handling for rotation effects when on the ground
   P6DofLandingGear* gear = aObject->GetLandingGear();
   if (gear != nullptr && gear->FrictionHoldingStill())
   {
      double rates[3];
      aState.GetOmegaBody().Get(rates);
      rates[0] = 0.0;
      rates[2] = 0.0;
      aState.SetOmegaBody(rates);

      // Set velocity to zero
      UtVec3dX velNED_mps = aState.GetVelocityNED_mps();
      double   vecNED_mps[3];
      velNED_mps.Get(vecNED_mps);
      aState.SetVelocityNED(0.0, 0.0, vecNED_mps[2]);
   }

   // **************************************************************************
   // If a simple yaw damper is used, we will yaw by the current beta angle, but
   // only if the vehicle is off of the ground
   bool offGround = true;
   if (gear != nullptr)
   {
      if (gear->WeightOnWheels())
      {
         offGround = false;
      }
   }

   if (aObject->UseSimpleYawDamper() && offGround)
   {
      double deltaAngle_rad = aState.GetBeta_rad();

      double yawRate_rps = 0.0;
      if (deltaAngle_rad != 0.0 && aDeltaT_sec > P6DofUtils::cEPSILON_SIMTIME_SEC)
      {
         // Guard against tiny deltaT
         yawRate_rps = deltaAngle_rad / aDeltaT_sec;
      }

      UtVec3dX temp = aState.GetOmegaBody();
      temp.Set(2, yawRate_rps);
      aState.SetOmegaBody(temp); // Set the rate

      // Reset omega dot with the zeroed omegaZdot
      omegaZ_dot = 0; // Freeze the acceleration
      aState.SetOmegaBodyDot(UtVec3dX(omegaX_dot, omegaY_dot, omegaZ_dot));

      // Reset omega with a zero z-component
      double omegaVec[3];
      aState.GetOmegaBody().Get(omegaVec);
      omegaVec[2] = yawRate_rps;
      aState.SetOmegaBody(omegaVec);
   }
   // **************************************************************************

   // Save the angle deltas (needed by PilotObject)
   aState.SetAngleDeltas(delAng.Z(), delAng.Y(), delAng.X());

   // ----------------------------------------------------------------------

   // Old DCM approach to integrating attitude (no longer used):
   // Adjust DCM with ypr deltas
   // UtDCM yawDCM(deltaYaw_rad,0.0,0.0);
   // UtDCM pitchDCM(0.0,deltaPitch_rad,0.0);
   // UtDCM rollDCM(0.0,0.0,deltaRoll_rad);
   // mDCM = rollDCM * (pitchDCM * (yawDCM * mDCM));

   // Quaternion approach to integrating attitude

   // Set the attitude quaternion using the current DCM
   UtQuaternion attitudeQuaternion(aState.GetDCM());

   // Set the body rates
   UtVec3dX bodyRates(aState.GetOmegaBody().X(), aState.GetOmegaBody().Y(), aState.GetOmegaBody().Z());

   // Set the rate quaternion using the body rates and the attitude quaternion
   UtQuaternion rateQuaternion;
   rateQuaternion.SetRate(attitudeQuaternion, bodyRates);

   // Obtain the new attitude quaternion
   UtQuaternion newAttitudeQ = attitudeQuaternion + (rateQuaternion * aDeltaT_sec);

   // Normalize the quaternion -- this is very important
   newAttitudeQ.Normalize();

   // Get the new DCM from the new quaternion
   UtDCM dcm;
   newAttitudeQ.Get(dcm);
   aState.SetDCM(dcm);
}
