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

#include "WsfOrbitalManeuversChangeRAAN.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace WsfOrbitalManeuvers
{
// static
namespace // anonymous
{
double cCHANGE_RAAN_INVALID_ANGLE = -999.0;
}

ChangeRAAN::ChangeRAAN(const WsfScenario& aScenario)
   : ChangeRAAN_Inclination(aScenario)
{
}

ChangeRAAN::ChangeRAAN(const WsfScenario& aScenario, const UtAngleValue& aRAAN)
   : ChangeRAAN_Inclination(aScenario, aRAAN, cCHANGE_RAAN_INVALID_ANGLE)
{
   SetType(GetTypeId());
}

WsfStringId ChangeRAAN::GetTypeId()
{
   return "CHANGE_RAAN";
}

bool ChangeRAAN::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "inclination")
   {
      // Because we delegate to ChangeRAAN_Inclination, we have to intercept 'inclination'
      // and raise an error here, before we delegate.
      myCommand = false;
   }
   else
   {
      myCommand = ChangeRAAN_Inclination::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitalManeuvers::ChangeRAAN::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   SetInclination(aContext.GetPropagator().GetOrbitalState().GetOrbitalElementsTOD().GetInclination());
   return ChangeRAAN_Inclination::Initialize(aEpoch, aContext);
}

bool WsfOrbitalManeuvers::ChangeRAAN::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePreconditions(aContext);
   if (ok)
   {
      ok &= aContext.GetPropagator().OrbitIsCircular();
      if (ok)
      {
         ok = (!aContext.GetPropagator().OrbitIsEquatorial());
         if (!ok)
         {
            auto logger = ut::log::error() << "Maneuver invalid for equatorial orbits.";
            logger.AddNote() << "Type: " << GetType();
         }
         else
         {
            ok = GetRAAN() != cCHANGE_RAAN_INVALID_ANGLE;
            if (!ok)
            {
               auto logger = ut::log::error() << "You must specify a valid RAAN.";
               logger.AddNote() << "Type: " << GetType();
            }
         }
      }
      else
      {
         auto logger = ut::log::error() << "Initial orbit must be circular to execute this maneuver.";
         logger.AddNote() << "Type: " << GetType();
      }
   }
   return ok;
}

bool WsfOrbitalManeuvers::ChangeRAAN::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval = ValidateRAAN();
   retval      = WsfOrbitalManeuver::ValidateParameterRanges(aExecutingPlatformName) && retval;
   return retval;
}

void ChangeRAAN::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitChangeRAAN(this);
}

} // namespace WsfOrbitalManeuvers
