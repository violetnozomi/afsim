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
*   File: WsfScriptBMEvalAssignmentClass.cpp
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

#include "WsfScriptBMEvalAssignmentClass.hpp"
#include "WsfBMEvalAssignment.hpp"
#include "WsfBMTrackMessage.hpp"
#include "WsfBMAssignmentMessage.hpp"
#include <script/WsfScriptDefs.hpp>

#include "UtMemory.hpp"

#include "WsfIADSC2ScenarioExtension.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Script Class /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WsfScriptBMEvalAssignmentClass::WsfScriptBMEvalAssignmentClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfBMEvalAssignment");

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<GetTrack>());
   AddMethod(ut::make_unique<GetDelegation>());
   AddMethod(ut::make_unique<GetAssignment>());
}

WsfScriptBMEvalAssignmentClass::~WsfScriptBMEvalAssignmentClass()
{
}

void* WsfScriptBMEvalAssignmentClass::Create(const UtScriptContext& aContext)
{
   WsfBMEvalAssignment* objPtr = new WsfBMEvalAssignment(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext)));
   return objPtr;
}

void* WsfScriptBMEvalAssignmentClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMEvalAssignment*>(aObjectPtr)->Clone();
}

void  WsfScriptBMEvalAssignmentClass::Destroy(void* aObjectPtr)
{
   WsfBMEvalAssignment* objPtr = reinterpret_cast<WsfBMEvalAssignment*>(aObjectPtr);
   delete objPtr;
}

//UT_DEFINE_SCRIPT_METHOD(WsfScriptBMEvalAssignmentClass, WsfBMEvalAssignment, TrackExists, 0, "bool", "")
//{
//   auto &eval_ptr = aObjectPtr->GetRawAccess ();
//
//   aReturnVal.SetBool (eval_ptr.Track());
//}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMEvalAssignmentClass, WsfBMEvalAssignment, GetTrack, 0, "WsfBMTrackMessage", "")
{
   auto& internal_ptr = aObjectPtr->GetRawAccess().Track();

   if (internal_ptr)
   {
      std::shared_ptr<il::airTargetMessage> atm_copy = std::shared_ptr<il::airTargetMessage>(new il::airTargetMessage(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext))));
      internal_ptr->generateAirTargetMessage(*atm_copy);
      WsfBMTrackMessage* ext_ptr = new WsfBMTrackMessage(WsfScriptContext::GetPLATFORM(aContext), atm_copy);
      aReturnVal.SetPointer(new UtScriptRef(ext_ptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(0, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMEvalAssignmentClass, WsfBMEvalAssignment, GetDelegation, 0, "WsfBMAssignmentMessage", "")
{
   auto& internal_ptr = aObjectPtr->GetRawAccess().DelegateAssignment();

   if (internal_ptr)
   {
      WsfBMAssignmentMessage* ext_ptr = new WsfBMAssignmentMessage(WsfScriptContext::GetPLATFORM(aContext), internal_ptr);
      aReturnVal.SetPointer(new UtScriptRef(ext_ptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(0, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMEvalAssignmentClass, WsfBMEvalAssignment, GetAssignment, 0, "WsfBMAssignmentMessage", "")
{
   auto& internal_ptr = aObjectPtr->GetRawAccess().Assignment();

   if (internal_ptr)
   {
      WsfBMAssignmentMessage* ext_ptr = new WsfBMAssignmentMessage(WsfScriptContext::GetPLATFORM(aContext), internal_ptr);
      aReturnVal.SetPointer(new UtScriptRef(ext_ptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(0, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
   }
}
