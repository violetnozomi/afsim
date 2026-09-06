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

#ifndef UT_MATH_6D_MOTION_HPP
#define UT_MATH_6D_MOTION_HPP

class UtInput;

#include "ut_export.h"

#include "UtAirspeed.hpp"
#include "UtAtmosphere.hpp"
#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtGuidance.hpp"
#include "UtQuaternion.hpp"
#include "UtVec3dX.hpp"

/**
A UtQuaternion-based motion model to simulate smooth 6-D motion in both
rotational and linear aspects.  Unlike the similar UtMath3D_Motion class, this
expansion is a simplified six-degrees-of-freedom model and contains a built-
in rudimentary aerodynamics and point mass model.  It generates plausible
angle of attack values while cruising and maneuvering, and employs aircraft-
like bank-to-turn steering.  It does simulate lift as a mechanism for producing
acceleration in a given direction, but does not consider drag or thrust, and
continually accelerates if needed to obtain a supplied target speed.  The
primary goal is to present more realistic signature attitudes to external
sensors.  Class does not implement rotational dynamics in the presence of
moments of inertia, but assumes that control authority is always available to
drive actual attitude to the desired.  This class is a _control_only_ paradigm,
and must be supplied with a Fly-To-Point for guidance.  WARNING:  After
Initialize(), the class assumes it has sole motion control of the pointed to
entity, with no outside interference, so the container application must "inform"
it if the motion state is changed externally, via a new Initialize() call.

Particularly of interest in this model is how to deduce what the platform
orientation is.  The truth of the matter is that it does not matter, at all.
The more appropriate question is what it _should_ be.  The platform begins
with zero beta and an appropriate alpha to oppose gravity.  But as intentional
accelerations are requested/produced that alter the true velocity, it is
inferred (after the fact) what alpha and bank was needed to aerodynamically
produce such forces (assuming zero wind).  The platform orientation is washed
in to (hopefully) "appear" correct.  If the motion is extremely dynamic, the
visual results may not pass a common sense test.  Alpha and beta angles are
not under direct control, but merely result from the current acceleration.
Wind will _never_ have any effect in this motion (!).
*/

class UT_EXPORT UtMath6D_Motion
{
public:
   enum AccelFrame
   {
      cWCS          = 0,
      cNED          = 1,
      cECS          = 2,
      cFLY_TO_POINT = 3
   };

   UtMath6D_Motion(const UtAtmosphere& aAtmosphere);
   UtMath6D_Motion(const UtMath6D_Motion& aRhs);
   virtual ~UtMath6D_Motion() = default;

   virtual bool Initialize(double aSimTime, UtEntity* aEntityPtr);
   virtual bool ProcessInput(UtInput& aInput);
   virtual void Update(double aSimTime);

   double CurrentSpeed() const { return mSpeed; }
   double GroundSpeed() const;
   double Altitude() const { return mAltitude; }

   // The desired acceleration set here does not consider gravity.
   // (i.e. a vector of zero, zero, zero is to continue current velocity unchanged.)
   // void SetDesiredAccel(const UtVec3dX& aAccel,
   //                     AccelFrame      aFrame);
   void SetFlyToPoint(const UtVec3dX& aLocWCS);
   void SetFlyToPoint(double aLatDeg, double aLonDeg, double aAlta);

   bool RecommendSwitchToNewWaypoint();

   double TurnRadius();
   double PreviousAltitude() const { return mPrevAlt; }

   UtAirspeed& TargetAirspeed() { return mTargetAirspeed; }
   void        SetTargetSpeed(double aValue);
   void        SetMaximumOmega(double aValue) { mMaxTangentialOmega = aValue; }
   void        SetPreviousAlt(double aValue) { mPrevAlt = aValue; }

   void SetMaximumLinearAcceleration(double aValue) { mMaxLinearAccel = aValue; }
   void SetMaximumNormalAcceleration(double aValue) { mMaxNormalAccel = aValue; }

   void SetMass(double aValue) { mMass = aValue; }
   void SetReferenceArea(double aValue) { mRefArea = aValue; }
   void SetCL_Alpha(double aValue) { mCL_Alpha = aValue; }

   UtVec3dX DesiredLocWCS() const { return mDesiredLocWCS; }
   UtVec3dX DesiredAccelWCS() const { return mDesiredAccelWCS; }

   bool FlyToPointReached();

   class Filter
   {
   public:
      Filter(size_t aSize = 3);
      void     Reset(const UtVec3dX& aInitialValue);
      UtVec3dX Update(const UtVec3dX& aInput);
      void     ReSize(size_t aSize);

   private:
      std::vector<UtVec3dX> mBuffer;
      size_t                mModulus;
   };

   UtVec3dX LocationWCS() const { return mLocWCS; }
   UtVec3dX VelocityWCS() const { return mVelWCS; }
   UtVec3dX VelocityNED() const { return mVelNED; }

   UtDCM DcmWCStoECS() const { return mDcmWCStoECS; }
   UtDCM DcmWCStoNED() const { return mDcmWCStoNED; }
   UtDCM DcmWCStoStab() const { return mDcmWCStoStab; } // "Stability" axes (aligned to velocity vector).

protected:
   void   ConstrainStabRates();
   bool   LiftValuesAreOK();
   void   ComputeAndSetNED_Attitude(double aSimTime);
   void   UpdateNED_AfterMove();
   double NeededTangentialAccel();
   double GroundTurnRadius() const;
   void   SetAttitudeFrom(double aAlphaStab, bool aStartFilters = false);

   UtEntity*  mEntityPtr;
   UtGuidance mGuidance;
   double     mLastUpdateTime;

   // === Platform Representation Values: ===
   double mRefArea;
   double mCL_Alpha; // partial derivative CL wrt Alpha 1/rad.
   double mMaxAlpha;
   double mMaxCL;
   double mMass;
   // === =============================== ===

   double mInitSpeed;
   double mInitFPA;

   // Controls and Constraints:
   UtAirspeed mTargetAirspeed;
   double     mTargetLinearSpeed;
   double     mSpeed;
   double     mGroundSpeed;
   double     mMaxLinearAccel;
   double     mMaxNormalAccel;
   double     mMaxTangentialOmega; // sanity check on rates.
   double     mDistToPoint;
   double     mPrevAlt;

   UtVec3dX mLocWCS;
   UtVec3dX mVelWCS;
   UtVec3dX mDesiredAccelWCS;

   // North-East-Down Attributes (all derived?).
   UtVec3dX mVelNED;
   double   mGammaNED; // needed for constraints
   double   mCourseNED;
   double   mAltitude;
   double   mAltitudeRate;
   double   mHeadingNED;
   double   mPitchNED;
   double   mRollNED; // needed for roll control
   double   mLatitudeDeg;
   double   mLongitudeDeg;

   double mSpeedGain;

   UtVec3dX mAccelNED_WOTFG; // without thrust, fighting gravity

   bool mDetailDebug;
   bool mDidSwitchFTP;

   UtVec3dX mStabDot; // Inertial slew rates of Stab axes, expressed in Stability (velocity) frame.

   UtDCM mDcmWCStoNED;  // = f(Lat, Lon, Alt)
   UtDCM mDcmNEDtoStab; // = f(course, gamma, 0.0)
   UtDCM mDcmWCStoStab; // <property>
   UtDCM mDcmWCStoECS;

   UtQuaternion mQuatWCStoStab;     // Velocity Quaternion (Stability axes)
   UtQuaternion mQuatRateWCStoStab; // Velocity Quaternion (Stability axes) rates.

   UtVec3dX mPrevDeltaWCS;
   UtVec3dX mDeltaWCS;

   UtVec3dX   mDesiredLocWCS;
   AccelFrame mDesiredAccelFrame;

   Filter mFilteredUnitFwdNED;
   Filter mFilteredUnitDwnNED;

   UtAtmosphere mAtm;
};

#endif
