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

#ifndef WSFWSFORBITALMANEUVERSMATCHVELOCITY_HPP
#define WSFWSFORBITALMANEUVERSMATCHVELOCITY_HPP

#include "wsf_space_export.h"

#include <memory>

class UtCalendar;
#include "UtVec3.hpp"
#include "WsfOrbitalManeuversTargetingCapableManeuver.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalManeuver).
namespace WsfOrbitalManeuvers
{
//! A maneuver executed after a Target maneuver, meant to complete a  Rendezvous maneuver,
//! that matches the velocity of the target platform.
class WSF_SPACE_EXPORT MatchVelocity : public TargetingCapableManeuver
{
public:
   explicit MatchVelocity(const WsfScenario& aScenario);
   MatchVelocity(const WsfScenario& aScenario, const WsfTrackId& aLocalTrackId);
   MatchVelocity(const MatchVelocity& aOther) = default;
   ~MatchVelocity() override                  = default;

   static WsfStringId GetTypeId();
   bool               ProcessInput(UtInput& aInput) override;
   MatchVelocity*     Clone() const override { return new MatchVelocity(*this); }
   void               ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                    const UtCalendar&              aEvaluationTime,
                                    const UtOrbitalPropagatorBase& aPropagator,
                                    UtVec3d&                       aDeltaV) const override;

   const char* GetScriptClassName() const override { return "WsfMatchVelocity"; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;
};

} // namespace WsfOrbitalManeuvers

#endif
