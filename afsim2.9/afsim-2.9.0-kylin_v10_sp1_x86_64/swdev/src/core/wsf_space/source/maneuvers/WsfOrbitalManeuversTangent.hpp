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

#ifndef WSFORBITALMANEUVERSTANGENT_HPP
#define WSFORBITALMANEUVERSTANGENT_HPP

#include "wsf_space_export.h"

#include "UtUnitTypes.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfStringId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalEvent).
namespace WsfOrbitalManeuvers
{
//! Perform a simple maneuver tangent to (along) the direction of the velocity vector.
class WSF_SPACE_EXPORT Tangent : public WsfOrbitalManeuver
{
public:
   explicit Tangent(const WsfScenario& aScenario)
      : WsfOrbitalManeuver(aScenario)
   {
   }

   Tangent(const WsfScenario& aScenario, const UtSpeedValue& aDeltaV);
   ~Tangent() override = default;

   static WsfStringId GetTypeId();
   bool               ProcessInput(UtInput& aInput) override;
   Tangent*           Clone() const override { return new Tangent(*this); }
   bool               EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   bool               EvaluatePostconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   void               ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                    const UtCalendar&              aEvaluationTime,
                                    const UtOrbitalPropagatorBase& aPropagator,
                                    UtVec3d&                       aDeltaV) const override;
   const char*        GetScriptClassName() const override { return "WsfTangentManeuver"; }

   void         SetManeuverDeltaV(const UtSpeedValue& aDeltaV) { mManeuverDeltaV = aDeltaV; }
   UtSpeedValue GetManeuverDeltaV() const { return mManeuverDeltaV; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

private:
   bool   InitializeVariables(const UtCalendar&                        aCurrentTime,
                              const UtCalendar&                        aEvaluationTime,
                              const wsf::space::OrbitalMissionContext& aContext) override;
   double GetSpeed(const UtCalendar& aTime, const UtOrbitalPropagatorBase& aPropagator) const;

   UtSpeedValue mManeuverDeltaV{0.0};
   double       mFractionalDeltaV{0.0};
   double       mInitialSpeed{0.0};
   bool         mAsFraction{false};
};

} // namespace WsfOrbitalManeuvers

#endif
