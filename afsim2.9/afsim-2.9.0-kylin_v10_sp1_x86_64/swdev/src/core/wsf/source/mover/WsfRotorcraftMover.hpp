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

#ifndef WSFROTORCRAFTMOVER_HPP
#define WSFROTORCRAFTMOVER_HPP

#include "UtControl.hpp"
#include "UtDCM.hpp"
class UtInput;
class UtInputBlock;
#include "wsf_export.h"

#include "UtPID_Controller.hpp"
#include "UtVec3dX.hpp"
#include "WsfIterativeRouteMover.hpp"
#include "WsfMoverMode.hpp"
#include "WsfWaypointPathGuidance.hpp"

/**
New motion model to simulate Rotorcraft dynamics.  The WCS frame is assumed
the Inertial frame, so integrations are there, but most calculations are in
the NED frame, whose rotation rate is assumed insignificant.  Reference frame
hierarchy is WCS => NED => RotorDiskPlane (X points North) => Heading, and DCMs
are used to do each transform.  Acceleration and Velocity are controlled in
the NED frame, with the heading determined independently.  "Speed" is usually
taken as ground speed in this context, as rate of climb is relatively small.

The rotorcraft rotor disk cants in the direction of the acceleration vector,
which at hover is vertical, but during turns and velocity transitions can be
inclined significantly.

Heading is subject to a weathercocking model.  Desired heading is held
independent of NED acceleration, until mWeatherCockSpeed is exceeded, after
which the heading slews to match the flight course.  When speed decays below
weathercock speed, desired heading takes over, and the heading slews back
to the desired value again.

Constraint values come from WsfPathGuidance, but there is some judgment used
as to what values to map to what for Rotorcraft use.
*/

class WSF_EXPORT WsfRotorcraftMover : public WsfIterativeRouteMover
{
public:
   WsfRotorcraftMover() = delete;
   WsfRotorcraftMover(WsfScenario& aScenario);
   WsfRotorcraftMover(const WsfRotorcraftMover& aRhs);
   WsfRotorcraftMover& operator=(const WsfRotorcraftMover&) = delete;
   ~WsfRotorcraftMover() override;

   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;

   // This is needed only for Data Logging infrastructure.
   // May be removed later as the algorithm becomes more mature.
   bool Initialize2(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   void SetMode(WsfStringId aModeId) override;

   void SetHeading(double aHeading) override;

   bool TurnToHeading(double aSimTime, double aHeading, double aRadialAccel, WsfPath::TurnDirection aTurnDirection) override;

   void SetOrientationNED(double aHeading, double aPitch, double aRoll) override;

   bool GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude) override;

   bool GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute) override;

   bool GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute) override;

   bool Unpause(double aSimTime, bool aInterrupt) override;

   // ============= Begin Mode ===================
   //! A 'mode' of the Rotorcraft Mover.
   class RotorcraftMode : public WsfMoverMode
   {
   public:
      static const double cNOT_SET;

      RotorcraftMode();
      RotorcraftMode(const RotorcraftMode& aSrc);
      RotorcraftMode& operator=(const RotorcraftMode&) = delete;
      ~RotorcraftMode() override                       = default;

      WsfMode* Clone() const override;
      bool     ProcessInput(UtInput& aInput) override;

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      double mWeathercockSpeed;
      double mMaximumAttitudeRate;
      double mMinimumUpwardAccel; // "mast bumping" constraint m/s2
      double mBodyRatesGain;
      double mMaximumGroundSpeed;   // m/s
      double mMaximumRateOfClimb;   // m/s
      double mMaximumRateOfDescent; // m/s
      double mMaximumTotalAccel;    // m/s2
   };
   // ============= End Mode ===================

   // This struct is filled in directly purely
   // from WsfWaypointPathGuidance or Script-Provided values:
   struct LocalGuidance
   {
      LocalGuidance();

      double mTargetGroundSpeed;
      double mTargetCourseRad;

      double mTargetLatitude;
      double mTargetLongitude;

      double mTargetAltitude;
      double mTargetHeadingRad; // (Persistent.)

      bool mLatLonIsValid;
      bool mCmdSpeedIsZero;
   };

protected:
   void SetModeId_P(WsfStringId aModeId);

   bool SetLocalGuidance(double aSimTime);

   void SetSwitches(double aSimTime);

   void SetPositionHold(double aSimTime, bool aValue);
   void SetAltitudeHold(double aSimTime, bool aValue);

   void LocalUpdate(double aSimTime, bool& aMoved, bool& aAdvanceToNextWaypoint) override;

   bool ComputeAccel(double aCurrentRate, double aCurrentValue, double aTargetValue, double aCriticalAccel, double& aCriticalTime);

   void ExportMotionState();

   double MaximumGroundSpeed() const;
   double MaximumAcceleration() const;
   double MaximumClimbRate() const;
   double MaximumDescentRate() const;
   double MinimumUpwardAccel() const;
   double BodyRatesGain() const;
   double WeathercockSpeed() const;
   double MaximumBodyRate() const;

   unsigned int mFlightIndex;

   WsfStringId mStartModeId;

   bool mIsWeatherCocking;
   bool mNeedToClimb;
   bool mIsAltitudeHold;
   bool mIsPositionHold;

   //! The mover-specific list of modes (not valid until Initialize is called)
   std::vector<RotorcraftMode*> mRotorcraftModeList;

   LocalGuidance mLocalGuidance;

   double mLocalUpdateTime;
   double mCourseRad;
   double mAltErrToROC_Gain;
   double mFabsROC_Limit;
   double mHeadingRad;
   double mHeadingErrorRad;
   double mSouthAndEastTipTimeConstant;
   double mPosHoldCaptureRadius;
   double mDesiredAccelUp;

   UtVec3dX mDesiredAccelNED;
   UtVec3dX mDesiredVelNED;

   UtVec3dX mVelNED;   // Reference value (WCS is actual)
   UtVec3dX mAccelNED; // Reference Value (WCS is actual)

   UtVec3dX mNetAccelNED; // Does not include gravity

   UtVec3dX mNetAccelWCS; // *** Integrated Value (no gravity incl)
   UtVec3dX mPrevNetAccelWCS;

   UtVec3dX mVelWCS; // *** Integrated Value
   UtVec3dX mPrevVelWCS;
   UtVec3dX mLocWCS; // *** Integrated Value
   UtVec3dX mPrevLocWCS;

   UtVec3dX mNED_Error;
   UtVec3dX mVelNED_Error;

   // All lateral acceleration commands get sent through
   // a first order filter after coming from the PID.
   UtControl::TF_1V mSouthTipFilter;
   UtControl::TF_1V mEastTipFilter;

   UtDCM mDcmWCStoNED;
   UtDCM mDcmNEDtoDisk;
   UtDCM mDcmDiskToECS;
   UtDCM mDcmNEDtoECS;

   UtPID_Controller mNorthAccelValuePID;
   UtPID_Controller mEastAccelValuePID;
   UtPID_Controller mVerticalAccelValuePID;
   UtPID_Controller mNorthAccelRatePID;
   UtPID_Controller mEastAccelRatePID;
   UtPID_Controller mVerticalAccelRatePID;

   WsfWaypointPathGuidance* mWPP_GuidancePtr;

   RotorcraftMode* mModePtr;

   double mLatitudeDeg;
   double mLongitudeDeg;
   double mAltitude;
   double mAltitudeRate;
   double mGroundSpeed;
   double mAltitudeError;
   double mTimeToTargetAlt;
   double mDesiredROC;
   double mReductionRatio;
   double mDistToTgtLL;
   double mTimeToTgtLL;
};

#endif
