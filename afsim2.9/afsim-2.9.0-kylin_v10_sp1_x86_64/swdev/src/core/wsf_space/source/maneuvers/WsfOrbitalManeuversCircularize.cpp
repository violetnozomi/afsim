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

#include "WsfOrbitalManeuversCircularize.hpp"

#include <iostream>

#include "UtLog.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace WsfOrbitalManeuvers
{
Circularize::Circularize(const WsfScenario& aScenario)
   : WsfOrbitalManeuver(aScenario)
{
   SetType(GetTypeId());
}

WsfStringId Circularize::GetTypeId()
{
   return "CIRCULARIZE";
}

bool WsfOrbitalManeuvers::Circularize::VerifyCondition() const
{
   auto t = mCondition->GetType();
   return ((t == wsf::space::AscendingRadiusCondition::cTYPE) || (t == wsf::space::DescendingRadiusCondition::cTYPE));
}

bool WsfOrbitalManeuvers::Circularize::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePreconditions(aContext);

   if (ok)
   {
      double periapsis = aContext.GetPropagator().GetOrbitalState().GetOrbitalElements().GetPeriapsisRadius();
      double apoapsis  = aContext.GetPropagator().GetOrbitalState().GetOrbitalElements().GetApoapsisRadius();
      auto   radius    = GetRadius();
      ok               = ((radius >= periapsis) && (radius <= apoapsis));
      if (!ok)
      {
         auto logger = ut::log::error() << "Circularize radius out of bounds.";
         logger.AddNote() << "Must be between " << periapsis / 1000.0 << " and " << apoapsis / 1000.0 << " km";
         logger.AddNote() << "Type: " << GetType();
      }
   }

   return ok;
}

// virtual
void Circularize::ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                const UtCalendar&              aEvaluationTime,
                                const UtOrbitalPropagatorBase& aPropagator,
                                UtVec3d&                       aDeltaV) const
{
   double newSpeed = sqrt(aPropagator.GetOrbitalState().GetCentralBody().GetGravitationalParameter() / GetRadius());
   ut::OrbitalState::Vector osvInertial = aPropagator.GetOrbitalState().GetOrbitalStateVectorInertial();
   UtVec3d                  velECI      = osvInertial.GetVelocity();
   UtVec3d                  locECI      = osvInertial.GetLocation();
   // find direction of new velocity from cross products
   UtVec3d vCrossR;
   vCrossR.CrossProduct(velECI, locECI);
   UtVec3d circVelECI;
   circVelECI.CrossProduct(locECI, vCrossR);
   circVelECI.Normalize();
   circVelECI.Multiply(newSpeed);
   aDeltaV.Subtract(circVelECI, velECI);
}

void Circularize::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitCircularize(this);
}

UtLengthValue Circularize::GetRadius() const
{
   // If the cast fails, this will throw std::bad_cast, which is not caught here
   // because that would indicate a usage error.
   const auto& c = dynamic_cast<const wsf::space::RadiusCondition&>(GetCondition());
   return c.GetRadius();
}

} // namespace WsfOrbitalManeuvers
