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

#ifndef WSFSCRIPTCOVARIANCECLASS_HPP
#define WSFSCRIPTCOVARIANCECLASS_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"

class WSF_EXPORT WsfScriptCovarianceClass : public UtScriptClass
{
public:
   WsfScriptCovarianceClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~WsfScriptCovarianceClass() override;

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(CenterLocation);
   UT_DECLARE_SCRIPT_METHOD(MajorAxis);
   UT_DECLARE_SCRIPT_METHOD(MinorAxis);
   UT_DECLARE_SCRIPT_METHOD(Bearing);
   UT_DECLARE_SCRIPT_METHOD(Trace);
   UT_DECLARE_SCRIPT_METHOD(CovarianceAtTime);
   UT_DECLARE_SCRIPT_METHOD(Rows);
   UT_DECLARE_SCRIPT_METHOD(Columns);
   UT_DECLARE_SCRIPT_METHOD(Element);
   UT_DECLARE_SCRIPT_METHOD(EllipsoidNED);
};

#endif
