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

#ifndef WSFSPACEORBITALMISSIONSIMULATIONCONTEXT_HPP
#define WSFSPACEORBITALMISSIONSIMULATIONCONTEXT_HPP

#include "UtCloneablePtr.hpp"
class UtInput;

#include "WsfEvent.hpp"
#include "WsfOrbitalMissionSequence.hpp"
class WsfSpaceMoverBase;
#include "WsfSpaceOrbitalMissionContext.hpp"

namespace wsf
{
namespace space
{

//! The simulation mission context is used for misson sequence execution during a simulation.
//!
//! This orbital mission context connects to simulation objects during the execution
//! of the mission sequence during the simulation. Additionally, this context stores the mission
//! sequence for space movers.
class OrbitalMissionSimulationContext : public OrbitalMissionContext
{
public:
   explicit OrbitalMissionSimulationContext(WsfSpaceMoverBase& aMover);
   OrbitalMissionSimulationContext(WsfSpaceMoverBase& aMover, const OrbitalMissionSimulationContext& aOther);
   OrbitalMissionSimulationContext(const OrbitalMissionSimulationContext& aOther) = delete;
   ~OrbitalMissionSimulationContext() override                                    = default;

   void                           Update(const UtCalendar& aEpoch) override;
   bool                           Initialize(const UtCalendar& aEpoch) override;
   const UtOrbitalPropagatorBase& GetPropagator() const override;
   WsfSimulation*                 GetSimulation() const override;
   WsfSpaceMoverBase*             GetSpaceMover() const override;
   void   InitiateMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const override;
   void   UpdateMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const override;
   void   CompleteMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const override;
   void   CancelMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const override;
   bool   Maneuver(const UtCalendar& aEpoch, const UtVec3d& aCommandedDeltaV, UtVec3d& aActualDeltaV) const override;
   double GetAvailableDeltaV() const override;
   double GetRequiredDeltaV(double aDuration) const override;
   bool   PerformStagingOperation() const override;
   std::string GetName() const override;
   double GetManeuverDuration(const UtCalendar& aEpoch, const WsfOrbitalManeuver& aManeuver, double aDeltaV) const override;

   // Methods specific to the simulation context

   //! This method will read in a mission sequence from input.
   bool ProcessInput(UtInput& aInput);

   UtCalendar GetLastUpdateTime() const { return mLastUpdateTime; }

   //! Schedule the given orbital event as a simulation event.
   void Schedule(WsfOrbitalEvent* aMissionEventPtr);

   //! Set the MissionEvent sequence using an event that executes at the current simulation time
   //! (necessary because a scripted event can schedule a new sequence, thus potentially deleting itself).
   //! Then, initialize and schedule the first event in the new sequence.
   //! @param aEpoch The current epoch, to be used as the basis for scheduling mission.
   //! @param aMissionEventsPtr The new mission events sequence to be executed.
   void ScheduleMission(const UtCalendar& aEpoch, std::unique_ptr<WsfOrbitalMissionSequence> aMissionEventsPtr);

   //! Return the mission events configured for this context.
   const WsfOrbitalMissionSequence& GetMissionEvents() const;

private:
   //! Execute an orbital event in the simulation.
   class Event : public WsfEvent
   {
   public:
      Event(WsfOrbitalEvent* aMissionEventPtr, OrbitalMissionSimulationContext& aContext);
      ~Event() override = default;
      WsfEvent::EventDisposition Execute() override;

   private:
      WsfOrbitalEvent*                             mMissionEventPtr;
      wsf::space::OrbitalMissionSimulationContext& mContext;
      UtCalendar                                   mTime;          // Absolute value of event time from GetTime().
      size_t                                       mPlatformIndex; // to verify future platform validity
      size_t                                       mMissionEventId;
   };

   //! Provides an execution-safe way to set a new set of mission events in the object.
   class SetEvents : public WsfEvent
   {
   public:
      SetEvents(const UtCalendar&                          aEpoch,
                std::unique_ptr<WsfOrbitalMissionSequence> aMissionEventsPtr,
                OrbitalMissionSimulationContext&           aContext);
      ~SetEvents() override = default;
      WsfEvent::EventDisposition Execute() override;

   private:
      UtCalendar                                 mEpoch;
      std::unique_ptr<WsfOrbitalMissionSequence> mMissionEventsPtr;
      OrbitalMissionSimulationContext&           mContext;
   };

   //! Return the current mission event.
   const WsfOrbitalEvent* GetCurrentMissionEvent() const;

   //! Schedule the next mission event with the simulation.
   //!
   //! This is called when one mission event completes, and the next is to be
   //! executed. In the simulation context, this scheduling is done with a
   //! simulation event.
   void ScheduleNextMissionEvent(const UtCalendar& aEpoch);


   //! Schedule the given mission sequence.
   bool ScheduleMissionP(const UtCalendar& aEpoch, std::unique_ptr<WsfOrbitalMissionSequence> aMissionEventsPtr);

   //! Filter down event types for reporting to the observers.
   bool IsReportable(const WsfOrbitalEvent& aOrbitalEvent) const;

   WsfSpaceMoverBase&                          mMover; //! The mover whose mission sequence is being executed.
   UtCalendar                                  mLastUpdateTime{};
   ut::CloneablePtr<WsfOrbitalMissionSequence> mMissionEventsPtr;
};

} // namespace space
} // namespace wsf

#endif // WSFSPACEORBITALMISSIONSIMULATIONCONTEXT_HPP
