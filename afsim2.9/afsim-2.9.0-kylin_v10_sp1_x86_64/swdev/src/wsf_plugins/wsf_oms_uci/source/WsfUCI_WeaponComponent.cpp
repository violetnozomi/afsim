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

#include "WsfUCI_WeaponComponent.hpp"

#include <string>
#include <sstream>
#include <vector>

#include "UtLog.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfGuidedMover.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfUCI_Utils.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

namespace wsf
{

UCI_WeaponComponent::UCI_WeaponComponent(const WsfScenario& aScenario)
   : UCI_PartComponent(aScenario, cWSF_COMPONENT_UCI_WEAPON)
{
}

UCI_WeaponComponent::UCI_WeaponComponent(const UCI_WeaponComponent& aSrc)
   : UCI_PartComponent(aSrc)
   , mStoreInfo(aSrc.mStoreInfo)
{
}


const int* UCI_WeaponComponent::GetComponentRoles() const
{
   static const int roles[] = { cWSF_COMPONENT_UCI_WEAPON,
                                cWSF_COMPONENT_UCI_PART,
                                cWSF_COMPONENT_UCI,
                                cWSF_COMPONENT_PLATFORM_PART,
                                cWSF_COMPONENT_NULL
                              };
   return roles;
}

bool wsf::UCI_WeaponComponent::ProcessInput(UtInput& aInput)
{
   bool myCommand{ true };
   std::string command = aInput.GetCommand();
   if (command == "capability")
   {
      UtInputBlock capabilityBlock(aInput);
      std::string capability;
      aInput.ReadCommand(capability);
      while (capabilityBlock.ReadCommand(command))
      {
         if (command == "uuids")
         {
            UtInputBlock idBlock(aInput);
            std::string name;
            int count = 1;
            while (idBlock.ReadCommand(command))
            {
               name = capability + "_" + std::to_string(count);
               mCapabilityToUUID[name] = command;
               count++;
            }
         }
         else if (command == "store_category")
         {
            std::string category;
            aInput.ReadCommand(category);
            mStoreInfo[capability].mStoreCategory = GetStoreCategoryEnum(category);
         }
         else if (command == "store_type")
         {
            aInput.ReadValue(mStoreInfo[capability].mStoreType);
         }
         else
         {
            throw UtInput::BadValue(aInput);
         }
      }
   }
   else
   {
      myCommand = UCI_PartComponent::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void* UCI_WeaponComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_UCI_WEAPON)
   {
      return this;
   }
   return UCI_PartComponent::QueryInterface(aRole);
}

// virtual
WsfComponent* UCI_WeaponComponent::CloneComponent() const
{
   return new UCI_WeaponComponent(*this);
}

bool wsf::UCI_WeaponComponent::Initialize2(double aSimTime)
{
   try
   {
      if (IsActive())
      {
         mActivityMessagePtr = ut::make_unique<UCI_StrikeActivityMessage>(GetComponentParent());
         mCapabilityMessagePtr = ut::make_unique<UCI_StrikeCapabilityMessage>(GetComponentParent());
         mCapabilityStatusMessagePtr = ut::make_unique<UCI_StrikeCapabilityStatusMessage>(GetComponentParent());
         mControlStatusMessagePtr = ut::make_unique<UCI_ControlStatusMessage>(GetComponentParent());
         mSubsystemStatusMessagePtr = ut::make_unique<UCI_SubsystemStatusMessage>(GetComponentParent());
         mSystemStatusMessagePtr = ut::make_unique<UCI_SystemStatusMessage>(GetComponentParent());

         ISM_OwnerProducerType ownPro(GetComponentParent());
         ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
         SetOwnerProducer(ownPro);

         mControlStatusMessagePtr->Get().getMessageData().getControlType().setControlStatusControlTypeChoiceOrdinal(
            uci::type::ControlStatusControlType::CONTROLSTATUSCONTROLTYPE_CHOICE_CAPABILITYCONTROL);

         auto& activityData = mActivityMessagePtr->Get().getMessageData();
         auto& capData = mCapabilityMessagePtr->Get().getMessageData();
         auto& capStatusData = mCapabilityStatusMessagePtr->Get().getMessageData();

         for (WsfComponentList::RoleIterator<WsfWeapon> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
         {
            auto weaponName = iter->GetName();
            int storeType{ GetStoreType(weaponName) };
            auto storeCategory = GetStoreCategory(weaponName);      
            if (storeType < 0 ||
                storeCategory == uci::type::StoreCategoryEnum::enumNotSet)
            {
               auto warning = ut::log::warning() << "Weapon's \"store_type\" and/or \"store_category\" is undefined.";
               warning.AddNote() << "This weapon will be ignored by all UCI functionality.";
               warning.AddNote() << "Weapon: " << weaponName;
               mIgnoredWeapons.emplace(weaponName);
               continue;
            }
            for (int i = 1; i <= iter->GetInitialStateQuantityRemaining(); ++i)
            {
               //For the Weapon component, every weapon on the platform is a capability.
               mWeaponStatus.emplace(iter->GetName(), WeaponStatus());
               std::string capabilityUUID = GetInterface()->GetConnection()->generateUUID().getValueAsString();
               std::string name = weaponName + "_" + std::to_string(i);
               auto it = mCapabilityToUUID.find(name);
               if (it != mCapabilityToUUID.end())
               {
                  capabilityUUID = it->second;
               }
               else
               {
                  mCapabilityToUUID.emplace(name, capabilityUUID);
               }
               mUUIDtoCapability.emplace(capabilityUUID, name);
               std::string label = UCI_Util::RemoveAngleBrackets(GetPlatform()->GetName() + ":" + name);

               UCI_StrikeCapabilityType cap(GetComponentParent());
               cap.Get().getCapabilityID().getUUID().setValue(capabilityUUID);
               cap.Get().getCapabilityID().setDescriptiveLabel(label);

               cap.Get().getCapabilityType().setStoreType(storeType >= 0 ? storeType : 0);
               cap.Get().getCapabilityType().setStoreCategory(storeCategory);

               UCI_CapabilityControlInterfaceEnumType controlInterfaceEnum(GetComponentParent());
               controlInterfaceEnum.Get().setValue(uci::type::CapabilityControlInterfacesEnum::UCI_ACTIVITY_COMMAND);
               cap.Get().getAcceptedInterface().push_back(controlInterfaceEnum.Get());
               controlInterfaceEnum.Get().setValue(uci::type::CapabilityControlInterfacesEnum::UCI_CONSENT);
               cap.Get().getAcceptedInterface().push_back(controlInterfaceEnum.Get());
               controlInterfaceEnum.Get().setValue(uci::type::CapabilityControlInterfacesEnum::UCI_MISSION_PLAN_ACTION);
               cap.Get().getAcceptedInterface().push_back(controlInterfaceEnum.Get());
               controlInterfaceEnum.Get().setValue(uci::type::CapabilityControlInterfacesEnum::UCI_MISSION_PLAN_TASK);
               cap.Get().getAcceptedInterface().push_back(controlInterfaceEnum.Get());

               cap.Get().getCapabilityOptions().setInterruptOtherActivities(true);

               capData.getCapability().push_back(cap.Get());

               UCI_StrikeCapabilityStatusType capStatus(GetComponentParent());
               capStatus.Get().setCapabilityID(cap.Get().getCapabilityID());

               auto weaponPtr(GetPlatform()->GetComponent<WsfWeapon>(iter->GetName()));
               capStatus.Get().getAvailabilityInfo().setAvailability(GetWeaponAvailability(weaponPtr, capabilityUUID));

               capStatusData.getCapabilityStatus().push_back(capStatus.Get());

               mSubsystemStatusMessagePtr->Get().getMessageData().getConfiguration().getCapabilityID().push_back(cap.Get().getCapabilityID());

               UCI_ControlStatusCapabilityControlType capabilityControl(GetComponentParent());
               capabilityControl.Get().setCapabilityID(cap.Get().getCapabilityID());
               capabilityControl.Get().getAcceptedInterface().push_back(controlInterfaceEnum.Get());
               capabilityControl.Get().enablePrimaryController();
               auto& primaryController = capabilityControl.Get().getPrimaryController();
               auto& serviceID = GetInterface()->GetConnection()->getMyServiceUUID();
               auto& systemID = GetInterface()->GetConnection()->getMySystemUUID();
               primaryController.getServiceID().setUUID(serviceID);
               primaryController.getSystemID().setUUID(systemID);
               primaryController.getSystemID().setDescriptiveLabel(GetInterface()->GetConnection()->getMySystemLabel());

               mPrimaryController[capabilityUUID] = Controller(systemID.getValueAsString(), serviceID.getValueAsString());

               mControlStatusMessagePtr->Get().getMessageData().getControlType().getCapabilityControl().push_back(capabilityControl.Get());
            }
         }

         if (mSubsystemUUID.empty())
         {
            mSubsystemUUID = GetInterface()->GetConnection()->getMySubsystemUUID().getValueAsString();
         }
         activityData.getSubsystemID().getUUID().setValue(mSubsystemUUID);
         mCallbacks.Add(WsfObserver::WeaponTerminated(&GetSimulation()).Connect(&UCI_WeaponComponent::WeaponTerminatedCB, this));
         mCallbacks.Add(WsfObserver::WeaponHit(&GetSimulation()).Connect(&UCI_WeaponComponent::WeaponHitCB, this));
         mCallbacks.Add(WsfObserver::WeaponMissed(&GetSimulation()).Connect(&UCI_WeaponComponent::WeaponMissedCB, this));
         mCallbacks.Add(WsfObserver::WeaponFired(&GetSimulation()).Connect(&UCI_WeaponComponent::WeaponFiredCB, this));
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }

   return UCI_PartComponent::Initialize2(aSimTime);
}

bool UCI_WeaponComponent::ReceiveMessage(double            aSimTime,
                                         const WsfMessage& aMessage)
{
   bool received{ false };
   if (CanHandleMessage())
   {
      auto type = aMessage.GetType();

      if (type == UCI_StrikeCommandMessage::GetTypeId())
      {
         const auto& message = dynamic_cast<const UCI_StrikeCommandMessage&>(aMessage);
         received = HandleCommandMessage(aSimTime, message);
      }
      else if (type == UCI_DMPI_Message::GetTypeId())
      {
         const auto& message = dynamic_cast<const UCI_DMPI_Message&>(aMessage);
         received = HandleDMPI_Message(aSimTime, message);
      }
      else
      {
         received = UCI_PartComponent::ReceiveMessage(aSimTime, aMessage);
      }
   }
   else if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Message not received.";
      debug.AddNote() << "Component is turned off and/or not operational.";
      debug.AddNote() << "Component: " << GetName();
   }
   return received;
}

int wsf::UCI_WeaponComponent::GetStoreType(const std::string& aWeapon)
{
   int storeType{ -1 };
   auto it = mStoreInfo.find(aWeapon);
   if (it != mStoreInfo.end())
   {
      storeType = it->second.mStoreType;
   }
   return storeType;
}

uci::type::StoreCategoryEnum::EnumerationItem wsf::UCI_WeaponComponent::GetStoreCategory(const std::string& aWeapon)
{
   uci::type::StoreCategoryEnum::EnumerationItem category{ uci::type::StoreCategoryEnum::enumNotSet};
   auto it = mStoreInfo.find(aWeapon);
   if (it != mStoreInfo.end())
   {
      category = it->second.mStoreCategory;
   }
   return category;
}

uci::type::StoreCategoryEnum::EnumerationItem wsf::UCI_WeaponComponent::GetStoreCategoryEnum(const std::string& aCategory)
{
   uci::type::StoreCategoryEnum::EnumerationItem category{ uci::type::StoreCategoryEnum::enumNotSet};
   if (aCategory == "air")
   {
      category = uci::type::StoreCategoryEnum::UCI_AIR;
   }
   else if (aCategory == "ground")
   {
      category = uci::type::StoreCategoryEnum::UCI_GROUND;
   }
   else if (aCategory == "gun")
   {
      category = uci::type::StoreCategoryEnum::UCI_GUN;
   }
   else if (aCategory == "sea_subsurface")
   {
      category = uci::type::StoreCategoryEnum::UCI_SEA_SUBSURFACE;
   }
   else if (aCategory == "sea_surface")
   {
      category = uci::type::StoreCategoryEnum::UCI_SEA_SURFACE;
   }
   else if (aCategory == "sea_surface_samssm")
   {
      category = uci::type::StoreCategoryEnum::UCI_SEA_SURFACE_SAMSSM;
   }
   return category;
}

void wsf::UCI_WeaponComponent::WeaponFireResults(double                     aSimTime,
                                                 const WsfWeaponEngagement* aEngagementPtr,
                                                 WsfPlatform*               aTargetPlatformPtr,
                                                 const std::string&         aStatus)
{
   if (aEngagementPtr != nullptr)
   {
      auto it = mSerialToActivity.find(aEngagementPtr->GetSerialNumber());
      if (it != mSerialToActivity.end())
      {
         auto& activity = mActivityMessagePtr->Get().getMessageData().getActivity();
         for (size_t i = 0; i < activity.size(); ++i)
         {
            auto& activityID = activity.at(i).getActivityID();
            if (it->second.Get().getActivityID().getUUID() == activityID.getUUID())
            {
               activity.at(i).setActivityState(uci::type::ActivityStateEnum::UCI_COMPLETED);
            }
         }

         if (DebugEnabled())
         {
            auto debug = ut::log::debug() << "Strike command completed.";
            debug.AddNote() << "Weapon " << aStatus << '.';
            debug.AddNote() << "T = " << aSimTime;
            debug.AddNote() << "Activity ID: " << it->second.Get().getActivityID().getUUID().getValueAsString();
            debug.AddNote() << "Capability ID: " << it->second.Get().getCapabilityID().at(0).getUUID().getValueAsString();
            std::string geometricResult{ "Unknown" };
            aEngagementPtr->EnumToString(geometricResult, aEngagementPtr->GetGeometryResult());
            debug.AddNote() << "Geometric Result: " << geometricResult;
            if (aTargetPlatformPtr != nullptr)
            {
               debug.AddNote() << "Target: " << aTargetPlatformPtr->GetName();
            }
         }

         SendActivityMessage(aSimTime, uci::type::MessageStateEnum::UCI_UPDATE);
         mSerialToActivity.erase(it);
         mSendActivityMessage = !mSerialToActivity.empty();

         //Remove all completed Activities
         UCI_StrikeActivityMessage activityMsg(GetComponentParent());
         auto& newActivity = activityMsg.Get().getMessageData().getActivity();
         for (size_t i = 0; i < activity.size(); ++i)
         {
            if (activity.at(i).getActivityState() != uci::type::ActivityStateEnum::UCI_COMPLETED)
            {
               newActivity.push_back(activity.at(i));
            }
         }
         mActivityMessagePtr->Get().getMessageData().setActivity(newActivity);
      }
   }

}

bool wsf::UCI_WeaponComponent::SetFireLocation(double             aSimTime,
                                               WsfWeapon*         aWeaponPtr,
                                               const std::string& aUUID)
{
   bool set{ false };
   auto it = mDMPI_Messages.find(aUUID);
   if (it != mDMPI_Messages.end())
   {
      auto launchComputerPtr = aWeaponPtr->GetLaunchComputer();
      if (launchComputerPtr != nullptr)
      {
         launchComputerPtr->ClearCommandedValues();

         auto& coordinates = it->second.first.Get().getMessageData().getDMPI_Location().getDMPI_Coordinates();
         auto lat = coordinates.getLatitude() * UtMath::cDEG_PER_RAD;
         auto lon = coordinates.getLongitude() * UtMath::cDEG_PER_RAD;
         auto alt = coordinates.getAltitude() * UtMath::cDEG_PER_RAD;
         WsfGeoPoint targetLoc(lat, lon, alt);
         auto testComputerPtr = launchComputerPtr->Clone();
         testComputerPtr->SetTargetImpactPoint(targetLoc);
         set = testComputerPtr->TargetImpactPointIsValid();
         delete testComputerPtr;
         if (set)
         {
            launchComputerPtr->SetTargetImpactPoint(targetLoc);
         }
         else
         {
            if (DebugEnabled())
            {
               auto debug = ut::log::debug() << "Command REJECTED.";
               debug.AddNote() << "The coordinates for the given DMPI command are invalid for this weapon's launch computer.";
               debug.AddNote() << "T = " << aSimTime;
               debug.AddNote() << "DMPI ID: " << aUUID;
               debug.AddNote() << "Weapon: " << aWeaponPtr->GetName();
               auto location = debug.AddNote() << "Target coordinates: ";
               location.AddNote() << "Latitude: " << UtLatPos(coordinates.getLatitude() * UtMath::cDEG_PER_RAD, UtAngle::FmtDMS + 2);
               location.AddNote() << "Longitude: " << UtLonPos(coordinates.getLongitude() * UtMath::cDEG_PER_RAD, UtAngle::FmtDMS + 2);
               location.AddNote() << "Altitude: " << coordinates.getAltitude() * UtMath::cDEG_PER_RAD << " m";
            }
            UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Invalid coordinates.");
         }
      }
      else
      {
         if (DebugEnabled())
         {
            auto debug = ut::log::debug() << "Command REJECTED.";
            debug.AddNote() << "The weapon does not have a guided mover or launch computer to control the target location.";
            debug.AddNote() << "T = " << aSimTime;
            debug.AddNote() << "DMPI ID: " << aUUID;
            debug.AddNote() << "Weapon: " << aWeaponPtr->GetName();
         }
         UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "No guided mover or launch computer");
      }
   }
   return set;
}

void wsf::UCI_WeaponComponent::WeaponHitCB(double                     aSimTime,
                                           const WsfWeaponEngagement* aEngagementPtr,
                                           WsfPlatform*               aTargetPlatformPtr)
{
   WeaponFireResults(aSimTime, aEngagementPtr, aTargetPlatformPtr, "hit");
}

void wsf::UCI_WeaponComponent::WeaponTerminatedCB(double                     aSimTime,
                                                  const WsfWeaponEngagement* aEngagementPtr)
{
   WeaponFireResults(aSimTime, aEngagementPtr, nullptr, "terminated");
}

void wsf::UCI_WeaponComponent::WeaponMissedCB(double                     aSimTime,
                                              const WsfWeaponEngagement* aEngagementPtr,
                                              WsfPlatform*               aTargetPlatformPtr)
{
   WeaponFireResults(aSimTime, aEngagementPtr, aTargetPlatformPtr, "missed");
}

void wsf::UCI_WeaponComponent::WeaponFiredCB(double                     aSimTime,
                                             const WsfWeaponEngagement* aEngagementPtr,
                                             const WsfTrack*            aTargetPtr)
{
   if (aEngagementPtr != nullptr &&
       !mCurrentlyFiring)
   {
      auto weaponName = aEngagementPtr->GetWeaponSystemName();
      auto weaponPtr(GetPlatform()->GetComponent<WsfWeapon>(weaponName));
      if (weaponPtr)
      {
         int lastFired = static_cast<int>(weaponPtr->GetTotalQuantityUsed()) + 1;
         auto capabilityName = weaponName + "_" + std::to_string(lastFired);
         auto capability = mCapabilityToUUID.find(capabilityName);
         mFiredCapabilities.emplace(capability->second);
      }
   }
}

void wsf::UCI_WeaponComponent::UpdateCapabilityStatus()
{
   int count{ 0 };
   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      auto weaponPtr(GetPlatform()->GetComponent<WsfWeapon>(iter->GetName()));
      if (mIgnoredWeapons.find(iter->GetName()) != mIgnoredWeapons.end())
      {
         continue;
      }
      for (int i = 1; i <= iter->GetInitialStateQuantityRemaining(); ++i)
      {
         auto name = weaponPtr->GetName() + "_" + std::to_string(i);
         auto it = mCapabilityToUUID.find(name);
         mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().at(count).getAvailabilityInfo().setAvailability(GetWeaponAvailability(weaponPtr, it->second));
         ++count;
      }
   }
}

void wsf::UCI_WeaponComponent::SetOwnerProducer(const ISM_OwnerProducerType& aOwnerType)
{
   mActivityMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   mCapabilityMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   mCapabilityStatusMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   UCI_PartComponent::SetOwnerProducer(aOwnerType);
}

std::string wsf::UCI_WeaponComponent::GetCommandTypeString(uci::type::StrikeWeaponCommandType::StrikeWeaponCommandTypeChoice aType)
{
   std::string type{ "None" };
   switch (aType)
   {
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTFORKEYLOAD:
         type = "Select for key load";
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_ASSIGNDMPI_ID:
         type = "Assign DMPI ID";
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_WEAPONARM:
         type = "Weapon arm";
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTFORJETTISON:
         type = "Select for jettison";
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_GENERATEDYNAMICLAR:
         type = "Generate Dynamic LAR";
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTFORRELEASE:
         type = "Select for release";
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTAO_CODE:
         type = "Select AO code";
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_NONE:
      default:
         break;
   }
   return type;
}

bool wsf::UCI_WeaponComponent::HandleCommandMessage(double                          aSimTime,
                                                    const UCI_StrikeCommandMessage& aMessage)
{
   bool handled{ false };
   auto& data = aMessage.Get().getMessageData();
   auto commandSize = data.getCommand().size();
   auto& msgHeader = aMessage.Get().getSequenceGroup().getMessageHeader();
   auto& msgState = aMessage.Get().getMessageState();
   for (size_t i = 0; i < commandSize; ++i)
   {
      auto& command = data.getCommand().at(i);

      if (command.getStrikeCommandTypeChoiceOrdinal() == uci::type::StrikeCommandType::STRIKECOMMANDTYPE_CHOICE_CAPABILITY)
      {
         auto capabilityId = command.getCapability().getCapabilityID().getUUID().getValueAsString();
         if (HasControl(capabilityId, msgHeader.getServiceID(), msgHeader.getSystemID()) &&
             HandleCapabilityCommand(aSimTime, command.getCapability()))
         {
            if (i == 0)
            {
               handled = true;
            }
            SendCommandStatusMessage<UCI_StrikeCommandStatusMessage, uci::type::CapabilityCommandID_Type>(aSimTime, msgState.getValue(), command.getCapability().getCommandID());
            if (IsCommandAccepted())
            {
               CommandWeapon(aSimTime, command.getCapability());
               SendActivityMessage(aSimTime, uci::type::MessageStateEnum::UCI_NEW);
            }
         }
         else
         {
            handled = false;
         }
      }
      else
      {
         if (HandleActivityCommand(aSimTime, command.getActivity()))
         {
            if (i == 0)
            {
               handled = true;
            }
            SendCommandStatusMessage<UCI_StrikeCommandStatusMessage, uci::type::CapabilityCommandID_Type>(aSimTime, msgState.getValue(), command.getActivity().getCommandID());
         }
         else
         {
            handled = false;
         }
      }
   }
   return handled;
}

bool wsf::UCI_WeaponComponent::HandleCapabilityCommand(double                                        aSimTime,
                                                       const uci::type::StrikeCapabilityCommandType& aCapabilityCommand)
{
   bool handled{ false };
   auto& capabilityUUID = aCapabilityCommand.getCapabilityID();
   auto uuidStr = capabilityUUID.getUUID().getValueAsString();
   auto it = mUUIDtoCapability.find(uuidStr);
   //Check if the capability exists on the platform
   if (it != mUUIDtoCapability.end())
   {
      handled = true;

      auto weaponName(GetWeaponName(it->second));
      auto weaponPtr(GetPlatform()->GetComponent<WsfWeapon>(weaponName));
      if (weaponPtr)
      {
         auto& commandType = aCapabilityCommand.getCommandType();
         bool isDMPI{ commandType.getStrikeWeaponCommandTypeChoiceOrdinal() == uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_ASSIGNDMPI_ID };
        
         if (!CheckPartStatus(weaponPtr) || 
            !WeaponAvailable(weaponPtr, uuidStr))
         {
            return handled;
         }
         if (isDMPI)
         {
            auto id = commandType.getAssignDMPI_ID().getUUID().getValueAsString();
            if (!MeetsDMPI_Requirements(id))
            {
               return handled;
            }
         }
   
         if (isDMPI && !CanFire(aSimTime, weaponPtr, commandType.getAssignDMPI_ID().getUUID().getValueAsString()))
         {
            if (DebugEnabled())
            {
               auto debug = ut::log::debug() << "Command REJECTED.";
               debug.AddNote() << "Weapon cannot fire at the given target. Ensure the weapon has a launch computer,"
                               << " the coordinates are valid and are within the weapon's range,"
                               << " and the DMPI has been allocated.";
               debug.AddNote() << "Weapon: " << weaponName;
               auto target = debug.AddNote() << "Target:";
               auto it = mDMPI_Messages.find(commandType.getAssignDMPI_ID().getUUID().getValueAsString());
               auto& coordinates = it->second.first.Get().getMessageData().getDMPI_Location().getDMPI_Coordinates();
               target.AddNote() << "Latitude: " << UtLatPos(coordinates.getLatitude() * UtMath::cDEG_PER_RAD);
               target.AddNote() << "Longitude: " << UtLonPos(coordinates.getLongitude() * UtMath::cDEG_PER_RAD);
               target.AddNote() << "Altitude: " << coordinates.getAltitude() << " m";
            }
            UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Weapon cannot fire at target.");
            return handled;
         }
         
         UpdateCommandResults(uci::type::CommandStateEnum::UCI_ACCEPTED, "Accepted");
      }
      else
      {
         if (DebugEnabled())
         {
            auto debug = ut::log::debug() << "Command REJECTED.";
            debug.AddNote() << "Weapon does not exist.";
            debug.AddNote() << "Weapon: " << weaponName;
         }
         UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Weapon does not exist");
      }

   }
   return handled;
}

bool UCI_WeaponComponent::HandleActivityCommand(double                                      aSimTime,
                                                const uci::type::StrikeActivityCommandType& aActivityCommand)
{
   bool handled{ false };
   auto it = mActivityToCabability.find(aActivityCommand.getActivityID().getUUID().getValueAsString());
   if (it != mActivityToCabability.end())
   {
      auto modeIt = mCapabilityToUUID.find(it->second);
      if (modeIt != mCapabilityToUUID.end())
      {
         handled = true;
         if (DebugEnabled())
         {
            auto debug = ut::log::debug() << "Command REJECTED.";
            debug.AddNote() << "Activity commands are currently unavailable.";
         }
         UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Activity commands unavailable");
      }
   }
   return handled;
}

bool wsf::UCI_WeaponComponent::HandleDMPI_Message(double                  aSimTime,
                                                  const UCI_DMPI_Message& aMessage)
{
   bool handled{ GetInterface() != nullptr };
   if (handled)
   {
      auto& dmpiId = aMessage.Get().getMessageData().getDMPI_ID();
      auto id = dmpiId.getUUID().getValueAsString();
      mDMPI_Messages.emplace(id, std::make_pair(aMessage, uci::type::DMPI_StoreStateEnum::UCI_ALLOCATED));
      SendDMPI_StatusMessage(aSimTime,
                             uci::type::MessageStateEnum::UCI_NEW,
                             dmpiId,
                             uci::type::DMPI_StoreStateEnum::UCI_ALLOCATED);
   }
   return handled;
}

bool wsf::UCI_WeaponComponent::CommandWeapon(double                                        aSimTime,
                                             const uci::type::StrikeCapabilityCommandType& aCapabilityCommand)
{
   auto& capabilityUUID = aCapabilityCommand.getCapabilityID();
   auto uuidStr = capabilityUUID.getUUID().getValueAsString();
   auto it = mUUIDtoCapability.find(uuidStr);
   auto& commandType = aCapabilityCommand.getCommandType();
   auto weaponName(GetWeaponName(it->second));
   auto weaponPtr(GetPlatform()->GetComponent<WsfWeapon>(weaponName));
   auto ordinal = commandType.getStrikeWeaponCommandTypeChoiceOrdinal();
   bool isDMPI = false;
   //Set new weapon values   
   switch (ordinal)
   {
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTFORKEYLOAD:
         SetSelectedForKeyLoad(weaponName, commandType.getSelectForKeyLoad());
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_WEAPONARM:
         SetWeaponArmed(weaponName, commandType.getWeaponArm());
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTFORJETTISON:
         SetSelectedForJettison(weaponName, commandType.getSelectForJettison());
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTFORRELEASE:
         SetSelectedForRelease(weaponName, commandType.getSelectForRelease());
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_ASSIGNDMPI_ID:
         isDMPI = true;
         break;
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_GENERATEDYNAMICLAR:
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_SELECTAO_CODE:
      case uci::type::StrikeWeaponCommandType::STRIKEWEAPONCOMMANDTYPE_CHOICE_NONE:
      default:
         break;
   }

   //create new activity and push to StrikeActivityMessage
   UCI_StrikeActivityType activity(GetComponentParent());
   activity.Get().getActivityID().setUUID(GetInterface()->GetConnection()->generateUUID());
   activity.Get().getCapabilityID().push_back(capabilityUUID);
   activity.Get().getActivityRank().setPrecedence(0);
   activity.Get().getActivityRank().setPriority(0);
   activity.Get().setInteractive(false);
   activity.Get().setActivityState(uci::type::ActivityStateEnum::UCI_ACTIVE_UNCONSTRAINED);
   auto status = GetWeaponStatus(weaponName);
   activity.Get().setSelectedForJettison(status.mSelectedForJettison);
   activity.Get().setSelectedForKeyLoad(status.mSelectedForKeyLoad);
   activity.Get().setSelectedForRelease(status.mSelectedForRelease);
   activity.Get().setWeaponArmed(status.mWeaponArmed);
   activity.Get().setReadyForRelease(status.mReadyForRelease);
   if (isDMPI)
   {
      activity.Get().setAssignedDMPI_ID(commandType.getAssignDMPI_ID());
   }
   mActivityMessagePtr->Get().getMessageData().getActivity().push_back(activity.Get());
   WsfWeapon::FireResult result;
   WsfWeapon::FireOptions options;
   options.mWeaponPlatformName = it->second;

   if (status.mSelectedForRelease)
   {
      mCurrentlyFiring = true;
      result = weaponPtr->Fire(aSimTime, WsfWeapon::FireTarget(), options);
   }
   else if (isDMPI)
   {
      mCurrentlyFiring = true;
      WsfTrack location;
      auto it = mDMPI_Messages.find(commandType.getAssignDMPI_ID().getUUID().getValueAsString());
      auto& coordinates = it->second.first.Get().getMessageData().getDMPI_Location().getDMPI_Coordinates();
      location.SetLocationLLA(coordinates.getLatitude() * UtMath::cDEG_PER_RAD,
                              coordinates.getLongitude() * UtMath::cDEG_PER_RAD,
                              coordinates.getAltitude());
      WsfWeapon::FireTarget target(&location);
      result = weaponPtr->Fire(aSimTime, target, options);
   }

   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Command Fire Results.";
      debug.AddNote() << "Command ID: " << aCapabilityCommand.getCommandID().getUUID().getValueAsString();
      debug.AddNote() << "Capability ID: " << uuidStr;
      debug.AddNote() << "Command Type: " << GetCommandTypeString(ordinal);
      std::string resultStr{ result.mSuccess ? "Success" : "Failed" };
      debug.AddNote() << "Result: " << resultStr;
      std::string geometricResult{ "Unknown" };
      if (result.mEngagementPtr != nullptr)
      {
         result.mEngagementPtr->EnumToString(geometricResult, result.mEngagementPtr->GetGeometryResult());
         debug.AddNote() << "Geometric Result: " << geometricResult;
      }
   }

   if (result.mEngagementPtr != nullptr)
   {
      mFiredCapabilities.emplace(uuidStr);
      mSerialToActivity.emplace(result.mEngagementPtr->GetSerialNumber(), activity);
      mSendActivityMessage = true;
      if (isDMPI)
      {
         auto it = mDMPI_Messages.find(commandType.getAssignDMPI_ID().getUUID().getValueAsString());
         it->second.second = uci::type::DMPI_StoreStateEnum::UCI_RELEASED;
         SendDMPI_StatusMessage(aSimTime,
                                uci::type::MessageStateEnum::UCI_UPDATE,
                                commandType.getAssignDMPI_ID(),
                                uci::type::DMPI_StoreStateEnum::UCI_RELEASED);
      }
   }
   mCurrentlyFiring = false;

   
   return false;
}

void wsf::UCI_WeaponComponent::SendCapabilityMessage(double                                              aSimTime,
                                                     const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         mCapabilityMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mCapabilityMessagePtr);
         PrintMessageSent(aSimTime, "StrikeCapability");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "StrikeCapability",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void wsf::UCI_WeaponComponent::SendActivityMessage(double                                              aSimTime,
                                                   const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         if (mSendActivityMessage)
         {
            mActivityMessagePtr->Get().setMessageState(aMessageState);
            SendMessage(aSimTime, *mActivityMessagePtr);
            PrintMessageSent(aSimTime, "StrikeActivity");
         }
         else if (DebugEnabled())
         {
            PrintMessageNotSent(aSimTime,
                                "StrikeActivity",
                                "Activity hasn't been engaged or has completed.");
         }
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "StrikeActivity",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void wsf::UCI_WeaponComponent::SendCapabilityStatusMessage(double                                              aSimTime,
                                                           const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         //Update the weapon status
         UpdateCapabilityStatus();
         mCapabilityStatusMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mCapabilityStatusMessagePtr);
         PrintMessageSent(aSimTime, "StrikeCapabilityStatus");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "StrikeCapabilityStatus",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void wsf::UCI_WeaponComponent::SendDMPI_StatusMessage(double                                                 aSimTime,
                                                      const uci::type::MessageStateEnum::EnumerationItem&    aMessageState,
                                                      const uci::type::DMPI_ID_Type&                         aDMPI_ID,
                                                      const uci::type::DMPI_StoreStateEnum::EnumerationItem& aStoreState)
{
   UCI_DMPI_StatusMessage msg(GetComponentParent());
   UCI_DMPI_StoreStatusType storeStatus(GetComponentParent());
   storeStatus.Get().setStoreState(aStoreState);
   msg.Get().getMessageData().getStoreStatus().push_back(storeStatus.Get());
   msg.Get().getMessageData().setDMPI_ID(aDMPI_ID);
   msg.Get().setMessageState(aMessageState);

   ISM_OwnerProducerType ownPro(GetComponentParent());
   ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
   msg.Get().getSecurityAttributesGroup().getOwnerProducer().push_back(ownPro.Get());
   SendMessage(aSimTime, msg);
}

void wsf::UCI_WeaponComponent::UpdateAllMessages()
{
   UpdateSubsystemStatusMessage(uci::type::SubsystemStateEnum::UCI_OPERATE);
}

bool wsf::UCI_WeaponComponent::HasDMPI_Message(const std::string& aDMPI_ID)
{
   return mDMPI_Messages.find(aDMPI_ID) != mDMPI_Messages.end();
}

bool wsf::UCI_WeaponComponent::WeaponAvailable(WsfWeapon*         aWeaponPtr,
                                               const std::string& aUUID)
{
   bool available{ true };
   auto availability = GetWeaponAvailability(aWeaponPtr, aUUID);
   if (availability != uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE)
   {
      available = false;
      if (DebugEnabled())
      {
         auto debug = ut::log::debug() << "Command REJECTED.";
         debug.AddNote() << "Weapon " << GetAvailabilityString(availability);
         debug.AddNote() << "T = " << GetSimulation().GetSimTime();
         debug.AddNote() << "Weapon: " << aWeaponPtr->GetName();
         debug.AddNote() << "Capability UUID: " << aUUID;
      }
      UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Weapon not available.");
   }
   return available;
}

bool wsf::UCI_WeaponComponent::MeetsDMPI_Requirements(const std::string& aDMPI_Id)
{
   bool requirementsMet{ HasDMPI_Message(aDMPI_Id) };
   if (!requirementsMet)
   {
      if (DebugEnabled())
      {
         auto debug = ut::log::debug() << "Command REJECTED.";
         debug.AddNote() << "Corresponding DMPI message has not been received.";
         debug.AddNote() << "T = " << GetSimulation().GetSimTime();
         debug.AddNote() << "DMPI ID: " << aDMPI_Id;
      }
      UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "DMPI Message not received.");
   }
   
   return requirementsMet;
}

std::string wsf::UCI_WeaponComponent::GetWeaponName(const std::string& aName)
{
   std::string name{ aName };
   name = name.substr(0, name.find_last_of("_"));
   return name;
}

bool wsf::UCI_WeaponComponent::CanFire(double             aSimTime,
                                       WsfWeapon*         aWeaponPtr,
                                       const std::string& aUUID)
{
   bool canFire{ false };
   auto it = mDMPI_Messages.find(aUUID);
   if (it != mDMPI_Messages.end() && 
       aWeaponPtr->IsOperational() &&
       aWeaponPtr->IsTurnedOn())
   {
      if (it->second.second == uci::type::DMPI_StoreStateEnum::UCI_ALLOCATED)
      {
         auto& coordinates = it->second.first.Get().getMessageData().getDMPI_Location().getDMPI_Coordinates();
         WsfTrack track;
         track.SetSimulation(&GetSimulation());
         track.SetLocationLLA(coordinates.getLatitude() * UtMath::cDEG_PER_RAD,
                              coordinates.getLongitude() * UtMath::cDEG_PER_RAD, 
                              coordinates.getAltitude());
         if (track.LocationValid())
         {
            auto launchComputer = aWeaponPtr->GetLaunchComputer();
            canFire = (launchComputer != nullptr &&
                       launchComputer->EstimatedTimeToIntercept(aSimTime, track, 0.0) < WsfLaunchComputer::cFOREVER);
         }
      }
   }
   
   return canFire;
}

void wsf::UCI_WeaponComponent::SetSelectedForKeyLoad(const std::string& aWeapon,
                                                     bool               aSelected)
{
   auto it = mWeaponStatus.find(aWeapon);
   if (it != mWeaponStatus.end())
   {
      it->second.mSelectedForKeyLoad = aSelected;
   }
}

void wsf::UCI_WeaponComponent::SetWeaponArmed(const std::string& aWeapon,
                                              bool               aArmed)
{
   auto it = mWeaponStatus.find(aWeapon);
   if (it != mWeaponStatus.end())
   {
      it->second.mWeaponArmed = aArmed;
   }
}

void wsf::UCI_WeaponComponent::SetReadyForRelease(const std::string& aWeapon,
                                                  bool               aReady)
{
   auto it = mWeaponStatus.find(aWeapon);
   if (it != mWeaponStatus.end())
   {
      it->second.mReadyForRelease = aReady;
   }
}

void wsf::UCI_WeaponComponent::SetSelectedForRelease(const std::string& aWeapon,
                                                     bool               aSelected)
{
   auto it = mWeaponStatus.find(aWeapon);
   if (it != mWeaponStatus.end())
   {
      it->second.mSelectedForRelease = aSelected;
   }
}

void wsf::UCI_WeaponComponent::SetSelectedForJettison(const std::string& aWeapon,
                                                      bool               aSelected)
{
   auto it = mWeaponStatus.find(aWeapon);
   if (it != mWeaponStatus.end())
   {
      it->second.mSelectedForJettison = aSelected;
   }
}

wsf::UCI_WeaponComponent::WeaponStatus wsf::UCI_WeaponComponent::GetWeaponStatus(const std::string& aWeapon)
{
   WeaponStatus status;
   auto it = mWeaponStatus.find(aWeapon);
   if (it != mWeaponStatus.end())
   {
      status = it->second;
   }
   return status;
}

uci::type::CapabilityAvailabilityEnum::EnumerationItem wsf::UCI_WeaponComponent::GetWeaponAvailability(const WsfWeapon*   aWeaponPtr,
                                                                                                       const std::string& aUUID)
{
   auto availability = uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE;
   auto iter = mFiredCapabilities.find(aUUID);
   if (iter != mFiredCapabilities.end())
   {
      availability = uci::type::CapabilityAvailabilityEnum::UCI_EXPENDED;
   }
   else if (aWeaponPtr->IsBroken())
   {
      availability = uci::type::CapabilityAvailabilityEnum::UCI_FAULTED;
   }
   else if (aWeaponPtr->CanBeTurnedOn())
   {
      availability = uci::type::CapabilityAvailabilityEnum::UCI_TEMPORARILY_UNAVAILABLE;
   }

   return availability;
}

std::string wsf::UCI_WeaponComponent::GetAvailabilityString(uci::type::CapabilityAvailabilityEnum::EnumerationItem aAvailability)
{
   std::string availability = "Unknown";
   switch (aAvailability)
   {
      case uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE:
         availability = "AVAILABLE";
         break;
      case uci::type::CapabilityAvailabilityEnum::UCI_UNAVAILABLE:
         availability = "UNAVAILABLE";
         break;
      case uci::type::CapabilityAvailabilityEnum::UCI_TEMPORARILY_UNAVAILABLE:
         availability = "TEMPORARILY_UNAVAILABLE";
         break;
      case uci::type::CapabilityAvailabilityEnum::UCI_EXPENDED:
         availability = "EXPENDED";
         break;
      case uci::type::CapabilityAvailabilityEnum::UCI_DISABLED:
         availability = "DISABLED";
         break;
      case uci::type::CapabilityAvailabilityEnum::UCI_FAULTED:
         availability = "FAULTED";
         break;
      case uci::type::CapabilityAvailabilityEnum::enumNotSet:
      case uci::type::CapabilityAvailabilityEnum::enumMaxExclusive:
         break;
      default:
         break;
   }
   return availability;
}

} // wsf
