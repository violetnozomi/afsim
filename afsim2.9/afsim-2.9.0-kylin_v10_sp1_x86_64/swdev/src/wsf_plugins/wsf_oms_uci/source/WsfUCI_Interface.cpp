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

#include "WsfUCI_Interface.hpp"

#include <iostream>
#include <unordered_set>

#include "UtLog.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"

#include "uci/base/Accessor.h"
#include "uci/base/AbstractServiceBusConnection.h"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfMessage.hpp"
#include "WsfSimulation.hpp"
#include "observer/WsfSimulationObserver.hpp"
#include "WsfUCI_Component.hpp"
#include "WsfUCI_Messages.hpp"

namespace wsf
{

namespace
{
   std::unordered_set<std::string> sMessageNames { UCI_AMTI_ActivityMessage::GetUCI_TypeName(),
                                                   UCI_AMTI_CapabilityMessage::GetUCI_TypeName(),
                                                   UCI_AMTI_CapabilityStatusMessage::GetUCI_TypeName(),
                                                   UCI_AMTI_CommandMessage::GetUCI_TypeName(),
                                                   UCI_AMTI_CommandStatusMessage::GetUCI_TypeName(),
                                                   UCI_AMTI_SettingsCommandMessage::GetUCI_TypeName(),
                                                   UCI_AMTI_SettingsCommandStatusMessage::GetUCI_TypeName(),
                                                   UCI_ControlRequestMessage::GetUCI_TypeName(),
                                                   UCI_ControlRequestStatusMessage::GetUCI_TypeName(),
                                                   UCI_ControlStatusMessage::GetUCI_TypeName(),
                                                   UCI_DMPI_Message::GetUCI_TypeName(),
                                                   UCI_DMPI_StatusMessage::GetUCI_TypeName(),
                                                   UCI_EntityMessage::GetUCI_TypeName(),
                                                   UCI_ESM_ActivityMessage::GetUCI_TypeName(),
                                                   UCI_ESM_CapabilityMessage::GetUCI_TypeName(),
                                                   UCI_ESM_CapabilityStatusMessage::GetUCI_TypeName(),
                                                   UCI_ESM_CommandMessage::GetUCI_TypeName(),
                                                   UCI_ESM_CommandStatusMessage::GetUCI_TypeName(),
                                                   UCI_ESM_SettingsCommandMessage::GetUCI_TypeName(),
                                                   UCI_ESM_SettingsCommandStatusMessage::GetUCI_TypeName(),
                                                   UCI_POST_ActivityMessage::GetUCI_TypeName(),
                                                   UCI_POST_CapabilityMessage::GetUCI_TypeName(),
                                                   UCI_POST_CapabilityStatusMessage::GetUCI_TypeName(),
                                                   UCI_POST_CommandMessage::GetUCI_TypeName(),
                                                   UCI_POST_CommandStatusMessage::GetUCI_TypeName(),
                                                   UCI_POST_SettingsCommandMessage::GetUCI_TypeName(),
                                                   UCI_POST_SettingsCommandStatusMessage::GetUCI_TypeName(),
                                                   UCI_StrikeActivityMessage::GetUCI_TypeName(),
                                                   UCI_StrikeCapabilityMessage::GetUCI_TypeName(),
                                                   UCI_StrikeCapabilityStatusMessage::GetUCI_TypeName(),
                                                   UCI_StrikeCommandMessage::GetUCI_TypeName(),
                                                   UCI_StrikeCommandStatusMessage::GetUCI_TypeName(),
                                                   UCI_SubsystemStatusMessage::GetUCI_TypeName(),
                                                   UCI_SystemStatusMessage::GetUCI_TypeName()
                                            };
};

bool UCI_Interface::ProcessInput(UtInput& aInput)
{
   std::string command = aInput.GetCommand();
   bool myCommand = false;
   if (command == "uci_interface")
   {
      myCommand = true;
      UtInputBlock block(aInput);
      while(block.ReadCommand(command))
      {
         if (command == "message_topic")
         {
            std::string msgType;
            block.ReadCommand(msgType);

            // Determine if this is a valid type
            if (sMessageNames.find(msgType) != sMessageNames.end())
            {
               std::string topic;
               myCommand = block.ReadCommand(topic);
               // Map the type to the topic.
               mTopics.emplace(std::make_pair(msgType, topic));
            }
            else
            {
               auto error = ut::log::error("Invalid UCI Message Type");
               error.AddNote(msgType);
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (command == "service_name")
         {
            aInput.ReadValue(mServiceName);
         }
         else if (command == "service_descriptor")
         {
           aInput.ReadValue(mServiceDescriptor);
         }
      }

      mIsConfigured = true;
   }
   return myCommand;
}

//! Initialize the abstract bus interface
//! @param aSimulation The simulation object
bool UCI_Interface::Initialize(WsfSimulation& aSimulation)
{
   if (!mIsInitialized)
   {
      if (mServiceName == "")
      {
         auto out = ut::log::error() << "You must specify a service_name in a uci_interface block";
         out.AddNote("Initialization of UCI_Interface failed.");
         return false;
      }

      mAsbPtr = uci_getAbstractServiceBusConnection(mServiceName, "ASB");
      if (mAsbPtr == nullptr)
      {
         // This condition has not been encountered yet (only the service UUID == 0).
         ut::log::error() << "Unable to retrieve the UCI Abstract Bus pointer";
         return false;
      }

      const uci::base::UUID& myUUID = mAsbPtr->getMyServiceUUID();
      const uci::base::UUID::ValueUUID& value = myUUID.getValueAsValueUUID();
      if ((value.mostSignificantBits == 0) && (value.leastSignificantBits == 0))
      {
         auto error = ut::log::error() << "Invalid UUID for UCI Abstract Bus service ID";
         error.AddNote("Does the service_name in the uci_interface block match a service name in the ActiveMQ configuration file?");
         return false;
      }

      mSimulationPtr = &aSimulation;

      // Create a message service for every supported UCI message type.
      CreateMessageService<UCI_AMTI_ActivityMessage>();
      CreateMessageService<UCI_AMTI_CapabilityMessage>();
      CreateMessageService<UCI_AMTI_CapabilityStatusMessage>();
      CreateMessageService<UCI_AMTI_CommandMessage>();
      CreateMessageService<UCI_AMTI_CommandStatusMessage>();
      CreateMessageService<UCI_AMTI_SettingsCommandMessage>();
      CreateMessageService<UCI_AMTI_SettingsCommandStatusMessage>();
      CreateMessageService<UCI_ControlRequestMessage>();
      CreateMessageService<UCI_ControlRequestStatusMessage>();
      CreateMessageService<UCI_ControlStatusMessage>();
      CreateMessageService<UCI_DMPI_Message>();
      CreateMessageService<UCI_DMPI_StatusMessage>();
      CreateMessageService<UCI_EntityMessage>();
      CreateMessageService<UCI_ESM_ActivityMessage>();
      CreateMessageService<UCI_ESM_CapabilityMessage>();
      CreateMessageService<UCI_ESM_CapabilityStatusMessage>();
      CreateMessageService<UCI_ESM_CommandMessage>();
      CreateMessageService<UCI_ESM_CommandStatusMessage>();
      CreateMessageService<UCI_ESM_SettingsCommandMessage>();
      CreateMessageService<UCI_ESM_SettingsCommandStatusMessage>();
      CreateMessageService<UCI_POST_ActivityMessage>();
      CreateMessageService<UCI_POST_CapabilityMessage>();
      CreateMessageService<UCI_POST_CapabilityStatusMessage>();
      CreateMessageService<UCI_POST_CommandMessage>();
      CreateMessageService<UCI_POST_CommandStatusMessage>();
      CreateMessageService<UCI_POST_SettingsCommandMessage>();
      CreateMessageService<UCI_POST_SettingsCommandStatusMessage>();
      CreateMessageService<UCI_StrikeActivityMessage>();
      CreateMessageService<UCI_StrikeCapabilityMessage>();
      CreateMessageService<UCI_StrikeCapabilityStatusMessage>();
      CreateMessageService<UCI_StrikeCommandMessage>();
      CreateMessageService<UCI_StrikeCommandStatusMessage>();
      CreateMessageService<UCI_SubsystemStatusMessage>();
      CreateMessageService<UCI_SystemStatusMessage>();

      mIsInitialized = true;
   }

   return mIsInitialized;
}

UCI_Component* wsf::UCI_Interface::GetUCI_Component(const std::string& aCapabilityId)
{
   UCI_Component* componentPtr = nullptr;
   auto it = mRegisteredCapabilities.find(aCapabilityId);
   if (it != mRegisteredCapabilities.end())
   {
      componentPtr = it->second;
   }
   return componentPtr;
}

//! Shutdown the interface.  If there is an active abstract bus connection,
//! all services and factories will also be shutdown.
void wsf::UCI_Interface::Shutdown()
{
   if (mAsbPtr != nullptr)
   {
      auto serviceIter = mServices.begin();
      while (serviceIter != mServices.end())
      {
         serviceIter->second->Shutdown();
         ++serviceIter;
      }
      auto factoryIter = mFactories.begin();
      while (factoryIter != mFactories.end())
      {
         factoryIter->second->Shutdown();
         ++factoryIter;
      }
      mAsbPtr->shutdown();
   }
}

void UCI_Interface::RegisterComponent(UCI_Component& aComponent)
{
   // Add component to map
   mRegisteredComponents.insert(make_pair(aComponent.GetName(), &aComponent));
   
}

void UCI_Interface::DeregisterComponent(UCI_Component& aComponent)
{
   // Remove component from map
   mRegisteredComponents.erase(aComponent.GetName());
   
}

void UCI_Interface::RegisterCapabilities(UCI_Component& aComponent)
{
   for (auto& uuid : aComponent.GetCapabilityUUIDs())
   {
      mRegisteredCapabilities.insert(make_pair(uuid, &aComponent));
   }
}

void UCI_Interface::DeregisterCapabilities(UCI_Component& aComponent)
{
   for (auto& uuid : aComponent.GetCapabilityUUIDs())
   {
      mRegisteredCapabilities.erase(uuid);
   }
}

void UCI_Interface::ReceiveMessage(const uci::type::MessageType* aMessagePtr) const
{
   // dispatch to interested parties.
   auto componentIter = mRegisteredComponents.begin();
   while (componentIter != mRegisteredComponents.end())
   {
      auto componentPtr = componentIter->second;
      componentPtr->ReceiveUCI_Message(aMessagePtr);
      ++componentIter;
   }
}

void UCI_Interface::PopulateHeader(const uci::type::DateTimeTypeValue& aSimTime,
                                   uci::type::HeaderType&              aHeader)
{
   // System ID
   const uci::base::UUID& systemUuid = mAsbPtr->getMySystemUUID();
   aHeader.getSystemID().setUUID(systemUuid);
   aHeader.getSystemID().setDescriptiveLabel(mAsbPtr->getMySystemLabel());

   //ServiceID
   {
      aHeader.enableServiceID();
      const uci::base::UUID& serUuid = mAsbPtr->getMyServiceUUID();
      aHeader.getServiceID().setUUID(serUuid);
      aHeader.getServiceID().setDescriptiveLabel(mServiceDescriptor);
   }

   // Set the timestamp (time in seconds since 1970 January 1 00:00:00.000)			
   aHeader.setTimestamp(aSimTime);

   // Schema version
   aHeader.setSchemaVersion(mAsbPtr->getOmsSchemaVersion());

   // Message mode
   aHeader.setMode(uci::type::MessageModeEnum::UCI_SIMULATION);
}

}
