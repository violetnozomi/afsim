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

#ifndef WSFORBITALMANEUVERSCIRCULARIZE_HPP
#define WSFORBITALMANEUVERSCIRCULARIZE_HPP

#include "wsf_space_export.h"

class UtCalendar;
class WsfOrbitalEvent;
#include "WsfOrbitalManeuver.hpp"
#include "WsfStringId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalEvent).
namespace WsfOrbitalManeuvers
{
//! A maneuver to circularize the orbit at a particular radius in the current orbit.
//! @note This maneuver must be performed using  WsfOrbitalManeuver::Condition::cAT_ASCENDING_RADIUS
//! or WsfOrbitalManeuver::Condition::cAT_DESCENDING_RADIUS.
class WSF_SPACE_EXPORT Circularize : public WsfOrbitalManeuver
{
public:
   Circularize(const WsfScenario& aScenario);
   ~Circularize() override = default;

   static WsfStringId GetTypeId();
   Circularize*       Clone() const override { return new Circularize(*this); }
   bool               VerifyCondition() const override;
   bool               EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   void               ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                    const UtCalendar&              aEvaluationTime,
                                    const UtOrbitalPropagatorBase& aPropagator,
                                    UtVec3d&                       aDeltaV) const override;
   const char*        GetScriptClassName() const override { return "WsfCircularize"; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

private:
   UtLengthValue GetRadius() const;
};

} // namespace WsfOrbitalManeuvers

#endif
