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

#ifndef WSFSCRIPTMESSAGECLASS_HPP
#define WSFSCRIPTMESSAGECLASS_HPP

#include "wsf_export.h"

#include "UtScriptClass.hpp"
#include "WsfScriptAuxDataUtil.hpp"

//! WsfScriptMessageClass is a script class that exposes methods in WsfMessage for use in script.
class WSF_EXPORT WsfScriptMessageClass : public UtScriptClass
{
public:
   WsfScriptMessageClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~WsfScriptMessageClass() override;

   // See the base class for a description of Create, Clone, and Destroy.

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   // Each of the exported methods are defined as function objects.

   UT_DECLARE_SCRIPT_METHOD(Originator);
   UT_DECLARE_SCRIPT_METHOD(SizeInBits);
   UT_DECLARE_SCRIPT_METHOD(SetSizeInBits);
   UT_DECLARE_SCRIPT_METHOD(SizeInBytes);
   UT_DECLARE_SCRIPT_METHOD(SetSizeInBytes);
   UT_DECLARE_SCRIPT_METHOD(Priority);
   UT_DECLARE_SCRIPT_METHOD(SetPriority);
   UT_DECLARE_SCRIPT_METHOD(SerialNumber);
   UT_DECLARE_SCRIPT_METHOD(DataTag);
   UT_DECLARE_SCRIPT_METHOD(SetDataTag);
   UT_DECLARE_SCRIPT_METHOD(CreateDataTag);
   UT_DECLARE_SCRIPT_METHOD(Type);
   UT_DECLARE_SCRIPT_METHOD(SubType);
   UT_DECLARE_SCRIPT_METHOD(SetType);
   UT_DECLARE_SCRIPT_METHOD(SetSubType);
   UT_DECLARE_SCRIPT_METHOD(Destination);
   UT_DECLARE_SCRIPT_METHOD(NextHop);

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()
};

#endif
