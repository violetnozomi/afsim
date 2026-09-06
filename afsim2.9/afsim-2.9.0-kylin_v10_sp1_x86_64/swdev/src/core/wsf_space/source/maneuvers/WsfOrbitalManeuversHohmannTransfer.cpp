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

#include "WsfOrbitalManeuversHohmannTransfer.hpp"

#include <iostream>
#include <sstream>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeSemiMajorAxis.hpp"

namespace WsfOrbitalManeuvers
{
HohmannTransfer::HohmannTransfer(const WsfScenario& aScenario)
   : HohmannTransfer::HohmannTransfer(aScenario, 0.0)
{
}

HohmannTransfer::HohmannTransfer(const WsfScenario& aScenario, const UtLengthValue& aFinalRadius)
   : WsfOrbitalMissionSequence(aScenario)
   , mFinalSMA(aFinalRadius)
   , mInputAsRadius{false}
{
   SetType(GetTypeId());
}

WsfStringId HohmannTransfer::GetTypeId()
{
   return "HOHMANN_TRANSFER";
}

bool HohmannTransfer::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   // A Hohmann transfer consists of two separate change semi-major axis MissionEvents.
   // Compute the intermediate semi major axis, create, and schedule the two MissionEvents.
   double a1 = aContext.GetPropagator().GetOrbitalState().GetOrbitalElements().GetSemiMajorAxis();
   double e1 = aContext.GetPropagator().GetOrbitalState().GetOrbitalElements().GetEccentricity();

   // The transfer SMA will be set below.
   auto transferManeuverPtr = ut::make_unique<ChangeSemiMajorAxis>(GetScenario());
   auto finalManeuverPtr    = ut::make_unique<ChangeSemiMajorAxis>(GetScenario(), mFinalSMA);

   // Determine whether this transfer is to a higher orbit or a lower orbit.
   // The calculation of the transfer SMA is slightly different for each case.
   if (mFinalSMA > a1) // Final SMA is a higher orbit
   {
      double transferSMA = (mFinalSMA + a1 * (1.0 - e1)) / 2.0;
      transferManeuverPtr->SetFinalSMA(transferSMA);
      transferManeuverPtr->SetCondition(ut::make_unique<wsf::space::PeriapsisCondition>());
      finalManeuverPtr->SetCondition(ut::make_unique<wsf::space::ApoapsisCondition>());
   }
   else // Final SMA is a lower orbit.
   {
      double transferSMA = (mFinalSMA + a1 * (1.0 + e1)) / 2.0;
      transferManeuverPtr->SetFinalSMA(transferSMA);
      transferManeuverPtr->SetCondition(ut::make_unique<wsf::space::ApoapsisCondition>());
      finalManeuverPtr->SetCondition(ut::make_unique<wsf::space::PeriapsisCondition>());
   }

   if (IsFinite())
   {
      transferManeuverPtr->SetFinite(true);
      finalManeuverPtr->SetFinite(true);
   }
   AddMissionEvent(std::move(transferManeuverPtr));
   AddMissionEvent(std::move(finalManeuverPtr));
   return WsfOrbitalMissionSequence::Initialize(aEpoch, aContext);
}

bool WsfOrbitalManeuvers::HohmannTransfer::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePreconditions(aContext);
   if (ok)
   {
      ok = (mFinalSMA > UtSphericalEarth::cEARTH_MEAN_RADIUS);
      if (!ok)
      {
         auto logger = ut::log::error() << "You must specify a final_semi_major_axis greater than Earth radius.";
         logger.AddNote() << "Type: " << GetType();
      }
   }
   return ok;
}

bool WsfOrbitalManeuvers::HohmannTransfer::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval{true};
   if (mFinalSMA <= 0.0)
   {
      auto logger = ut::log::error() << "final_semi_major_axis must be greater than 0.";
      logger.AddNote() << "Type: " << GetType();
      retval = false;
   }
   retval = WsfOrbitalEvent::ValidateParameterRanges(aExecutingPlatformName) && retval;
   return retval;
}

bool HohmannTransfer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if ((command == "final_semi_major_axis"))
   {
      aInput.ReadUnitValue(mFinalSMA);
      aInput.ValueGreater<double>(mFinalSMA, 0.0);
   }
   else if (command == "final_radius")
   {
      aInput.ReadUnitValue(mFinalSMA);
      aInput.ValueGreater<double>(mFinalSMA, 0.0);
      mInputAsRadius = true;
   }
   else
   {
      myCommand = WsfOrbitalMissionSequence::ProcessInput(aInput);
   }
   return myCommand;
}

void HohmannTransfer::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitHohmannTransfer(this);
}

void HohmannTransfer::SetFinalSMA(const UtLengthValue& aSMA)
{
   mFinalSMA      = aSMA;
   mInputAsRadius = false;
}

void HohmannTransfer::SetFinalRadius(const UtLengthValue& aRadius)
{
   mFinalSMA      = aRadius;
   mInputAsRadius = true;
}

} // namespace WsfOrbitalManeuvers
