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

#ifndef WSFSCRIPTMOVERCLASS_HPP
#define WSFSCRIPTMOVERCLASS_HPP

#include "wsf_export.h"

#include "script/WsfScriptPlatformPartClass.hpp"

//! Define script methods for WsfMover.
class WSF_EXPORT WsfScriptMoverClass : public WsfScriptPlatformPartClass
{
public:
   WsfScriptMoverClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(IsA_TypeOf); // static IsA_TypeOf("derived-type", "base-type")

   UT_DECLARE_SCRIPT_METHOD(Create);

   UT_DECLARE_SCRIPT_METHOD(UpdateInterval);
   UT_DECLARE_SCRIPT_METHOD(Route);
   UT_DECLARE_SCRIPT_METHOD(DefaultRoute);
   UT_DECLARE_SCRIPT_METHOD(IsExtrapolating);
   UT_DECLARE_SCRIPT_METHOD(SetMode);
   UT_DECLARE_SCRIPT_METHOD(PropertyInt);
   UT_DECLARE_SCRIPT_METHOD(PropertyDouble);
   UT_DECLARE_SCRIPT_METHOD(PropertyString);
   UT_DECLARE_SCRIPT_METHOD(SetTSPI_FileName);
   UT_DECLARE_SCRIPT_METHOD(BurnedOut);
   UT_DECLARE_SCRIPT_METHOD(TurnOn);
   UT_DECLARE_SCRIPT_METHOD(TurnOff);

   UT_DECLARE_SCRIPT_METHOD(TurnRadius_1);              // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(TurnRadius_2);              // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(MaximumRadialAcceleration); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(MaximumTurnRate);           // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(CornerVelocity);            // NO_DOC | DEPRECATED
};

#endif
