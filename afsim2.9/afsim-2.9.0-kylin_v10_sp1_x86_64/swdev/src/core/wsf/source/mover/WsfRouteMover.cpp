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

#include "WsfRouteMover.hpp"

#include <iomanip>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfCallback.hpp"
#include "WsfDraw.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfNavigationMesh.hpp"
#include "WsfPathComputer.hpp"
#include "WsfPathConstraints.hpp"
#include "WsfPathFinder.hpp"
#include "WsfPathList.hpp"
#include "WsfPathState.hpp"
#include "WsfPlatform.hpp"
#include "WsfRouteComputer.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"

// =================================================================================================
WsfRouteMover::WsfRouteMover(const WsfScenario& aScenario)
   : WsfMover(aScenario)
   , mTerrainPtr(nullptr)
   , mForceUpdates(false)
   , mTurnFailOption(cCLOSEST_POINT)
   , mTurnFailureThreshold(0.01)
   , mUnpauseTime(-1.0)
   , // when less than zero it indicates it hasn't been initialize from mStartTime
   mAltitudeOffset(0.0)
   , mIsInitialized(false)
   , mUpdating(true)
   , mDisableTerrain(false)
   , mPrintRoutes(false)
   , mDrawRoutes(false)
   , mProcessPointStage(-1)
   , mCurrentIndex()
   , mTargetIndex()
   , mSwitch(WsfPath::cSWITCH_ON_PASSING)
   , mLastRouteTargetIndex(0)
   , mEndOfPath(WsfPath::cEP_EXTRAPOLATE)
   , mRouteComputerPtr(nullptr)
   , mUnpauseEventPtr(nullptr)
   , mAltRef(WsfPath::cALT_REF_MSL)
   , mStartTime(0.0, WsfRandomVariable::cNON_NEGATIVE)
{
}

// =================================================================================================
WsfRouteMover::WsfRouteMover(const WsfRouteMover& aSrc)
   : WsfMover(aSrc)
   , mTerrainPtr(aSrc.mTerrainPtr)
   , mForceUpdates(false)
   , mTurnFailOption(aSrc.mTurnFailOption)
   , mTurnFailureThreshold(aSrc.mTurnFailureThreshold)
   , mUnpauseTime(aSrc.mUnpauseTime)
   , mAltitudeOffset(aSrc.mAltitudeOffset)
   , mIsInitialized(aSrc.mIsInitialized)
   , mUpdating(aSrc.mUpdating)
   , mDisableTerrain(aSrc.mDisableTerrain)
   , mPrintRoutes(aSrc.mPrintRoutes)
   , mDrawRoutes(aSrc.mDrawRoutes)
   , mProcessPointStage(aSrc.mProcessPointStage)
   , mCurrentIndex(aSrc.mCurrentIndex)
   , mTargetIndex(aSrc.mTargetIndex)
   , mRoute(aSrc.mRoute)
   , mSwitch(aSrc.mSwitch)
   , mStartLabelId(aSrc.mStartLabelId)
   , mDefaultRoute(aSrc.mDefaultRoute)
   , mLastRouteTargetIndex(aSrc.mLastRouteTargetIndex)
   , mEndOfPath(aSrc.mEndOfPath)
   , mRouteComputerPtr(aSrc.mRouteComputerPtr)
   , mUnpauseEventPtr(nullptr)
   , // do not copy
   mAltRef(aSrc.mAltRef)
   , mStartTime(aSrc.mStartTime)
{
   if (aSrc.mTerrainPtr != nullptr)
   {
      mTerrainPtr = new wsf::Terrain(nullptr);
   }
}

// =================================================================================================
WsfRouteMover::~WsfRouteMover()
{
   if (mUnpauseEventPtr != nullptr)
   {
      mUnpauseEventPtr->Disable();
   }
   delete mTerrainPtr;
}

// =================================================================================================
// protected
void WsfRouteMover::DisableAndClearUnpause()
{
   // If the WsfSimulation engine already has a pause pending,
   // we cannot remove it, so the only thing we can do is disable it.
   if (mUnpauseEventPtr != nullptr)
   {
      mUnpauseEventPtr->Disable();
      mUnpauseEventPtr = nullptr;
   }
}

// ================================================================================================
//! Called when the pause time has expired to resume movement
void WsfRouteMover::TriggerUnpause(double aSimTime)
{
   mUnpauseEventPtr = nullptr;
   Unpause(aSimTime, false);
   Update(aSimTime);
}

// =================================================================================================
// virtual
bool WsfRouteMover::Initialize(double aSimTime)
{
   mForceUpdates = mUpdating = false;

   // Initialize script variable references...
   bool ok = mStartTime.Initialize("start_time", this, &(GetPlatform()->GetScriptContext()), *GetSimulation());

   if (mDisableTerrain)
   {
      delete mTerrainPtr;
      mTerrainPtr = nullptr;
   }
   else if (mTerrainPtr == nullptr)
   {
      mTerrainPtr = new wsf::Terrain(GetSimulation()->GetTerrainInterface());
   }
   else
   {
      mTerrainPtr->Initialize(GetSimulation()->GetTerrainInterface());
   }

   // Initialize the unpause time from start_time if not already done...
   InitializeUnpauseTime();

   unsigned int point = 0;
   if (mStartLabelId != 0)
   {
      for (unsigned int i = 0; i < mRoute.GetSize(); ++i)
      {
         if (mRoute[i].GetLabelId() == mStartLabelId)
         {
            point = i;
            break;
         }
      }
   }

   mDefaultRoute = mRoute;

   mIsInitialized = true;
   if (mRoute.GetSize() > point)
   {
      // This is somewhat convoluted.  mUnpauseTime is used as the start_time.
      // Pass the pause time to the start point, and remove it after
      // the waypoint has been processed.
      double pauseTime = mUnpauseTime;
      if (mUnpauseTime > 0 && !mRoute.Empty())
      {
         mRoute[point].SetPauseTime(mRoute[point].GetPauseTime() + pauseTime);
      }
      mUnpauseTime = 0.0;

      // Assume extrapolation for single-point initial routes, unless overridden in route
      if (mRoute.GetSize() == 1 && mRoute[0].GetEndOfPathOption() == WsfPath::cEP_MOVER_DEFAULT)
      {
         mRoute[0].SetEndOfPathOption(WsfPath::cEP_EXTRAPOLATE);
      }

      SetRoutePoint(aSimTime, mRoute, point);
      if (mRoute.GetSize() > point)
      {
         mRoute[point].SetPauseTime(mRoute[point].GetPauseTime() - pauseTime);
      }
   }
   else
   {
      // No route given.  Assume current kinematic state.
      UtVec3d velNED;
      GetPlatform()->GetVelocityNED(velNED.GetData());
      if (velNED.MagnitudeSquared() > .01)
      {
         mLastUpdateTime = aSimTime;
         GoToSpeed(aSimTime, velNED.Magnitude(), 0.0, false);
         double altDirection = UtMath::Sign(velNED[2]);
         if (altDirection * velNED[2] > .1)
         {
            GoToAltitude(aSimTime, altDirection * 1.0E20, altDirection * velNED[2], false);
         }
      }
      else
      {
         Pause(aSimTime, UtMath::cDOUBLE_MAX);
      }
   }

   mDefaultRoute = mRoute;
   ok &= WsfMover::Initialize(aSimTime);
   return ok;
}

// =================================================================================================
bool WsfRouteMover::Initialize2(double aSimTime)
{
   // Finally check if the mover is initially turned on or not
   // This check needs to occur after the WsfRouteMover::Initialize as this
   // handles the case where the mover has a route but is not on; the initial path will
   // be adequately initialized but no movement will occur
   if (!InitiallyTurnedOn())
   {
      Pause(aSimTime, UtMath::cDOUBLE_MAX);
   }

   return WsfMover::Initialize2(aSimTime);
}

// =================================================================================================
// virtual
void WsfRouteMover::Update(double aSimTime)
{
   if (mUpdating)
   {
      return;
   }
   mUpdating = true;
   double dt = aSimTime - mLastUpdateTime;
   if ((dt > mUpdateTimeTolerance) || (mForceUpdates && dt >= 0))
   {
      if (mUnpauseTime > 0.0 && mUnpauseTime < aSimTime)
      {
         mUnpauseTime = 0.0;
         ProcessNextPoint(aSimTime);
      }

      UpdatePosition(aSimTime);
      WsfMover::Update(aSimTime);

      // RefinePath here if using a navigation mesh
      bool onDefaultRoute = (GetDefaultRoute()->GetNameId() == GetRoute()->GetNameId()) ? true : false;
      if (GetRouteComputer() && GetRouteComputer()->GetEnabled() && GetRouteComputer()->IsNavMesh() && !onDefaultRoute)
      {
         int  curIndex    = GetCurrentIndex();
         int  targetIndex = GetTargetIndex();
         bool refine      = true;
         if (curIndex == targetIndex)
         {
            // the platform has reached its target, do nothing
            refine = false;
         }
         WsfRoute tmpRoute;
         double   LLA[3];
         GetPlatform()->GetLocationLLA(LLA[0], LLA[1], LLA[2]);
         WsfNavigationMesh* navMeshPtr = static_cast<WsfNavigationMesh*>(GetRouteComputer());
         double             updateTime = navMeshPtr->GetRefineUpdateTime();
         if (refine && updateTime > navMeshPtr->GetNavMeshThinkTime())
         {
            refine = true;
            navMeshPtr->SetRefineUpdateTime(0.0);
         }
         else
         {
            updateTime += dt;
            navMeshPtr->SetRefineUpdateTime(updateTime);
            refine = false;
         }

         if (refine && navMeshPtr->RefineFromCurrentPosition(0, LLA[0], LLA[1], 0.0, tmpRoute, GetRoute()))
         {
            double speed = GetRoute()->GetSpeed(0);
            if (speed < 0)
            {
               speed = mDefaultRoute.GetSpeed(0);
            }
            tmpRoute.SetSpeed(speed);
            SetRoute(aSimTime, tmpRoute);
         }
      }
   }
   mUpdating = false;
}

// =================================================================================================
// virtual
bool WsfRouteMover::ProcessInput(UtInput& aInput)
{
   std::unique_ptr<WsfRoute> routePtr(nullptr);
   bool                      myCommand = true;
   const std::string&        command   = aInput.GetCommand();
   if (WsfRouteTypes::Get(WsfScenario::FromInput(aInput)).LoadInstance(aInput, routePtr) && routePtr != nullptr)
   {
      mRoute = *routePtr;
   }
   else if (command == "start_at")
   {
      std::string startAtLabel;
      aInput.ReadValue(startAtLabel);
      mStartLabelId = startAtLabel;
   }
   else if (command == "at_end_of_path")
   {
      std::string endOfPathOption;
      aInput.ReadValue(endOfPathOption);
      if (endOfPathOption == "extrapolate")
      {
         mEndOfPath = WsfPath::cEP_EXTRAPOLATE;
      }
      else if (endOfPathOption == "stop")
      {
         mEndOfPath = WsfPath::cEP_STOP;
      }
      else if (endOfPathOption == "remove")
      {
         mEndOfPath = WsfPath::cEP_REMOVE;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "start_time")
   {
      mStartTime.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "switch_on_approach")
   {
      mSwitch = WsfPath::cSWITCH_ON_APPROACH;
   }
   else if (command == "switch_on_passing")
   {
      mSwitch = WsfPath::cSWITCH_ON_PASSING;
   }
   else if (command == "pathfinder")
   {
      std::string pathFinder;
      aInput.ReadValue(pathFinder);
      mRouteComputerPtr = WsfScenario::FromInput(aInput).GetPathFinderList().Find(pathFinder);
      if (mRouteComputerPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "PathFinder not found: " + pathFinder);
      }
   }
   else if (command == "terrain")
   {
      aInput.ReadValue(mDisableTerrain);
      mDisableTerrain = !mDisableTerrain;
   }
   else if (command == "altitude_offset")
   {
      aInput.ReadValueOfType(mAltitudeOffset, UtInput::cLENGTH);
   }
   else if (command == "print_route")
   {
      aInput.ReadValue(mPrintRoutes);
   }
   else if (command == "draw_route")
   {
      aInput.ReadValue(mDrawRoutes);
   }
   else if (command == "on_turn_failure")
   {
      std::string option;
      aInput.ReadValue(option);
      if (option == "best_effort")
      {
         mTurnFailOption = cCLOSEST_POINT;
      }
      else if (option == "reverse_turn")
      {
         mTurnFailOption = cREVERSE_TURN;
      }
      else if (option == "ignore_point")
      {
         mTurnFailOption = cSKIP_POINT;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "turn_failure_threshold")
   {
      aInput.ReadValue(mTurnFailureThreshold);
      aInput.ValueGreaterOrEqual(mTurnFailureThreshold, 0.0);
      aInput.ValueLessOrEqual(mTurnFailureThreshold, 1.0);
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
// virtual
void WsfRouteMover::TurnOff(double aSimTime)
{
   // Base class
   WsfMover::TurnOff(aSimTime);

   // Pause the mover if not already paused
   if (!IsPaused())
   {
      Pause(aSimTime, UtMath::cDOUBLE_MAX);
   }
}

// ================================================================================================
// virtual
void WsfRouteMover::TurnOn(double aSimTime)
{
   // Base class
   WsfMover::TurnOn(aSimTime);

   // Check for the many conditions that could cause the mover to unpause
   if (IsPaused())
   {
      // The following booleans are for readability and clarity

      // If the current time is the creation time then do not unpause if initialized in a paused state
      bool creationTime(aSimTime == GetPlatform()->GetCreationTime());

      // If a pause event is scheduled then do not unpause; just wait for the event
      bool pauseEventScheduled(mUnpauseEventPtr != nullptr);

      // If the pause time is set to double max then unpause
      bool pausedToInfinity(mUnpauseTime == UtMath::cDOUBLE_MAX);

      if (!creationTime && !pauseEventScheduled && pausedToInfinity &&
          !IsSwitchingPoints() &&          // Not currently in the middle of switching points
          (mTargetIndex != mCurrentIndex)) // and have not reached the target point yet
      {
         Unpause(aSimTime, false);
      }
   }
}

// =================================================================================================
//! Called when waypoint is reached
// * Execute callbacks
// * Process next waypoint, and execute AdvanceToNextWaypoint
void WsfRouteMover::WaypointReached(double aSimTime)
{
   ProcessNextPoint(aSimTime);
}

// =================================================================================================
//! Called by ProcessNextPoint() to perform one piece of processing the next point at a time.
//! In the event of a pause or route change, processing of some stages may be skipped or delayed
bool WsfRouteMover::ProcessNextPointStage(double aSimTime)
{
   bool advanced   = true;
   int  startStage = mProcessPointStage;

   if (mProcessPointStage <= 0) // Switch target stage
   {
      if (IsPaused())
      {
         advanced = false;
      }
      else if (mTargetIndex < (int)mRoute.GetSize())
      {
         int newTargetIndex = GetNextWaypointIndex(mTargetIndex);
         if (int(mRoute.GetSize()) > newTargetIndex)
         {
            mCurrentIndex = mTargetIndex;
            mTargetIndex  = newTargetIndex;
            SetGuidanceFrom(mRoute[mCurrentIndex]);
            SetGuidanceTo(mRoute[mTargetIndex]);
         }
         else
         {
            mCurrentIndex = mTargetIndex;
            mTargetIndex  = newTargetIndex;
         }
         mProcessPointStage = 1;
      }
      else
      {
         // It's possible to have an invalid target index.
         // This prevents executing triggers on the last point more than once
         mProcessPointStage = 2;
      }
   }
   else if (mProcessPointStage == 1) // Triggers stage
   {
      ++mProcessPointStage;
      ExecuteTriggers(aSimTime, mCurrentIndex);
   }
   else if (mProcessPointStage == 2) // Pause Stage
   {
      ++mProcessPointStage;
      WsfWaypoint& currentPoint = mRoute[mCurrentIndex];
      if (currentPoint.GetPauseTime() > 0.0)
      {
         // Waypoint contains a pause, wait until end of pause to execute remaining stages
         Pause(aSimTime, aSimTime + currentPoint.GetPauseTime());
         advanced = false;
      }
   }
   else if (mProcessPointStage == 3) // Final stage, go to next waypoint or End of path.
   {
      mProcessPointStage        = -1;
      WsfWaypoint& currentPoint = mRoute[mCurrentIndex];
      if (int(mRoute.GetSize()) > mTargetIndex)
      {
         DoAdvanceToNextWaypoint(aSimTime);
      }
      else
      {
         ExecuteEndOfPath(aSimTime, currentPoint);
         WsfObserver::MoverPathEnded(GetSimulation())(aSimTime, this);
      }
      advanced = false;
   }
   // If the state is changed by a callback, this prevents executing the remainder stages
   advanced = advanced && (mProcessPointStage > startStage);
   return advanced;
}

// =================================================================================================
//! Transitions to the next waypoint.
//! Returns 'true' if the next point is completely processed
//!         'false' is returned if this method requires another call
bool WsfRouteMover::ProcessNextPoint(double aSimTime)
{
   while (ProcessNextPointStage(aSimTime))
   {
   }
   return mProcessPointStage == -1;
}

// =================================================================================================
bool WsfRouteMover::GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute)
{
   WsfNavigationMesh* navMeshPtr = dynamic_cast<WsfNavigationMesh*>(GetRouteComputer());
   if (navMeshPtr != nullptr)
   {
      mDefaultRoute.SetSpeed(aSpeed);
      mRoute.SetSpeed(aSpeed);
   }
   return true;
}

// =================================================================================================
//! Executes any actions tied to the waypoint at aPointIndex
bool WsfRouteMover::ExecuteTriggers(double aSimTime, unsigned int aPointIndex)
{
   bool tookAction = false;
   if (aPointIndex < mRoute.GetSize())
   {
      WsfStringId  scriptId(mRoute[aPointIndex].GetScriptId());
      WsfCallback* callbackPtr = dynamic_cast<WsfCallback*>(mRoute[aPointIndex].GetCallback());

      WsfStringId mode = mRoute[aPointIndex].GetNewModeId();
      if ((mode != 0) && (GetModeId() != mode))
      {
         SetMode(mode);
         tookAction = true;
      }

      // Execute any action associated with reaching the current point.
      if (scriptId > nullptr)
      {
         if (GetPlatform()->HasScript(scriptId))
         {
            GetPlatform()->ExecuteScript(aSimTime, scriptId.GetString());
            tookAction = true;
         }
         else
         {
            tookAction = WsfCallback::ExecuteAndNotify(aSimTime, GetPlatform(), scriptId);
         }
      }

      if (callbackPtr != nullptr)
      {
         // Note: This looks strange, but we have to clone the callback
         //       because it's associated with a waypoint that might
         //       be deleted by the callback.  For example, if someone
         //       performs a reroute, the current route will be deleted
         //       along with the waypoint and this callback.
         WsfCallback* cbPtr = callbackPtr->Clone();
         cbPtr->Execute(aSimTime, GetPlatform());
         delete cbPtr;
         tookAction = true;
      }
   }

   return tookAction;
}

// =================================================================================================
//! Resets the mover to move along a route, starting at an offset point.
//! @param aSimTime The current simulation time.
//! @param aRoute The route to move along
//! @param aTargetPoint The waypoint index at which to start moving
//! @note This method is primarily for initialization, and will immediately move the
//!       platform to the position at the route start.
bool WsfRouteMover::SetRoutePoint(double aSimTime, const WsfRoute& aRoute, int aTargetPoint)
{
   RouteChanged();
   if (&mRoute != &aRoute)
   {
      mRoute = aRoute;
   }

   if (mRouteComputerPtr != nullptr && GetRouteComputer()->GetEnabled())
   {
      mRouteComputerPtr->ComputeSetRoute(*this, mRoute, aTargetPoint);
   }

   int pointIndex = aTargetPoint;

   NormalizeRoute(mRoute, true);

   if (!mIsInitialized)
   {
      return true;
   }
   if (mRoute.Empty())
   {
      return !mIsInitialized;
   }

   assert((int(aRoute.GetSize()) > pointIndex) && (aRoute[pointIndex].GetPointType() & WsfWaypoint::cLOCATION_MASK));

   WsfMover::Update(aSimTime);

   const WsfPathConstraints* pathConstraintsPtr = GetMoverConstraints();

   int nextPointIndex = pointIndex + 1;

   // Attempt to bump the path up to the correct height.
   if (GetRouteComputer() && GetRouteComputer()->GetEnabled() && GetRouteComputer()->IsNavMesh())
   {
      if (mTerrainPtr != nullptr && mTerrainPtr->IsEnabled())
      {
         double LLA[3];
         float  terrainAltitude;
         for (unsigned int i = 0; i < mRoute.GetSize(); ++i)
         {
            WsfWaypoint& wpt = mRoute[i];

            wpt.GetLocationLLA(LLA[0], LLA[1], LLA[2]);
            mTerrainPtr->GetElevInterp(LLA[0], LLA[1], terrainAltitude);
            wpt.SetAlt(terrainAltitude);
         }
      }
   }

   WsfWaypoint& start   = mRoute[pointIndex];
   double       initAlt = 0.0;
   if (start.GetAlt() != WsfPath::cDOUBLE_NOT_SET)
   {
      initAlt = start.GetAlt();
   }
   if ((pathConstraintsPtr != nullptr) && (pathConstraintsPtr->mMaxAltitude < initAlt))
   {
      initAlt = pathConstraintsPtr->mMaxAltitude;
   }

   GetPlatform()->SetLocationLLA(start.GetLat(), start.GetLon(), (initAlt + mAltitudeOffset));

   double heading = start.GetHeading();
   double speed   = 0.0;
   if (start.GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
   {
      speed = start.GetSpeed();
   }
   if ((pathConstraintsPtr != nullptr) && (pathConstraintsPtr->mMaxSpeed < speed))
   {
      speed = pathConstraintsPtr->mMaxSpeed;
   }

   if ((heading == WsfPath::cDOUBLE_NOT_SET) && (int(mRoute.GetSize()) > nextPointIndex))
   {
      heading             = 0.0;
      WsfWaypoint& target = mRoute[nextPointIndex];
      if (target.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
      {
         double distance;
         UtSphericalEarth::GreatCircleHeadingAndDistance(start.GetLat(),
                                                         start.GetLon(),
                                                         target.GetLat(),
                                                         target.GetLon(),
                                                         heading,
                                                         distance);
         heading *= UtMath::cRAD_PER_DEG;
      }
   }
   GetPlatform()->SetOrientationNED(heading, 0.0, 0.0);
   double velocityNED[3] = {cos(heading) * speed, sin(heading) * speed, 0.0};
   GetPlatform()->SetVelocityNED(velocityNED);

   mCurrentIndex = mTargetIndex = pointIndex;
   if (int(mRoute.GetSize()) <= nextPointIndex)
   {
      // Don't try to process the next waypoint
      mProcessPointStage = 1;
   }
   Unpause(aSimTime, true);
   ProcessNextPoint(aSimTime);

   if (mPrintRoutes)
   {
      auto out = ut::log::debug() << "Platform's Route set.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      auto routeNote = out.AddNote() << "Route:";
      mRoute.Print(routeNote);
   }
   if (mDrawRoutes || mPrintRoutes)
   {
      DrawDebugRoute();
   }
   WsfObserver::RouteChanged(GetSimulation())(this); // notify that the platform's route has changed
   return true;
}

// =================================================================================================
//! Attempt to find a WsfWaypoint label, and go there.
//! @param aSimTime The current simulated clock time (seconds).
//! @param aLabelId The ID of the label to go to.
//! @returns true if successful.
// virtual
bool WsfRouteMover::GoToLabel(double aSimTime, WsfStringId aLabelId)
{
   bool found = false;
   for (unsigned int i = 0; i < mRoute.GetSize(); ++i)
   {
      if (mRoute[i].GetLabelId() == aLabelId)
      {
         GoToWaypoint(aSimTime, i);
         found = true;
         break;
      }
   }
   return found;
}

// =================================================================================================
//! Attempt to find a WsfWaypoint index, and go there.
//! @param aSimTime The current simulated clock time (seconds).
//! @param aDestination Waypoint index to find.
//! @returns true if successful.
// virtual
bool WsfRouteMover::GoToWaypoint(double aSimTime, unsigned int aDestination)
{
   int destination = aDestination;
   if (destination >= int(mRoute.GetSize()))
   {
      destination = mRoute.GetSize() - 1;
   }
   if (destination != mTargetIndex)
   {
      RouteChanged();
      mTargetIndex = destination;
      SetGuidanceTo(mRoute[mTargetIndex]);
      DoAdvanceToNextWaypoint(aSimTime);
   }
   return true;
}

// =================================================================================================
// virtual
int WsfRouteMover::GetNextWaypointIndex(int aPointIndex)
{
   const WsfWaypoint& targetPoint = mRoute[aPointIndex];
   int                targetIndex = aPointIndex + 1;
   WsfStringId        gotoLabel   = targetPoint.GetGotoId();
   if (gotoLabel != 0)
   {
      int gotoIndex = mRoute.GetWaypointIndexAt(gotoLabel);
      if (gotoIndex < int(mRoute.GetSize()))
      {
         targetIndex = gotoIndex;
      }
   }
   return targetIndex;
}

// =================================================================================================
// virtual
bool WsfRouteMover::GoToClosestPoint(double aSimTime)
{
   unsigned int closestIndex    = 0;
   double       closestDSquared = UtMath::cDOUBLE_MAX;

   double curLocWCS[3];
   GetPlatform()->GetLocationWCS(curLocWCS);

   double testDSquared;
   double testWCS[3];
   double diffWCS[3];
   for (unsigned int i = 0; i != mRoute.GetSize(); ++i)
   {
      const WsfWaypoint& wp = mRoute[i];
      if (wp.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
      {
         UtEntity::ConvertLLAToWCS(wp.GetLat(), wp.GetLon(), wp.GetAlt(), testWCS);
         UtVec3d::Subtract(diffWCS, testWCS, curLocWCS);
         testDSquared = UtVec3d::MagnitudeSquared(diffWCS);
         if (testDSquared < closestDSquared)
         {
            closestIndex    = i;
            closestDSquared = testDSquared;
         }
      }
   }

   if (closestDSquared < UtMath::cDOUBLE_MAX)
   {
      GoToWaypoint(aSimTime, closestIndex);
   }
   return true;
}

// =================================================================================================
// virtual
bool WsfRouteMover::GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "WsfRouteMover commanded to go to location.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Mover: " << GetName();
      out.AddNote() << "Lat: " << aLatitude << " deg";
      out.AddNote() << "Lon: " << aLongitude << " deg";
      out.AddNote() << "Alt: " << aAltitude << " m";
   }
   WsfRoute    route;
   WsfWaypoint wpt;
   wpt.SetLat(aLatitude);
   wpt.SetLon(aLongitude);
   wpt.SetAlt(aAltitude);
   route.Append(wpt);
   bool value = UpdateRoute(aSimTime, route);
   return value;
}

// =================================================================================================
// virtual
bool WsfRouteMover::UpdateRoute(double aSimTime, const WsfRoute& aRoute)
{
   if (aRoute.Empty())
   {
      return false;
   }

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform's route updated.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform:  " << GetPlatform()->GetName();
      out.AddNote() << "Old Size: " << mRoute.GetSize();
      out.AddNote() << "New Size: " << aRoute.GetSize();
      out.AddNote() << "Target Point: " << mCurrentIndex;
   }

   Update(aSimTime);

   // If we are on the default route, save the target index.
   // It will be used during any subsequent call to ReturnToRoute().
   if (mRoute.GetNameId() == mDefaultRoute.GetNameId())
   {
      mLastRouteTargetIndex = mTargetIndex;
   }
   if (&mRoute != &aRoute)
   {
      mRoute.Clear();
      mRoute.SetName(aRoute.GetName());
      RouteChanged();
      mRoute = aRoute;
   }
   else
   {
      RouteChanged();
   }

   mCurrentIndex = 0;


   if (mRouteComputerPtr != nullptr && GetRouteComputer()->GetEnabled())
   {
      mRouteComputerPtr->ComputeUpdateRoute(*this, mRoute);
   }

   if (GetRouteComputer() && GetRouteComputer()->GetEnabled() && !GetRouteComputer()->IsNavMesh())
   {
      NormalizeRoute(mRoute, false);
   }
   else
   {
      NormalizeRoute(mRoute, false);
   }

   if (mRoute.GetSize() > 0)
   {
      // If paused, kick us into motion
      mRoute[0].SetPauseTime(0.0);
      mUnpauseTime = 0.0;

      // The turn direction guidance must (TDG) be reset on a route update. The reason is a little complex, but it goes
      // like this...  The TDG is a 'Guidance From' attribute, thus it is set when a waypoint is encountered. If the
      // route is continually updated, the first waypoint in the updated route may never be encountered, and therefore
      // the TDG never would never get updated. This causes any turn required to hit the first waypoint in the new route
      // to use whatever TDG was present at the time of the route update.  If the TDG was anything but turn-shortest,
      // any maneuvering would always result in turns in the same direction, regardless of what was best.

      mCurrentIndex   = 0;
      mTargetIndex    = 0;
      WsfWaypoint wpt = mRoute[0];
      wpt.SetTurnDirection(WsfPath::cTURN_DIR_SHORTEST);
      SetGuidanceFrom(wpt);
      SetGuidanceTo(wpt);
      DoAdvanceToNextWaypoint(aSimTime);
      WsfObserver::RouteChanged(GetSimulation())(this);
   }
   else
   {
      return false;
   }

   if (mPrintRoutes)
   {
      auto out = ut::log::info() << "Platform route updated.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      auto routeNote = out.AddNote() << "Route:";
      mRoute.Print(routeNote);
   }
   if (mDrawRoutes || mPrintRoutes)
   {
      DrawDebugRoute();
   }

   return true;
}

// =================================================================================================
// virtual
bool WsfRouteMover::UpdateDefaultRoute()
{
   mDefaultRoute         = mRoute;
   mLastRouteTargetIndex = mTargetIndex;
   return true;
}

// =================================================================================================
//! After going elsewhere, rejoin the route, reverting to the previous destination waypoint.
//! @returns true if successful.
// virtual
bool WsfRouteMover::ReturnToRoute(double aSimTime)
{
   bool computedRoute = false;
   if (mRouteComputerPtr != nullptr && GetRouteComputer()->GetEnabled())
   {
      computedRoute = mRouteComputerPtr->ReturnToRoute(*this, mRoute);
   }
   bool succeeded(false);
   if (computedRoute)
   {
      succeeded = UpdateRoute(aSimTime, mRoute);
   }
   else
   {
      // if mLastRouteTargetIndex == 0, mDefaultRoute is the current route, but we may be doing a GoTo()/TurnTo().
      int targetIndex = mTargetIndex;
      if (mLastRouteTargetIndex != 0)
      {
         targetIndex = mLastRouteTargetIndex;
      }
      if (UpdateRoute(aSimTime, mDefaultRoute) && GoToWaypoint(aSimTime, targetIndex))
      {
         succeeded = true;
      }

      // Reset the variable in case a later deviation occurs.
      mLastRouteTargetIndex = 0;
   }
   return succeeded;
}

// =================================================================================================
// virtual
bool WsfRouteMover::IsPaused() const
{
   if (mUnpauseTime < 0.0)
   {
      InitializeUnpauseTime();
   }
   return mUnpauseTime > 0.0;
}

// =================================================================================================
//! Begins the processing of the next waypoint in the route
void WsfRouteMover::DoAdvanceToNextWaypoint(double aSimTime)
{
   const WsfWaypoint& targetPoint    = mRoute[mTargetIndex];
   unsigned int       nextPointIndex = GetNextWaypointIndex(mTargetIndex);
   const WsfWaypoint* nextPointPtr   = nullptr;
   if (nextPointIndex < mRoute.GetSize())
   {
      nextPointPtr = &mRoute[nextPointIndex];
   }
   AdvanceToNextWaypoint(aSimTime, targetPoint, nextPointPtr);
}

// =================================================================================================
//! Execute the end of path option in aEndWaypoint
void WsfRouteMover::ExecuteEndOfPath(double aSimTime, const WsfWaypoint& aEndPoint)
{
   WsfPath::EndPath endOfPathOption = aEndPoint.GetEndOfPathOption();
   if (endOfPathOption == WsfPath::cEP_MOVER_DEFAULT)
   {
      endOfPathOption = mEndOfPath;
   }
   if (endOfPathOption == WsfPath::cEP_EXTRAPOLATE)
   {
      bool computedRoute = false;
      if (mRouteComputerPtr != nullptr && GetRouteComputer()->GetEnabled())
      {
         computedRoute = mRouteComputerPtr->ComputeExtrapolate(*this, mRoute);
      }
      if (computedRoute)
      {
         UpdateRoute(aSimTime, mRoute);
      }
      else
      {
         SetGuidanceFrom(aEndPoint);
         double heading = 0;
         if (aEndPoint.GetHeading() != WsfPath::cDOUBLE_NOT_SET && (aEndPoint.GetPointType() & WsfWaypoint::cLOCATION_MASK))
         {
            heading = aEndPoint.GetHeading();
         }
         else
         {
            double pitch, roll;
            GetPlatform()->GetOrientationNED(heading, pitch, roll);
         }
         TurnToHeading(aSimTime, heading, 0.0, WsfPath::cTURN_DIR_SHORTEST);
      }
   }
   else if (endOfPathOption == WsfPath::cEP_STOP)
   {
      if ((aEndPoint.GetHeading() != WsfPath::cDOUBLE_NOT_SET) &&
          ((aEndPoint.GetPointType() & WsfWaypoint::cHEADING_MASK) == 0))
      {
         // If the heading is set in the last waypoint, set orientation before pause
         GetPlatform()->SetOrientationNED(aEndPoint.GetHeading(), 0.0, 0.0);
      }
      Pause(aSimTime, UtMath::cDOUBLE_MAX);
      mUnpauseTime = UtMath::cDOUBLE_MAX;
   }
   else if (endOfPathOption == WsfPath::cEP_REMOVE)
   {
      GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
   }
}

// =================================================================================================
//! Called when the route is changed to reset processing of current point
void WsfRouteMover::RouteChanged()
{
   mProcessPointStage = -1;
}

namespace
{

//! Normalizes a route so a waypoint mover can more easily process the route.
//! * Converts AGL to MSL
//! * Computes true location of offset waypoints
//! * Removes coincident points
class RouteNormalizer
{
public:
   RouteNormalizer(WsfPlatform*    aPlatformPtr,
                   WsfPath::AltRef aAltRef     = WsfPath::cALT_REF_MSL,
                   wsf::Terrain*   aTerrainPtr = nullptr)
   {
      mPlatformPtr = aPlatformPtr;
      mRefSet      = false;
      mRefLat = mRefLon = mRefHeading = 0.0;
      mAltRef                         = aAltRef;
      mTerrainPtr                     = aTerrainPtr;
   }
   // ================================================================================================
   // Orients the platform onto the course from current way-point to the next way-point.
   // Used with Flight Simulation's geodetic databases.
   // param aPlatformPtr Platform pointer of platform holding way-points.
   void OrientToCourse(WsfPlatform* aPlatformPtr)
   {
      WsfRouteMover* routeMoverPtr = dynamic_cast<WsfRouteMover*>(aPlatformPtr->GetMover());
      if (routeMoverPtr != nullptr)
      {
         if ((routeMoverPtr->GetRoute() != nullptr) && (routeMoverPtr->GetRoute()->GetSize() > 0))
         {
            double headingInDegrees;
            double distance;
            double courseRad = 0.0;
            // look ahead one way-point
            if ((routeMoverPtr->GetTargetIndex()) < (int)routeMoverPtr->GetRoute()->GetSize())
            {
               const WsfWaypoint& wpC = routeMoverPtr->GetRoute()->GetWaypointAt(routeMoverPtr->GetCurrentIndex());
               const WsfWaypoint& wpT = routeMoverPtr->GetRoute()->GetWaypointAt(routeMoverPtr->GetTargetIndex());
               // calculate heading to next point to be used for heading for platform
               UtSphericalEarth::GreatCircleHeadingAndDistance(wpC.GetLat(),
                                                               wpC.GetLon(),
                                                               wpT.GetLat(),
                                                               wpT.GetLon(),
                                                               headingInDegrees,
                                                               distance);
               courseRad = headingInDegrees * UtMath::cRAD_PER_DEG;
            }
            else
            {
               // last point; check to see if it is extrapolating then let it go
               // or stopped, let it go
               if (!(routeMoverPtr->IsExtrapolating()) && !(routeMoverPtr->IsStopped()))
               {
                  const WsfWaypoint& wpC = routeMoverPtr->GetRoute()->GetWaypointAt(routeMoverPtr->GetCurrentIndex());
                  const WsfWaypoint& wpT = routeMoverPtr->GetRoute()->GetWaypointAt(0);
                  UtSphericalEarth::GreatCircleHeadingAndDistance(wpC.GetLat(),
                                                                  wpC.GetLon(),
                                                                  wpT.GetLat(),
                                                                  wpT.GetLon(),
                                                                  headingInDegrees,
                                                                  distance);
                  courseRad = headingInDegrees * UtMath::cRAD_PER_DEG;
               }
            }
            // This fix was put in, because the platform will move along the correct course, but not be oriented
            // on the course correctly.  If wind is a factor this will have to have wind calculated into courseRad.
            double myhdg, mypitch, myroll;
            aPlatformPtr->GetOrientationNED(myhdg, mypitch, myroll);
            aPlatformPtr->SetOrientationNED(courseRad, mypitch, myroll);
         }
      }
   }

   bool Normalize(WsfRoute& aRoute)
   {
      bool         ok = true;
      WsfPathState state(*mPlatformPtr);
      if (aRoute[0].GetAlt() != WsfPath::cDOUBLE_NOT_SET)
      {
         state.mAltitude = aRoute[0].GetAlt();
      }
      if (aRoute[0].GetLat() != WsfPath::cDOUBLE_NOT_SET)
      {
         state.mLatitude = aRoute[0].GetLat();
      }
      if (aRoute[0].GetLon() != WsfPath::cDOUBLE_NOT_SET)
      {
         state.mLongitude = aRoute[0].GetLon();
      }
      if (aRoute[0].GetHeading() != WsfPath::cDOUBLE_NOT_SET && aRoute[0].GetPointType() & WsfWaypoint::cLOCATION_MASK)
      {
         state.mOrientationNED[0] = aRoute[0].GetHeading();
      }
      if (aRoute[0].GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
      {
         state.mVelocityNED[0] = aRoute[0].GetSpeed() * std::cos(state.mOrientationNED[0]);
         state.mVelocityNED[1] = aRoute[0].GetSpeed() * std::sin(state.mOrientationNED[0]);
      }
      WsfPathComputer pc;     // Only necessary if there are turn waypoints
      WsfPathTarget   target; // Only necessary if there are turn waypoints
      size_t          lastTurnIndex = 0;
      bool            hasTurnIndex  = false;
      for (size_t i = 0; i < aRoute.GetSize(); ++i)
      {
         WsfWaypoint& wpt = aRoute[i];
         if (wpt.GetPointType() & WsfWaypoint::cHEADING_MASK)
         {
            lastTurnIndex = i;
            hasTurnIndex  = true;
         }
      }
      for (unsigned int i = 0; i < aRoute.GetSize();)
      {
         WsfWaypoint& wpt = aRoute[i];

         if (wpt.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
         {
            // If this is a coincident point, remove it
            if (i < aRoute.GetSize() - 1)
            {
               WsfWaypoint& next = aRoute[i + 1];
               if ((next.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE) && (next.GetLat() == wpt.GetLat()) &&
                   (next.GetLon() == wpt.GetLon()))
               {
                  aRoute.Remove(i);
                  continue;
               }
            }

            if (i == 0 && !mRefSet)
            {
               mRefSet     = true;
               mRefLat     = wpt.GetLat();
               mRefLon     = wpt.GetLon();
               mRefHeading = 0;
               if (wpt.GetHeading() != WsfPath::cDOUBLE_NOT_SET)
               {
                  mRefHeading = wpt.GetHeading();
               }
            }

            if (i > 0)
            {
               mRefSet = false;
            }
         }
         if (wpt.GetPointType() & WsfWaypoint::cRELATIVE_OFFSET)
         {
            if (mRefSet)
            {
               double x          = wpt.GetLat();
               double y          = wpt.GetLon();
               double range      = sqrt(x * x + y * y);
               double relBearing = atan2(y, x);
               double headingDeg = (relBearing + mRefHeading) * UtMath::cDEG_PER_RAD;
               double lat, lon;
               UtSphericalEarth::ExtrapolateGreatCirclePosition(mRefLat, mRefLon, headingDeg, range, lat, lon);
               wpt.SetLat(lat);
               wpt.SetLon(lon);
               mRefLat = wpt.GetLat();
               mRefLon = wpt.GetLon();
               wpt.SetPointType((wpt.GetPointType() & ~WsfWaypoint::cLOCATION_MASK) | WsfWaypoint::cLATITUDE_AND_LONGITUDE);
            }
            else
            {
               throw UtException("WsfRouteMover: Routes containing offsets must not have other waypoint types.");
            }
         }

         if (hasTurnIndex && i <= lastTurnIndex)
         {
            auto mover = dynamic_cast<WsfRouteMover*>(mPlatformPtr->GetMover());
            if (mover && mover->GetMoverConstraints())
            {
               if (wpt.GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
               {
                  target.mSpeed = wpt.GetSpeed();
                  target.mTargetType |= WsfPathTarget::cSPEED;
               }

               if (wpt.GetAlt() != WsfPath::cDOUBLE_NOT_SET)
               {
                  target.mAltitude = wpt.GetAlt();
                  target.mTargetType |= WsfPathTarget::cALTITUDE;
               }
               auto&              constraints = *(mover->GetMoverConstraints());
               WsfPathConstraints currentConstraints(constraints);
               pc.GetConstraints()          = constraints;
               const WsfWaypoint* nextPoint = nullptr;
               if (i < aRoute.GetSize() - 1)
               {
                  nextPoint = &aRoute[i + 1];
               }
               pc.CreateTarget(state, currentConstraints, constraints, aRoute[i], nextPoint, target);
               pc.ConstrainTarget(target, currentConstraints);
               WsfPathList path;
               pc.ComputePath(state, target, path);
               state = path.GetEndState();
            }
         }
         if (wpt.GetPointType() & WsfWaypoint::cHEADING_MASK)
         {
            wpt.SetLat(state.mLatitude);
            wpt.SetLon(state.mLongitude);
         }

         if (aRoute[i].GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
         {
            if ((aRoute[i].GetLat() < -90.0) || (aRoute[i].GetLat() > 90.0) || (aRoute[i].GetLon() < -180.0) ||
                (aRoute[i].GetLon() > 180.0))
            {
               auto out = ut::log::warning() << "Attempting to add an invalid lat/lon to the route of a platform.";
               if (mPlatformPtr != nullptr)
               {
                  out.AddNote() << "Platform: " << mPlatformPtr->GetName();
               }
               else
               {
                  out.AddNote() << "Platform: <undefined>";
               }
               out.AddNote() << "Index: " << i;
               out.AddNote() << "Lat: " << aRoute[i].GetLat();
               out.AddNote() << "Lon: " << aRoute[i].GetLon();
               aRoute.Remove(i);
               ok = false;
               continue;
            }
         }

         // If a location point, and AGL is requested
         bool requestedAGL = (wpt.GetAltRef() == WsfPath::cALT_REF_AGL) ||
                             (wpt.GetAltRef() == WsfPath::cALT_REF_MOVER_DEFAULT && mAltRef == WsfPath::cALT_REF_AGL);
         if ((wpt.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE) && requestedAGL)
         {
            float terrainElevation(0.0f);
            if (mTerrainPtr != nullptr)
            {
               mTerrainPtr->GetElevInterp(wpt.GetLat(), wpt.GetLon(), terrainElevation);
            }
            wpt.SetAlt(wpt.GetAlt() + terrainElevation);
         }
         else if (requestedAGL)
         {
            // Don't allow AGL on non-location points
            wpt.SetAltRef(WsfPath::cALT_REF_MOVER_DEFAULT);
            wpt.SetAlt(WsfPath::cDOUBLE_NOT_SET);
         }
         // If using databases created by Flight Simulation (Dave Findley)
         if (mTerrainPtr && mTerrainPtr->IsGeodeticDatabase())
         {
            // Orient the platform on the route segment. By not calling this,
            // the platform's heading will not match the course of the segment.
            OrientToCourse(mPlatformPtr);
         }

         if (i + 1 < aRoute.GetSize() && (aRoute[i + 1].GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE))
         {
            double dist;
            UtSphericalEarth::GreatCircleHeadingAndDistance(wpt.GetLat(),
                                                            wpt.GetLon(),
                                                            aRoute[i + 1].GetLat(),
                                                            aRoute[i + 1].GetLon(),
                                                            mRefHeading,
                                                            dist);
            mRefHeading *= UtMath::cRAD_PER_DEG;
         }
         ++i;
      }
      return ok;
   }

   void SetInitialState(double aLat, double aLon, double aHeading)
   {
      mRefSet     = true;
      mRefLat     = aLat;
      mRefLon     = aLon;
      mRefHeading = aHeading;
   }

   WsfPlatform*    mPlatformPtr;
   bool            mRefSet;
   double          mRefLat;
   double          mRefLon;
   double          mRefHeading;
   WsfPath::AltRef mAltRef;
   wsf::Terrain*   mTerrainPtr;
};
} // namespace

// =================================================================================================
void WsfRouteMover::NormalizeRoute(WsfRoute& aRoute, bool aIsInitialRoute)
{
   RouteNormalizer normalizer(GetPlatform(), mAltRef, mTerrainPtr);
   if (!aIsInitialRoute)
   {
      double lat, lon, alt;
      double yaw, pitch, roll;
      GetPlatform()->GetLocationLLA(lat, lon, alt);
      GetPlatform()->GetOrientationNED(yaw, pitch, roll);
      normalizer.SetInitialState(lat, lon, yaw);
   }
   else
   {
      // For an initial route, the first point is taken as the initial position of the platform.
      // If the first point isn't a lat/lon point, then insert the current position into the route.
      if (!aRoute.Empty())
      {
         if ((aRoute[0].GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE) == 0)
         {
            WsfWaypoint startPoint;
            double      lat, lon, alt;
            double      yaw, pitch, roll;
            GetPlatform()->GetLocationLLA(lat, lon, alt);
            GetPlatform()->GetOrientationNED(yaw, pitch, roll);
            startPoint.SetLat(lat);
            startPoint.SetLon(lon);
            startPoint.SetHeading(yaw);
            if (aRoute[0].GetAlt() != WsfPath::cDOUBLE_NOT_SET)
            {
               startPoint.SetAlt(aRoute[0].GetAlt());
            }
            else
            {
               startPoint.SetAlt(alt);
            }
            if (aRoute[0].GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
            {
               startPoint.SetSpeed(aRoute[0].GetSpeed());
            }
            aRoute.Insert(aRoute.Begin(), startPoint);
            normalizer.SetInitialState(lat, lon, yaw);
         }
      }
   }
   normalizer.Normalize(aRoute);
}

// =================================================================================================
void WsfRouteMover::AddUnpauseEventFor(WsfRouteMover* aMoverPtr, double aUnpauseTime)
{
   // First consider if an existing pause is in effect.  Null it.
   DisableAndClearUnpause();

   if (aUnpauseTime != UtMath::cDOUBLE_MAX)
   {
      assert(mUnpauseEventPtr == nullptr);
      auto eventPtr    = ut::make_unique<UnpauseEvent>(aMoverPtr, aUnpauseTime);
      mUnpauseEventPtr = eventPtr.get();
      aMoverPtr->GetSimulation()->AddEvent(std::move(eventPtr));
   }
}

// =================================================================================================
//! Resume movement
//! @param aSimTime The current simulation time
//! @param aInterrupt 'true' if the unpause was not scheduled
bool WsfRouteMover::Unpause(double aSimTime, bool aInterrupt)
{
   bool unpaused = false;
   if (mUnpauseTime > 1.0E-20)
   {
      unpaused     = true;
      mUnpauseTime = 0.0;
      // Don't continue processing the current point if pause was interrupted
      if (aInterrupt)
      {
         mProcessPointStage = -1;
      }
      else
      {
         WsfMover::Update(aSimTime);
         // Resume going to current point, but don't go to next point.
         mProcessPointStage = 3;
         ProcessNextPoint(aSimTime);
         UpdatePosition(aSimTime);
      }
   }
   return unpaused;
}

// =================================================================================================
//! This method uses mRouteComputerPtr to build a route between aStartPtr and
//! aEndPtr, and then sets the current mRoute to that route.
//! Returns false if the current route is not changed.
bool WsfRouteMover::FindAndSetPath(double aSimTime, WsfGeoPoint* aStartPtr, WsfGeoPoint* aEndPtr)
{
   bool routeSet = false;
   if (mRouteComputerPtr != nullptr && !mRoute.Empty() && GetRouteComputer()->GetEnabled())
   {
      WsfRoute    newRoute;
      WsfWaypoint wpt;
      wpt.SetLat(aStartPtr->GetLat());
      wpt.SetLon(aStartPtr->GetLon());
      wpt.SetAlt(0.0);
      wpt.SetSpeed(mRoute[0].GetSpeed());
      newRoute.Append(wpt);
      wpt.SetLat(aEndPtr->GetLat());
      wpt.SetLon(aEndPtr->GetLon());
      newRoute.Append(wpt);
      newRoute.SetName("routeMoverRoute");
      if (mRouteComputerPtr->ComputeFindPath(*this, newRoute))
      {
         newRoute.SetSpeed(mRoute.GetSpeed(mCurrentIndex));
         newRoute.SetName("routeMoverRoute");
         routeSet = true;
         SetRoute(aSimTime, newRoute);
      }
   }
   return routeSet;
}

// =================================================================================================
void WsfRouteMover::SetRouteComputer(WsfRouteComputer* aRouteComputerPtr)
{
   mRouteComputerPtr = aRouteComputerPtr;
}

// =================================================================================================
// Draws a visual of the current route with WsfDraw
void WsfRouteMover::DrawDebugRoute()
{
   WsfDraw     draw(*GetSimulation());
   std::string drawId = GetPlatform()->GetName() + "_route";
   draw.Erase(WsfStringId(drawId));
   draw.SetId(WsfStringId(drawId));
   draw.SetColor(0, 0, 1);
   draw.BeginPolyline();
   draw.VertexRoute(mRoute);
   draw.End();
   draw.SetPointSize(5);
   draw.SetColor(1, 1, 1);
   draw.BeginPoints();
   draw.VertexRoute(mRoute);
   draw.End();
}

// =================================================================================================
//! Initialize the 'unpause time'.
//! Because of the way things get initialized, IsPaused may get called before Initialize (for instance,
//! WsfWaypointMover::Initialize calls IsPaused prior to invoking WsfRouteMover::Initialize). This method
//! ensures mUnpauseTime is initialized from 'mStartTime' if it is necessary.
//!
//! @note This method must be 'const' and 'mUnpauseTime' and 'mStartTime' must be declared mutable because
//! this is called from IsPaused, which itself is 'const'.
// private
void WsfRouteMover::InitializeUnpauseTime() const
{
   if ((mUnpauseTime < 0.0) && (!mIsInitialized))
   {
      mUnpauseTime = 0.0;

      // NOTE: mStartTime must be initialized here and in Initialize() because we may get called here
      //       *BEFORE* Initialize() is called (see comments above). Multiple initialization of
      //       WsfRandomVariable is ok...   { cast is required to get rid of const-ness of 'this' }
      if (mStartTime.Initialize("start_time",
                                const_cast<WsfRouteMover*>(this),
                                &(GetPlatform()->GetScriptContext()),
                                *GetSimulation()))
      {
         mUnpauseTime = mStartTime.Draw();
         if (mUnpauseTime < GetPlatform()->GetCreationTime())
         {
            mUnpauseTime = 0.0;
         }
      }
   }
}
// Returns the constraints of the mover if the mover implementation implements this method
const WsfPathConstraints* WsfRouteMover::GetMoverConstraints() const
{
   return nullptr;
}
