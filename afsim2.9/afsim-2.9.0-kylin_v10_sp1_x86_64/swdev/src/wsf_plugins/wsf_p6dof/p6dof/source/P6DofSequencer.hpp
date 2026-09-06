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

#ifndef P6DOFSEQUENCER_H
#define P6DOFSEQUENCER_H

#include <cinttypes>
#include <set>
#include <string>
#include <vector>

#include "P6DofObject.hpp"
#include "UtVec3dX.hpp"

class P6DofScenario;
class P6DofVehicle;
class UtInput;

class P6DofSequencer : public P6DofObject
{
public:
   explicit P6DofSequencer(P6DofScenario* aScenario);
   ~P6DofSequencer() override;

   P6DofSequencer& operator=(const P6DofSequencer& other) = delete;

   P6DofSequencer* Clone() const;

   void SetParentVehicle(P6DofVehicle* aParentVehicle) override;

   bool ProcessInput(UtInput& aInput);
   bool Initialize(int64_t aSimTime_nanosec);

   // Update should be called every frame
   void Update(int64_t aSimTime_nanosec);

   // This adds the prefix to all object names to this
   // sequencer and all of its subobjects
   void AdjustNaming(const std::string& aNamePrefix);

   // This should be called when a sequencer is first started
   void StartUp(int64_t aSimTime_nanosec);

   // This returns true if there are time-based actions pending.
   // aTimeOfEvents_nanosec contains the sim time of the all pending time-based actions
   bool PendingTimeEvents(int64_t aSimTime_nanosec, std::set<int64_t>& aTimeOfEvents_nanosec);

   // This returns true if there if the sequencer was newly activated.
   // It returns false if the sequencer was already activated.
   bool ActivateExternally();

protected:
   P6DofSequencer(const P6DofSequencer& aSrc);

   // Events (can be daisy-chained or triggered in parallel)
   // Events include things such as elapsed flight time, reaching an
   // altitude, dynamic pressure falling below a set level, reaching
   // a downrange distance, sensing a g-level (such as decaying
   // thrust), etc.
   enum SequencerEventType
   {
      Timer,
      LifeTime,
      AboveAlt_ft,
      BelowAlt_ft,
      DynamicPressureAbove_psf,
      DynamicPressureBelow_psf,
      StaticPressureAbove_psf,
      StaticPressureBelow_psf,
      FuelPercentBelow,
      GroundDistance_m,
      NxAbove_g,
      NxBelow_g,
      NyAbove_g,
      NyBelow_g,
      NzAbove_g,
      NzBelow_g,
      ReleasedFromParent,
      ManualInputTrigger,
      ManualInputButton,
      ManualInputButtonReleased,
      BooleanInput,
      NoEvent
   };

   struct SequencerEvent
   {
      SequencerEventType eventType;
      std::string        argumentString;
      double             argumentValue;
      int                argumentInteger;
      size_t             argumentHandle;
      int64_t            argumentNanoTime;
   };

   // Actions (can be daisy-chained or triggered in parallel)
   // Actions include things like jettisoning a stage/object, firing
   // retro thrusters on the separated stage during separation,
   // firing the engine on the current stage, and changing to a new
   // aero mode (typically by name) to represent the new aero mode
   // without the previous stage.
   enum SequencerActionType
   {
      NoAction,
      JettisonObject,
      JettisonSelf,
      IgniteEngine,
      IgniteSubObject,
      IgniteSelfObject,
      ShutdownSubObject,
      ShutdownEngine,
      TerminateThrust,
      ChangeAeroMode,
      EnableSizeFactor,
      SetGraphicalModel,
      ActivateSubobjectSequencer,
      EnableControls,
      DisableControls
   };

   struct SequencerAction
   {
      SequencerActionType actionType;
      std::string         argumentString;
      std::string         argumentSecondString;
      double              argumentValue;
      UtVec3dX            argumentVector;
   };

   // This is used by TestForActivation to set
   // the various counts/flags used for working
   // with the 'all events' setting
   void SetAllEventCounts();

   // This is used by TestForActivation to determine
   // if activation should occur, based on the
   // 'all events' setting
   void CheckForAllEventActivation();

   // This will test the sequencer and activate it
   // and perform action(s) if appropriate. It returns
   // the time (sim_time) of activation
   int64_t TestForActivation(int64_t aSimTime_nanosec);

   int64_t GetTimerTime() const;

   int64_t mLastSimTime_nanosec;
   int64_t mElapsedTimer_nanosec;

   std::vector<SequencerEvent*>  mEventList;
   std::vector<SequencerAction*> mActionList;
   std::vector<P6DofSequencer*>  mSubSequencerList;

   bool mActivated;
   bool mAllEvents;

   size_t mAllEventNeededCount;
   size_t mAllEventCurrentCount;

   bool mExternalActivation;
};
#endif
