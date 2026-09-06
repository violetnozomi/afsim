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

#include "WsfNORAD_SpaceMover.hpp"

#include <iostream>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtMemory.hpp"
#include "WsfDateTime.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

// static
namespace // anonymous
{
bool sSuppressNORADSpaceMoverWarnings = false;
}

WsfNORAD_SpaceMover::WsfNORAD_SpaceMover(const WsfScenario& aScenario)
   : WsfSpaceMoverBase(aScenario, ut::make_unique<WsfNORAD_OrbitalPropagator>())
   , mWaypointPtr{nullptr}
{
   SetTLEInputAllowed(true);
}

WsfNORAD_SpaceMover::WsfNORAD_SpaceMover(const WsfNORAD_SpaceMover& aSrc)
   : WsfSpaceMoverBase(aSrc)
   , mWaypointPtr{nullptr}
{
   SetTLEInputAllowed(true);
}

// virtual
WsfMover* WsfNORAD_SpaceMover::Clone() const
{
   return new WsfNORAD_SpaceMover(*this);
}

// virtual
bool WsfNORAD_SpaceMover::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "suppress_warnings")
   {
      aInput.ReadValue(sSuppressNORADSpaceMoverWarnings);
   }
   else
   {
      myCommand = WsfSpaceMoverBase::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool WsfNORAD_SpaceMover::Initialize(double aSimTime)
{
   UtCalendar initialTime = GetSimulation()->GetDateTime().GetStartDateAndTime();
   initialTime.AdvanceTimeBy(aSimTime);

   if (mWaypointPtr != nullptr)
   {
      double locECI[3];
      double velECI[3];
      ComputeInitialFromWaypoint(initialTime, locECI, velECI);
      ut::OrbitalState initialState = GetPropagator().GetInitialOrbitalState();
      initialState.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);
      initialState.SetOrbitalStateVector(ut::OrbitalState::Vector(locECI, velECI));
      GetPropagator().SetInitialOrbitalState(initialState);
      mWaypointPtr.reset(nullptr);
   }

   bool retval = WsfSpaceMoverBase::Initialize(aSimTime);
   if (retval && !sSuppressNORADSpaceMoverWarnings)
   {
      // Print a warning if the simulation epoch is substantially past the satellite reference epoch.
      const UtCalendar& currentTime = GetPropagator().GetCurrentTime();
      const UtCalendar& tleTime     = GetPropagator().GetInitialOrbitalState().GetEpoch();

      const double difference = currentTime.GetJ2000_Date() - tleTime.GetJ2000_Date();
      if (std::abs(difference) >= UtCalendar::cDAYS_IN_WEEK) // If more than one week different.
      {
         auto out =
            ut::log::warning() << "Simulation epoch is substantially different from the satellite reference epoch.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Reference / TLE epoch: " << tleTime.GetEpoch();
         out.AddNote() << "Current simulation epoch: " << currentTime.GetEpoch();
         out.AddNote() << "Orbital Propagation may be unstable.";
      }
   }
   return retval;
}

//! Set the initial point of the mover.
bool WsfNORAD_SpaceMover::SetInitialPoint(double aLat, double aLon, double aAlt, double aSpeed, double aHeading)
{
   mWaypointPtr = ut::make_unique<WsfWaypoint>(aLat, aLon, aAlt, aSpeed);
   mWaypointPtr->SetHeading(aHeading * UtMath::cRAD_PER_DEG);
   return true;
}

void WsfNORAD_SpaceMover::ComputeInitialFromWaypoint(const UtCalendar& aEpoch, double aLocECI[3], double aVelECI[3]) const
{
   UtEntity sat;
   sat.SetECI_ReferenceEpoch(aEpoch);
   sat.SetTime(0.0);
   sat.SetLocationLLA(mWaypointPtr->GetLat(), mWaypointPtr->GetLon(), mWaypointPtr->GetAlt());
   sat.GetLocationECI(aLocECI);

   double speed = mWaypointPtr->GetSpeed();
   if (speed <= 0.0)
   {
      double dist = UtVec3d::Magnitude(aLocECI);
      double rxy  = sqrt(aLocECI[0] * aLocECI[0] + aLocECI[1] * aLocECI[1]);
      speed       = sqrt(GetPropagator().GetInitialOrbitalState().GetCentralBody().GetGravitationalParameter() / dist);
      double nedVel[3] = {speed * cos(mWaypointPtr->GetHeading()),
                          speed * sin(mWaypointPtr->GetHeading()) - UtEarth::cOMEGA * rxy,
                          0.0};
      sat.SetVelocityNED(nedVel);
   }
   else
   {
      double nedVel[3] = {speed * cos(mWaypointPtr->GetHeading()), speed * sin(mWaypointPtr->GetHeading()), 0.0};
      sat.SetVelocityNED(nedVel);
   }
   sat.GetVelocityECI(aVelECI);
}
