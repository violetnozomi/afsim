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

#ifndef WSFSPEEDKTASMANEUVER_HPP
#define WSFSPEEDKTASMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfSpeedKTAS_Maneuver : public WsfManeuver
{
public:
   WsfSpeedKTAS_Maneuver();
   WsfSpeedKTAS_Maneuver(const WsfSpeedKTAS_Maneuver& aOther);
   ~WsfSpeedKTAS_Maneuver() override = default;

   WsfSpeedKTAS_Maneuver& operator=(const WsfSpeedKTAS_Maneuver& aOther) = delete;

   WsfSpeedKTAS_Maneuver* Clone() const override { return new WsfSpeedKTAS_Maneuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSpeedKTAS_Maneuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "SPEED-KTAS"; }

   void   SetSpeed(double aSpeedKTAS) { mSpeed = aSpeedKTAS; }
   double GetSpeed() const { return mSpeed; }

private:
   double mSpeed;
};

#endif // WSFSPEEDKTASMANEUVER_HPP
