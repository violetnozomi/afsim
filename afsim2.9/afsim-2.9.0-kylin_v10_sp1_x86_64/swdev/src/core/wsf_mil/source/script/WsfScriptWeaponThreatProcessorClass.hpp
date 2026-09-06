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

#ifndef WSFSCRIPTWEAPONTHREATPROCESSORCLASS_HPP
#define WSFSCRIPTWEAPONTHREATPROCESSORCLASS_HPP

#include "wsf_mil_export.h"

#include "script/WsfScriptProcessorClass.hpp"

//! Define script methods for WsfProcessor.
class WSF_MIL_EXPORT WsfScriptWeaponThreatProcessorClass : public WsfScriptProcessorClass
{
public:
   WsfScriptWeaponThreatProcessorClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~WsfScriptWeaponThreatProcessorClass() override = default;

   //! Creates a new script reference of a new object
   // static WsfScriptWeaponThreatProcessorClass* sClassPtr;
   // static UtScriptRef* Create(const WsfWeaponThreatProcessor& aObjectPtr);

   void*            Create(const UtScriptContext& aInstance) override;
   void             Destroy(void* aObjectPtr) override;
   void*            Clone(void* aObjectPtr) override;
   UtScriptContext* GetContext(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(QueryDurationSec);
   UT_DECLARE_SCRIPT_METHOD(RunQuerySortProbabilityKillAtT0);
   UT_DECLARE_SCRIPT_METHOD(RunQuerySortProbabilityThreatAtT0);
   UT_DECLARE_SCRIPT_METHOD(RunQuerySortProbabilityKillMax);
   UT_DECLARE_SCRIPT_METHOD(RunQuerySortProbabilityThreatMax);

   UT_DECLARE_SCRIPT_METHOD(ReportCount);
   UT_DECLARE_SCRIPT_METHOD(ThreatReport);
};
#endif
