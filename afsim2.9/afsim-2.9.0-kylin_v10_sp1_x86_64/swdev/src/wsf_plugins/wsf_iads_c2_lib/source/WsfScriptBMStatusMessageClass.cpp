// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfScriptBMStatusMessageClass.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#include "WsfScriptBMStatusMessageClass.hpp"
#include "WsfBMStatusMessage.hpp"
#include <UtInput.hpp>
#include <WsfGeoPoint.hpp>
#include <UtVec3.hpp>
#include <script/WsfScriptContext.hpp>
#include <script/WsfScriptDefs.hpp>

#include <iadsLib/enum.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"

const char* WsfScriptBMStatusMessageClass::SCRIPT_CLASS_NAME = "WsfBMStatusMessage";
const char* WsfScriptBMStatusMessageClass::BASE_CLASS_NAME = "WSF_BM_STATUS_MESSAGE";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Script Class /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WsfScriptBMStatusMessageClass::WsfScriptBMStatusMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName(SCRIPT_CLASS_NAME);

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<SetStatusTime>());
   AddMethod(ut::make_unique<GetStatusTime>());

   AddMethod(ut::make_unique<SetPlatform>());
   AddMethod(ut::make_unique<GetPlatform>());

   AddMethod(ut::make_unique<SetMaxAssignments>());
   AddMethod(ut::make_unique<GetMaxAssignments>());
   AddMethod(ut::make_unique<SetNumAssignments>());
   AddMethod(ut::make_unique<GetNumAssignments>());

   AddMethod(ut::make_unique<SetWeaponStatus>());

   AddMethod(ut::make_unique<SetSystemStatusWhite>());
   AddMethod(ut::make_unique<SetSystemStatusGreen>());
   AddMethod(ut::make_unique<SetSystemStatusYellow>());
   AddMethod(ut::make_unique<SetSystemStatusRed>());
   AddMethod(ut::make_unique<IsSystemStatusWhite>());
   AddMethod(ut::make_unique<IsSystemStatusGreen>());
   AddMethod(ut::make_unique<IsSystemStatusYellow>());
   AddMethod(ut::make_unique<IsSystemStatusRed>());

   AddMethod(ut::make_unique<SetPosition>());
   AddMethod(ut::make_unique<HasPosition>());
   AddMethod(ut::make_unique<GetPosition>());
   AddMethod(ut::make_unique<GetPositionTime>());

   AddMethod(ut::make_unique<SetVelocityECEF>());
   AddMethod(ut::make_unique<HasVelocity>());
   AddMethod(ut::make_unique<GetVelocityECEF>());
}

WsfScriptBMStatusMessageClass::~WsfScriptBMStatusMessageClass()
{
}

void* WsfScriptBMStatusMessageClass::Create(const UtScriptContext& aContext)
{
   auto generating_plat = WsfScriptContext::GetPLATFORM(aContext);
   //return generating_plat ? new WsfBMStatusMessage (generating_plat) : new WsfBMStatusMessage (WsfScriptContext::GetSIMULATION (aContext));
   return generating_plat ? new WsfBMStatusMessage(generating_plat) : new WsfBMStatusMessage(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext)));
}

void* WsfScriptBMStatusMessageClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMStatusMessage*>(aObjectPtr)->Clone();
}

void  WsfScriptBMStatusMessageClass::Destroy(void* aObjectPtr)
{
   WsfBMStatusMessage* objPtr = reinterpret_cast<WsfBMStatusMessage*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetStatusTime, 1, "void", "double")
{
   aObjectPtr->SetStatusTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, GetStatusTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetStatusTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetPlatform, 1, "void", "WsfPlatform")
{
   const WsfPlatform* platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetPlatform(platform);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, GetPlatform, 0, "WsfPlatform", "")
{
   auto platform = aObjectPtr->GetPlatform(SIMULATION);

   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(platform), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetMaxAssignments, 1, "void", "int")
{
   aObjectPtr->SetMaxAssignments(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, GetMaxAssignments, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetMaxAssignments());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetNumAssignments, 1, "void", "int")
{
   aObjectPtr->SetNumAssignments(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, GetNumAssignments, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetNumAssignments());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetWeaponStatus, 2, "void", "WsfPlatform, WsfWeapon")
{
   // parse inputs
   const WsfPlatform* platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   const WsfWeapon* weapon = reinterpret_cast<WsfWeapon*>(aVarArgs[1].GetPointer()->GetAppObject());

   aObjectPtr->SetWeaponStatus(platform, weapon);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetSystemStatusWhite, 0, "void", "")
{
   aObjectPtr->SetSystemStatus(WsfBMStatusMessage::E_SYS_WHITE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetSystemStatusGreen, 0, "void", "")
{
   aObjectPtr->SetSystemStatus(WsfBMStatusMessage::E_SYS_GREEN);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetSystemStatusYellow, 0, "void", "")
{
   aObjectPtr->SetSystemStatus(WsfBMStatusMessage::E_SYS_YELLOW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetSystemStatusRed, 0, "void", "")
{
   aObjectPtr->SetSystemStatus(WsfBMStatusMessage::E_SYS_RED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, IsSystemStatusWhite, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSystemStatus() == WsfBMStatusMessage::E_SYS_WHITE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, IsSystemStatusGreen, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSystemStatus() == WsfBMStatusMessage::E_SYS_GREEN);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, IsSystemStatusYellow, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSystemStatus() == WsfBMStatusMessage::E_SYS_YELLOW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, IsSystemStatusRed, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSystemStatus() == WsfBMStatusMessage::E_SYS_RED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetPosition, 2, "void", "double, WsfGeoPoint")
{
   double position_time = aVarArgs[0].GetDouble();
   WsfGeoPoint* position = reinterpret_cast<WsfGeoPoint*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->SetPosition(position_time, *position);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, HasPosition, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasPosition());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, GetPositionTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPositionTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, GetPosition, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint* returnDataPtr = new WsfGeoPoint;
   *returnDataPtr = aObjectPtr->GetPosition();
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, SetVelocityECEF, 1, "void", "Vec3")
{
   UtVec3d* vel = reinterpret_cast<UtVec3d*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetVelocityECEF(*vel);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, HasVelocity, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasVelocity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMStatusMessageClass, WsfBMStatusMessage, GetVelocityECEF, 0, "Vec3", "")
{
   UtVec3d* returnDataPtr = new UtVec3d;
   *returnDataPtr = aObjectPtr->GetVelocityECEF();
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
