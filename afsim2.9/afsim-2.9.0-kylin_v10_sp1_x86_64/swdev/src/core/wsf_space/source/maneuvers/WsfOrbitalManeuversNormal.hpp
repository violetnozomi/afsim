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

#ifndef WSFORBITALMANEUVERSNORMAL_HPP
#define WSFORBITALMANEUVERSNORMAL_HPP

#include "wsf_space_export.h"

class UtCalendar;
class UtInput;
#include "UtUnitTypes.hpp"
#include "UtVec3.hpp"
class WsfOrbitalEvent;
#include "WsfOrbitalManeuver.hpp"
#include "WsfStringId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalEvent).
namespace WsfOrbitalManeuvers
{
//! Perform a burn that is normal to the current velocity vector.
class WSF_SPACE_EXPORT Normal : public WsfOrbitalManeuver
{
public:
   explicit Normal(const WsfScenario& aScenario);
   Normal(const WsfScenario& aScenario, const UtSpeedValue& aDeltaV);
   ~Normal() override = default;

   static WsfStringId GetTypeId();
   bool               ProcessInput(UtInput& aInput) override;
   Normal*            Clone() const override { return new Normal(*this); }
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   bool               EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   void               ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                    const UtCalendar&              aEvaluationTime,
                                    const UtOrbitalPropagatorBase& aPropagator,
                                    UtVec3d&                       aDeltaV) const override;
   const char*        GetScriptClassName() const override { return "WsfNormalManeuver"; }

   void         SetManeuverDeltaV(const UtSpeedValue& aDeltaV) { mManeuverDeltaV = aDeltaV; }
   UtSpeedValue GetManeuverDeltaV() const { return mManeuverDeltaV; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

private:
   bool InitializeVariables(const UtCalendar&                        aCurrentTime,
                            const UtCalendar&                        aEvaluationTime,
                            const wsf::space::OrbitalMissionContext& aContext) override;
   union
   {
      UtSpeedValue mManeuverDeltaV;
      double       mFractionalDeltaV;
   };
   bool mAsFraction;
};

} // namespace WsfOrbitalManeuvers

#endif
