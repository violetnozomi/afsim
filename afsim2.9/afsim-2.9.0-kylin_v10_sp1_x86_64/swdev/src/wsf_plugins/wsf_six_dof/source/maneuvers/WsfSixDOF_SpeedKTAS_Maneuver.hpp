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

#ifndef WSF_SIXDOF_SPEEDKTASMANEUVER_HPP
#define WSF_SIXDOF_SPEEDKTASMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT SpeedKTAS_Maneuver : public Maneuver
{
public:
   SpeedKTAS_Maneuver();
   SpeedKTAS_Maneuver(const SpeedKTAS_Maneuver& aOther);
   ~SpeedKTAS_Maneuver() override = default;

   SpeedKTAS_Maneuver& operator=(const SpeedKTAS_Maneuver& aOther) = delete;

   SpeedKTAS_Maneuver* Clone() const override { return new SpeedKTAS_Maneuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSixDOF_SpeedKTAS_Maneuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "SPEED-KTAS"; }

   void   SetSpeed(double aSpeedKTAS) { mSpeed = aSpeedKTAS; }
   double GetSpeed() const { return mSpeed; }

private:
   double mSpeed;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_SPEEDKTASMANEUVER_HPP
