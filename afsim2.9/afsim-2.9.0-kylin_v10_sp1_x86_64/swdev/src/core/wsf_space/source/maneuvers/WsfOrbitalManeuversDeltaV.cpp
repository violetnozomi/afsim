// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalManeuversDeltaV.hpp"

#include <iostream>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtLambertProblem.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtRIC_Frame.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace WsfOrbitalManeuvers
{
DeltaV::DeltaV(const WsfScenario&                aScenario,
               const double                      dv_x,
               const double                      dv_y,
               const double                      dv_z,
               wsf::space::OrbitalReferenceFrame aFrame)
   : DeltaV::DeltaV(aScenario, UtVec3d{dv_x, dv_y, dv_z}, aFrame)
{
}

DeltaV::DeltaV(const WsfScenario& aScenario, const UtVec3d& aDeltaV, wsf::space::OrbitalReferenceFrame aFrame)
   : WsfOrbitalManeuver(aScenario)
   , mConfiguredDeltaV(aDeltaV)
   , mFrame{aFrame}
{
   SetType(GetTypeId());
}

WsfStringId DeltaV::GetTypeId()
{
   return "DELTA_V";
}

bool DeltaV::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "dv_x") // DEPRECATED
   {
      double dv_x;
      aInput.ReadValueOfType(dv_x, UtInput::cSPEED);

      mConfiguredDeltaV.Set(0, dv_x);
      mFrame = wsf::space::OrbitalReferenceFrame::cINERTIAL;

      auto msg = ut::log::warning() << "The 'dv_x' input has been deprecated.";
      msg.AddNote() << "Please see 'delta_v'.";
   }
   else if (command == "dv_y") // DEPRECATED
   {
      double dv_y;
      aInput.ReadValueOfType(dv_y, UtInput::cSPEED);

      mConfiguredDeltaV.Set(1, dv_y);
      mFrame = wsf::space::OrbitalReferenceFrame::cINERTIAL;

      auto msg = ut::log::warning() << "The 'dv_y' input has been deprecated.";
      msg.AddNote() << "Please see 'delta_v'.";
   }
   else if (command == "dv_z") // DEPRECATED
   {
      double dv_z;
      aInput.ReadValueOfType(dv_z, UtInput::cSPEED);

      mConfiguredDeltaV.Set(2, dv_z);
      mFrame = wsf::space::OrbitalReferenceFrame::cINERTIAL;

      auto msg = ut::log::warning() << "The 'dv_z' input has been deprecated.";
      msg.AddNote() << "Please see 'delta_v'.";
   }
   else if (command == "delta_v")
   {
      std::string frameId;
      aInput.ReadCommand(frameId);
      mFrame = wsf::space::OrbitalReferenceFrameFromIdentifier(frameId);
      if (mFrame == wsf::space::OrbitalReferenceFrame::cUNKNOWN)
      {
         throw UtInput::BadValue(aInput, "Unknown frame type for delta-v maneuver.");
      }
      else if (!DeltaV::IsFrameSupported(mFrame))
      {
         throw UtInput::BadValue(aInput, "Unsupported orbital reference frame.");
      }

      for (int i = 0; i < 3; ++i)
      {
         double component;
         aInput.ReadValueOfType(component, UtInput::cSPEED);
         mConfiguredDeltaV.Set(i, component);
      }
   }
   else
   {
      myCommand = WsfOrbitalManeuver::ProcessInput(aInput);
   }
   return myCommand;
}

bool DeltaV::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePreconditions(aContext) && IsFrameSupported(mFrame);
   if (ok)
   {
      auto propPtr = ut::clone(aContext.GetPropagator());
      propPtr->Initialize(aContext.GetPropagator().GetOrbitalState());
      propPtr->Update(mEvaluationTime);
      ut::OrbitalState::Vector osvInertial = propPtr->GetOrbitalState().GetOrbitalStateVectorInertial();
      UtVec3d                  locECI      = osvInertial.GetLocation();
      UtVec3d                  velECI      = osvInertial.GetVelocity();
      if (UtLambertProblem::Hyperbolic(propPtr->GetCentralBody().GetGravitationalParameter(),
                                       locECI,
                                       velECI + mConfiguredDeltaV) &&
          !propPtr->HyperbolicPropagationAllowed())
      {
         auto logger = ut::log::error() << "Solution is hyperbolic (not supported).";
         logger.AddNote() << "Type: " << GetType();
         ok = false;
      }
   }

   return ok;
}

bool DeltaV::EvaluatePostconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = WsfOrbitalEvent::EvaluatePostconditions(aContext);
   if (ok)
   {
      if (aContext.GetPropagator().GetOrbitalState().OrbitIntersectsCentralBody())
      {
         auto logger = ut::log::error() << "Final orbit periapsis intersects central body.";
         logger.AddNote() << "Type: " << GetType();
      }
   }

   return ok;
}

void DeltaV::ComputeDeltaV(const UtCalendar&              aCurrentTime,
                           const UtCalendar&              aEvaluationTime,
                           const UtOrbitalPropagatorBase& aPropagator,
                           UtVec3d&                       aDeltaV) const
{
   switch (mFrame)
   {
   case wsf::space::OrbitalReferenceFrame::cINERTIAL:
      aDeltaV.Set(mConfiguredDeltaV);
      break;
   case wsf::space::OrbitalReferenceFrame::cRIC:
   {
      auto    osvInertial = aPropagator.GetOrbitalState().GetOrbitalStateVectorInertial();
      UtVec3d vInertial   = ut::RIC_Frame::VelocityFromRIC(osvInertial.GetLocation(),
                                                         osvInertial.GetVelocity(),
                                                         UtVec3d{0.0, 0.0, 0.0},
                                                         mConfiguredDeltaV);
      aDeltaV.Set(vInertial - osvInertial.GetVelocity());
   }
   break;
   case wsf::space::OrbitalReferenceFrame::cUNKNOWN: // Intentional fall-through
   default:                                          //
      throw std::runtime_error{"Delta-v maneuver with unknown frame."};
      break;
   }
}

UtSpeedValue DeltaV::GetDeltaV_Component(int aIndex) const
{
   return UtSpeedValue{mConfiguredDeltaV.Get(aIndex), UtUnitSpeed::cMETERS_PER_SECOND};
}

void DeltaV::SetDeltaV_Component(int aIndex, const UtSpeedValue& aDeltaV)
{
   mConfiguredDeltaV.Set(aIndex, aDeltaV);
}

void DeltaV::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitDeltaV(this);
}

bool DeltaV::IsFrameSupported(wsf::space::OrbitalReferenceFrame aFrame)
{
   return aFrame == wsf::space::OrbitalReferenceFrame::cINERTIAL || aFrame == wsf::space::OrbitalReferenceFrame::cRIC;
}

} // namespace WsfOrbitalManeuvers
