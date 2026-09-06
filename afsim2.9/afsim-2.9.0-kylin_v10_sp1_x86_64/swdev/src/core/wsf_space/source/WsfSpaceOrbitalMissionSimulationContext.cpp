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

#include "WsfSpaceOrbitalMissionSimulationContext.hpp"

#include <stdexcept>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfOrbitalManeuvering.hpp"
#include "WsfOrbitalScriptEvent.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfSpaceObserver.hpp"

namespace wsf
{
namespace space
{

OrbitalMissionSimulationContext::OrbitalMissionSimulationContext(WsfSpaceMoverBase& aMover)
   : mMover(aMover)
   , mMissionEventsPtr{new WsfOrbitalMissionSequence{aMover.GetScenario()}}
{
}

OrbitalMissionSimulationContext::OrbitalMissionSimulationContext(WsfSpaceMoverBase&                     aMover,
                                                                 const OrbitalMissionSimulationContext& aOther)
   : mMover(aMover)
   , mMissionEventsPtr{aOther.mMissionEventsPtr}
{
}

//! Update the MissionEvents state to the specified absolute time.
void OrbitalMissionSimulationContext::Update(const UtCalendar& aEpoch)
{
   if (aEpoch.GetTimeSince(mLastUpdateTime) > 1.0e-6)
   {
      mLastUpdateTime = aEpoch;
      double simTime  = aEpoch.GetTimeSince(mMover.GetSimulation()->GetDateTime().GetStartDateAndTime());
      mMover.GetPlatform()->Update(simTime);
      mMover.GetOrbitalManeuvering().SetPlatformAttributes(mMover.GetPlatform());
   }
}

bool OrbitalMissionSimulationContext::Initialize(const UtCalendar& aEpoch)
{
   mLastUpdateTime = aEpoch;
   bool ok         = mMissionEventsPtr->Initialize(aEpoch, *this);

   mMover.GetPropagator().Update(aEpoch);

   if (ok)
   {
      if (!mMissionEventsPtr->IsEmpty())
      {
         WsfOrbitalEvent* missionEventPtr = mMissionEventsPtr->GetCurrentMissionEvent();
         if (missionEventPtr)
         {
            Schedule(missionEventPtr);
         }
      }
   }

   return ok;
}

const UtOrbitalPropagatorBase& OrbitalMissionSimulationContext::GetPropagator() const
{
   return mMover.GetPropagator();
}

WsfSimulation* OrbitalMissionSimulationContext::GetSimulation() const
{
   return mMover.GetSimulation();
}

WsfSpaceMoverBase* OrbitalMissionSimulationContext::GetSpaceMover() const
{
   return &mMover;
}

void OrbitalMissionSimulationContext::InitiateMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const
{
   if (IsReportable(aMissionEvent))
   {
      double simTime = aEpoch.GetTimeSince(GetSimulation()->GetDateTime().GetStartDateAndTime());
      WsfObserver::OrbitalManeuverInitiated(GetSimulation())(simTime, GetSpaceMover(), aMissionEvent);
   }
}

void OrbitalMissionSimulationContext::UpdateMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const
{
   if (IsReportable(aMissionEvent))
   {
      double simTime = aEpoch.GetTimeSince(GetSimulation()->GetDateTime().GetStartDateAndTime());
      WsfObserver::OrbitalManeuverUpdated(GetSimulation())(simTime, GetSpaceMover(), aMissionEvent);
   }
}

void OrbitalMissionSimulationContext::CompleteMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const
{
   if (IsReportable(aMissionEvent))
   {
      double simTime = aEpoch.GetTimeSince(GetSimulation()->GetDateTime().GetStartDateAndTime());
      WsfObserver::OrbitalManeuverCompleted(GetSimulation())(simTime, GetSpaceMover(), aMissionEvent);
   }
}

void OrbitalMissionSimulationContext::CancelMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const
{
   if (IsReportable(aMissionEvent))
   {
      double simTime = aEpoch.GetTimeSince(GetSimulation()->GetDateTime().GetStartDateAndTime());
      WsfObserver::OrbitalManeuverCanceled(GetSimulation())(simTime, GetSpaceMover(), aMissionEvent);
   }
}

bool OrbitalMissionSimulationContext::Maneuver(const UtCalendar& aEpoch,
                                               const UtVec3d&    aCommandedDeltaV,
                                               UtVec3d&          aActualDeltaV) const
{
   return mMover.Maneuver(aEpoch, aCommandedDeltaV, aActualDeltaV);
}

double OrbitalMissionSimulationContext::GetAvailableDeltaV() const
{
   return mMover.GetAvailableDeltaV();
}

double OrbitalMissionSimulationContext::GetRequiredDeltaV(double aDuration) const
{
   return mMover.GetRequiredDeltaV(aDuration);
}

bool OrbitalMissionSimulationContext::PerformStagingOperation() const
{
   return mMover.PerformStagingOperation();
}

std::string OrbitalMissionSimulationContext::GetName() const
{
   return mMover.GetPlatform()->GetName();
}

double OrbitalMissionSimulationContext::GetManeuverDuration(const UtCalendar&         aEpoch,
                                                            const WsfOrbitalManeuver& aManeuver,
                                                            double                    aDeltaV) const
{
   return mMover.GetManeuverDuration(aEpoch, aManeuver, aDeltaV);
}

bool OrbitalMissionSimulationContext::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if ((command == "mission_sequence") || (command == "maneuvers"))
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         myCommand = mMissionEventsPtr->ProcessInput(aInput);

         if (!myCommand)
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void OrbitalMissionSimulationContext::Schedule(WsfOrbitalEvent* aMissionEventPtr)
{
   auto eventPtr = ut::make_unique<Event>(aMissionEventPtr, *this);
   eventPtr->SetTime(aMissionEventPtr->GetStartTime().GetTimeSince(GetSimulation()->GetDateTime().GetStartDateAndTime()));
   GetSimulation()->AddEvent(std::move(eventPtr));
}

void OrbitalMissionSimulationContext::ScheduleMission(const UtCalendar&                          aEpoch,
                                                      std::unique_ptr<WsfOrbitalMissionSequence> aMissionEventsPtr)
{
   auto setEventsPtr = ut::make_unique<SetEvents>(aEpoch, move(aMissionEventsPtr), *this);
   setEventsPtr->SetTime(GetSimulation()->GetSimTime() + 1.0e-9);
   GetSimulation()->AddEvent(std::move(setEventsPtr));
}

const WsfOrbitalMissionSequence& OrbitalMissionSimulationContext::GetMissionEvents() const
{
   return *mMissionEventsPtr;
}

OrbitalMissionSimulationContext::Event::Event(WsfOrbitalEvent* aMissionEventPtr, OrbitalMissionSimulationContext& aContext)
   : mMissionEventPtr{aMissionEventPtr}
   , mContext(aContext)
   , mTime{aMissionEventPtr->GetStartTime()}
   , mPlatformIndex{0}
   , mMissionEventId{aMissionEventPtr->GetUniqueId()}
{
   if (mContext.GetSpaceMover())
   {
      mPlatformIndex = mContext.GetSpaceMover()->GetPlatform()->GetIndex();
   }
   else
   {
      throw std::runtime_error{"Orbital mission unable to retrieve space mover. Context error."};
   }
}

WsfEvent::EventDisposition OrbitalMissionSimulationContext::Event::Execute()
{
   WsfEvent::EventDisposition disposition = WsfEvent::cDELETE;
   if (mContext.GetSimulation()->PlatformExists(mPlatformIndex))
   {
      const auto* currentMissionEventPtr = mContext.GetCurrentMissionEvent();

      // Make sure the current MissionEvent is the one we are executing;
      // Otherwise it was canceled or another MissionEvent was scheduled instead.
      if ((currentMissionEventPtr != nullptr) && (currentMissionEventPtr->GetUniqueId() == mMissionEventId))
      {
         mContext.Update(mTime);

         if (!mMissionEventPtr->IsComplete())
         {
            // Update the platform to the current time and execute the MissionEvent.
            mMissionEventPtr->Execute(mTime, mContext);
         }

         if (mMissionEventPtr->IsComplete())
         {
            mContext.ScheduleNextMissionEvent(mTime);
         }
         else if (mMissionEventPtr->IsSequence())
         {
            // If a sequence is not complete, any initial time delay has completed, and we move on to the next one.
            mContext.ScheduleNextMissionEvent(mTime);
         }
         else // not complete and not a sequence
         {
            UtCalendar nextTime;
            bool shouldExecute = mMissionEventPtr->GetNextExecutionTime(mTime, mContext.GetPropagator(), nextTime);
            if (shouldExecute)
            {
               double dt = nextTime.GetTimeSince(mTime);
               SetTime(GetTime() + dt);
               mTime       = nextTime;
               disposition = WsfEvent::cRESCHEDULE;
            }
         }
      }
   }

   return disposition;
}

OrbitalMissionSimulationContext::SetEvents::SetEvents(const UtCalendar&                          aEpoch,
                                                      std::unique_ptr<WsfOrbitalMissionSequence> aMissionEventsPtr,
                                                      OrbitalMissionSimulationContext&           aContext)
   : mEpoch(aEpoch)
   , mMissionEventsPtr(move(aMissionEventsPtr))
   , mContext(aContext)
{
}

WsfEvent::EventDisposition OrbitalMissionSimulationContext::SetEvents::Execute()
{
   const WsfOrbitalEvent* currentEventPtr = mContext.GetCurrentMissionEvent();

   // The following checks to see whether this is a nullptr or if it is a scripted event that has not completed.
   if ((currentEventPtr == nullptr) ||
       !((currentEventPtr->GetType() == WsfOrbitalScriptEvent::GetTypeId()) && !currentEventPtr->IsComplete()))
   {
      bool scheduled = mContext.ScheduleMissionP(mEpoch, move(mMissionEventsPtr));
      if (!scheduled && currentEventPtr)
      {
         auto logger = ut::log::warning() << "Mission sequence could not execute.";
         logger.AddNote() << "T = " << GetTime();
         logger.AddNote() << "Manager: " << mContext.GetName();
         logger.AddNote() << "Current event " << currentEventPtr->GetType() << "(" << currentEventPtr->GetUniqueId()
                          << ") could not be initialized.";
      }
   }
   else // This is a scripted event that has not completed.
   {
      // Script events cannot be canceled until complete because they execute using the event that would be deleted.
      auto logger = ut::log::warning() << "Mission sequence could not execute.";
      logger.AddNote() << "T = " << GetTime();
      logger.AddNote() << "Manager: " << mContext.GetName();
      logger.AddNote() << "Current script event " << currentEventPtr->GetType() << "(" << currentEventPtr->GetUniqueId()
                       << ") not complete.";
   }
   return WsfEvent::cDELETE;
}

const WsfOrbitalEvent* OrbitalMissionSimulationContext::GetCurrentMissionEvent() const
{
   return mMover.GetCurrentMissionEvent();
}

void OrbitalMissionSimulationContext::ScheduleNextMissionEvent(const UtCalendar& aEpoch)
{
   WsfOrbitalEvent* currentMissionEventPtr = nullptr;
   try
   {
      currentMissionEventPtr = mMissionEventsPtr->AdvanceMissionEvent(aEpoch, *this);
      if (currentMissionEventPtr != nullptr)
      {
         Schedule(currentMissionEventPtr);
      }
   }
   catch (const WsfOrbitalEvent::AdvanceMissionEventException&)
   {
      ut::log::warning() << "Mission Sequence Terminated.";
   }
}

bool OrbitalMissionSimulationContext::ScheduleMissionP(const UtCalendar&                          aEpoch,
                                                       std::unique_ptr<WsfOrbitalMissionSequence> aMissionEventsPtr)
{
   bool ok = aMissionEventsPtr->Initialize(aEpoch, *this);
   if (ok)
   {
      mLastUpdateTime = aEpoch;

      // The propagator must be updated to the current epoch.
      mMover.GetPropagator().Update(aEpoch);

      // Cleanup and log the current event if necessary.
      WsfOrbitalEvent* currentEventPtr = mMissionEventsPtr->GetCurrentMissionEvent();
      if ((currentEventPtr != nullptr) && (!currentEventPtr->IsComplete()))
      {
         // Cancel the current event.
         currentEventPtr->SetCanceled();
         CancelMissionEvent(mLastUpdateTime, *currentEventPtr);
      }

      mMissionEventsPtr = std::move(aMissionEventsPtr);

      WsfOrbitalEvent* missionEventPtr = mMissionEventsPtr->GetCurrentMissionEvent();
      if (missionEventPtr != nullptr)
      {
         Schedule(missionEventPtr);
      }
   }

   return ok;
}

bool OrbitalMissionSimulationContext::IsReportable(const WsfOrbitalEvent& aOrbitalEvent) const
{
   return aOrbitalEvent.IsA_TypeOf(WsfOrbitalManeuver::GetTypeId()) ||
          (aOrbitalEvent.IsA_TypeOf(WsfOrbitalMissionSequence::GetTypeId()) && aOrbitalEvent.GetType() != "SEQUENCE");
}

} // namespace space
} // namespace wsf
