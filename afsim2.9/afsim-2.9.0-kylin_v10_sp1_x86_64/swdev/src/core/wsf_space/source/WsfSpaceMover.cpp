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

#include "WsfSpaceMover.hpp"

#include <iomanip>
#include <iostream>
#include <string>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtSun.hpp"
#include "UtTwoLineElement.hpp"
#include "UtVec3.hpp"
#include "WsfConjunctionInput.hpp"
#include "WsfDateTime.hpp"
#include "WsfEnvironment.hpp"
#include "WsfKeplerianOrbitalPropagator.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfWaypoint.hpp"

WsfSpaceMover::WsfSpaceMover(const WsfScenario& aScenario)
   : WsfSpaceMoverBase(aScenario, ut::make_unique<wsf::space::KeplerianOrbitalPropagator>())
   , mDebugShowStatus(false)
   , mInitialPointPtr(nullptr)
{
   // Set update time tolerance as the time required to travel 1 meter at 17000 mi/hr (about 1.3E-4)
   static const double sUpdateTimeTolerance = 1.0 / (17000.0 * 5280.0 * UtMath::cFT_PER_M / 3600.0);
   mUpdateTimeTolerance                     = sUpdateTimeTolerance;
}

// virtual
bool WsfSpaceMover::SetInitialPoint(double lat, double lon, double alt, double speed, double heading)
{
   mInitialPointPtr = ut::make_unique<WsfWaypoint>(lat, lon, alt, speed);
   mInitialPointPtr->SetHeading(heading * UtMath::cRAD_PER_DEG);
   return true;
}

// private
/** Set the initial orbital elements based on data contained in a waypoint. It is assumed that the
   satellite will be in a circular orbit if no speed is specified; otherwise the given speed
   will be used to produce the orbit.
   @note The preferred way to initialize a satellite is with the complete orbital elements
   specified in a two-line element ( see UtOrbitalElements::ReadOrbitalData ). When using this method,
   keep in mind the following:
   \li LEO (Low-Earth Orbit) is typically above an altitude of 400 km.
   \li GEO (Geocentric Orbit) is at an altitude of 22,240 mi, or 35786 km, with zero inclination (heading = 90 degrees).
   \li Orbits often have inclinations of launch latitude (heading = 90-launch latitude).
   \li Orbits are usually prograde (heading between 0 and 180 degrees).
   @note if the speed is zero, assume that a circular orbit is desired; this speed will be calculated.
*/
void WsfSpaceMover::InitializeFromWaypoint(const UtCalendar& aEpoch)
{
   UtEntity sat(GetInitialOrbitalState().GetCentralBody());
   sat.SetECI_ReferenceEpoch(aEpoch);
   sat.SetTime(0.0);
   sat.SetLocationLLA(mInitialPointPtr->GetLat(), mInitialPointPtr->GetLon(), mInitialPointPtr->GetAlt());
   double locECI[3];
   sat.GetLocationECI(locECI);

   // compute circular velocity
   double speed       = mInitialPointPtr->GetSpeed();
   auto&  centralBody = GetSimulation()->GetEnvironment().GetCentralBody();
   if (speed < 0.0) // compute circular velocity.
   {
      double dist      = UtVec3d::Magnitude(locECI);
      double rxy       = sqrt(locECI[0] * locECI[0] + locECI[1] * locECI[1]);
      speed            = sqrt(centralBody.GetGravitationalParameter() / dist);
      double nedVel[3] = {speed * cos(mInitialPointPtr->GetHeading()),
                          speed * sin(mInitialPointPtr->GetHeading()) - centralBody.GetRotationRate() * rxy,
                          0.0};
      sat.SetVelocityNED(nedVel);
   }
   else
   {
      double cosElevation = 1.0;
      double sinElevation = 0.0;
      if (mInitialPointPtr->GetAuxDataConst().AttributeExists("elevation"))
      {
         double elevation = mInitialPointPtr->GetAuxDataConst().GetDouble("elevation");
         cosElevation     = cos(elevation);
         sinElevation     = sin(elevation);
      }
      double nedVel[3] = {speed * cos(mInitialPointPtr->GetHeading()) * cosElevation,
                          speed * sin(mInitialPointPtr->GetHeading()) * cosElevation,
                          speed * (-sinElevation)};
      sat.SetVelocityNED(nedVel);
   }
   double velECI[3];
   sat.GetVelocityECI(velECI);

   GetPropagator().SetInitialOrbitalState(ut::OrbitalState(aEpoch,
                                                           centralBody,
                                                           ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                           ut::OrbitalState::ReferenceFrame::cECI,
                                                           ut::OrbitalState::Vector(locECI, velECI)));
}

// virtual
bool WsfSpaceMover::Initialize(double aSimTime)
{
   // Determine absolute time for initialization.
   UtCalendar epoch = GetDateTime().GetStartDateAndTime();
   epoch.AdvanceTimeBy(aSimTime);

   if (mInitialPointPtr != nullptr)
   {
      InitializeFromWaypoint(epoch);

      // Don't use it again as we may need to maneuver.
      mInitialPointPtr = nullptr;
   }
   else if (!GetPropagator().GetInitialOrbitalState().IsValid())
   {
      if (GetPlatform()->GetAltitude() > 0.0) // Was an initial location set on the parent platform?
      {
         UtVec3d location;
         UtVec3d velocity;
         // Get initial conditions from the parent platform (it may be initializing as a weapon platform's mover).
         GetPlatform()->GetLocationECI(location.GetData());
         GetPlatform()->GetVelocityECI(velocity.GetData());
         ut::OrbitalState initialState = GetPropagator().GetInitialOrbitalState();
         initialState.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);
         initialState.Set(epoch, ut::OrbitalState::Vector(location, velocity));
         GetPropagator().SetInitialOrbitalState(initialState);
      }
   }
   else if (GetInitialOrbitalState().OrbitalElementsValid() &&
            GetInitialOrbitalState().GetOrbitalElements().HasMeanElements())
   {
      // AFSIM has always allowed Keplerian orbital element input from TLEs, so
      // in this case, they are to be interpreted as instantaneous.
      UtOrbitalElements keplerianElements = GetInitialOrbitalState().GetOrbitalElements();
      keplerianElements.SetHasMeanElements(false);
      ut::OrbitalState initialState(GetInitialOrbitalState().GetCoordinateSystem(),
                                    ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE,
                                    UtTwoLineElement(keplerianElements));
      GetPropagator().SetInitialOrbitalState(initialState);
   }

   // Do this last, because we have to first have the OEs correct (handled above).
   // Calling the base class' Initialize forces an initial call to Update.
   bool ok = WsfSpaceMoverBase::Initialize(aSimTime);

   if (ok && mDebugShowStatus)
   {
      ShowStatus(aSimTime);
   }

   return ok;
}

// virtual
bool WsfSpaceMover::ProcessInput(UtInput& aInput)
{
   std::string command = aInput.GetCommand();
   bool        ok      = true;
   if ((command == "position") || (command == "initial_state_lla"))
   {
      double lat, lon;
      mInitialPointPtr = ut::make_unique<WsfWaypoint>();
      aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
      mInitialPointPtr->SetLat(lat);
      mInitialPointPtr->SetLon(lon);
      mInitialPointPtr->SetPointType(WsfWaypoint::cLATITUDE_AND_LONGITUDE);
      if (command == "position")
      {
         mInitialPointPtr->ProcessAllInput(aInput);
      }
      else // initial_state_lla
      {
         double  alt;
         UtVec3d velNED;
         aInput.ReadValueOfType(alt, UtInput::cLENGTH);
         aInput.ReadValueOfType(velNED[0], UtInput::cSPEED);
         aInput.ReadValueOfType(velNED[1], UtInput::cSPEED);
         aInput.ReadValueOfType(velNED[2], UtInput::cSPEED);
         mInitialPointPtr->SetAlt(alt);
         double speed = velNED.Magnitude();
         mInitialPointPtr->SetSpeed(speed);
         if (speed > 0.0)
         {
            mInitialPointPtr->SetHeading(atan2(velNED[1], velNED[0]));
            mInitialPointPtr->GetAuxData().AssignDouble("elevation",
                                                        atan2(-velNED[2],
                                                              sqrt(velNED[0] * velNED[0] + velNED[1] * velNED[1])));
         }
      }
   }
   else if (command == "oblate_earth")
   {
      bool precess = false;
      aInput.ReadValue(precess);
      static_cast<UtOrbitalPropagator&>(GetPropagator()).SetPrecession(precess);
   }
   else if (command == "initial_state_eci")
   {
      // Read Location and Velocity.
      double locECI[3];
      aInput.ReadValueOfType(locECI[0], UtInput::cLENGTH);
      aInput.ReadValueOfType(locECI[1], UtInput::cLENGTH);
      aInput.ReadValueOfType(locECI[2], UtInput::cLENGTH);

      double velECI[3];
      aInput.ReadValueOfType(velECI[0], UtInput::cSPEED);
      aInput.ReadValueOfType(velECI[1], UtInput::cSPEED);
      aInput.ReadValueOfType(velECI[2], UtInput::cSPEED);

      // we don't have the central body yet, so assume the default.
      ut::OrbitalState initialState(ut::EarthWGS84(),
                                    ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                    ut::OrbitalState::ReferenceFrame::cECI);
      initialState.SetOrbitalStateVector(ut::OrbitalState::Vector(locECI, velECI));
      if (GetPropagator().GetInitialOrbitalState().HasEpoch())
      {
         initialState.SetEpoch(GetPropagator().GetInitialOrbitalState().GetEpoch());
      }
      GetPropagator().SetInitialOrbitalState(initialState);
   }
   else if (command == "show_status")
   {
      mDebugShowStatus = true;
   }
   else
   {
      ok = WsfSpaceMoverBase::ProcessInput(aInput);
   }

   return ok;
}

void WsfSpaceMover::ShowStatus(double aSimTime)
{
   const UtTwoLineElement& tle =
      static_cast<const UtTwoLineElement&>(GetPropagator().GetOrbitalState().GetOrbitalElements());
   auto logger = ut::log::info() << "Two line element data";
   logger.AddNote() << "Platform: " << GetPlatform()->GetName();
   tle.PrintDiagnostic();
}

const WsfDateTime& WsfSpaceMover::GetDateTime() const
{
   return GetSimulation()->GetDateTime();
}
