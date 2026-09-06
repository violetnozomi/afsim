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

#ifndef WSFORBITALMANEUVERSCHANGEECCENTRICITY_HPP
#define WSFORBITALMANEUVERSCHANGEECCENTRICITY_HPP

#include "wsf_space_export.h"

class UtCalendar;
class UtInput;
#include "UtVec3.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfStringId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalEvent).
namespace WsfOrbitalManeuvers
{
//! A maneuver to change the eccentricity of the orbit.
//! @note The semi-major axis of the orbit will also change.
//! @note If the orbit is not initially circular, this maneuver must be performed using
//! WsfOrbitalManeuver::Condition::cAT_PERIAPSIS or WsfOrbitalManeuver::Condition::cAT_APOAPSIS.
class WSF_SPACE_EXPORT ChangeEccentricity : public WsfOrbitalManeuver
{
public:
   ChangeEccentricity(const WsfScenario& aScenario);
   explicit ChangeEccentricity(const WsfScenario& aScenario, double aEccentricity);
   ChangeEccentricity(const ChangeEccentricity&) = default;
   ~ChangeEccentricity() override                = default;

   static WsfStringId  GetTypeId();
   bool                ProcessInput(UtInput& aInput) override;
   bool                EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   bool                EvaluatePostconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   bool                ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   ChangeEccentricity* Clone() const override { return new ChangeEccentricity(*this); }
   void                ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                     const UtCalendar&              aEvaluationTime,
                                     const UtOrbitalPropagatorBase& aPropagator,
                                     UtVec3d&                       aDeltaV) const override;
   const char*         GetScriptClassName() const override { return "WsfChangeEccentricity"; }

   //! Get the final eccentricity for this maneuver.
   double GetEccentricity() const { return mEccentricity; }

   //! Set the final eccentricity for this maneuver.
   void SetEccentricity(double aEccentricity) { mEccentricity = aEccentricity; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

private:
   double mEccentricity;
};

} // namespace WsfOrbitalManeuvers

#endif
