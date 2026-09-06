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

#ifndef WSFSPEEDKCASMANEUVER_HPP
#define WSFSPEEDKCASMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfSpeedKCAS_Maneuver : public WsfManeuver
{
public:
   WsfSpeedKCAS_Maneuver();
   WsfSpeedKCAS_Maneuver(const WsfSpeedKCAS_Maneuver& aOther);
   ~WsfSpeedKCAS_Maneuver() override = default;

   WsfSpeedKCAS_Maneuver& operator=(const WsfSpeedKCAS_Maneuver& aOther) = delete;

   WsfSpeedKCAS_Maneuver* Clone() const override { return new WsfSpeedKCAS_Maneuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSpeedKCAS_Maneuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "SPEED-KCAS"; }

   void   SetSpeed(double aSpeedKCAS) { mSpeed = aSpeedKCAS; }
   double GetSpeed() const { return mSpeed; }

private:
   double mSpeed;
};

#endif // WSFSPEEDKCASMANEUVER_HPP
