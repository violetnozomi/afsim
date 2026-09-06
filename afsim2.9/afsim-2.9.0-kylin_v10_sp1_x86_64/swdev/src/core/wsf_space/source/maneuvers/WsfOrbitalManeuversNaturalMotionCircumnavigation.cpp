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

#include "WsfOrbitalManeuversNaturalMotionCircumnavigation.hpp"

#include <cmath>

#include "UtLog.hpp"
#include "WsfOrbitalEventVisitor.hpp"

namespace WsfOrbitalManeuvers
{

NaturalMotionCircumnavigation::NaturalMotionCircumnavigation(const WsfScenario& aScenario)
   : Rendezvous(aScenario)
{
   SetType(GetTypeId());
}

NaturalMotionCircumnavigation::NaturalMotionCircumnavigation(const WsfScenario&                    aScenario,
                                                             const wsf::space::TargetPointOptions& aOptions,
                                                             OptimizeOption                        aOptimizeOption,
                                                             const UtTimeValue&                    aMaxTime,
                                                             const UtSpeedValue&                   aMaxDeltaV)
   : Rendezvous(aScenario, aOptions, aOptimizeOption, aMaxTime, aMaxDeltaV)
{
   SetType(GetTypeId());
}

NaturalMotionCircumnavigation::NaturalMotionCircumnavigation(const WsfScenario&                      aScenario,
                                                             const wsf::space::TargetPointOptions&   aOptions,
                                                             const wsf::space::OrbitalTargetingCost& aOptimizeCost,
                                                             const UtTimeValue&                      aMaxTime,
                                                             const UtSpeedValue&                     aMaxDeltaV)
   : Rendezvous(aScenario, aOptions, aOptimizeCost, aMaxTime, aMaxDeltaV)
{
   SetType(GetTypeId());
}

bool NaturalMotionCircumnavigation::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   if (mOrbitSize <= 0.0)
   {
      auto msg = ut::log::error() << "Must specify a positive orbit size.";
      msg.AddNote() << "Type: " << GetType();
      return false;
   }

   auto targetPropPtr = GetTargetPropagator(aContext);

   bool retval{true};
   if (targetPropPtr)
   {
      if (targetPropPtr->GetInitialOrbitalState().OrbitIsCircular())
      {
         // Get the mean motion
         double n = targetPropPtr->GetInitialOrbitalState().GetOrbitalElements().GetMeanMotion();

         // Compute the initial offsets from the input arguments
         // NOTE: We dropped a minus sign on mOrbitPhase so the phase advances in the same
         // direction as the relative orbit.
         double x0 = mOrbitSize / 2.0 * cos(mOrbitPhase);
         double y0 = -mOrbitSize * sin(mOrbitPhase);
         double z0 = mOutOfPlaneAmplitude * cos(-mOutOfPlanePhase);

         // Compute the initial velocity offsets
         double vx0 = n * y0 / 2.0;
         double vy0 = -2.0 * n * x0;
         double vz0 = -mOutOfPlaneAmplitude * n * sin(-mOutOfPlanePhase);

         // Set these values on the target
         SetOffset(UtVec3d{x0, y0, z0}, wsf::space::OrbitalReferenceFrame::cRIC);
         SetVelocityOffset(UtVec3d{vx0, vy0, vz0}, wsf::space::OrbitalReferenceFrame::cRIC);
      }
      else
      {
         retval   = false;
         auto msg = ut::log::error() << "Target must have circular orbit.";
         msg.AddNote() << "Type: " << GetType();
      }
   }

   return retval && Rendezvous::Initialize(aEpoch, aContext);
}

bool NaturalMotionCircumnavigation::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval = Rendezvous::ValidateParameterRanges(aExecutingPlatformName);

   if (mOrbitSize <= 0.0)
   {
      auto msg = ut::log::error() << "orbit_size must be positive.";
      msg.AddNote() << "Type: " << GetType();
      retval = false;
   }
   if (mOutOfPlaneAmplitude < 0.0)
   {
      auto msg = ut::log::error() << "out_of_plane_amplitude must be non-negative.";
      msg.AddNote() << "Type: " << GetType();
      retval = false;
   }

   return retval;
}

bool NaturalMotionCircumnavigation::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command = aInput.GetCommand();
   if (command == "target" || command == "platform" || command == "offset")
   {
      throw UtInput::UnknownCommand{aInput};
   }
   else if (command == "target_platform")
   {
      std::string platName;
      aInput.ReadValue(platName);
      SetPlatformName(platName);
   }
   else if (command == "orbit_size")
   {
      aInput.ReadUnitValue(mOrbitSize);
      aInput.ValueGreater(mOrbitSize.GetAsUnit(), 0.0);
   }
   else if (command == "orbit_phase")
   {
      aInput.ReadUnitValue(mOrbitPhase);
   }
   else if (command == "out_of_plane_amplitude")
   {
      aInput.ReadUnitValue(mOutOfPlaneAmplitude);
      aInput.ValueGreaterOrEqual(mOutOfPlaneAmplitude.GetAsUnit(), 0.0);
   }
   else if (command == "out_of_plane_phase")
   {
      aInput.ReadUnitValue(mOutOfPlanePhase);
   }
   else
   {
      myCommand = Rendezvous::ProcessInput(aInput);
   }
   return myCommand;
}

void NaturalMotionCircumnavigation::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitNaturalMotionCircumnavigation(this);
}

} // namespace WsfOrbitalManeuvers
