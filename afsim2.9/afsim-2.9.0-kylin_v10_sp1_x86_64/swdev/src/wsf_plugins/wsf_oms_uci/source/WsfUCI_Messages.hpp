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

#ifndef WSFUCI_MESSAGES_HPP
#define WSFUCI_MESSAGES_HPP

#include "wsf_oms_uci_export.h"

#include "uci/base/accessorType.h"

#include "UtMemory.hpp"

#include "WsfDateTime.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

#include "WsfUCI_Interface.hpp"
#include "WsfUCI_MessageBaseType.hpp"
#include "WsfUCI_MessageService.hpp"
#include "WsfUCI_Utils.hpp"

//Message types
#include "uci/type/AMTI_Activity.h"
#include "uci/type/AMTI_ActivityType.h"
#include "uci/type/AMTI_Capability.h"
#include "uci/type/AMTI_CapabilityStatus.h"
#include "uci/type/AMTI_Command.h"
#include "uci/type/AMTI_CommandStatus.h"
#include "uci/type/AMTI_SettingsCommand.h"
#include "uci/type/AMTI_SettingsCommandStatus.h"
#include "uci/type/ControlRequest.h"
#include "uci/type/ControlRequestStatus.h"
#include "uci/type/ControlStatus.h"
#include "uci/type/DMPI.h"
#include "uci/type/DMPI_Status.h"
#include "uci/type/Entity.h"
#include "uci/type/ESM_Activity.h"
#include "uci/type/ESM_ActivityType.h"
#include "uci/type/ESM_Capability.h"
#include "uci/type/ESM_CapabilityStatus.h"
#include "uci/type/ESM_Command.h"
#include "uci/type/ESM_CommandStatus.h"
#include "uci/type/ESM_SettingsCommand.h"
#include "uci/type/ESM_SettingsCommandStatus.h"
#include "uci/type/POST_Activity.h"
#include "uci/type/POST_Capability.h"
#include "uci/type/POST_Command.h"
#include "uci/type/POST_CommandStatus.h"
#include "uci/type/POST_SettingsCommand.h"
#include "uci/type/POST_SettingsCommandStatus.h"
//#include "uci/type/SMTI_Activity.h"
//#include "uci/type/SMTI_ActivityType.h"
//#include "uci/type/SMTI_Capability.h"
//#include "uci/type/SMTI_CapabilityStatus.h"
//#include "uci/type/SMTI_Command.h"
//#include "uci/type/SMTI_CommandStatus.h"
#include "uci/type/StrikeActivity.h"
#include "uci/type/StrikeCapability.h"
#include "uci/type/StrikeCapabilityStatus.h"
#include "uci/type/StrikeCommand.h"
#include "uci/type/StrikeCommandStatus.h"
#include "uci/type/SubsystemStatus.h"
#include "uci/type/SystemStatus.h"

//factories
#include "uci/factory/AMTI_ActivityFactory.h"
#include "uci/factory/AMTI_CapabilityFactory.h"
#include "uci/factory/AMTI_CapabilityStatusFactory.h"
#include "uci/factory/AMTI_CommandFactory.h"
#include "uci/factory/AMTI_CommandStatusFactory.h"
#include "uci/factory/AMTI_SettingsCommandFactory.h"
#include "uci/factory/AMTI_SettingsCommandStatusFactory.h"
#include "uci/factory/ControlRequestFactory.h"
#include "uci/factory/ControlRequestStatusFactory.h"
#include "uci/factory/ControlStatusFactory.h"
#include "uci/factory/DMPIFactory.h"
#include "uci/factory/DMPI_StatusFactory.h"
#include "uci/factory/EntityFactory.h"
#include "uci/factory/ESM_ActivityFactory.h"
#include "uci/factory/ESM_CapabilityFactory.h"
#include "uci/factory/ESM_CapabilityStatusFactory.h"
#include "uci/factory/ESM_CommandFactory.h"
#include "uci/factory/ESM_CommandStatusFactory.h"
#include "uci/factory/ESM_SettingsCommandFactory.h"
#include "uci/factory/ESM_SettingsCommandStatusFactory.h"
#include "uci/factory/POST_ActivityFactory.h"
#include "uci/factory/POST_CapabilityFactory.h"
#include "uci/factory/POST_CapabilityStatusFactory.h"
#include "uci/factory/POST_CommandFactory.h"
#include "uci/factory/POST_CommandStatusFactory.h"
#include "uci/factory/POST_SettingsCommandFactory.h"
#include "uci/factory/POST_SettingsCommandStatusFactory.h"
//#include "uci/factory/SMTI_ActivityFactory.h"
//#include "uci/factory/SMTI_CapabilityFactory.h"
//#include "uci/factory/SMTI_CapabilityStatusFactory.h"
//#include "uci/factory/SMTI_CommandFactory.h"
//#include "uci/factory/SMTI_CommandStatusFactory.h"
#include "uci/factory/StrikeActivityFactory.h"
#include "uci/factory/StrikeCapabilityFactory.h"
#include "uci/factory/StrikeCapabilityStatusFactory.h"
#include "uci/factory/StrikeCommandFactory.h"
#include "uci/factory/StrikeCommandStatusFactory.h"
#include "uci/factory/SubsystemStatusFactory.h"
#include "uci/factory/SystemStatusFactory.h"

//readers
#include "uci/reader/EntityReader.h"
#include "uci/reader/ESM_CapabilityReader.h"
#include "uci/reader/POST_ActivityReader.h"
#include "uci/reader/POST_CapabilityReader.h"
#include "uci/reader/POST_CommandReader.h"
#include "uci/reader/SubsystemStatusReader.h"

//writers
#include "uci/writer/EntityWriter.h"
#include "uci/writer/ESM_CapabilityWriter.h"
#include "uci/writer/ESM_CapabilityStatusWriter.h"
#include "uci/writer/POST_ActivityWriter.h"
#include "uci/writer/POST_CapabilityWriter.h"
#include "uci/writer/POST_CommandWriter.h"
#include "uci/writer/SubsystemStatusWriter.h"

namespace wsf
{

//! Class parameterizing UCI message types.
template< typename    Type,
          typename    Factory,
          typename    Reader,
          typename    Writer,
          typename    Listener,
          uci::base::accessorType::AccessorType TypeName>      // Used for type resolution; see GetTypeId()
class WSF_OMS_UCI_EXPORT UCI_MessageType : public UCI_MessageBaseType
{   
   public:
      
      explicit UCI_MessageType(WsfPlatform* aPlatformPtr);
      explicit UCI_MessageType(UCI_Interface& aInterface);
      UCI_MessageType(UCI_Interface& aInterface,
                      const Type&   aType);
      UCI_MessageType(const UCI_MessageType& aSrc);
      ~UCI_MessageType() override;

      WsfMessage* Clone() const override
      {
         return new UCI_MessageType<Type, Factory, Reader, Writer, Listener, TypeName>(*this);
      }

      // Todo change to GetUCI_AccessorType()
      static uci::base::accessorType::AccessorType GetUCI_TypeId() { return TypeName; }

      //! Get the underlying UCI type.
      Type& Get() const { return *mTypePtr; }

      //! Return this message as a uci::type::MessageType (common base class)
      //! @note The "this->" in the following line was added to work around a gcc/linux compiler issue.
      const uci::type::HeaderType* GetHeader() const override
      {
         return &(mTypePtr->getSequenceGroup().getMessageHeader());
      }

      //! Send this message using the writer. This method calls PopulateHeader before sending.
      void Send(const uci::type::DateTimeTypeValue& aSimTime) const override;

   private:

      //! Populate the header in preparation for sending locally or to another simulation.
      void PopulateHeader() const;

      Type*                                                              mTypePtr         { nullptr };
      const UCI_FactoryService<Type, Factory>*                           mTypeFactoryPtr  { nullptr };
      const UCI_MessageService<Type, Factory, Reader, Writer, Listener>* mTypeServicePtr  { nullptr };
};

template<typename Type, typename Factory, typename Reader, typename Writer, typename Listener, typename uci::base::accessorType::AccessorType TypeName>
inline UCI_MessageType<Type, Factory, Reader, Writer, Listener, TypeName>::UCI_MessageType(WsfPlatform* aPlatformPtr)
   : UCI_MessageBaseType(*aPlatformPtr->GetSimulation(), *UCI_Util::GetInterface(aPlatformPtr))
{
   GetInterface().GetTypeFactory(TypeName, mTypeFactoryPtr);
   mTypePtr = &(mTypeFactoryPtr->GetFactory().create());
   GetInterface().GetMessageService(TypeName, mTypeServicePtr);
   PopulateHeader();
}

template<typename Type, typename Factory, typename Reader, typename Writer, typename Listener, uci::base::accessorType::AccessorType TypeName>
inline UCI_MessageType<Type, Factory, Reader, Writer, Listener, TypeName>::UCI_MessageType(UCI_Interface& aInterface)
   : UCI_MessageBaseType(aInterface.GetSimulation(), aInterface)
{
   GetInterface().GetTypeFactory(TypeName, mTypeFactoryPtr);
   mTypePtr = &(mTypeFactoryPtr->GetFactory().create());
   GetInterface().GetMessageService(TypeName, mTypeServicePtr);
   PopulateHeader();
}

template<typename Type, typename Factory, typename Reader, typename Writer, typename Listener, uci::base::accessorType::AccessorType TypeName>
inline UCI_MessageType<Type, Factory, Reader, Writer, Listener, TypeName>::UCI_MessageType(wsf::UCI_Interface& aInterface, const Type& aType)
   : UCI_MessageBaseType(aInterface.GetSimulation(), aInterface)
{
   GetInterface().GetTypeFactory(TypeName, mTypeFactoryPtr);
   mTypePtr = &(mTypeFactoryPtr->GetFactory().create(aType));
   GetInterface().GetMessageService(TypeName, mTypeServicePtr);
}

template<typename Type, typename Factory, typename Reader, typename Writer, typename Listener, typename uci::base::accessorType::AccessorType TypeName>
inline UCI_MessageType<Type, Factory, Reader, Writer, Listener, TypeName>::UCI_MessageType(const UCI_MessageType& aSrc)
   : UCI_MessageBaseType(aSrc),
     mTypeFactoryPtr(aSrc.mTypeFactoryPtr),
     mTypeServicePtr(aSrc.mTypeServicePtr)
{
   mTypePtr = &(mTypeFactoryPtr->GetFactory().create(*aSrc.mTypePtr));
}

template<typename Type, typename Factory, typename Reader, typename Writer, typename Listener, typename uci::base::accessorType::AccessorType TypeName>
inline UCI_MessageType<Type, Factory, Reader, Writer, Listener, TypeName>::~UCI_MessageType()
{
   mTypeFactoryPtr->GetFactory().destroy(*mTypePtr);
}

template<typename Type, typename Factory, typename Reader, typename Writer, typename Listener, typename uci::base::accessorType::AccessorType TypeName>
inline void UCI_MessageType<Type, Factory, Reader, Writer, Listener, TypeName>::Send(const uci::type::DateTimeTypeValue& aSimTime) const
{
   try
   {
      GetInterface().PopulateHeader(aSimTime, Get().getSequenceGroup().getMessageHeader());
      mTypeServicePtr->GetWriter().write(Get());
   }
   catch (const uci::base::UCIException& ex)
   {
      auto error = ut::log::error() << "Cannot send UCI Message.";
      error.AddNote() << ex.what();
   }
}

template<typename Type, typename Factory, typename Reader, typename Writer, typename Listener, uci::base::accessorType::AccessorType TypeName>
inline void UCI_MessageType<Type, Factory, Reader, Writer, Listener, TypeName>::PopulateHeader() const
{
   uci::type::DateTimeTypeValue absoluteTime = UCI_Util::GetTimeStamp(GetInterface().GetSimulation(),
                                                                      GetInterface().GetSimulation().GetSimTime());
   GetInterface().PopulateHeader(absoluteTime, Get().getSequenceGroup().getMessageHeader());
}

//=======================================================================================================
//! Define a WSF message type with the contents of an existing UCI message type.  The WSF type can
//! be constructed and destructed without direct reference to the abstract bus.
//! Arguments:
//! WSF_MESSAGE_TYPE : The name of the WSF message type.
//! UCI_MESSAGE_TYPE : The name of the existing UCI message type.
//! WSF_TYPE_NAME    : The name of the message as referenced in on_message blocks in WSF.
//! UCI_ACCESSOR_NAME : The accessor id corresponding with the existing UCI message type (from uci::type::accessorType).
//!
//! For example, the following definition:
//! @code
//! DEFINE_UCI_MESSAGE_TYPE(UCI_ControlRequestMessage, ControlRequest, UCI_CONTROL_REQUEST_MESSAGE, controlRequestMT);
//! @endcode
//! produces the following code expansion:
//! @code
//! class UCI_ControlRequestMessage : public UCI_MessageType<uci::type::ControlRequest,
//!                                                          uci::factory::ControlRequestFactory,
//!                                                          uci::reader::ControlRequestReader,
//!                                                          uci::writer::ControlRequestWriter,
//!                                                          uci::reader::ControlRequestListener,
//!                                                          uci::type::accessorType::controlRequestMT>
//! {
//!    public:
//!       explicit UCI_ControlRequestMessage(WsfPlatform* aPlatform) : UCI_MessageType(aPlatformPtr) {}
//!       explicit UCI_ControlRequestMessage(UCI_Interface& aInterface) : UCI_MessageType(aInterface) {}
//!       UCI_ControlRequestMessage(const UCI_ControlRequestMessage& aSrc) = default;
//!       UCI_ControlRequestMessage(UCI_Interface& aInterface, const uci::type::UCI_ControlRequestMessage& aType) : UCI_MessageType(aInterface, aType) {}
//!       ~UCI_ControlRequestMessage() override = default;
//!       WsfMessage* Clone() const override { return new UCI_ControlRequestMessage(*this); }
//!       const char* GetScriptClassName() const override { return "UCI_ControlRequestMessage"; }
//!       static WsfStringId GetTypeId() { static WsfStringId sMessageTypeId("UCI_CONTROL_REQUEST_MESSAGE"); return sMessageTypeId; }
//! };
//! @endcode
#define DEFINE_UCI_MESSAGE_TYPE(WSF_MESSAGE_TYPE, UCI_MESSAGE_TYPE, WSF_TYPE_NAME, UCI_ACCESSOR_NAME)       \
      using WSF_MESSAGE_TYPE##Service = UCI_MessageService<uci::type::UCI_MESSAGE_TYPE,       \
                                                uci::factory::UCI_MESSAGE_TYPE##Factory,      \
                                                uci::reader::UCI_MESSAGE_TYPE##Reader,        \
                                                uci::writer::UCI_MESSAGE_TYPE##Writer,        \
                                                uci::reader::UCI_MESSAGE_TYPE##Listener>;     \
                                                                                         \
class WSF_MESSAGE_TYPE : public UCI_MessageType<uci::type::UCI_MESSAGE_TYPE,             \
                                                uci::factory::UCI_MESSAGE_TYPE##Factory, \
                                                uci::reader::UCI_MESSAGE_TYPE##Reader,   \
                                                uci::writer::UCI_MESSAGE_TYPE##Writer,   \
                                                uci::reader::UCI_MESSAGE_TYPE##Listener, \
                                                uci::type::accessorType::UCI_ACCESSOR_NAME>   \
{                                                                                             \
   public:                                                                                    \
      explicit WSF_MESSAGE_TYPE(WsfPlatform* aPlatformPtr) : UCI_MessageType(aPlatformPtr)                                       { SetType(GetTypeId()); } \
      explicit WSF_MESSAGE_TYPE(UCI_Interface& aInterface) : UCI_MessageType(aInterface)                                         { SetType(GetTypeId()); } \
      WSF_MESSAGE_TYPE(UCI_Interface& aInterface, const uci::type::UCI_MESSAGE_TYPE& aType) : UCI_MessageType(aInterface, aType) { SetType(GetTypeId()); } \
      WSF_MESSAGE_TYPE(const WSF_MESSAGE_TYPE& aSrc) = default;                               \
      ~WSF_MESSAGE_TYPE() override = default;                                                 \
      WsfMessage* Clone() const override { return new WSF_MESSAGE_TYPE(*this); }              \
      static std::unique_ptr<WSF_MESSAGE_TYPE##Service> CreateMessageService(UCI_Interface&                           aInterface, \
                                                                             uci::base::AbstractServiceBusConnection* aAsbPtr,    \
                                                                             const char*                              aTopic)     \
      { return ut::make_unique<WSF_MESSAGE_TYPE##Service>(aInterface, aAsbPtr, aTopic); }                                  \
      const char* GetScriptClassName() const override { return #WSF_MESSAGE_TYPE; }           \
      static WsfStringId GetTypeId() { return #WSF_TYPE_NAME; }                               \
      static WsfStringId GetUCI_TypeName() { return #UCI_MESSAGE_TYPE; }                      \
};
// TODO ABOVE Change GetUCI_TypeName();
//=======================================================================================================
DEFINE_UCI_MESSAGE_TYPE(UCI_AMTI_ActivityMessage, AMTI_Activity, UCI_AMTI_ACTIVITY_MESSAGE, aMTI_ActivityMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_AMTI_CapabilityMessage, AMTI_Capability, UCI_AMTI_CAPABILITY_MESSAGE, aMTI_CapabilityMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_AMTI_CapabilityStatusMessage, AMTI_CapabilityStatus, UCI_AMTI_CAPABILITY_STATUS_MESSAGE, aMTI_CapabilityStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_AMTI_CommandMessage, AMTI_Command, UCI_AMTI_COMMAND_MESSAGE, aMTI_CommandMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_AMTI_CommandStatusMessage, AMTI_CommandStatus, UCI_AMTI_COMMAND_STATUS_MESSAGE, aMTI_CommandStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_AMTI_SettingsCommandMessage, AMTI_SettingsCommand, UCI_AMTI_SETTINGS_COMMAND_MESSAGE, aMTI_SettingsCommandMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_AMTI_SettingsCommandStatusMessage, AMTI_SettingsCommandStatus, UCI_AMTI_SETTINGS_COMMAND_STATUS_MESSAGE, aMTI_SettingsCommandStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_ControlRequestMessage, ControlRequest, UCI_CONTROL_REQUEST_MESSAGE, controlRequestMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_ControlRequestStatusMessage, ControlRequestStatus, UCI_CONTROL_REQUEST_STATUS_MESSAGE, controlRequestStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_ControlStatusMessage, ControlStatus, UCI_CONTROL_STATUS_MESSAGE, controlStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_DMPI_Message, DMPI, UCI_DMPI_MESSAGE, dMPI_MT)
DEFINE_UCI_MESSAGE_TYPE(UCI_DMPI_StatusMessage, DMPI_Status, UCI_DMPI_STATUS_MESSAGE, dMPI_StatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_EntityMessage, Entity, UCI_ENTITY_MESSAGE, entityMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_ESM_ActivityMessage, ESM_Activity, UCI_ESM_ACTIVITY_MESSAGE, eSM_ActivityMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_ESM_CapabilityMessage, ESM_Capability, UCI_ESM_CAPABILITY_MESSAGE, eSM_CapabilityMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_ESM_CapabilityStatusMessage, ESM_CapabilityStatus, UCI_ESM_CAPABILITY_STATUS_MESSAGE, eSM_CapabilityStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_ESM_CommandMessage, ESM_Command, UCI_ESM_COMMAND_MESSAGE, eSM_CommandMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_ESM_CommandStatusMessage, ESM_CommandStatus, UCI_ESM_COMMAND_STATUS_MESSAGE, eSM_CommandStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_ESM_SettingsCommandMessage, ESM_SettingsCommand, UCI_ESM_SETTINGS_COMMAND_MESSAGE, eSM_SettingsCommandMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_ESM_SettingsCommandStatusMessage, ESM_SettingsCommandStatus, UCI_ESM_SETTINGS_COMMAND_STATUS_MESSAGE, eSM_SettingsCommandStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_POST_ActivityMessage, POST_Activity, UCI_POST_ACTIVITY_MESSAGE, pOST_ActivityMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_POST_CapabilityMessage, POST_Capability, UCI_POST_CAPABILITY_MESSAGE, pOST_CapabilityMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_POST_CapabilityStatusMessage, POST_CapabilityStatus, UCI_POST_CAPABILITY_STATUS_MESSAGE, pOST_CapabilityStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_POST_CommandMessage, POST_Command, UCI_POST_COMMAND_MESSAGE, pOST_CommandMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_POST_CommandStatusMessage, POST_CommandStatus, UCI_POST_COMMAND_STATUS_MESSASGE, pOST_CommandStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_POST_SettingsCommandMessage, POST_SettingsCommand, UCI_POST_SETTINGS_COMMAND_MESSAGE, pOST_SettingsCommandMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_POST_SettingsCommandStatusMessage, POST_SettingsCommandStatus, UCI_POST_SETTINGS_COMMAND_STATUS_MESSAGE, pOST_SettingsCommandStatusMT)
//DEFINE_UCI_MESSAGE_TYPE(UCI_SMTI_ActivityMessage, SMTI_Activity, UCI_SMTI_ACTIVITY_MESSAGE, sMTI_ActivityMT)
//DEFINE_UCI_MESSAGE_TYPE(UCI_SMTI_CapabilityMessage, SMTI_Capability, UCI_SMTI_CAPABILITY_MESSAGE, sMTI_CapabilityMT)
//DEFINE_UCI_MESSAGE_TYPE(UCI_SMTI_CapabilityStatusMessage, SMTI_CapabilityStatus, UCI_SMTI_CAPABILITY_STATUS_MESSAGE, sMTI_CapabilityStatusMT)
//DEFINE_UCI_MESSAGE_TYPE(UCI_SMTI_CommandMessage, SMTI_Command, UCI_SMTI_COMMAND_MESSAGE, sMTI_CommandMT)
//DEFINE_UCI_MESSAGE_TYPE(UCI_SMTI_CommandStatusMessage, SMTI_CommandStatus, UCI_SMTI_COMMAND_STATUS_MESSAGE, sMTI_CommandStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_StrikeActivityMessage, StrikeActivity, UCI_STRIKE_ACTIVITY_MESSAGE, strikeActivityMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_StrikeCapabilityMessage, StrikeCapability, UCI_STRIKE_CAPABILITY_MESSAGE, strikeCapabilityMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_StrikeCapabilityStatusMessage, StrikeCapabilityStatus, UCI_STRIKE_CAPABILITY_STATUS_MESSAGE, strikeCapabilityStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_StrikeCommandMessage, StrikeCommand, UCI_STRIKE_COMMAND_MESSAGE, strikeCommandMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_StrikeCommandStatusMessage, StrikeCommandStatus, UCI_STRIKE_COMMAND_STATUS_MESSAGE, strikeCommandStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_SubsystemStatusMessage, SubsystemStatus, UCI_SUBSYSTEM_STATUS_MESSAGE, subsystemStatusMT)
DEFINE_UCI_MESSAGE_TYPE(UCI_SystemStatusMessage, SystemStatus, UCI_SYSTEM_STATUS_MESSAGE, systemStatusMT)

} // namespace wsf
#endif
