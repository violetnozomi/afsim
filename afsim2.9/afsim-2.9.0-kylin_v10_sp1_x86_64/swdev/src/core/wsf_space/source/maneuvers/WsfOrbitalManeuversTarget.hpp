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

#ifndef WSFORBITALMANEUVERSTARGET_HPP
#define WSFORBITALMANEUVERSTARGET_HPP

#include "wsf_space_export.h"

#include <memory>

class UtCalendar;
#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtUnitTypes.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalManeuversTargetingCapableManeuver.hpp"
#include "WsfOrbitalTargetingCost.hpp"
class WsfScenario;
#include "WsfStringId.hpp"
#include "WsfTargetPointOptions.hpp"
#include "WsfTrackId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalManeuver).
namespace WsfOrbitalManeuvers
{
//! The option to use in optimization of an Intercept or Rendezvous maneuver.
enum OptimizeOption
{
   cOPTIMIZE_NONE    = 0, //!< There is no attempt to optimize.
   cOPTIMIZE_TIME    = 1, //!< Optimize to execute at earliest possible time (Intercept or rendezvous maneuvers).
   cOPTIMIZE_DELTA_V = 3, //!< Optimize to execute with lowest delta-V, given an execution time window (Intercept or
                          //!< rendezvous maneuvers).
   cOPTIMIZE_COST = 4,    //!< Optimize to execute so that a provided cost function is minimized, given constraints on
                          //!< delta-t and delta-v.
};

//! A maneuver that computes an orbit to intersect another satellite at a point in that satellite's orbit.
//! The computation proceeds with a provided OptimizeOption.
class WSF_SPACE_EXPORT Target : public TargetingCapableManeuver
{
public:
   explicit Target(const WsfScenario& aScenario);
   Target(const WsfScenario&  aScenario,
          const WsfTrackId&   aLocalTrackId,
          OptimizeOption      aOptimizeOption,
          const UtTimeValue&  aMaxTime,
          const UtSpeedValue& aMaxDelta_V = -1.0);
   Target(const WsfScenario&                    aScenario,
          const wsf::space::TargetPointOptions& aOptions,
          OptimizeOption                        aOptimizeOption,
          const UtTimeValue&                    aMaxTime,
          const UtSpeedValue&                   aMaxDelta_V = -1.0);
   Target(const WsfScenario&                      aScenario,
          const wsf::space::TargetPointOptions&   aOptions,
          const wsf::space::OrbitalTargetingCost& aOptimizeCost,
          const UtTimeValue&                      aMaxTime,
          const UtSpeedValue&                     aMaxDelta_V = -1.0);
   Target(const Target& aSrc);
   ~Target() override = default;

   Target& operator=(const Target& aOther) = delete;

   static WsfStringId GetTypeId();
   bool               ProcessInput(UtInput& aInput) override;
   bool               Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;

   const char* GetScriptClassName() const override { return "WsfTargetManeuver"; }

   //! Return the time that the actual intercept will occur.
   const UtCalendar& GetInterceptTime() const { return mInterceptTime; }

   Target* Clone() const override { return new Target(*this); }
   void    ComputeDeltaV(const UtCalendar&              aCurrentTime,
                         const UtCalendar&              aEvaluationTime,
                         const UtOrbitalPropagatorBase& aPropagator,
                         UtVec3d&                       aDeltaV) const override;

   //! Get the delta-v difference between the interceptor and target at the time of intercept.
   //! (used by the MatchVelocity maneuver in a Rendezvous sequence.
   const UtVec3d& GetFinalDeltaV() const { return mFinalDeltaV; }

   //! Set whether this is part of rendezvous maneuver.
   void SetComputeForRendezvous(bool aComputeForRendezvous) { mComputeForRendezvous = aComputeForRendezvous; }

   UtTimeValue                             GetMaximumDeltaT() const { return mMaxTime; }
   UtSpeedValue                            GetMaximumDeltaV() const { return mMaxDeltaV; }
   UtTimeValue                             GetDeltaTime() const { return mDeltaTime; }
   OptimizeOption                          GetOptimizeOption() const { return mOptimizeOption; }
   double                                  GetTolerance() const { return mTolerance; }
   const wsf::space::OrbitalTargetingCost* GetOptimizationCost() const { return mCostPtr.get(); }

   void SetMaximumDeltaT(const UtTimeValue& aMaxDeltaT) { mMaxTime = aMaxDeltaT; }
   void SetMaximumDeltaV(const UtSpeedValue& aMaxDeltaV) { mMaxDeltaV = aMaxDeltaV; }
   void SetOptimizeOption(OptimizeOption aOption) { mOptimizeOption = aOption; }
   void SetDeltaTime(const UtTimeValue& aDeltaTime);
   void SetTolerance(double aTolerance) { mTolerance = aTolerance; }
   void SetOptimizationCost(std::unique_ptr<wsf::space::OrbitalTargetingCost> aCostPtr);

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

protected:
   bool                     OptimizeSolution(const UtOrbitalPropagatorBase& aPropagator);
   bool                     OptimizeNone(const UtOrbitalPropagatorBase& aPropagator);
   UtLambertProblem::Result FixedDtSolve(const UtCalendar&              aEvaluationTime,
                                         const UtOrbitalPropagatorBase& aPropagator,
                                         UtVec3d&                       aDeltaV) const;

   bool                                               mComputeForRendezvous;
   OptimizeOption                                     mOptimizeOption;
   ut::CloneablePtr<wsf::space::OrbitalTargetingCost> mCostPtr;
   UtTimeValue                                        mMaxTime;
   UtTimeValue                                        mDeltaTime;
   UtSpeedValue                                       mMaxDeltaV;
   UtCalendar                                         mInterceptTime;
   mutable UtVec3d                                    mFinalDeltaV;
   double                                             mTolerance{1.0e-9};
};

} // namespace WsfOrbitalManeuvers

#endif
