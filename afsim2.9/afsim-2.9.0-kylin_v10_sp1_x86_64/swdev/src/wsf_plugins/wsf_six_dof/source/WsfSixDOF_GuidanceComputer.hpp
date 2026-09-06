// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSIXDOFGUIDANCECOMPUTER_HPP
#define WSFSIXDOFGUIDANCECOMPUTER_HPP

#include "wsf_six_dof_export.h"

#include "WsfGuidanceComputer.hpp"
#include "WsfSixDOF_Mover.hpp"

namespace wsf
{
namespace six_dof
{
//! This class derives from WsfGuidanceComputer and provides a class representing
//! a guidance computer for steering/influencing a SixDOF Mover. The computer will
//! follow proportional homing guidance to null line-of-sight rates to intercept
//! a track. The track pointer will be "pushed" into this processor from some
//! external source, and extrapolated in time if required. The primary output of
//! the class are constrained steering commands in pitch and yaw, which are pushed
//! into the Mover to control vehicle motion/guidance.

class WSF_SIX_DOF_EXPORT GuidanceComputer : public WsfGuidanceComputer
{
public:
   GuidanceComputer() = delete;
   explicit GuidanceComputer(WsfScenario& aScenarioPtr);
   ~GuidanceComputer()       = default;
   GuidanceComputer& operator=(const GuidanceComputer& other) = delete;

   WsfProcessor* Clone() const override;

   bool Initialize(double aSimTime) override;

   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;

   void UpdateGuidance(double aSimTime, double aCurTime, double aEndTime) override;

   double GetCommandedAltitude() { return WsfGuidanceComputer::GetCurrentPhase().mCommandedAlt; }
   double GetCommandedSpeed() { return WsfGuidanceComputer::GetCurrentPhase().mCommandedSpeed; }
   bool   GetAllowRouteFollowing() { return WsfGuidanceComputer::GetCurrentPhase().mAllowRouteFollowing; }

   bool StopEngines() override;

   void RouteChanged(double aSimTime) override;

   void StageIgnition(double aSimTime, double aEventTime) override;

   void StageBurnout(double aSimTime, double aEventTime) override;

   void StageSeparation(double aSimTime, double aEventTime) override;

protected:
   GuidanceComputer(const GuidanceComputer& aSrc);

   void SelectPhase(double aSimTime, double aChangeTime, WsfStringId aPhaseName) override;

   void SelectWaypoint(double aSimTime, double aChangeTime, unsigned int aNewIndex) override;

   bool UpdateAimpoint(double aCurTime, const Phase& aPhase) override;

   void UpdateTargetPerception(double aCurTime) override;

   void UpdateWeaponPerception(double aCurTime) override;

   void UpdateTerrainFollowing(const Phase& aPhase);

   void UpdateRouteFollowing(const Phase& aPhase);

   void CheckForWaypointChange(double aSimTime, double aEndTime) override;

   Mover* mVehicleMoverPtr = nullptr;
};
} // namespace six_dof
} // namespace wsf


#endif
