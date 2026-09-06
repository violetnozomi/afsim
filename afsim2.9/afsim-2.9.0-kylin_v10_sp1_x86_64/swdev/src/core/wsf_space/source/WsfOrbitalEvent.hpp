// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALMISSIONEVENT_HPP
#define WSFORBITALMISSIONEVENT_HPP

#include "wsf_space_export.h"

#include "UtCalendar.hpp"
#include "UtCloneablePtr.hpp"
#include "UtException.hpp"
class UtInput;
class UtOrbitalPropagatorBase;
#include "UtScriptClassDefine.hpp"
#include "UtUnitTypes.hpp"
class WsfOrbitalEventVisitor;
#include "WsfSpaceOrbitalPropagatorCondition.hpp"
class WsfSpaceMoverBase;
class WsfScenario;
#include "WsfObject.hpp"
#include "WsfStringId.hpp"
#include "WsfUniqueId.hpp"

namespace wsf
{
namespace space
{
class OrbitalMissionContext;

} // namespace space
} // namespace wsf

//! A Base class for orbital events.  Derived classes must
//! define the "ComputeDeltaV" and "Clone" methods.  For any events that rely on a Condition
//! (see below) to succeed, the "EvaluatePreconditions" method should be implemented.
//! Likewise, for any post-event criteria that must be evaluated (e.g., orbit not
//! intersecting the Earth), the "EvaluatePostConditions" method should be implemented.
//! Orbital events can either be executed with a mover in the context of a simulation,
//! or they can be executed exclusively with an orbital propagator.  In the latter case,
//! the unique id and script accessible base classes are not used.
class WSF_SPACE_EXPORT WsfOrbitalEvent : public WsfUniqueId, public WsfObject
{
public:
   WsfOrbitalEvent(const WsfOrbitalEvent& aSrc);
   WsfOrbitalEvent& operator=(const WsfOrbitalEvent&) = delete;
   ~WsfOrbitalEvent() override                        = default;

   //! Make a copy of this object.
   WsfOrbitalEvent* Clone() const override = 0;

   static WsfOrbitalEvent* Create(const WsfScenario& aScenario, UtInput& aInput, bool aIgnoreSomeExceptions = false);

   bool         ProcessInput(UtInput& aInput) override;
   virtual bool Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext);

   //! Return whether this event can be verified.
   bool CanVerify() const { return mCanVerify; }

   virtual bool VerifyCondition() const;

   virtual bool EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const;

   virtual bool EvaluatePostconditions(const wsf::space::OrbitalMissionContext& aContext) const;

   virtual bool ValidateParameterRanges(const std::string& aExecutingPlatformName) const;

   //! Return whether the event is complete.
   bool IsComplete() const { return mIsComplete; }

   //! Return whether the event is canceled.
   bool IsCanceled() const { return mIsCanceled; }

   //! Mark the event as canceled.
   void SetCanceled() { mIsCanceled = true; }

   bool Execute(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext);

   const char* GetScriptClassName() const override { return "WsfOrbitalEvent"; }

   //! Is the event finite?
   //!@return True if finite; false if impulsive.
   bool IsFinite() const { return mIsFinite; }

   //! Set whether the event is finite.
   void SetFinite(bool aIsFinite) { mIsFinite = aIsFinite; }

   //! Return whether the event has executed at least once (i.e., Execute() has been called).
   bool HasExecuted() const { return mHasExecuted; }

   //! Return the condition for event execution (const).
   const wsf::space::OrbitalPropagatorCondition& GetCondition() const { return *mCondition; }

   //! Return the condition for event execution (non-const).
   wsf::space::OrbitalPropagatorCondition& GetCondition() { return *mCondition; }

   ////! Return the enumerated condition type.
   // Condition::Type GetConditionType() const { return mCondition.GetType(); }

   //! Return the condition string for event execution.
   WsfStringId GetConditionString() const { return mCondition->GetConditionString(); }

   //! Set the condition for execution.
   void SetCondition(std::unique_ptr<wsf::space::OrbitalPropagatorCondition> aConditionPtr)
   {
      mCondition = std::move(aConditionPtr);
   }

   // void SetConditionType(Condition::Type aConditionType) { mCondition.SetType(aConditionType); }

   //! Return the time at which the event will begin executing.
   const UtCalendar& GetStartTime() const { return mStartTime; }

   //! Set the start time to begin event execution.
   void SetStartTime(const UtCalendar& aStartTime) { mStartTime = aStartTime; }

   //! Return the nominal time to execute the event impulsively.
   const UtCalendar& GetEvaluationTime() const { return mEvaluationTime; }

   //! Return the full duration of the event.  If the event is impulsive, this method returns 0.
   UtTimeValue GetDuration() const { return mDuration; }

   void SetDuration(const UtTimeValue& aDuration);
   void SetOnlyDuration(const UtTimeValue& aDuration);

   //! For finite events, return the interval at which the event is to be re-evaluated.
   UtTimeValue GetUpdateInterval() const { return mUpdateInterval; }

   //! Set the update interval for which the event is to be re-evaluated.
   void SetUpdateInterval(const UtTimeValue& aUpdateInterval) { mUpdateInterval = aUpdateInterval; }

   //! Return the current event object (see WsfOrbitalMissionSequence::GetCurrentEvent).
   //! If this event is not a event sequence, "this" is returned.
   virtual const WsfOrbitalEvent* GetCurrentMissionEvent() const { return this; }

   //! Return the current MissionEvent object (see WsfOrbitalMissionSequence::GetCurrentMissionEvent).
   //! If this MissionEvent is not a MissionEvent sequence, "this" is returned.
   virtual WsfOrbitalEvent* GetCurrentMissionEvent() { return this; }

   //! Return the next MissionEvent object in a sequence (see WsfMissionEventSequence::AdvanceMissionEvent).
   //! If this MissionEvent is not a MissionEvent sequence, zero is returned.
   virtual WsfOrbitalEvent* AdvanceMissionEvent(const UtCalendar&                        aEpoch,
                                                const wsf::space::OrbitalMissionContext& aContext);

   //! If AdvanceManevuer cannot properly initialize the next MissionEvent,
   //! this exception is thrown.
   class AdvanceMissionEventException : public UtException
   {
   public:
      AdvanceMissionEventException();
   };

   //! Is this MissionEvent a MissionEvent sequence (WsfMissionEventSequence)?
   virtual bool IsSequence() const { return false; }

   //! Return the minimum required delta-v needed to complete this event; the default is to return 0.0.
   virtual double GetRequiredDeltaV(const UtOrbitalPropagatorBase& aPropagator) const { return 0.0; }

   //! Get the delta V expended to execute this event; the default is to return 0.0.
   virtual double GetDeltaV() const { return 0.0; }

   //! For finite events, return the next time the event should be executed.
   virtual bool GetNextExecutionTime(const UtCalendar&              aCurrentTime,
                                     const UtOrbitalPropagatorBase& aPropagator,
                                     UtCalendar&                    aNextExecutionTime);

   virtual void Accept(WsfOrbitalEventVisitor* aVisitorPtr) = 0;

protected:
   explicit WsfOrbitalEvent(const WsfScenario& aScenario);

   virtual bool InitializeVariables(const UtCalendar&                        aCurrentTime,
                                    const UtCalendar&                        aEvaluationTime,
                                    const wsf::space::OrbitalMissionContext& aContext);

   virtual bool InitializeTimes(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext);

   //! Perform any necessary event processing.  For example, in the case of orbital maneuvers,
   //! the actual Maneuver(...) method is called and delta-V computations are performed.
   //!@param aCurrentTime The current time to be used in the event execution.
   //!@param aContext The orbital mission context in which this event is executing.
   //!@note This is a template method called from within Execute(...).
   virtual bool ExecuteEvent(const UtCalendar& aCurrentTime, wsf::space::OrbitalMissionContext& aContext) = 0;

   virtual bool EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator);

   static WsfOrbitalEvent* Create(const WsfScenario& aScenario, WsfStringId aType);

   ut::CloneablePtr<wsf::space::OrbitalPropagatorCondition> mCondition{};
   bool                                                     mIsFinite;
   bool                                                     mHasExecuted;
   bool                                                     mIsComplete;
   bool                                                     mIsCanceled;
   bool                                                     mDebug;
   bool               mCanVerify; //! Can this event be verified without running in a simulation?
   UtCalendar         mStartTime;
   UtCalendar         mEvaluationTime;
   UtTimeValue        mDuration;
   UtTimeValue        mUpdateInterval;
   const WsfScenario& mScenario;
};

#endif
