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

#ifndef WSFSPEEDMACHMANEUVER_HPP
#define WSFSPEEDMACHMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include "WsfManeuver.hpp"

class WSF_P6DOF_EXPORT WsfSpeedMachManeuver : public WsfManeuver
{
public:
   WsfSpeedMachManeuver();
   WsfSpeedMachManeuver(const WsfSpeedMachManeuver& aOther);
   ~WsfSpeedMachManeuver() override = default;

   WsfSpeedMachManeuver& operator=(const WsfSpeedMachManeuver& aOther) = delete;

   WsfSpeedMachManeuver* Clone() const override { return new WsfSpeedMachManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfSpeedMachManeuver"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "SPEED-MACH"; }

   void   SetMach(double aSpeedMach) { mMach = aSpeedMach; }
   double GetMach() const { return mMach; }

private:
   double mMach;
};

#endif // WSFSPEEDMACHMANEUVER_HPP
