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

#include "WsfOrbitalManeuver.hpp"

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtOrbitalState.hpp"
#include "WsfDateTime.hpp"
#include "WsfEnvironment.hpp"
#include "WsfNORAD_OrbitalPropagator.hpp"
#include "WsfNORAD_PropagatorInverter.hpp"
#include "WsfOrbitalManeuvering.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"
#include "WsfTrackId.hpp"
#include "WsfTrackManager.hpp"

//! Direct the space mover associated with this maneuver to execute the maneuver.  The space mover may
//! or may not be successful in actually executing the maneuver due to delta-V limitations.  In the case
//! of finite maneuvers, this method is called multiple times at the given update interval.
//!@param aEpoch The absolute time at which to execute the maneuver.
//!@param aContext The orbital mission context in which this event is executing.
bool WsfOrbitalManeuver::ExecuteEvent(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext)
{
   UtVec3d deltaV;
   aContext.Update(aEpoch);
   ComputeDeltaV(aEpoch, mEvaluationTime, aContext.GetPropagator(), deltaV);

   UtVec3d actualDeltaV;
   bool    success = aContext.Maneuver(aEpoch, deltaV, actualDeltaV);
   if (success)
   {
      double deltaV_Mag       = deltaV.Magnitude();
      double actualDeltaV_Mag = actualDeltaV.Magnitude();
      mDeltaV += actualDeltaV_Mag;
      mRemainingDeltaV = deltaV_Mag - actualDeltaV_Mag;
   }
   return success;
}

WsfStringId WsfOrbitalManeuver::GetTypeId()
{
   return "ORBITAL_MANEUVER";
}

//! Return the minimum required delta-v needed to complete this maneuver.  If the maneuver is impulsive, this
//! value will be the actual required value; otherwise if finite, the actual value will be greater.
//! @note Use this method to determine the feasibility of maneuver execution for a given delta-V budget.
double WsfOrbitalManeuver::GetRequiredDeltaV(const UtOrbitalPropagatorBase& aPropagator) const
{
   UtVec3d deltaV;
   ComputeDeltaV(aPropagator.GetCurrentTime(), mEvaluationTime, aPropagator, deltaV);
   return deltaV.Magnitude();
}

//! Verify that the condition set in the maneuver is one of the possible valid constraints.
//! Note that when executed the constraint may not be valid at the time.
bool WsfOrbitalManeuver::VerifyCondition() const
{
   // Default implementation
   return true;
}

//! As part of maneuver verification, evaluate any preconditions associated with the maneuver.
//! Usually this involves making sure that the provided WsfOrbitalManeuver::Condition is correct.
//! @param aContext The orbital mission context in which this event is executing.
//! @return True if the preconditions are met, false otherwise.
bool WsfOrbitalManeuver::EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = VerifyCondition();
   if (!ok)
   {
      auto logger = ut::log::info() << "No valid constraint specified for this maneuver.";
      logger.AddNote() << "Type: " << GetType();
   }
   return ok;
}

//! As part of maneuver verification, evaluate any required postconditions associated with the maneuver.
//! These checks involve making sure that this is an expected post-maneuver orbit; for instance,
//! there is no intersection with the earth and it is not hyperbolic.
//! @param aContext The orbital mission context in which this event is executing.
//! @return True if the post-conditions are met, false otherwise.
bool WsfOrbitalManeuver::EvaluatePostconditions(const wsf::space::OrbitalMissionContext& aContext) const
{
   bool ok = (aContext.GetAvailableDeltaV() >= 0.0);
   if (!ok)
   {
      auto logger = ut::log::info() << "There was not enough delta-V to execute this maneuver.";
      logger.AddNote() << "Type: " << GetType();
      logger.AddNote() << "Delta-V Required: " << GetRequiredDeltaV(aContext.GetPropagator()) / 1000.0 << " km/s";
      logger.AddNote() << "Delta-V Actual: " << aContext.GetAvailableDeltaV() / 1000.0 << " km/s.";
   }
   return ok;
}

// private
//! Initialize the start and evaluation times for the maneuver.
//! In the case of finite maneuvering set the start time according to a patched conic approximation.
//!@note For finite maneuvers the start time can be before the epoch; in that case we advance the maneuver
//! one or more orbits and print a warning.
bool WsfOrbitalManeuver::InitializeTimes(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   mEvaluationTime         = aEpoch;
   double timeToConstraint = 0.0;
   bool   ok               = mCondition->GetTimeToCondition(aContext.GetPropagator(), timeToConstraint);

   if (ok)
   {
      if (timeToConstraint != 0.0)
      {
         mEvaluationTime.AdvanceTimeBy(timeToConstraint);
      }

      mStartTime = mEvaluationTime; // For impulsive maneuvers; otherwise mStartTime is adjusted below.

      // Allow individual maneuvers to initialize internal variables that depend on the start and evaluation times.
      ok = InitializeVariables(aEpoch, mEvaluationTime, aContext);

      if (ok && mIsFinite && (mCondition->GetType() != wsf::space::RelativeTimeCondition::cTYPE))
      {
         if (mDuration == 0.0)
         {
            // No duration was specified; compute the duration based on parameters provided to the maneuvering object.
            UtVec3d deltaV;
            ComputeDeltaV(mStartTime, mEvaluationTime, aContext.GetPropagator(), deltaV);
            double deltaV_Mag = deltaV.Magnitude();
            if (deltaV_Mag <= aContext.GetAvailableDeltaV())
            {
               mDuration = aContext.GetManeuverDuration(mEvaluationTime, *this, deltaV_Mag);
            }
            else
            {
               ok          = false;
               auto logger = ut::log::error()
                             << " Minimum delta-V required for maneuver is greater than total available";
               logger.AddNote() << "Type: " << GetType();
               logger.AddNote() << "Delta-V Required: " << GetRequiredDeltaV(aContext.GetPropagator()) / 1000.0 << " km/s";
               logger.AddNote() << "Delta-V Actual: " << aContext.GetAvailableDeltaV() / 1000.0 << " km/s.";
            }
         }

         // Estimate a start time to execute the maneuver to begin at a time earlier than the evaluation time by half
         // the maneuver duration.
         mStartTime.AdvanceTimeBy(-mDuration / 2.0);

         if (mStartTime < aEpoch)
         {
            auto logger = ut::log::warning() << "Orbital maneuver: Inserting delay (condition simultaneous with "
                                                "previous maneuver or execution time before simulation epoch).";
            logger.AddNote() << "Type: " << GetType();
            logger.AddNote() << "Name: " << aContext.GetName();

            GetCondition().SetOrbitNumber(GetCondition().GetOrbitNumber() + 1);
            InitializeTimes(aEpoch, aContext);
         }
      }
   }

   return ok;
}

WsfOrbitalManeuver::WsfOrbitalManeuver(const WsfScenario& aScenario)
   : WsfOrbitalEvent(aScenario)
   , mDeltaV(0.0)
   , mRemainingDeltaV(WsfOrbitalManeuvering::cINFINITE_DELTA_V)
{
   SetType(GetTypeId());
}

// protected
//! Evaluate whether a maneuver is marked as complete.  If it is complete, mIsComplete is true in Execute().
//! otherwise do nothing.
//! The default implementation checks to see whether sufficient delta-V has been expended to complete
//! the maneuver.
bool WsfOrbitalManeuver::EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator)
{
   static const double cDELTA_V_TOLERANCE = 0.001;
   return (mRemainingDeltaV <= cDELTA_V_TOLERANCE);
}

//! Get the target propagator already associated with a track, or if possible, create and attach a propagator
//! (in aux data with a key of "propagator").
const UtOrbitalPropagatorBase*
WsfOrbitalManeuver::GetTargetPropagatorPerception(WsfLocalTrack&                           aLocalTrack,
                                                  const wsf::space::OrbitalMissionContext& aContext) const
{
   UtOrbitalPropagatorBase*                 propPtr         = nullptr;
   std::shared_ptr<UtOrbitalPropagatorBase> concretePropPtr = nullptr;
   if (aLocalTrack.GetAuxDataConst().AttributeExists("propagator"))
   {
      aLocalTrack.GetAuxDataConst().GetAttribute("propagator").Get(concretePropPtr);
      propPtr = concretePropPtr.get();
   }
   else if (aLocalTrack.LocationValid() && aLocalTrack.VelocityValid())
   {
      // Create a new propagator based on valid position and velocity in the track.
      double locWCS[3];
      double velWCS[3];
      aLocalTrack.GetLocationWCS(locWCS);
      aLocalTrack.GetVelocityWCS(velWCS);
      UtCalendar updateTime = aContext.GetSimulation()->GetDateTime().GetStartDateAndTime();
      updateTime.AdvanceTimeBy(aLocalTrack.GetUpdateTime());
      UtECI_Conversion conv(updateTime, aContext.GetSimulation()->GetEnvironment().GetCentralBody());
      conv.SetLocationWCS(locWCS);
      conv.SetVelocityWCS(velWCS);

      WsfPlatform* trackTargetPtr = aContext.GetSimulation()->GetPlatformByIndex(aLocalTrack.GetTargetIndex());

      if (trackTargetPtr != nullptr)
      {
         WsfSpaceMoverBase* spmbPtr = dynamic_cast<WsfSpaceMoverBase*>(trackTargetPtr->GetMover());
         if (spmbPtr != nullptr)
         {
            concretePropPtr    = std::shared_ptr<UtOrbitalPropagatorBase>(spmbPtr->GetPropagator().Clone());
            bool foundSolution = false;
            if (spmbPtr->IsA_TypeOf("WSF_NORAD_SPACE_MOVER"))
            {
               // Use NORAD propagator inverter
               auto arg = ut::make_unique<WsfNORAD_OrbitalPropagator>(
                  dynamic_cast<const WsfNORAD_OrbitalPropagator&>(*concretePropPtr));
               WsfNORAD_PropagatorInverter inv(ut::OrbitalState(updateTime,
                                                                ut::EarthWGS84(),
                                                                ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                                ut::OrbitalState::ReferenceFrame::cECI,
                                                                ut::OrbitalState::Vector(conv.GetLocationECI(),
                                                                                         conv.GetVelocityECI())),
                                               std::move(arg));
               foundSolution = inv.SolutionFound();
               if (foundSolution)
               {
                  // The orbital state is specific to the NORAD propagator.
                  concretePropPtr->SetInitialOrbitalState(inv.GetOrbitalState());
               }
               else
               {
                  auto logger = ut::log::warning() << "Unable to compute mean-value TLE for platform.";
                  logger.AddNote() << "Platform: " << trackTargetPtr->GetName();
               }
            }
            if (!foundSolution)
            {
               const ut::OrbitalState& currentState = concretePropPtr->GetOrbitalState();
               ut::OrbitalState        initialState(updateTime,
                                             currentState.GetCentralBody(),
                                             currentState.GetCoordinateSystem(),
                                             ut::OrbitalState::ReferenceFrame::cECI,
                                             ut::OrbitalState::Vector(conv.GetLocationECI(), conv.GetVelocityECI()));
               concretePropPtr->SetInitialOrbitalState(initialState);
            }
         }
      }
      else
      {
         if (concretePropPtr == nullptr)
         {
            auto orbitalState = ut::make_unique<ut::OrbitalState>(aContext.GetPropagator().GetCentralBody(),
                                                                  ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                                  ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE);
            concretePropPtr   = std::make_shared<UtOrbitalPropagator>(std::move(orbitalState));
         }
         ut::OrbitalState state = concretePropPtr->GetInitialOrbitalState();
         state.Set(updateTime, ut::OrbitalState::Vector(conv.GetLocationTOD(), conv.GetVelocityTOD()));
         concretePropPtr->SetInitialOrbitalState(state);
      }

      auto propPtrAttrPtr = ut::make_unique<UtAttribute<std::shared_ptr<UtOrbitalPropagatorBase>>>();
      propPtrAttrPtr->SetName("propagator");
      concretePropPtr->Initialize(updateTime);
      propPtrAttrPtr->Set(concretePropPtr);
      propPtr = concretePropPtr.get();
      aLocalTrack.GetAuxData().AddAttribute(std::move(propPtrAttrPtr));
   }
   return propPtr;
}

//! Get the truth target propagator associated with a space mover on the named platform.
const UtOrbitalPropagatorBase* WsfOrbitalManeuver::GetTargetPropagatorTruth(WsfStringId    aTargetName,
                                                                            WsfSimulation* aSimulationPtr) const
{
   const UtOrbitalPropagatorBase* propPtr = nullptr;
   if (aSimulationPtr)
   {
      WsfPlatform* targetPlatformPtr = aSimulationPtr->GetPlatformByName(aTargetName);
      if (targetPlatformPtr != nullptr)
      {
         WsfSpaceMoverBase* smPtr = dynamic_cast<WsfSpaceMoverBase*>(targetPlatformPtr->GetMover());
         if (smPtr != nullptr)
         {
            propPtr = &(static_cast<const UtOrbitalPropagatorBase&>(smPtr->GetPropagator()));
         }
         else
         {
            auto logger = ut::log::warning() << "Unable to find a propagator on target platform.";
            logger.AddNote() << "Target Platform: " << aTargetName;
         }
      }
      else
      {
         auto logger = ut::log::warning() << "Unable to find target platform.";
         logger.AddNote() << "Platform Name: " << aTargetName;
      }
   }
   return propPtr;
}
