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

#ifndef WSFORBITALMANEUVERSCHANGERAAN_HPP
#define WSFORBITALMANEUVERSCHANGERAAN_HPP

#include "wsf_space_export.h"

#include "UtUnitTypes.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfStringId.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeRAAN_Inclination.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalEvent).
namespace WsfOrbitalManeuvers
{
//! A maneuver to change the right ascension of the ascending node (RAAN).
//! @note This maneuver should be performed using WsfOrbitalManeuver::Condition::cAT_NORTHERN_INTERSECTION
//! or WsfOrbitalManeuver::Condition::cAT_SOUTHERN_INTERSECTION.
class WSF_SPACE_EXPORT ChangeRAAN : public ChangeRAAN_Inclination
{
public:
   explicit ChangeRAAN(const WsfScenario& aScenario);
   ChangeRAAN(const WsfScenario& aScenario, const UtAngleValue& aRAAN);
   ~ChangeRAAN() override = default;

   static WsfStringId GetTypeId();
   bool               ProcessInput(UtInput& aInput) override;
   bool               Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   ChangeRAAN*        Clone() const override { return new ChangeRAAN(*this); }
   bool               EvaluatePreconditions(const wsf::space::OrbitalMissionContext& aContext) const override;
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   const char*        GetScriptClassName() const override { return "WsfChangeRAAN"; }
   void               Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

private:
   using ChangeRAAN_Inclination::GetInclination;
   using ChangeRAAN_Inclination::SetInclination;
};

} // namespace WsfOrbitalManeuvers

#endif
