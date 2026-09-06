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

#ifndef WSF_SIXDOF_SPEEDMACHMANEUVER_HPP
#define WSF_SIXDOF_SPEEDMACHMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT SpeedMachManeuver : public Maneuver
{
public:
   SpeedMachManeuver();
   SpeedMachManeuver(const SpeedMachManeuver& aOther);
   ~SpeedMachManeuver() override = default;

   SpeedMachManeuver& operator=(const SpeedMachManeuver& aOther) = delete;

   SpeedMachManeuver* Clone() const override { return new SpeedMachManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_SpeedMachManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "SPEED-MACH"; }

   void   SetMach(double aSpeedMach) { mMach = aSpeedMach; }
   double GetMach() const { return mMach; }

private:
   double mMach;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SPEEDMACHMANEUVER_HPP
