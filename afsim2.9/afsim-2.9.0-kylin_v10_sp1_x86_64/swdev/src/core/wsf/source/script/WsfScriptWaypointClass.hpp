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

#ifndef WSFSCRIPTWAYPOINTCLASS_HPP
#define WSFSCRIPTWAYPOINTCLASS_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfScriptAuxDataUtil.hpp"

//! The script interface 'class'
class WsfScriptWaypointClass : public UtScriptClass
{
public:
   WsfScriptWaypointClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Latitude);
   UT_DECLARE_SCRIPT_METHOD(Longitude);
   UT_DECLARE_SCRIPT_METHOD(Altitude);
   UT_DECLARE_SCRIPT_METHOD(AltitudeMSL);
   UT_DECLARE_SCRIPT_METHOD(AltitudeAGL);
   UT_DECLARE_SCRIPT_METHOD(SetAltitude);
   UT_DECLARE_SCRIPT_METHOD(SetAltitudePrevious);
   UT_DECLARE_SCRIPT_METHOD(Heading);
   UT_DECLARE_SCRIPT_METHOD(SetHeading);
   UT_DECLARE_SCRIPT_METHOD(Location);
   UT_DECLARE_SCRIPT_METHOD(SetLocation);
   UT_DECLARE_SCRIPT_METHOD(Index);

   UT_DECLARE_SCRIPT_METHOD(Mode);
   UT_DECLARE_SCRIPT_METHOD(SetMode);
   UT_DECLARE_SCRIPT_METHOD(SetGoToLabel);
   UT_DECLARE_SCRIPT_METHOD(GoToLabel);
   UT_DECLARE_SCRIPT_METHOD(SetLabel);
   UT_DECLARE_SCRIPT_METHOD(Label);
   UT_DECLARE_SCRIPT_METHOD(SetCallback_1);
   UT_DECLARE_SCRIPT_METHOD(SetCallback_2);
   UT_DECLARE_SCRIPT_METHOD(SetCallback_3);
   UT_DECLARE_SCRIPT_METHOD(Speed);
   UT_DECLARE_SCRIPT_METHOD(SetSpeed);
   UT_DECLARE_SCRIPT_METHOD(ClimbRate);
   UT_DECLARE_SCRIPT_METHOD(SetClimbRate);
   UT_DECLARE_SCRIPT_METHOD(LinearAcceleration);
   UT_DECLARE_SCRIPT_METHOD(SetLinearAcceleration);
   UT_DECLARE_SCRIPT_METHOD(RadialAcceleration);
   UT_DECLARE_SCRIPT_METHOD(SetRadialAcceleration);

   UT_DECLARE_SCRIPT_METHOD(SetSwitch);
   UT_DECLARE_SCRIPT_METHOD(Switch);
   UT_DECLARE_SCRIPT_METHOD(SetSwitchTime);
   UT_DECLARE_SCRIPT_METHOD(SetSwitchDistance);
   UT_DECLARE_SCRIPT_METHOD(SetTimeToPoint);
   UT_DECLARE_SCRIPT_METHOD(SetEndOfPathOption);

   UT_DECLARE_SCRIPT_METHOD(Create_1);
   UT_DECLARE_SCRIPT_METHOD(Create_2);
   UT_DECLARE_SCRIPT_METHOD(Create_3);

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()
};
#endif
