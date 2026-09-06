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

#ifndef WSFSPACEORBITALMISSIONCONTEXT_HPP
#define WSFSPACEORBITALMISSIONCONTEXT_HPP

#include <string>

class UtCalendar;
class UtOrbitalPropagatorBase;
#include "UtVec3.hpp"

class WsfOrbitalEvent;
class WsfOrbitalManeuver;
class WsfSimulation;
class WsfSpaceMoverBase;

namespace wsf
{
namespace space
{

//! This class defines the interface for an execution context for orbital missions.
//!
//! Concrete subclasses of this class provides the needed data for orbital mission
//! execution for a particular use case. For example, one subclass will provide the
//! needed dependencies when executing the mission in the simulation, and one will
//! provide the needed dependencies when executing the mission when verifying it.
class OrbitalMissionContext
{
public:
   OrbitalMissionContext()          = default;
   virtual ~OrbitalMissionContext() = default;

   //! Update to the given @p aEpoch.
   //!
   //! This typically will update member variables for those subclasses that
   //! hold any.
   //!
   //! \param aEpoch The epoch to which to update this context.
   virtual void Update(const UtCalendar& aEpoch) = 0;

   //! Initialize the context at the given @p aEpoch.
   //!
   //! \param aEpoch The epoch at which this context is being initialized.
   //! \returns true if initialization was successful, false otherwise.
   virtual bool Initialize(const UtCalendar& aEpoch) = 0;

   //! Return the propagator representing the current state of the executing platform.
   //!
   //! In some contexts, this will not be a propagator owned by any platform,
   //! but will instead be the propagator a platform would have were it executing
   //! the mission sequence during a simulation.
   //!
   //! \return The propagator giving the current state during the execution of the mission.
   virtual const UtOrbitalPropagatorBase& GetPropagator() const = 0;

   //! Return the simulation to which this context refers.
   //!
   //! Some OrbitalMissionContexts will be associated with a simulation, either because
   //! the mission is executing in the simulation, or because the simulation is available
   //! as a source of information.
   //!
   //! \returns The simulation to which this context refers, or nullptr.
   virtual WsfSimulation* GetSimulation() const { return nullptr; }

   //! Return the space mover to which this context refers.
   //!
   //! Some OrbitalMissionContexts will be associtated with a space mover, typically
   //! because the context is on in which a space mover's state is actually being
   //! updated by the mission sequence.
   //!
   //! \returns The space mover to which this context refers, or nullptr.
   virtual WsfSpaceMoverBase* GetSpaceMover() const { return nullptr; }

   //! Indicate that an orbital mission event was initiated.
   //!
   //! This method offers a means for this context to take action whenever a mission event
   //! is initiated, such as triggering observers.
   //!
   //! \param aEpoch        The epoch at which point a mission event was initiated.
   //! \param aMissionEvent The mission event being initiated.
   virtual void InitiateMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const {}

   //! Indicate that an orbital mission event was updated.
   //!
   //! This method offers a means for this context to take action whenever a mission event
   //! is updated, such as triggering observers.
   //!
   //! \param aEpoch        The epoch at which point a mission event was updated.
   //! \param aMissionEvent The mission event being updated.
   virtual void UpdateMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const {}

   //! Indicate that an orbital mission event was completed.
   //!
   //! This method offers a means for this context to take action whenever a mission event
   //! is completed, such as triggering observers.
   //!
   //! \param aEpoch        The epoch at which point a mission event was completed.
   //! \param aMissionEvent The mission event being completed.
   virtual void CompleteMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const {}

   //! Indicate that an orbital mission event was cancelled.
   //!
   //! This method offers a means for this context to take action whenever a mission event
   //! is cancelled, such as triggering observers.
   //!
   //! \param aEpoch        The epoch at which point a mission event was cancelled.
   //! \param aMissionEvent The mission event being cancelled.
   virtual void CancelMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const {}

   //! Command a maneuver at the given epoch.
   //!
   //! Indicate to the context that a maneuver should occur at the given @p aEpoch
   //! and there should be a change in ECI velocity of the given @p aCommandedDeltaV.
   //! The actual execution of the maneuver may be unable to achieve the commanded
   //! delta-v, so the actually achieved delta-v is returned via @p aActualDeltaV.
   //! If the maneuver was unsuccessful completely, false is returned.
   //!
   //! \param aEpoch              The epoch at which the maneuver is to occur.
   //! \param aCommandedDeltaV    The delta-v commanded for the maneuver.
   //! \param aActualDeltaV [out] The actual delta-v that was possible.
   //! \returns                   true if the maneuver was successful, false otherwise.
   virtual bool Maneuver(const UtCalendar& aEpoch, const UtVec3d& aCommandedDeltaV, UtVec3d& aActualDeltaV) const
   {
      return false;
   }

   //! Get the available delta-v in the executing context.
   virtual double GetAvailableDeltaV() const { return -1.0; }

   //! Get the delta-v required to maneuver for the given @p aDuration.
   virtual double GetRequiredDeltaV(double aDuration) const { return -1.0; }

   //! Perform a staging operation.
   virtual bool PerformStagingOperation() const { return false; }

   //! Get a name identifying this orbital mission context.
   virtual std::string GetName() const { return std::string{}; }

   //! Return the duration of the given maneuver for the given delta-v in this context.
   //!
   //! \param aEpoch    The epoch at which the maneuver is executing.
   //! \param aManeuver The maneuver in question.
   //! \param aDeltaV   The delta-v to be expended in the maneuver.
   //! \returns         The expected duration of the maneuver.
   virtual double GetManeuverDuration(const UtCalendar& aEpoch, const WsfOrbitalManeuver& aManeuver, double aDeltaV) const
   {
      return -1.0;
   }
};

} // namespace space
} // namespace wsf

#endif // WSFSPACEORBITALMISSIONCONTEXT_HPP
