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

#ifndef WSFFRAMESTEPSIMULATION_HPP
#define WSFFRAMESTEPSIMULATION_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
class WsfFrameStepSimulationInput;
#include "WsfSimulation.hpp"
#include "WsfThreadPool.hpp"
/**
   A specialization of WsfSimulation that implements an frame-based simulation.

   This is just an example of a frame-based (fixed timestep) simulation.  This
   implementation can be used as a guide for implementing a custom frame-based
   simulation.
*/

class WSF_EXPORT WsfFrameStepSimulation : public WsfSimulation
{
public:
   WsfFrameStepSimulation(WsfScenario& aScenario, unsigned int aRunNumber);
   ~WsfFrameStepSimulation() override = default;

   void AddEvent(std::unique_ptr<WsfEvent> aEventPtr) override;

   double AdvanceTime() override;

   double AdvanceTime(double aSimTime) override;

   bool AddPlatform(WsfPlatform* aPlatformPtr) override;

   bool AddPlatform(double aSimTime, WsfPlatform* aPlatformPtr) override;

   void DeletePlatform(double aSimTime, WsfPlatform* aPlatformPtr, bool aDeleteMemory = true) override;

   void Initialize() override;

   void Start() override;

   void Complete(double aSimTime) override;

   void SetRealtime(double aSimTime, bool aIsRealTime) override;

   void WaitForAdvanceTime() override;

   double GetFrameTime() const;

   UtCallbackListN<void(double)> AdvanceFrameObjects;

protected:
   virtual double AdvanceFrame();

   void CreateClock() override;


   bool TurnPartOffP(double aSimTime, WsfPlatformPart* aPartPtr) override;

   bool TurnPartOnP(double aSimTime, WsfPlatformPart* aPartPtr) override;

   const WsfFrameStepSimulationInput* mFrameStepInput;
   //! The number of frames executed.
   //!
   //! This could be an 'int' or an 'unsigned int', but there is a possibility
   //! (albiet remote) that one could overflow the value.  A year has 31,556,926
   //! seconds so a 20 Hz update would give 631,138,520 frames.  Given that a
   //! typical 'int' has a range of 2,147,483,647 (2^31-1), this would give us
   //! a range of about 3.4 years.  Not knowing how this may be used (frame rates,
   //! durations, etc), it was just decided to make this a double.
   double mFrameCount;
   double mNextFrameTime;
   double mTimeUntilNextFrame;

   double mTotalFrameUnderTime;
   double mTotalFrameUnderCount;
   double mTotalFrameOverTime;
   double mTotalFrameOverCount;
   double mSkippedFrameCount;
   double mWorstFrameOverTime;

   std::vector<WsfPlatform*>     mPlatforms;
   std::vector<wsf::comm::Comm*> mComms;
   std::vector<WsfProcessor*>    mProcessors;
   std::vector<WsfSensor*>       mSensors;
};

#endif
