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

#ifndef WSFROUTEMOVER_HPP
#define WSFROUTEMOVER_HPP

#include "wsf_export.h"

#include "WsfEvent.hpp"
#include "WsfMover.hpp"
class WsfPathFinder;
#include "WsfRandomVariable.hpp"
#include "WsfRoute.hpp"
class WsfRouteComputer;
#include "WsfWaypoint.hpp"
struct WsfPathConstraints;

namespace wsf
{
class Terrain;
}

//! A derivation of WsfMover that may follow a WsfRoute path.
//! This type is a base class for other WsfMovers that primarily follow a WsfRoute.  The
//! class handles reading in the desired route, setting, incrementing, and resetting the
//! indices of the current and target waypoints, pause logic, etc.  The mover may begin
//! at a position and a specified velocity (even zero), then be given a WsfRoute to follow,
//! and may be commanded to depart the route, to rejoin it later.  Derived classes need to
//! implement the pure virtual methods, as well as GoToAltitude, GoToSpeed, TurnToRelativeHeading,
//! and TurnToHeading methods.
class WSF_EXPORT WsfRouteMover : public WsfMover
{
public:
   enum MissTurnOption
   {
      cCLOSEST_POINT = 0,
      cREVERSE_TURN  = 1,
      cSKIP_POINT    = 2
   };

   WsfRouteMover() = delete;
   WsfRouteMover(const WsfScenario& aScenario);
   WsfRouteMover(const WsfRouteMover& aSrc);
   WsfRouteMover& operator=(const WsfRouteMover&) = delete;
   ~WsfRouteMover() override;

   bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;
   void Update(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;

   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;

   const WsfRoute* GetRoute() const override { return &mRoute; }

   const WsfRoute* GetDefaultRoute() const override { return &mDefaultRoute; }

   int RoutePointIndex() const override { return mTargetIndex; }

   bool SetRoute(double aSimTime, const WsfRoute& aRoute) override { return SetRoutePoint(aSimTime, aRoute, 0); }

   bool GoToLabel(double aSimTime, WsfStringId aLabelId) override;

   bool GoToWaypoint(double aSimTime, unsigned int aDestination) override;

   bool GoToClosestPoint(double aSimTime) override;

   bool FindAndSetPath(double aSimTime, WsfGeoPoint* aStartPtr, WsfGeoPoint* aEndPtr) override;

   bool GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude) override;

   //! Sets the mover to a paused state
   virtual void Pause(double aSimTime, double aUnpauseTime) = 0;

   virtual bool Unpause(double aSimTime, bool aInterrupt);

   bool UpdateRoute(double aSimTime, const WsfRoute& aRoute) override;

   bool UpdateDefaultRoute() override;

   virtual bool SetRoutePoint(double aSimTime, const WsfRoute& aRoute, int aTargetPoint);

   bool ReturnToRoute(double aSimTime) override;

   //! Returns true if the mover is paused
   bool IsPaused() const override;

   WsfRouteComputer* GetRouteComputer() const { return mRouteComputerPtr; }
   void              SetRouteComputer(WsfRouteComputer* aRouteComputerPtr);

   //! Sets the default end-of-path option for the mover
   void SetEndOfPath(WsfPath::EndPath aEndOfPathOption) { mEndOfPath = aEndOfPathOption; }

   //! Return next waypoint index
   int  GetTargetIndex() const { return mTargetIndex; }
   void SetTargetIndex(int aIndex) { mTargetIndex = aIndex; }
   //! Return current waypoint index
   int GetCurrentIndex() const { return mCurrentIndex; }

   bool GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute) override;

   MissTurnOption                    GetTurnFailOption() const { return mTurnFailOption; }
   double                            GetTurnFailThreshold() const { return mTurnFailureThreshold; }
   WsfPath::Switch                   GetSwitch() const { return mSwitch; }
   virtual const WsfPathConstraints* GetMoverConstraints() const;

   class UnpauseEvent : public WsfEvent
   {
   public:
      UnpauseEvent(WsfRouteMover* aMoverPtr, double aUnpauseTime)
         : WsfEvent(aUnpauseTime)
         , mMoverPtr(aMoverPtr)
         , mEnabled(true)
      {
      }

      // If this event is deleted
      ~UnpauseEvent() override
      {
         if (mEnabled)
         {
            mMoverPtr->mUnpauseEventPtr = nullptr;
         }
      }

      EventDisposition Execute() override
      {
         if (mEnabled)
         {
            mEnabled = false;
            mMoverPtr->TriggerUnpause(GetTime());
         }
         return cDELETE;
      }

      void Disable() { mEnabled = false; }

   private:
      WsfRouteMover* mMoverPtr;
      bool           mEnabled;
   };

protected:
   void TriggerUnpause(double aSimTime);

   void            SetAltRef(WsfPath::AltRef aAltRef) { mAltRef = aAltRef; }
   WsfPath::AltRef GetAltRef() const { return mAltRef; }

   // bool RouteIsValid(double          aSimTime,
   //                   const WsfRoute& aRoute);

   //! Mover implementing this method should adjust current constraints to comply
   //! with those in aPreviousWaypoint.
   virtual void SetGuidanceFrom(const WsfWaypoint& aPreviousWaypoint) = 0;

   //! Mover implementing this method should adjust current constraints to comply
   //! with those in aNextWaypoint.
   virtual void SetGuidanceTo(const WsfWaypoint& aNextWaypoint) = 0;

   //! Mover implementing this method should begin movement toward aTarget.
   //! WaypointReached() should be called when target waypoint is reached.
   //! @param aSimTime The current simulation time.
   //! @param aTarget The new target waypoint
   //! @param aNextPointPtr The waypoint in the route after aTarget.  May be null.
   virtual void AdvanceToNextWaypoint(double aSimTime, const WsfWaypoint& aTarget, const WsfWaypoint* aNextPointPtr) = 0;

   virtual void UpdatePosition(double aSimTime) = 0;

   virtual void WaypointReached(double aSimTime);

   void RouteChanged();

   bool IsSwitchingPoints() { return mProcessPointStage != -1; }

   void AddUnpauseEventFor(WsfRouteMover* aMoverPtr, double aUnpauseTime);
   void DisableAndClearUnpause();

   wsf::Terrain* mTerrainPtr;
   //! 'true' if the mover should update event if within update tolerance
   //! this is set to get finer accuracy on updates prior to switching waypoints
   bool mForceUpdates;

   //! If true, the mover will ensure it passes each point
   MissTurnOption mTurnFailOption;
   //! Threshold of turn failure as percentage of turn radius
   double mTurnFailureThreshold;
   //! Simulation time at which the mover will unpause
   mutable double mUnpauseTime;

   //! Altitude offset of the parent platform's center of gravity / mass (CG).
   double mAltitudeOffset;

private:
   void InitializeUnpauseTime() const;

   void NormalizeRoute(WsfRoute& aRoute, bool aIsInitialRoute);

   void DoAdvanceToNextWaypoint(double aSimTime);

   int GetNextWaypointIndex(int aPointIndex);

   bool ProcessNextPoint(double aSimTime);

   bool ProcessNextPointStage(double aSimTime);

   bool ExecuteTriggers(double aSimTime, unsigned int aPointIndex) override;

   void ExecuteEndOfPath(double aSimTime, const WsfWaypoint& aEndPoint);

   void DrawDebugRoute();

   //! Is the mover initialized?
   bool mIsInitialized;
   //! Variable to prevent re-entering Update()
   bool mUpdating;
   //! If true, terrain will not be used for this mover
   bool mDisableTerrain;
   //! If true, routes passed to SetRoute and UpdateRoute are printed
   bool mPrintRoutes;
   //! If true, routes passed to SetRoute and UpdateRoute are drawn with WsfDraw
   bool mDrawRoutes;
   //! The next stage of processing to be done for the current point
   int mProcessPointStage;
   //! Index of the last waypoint passed by the mover
   int mCurrentIndex;
   //! Index of the next waypoint to be passed
   //! This will be out of range if at end of path
   int mTargetIndex;
   //! Copy of the route the mover is executing
   WsfRoute mRoute;
   //! Default waypoint switch mode
   WsfPath::Switch mSwitch;
   //! If set, label of starting waypoint used on Initialize
   WsfStringId mStartLabelId;

   WsfRoute mDefaultRoute;
   //! Index used when resuming default route
   int mLastRouteTargetIndex;
   //! Default end of path option
   WsfPath::EndPath mEndOfPath;

   WsfRouteComputer* mRouteComputerPtr;

   //! Event triggered when mover unpauses
   UnpauseEvent* mUnpauseEventPtr;

   //! Default altitude reference used to interpret waypoints
   WsfPath::AltRef mAltRef;

   //! A potentially random start time at which to start moving.
   mutable WsfRandomVariable mStartTime;
};

#endif
