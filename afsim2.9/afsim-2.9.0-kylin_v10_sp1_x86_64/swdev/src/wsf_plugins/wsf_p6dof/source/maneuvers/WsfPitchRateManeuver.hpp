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

#ifndef WSFPITCHRATEMANEUVER_HPP
#define WSFPITCHRATEMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfPitchRateManeuver : public WsfManeuver
{
public:
   WsfPitchRateManeuver();
   WsfPitchRateManeuver(const WsfPitchRateManeuver& aOther);
   ~WsfPitchRateManeuver() override = default;

   WsfPitchRateManeuver& operator=(const WsfPitchRateManeuver& aOther) = delete;

   WsfPitchRateManeuver* Clone() const override { return new WsfPitchRateManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfPitchRateManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "PITCH-RATE"; }

   //! Get the pitch rate in radians per second.
   double GetRate() const { return mRate; }

   //! Set the pitch rate in radians per second.
   void SetRate(double aPitchRate) { mRate = aPitchRate; }

private:
   double mRate;
};

#endif // WSFPITCHRATEMANEUVER_HPP
