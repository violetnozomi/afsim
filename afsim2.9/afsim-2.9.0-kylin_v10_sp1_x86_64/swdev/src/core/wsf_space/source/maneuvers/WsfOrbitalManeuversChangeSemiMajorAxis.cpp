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

#include "WsfOrbitalManeuversChangeSemiMajorAxis.hpp"

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
void ChangeSemiMajorAxis::ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                        const UtCalendar&              aEvaluationTime,
                                        const UtOrbitalPropagatorBase& aPropagator,
                                        UtVec3d&                       aDeltaV) const
{
   auto propPtr = ut::clone(aPropagator);
   propPtr->Initialize(aPropagator.GetOrbitalState());
   propPtr->Update(aEvaluationTime);
   ut::OrbitalState::Vector osvInertial = propPtr->GetOrbitalState().GetOrbitalStateVectorInertial();
   UtVec3d                  locECI      = osvInertial.GetLocation();
   UtVec3d                  velECI      = osvInertial.GetVelocity();
   double                   rlen        = locECI.Magnitude();
   double                   vlen        = velECI.Magnitude();
   double                   neededVlen =
      sqrt(aPropagator.GetOrbitalState().GetCentralBody().GetGravitationalParameter() * (2.0 / rlen - 1.0 / mFinalSMA));
   double deltaV = neededVlen - vlen;
   velECI.Normalize();
   velECI.Multiply(deltaV);
   aDeltaV.Set(velECI);
}

ChangeSemiMajorAxis::ChangeSemiMajorAxis(const WsfScenario& aScenario)
   : ChangeSemiMajorAxis::ChangeSemiMajorAxis(aScenario, 0.0)
{
}

ChangeSemiMajorAxis::ChangeSemiMajorAxis(const WsfScenario& aScenario, const UtLengthValue& aFinalSMA)
   : WsfOrbitalManeuver(aScenario)
   , mFinalSMA(aFinalSMA)
   , mApoapsisAltitude(0.0)
   , mPeriapsisAltitude(0.0)
   , mInputType{InputOption::cFINAL_SMA}
{
   SetType(GetTypeId());
}

WsfStringId ChangeSemiMajorAxis::GetTypeId()
{
   return "CHANGE_SEMI_MAJOR_AXIS";
}

bool ChangeSemiMajorAxis::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "semi_major_axis")
   {
      aInput.ReadUnitValue(mFinalSMA);
      mInputType = InputOption::cFINAL_SMA;
   }
   else if (command == "radius")
   {
      aInput.ReadUnitValue(mFinalSMA);
      mInputType = InputOption::cRADIUS;
   }
   else if (command == "apoapsis_altitude")
   {
      if (mPeriapsisAltitude == 0.0)
      {
         aInput.ReadUnitValue(mApoapsisAltitude);
         mInputType = InputOption::cAPOAPSIS_ALT;
      }
      else
      {
         ut::log::error() << "You cannot specify both apoapsis_altitude and periapsis_altitude.";
         myCommand = false;
      }
   }
   else if (command == "periapsis_altitude")
   {
      if (mPeriapsisAltitude == 0.0)
      {
         aInput.ReadUnitValue(mPeriapsisAltitude);
         mInputType = InputOption::cPERIAPSIS_ALT;
      }
      else
      {
         ut::log::error() << "You cannot specify both apoapsis_altitude and periapsis_altitude.";
         myCommand = false;
      }
   }
   else
   {
      myCommand = WsfOrbitalManeuver::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitalManeuvers::ChangeSemiMajorAxis::InitializeVariables(const UtCalendar& aCurrentTime,
                                                                   const UtCalendar& aEvaluationTime,
                                                                   const wsf::space::OrbitalMissionContext& aContext)
{
   if (mPeriapsisAltitude != 0.0)
   {
      double apoapsisLength = aContext.GetPropagator().GetOrbitalState().GetOrbitalElements().GetSemiMajorAxis() *
                              (1.0 + aContext.GetPropagator().GetOrbitalState().GetOrbitalElements().GetEccentricity());
      mFinalSMA.Set((apoapsisLength + mPeriapsisAltitude + UtSphericalEarth::cEARTH_MEAN_RADIUS) / 2.0,
                    UtUnitLength::cMETERS);
   }
   else if (mApoapsisAltitude != 0.0)
   {
      double periapsisLength = aContext.GetPropagator().GetOrbitalState().GetOrbitalElements().GetSemiMajorAxis() *
                               (1.0 - aContext.GetPropagator().GetOrbitalState().GetOrbitalElements().GetEccentricity());
      mFinalSMA.Set((periapsisLength + mApoapsisAltitude + UtSphericalEarth::cEARTH_MEAN_RADIUS) / 2.0,
                    UtUnitLength::cMETERS);
   }
   return true;
}

bool ChangeSemiMajorAxis::EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator)
{
   double sma = aPropagator.GetOrbitalState().GetOrbitalElements().GetSemiMajorAxis();
   return (fabs(sma - mFinalSMA.GetAsUnit(UtUnitLength::cMETERS)) < 10.0);
}

bool WsfOrbitalManeuvers::ChangeSemiMajorAxis::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePreconditions(aContext);
   if (ok)
   {
      ok = (mFinalSMA > UtSphericalEarth::cEARTH_MEAN_RADIUS);
      if (!ok)
      {
         auto logger = ut::log::error() << "Final semi-major axis must be greater than earth radius.";
         logger.AddNote() << "Type: " << GetType();
      }
   }
   return ok;
}

void ChangeSemiMajorAxis::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitChangeSemiMajorAxis(this);
}

void ChangeSemiMajorAxis::SetFinalSMA(const UtLengthValue& aSMA)
{
   mFinalSMA          = aSMA;
   mInputType         = InputOption::cFINAL_SMA;
   mApoapsisAltitude  = 0.0;
   mPeriapsisAltitude = 0.0;
}

void ChangeSemiMajorAxis::SetRadius(const UtLengthValue& aRadius)
{
   mFinalSMA          = aRadius;
   mInputType         = InputOption::cRADIUS;
   mApoapsisAltitude  = 0.0;
   mPeriapsisAltitude = 0.0;
}

void ChangeSemiMajorAxis::SetApoapsisAltitude(const UtLengthValue& aAlt)
{
   mApoapsisAltitude  = aAlt;
   mInputType         = InputOption::cAPOAPSIS_ALT;
   mFinalSMA          = 0.0;
   mPeriapsisAltitude = 0.0;
}

void ChangeSemiMajorAxis::SetPeriapsisAltitude(const UtLengthValue& aAlt)
{
   mPeriapsisAltitude = aAlt;
   mInputType         = InputOption::cPERIAPSIS_ALT;
   mFinalSMA          = 0.0;
   mApoapsisAltitude  = 0.0;
}

} // namespace WsfOrbitalManeuvers
