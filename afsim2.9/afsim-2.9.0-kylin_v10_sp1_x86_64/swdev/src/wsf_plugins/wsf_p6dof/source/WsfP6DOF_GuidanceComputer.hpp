// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFP6DOF_GUIDANCECOMPUTER_HPP
#define WSFP6DOF_GUIDANCECOMPUTER_HPP

#include "wsf_p6dof_export.h"

#include "WsfGuidanceComputer.hpp"
#include "WsfP6DOF_Mover.hpp"

//! This class derives from WsfGuidanceComputer and provides a class representing
//! a guidance computer for steering/influencing a P6DOF Mover. The computer will
//! follow proportional homing guidance to null line-of-sight rates to intercept
//! a track. The track pointer will be "pushed" into this processor from some
//! external source, and extrapolated in time if required. The primary output of
//! the class are constrained steering commands in pitch and yaw, which are pushed
//! into the WsfP6DOF_Mover to control vehicle motion/guidance.

class WSF_P6DOF_EXPORT WsfP6DOF_GuidanceComputer : public WsfGuidanceComputer
{
public:
   WsfP6DOF_GuidanceComputer() = delete;
   explicit WsfP6DOF_GuidanceComputer(WsfScenario& aScenarioPtr);
   ~WsfP6DOF_GuidanceComputer() override = default;
   WsfP6DOF_GuidanceComputer& operator=(const WsfP6DOF_GuidanceComputer& other) = delete;

   WsfProcessor* Clone() const override;

   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;

   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;

   void UpdateGuidance(double aSimTime, double aCurTime, double aEndTime) override;

   bool StopEngines() override;

   void RouteChanged(double aSimTime) override;

   void StageIgnition(double aSimTime, double aEventTime) override;

   void StageBurnout(double aSimTime, double aEventTime) override;

   void StageSeparation(double aSimTime, double aEventTime) override;

   void SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr) override;

protected:
   WsfP6DOF_GuidanceComputer(const WsfP6DOF_GuidanceComputer& aSrc);

   void CheckForPhaseChange(double aSimTime, double aEndTime) override;

   void SelectPhase(double aSimTime, double aChangeTime, WsfStringId aPhaseName) override;

   bool UpdateAimpoint(double aCurTime, const Phase& aPhase) override;

   void UpdateTargetPerception(double aCurTime) override;

   void UpdateWeaponPerception(double aCurTime) override;

   WsfP6DOF_Mover* mP6DofMoverPtr;
};

#endif
