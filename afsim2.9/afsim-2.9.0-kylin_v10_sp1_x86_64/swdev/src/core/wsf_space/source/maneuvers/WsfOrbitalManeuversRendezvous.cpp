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

#include "WsfOrbitalManeuversRendezvous.hpp"

#include <iostream>
#include <stdexcept>

#include "UtCalendar.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfAttitudeController.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfOrbitalTargetPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace WsfOrbitalManeuvers
{
Rendezvous::Rendezvous(const WsfScenario& aScenario)
   : Rendezvous(aScenario, WsfTrackId(), cOPTIMIZE_DELTA_V, 0.0, 0.0)
{
}

Rendezvous::Rendezvous(const WsfScenario&  aScenario,
                       const WsfTrackId&   aLocalTrackId,
                       OptimizeOption      aOptimizeOption,
                       const UtTimeValue&  aMaxTime,
                       const UtSpeedValue& aMaxDeltaV)
   : WsfOrbitalMissionSequence(aScenario)
{
   AddMissionEvent(ut::make_unique<Target>(aScenario, aLocalTrackId, aOptimizeOption, aMaxTime, aMaxDeltaV));
   AddMissionEvent(ut::make_unique<MatchVelocity>(aScenario));
   SetupManevuerPointers();
   mCanVerify = false;
   GetTargetManeuver().SetComputeForRendezvous(true);
   SetType(GetTypeId());
}

Rendezvous::Rendezvous(const WsfScenario&                    aScenario,
                       const wsf::space::TargetPointOptions& aOptions,
                       OptimizeOption                        aOptimizeOption,
                       const UtTimeValue&                    aMaxTime,
                       const UtSpeedValue&                   aMaxDeltaV)
   : WsfOrbitalMissionSequence(aScenario)
{
   AddMissionEvent(ut::make_unique<Target>(aScenario, aOptions, aOptimizeOption, aMaxTime, aMaxDeltaV));
   AddMissionEvent(ut::make_unique<MatchVelocity>(aScenario));
   SetupManevuerPointers();
   mCanVerify = false;
   GetTargetManeuver().SetComputeForRendezvous(true);
   SetType(GetTypeId());
}

Rendezvous::Rendezvous(const WsfScenario&                      aScenario,
                       const wsf::space::TargetPointOptions&   aOptions,
                       const wsf::space::OrbitalTargetingCost& aOptimizeCost,
                       const UtTimeValue&                      aMaxTime,
                       const UtSpeedValue&                     aMaxDeltaV)
   : WsfOrbitalMissionSequence(aScenario)
{
   AddMissionEvent(ut::make_unique<Target>(aScenario, aOptions, aOptimizeCost, aMaxTime, aMaxDeltaV));
   AddMissionEvent(ut::make_unique<MatchVelocity>(aScenario));
   SetupManevuerPointers();
   mCanVerify = false;
   GetTargetManeuver().SetComputeForRendezvous(true);
   SetType(GetTypeId());
}

Rendezvous::Rendezvous(const Rendezvous& aSrc)
   : WsfOrbitalMissionSequence(aSrc)
{
   SetupManevuerPointers();
}

WsfStringId Rendezvous::GetTypeId()
{
   return "RENDEZVOUS";
}

bool Rendezvous::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   GetMatchVelocityManeuver().SetCondition(ut::make_unique<wsf::space::RelativeTimeCondition>());
   GetMatchVelocityManeuver().SetTargetPointOptions(GetTargetManeuver().GetTargetPointOptions());
   return WsfOrbitalMissionSequence::Initialize(aEpoch, aContext);
}

bool Rendezvous::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval = GetTargetManeuver().ValidateParameterRanges(aExecutingPlatformName);
   return WsfOrbitalEvent::ValidateParameterRanges(aExecutingPlatformName) && retval;
}

bool Rendezvous::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   if (aInput.GetCommand() == "finite")
   {
      auto logger =
         ut::log::warning() << "Finite rendezvous maneuvers will have less accuracy in the resulting solution.";
      logger.AddNote() << "Type: " << GetType();
   }

   if (WsfOrbitalEvent::ProcessInput(aInput))
   {
   }
   else if (GetTargetManeuver().ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = GetMatchVelocityManeuver().ProcessInput(aInput);
   }
   return myCommand;
}

WsfOrbitalEvent* WsfOrbitalManeuvers::Rendezvous::AdvanceMissionEvent(const UtCalendar&                        aEpoch,
                                                                      const wsf::space::OrbitalMissionContext& aContext)
{
   WsfOrbitalEvent* eventPtr = GetCurrentMissionEvent();
   if ((eventPtr != nullptr) && (eventPtr->GetType() == Target::GetTypeId()) && eventPtr->IsComplete())
   {
      const Target& intercept = static_cast<const Target&>(*eventPtr);
      // TODO: Can't I use the member variable here?
      MatchVelocity& matchVelocity = static_cast<MatchVelocity&>(*GetNextMissionEvent());
      UtTimeValue    offsetTime{intercept.GetInterceptTime().GetTimeSince(intercept.GetStartTime()),
                             UtUnitTime::cSTANDARD_UNIT_ID};
      auto&          condition = dynamic_cast<wsf::space::RelativeTimeCondition&>(matchVelocity.GetCondition());
      condition.SetOffsetTime(offsetTime);
   }
   return WsfOrbitalMissionSequence::AdvanceMissionEvent(aEpoch, aContext);
}

UtVec3d WsfOrbitalManeuvers::Rendezvous::GetOffset() const
{
   return GetTargetManeuver().GetOffset();
}

UtVec3d WsfOrbitalManeuvers::Rendezvous::GetOffsetInUnits() const
{
   return GetTargetManeuver().GetOffsetInUnits();
}

std::string WsfOrbitalManeuvers::Rendezvous::GetOffsetUnit() const
{
   return GetTargetManeuver().GetOffsetUnit();
}

void WsfOrbitalManeuvers::Rendezvous::SetOffset(const UtVec3d& aOffset, wsf::space::OrbitalReferenceFrame aFrame)
{
   GetTargetManeuver().SetOffset(aOffset, aFrame);
}

void WsfOrbitalManeuvers::Rendezvous::SetOffsetInUnits(const UtVec3d&                    aOffset,
                                                       const std::string&                aUnit,
                                                       wsf::space::OrbitalReferenceFrame aFrame)
{
   GetTargetManeuver().SetOffsetInUnits(aOffset, aUnit, aFrame);
}

UtVec3d WsfOrbitalManeuvers::Rendezvous::GetVelocityOffset() const
{
   return GetTargetManeuver().GetVelocityOffset();
}

UtVec3d WsfOrbitalManeuvers::Rendezvous::GetVelocityOffsetInUnits() const
{
   return GetTargetManeuver().GetVelocityOffsetInUnits();
}

std::string WsfOrbitalManeuvers::Rendezvous::GetVelocityOffsetUnit() const
{
   return GetTargetManeuver().GetVelocityOffsetUnit();
}

void WsfOrbitalManeuvers::Rendezvous::SetVelocityOffset(const UtVec3d& aOffset, wsf::space::OrbitalReferenceFrame aFrame)
{
   GetTargetManeuver().SetVelocityOffset(aOffset, aFrame);
}

void WsfOrbitalManeuvers::Rendezvous::SetVelocityOffsetInUnits(const UtVec3d&                    aOffset,
                                                               const std::string&                aUnit,
                                                               wsf::space::OrbitalReferenceFrame aFrame)
{
   GetTargetManeuver().SetVelocityOffsetInUnits(aOffset, aUnit, aFrame);
}

UtTimeValue WsfOrbitalManeuvers::Rendezvous::GetTargetOffsetTime() const
{
   return GetTargetManeuver().GetTargetOffsetTime();
}

void WsfOrbitalManeuvers::Rendezvous::SetTargetOffsetTime(const UtTimeValue& aTimeOffset)
{
   GetTargetManeuver().SetTargetOffsetTime(aTimeOffset);
}

UtTimeValue WsfOrbitalManeuvers::Rendezvous::GetTargetLagTime() const
{
   return GetTargetManeuver().GetTargetLagTime();
}

void WsfOrbitalManeuvers::Rendezvous::SetTargetLagTime(const UtTimeValue& aLagTime)
{
   GetTargetManeuver().SetTargetLagTime(aLagTime);
}

//! Return the maximum delta T for the intercept.
UtTimeValue WsfOrbitalManeuvers::Rendezvous::GetMaximumDeltaT() const
{
   return GetTargetManeuver().GetMaximumDeltaT();
}

//! Return the maximum delta V for the intercept.
UtSpeedValue WsfOrbitalManeuvers::Rendezvous::GetMaximumDeltaV() const
{
   return GetTargetManeuver().GetMaximumDeltaV();
}

//! Return the delta T for the intercept.
UtTimeValue WsfOrbitalManeuvers::Rendezvous::GetDeltaTime() const
{
   return GetTargetManeuver().GetDeltaTime();
}

//! Return the optimization option for the intercept.
OptimizeOption WsfOrbitalManeuvers::Rendezvous::GetOptimizeOption() const
{
   return GetTargetManeuver().GetOptimizeOption();
}

//! Return the optimization cost for the maneuver.
const wsf::space::OrbitalTargetingCost* WsfOrbitalManeuvers::Rendezvous::GetOptimizationCost() const
{
   return GetTargetManeuver().GetOptimizationCost();
}

//! Return the name of the platform targeted by this intercept.
WsfStringId WsfOrbitalManeuvers::Rendezvous::GetPlatformName() const
{
   return GetTargetManeuver().GetPlatformName();
}

//! Set the maximum delta T for the intercept.
void WsfOrbitalManeuvers::Rendezvous::SetMaximumDeltaT(const UtTimeValue& aMaxTime)
{
   GetTargetManeuver().SetMaximumDeltaT(aMaxTime);
}

//! Set the maximum delta V for the intercept.
void WsfOrbitalManeuvers::Rendezvous::SetMaximumDeltaV(const UtSpeedValue& aMaxDeltaV)
{
   GetTargetManeuver().SetMaximumDeltaV(aMaxDeltaV);
}

//! Set the delta T for the intercept.
void WsfOrbitalManeuvers::Rendezvous::SetDeltaTime(const UtTimeValue& aDeltaTime)
{
   GetTargetManeuver().SetDeltaTime(aDeltaTime);
}

//! Set the optimization option for the intercept.
void WsfOrbitalManeuvers::Rendezvous::SetOptimizeOption(OptimizeOption aOption)
{
   GetTargetManeuver().SetOptimizeOption(aOption);
}

//! Set the optimization cost function for the maneuver.
void WsfOrbitalManeuvers::Rendezvous::SetOptimizationCost(std::unique_ptr<wsf::space::OrbitalTargetingCost> aCostPtr)
{
   GetTargetManeuver().SetOptimizationCost(std::move(aCostPtr));
}

//! Set the platform target for the intercept.
//! This will only have an effect if Initialize() has not been invoked.
void WsfOrbitalManeuvers::Rendezvous::SetPlatformName(WsfStringId aPlatformName)
{
   GetTargetManeuver().SetPlatformName(aPlatformName);
}

void WsfOrbitalManeuvers::Rendezvous::SetLocalTrackId(const WsfTrackId& aTrackId)
{
   GetTargetManeuver().SetLocalTrackId(aTrackId);
}

void WsfOrbitalManeuvers::Rendezvous::SetLibrationPoint(wsf::space::LibrationPoint::System aSystem,
                                                        wsf::space::LibrationPoint::Point  aPoint)
{
   GetTargetManeuver().SetLibrationPoint(aSystem, aPoint);
}

void WsfOrbitalManeuvers::Rendezvous::SetKinematicStateTarget(const UtVec3d&     aPositionInertial,
                                                              const std::string& aPositionUnits,
                                                              const UtVec3d&     aVelocityInertial,
                                                              const std::string& aVelocityUnits)
{
   GetTargetManeuver().SetKinematicStateTarget(aPositionInertial, aPositionUnits, aVelocityInertial, aVelocityUnits);
}

void WsfOrbitalManeuvers::Rendezvous::SetTargetPoint(std::unique_ptr<wsf::space::OrbitalTargetPoint> aTargetPointPtr)
{
   GetTargetManeuver().SetTargetPoint(std::move(aTargetPointPtr));
}

void WsfOrbitalManeuvers::Rendezvous::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitRendezvous(this);
}

double WsfOrbitalManeuvers::Rendezvous::GetTolerance() const
{
   return GetTargetManeuver().GetTolerance();
}

void WsfOrbitalManeuvers::Rendezvous::SetTolerance(double aTolerance)
{
   GetTargetManeuver().SetTolerance(aTolerance);
}

const wsf::space::TargetPointOptions& WsfOrbitalManeuvers::Rendezvous::GetTargetPointOptions() const
{
   return GetTargetManeuver().GetTargetPointOptions();
}

const UtOrbitalPropagatorBase*
WsfOrbitalManeuvers::Rendezvous::GetTargetPropagator(const wsf::space::OrbitalMissionContext& aContext) const
{
   return GetTargetManeuver().GetTargetPropagator(aContext);
}

void WsfOrbitalManeuvers::Rendezvous::SetupManevuerPointers()
{
   auto iter  = GetSequence().begin();
   mTargetPtr = dynamic_cast<Target*>(iter->get());
   if (mTargetPtr == nullptr)
   {
      throw std::runtime_error{"Unable to find Target maneuver as part of Rendezvous."};
   }
   ++iter;
   mMatchVelocityPtr = dynamic_cast<MatchVelocity*>(iter->get());
   if (mMatchVelocityPtr == nullptr)
   {
      throw std::runtime_error{"Unable to find MatchVelocity maneuver as part of Rendezvous."};
   }
}

Target& WsfOrbitalManeuvers::Rendezvous::GetTargetManeuver() const
{
   return *mTargetPtr;
}

MatchVelocity& WsfOrbitalManeuvers::Rendezvous::GetMatchVelocityManeuver() const
{
   return *mMatchVelocityPtr;
}

} // namespace WsfOrbitalManeuvers
