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

#ifndef WSFSCRIPTTHREATREPORTCLASS_HPP
#define WSFSCRIPTTHREATREPORTCLASS_HPP

#include "wsf_mil_export.h"

#include "UtScriptBasicTypes.hpp"

//! Define script methods for WsfThreatReport.
class WSF_MIL_EXPORT WsfScriptThreatReportClass : public UtScriptClass
{
public:
   static const char* GetScriptClassname() { return "WsfThreatReport"; }
   // static const string GetScriptClassnameString() {return string("WsfThreatReport");}

   WsfScriptThreatReportClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~WsfScriptThreatReportClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aObjectPtr) override;
   void* Clone(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(TrackId);
   UT_DECLARE_SCRIPT_METHOD(EntityTypeId);
   UT_DECLARE_SCRIPT_METHOD(WeaponTypeId);
   UT_DECLARE_SCRIPT_METHOD(EntityType);
   UT_DECLARE_SCRIPT_METHOD(WeaponType);
   UT_DECLARE_SCRIPT_METHOD(ProbabilityEnityType);
   UT_DECLARE_SCRIPT_METHOD(ProbabilityKillAtT0);
   UT_DECLARE_SCRIPT_METHOD(ProbabilityThreatAtT0);
   UT_DECLARE_SCRIPT_METHOD(TimeOfProbabilityThreatMax);
   UT_DECLARE_SCRIPT_METHOD(ProbabilityKillMax);
   UT_DECLARE_SCRIPT_METHOD(ProbabilityThreatMax);
   UT_DECLARE_SCRIPT_METHOD(ProbabilityOfDetection);
   UT_DECLARE_SCRIPT_METHOD(ProbabilityWeaponType);
};

#endif
