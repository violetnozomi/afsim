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

#ifndef WSF_SIXDOF_HOLDCURRENTSPEEDMANEUVER_HPP
#define WSF_SIXDOF_HOLDCURRENTSPEEDMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT HoldCurrentSpeedManeuver : public Maneuver
{
public:
   HoldCurrentSpeedManeuver()
      : Maneuver()
   {
   }
   HoldCurrentSpeedManeuver(const HoldCurrentSpeedManeuver& aOther)
      : Maneuver(aOther)
   {
   }
   ~HoldCurrentSpeedManeuver() override = default;

   HoldCurrentSpeedManeuver& operator=(const HoldCurrentSpeedManeuver& aOther) = delete;

   HoldCurrentSpeedManeuver* Clone() const override { return new HoldCurrentSpeedManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_HoldCurrentSpeedManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "HOLD-CURRENT-SPEED"; }
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_HOLDCURRENTSPEEDMANEUVER_HPP
