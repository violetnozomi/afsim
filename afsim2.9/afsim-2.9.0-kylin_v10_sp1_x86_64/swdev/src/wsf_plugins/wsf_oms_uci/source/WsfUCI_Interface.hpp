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

#ifndef WSFUCI_INTERFACE_HPP
#define WSFUCI_INTERFACE_HPP

#include "wsf_oms_uci_export.h"

#include <map>
#include <memory>
#include <string>
#include <iostream>

class UtInput;
#include "UtMemory.hpp"

class WsfMessage;
class WsfSimulation;

#include "WsfUCI_MessageBaseType.hpp"
#include "WsfUCI_FactoryService.hpp"
#include "WsfUCI_MessageService.hpp"

//Fundamental Types
#include "uci/base/AbstractServiceBusConnection.h"
#include "uci/base/UUID.h"
#include "uci/type/HeaderType.h"
#include "uci/type/MessageType.h"

//Factories
#include "uci/factory/AMTI_ActivityFactory.h"
#include "uci/factory/AMTI_CapabilityFactory.h"
#include "uci/factory/AMTI_CapabilityStatusFactory.h"
#include "uci/factory/AMTI_CommandFactory.h"
#include "uci/factory/AMTI_CommandStatusFactory.h"
#include "uci/factory/AMTI_SettingsCommandFactory.h"
#include "uci/factory/AMTI_SettingsCommandStatusFactory.h"
#include "uci/factory/CapabilityControlInterfacesEnumFactory.h"
#include "uci/factory/ControlRequestFactory.h"
#include "uci/factory/ControlRequestStatusFactory.h"
#include "uci/factory/ControlStatusFactory.h"
#include "uci/factory/EntityFactory.h"
#include "uci/factory/EntityID_TypeFactory.h"
#include "uci/factory/EntityMTFactory.h"
#include "uci/factory/EntityMDTFactory.h"
#include "uci/factory/ESM_ActivityFactory.h"
#include "uci/factory/ESM_CapabilityFactory.h"
#include "uci/factory/ESM_CapabilityStatusFactory.h"
#include "uci/factory/ESM_CommandFactory.h"
#include "uci/factory/ESM_CommandStatusFactory.h"
#include "uci/factory/ESM_SettingsCommandFactory.h"
#include "uci/factory/ESM_SettingsCommandStatusFactory.h"
#include "uci/factory/KinematicsTypeFactory.h"
#include "uci/factory/MessageStateEnumFactory.h"
#include "uci/factory/PointingTypeEnumFactory.h"
#include "uci/factory/POST_ActivityFactory.h"
#include "uci/factory/POST_CapabilityFactory.h"
#include "uci/factory/POST_CapabilityTypeFactory.h"
#include "uci/factory/POST_CapabilityMDTFactory.h"
#include "uci/factory/POST_CapabilityStatusFactory.h"
#include "uci/factory/POST_CapabilityStatusTypeFactory.h"
#include "uci/factory/POST_CommandFactory.h"
#include "uci/factory/POST_CommandStatusFactory.h"
#include "uci/factory/POST_SettingsCommandFactory.h"
#include "uci/factory/POST_SettingsCommandStatusFactory.h"
#include "uci/factory/POST_SubCapabilityTypeFactory.h"
#include "uci/factory/SubsystemStatusFactory.h"
#include "uci/factory/SystemStatusFactory.h"

// Readers
#include "uci/reader/EntityReader.h"
#include "uci/reader/ESM_ActivityReader.h"
#include "uci/reader/ESM_CapabilityReader.h"
#include "uci/reader/ESM_CapabilityStatusReader.h"
#include "uci/reader/ESM_CommandReader.h"
#include "uci/reader/ESM_CommandStatusReader.h"
#include "uci/reader/ESM_SettingsCommandReader.h"
#include "uci/reader/ESM_SettingsCommandStatusReader.h"
#include "uci/reader/POST_ActivityReader.h"
#include "uci/reader/POST_CapabilityReader.h"
#include "uci/reader/POST_CapabilityStatusReader.h"
#include "uci/reader/POST_CommandReader.h"
#include "uci/reader/POST_CommandStatusReader.h"
#include "uci/reader/SubsystemStatusReader.h"
#include "uci/reader/SystemStatusReader.h"

// Writers
#include "uci/writer/EntityWriter.h"
#include "uci/writer/ESM_ActivityWriter.h"
#include "uci/writer/ESM_CapabilityWriter.h"
#include "uci/writer/ESM_CapabilityStatusWriter.h"
#include "uci/writer/ESM_CommandWriter.h"
#include "uci/writer/ESM_SettingsCommandWriter.h"
#include "uci/writer/ESM_SettingsCommandStatusWriter.h"
#include "uci/writer/ESM_CommandStatusWriter.h"
#include "uci/writer/POST_ActivityWriter.h"
#include "uci/writer/POST_CapabilityWriter.h"
#include "uci/writer/POST_CapabilityStatusWriter.h"
#include "uci/writer/POST_CommandWriter.h"
#include "uci/writer/POST_CommandStatusWriter.h"
#include "uci/writer/SubsystemStatusWriter.h"
#include "uci/writer/SystemStatusWriter.h"

namespace wsf
{
   class UCI_Component;

   //! The OMS / UCI Interface Object to the UCI middleware.
   class WSF_OMS_UCI_EXPORT UCI_Interface
   {
      public:

         UCI_Interface() = default;
         UCI_Interface(const UCI_Interface& aSrc) = delete;
         const UCI_Interface& operator=(const UCI_Interface& aRhs) = delete;
         virtual ~UCI_Interface() = default;

         bool ProcessInput(UtInput& aInput);
         bool Initialize(WsfSimulation&     aSimulation);

         //! Return whether this interface has been configured in ProcessInput.
         bool IsConfigured() const { return mIsConfigured; }

         UCI_Component* GetUCI_Component(const std::string& aCapabilityId);

         WsfSimulation& GetSimulation() { return *mSimulationPtr; } // Used for creating messages

         void Shutdown();

         void SetServiceDescriptor(const std::string& aServiceDescriptor) { mServiceDescriptor = aServiceDescriptor; }

         void RegisterComponent(UCI_Component& aComponent);

         void DeregisterComponent(UCI_Component& aComponent);

         void RegisterCapabilities(UCI_Component& aComponent);

         void DeregisterCapabilities(UCI_Component& aComponent);

         //! Return the abstract service bus connection object.
         uci::base::AbstractServiceBusConnection* GetConnection() const { return mAsbPtr; }

         void ReceiveMessage(const uci::type::MessageType* aMessagePtr) const;

         void PopulateHeader(const uci::type::DateTimeTypeValue& aSimTime,
                             uci::type::HeaderType&              aHeader);

         //! Create and store a message service.  This must be called for all relevant message types.
         template<typename T> void CreateMessageService()
         {
            uci::base::accessorType::AccessorType type = T::GetUCI_TypeId();
            std::string topic = T::GetUCI_TypeName();
            auto topicsIter = mTopics.find(T::GetUCI_TypeName());
            if (topicsIter != mTopics.end())
            {
               topic = topicsIter->second;
            }
            auto servicesIter = mServices.find(type);
            if (servicesIter == mServices.end())
            {
               auto servicePtr = T::CreateMessageService(*this, mAsbPtr, topic.c_str());
               mServices.emplace(std::make_pair(type, std::move(servicePtr)));
            }
         }

         template<typename T> void GetMessageService(uci::base::accessorType::AccessorType aType, const T*& aServicePtr)
         {
            ServiceMap::iterator iter = mServices.find(aType);
            if (iter != mServices.end())
            {
               aServicePtr = static_cast<T*>(iter->second.get());
            }
            else
            {
               aServicePtr = nullptr;
            }
         }

         template<typename T> void GetTypeFactory(uci::base::accessorType::AccessorType aType, const T*& aFactoryPtr)
         {
            ServiceMap::iterator iter = mFactories.find(aType);
            if (iter == mFactories.end())
            {
               mFactories.emplace(std::make_pair(aType, ut::make_unique<T>(mAsbPtr)));
               // todo get the pointer in the return value above.
               iter = mFactories.find(aType);
            }
            UCI_ServiceBase* sbp = iter->second.get();
            aFactoryPtr = static_cast<T*>(sbp);
         }

      private:

         // be able to map incoming messages to the components that can process them (TODO)
         using ComponentMap = std::map<std::string, UCI_Component*>;
         using ServiceMap = std::map<uci::base::accessorType::AccessorType, std::unique_ptr<UCI_ServiceBase>>;
         using TopicMap = std::map<std::string, std::string>;

         WsfSimulation*                           mSimulationPtr     { nullptr };
         uci::base::AbstractServiceBusConnection* mAsbPtr            { nullptr };
         std::string                              mServiceName;
         std::string                              mServiceDescriptor { "uci" };
         bool                                     mIsInitialized     { false };
         bool                                     mIsConfigured      { false };
         ServiceMap                               mServices;
         ServiceMap                               mFactories;
         ComponentMap                             mRegisteredComponents;
         ComponentMap                             mRegisteredCapabilities;
         TopicMap                                 mTopics;

   };
}

#endif
