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
*   File: WsfScriptBMAssignmentStatusClass.cpp
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

#include <script/WsfScriptDefs.hpp>
#include "WsfScriptBMAssignmentStatusClass.hpp"
#include "WsfBMAssignmentStatusMessage.hpp"
#include <WsfTrackId.hpp>
#include <WsfBMUtils.hpp>

#include <script/WsfScriptDefs.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"

const char* WsfScriptBMAssignmentStatusClass::SCRIPT_CLASS_NAME = "WsfBMAssignmentStatusMessage";
const char* WsfScriptBMAssignmentStatusClass::BASE_CLASS_NAME = "WSF_BM_ASSIGNMENT_STATUS_MESSAGE";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Script Class /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WsfScriptBMAssignmentStatusClass::WsfScriptBMAssignmentStatusClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName(SCRIPT_CLASS_NAME);

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<SetAckTime>());
   AddMethod(ut::make_unique<GetAckTime>());

   AddMethod(ut::make_unique<SetAssignmentReferenceTrackID>());
   AddMethod(ut::make_unique<GetAssignmentReferenceTrackID>());

   AddMethod(ut::make_unique<SetAssignedWeapon>());
   AddMethod(ut::make_unique<GetAssignedPlatform>());
   AddMethod(ut::make_unique<GetAssignedWeapon>());

   AddMethod(ut::make_unique<SetInitiatingPlatform>());
   AddMethod(ut::make_unique<GetInitiatingPlatform>());

   AddMethod(ut::make_unique<SetAssigningPlatform>());
   AddMethod(ut::make_unique<GetAssigningPlatform>());

   AddMethod(ut::make_unique<SetAssignmentStatusUnacknowledged>());
   AddMethod(ut::make_unique<SetAssignmentStatusWilco>());
   AddMethod(ut::make_unique<SetAssignmentStatusCommitted>());
   AddMethod(ut::make_unique<SetAssignmentStatusCueTAR>());
   AddMethod(ut::make_unique<SetAssignmentStatusCueTTR>());
   AddMethod(ut::make_unique<SetAssignmentStatusTARSearching>());
   AddMethod(ut::make_unique<SetAssignmentStatusTTRSearching>());
   AddMethod(ut::make_unique<SetAssignmentStatusTARTracking>());
   AddMethod(ut::make_unique<SetAssignmentStatusTTRTracking>());
   AddMethod(ut::make_unique<SetAssignmentStatusFiring>());
   AddMethod(ut::make_unique<SetAssignmentStatusMiss>());
   AddMethod(ut::make_unique<SetAssignmentStatusChangedAssignedUnit>());
   AddMethod(ut::make_unique<SetAssignmentStatusDoctrineUpdated>());
   AddMethod(ut::make_unique<SetAssignmentStatusKill>());
   AddMethod(ut::make_unique<SetAssignmentStatusCancelled>());
   AddMethod(ut::make_unique<SetAssignmentStatusHavecoSuccess>());
   AddMethod(ut::make_unique<SetAssignmentStatusHavecoFailure>());
   AddMethod(ut::make_unique<SetAssignmentStatusCantco>());

   AddMethod(ut::make_unique<IsAssignmentStatusUnacknowledged>());
   AddMethod(ut::make_unique<IsAssignmentStatusWilco>());
   AddMethod(ut::make_unique<IsAssignmentStatusCommitted>());
   AddMethod(ut::make_unique<IsAssignmentStatusCueTAR>());
   AddMethod(ut::make_unique<IsAssignmentStatusCueTTR>());
   AddMethod(ut::make_unique<IsAssignmentStatusTARSearching>());
   AddMethod(ut::make_unique<IsAssignmentStatusTTRSearching>());
   AddMethod(ut::make_unique<IsAssignmentStatusTARTracking>());
   AddMethod(ut::make_unique<IsAssignmentStatusTTRTracking>());
   AddMethod(ut::make_unique<IsAssignmentStatusFiring>());
   AddMethod(ut::make_unique<IsAssignmentStatusMiss>());
   AddMethod(ut::make_unique<IsAssignmentStatusChangedAssignedUnit>());
   AddMethod(ut::make_unique<IsAssignmentStatusDoctrineUpdated>());
   AddMethod(ut::make_unique<IsAssignmentStatusKill>());
   AddMethod(ut::make_unique<IsAssignmentStatusCancelled>());
   AddMethod(ut::make_unique<IsAssignmentStatusHavecoSuccess>());
   AddMethod(ut::make_unique<IsAssignmentStatusHavecoFailure>());
   AddMethod(ut::make_unique<IsAssignmentStatusCantco>());

   AddMethod(ut::make_unique<SetSalvosFired>());
   AddMethod(ut::make_unique<GetSalvosFired>());

   AddMethod(ut::make_unique<SetCantcoReason>());
   AddMethod(ut::make_unique<GetCantcoReason>());

   AddMethod(ut::make_unique<SetSystemicCantcoGlobal>());
   AddMethod(ut::make_unique<SetSystemicCantcoLocal>());
   AddMethod(ut::make_unique<IsSystemicCantcoGlobal>());
   AddMethod(ut::make_unique<IsSystemicCantcoLocal>());

   AddMethod(ut::make_unique<SetOverridingPlatform>());
   AddMethod(ut::make_unique<GetOverridingPlatform>());

   AddMethod(ut::make_unique<SetNewlyAssignedWeapon>());
   AddMethod(ut::make_unique<GetNewlyAssignedPlatform>());
   AddMethod(ut::make_unique<GetNewlyAssignedWeapon>());

   AddMethod(ut::make_unique<Print>());
}

WsfScriptBMAssignmentStatusClass::~WsfScriptBMAssignmentStatusClass()
{
}

void* WsfScriptBMAssignmentStatusClass::Create(const UtScriptContext& aContext)
{
   auto generating_plat = WsfScriptContext::GetPLATFORM(aContext);
   return generating_plat ? new WsfBMAssignmentStatusMessage(generating_plat) : new WsfBMAssignmentStatusMessage(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext)));
}

void* WsfScriptBMAssignmentStatusClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMAssignmentStatusMessage*>(aObjectPtr)->Clone();
}

void  WsfScriptBMAssignmentStatusClass::Destroy(void* aObjectPtr)
{
   WsfBMAssignmentStatusMessage* objPtr = reinterpret_cast<WsfBMAssignmentStatusMessage*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAckTime, 1, "void", "double")
{
   aObjectPtr->SetAckTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetAckTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAckTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentReferenceTrackID, 1, "void", "WsfTrackId")
{
   auto track_id = reinterpret_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());

   aObjectPtr->SetAssignmentReferenceTrackID(*track_id);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetAssignmentReferenceTrackID, 0, "WsfTrackId", "")
{
   WsfTrackId* track_id = new WsfTrackId;
   *track_id = aObjectPtr->GetAssignmentReferenceTrackID();

   aReturnVal.SetPointer(new UtScriptRef(track_id, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignedWeapon, 2, "void", "WsfPlatform, WsfWeapon")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto weapon = reinterpret_cast<WsfWeapon*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->SetAssignedWeapon(platform, weapon);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetAssignedPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetAssignedPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointe
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetAssignedWeapon, 0, "WsfWeapon", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfWeapon*>(aObjectPtr->GetAssignedWeapon(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetInitiatingPlatform, 1, "void", "WsfPlatform")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject()); ;
   aObjectPtr->SetInitiatingPlatform(platform);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetInitiatingPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetInitiatingPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssigningPlatform, 1, "void", "WsfPlatform")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetAssigningPlatform(platform);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetAssigningPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetAssigningPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointe
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusUnacknowledged, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::UNACKNOWLEDGED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusWilco, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::WILCO);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusCommitted, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::COMITTED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusCueTAR, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::CUE_TAR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusCueTTR, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::CUE_TTR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusTARSearching, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::TAR_SEARCHING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusTTRSearching, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::TTR_SEARCHING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusTARTracking, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::TAR_TRACKING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusTTRTracking, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::TTR_TRACKING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusFiring, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::FIRING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusMiss, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::MISS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusChangedAssignedUnit, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::CHANGED_ASSIGNED_UNIT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusDoctrineUpdated, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::UPDATED_SHOT_DOCTRINE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusKill, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::KILL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusCancelled, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::CANCELLED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusHavecoSuccess, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::HAVCO_SUCCESS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusHavecoFailure, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::HAVCO_FAILURE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetAssignmentStatusCantco, 0, "void", "")
{
   aObjectPtr->SetAssignmentStatus(il::assignmentACKMessage::CANTCO);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusUnacknowledged, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::UNACKNOWLEDGED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusWilco, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::WILCO);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusCommitted, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::COMITTED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusCueTAR, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::CUE_TAR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusCueTTR, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::CUE_TTR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusTARSearching, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::TAR_SEARCHING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusTTRSearching, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::TTR_SEARCHING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusTARTracking, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::TAR_TRACKING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusTTRTracking, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::TTR_TRACKING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusFiring, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::FIRING);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusMiss, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::MISS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusChangedAssignedUnit, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::CHANGED_ASSIGNED_UNIT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusDoctrineUpdated, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::UPDATED_SHOT_DOCTRINE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusKill, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::KILL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusCancelled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::CANCELLED);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusHavecoSuccess, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::HAVCO_SUCCESS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusHavecoFailure, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::HAVCO_FAILURE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsAssignmentStatusCantco, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentStatus() == il::assignmentACKMessage::CANTCO);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetSalvosFired, 1, "void", "int")
{
   aObjectPtr->SetSalvosFired(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetSalvosFired, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetSalvosFired());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetCantcoReason, 1, "void", "string")
{
   aObjectPtr->SetCantcoReason(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetCantcoReason, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetCantcoReason());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetSystemicCantcoGlobal, 0, "void", "")
{
   aObjectPtr->SetSystemicCantcoScope(il::assignmentACKMessage::GLOBAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetSystemicCantcoLocal, 0, "void", "")
{
   aObjectPtr->SetSystemicCantcoScope(il::assignmentACKMessage::LOCAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsSystemicCantcoGlobal, 0, "bool", "")
{
   il::assignmentACKMessage::eSystemicCantcoScope scope;
   if (aObjectPtr->IsSystemicCantco() && (scope = aObjectPtr->GetSystemicCantcoScope()) == il::assignmentACKMessage::GLOBAL)
   {
      aReturnVal.SetBool(true);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, IsSystemicCantcoLocal, 0, "bool", "")
{
   il::assignmentACKMessage::eSystemicCantcoScope scope;
   if (aObjectPtr->IsSystemicCantco() && (scope = aObjectPtr->GetSystemicCantcoScope()) == il::assignmentACKMessage::LOCAL)
   {
      aReturnVal.SetBool(true);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetOverridingPlatform, 1, "void", "WsfPlatform")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetOverridingPlatform(platform);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetOverridingPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetOverridingPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, SetNewlyAssignedWeapon, 2, "void", "WsfPlatform, WsfWeapon")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto weapon = reinterpret_cast<WsfWeapon*>(aVarArgs[1].GetPointer()->GetAppObject());

   aObjectPtr->SetNewlyAssignedWeapon(platform, weapon);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetNewlyAssignedPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetNewlyAssignedPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, GetNewlyAssignedWeapon, 0, "WsfWeapon", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfWeapon*>(aObjectPtr->GetNewlyAssignedWeapon(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentStatusClass, WsfBMAssignmentStatusMessage, Print, 0, "void", "")
{
   aObjectPtr->Print();
}
