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

#ifndef WSFORBITALMANEUVERSCHANGERAAN_INCLINATION_HPP
#define WSFORBITALMANEUVERSCHANGERAAN_INCLINATION_HPP

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
//! A maneuver to change the right ascension of the ascending node (RAAN) and inclination simultaneously.
//! @note This maneuver should be performed using WsfOrbitalManeuver::Condition::cAT_NORTHERN_INTERSECTION
//! or WsfOrbitalManeuver::Condition::cAT_SOUTHERN_INTERSECTION.
class WSF_SPACE_EXPORT ChangeRAAN_Inclination : public WsfOrbitalManeuver
{
public:
   explicit ChangeRAAN_Inclination(const WsfScenario& aScenario);
   ChangeRAAN_Inclination(const WsfScenario& aScenario, const UtAngleValue& aRAAN, const UtAngleValue& aInclination);
   ~ChangeRAAN_Inclination() override = default;

   static WsfStringId GetTypeId();
   bool               ProcessInput(UtInput& aInput) override;
   bool               Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   bool               EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   ChangeRAAN_Inclination* Clone() const override { return new ChangeRAAN_Inclination(*this); }
   bool                    VerifyCondition() const override;
   void                    ComputeDeltaV(const UtCalendar&              aCurrentTime,
                                         const UtCalendar&              aEvaluationTime,
                                         const UtOrbitalPropagatorBase& aPropagator,
                                         UtVec3d&                       aDeltaV) const override;
   const char*             GetScriptClassName() const override { return "WsfChangeRAAN_Inclination"; }

   //! Get the final RAAN for this maneuver.
   UtAngleValue GetRAAN() const { return mRAAN; }

   //! Set the final RAAN for this maneuver.
   void SetRAAN(const UtAngleValue& aRAAN) { mRAAN = aRAAN; }

   //! Get the final inclination for this maneuver.
   UtAngleValue GetInclination() const { return mInclination; }

   //! Set the final inclination for this maneuver.
   void SetInclination(const UtAngleValue& aInclination) { mInclination = aInclination; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

protected:
   bool ValidateRAAN() const;
   bool ValidateInclination() const;

private:
   bool EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator) override;

   UtAngleValue mRAAN;
   UtAngleValue mInclination;
};

} // namespace WsfOrbitalManeuvers

#endif
