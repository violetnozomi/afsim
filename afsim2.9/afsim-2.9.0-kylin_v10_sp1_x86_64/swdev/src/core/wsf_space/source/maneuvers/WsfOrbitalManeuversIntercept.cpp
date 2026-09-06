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

#include "WsfOrbitalManeuversIntercept.hpp"

#include <iostream>
#include <stdexcept>

#include "UtLog.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtUnitValue.hpp"
#include "WsfOrbitalEventVisitor.hpp"

namespace WsfOrbitalManeuvers
{
Intercept::Intercept(const WsfScenario& aScenario)
   : Intercept(aScenario, WsfTrackId(), cOPTIMIZE_NONE, 0.0, 0.0)
{
}

Intercept::Intercept(const WsfScenario&  aScenario,
                     const WsfTrackId&   aLocalTrackId,
                     OptimizeOption      aOptimizeOption,
                     const UtTimeValue&  aMaxTime,
                     const UtSpeedValue& aMaxDeltaV)
   : WsfOrbitalMissionSequence(aScenario)
{
   AddMissionEvent(ut::make_unique<Target>(aScenario, aLocalTrackId, aOptimizeOption, aMaxTime, aMaxDeltaV));
   AddMissionEvent(ut::make_unique<CompleteInterceptEvent>(aScenario));
   SetupManeuverPointers();
   mCanVerify = false;
   GetTargetManeuver().SetComputeForRendezvous(false);
   SetType(GetTypeId());
}

Intercept::Intercept(const WsfScenario&                    aScenario,
                     const wsf::space::TargetPointOptions& aOptions,
                     OptimizeOption                        aOptimizeOption,
                     const UtTimeValue&                    aMaxTime,
                     const UtSpeedValue&                   aMaxDeltaV)
   : WsfOrbitalMissionSequence(aScenario)
{
   AddMissionEvent(ut::make_unique<Target>(aScenario, aOptions, aOptimizeOption, aMaxTime, aMaxDeltaV));
   AddMissionEvent(ut::make_unique<CompleteInterceptEvent>(aScenario));
   SetupManeuverPointers();
   mCanVerify = false;
   GetTargetManeuver().SetComputeForRendezvous(false);
   SetType(GetTypeId());
}

Intercept::Intercept(const WsfScenario&                      aScenario,
                     const wsf::space::TargetPointOptions&   aOptions,
                     const wsf::space::OrbitalTargetingCost& aOptimizeCost,
                     const UtTimeValue&                      aMaxTime,
                     const UtSpeedValue&                     aMaxDeltaV)
   : WsfOrbitalMissionSequence(aScenario)
{
   AddMissionEvent(ut::make_unique<Target>(aScenario, aOptions, aOptimizeCost, aMaxTime, aMaxDeltaV));
   AddMissionEvent(ut::make_unique<CompleteInterceptEvent>(aScenario));
   SetupManeuverPointers();
   mCanVerify = false;
   GetTargetManeuver().SetComputeForRendezvous(false);
   SetType(GetTypeId());
}

WsfOrbitalManeuvers::Intercept::Intercept(const Intercept& aSrc)
   : WsfOrbitalMissionSequence(aSrc)
{
   SetupManeuverPointers();
}

WsfOrbitalManeuvers::Intercept::~Intercept() {}

WsfStringId Intercept::GetTypeId()
{
   return "INTERCEPT";
}

bool Intercept::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   GetCompleteInterceptEvent().SetCondition(ut::make_unique<wsf::space::RelativeTimeCondition>());
   return WsfOrbitalMissionSequence::Initialize(aEpoch, aContext);
}

bool Intercept::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   if (aInput.GetCommand() == "finite")
   {
      auto logger =
         ut::log::warning() << "Finite intercept maneuvers will have less accuracy in the resulting solution.";
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
      myCommand = GetCompleteInterceptEvent().ProcessInput(aInput);
   }
   return myCommand;
}

bool Intercept::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval = GetTargetManeuver().ValidateParameterRanges(aExecutingPlatformName);
   retval      = WsfOrbitalEvent::ValidateParameterRanges(aExecutingPlatformName) && retval;
   return retval;
}

WsfOrbitalEvent* WsfOrbitalManeuvers::Intercept::AdvanceMissionEvent(const UtCalendar&                        aEpoch,
                                                                     const wsf::space::OrbitalMissionContext& aContext)
{
   WsfOrbitalEvent* eventPtr = GetCurrentMissionEvent();
   if (eventPtr && (eventPtr->GetType() == Target::GetTypeId()) && eventPtr->IsComplete())
   {
      const Target&    intercept         = static_cast<const Target&>(*eventPtr);
      WsfOrbitalEvent& completeIntercept = *GetNextMissionEvent();
      UtTimeValue      offsetTime{intercept.GetInterceptTime().GetTimeSince(intercept.GetStartTime()),
                             UtUnitTime::cSTANDARD_UNIT_ID};
      auto&            condition = dynamic_cast<wsf::space::RelativeTimeCondition&>(completeIntercept.GetCondition());
      condition.SetOffsetTime(offsetTime);
   }
   return WsfOrbitalMissionSequence::AdvanceMissionEvent(aEpoch, aContext);
}

UtVec3d WsfOrbitalManeuvers::Intercept::GetOffset() const
{
   return GetTargetManeuver().GetOffset();
}

UtVec3d WsfOrbitalManeuvers::Intercept::GetOffsetInUnits() const
{
   return GetTargetManeuver().GetOffsetInUnits();
}

std::string WsfOrbitalManeuvers::Intercept::GetOffsetUnit() const
{
   return GetTargetManeuver().GetOffsetUnit();
}

void WsfOrbitalManeuvers::Intercept::SetOffset(const UtVec3d& aOffset, wsf::space::OrbitalReferenceFrame aFrame)
{
   GetTargetManeuver().SetOffset(aOffset, aFrame);
}

void WsfOrbitalManeuvers::Intercept::SetOffsetInUnits(const UtVec3d&                    aOffset,
                                                      const std::string&                aUnit,
                                                      wsf::space::OrbitalReferenceFrame aFrame)
{
   GetTargetManeuver().SetOffsetInUnits(aOffset, aUnit, aFrame);
}

UtVec3d WsfOrbitalManeuvers::Intercept::GetVelocityOffset() const
{
   return GetTargetManeuver().GetVelocityOffset();
}

UtVec3d WsfOrbitalManeuvers::Intercept::GetVelocityOffsetInUnits() const
{
   return GetTargetManeuver().GetVelocityOffsetInUnits();
}

std::string WsfOrbitalManeuvers::Intercept::GetVelocityOffsetUnit() const
{
   return GetTargetManeuver().GetVelocityOffsetUnit();
}

void WsfOrbitalManeuvers::Intercept::SetVelocityOffset(const UtVec3d& aOffset, wsf::space::OrbitalReferenceFrame aFrame)
{
   GetTargetManeuver().SetVelocityOffset(aOffset, aFrame);
}

void WsfOrbitalManeuvers::Intercept::SetVelocityOffsetInUnits(const UtVec3d&                    aOffset,
                                                              const std::string&                aUnit,
                                                              wsf::space::OrbitalReferenceFrame aFrame)
{
   GetTargetManeuver().SetVelocityOffsetInUnits(aOffset, aUnit, aFrame);
}

UtTimeValue WsfOrbitalManeuvers::Intercept::GetTargetOffsetTime() const
{
   return GetTargetManeuver().GetTargetOffsetTime();
}

void WsfOrbitalManeuvers::Intercept::SetTargetOffsetTime(const UtTimeValue& aTimeOffset)
{
   GetTargetManeuver().SetTargetOffsetTime(aTimeOffset);
}

UtTimeValue WsfOrbitalManeuvers::Intercept::GetTargetLagTime() const
{
   return GetTargetManeuver().GetTargetLagTime();
}

void WsfOrbitalManeuvers::Intercept::SetTargetLagTime(const UtTimeValue& aLagTime)
{
   GetTargetManeuver().SetTargetLagTime(aLagTime);
}

//! Return the maximum delta T for the intercept.
//! This will return a valid result only if Initialize() has not been
//! invoked.
UtTimeValue WsfOrbitalManeuvers::Intercept::GetMaximumDeltaT() const
{
   return GetTargetManeuver().GetMaximumDeltaT();
}

//! Return the maximum delta V for the intercept.
//! This will return a valid result only if Initialize() has not been
//! invoked.
UtSpeedValue WsfOrbitalManeuvers::Intercept::GetMaximumDeltaV() const
{
   return GetTargetManeuver().GetMaximumDeltaV();
}

//! Return the delta T for the intercept.
//! This will return a valid result only if Initialize() has not been
//! invoked.
UtTimeValue WsfOrbitalManeuvers::Intercept::GetDeltaTime() const
{
   return GetTargetManeuver().GetDeltaTime();
}

//! Return the optimization option for the intercept.
//! This will return a valid result only if Initialize() has not been
//! invoked.
OptimizeOption WsfOrbitalManeuvers::Intercept::GetOptimizeOption() const
{
   return GetTargetManeuver().GetOptimizeOption();
}

//! Return the optimization cost for this intercept.
const wsf::space::OrbitalTargetingCost* WsfOrbitalManeuvers::Intercept::GetOptimizationCost() const
{
   return GetTargetManeuver().GetOptimizationCost();
}

//! Return the name of the platform targeted by this intercept.
//! This will return a valid result only if Initialize() has not been
//! invoked.
WsfStringId WsfOrbitalManeuvers::Intercept::GetPlatformName() const
{
   return GetTargetManeuver().GetPlatformName();
}

//! Set the maximum delta T for the intercept.
//! This will only have an effect if Initialize() has not been invoked.
void WsfOrbitalManeuvers::Intercept::SetMaximumDeltaT(const UtTimeValue& aMaxTime)
{
   GetTargetManeuver().SetMaximumDeltaT(aMaxTime);
}

//! Set the maximum delta V for the intercept.
//! This will only have an effect if Initialize() has not been invoked.
void WsfOrbitalManeuvers::Intercept::SetMaximumDeltaV(const UtSpeedValue& aMaxDeltaV)
{
   GetTargetManeuver().SetMaximumDeltaV(aMaxDeltaV);
}

//! Set the delta T for the intercept.
//! This will only have an effect if Initialize() has not been invoked.
void WsfOrbitalManeuvers::Intercept::SetDeltaTime(const UtTimeValue& aDeltaTime)
{
   GetTargetManeuver().SetDeltaTime(aDeltaTime);
}

//! Set the optimization option for the intercept.
//! This will only have an effect if Initialize() has not been invoked.
void WsfOrbitalManeuvers::Intercept::SetOptimizeOption(OptimizeOption aOption)
{
   GetTargetManeuver().SetOptimizeOption(aOption);
}

//! Set the optimization cost function for the maneuver.
//! This will only have an effect if Initialize() has not been invoked.
void WsfOrbitalManeuvers::Intercept::SetOptimizationCost(std::unique_ptr<wsf::space::OrbitalTargetingCost> aCostPtr)
{
   GetTargetManeuver().SetOptimizationCost(std::move(aCostPtr));
}

//! Set the platform target for the intercept.
//! This will only have an effect if Initialize() has not been invoked.
void WsfOrbitalManeuvers::Intercept::SetPlatformName(WsfStringId aPlatformName)
{
   GetTargetManeuver().SetPlatformName(aPlatformName);
}

void WsfOrbitalManeuvers::Intercept::SetLocalTrackId(const WsfTrackId& aTrackId)
{
   GetTargetManeuver().SetLocalTrackId(aTrackId);
}

void WsfOrbitalManeuvers::Intercept::SetLibrationPoint(wsf::space::LibrationPoint::System aSystem,
                                                       wsf::space::LibrationPoint::Point  aPoint)
{
   GetTargetManeuver().SetLibrationPoint(aSystem, aPoint);
}

void WsfOrbitalManeuvers::Intercept::SetKinematicStateTarget(const UtVec3d&     aPositionInertial,
                                                             const std::string& aPositionUnits,
                                                             const UtVec3d&     aVelocityInertial,
                                                             const std::string& aVelocityUnits)
{
   GetTargetManeuver().SetKinematicStateTarget(aPositionInertial, aPositionUnits, aVelocityInertial, aVelocityUnits);
}

void WsfOrbitalManeuvers::Intercept::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitIntercept(this);
}

double WsfOrbitalManeuvers::Intercept::GetTolerance() const
{
   return GetTargetManeuver().GetTolerance();
}

void WsfOrbitalManeuvers::Intercept::SetTolerance(double aTolerance)
{
   GetTargetManeuver().SetTolerance(aTolerance);
}

const wsf::space::TargetPointOptions& WsfOrbitalManeuvers::Intercept::GetTargetPointOptions() const
{
   return GetTargetManeuver().GetTargetPointOptions();
}

WsfOrbitalManeuvers::Intercept::CompleteInterceptEvent::CompleteInterceptEvent(const WsfScenario& aScenario)
   : WsfOrbitalEvent{aScenario}
{
}

WsfOrbitalManeuvers::Intercept::CompleteInterceptEvent::CompleteInterceptEvent(const CompleteInterceptEvent& aSrc)
   : WsfOrbitalEvent(aSrc)
{
}

void WsfOrbitalManeuvers::Intercept::SetupManeuverPointers()
{
   auto iter  = GetSequence().begin();
   mTargetPtr = dynamic_cast<Target*>(iter->get());
   if (!mTargetPtr)
   {
      throw std::runtime_error{"Unable to find Target maneuver as part of Intercept."};
   }
   ++iter;
   mCompleteInterceptPtr = dynamic_cast<CompleteInterceptEvent*>(iter->get());
   if (!mCompleteInterceptPtr)
   {
      throw std::runtime_error{"Unable to find CompleteInterceptEvent as part of Intercept."};
   }
}

Target& WsfOrbitalManeuvers::Intercept::GetTargetManeuver() const
{
   return *mTargetPtr;
}

WsfOrbitalManeuvers::Intercept::CompleteInterceptEvent& WsfOrbitalManeuvers::Intercept::GetCompleteInterceptEvent() const
{
   return *mCompleteInterceptPtr;
}

} // namespace WsfOrbitalManeuvers
