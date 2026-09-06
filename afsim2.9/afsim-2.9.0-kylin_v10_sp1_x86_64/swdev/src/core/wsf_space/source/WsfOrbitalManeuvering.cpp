// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalManeuvering.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtVec3.hpp"
#include "WsfDateTime.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

bool WsfOrbitalManeuvering::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;
   if (command == "debug")
   {
      mDebug = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool WsfOrbitalManeuvering::Initialize(const UtCalendar& aEpoch)
{
   mUpdateTime = aEpoch;
   if (mDebug && (GetNameId() != 0))
   {
      auto logger = ut::log::debug() << "Initializing orbital maneuvering";
      logger.AddNote() << "Name: " << GetName();
      logger.AddNote() << "Delta-V: " << GetAvailableDeltaV() << " m/s";
      logger.AddNote() << "Total Duration: " << GetRequiredDuration(GetAvailableDeltaV()) << " s";
   }
   return true;
}

//! Get the duration of the current maneuver for the given delta-v.  If a duration was specified for the maneuver,
//! that duration will be returned if it exceeds the value calculated by the model.
//!@param aEpoch The absolute time of execution of the maneuver.
//!@param aManeuver The maneuver to evaluate.
//!@param aDeltaV The delta-v to be expended in the maneuver.
//!@return The expected total duration of the maneuver.
//!@note This method assumes sufficient delta-v is available (GetAvailableDeltaV() >= aDeltaV).
double WsfOrbitalManeuvering::GetManeuverDuration(const UtCalendar&         aEpoch,
                                                  const WsfOrbitalManeuver& aManeuver,
                                                  double                    aDeltaV) const
{
   double duration = GetRequiredDuration(aDeltaV);
   if (static_cast<double>(aManeuver.GetDuration()) > 0.0)
   {
      double timeSinceStart = aEpoch.GetTimeSince(aManeuver.GetStartTime());
      duration              = std::max(duration, static_cast<double>(aManeuver.GetDuration()) - timeSinceStart);
   }
   return duration;
}

//! Return the remaining delta-V available.  The value is allowed to go negative for verification purposes.
//! Any negative value indicates no more delta-v is available.
double WsfOrbitalManeuvering::GetAvailableDeltaV() const
{
   return cINFINITE_DELTA_V;
}

//! Return the available duration corresponding with the available delta-v.
double WsfOrbitalManeuvering::GetAvailableDuration() const
{
   return cINFINITE_DURATION;
}

//! Remove available delta V if it is being monitored.  Change any internal state as appropriate.
//! @return true if the delta-v could be removed; false otherwise.
bool WsfOrbitalManeuvering::ReduceAvailableDeltaV_By(double aDeltaV)
{
   return true;
}

//! Apply a delta-v over a specified period of time.
//!@param aEpoch The absolute time to which we update the maneuver.
//!@param aManeuver The maneuver that is being executed.
//!@param aCommandedDeltaV The total delta-v needed to complete the maneuver at the given time.
//!@param aActualDeltaV The actual delta-v that can be produced by the mover in the given delta-t.
//!@return Whether the maneuver successfully executed.
//!@note Derived classes should override this method to provide a higher-fidelity maneuver implementation.
bool WsfOrbitalManeuvering::Maneuver(const UtCalendar&         aEpoch,
                                     const WsfOrbitalManeuver& aManeuver,
                                     const UtVec3d&            aCommandedDeltaV,
                                     UtVec3d&                  aActualDeltaV)
{
   bool executed = true;
   aActualDeltaV.Set(0.0);
   double availableDeltaV = GetAvailableDeltaV();

   double commandedDeltaV_Mag = aCommandedDeltaV.Magnitude();
   if (commandedDeltaV_Mag < availableDeltaV)
   {
      if (!aManeuver.IsFinite())
      {
         aActualDeltaV.Set(aCommandedDeltaV);
         executed = ReduceAvailableDeltaV_By(aActualDeltaV.Magnitude());
      }
      else // Finite maneuvers
      {
         double deltaT = 0.0;
         if (aManeuver.HasExecuted())
         {
            deltaT = aEpoch.GetTimeSince(mUpdateTime);
         }
         else
         {
            deltaT = aEpoch.GetTimeSince(aManeuver.GetStartTime());
         }
         if (deltaT > 0.0)
         {
            double scalarDeltaV = aCommandedDeltaV.Magnitude();
            double duration     = GetManeuverDuration(aEpoch, aManeuver, scalarDeltaV);
            assert(duration != cINFINITE_DURATION);
            if (duration > deltaT)
            {
               double fractionalDeltaV = deltaT / duration;
               aActualDeltaV.Multiply(aCommandedDeltaV, fractionalDeltaV);
               mUpdateTime = aEpoch;
            }
            else
            {
               aActualDeltaV.Set(aCommandedDeltaV);
               mUpdateTime.AdvanceTimeBy(duration);
            }
            executed = ReduceAvailableDeltaV_By(aActualDeltaV.Magnitude());
         }
      }
   }
   else
   {
      executed    = false;
      auto logger = ut::log::warning() << "Delta-V not sufficient to complete desired maneuver.";
      logger.AddNote() << "Type: " << aManeuver.GetType();
      logger.AddNote() << "Required Delta-V: " << commandedDeltaV_Mag;
      logger.AddNote() << "Available Delta-V: " << availableDeltaV;
   }

   mUpdateTime = aEpoch;
   return executed;
}

// virtual
//! Default method returns false (see WsfRocketOrbitalManeuvering).
bool WsfOrbitalManeuvering::PerformStagingOperation()
{
   auto logger = ut::log::error() << "Maneuvering model does not support staging operations.";
   logger.AddNote() << "Type: " << GetType();
   return false;
}
