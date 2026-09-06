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

#ifndef WSFSCRIPTLAUNCHCOMPUTERCLASS_HPP
#define WSFSCRIPTLAUNCHCOMPUTERCLASS_HPP

#include "wsf_mil_export.h"

#include "script/WsfScriptObjectClass.hpp"

//! Define script methods for WsfLaunchComputer.
class WSF_MIL_EXPORT WsfScriptLaunchComputerClass : public WsfScriptObjectClass
{
public:
   WsfScriptLaunchComputerClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Weapon);   // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(Platform); // NO_DOC | DEPRECATED

   UT_DECLARE_SCRIPT_METHOD(HaveResultsFor);
   UT_DECLARE_SCRIPT_METHOD(LaunchTimeIsValid);
   UT_DECLARE_SCRIPT_METHOD(LauncherBearingIsValid);
   UT_DECLARE_SCRIPT_METHOD(LauncherElevationIsValid);
   UT_DECLARE_SCRIPT_METHOD(LoftAngleIsValid);
   UT_DECLARE_SCRIPT_METHOD(BurnTimeIsValid);
   UT_DECLARE_SCRIPT_METHOD(TimeOfFlightIsValid);
   UT_DECLARE_SCRIPT_METHOD(InterceptTimeIsValid);
   UT_DECLARE_SCRIPT_METHOD(InterceptPointIsValid);
   UT_DECLARE_SCRIPT_METHOD(TargetImpactTimeIsValid);
   UT_DECLARE_SCRIPT_METHOD(TargetImpactPointIsValid);
   UT_DECLARE_SCRIPT_METHOD(EndTimeIsValid);
   UT_DECLARE_SCRIPT_METHOD(EndPointIsValid);

   UT_DECLARE_SCRIPT_METHOD(CanIntercept_1);
   UT_DECLARE_SCRIPT_METHOD(CanIntercept_2);
   UT_DECLARE_SCRIPT_METHOD(LaunchTime);
   UT_DECLARE_SCRIPT_METHOD(LauncherBearing);
   UT_DECLARE_SCRIPT_METHOD(LauncherElevation);
   UT_DECLARE_SCRIPT_METHOD(LoftAngle);
   UT_DECLARE_SCRIPT_METHOD(BurnTime);
   UT_DECLARE_SCRIPT_METHOD(TimeOfFlight);
   UT_DECLARE_SCRIPT_METHOD(InterceptTime);
   UT_DECLARE_SCRIPT_METHOD(InterceptPoint);
   UT_DECLARE_SCRIPT_METHOD(TargetImpactTime);
   UT_DECLARE_SCRIPT_METHOD(TargetImpactPoint);
   UT_DECLARE_SCRIPT_METHOD(EndTime);
   UT_DECLARE_SCRIPT_METHOD(EndPoint);

   //! @name Special methods for WsfScriptLaunchComputer.
   //! These methods are only for WsfScriptLaunchComputer so it has a means for defining the
   //! results it has computed. There is nothing to prohibit others from calling them,
   //! but it isn't recommended.
   //@{
   UT_DECLARE_SCRIPT_METHOD(SetInterceptTime);
   UT_DECLARE_SCRIPT_METHOD(SetInterceptPoint);
   UT_DECLARE_SCRIPT_METHOD(SetLaunchTime);
   UT_DECLARE_SCRIPT_METHOD(SetTimeOfFlight);
   UT_DECLARE_SCRIPT_METHOD(SetLoftAngle);
   //@}

   //! @name Special methods for WsfBallisticMissileComputer
   //@{
   UT_DECLARE_SCRIPT_METHOD(ComputeEndPoint);
   UT_DECLARE_SCRIPT_METHOD(ComputeLaunchWindows);
   UT_DECLARE_SCRIPT_METHOD(LaunchWindowCount);
   UT_DECLARE_SCRIPT_METHOD(FirstLaunchTime);
   UT_DECLARE_SCRIPT_METHOD(FirstInterceptTime);
   UT_DECLARE_SCRIPT_METHOD(FirstInterceptPoint);
   UT_DECLARE_SCRIPT_METHOD(LastLaunchTime);
   UT_DECLARE_SCRIPT_METHOD(LastInterceptTime);
   UT_DECLARE_SCRIPT_METHOD(LastInterceptPoint);
   UT_DECLARE_SCRIPT_METHOD(LateralTargetOffset);
   UT_DECLARE_SCRIPT_METHOD(TargetApogeeTime);
   UT_DECLARE_SCRIPT_METHOD(TargetApogeePoint);
   //@}

   //! @name Special methods for WsfAirToAirLaunchComputer
   //@{
   UT_DECLARE_SCRIPT_METHOD(LookupResult);
   //@}

   //! @name Deprecated methods
   //@{
   UT_DECLARE_SCRIPT_METHOD(LastLaunchTime_OLD);     // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(LastInterceptTime_OLD);  // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(LastInterceptPoint_OLD); // NO_DOC | DEPRECATED
                                                     //@}
};

#endif
