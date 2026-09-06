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
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#include <script/WsfScriptDefs.hpp>
#include <WsfScriptBMAssignmentTrackMessageClass.hpp>
#include <WsfBMAssignmentTrackMessage.hpp>
#include <UtInput.hpp>
#include <logger.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/util.hpp>
#include <WsfTrackId.hpp>
#include <WsfBMUtils.hpp>

#include <script/WsfScriptDefs.hpp>

#include "UtMemory.hpp"

const char* WsfScriptBMAssignmentTrackMessageClass::SCRIPT_CLASS_NAME = "WsfBMAssignmentTrackMessage";
const char* WsfScriptBMAssignmentTrackMessageClass::BASE_CLASS_NAME = "WSF_BM_ASSIGNMENT_TRACK_MESSAGE";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Script Class /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WsfScriptBMAssignmentTrackMessageClass::WsfScriptBMAssignmentTrackMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMessageClass(aClassName, aScriptTypesPtr)
{
   SetClassName(SCRIPT_CLASS_NAME);

   AddMethod(ut::make_unique<SetTrack>());
   AddMethod(ut::make_unique<GetTrack>());

   AddMethod(ut::make_unique<SetAssignedPlatform>());
   AddMethod(ut::make_unique<GetAssignedPlatform>());
}

WsfScriptBMAssignmentTrackMessageClass::~WsfScriptBMAssignmentTrackMessageClass()
{
}

void* WsfScriptBMAssignmentTrackMessageClass::Create(const UtScriptContext& aContext)
{
   auto generating_plat = WsfScriptContext::GetPLATFORM(aContext);
   return generating_plat ? new WsfBMAssignmentTrackMessage(generating_plat) : new WsfBMAssignmentTrackMessage(WsfScriptContext::GetSIMULATION(aContext));
}

void* WsfScriptBMAssignmentTrackMessageClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMAssignmentTrackMessage*>(aObjectPtr)->Clone();
}

void  WsfScriptBMAssignmentTrackMessageClass::Destroy(void* aObjectPtr)
{
   auto* objPtr = reinterpret_cast<WsfBMAssignmentTrackMessage*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentTrackMessageClass, WsfBMAssignmentTrackMessage, SetTrack, 1, "void", "WsfLocalTrack")
{
   auto track = reinterpret_cast<WsfLocalTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetTrack(SIMULATION, *track);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentTrackMessageClass, WsfBMAssignmentTrackMessage, GetTrack, 0, "WsfTrack", "")
{
   WsfTrack* track = new WsfTrack;
   *track = aObjectPtr->GetTrack();
   aReturnVal.SetPointer(new UtScriptRef(track, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentTrackMessageClass, WsfBMAssignmentTrackMessage, SetAssignedPlatform, 1, "void", "WsfPlatform")
{
   auto platform = reinterpret_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetAssignedPlatform(platform);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssignmentTrackMessageClass, WsfBMAssignmentTrackMessage, GetAssignedPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(const_cast<WsfPlatform*>(aObjectPtr->GetAssignedPlatform(SIMULATION)), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));  // we don't own this pointer
}
