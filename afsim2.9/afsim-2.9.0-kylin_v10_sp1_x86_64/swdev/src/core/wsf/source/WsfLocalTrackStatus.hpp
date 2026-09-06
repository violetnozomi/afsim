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

#ifndef WSFLOCALTRACKSTATUS_HPP
#define WSFLOCALTRACKSTATUS_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

class UtScriptClass;
class UtScriptTypes;
class WsfLocalTrack;
#include "WsfTrackId.hpp"
class WsfSimulation;

//! An auxiliary class for storing decision-making data that pertains to a local track.
class WSF_EXPORT WsfLocalTrackStatus
{
public:
   WsfLocalTrackStatus() = default;
   WsfLocalTrackStatus(double aSimTime, WsfSimulation& aSimulation, const WsfLocalTrack* aLocalTrackPtr);
   virtual ~WsfLocalTrackStatus() = default;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   //! @name Local Track ID methods.
   //@{

   //! the track ID of the local track with which this status is associated.
   const WsfTrackId& GetLocalTrackId() const { return mLocalTrackId; }

   //@}

   //! @name Target Name methods.
   //@{

   //! Get the string ID of the target that is associated with the local track.
   WsfStringId GetTargetName() const { return mTargetName; }

   void UpdateTargetName(WsfSimulation& aSimulation, const WsfLocalTrack* aLocalTrackPtr);

   //@}

   //! @name State management methods.
   //@{

   //! Get the state index within the state machine.
   int GetStateIndex() const { return mStateIndex; }

   //! Get the state name that is associated with the above state index (diagnostic use only).
   WsfStringId GetStateName() const { return mStateName; }

   double GetLastStateChangeTime() const { return mLastStateChangeTime; }

   //! Set the state index and name of the current state of the associated local track.
   //! @param aSimTime    The current simulation time.
   //! @param aStateIndex The index of the state within the state machine.
   //! @param aStateName  The name  of the state within the state machine.
   void SetState(double aSimTime, int aStateIndex, WsfStringId aStateName)
   {
      if (aStateIndex != mStateIndex)
      {
         mLastStateChangeTime = aSimTime;
      }
      mStateIndex = aStateIndex;
      mStateName  = aStateName;
   }

   //! Is a transition pending into the current state.
   bool TransitionPending() const { return mTransitionPending; }

   //! Set or clear the 'transition pending' status.
   void SetTransitionPending(bool aTransitionPending) { mTransitionPending = aTransitionPending; }

   //@}

   //! Periodic evaluation control.
   //@{

   unsigned int EvaluationEpoch() const { return mEvaluationEpoch; }
   void         UpdateEvaluationEpoch() { ++mEvaluationEpoch; }

   //@}

private:
   //! The track ID of the local track with which this status is associated.
   WsfTrackId mLocalTrackId;

   //! The time of the last state transition.
   double mLastStateChangeTime;

   //! The index within the state machine.
   int mStateIndex;

   //! The string ID of the state name.
   WsfStringId mStateName;

   //! The string ID of the name of the object associated with the local track.
   WsfStringId mTargetName;

   //! The evaluation epoch.
   unsigned int mEvaluationEpoch;

   //! 'true' if the transition into the current state is pending.
   //! (The exit from the last former state has occurred but the entry into the current state has not.)
   bool mTransitionPending;
};

#endif
