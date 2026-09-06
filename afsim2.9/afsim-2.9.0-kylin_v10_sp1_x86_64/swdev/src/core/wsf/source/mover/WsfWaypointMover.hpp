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

#ifndef WSFWAYPOINTMOVER_HPP
#define WSFWAYPOINTMOVER_HPP

#include "wsf_export.h"

class WsfAngleOfAttack;
#include "WsfEvent.hpp"
#include "WsfMover.hpp"
#include "WsfPathComputer.hpp"
#include "WsfPathList.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteMover.hpp"
#include "WsfWaypoint.hpp"

//! A specialized mover that will move along a path defined by WsfRoute,
//! using a parametric, linearized, non-physics-based approach to motion.
//! This class contains the ability to move on the ground or in the air and to
//! change at any time (like an airplane that taxis and then takes-off).
//! WsfAirMover and WsfGroundMover are specific examples of classes that derive
//! from this class.  They differ only in the fact that they declare the default
//! state (ground or air).
class WSF_EXPORT WsfWaypointMover : public WsfRouteMover
{
public:
   WsfWaypointMover() = delete;
   WsfWaypointMover(const WsfScenario& aScenario);
   WsfWaypointMover(const WsfWaypointMover& aSrc);
   WsfWaypointMover& operator=(const WsfWaypointMover&) = delete;
   ~WsfWaypointMover() override;
   WsfMover* Clone() const override;

   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;

   void SetPlatform(WsfPlatform* aPlatformPtr) override;

   bool GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute) override;

   bool GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute) override;

   bool TurnToRelativeHeading(double                 aSimTime,
                              double                 aHeadingChange,
                              double                 aRadialAccel,
                              WsfPath::TurnDirection aTurnDirection) override;

   bool TurnToHeading(double aSimTime, double aHeading, double aRadialAccel, WsfPath::TurnDirection aTurnDirection) override;

   bool GetFutureLocationWCS(double aSimTime, double aLocationWCS[3]) override;

   WsfPathConstraints& GetConstraints() { return mMoverConstraints; }

   bool IsOnGround() const { return mIsOnGround; }

   void SetIsOnRoad(bool aIsOnRoad) { mIsOnRoad = aIsOnRoad; }
   bool IsOnRoad() const { return mIsOnRoad; }

   const WsfPathConstraints* GetMoverConstraints() const override;
   const WsfPathConstraints& GetCurrentConstraints() const { return mCurrentConstraints; }
   const WsfPathTarget&      GetTarget() const { return mTarget; }
   WsfPathComputer&          GetPathComputer() { return mPathComputer; }
   const WsfAngleOfAttack*   GetAngleOfAttackTable() { return mAngleOfAttackPtr; }

   bool GetPropertyDouble(const std::string& aPropertyName, double& aProperty) override;

   virtual double MaximumRadialAcceleration();          // returns meters/second^2
   double         TurnRadius();                         // TODO                //returns meters; assumed current speed
   double         TurnRadius(double atSpeed) override;  // TODO  //returns meters
   virtual double MaximumTurnRate();                    // returns radians/second
   virtual double CornerVelocity(WsfGeoPoint* aTarget); // returns meters/second

   void Pause(double aSimTime, double aUnpauseTime) override;

   bool Unpause(double aSimTime, bool aInterrupt) override;

   //! Informs the mover to use the current platform state
   //  * Used in SetRoute or to move platform
   void ResetPosition(double aSimTime) override;

   bool IsExtrapolating() const override;

   class BaseEvent : public WsfEvent
   {
   public:
      BaseEvent()
         : mMoverPtr(nullptr)
         , mEnabled()
      {
      }
      BaseEvent(WsfWaypointMover* aMoverPtr)
         : mMoverPtr(aMoverPtr)
         , mEnabled(true)
      {
      }
      ~BaseEvent() override
      {
         if (mEnabled)
         {
            mMoverPtr->mHitEventPtr = nullptr;
         }
      }
      void Disable() { mEnabled = false; }

   protected:
      WsfWaypointMover* mMoverPtr;
      bool              mEnabled;
   };

   class HitPointEvent : public WsfWaypointMover::BaseEvent
   {
   public:
      HitPointEvent() = default;
      HitPointEvent(WsfWaypointMover* aMoverPtr)
         : BaseEvent(aMoverPtr)
      {
      }
      ~HitPointEvent() override = default;
      EventDisposition Execute() override
      {
         if (mEnabled)
         {
            mEnabled = false;
            mMoverPtr->HitWaypoint(GetTime());
         }
         return cDELETE;
      }
   };

protected:
   void SetGuidanceFrom(const WsfWaypoint& aPreviousWaypoint) override;

   void SetGuidanceTo(const WsfWaypoint& aNextWaypoint) override;

   //!
   void AdvanceToNextWaypoint(double aSimTime, const WsfWaypoint& aTarget, const WsfWaypoint* aNextPointPtr) override;

   double GetHeightOfTerrain() const { return mHeightOfTerrain; }

   void UpdateHeightOfTerrain();

   void SetIsOnGround(bool aIsOnGround) { mIsOnGround = aIsOnGround; }

   virtual void PostMoveUpdate(double aSimTime) {}

   void EnableTerrain(bool aUseTerrain);

   void ExecutePath(double aSimTime);

   void UpdatePosition(double aSimTime) override;

   void HitWaypoint(double aSimTime);

   friend class HitPointEvent;

   void ResetPath(double aSimTime);

   void SetCompleteTime(double aSimTime);
   void SetComplete();

   void UpdateConstraints(const WsfWaypoint& aWaypoint);

   void CreatePath(const WsfWaypoint& aTargetPoint, const WsfWaypoint* aNextPointPtr = nullptr);

   void CreatePath(bool aResetDefaults = true);

   void Extrapolate(double aSimTime);

   void MoveToGround();

   void SetGroundAttitude(double aLatitude, double aLongitude);

   //! Computes the mover's path.
   WsfPathComputer mPathComputer;
   //! The sim-time at which mPath was begun
   double mPathStartTime;
   //! The current path being traversed by the platform
   WsfPathList mPath;
   //! If mDebugKeepPath is set, this contains entire path traversed by platform
   WsfPathList mTotalPath;
   //! Current 'goal' parameters
   WsfPathTarget mTarget;
   //! Event triggered when mover reaches waypoint
   HitPointEvent* mHitEventPtr;
   ////! Event triggered when mover unpauses
   // UnpauseEvent*        mUnpauseEventPtr;
   //! Constraints of the mover
   WsfPathConstraints mMoverConstraints;
   //! Currently used constraints
   WsfPathConstraints mCurrentConstraints;

   //! Default values are used when no other rate has yet been requested, or when a
   //! waypoint rate is set to 'default'
   //!{
   double mDefaultRadialAcceleration;
   double mDefaultLinearAcceleration;
   double mDefaultClimbRate;
   //!}

   //! Saves velocity prior to pause to be used after resume
   double mPrePauseVelocity[3];

   double mHeightOfTerrain;

   unsigned int mTargetTypeFlags;

   //! Is the platform moving along the ground
   bool mIsOnGround;
   //! Is the platform moving along roads
   bool mIsOnRoad;
   bool mPitchEnable;
   bool mDebugKeepPath;
   bool mDebugPathComputation;
   bool mUseTerrain;
   //! Mover will optionally recompute path on a regular interval if this is non-zero.
   double            mPathComputeTimestep;
   WsfAngleOfAttack* mAngleOfAttackPtr;
};

#endif
