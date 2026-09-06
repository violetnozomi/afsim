// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SIXDOF_TURNTOHEADINGMANEUVER_HPP
#define WSF_SIXDOF_TURNTOHEADINGMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfPath.hpp"
#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT TurnToHeadingManeuver : public Maneuver
{
public:
   explicit TurnToHeadingManeuver(double aHeading);
   TurnToHeadingManeuver(const TurnToHeadingManeuver& aOther) = default;
   ~TurnToHeadingManeuver() override                          = default;

   TurnToHeadingManeuver& operator=(const TurnToHeadingManeuver& aOther) = delete;

   TurnToHeadingManeuver* Clone() const override { return new TurnToHeadingManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_TurnToHeadingManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "TURN-TO-HEADING"; }

   //! Get heading in radians.
   double GetHeading() const { return mHeading; }

   void SetHeading(double aHeading);

private:
   void UpdateExitConstraint();

   double mHeading;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_TURNTOHEADINGMANEUVER_HPP
