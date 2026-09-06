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
*   File: WsfScriptBMAssignmentMessageClass.cpp
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
#include "WsfScriptBMAssignmentMessageClass.hpp"
#include "WsfBMAssignmentMessage.hpp"
#include <UtInput.hpp>
#include <logger.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/util.hpp>
#include <WsfTrackId.hpp>
#include <WsfBMUtils.hpp>

#include <script/WsfScriptDefs.hpp>

const char* WsfScriptBMAssignmentMessageClass::SCRIPT_CLASS_NAME = "WsfBMAssignmentMessage";
const char* WsfScriptBMAssignmentMessageClass::BASE_CLASS_NAME = "WSF_BM_ASSIGNMENT_MESSAGE";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Script Class /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WsfScriptBMAssignmentMessageClass::WsfScriptBMAssignmentMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName(SCRIPT_CLASS_NAME);

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<SetAssignmentTime>());
   AddMethod(ut::make_unique<GetAssignmentTime>());

   AddMethod(ut::make_unique<SetReferenceTrackID>());
   AddMethod(ut::make_unique<GetReferenceTrackID>());

   AddMethod(ut::make_unique<SetMasterTrackID>());
   AddMethod(ut::make_unique<GetMasterTrackID>());

   AddMethod(ut::make_unique<SetAssignedWeapon>());
   AddMethod(ut::make_unique<GetAssignedPlatform>());
   AddMethod(ut::make_unique<GetAssignedWeapon>());

   AddMethod(ut::make_unique<SetInitiatingPlatform>());
   AddMethod(ut::make_unique<GetInitiatingPlatform>());

   AddMethod(ut::make_unique<SetAssigningPlatform>());
   AddMethod(ut::make_unique<GetAssigningPlatform>());

   AddMethod(ut::make_unique<SetSelfDefenseFlag>());
   AddMethod(ut::make_unique<GetSelfDefenseFlag>());

   AddMethod(ut::make_unique<SetMaxFiringTime>());
   AddMethod(ut::make_unique<GetMaxFiringTime>());

   AddMethod(ut::make_unique<SetAssignmentReasonNewAssignment>());
   AddMethod(ut::make_unique<SetAssignmentReasonRetransmit>());
   AddMethod(ut::make_unique<SetAssignmentReasonUpdate>());
   AddMethod(ut::make_unique<SetAssignmentReasonReassignment>());
   AddMethod(ut::make_unique<SetAssignmentReasonCancel>());
   AddMethod(ut::make_unique<IsAssignmentReasonNewAssignment>());
   AddMethod(ut::make_unique<IsAssignmentReasonRetransmit>());
   AddMethod(ut::make_unique<IsAssignmentReasonUpdate>());
   AddMethod(ut::make_unique<IsAssignmentReasonReassignment>());
   AddMethod(ut::make_unique<IsAssignmentReasonCancel>());
   AddMethod(ut::make_unique<GetAssignmentReasonString>());

   AddMethod(ut::make_unique<SetShotDoctrineS1>());
   AddMethod(ut::make_unique<SetShotDoctrineS2>());
   AddMethod(ut::make_unique<SetShotDoctrineSLS>());
   AddMethod(ut::make_unique<IsShotDoctrineS1>());
   AddMethod(ut::make_unique<IsShotDoctrineS2>());
   AddMethod(ut::make_unique<IsShotDoctrineSLS>());
   AddMethod(ut::make_unique<GetShotDoctrineString>());

   AddMethod(ut::make_unique<Print>());
}

WsfScriptBMAssignmentMessageClass::~WsfScriptBMAssignmentMessageClass()
{
}

void* WsfScriptBMAssignmentMessageClass::Create(const UtScriptContext& aContext)
{
   auto generating_plat = WsfScriptContext::GetPLATFORM(aContext);
   return generating_plat ? new WsfBMAssignmentMessage(generating_plat) : new WsfBMAssignmentMessage(WsfScriptContext::GetSIMULATION(aContext));
}

void* WsfScriptBMAssignmentMessageClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMAssignmentMessage*>(aObjectPtr)->Clone();
}

void  WsfScriptBMAssignmentMessageClass::Destroy(void* aObjectPtr)
{
   WsfBMAssignmentMessage* objPtr = reinterpret_cast<WsfBMAssignmentMessage*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetAssignmentTime, 1, "void", "double")
{
   aObjectPtr->SetAssignmentTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetAssignmentTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAssignmentTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetAssignedWeapon, 2, "void", "WsfPlatform, WsfWeapon")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto weapon = reinterpret_cast<WsfWeapon*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->SetAssignedWeapon(platform, weapon);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetAssignedPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetAssignedPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetAssignedWeapon, 0, "WsfWeapon", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfWeapon*>(aObjectPtr->GetAssignedWeapon(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetReferenceTrackID, 1, "void", "WsfTrackId")
{
   auto track_id = reinterpret_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());

   aObjectPtr->SetAssignmentReferenceTrackID(*track_id);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetReferenceTrackID, 0, "WsfTrackId", "")
{
   WsfTrackId* track_id = new WsfTrackId;
   *track_id = aObjectPtr->GetAssignmentReferenceTrackID();
   aReturnVal.SetPointer(new UtScriptRef(track_id, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetMasterTrackID, 1, "void", "WsfTrackId")
{
   auto track_id = reinterpret_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetAssignmentLocalTrackID(*track_id);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetMasterTrackID, 0, "WsfTrackId", "")
{
   WsfTrackId* track_id = new WsfTrackId;
   *track_id = aObjectPtr->GetAssignmentLocalTrackID();

   aReturnVal.SetPointer(new UtScriptRef(track_id, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetInitiatingPlatform, 1, "void", "WsfPlatform")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject()); ;
   aObjectPtr->SetInitiatingPlatform(platform);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetInitiatingPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetInitiatingPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetAssigningPlatform, 1, "void", "WsfPlatform")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetAssigningPlatform(platform);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetAssigningPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetAssigningPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetSelfDefenseFlag, 1, "void", "bool")
{
   aObjectPtr->SetSelfDefenseFlag(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetSelfDefenseFlag, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSelfDefenseFlag());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetMaxFiringTime, 1, "void", "double")
{
   aObjectPtr->SetMaxFiringTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetMaxFiringTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaxFiringTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetAssignmentReasonNewAssignment, 0, "void", "")
{
   aObjectPtr->SetAssignmentReason(il::assignmentMessage::NEW_ASSIGNMENT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetAssignmentReasonRetransmit, 0, "void", "")
{
   aObjectPtr->SetAssignmentReason(il::assignmentMessage::RETRANSMIT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetAssignmentReasonUpdate, 0, "void", "")
{
   aObjectPtr->SetAssignmentReason(il::assignmentMessage::UPDATE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetAssignmentReasonReassignment, 0, "void", "")
{
   aObjectPtr->SetAssignmentReason(il::assignmentMessage::REASSIGNMENT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetAssignmentReasonCancel, 0, "void", "")
{
   aObjectPtr->SetAssignmentReason(il::assignmentMessage::CANCEL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, IsAssignmentReasonNewAssignment, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentReason() == il::assignmentMessage::NEW_ASSIGNMENT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, IsAssignmentReasonRetransmit, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentReason() == il::assignmentMessage::RETRANSMIT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, IsAssignmentReasonUpdate, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentReason() == il::assignmentMessage::UPDATE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, IsAssignmentReasonReassignment, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentReason() == il::assignmentMessage::REASSIGNMENT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, IsAssignmentReasonCancel, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetAssignmentReason() == il::assignmentMessage::CANCEL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetAssignmentReasonString, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAssignmentReasonString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetShotDoctrineS1, 0, "void", "")
{
   aObjectPtr->SetShotDoctrine(il::SHOOT_1);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetShotDoctrineS2, 0, "void", "")
{
   aObjectPtr->SetShotDoctrine(il::SHOOT_2);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, SetShotDoctrineSLS, 0, "void", "")
{
   aObjectPtr->SetShotDoctrine(il::SHOOT_LOOK_SHOOT);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, IsShotDoctrineS1, 0, "bool", "")
{
   aReturnVal.SetBool(il::SHOOT_1 == aObjectPtr->GetShotDoctrine());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, IsShotDoctrineS2, 0, "bool", "")
{
   aReturnVal.SetBool(il::SHOOT_2 == aObjectPtr->GetShotDoctrine());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, IsShotDoctrineSLS, 0, "bool", "")
{
   aReturnVal.SetBool(il::SHOOT_LOOK_SHOOT == aObjectPtr->GetShotDoctrine());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, GetShotDoctrineString, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetshotDoctrineString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentMessageClass, WsfBMAssignmentMessage, Print, 0, "void", "")
{
   aObjectPtr->Print();
}
