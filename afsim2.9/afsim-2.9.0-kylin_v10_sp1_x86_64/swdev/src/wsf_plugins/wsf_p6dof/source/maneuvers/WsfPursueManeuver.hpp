// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPURSUEMANEUVER_HPP
#define WSFPURSUEMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include <string>

#include "WsfRelativeManeuver.hpp"

class WSF_P6DOF_EXPORT WsfPursueManeuver : public WsfRelativeManeuver
{
public:
   WsfPursueManeuver();
   WsfPursueManeuver(const WsfPursueManeuver& aOther);
   WsfPursueManeuver(WsfPursueManeuver&& aOther) = delete;

   ~WsfPursueManeuver() override = default;

   WsfPursueManeuver& operator=(const WsfPursueManeuver& aOther) = delete;
   WsfPursueManeuver& operator=(WsfPursueManeuver&& aOther) = delete;

   WsfPursueManeuver* Clone() const override { return new WsfPursueManeuver{*this}; }

   const char* GetScriptClassName() const override { return "WsfPursueManeuver"; }

   bool        UpdateData() override;
   const char* Type() const override { return "PURSUE"; }

   const std::string& GetTargetPlatformName() const { return mTargetPlatformName; }
   void SetTargetPlatformName(const std::string& aTargetPlatformName) { mTargetPlatformName = aTargetPlatformName; }

   double GetPursuitDistanceMeters() const { return mPursuitDistance_m; }
   void   SetPursuitDistanceMeters(double aDistance_m) { mPursuitDistance_m = aDistance_m; }

private:
   std::string mTargetPlatformName{};
   double      mPursuitDistance_m{0.0}; // Distance ahead of the target
};

#endif // WSFPURSUEMANEUVER_HPP
