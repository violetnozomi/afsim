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

#include "WsfSpaceOrbitalMissionVerificationContext.hpp"

#include "WsfOrbitalManeuver.hpp"

namespace wsf
{
namespace space
{

void OrbitalMissionVerificationContext::Update(const UtCalendar& aEpoch)
{
   mPropagatorPtr->Update(aEpoch);
}

bool OrbitalMissionVerificationContext::Initialize(const UtCalendar& aEpoch)
{
   mPropagatorPtr->Initialize(aEpoch);
   mManeuveringPtr->Initialize(aEpoch);
   return mMissionEventsPtr->Initialize(aEpoch, *this);
}

bool OrbitalMissionVerificationContext::Maneuver(const UtCalendar& aEpoch,
                                                 const UtVec3d&    aCommandedDeltaV,
                                                 UtVec3d&          aActualDeltaV) const
{
   bool maneuvered =
      mManeuveringPtr->Maneuver(aEpoch,
                                static_cast<const WsfOrbitalManeuver&>(*mMissionEventsPtr->GetCurrentMissionEvent()),
                                aCommandedDeltaV,
                                aActualDeltaV);
   if (maneuvered && (aActualDeltaV.MagnitudeSquared() > 0.0))
   {
      const auto& orbitalState = mPropagatorPtr->GetOrbitalState();
      auto        stateVector  = orbitalState.GetOrbitalStateVectorInertial();
      auto        velocity     = stateVector.GetVelocity();
      velocity.Add(velocity, aActualDeltaV);
      mPropagatorPtr->Initialize(ut::OrbitalState(GetPropagator().GetCurrentTime(),
                                                  orbitalState.GetCentralBody(),
                                                  orbitalState.GetCoordinateSystem(),
                                                  ut::OrbitalState::ReferenceFrame::cECI,
                                                  ut::OrbitalState::Vector(stateVector.GetLocation(), velocity)));
   }
   return maneuvered;
}

double OrbitalMissionVerificationContext::GetAvailableDeltaV() const
{
   return mManeuveringPtr->GetAvailableDeltaV();
}

double OrbitalMissionVerificationContext::GetRequiredDeltaV(double aDuration) const
{
   return mManeuveringPtr->GetRequiredDeltaV(aDuration);
}

bool OrbitalMissionVerificationContext::PerformStagingOperation() const
{
   return mManeuveringPtr->PerformStagingOperation();
}

std::string OrbitalMissionVerificationContext::GetName() const
{
   return mPlatformName + std::string{"_mission_verification"};
}

double OrbitalMissionVerificationContext::GetManeuverDuration(const UtCalendar&         aEpoch,
                                                              const WsfOrbitalManeuver& aManeuver,
                                                              double                    aDeltaV) const
{
   return mManeuveringPtr->GetManeuverDuration(aEpoch, aManeuver, aDeltaV);
}

bool OrbitalMissionVerificationContext::VerifyMission(verify::PreOperation aPreVerifyOp, verify::PostOperation aPostVerifyOp)
{
   bool verified{true};

   WsfOrbitalEvent* currentMissionEventPtr = GetCurrentMissionEvent();
   int              sequenceIndex{0};
   while (verified)
   {
      Update(currentMissionEventPtr->GetStartTime());
      if (aPreVerifyOp)
      {
         aPreVerifyOp(sequenceIndex,
                      currentMissionEventPtr,
                      GetPropagator().GetCurrentTime(),
                      GetPropagator().GetOrbitalState().GetOrbitalElements(),
                      GetAvailableDeltaV());
      }

      auto status = VerifyEvent(*currentMissionEventPtr);
      if (aPostVerifyOp)
      {
         aPostVerifyOp(sequenceIndex,
                       currentMissionEventPtr,
                       GetPropagator().GetCurrentTime(),
                       GetPropagator().GetOrbitalState().GetOrbitalElements(),
                       GetAvailableDeltaV(),
                       status);
      }

      if (status == verify::Status::cUNVERIFIABLE)
      {
         break;
      }
      else if (status == verify::Status::cFAIL)
      {
         verified    = false;
         auto logger = ut::log::warning() << "Orbital MissionEvent failed verification.";
         logger.AddNote() << "Type: " << currentMissionEventPtr->GetType();
         logger.AddNote() << "Name: " << GetName();
         break;
      }

      try
      {
         // Note that the calendar must be copied...
         UtCalendar currentEpoch = GetPropagator().GetCurrentTime();
         currentMissionEventPtr  = AdvanceMission(currentEpoch);
         ++sequenceIndex;
      }
      catch (const WsfOrbitalEvent::AdvanceMissionEventException&)
      {
         if (aPostVerifyOp)
         {
            aPostVerifyOp(sequenceIndex + 1,
                          nullptr,
                          GetPropagator().GetCurrentTime(),
                          GetPropagator().GetOrbitalState().GetOrbitalElements(),
                          GetAvailableDeltaV(),
                          verify::Status::cFAIL);
         }
         verified = false;
         break;
      }
      if (currentMissionEventPtr == nullptr)
      {
         break;
      }
   };

   return verified;
}

WsfOrbitalEvent* OrbitalMissionVerificationContext::GetCurrentMissionEvent() const
{
   return mMissionEventsPtr->GetCurrentMissionEvent();
}

WsfOrbitalEvent* OrbitalMissionVerificationContext::AdvanceMission(const UtCalendar& aEpoch) const
{
   return mMissionEventsPtr->AdvanceMissionEvent(aEpoch, *this);
}

//! Verify an event.
//!
//! Verification can fail in the case that event-specific preconditions or postconditions are
//! not met.  Preconditions are usually that the specified WsfOrbitalEvent::Condition is met.
//! Postcondition checks are meant to catch nonphysical or undesirable situations (such as
//! unintentional intersection with Earth).
verify::Status OrbitalMissionVerificationContext::VerifyEvent(WsfOrbitalEvent& aEvent)
{
   verify::Status retval{verify::Status::cUNVERIFIABLE};

   if (aEvent.CanVerify())
   {
      UtCalendar updateTime = aEvent.GetStartTime();
      bool       verified   = aEvent.EvaluatePreconditions(*this);

      if (verified)
      {
         do
         {
            verified = aEvent.Execute(updateTime, *this);

            if (aEvent.IsFinite() && !aEvent.IsComplete())
            {
               assert(static_cast<double>(aEvent.GetUpdateInterval()) > 0.0);
               updateTime.AdvanceTimeBy(static_cast<double>(aEvent.GetUpdateInterval()));
            }
         } while (verified && !aEvent.IsSequence() && !aEvent.IsComplete());
         verified &= aEvent.EvaluatePostconditions(*this);
      }

      if (verified)
      {
         retval = verify::Status::cPASS;
      }
      else
      {
         retval = verify::Status::cFAIL;
      }
   }

   return retval;
}

} // namespace space
} // namespace wsf
