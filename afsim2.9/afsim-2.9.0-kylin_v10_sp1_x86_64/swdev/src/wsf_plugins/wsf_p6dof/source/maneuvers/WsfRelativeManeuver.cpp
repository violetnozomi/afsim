// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfRelativeManeuver.hpp"

#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"

//! Returns the state as a waypoint.
WsfWaypoint RelativeManeuverKinematics::GetWaypoint() const
{
   UtEntity utilEntity{};
   utilEntity.SetLocationWCS(mLocWCS.GetData());
   utilEntity.SetVelocityWCS(mVelWCS.GetData());

   double lat, lon, alt;
   utilEntity.GetLocationLLA(lat, lon, alt);
   WsfWaypoint retval{lat, lon, alt, utilEntity.GetSpeed()};
   retval.SetHeading(mAnglesNED[0]);

   return retval;
}

//! Return a UtEntity that has the same kinematics as this state.
UtEntity RelativeManeuverKinematics::GetEntity() const
{
   UtEntity retval;
   retval.SetLocationWCS(mLocWCS.GetData());
   retval.SetVelocityWCS(mVelWCS.GetData());
   retval.SetAccelerationWCS(mAccWCS.GetData());
   retval.SetOrientationNED(mAnglesNED[0], mAnglesNED[1], mAnglesNED[2]);
   return retval;
}

//! Return the turn circle corresponding to this state.
ManeuverTurnCircle RelativeManeuverKinematics::GetTurnCircle() const
{
   return ManeuverTurnCircle{mLocWCS, mVelWCS, mAccWCS};
}

WsfRelativeManeuver::WsfRelativeManeuver(std::unique_ptr<RelativeManeuverState> aInitialStatePtr)
   : WsfManeuver()
   , mStatePtr{std::move(aInitialStatePtr)}
{
}

//! Copy construct a relative maneuver.
//!
//! Note that this will not set the initial state. That must be handled by
//! derived classes.
WsfRelativeManeuver::WsfRelativeManeuver(const WsfRelativeManeuver& aOther)
   : WsfManeuver(aOther)
   , mData(aOther.mData)
   , mStatePtr{nullptr}
   , mG_LoadMax{aOther.mG_LoadMax}
   , mG_LoadMin{aOther.mG_LoadMin}
   , mSpeedMpsMax{aOther.mSpeedMpsMax}
   , mSpeedMpsMin{aOther.mSpeedMpsMin}
{
}

//! Execute the relative maneuver.
//!
//! This will process the state-machine nature of relative maneuvers, updating to new states
//! when needed. This also calls into the abstract method UpdateData to ready the specific
//! data for the maneuver's processing. If there is ever a failure to update the data,
//! the maneuver levels out the controls and completes.
//!
//! \param aSimTime - the time at which the event is executing.
//! \returns        - the next evaluation time of this event.
double WsfRelativeManeuver::Execute(double aSimTime)
{
   double retval{-1.0};

   if (mStatePtr)
   {
      if (UpdateData())
      {
         auto nextStatePtr = mStatePtr->Evaluate();
         if (nextStatePtr)
         {
            mStatePtr = std::move(nextStatePtr);
         }
         retval = aSimTime + mStatePtr->GetEvaluationInterval();
      }
      else
      {
         FinalControls();
      }
   }

   return retval;
}

//! Limit the given speed value according to the limits set on this maneuver.
double WsfRelativeManeuver::LimitSpeed(double aSpeedMps) const
{
   return UtMath::Limit(aSpeedMps, mSpeedMpsMin, mSpeedMpsMax);
}

//! Predicate to determine if the given speed is inside the limits set on this maneuver.
bool WsfRelativeManeuver::IsSpeedInsideAllowedRange(double aSpeedMps) const
{
   return aSpeedMps >= mSpeedMpsMin && aSpeedMps <= mSpeedMpsMax;
}

//! Limit the given g-load according to the limits set on this maneuver.
double WsfRelativeManeuver::LimitG_Load(double aG_Load) const
{
   return UtMath::Limit(aG_Load, mG_LoadMin, mG_LoadMax);
}

void WsfRelativeManeuver::FinalControls() const
{
   WsfP6DOF_Mover* moverPtr = GetMover();
   moverPtr->SetAutopilotFlightPathAngle(0.0);
   moverPtr->SetAutopilotRollAngle(0.0);
}
