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

#include "P6DofLandingGear.hpp"

#include "P6DofGroundReactionPoint.hpp"
#include "P6DofUtils.hpp"
#include "P6DofVehicle.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofLandingGear::P6DofLandingGear(P6DofScenario* aScenario)
   : P6DofObject(aScenario)
   , mLastUpdateSimTime_nanosec(0)
   , mApplyExternalForce(false)
   , mCurrentExternalForceBody_lbs(0., 0., 0.)
   , mTotalForce_lbs(0., 0., 0.)
   , mTotalMoment_ftlbs(0., 0., 0.)
   , mLastVehicleVelocityOnGround_mps(0., 0., 0.)
   , mSomethingExceededMaxCompression(false)
   , mSomethingIsInContactWithGround(false)
   , mWeightOnWheelsFlag(false)
   , mWeightOnNoseWheelFlag(false)
   , mLastTimeAtRestOnSurface(false)
   , mFrictionHoldingStill(false)
   , mNWS_IsEnabled(false)
   , mNWSEnableControlHandle(0)
   , mNominalHgtAboveGndOnGear_m(0)
   , mNoseGear(nullptr)
   , mRightMainGear(nullptr)
   , mLeftMainGear(nullptr)
{
}

P6DofLandingGear::~P6DofLandingGear()
{
   std::vector<P6DofGroundReactionPoint*>::iterator reactionPointIter = mListOfGroundReactionPoints.begin();
   while (reactionPointIter != mListOfGroundReactionPoints.end())
   {
      P6DofGroundReactionPoint* temp = *reactionPointIter;
      ++reactionPointIter;
      delete temp;
   }
   mListOfGroundReactionPoints.clear();
}

P6DofLandingGear::P6DofLandingGear(const P6DofLandingGear& aSrc)
   : P6DofObject(aSrc)
   , mLastUpdateSimTime_nanosec(aSrc.mLastUpdateSimTime_nanosec)
   , mApplyExternalForce(aSrc.mApplyExternalForce)
   , mCurrentExternalForceBody_lbs(aSrc.mCurrentExternalForceBody_lbs)
   , mTotalForce_lbs(aSrc.mTotalForce_lbs)
   , mTotalMoment_ftlbs(aSrc.mTotalMoment_ftlbs)
   , mLastVehicleVelocityOnGround_mps(aSrc.mLastVehicleVelocityOnGround_mps)
   , mSomethingExceededMaxCompression(aSrc.mSomethingExceededMaxCompression)
   , mSomethingIsInContactWithGround(aSrc.mSomethingIsInContactWithGround)
   , mWeightOnWheelsFlag(aSrc.mWeightOnWheelsFlag)
   , mWeightOnNoseWheelFlag(aSrc.mWeightOnNoseWheelFlag)
   , mLastTimeAtRestOnSurface(aSrc.mLastTimeAtRestOnSurface)
   , mFrictionHoldingStill(aSrc.mFrictionHoldingStill)
   , mNWS_IsEnabled(aSrc.mNWS_IsEnabled)
   , mNWSEnableControlName(aSrc.mNWSEnableControlName)
   , mNWSEnableControlHandle(aSrc.mNWSEnableControlHandle)
   , mNominalHgtAboveGndOnGear_m(aSrc.mNominalHgtAboveGndOnGear_m)
   , mNoseGear(nullptr)
   , mRightMainGear(nullptr)
   , mLeftMainGear(nullptr)
{
   // Copy reaction points list
   if (!aSrc.mListOfGroundReactionPoints.empty())
   {
      std::vector<P6DofGroundReactionPoint*>::const_iterator reactionPointIter = aSrc.mListOfGroundReactionPoints.begin();
      while (reactionPointIter != aSrc.mListOfGroundReactionPoints.end())
      {
         P6DofGroundReactionPoint* srcReactionPoint  = *reactionPointIter;
         P6DofGroundReactionPoint* tempReactionPoint = srcReactionPoint->Clone();
         mListOfGroundReactionPoints.push_back(tempReactionPoint);
         ++reactionPointIter;
      }
   }
}

P6DofLandingGear* P6DofLandingGear::Clone() const
{
   return new P6DofLandingGear(*this);
}

void P6DofLandingGear::SetParentVehicle(P6DofVehicle* aParentVehicle)
{
   P6DofObject::SetParentVehicle(aParentVehicle);
   for (auto& i : mListOfGroundReactionPoints)
   {
      i->SetParentVehicle(mParentVehicle);
   }
}

bool P6DofLandingGear::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "landing_gear")
   {
      UtInputBlock block(aInput, "end_landing_gear");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "nws_enable_control_name")
         {
            std::string value;
            aInput.ReadValue(value);
            mNWSEnableControlName = value;
         }
         else if (localCommand == "nominal_height_above_ground_on_gear")
         {
            double value_m;
            aInput.ReadValueOfType(value_m, UtInput::cLENGTH);
            mNominalHgtAboveGndOnGear_m = value_m;
         }
         else if (localCommand == "ground_reaction_point")
         {
            P6DofGroundReactionPoint* tempReactionPoint = new P6DofGroundReactionPoint(GetScenario());
            tempReactionPoint->ProcessInput(aInput);

            std::string name = tempReactionPoint->GetName();

            // Verify name is unique
            if (NameIsUnique(name))
            {
               mListOfGroundReactionPoints.push_back(tempReactionPoint);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Duplicate Reaction point (" + name + ")");
            }
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofLandingGear::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofLandingGear::Initialize(int64_t aSimTime_nanosec)
{
   mLastUpdateSimTime_nanosec = aSimTime_nanosec;

   if (mParentVehicle)
   {
      if (!mNWSEnableControlName.empty())
      {
         // Try to get a boolean handle for mNWSEnableControlName
         mNWSEnableControlHandle = mParentVehicle->GetFlightControls()->GetBooleanControlHandle(mNWSEnableControlName);
      }

      std::vector<P6DofGroundReactionPoint*>::iterator iter = mListOfGroundReactionPoints.begin();
      while (iter != mListOfGroundReactionPoints.end())
      {
         (*iter)->Initialize();
         ++iter;
      }

      // These functions return distances (which we ignore here) but also "find" the mNoseGear,
      // mRightMainGear and mLeftMainGear, which is why these are called at this location
      NoseGearDistForwardOfReference_ft();
      RightMainGearRearwardOfReference_ft();

      return true;
   }

   return false;
}

bool P6DofLandingGear::Initialize2()
{
   // Get the mNoseGear and mRightMainGear and mLeftMainGear pointers by calling these functions
   NoseGearDistForwardOfReference_ft();
   RightMainGearRearwardOfReference_ft(); // This will find the mRightMainGear and mLeftMainGear
   return true;
};

bool P6DofLandingGear::AllGearAtRest() const
{
   std::vector<P6DofGroundReactionPoint*>::const_iterator reactionPointIter = mListOfGroundReactionPoints.begin();
   while (reactionPointIter != mListOfGroundReactionPoints.end())
   {
      P6DofGroundReactionPoint* point = *reactionPointIter;
      if (!point->NotMoving())
      {
         return false;
      }

      ++reactionPointIter;
   }

   return true;
}

void P6DofLandingGear::ApplyExternalForce(double aForceMagnitude_lbs, double aAngle_rad)
{
   UtVec3dX externalForceBody_lbs(aForceMagnitude_lbs * cos(aAngle_rad), aForceMagnitude_lbs * sin(aAngle_rad), 0.);
   ApplyExternalForce(externalForceBody_lbs);
}

void P6DofLandingGear::ApplyExternalForce(const UtVec3dX& aExternalForceBody_lbs)
{
   mApplyExternalForce           = true;
   mCurrentExternalForceBody_lbs = aExternalForceBody_lbs;
}

void P6DofLandingGear::RemoveExternalForce()
{
   mApplyExternalForce = false;
}

//  Landing gear update involves a multi-step process:
//
//  1) Determine normal force and moment produced by each reaction point.
//  2) Determine if the vehicle is "at rest".
//  3) If not at rest, use "standard" velocity vector approach (with dynamic coefficient for
//     rolling and braking, but static coefficient for scuffing, assuming that the tire does
//     not skid) for each reaction point and sum all forces and moments.
//  4) If at rest, determine the direction of the friction force, which opposes the net force
//     in the surface plane.
//  5) Next, determine the maximum friction force magnitude (static coefficients) in the
//     friction direction for each reaction point, sum these max friction force magnitudes
//     and compare with the non-friction force magnitude.
//  6) If the max friction force is less than the non-friction force, the body will begin to
//     roll. Use the non-friction force direction as the friction direction (with dynamic
//     coefficient for rolling and braking, but static coefficient for scuffing, assuming that
//     the tire does not skid) for each reaction point and sum all forces and moments.
//  7) If the max friction force is greater than the non-friction force, the body will remain
//     at rest, so continue with the following steps.
//  8) Set the friction vector equal and opposite to the non-friction force component in the
//     surface plane. This will result in no linear acceleration in the plane of the surface.
//  9) Determine the ratio of max friction to required friction. Use this ratio along with the
//     force-based friction direction (with dynamic coefficient for rolling and braking, but
//     static coefficient for scuffing, assuming that the tire does not skid) for each
//     reaction point and sum all forces and moments.

void P6DofLandingGear::Update(int64_t aSimTime_nanosec, UtVec3dX& aRequiredOpposingForceInSurfaceVecNED_lbs)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   int64_t dT_nanosec = aSimTime_nanosec - mLastUpdateSimTime_nanosec;

   if (dT_nanosec < 0)
   {
      return;
   }

   double dT_sec = P6DofUtils::TimeToTime(dT_nanosec);

   // Clear current values
   mTotalForce_lbs.Set(0, 0, 0);
   mTotalMoment_ftlbs.Set(0, 0, 0);
   mSomethingIsInContactWithGround = false;
   mWeightOnWheelsFlag             = false;
   mWeightOnNoseWheelFlag          = false;
   mFrictionHoldingStill           = false;
   UtVec3dX currentNormalForce_lbs(0., 0., 0.);
   UtVec3dX currentNormalMoment_ftlbs(0., 0., 0.);
   UtVec3dX currentFrictionForce_lbs(0., 0., 0.);
   UtVec3dX currentFrictionMoment_ftlbs(0., 0., 0.);

   if (mNWSEnableControlHandle)
   {
      EnableNWS(mParentVehicle->GetFlightControls()->GetControlBoolean(mNWSEnableControlHandle));
   }

   // These are used to sum-up external forces (in body coordinates)
   UtVec3dX currentExternalForceBody_lbs(0., 0., 0.);
   UtVec3dX currentExternalMomentBody_ftlbs(0., 0., 0.);

   // This is the external force in NED coordinates
   UtVec3dX currentExternalForceNED_lbs(0., 0., 0.);

   // If "external force" is enabled, loop through all landing gear, and for those gear
   // that are "nose gear", set their steering angle to the same "angle" as the external
   // control. This means that the external force, if present, will override any pilot-commanded
   // nose gear angle -- this is intentional.
   //
   // If "external force" is enabled, loop through all landing gear, and for those gear
   // that have an external force applied, add their external force vector (in body coordinates)
   // to currentExternalForceBody_lbs and then calculate the induced moment and add it to
   // currentExternalMomentBody_ftlbs. We then have the total external force/moments in body
   // coordinates.

   if (mApplyExternalForce)
   {
      double steeringAngle_deg = 0;
      // Get the angle (deg) of the external force.  Angle zero, is aligned with the nose, with
      //  90 degrees being out the right wing.
      double x = mCurrentExternalForceBody_lbs.Get(0);
      double y = mCurrentExternalForceBody_lbs.Get(1);
      if (fabs(x) < 0.00001)
      {
         if (y < 0)
         {
            steeringAngle_deg = -90.0;
         }
         else
         {
            steeringAngle_deg = 90.0;
         }
      }
      else
      {
         steeringAngle_deg = atan2(y, x) * UtMath::cDEG_PER_RAD;

         // Always keep the steering angle in the 1st/4th quadrant (-90 to 90 degrees).
         //  Example: The Wheels can roll both forward/backwards, no need to turn the wheel all
         //  the way around to move backwards
         if (steeringAngle_deg > 90.0)
         {
            steeringAngle_deg = steeringAngle_deg - 180;
         }
         else if (steeringAngle_deg < -90.0)
         {
            steeringAngle_deg = steeringAngle_deg + 180;
         }
      }

      for (auto& point : mListOfGroundReactionPoints)
      {
         if (point->mNoseGear)
         {
            // Set flag
            point->mExternalForceBeingApplied = true;
            // Set the steering angle
            point->SetSteeringAngle_deg(steeringAngle_deg);
            // Sum up the current external forces (this should be same as
            //  mCurrentExternalForceBody_lbs * # of Nose Gear)
            currentExternalForceBody_lbs += mCurrentExternalForceBody_lbs;
            // Calculate the induced moment and add it to the currentExternalMomentBody_ftlbs
            currentExternalMomentBody_ftlbs += point->mGearPos_m.Cross(mCurrentExternalForceBody_lbs);
         }
      }

      // We also need the external forces in NED.
      // Get the parent object from the nose gear
      if (!mListOfGroundReactionPoints.empty())
      {
         // The parent vehicle for the ground reaction points is the same as the parent vehicle for the landing gear.
         currentExternalForceNED_lbs =
            mParentVehicle->GetKinematicState().CalcNEDVecFromBodyVec(currentExternalForceBody_lbs);
      }
   }

   // Loop through each reaction point, update steering angle and braking, and calculate normal forces.
   for (auto& point : mListOfGroundReactionPoints)
   {
      UtVec3dX normalForce_lbs(0., 0., 0.);
      UtVec3dX normalMoment_ftlbs(0., 0., 0.);
      bool     maxCompressionExceeded    = false;
      bool     inContactWithGround       = false;
      bool     externalForceBeingApplied = point->mExternalForceBeingApplied;

      // Clear flag (local version set above)
      point->mExternalForceBeingApplied = false;

      // Get braking and steering control inputs

      if (!externalForceBeingApplied)
      {
         double steeringAngle_deg =
            mParentVehicle->GetFlightControls()->GetControlSurfaceAngle_deg(point->GetSteeringControlHandle());
         point->SetSteeringAngle_deg(steeringAngle_deg);
      }

      double brakingValue = mParentVehicle->GetFlightControls()->GetControlValue(point->GetBrakingControlHandle());
      point->SetBraking(brakingValue);

      // Calculate normal force
      inContactWithGround =
         point->CalcNormalForceMoment(normalForce_lbs, normalMoment_ftlbs, maxCompressionExceeded, dT_sec);
      currentNormalForce_lbs += normalForce_lbs;
      currentNormalMoment_ftlbs += normalMoment_ftlbs;

      if (maxCompressionExceeded)
      {
         mSomethingExceededMaxCompression = true;
      }
      if (inContactWithGround)
      {
         mSomethingIsInContactWithGround = true;

         // We need to have both mWeightOnWheelsFlag (which means there is weight on at least
         // one wheel) and mWeightOnNoseGearFlag (which means there is weight on a nose gear. The nose
         // gear flag will be used when "rotating" for takeoff and when lowering the nose after landing.

         // If this is landing gear, we have "weight on wheels"
         if (point->IsLandingGear())
         {
            mWeightOnWheelsFlag = true;

            if (point->mNoseGear)
            {
               mWeightOnNoseWheelFlag = true;
            }
         }
      }
   }

   // Determine if vehicle is at rest and on the ground
   bool atRest = false;
   if (mSomethingIsInContactWithGround)
   {
      atRest = VehicleIsAtRest();
   }

   if (!atRest)
   {
      if (mSomethingIsInContactWithGround)
      {
         // Not at rest, so loop through all reaction points and sum-up the frictional forces and moments
         for (auto& point : mListOfGroundReactionPoints)
         {
            UtVec3dX frictionForce_lbs(0., 0., 0.);
            UtVec3dX frictionMoment_ftlbs(0., 0., 0.);

            // In motion, so use "standard case"
            point->CalcFrictionForceMomentWhileInMotion(frictionForce_lbs, frictionMoment_ftlbs);

            currentFrictionForce_lbs += frictionForce_lbs;
            currentFrictionMoment_ftlbs += frictionMoment_ftlbs;
         }
      }
   }
   else
   {
      UtVec3dX desiredForceBasedFrictionDirectionInSurfaceVecNED(0., 0., 0.);
      double   nonLandingGearForce_lbs = 0.0;
      if (mApplyExternalForce)
      {
         // Subtraction the current external this is because friction will oppose, and hence be negative.
         desiredForceBasedFrictionDirectionInSurfaceVecNED =
            aRequiredOpposingForceInSurfaceVecNED_lbs - currentExternalForceNED_lbs;
         nonLandingGearForce_lbs = (aRequiredOpposingForceInSurfaceVecNED_lbs - currentExternalForceNED_lbs).Magnitude();
      }
      else
      {
         desiredForceBasedFrictionDirectionInSurfaceVecNED = aRequiredOpposingForceInSurfaceVecNED_lbs;
         nonLandingGearForce_lbs                           = aRequiredOpposingForceInSurfaceVecNED_lbs.Magnitude();
      }
      desiredForceBasedFrictionDirectionInSurfaceVecNED.Normalize();

      // Determine the maximum friction
      double maxTotalFriction_lbs = 0;
      for (auto& point : mListOfGroundReactionPoints)
      {
         maxTotalFriction_lbs +=
            point->CalcMaxFrictionForceMagnitude_lbs(desiredForceBasedFrictionDirectionInSurfaceVecNED);
      }

      if (maxTotalFriction_lbs < nonLandingGearForce_lbs)
      {
         // Begin to roll
         for (auto& point : mListOfGroundReactionPoints)
         {
            UtVec3dX frictionForce_lbs(0., 0., 0.);
            UtVec3dX frictionMoment_ftlbs(0., 0., 0.);

            point->CalcFrictionForceMomentStartingToMove(frictionForce_lbs,
                                                         frictionMoment_ftlbs,
                                                         desiredForceBasedFrictionDirectionInSurfaceVecNED);

            currentFrictionForce_lbs += frictionForce_lbs;
            currentFrictionMoment_ftlbs += frictionMoment_ftlbs;
         }
      }
      else
      {
         // Friction is preventing the vehicle from moving
         for (auto& point : mListOfGroundReactionPoints)
         {
            UtVec3dX frictionForce_lbs(0., 0., 0.);
            UtVec3dX frictionMoment_ftlbs(0., 0., 0.);

            point->CalcFrictionForceMomentStartingToMove(frictionForce_lbs,
                                                         frictionMoment_ftlbs,
                                                         desiredForceBasedFrictionDirectionInSurfaceVecNED);

            currentFrictionForce_lbs += frictionForce_lbs;
            currentFrictionMoment_ftlbs += frictionMoment_ftlbs;
         }

         // Special case -- we are not moving
         // Determine ratio
         double requiredOpposingForce_lbs = aRequiredOpposingForceInSurfaceVecNED_lbs.Magnitude();
         double currentForce_lbs          = currentFrictionForce_lbs.Magnitude();
         double ratio                     = 1.0;
         if (currentForce_lbs > 0)
         {
            ratio = requiredOpposingForce_lbs / currentForce_lbs;
         }

         // Match the required force
         currentFrictionForce_lbs = aRequiredOpposingForceInSurfaceVecNED_lbs;

         // Eliminate rolling and yawing moments
         double moment[3];
         currentFrictionMoment_ftlbs.Get(moment);
         moment[0] = 0.0;
         moment[1] *= ratio;
         moment[2] = 0.0;
         currentFrictionMoment_ftlbs.Set(moment);

         mFrictionHoldingStill = true;
      }
   }

   if (mApplyExternalForce)
   {
      mTotalForce_lbs    = currentNormalForce_lbs + currentFrictionForce_lbs + currentExternalForceBody_lbs;
      mTotalMoment_ftlbs = currentNormalMoment_ftlbs + currentFrictionMoment_ftlbs + currentExternalMomentBody_ftlbs;
   }
   else
   {
      mTotalForce_lbs    = currentNormalForce_lbs + currentFrictionForce_lbs;
      mTotalMoment_ftlbs = currentNormalMoment_ftlbs + currentFrictionMoment_ftlbs;
   }

   // Set last velocity
   if (mWeightOnWheelsFlag)
   {
      mLastVehicleVelocityOnGround_mps = mParentVehicle->GetVelocityNED_mps();
   }
   else
   {
      mLastVehicleVelocityOnGround_mps.Set(0, 0, 0);
   }

   if (mSomethingIsInContactWithGround)
   {
      mLastTimeAtRestOnSurface = true;
   }
   else
   {
      mLastTimeAtRestOnSurface = false;
   }

   // Set the last time
   mLastUpdateSimTime_nanosec = aSimTime_nanosec;
}

void P6DofLandingGear::GetForceAndMoments(UtVec3dX& aForce_lbs, UtVec3dX& aMoment_ftlbs) const
{
   aForce_lbs    = mTotalForce_lbs;
   aMoment_ftlbs = mTotalMoment_ftlbs;
}

bool P6DofLandingGear::NameIsUnique(const std::string& aName) const
{
   std::vector<P6DofGroundReactionPoint*>::const_iterator iter = mListOfGroundReactionPoints.begin();
   while (iter != mListOfGroundReactionPoints.end())
   {
      if (aName == (*iter)->GetName())
      {
         // Match found - name is not unique
         return false;
      }
      ++iter;
   }

   return true;
}

bool P6DofLandingGear::VehicleIsAtRest() const
{
   if (mParentVehicle == nullptr)
   {
      return false;
   }

   UtVec3dX currentVel_mps = mParentVehicle->GetVelocityNED_mps();

   // We need to determine some threshold speed, which if the vehicle speed is
   // below, it will be considered to be at rest
   double velThreshold_mps = 0.1; // 0.5; //1.0;

   double currentSpd_mps = currentVel_mps.Magnitude();

   // If the current vel is "at rest" return true
   if (currentSpd_mps < velThreshold_mps)
   {
      return true;
   }

   // Average the current and last vel vectors (since it may be "vibrating")
   UtVec3dX avgVel_mps = currentVel_mps + mLastVehicleVelocityOnGround_mps;
   avgVel_mps *= 0.5;

   double avgSpd_mps = avgVel_mps.Magnitude();

   // If the avg vel is "at rest" return true
   if (avgSpd_mps < velThreshold_mps)
   {
      return true;
   }

   return false;
}

void P6DofLandingGear::EnableNWS(bool aEnable)
{
   if (mParentVehicle == nullptr)
   {
      return;
   }

   mNWS_IsEnabled = aEnable;

   // Set enabled flag for all points
   for (auto& point : mListOfGroundReactionPoints)
   {
      point->mNWS_Engaged = aEnable;
   }
}

double P6DofLandingGear::NoseGearDistForwardOfReference_ft()
{
   if (mNoseGear == nullptr)
   {
      P6DofGroundReactionPoint* bestNoseGear = nullptr;
      double                    bestDist_m   = -1.0;

      // Loop through all points and find the one that with the highest 'x' value
      for (auto& point : mListOfGroundReactionPoints)
      {
         if (point->mNoseGear)
         {
            double tempDist_m = point->mGearPos_m.X();

            // Compare current distance with "best"
            if (tempDist_m > bestDist_m)
            {
               bestNoseGear = point;
               bestDist_m   = bestNoseGear->mGearPos_m.X();
            }
         }
      }

      // Set the nose gear pointer
      mNoseGear = bestNoseGear;
   }

   // If the nose gear pointer is valid, return the distance
   if (mNoseGear)
   {
      return mNoseGear->mGearPos_m.X() * UtMath::cFT_PER_M;
   }

   // No nose gear present, so return a zero distance
   return 0.0;
}

double P6DofLandingGear::RightMainGearRearwardOfReference_ft()
{
   if (mRightMainGear == nullptr)
   {
      P6DofGroundReactionPoint* bestMainGear = nullptr;
      double                    bestDist_m   = -1.0;

      // Loop through all points and find the one that with the highest 'y' value
      for (auto& point : mListOfGroundReactionPoints)
      {
         // To be a main gear, it cannot be a nose gear
         if (!point->mNoseGear)
         {
            // We need it to be on the right side (+y)
            // We will consider any gear that is a bit more than an inch to the right
            // to be a potential right main gear
            if (point->mGearPos_m.Y() > 0.1)
            {
               double tempDist_m = point->mGearPos_m.Y();

               // Compare current distance with "best"
               if (tempDist_m > bestDist_m)
               {
                  bestMainGear = point;
                  bestDist_m   = bestMainGear->mGearPos_m.Y();
               }
            }
         }
      }

      // Set the main gear pointer
      mRightMainGear = bestMainGear;
   }

   if (mLeftMainGear == nullptr)
   {
      P6DofGroundReactionPoint* bestMainGear = nullptr;
      double                    bestDist_m   = 1.0;

      // Loop through all points and find the one that with the lowest 'y' value
      for (auto& point : mListOfGroundReactionPoints)
      {
         // To be a main gear, it cannot be a nose gear
         if (!point->mNoseGear)
         {
            // We need it to be on the left side (-y)
            // We will consider any gear that is a bit more than an inch to the left
            // to be a potential left main gear
            if (point->mGearPos_m.Y() < -0.1)
            {
               double tempDist_m = point->mGearPos_m.Y();

               // Compare current distance with "best"
               if (tempDist_m < bestDist_m)
               {
                  bestMainGear = point;
                  bestDist_m   = bestMainGear->mGearPos_m.Y();
               }
            }
         }
      }

      // Set the main gear pointer
      mLeftMainGear = bestMainGear;
   }

   // If the main gear pointer is valid, return the distance
   if (mRightMainGear)
   {
      // Note that we negate, since we want "rearward" dist, not forward dist
      return -(mRightMainGear->mGearPos_m.X() * UtMath::cFT_PER_M);
   }

   // No main gear present, so return a zero distance
   return 0.0;
}

double P6DofLandingGear::RightMainGearSidewaysOfReference_ft()
{
   if (mRightMainGear == nullptr)
   {
      P6DofGroundReactionPoint* bestMainGear = nullptr;
      double                    bestDist_m   = -1.0;

      // Loop through all points and find the one that with the highest 'y' value
      for (auto& point : mListOfGroundReactionPoints)
      {
         // To be a main gear, it cannot be a nose gear
         if (!point->mNoseGear)
         {
            // We need it to be on the right side (+y)
            // We will consider any gear that is a bit more than an inch to the right
            // to be a potential right main gear
            if (point->mGearPos_m.Y() > 0.1)
            {
               double tempDist_m = point->mGearPos_m.Y();

               // Compare current distance with "best"
               if (tempDist_m > bestDist_m)
               {
                  bestMainGear = point;
                  bestDist_m   = bestMainGear->mGearPos_m.Y();
               }
            }
         }
      }

      // Set the main gear pointer
      mRightMainGear = bestMainGear;
   }

   // If the main gear pointer is valid, return the distance
   if (mRightMainGear)
   {
      return mRightMainGear->mGearPos_m.Y() * UtMath::cFT_PER_M;
   }

   // No main gear present, so return a zero distance
   return 0.0;
}

void P6DofLandingGear::SteeringAngleForDesiredRadiusFt_deg(double  aTurnRadius_ft,
                                                           double& aSteeringAngle_deg,
                                                           double  aMaxSteeringAngle_deg)

{
   double noseUp_ft   = NoseGearDistForwardOfReference_ft();
   double mainBack_ft = RightMainGearRearwardOfReference_ft();

   // [R*R] - [X*X] = [(Y+D)*(Y+D)]
   double turnRadiusSquared = aTurnRadius_ft * aTurnRadius_ft;
   double mainBackSquared   = mainBack_ft * mainBack_ft;
   if ((turnRadiusSquared - mainBackSquared) < 0.0)
   {
      // We have a problem, just use the max steering angle

      if (aTurnRadius_ft > 0)
      {
         aSteeringAngle_deg = aMaxSteeringAngle_deg;
      }
      else
      {
         aSteeringAngle_deg = -aMaxSteeringAngle_deg;
      }

      auto out = ut::log::debug() << "P6DofLandingGear::SteeringAngleForDesiredRadiusFt_deg():";
      out.AddNote() << "mainBack_ft: " << mainBack_ft;
      out.AddNote() << "aTurnRadius_ft: " << aTurnRadius_ft;

      return;
   }

   double axialDistToTurnCenter_ft  = sqrt(turnRadiusSquared - mainBackSquared);
   double distFromAxleToNoseGear_ft = mainBack_ft + noseUp_ft;

   // Calc the steering angle
   double angle_rad = atan2(distFromAxleToNoseGear_ft, axialDistToTurnCenter_ft);

   aSteeringAngle_deg = angle_rad * UtMath::cDEG_PER_RAD;

   if (aTurnRadius_ft < 0)
   {
      aSteeringAngle_deg *= -1.0;
   }

   // Limit by max angle
   if (aSteeringAngle_deg > aMaxSteeringAngle_deg)
   {
      aSteeringAngle_deg = aMaxSteeringAngle_deg;
   }
   else if (aSteeringAngle_deg < -aMaxSteeringAngle_deg)
   {
      aSteeringAngle_deg = -aMaxSteeringAngle_deg;
   }
}

P6DofGroundReactionPoint* P6DofLandingGear::GetNoseGear()
{
   // If nose gear is null, try to get it
   if (mNoseGear == nullptr)
   {
      P6DofGroundReactionPoint* bestNoseGear = nullptr;
      double                    bestDist_m   = -1.0;

      for (auto& point : mListOfGroundReactionPoints)
      {
         if (point->mNoseGear)
         {
            double tempDist_m = point->mGearPos_m.X();

            // Compare current distance with "best"
            if (tempDist_m > bestDist_m)
            {
               bestNoseGear = point;
               bestDist_m   = bestNoseGear->mGearPos_m.X();
            }
         }
      }

      // Set the nose gear pointer
      mNoseGear = bestNoseGear;
   }

   return mNoseGear;
}

P6DofGroundReactionPoint* P6DofLandingGear::GetRightMainGear()
{
   // If right gear is null, try to get it
   if (mRightMainGear == nullptr)
   {
      P6DofGroundReactionPoint* bestMainGear = nullptr;
      double                    bestDist_m   = -1.0;

      // Loop through all points and find the one that with the highest 'y' value
      for (auto& point : mListOfGroundReactionPoints)
      {
         // To be a main gear, it cannot be a nose gear
         if (!point->mNoseGear)
         {
            // We need it to be on the right side (+y)
            // We will consider any gear that is a bit more than an inch to the right
            // to be a potential right main gear
            if (point->mGearPos_m.Y() > 0.1)
            {
               double tempDist_m = point->mGearPos_m.Y();

               // Compare current distance with "best"
               if (tempDist_m > bestDist_m)
               {
                  bestMainGear = point;
                  bestDist_m   = bestMainGear->mGearPos_m.Y();
               }
            }
         }
      }

      // Set the main gear pointer
      mRightMainGear = bestMainGear;
   }

   return mRightMainGear;
}

P6DofGroundReactionPoint* P6DofLandingGear::GetLeftMainGear()
{
   // If left gear is null, try to get it

   if (mLeftMainGear == nullptr)
   {
      P6DofGroundReactionPoint* bestMainGear = nullptr;
      double                    bestDist_m   = 1.0;

      // Loop through all points and find the one that with the lowest 'y' value
      for (auto& point : mListOfGroundReactionPoints)
      {
         // To be a main gear, it cannot be a nose gear
         if (!point->mNoseGear)
         {
            // We need it to be on the left side (-y)
            // We will consider any gear that is a bit more than an inch to the left
            // to be a potential left main gear
            if (point->mGearPos_m.Y() < -0.1)
            {
               double tempDist_m = point->mGearPos_m.Y();

               // Compare current distance with "best"
               if (tempDist_m < bestDist_m)
               {
                  bestMainGear = point;
                  bestDist_m   = bestMainGear->mGearPos_m.Y();
               }
            }
         }
      }

      // Set the main gear pointer
      mLeftMainGear = bestMainGear;
   }

   return mLeftMainGear;
}
