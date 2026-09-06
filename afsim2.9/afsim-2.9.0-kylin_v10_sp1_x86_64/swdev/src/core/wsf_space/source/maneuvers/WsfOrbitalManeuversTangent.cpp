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

#include "WsfOrbitalManeuversTangent.hpp"

#include <iostream>

#include "UtCalendar.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfOrbitalManeuvering.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace WsfOrbitalManeuvers
{
Tangent::Tangent(const WsfScenario& aScenario, const UtSpeedValue& aDeltaV)
   : WsfOrbitalManeuver(aScenario)
   , mManeuverDeltaV(aDeltaV)
{
   SetType(GetTypeId());
}

bool WsfOrbitalManeuvers::Tangent::InitializeVariables(const UtCalendar&                        aCurrentTime,
                                                       const UtCalendar&                        aEvaluationTime,
                                                       const wsf::space::OrbitalMissionContext& aContext)
{
   mInitialSpeed = GetSpeed(aEvaluationTime, aContext.GetPropagator());
   if (mDuration > 0.0)
   {
      mManeuverDeltaV.Set(aContext.GetRequiredDeltaV(mDuration), UtUnitSpeed::cMETERS_PER_SECOND);
      if (mManeuverDeltaV > aContext.GetAvailableDeltaV())
      {
         auto logger = ut::log::error() << "Specified maneuver duration greater than total available.";
         logger.AddNote() << "Type: " << GetType();
         return false;
      }
   }
   if (mAsFraction) // We entered a fractional value for the deltaV
   {
      // and need to scale it now.
      mManeuverDeltaV.Set(mFractionalDeltaV * mInitialSpeed, UtUnitSpeed::cMETERS_PER_SECOND);
   }
   mRemainingDeltaV = fabs(mManeuverDeltaV);
   return true;
}

double Tangent::GetSpeed(const UtCalendar& aTime, const UtOrbitalPropagatorBase& aPropagator) const
{
   return aPropagator.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity().Magnitude();
}

WsfStringId Tangent::GetTypeId()
{
   return "TANGENT";
}

bool Tangent::ProcessInput(UtInput& aInput)
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
      aInput.ValueInClosedRange(mFractionalDeltaV, -1.0, 1.0);
      mAsFraction = true;
   }
   else
   {
      myCommand = WsfOrbitalManeuver::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitalManeuvers::Tangent::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePreconditions(aContext);
   if (ok)
   {
      if ((fabs(mManeuverDeltaV) > 0.0) || (mDuration > 0.0))
      {
         if (!mAsFraction && (fabs(mManeuverDeltaV) > 0.0))
         {
            ok = (fabs(mManeuverDeltaV) <= aContext.GetAvailableDeltaV());
            if (!ok)
            {
               auto logger = ut::log::error() << "The delta_v specified was greater than the total available.";
               logger.AddNote() << "Type: " << GetType();
            }
         }
      }
      else
      {
         ok = false;
         auto logger =
            ut::log::error() << "You must specify a nonzero delta_v, delta_v fraction, or maneuver duration.";
         logger.AddNote() << "Type: " << GetType();
      }
   }
   return ok;
}

bool WsfOrbitalManeuvers::Tangent::EvaluatePostconditions(const wsf::space::OrbitalMissionContext& aContext) const
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

bool Tangent::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval{true};
   if (mAsFraction)
   {
      if (mFractionalDeltaV < -1.0 || mFractionalDeltaV > 1.0)
      {
         auto logger = ut::log::error() << "delta_v_fraction must be in the range [-1, 1].";
         logger.AddNote() << "Type: " << GetType();
         retval = false;
      }
   }
   retval = WsfOrbitalManeuver::ValidateParameterRanges(aExecutingPlatformName) && retval;
   return retval;
}

void Tangent::ComputeDeltaV(const UtCalendar&              aCurrentTime,
                            const UtCalendar&              aEvaluationTime,
                            const UtOrbitalPropagatorBase& aPropagator,
                            UtVec3d&                       aDeltaV) const
{
   UtVec3d velECI = aPropagator.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
   velECI.Normalize();

   // Note that the tangent burn can be in the positive or negative direction.
   // We must recover this information as the remaining delta-v variable is always positive.
   if (std::signbit(mManeuverDeltaV))
   {
      velECI.Multiply(-mRemainingDeltaV);
   }
   else
   {
      velECI.Multiply(mRemainingDeltaV);
   }

   aDeltaV.Set(velECI);
}

void Tangent::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitTangent(this);
}

} // namespace WsfOrbitalManeuvers
