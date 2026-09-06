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
*   File: WsfScriptBMCueMessageClass.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#include <WsfScriptBMCueMessageClass.hpp>

#include <script/WsfScriptDefs.hpp>
#include <WsfBMCueMessage.hpp>

#include "UtMemory.hpp"

const char* WsfScriptBMCueMessageClass::SCRIPT_CLASS_NAME = "WsfBMCueMessage";
const char* WsfScriptBMCueMessageClass::BASE_CLASS_NAME = "WSF_BM_CUE_MESSAGE";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Script Class /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WsfScriptBMCueMessageClass::WsfScriptBMCueMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName(SCRIPT_CLASS_NAME);

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<SetCueTime>());
   AddMethod(ut::make_unique<GetCueTime>());

   AddMethod(ut::make_unique<SetReferenceTrackID>());
   AddMethod(ut::make_unique<GetReferenceTrackID>());

   AddMethod(ut::make_unique<SetMasterTrackID>());
   AddMethod(ut::make_unique<GetMasterTrackID>());

   AddMethod(ut::make_unique<SetInitiatingPlatform>());
   AddMethod(ut::make_unique<GetInitiatingPlatform>());

   AddMethod(ut::make_unique<SetCuedSensor>());
   AddMethod(ut::make_unique<GetCuedPlatform>());
   AddMethod(ut::make_unique<GetCuedSensor>());

   AddMethod(ut::make_unique<SetCuedReasonNewCue>());
   AddMethod(ut::make_unique<SetCuedReasonCancelCue>());
   AddMethod(ut::make_unique<IsCancelCue>());

   AddMethod(ut::make_unique<Print>());
}

WsfScriptBMCueMessageClass::~WsfScriptBMCueMessageClass()
{
}

void* WsfScriptBMCueMessageClass::Create(const UtScriptContext& aContext)
{
   auto generating_plat = WsfScriptContext::GetPLATFORM(aContext);
   return generating_plat ? new WsfBMCueMessage(generating_plat) : new WsfBMCueMessage(WsfScriptContext::GetSIMULATION(aContext));
}

void* WsfScriptBMCueMessageClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMCueMessage*>(aObjectPtr)->Clone();
}

void  WsfScriptBMCueMessageClass::Destroy(void* aObjectPtr)
{
   WsfBMCueMessage* objPtr = reinterpret_cast<WsfBMCueMessage*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, SetCueTime, 1, "void", "double")
{
   aObjectPtr->SetCueTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, GetCueTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCueTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, SetReferenceTrackID, 1, "void", "WsfTrackId")
{
   auto track_id = reinterpret_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetReferenceTrackID(*track_id);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, GetReferenceTrackID, 0, "WsfTrackId", "")
{
   WsfTrackId* track_id = new WsfTrackId;
   *track_id = aObjectPtr->GetReferenceTrackID();
   aReturnVal.SetPointer(new UtScriptRef(track_id, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, SetMasterTrackID, 1, "void", "WsfTrackId")
{
   auto track_id = reinterpret_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());

   aObjectPtr->SetLocalTrackID(*track_id);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, GetMasterTrackID, 0, "WsfTrackId", "")
{
   WsfTrackId* track_id = new WsfTrackId;;
   *track_id = aObjectPtr->GetLocalTrackID();

   aReturnVal.SetPointer(new UtScriptRef(track_id, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, SetInitiatingPlatform, 1, "void", "WsfPlatform")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject()); ;
   aObjectPtr->SetInitiatingPlatform(platform);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, GetInitiatingPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetInitiatingPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, SetCuedSensor, 2, "void", "WsfPlatform, WsfSensor")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto sensor = reinterpret_cast<WsfSensor*>(aVarArgs[1].GetPointer()->GetAppObject());

   aObjectPtr->SetCuedSensor(platform, sensor);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, GetCuedPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetCuedPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, GetCuedSensor, 0, "WsfSensor", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfSensor*>(aObjectPtr->GetCuedSensor(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, SetCuedReasonNewCue, 0, "void", "")
{
   aObjectPtr->SetCueReason(il::cueMessage::NEW_CUE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, SetCuedReasonCancelCue, 0, "void", "")
{
   aObjectPtr->SetCueReason(il::cueMessage::CANCEL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, IsCancelCue, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCueReason() == il::cueMessage::CANCEL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMCueMessageClass, WsfBMCueMessage, Print, 0, "void", "")
{
   aObjectPtr->Print();
}
