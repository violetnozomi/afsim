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

#include "WsfOrbitalManeuversDrift.hpp"

#include <cmath>

#include "UtLog.hpp"
#include "UtOrbitalState.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfPropagatorTargetPoint.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace WsfOrbitalManeuvers
{
Drift::Drift(const WsfScenario& aScenario)
   : Rendezvous(aScenario)
{
   SetType(GetTypeId());
}

Drift::Drift(const WsfScenario&  aScenario,
             OptimizeOption      aOptimizeOption,
             const UtTimeValue&  aMaxTime,
             const UtSpeedValue& aMaxDeltaV)
   : Rendezvous(aScenario, wsf::space::TargetPointOptions{WsfTrackId{}}, aOptimizeOption, aMaxTime, aMaxDeltaV)
{
   SetType(GetTypeId());
}

Drift::Drift(const WsfScenario&                      aScenario,
             const wsf::space::OrbitalTargetingCost& aOptimizeCost,
             const UtTimeValue&                      aMaxTime,
             const UtSpeedValue&                     aMaxDeltaV)
   : Rendezvous(aScenario, wsf::space::TargetPointOptions{WsfTrackId{}}, aOptimizeCost, aMaxTime, aMaxDeltaV)
{
   SetType(GetTypeId());
}

bool Drift::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   if (!VerifyCondition())
   {
      auto msg = ut::log::error() << "Condition type not allowed.";
      msg.AddNote() << "Type: " << GetType();
      msg.AddNote() << "Condition: " << GetConditionString();
      return false;
   }

   if (!aContext.GetPropagator().OrbitIsCircular())
   {
      auto msg = ut::log::error() << "Must have a circular orbit for maneuver.";
      msg.AddNote() << "Type: " << GetType();
      return false;
   }

   bool       retval{true};
   UtCalendar evalTime = ComputeContraintEpoch(aEpoch, aContext.GetPropagator(), retval);
   if (retval)
   {
      auto propPtr = ut::clone(aContext.GetPropagator());
      propPtr->Initialize(aContext.GetPropagator().GetOrbitalState());
      propPtr->Update(evalTime);

      // work out the new radius
      auto   state = propPtr->GetOrbitalState();
      double R0    = state.GetOrbitalStateVector().GetLocation().Magnitude();
      double n0    = sqrt(state.GetGravitationalParameter() / (R0 * R0 * R0));

      // The following expression is a rearrangement of
      //   mDriftRate = n - n0
      // where n is the mean motion for the new radius, and n0 is the mean motion
      // at the initial radius.
      double alpha = std::pow(1.0 + mDriftRate / n0, 1.0 / 3.0);
      double R     = R0 / (alpha * alpha);

      if (R < propPtr->GetCentralBody().GetMeanRadius())
      {
         auto msg = ut::log::error() << "Final radius would be inside central body.";
         msg.AddNote() << "Type: " << GetType();
         msg.AddNote() << "Final Radius: " << R << " m";
         msg.AddNote() << "Central Body: " << propPtr->GetCentralBody().GetName();
         msg.AddNote() << "Central Body Radius: " << propPtr->GetCentralBody().GetMeanRadius() << " m";
         retval = false;
      }
      else
      {
         UtVec3d pos = state.GetOrbitalStateVector().GetLocation();
         pos.Multiply(R / R0);
         UtVec3d vel = state.GetOrbitalStateVector().GetVelocity();
         vel.Multiply(alpha);

         state.SetOrbitalStateVector(ut::OrbitalStateVector{pos, vel});

         propPtr->SetInitialOrbitalState(state);
         propPtr->Initialize(evalTime);

         // Set the target point
         auto targetPtr = ut::make_unique<wsf::space::PropagatorTargetPoint>(std::move(propPtr));
         SetTargetPoint(std::move(targetPtr));
      }
   }
   else
   {
      auto msg = ut::log::error() << "Unable to compute constraint time.";
      msg.AddNote() << "Type: " << GetType();
      msg.AddNote() << "Constraint: " << GetConditionString();
   }

   return retval && Rendezvous::Initialize(aEpoch, aContext);
}

bool Drift::VerifyCondition() const
{
   auto type = GetCondition().GetType();
   return type == wsf::space::NoneCondition::cTYPE || type == wsf::space::RelativeTimeCondition::cTYPE ||
          type == wsf::space::AscendingNodeCondition::cTYPE || type == wsf::space::DescendingNodeCondition::cTYPE ||
          type == wsf::space::EclipseEntryCondition::cTYPE || type == wsf::space::EclipseExitCondition::cTYPE;
}

bool Drift::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval = Rendezvous::ValidateParameterRanges(aExecutingPlatformName);

   if (mDriftRate == 0.0)
   {
      auto msg = ut::log::error() << "drift_rate must be non-zero.";
      msg.AddNote() << "Type: " << GetType();
      retval = false;
   }

   return retval;
}

bool Drift::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command = aInput.GetCommand();
   if (command == "target" || command == "platform" || command == "offset")
   {
      // Disallow certain commands that the base class accepts.
      throw UtInput::UnknownCommand{aInput};
   }
   else if (command == "drift_rate")
   {
      aInput.ReadUnitValue(mDriftRate);
   }
   else
   {
      myCommand = Rendezvous::ProcessInput(aInput);
   }
   return myCommand;
}

void Drift::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitDrift(this);
}

} // namespace WsfOrbitalManeuvers
