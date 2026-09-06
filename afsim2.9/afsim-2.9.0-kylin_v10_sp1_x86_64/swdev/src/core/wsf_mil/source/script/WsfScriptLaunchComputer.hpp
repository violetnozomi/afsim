// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTLAUNCHCOMPUTER_HPP
#define WSFSCRIPTLAUNCHCOMPUTER_HPP

#include "wsf_mil_export.h"

#include "WsfLaunchComputer.hpp"
class UtScript;
class WsfScriptContext;

//! A launch computer whose implementation is provided via script.
class WSF_MIL_EXPORT WsfScriptLaunchComputer : public WsfLaunchComputer
{
public:
   //! The basic type name (needed by weapon_tools and launch computer processor)
   static std::string BaseTypeName() { return "WSF_SCRIPT_LAUNCH_COMPUTER"; }

   WsfScriptLaunchComputer();
   WsfScriptLaunchComputer(WsfScenario& aScenario);
   ~WsfScriptLaunchComputer() override;

   WsfLaunchComputer* Clone() const override;
   bool               Initialize(double aSimTime, WsfWeapon* aWeaponPtr) override;
   bool               ProcessInput(UtInput& aInput) override;

   double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime) override;

protected:
   WsfScriptLaunchComputer(const WsfScriptLaunchComputer& aSrc);
   WsfScriptLaunchComputer& operator=(const WsfScriptLaunchComputer&) = delete;

   bool InitializeTTIData() override;

   WsfScriptContext* mContextPtr;
   UtScript*         mScriptPtr;
};

#endif
