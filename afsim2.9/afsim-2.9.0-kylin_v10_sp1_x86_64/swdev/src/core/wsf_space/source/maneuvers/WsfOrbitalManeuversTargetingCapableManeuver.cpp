// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalManeuversTargetingCapableManeuver.hpp"

#include <string>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"
#include "WsfTrackManager.hpp"

namespace WsfOrbitalManeuvers
{

bool TargetingCapableManeuver::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "target")
   {
      UtInputBlock block{aInput};
      block.ProcessInput(&mTargetOptions);
   }
   else
   {
      myCommand = WsfOrbitalManeuver::ProcessInput(aInput);
   }
   return myCommand;
}

bool TargetingCapableManeuver::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   if (!mTargetOptions.IsValid())
   {
      auto logger = ut::log::error()
                    << "Must specify either a valid platform name, local track id, or libration point to intercept.";
      logger.AddNote() << "Type: " << GetType();
      return false;
   }

   bool ok = WsfOrbitalManeuver::Initialize(aEpoch, aContext);
   if (ok && aContext.GetSimulation())
   {
      // Instantiate the point
      if (mTargetOptions.RequiresTargetPropagator())
      {
         mTargetOptions.SetTargetPropagator(ut::clone(GetTargetPropagator(aContext)));
      }
      if (mTargetOptions.RequiresManeuveringPropagator())
      {
         mTargetOptions.SetManeuveringPropagator(ut::clone(aContext.GetPropagator()));
      }
      mTargetPtr = mTargetOptions.InstantiatePoint(aEpoch);
      if (!mTargetPtr)
      {
         ok = false;
      }
   }
   return ok;
}

bool TargetingCapableManeuver::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval = mTargetOptions.ValidateParameterRanges(aExecutingPlatformName, GetType());
   return WsfOrbitalManeuver::ValidateParameterRanges(aExecutingPlatformName) && retval;
}

const UtOrbitalPropagatorBase* TargetingCapableManeuver::GetTargetPropagator(const wsf::space::OrbitalMissionContext& aContext) const
{
   const UtOrbitalPropagatorBase* propPtr{nullptr};
   if (!GetLocalTrackId().IsNull())
   {
      const WsfSpaceMoverBase* spaceMoverPtr = aContext.GetSpaceMover();
      if (spaceMoverPtr)
      {
         WsfLocalTrack* ltPtr = spaceMoverPtr->GetPlatform()->GetTrackManager().FindTrack(GetLocalTrackId());
         if (ltPtr)
         {
            propPtr = GetTargetPropagatorPerception(*ltPtr, aContext);
         }
         else
         {
            auto logger = ut::log::warning() << "Unable to find a local track corresponding with local track id.";
            logger.AddNote() << "Local track ID: " << GetLocalTrackId();
         }
      }
   }
   if (!propPtr)
   {
      propPtr = GetTargetPropagatorTruth(GetPlatformName(), aContext.GetSimulation());
   }
   return propPtr;
}

void TargetingCapableManeuver::UpdateTargetPoint(const UtCalendar& aEpoch) const
{
   if (mTargetPtr)
   {
      mTargetPtr->Update(aEpoch);
   }
}

} // namespace WsfOrbitalManeuvers
