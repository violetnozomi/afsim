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

#ifndef WSFORBITALMANEUVERSCHANGEINCLINATION_HPP
#define WSFORBITALMANEUVERSCHANGEINCLINATION_HPP

#include "wsf_space_export.h"

class UtCalendar;
class UtInput;
#include "UtUnitTypes.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfStringId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalEvent).
namespace WsfOrbitalManeuvers
{
//! A maneuver to change the inclination of the orbit.
//! @note This maneuver should be performed using WsfOrbitalManeuver::Condition::cAT_ASCENDING_NODE
//! or  WsfOrbitalManeuver::Condition::cAT_DESCENDING_NODE.
class WSF_SPACE_EXPORT ChangeInclination : public WsfOrbitalManeuver
{
public:
   explicit ChangeInclination(const WsfScenario& aScenario);
   ChangeInclination(const WsfScenario& aScenario, const UtAngleValue& aInclination);
   ~ChangeInclination() override = default;

   static WsfStringId GetTypeId();
   bool               ProcessInput(UtInput& aInput) override;
   bool               EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   ChangeInclination* Clone() const override { return new ChangeInclination(*this); }
   void               ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                    const UtCalendar&              aEvaluationTime,
                                    const UtOrbitalPropagatorBase& aPropagator,
                                    UtVec3d&                       aDeltaV) const override;
   const char*        GetScriptClassName() const override { return "WsfChangeInclination"; }

   //! Get the final inclination of this maneuver.
   UtAngleValue GetInclination() const { return mInclination; }

   //! Set the final inclination of this maneuver.
   void SetInclination(const UtAngleValue& aInclination) { mInclination = aInclination; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

private:
   bool EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator) override;

   UtAngleValue mInclination;
};

} // namespace WsfOrbitalManeuvers

#endif
