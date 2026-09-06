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

#include "WsfScriptUCI_POST_CommandMessage.hpp"

#include "script/WsfScriptDefs.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Types.hpp"

namespace wsf
{
namespace script
{

UCI_POST_CommandMessageClass::UCI_POST_CommandMessageClass(const std::string& aClassName,
                                                           UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<LOS>());
   AddMethod(ut::make_unique<PushBack>());
   AddMethod(ut::make_unique<CommandUUID>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CommandMessageClass, UCI_POST_CommandMessage, Construct, 1, "UCI_POST_CommandMessage", "UCI_POST_Command")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto messagePtr = new UCI_POST_CommandMessage(*interfacePtr);
      auto commandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_POST_CommandType>();

      ISM_OwnerProducerType ownPro(SIMULATION);
      ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
      messagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(ownPro.Get());

      messagePtr->Get().getMessageData().getCommand().push_back(commandPtr->Get());
      messagePtr->Get().getSequenceGroup().getMessageHeader().getSystemID().setUUID(interfacePtr->GetConnection()->getMySystemUUID());
      messagePtr->Get().getSequenceGroup().getMessageHeader().enableServiceID();
      messagePtr->Get().getSequenceGroup().getMessageHeader().getServiceID().setUUID(interfacePtr->GetConnection()->getMyServiceUUID());
      messagePtr->Get().getSequenceGroup().getMessageHeader().setMode(uci::type::MessageModeEnum::UCI_SIMULATION);
      aReturnVal.SetPointer(new UtScriptRef(messagePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CommandMessageClass, UCI_POST_CommandMessage, PushBack, 1, "UCI_POST_CommandMessage", "UCI_POST_Command")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto commandPtr = aVarArgs[0].GetPointer()->GetAppObject<UCI_POST_CommandType>();
      aObjectPtr->Get().getMessageData().getCommand().push_back(commandPtr->Get());
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CommandMessageClass, UCI_POST_CommandMessage, CommandUUID, 1, "string", "int")
{
   bool set{ false };
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto index = static_cast<size_t>(aVarArgs[0].GetInt());
      auto& command = aObjectPtr->Get().getMessageData().getCommand();
      if (!command.empty() &&
          index < command.size())
      {
         set = true;
         aReturnVal.SetString(command.at(index).getCapability().getCommandID().getUUID().getValueAsString());
      }
   }
   if (!set)
   {
      aReturnVal.SetNullString();
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CommandMessageClass, UCI_POST_CommandMessage, LOS, 1, "UCI_LOS", "int")
{
   bool set{ false };
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto index = static_cast<size_t>(aVarArgs[0].GetInt());
      auto& command = aObjectPtr->Get().getMessageData().getCommand();
      if (!command.empty() &&
          index < command.size())
      {
         if (command.at(index).getCapability().hasPointing())
         {
            set = true;
            auto& pointing = command.at(index).getCapability().getPointing();
            auto losPtr = new UCI_LOS_Type(*interfacePtr,
                                           pointing.getLOSOption().getLOS());
            aReturnVal.SetPointer(new UtScriptRef(losPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
         }
      }
   }
   if (!set)
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UCI_LOS_TypeClass::UCI_LOS_TypeClass(const std::string& aClassName,
                                     UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<Azimuth>());
   AddMethod(ut::make_unique<Elevation>());
   AddMethod(ut::make_unique<Reference>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_LOS_TypeClass, UCI_LOS_Type, Azimuth, 0, "double", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetDouble(aObjectPtr->Get().getAzimuth());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_LOS_TypeClass, UCI_LOS_Type, Elevation, 0, "double", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetDouble(aObjectPtr->Get().getElevation());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_LOS_TypeClass, UCI_LOS_Type, Reference, 0, "UCI_LOS_Reference", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto referencePtr = new UCI_LOS_ReferenceEnumType(SIMULATION);
      referencePtr->Get().setValue(aObjectPtr->Get().getReference().getValue());
      aReturnVal.SetPointer(new UtScriptRef(referencePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

} // script
} // wsf
