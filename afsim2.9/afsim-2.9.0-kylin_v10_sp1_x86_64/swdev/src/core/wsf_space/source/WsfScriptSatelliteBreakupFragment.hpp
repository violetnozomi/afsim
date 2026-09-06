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

#ifndef WSFSCRIPTBREAKUPFRAGMENT_HPP
#define WSFSCRIPTBREAKUPFRAGMENT_HPP

#include "wsf_space_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WSF_SPACE_EXPORT WsfScriptSatelliteBreakupFragment : public UtScriptClass
{
public:
   explicit WsfScriptSatelliteBreakupFragment(UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptSatelliteBreakupFragment() override = default;

   void Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(GetLength);
   UT_DECLARE_SCRIPT_METHOD(GetAoverM);
   UT_DECLARE_SCRIPT_METHOD(GetMass);
   UT_DECLARE_SCRIPT_METHOD(GetArea);
   UT_DECLARE_SCRIPT_METHOD(GetDeltaV);
   UT_DECLARE_SCRIPT_METHOD(GetName);
   UT_DECLARE_SCRIPT_METHOD(IsInitialized);
};

#endif // WSFSCRIPTBREAKUPFRAGMENT_HPP
