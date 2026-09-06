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

#ifndef WSFORBITALMANEUVER_HPP
#define WSFORBITALMANEUVER_HPP

#include "wsf_space_export.h"

class UtCalendar;
class UtInput;
#include "UtVec3.hpp"
class WsfLocalTrack;
#include "WsfOrbitalEvent.hpp"
#include "WsfStringId.hpp"
class WsfTrackId;

//! A Base class for orbital maneuvers.  Derived classes must
//! define the "ComputeDeltaV" and "Clone" methods.  For any maneuvers that rely on a Condition
//! (see below) to succeed, the "EvaluatePreconditions" method should be implemented.
//! Likewise, for any post-maneuver criteria that must be evaluated (e.g., orbit not
//! intersecting the Earth), the "EvaluatePostConditions" method should be implemented.
//! Orbital maneuvers can either be executed with a mover in the context of a simulation,
//! or they can be executed exclusively with an orbital propagator.  In the latter case,
//! the unique id and script accessible base classes are not used.
class WSF_SPACE_EXPORT WsfOrbitalManeuver : public WsfOrbitalEvent
{
public:
   WsfOrbitalManeuver(const WsfOrbitalManeuver& aSrc) = default;
   WsfOrbitalManeuver& operator=(const WsfOrbitalManeuver&) = delete;
   ~WsfOrbitalManeuver() override                           = default;

   //! Make a copy of this object.
   WsfOrbitalManeuver* Clone() const override = 0;

   bool VerifyCondition() const override;

   bool EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;

   bool EvaluatePostconditions(const wsf::space::OrbitalMissionContext& aContext) const override;

   //! Compute the total delta-V needed to complete the associated maneuver.
   //! If this is a finite maneuver this method will be called multiple
   //! times to complete the maneuver.
   //!@param aCurrentTime The current time, which may be different than the
   //! evaluation time in a finite maneuver.  For example, the maneuver may be
   //! scheduled to start before the evaluation time to provide an optimal burn
   //! interval about the given constraint.  Typically this will start at mStartTime
   //! and continue to be evaluated at the update interval of the maneuver.
   //!@param aEvaluationTime The time at which the delta-v is to be computed.
   //! Typically this will be mEvaluationTime.
   //!@param aPropagator The propagator representing the current state of the executor
   //! of the maneuver. When executing in a simulation context, this propagator will
   //! refer to the space mover's propagator. When executing in a verification context,
   //! this propagator will be the propagator performing the trial run of the mission
   //! sequence.
   //!@param aDeltaV The delta-v required to complete the maneuver.
   virtual void ComputeDeltaV(const UtCalendar&              aCurrentTime,
                              const UtCalendar&              aEvaluationTime,
                              const UtOrbitalPropagatorBase& aPropagator,
                              UtVec3d&                       aDeltaV) const {}; // = 0;

   //! Return the name of the script class associated with this class.
   const char* GetScriptClassName() const override { return "WsfOrbitalManeuver"; }

   static WsfStringId GetTypeId();

   //! Get the delta V expended to execute this maneuver.  This value may increment during execution of a finite
   //! maneuver. the full value is only valid when IsComplete() is true.
   double GetDeltaV() const override { return mDeltaV; }

   double GetRequiredDeltaV(const UtOrbitalPropagatorBase& aPropagator) const override;

protected:
   WsfOrbitalManeuver(const WsfScenario& aScenario);

   bool InitializeTimes(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;

   bool ExecuteEvent(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext) override;
   bool EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator) override;

   const UtOrbitalPropagatorBase* GetTargetPropagatorPerception(WsfLocalTrack&                           aLocalTrack,
                                                                const wsf::space::OrbitalMissionContext& aContext) const;
   const UtOrbitalPropagatorBase* GetTargetPropagatorTruth(WsfStringId aTargetName, WsfSimulation* aSimulationPtr) const;

   double mDeltaV;
   double mRemainingDeltaV;
};

#endif
