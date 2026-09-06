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

#ifndef WSF_SIXDOF_ROLLRATEMANEUVER_HPP
#define WSF_SIXDOF_ROLLRATEMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT RollRateManeuver : public Maneuver
{
public:
   RollRateManeuver();
   RollRateManeuver(const RollRateManeuver& aOther);
   ~RollRateManeuver() override = default;

   RollRateManeuver& operator=(const RollRateManeuver& aOther) = delete;

   RollRateManeuver* Clone() const override { return new RollRateManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_RollRateManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "ROLL-RATE"; }

   //! Get the roll rate in radians per second.
   double GetRate() const { return mRate; }

   //! Set the roll rate in radians per second.
   void SetRate(double aRollRate) { mRate = aRollRate; }

private:
   double mRate;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_ROLLRATEMANEUVER_HPP
