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
#include "WsfUCI_MessageFactory.hpp"

#include "uci/base/accessorType.h"

#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Messages.hpp"

namespace wsf
{

UCI_MessageFactory::UCI_MessageFactory(UCI_Interface& aInterface)
   : mInterface(aInterface)
{
}

UCI_MessageFactory::~UCI_MessageFactory()
{
}

//! Create a message type based on the type provided in the message header (see uci/type/accessorType.h)
std::unique_ptr<UCI_MessageBaseType> UCI_MessageFactory::Create(const uci::type::MessageType* aMessagePtr) const
{
   auto accessorType = aMessagePtr->getAccessorType();
   if (accessorType == UCI_AMTI_ActivityMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_AMTI_ActivityMessage>(mInterface, dynamic_cast<const uci::type::AMTI_Activity&>(*aMessagePtr));
   }
   else if (accessorType == UCI_AMTI_CapabilityMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_AMTI_CapabilityMessage>(mInterface, dynamic_cast<const uci::type::AMTI_Capability&>(*aMessagePtr));
   }
   else if (accessorType == UCI_AMTI_CapabilityStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_AMTI_CapabilityStatusMessage>(mInterface, dynamic_cast<const uci::type::AMTI_CapabilityStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_AMTI_CommandMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_AMTI_CommandMessage>(mInterface, dynamic_cast<const uci::type::AMTI_Command&>(*aMessagePtr));
   }
   else if (accessorType == UCI_AMTI_CommandStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_AMTI_CommandStatusMessage>(mInterface, dynamic_cast<const uci::type::AMTI_CommandStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_AMTI_SettingsCommandMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_AMTI_SettingsCommandMessage>(mInterface, dynamic_cast<const uci::type::AMTI_SettingsCommand&>(*aMessagePtr));
   }
   else if (accessorType == UCI_AMTI_SettingsCommandStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_AMTI_SettingsCommandStatusMessage>(mInterface, dynamic_cast<const uci::type::AMTI_SettingsCommandStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_ControlRequestMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_ControlRequestMessage>(mInterface, dynamic_cast<const uci::type::ControlRequest&>(*aMessagePtr));
   }
   else if (accessorType == UCI_ControlRequestStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_ControlRequestStatusMessage>(mInterface, dynamic_cast<const uci::type::ControlRequestStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_ControlStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_ControlStatusMessage>(mInterface, dynamic_cast<const uci::type::ControlStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_DMPI_Message::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_DMPI_Message>(mInterface, dynamic_cast<const uci::type::DMPI&>(*aMessagePtr));
   }
   else if (accessorType == UCI_DMPI_StatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_DMPI_StatusMessage>(mInterface, dynamic_cast<const uci::type::DMPI_Status&>(*aMessagePtr));
   }
   else if (accessorType == UCI_EntityMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_EntityMessage>(mInterface, dynamic_cast<const uci::type::Entity&>(*aMessagePtr));
   }
   else if (accessorType == UCI_ESM_ActivityMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_ESM_ActivityMessage>(mInterface, dynamic_cast<const uci::type::ESM_Activity&>(*aMessagePtr));
   }
   else if (accessorType == UCI_ESM_CapabilityMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_ESM_CapabilityMessage>(mInterface, dynamic_cast<const uci::type::ESM_Capability&>(*aMessagePtr));
   }
   else if (accessorType == UCI_ESM_CapabilityStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_ESM_CapabilityStatusMessage>(mInterface, dynamic_cast<const uci::type::ESM_CapabilityStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_ESM_CommandMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_ESM_CommandMessage>(mInterface, dynamic_cast<const uci::type::ESM_Command&>(*aMessagePtr));
   }
   else if (accessorType == UCI_ESM_CommandStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_ESM_CommandStatusMessage>(mInterface, dynamic_cast<const uci::type::ESM_CommandStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_ESM_SettingsCommandMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_ESM_SettingsCommandMessage>(mInterface, dynamic_cast<const uci::type::ESM_SettingsCommand&>(*aMessagePtr));
   }
   else if (accessorType == UCI_ESM_SettingsCommandStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_ESM_SettingsCommandStatusMessage>(mInterface, dynamic_cast<const uci::type::ESM_SettingsCommandStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_POST_ActivityMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_POST_ActivityMessage>(mInterface, dynamic_cast<const uci::type::POST_Activity&>(*aMessagePtr));
   }
   else if (accessorType == UCI_POST_CapabilityMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_POST_CapabilityMessage>(mInterface, dynamic_cast<const uci::type::POST_Capability&>(*aMessagePtr));
   }
   else if (accessorType == UCI_POST_CapabilityStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_POST_CapabilityStatusMessage>(mInterface, dynamic_cast<const uci::type::POST_CapabilityStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_POST_CommandMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_POST_CommandMessage>(mInterface, dynamic_cast<const uci::type::POST_Command&>(*aMessagePtr));
   }
   else if (accessorType == UCI_POST_CommandStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_POST_CommandStatusMessage>(mInterface, dynamic_cast<const uci::type::POST_CommandStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_POST_SettingsCommandMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_POST_SettingsCommandMessage>(mInterface, dynamic_cast<const uci::type::POST_SettingsCommand&>(*aMessagePtr));
   }
   else if (accessorType == UCI_POST_SettingsCommandStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_POST_SettingsCommandStatusMessage>(mInterface, dynamic_cast<const uci::type::POST_SettingsCommandStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_StrikeActivityMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_StrikeActivityMessage>(mInterface, dynamic_cast<const uci::type::StrikeActivity&>(*aMessagePtr));
   }
   else if (accessorType == UCI_StrikeCapabilityMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_StrikeCapabilityMessage>(mInterface, dynamic_cast<const uci::type::StrikeCapability&>(*aMessagePtr));
   }
   else if (accessorType == UCI_StrikeCapabilityStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_StrikeCapabilityStatusMessage>(mInterface, dynamic_cast<const uci::type::StrikeCapabilityStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_StrikeCommandMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_StrikeCommandMessage>(mInterface, dynamic_cast<const uci::type::StrikeCommand&>(*aMessagePtr));
   }
   else if (accessorType == UCI_StrikeCommandStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_StrikeCommandStatusMessage>(mInterface, dynamic_cast<const uci::type::StrikeCommandStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_SubsystemStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_SubsystemStatusMessage>(mInterface, dynamic_cast<const uci::type::SubsystemStatus&>(*aMessagePtr));
   }
   else if (accessorType == UCI_SystemStatusMessage::GetUCI_TypeId())
   {
      return ut::make_unique<UCI_SystemStatusMessage>(mInterface, dynamic_cast<const uci::type::SystemStatus&>(*aMessagePtr));
   }
   else
   {
      auto error = ut::log::error() << "Could not create UCI message.";
      error.AddNote() << "Accessor Type: " << accessorType;
      return nullptr;
   }
   // others... todo
}

} // wsf
