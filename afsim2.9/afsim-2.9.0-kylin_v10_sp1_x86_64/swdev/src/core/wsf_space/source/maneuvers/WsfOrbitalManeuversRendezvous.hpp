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

#ifndef WSFORBITALMANEUVERSRENDEZVOUS_HPP
#define WSFORBITALMANEUVERSRENDEZVOUS_HPP

#include "wsf_space_export.h"

#include <memory>

class UtCalendar;
#include "UtCloneablePtr.hpp"
class UtOrbitalPropagatorBase;
#include "UtUnitTypes.hpp"
#include "UtVec3.hpp"
#include "WsfLibrationPoint.hpp"
class WsfOrbitalEvent;
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfStringId.hpp"
#include "WsfTargetPointOptions.hpp"
#include "maneuvers/WsfOrbitalManeuversMatchVelocity.hpp"
#include "maneuvers/WsfOrbitalManeuversTarget.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalManeuver).
namespace WsfOrbitalManeuvers
{

//! A maneuver that matches orbits with a target satellite by performing a  Target maneuver, followed by
//! a MatchVelocity maneuver.  The computation proceeds with a provided Target::OptimizeOption.
class WSF_SPACE_EXPORT Rendezvous : public WsfOrbitalMissionSequence
{
public:
   explicit Rendezvous(const WsfScenario& aScenario);
   Rendezvous(const WsfScenario&  aScenario,
              const WsfTrackId&   aLocalTrackId,
              OptimizeOption      aOptimizeOption,
              const UtTimeValue&  aMaxTime,
              const UtSpeedValue& aMaxDeltaV = 0.0);
   Rendezvous(const WsfScenario&                    aScenario,
              const wsf::space::TargetPointOptions& aOptions,
              OptimizeOption                        aOptimizeOption,
              const UtTimeValue&                    aMaxTime,
              const UtSpeedValue&                   aMaxDeltaV = 0.0);
   Rendezvous(const WsfScenario&                      aScenario,
              const wsf::space::TargetPointOptions&   aOptions,
              const wsf::space::OrbitalTargetingCost& aOptimizeCost,
              const UtTimeValue&                      aMaxTime,
              const UtSpeedValue&                     aMaxDeltaV = 0.0);
   Rendezvous(const Rendezvous& aSrc);
   Rendezvous& operator=(const Rendezvous&) = delete;
   ~Rendezvous() override                   = default;

   static WsfStringId GetTypeId();
   bool               Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   bool               ProcessInput(UtInput& aInput) override;
   Rendezvous*        Clone() const override { return new Rendezvous(*this); }
   const char*        GetScriptClassName() const override { return "WsfRendezvousManeuver"; }
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
   void SetTargetPoint(std::unique_ptr<wsf::space::OrbitalTargetPoint> aTargetPointPtr);

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   double GetTolerance() const;
   void   SetTolerance(double aTolerance);

   const wsf::space::TargetPointOptions& GetTargetPointOptions() const;

protected:
   const UtOrbitalPropagatorBase* GetTargetPropagator(const wsf::space::OrbitalMissionContext& aContext) const;

private:
   void SetupManevuerPointers();

   Target&        GetTargetManeuver() const;
   MatchVelocity& GetMatchVelocityManeuver() const;

   Target*        mTargetPtr;
   MatchVelocity* mMatchVelocityPtr;
};

} // namespace WsfOrbitalManeuvers

#endif
