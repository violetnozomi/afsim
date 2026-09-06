// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptUCI_ControlRequestMessage.hpp"

#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "UtScriptRef.hpp"
#include "script/WsfScriptDefs.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"

namespace wsf
{
namespace script
{

UCI_ControlRequestMessageClass::UCI_ControlRequestMessageClass(const std::string& aClassName,
                                                               UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<UUID>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlRequestMessageClass, UCI_ControlRequestMessageClass, Construct, 4, "UCI_ControlRequestMessage",
                        "UCI_Control, UCI_ControlRequest, UCI_SystemId, UCI_CapabilityId")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto controlTypePtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_ControlType>();
      auto controlRequestTypePtr = aVarArgs[1].GetPointer()->GetAppObject<UCI_ControlRequestType>();
      auto systemIdPtr = aVarArgs[2].GetPointer()->GetAppObject<UCI_SystemID_Type>();
      auto capabilityIdPtr = aVarArgs[3].GetPointer()->GetAppObject<UCI_CapabilityID_Type>();
      auto msgPtr = new UCI_ControlRequestMessage(WsfScriptContext::GetPLATFORM(aContext));

      ISM_OwnerProducerType ownPro(SIMULATION);
      ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
      msgPtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(ownPro.Get());

      auto& msgData = msgPtr->Get().getMessageData();
      msgData.getControlRequestID().setUUID(msgPtr->GetInterface().GetConnection()->generateUUID());

      //Sets controller data
      msgData.getController().getSystemID().setUUID(interfacePtr->GetConnection()->getMySystemUUID());
      msgData.getController().getSystemID().setDescriptiveLabel(interfacePtr->GetConnection()->getMySystemLabel());
      msgData.getController().enableServiceID();
      msgData.getController().getServiceID().setUUID(interfacePtr->GetConnection()->getMyServiceUUID());

      msgData.setRequestType(controlRequestTypePtr->Get());
      msgData.setControlType(controlTypePtr->Get());

      //Sets controlee type
      UCI_ControleeType controlee(WsfScriptContext::GetPLATFORM(aContext));
      controlee.Get().setSystemID(systemIdPtr->Get());
      controlee.Get().enableCapabilityID();
      controlee.Get().setCapabilityID(capabilityIdPtr->Get());
      msgData.getControlee().push_back(controlee.Get());

      aReturnVal.SetPointer(new UtScriptRef(msgPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlRequestMessageClass, UCI_ControlRequestMessage, UUID, 0, "string", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetString(aObjectPtr->Get().getMessageData().getControlRequestID().getUUID().getValueAsString());
   }
   else
   {
      aReturnVal.SetNullString();
   }
}

} // script
} // wsf
