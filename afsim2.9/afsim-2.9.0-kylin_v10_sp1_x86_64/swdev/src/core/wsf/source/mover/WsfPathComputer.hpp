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

#ifndef WSFPATHCOMPUTER_HPP
#define WSFPATHCOMPUTER_HPP

#include "wsf_export.h"

#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "WsfPathConstraints.hpp"
#include "WsfPathList.hpp"
#include "WsfWaypoint.hpp"

struct WsfPathTarget
{
   WsfPathTarget()
      : mTargetType(cLOCATION)
      , mLatitude(0.0)
      , mLongitude(0.0)
      , mAltitude(0.0)
      , mHeading(0.0)
      , mSpeed(0.0)
      , mDistance(0.0)
      , mTime(0.0)
      , mTurnDirection(cTURN_DIR_SHORTEST)
      , mNextPointLat(0.0)
      , mNextPointLon(0.0)
      , mNextPointRadialAccel(0.0)
      , mTurnFailureThreshold(0.01)
   {
   }

   enum TargetType
   {
      cLOCATION         = 0x1,
      cALTITUDE         = 0x2,
      cHEADING          = 0x4,
      cSPEED            = 0x8,
      cEXTRAPOLATE      = 0x10,
      cTURN_ON_APPROACH = 0x20,
      cRELATIVE_TURN    = 0x40,
      cREQUIRED_POINT   = 0x80,
      cOPTIONAL_POINT   = 0x100,
      cTIME_TO_POINT    = 0x200
   };

   enum TurnDirection
   {
      cTURN_DIR_LEFT     = -1, //!< Turn to the left, even if right is closer.
      cTURN_DIR_SHORTEST = 0,  //!< Turn in the direction that results in the shortest turn (DEFAULT).
      cTURN_DIR_RIGHT    = 1   //!< Turn to the right, even if left is closer.
   };

   void Print(ut::log::MessageStream& aStream);

   //! Bit combination of TargetType
   unsigned int mTargetType;
   //! Specifies the target latitude.  Must be used with (mTargetType & cLOCATION)
   double mLatitude;
   //! Specifies the target longitude.  Must be used with (mTargetType & cLOCATION)
   double mLongitude;
   //! Specifies the target altitude.  Must be used with (mTargetType & cALTITUDE)
   double mAltitude;
   //! Specifies the target heading.  Must be used with (mTargetType & cHEADING)
   double mHeading;
   //! Specifies the target speed.  Must be used with (mTargetType & cSPEED)
   double mSpeed;
   //! Specifies the distance traveled before completion
   double mDistance;
   //! Specifies the length of time to travel before completion
   double mTime;
   //! Specifies the direction used to complete a turn.
   TurnDirection mTurnDirection;
   //! Specifies the location and radial acceleration used to get to the point
   //! after the target point.  Only used if point type includes cTURN_ON_APPROACH.
   //!{
   double mNextPointLat;
   double mNextPointLon;
   double mNextPointRadialAccel;
   //!}
   double mTurnFailureThreshold;
};

//! WsfPathComputer computes a WsfPathList for a platform to reach a target state.
class WSF_EXPORT WsfPathComputer
{
public:
   WsfPathComputer(ut::Random* aRandomPtr = nullptr);

   WsfPathConstraints& GetConstraints() { return mConstraints; }

   // Appends instructions to aPathList to transition from State to TargetState
   void ComputePath(const WsfPathState& aState, const WsfPathTarget& aTarget, WsfPathList& aPathList);

   void CreateTarget(const WsfPathState&       aCurrentState,
                     WsfPathConstraints&       aCurrentConstraints,
                     const WsfPathConstraints& aMoverConstraints,
                     const WsfWaypoint&        aTargetPoint,
                     const WsfWaypoint*        aNextPointPtr,
                     WsfPathTarget&            aTarget);

   void ConstrainTarget(WsfPathTarget& aTarget, const WsfPathConstraints& aConstraints);

   void RevisePathHitPointAtTime(WsfPathTarget& aTarget, WsfPathList& aPath);

   //! Sets the maximum turn (radians).  Turns greater than the maximum turn are ignored.
   //! Determines how hard we'll try to get to points behind us.
   void SetMaximumTurn(double aMaxTurn) { mMaximumTurn = aMaxTurn; }

   double GetMaximumTurn() const { return mMaximumTurn; }

   void SetPathVarianceRadius(double aPathVarianceRadius) { mPathVarianceRadius = aPathVarianceRadius; }
   void SetSpeedVariancePct(double aSpeedVariancePct) { mSpeedVariancePct = aSpeedVariancePct; }

   void SetDefaultSwitch(WsfPath::Switch aDefaultSwitch) { mDefaultSwitch = aDefaultSwitch; }

   //! Sets the random device for computation of 'variance' to routes
   //! The random device must remain valid until the path computer is destroyed.
   void SetRandom(ut::Random* aRandomPtr) { mRandomPtr = aRandomPtr; }

   double GetTurnRadius(double aSpeed);

   void TurnToHeading(const WsfPathState&          aState,
                      const WsfPathTarget&         aTarget,
                      WsfPathList&                 aPathList,
                      WsfPathTarget::TurnDirection aTurnDirection);

   void TurnRelative(const WsfPathState&  aState,
                     const WsfPathTarget& aTarget,
                     WsfPathList&         aPathList,
                     double               aTurnAngle,
                     double               aTurnRadius = -1.0);

   bool TurnToPointHeading(const WsfPathState& aState, const WsfPathTarget& aTarget, WsfPathList& aPath);

   void StraightToPoint(const WsfPathState& aState, const WsfPathTarget& aTarget, WsfPathList& aPath, bool aAllowHeadingChange);

   void Rotate(UtVec3d              aRotationAxis,
               double               aArcLength,
               double               aTime,
               const WsfPathTarget& aTarget,
               const WsfPathState&  aState,
               WsfPathList&         aPath,
               double               aRotationRadius,
               double               aTargetRoll = 0.0);

   void StraightOnHeading(const WsfPathState& aState, const WsfPathTarget& aTarget, WsfPathList& aPath);

   struct SpeedAltitudeBreakpoint
   {
      double mSimTime;
      double mLinearAcceleration;
      double mClimbRate;
      double mFlightAngle;
   };

   bool ComputeSpeedAltitudeTargets(const WsfPathState&      aState,
                                    const WsfPathTarget&     aTarget,
                                    SpeedAltitudeBreakpoint& aBreakpoint);

   bool UsingComputedClimbRate() { return mUsingComputedClimbRate; }
   void SetUsingComputedClimbRate(bool aBool) { mUsingComputedClimbRate = aBool; }
   void ForceFlyFlightPath(bool aBool) { mFlyFlightPathToTarget = aBool; }

   ut::Random*        mRandomPtr;
   WsfPathConstraints mConstraints;
   double             mSimTime;
   double             mIntegrationTime;

   double mMaximumTurn;

   double mPathVarianceRadius;
   double mSpeedVariancePct;

   //! The default criteria used to determine when a waypoint switch should occur
   //! if the waypoint does not specify a switch criteria.
   WsfPath::Switch mDefaultSwitch;

   //! 'true' if the current target has a computed flight angle
   bool mUsingComputedClimbRate;
   //! 'true' if the current target has a computed linear acceleration
   bool mUsingComputedLinearAcceleration;
   //! 'true' if the current target has a computed speed.
   bool mUsingComputedSpeed;
   //! 'true' use calculated flight path constraints to fly along slope to next target point
   bool mFlyFlightPathToTarget;

private:
   bool TurnToPoint(const WsfPathState& aState, const WsfPathTarget& aTarget, WsfPathList& aPath, int aIterations);
};

#endif
