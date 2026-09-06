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

#ifndef WSFDISSCRIPTCLASS_HPP
#define WSFDISSCRIPTCLASS_HPP

#include "wsf_export.h"

#include <string>

#include "UtScriptBasicTypes.hpp"

class WSF_EXPORT WsfDisScriptClass : public UtScriptClass
{
public:
   WsfDisScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfDisScriptClass() override = default;

   bool AddStaticMethodP(std::unique_ptr<InterfaceMethod> aInterfaceMethod)
   {
      return UtScriptClass::AddStaticMethod(std::move(aInterfaceMethod));
   }

   static WsfDisScriptClass* GetInstance() { return mInstancePtr; }

   UT_DECLARE_SCRIPT_METHOD(EntityType_1);
   UT_DECLARE_SCRIPT_METHOD(EntityType_2);

protected:
   static WsfDisScriptClass* mInstancePtr;
};
#endif
