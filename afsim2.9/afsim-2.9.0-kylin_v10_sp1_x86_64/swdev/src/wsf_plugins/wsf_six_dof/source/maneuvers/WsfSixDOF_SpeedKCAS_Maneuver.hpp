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

#ifndef WSF_SIXDOF_SPEEDKCASMANEUVER_HPP
#define WSF_SIXDOF_SPEEDKCASMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT SpeedKCAS_Maneuver : public Maneuver
{
public:
   SpeedKCAS_Maneuver();
   SpeedKCAS_Maneuver(const SpeedKCAS_Maneuver& aOther);
   ~SpeedKCAS_Maneuver() override = default;

   SpeedKCAS_Maneuver& operator=(const SpeedKCAS_Maneuver& aOther) = delete;

   SpeedKCAS_Maneuver* Clone() const override { return new SpeedKCAS_Maneuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_SpeedKCAS_Maneuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "SPEED-KCAS"; }

   void   SetSpeed(double aSpeedKCAS) { mSpeed = aSpeedKCAS; }
   double GetSpeed() const { return mSpeed; }

private:
   double mSpeed;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SPEEDKCASMANEUVER_HPP
