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

#include "WsfSixDOF_Integrator.hpp"

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtQuaternion.hpp"
#include "WsfSixDOF_KinematicState.hpp"
#include "WsfSixDOF_Mover.hpp"

void wsf::six_dof::Integrator::UpdateFuelBurn(KinematicState& aState, int64_t aSimTime_nanosec, double aDeltaT_sec) const
{
   if (GetParentVehicle() == nullptr)
   {
      return;
   }

   // Update the fuel/propellant burn for engines and fuel tanks
   GetParentVehicle()->UpdateFuelBurn(aSimTime_nanosec, aDeltaT_sec, aState);
}

void wsf::six_dof::Integrator::PropagateTranslation(KinematicState& aState,
                                                    const UtVec3dX& aBodyAccel_mps2,
                                                    double          aDeltaT_sec) const
{
   if (GetParentVehicle() == nullptr)
   {
      return;
   }

   // Simple rotation
   UtVec3dX accelerationWCS_mps2 = aState.CalcWCSVecFromBodyVec(aBodyAccel_mps2);

   if (aState.UseRotatingEarth())
   {
      UtVec3dX earthRotationRate_rps;
      earthRotationRate_rps.Set(2, aState.GetUtEntity().GetCentralBody().GetRotationRate());

      UtVec3dX centripetalAcceleration_mps2 =
         -earthRotationRate_rps.Cross(earthRotationRate_rps.Cross(aState.GetLocationWCS()));
      UtVec3dX coriolisAcceleration_mps2 = -2.0 * earthRotationRate_rps.Cross(aState.GetVelocityWCS());

      accelerationWCS_mps2 += (centripetalAcceleration_mps2 + coriolisAcceleration_mps2);
   }

   aState.SetAccelerationWCS(accelerationWCS_mps2);

   // Remember the current lat/lon/alt
   UtLLAPos savedLLA         = aState.GetCurrentPositionLLA();
   UtVec3dX savedVelocityNED = aState.GetVelocityNED_mps();

   // Update position and velocity
   aState.SetLocationWCS(aState.GetLocationWCS() + aState.GetVelocityWCS() * aDeltaT_sec +
                         0.5 * aState.GetAccelerationWCS() * aDeltaT_sec * aDeltaT_sec);
   aState.SetVelocityWCS(aState.GetVelocityWCS() + aState.GetAccelerationWCS() * aDeltaT_sec);

   // Translational freeze flags are evaluated post-integration

   FreezeFlags* freezeFlags = GetParentVehicle()->GetFreezeFlags();

   if (freezeFlags->location)
   {
      // Revert to saved values for lat/lon
      aState.SetPositionLLA(savedLLA.GetLat(), savedLLA.GetLon(), aState.GetAlt_m());
   }

   if (freezeFlags->altitude)
   {
      // Revert to saved values for alt
      aState.SetAlt(savedLLA.GetAlt());
   }

   if (freezeFlags->verticalSpeed)
   {
      // Convert to NED velocity
      UtVec3dX velocityNED = aState.GetVelocityNED_mps();

      // Remove vertical speed component
      velocityNED.Set(2, 0.0);

      // Convert back
      aState.SetVelocityNED(velocityNED.X(), velocityNED.Y(), velocityNED.Z());
   }

   if (freezeFlags->speed)
   {
      // Reset the velocity magnitude

      UtVec3dX velocityWCS = aState.GetVelocityWCS();
      velocityWCS.Normalize();
      aState.SetVelocityWCS(velocityWCS * savedVelocityNED.Magnitude());
   }
}

void wsf::six_dof::Integrator::PropagateRotation(KinematicState& aState, UtVec3dX aRotationalAccel_rps2, double aDeltaT_sec) const
{
   if (GetParentVehicle() == nullptr)
   {
      return;
   }

   // Rotational freeze flags are evaluated pre-integration

   FreezeFlags* freezeFlags = GetParentVehicle()->GetFreezeFlags();

   if (freezeFlags->roll)
   {
      aRotationalAccel_rps2.Set(0, 0.0); // Freeze the accel
      aState.SetRollRate(0.0);           // Freeze the rate
   }

   if (freezeFlags->pitch)
   {
      aRotationalAccel_rps2.Set(1, 0.0); // Freeze the accel
      aState.SetPitchRate(0.0);          // Freeze the rate
   }

   if (freezeFlags->yaw)
   {
      aRotationalAccel_rps2.Set(2, 0.0); // Freeze the accel
      aState.SetYawRate(0.0);            // Freeze the rate
   }

   aState.SetOmegaBodyDot(aRotationalAccel_rps2);

   // Set new body rates
   aState.SetOmegaBody(aState.GetOmegaBody() + aState.GetOmegaBodyDot() * aDeltaT_sec);

   // ----------------------------------------------------------------------

   // Set the attitude quaternion using the current DCM
   UtQuaternion attitudeQuaternion(aState.GetDCM());

   // Set the rate quaternion using the body rates and the attitude quaternion
   UtQuaternion rateQuaternion;
   rateQuaternion.SetRate(attitudeQuaternion, aState.GetOmegaBody());

   // Obtain the new attitude quaternion
   attitudeQuaternion.Update(rateQuaternion, aDeltaT_sec);

   // Normalize the quaternion -- this is very important
   attitudeQuaternion.Normalize();

   // Get the new DCM from the new quaternion
   UtDCM dcm;
   attitudeQuaternion.Get(dcm);
   aState.SetDCM(dcm);

   // Save the angle deltas (needed by PilotObject)
   UtVec3dX delAng = aState.GetOmegaBody() * aDeltaT_sec + 0.5 * aState.GetOmegaBodyDot() * aDeltaT_sec * aDeltaT_sec;
   aState.SetAngleDeltas(delAng.Z(), delAng.Y(), delAng.X());
}
