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

#include "WsfOrbitalManeuversChangeInclination.hpp"

#include <iostream>

#include "UtDCM.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace WsfOrbitalManeuvers
{
ChangeInclination::ChangeInclination(const WsfScenario& aScenario)
   : ChangeInclination::ChangeInclination(aScenario, -1.0)
{
}

ChangeInclination::ChangeInclination(const WsfScenario& aScenario, const UtAngleValue& aInclination)
   : WsfOrbitalManeuver(aScenario)
   , mInclination(aInclination)
{
   SetType(GetTypeId());
}

WsfStringId ChangeInclination::GetTypeId()
{
   return "CHANGE_INCLINATION";
}

bool ChangeInclination::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "inclination")
   {
      aInput.ReadUnitValue(mInclination);
      aInput.ValueInClosedRange<double>(mInclination, 0.0, UtMath::cPI);
   }
   else if (command == "zero_inclination")
   {
      mInclination = 0.0;
   }
   else
   {
      myCommand = WsfOrbitalManeuver::ProcessInput(aInput);
   }
   return myCommand;
}

bool ChangeInclination::EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator)
{
   // todo check time
   double inclination = aPropagator.GetOrbitalState().GetOrbitalElementsTOD().GetInclination();
   return (fabs(inclination - mInclination) < 0.001);
}

bool WsfOrbitalManeuvers::ChangeInclination::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePreconditions(aContext);

   if (ok)
   {
      if (!aContext.GetPropagator().OrbitIsEquatorial())
      {
         auto t = GetCondition().GetType();
         ok     = (t == wsf::space::AscendingNodeCondition::cTYPE) || (t == wsf::space::DescendingNodeCondition::cTYPE);
         if (!ok)
         {
            auto logger = ut::log::error() << "Execution constraint must be either ascending_node or descending_node.";
            logger.AddNote() << "Type: " << GetType();
         }
      }
   }

   return ok;
}

bool WsfOrbitalManeuvers::ChangeInclination::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval{true};
   if (mInclination < 0.0 || mInclination > UtMath::cPI)
   {
      auto logger = ut::log::error() << "Inclination must be in the range[0, 180.0 deg].";
      logger.AddNote() << "Type: " << GetType();
      retval = false;
   }
   retval = WsfOrbitalManeuver::ValidateParameterRanges(aExecutingPlatformName) && retval;
   return retval;
}

// virtual
void ChangeInclination::ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                      const UtCalendar&              aEvaluationTime,
                                      const UtOrbitalPropagatorBase& aPropagator,
                                      UtVec3d&                       aDeltaV) const
{
   // Find the velocity at the provided current time.
   auto opPtr = aPropagator.GetManeuveringPropagator();
   opPtr->Update(aCurrentTime);

   double inclination    = opPtr->GetOrbitalState().GetOrbitalElementsTOD().GetInclination();
   double newInclination = mInclination.GetAsUnit("rad");
   if (inclination != newInclination)
   {
      opPtr->Update(aEvaluationTime);

      // Find the desired final velocity at the given evaluation time
      // (time of constraint; either ascending or descending node).
      // The velocity is broken up into its in-plane and radial components.
      //    The in-plane component will rotate through an angle equal to the change in inclination
      //    The radial component will be same before and after the maneuver (with a sign change in some cases)

      ut::OrbitalState::Vector stateECI = opPtr->GetOrbitalState().GetOrbitalStateVectorInertial();
      UtVec3d                  posECI   = stateECI.GetLocation();
      UtVec3d                  velECI   = stateECI.GetVelocity();

      // Get initial radial velocity vector
      UtVec3d radialDirECI    = posECI.GetNormal();
      double  radialVelECI    = radialDirECI.DotProduct(velECI);
      UtVec3d radialVelVecECI = radialDirECI * radialVelECI;

      // Get initial in-plane velocity vector
      UtVec3d inPlaneVelVecECI = velECI - radialVelVecECI;

      UtDCM M;
      if (velECI[2] > 0)
      {
         // Ascending node; rotate about the radial direction
         M = UtDCM(newInclination - inclination, (radialDirECI).GetData());
      }
      else
      {
         // Descending node; rotate about the negative of the radial direction
         M = UtDCM(newInclination - inclination, (-radialDirECI).GetData());
      }

      // Calculate new in-plane velocity vector
      UtVec3d newInPlaneVelVecECI = M.Transform(inPlaneVelVecECI);

      // Calculate new radial velocity vector
      //    If the final orbit is equatorial, the radial velocity will change sign
      //    because of changes to how raan and argument of periapsis are defined.
      UtVec3d newRadialVelVecECI = radialVelECI * radialDirECI;
      if (UtMath::NearlyEqual(newInclination, 0.0) || UtMath::NearlyEqual(newInclination, UtMath::cPI))
      {
         newRadialVelVecECI *= -1.0;
      }

      // Add in-plane and radial components together to get the final velocity
      UtVec3d newVelECI;
      newVelECI.Add(newInPlaneVelVecECI, newRadialVelVecECI);

      // The difference between the two velocities is the desired delta-v.
      aDeltaV.Subtract(newVelECI, velECI);
   }
   else
   {
      aDeltaV.Set(0.0);
   }
}

void ChangeInclination::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitChangeInclination(this);
}

} // namespace WsfOrbitalManeuvers
