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

#ifndef WSFORBITALMANEUVERSINTERCEPT_HPP
#define WSFORBITALMANEUVERSINTERCEPT_HPP

#include "wsf_space_export.h"

#include <memory>

#include "UtCallbackHolder.hpp"
class UtCalendar;
class UtOrbitalPropagatorBase;
#include "UtUnitTypes.hpp"
#include "UtVec3.hpp"
#include "WsfLibrationPoint.hpp"
class WsfOrbitalEvent;
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfStringId.hpp"
#include "WsfTargetPointOptions.hpp"
#include "maneuvers/WsfOrbitalManeuversTarget.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalManeuver).
namespace WsfOrbitalManeuvers
{
//! A maneuver that matches orbits with a target satellite by performing a  Target maneuver, followed by
//! a CompleteInterceptEvent (simply, an event that executes when the platform completes the intercept).
//! The computation proceeds with a provided Target::OptimizeOption.
class WSF_SPACE_EXPORT Intercept : public WsfOrbitalMissionSequence
{
public:
   explicit Intercept(const WsfScenario& aScenario);
   Intercept(const WsfScenario&  aScenario,
             const WsfTrackId&   aLocalTrackId,
             OptimizeOption      aOptimizeOption,
             const UtTimeValue&  aMaxTime,
             const UtSpeedValue& aMaxDeltaV = 0.0);
   Intercept(const WsfScenario&                    aScenario,
             const wsf::space::TargetPointOptions& aOptions,
             OptimizeOption                        aOptimizeOption,
             const UtTimeValue&                    aMaxTime,
             const UtSpeedValue&                   aMaxDeltaV = 0.0);
   Intercept(const WsfScenario&                      aScenario,
             const wsf::space::TargetPointOptions&   aOptions,
             const wsf::space::OrbitalTargetingCost& aOptimizeCost,
             const UtTimeValue&                      aMaxTime,
             const UtSpeedValue&                     aMaxDeltaV = 0.0);
   Intercept(const Intercept& aSrc);
   Intercept& operator=(const Intercept&) = delete;
   ~Intercept() override;

   static WsfStringId GetTypeId();
   bool               Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   bool               ProcessInput(UtInput& aInput) override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   Intercept*         Clone() const override { return new Intercept(*this); }
   const char*        GetScriptClassName() const override { return "WsfInterceptManeuver"; }
   WsfOrbitalEvent* AdvanceMissionEvent(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   UtVec3d          GetOffset() const;
   UtVec3d          GetOffsetInUnits() const;
   std::string GetOffsetUnit() const;
   void        SetOffset(const UtVec3d& aOffset, wsf::space::OrbitalReferenceFrame aFrame);
   void SetOffsetInUnits(const UtVec3d& aOffset, const std::string& aUnit, wsf::space::OrbitalReferenceFrame aFrame);

   UtVec3d     GetVelocityOffset() const;
   UtVec3d     GetVelocityOffsetInUnits() const;
   std::string GetVelocityOffsetUnit() const;
   void        SetVelocityOffset(const UtVec3d& aOffset, wsf::space::OrbitalReferenceFrame aFrame);
   void SetVelocityOffsetInUnits(const UtVec3d& aOffset, const std::string& aUnit, wsf::space::OrbitalReferenceFrame aFrame);

   UtTimeValue GetTargetOffsetTime() const;
   void        SetTargetOffsetTime(const UtTimeValue& aTimeOffset);

   UtTimeValue GetTargetLagTime() const;
   void        SetTargetLagTime(const UtTimeValue& aLagTime);

   UtTimeValue                             GetMaximumDeltaT() const;
   UtSpeedValue                            GetMaximumDeltaV() const;
   UtTimeValue                             GetDeltaTime() const;
   OptimizeOption                          GetOptimizeOption() const;
   const wsf::space::OrbitalTargetingCost* GetOptimizationCost() const;
   WsfStringId                             GetPlatformName() const;

   void SetMaximumDeltaT(const UtTimeValue& aMaxTime);
   void SetMaximumDeltaV(const UtSpeedValue& aMaxDeltaV);
   void SetDeltaTime(const UtTimeValue& aDeltaTime);
   void SetOptimizeOption(OptimizeOption aOption);
   void SetOptimizationCost(std::unique_ptr<wsf::space::OrbitalTargetingCost> aCostPtr);
   void SetPlatformName(WsfStringId aPlatformName);
   void SetLocalTrackId(const WsfTrackId& aTrackId);
   void SetLibrationPoint(wsf::space::LibrationPoint::System aSystem, wsf::space::LibrationPoint::Point aPoint);
   void SetKinematicStateTarget(const UtVec3d&     aPositionInertial,
                                const std::string& aPositionUnits,
                                const UtVec3d&     aVelocityInertial,
                                const std::string& aVelocityUnits);

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   double GetTolerance() const;
   void   SetTolerance(double aTolerance);

   const wsf::space::TargetPointOptions& GetTargetPointOptions() const;

private:
   class CompleteInterceptEvent : public WsfOrbitalEvent
   {
   public:
      explicit CompleteInterceptEvent(const WsfScenario& aScenario);
      CompleteInterceptEvent(const CompleteInterceptEvent& aSrc);
      ~CompleteInterceptEvent() override = default;
      CompleteInterceptEvent& operator=(const CompleteInterceptEvent&) = delete;

      bool ExecuteEvent(const UtCalendar& aCurrentTime, wsf::space::OrbitalMissionContext& aContext) override
      {
         return true;
      }
      WsfOrbitalEvent* Clone() const { return new CompleteInterceptEvent(*this); }

      void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override {}
   };

   void SetupManeuverPointers();

   Target&                 GetTargetManeuver() const;
   CompleteInterceptEvent& GetCompleteInterceptEvent() const;

   Target*                 mTargetPtr;
   CompleteInterceptEvent* mCompleteInterceptPtr;
};

} // namespace WsfOrbitalManeuvers
#endif
