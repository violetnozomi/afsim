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

#ifndef WSFSCRIPTTOWEDMOVERCLASS_HPP
#define WSFSCRIPTTOWEDMOVERCLASS_HPP

#include "wsf_mil_export.h"

#include "script/WsfScriptMoverClass.hpp"

class UtScriptClass;
class UtScriptTypes;

class WSF_MIL_EXPORT WsfScriptTowedMoverClass : public WsfScriptMoverClass
{
public:
   WsfScriptTowedMoverClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptTowedMoverClass() override = default;

   UT_DECLARE_SCRIPT_METHOD(SetTowLength);
   UT_DECLARE_SCRIPT_METHOD(SetStartReelInTimeAbsolute);
   UT_DECLARE_SCRIPT_METHOD(SetStartReelInTimeRelative);
   UT_DECLARE_SCRIPT_METHOD(SetReelInTimeAfterDeploymentRelative);
   UT_DECLARE_SCRIPT_METHOD(SetReelInRate);
   UT_DECLARE_SCRIPT_METHOD(SetReelOutRate);

   UT_DECLARE_SCRIPT_METHOD(State);
   UT_DECLARE_SCRIPT_METHOD(State_1);
   UT_DECLARE_SCRIPT_METHOD(ReelInRate);
   UT_DECLARE_SCRIPT_METHOD(ReelInRate_1);
   UT_DECLARE_SCRIPT_METHOD(ReelOutRate);
   UT_DECLARE_SCRIPT_METHOD(ReelOutRate_1);
   UT_DECLARE_SCRIPT_METHOD(StartReelInTime);
   UT_DECLARE_SCRIPT_METHOD(StartReelInTime_1);
   UT_DECLARE_SCRIPT_METHOD(ReelInTimeAfterDeploymentRelative);
   UT_DECLARE_SCRIPT_METHOD(ReelInTimeAfterDeploymentRelative_1);

   UT_DECLARE_SCRIPT_METHOD(Distance);
};
#endif
