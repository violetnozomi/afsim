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

#include "WsfOrbitalManeuversTeardrop.hpp"

#include <cmath>

#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "maneuvers/WsfOrbitalManeuversDeltaV.hpp"

namespace WsfOrbitalManeuvers
{
Teardrop::Teardrop(const WsfScenario& aScenario)
   : Rendezvous(aScenario)
{
   SetType(GetTypeId());
}

Teardrop::Teardrop(const WsfScenario&                    aScenario,
                   const wsf::space::TargetPointOptions& aOptions,
                   OptimizeOption                        aOptimizeOption,
                   const UtTimeValue&                    aMaxTime,
                   const UtSpeedValue&                   aMaxDeltaV)
   : Rendezvous(aScenario, aOptions, aOptimizeOption, aMaxTime, aMaxDeltaV)
{
   SetType(GetTypeId());
}

Teardrop::Teardrop(const WsfScenario&                      aScenario,
                   const wsf::space::TargetPointOptions&   aOptions,
                   const wsf::space::OrbitalTargetingCost& aOptimizeCost,
                   const UtTimeValue&                      aMaxTime,
                   const UtSpeedValue&                     aMaxDeltaV)
   : Rendezvous(aScenario, aOptions, aOptimizeCost, aMaxTime, aMaxDeltaV)
{
   SetType(GetTypeId());
}

bool Teardrop::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   if (!VerifyCondition())
   {
      auto msg = ut::log::error() << "Condition type not allowed.";
      msg.AddNote() << "Type: " << GetType();
      msg.AddNote() << "Condition: " << GetConditionString();
      return false;
   }

   if (mTimeToPOCA < mPeriod / 2.0)
   {
      auto msg = ut::log::error() << "Time to POCA should be greater than half the teardrop period.";
      msg.AddNote() << "Type: " << GetType();
      msg.AddNote() << "Period: " << mPeriod;
      msg.AddNote() << "Time to POCA: " << mTimeToPOCA;
      return false;
   }

   auto targetPropPtr = GetTargetPropagator(aContext);

   bool retval{true};
   if (targetPropPtr)
   {
      if (targetPropPtr->GetInitialOrbitalState().OrbitIsCircular())
      {
         double n = targetPropPtr->GetInitialOrbitalState().GetOrbitalElements().GetMeanMotion();

         double gammaT = n * mPeriod / 2.0;
         double denom  = (4.0 * std::sin(gammaT) - 3.0 * gammaT);
         if (denom > 0.0)
         {
            double xd  = 4.0 * mRadialOffsetAtPOCA * std::sin(gammaT) / denom;
            double x0  = mRadialOffsetAtPOCA;
            double vy0 = n * (xd - 4.0 * x0) / 2.0;

            SetOffset(UtVec3d{x0, 0.0, 0.0}, wsf::space::OrbitalReferenceFrame::cRIC);
            SetVelocityOffset(UtVec3d{0.0, vy0, 0.0}, wsf::space::OrbitalReferenceFrame::cRIC);
            SetTargetOffsetTime(mTimeToPOCA);

            // Now add repetitions as needed
            double dvR     = -6.0 * gammaT * mRadialOffsetAtPOCA * n * std::sin(gammaT) / denom;
            double repTime = mTimeToPOCA + mPeriod / 2.0; // First extra repetition has a longer wait
            for (unsigned int rep = 1; rep < mReps; ++rep)
            {
               auto mvrPtr =
                  ut::make_unique<DeltaV>(GetScenario(), UtVec3d{dvR, 0.0, 0.0}, wsf::space::OrbitalReferenceFrame::cRIC);
               auto conditionPtr = ut::make_unique<wsf::space::RelativeTimeCondition>();
               conditionPtr->SetOffsetTime(repTime);
               mvrPtr->SetCondition(std::move(conditionPtr));
               AddMissionEvent(std::move(mvrPtr));
               repTime = mPeriod; // Any subsequent repetition needs to wait only one period.
            }
         }
         else
         {
            retval   = false;
            auto msg = ut::log::error() << "No teardrop exists for the given parameters.";
            msg.AddNote() << "Type: " << GetType();
            msg.AddNote() << "Teardrop period: " << mPeriod;

            // This constant (1.2757) is approximately the value of gammaT that solves
            // denom = 0.
            double      limit = 2.0 * 1.2757 / n;
            UtTimeValue limitWithUnits{limit};
            limitWithUnits.ConvertToUnit(mPeriod.GetUnit());
            msg.AddNote() << "Teardrop period must be less than: " << limitWithUnits;
         }
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

bool Teardrop::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval = Rendezvous::ValidateParameterRanges(aExecutingPlatformName);

   if (mRadialOffsetAtPOCA == 0.0)
   {
      auto msg = ut::log::error() << "Radial offset at POCA must not be zero.";
      msg.AddNote() << "Type: " << GetType();
      retval = false;
   }
   if (mPeriod <= 0.0)
   {
      auto msg = ut::log::error() << "Period must be positive.";
      msg.AddNote() << "Type: " << GetType();
      retval = false;
   }
   if (mTimeToPOCA <= 0.0)
   {
      auto msg = ut::log::error() << "Time to POCA must be positive.";
      msg.AddNote() << "Type: " << GetType();
      retval = false;
   }
   if (mTimeToPOCA < mPeriod / 2.0)
   {
      auto msg = ut::log::error() << "Time to POCA should be greater than half of the period.";
      msg.AddNote() << "Type: " << GetType();
      retval = false;
   }
   if (mReps < 1)
   {
      auto msg = ut::log::error() << "Repetitions should be greater than 0.";
      msg.AddNote() << "Type: " << GetType();
      retval = false;
   }

   return retval;
}

bool Teardrop::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command = aInput.GetCommand();
   if (command == "target" || command == "platform" || command == "offset")
   {
      // Here we intercept Rendezvous commands that we disallow in this subclass.
      throw UtInput::UnknownCommand{aInput};
   }
   else if (command == "target_platform")
   {
      std::string platName;
      aInput.ReadValue(platName);
      SetPlatformName(platName);
   }
   else if (command == "radial_offset_at_poca")
   {
      aInput.ReadUnitValue(mRadialOffsetAtPOCA);
      if (mRadialOffsetAtPOCA == 0.0)
      {
         throw UtInput::BadValue(aInput, "radial_offset_at_poca must be non-zero.");
      }
   }
   else if (command == "period")
   {
      aInput.ReadUnitValue(mPeriod);
      aInput.ValueGreater<double>(mPeriod, 0.0);
   }
   else if (command == "time_to_poca")
   {
      aInput.ReadUnitValue(mTimeToPOCA);
      aInput.ValueGreater<double>(mTimeToPOCA, 0.0);
   }
   else if (command == "repetitions")
   {
      int val;
      aInput.ReadValue(val);
      aInput.ValueGreater(val, 0);
      mReps = ut::safe_cast<unsigned int>(val);
   }
   else
   {
      myCommand = Rendezvous::ProcessInput(aInput);
   }
   return myCommand;
}

void Teardrop::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitTeardrop(this);
}

} // namespace WsfOrbitalManeuvers
