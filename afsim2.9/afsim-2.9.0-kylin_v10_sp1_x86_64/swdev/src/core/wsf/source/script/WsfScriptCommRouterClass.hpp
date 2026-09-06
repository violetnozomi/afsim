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

#ifndef WSFSCRIPTCOMMROUTERCLASS_HPP
#define WSFSCRIPTCOMMROUTERCLASS_HPP

#include "wsf_export.h"

#include "script/WsfScriptPlatformPartClass.hpp"

//! Define script methods for WsfCommRouter
class WSF_EXPORT WsfScriptCommRouterClass : public WsfScriptPlatformPartClass
{
public:
   WsfScriptCommRouterClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(IsA_TypeOf);
   UT_DECLARE_SCRIPT_METHOD(TurnOff);
   UT_DECLARE_SCRIPT_METHOD(TurnOn);

   UT_DECLARE_SCRIPT_METHOD(GetMedium);

   UT_DECLARE_SCRIPT_METHOD(HasGateway);
   UT_DECLARE_SCRIPT_METHOD(GetGateway);
   UT_DECLARE_SCRIPT_METHOD(SetGateway);

   UT_DECLARE_SCRIPT_METHOD(GetHopLimit);
   UT_DECLARE_SCRIPT_METHOD(SetHopLimit);

   UT_DECLARE_SCRIPT_METHOD(HasInterface);
   UT_DECLARE_SCRIPT_METHOD(AddInterface);
   UT_DECLARE_SCRIPT_METHOD(RemoveInterface);
   UT_DECLARE_SCRIPT_METHOD(GetInterfaces);
};

#endif
