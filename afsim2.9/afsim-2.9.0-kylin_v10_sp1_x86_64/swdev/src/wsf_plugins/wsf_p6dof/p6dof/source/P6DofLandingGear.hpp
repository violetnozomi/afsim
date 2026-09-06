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

#ifndef P6DOFLANDINGGEAR_H
#define P6DOFLANDINGGEAR_H

#include "p6dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "P6DofObject.hpp"
#include "UtVec3dX.hpp"

class UtInput;
class P6DofGroundReactionPoint;
class P6DofVehicle;

// The P6DofLandingGear class provides a means to provide forces and moments produced
// by landing gear and/or other contact points on the vehicle which are handled by
// instances of the P6DofGroundReactionPoint class.

class P6DOF_EXPORT P6DofLandingGear : public P6DofObject
{
public:
   explicit P6DofLandingGear(P6DofScenario* aScenario);
   ~P6DofLandingGear() override;

   P6DofLandingGear& operator=(const P6DofLandingGear& other) = delete;

   P6DofLandingGear* Clone() const;

   void SetParentVehicle(P6DofVehicle* aParentVehicle) override;

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput);
   bool Initialize(int64_t aSimTime_nanosec);
   bool Initialize2();

   // This function should be called to "update" the landing gear, including updating
   // internal parameters. The aDesiredForceBasedFrictionDirectionInSurfaceVecNED
   // is used for force-based friction conditions.
   void Update(int64_t aSimTime_nanosec, UtVec3dX& aRequiredOpposingForceInSurfaceVecNED_lbs);

   void SetLastUpdateSimTime(int64_t aLastUpdateSimTime_nanosec)
   {
      mLastUpdateSimTime_nanosec = aLastUpdateSimTime_nanosec;
   }

   // This returns the total force and moment produced by all reaction points
   // It should be called after Update() is called.
   void GetForceAndMoments(UtVec3dX& aForce_lbs, UtVec3dX& aMoment_ftlbs) const;

   // This returns true if at least one reaction point exceeded its maximum compression.
   // Exceeding maximum compression usually means the vehicle has crashed.
   bool SomethingExceededMaxCompression() const { return mSomethingExceededMaxCompression; }

   // This returns true if at least one reaction point is in contact with the ground/surface
   bool SomethingIsInContactWithGround() const { return mSomethingIsInContactWithGround; }

   // This returns true if at least one landing gear point is in contact with the ground/surface
   bool WeightOnWheels() const { return mWeightOnWheelsFlag; }
   bool WeightOnNoseWheel() const { return mWeightOnNoseWheelFlag; }

   // This returns true if all landing gear are not moving
   bool AllGearAtRest() const;

   // This returns true if friction is large enough to hold the aircraft from moving
   bool FrictionHoldingStill() const { return mFrictionHoldingStill; }

   // This allows an external force (such as a ground tug) to apply a force
   // to the nose gear. The angle is measured as zero being forward and
   // positive angles being to the right
   void ApplyExternalForce(double aForceMagnitude_lbs, double aAngle_deg);

   // This allows an external force (such as a ground tug) to apply a force
   // to the nose gear. The specified vector is in body coordinates.
   void ApplyExternalForce(const UtVec3dX& aExternalForceBody_lbs);

   // This removes any and all external forces on the nose gear
   void RemoveExternalForce();

   // This controls enabling of NWS (which allows a greater steering angle than normal mode
   void EnableNWS(bool aEnable);

   // This returns true is NWS is enabled
   bool NWS_IsEnabled() const { return mNWS_IsEnabled; }

   // This returns the distance (in feet) that the nose gear is from the reference point
   // in body coordinates. If more than one nose gear exists, it returns the value
   // for the first one encountered.
   double NoseGearDistForwardOfReference_ft();

   // This returns the distance (in feet) that the right main gear is from the reference point
   // in body coordinates. If more than one right main gear exists, it returns the value
   // for the first one encountered.
   double RightMainGearRearwardOfReference_ft();

   // This returns the distance (in feet) that the right main gear is from the reference point
   // in body coordinates. If more than one right main gear exists, it returns the value
   // for the first one encountered.
   double RightMainGearSidewaysOfReference_ft();

   // This calculates the steering angle to achieve the specified turn radius.
   // Returns false if it cannot achieve the desired turn radius
   void SteeringAngleForDesiredRadiusFt_deg(double aTurnRadius_ft, double& aSteeringAngle_deg, double aMaxSteeringAngle_deg);

   // This returns a pointer to the nose gear
   P6DofGroundReactionPoint* GetNoseGear();

   // This returns a pointer to the right main gear
   P6DofGroundReactionPoint* GetRightMainGear();

   // This returns a pointer to the left main gear
   P6DofGroundReactionPoint* GetLeftMainGear();

   // This returns the nominal height (in meters) of the vehicle above ground
   // level when resting on the landing gear. It is useful for determining the
   // "altitude" of the vehicle when on the ground. Note that this is a nominal
   // value -- the actual value will be based on gross weight, which will
   // compress the gear more or less than nominal, changing the height.
   double GetNominalHeightAboveGroundOnGear() { return mNominalHgtAboveGndOnGear_m; }

protected:
   P6DofLandingGear(const P6DofLandingGear& aSrc);

   bool NameIsUnique(const std::string& aName) const;

   // This is used (when at least one point is in contact with ground) to
   // determine if the vehicle is at rest.
   bool VehicleIsAtRest() const;

   int64_t                                mLastUpdateSimTime_nanosec;
   std::vector<P6DofGroundReactionPoint*> mListOfGroundReactionPoints;
   bool                                   mApplyExternalForce;
   UtVec3dX                               mCurrentExternalForceBody_lbs;
   UtVec3dX                               mTotalForce_lbs;
   UtVec3dX                               mTotalMoment_ftlbs;
   UtVec3dX                               mLastVehicleVelocityOnGround_mps;
   bool                                   mSomethingExceededMaxCompression;
   bool                                   mSomethingIsInContactWithGround;
   bool                                   mWeightOnWheelsFlag;
   bool                                   mWeightOnNoseWheelFlag;
   bool                                   mLastTimeAtRestOnSurface;
   bool                                   mFrictionHoldingStill;
   bool                                   mNWS_IsEnabled;
   std::string                            mNWSEnableControlName;
   size_t                                 mNWSEnableControlHandle;
   double                                 mNominalHgtAboveGndOnGear_m;
   P6DofGroundReactionPoint*              mNoseGear;
   P6DofGroundReactionPoint*              mRightMainGear;
   P6DofGroundReactionPoint*              mLeftMainGear;
};

#endif
