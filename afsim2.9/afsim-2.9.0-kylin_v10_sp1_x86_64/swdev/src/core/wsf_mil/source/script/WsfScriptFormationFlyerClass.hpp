// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTFORMATIONFLYERCLASS_HPP
#define WSFSCRIPTFORMATIONFLYERCLASS_HPP

#include "wsf_mil_export.h"

#include "script/WsfScriptMoverClass.hpp"

class UtScriptTypes;

//! The script interface 'class' for WsfFormationFlyer.
class WSF_MIL_EXPORT WsfScriptFormationFlyerClass : public WsfScriptMoverClass
{
public:
   WsfScriptFormationFlyerClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptFormationFlyerClass() override;

   UT_DECLARE_SCRIPT_METHOD(SetLeadAircraft);
   UT_DECLARE_SCRIPT_METHOD(SetFormationRollsWithLead);
   UT_DECLARE_SCRIPT_METHOD(SetOffsetFromLeadECS);

   UT_DECLARE_SCRIPT_METHOD(GetLeadAircraft);
   UT_DECLARE_SCRIPT_METHOD(GetFormationRollsWithLead);
   UT_DECLARE_SCRIPT_METHOD(GetOffsetFromLeadECS);
};
#endif
