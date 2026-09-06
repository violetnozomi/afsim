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

#ifndef WSFSCRIPTOBJECTCLASS_HPP
#define WSFSCRIPTOBJECTCLASS_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"

class UtScriptContext;

//! WsfScriptObjectClass is a UtScriptClass that defines the methods in
//! WsfObject.  This makes the methods available for use in script.

class WSF_EXPORT WsfScriptObjectClass : public UtScriptClass
{
public:
   WsfScriptObjectClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~WsfScriptObjectClass() override;

   // See the base class for a description of Create, Clone, and Destroy.

   void*       Create(const UtScriptContext& aInstance) override;
   void*       Clone(void* aObjectPtr) override;
   void        Destroy(void* aObjectPtr) override;
   std::string ToString(void* aObjectPtr) const override;

   // Each of the exported methods are defined as function objects.

   UT_DECLARE_SCRIPT_METHOD(Name);
   UT_DECLARE_SCRIPT_METHOD(Type);
   UT_DECLARE_SCRIPT_METHOD(BaseType);
   UT_DECLARE_SCRIPT_METHOD(IsA_TypeOf);
   UT_DECLARE_SCRIPT_METHOD(ProcessInput);
};

#endif
