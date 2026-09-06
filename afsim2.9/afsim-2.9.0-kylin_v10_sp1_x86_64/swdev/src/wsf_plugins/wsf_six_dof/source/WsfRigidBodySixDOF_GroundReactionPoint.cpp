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

#include "WsfRigidBodySixDOF_GroundReactionPoint.hpp"

#include <stdio.h>

#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfRigidBodySixDOF_Mover.hpp"
#include "WsfRigidBodySixDOF_Utils.hpp"
#include "WsfSixDOF_Environment.hpp"

wsf::six_dof::RigidBodyGroundReactionPoint* wsf::six_dof::RigidBodyGroundReactionPoint::Clone() const
{
   return new RigidBodyGroundReactionPoint(*this);
}

bool wsf::six_dof::RigidBodyGroundReactionPoint::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "ground_reaction_point")
   {
      std::string name;
      aInput.ReadValue(name);
      SetName(name);

      UtInputBlock block(aInput, "end_ground_reaction_point");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "control_surface_name")
         {
            std::string value;
            aInput.ReadValue(value);
            mControlSurfaceName = value;
         }
         else if (localCommand == "steering_control_surface_name")
         {
            std::string value;
            aInput.ReadValue(value);
            mSteeringControlSurfaceName = value;
         }
         else if (localCommand == "nws_angle_control_surface_name")
         {
            std::string value;
            aInput.ReadValue(value);
            mNWSAngleControlSurfaceName = value;
         }
         else if (localCommand == "is_nose_gear")
         {
            bool value;
            aInput.ReadValue(value);
            mNoseGear = value;
         }
         else if (localCommand == "braking_control_surface_name")
         {
            std::string value;
            aInput.ReadValue(value);
            mBrakeControlName = value;
         }
         else if (localCommand == "gear_extended_relative_position_x")
         {
            double length_m = 0.0;
            aInput.ReadValueOfType(length_m, UtInput::cLENGTH);
            double vec[3];
            mGearPos_m.Get(vec);
            vec[0] = length_m;
            mGearPos_m.Set(vec);
         }
         else if (localCommand == "gear_extended_relative_position_y")
         {
            double length_m = 0.0;
            aInput.ReadValueOfType(length_m, UtInput::cLENGTH);
            double vec[3];
            mGearPos_m.Get(vec);
            vec[1] = length_m;
            mGearPos_m.Set(vec);
         }
         else if (localCommand == "gear_extended_relative_position_z")
         {
            double length_m = 0.0;
            aInput.ReadValueOfType(length_m, UtInput::cLENGTH);
            double vec[3];
            mGearPos_m.Get(vec);
            vec[2] = length_m;
            mGearPos_m.Set(vec);
         }
         else if (localCommand == "gear_compression_vector_x")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            double vec[3];
            mGearCompressionVec.Get(vec);
            vec[0] = value;
            mGearCompressionVec.Set(vec);
         }
         else if (localCommand == "gear_compression_vector_y")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            double vec[3];
            mGearCompressionVec.Get(vec);
            vec[1] = value;
            mGearCompressionVec.Set(vec);
         }
         else if (localCommand == "gear_compression_vector_z")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            double vec[3];
            mGearCompressionVec.Get(vec);
            vec[2] = value;
            mGearCompressionVec.Set(vec);
         }
         else if (localCommand == "gear_rolling_vector_x")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            double vec[3];
            mGearRollingVec.Get(vec);
            vec[0] = value;
            mGearRollingVec.Set(vec);
         }
         else if (localCommand == "gear_rolling_vector_y")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            double vec[3];
            mGearRollingVec.Get(vec);
            vec[1] = value;
            mGearRollingVec.Set(vec);
         }
         else if (localCommand == "gear_rolling_vector_z")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            double vec[3];
            mGearRollingVec.Get(vec);
            vec[2] = value;
            mGearRollingVec.Set(vec);
         }
         else if (localCommand == "spring_constant_lbs_per_ft")
         {
            double k_ftlbs = 0.0;
            aInput.ReadValue(k_ftlbs);
            mSpringConstant_lbs_per_ft = k_ftlbs;
         }
         else if (localCommand == "damper_constant_lbs_per_fps")
         {
            double k_lbs_per_fps = 0.0;
            aInput.ReadValue(k_lbs_per_fps);
            mDamperConstant_lbs_per_fps = k_lbs_per_fps;
         }
         else if (localCommand == "uncompressed_length")
         {
            double length_m = 0.0;
            aInput.ReadValueOfType(length_m, UtInput::cLENGTH);
            mUncompressedLength_ft = length_m * UtMath::cFT_PER_M;
         }
         else if (localCommand == "max_compression")
         {
            double length_m = 0.0;
            aInput.ReadValueOfType(length_m, UtInput::cLENGTH);
            mMaximumCompression_ft = length_m * UtMath::cFT_PER_M;
         }
         else if (localCommand == "rolling_coefficient_of_friction")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mRollingFrictionCoefficient = value;
         }
         else if (localCommand == "braking_coefficient_of_friction")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mBrakingFrictionCoefficient = value;
         }
         else if (localCommand == "scuffing_coefficient_of_friction")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mScuffingFrictionCoefficient = value;
         }
         else if (localCommand == "ignore_friction")
         {
            bool value = false;
            aInput.ReadValue(value);
            mUseFriction = !value;
         }
         else if (localCommand == "is_landing_gear")
         {
            bool value = false;
            aInput.ReadValue(value);
            mIsLandingGear = value;
         }
         else if (localCommand == "is_contact_point")
         {
            bool value = false;
            aInput.ReadValue(value);
            mIsLandingGear = !value; // reverse the answer
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool wsf::six_dof::RigidBodyGroundReactionPoint::Initialize()
{
   if (mParentVehicle == nullptr)
   {
      return false;
   }

   RigidBodyFlightControlSystem* fcs = utils::CastToRigidBody(mParentVehicle)->GetFlightControls();

   mControlSurfaceHandle = fcs->GetControlSurfaceHandle(mControlSurfaceName);

   if (!mSteeringControlSurfaceName.empty())
   {
      mSteeringControlSurfaceHandle = fcs->GetControlSurfaceHandle(mSteeringControlSurfaceName);
   }

   if (!mNWSAngleControlSurfaceName.empty())
   {
      mNWSAngleControlSurfaceHandle = fcs->GetControlSurfaceHandle(mNWSAngleControlSurfaceName);
   }

   if (!mBrakeControlName.empty())
   {
      mBrakeControlHandle = fcs->GetControlValueHandle(mBrakeControlName);
   }

   return true;
}

size_t wsf::six_dof::RigidBodyGroundReactionPoint::GetBrakingControlHandle() const
{
   return mBrakeControlHandle;
}

void wsf::six_dof::RigidBodyGroundReactionPoint::SetBraking(double aNormalizedValue)
{
   mNormalizedBrakingValue = aNormalizedValue;
}

double wsf::six_dof::RigidBodyGroundReactionPoint::GetBraking() const
{
   return mNormalizedBrakingValue;
}

size_t wsf::six_dof::RigidBodyGroundReactionPoint::GetSteeringControlHandle() const
{
   if (mNWS_Engaged)
   {
      return mNWSAngleControlSurfaceHandle;
   }

   return mSteeringControlSurfaceHandle;
}

void wsf::six_dof::RigidBodyGroundReactionPoint::SetSteeringAngle_deg(double aSteeringAngle_deg)
{
   mSteeringAngle_deg = aSteeringAngle_deg;
}

double wsf::six_dof::RigidBodyGroundReactionPoint::GetSteeringAngle_deg() const
{
   return mSteeringAngle_deg;
}

bool wsf::six_dof::RigidBodyGroundReactionPoint::CalcNormalForceMoment(UtVec3dX& aForce_lbs,
                                                                       UtVec3dX& aMoment_ftlbs,
                                                                       bool&     aMaxCompressionExceeded,
                                                                       double    aDeltaT_sec)
{
   // If we lack a parent, we can't really do anything
   if (mParentVehicle == nullptr)
   {
      return SetDataWhenNotInContactWithGround(aForce_lbs, aMoment_ftlbs, aMaxCompressionExceeded);
   }

   // Check for a tiny delta t
   if (aDeltaT_sec < utils::cEPSILON_SIMTIME_SEC) // if less than epsilon, do nothing new
   {
      return SetDataWhenTinyDeltaT(aForce_lbs, aMoment_ftlbs, aMaxCompressionExceeded);
   }

   double   lat                  = 0.0;
   double   lon                  = 0.0;
   double   alt_m                = 0.0;
   double   heightAboveTerrain_m = 0.0;
   UtVec3dX compressionVectorNED;
   UtVec3dX rollingVectorInSurfaceNED;
   UtVec3dX normalVectorNED;
   UtVec3dX surfaceSpeedVectorNED_mps;
   UtVec3dX currentGearVel_NED_mps;
   double   frictionMultiplier = 0.0;
   double   compression_m      = 0.0;

   double vehicleLatitude  = mParentVehicle->GetKinematicState()->GetLat();
   double vehicleLongitude = mParentVehicle->GetKinematicState()->GetLon();

   double terrainElevation_m = mParentVehicle->GetEnvironment()->GetTerrainElevation(vehicleLatitude, vehicleLongitude);

   bool terrainDataValid = KinematicState::CalculateLLAofRelativePoint(terrainElevation_m,
                                                                       mParentVehicle->GetKinematicState(),
                                                                       mGearPos_m,
                                                                       mGearCompressionVec,
                                                                       mGearRollingVec,
                                                                       currentGearVel_NED_mps,
                                                                       lat,
                                                                       lon,
                                                                       alt_m,
                                                                       heightAboveTerrain_m,
                                                                       compressionVectorNED,
                                                                       rollingVectorInSurfaceNED,
                                                                       normalVectorNED,
                                                                       surfaceSpeedVectorNED_mps,
                                                                       frictionMultiplier,
                                                                       compression_m);
   if (!terrainDataValid)
   {
      // Clear things
      return SetDataWhenNotInContactWithGround(aForce_lbs, aMoment_ftlbs, aMaxCompressionExceeded);
   }

   // Set the HAT
   mHeightAboveTerrain_ft = heightAboveTerrain_m * UtMath::cFT_PER_M;

   // If this reaction point is a landing gear, check if valid (down and locked). Contact points are always valid.
   if (mIsLandingGear)
   {
      if (mControlSurfaceHandle)
      {
         double normalizedValue =
            utils::CastToRigidBody(mParentVehicle)->GetFlightControls()->GetControlSurfaceValueNormalized(mControlSurfaceHandle);
         if (normalizedValue > 0.99)
         {
            mIsValid = true;
         }
         else
         {
            mIsValid = false;
         }
      }
      else
      {
         mIsValid = false;
      }
   }
   else
   {
      mIsValid = true;
   }

   // If not valid, clear things since we're not in contact with ground
   if (!mIsValid)
   {
      return SetDataWhenNotInContactWithGround(aForce_lbs, aMoment_ftlbs, aMaxCompressionExceeded);
   }

   // At this point, everything is valid so proceed with calculations

   // Check for contact with ground. We consider any compression less than a micrometer
   // as being not in contact. (This avoids problems with trying to be exactly zero.)
   if (compression_m <= 0.000001)
   {
      // Clear things
      return SetDataWhenNotInContactWithGround(aForce_lbs, aMoment_ftlbs, aMaxCompressionExceeded);
   }

   // At this point, we know that the reaction point is in contact with the ground so we can
   // proceed to calculating the normal force

   // Set current values
   mInContactWithGround        = true;
   mCurrentCompression_ft      = compression_m * UtMath::cFT_PER_M;
   mCurrentCompressionRate_fps = (mCurrentCompression_ft - mLastCompression_ft) / aDeltaT_sec;

   // Check for max compression
   if (mCurrentCompression_ft > mMaximumCompression_ft)
   {
      // Hit max compression!

      // Limit the current compression to the maximum
      mCurrentCompression_ft = mMaximumCompression_ft;

      // Set exceeded flag
      mMaxCompressionExceeded = true;
   }

   // Calc normal forces (frictional forces will be calculated later)
   double springForceMagnitude_lbs      = mCurrentCompression_ft * mSpringConstant_lbs_per_ft;
   double dampingForceMagnitude_lbs     = mCurrentCompressionRate_fps * mDamperConstant_lbs_per_fps;
   double totalNormalForceMagnitude_lbs = springForceMagnitude_lbs + dampingForceMagnitude_lbs;

   // Now, calculate the NED vector for the normal force
   UtVec3dX currentNormalForceVecNED_lbs = normalVectorNED * totalNormalForceMagnitude_lbs;

   // Convert to body coords
   mCurrentNormalForceBody_lbs.Set(0, 0, 0);
   mCurrentNormalForceBody_lbs = mParentVehicle->GetKinematicState()->CalcBodyVecFromNEDVec(currentNormalForceVecNED_lbs);

   // At this point, we have the normal force vector and can proceed to calculating the friction

   mLastNormalForceMagnitude_lbs = mCurrentNormalForceBody_lbs.Magnitude();

   mCurrentForce_lbs       = mCurrentNormalForceBody_lbs;
   UtVec3dX gearPosVec_ft  = mGearPos_m * UtMath::cFT_PER_M;
   mCurrentMoment_ftlbs    = gearPosVec_ft.Cross(mCurrentForce_lbs);
   mCurrentGearVel_NED_mps = currentGearVel_NED_mps;

   // Set 'last' values
   mLastCompression_ft      = mCurrentCompression_ft;
   mLastCompressionRate_fps = mCurrentCompressionRate_fps;

   // Return current conditions
   aForce_lbs              = mCurrentForce_lbs;
   aMoment_ftlbs           = mCurrentMoment_ftlbs;
   aMaxCompressionExceeded = mMaxCompressionExceeded;

   return mInContactWithGround;
}

double wsf::six_dof::RigidBodyGroundReactionPoint::CalcMaxFrictionForceMagnitude_lbs(
   const UtVec3dX& aDesiredForceBasedFrictionDirectionInSurfaceVecNED)
{
   double   staticMuFactor       = 1.5; // We use static friction since we're not moving
   double   lat                  = 0.0;
   double   lon                  = 0.0;
   double   alt_m                = 0.0;
   double   heightAboveTerrain_m = 0.0;
   UtVec3dX compressionVectorNED;
   UtVec3dX rollingVectorInSurfaceNED;
   UtVec3dX normalVectorNED;
   UtVec3dX surfaceSpeedVectorNED_mps;
   UtVec3dX currentGearVel_NED_mps;
   double   frictionMultiplier = 0.0;
   double   compression_m      = 0.0;

   double vehicleLatitude  = mParentVehicle->GetKinematicState()->GetLat();
   double vehicleLongitude = mParentVehicle->GetKinematicState()->GetLon();

   double terrainElevation_m = mParentVehicle->GetEnvironment()->GetTerrainElevation(vehicleLatitude, vehicleLongitude);

   bool terrainDataValid = KinematicState::CalculateLLAofRelativePoint(terrainElevation_m,
                                                                       mParentVehicle->GetKinematicState(),
                                                                       mGearPos_m,
                                                                       mGearCompressionVec,
                                                                       mGearRollingVec,
                                                                       currentGearVel_NED_mps,
                                                                       lat,
                                                                       lon,
                                                                       alt_m,
                                                                       heightAboveTerrain_m,
                                                                       compressionVectorNED,
                                                                       rollingVectorInSurfaceNED,
                                                                       normalVectorNED,
                                                                       surfaceSpeedVectorNED_mps,
                                                                       frictionMultiplier,
                                                                       compression_m);
   if (!terrainDataValid)
   {
      return 0.0;
   }

   UtVec3dX unitSideVecNED = rollingVectorInSurfaceNED.Cross(normalVectorNED);
   unitSideVecNED.Normalize();

   double steeringAng_rad = mSteeringAngle_deg * UtMath::cRAD_PER_DEG;

   UtVec3dX xVec = rollingVectorInSurfaceNED * cos(steeringAng_rad);
   UtVec3dX yVec = unitSideVecNED * sin(steeringAng_rad);

   mCurrentGearRollingVecNED = xVec + yVec;
   mCurrentGearRollingVecNED.Normalize();
   UtVec3dX unitRollingVecNED = mCurrentGearRollingVecNED;

   // Calc new side vec
   unitSideVecNED = mCurrentGearRollingVecNED.Cross(normalVectorNED);
   unitSideVecNED.Normalize();

   // Rolling friction
   double   dotRolling    = unitRollingVecNED.Dot(aDesiredForceBasedFrictionDirectionInSurfaceVecNED);
   UtVec3dX rollingVecNED = unitRollingVecNED * dotRolling;

   // Scuffing friction
   double   dotScuffing    = unitSideVecNED.Dot(aDesiredForceBasedFrictionDirectionInSurfaceVecNED);
   UtVec3dX scuffingVecNED = unitSideVecNED * dotScuffing;

   // Normal force magnitude
   double normalForceMagnitude_lbs = mLastNormalForceMagnitude_lbs;

   // Friction magnitudes
   double maxRollingFrictionMagnitude_lbs = normalForceMagnitude_lbs * mRollingFrictionCoefficient * staticMuFactor;
   double maxBrakingFrictionMagnitude_lbs =
      normalForceMagnitude_lbs * (mBrakingFrictionCoefficient * mNormalizedBrakingValue) * staticMuFactor;
   double maxCombinedRollingFrictionMagnitude_lbs = maxRollingFrictionMagnitude_lbs + maxBrakingFrictionMagnitude_lbs;
   double maxScuffingMagnitudeFriction_lbs        = normalForceMagnitude_lbs * mScuffingFrictionCoefficient;

   // Rolling/braking vector
   UtVec3dX maxRollingForceVecNED_lbs = rollingVecNED * maxCombinedRollingFrictionMagnitude_lbs;

   // Scuffing vector
   UtVec3dX maxScuffingForceVecNED_lbs = scuffingVecNED * maxScuffingMagnitudeFriction_lbs;

   // Combined friction force vector
   UtVec3dX maxCurrentFrictionForceVecNED_lbs = maxRollingForceVecNED_lbs + maxScuffingForceVecNED_lbs;

   // Get the direction of the max friction
   UtVec3dX maxFrictionDirectionNED = maxCurrentFrictionForceVecNED_lbs;
   maxFrictionDirectionNED.Normalize();

   // Get the vector in the friction direction
   double   dotTotal              = maxFrictionDirectionNED.Dot(aDesiredForceBasedFrictionDirectionInSurfaceVecNED);
   UtVec3dX maxFrictionVecNED_lbs = maxCurrentFrictionForceVecNED_lbs * dotTotal;

   return maxFrictionVecNED_lbs.Magnitude();
}

void wsf::six_dof::RigidBodyGroundReactionPoint::CalcFrictionForceMomentWhileInMotion(UtVec3dX& aForce_lbs,
                                                                                      UtVec3dX& aMoment_ftlbs)
{
   UtVec3dX frictionDirectionInSurfaceNED(0., 0., 0.);
   CalcFrictionForceMoment(aForce_lbs, aMoment_ftlbs, frictionDirectionInSurfaceNED, false);
}

void wsf::six_dof::RigidBodyGroundReactionPoint::CalcFrictionForceMomentStartingToMove(UtVec3dX& aForce_lbs,
                                                                                       UtVec3dX& aMoment_ftlbs,
                                                                                       const UtVec3dX& aFrictionDirectionInSurfaceNED)
{
   CalcFrictionForceMoment(aForce_lbs, aMoment_ftlbs, aFrictionDirectionInSurfaceNED, true);
}

void wsf::six_dof::RigidBodyGroundReactionPoint::CalcFrictionForceMoment(UtVec3dX&       aForce_lbs,
                                                                         UtVec3dX&       aMoment_ftlbs,
                                                                         const UtVec3dX& aFrictionDirectionInSurfaceNED,
                                                                         bool            aUseForceBasedFriction)
{
   double staticMuFactor = 1.0; // We are now moving
   // double   scuffStaticMuFactor = 1.5; // We assume scuffing not skidding
   double   lat                  = 0.0;
   double   lon                  = 0.0;
   double   alt_m                = 0.0;
   double   heightAboveTerrain_m = 0.0;
   UtVec3dX compressionVectorNED;
   UtVec3dX rollingVectorInSurfaceNED;
   UtVec3dX normalVectorNED;
   UtVec3dX surfaceSpeedVectorNED_mps;
   UtVec3dX currentGearVel_NED_mps;
   double   frictionMultiplier = 0.0;
   double   compression_m      = 0.0;

   double vehicleLatitude  = mParentVehicle->GetKinematicState()->GetLat();
   double vehicleLongitude = mParentVehicle->GetKinematicState()->GetLon();

   double terrainElevation_m = mParentVehicle->GetEnvironment()->GetTerrainElevation(vehicleLatitude, vehicleLongitude);

   bool terrainDataValid = KinematicState::CalculateLLAofRelativePoint(terrainElevation_m,
                                                                       mParentVehicle->GetKinematicState(),
                                                                       mGearPos_m,
                                                                       mGearCompressionVec,
                                                                       mGearRollingVec,
                                                                       currentGearVel_NED_mps,
                                                                       lat,
                                                                       lon,
                                                                       alt_m,
                                                                       heightAboveTerrain_m,
                                                                       compressionVectorNED,
                                                                       rollingVectorInSurfaceNED,
                                                                       normalVectorNED,
                                                                       surfaceSpeedVectorNED_mps,
                                                                       frictionMultiplier,
                                                                       compression_m);
   if (!terrainDataValid)
   {
      return;
   }

   UtVec3dX unitSideVecNED = rollingVectorInSurfaceNED.Cross(normalVectorNED);
   unitSideVecNED.Normalize();

   double steeringAng_rad = mSteeringAngle_deg * UtMath::cRAD_PER_DEG;

   UtVec3dX xVec = rollingVectorInSurfaceNED * cos(steeringAng_rad);
   UtVec3dX yVec = unitSideVecNED * sin(steeringAng_rad);

   mCurrentGearRollingVecNED = xVec + yVec;
   mCurrentGearRollingVecNED.Normalize();
   UtVec3dX unitRollingVecNED = mCurrentGearRollingVecNED;

   // Calc new side vec
   unitSideVecNED = mCurrentGearRollingVecNED.Cross(normalVectorNED);
   unitSideVecNED.Normalize();

   // Determine friction direction
   UtVec3dX frictionDirectionInSurfaceNED;
   if (aUseForceBasedFriction)
   {
      frictionDirectionInSurfaceNED = aFrictionDirectionInSurfaceNED;
   }
   else
   {
      // Calc the vel of the gear relative to the surface
      UtVec3dX netGearVel_NED_mps = currentGearVel_NED_mps - surfaceSpeedVectorNED_mps;

      // The friction opposes the motion
      frictionDirectionInSurfaceNED = netGearVel_NED_mps * -1.0;

      // Now, we have the vel vector, but it is not yet in the surface plane. We now get the
      // cross product of the vel vector and the surface normal
      UtVec3dX vectorInSurfaceNED_fps = frictionDirectionInSurfaceNED.Cross(normalVectorNED);

      // Next, we get the cross product of the surface normal and vectorInSurfaceNED_mps which
      // gives us our desired vel vector in the surface plane.
      frictionDirectionInSurfaceNED = normalVectorNED.Cross(vectorInSurfaceNED_fps);
      frictionDirectionInSurfaceNED.Normalize();
   }

   // Rolling friction
   double   dotRolling    = unitRollingVecNED.Dot(frictionDirectionInSurfaceNED);
   UtVec3dX rollingVecNED = unitRollingVecNED * dotRolling;

   // Scuffing friction
   double   dotScuffing    = unitSideVecNED.Dot(frictionDirectionInSurfaceNED);
   UtVec3dX scuffingVecNED = unitSideVecNED * dotScuffing;

   // Normal force magnitude
   double normalForceMagnitude_lbs = mLastNormalForceMagnitude_lbs;

   // Friction magnitudes
   double maxRollingFrictionMagnitude_lbs = normalForceMagnitude_lbs * mRollingFrictionCoefficient * staticMuFactor;
   double maxBrakingFrictionMagnitude_lbs =
      normalForceMagnitude_lbs * (mBrakingFrictionCoefficient * mNormalizedBrakingValue) * staticMuFactor;
   double maxCombinedRollingFrictionMagnitude_lbs = maxRollingFrictionMagnitude_lbs + maxBrakingFrictionMagnitude_lbs;
   double maxScuffingMagnitudeFriction_lbs        = normalForceMagnitude_lbs * mScuffingFrictionCoefficient;

   // Rolling/braking vector
   UtVec3dX rollingForceVecNED_lbs = rollingVecNED * maxCombinedRollingFrictionMagnitude_lbs;

   // Scuffing vector
   UtVec3dX scuffingForceVecNED_lbs = scuffingVecNED * maxScuffingMagnitudeFriction_lbs;

   // Combined friction force vector
   UtVec3dX currentFrictionForceVecNED_lbs = rollingForceVecNED_lbs + scuffingForceVecNED_lbs;

   // Convert to body coords
   mCurrentFrictionForceBody_lbs.Set(0, 0, 0);
   mCurrentFrictionForceBody_lbs =
      mParentVehicle->GetKinematicState()->CalcBodyVecFromNEDVec(currentFrictionForceVecNED_lbs);

   // Common data
   mCurrentForce_lbs      = mCurrentNormalForceBody_lbs + mCurrentFrictionForceBody_lbs;
   UtVec3dX gearPosVec_ft = mGearPos_m * UtMath::cFT_PER_M;
   mCurrentMoment_ftlbs   = gearPosVec_ft.Cross(mCurrentForce_lbs);

   // Return current conditions
   aForce_lbs    = mCurrentFrictionForceBody_lbs;
   aMoment_ftlbs = gearPosVec_ft.Cross(mCurrentFrictionForceBody_lbs);
   ;
}

// --------------------------------------------------------------------------------------------------

bool wsf::six_dof::RigidBodyGroundReactionPoint::CalcGroundReactionForceMoment(UtVec3dX&       aForce_lbs,
                                                                               UtVec3dX&       aMoment_ftlbs,
                                                                               bool&           aMaxCompressionExceeded,
                                                                               double          aDeltaT_sec,
                                                                               const UtVec3dX& aAdditiveVelocityNED_fps)
{
   // If we lack a parent, we can't really do anything
   if (mParentVehicle == nullptr)
   {
      return SetDataWhenNotInContactWithGround(aForce_lbs, aMoment_ftlbs, aMaxCompressionExceeded);
   }

   // Check for a tiny delta t
   if (aDeltaT_sec < utils::cEPSILON_SIMTIME_SEC) // if less than epsilon, do nothing new
   {
      return SetDataWhenTinyDeltaT(aForce_lbs, aMoment_ftlbs, aMaxCompressionExceeded);
   }

   double   lat                  = 0.0;
   double   lon                  = 0.0;
   double   alt_m                = 0.0;
   double   heightAboveTerrain_m = 0.0;
   UtVec3dX compressionVectorNED;
   UtVec3dX rollingVectorInSurfaceNED;
   UtVec3dX normalVectorNED;
   UtVec3dX surfaceSpeedVectorNED_mps;
   UtVec3dX currentGearVel_NED_mps;
   double   frictionMultiplier = 0.0;
   double   compression_m      = 0.0;

   double vehicleLatitude  = mParentVehicle->GetKinematicState()->GetLat();
   double vehicleLongitude = mParentVehicle->GetKinematicState()->GetLon();

   double terrainElevation_m = mParentVehicle->GetEnvironment()->GetTerrainElevation(vehicleLatitude, vehicleLongitude);

   bool terrainDataValid = KinematicState::CalculateLLAofRelativePoint(terrainElevation_m,
                                                                       mParentVehicle->GetKinematicState(),
                                                                       mGearPos_m,
                                                                       mGearCompressionVec,
                                                                       mGearRollingVec,
                                                                       currentGearVel_NED_mps,
                                                                       lat,
                                                                       lon,
                                                                       alt_m,
                                                                       heightAboveTerrain_m,
                                                                       compressionVectorNED,
                                                                       rollingVectorInSurfaceNED,
                                                                       normalVectorNED,
                                                                       surfaceSpeedVectorNED_mps,
                                                                       frictionMultiplier,
                                                                       compression_m);
   if (!terrainDataValid)
   {
      // Clear things
      return SetDataWhenNotInContactWithGround(aForce_lbs, aMoment_ftlbs, aMaxCompressionExceeded);
   }

   // Set the HAT
   mHeightAboveTerrain_ft = heightAboveTerrain_m * UtMath::cFT_PER_M;

   // If this reaction point is a landing gear, check if valid (down and locked). Contact points are always valid.
   if (mIsLandingGear)
   {
      if (mControlSurfaceHandle)
      {
         double normalizedValue =
            utils::CastToRigidBody(mParentVehicle)->GetFlightControls()->GetControlSurfaceValueNormalized(mControlSurfaceHandle);
         if (normalizedValue > 0.99)
         {
            mIsValid = true;
         }
         else
         {
            mIsValid = false;
         }
      }
      else
      {
         mIsValid = false;
      }
   }
   else
   {
      mIsValid = true;
   }

   // If not valid, clear things since we're not in contact with ground
   if (!mIsValid)
   {
      return SetDataWhenNotInContactWithGround(aForce_lbs, aMoment_ftlbs, aMaxCompressionExceeded);
   }

   // At this point, everything is valid so proceed with calculations

   // Check for contact with ground. We consider any compression less than a micrometer
   // as being not in contact. (This avoids problems with trying to be exactly zero.)
   if (compression_m <= 0.000001)
   {
      // Clear things
      return SetDataWhenNotInContactWithGround(aForce_lbs, aMoment_ftlbs, aMaxCompressionExceeded);
   }

   // At this point, we know that the reaction point is in contact with the ground so we can
   // proceed to calculating the normal force
   // ---------------------------------------------------------------------------------------------

   // Set current values
   mInContactWithGround        = true;
   mCurrentCompression_ft      = compression_m * UtMath::cFT_PER_M;
   mCurrentCompressionRate_fps = (mCurrentCompression_ft - mLastCompression_ft) / aDeltaT_sec;

   // Check for max compression
   if (mCurrentCompression_ft > mMaximumCompression_ft)
   {
      // Hit max compression!

      // Limit the current compression to the maximum
      mCurrentCompression_ft = mMaximumCompression_ft;

      // Set exceeded flag
      mMaxCompressionExceeded = true;
   }

   // Calc normal forces (frictional forces will be calculated later)
   double springForceMagnitude_lbs      = mCurrentCompression_ft * mSpringConstant_lbs_per_ft;
   double dampingForceMagnitude_lbs     = mCurrentCompressionRate_fps * mDamperConstant_lbs_per_fps;
   double totalNormalForceMagnitude_lbs = springForceMagnitude_lbs + dampingForceMagnitude_lbs;

   // Now, calculate the NED vector for the normal force
   UtVec3dX currentNormalForceVecNED_lbs = normalVectorNED * totalNormalForceMagnitude_lbs;

   // Convert to body coords
   mCurrentNormalForceBody_lbs.Set(0, 0, 0);
   mCurrentNormalForceBody_lbs = mParentVehicle->GetKinematicState()->CalcBodyVecFromNEDVec(currentNormalForceVecNED_lbs);

   // At this point, we have the normal force vector and can proceed to calculating the friction

   // Zero data before setting it
   mCurrentFrictionForceBody_lbs.Set(0, 0, 0);
   mCurrentForce_lbs.Set(0, 0, 0);
   mCurrentMoment_ftlbs.Set(0, 0, 0);

   // Calc the vel of the gear relative to the surface
   UtVec3dX netGearVel_NED_mps = currentGearVel_NED_mps - surfaceSpeedVectorNED_mps;

   // Set speed in ft/sec
   UtVec3dX netGearVel_NED_fps = netGearVel_NED_mps * UtMath::cFT_PER_M;

   // Add in the "additive" velocity
   netGearVel_NED_fps += aAdditiveVelocityNED_fps;

   // Now, we have the vel vector, but it is not yet in the surface plane. We now get the
   // cross product of the vel vector and the surface normal
   UtVec3dX vectorInSurfaceNED_fps = netGearVel_NED_fps.Cross(normalVectorNED);

   // Next, we get the cross product of the surface normal and vectorInSurfaceNED_mps which
   // gives us our desired vel vector in the surface plane.
   UtVec3dX netGearVelInSurfaceNED_fps = normalVectorNED.Cross(vectorInSurfaceNED_fps);

   // Get the speed of gear (magnitude of velocity)
   double gearVelInSurfaceNEDMagnitude_fps = netGearVelInSurfaceNED_fps.Magnitude();

   // Check for 'static' friction (when not moving)
   double staticMuFactor        = 1.0;
   double velocityThreshold_fps = 1.0;
   bool   atRest                = false;

   // Determine if we are moving slowly enough to be considered at rest
   if (gearVelInSurfaceNEDMagnitude_fps < velocityThreshold_fps)
   {
      atRest = true;
   }

   // If we are at rest, increase the static friction multiplier, since typical
   // static friction is approx 1.5x dynamic friction
   if (atRest)
   {
      staticMuFactor = 1.5;
   }

   // Clear current friction to zero

   if (mIsLandingGear)
   {
      UtVec3dX unitSideVecNED = rollingVectorInSurfaceNED.Cross(normalVectorNED);
      unitSideVecNED.Normalize();

      double steeringAng_rad = mSteeringAngle_deg * UtMath::cRAD_PER_DEG;

      UtVec3dX xVec = rollingVectorInSurfaceNED * cos(steeringAng_rad);
      UtVec3dX yVec = unitSideVecNED * sin(steeringAng_rad);

      mCurrentGearRollingVecNED = xVec + yVec;
      mCurrentGearRollingVecNED.Normalize();
      UtVec3dX unitRollingVecNED = mCurrentGearRollingVecNED;

      // Calc new side vec
      unitSideVecNED = mCurrentGearRollingVecNED.Cross(normalVectorNED);
      unitSideVecNED.Normalize();

      // Normalized speed vector
      UtVec3dX unitSpeedVecInSurfaceNED = netGearVelInSurfaceNED_fps;
      unitSpeedVecInSurfaceNED.Normalize();

      // Velocity components in rolling and scuffing directions

      // Reverse direction since friction opposes motion
      UtVec3dX frictionDirectionInSurfaceNEDVec = unitSpeedVecInSurfaceNED * -1.0;

      // Rolling friction
      double   dotRolling    = unitRollingVecNED.Dot(frictionDirectionInSurfaceNEDVec);
      UtVec3dX rollingVecNED = unitRollingVecNED * dotRolling;

      // Scuffing friction
      double   dotScuffing    = unitSideVecNED.Dot(frictionDirectionInSurfaceNEDVec);
      UtVec3dX scuffingVecNED = unitSideVecNED * dotScuffing;

      // Normal force magnitude (we can use body coords, since we just want magnitude)
      double normalForceMagnitude_lbs = mCurrentNormalForceBody_lbs.Magnitude();

      // Friction
      double maxRollingFrictionMagnitude_lbs = normalForceMagnitude_lbs * mRollingFrictionCoefficient * staticMuFactor;
      double maxBrakingFrictionMagnitude_lbs =
         normalForceMagnitude_lbs * (mBrakingFrictionCoefficient * mNormalizedBrakingValue) * staticMuFactor;
      double maxCombinedRollingFrictionMagnitude_lbs = maxRollingFrictionMagnitude_lbs + maxBrakingFrictionMagnitude_lbs;

      // Scuffing
      double maxScuffingMagnitudeFriction_lbs = normalForceMagnitude_lbs * mScuffingFrictionCoefficient;

      // Calc the force needed to reduce velocity to zero during this time step
      UtVec3dX velocityVecToOpposeNED_fps = netGearVelInSurfaceNED_fps * -1.0;
      UtVec3dX accelVecToOpposeNED_fps2   = velocityVecToOpposeNED_fps / aDeltaT_sec;
      double   wgt_lbs                    = normalForceMagnitude_lbs;
      UtVec3dX frictionVecNeededNED_lbs   = accelVecToOpposeNED_fps2 * (wgt_lbs / 32.0);

      // Amount needed in rolling direction
      UtVec3dX netRollingFrictionVecNED_lbs(0., 0., 0.);
      double   amountInRollingDirection_lbs = rollingVecNED.Dot(frictionVecNeededNED_lbs);
      if (fabs(amountInRollingDirection_lbs) < maxCombinedRollingFrictionMagnitude_lbs)
      {
         netRollingFrictionVecNED_lbs = rollingVecNED * amountInRollingDirection_lbs;
      }
      else
      {
         netRollingFrictionVecNED_lbs =
            rollingVecNED *
            (maxCombinedRollingFrictionMagnitude_lbs * amountInRollingDirection_lbs / fabs(amountInRollingDirection_lbs));
      }

      // Amount needed in scuffing direction
      UtVec3dX netScuffingFrictionVecNED_lbs(0., 0., 0.);
      double   amountInScuffingDirection_lbs = scuffingVecNED.Dot(frictionVecNeededNED_lbs);
      if (fabs(amountInScuffingDirection_lbs) < maxScuffingMagnitudeFriction_lbs)
      {
         netScuffingFrictionVecNED_lbs = scuffingVecNED * amountInScuffingDirection_lbs;
      }
      else
      {
         netScuffingFrictionVecNED_lbs =
            scuffingVecNED *
            (maxScuffingMagnitudeFriction_lbs * amountInScuffingDirection_lbs / fabs(amountInScuffingDirection_lbs));
      }

      // Total friction
      UtVec3dX currentFrictionForceVecNED_lbs = netRollingFrictionVecNED_lbs + netScuffingFrictionVecNED_lbs;

      // Determine the friction force needed in each direction (rolling and scuffing)

      // We set the not moving flag to false and then see if it should be true
      mNotMoving = false;
      if (atRest)
      {
         // if( combinedRollingFrictionMagnitude_lbs > frictionMagnitudeNeededNED_lbs )
         // {
         //    // The friction *should* be able to keep the body at rest
         //
         //    // First, calculate the friction vector needed
         //    UtVec3dX frictionVecNeededNED_lbs = directionToOpposeNED * frictionMagnitudeNeededNED_lbs;
         //
         //    // currentFrictionForceVecNED_lbs is the calculated friction, but we need to limit it
         //
         //    // For now, assume that we meet the desired friction
         //    currentFrictionForceVecNED_lbs = frictionVecNeededNED_lbs;
         // }

         mNotMoving = true;
      }

      // Transform friction to body coords
      UtVec3dX frictionForceBody_lbs(0., 0., 0.);
      frictionForceBody_lbs = mParentVehicle->GetKinematicState()->CalcBodyVecFromNEDVec(currentFrictionForceVecNED_lbs);

      // This is in body coords
      mCurrentFrictionForceBody_lbs = frictionForceBody_lbs;
   }

   mCurrentForce_lbs       = mCurrentNormalForceBody_lbs + mCurrentFrictionForceBody_lbs;
   UtVec3dX gearPosVec_ft  = mGearPos_m * UtMath::cFT_PER_M;
   mCurrentMoment_ftlbs    = gearPosVec_ft.Cross(mCurrentForce_lbs);
   mCurrentGearVel_NED_mps = currentGearVel_NED_mps;

   // Set 'last' values
   mLastCompression_ft      = mCurrentCompression_ft;
   mLastCompressionRate_fps = mCurrentCompressionRate_fps;

   // Return current conditions
   aForce_lbs              = mCurrentForce_lbs;
   aMoment_ftlbs           = mCurrentMoment_ftlbs;
   aMaxCompressionExceeded = mMaxCompressionExceeded;

   return mInContactWithGround;
}

bool wsf::six_dof::RigidBodyGroundReactionPoint::NotMoving() const
{
   return mNotMoving;
}

void wsf::six_dof::RigidBodyGroundReactionPoint::GetData(UtVec3dX& aGearPos_m,
                                                         UtVec3dX& aGearCompressionVec,
                                                         UtVec3dX& aCurrentGearRollingVec,
                                                         UtVec3dX& aCurrentForce_lbs,
                                                         UtVec3dX& aCurrentMoment_ftlbs,
                                                         UtVec3dX& aCurrentNormalForce_lbs,
                                                         UtVec3dX& aCurrentFrictionForce_lbs,
                                                         double&   aUncompressedLength_ft,
                                                         double&   aMaximumCompression_ft,
                                                         double&   aCurrentCompression_ft,
                                                         double&   aNormalizedBrakingValue,
                                                         double&   aSteeringAngle_deg,
                                                         double&   aHeightAboveTerrain_ft,
                                                         bool&     aIsLandingGear,
                                                         bool&     aInContactWithGround,
                                                         bool&     aMaxCompressionExceeded)
{
   aGearPos_m                = mGearPos_m;
   aGearCompressionVec       = mGearCompressionVec;
   aCurrentGearRollingVec    = mCurrentGearRollingVecNED;
   aCurrentForce_lbs         = mCurrentForce_lbs;
   aCurrentMoment_ftlbs      = mCurrentMoment_ftlbs;
   aCurrentNormalForce_lbs   = mCurrentNormalForceBody_lbs;
   aCurrentFrictionForce_lbs = mCurrentFrictionForceBody_lbs;
   aUncompressedLength_ft    = mUncompressedLength_ft;
   aMaximumCompression_ft    = mMaximumCompression_ft;
   aCurrentCompression_ft    = mCurrentCompression_ft;
   aNormalizedBrakingValue   = mNormalizedBrakingValue;
   aSteeringAngle_deg        = mSteeringAngle_deg;
   aHeightAboveTerrain_ft    = mHeightAboveTerrain_ft;
   aIsLandingGear            = mIsLandingGear;
   aInContactWithGround      = mInContactWithGround;
   aMaxCompressionExceeded   = mMaxCompressionExceeded;
}

void wsf::six_dof::RigidBodyGroundReactionPoint::GetCompressionData(double& aUncompressedLength_ft,
                                                                    double& aMaximumCompression_ft,
                                                                    double& aCurrentCompression_ft,
                                                                    double& aNormalizedBrakingValue,
                                                                    double& aSteeringAngle_deg,
                                                                    bool&   aInContactWithGround,
                                                                    bool&   aMaxCompressionExceeded)
{
   aUncompressedLength_ft  = mUncompressedLength_ft;
   aMaximumCompression_ft  = mMaximumCompression_ft;
   aCurrentCompression_ft  = mCurrentCompression_ft;
   aNormalizedBrakingValue = mNormalizedBrakingValue;
   aSteeringAngle_deg      = mSteeringAngle_deg;
   aInContactWithGround    = mInContactWithGround;
   aMaxCompressionExceeded = mMaxCompressionExceeded;
}

void wsf::six_dof::RigidBodyGroundReactionPoint::GetSteeringData(double& aCurrentSteeringAngle_deg,
                                                                 double& aMaxSteeringAngle_deg) const
{
   aCurrentSteeringAngle_deg = mSteeringAngle_deg;

   if (mParentVehicle == nullptr)
   {
      aMaxSteeringAngle_deg = 0;
      return;
   }
   else
   {
      RigidBodyFlightControlSystem* controls = utils::CastToRigidBody(mParentVehicle)->GetFlightControls();

      if (controls == nullptr)
      {
         aMaxSteeringAngle_deg = 0;
         return;
      }
      else if (mControlSurfaceHandle == 0)
      {
         aMaxSteeringAngle_deg = 0;
         return;
      }
   }

   // Get the appropriate steering handle
   size_t steeringHandle = 0;
   if (mNWS_Engaged)
   {
      steeringHandle = mNWSAngleControlSurfaceHandle;
   }
   else
   {
      steeringHandle = mSteeringControlSurfaceHandle;
   }

   // Get the min/max values
   double minAngle_deg =
      utils::CastToRigidBody(mParentVehicle)->GetFlightControls()->GetControlSurfaceMinAngle_deg(steeringHandle);
   double maxAngle_deg =
      utils::CastToRigidBody(mParentVehicle)->GetFlightControls()->GetControlSurfaceMaxAngle_deg(steeringHandle);

   // We want the max angle
   if (std::abs(minAngle_deg) > maxAngle_deg)
   {
      aMaxSteeringAngle_deg = std::abs(minAngle_deg);
   }
   else
   {
      aMaxSteeringAngle_deg = maxAngle_deg;
   }
}

bool wsf::six_dof::RigidBodyGroundReactionPoint::SetDataWhenNotInContactWithGround(UtVec3dX& aForce_lbs,
                                                                                   UtVec3dX& aMoment_ftlbs,
                                                                                   bool&     aMaxCompressionExceeded)
{
   // Clear things
   mCurrentCompression_ft      = 0.0;
   mCurrentCompressionRate_fps = 0.0;
   mLastCompression_ft         = 0.0;
   mLastCompressionRate_fps    = 0.0;
   mNotMoving                  = false;
   mCurrentForce_lbs.Set(0, 0, 0);
   mCurrentMoment_ftlbs.Set(0, 0, 0);

   // Note that this function does not set mHeightAboveTerrain_ft

   // Set 'last' values
   mLastCompression_ft      = mCurrentCompression_ft;
   mLastCompressionRate_fps = mCurrentCompressionRate_fps;

   // Set outgoing data
   aForce_lbs              = mCurrentForce_lbs;
   aMoment_ftlbs           = mCurrentMoment_ftlbs;
   aMaxCompressionExceeded = mMaxCompressionExceeded;

   // Not in contact with ground
   mInContactWithGround = false;

   return mInContactWithGround;
}

bool wsf::six_dof::RigidBodyGroundReactionPoint::SetDataWhenTinyDeltaT(UtVec3dX& aForce_lbs,
                                                                       UtVec3dX& aMoment_ftlbs,
                                                                       bool&     aMaxCompressionExceeded)
{
   // Set 'last' values
   mLastCompression_ft      = mCurrentCompression_ft;
   mLastCompressionRate_fps = mCurrentCompressionRate_fps;

   // Return current conditions
   aForce_lbs              = mCurrentForce_lbs;
   aMoment_ftlbs           = mCurrentMoment_ftlbs;
   aMaxCompressionExceeded = mMaxCompressionExceeded;

   return mInContactWithGround;
}

bool wsf::six_dof::RigidBodyGroundReactionPoint::ReturnWhenNotInContactWithGround(UtVec3dX& aForce_lbs,
                                                                                  UtVec3dX& aMoment_ftlbs,
                                                                                  bool&     aMaxCompressionExceeded)
{
   // Set outgoing data
   aForce_lbs.Set(0, 0, 0);
   aMoment_ftlbs.Set(0, 0, 0);
   aMaxCompressionExceeded = mMaxCompressionExceeded;

   return false;
}

bool wsf::six_dof::RigidBodyGroundReactionPoint::ReturnWhenTinyDeltaT(UtVec3dX& aForce_lbs,
                                                                      UtVec3dX& aMoment_ftlbs,
                                                                      bool&     aMaxCompressionExceeded)
{
   // Return current conditions
   aForce_lbs              = mCurrentForce_lbs;
   aMoment_ftlbs           = mCurrentMoment_ftlbs;
   aMaxCompressionExceeded = mMaxCompressionExceeded;

   return mInContactWithGround;
}
