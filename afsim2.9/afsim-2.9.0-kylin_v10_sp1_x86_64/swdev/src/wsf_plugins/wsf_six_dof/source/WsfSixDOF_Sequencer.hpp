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

#ifndef WSFSIXDOFSEQUENCER_HPP
#define WSFSIXDOFSEQUENCER_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <set>
#include <string>
#include <vector>

#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtVec3dX.hpp"
#include "WsfSixDOF_Object.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT Sequencer : public Object
{
public:
   Sequencer() = default;

   Sequencer& operator=(const Sequencer& other) = delete;

   Sequencer* Clone() const;

   void SetParentVehicle(Mover* aParentVehicle) override;

   bool ProcessInput(UtInput& aInput);
   bool Initialize(int64_t aSimTime_nanosec);

   std::string GetName() const override { return mObjName; }
   void        SetName(const std::string& aName) override { mObjName = aName; }

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
   Sequencer(const Sequencer& aSrc);

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
      SequencerEventType eventType = SequencerEventType::NoEvent;
      std::string        argumentString;
      double             argumentValue    = 0.0;
      int                argumentInteger  = 0;
      size_t             argumentHandle   = 0;
      int64_t            argumentNanoTime = 0;
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
      ActivateSubobjectSequencer,
      EnableControls,
      DisableControls
   };

   struct SequencerAction
   {
      SequencerActionType actionType = NoAction;
      std::string         argumentString;
      std::string         argumentSecondString;
      double              argumentValue = 0.0;
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

   int64_t mLastSimTime_nanosec  = -1;
   int64_t mElapsedTimer_nanosec = 0;

   std::vector<std::unique_ptr<SequencerEvent>>  mEventList;
   std::vector<std::unique_ptr<SequencerAction>> mActionList;
   std::vector<UtCloneablePtr<Sequencer>>        mSubSequencerList;

   bool mActivated = false;
   bool mAllEvents = false;

   size_t mAllEventNeededCount  = 0;
   size_t mAllEventCurrentCount = 0;

   bool mExternalActivation = false;
};
} // namespace six_dof
} // namespace wsf

#endif
