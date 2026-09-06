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

#ifndef WSFSCRIPTGUIDANCECOMPUTERCLASS_HPP
#define WSFSCRIPTGUIDANCECOMPUTERCLASS_HPP

#include "wsf_mil_export.h"

#include "script/WsfScriptProcessorClass.hpp"

//! The script interface 'class' for WsfGuidanceComputer.
class WSF_MIL_EXPORT WsfScriptGuidanceComputerClass : public WsfScriptProcessorClass
{
public:
   WsfScriptGuidanceComputerClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(SelectPhase);
   UT_DECLARE_SCRIPT_METHOD(Phase);
   UT_DECLARE_SCRIPT_METHOD(EjectStage_1);
   UT_DECLARE_SCRIPT_METHOD(EjectStage_2);
   UT_DECLARE_SCRIPT_METHOD(StopEngines);

   UT_DECLARE_SCRIPT_METHOD(ClearCommandedAltitude_1);
   UT_DECLARE_SCRIPT_METHOD(ClearCommandedAltitude_2);

   UT_DECLARE_SCRIPT_METHOD(ClearCommandedFlightPathAngle_1);
   UT_DECLARE_SCRIPT_METHOD(ClearCommandedFlightPathAngle_2);

   UT_DECLARE_SCRIPT_METHOD(ClearCommandedSpeed_1);
   UT_DECLARE_SCRIPT_METHOD(ClearCommandedSpeed_2);

   UT_DECLARE_SCRIPT_METHOD(ClearCommandedThrottle_1);
   UT_DECLARE_SCRIPT_METHOD(ClearCommandedThrottle_2);

   UT_DECLARE_SCRIPT_METHOD(SetAllowRouteFollowing_1);
   UT_DECLARE_SCRIPT_METHOD(SetAllowRouteFollowing_2);

   UT_DECLARE_SCRIPT_METHOD(SetAimpointAltitudeOffset_1);
   UT_DECLARE_SCRIPT_METHOD(SetAimpointAltitudeOffset_2);

   UT_DECLARE_SCRIPT_METHOD(SetAimpointAzimuthOffset_1);
   UT_DECLARE_SCRIPT_METHOD(SetAimpointAzimuthOffset_2);

   UT_DECLARE_SCRIPT_METHOD(SetAimpointRangeOffset_1);
   UT_DECLARE_SCRIPT_METHOD(SetAimpointRangeOffset_2);

   UT_DECLARE_SCRIPT_METHOD(SetAimpointEvaluationInterval_1);
   UT_DECLARE_SCRIPT_METHOD(SetAimpointEvaluationInterval_2);

   UT_DECLARE_SCRIPT_METHOD(SetCommandedAltitude_1);
   UT_DECLARE_SCRIPT_METHOD(SetCommandedAltitude_2);

   UT_DECLARE_SCRIPT_METHOD(SetCommandedAltitudeAGL_1);
   UT_DECLARE_SCRIPT_METHOD(SetCommandedAltitudeAGL_2);

   UT_DECLARE_SCRIPT_METHOD(SetCommandedAzimuthOffset_1);
   UT_DECLARE_SCRIPT_METHOD(SetCommandedAzimuthOffset_2);

   UT_DECLARE_SCRIPT_METHOD(SetCommandedFlightPathAngle_1);
   UT_DECLARE_SCRIPT_METHOD(SetCommandedFlightPathAngle_2);

   UT_DECLARE_SCRIPT_METHOD(SetCommandedMach_1);
   UT_DECLARE_SCRIPT_METHOD(SetCommandedMach_2);

   UT_DECLARE_SCRIPT_METHOD(SetCommandedSpeed_1);
   UT_DECLARE_SCRIPT_METHOD(SetCommandedSpeed_2);

   UT_DECLARE_SCRIPT_METHOD(SetCommandedThrottle_1);
   UT_DECLARE_SCRIPT_METHOD(SetCommandedThrottle_2);

   UT_DECLARE_SCRIPT_METHOD(SetGeeBias_1);
   UT_DECLARE_SCRIPT_METHOD(SetGeeBias_2);

   UT_DECLARE_SCRIPT_METHOD(SetGuidanceDelay_1);
   UT_DECLARE_SCRIPT_METHOD(SetGuidanceDelay_2);

   UT_DECLARE_SCRIPT_METHOD(SetGuidanceTarget_1);
   UT_DECLARE_SCRIPT_METHOD(SetGuidanceTarget_2);

   UT_DECLARE_SCRIPT_METHOD(SetLateralGeeBias_1);
   UT_DECLARE_SCRIPT_METHOD(SetLateralGeeBias_2);

   UT_DECLARE_SCRIPT_METHOD(SetMaximumAscentRate_1);
   UT_DECLARE_SCRIPT_METHOD(SetMaximumAscentRate_2);

   UT_DECLARE_SCRIPT_METHOD(SetMaximumCommandedGees_1);
   UT_DECLARE_SCRIPT_METHOD(SetMaximumCommandedGees_2);

   UT_DECLARE_SCRIPT_METHOD(SetMaximumDescentRate_1);
   UT_DECLARE_SCRIPT_METHOD(SetMaximumDescentRate_2);

   UT_DECLARE_SCRIPT_METHOD(SetMaximumPitchAngle_1);
   UT_DECLARE_SCRIPT_METHOD(SetMaximumPitchAngle_2);

   UT_DECLARE_SCRIPT_METHOD(SetPitchChangeGain_1);
   UT_DECLARE_SCRIPT_METHOD(SetPitchChangeGain_2);

   UT_DECLARE_SCRIPT_METHOD(SetProportionalNavigationGain_1);
   UT_DECLARE_SCRIPT_METHOD(SetProportionalNavigationGain_2);

   UT_DECLARE_SCRIPT_METHOD(SetProportionalNavigationLimitAngle_1);
   UT_DECLARE_SCRIPT_METHOD(SetProportionalNavigationLimitAngle_2);

   UT_DECLARE_SCRIPT_METHOD(SetProportionalNavigationMethod_1);
   UT_DECLARE_SCRIPT_METHOD(SetProportionalNavigationMethod_2);

   UT_DECLARE_SCRIPT_METHOD(SetVelocityPursuitGain_1);
   UT_DECLARE_SCRIPT_METHOD(SetVelocityPursuitGain_2);
};

#endif
