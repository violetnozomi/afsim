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

#ifndef WSFSPACEORBITALMISSIONVERIFICATIONCONTEXT_HPP
#define WSFSPACEORBITALMISSIONVERIFICATIONCONTEXT_HPP

#include "wsf_space_export.h"

#include <string>

#include "UtMemory.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "WsfOrbitalManeuvering.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"
#include "WsfSpaceVerifyTypes.hpp"

namespace wsf
{
namespace space
{

//! An orbital mission context used when verifying mission sequences during initialization.
//!
//! This context is used to give a trial-run of the mission sequence during the process of
//! initializing the simulation. Unlike the OrbitalMissionSimulationContext this context
//! does not result in the modification of any simulation objects. Instead, copies of
//! objects are made when this context is created, and changes resulting from the mission
//! only affect those copies.
class WSF_SPACE_EXPORT OrbitalMissionVerificationContext : public OrbitalMissionContext
{
public:
   OrbitalMissionVerificationContext(const WsfOrbitalManeuvering&     aManeuvering,
                                     const UtOrbitalPropagatorBase&   aPropagator,
                                     const WsfOrbitalMissionSequence& aMissionEvents,
                                     const std::string&               aPlatformName)
      : mManeuveringPtr{ut::clone(aManeuvering)}
      , mPropagatorPtr{ut::clone(aPropagator)}
      , mMissionEventsPtr{ut::clone(aMissionEvents)}
      , mPlatformName{aPlatformName}
   {
   }

   ~OrbitalMissionVerificationContext() override = default;

   void Update(const UtCalendar& aEpoch) override;

   bool Initialize(const UtCalendar& aEpoch) override;

   const UtOrbitalPropagatorBase& GetPropagator() const override { return *(mPropagatorPtr); }
   WsfSimulation*                 GetSimulation() const override { return nullptr; }
   WsfSpaceMoverBase*             GetSpaceMover() const override { return nullptr; }
   void   InitiateMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const override {}
   void   UpdateMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const override {}
   void   CompleteMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const override {}
   void   CancelMissionEvent(const UtCalendar& aEpoch, const WsfOrbitalEvent& aMissionEvent) const override {}
   bool   Maneuver(const UtCalendar& aEpoch, const UtVec3d& aCommandedDeltaV, UtVec3d& aActualDeltaV) const override;
   double GetAvailableDeltaV() const override;
   double GetRequiredDeltaV(double aDuration) const override;
   bool   PerformStagingOperation() const override;
   std::string GetName() const override;
   double GetManeuverDuration(const UtCalendar& aEpoch, const WsfOrbitalManeuver& aManeuver, double aDeltaV) const override;

   //! This method verifies the mission with which this context was created.
   //!
   //! This driver routine manages the trial execution of the mission sequence,
   //! and reports on the ultimate verification of the mission sequence.
   //!
   //! \param aPreVerifyOp An operation called before each event is verified.
   //! \param aPostVerifyOp An operation called after each event is verified.
   //! \return true if the mission sequence passed verification, false otherwise.
   bool VerifyMission(verify::PreOperation  aPreVerifyOp  = verify::PreOperation{},
                      verify::PostOperation aPostVerifyOp = verify::PostOperation{});

private:
   //! Get the current mission event.
   WsfOrbitalEvent* GetCurrentMissionEvent() const;

   //! Advance to the next mission event.
   WsfOrbitalEvent* AdvanceMission(const UtCalendar& aEpoch) const;

   verify::Status VerifyEvent(WsfOrbitalEvent& aEvent);

   std::unique_ptr<WsfOrbitalManeuvering>     mManeuveringPtr;   //! The maneuvering being used for the mission.
   std::unique_ptr<UtOrbitalPropagatorBase>   mPropagatorPtr;    //! The propagator updated by the mission.
   std::unique_ptr<WsfOrbitalMissionSequence> mMissionEventsPtr; //! The mission sequence.
   std::string mPlatformName; //! The name of the platform that would execute the maneuver.
};

} // namespace space
} // namespace wsf

#endif // WSFSPACEORBITALMISSIONVERIFICATIONCONTEXT_HPP
