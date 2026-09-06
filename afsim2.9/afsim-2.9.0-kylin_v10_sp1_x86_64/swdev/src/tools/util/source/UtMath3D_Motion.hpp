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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UT_MATH_3D_MOTION_HPP
#define UT_MATH_3D_MOTION_HPP

class UtInput;

#include "ut_export.h"

#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtGuidance.hpp"
#include "UtQuaternion.hpp"
#include "UtVec3dX.hpp"

/**
A UtQuaternion-based motion utility to simulate smooth dynamic motion in both
rotational and linear aspects, but without concern for mass properties.  This
is NOT an actual mover, is a utility that a mover type will control in order
to accomplish smooth math-based motion, without discontinuities.  Subjected
to constraints imposed on axial and lateral accelerations, and roll rate.
The algorithm will try to bank-to-turn, unless 'prefer_canopy-up' is requested,
when attitude stays vertical (z axis "down").  Attitude and velocity are
always maintained co-linear (3-DOF like behavior).  The class was first
developed to model torpedo-like motion, so has a Broach() function embedded to
prevent the object from ascending subsurface to surface.  The inertial frame
assumed is WCS-84.  WARNING:  After Initialize() this class assumes it has
sole motion control of the pointed to entity, with no outside interference, so
the containing application must "inform" it if the motion state is changed
externally, via a new Initialize() call.
*/

class UT_EXPORT UtMath3D_Motion
{
public:
   UtMath3D_Motion(bool aBroachAtSeaLevel = false);
   UtMath3D_Motion(const UtMath3D_Motion& aRhs);
   virtual ~UtMath3D_Motion() = default;

   virtual bool Initialize(double aSimTime, UtEntity* aEntityPtr);
   virtual bool ProcessInput(UtInput& aInput);
   virtual void Update(double aSimTime);

   double PathDistanceTraveled() const { return mPathDistanceTravelled; }
   double GroundDistanceTraveled() const { return mGroundDistanceTravelled; }
   double TimeTravelled() const { return mTimeTravelled; }
   double CurrentSpeed() const { return mCurrentLinearSpeed; }
   double GroundSpeed() const;
   double TurnRadius() const;
   double GroundTurnRadius() const;
   double StartTime() const { return mStartSimTime; }
   double MaxRollRate() const { return mMaxRollRate; }
   double Altitude() const { return mAltitude; }

   const UtVec3dX& LocWCS() const { return mLocWCS; }
   const UtVec3dX& VelWCS() const { return mVelWCS; }
   const UtVec3dX& AccWCS() const { return mAccWCS; }
   UtVec3dX        VelNED() const;
   UtVec3dX        AngleRatesPQR() const { return mPQR; }

   const UtDCM& WCStoNED() const { return mWCStoNED; }
   const UtDCM& WCStoECS() const { return mWCStoVelAndECS; }

   double CourseNED() const { return mCourseNED; }
   void   NED_Orientation(double& aHeading, double& aPitch, double& aRoll);

   // The following two inputs (choose one) will translate into
   // Azimuth and Elevation, ignoring roll accel, but will then
   // attempt to place the ECS "Up" vector into the turn plane.
   void SetDesiredAccelWCS(const UtVec3dX& aAccelWCS);
   void SetDesiredAccelECS(const UtVec3dX& aAccelECS);

   void SetFlyToPoint(const UtVec3dX& aAimpointWCS);
   void SetFlyToPoint(double aLat, double aLon, double aAlt);

   void SetTargetSpeed(double aValue) { mTargetLinearSpeed = std::abs(aValue); }

   void SetMaxLinearAcceleration(double aValue) { mMaxLinearAccel = aValue; }
   void SetMaxNormalAcceleration(double aValue) { mMaxNormalAccel = aValue; }
   void SetMaxRollRate(double aValue) { mMaxRollRate = aValue; }

   bool     FlyToPointReached(double aSimTime) const;
   UtVec3dX GetFlyToPoint() const { return mDesiredLocWCS; }
   void     SetLastUpdateTime(double aValue) { mLocalLastUpdateTime = aValue; }

protected:
   void UpdateLinearSpeed(double aSimTime);
   void UpdateGuidance(double aSimTime);
   void ConstrainBodyRates();
   void UpdateNED_AfterWCS_Move(double aSimTime);
   void PrintState(double aSimTime);
   void GetAllMotionFromEntity();
   void SetMotionToInitConditions();
   void GetDCMsAndQuatFromEntity();
   void Broach(double aSimTime);

   UtEntity* mEntityPtr;
   double    mStartSimTime;
   double    mLocalLastUpdateTime;

   double mPathDistanceTravelled;
   double mGroundDistanceTravelled;
   double mTimeTravelled;

   double mInitialSpeed;
   double mInitialFPA;
   double mTargetLinearSpeed;
   double mCurrentLinearSpeed; // key attribute
   double mCurrentGroundSpeed;
   double mMaxLinearAccel;
   double mMaxNormalAccel;
   double mMaxRollRate;
   double mMaxTangentialOmega; // sanity check on rates.

   UtVec3dX mLocWCS; // key attribute
   UtVec3dX mVelWCS; // key attribute
   UtVec3dX mAccWCS;

   UtVec3dX mVelNED;
   UtVec3dX mAccNED;
   double   mGammaNED; // needed for constraints
   double   mCourseNED;
   double   mAltitude;
   double   mAltitudeRate;
   double   mRollNED; // needed for roll control
   double   mLatitudeDeg;
   double   mLongitudeDeg;

   bool mDetailDebug;
   bool mPreferCanopyUp;
   bool mBroachAtSeaLevel;
   bool mIsFlyingToPoint;
   bool mUseGrndTrnRadius;

   UtVec3dX mPQR; // Roll, Pitch, Yaw Rates rad/sec.

   UtDCM mWCStoNED; // = f (Lat, Lon, Alt)
   UtDCM mNEDtoECS; // NOTE:  The linear velocity will ALWAYS be directed along the ECS X (or "Forward" direction).
   UtDCM mWCStoVelAndECS;
   UtQuaternion mQuatWCStoVelAndECS; // key attribute
   UtQuaternion mQuatRate;           // Quaternion rates.

   UtVec3dX mPrevDeltaWCS;
   UtVec3dX mCurrentDeltaWCS;

   UtVec3dX mDesiredAccel;
   UtVec3dX mDesiredLocWCS; // Only used if mIsFlyingToPoint.
   bool     mDesiredAccelIsWCS;
   double   mDesiredBankNED;

   UtGuidance mGuidance;
};

#endif
