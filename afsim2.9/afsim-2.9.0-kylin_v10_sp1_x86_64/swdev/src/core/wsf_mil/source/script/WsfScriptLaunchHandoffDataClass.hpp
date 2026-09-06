// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTLAUNCHHANDOFFDATACLASS_HPP
#define WSFSCRIPTLAUNCHHANDOFFDATACLASS_HPP

#include "wsf_mil_export.h"

#include "UtScriptClass.hpp"

// =================================================================================================
//! The script interface 'class'
// =================================================================================================

class WSF_MIL_EXPORT WsfScriptLaunchHandoffDataClass : public UtScriptClass
{
public:
   WsfScriptLaunchHandoffDataClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aObjectPtr) override;

   // UT_DECLARE_SCRIPT_METHOD(Originator);
};

#endif
