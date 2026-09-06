// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptFormationFlyerClass.hpp"

#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfFormationFlyer.hpp"

WsfScriptFormationFlyerClass::WsfScriptFormationFlyerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMoverClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfFormationFlyer");

   AddMethod(ut::make_unique<SetLeadAircraft>());
   AddMethod(ut::make_unique<SetFormationRollsWithLead>());
   AddMethod(ut::make_unique<SetOffsetFromLeadECS>());

   AddMethod(ut::make_unique<GetLeadAircraft>());
   AddMethod(ut::make_unique<GetFormationRollsWithLead>());
   AddMethod(ut::make_unique<GetOffsetFromLeadECS>());
}

WsfScriptFormationFlyerClass::~WsfScriptFormationFlyerClass() = default;

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationFlyerClass, WsfFormationFlyer, SetLeadAircraft, 1, "void", "WsfPlatform")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->SetLeadAircraft(platformPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationFlyerClass, WsfFormationFlyer, SetFormationRollsWithLead, 1, "void", "bool")
{
   aObjectPtr->SetFormationRollsWithLead(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationFlyerClass, WsfFormationFlyer, SetOffsetFromLeadECS, 3, "void", "double, double, double")
{
   const double offsetFromLeadECS[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   aObjectPtr->SetOffsetFromLeadECS(offsetFromLeadECS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationFlyerClass, WsfFormationFlyer, GetLeadAircraft, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetLeadAircraft(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationFlyerClass, WsfFormationFlyer, GetFormationRollsWithLead, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetFormationRollsWithLead());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptFormationFlyerClass, WsfFormationFlyer, GetOffsetFromLeadECS, 0, "Vec3", "")
{
   double offsetFromLeadECS[3];
   aObjectPtr->GetOffsetFromLeadECS(offsetFromLeadECS);
   aReturnVal.SetPointer(UtScriptVec3::Create(offsetFromLeadECS));
}
