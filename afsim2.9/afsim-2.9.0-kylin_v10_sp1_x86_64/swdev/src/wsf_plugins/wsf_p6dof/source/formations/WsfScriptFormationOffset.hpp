// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTFORMATIONOFFSET_HPP
#define WSFSCRIPTFORMATIONOFFSET_HPP

#include "wsf_p6dof_export.h"

#include "UtScriptBasicTypes.hpp"

class WSF_P6DOF_EXPORT WsfScriptFormationOffset : public UtScriptClass
{
public:
   WsfScriptFormationOffset(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptFormationOffset() override = default;

   void* Create(const UtScriptContext& aContext) override;
   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);

   UT_DECLARE_SCRIPT_METHOD(GetStack);
   UT_DECLARE_SCRIPT_METHOD(GetRight);
   UT_DECLARE_SCRIPT_METHOD(GetAhead);
   UT_DECLARE_SCRIPT_METHOD(GetRelativeBearing);
   UT_DECLARE_SCRIPT_METHOD(GetRange);
   UT_DECLARE_SCRIPT_METHOD(IsWelded);

   UT_DECLARE_SCRIPT_METHOD(SetStack);
   UT_DECLARE_SCRIPT_METHOD(SetRight);
   UT_DECLARE_SCRIPT_METHOD(SetAhead);
   UT_DECLARE_SCRIPT_METHOD(SetRangeAndBearing);
   UT_DECLARE_SCRIPT_METHOD(SetWelded);

   UT_DECLARE_SCRIPT_METHOD(Add);
   UT_DECLARE_SCRIPT_METHOD(Subtract);
};

#endif // WSFSCRIPTFORMATIONOFFSET_HPP
