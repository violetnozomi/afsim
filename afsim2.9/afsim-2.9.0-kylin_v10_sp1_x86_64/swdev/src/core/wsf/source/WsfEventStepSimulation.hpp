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

#ifndef WSFEVENTSTEPSIMULATION_HPP
#define WSFEVENTSTEPSIMULATION_HPP

#include "wsf_export.h"

class WsfEventStepSimulationInput;
#include "WsfSimulation.hpp"

//! A specialization of WsfSimulation that implements an event-stepped simulation.
class WSF_EXPORT WsfEventStepSimulation : public WsfSimulation
{
public:
   WsfEventStepSimulation(WsfScenario& aScenario, unsigned int aRunNumber);
   ~WsfEventStepSimulation() override;

   void Initialize() override;

   void Complete(double aSimTime) override;

   bool PlatformDeleted(double aSimTime, WsfPlatform* aOldPlatformPtr) override;

   void SetPartUpdateInterval(double aSimTime, WsfPlatformPart* aPartPtr, double aUpdateInterval) override;

   bool TurnPartOff(double aSimTime, WsfPlatformPart* aPartPtr) override;
   bool TurnPartOn(double aSimTime, WsfPlatformPart* aPartPtr) override;

   void SetRealtime(double aSimTime, bool aIsRealTime) override;

   void WaitForAdvanceTime() override;

protected:
   bool PlatformInitialized(double aSimTime, WsfPlatform* aNewPlatformPtr) override;

   bool PlatformIntroduced(double aSimTime, WsfPlatform* aNewPlatformPtr) override;

   void CreateClock() override;

private:
   const WsfEventStepSimulationInput* mEventStepInput;
   bool                               mPreviouslyBehind;
};

#endif
