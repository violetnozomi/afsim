// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalManeuversTarget.hpp"

#include <iostream>
#include <limits>
#include <stdexcept>

#include "UtCalendar.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfOrbitalTargeting.hpp"
#include "WsfOrbitalTargetingCostTypes.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace WsfOrbitalManeuvers
{
Target::Target(const WsfScenario& aScenario)
   : Target::Target(aScenario, WsfTrackId(), cOPTIMIZE_NONE, 0.0, 0.0)
{
   mCanVerify = false;
}

Target::Target(const WsfScenario&  aScenario,
               const WsfTrackId&   aLocalTrackId,
               OptimizeOption      aOptimizeOption,
               const UtTimeValue&  aMaxTime,
               const UtSpeedValue& aMaxDelta_V)
   : TargetingCapableManeuver{aScenario, aLocalTrackId}
   , mComputeForRendezvous(false)
   , mOptimizeOption(aOptimizeOption)
   , mCostPtr(nullptr)
   , mMaxTime(0.0)
   , mDeltaTime(0.0)
   , mMaxDeltaV(aMaxDelta_V)
   , mInterceptTime()
   , mFinalDeltaV(0.0)
{
   // The aTime parameter can either correspond with
   // the non-optimized delta_time or the optimized maximum_time.
   if (mOptimizeOption == cOPTIMIZE_NONE)
   {
      mDeltaTime = aMaxTime;
   }
   else
   {
      mMaxTime = aMaxTime;
   }
   mCanVerify = false;
   SetType(GetTypeId());
}

Target::Target(const WsfScenario&                    aScenario,
               const wsf::space::TargetPointOptions& aOptions,
               OptimizeOption                        aOptimizeOption,
               const UtTimeValue&                    aMaxTime,
               const UtSpeedValue&                   aMaxDelta_V)
   : TargetingCapableManeuver{aScenario, aOptions}
   , mComputeForRendezvous(false)
   , mOptimizeOption(aOptimizeOption)
   , mCostPtr(nullptr)
   , mMaxTime(0.0)
   , mDeltaTime(0.0)
   , mMaxDeltaV(aMaxDelta_V)
   , mInterceptTime()
   , mFinalDeltaV(0.0)
{
   // The aTime parameter can either correspond with
   // the non-optimized delta_time or the optimized maximum_time.
   if (mOptimizeOption == cOPTIMIZE_COST)
   {
      throw std::runtime_error{"Cost optimization selected in Target constructor unable to accept cost optimization"};
   }
   else if (mOptimizeOption == cOPTIMIZE_NONE)
   {
      mDeltaTime = aMaxTime;
   }
   else
   {
      mMaxTime = aMaxTime;
   }
   mCanVerify = false;
   SetType(GetTypeId());
}

Target::Target(const WsfScenario&                      aScenario,
               const wsf::space::TargetPointOptions&   aOptions,
               const wsf::space::OrbitalTargetingCost& aOptimizeCost,
               const UtTimeValue&                      aMaxTime,
               const UtSpeedValue&                     aMaxDelta_V)
   : TargetingCapableManeuver{aScenario, aOptions}
   , mComputeForRendezvous(false)
   , mOptimizeOption(OptimizeOption::cOPTIMIZE_COST)
   , mCostPtr(ut::clone(aOptimizeCost))
   , mMaxTime(aMaxTime)
   , mDeltaTime(0.0)
   , mMaxDeltaV(aMaxDelta_V)
   , mInterceptTime()
   , mFinalDeltaV(0.0)
{
   mCanVerify = false;
   SetType(GetTypeId());
}

WsfOrbitalManeuvers::Target::Target(const Target& aSrc)
   : TargetingCapableManeuver(aSrc)
   , mComputeForRendezvous(aSrc.mComputeForRendezvous)
   , mOptimizeOption(aSrc.mOptimizeOption)
   , mCostPtr(aSrc.mCostPtr)
   , mMaxTime(aSrc.mMaxTime)
   , mDeltaTime(aSrc.mDeltaTime)
   , mMaxDeltaV(aSrc.mMaxDeltaV)
   , mInterceptTime(aSrc.mInterceptTime)
   , mFinalDeltaV(aSrc.mFinalDeltaV)
   , mTolerance(aSrc.mTolerance)
{
   SetType(GetTypeId());
}

WsfStringId Target::GetTypeId()
{
   return "TARGET";
}

bool Target::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if ((command == "maximum_time") || (command == "maximum_delta_time"))
   {
      aInput.ReadUnitValue(mMaxTime);
      aInput.ValueGreater<double>(mMaxTime, 0.0);
   }
   else if (command == "delta_time")
   {
      aInput.ReadUnitValue(mDeltaTime);
      aInput.ValueGreater<double>(mDeltaTime, 0.0);
      mOptimizeOption = cOPTIMIZE_NONE;
   }
   else if (command == "maximum_delta_v")
   {
      aInput.ReadUnitValue(mMaxDeltaV);
      aInput.ValueGreater<double>(mMaxDeltaV, 0.0);
   }
   else if (command == "optimize_time")
   {
      mOptimizeOption = cOPTIMIZE_TIME;
   }
   else if (command == "optimize_delta_v")
   {
      mOptimizeOption = cOPTIMIZE_DELTA_V;
   }
   else if (command == "optimize_cost")
   {
      const auto& costTypes = wsf::space::OrbitalTargetingCostTypes::Get(mScenario);
      std::unique_ptr<wsf::space::OrbitalTargetingCost> costPtr{};
      if (costTypes.LoadInstance(aInput, costPtr))
      {
         mCostPtr        = std::move(costPtr);
         mOptimizeOption = cOPTIMIZE_COST;
      }
      else
      {
         myCommand = false;
      }
   }
   else if (command == "tolerance")
   {
      aInput.ReadValue(mTolerance);
      aInput.ValueGreater(mTolerance, 0.0);
   }
   else
   {
      if (command == "finite")
      {
         auto logger = ut::log::warning()
                       << "Finite targeting maneuvers will have less accuracy in the resulting solution.";
         logger.AddNote() << "Type: " << GetType();
      }
      myCommand = TargetingCapableManeuver::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitalManeuvers::Target::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   if ((mMaxTime == 0.0) && (mMaxDeltaV == 0.0) && (mDeltaTime == 0.0))
   {
      auto logger =
         ut::log::error() << "Must define a delta time, maximum delta time, or maximum delta-v to intercept.";
      logger.AddNote() << "Type: " << GetType();
      return false;
   }

   if (mMaxDeltaV > aContext.GetAvailableDeltaV())
   {
      auto logger =
         ut::log::error() << "Delta-V specified for intercept maneuver greater than the platform's total available.";
      logger.AddNote() << "Type: " << GetType();
      logger.AddNote() << "Delta-V: " << mMaxDeltaV.GetAsUnit(UtUnitSpeed::cMETERS_PER_SECOND) / 1000.0 << " km/s";
      logger.AddNote() << "Total Available: " << aContext.GetAvailableDeltaV() / 1000.0 << " km/s";
      return false;
   }

   if (mOptimizeOption == cOPTIMIZE_COST)
   {
      if (!mCostPtr)
      {
         auto logger = ut::log::error() << "Cost function undefined.";
         logger.AddNote() << "Type: " << GetType();
         logger.AddNote() << "Maneuver configured for cost-function optimization, but no cost function is supplied.";
         return false;
      }
      else if (!mCostPtr->IsValid())
      {
         auto logger = ut::log::error() << "Cost function has invalid configuration.";
         logger.AddNote() << "Type: " << GetType();
         logger.AddNote() << mCostPtr->GetConfigurationRequirements();
         return false;
      }
   }

   bool ok = TargetingCapableManeuver::Initialize(aEpoch, aContext);
   if (ok)
   {
      if (aContext.GetSimulation())
      {
         if (GetTargetPoint())
         {
            if (mMaxTime == 0.0)
            {
               mMaxTime.Set(GetTargetPoint()->GetCharacteristicTime(), UtUnitTime::cSECONDS);
            }
            if (mMaxDeltaV == 0.0)
            {
               mMaxDeltaV.Set(aContext.GetAvailableDeltaV(), UtUnitSpeed::cMETERS_PER_SECOND);
            }

            if (mOptimizeOption == cOPTIMIZE_NONE)
            {
               ok = ok && OptimizeNone(aContext.GetPropagator());
            }
            else
            {
               ok = ok && OptimizeSolution(aContext.GetPropagator());
            }
         }
         else
         {
            ok = false;
         }
      }
   }

   return ok;
}

bool Target::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval{true};

   if (mMaxTime == 0.0 && mDeltaTime == 0.0 && mMaxDeltaV == 0.0)
   {
      auto logger =
         ut::log::error() << "At least one of delta_time, maximum_delta_time or maximum_delta_v must be non-zero.";
      logger.AddNote() << "Type: " << GetType();
      retval = false;
   }
   else
   {
      if (mMaxTime < 0.0)
      {
         auto logger = ut::log::error() << "maximum_delta_time must be non-negative.";
         logger.AddNote() << "Type: " << GetType();
         retval = false;
      }
      if (mDeltaTime < 0.0)
      {
         auto logger = ut::log::error() << "delta_time must be non-negative.";
         logger.AddNote() << "Type: " << GetType();
         retval = false;
      }
      if (mMaxDeltaV < 0.0)
      {
         auto logger = ut::log::error() << "maximum_delta_v must be non-negative.";
         logger.AddNote() << "Type: " << GetType();
         retval = false;
      }
   }
   if (mOptimizeOption == OptimizeOption::cOPTIMIZE_COST)
   {
      if (!mCostPtr)
      {
         auto logger = ut::log::error() << "Cost function undefined.";
         logger.AddNote() << "Type: " << GetType();
         retval = false;
      }
      else if (!mCostPtr->IsValid())
      {
         auto logger = ut::log::error() << "Cost function has invalid configuration.";
         logger.AddNote() << "Type: " << GetType();
         logger.AddNote() << mCostPtr->GetConfigurationRequirements();
         retval = false;
      }
   }
   retval = TargetingCapableManeuver::ValidateParameterRanges(aExecutingPlatformName) && retval;

   return retval;
}

// virtual
void Target::ComputeDeltaV(const UtCalendar&              aCurrentTime,
                           const UtCalendar&              aEvaluationTime,
                           const UtOrbitalPropagatorBase& aPropagator,
                           UtVec3d&                       aDeltaV) const
{
   auto result = FixedDtSolve(aEvaluationTime, aPropagator, aDeltaV);

   if (!result.Assess(aPropagator.HyperbolicPropagationAllowed(), false))
   {
      // The maneuver cannot be executed.
      auto msg = ut::log::warning() << "Unable to find targeting solution.";
      msg.AddNote() << "Max. search time: " << mMaxTime;
      msg.AddNote() << "Max. search delta-v: " << mMaxDeltaV;
      if (mComputeForRendezvous)
      {
         msg.AddNote() << "Computing for rendezvous.";
      }
      msg.AddNote() << "Tolerance: " << mTolerance;
      aDeltaV.Set(std::numeric_limits<double>::max());
   }
}

bool WsfOrbitalManeuvers::Target::OptimizeSolution(const UtOrbitalPropagatorBase& aPropagator)
{
   auto myProp = ut::clone(aPropagator);
   myProp->Initialize(aPropagator.GetOrbitalState());
   myProp->Update(mStartTime);
   // Note that GetTargetPoint will return a non-null pointer here, because this method
   // is only called after the base class is initialized successfully, which guarantees
   // that the target point object is valid.
   WsfOrbitalTargeting tgt(mStartTime, *myProp, *GetTargetPoint());
   tgt.SetTolerance(mTolerance);

   // Find solution
   double                   dT;
   double                   dV;
   UtLambertProblem::Result result{};
   if (mOptimizeOption == cOPTIMIZE_TIME || (mOptimizeOption == cOPTIMIZE_COST && mCostPtr->IsLeastTime()))
   {
      static constexpr double cTIME_TOLERANCE = 1.0E-2; // Optimize to within 1/100 of a second.
      result = tgt.MinimizeDeltaT(mMaxTime, mMaxDeltaV, mComputeForRendezvous, cTIME_TOLERANCE, dT, dV);
   }
   else if (mOptimizeOption == cOPTIMIZE_DELTA_V)
   {
      static constexpr double cTIME_TOLERANCE = 1.0E-4;
      result = tgt.MinimizeDeltaV(mMaxTime, mMaxDeltaV, mComputeForRendezvous, cTIME_TOLERANCE, dT, dV);
   }
   else if (mOptimizeOption == cOPTIMIZE_COST)
   {
      static constexpr double cTIME_TOLERANCE = 1.0E-4;
      result = tgt.MinimizeCost(*mCostPtr, mMaxTime, mMaxDeltaV, mComputeForRendezvous, cTIME_TOLERANCE, dT, dV);
   }
   else
   {
      return false;
   }

   // some final things
   mInterceptTime = mStartTime;
   mInterceptTime.AdvanceTimeBy(dT);
   if (result.IsSolution() && dV <= mMaxDeltaV)
   {
      if (mDebug)
      {
         auto logger = ut::log::debug() << "Intercept time";
         logger.AddNote() << "T = " << mInterceptTime.GetTimeSince(mEvaluationTime);
         logger.AddNote() << "Delta-V: " << dV;
      }
   }
   else if (result.IsHyperbolic() && !aPropagator.HyperbolicPropagationAllowed())
   {
      auto logger = ut::log::error() << "Solution is hyperbolic (not supported).";
      logger.AddNote() << "Type: " << GetType();
   }
   else if (result.HitsCentralBody())
   {
      auto logger = ut::log::error() << "Solution intersects Earth.";
      logger.AddNote() << "Type: " << GetType();
   }
   else if (dV > mMaxDeltaV)
   {
      auto logger = ut::log::error() << "Solution not within delta-v limit";
      logger.AddNote() << "Type: " << GetType();
      logger.AddNote() << "Delta-V: " << dV;
      logger.AddNote() << "Max Delta-V: " << mMaxDeltaV;
   }

   return result.IsSolution();
}

//! Given a fixed target and time, find the shortest time that will provide that delta-V or less.
//! The maneuver must be executed in one orbit or less.
bool WsfOrbitalManeuvers::Target::OptimizeNone(const UtOrbitalPropagatorBase& aPropagator)
{
   mInterceptTime = mStartTime;
   mInterceptTime.AdvanceTimeBy(mDeltaTime);
   UtVec3d deltaV;
   auto    retval = FixedDtSolve(mStartTime, aPropagator, deltaV);

   if (retval.IsHyperbolic() && !aPropagator.HyperbolicPropagationAllowed())
   {
      auto logger = ut::log::error() << "Solution is hyperbolic (not supported).";
      logger.AddNote() << "Type: " << GetType();
   }
   else if (retval.HitsCentralBody())
   {
      auto logger = ut::log::error() << "Solution intersects Earth.";
      logger.AddNote() << "Type: " << GetType();
   }
   else
   {
      if (mDebug)
      {
         auto logger = ut::log::info() << "Intercept time";
         logger.AddNote() << "T = " << mInterceptTime.GetTimeSince(mEvaluationTime);
         logger.AddNote() << "Delta-V: " << deltaV.Magnitude();
      }
   }

   return retval.Assess(aPropagator.HyperbolicPropagationAllowed(), false);
}

UtLambertProblem::Result WsfOrbitalManeuvers::Target::FixedDtSolve(const UtCalendar&              aEvaluationTime,
                                                                   const UtOrbitalPropagatorBase& aPropagator,
                                                                   UtVec3d&                       aDeltaV) const
{
   auto myProp = ut::clone(aPropagator);
   myProp->Initialize(aPropagator.GetOrbitalState());
   myProp->Update(aEvaluationTime);
   UpdateTargetPoint(mInterceptTime);
   WsfOrbitalTargeting tgt{aEvaluationTime, *myProp, *GetTargetPoint()};
   tgt.SetTolerance(mTolerance);

   double dt = mInterceptTime.GetTimeSince(mEvaluationTime);

   return tgt.Solve(dt, aDeltaV, mFinalDeltaV);
}

//! Set the delta time to be used for this targeting operation.
//! This will set the time used for the targeting operation, and set the
//! optimization style to none.
void WsfOrbitalManeuvers::Target::SetDeltaTime(const UtTimeValue& aDeltaTime)
{
   mDeltaTime      = aDeltaTime;
   mOptimizeOption = OptimizeOption::cOPTIMIZE_NONE;
}

void WsfOrbitalManeuvers::Target::SetOptimizationCost(std::unique_ptr<wsf::space::OrbitalTargetingCost> aCostPtr)
{
   if (aCostPtr)
   {
      mCostPtr        = std::move(aCostPtr);
      mOptimizeOption = OptimizeOption::cOPTIMIZE_COST;
   }
}

void WsfOrbitalManeuvers::Target::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitTarget(this);
}

} // namespace WsfOrbitalManeuvers
