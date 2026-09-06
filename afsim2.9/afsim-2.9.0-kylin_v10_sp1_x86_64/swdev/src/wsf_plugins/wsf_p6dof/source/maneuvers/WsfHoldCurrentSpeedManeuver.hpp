// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFHOLDCURRENTSPEEDMANEUVER_HPP
#define WSFHOLDCURRENTSPEEDMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfHoldCurrentSpeedManeuver : public WsfManeuver
{
public:
   WsfHoldCurrentSpeedManeuver()
      : WsfManeuver()
   {
   }
   WsfHoldCurrentSpeedManeuver(const WsfHoldCurrentSpeedManeuver& aOther)
      : WsfManeuver(aOther)
   {
   }
   ~WsfHoldCurrentSpeedManeuver() override = default;

   WsfHoldCurrentSpeedManeuver& operator=(const WsfHoldCurrentSpeedManeuver& aOther) = delete;

   WsfHoldCurrentSpeedManeuver* Clone() const override { return new WsfHoldCurrentSpeedManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfHoldCurrentSpeedManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "HOLD-CURRENT-SPEED"; }
};

#endif // WSFHOLDCURRENTSPEEDMANEUVER_HPP
