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

#ifndef WSFROLLRATEMANEUVER_HPP
#define WSFROLLRATEMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfRollRateManeuver : public WsfManeuver
{
public:
   WsfRollRateManeuver();
   WsfRollRateManeuver(const WsfRollRateManeuver& aOther);
   ~WsfRollRateManeuver() override = default;

   WsfRollRateManeuver& operator=(const WsfRollRateManeuver& aOther) = delete;

   WsfRollRateManeuver* Clone() const override { return new WsfRollRateManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfRollRateManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "ROLL-RATE"; }

   //! Get the roll rate in radians per second.
   double GetRate() const { return mRate; }

   //! Set the roll rate in radians per second.
   void SetRate(double aRollRate) { mRate = aRollRate; }

private:
   double mRate;
};

#endif // WSFROLLRATEMANEUVER_HPP
