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

#include "WsfOrbitalManeuversChangeEccentricity.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace WsfOrbitalManeuvers
{
ChangeEccentricity::ChangeEccentricity(const WsfScenario& aScenario)
   : ChangeEccentricity::ChangeEccentricity(aScenario, -1.0)
{
}

ChangeEccentricity::ChangeEccentricity(const WsfScenario& aScenario, double aEccentricity)
   : WsfOrbitalManeuver(aScenario)
   , mEccentricity(aEccentricity)
{
   SetType(GetTypeId());
}

WsfStringId ChangeEccentricity::GetTypeId()
{
   return "CHANGE_ECCENTRICITY";
}

bool ChangeEccentricity::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "eccentricity")
   {
      aInput.ReadValue(mEccentricity);
      aInput.ValueInClosedRange(mEccentricity, 0.0, 1.0);
   }
   else if (command == "zero_eccentricity")
   {
      mEccentricity = 0.0;
   }
   else
   {
      myCommand = WsfOrbitalManeuver::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitalManeuvers::ChangeEccentricity::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePreconditions(aContext);

   if (ok)
   {
      auto t = GetCondition().GetType();
      if (!aContext.GetPropagator().OrbitIsCircular())
      {
         ok = ((t == wsf::space::PeriapsisCondition::cTYPE) || (t == wsf::space::ApoapsisCondition::cTYPE));
         if (!ok)
         {
            auto logger = ut::log::error() << "Execution constraint must be either at_periapsis or at_apoapsis.";
            logger.AddNote() << "Type: " << GetType();
         }
      }
      else
      {
         ok = ((t == wsf::space::PeriapsisCondition::cTYPE) || (t == wsf::space::NoneCondition::cTYPE));
         if (!ok)
         {
            auto logger = ut::log::error()
                          << "Execution constraint must be either absent (none) or at_periapsis or (circular orbit).";
            logger.AddNote() << "Type: " << GetType();
         }
      }
   }

   return ok;
}

bool WsfOrbitalManeuvers::ChangeEccentricity::EvaluatePostconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePostconditions(aContext);

   if (ok)
   {
      ok = (!aContext.GetPropagator().GetOrbitalState().OrbitIntersectsCentralBody());
      if (!ok)
      {
         auto logger = ut::log::error() << "Final orbit periapsis intersects Earth.";
         logger.AddNote() << "Type: " << GetType();
      }
   }

   return ok;
}

bool WsfOrbitalManeuvers::ChangeEccentricity::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval{true};
   if (mEccentricity < 0.0 || mEccentricity > 1.0)
   {
      auto logger = ut::log::error() << "Eccentricity must be in the range [0, 1].";
      logger.AddNote() << "Type: " << GetType();
      retval = false;
   }
   retval = WsfOrbitalManeuver::ValidateParameterRanges(aExecutingPlatformName) && retval;
   return retval;
}

// virtual
void ChangeEccentricity::ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                       const UtCalendar&              aEvaluationTime,
                                       const UtOrbitalPropagatorBase& aPropagator,
                                       UtVec3d&                       aDeltaV) const
{
   // Use the current set of orbital elements, with the new epoch and current anomaly,
   // as the basis for the new orbit.
   auto opPtr = aPropagator.GetManeuveringPropagator();

   // Compute and set the new semi-major axis that results from the eccentricity change.
   opPtr->Update(aCurrentTime);
   ut::OrbitalState::Vector osvInertial = opPtr->GetOrbitalState().GetOrbitalStateVectorInertial();
   UtVec3d                  rVec        = osvInertial.GetLocation();
   UtVec3d                  velECI      = osvInertial.GetVelocity();

   UtOrbitalElements initialElements = opPtr->GetOrbitalState().GetOrbitalElements();
   initialElements.SetEpoch(aCurrentTime);

   // Set the new eccentricity.
   initialElements.SetEccentricity(mEccentricity);

   double a = rVec.Magnitude();
   if ((GetCondition().GetType() == wsf::space::PeriapsisCondition::cTYPE) || (opPtr->OrbitIsCircular()))
   {
      a /= (1.0 - mEccentricity);
   }
   else
   {
      a /= (1.0 + mEccentricity);
   }
   initialElements.SetSemiMajorAxis(a);

   // Set the elements in a propagator and retrieve the new velocity.
   opPtr->Initialize(ut::OrbitalState(opPtr->GetOrbitalState().GetCoordinateSystem(),
                                      opPtr->GetOrbitalState().GetReferenceFrame(),
                                      initialElements));
   opPtr->Update(aEvaluationTime);
   UtVec3d newVelECI = opPtr->GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();

   // The difference between the new orbit velocity and the current one is the delta-v.
   aDeltaV.Subtract(newVelECI, velECI);
}

void ChangeEccentricity::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitChangeEccentricity(this);
}

} // namespace WsfOrbitalManeuvers
