// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfUCI_PartComponent.hpp"

#include <string>

#include "UtLog.hpp"
#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{

UCI_PartComponent::UCI_PartComponent(const WsfScenario& aScenario,
                                     int                aPartType)
   : UCI_Component(aScenario, aPartType)
{
}

UCI_PartComponent::UCI_PartComponent(const UCI_PartComponent& aSrc)
   : UCI_Component(aSrc)
   , mCapabilityToUUID(aSrc.mCapabilityToUUID)
   , mUpdateMessageTime(aSrc.mUpdateMessageTime)
{
}

bool wsf::UCI_PartComponent::Initialize2(double aSimTime)
{
   try
   {
      if (IsActive())
      {
         auto& subsystemData = mSubsystemStatusMessagePtr->Get().getMessageData();
         auto subsystemUUID = mSubsystemUUID;
         if (mSubsystemUUID.empty())
         {
            subsystemUUID = GetInterface()->GetConnection()->getMySubsystemUUID().getValueAsString();
         }
         subsystemData.getSubsystemID().getUUID().setValue(subsystemUUID);
         if (mSubsystemLabel.empty())
         {
            mSubsystemLabel = UCI_Util::RemoveAngleBrackets(GetPlatform()->GetName() + ":" + GetName());
         }
         subsystemData.getSubsystemID().setDescriptiveLabel(mSubsystemLabel);

         subsystemData.getConfiguration().getAbout().setModel(mSubsystemLabel);

         auto& controlData = mControlStatusMessagePtr->Get().getMessageData();
         controlData.getSystemID().setUUID(GetInterface()->GetConnection()->getMySystemUUID());
         controlData.getSystemID().setDescriptiveLabel(mSubsystemLabel);

         auto& systemData = mSystemStatusMessagePtr->Get().getMessageData();
         systemData.getSystemID().setUUID(GetInterface()->GetConnection()->getMySystemUUID());
         systemData.setSource(uci::type::SystemSourceEnum::UCI_ACTUAL);
         systemData.setSystemState(uci::type::SystemStateEnum::UCI_OPERATIONAL);
         systemData.getSubsystemID().push_back(subsystemData.getSubsystemID());

         mControlStatusMessagePtr->Get().getMessageData().getSystemID().setDescriptiveLabel(mSubsystemLabel);

         UpdateAllMessages();
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }

   return UCI_Component::Initialize2(aSimTime);
}

bool UCI_PartComponent::ProcessInput(UtInput& aInput)
{
   auto command = aInput.GetCommand();
   bool myCommand{ true };
   if (command == "update_message_interval")
   {
      aInput.ReadValueOfType(mUpdateMessageTime, UtInput::cTIME);
   }
   else
   {
      myCommand = UCI_Component::ProcessInput(aInput);
   }
   return myCommand;
}

const int* UCI_PartComponent::GetComponentRoles() const
{
   static const int roles[] = { cWSF_COMPONENT_UCI_PART,
                                cWSF_COMPONENT_UCI,
                                cWSF_COMPONENT_PLATFORM_PART,
                                cWSF_COMPONENT_NULL
                              };
   return roles;
}

// virtual
void* UCI_PartComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_UCI_PART)
   {
      return this;
   }
   else
   {
      return UCI_Component::QueryInterface(aRole);
   }
}

bool wsf::UCI_PartComponent::CheckPartStatus(WsfArticulatedPart* aPartPtr)
{
   if (!aPartPtr->IsOperational())
   {
      if (DebugEnabled())
      {
         auto debug = ut::log::debug() << "Command REJECTED.";
         debug.AddNote() << "Part is not operational.";
         debug.AddNote() << "Name: " << aPartPtr->GetName();
      }
      UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Part not operational");
      return false;
   }

   if (!aPartPtr->IsTurnedOn())
   {
      if (DebugEnabled())
      {
         auto debug = ut::log::debug() << "Command REJECTED.";
         debug.AddNote() << "Part is not operational.";
         debug.AddNote() << "Name: " << aPartPtr->GetName();
      }
      UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Part turned off");
      return false;
   }

   if (mIsCommanded)
   {
      SetRejectedCurrentlyCommanded(aPartPtr->GetName());
      return false;
   }

   return true;
}

void UCI_PartComponent::SetRejectedCurrentlyCommanded(const std::string& aPartName)
{
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Command REJECTED.";
      debug.AddNote() << "Part is currently being commanded.";
      debug.AddNote() << "Part: " << aPartName;
   }
   UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Currently being commanded");
}

bool wsf::UCI_PartComponent::HandleControlRequestMessage(const UCI_ControlRequestMessage& aMessage)
{
   bool handled{ false };
   auto& data = aMessage.Get().getMessageData();
   auto& controlee = data.getControlee();

   for (size_t i = 0; i < controlee.size(); i++)
   {
      //Checks if this is the correct component
      if (controlee.at(i).getSystemID().getUUID() == GetInterface()->GetConnection()->getMySystemUUID())
      {
         if (i == 0)
         {
            handled = true;
         }
         if (data.getRequestType() == uci::type::ControlRequestEnum::UCI_ACQUIRE)
         {
            handled &= HandleAcquireMessage(aMessage);
         }
         else
         {
            handled &= HandleReleaseMessage(aMessage);
         }
      }
   }
   if (handled)
   {
      SendControlRequestStatusMessage(GetSimulation().GetSimTime(), data.getControlRequestID());
      SendControlStatusMessage(GetSimulation().GetSimTime(), uci::type::MessageStateEnum::UCI_UPDATE);
   }
   return handled;
}

bool wsf::UCI_PartComponent::HasControl(const std::string&         aCapbilityUUID, 
                                        uci::type::ServiceID_Type& aServiceID,
                                        uci::type::SystemID_Type&  aSystemID)
{
   bool hasControl{ false };
   auto serviceID = aServiceID.getUUID().getValueAsString();
   auto systemID = aSystemID.getUUID().getValueAsString();
   auto it = mPrimaryController.find(aCapbilityUUID);
   if (it != mPrimaryController.end())
   {
      if (serviceID == it->second.mServiceID &&
          systemID == it->second.mSystemID)
      {
         return true;
      }
   }
   
   auto secondIt = mSecondaryControllers.find(aCapbilityUUID);
   if (secondIt != mSecondaryControllers.end() &&
       !secondIt->second.empty())
   {
      auto iter = secondIt->second.find(Controller(systemID, serviceID));
      if (iter != secondIt->second.end())
      {
         return true;
      }
   }
   auto managerIt = mManagerControllers.find(aCapbilityUUID);
   if (managerIt != mManagerControllers.end() &&
       !managerIt->second.empty())
   {
      auto iter = managerIt->second.find(Controller(systemID, serviceID));
      if (iter != managerIt->second.end())
      {
         return true;
      }
   }
   return hasControl;
}

void UCI_PartComponent::SendControlRequestStatusMessage(double                                  aSimTime,
                                                        const uci::type::ControlRequestID_Type& aRequestID)
{
   try
   {
      if (CanHandleMessage())
      {
         UCI_ControlRequestStatusMessage msg(GetComponentParent());

         ISM_OwnerProducerType ownPro(GetComponentParent());
         ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
         msg.Get().getSecurityAttributesGroup().getOwnerProducer().push_back(ownPro.Get());

         auto& data = msg.Get().getMessageData();
         data.setControlRequestID(aRequestID);
         data.setApprovalType(GetApprovalType());
         data.enableRemarks();
         data.setRemarks(GetRemarks());
         SendMessage(aSimTime, msg);
         PrintMessageSent(aSimTime, "ControlRequestStatus");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "ControlRequestStatus",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

//=================================================================
//! Sends ControlStatusMessages. These messages describe the state of
//! who has the ability to command or control the sensor
void UCI_PartComponent::SendControlStatusMessage(double                                              aSimTime,
                                                 const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         mControlStatusMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mControlStatusMessagePtr);

         PrintMessageSent(aSimTime, "ControlStatus");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "ControlStatus",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

//==========================================================
//! Handles an acquire control request. A platform can request
//! to be a primary, secondary, or managing controller. A request
//! for primary ownership will remove the current primary owner
//!
//! @param aMessage The control request message that needs to be handled
bool wsf::UCI_PartComponent::HandleAcquireMessage(const UCI_ControlRequestMessage& aMessage)
{
   bool handled{ false };
   auto& data = aMessage.Get().getMessageData();
   auto controlType = data.getControlType().getValue();
   auto& messageSystemID = data.getController().getSystemID();
   auto controllerStr = messageSystemID.getUUID().getValueAsString();
   auto& capControl = mControlStatusMessagePtr->Get().getMessageData().getControlType().getCapabilityControl();
   auto capSize = capControl.size();
   auto& controleeList = data.getControlee();

   if (!data.getController().hasServiceID())
   {
      mApprovalType = uci::type::ApprovalEnum::EnumerationItem::UCI_REJECTED;
      mRemarks = "Request does not have service ID.";
      return true;
   }

   auto serviceID = data.getController().getServiceID().getUUID().getValueAsString();
   auto systemID = data.getController().getSystemID().getUUID().getValueAsString();

   mApprovalType = uci::type::ApprovalEnum::EnumerationItem::UCI_APPROVED;
   ClearRemarks();

   switch (controlType)
   {
      case uci::type::ControlTypeEnum::UCI_CAPABILITY_PRIMARY:
      {
         for (size_t i = 0; i < controleeList.size(); ++i)
         {
            if (controleeList.at(i).hasCapabilityID())
            {
               auto& controlee = controleeList.at(i);
               auto uuidStr = controlee.getCapabilityID().getUUID().getValueAsString();
               auto iter = mPrimaryController.find(uuidStr);
               if (iter != mPrimaryController.end())
               {
                  if (iter->second.mServiceID != serviceID ||
                      iter->second.mSystemID != systemID)
                  {
                     iter->second = Controller(systemID, serviceID);
                  }
                  else
                  {
                     return true;
                  }

                  for (size_t j = 0; j < capSize; ++j)
                  {
                     auto& controleeStatus = capControl.at(j);
                     if (controleeStatus.getCapabilityID().getUUID().getValueAsString() == uuidStr)
                     {
                        if (controleeStatus.hasPrimaryController())
                        {
                           controleeStatus.clearPrimaryController();
                        }
                        controleeStatus.enablePrimaryController();
                        controleeStatus.getPrimaryController().setServiceID(data.getController().getServiceID());
                        controleeStatus.getPrimaryController().setSystemID(messageSystemID);
                        handled = true;
                        break;
                     }
                  }
               }
            }
         }
         break;
      }
      case uci::type::ControlTypeEnum::UCI_CAPABILITY_SECONDARY:
      {
         for (size_t i = 0; i < controleeList.size(); ++i)
         {
            if (controleeList.at(i).hasCapabilityID())
            {
               auto& controlee = controleeList.at(i);
               auto uuidStr = controlee.getCapabilityID().getUUID().getValueAsString();
               auto iter = mSecondaryControllers.find(uuidStr);
               if (iter != mSecondaryControllers.end())
               {
                  Controller controller(systemID, serviceID);
                  if (!iter->second.emplace(controller).second)
                  {
                     return true;
                  }

                  for (size_t j = 0; j < capSize; ++j)
                  {
                     auto& controleeStatus = capControl.at(j);
                     if (controleeStatus.getCapabilityID().getUUID().getValueAsString() == uuidStr)
                     {
                        UCI_SecondaryControllerType secondaryController(GetComponentParent());
                        secondaryController.Get().setServiceID(data.getController().getServiceID());
                        secondaryController.Get().setSystemID(messageSystemID);
                        controleeStatus.getSecondaryController().push_back(secondaryController.Get());
                        handled = true;
                        break;
                     }
                  }
               }
               else
               {
                  auto& serviceID = data.getController().getServiceID();
                  auto& systemID = data.getController().getSystemID();
                  Controller controller(systemID.getUUID().getValueAsString(), serviceID.getUUID().getValueAsString());
                  std::set<Controller> set = { controller };
                  mSecondaryControllers.emplace(uuidStr, set);
                  for (size_t j = 0; j < capSize; ++j)
                  {
                     auto& controleeStatus = capControl.at(j);
                     if (controleeStatus.getCapabilityID().getUUID().getValueAsString() == uuidStr)
                     {
                        UCI_SecondaryControllerType secondaryController(GetComponentParent());
                        secondaryController.Get().setServiceID(data.getController().getServiceID());
                        secondaryController.Get().setSystemID(messageSystemID);
                        controleeStatus.getSecondaryController().push_back(secondaryController.Get());
                        break;
                     }
                  }
                  handled = true;
                  break;
               }
            }
         }
         break;
      }
      case uci::type::ControlTypeEnum::UCI_CAPABILITY_MANAGER:
      {
         for (size_t i = 0; i < controleeList.size(); ++i)
         {
            if (controleeList.at(i).hasCapabilityID())
            {
               auto& controlee = controleeList.at(i);
               auto uuidStr = controlee.getCapabilityID().getUUID().getValueAsString();
               auto iter = mManagerControllers.find(uuidStr);
               if (iter != mManagerControllers.end())
               {
                  Controller controller(systemID, serviceID);
                  if (!iter->second.emplace(controller).second)
                  {
                     return true;
                  }

                  for (size_t j = 0; j < capSize; ++j)
                  {
                     auto& controleeStatus = capControl.at(j);
                     if (controleeStatus.getCapabilityID().getUUID().getValueAsString() == uuidStr)
                     {
                        UCI_CapabilityManagerType manager(GetComponentParent());
                        manager.Get().setServiceID(data.getController().getServiceID());
                        manager.Get().setSystemID(messageSystemID);
                        controleeStatus.getCapabilityManager().push_back(manager.Get());
                        handled = true;
                        break;
                     }
                  }
               }
               else
               {
                  Controller controller(systemID, serviceID);
                  std::set<Controller> set = { controller };
                  mManagerControllers.emplace(uuidStr, set);
                  for (size_t j = 0; j < capSize; ++j)
                  {
                     auto& controleeStatus = capControl.at(j);
                     if (controleeStatus.getCapabilityID().getUUID().getValueAsString() == uuidStr)
                     {
                        UCI_CapabilityManagerType manager(GetComponentParent());
                        manager.Get().setServiceID(data.getController().getServiceID());
                        manager.Get().setSystemID(messageSystemID);
                        controleeStatus.getCapabilityManager().push_back(manager.Get());
                        handled = true;
                        break;
                     }
                  }
               }
            }
         }
         break;
      }
      case uci::type::ControlTypeEnum::UCI_MISSION:
      {
         //Not currently supported
         break;
      }
      default:
         break;
   }
   return handled;
}

//===================================================================
//! Handles request to relenquish control over a capability.

bool wsf::UCI_PartComponent::HandleReleaseMessage(const UCI_ControlRequestMessage& aMessage)
{
   bool handled{ false };
   auto& data = aMessage.Get().getMessageData();
   auto controlType = data.getControlType().getValue();
   auto& messageSystemID = data.getController().getSystemID();
   auto systemStr = messageSystemID.getUUID().getValueAsString();
   auto& controleeList = data.getControlee();

   if (!data.getController().hasServiceID())
   {
      mApprovalType = uci::type::ApprovalEnum::EnumerationItem::UCI_REJECTED;
      mRemarks = "Request does not have service ID.";
      return true;
   }

   auto& messageServiceID = data.getController().getServiceID();
   auto serviceStr = messageServiceID.getUUID().getValueAsString();

   mApprovalType = uci::type::ApprovalEnum::EnumerationItem::UCI_APPROVED;
   ClearRemarks();

   switch (controlType)
   {
      case uci::type::ControlTypeEnum::UCI_CAPABILITY_PRIMARY:
      {
         for (size_t i = 0; i < controleeList.size(); ++i)
         {
            auto& controlee = controleeList.at(i);
            if (controlee.hasCapabilityID())
            {
               handled = true;
               auto uuidStr = controlee.getCapabilityID().getUUID().getValueAsString();
               auto it = mPrimaryController.find(uuidStr);
               if (it != mPrimaryController.end())
               {
                  if (it->second.mServiceID == serviceStr &&
                      it->second.mSystemID == systemStr)
                  {
                     auto& capabilityControl = mControlStatusMessagePtr->Get().getMessageData().getControlType().getCapabilityControl();
                     for (int j = 0; capabilityControl.size(); ++j)
                     {
                        if (capabilityControl.at(j).getCapabilityID().getUUID().getValueAsString() == uuidStr)
                        {
                           capabilityControl.at(j).clearPrimaryController();
                        }
                     }
                  }
               }
            }
         }
         break;
      }
      case uci::type::ControlTypeEnum::UCI_CAPABILITY_SECONDARY:
      {
         for (size_t i = 0; i < controleeList.size(); ++i)
         {
            auto& controlee = controleeList.at(i);
            if (controlee.hasCapabilityID())
            {
               auto uuidStr = controlee.getCapabilityID().getUUID().getValueAsString();
               auto it = mSecondaryControllers.find(uuidStr);
               if (it != mSecondaryControllers.end())
               {
                  
                  auto controllerIt = it->second.find(Controller(systemStr, serviceStr));
                  if (controllerIt != it->second.end())
                  {
                     it->second.erase(controllerIt);
                     auto& capabilityControl = mControlStatusMessagePtr->Get().getMessageData().getControlType().getCapabilityControl();
                     UCI_ControlStatusCapabilityControlType controlStatusType(GetComponentParent());
                     for (size_t j = 0; j < capabilityControl.size(); ++j)
                     {
                        if (capabilityControl.at(j).getCapabilityID().getUUID().getValueAsString() == uuidStr)
                        {
                           auto& secondary = capabilityControl.at(j).getSecondaryController();
                           while (!secondary.empty())
                           {
                              auto& controller = secondary.at(secondary.size() - 1);
                              if (controller.getServiceID().getUUID() != data.getController().getServiceID().getUUID() ||
                                  controller.getSystemID().getUUID() != messageSystemID.getUUID())
                              {
                                 controlStatusType.Get().getSecondaryController().push_back(controller);
                              }
                              secondary.pop_back();
                           }
                           capabilityControl.at(j).setSecondaryController(controlStatusType.Get().getSecondaryController());
                           handled = true;
                           break;
                        }
                     }
                  }
               }
            }
         }
      }
      case uci::type::ControlTypeEnum::UCI_CAPABILITY_MANAGER:
      {
         for (size_t i = 0; i < controleeList.size(); ++i)
         {
            auto& controlee = controleeList.at(i);
            if (controlee.hasCapabilityID())
            {
               if (controlee.hasCapabilityID())
               {
                  auto uuidStr = controlee.getCapabilityID().getUUID().getValueAsString();
                  auto it = mManagerControllers.find(uuidStr);
                  if (it != mManagerControllers.end())
                  {
                     auto controllerIt = it->second.find(Controller(systemStr, serviceStr));
                     if (controllerIt != it->second.end())
                     {
                        it->second.erase(controllerIt);
                        auto& capabilityControl = mControlStatusMessagePtr->Get().getMessageData().getControlType().getCapabilityControl();
                        UCI_ControlStatusCapabilityControlType controlStatusType(GetComponentParent());
                        for (size_t j = 0; j < capabilityControl.size(); ++j)
                        {
                           if (capabilityControl.at(j).getCapabilityID().getUUID().getValueAsString() == uuidStr)
                           {
                              auto& capabilityManager = capabilityControl.at(j).getCapabilityManager();

                              while (!capabilityManager.empty())
                              {
                                 auto& controller = capabilityManager.at(capabilityManager.size() - 1);

                                 if (controller.getServiceID().getUUID() != data.getController().getServiceID().getUUID() ||
                                    controller.getSystemID().getUUID() != messageSystemID.getUUID())
                                 {
                                    controlStatusType.Get().getCapabilityManager().push_back(controller);
                                 }
                                 capabilityManager.pop_back();
                              }
                              capabilityControl.at(j).setCapabilityManager(controlStatusType.Get().getCapabilityManager());
                              handled = true;
                              break;
                           }
                        }
                     }
                  }
               }
            }
         }
         break;
      }
      case uci::type::ControlTypeEnum::UCI_MISSION:
      {
         //Not currently supported
         handled = true;
         break;
      }
      default:
         break;
   }
   return handled;
}

std::string wsf::UCI_PartComponent::GetApprovalTypeString(const uci::type::ApprovalEnum::EnumerationItem& aApprovalType)
{
   std::string retStr{ "UNKNOWN" };

   switch (aApprovalType)
   {
      case uci::type::ApprovalEnum::UCI_APPROVED:
         retStr = "APPROVED";
         break;
      case uci::type::ApprovalEnum::UCI_DELETED:
         retStr = "DELETED";
         break;
      case uci::type::ApprovalEnum::UCI_PENDING:
         retStr = "PENDING";
         break;
      case uci::type::ApprovalEnum::UCI_REJECTED:
         retStr = "REJECTED";
         break;
      default:
         break;
   }
   return retStr;
}

std::set<std::string> wsf::UCI_PartComponent::GetCapabilityUUIDs() const
{
   std::set<std::string> capabilities;
   for (const auto& uuid : mCapabilityToUUID)
   {
      capabilities.emplace(uuid.second);
   }
   return capabilities;
}

void wsf::UCI_PartComponent::SetOwnerProducer(const ISM_OwnerProducerType & aOwnerType)
{
   mControlStatusMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   mSubsystemStatusMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   mSystemStatusMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
}

void UCI_PartComponent::SendUpdateMessages(double                                              aSimTime,
                                           const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   SendActivityMessage(aSimTime, aMessageState);
   SendCapabilityMessage(aSimTime, aMessageState);
   SendCapabilityStatusMessage(aSimTime, aMessageState);
   SendControlStatusMessage(aSimTime, aMessageState);
   SendSubsystemStatusMessage(aSimTime, aMessageState);
   SendSystemStatusMessage(aSimTime, aMessageState);
}

//====================================================================
//! Process a received UCI message. Currently only handles Control
//! Request Messages.
//!
//! @param aSimTime The current simulation time
//! @param aMessage The message being processed
//! @return Returns true if the message is a known type; otherwise returns false.
bool wsf::UCI_PartComponent::ProcessMessage(double            aSimTime,
                                            const WsfMessage& aMessage)
{
   bool processed{ false };
   if (CanHandleMessage())
   {
      auto type = aMessage.GetType();
      if (type == UCI_ControlRequestMessage::GetTypeId())
      {
         const UCI_ControlRequestMessage& message = dynamic_cast<const UCI_ControlRequestMessage&>(aMessage);
         HandleControlRequestMessage(message);
         processed = true;
      }
   }
   return processed;
}

void UCI_PartComponent::SendSubsystemStatusMessage(double                                              aSimTime,
                                                   const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         mSubsystemStatusMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mSubsystemStatusMessagePtr);

         if (DebugEnabled())
         {
            auto debug = ut::log::debug() << "Message sent.";
            debug.AddNote() << "T= " << aSimTime;
            debug.AddNote() << "Message Type: SubsystemStatus";
         }
      }
      else if (DebugEnabled())
      {
         auto debug = ut::log::debug() << "Message not sent.";
         debug.AddNote() << "Component is turned off and/or not operational.";
         debug.AddNote() << "Component: " << GetName();
         debug.AddNote() << "Message Type: SubsystemStatus";
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void UCI_PartComponent::SendSystemStatusMessage(double                                              aSimTime,
                                                const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         mSystemStatusMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mSystemStatusMessagePtr);

         if (DebugEnabled())
         {
            auto debug = ut::log::debug() << "Message sent.";
            debug.AddNote() << "T= " << aSimTime;
            debug.AddNote() << "Message Type: SystemStatus";
            debug.AddNote() << "Status: OPERATIONAL";
         }
      }
      else if (DebugEnabled())
      {
         auto debug = ut::log::debug() << "Message not sent.";
         debug.AddNote() << "Component is turned off and/or not operational.";
         debug.AddNote() << "Component: " << GetName();
         debug.AddNote() << "Message Type: SystemStatus";
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void wsf::UCI_PartComponent::TurnOff(double aSimTime)
{
   WsfPlatformPart::TurnOff(aSimTime);
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Component turned off.";
      debug.AddNote() << "T= " << aSimTime;
      debug.AddNote() << "Component: " << GetName();
   }
}

void wsf::UCI_PartComponent::TurnOn(double aSimTime)
{
   if (CanBeTurnedOn())
   {
      WsfPlatformPart::TurnOn(aSimTime);

      if (DebugEnabled())
      {
         auto debug = ut::log::debug() << "Component turned on.";
         debug.AddNote() << "T= " << aSimTime;
         debug.AddNote() << "Component: " << GetName();
      }

      auto messageState = uci::type::MessageStateEnum::UCI_UPDATE;
      if (!mInitialMessagesSent)
      {
         mInitialMessagesSent = true;
         messageState = uci::type::MessageStateEnum::UCI_NEW;
      }

      SendUpdateMessages(aSimTime, messageState);

      // Create recurring event to send update messages
      auto updateMessageEvent = ut::make_unique<UCI_Message_Event>(this,
                                                                   aSimTime + GetUpdateMessageTime(),
                                                                   GetPlatform()->GetIndex(),
                                                                   GetUpdateMessageTime());
      GetSimulation().AddEvent(std::move(updateMessageEvent));
   }
}

void UCI_PartComponent::UpdateSubsystemStatusMessage(uci::type::SubsystemStateEnum::EnumerationItem aState)
{
   try
   {
      mSubsystemStatusMessagePtr->Get().getMessageData().getSubsystemState().setValue(aState);
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void wsf::UCI_PartComponent::PrintMessageSent(double             aSimTime,
                                              const std::string& aMessageType)
{
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Message sent.";
      debug.AddNote() << "T= " << aSimTime;
      debug.AddNote() << "Message Type: " << aMessageType;
   }
}

void wsf::UCI_PartComponent::PrintMessageNotSent(double             aSimTime,
                                                 const std::string& aMessageType,
                                                 const std::string& aReason)
{
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Message not sent.";
      debug.AddNote() << "Message Type: " << aMessageType;
      debug.AddNote() << "Reason: " << aReason;
   }
}

// ================================================================================================
//! Sends periodic update messages at a set interval if the corresponding platform still exists
//! in the simulation.
WsfEvent::EventDisposition wsf::UCI_Message_Event::Execute()
{
   auto disposition = WsfEvent::cRESCHEDULE;
   if (GetSimulation()->GetPlatformByIndex(mPlatformIndex) != nullptr &&
       mUCI_ComponentPtr->CanHandleMessage())
   {
      mUCI_ComponentPtr->SendUpdateMessages(GetTime(), uci::type::MessageStateEnum::UCI_UPDATE);
      SetTime(GetTime() + mSendInterval);
   }
   else
   {
      disposition = WsfEvent::cDELETE;
   }
   return disposition;
}

} // wsf
