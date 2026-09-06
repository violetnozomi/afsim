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
*   File: WsfScriptDisseminateC2Class.cpp
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
*   Abstract: This is an abstract script base class for all disseminate C2 processor
*     script classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

namespace il
{
   class AssetManagerInterface;
   class BattleManagerInterface;
}

#include "UtMemory.hpp"
#include <UtPluginManager.hpp>

#include "WsfScriptDisseminateC2Class.hpp"
#include <WsfBMDisseminateC2.hpp>
#include <iadsLib/disseminateC2Interface.hpp>
#include <WsfBattleManager.hpp>
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/BattleManagerIface.hpp>

// Messages
#include <WsfBMAssignmentMessage.hpp>
#include <WsfBMAssignmentStatusMessage.hpp>
#include <WsfBMStatusMessage.hpp>
#include <WsfBMTrackMessage.hpp>

#include <script/WsfScriptDefs.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"

const char* WsfScriptDisseminateC2Class::SCRIPT_CLASS_NAME = "WsfDisseminateC2";
const char* WsfScriptDisseminateC2Class::BASE_CLASS_NAME = "WSF_DISSEMINATE_C2";

WsfScriptDisseminateC2Class::WsfScriptDisseminateC2Class(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   AddClassName(WsfScriptDisseminateC2Class::SCRIPT_CLASS_NAME);

   AddMethod(ut::make_unique<SetAssetManager>());
   AddMethod(ut::make_unique<Run>());
   AddMethod(ut::make_unique<ClearOutgoingMessages>());
   AddMethod(ut::make_unique<GetOutgoingAssignmentMessages>());
   AddMethod(ut::make_unique<GetOutgoingAssignmentStatusMessages>());
   AddMethod(ut::make_unique<GetOutgoingStatusMessages>());
   AddMethod(ut::make_unique<GetOutgoingTrackMessages>());
}

void* WsfScriptDisseminateC2Class::Create(const UtScriptContext& aContext)
{
   std::shared_ptr<il::DisseminateC2Interface> dc2 = std::shared_ptr<il::DisseminateC2Interface>(new il::DisseminateC2Interface);
   return reinterpret_cast<void*>(new WsfBMDisseminateC2(*WsfScriptContext::GetSCENARIO(aContext), WsfScriptDisseminateC2Class::SCRIPT_CLASS_NAME, "PROCESSOR", dc2));
}

void* WsfScriptDisseminateC2Class::Clone(void* aObjectPtr)
{
   WsfBMDisseminateC2* objPtr = reinterpret_cast<WsfBMDisseminateC2*>(aObjectPtr);
   return objPtr->Clone();
}

void  WsfScriptDisseminateC2Class::Destroy(void* aObjectPtr)
{
   WsfBMDisseminateC2* objPtr = reinterpret_cast<WsfBMDisseminateC2*>(aObjectPtr);
   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptDisseminateC2Class::GetScriptClassName()
{
   return SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptDisseminateC2Class::GetBaseClassName()
{
   return BASE_CLASS_NAME;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDisseminateC2Class, WsfBMDisseminateC2, SetAssetManager, 1, "void", "WsfBattleManager") // TODO: verify change
{
   WsfBattleManager* mgr = (WsfBattleManager*)aVarArgs[0].GetPointer()->GetAppObject();

   HCL_INFO_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(mgr->GetScenario())) << "WsfScriptDisseminateC2Class::SetAssetManager(): bm = " << mgr->GetCoreBMRef() << " am = " << mgr->GetCoreAMRef();

   aObjectPtr->SetComponent(mgr->GetCoreAMRef());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDisseminateC2Class, WsfBMDisseminateC2, Run, 0, "void", "")
{
   aObjectPtr->GetCoreDC2Ref()->updateOutgoingMessages();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDisseminateC2Class, WsfBMDisseminateC2, ClearOutgoingMessages, 0, "void", "")
{
   aObjectPtr->GetCoreDC2Ref()->clearOutgoingMessages();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDisseminateC2Class, WsfBMDisseminateC2, GetOutgoingAssignmentMessages, 0, "Array<WsfBMAssignmentMessage>", "")
{
   auto messages = ut::make_unique<std::vector<ut::script::Data>>();

   UtScriptClass* containedClassPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());

   auto& outgoing_msgs = aObjectPtr->GetCoreDC2Ref()->getOutgoingMessages();
   for (auto it = outgoing_msgs.begin(); it != outgoing_msgs.cend(); ++it)
   {
      auto& msg_event = *it;
      if (msg_event->getMessageType() == il::ASSIGNMENT_MESSAGE)
      {
         WsfBMAssignmentMessage* ext_ptr = new WsfBMAssignmentMessage(WsfScriptContext::GetPLATFORM(aContext), msg_event);
         messages->emplace_back(new UtScriptRef(ext_ptr, containedClassPtr, UtScriptRef::cMANAGE));
      }
   }

   aReturnVal.SetPointer(new UtScriptRef(messages.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDisseminateC2Class, WsfBMDisseminateC2, GetOutgoingAssignmentStatusMessages, 0, "Array<WsfBMAssignmentStatusMessage>", "")
{
   auto messages = ut::make_unique<std::vector<ut::script::Data>>();

   UtScriptClass* containedClassPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());

   auto& outgoing_msgs = aObjectPtr->GetCoreDC2Ref()->getOutgoingMessages();
   for (auto it = outgoing_msgs.begin(); it != outgoing_msgs.cend(); ++it)
   {
      auto& msg_event = *it;
      if (msg_event->getMessageType() == il::ASSIGNMENT_ACK_MESSAGE)
      {
         WsfBMAssignmentStatusMessage* ext_ptr = new WsfBMAssignmentStatusMessage(PLATFORM, msg_event);
         messages->emplace_back(new UtScriptRef(ext_ptr, containedClassPtr, UtScriptRef::cMANAGE));
      }
   }

   aReturnVal.SetPointer(new UtScriptRef(messages.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDisseminateC2Class, WsfBMDisseminateC2, GetOutgoingStatusMessages, 0, "Array<WsfBMStatusMessage>", "")
{
   auto messages = ut::make_unique<std::vector<ut::script::Data>>();

   UtScriptClass* containedClassPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());

   auto& outgoing_msgs = aObjectPtr->GetCoreDC2Ref()->getOutgoingMessages();
   for (auto it = outgoing_msgs.begin(); it != outgoing_msgs.cend(); ++it)
   {
      auto& msg_event = *it;
      if (msg_event->getMessageType() == il::COMBAT_STATUS_MESSAGE)
      {
         WsfBMStatusMessage* ext_ptr = new WsfBMStatusMessage(WsfScriptContext::GetPLATFORM(aContext), msg_event);
         messages->emplace_back(new UtScriptRef(ext_ptr, containedClassPtr, UtScriptRef::cMANAGE));
      }
   }

   aReturnVal.SetPointer(new UtScriptRef(messages.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptDisseminateC2Class, WsfBMDisseminateC2, GetOutgoingTrackMessages, 0, "Array<WsfBMTrackMessage>", "")
{
   auto messages = ut::make_unique<std::vector<ut::script::Data>>();

   UtScriptClass* containedClassPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());

   auto& outgoing_msgs = aObjectPtr->GetCoreDC2Ref()->getOutgoingMessages();
   for (auto it = outgoing_msgs.begin(); it != outgoing_msgs.cend(); ++it)
   {
      auto& msg_event = *it;
      if (msg_event->getMessageType() == il::AIR_TARGET_MESSAGE)
      {
         WsfBMTrackMessage* ext_ptr = new WsfBMTrackMessage(WsfScriptContext::GetPLATFORM(aContext), msg_event);
         messages->emplace_back(new UtScriptRef(ext_ptr, containedClassPtr, UtScriptRef::cMANAGE));
      }
   }

   aReturnVal.SetPointer(new UtScriptRef(messages.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}
