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

#ifndef WSF_SIXDOF_PITCHRATEMANEUVER_HPP
#define WSF_SIXDOF_PITCHRATEMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PitchRateManeuver : public Maneuver
{
public:
   PitchRateManeuver();
   PitchRateManeuver(const PitchRateManeuver& aOther);
   ~PitchRateManeuver() override = default;

   PitchRateManeuver& operator=(const PitchRateManeuver& aOther) = delete;

   PitchRateManeuver* Clone() const override { return new PitchRateManeuver{*this}; }

   const char* GetScriptClassName() const override { return "PitchRateManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "PITCH-RATE"; }

   //! Get the pitch rate in radians per second.
   double GetRate() const { return mRate; }

   //! Set the pitch rate in radians per second.
   void SetRate(double aPitchRate) { mRate = aPitchRate; }

private:
   double mRate;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_PITCHRATEMANEUVER_HPP
