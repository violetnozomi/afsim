// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalManeuversMatchVelocity.hpp"

#include <iostream>

#include "UtCalendar.hpp"
#include "UtLog.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfAttitudeController.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMover.hpp"
#include "WsfSpaceOrientation.hpp"
#include "WsfTrackManager.hpp"

namespace WsfOrbitalManeuvers
{
MatchVelocity::MatchVelocity(const WsfScenario& aScenario)
   : MatchVelocity(aScenario, WsfTrackId())
{
}

MatchVelocity::MatchVelocity(const WsfScenario& aScenario, const WsfTrackId& aLocalTrackId)
   : TargetingCapableManeuver{aScenario, aLocalTrackId}
{
   mCanVerify = false;
   SetType(GetTypeId());
}

WsfStringId MatchVelocity::GetTypeId()
{
   return "MATCH_VELOCITY";
}

bool MatchVelocity::ProcessInput(UtInput& aInput)
{
   // Note that while this owns a TargetPointOptions object, and so would be able
   // to support all the options presented by that object, this will instead keep
   // the old user interface. The use of the TargetPointOptions object was so
   // that this object can participate in a Rendezvous maneuver (a sequence of a
   // Target followed by a MatchVelocity) for all the valid targets for the
   // Rendezvous, not as a way of extending the stand-alone capabilities of the
   // MatchVelocity maneuver.
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "platform")
   {
      std::string platformName;
      aInput.ReadValue(platformName);
      SetPlatformName(platformName);
   }
   else
   {
      myCommand = TargetingCapableManeuver::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void MatchVelocity::ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                  const UtCalendar&              aEvaluationTime,
                                  const UtOrbitalPropagatorBase& aPropagator,
                                  UtVec3d&                       aDeltaV) const
{
   auto propPtr = ut::clone(aPropagator);
   propPtr->Initialize(aPropagator.GetOrbitalState());
   propPtr->Update(aEvaluationTime);
   UtVec3d currentVelECI = propPtr->GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();

   UpdateTargetPoint(aEvaluationTime);
   UtVec3d targetVelECI = GetTargetPoint()->GetVelocityECI();

   aDeltaV.Subtract(targetVelECI, currentVelECI);
}

void WsfOrbitalManeuvers::MatchVelocity::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitMatchVelocity(this);
}

} // namespace WsfOrbitalManeuvers
