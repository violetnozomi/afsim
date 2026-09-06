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

#include "WsfWaypointMover.hpp"

#include <algorithm>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtMeasurementUtil.hpp"
#include "WsfAngleOfAttack.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfUtil.hpp"

// Helper functions / constants
namespace
{
//! Radial acceleration used for ground objects.
constexpr double cGROUND_RADIAL_ACCELERATION = 1.0E9;
//! Used to test if a double is finite.  This is less than std::numeric_limits<double>::max().
inline bool IsFinite(double aValue)
{
   return aValue < 1.0E250;
}
} // namespace

// ================================================================================================
WsfWaypointMover::WsfWaypointMover(const WsfScenario& aScenario)
   : WsfRouteMover(aScenario)
   , mPathComputer()
   , mPathStartTime()
   , mPath()
   , mTotalPath()
   , mTarget()
   , mHitEventPtr(nullptr)
   , mMoverConstraints()
   , mCurrentConstraints()
   , mDefaultRadialAcceleration(WsfPath::cDOUBLE_NOT_SET)
   , mDefaultLinearAcceleration(WsfPath::cDOUBLE_NOT_SET)
   , mDefaultClimbRate(WsfPath::cDOUBLE_NOT_SET)
   , mPrePauseVelocity{0.0, 0.0, 0.0}
   , mHeightOfTerrain(0.0)
   , mTargetTypeFlags(0)
   , mIsOnGround(false)
   , mIsOnRoad(false)
   , mPitchEnable(true)
   , mDebugKeepPath(false)
   , mDebugPathComputation(false)
   , mUseTerrain(false)
   , mPathComputeTimestep(0.0)
   , mAngleOfAttackPtr(nullptr)
{
}

// ================================================================================================
WsfWaypointMover::WsfWaypointMover(const WsfWaypointMover& aSrc)
   : WsfRouteMover(aSrc)
   , mPathComputer(aSrc.mPathComputer)
   , mPathStartTime(aSrc.mPathStartTime)
   , mPath(aSrc.mPath)
   , mTotalPath(aSrc.mTotalPath)
   , mHitEventPtr(nullptr)
   , mMoverConstraints(aSrc.mMoverConstraints)
   , mDefaultRadialAcceleration(aSrc.mDefaultRadialAcceleration)
   , mDefaultLinearAcceleration(aSrc.mDefaultLinearAcceleration)
   , mDefaultClimbRate(aSrc.mDefaultClimbRate)
   , mHeightOfTerrain(0.0)
   , mTargetTypeFlags(aSrc.mTargetTypeFlags)
   , mIsOnGround(aSrc.mIsOnGround)
   , mIsOnRoad(aSrc.mIsOnRoad)
   , mPitchEnable(aSrc.mPitchEnable)
   , mDebugKeepPath(aSrc.mDebugKeepPath)
   , mDebugPathComputation(aSrc.mDebugPathComputation)
   , mUseTerrain(aSrc.mUseTerrain)
   , mPathComputeTimestep(aSrc.mPathComputeTimestep)
   , mAngleOfAttackPtr(nullptr)
{
   for (size_t i = 0; i < 3; ++i)
   {
      mPrePauseVelocity[i] = aSrc.mPrePauseVelocity[i];
   }

   if (aSrc.mAngleOfAttackPtr != nullptr)
   {
      mAngleOfAttackPtr = new WsfAngleOfAttack(*aSrc.mAngleOfAttackPtr);
   }
}

// ================================================================================================
WsfWaypointMover::~WsfWaypointMover()
{
   SetComplete();

   if (mDebugKeepPath && GetPlatform() != nullptr)
   {
      auto out = ut::log::debug() << "Path for Platform: " << GetPlatform()->GetName();
      mTotalPath.Append(GetSimulation()->GetSimTime() - mPathStartTime, mPath);
      mTotalPath.PrintPath(out);
   }
   delete mAngleOfAttackPtr;
}

// ================================================================================================
bool WsfWaypointMover::Initialize(double aSimTime)
{
   mPathComputer.SetRandom(&GetSimulation()->GetRandom());
   EnableTerrain(mUseTerrain);
   if (mTerrainPtr != nullptr)
   {
      mTerrainPtr->Initialize(*GetSimulation());
   }

   mCurrentConstraints = mMoverConstraints;

   if (mDefaultClimbRate != WsfPath::cDOUBLE_NOT_SET)
   {
      mCurrentConstraints.mMaxClimbRate = mDefaultClimbRate;
   }
   if (mDefaultLinearAcceleration != WsfPath::cDOUBLE_NOT_SET)
   {
      mCurrentConstraints.mMaxLinearAccel = mDefaultLinearAcceleration;
   }
   if (mDefaultRadialAcceleration != WsfPath::cDOUBLE_NOT_SET)
   {
      mCurrentConstraints.mMaxRadialAccel = mDefaultRadialAcceleration;
   }

   mPathComputer.GetConstraints() = mCurrentConstraints;
   mPathComputer.SetDefaultSwitch(GetSwitch());

   if (mTurnFailOption == cSKIP_POINT)
   {
      mTargetTypeFlags |= WsfPathTarget::cOPTIONAL_POINT;
   }
   else if (mTurnFailOption == cREVERSE_TURN)
   {
      mTargetTypeFlags |= WsfPathTarget::cREQUIRED_POINT;
   }
   mTarget.mTurnFailureThreshold = mTurnFailureThreshold;
   // If we have been commanded to do anything, don't let
   // WsfRouteMover's default extrapolation kick in.
   if (mPath.GetDuration() > 0)
   {
      double zero[3] = {0, 0, 0};
      GetPlatform()->SetVelocityWCS(zero);
   }
   bool wasPaused = IsPaused();

   // Ensure the update interval is set when using a path compute timestep
   if (mPathComputeTimestep > 0.0 && (GetUpdateInterval() == 0 || GetUpdateInterval() > mPathComputeTimestep))
   {
      SetUpdateInterval(mPathComputeTimestep);
   }

   // WsfRouteMover has no access to mCurrentConstraints when normalizing route waypoints,
   // so we fake it.
   auto moverConstraints = mMoverConstraints; // save a copy
   mMoverConstraints     = mCurrentConstraints;
   bool ok               = WsfRouteMover::Initialize(aSimTime);
   mMoverConstraints     = moverConstraints;

   // Route mover will pause if no route exists.  This code resumes if a command like
   // GoToHeading() was called prior to Initialize().
   if ((!wasPaused) && IsPaused() && (mPath.GetDuration() > 0.0))
   {
      Unpause(aSimTime, true);
   }

   return ok;
}

// ================================================================================================
void WsfWaypointMover::SetPlatform(WsfPlatform* aPlatformPtr)
{
   if (GetPlatform() != nullptr && aPlatformPtr == nullptr)
   {
      SetComplete();
      DisableAndClearUnpause();
   }
   WsfRouteMover::SetPlatform(aPlatformPtr);
}

// ================================================================================================
void WsfWaypointMover::UpdateConstraints(const WsfWaypoint& aWaypoint)
{
   if (aWaypoint.GetClimbRate() > 0.0)
   {
      mCurrentConstraints.mMaxClimbRate = std::min(mMoverConstraints.mMaxClimbRate, aWaypoint.GetClimbRate());
   }
   else if (aWaypoint.GetClimbRate() == WsfPath::cUSE_DEFAULT)
   {
      mCurrentConstraints.mMaxClimbRate = mDefaultClimbRate;
   }

   if (aWaypoint.GetMaximumFlightPathAngle() > 0.0)
   {
      mCurrentConstraints.mMaxFlightPathAngle =
         std::min(mMoverConstraints.mMaxFlightPathAngle, aWaypoint.GetMaximumFlightPathAngle());
   }
   else if (aWaypoint.GetMaximumFlightPathAngle() == WsfPath::cUSE_DEFAULT)
   {
      mCurrentConstraints.mMaxFlightPathAngle = mMoverConstraints.mMaxFlightPathAngle;
   }

   if (aWaypoint.GetLinearAccel() > 0.0)
   {
      mCurrentConstraints.mMaxLinearAccel = std::min(mMoverConstraints.mMaxLinearAccel, aWaypoint.GetLinearAccel());
   }
   else if (aWaypoint.GetLinearAccel() == WsfPath::cUSE_DEFAULT)
   {
      mCurrentConstraints.mMaxLinearAccel = mDefaultLinearAcceleration;
   }

   if (aWaypoint.GetRadialAccel() > 0.0)
   {
      mCurrentConstraints.mMaxRadialAccel = std::min(mMoverConstraints.mMaxRadialAccel, aWaypoint.GetRadialAccel());
   }
   else if (aWaypoint.GetRadialAccel() == WsfPath::cUSE_DEFAULT)
   {
      mCurrentConstraints.mMaxRadialAccel = mDefaultRadialAcceleration;
   }

   mPathComputer.GetConstraints() = mCurrentConstraints;
}

// ================================================================================================
void WsfWaypointMover::UpdatePosition(double aSimTime)
{
   if ((GetPlatform()->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND) && (GetDamageFactor() >= 1.0))
   {
      // TO-DO This is not a sophisticated implementation,
      // but is used as a placeholder, version 1.0 if you will.
      // For Land domain platforms, a broken mover prevents any
      // further motion, paralyzed until repaired.
      double ZERO[3] = {0.0, 0.0, 0.0};
      GetPlatform()->SetVelocityWCS(ZERO);
      // This is needed in case the mover is restoreed.
      // We do not want it to immediately make up for lost time.
      SetLastUpdateTime(aSimTime);
      return;
   }

   if (IsPaused() || GetPlatform() == nullptr)
   {
      return;
   }
   double lastUpdateTime = GetLastUpdateTime();
   double timeChange     = aSimTime - lastUpdateTime;
   if (timeChange > 0.0 || (mForceUpdates && timeChange >= 0.0))
   {
      double relTime = aSimTime - mPathStartTime;
      if (relTime <= mPath.GetDuration())
      {
         WsfPathState state;
         mPath.GetState(relTime, state);
         UtEntity& platformEntity = *GetPlatform();
         assert(state.mLatitude >= -90.0 && state.mLatitude <= 90.0);
         assert(state.mLongitude >= -180.0 && state.mLongitude <= 180.0);
         if (mAngleOfAttackPtr != nullptr)
         {
            state.mOrientationNED[1] = mAngleOfAttackPtr->GetPitch(state.mOrientationNED[1],
                                                                   state.mAltitude,
                                                                   UtVec3d::Magnitude(state.mVelocityNED));
         }
         state.CopyToEntity(platformEntity);
      }
      else
      {
         // If we get here, it's an error.  mPath should be computed prior to UpdatePosition()
         assert(!mPath.Empty());
         UtEntity& platformEntity = *GetPlatform();
         mPath.GetEndState().CopyToEntity(platformEntity);
      }
      if (mIsOnGround)
      {
         MoveToGround();
      }
      else if (!(mPitchEnable))
      {
         double heading, pitch, roll;
         GetPlatform()->GetOrientationNED(heading, pitch, roll);
         GetPlatform()->SetOrientationNED(heading, 0.0, roll);
      }
   }
   PostMoveUpdate(aSimTime);

   if (mPathComputeTimestep > 0 && (mPathComputeTimestep < aSimTime - mPathStartTime) && mPath.GetDuration() > 0)
   {
      bool                            pathNeedsSegmenting = false;
      const WsfPathList::SegmentList& segments            = mPath.GetSegments();
      for (auto segment : segments)
      {
         if (segment->mIsApproximation)
         {
            pathNeedsSegmenting = true;
            break;
         }
      }
      if (pathNeedsSegmenting)
      {
         ResetPosition(aSimTime);
      }
   }
}

// ================================================================================================
// virtual
WsfMover* WsfWaypointMover::Clone() const
{
   return new WsfWaypointMover(*this);
}

// ================================================================================================
void WsfWaypointMover::SetGuidanceFrom(const WsfWaypoint& aPreviousWaypoint)
{
   UpdateConstraints(aPreviousWaypoint);
}

// ================================================================================================
void WsfWaypointMover::SetGuidanceTo(const WsfWaypoint& aTargetWaypoint)
{
   if (aTargetWaypoint.GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
   {
      mTarget.mSpeed = aTargetWaypoint.GetSpeed();
      mTarget.mTargetType |= WsfPathTarget::cSPEED;
   }

   if (aTargetWaypoint.GetAlt() != WsfPath::cDOUBLE_NOT_SET)
   {
      mTarget.mAltitude = aTargetWaypoint.GetAlt();
      mTarget.mTargetType |= WsfPathTarget::cALTITUDE;
   }
}

// ================================================================================================
void WsfWaypointMover::AdvanceToNextWaypoint(double aSimTime, const WsfWaypoint& aTarget, const WsfWaypoint* aNextPointPtr)
{
   if (aTarget.GetPointType() & WsfWaypoint::cHEADING_MASK)
   {
      UpdateConstraints(aTarget);
   }
   Update(aSimTime);
   ResetPath(aSimTime);
   CreatePath(aTarget, aNextPointPtr);
   ExecutePath(aSimTime);
}

// ================================================================================================
// virtual
bool WsfWaypointMover::GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute)
{
   Update(aSimTime);
   Unpause(aSimTime, true);
   mTarget.mAltitude = aAltitude;
   if (!aKeepRoute)
   {
      mTarget.mTargetType &= ~WsfPathTarget::cLOCATION;
      mTarget.mTargetType |= WsfPathTarget::cEXTRAPOLATE;
      mTarget.mTime = mTarget.mDistance = 0.0;
   }
   mTarget.mTargetType |= WsfPathTarget::cALTITUDE;
   if (aClimbDiveRate > 0.0)
   {
      // The climb / dive rate is in addition to any horizontal velocity.
      // Keep the current horizontal velocity and add substitute the climb / dive rate for the local down component.
      mPathComputer.GetConstraints().mMaxClimbRate = std::min(mMoverConstraints.mMaxClimbRate, aClimbDiveRate);
      double velNED[3];
      GetPlatform()->GetVelocityNED(velNED);
      velNED[2]                                          = aClimbDiveRate;
      double flightPathAngle                             = asin(aClimbDiveRate / UtVec3d::Magnitude(velNED));
      mPathComputer.GetConstraints().mMaxFlightPathAngle = flightPathAngle;
   }

   // If staying on route allow point switching logic to continue
   if (!aKeepRoute || !IsSwitchingPoints())
   {
      ResetPath(aSimTime);
      CreatePath(aKeepRoute);
      ExecutePath(aSimTime);
   }
   return true;
}

// ================================================================================================
// virtual
bool WsfWaypointMover::GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute)
{
   Update(aSimTime);
   Unpause(aSimTime, true);
   mTarget.mSpeed = aSpeed;

   WsfRouteMover::GoToSpeed(aSimTime, aSpeed, aLinearAccel, aKeepRoute);

   if (!aKeepRoute)
   {
      mTarget.mTargetType &= ~WsfPathTarget::cLOCATION;
      mTarget.mTargetType |= WsfPathTarget::cEXTRAPOLATE;
      mTarget.mTime = mTarget.mDistance = 0.0;
   }
   mTarget.mTargetType |= WsfPathTarget::cSPEED;

   if (aLinearAccel > 0.0)
   {
      mPathComputer.GetConstraints().mMaxLinearAccel = std::min(mMoverConstraints.mMaxLinearAccel, aLinearAccel);
   }
   // If staying on route allow point switching logic to continue
   if (!aKeepRoute || !IsSwitchingPoints())
   {
      ResetPath(aSimTime);
      CreatePath(aKeepRoute);
      ExecutePath(aSimTime);
   }
   return true;
}

// ================================================================================================
// virtual
bool WsfWaypointMover::TurnToHeading(double aSimTime, double aHeading, double aRadialAccel, WsfPath::TurnDirection aTurnDirection)
{
   Update(aSimTime);
   Unpause(aSimTime, true);
   mTarget.mHeading = aHeading;
   mTarget.mTargetType &= ~(WsfPathTarget::cLOCATION | WsfPathTarget::cRELATIVE_TURN);
   mTarget.mTargetType |= WsfPathTarget::cHEADING | WsfPathTarget::cEXTRAPOLATE;
   mTarget.mTime = mTarget.mDistance = 0.0;
   mTarget.mTurnDirection            = (WsfPathTarget::TurnDirection)aTurnDirection;
   if (aRadialAccel > 0.0)
   {
      mPathComputer.GetConstraints().mMaxRadialAccel = std::min(mMoverConstraints.mMaxRadialAccel, aRadialAccel);
   }
   ResetPath(aSimTime);
   CreatePath();
   ExecutePath(aSimTime);
   return true;
}

// ================================================================================================
// virtual
bool WsfWaypointMover::TurnToRelativeHeading(double                 aSimTime,
                                             double                 aHeadingChange,
                                             double                 aRadialAccel,
                                             WsfPath::TurnDirection aTurnDirection)
{
   Update(aSimTime);
   Unpause(aSimTime, true);

   // Heading change is relative to current platform heading
   // So get the current heading and apply the change
   double heading, pitch, roll;
   GetPlatform()->GetOrientationNED(heading, pitch, roll);
   mTarget.mHeading = heading + aHeadingChange;

   mTarget.mTargetType &= ~WsfPathTarget::cLOCATION;
   mTarget.mTargetType |= WsfPathTarget::cHEADING | WsfPathTarget::cEXTRAPOLATE;
   mTarget.mTurnDirection = (WsfPathTarget::TurnDirection)aTurnDirection;
   if (aRadialAccel > 0.0)
   {
      mPathComputer.GetConstraints().mMaxRadialAccel = std::min(mMoverConstraints.mMaxRadialAccel, aRadialAccel);
   }
   ResetPath(aSimTime);
   CreatePath(false);
   ExecutePath(aSimTime);
   return true;
}

// ================================================================================================
void WsfWaypointMover::ExecutePath(double aSimTime)
{
   Unpause(aSimTime, true);
   SetComplete();
   RouteChanged();
   mPathStartTime = aSimTime;
   if (IsFinite(mPath.GetDuration()))
   {
      SetCompleteTime(aSimTime + mPath.GetDuration());
   }
}

// ================================================================================================
void WsfWaypointMover::Extrapolate(double aSimTime)
{
   Update(aSimTime);
   WsfPathState state(*GetPlatform());

   mTarget.mTargetType &= ~(WsfPathTarget::cLOCATION | WsfPathTarget::cHEADING | WsfPathTarget::cALTITUDE |
                            WsfPathTarget::cSPEED | WsfPathTarget::cTIME_TO_POINT);
   mTarget.mTargetType |= WsfPathTarget::cEXTRAPOLATE;
   mTarget.mTargetType |= mTargetTypeFlags;
   mTarget.mDistance = mTarget.mTime = 0.0;

   mPathComputer.ComputePath(state, mTarget, mPath);
   ExecutePath(aSimTime);
}

// ================================================================================================
void WsfWaypointMover::CreatePath(const WsfWaypoint& aTargetPoint, const WsfWaypoint* aNextPointPtr)
{
   // Create the path target for the specified target point.

   mCurrentConstraints.mIsOnGround = mIsOnGround;
   WsfPathState currentState(*GetPlatform());
   mPathComputer.CreateTarget(currentState, mCurrentConstraints, mMoverConstraints, aTargetPoint, aNextPointPtr, mTarget);

   CreatePath(false);

   // If the target point is a 'TIME_TO_POINT', revise the speed to reach the point at the desired time.
   if ((aTargetPoint.GetPointType() & WsfWaypoint::cTIME_TO_POINT) && (aTargetPoint.GetDistanceOrTime() > 0.0))
   {
      mPathComputer.RevisePathHitPointAtTime(mTarget, mPath);
   }
}

// ================================================================================================
void WsfWaypointMover::CreatePath(bool aResetDefaults)
{
   if (aResetDefaults)
   {
      mTarget.mTargetType &= ~(WsfPathTarget::cTURN_ON_APPROACH | WsfPathTarget::cRELATIVE_TURN);
   }

   mPathComputer.ConstrainTarget(mTarget, mCurrentConstraints);
   mTarget.mTargetType |= mTargetTypeFlags;
   WsfPathState state(*GetPlatform());

   if (mDebugPathComputation)
   {
      auto out = ut::log::debug() << GetPlatform()->GetName() << " Compute path for State: ";
      out.AddNote() << "Lat: " << state.mLatitude;
      out.AddNote() << "Lon: " << state.mLongitude;
      out.AddNote() << "Alt: " << state.mAltitude;

      { // RAII block
         auto vel = out.AddNote() << "Velocity NED:";
         vel.AddNote() << "North: " << state.mVelocityNED[0];
         vel.AddNote() << "East: " << state.mVelocityNED[1];
         vel.AddNote() << "Down: " << state.mVelocityNED[2];
      }

      out.AddNote() << "Yaw: " << state.mOrientationNED[0];
      out.AddNote() << "Pitch: " << state.mOrientationNED[1];
      out.AddNote() << "Roll: " << state.mOrientationNED[2];

      { // RAII block
         auto targetNote = out.AddNote() << "Target:";
         mTarget.Print(targetNote);
      }

      mPathComputer.ComputePath(state, mTarget, mPath);

      { // RAII block
         auto pathNote = out.AddNote() << "Path:";
         mPath.PrintPath(pathNote);
      }
   }
   else
   {
      // This line gets executed in both paths.
      mPathComputer.ComputePath(state, mTarget, mPath);
   }
}

// ================================================================================================
//! Called when a waypoint is reached.  Triggers processing of next waypoint
void WsfWaypointMover::HitWaypoint(double aSimTime)
{
   // Clear the pointer to the event
   mHitEventPtr = nullptr;

   // Make sure the mover is at exactly aSimTime
   mForceUpdates = true;
   Update(aSimTime);
   mForceUpdates = false;

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform hit target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      if (mTarget.mTargetType & WsfPathTarget::cLOCATION)
      {
         UtLatPos lat(mTarget.mLatitude);
         UtLonPos lon(mTarget.mLongitude);
         out.AddNote() << "Lat: " << lat;
         out.AddNote() << "Lon: " << lon;
      }
      if (mTarget.mTargetType & WsfPathTarget::cALTITUDE)
      {
         out.AddNote() << "Alt: " << mTarget.mAltitude << " m";
      }
      if (mTarget.mTargetType & WsfPathTarget::cSPEED)
      {
         out.AddNote() << "Speed: " << mTarget.mSpeed << " m/s";
      }
      if (mTarget.mTargetType & WsfPathTarget::cHEADING)
      {
         out.AddNote() << "Heading: " << UtMath::cDEG_PER_RAD * mTarget.mHeading << " deg";
      }
      if (mTarget.mDistance > 0.0)
      {
         out.AddNote() << "Distance: " << mTarget.mDistance << " m";
      }
      if (mTarget.mTime > 0.0)
      {
         out.AddNote() << "Time: " << mTarget.mTime << " s";
      }
   }

   // Check for infinite loop condition when using a goto label
   if ((GetTargetIndex() < GetCurrentIndex()) && (fabs(aSimTime - mPathStartTime) < std::numeric_limits<double>::epsilon()))
   {
      // Pause the mover and issue a warning
      Pause(aSimTime, UtMath::cDOUBLE_MAX);
      auto out = ut::log::warning() << "Platform cannot execute route while movement is paused.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
   else
   {
      // Reset the path and proceed to the next waypoint
      mPathStartTime = aSimTime;
      ResetPath(aSimTime);
      WaypointReached(aSimTime);
   }
}

// ================================================================================================
//! Schedules the HitPointEvent to execute at aSimTime
void WsfWaypointMover::SetCompleteTime(double aSimTime)
{
   SetComplete();
   if (IsFinite(aSimTime))
   {
      auto eventPtr = ut::make_unique<HitPointEvent>(this);
      eventPtr->SetTime(aSimTime);
      mHitEventPtr = eventPtr.get();
      GetSimulation()->AddEvent(std::move(eventPtr));
   }
}

// ================================================================================================
//! Disables any pending HitPointEvent
void WsfWaypointMover::SetComplete()
{
   if (mHitEventPtr != nullptr)
   {
      mHitEventPtr->Disable();
      mHitEventPtr = nullptr;
   }
}

// ================================================================================================
// virtual
bool WsfWaypointMover::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();
   if (command == "default_radial_acceleration")
   {
      aInput.ReadValueOfType(mDefaultRadialAcceleration, UtInput::cACCELERATION);
   }
   else if (command == "default_linear_acceleration")
   {
      aInput.ReadValueOfType(mDefaultLinearAcceleration, UtInput::cACCELERATION);
   }
   else if (command == "default_climb_rate")
   {
      aInput.ReadValueOfType(mDefaultClimbRate, UtInput::cSPEED);
   }
   else if (command == "debug_whole_path")
   {
      aInput.ReadValue(mDebugKeepPath);
   }
   else if (command == "debug_path")
   {
      aInput.ReadValue(mDebugPathComputation);
   }
   else if (mMoverConstraints.ProcessInput(aInput))
   {
   }
   else if (command == "on_ground")
   {
      mIsOnGround = true;
   }
   else if (command == "on_road")
   {
      mIsOnRoad = true;
   }
   else if (command == "off_road")
   {
      mIsOnRoad = false;
   }
   else if ((command == "no_pitch") || (command == "pitch_disable"))
   {
      mPitchEnable = false;
   }
   else if (command == "pitch_enable")
   {
      mPitchEnable = true;
   }
   else if ((command == "airborne") || (command == "in_air"))
   {
      mIsOnGround = false;
   }
   else if (command == "angle_of_attack_table")
   {
      delete mAngleOfAttackPtr;
      mAngleOfAttackPtr = new WsfAngleOfAttack;
      mAngleOfAttackPtr->ProcessInput(aInput);
   }
   else if (command == "maximum_turn_angle")
   {
      double maxTurnAngle;
      aInput.ReadValueOfType(maxTurnAngle, UtInput::cANGLE);
      aInput.ValueGreater(maxTurnAngle, UtMath::cPI);
      if (maxTurnAngle > UtMath::cTWO_PI)
      {
         maxTurnAngle = UtMath::cTWO_PI;
      }
      mPathComputer.SetMaximumTurn(maxTurnAngle);
   }
   else if (command == "path_variance_radius")
   {
      double pathVarianceRadius;
      aInput.ReadValueOfType(pathVarianceRadius, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(pathVarianceRadius, 0.0);
      mPathComputer.SetPathVarianceRadius(pathVarianceRadius);
   }
   else if (command == "speed_variance_percent")
   {
      double speedVariancePct;
      aInput.ReadValue(speedVariancePct);
      aInput.ValueGreaterOrEqual(speedVariancePct, 0.0);
      mPathComputer.SetSpeedVariancePct(speedVariancePct);
   }
   else if (command == "path_compute_timestep")
   {
      aInput.ReadValueOfType(mPathComputeTimestep, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mPathComputeTimestep, 0.0);
   }
   else
   {
      myCommand = WsfRouteMover::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
// virtual
bool WsfWaypointMover::GetFutureLocationWCS(double aSimTime, double aLocationWCS[3])
{
   bool locationValid = false;
   UtVec3d::Set(aLocationWCS, 0.0);
   WsfPathState state;

   // If the specified time is within the time bounds of the currently constructed
   // path then get the future state directly. Otherwise we have to use a more complicated
   // method to build the path from the route and then get the location.

   double relTime = aSimTime - mPathStartTime;
   if (relTime <= mPath.GetDuration())
   {
      mPath.GetState(relTime, state);
      locationValid = true;
   }
   else
   {
      const WsfRoute* routePtr = GetRoute();
      if (routePtr != nullptr)
      {
         WsfPathComputer    pathComputer(mPathComputer);
         WsfPathConstraints currentConstraints(mCurrentConstraints);
         currentConstraints.mIsOnGround = mIsOnGround;
         WsfPathList   path(mPath);
         WsfPathTarget target(mTarget);

         // Iterate through the route and try to find the route segment that includes
         // the requested time.

         unsigned int targetIndex = static_cast<unsigned int>(GetTargetIndex());
         while ((relTime > path.GetDuration()) && (targetIndex < routePtr->GetSize()))
         {
            // Find the index of the new target point.
            const WsfWaypoint& currentTargetPoint = routePtr->GetWaypointAt(targetIndex);
            if (!currentTargetPoint.GetGotoId().IsNull())
            {
               targetIndex = routePtr->GetWaypointIndexAt(currentTargetPoint.GetGotoId());
            }
            else
            {
               ++targetIndex;
            }
            if (targetIndex >= routePtr->GetSize())
            {
               break;
            }
            const WsfWaypoint& targetPoint = routePtr->GetWaypointAt(targetIndex);

            // Find the index of the point after the new target point (for turn on approach)
            unsigned int nextPointIndex = targetIndex + 1;
            if (!targetPoint.GetGotoId().IsNull())
            {
               nextPointIndex = routePtr->GetWaypointIndexAt(targetPoint.GetGotoId());
            }
            const WsfWaypoint* nextPointPtr = nullptr;
            if (nextPointIndex < routePtr->GetSize())
            {
               nextPointPtr = &(routePtr->GetWaypointAt(nextPointIndex));
            }

            WsfPathState currentState(path.GetEndState());
            pathComputer.CreateTarget(currentState, currentConstraints, mMoverConstraints, targetPoint, nextPointPtr, target);
            target.mTargetType |= mTargetTypeFlags;

            pathComputer.ComputePath(currentState, target, path);

            // If the target point is a 'TIME_TO_POINT', revise the speed to reach the point at the desired time.
            if ((targetPoint.GetPointType() & WsfWaypoint::cTIME_TO_POINT) && (targetPoint.GetDistanceOrTime() > 0.0))
            {
               pathComputer.RevisePathHitPointAtTime(target, path);
            }
         }

         // If the desired route segment was found, get the location on the segment.
         if ((targetIndex < routePtr->GetSize()) && (relTime <= path.GetDuration()))
         {
            locationValid = true;
            path.GetState(relTime, state);
         }
      }
   }

   // If a location was determined then convert the location to WCS (after performing terrain
   // clamping if the mover is on the ground...)

   if (locationValid)
   {
      double altitude = state.mAltitude;
      if (mIsOnGround)
      {
         altitude = mAltitudeOffset;
         if (mTerrainPtr != nullptr && mTerrainPtr->IsEnabled())
         {
            float terrainAltitude;
            mTerrainPtr->GetElevInterp(state.mLatitude, state.mLongitude, terrainAltitude);
            altitude += terrainAltitude;
         }
      }
      UtEntity::ConvertLLAToWCS(state.mLatitude, state.mLongitude, altitude, aLocationWCS);
   }
   return locationValid;
}

// ================================================================================================
double WsfWaypointMover::MaximumRadialAcceleration()
{
   return mMoverConstraints.mMaxRadialAccel;
}

// ================================================================================================
double WsfWaypointMover::TurnRadius() // assumed current speed
{
   double speed = GetPlatform()->GetSpeed();
   return GetPathComputer().GetTurnRadius(speed);
}

// ================================================================================================
double WsfWaypointMover::TurnRadius(double atSpeed)
{
   return GetPathComputer().GetTurnRadius(atSpeed);
}

// ================================================================================================
double WsfWaypointMover::MaximumTurnRate()
{
   WsfPathConstraints pCon         = mMoverConstraints;
   double             maxRateLimit = 0.0;
   if (pCon.mMaxTurnRate > 0.0)
   {
      maxRateLimit = pCon.mMaxTurnRate;
   }
   if (pCon.mTurnRateLimit > 0.0)
   {
      maxRateLimit = std::min(pCon.mTurnRateLimit, maxRateLimit);
   }
   return maxRateLimit;
}

// ================================================================================================
bool WsfWaypointMover::GetPropertyDouble(const std::string& aPropertyName, double& aProperty)
{
   if (aPropertyName == "maximum_altitude")
   {
      aProperty = mPathComputer.GetConstraints().mMaxAltitude;
      return true;
   }
   if (aPropertyName == "minimum_altitude")
   {
      aProperty = mPathComputer.GetConstraints().mMinAltitude;
      return true;
   }
   if (aPropertyName == "maximum_speed")
   {
      aProperty = mPathComputer.GetConstraints().mMaxSpeed;
      return true;
   }
   if (aPropertyName == "minimum_speed")
   {
      aProperty = mPathComputer.GetConstraints().mMinSpeed;
      return true;
   }
   if (aPropertyName == "default_radial_acceleration")
   {
      aProperty = mDefaultRadialAcceleration;
      return true;
   }
   if (aPropertyName == "default_linear_acceleration")
   {
      aProperty = mDefaultLinearAcceleration;
      return true;
   }
   if (aPropertyName == "default_climb_rate")
   {
      aProperty = mDefaultClimbRate;
      return true;
   }
   if (aPropertyName == "turn_radius")
   {
      aProperty = TurnRadius();
      return true;
   }
   return WsfRouteMover::GetPropertyDouble(aPropertyName, aProperty);
}

// ================================================================================================
// computes the max velocity at which the platform can travel in order to reach the target location while turning as
// hard as possible the value returned might be above the platform's maximum speed (which means he has plenty of time to
// make the turn) the value returned might be below the platform's minimum speed (which means he will probably
// overshoot)
double WsfWaypointMover::CornerVelocity(WsfGeoPoint* aTarget)
{
   WsfPlatform* platformPtr = GetPlatform();
   double       platLocWCS[3];
   double       tgtLocWCS[3];
   platformPtr->GetLocationWCS(platLocWCS);
   aTarget->GetLocationWCS(tgtLocWCS);

   double range       = UtMeasurementUtil::GroundRange(platLocWCS, tgtLocWCS);
   double maxTurnRate = MaximumTurnRate();

   double maxRadAccel = MaximumRadialAcceleration();
   maxRadAccel =
      (maxRadAccel > 0.0) ? (maxRadAccel) : (6.0 * UtEarth::cACCEL_OF_GRAVITY); // default to 6 Gs if no value available

   double relBearing = fabs(platformPtr->RelativeBearing(tgtLocWCS)); // radians
   if (relBearing < 0.0000048481) // check if already facing the target, within 1/3600th of a degree
   {
      return 1000000000.0;
   } // return a large speed value: one billion m/s

   // find the radius of the circle on which the arc of the flight path sits
   double radius = range / (2.0 * sin(relBearing));
   // compute the velocity using the max radial acceleration and the radius:  A = V^2 / R
   double velocity = sqrt(maxRadAccel * radius);

   // limit the velocity by the turn rate, if such a limit exists
   if (maxTurnRate > 0.0)
   {
      // if we know the max turn rate, we know how long it will take to turn towards the target (while moving: requiring
      // twice the angle)
      double time          = 2.0 * relBearing / maxTurnRate;
      double arcLength     = radius * (2.0 * relBearing);
      double velocityLimit = arcLength / time;
      velocity             = std::min(velocity, velocityLimit);
   }
   return velocity;
}

// ================================================================================================
//! Stop the movement of the platform until aUnpauseTime
void WsfWaypointMover::Pause(double aSimTime, double aUnpauseTime)
{
   mUnpauseTime = aUnpauseTime;
   GetPlatform()->GetVelocityNED(mPrePauseVelocity);
   double zero[3] = {0, 0, 0};
   GetPlatform()->SetVelocityWCS(zero);
   GetPlatform()->SetAccelerationWCS(zero);
   // Disable HitPointEvent if one exists:
   SetComplete();
   AddUnpauseEventFor(this, aUnpauseTime);
}

// ================================================================================================
bool WsfWaypointMover::Unpause(double aSimTime, bool aInterrupt)
{
   bool unpaused = false;
   if (IsPaused())
   {
      GetPlatform()->SetVelocityNED(mPrePauseVelocity);
      if (WsfRouteMover::Unpause(aSimTime, aInterrupt))
      {
         unpaused = true;
      }
   }
   return unpaused;
}

// ================================================================================================
void WsfWaypointMover::UpdateHeightOfTerrain()
{
   mHeightOfTerrain = 0.0;
   if ((mTerrainPtr != nullptr) && mTerrainPtr->IsEnabled())
   {
      double lat, lon, alt;
      GetPlatform()->GetLocationLLA(lat, lon, alt);
      float terrainAltitude;
      mTerrainPtr->GetElevInterp(lat, lon, terrainAltitude);
      mHeightOfTerrain = (double)terrainAltitude;
   }
}

// ================================================================================================
void WsfWaypointMover::MoveToGround()
{
   mHeightOfTerrain = 0.0;
   if ((mTerrainPtr != nullptr) && mTerrainPtr->IsEnabled())
   {
      double lat, lon, alt;
      GetPlatform()->GetLocationLLA(lat, lon, alt);
      float terrainAltitude;
      mTerrainPtr->GetElevInterp(lat, lon, terrainAltitude);
      mHeightOfTerrain = (double)terrainAltitude;
      if (mIsOnGround)
      {
         GetPlatform()->SetLocationLLA(lat, lon, (double)terrainAltitude + mAltitudeOffset);
         SetGroundAttitude(lat, lon);
      }
   }
   else if (mIsOnGround)
   {
      double lat, lon, alt;
      GetPlatform()->GetLocationLLA(lat, lon, alt);
      GetPlatform()->SetLocationLLA(lat, lon, mAltitudeOffset);
      double yaw, pitch, roll;
      GetPlatform()->GetOrientationNED(yaw, pitch, roll);
      GetPlatform()->SetOrientationNED(yaw, 0, 0);
      SetGroundAttitude(lat, lon);
   }
}

// ================================================================================================
// protected
//! Update the orientation of the platform to conform to local ground slope.
//! It is assumed this method will _not_ be called unless already on ground.
void WsfWaypointMover::SetGroundAttitude(double aLatitude, double aLongitude)
{
   // Construct a local coordinate system with the proper orientation.
   double heading, pitch, roll;
   GetPlatform()->GetOrientationNED(heading, pitch, roll);
   pitch = 0.0;
   roll  = 0.0;

   // Start by getting the unit normal to the local terrain.
   if (mIsOnRoad && (!mPitchEnable))
   {
   }
   else if ((mTerrainPtr != nullptr) && mTerrainPtr->IsEnabled())
   {
      double sinHeading = sin(heading);
      double cosHeading = cos(heading);

      double localX[3] = {cosHeading, sinHeading, 0.0};
      double localY[3];
      double localZ[3]; // Note: this should be pointing up (-Z)
      mTerrainPtr->GetNormalNED(aLatitude, aLongitude, localZ);
      UtVec3d::Multiply(localZ, -1.0); // Use the normal pointing up

      // Use cross products of the local normal and the heading vector to produce
      // a full NED rotation matrix.  Then extract the euler angles from the matrix.
      UtVec3d::CrossProduct(localY, localZ, localX); // tentative Y = Z cross tentative X
      UtVec3d::CrossProduct(localX, localY, localZ);
      UtVec3d::Normalize(localX);
      UtVec3d::Normalize(localY);

      // Extract the local orientation
      heading = atan2(localX[1], localX[0]);
      pitch   = -atan2(localX[2], sqrt(localX[0] * localX[0] + localX[1] * localX[1]));
      roll    = atan2(localY[2], localZ[2]);

      if (mIsOnRoad)
      {
         roll = 0.0;
      }

      if (!mPitchEnable)
      {
         pitch = 0.0;
      }
   }
   GetPlatform()->SetOrientationNED(heading, pitch, roll);
}

// ================================================================================================
void WsfWaypointMover::EnableTerrain(bool aUseTerrain)
{
   if (aUseTerrain && (mTerrainPtr == nullptr))
   {
      if (GetSimulation())
      {
         mTerrainPtr = new wsf::Terrain(GetSimulation()->GetTerrainInterface());
      }
   }
   else if (!mUseTerrain && (mTerrainPtr != nullptr))
   {
      delete mTerrainPtr;
      mTerrainPtr = nullptr;
   }
   mUseTerrain = aUseTerrain;
}

// ================================================================================================
// virtual
void WsfWaypointMover::ResetPosition(double aSimTime)
{
   ResetPath(aSimTime);
   CreatePath(false);
   ExecutePath(aSimTime);
}

// ================================================================================================
// virtual
//! Return true if the mover is extrapolating after the end of a route
//! or after a GoTo/TurnTo command.
bool WsfWaypointMover::IsExtrapolating() const
{
   return (mTarget.mTargetType & WsfPathTarget::cEXTRAPOLATE) != 0;
}

// ================================================================================================
void WsfWaypointMover::ResetPath(double aSimTime)
{
   if (mDebugKeepPath)
   {
      mTotalPath.Append(aSimTime - mPathStartTime, mPath);
   }
   mPath.Reset();
}

const WsfPathConstraints* WsfWaypointMover::GetMoverConstraints() const
{
   return &mMoverConstraints;
}
