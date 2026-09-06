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

#include "WsfOrbitalManeuversNormal.hpp"

#include <iostream>

#include "UtCalendar.hpp"
#include "UtLog.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfOrbitalManeuvering.hpp"
#include "WsfPlatform.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace WsfOrbitalManeuvers
{
Normal::Normal(const WsfScenario& aScenario)
   : Normal::Normal(aScenario, 0.0)
{
}

Normal::Normal(const WsfScenario& aScenario, const UtSpeedValue& aDeltaV)
   : WsfOrbitalManeuver(aScenario)
   , mManeuverDeltaV(aDeltaV)
   , mAsFraction(false)
{
   SetType(GetTypeId());
}

WsfStringId Normal::GetTypeId()
{
   return "NORMAL";
}

bool Normal::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "delta_v")
   {
      aInput.ReadUnitValue(mManeuverDeltaV);
   }
   else if (command == "delta_v_fraction")
   {
      aInput.ReadValue(mFractionalDeltaV);
      mAsFraction = true;
   }
   else
   {
      myCommand = WsfOrbitalManeuver::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitalManeuvers::Normal::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval = mManeuverDeltaV != 0.0;
   if (!retval)
   {
      auto logger = ut::log::error() << "Must specify a nonzero delta.";
      logger.AddNote() << "Type: " << GetType();
   }
   return WsfOrbitalManeuver::ValidateParameterRanges(aExecutingPlatformName) && retval;
}

bool WsfOrbitalManeuvers::Normal::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePreconditions(aContext);
   if (ok)
   {
      ok = fabs(mManeuverDeltaV) > 0.0;
      if (!ok)
      {
         auto logger = ut::log::error() << "Must specify a nonzero delta_v.";
         logger.AddNote() << "Type: " << GetType();
      }
   }
   return ok;
}

// virtual
void Normal::ComputeDeltaV(const UtCalendar&              aCurrentTime,
                           const UtCalendar&              aEvaluationTime,
                           const UtOrbitalPropagatorBase& aPropagator,
                           UtVec3d&                       aDeltaV) const
{
   ut::OrbitalState::Vector osvInertial = aPropagator.GetOrbitalState().GetOrbitalStateVectorInertial();
   UtVec3d                  velECI      = osvInertial.GetVelocity();
   UtVec3d                  locECI      = osvInertial.GetLocation();
   UtVec3d                  angMomentum;
   angMomentum.CrossProduct(locECI, velECI);
   angMomentum.Normalize();

   // Note that the normal burn can be in the positive or negative direction.
   // We must recover this information as the remaining delta-v variable is always positive.
   if (std::signbit(mManeuverDeltaV))
   {
      angMomentum.Multiply(-mRemainingDeltaV);
   }
   else
   {
      angMomentum.Multiply(mRemainingDeltaV);
   }

   aDeltaV.Set(angMomentum);
}

bool WsfOrbitalManeuvers::Normal::InitializeVariables(const UtCalendar&                        aCurrentTime,
                                                      const UtCalendar&                        aEvaluationTime,
                                                      const wsf::space::OrbitalMissionContext& aContext)
{
   auto propPtr = ut::clone(aContext.GetPropagator());
   propPtr->Initialize(aContext.GetPropagator().GetOrbitalState());
   propPtr->Update(aEvaluationTime);
   if (mAsFraction) // We entered a fractional value for the deltaV
   {
      // and need to scale it now.
      UtVec3d velECI = aContext.GetPropagator().GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
      mManeuverDeltaV.Set(mFractionalDeltaV * velECI.Magnitude(), UtUnitSpeed::cMETERS_PER_SECOND);
   }
   mRemainingDeltaV = fabs(mManeuverDeltaV.GetAsUnit(UtUnitSpeed::cMETERS_PER_SECOND));
   return true;
}

void WsfOrbitalManeuvers::Normal::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitNormal(this);
}

} // namespace WsfOrbitalManeuvers
