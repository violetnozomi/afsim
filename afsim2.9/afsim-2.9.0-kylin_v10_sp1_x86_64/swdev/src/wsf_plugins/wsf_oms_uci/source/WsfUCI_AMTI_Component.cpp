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

#include "WsfUCI_AMTI_Component.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"

#include "WsfDateTime.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfEvent.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfTrack.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{

UCI_AMTI_Component::UCI_AMTI_Component(WsfScenario& aScenario)
   : UCI_SensorComponent(aScenario,
                         cWSF_COMPONENT_UCI_AMTI,
                         WsfSensor::cRADIO | WsfSensor::cACTIVE)
{
}

UCI_AMTI_Component::UCI_AMTI_Component(const UCI_AMTI_Component& aSrc)
   : UCI_SensorComponent(aSrc)
{
}

// virtual
UCI_AMTI_Component::~UCI_AMTI_Component()
{
   Detach();
}

bool UCI_AMTI_Component::Initialize2(double aSimTime)
{
   // uci type construction depends on availability of the platform uci interface, so it is here rather than Initialize.
   if (IsActive())
   {
      unsigned entry = 0;
      do
      {
         auto componentPtr = static_cast<UCI_AMTI_Component*>(GetComponentParent()->GetComponentEntryByRole(cWSF_COMPONENT_UCI_AMTI, entry));
         if (componentPtr != nullptr &&
             componentPtr != this)
         {
            auto componentSensorName = componentPtr->GetSensorName();
            if (mSensorName == componentSensorName)
            {
               auto error = ut::log::error() << "Cannot have multiple UCI Radar components on sensor.";
               error.AddNote() << "Sensor: " << mSensorName;
               return false;
            }
         }
         else
         {
            break;
         }
         ++entry;
      } while (true);

      mControlStatusMessagePtr = ut::make_unique<UCI_ControlStatusMessage>(GetComponentParent());
      mCapabilityMessagePtr = ut::make_unique<UCI_AMTI_CapabilityMessage>(GetComponentParent());
      mActivityMessagePtr = ut::make_unique<UCI_AMTI_ActivityMessage>(GetComponentParent());
      mCapabilityStatusMessagePtr = ut::make_unique<UCI_AMTI_CapabilityStatusMessage>(GetComponentParent());

      mSubsystemStatusMessagePtr = ut::make_unique<UCI_SubsystemStatusMessage>(GetComponentParent());
      mSystemStatusMessagePtr = ut::make_unique<UCI_SystemStatusMessage>(GetComponentParent());

      ISM_OwnerProducerType ownPro(GetComponentParent());
      ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
      SetOwnerProducer(ownPro);

      mControlStatusMessagePtr->Get().getMessageData().getControlType().setControlStatusControlTypeChoiceOrdinal(
         uci::type::ControlStatusControlType::CONTROLSTATUSCONTROLTYPE_CHOICE_CAPABILITYCONTROL);
      //Loop through all modes
      for (size_t i = 0; i < mSensorPtr->GetModeCount(); ++i)
      {
         UCI_AMTI_CapabilityType capabilityType(GetComponentParent());

         capabilityType.Get().setCapabilityType(uci::type::AMTI_CapabilityEnum::UCI_CUED);
         UCI_AMTI_SubCapabilityType subCapability(GetComponentParent());
         subCapability.Get().setValue(GetSubCapabilityEnum());
         capabilityType.Get().getSubCapabilityType().push_back(subCapability.Get());

         auto& capabilityOptions = capabilityType.Get().getCapabilityOptions();

         capabilityOptions.setInterruptOtherActivities(false);
         capabilityOptions.setCollectionPolicy(false);
         capabilityOptions.setConcurrentOperationSupported(false);

         UCI_FrequencyRangeType frequencyRange(GetComponentParent());
         frequencyRange.Get().setMax(0);
         frequencyRange.Get().setMin(0);
         capabilityType.Get().getSupportedFrequencies().push_back(frequencyRange.Get());

         UCI_CapabilityStatusType status(GetComponentParent());

         InitializeModeData<UCI_AMTI_CapabilityType, UCI_CapabilityStatusType>(capabilityType, status, i);

         mCapabilityMessagePtr->Get().getMessageData().getCapability().push_back(capabilityType.Get());
         mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().push_back(status.Get());
      }

      if (mSubsystemUUID.empty())
      {
         mSubsystemUUID = GetInterface()->GetConnection()->getMySubsystemUUID().getValueAsString();
      }
      mActivityMessagePtr->Get().getMessageData().getSubsystemID().getUUID().setValue(mSubsystemUUID);

      RegisterCapabilitiesWithInterface();
   }

   return UCI_SensorComponent::Initialize2(aSimTime);
}

// ================================================================================================
//! Inject an Radar component for all Radar sensors
//! @param aSimTime The simulation time
//! @param aPlatform The platform to inject the components on to
bool UCI_AMTI_Component::InjectConditionally(double       aSimTime,
                                             WsfPlatform& aPlatform)
{
   bool injected = false;
   unsigned entry = 0;
   do
   {
      WsfSensor* sensorPtr = static_cast<WsfSensor*>(aPlatform.GetComponentEntryByRole(cWSF_COMPONENT_SENSOR, entry));
      if (sensorPtr != nullptr)
      {
         if (sensorPtr->GetClass() == (WsfSensor::cRADIO | WsfSensor::cACTIVE)) // a type of Radar
         {
            std::unique_ptr<UCI_AMTI_Component> radarComponent{ ut::make_unique<UCI_AMTI_Component>(*this) };
            radarComponent->Attach(*sensorPtr);
            injected |= aPlatform.AddComponent(radarComponent.release());
         }
      }
      else
      {
         break;
      }
      ++entry;
   } while (true);

   return injected;
}

void wsf::UCI_AMTI_Component::UpdateAllMessages()
{
   UpdateCapabilityStatusMessage<UCI_AMTI_CapabilityStatusMessage>(mCapabilityStatusMessagePtr.get());
   UCI_SensorComponent::UpdateAllMessages();
}

void wsf::UCI_AMTI_Component::SetAllCapabilitiesAvailability(const uci::type::CapabilityAvailabilityEnum::EnumerationItem& aAvailability)
{
   size_t capSize = mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().size();
   for (size_t i = 0; i < capSize; ++i)
   {
      mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().at(i).getAvailabilityInfo().setAvailability(aAvailability);
   }
   //Whether we are enabling or disabling all modes, the "default" mode should be the lowest available
   mLowestAvailableCapability = 0;
}

//TODO - Filler, need to implement
uci::type::AMTI_SubCapabilityEnum::EnumerationItem wsf::UCI_AMTI_Component::GetSubCapabilityEnum() const
{
   return uci::type::AMTI_SubCapabilityEnum::UCI_AAS;
}

const int* UCI_AMTI_Component::GetComponentRoles() const
{
   static const int roles[] = { cWSF_COMPONENT_UCI_SENSOR,
                                cWSF_COMPONENT_UCI_AMTI,
                                cWSF_COMPONENT_UCI,
                                cWSF_COMPONENT_PLATFORM_PART,
                                cWSF_COMPONENT_NULL,
                              };
   return roles;
}

void UCI_AMTI_Component::OnSensorTrackInitiated(double          aSimTime,
                                                WsfSensor*      aSensorPtr,
                                                const WsfTrack& aTrack)
{
   if (aTrack.GetSpatialDomain() == WsfSpatialDomain::WSF_SPATIAL_DOMAIN_AIR)
   {
      SendEntityMessage(aSimTime,
                        aTrack,
                        uci::type::MessageStateEnum::UCI_NEW,
                        uci::type::EntitySourceEnum::UCI_CAPABILITY_AMTI);
   }
}

void UCI_AMTI_Component::OnSensorTrackUpdated(double          aSimTime,
                                              WsfSensor*      aSensorPtr,
                                              const WsfTrack& aTrack)
{
   if (aTrack.GetSpatialDomain() == WsfSpatialDomain::WSF_SPATIAL_DOMAIN_AIR)
   {
      SendEntityMessage(aSimTime,
                        aTrack,
                        uci::type::MessageStateEnum::UCI_UPDATE,
                        uci::type::EntitySourceEnum::UCI_CAPABILITY_AMTI);
   }
}

void wsf::UCI_AMTI_Component::SetOwnerProducer(const ISM_OwnerProducerType& aOwnerType)
{
   mActivityMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   mCapabilityMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   mCapabilityStatusMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   UCI_PartComponent::SetOwnerProducer(aOwnerType);
}

void wsf::UCI_AMTI_Component::ClearActivities()
{
   mActivityMessagePtr->Get().getMessageData().getActivity().clear();
}

bool wsf::UCI_AMTI_Component::HandleSettingsCommand(double                                 aSimTime,
                                                    const UCI_AMTI_SettingsCommandMessage& aMessage)
{
   bool handled{ false };
   auto& data = aMessage.Get().getMessageData();
   auto capabilitySize = data.getCapabilityState().size();
   auto& msgState = aMessage.Get().getMessageState();
   auto& status = mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus();
   auto currentIndex = mSensorPtr->GetCurrentMode()->GetModeIndex();
   bool currentIndexDisabled{ false };
   for (size_t i = 0; i < capabilitySize; ++i)
   {
      auto& capability = data.getCapabilityState().at(i);
      auto it = mUUID_ToModeIndex.find(capability.getCapabilityID().getUUID().getValueAsString());
      //Check if the capability exists on the platform
      if (it != mUUID_ToModeIndex.end())
      {
         handled = true;
         if (capability.getCommandedState() == uci::type::CapabilityStateCommandEnum::UCI_DISABLE)
         {
            status.at(it->second).getAvailabilityInfo().setAvailability(uci::type::CapabilityAvailabilityEnum::UCI_DISABLED);
            currentIndexDisabled = currentIndex == it->second || currentIndexDisabled;
         }
         else if (capability.getCommandedState() == uci::type::CapabilityStateCommandEnum::UCI_ENABLE)
         {
            status.at(it->second).getAvailabilityInfo().setAvailability(uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE);
         }
      }
   }
   if (handled)
   {
      UpdateCommandResults(uci::type::CommandStateEnum::UCI_ACCEPTED, "");
      SendCommandStatusMessage<UCI_AMTI_SettingsCommandStatusMessage, uci::type::CapabilitySettingsCommandID_Type>(aSimTime, msgState.getValue(), data.getCommandID());
      SendCapabilityStatusMessage(aSimTime, uci::type::MessageStateEnum::UCI_UPDATE);
      if (currentIndexDisabled)
      {
         mLowestAvailableCapability = 0;
         for (size_t i = 0; i < status.size(); ++i)
         {
            if (status.at(i).getAvailabilityInfo().getAvailability() == uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE)
            {
               mLowestAvailableCapability = i;
               break;
            }
         }
         SelectDefaultMode(aSimTime);
      }
      
   }
   return handled;
}

bool wsf::UCI_AMTI_Component::HandleCommandMessage(const UCI_AMTI_CommandMessage& aMessage)
{
   bool handled{ false };
   auto& data = aMessage.Get().getMessageData();
   auto commandSize = data.getCommand().size();
   auto& msgHeader = aMessage.Get().getSequenceGroup().getMessageHeader();
   auto& msgState = aMessage.Get().getMessageState();
   for (size_t i = 0; i < commandSize; ++i)
   {
      auto& command = data.getCommand().at(i);
      if (HasControl(command.getCapability().getCapabilityID().getUUID().getValueAsString(),
                     msgHeader.getServiceID(),
                     msgHeader.getSystemID()))
      {
         auto simTime = GetSimulation().GetSimTime();
         if (command.getAMTI_CommandTypeChoiceOrdinal() == uci::type::AMTI_CommandType::AMTI_COMMANDTYPE_CHOICE_CAPABILITY)
         {
            if (HandleCapabilityCommand(simTime, command.getCapability()))
            {
               if (i == 0)
               {
                  handled = true;
               }
               SendCommandStatusMessage<UCI_AMTI_CommandStatusMessage, uci::type::CapabilityCommandID_Type>(simTime, msgState.getValue(), command.getCapability().getCommandID());
            }
            else
            {
               handled = false;
            }
         }
         else
         {
            if (HandleActivityCommand(simTime, command.getActivity()))
            {
               if (i == 0)
               {
                  handled = true;
               }
               SendCommandStatusMessage<UCI_AMTI_CommandStatusMessage, uci::type::CapabilityCommandID_Type>(simTime, msgState.getValue(), command.getActivity().getCommandID());
            }
            else
            {
               handled = false;
            }
         }
      }
   }
   return handled;
}

bool wsf::UCI_AMTI_Component::HandleCapabilityCommand(double                                       aSimTime,
                                                      const uci::type::AMTI_CapabilityCommandType& aCapabilityCommand)
{
   bool handled{ false };
   auto it = mUUID_ToModeIndex.find(aCapabilityCommand.getCapabilityID().getUUID().getValueAsString());
   //Check if the capability exists on the platform
   if (it != mUUID_ToModeIndex.end())
   {
      handled = true;
      if (!CheckPartStatus(mSensorPtr))
      {
         return handled;
      }

      for (size_t i = 0; i < aCapabilityCommand.getTargetVolume().size(); ++i)
      {
         auto& targetVolume = aCapabilityCommand.getTargetVolume().at(i);
         if (targetVolume.getAirTargetVolumeCommandTypeChoiceOrdinal() ==
            uci::type::AirTargetVolumeCommandType::AIRTARGETVOLUMECOMMANDTYPE_CHOICE_AIRVOLUME)
         {
            auto tmpSensorPtr = mSensorPtr;
            tmpSensorPtr->SelectMode(GetSimulation().GetSimTime(), tmpSensorPtr->GetModeName(it->second));
            auto newIndex = tmpSensorPtr->GetCurrentMode()->GetModeIndex();
            auto availability = mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().at(newIndex).getAvailabilityInfo().getAvailability().getValue();

            if (availability == uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE)
            {
               if (tmpSensorPtr->GetCueMode() != WsfArticulatedPart::SlewMode::cSLEW_FIXED &&
                   tmpSensorPtr->GetSlewMode() != WsfArticulatedPart::SlewMode::cSLEW_FIXED)
               {
                  auto& airVolume = targetVolume.getAirVolume().getAirVolumeSensorReferenced();
                  double az = UtMath::NormalizeAngleMinusPi_Pi(airVolume.getAzimuthScanCenter());
                  double el = airVolume.getElevationScanCenter();

                  if (airVolume.getAzimuthScanStabilization() == uci::type::LOS_ReferenceEnum::UCI_INERTIAL)
                  {
                     double ned[3] = { 0, 0, 0 };
                     double dummyEl = airVolume.getElevationScanCenter();
                     tmpSensorPtr->GetAbsoluteCuedOrientation(az, dummyEl, ned);
                  }

                  if (UCI_Util::CheckLimits(tmpSensorPtr, az, el))
                  {
                     mSensorPtr->SelectMode(GetSimulation().GetSimTime(), mSensorPtr->GetModeName(it->second));

                     ClearActivities();
                     UCI_AMTI_ActivityType activity(GetComponentParent());
                     activity.Get().getActivityID().setUUID(GetInterface()->GetConnection()->generateUUID());
                     mActivityToCabability.emplace(activity.Get().getActivityID().getUUID().getValueAsString(),
                        aCapabilityCommand.getCapabilityID().getUUID().getValueAsString());
                     activity.Get().getCapabilityID().push_back(aCapabilityCommand.getCapabilityID());
                     activity.Get().getActivityRank().setPrecedence(0);
                     activity.Get().getActivityRank().setPriority(0);
                     activity.Get().setInteractive(false);
                     activity.Get().setActivityState(uci::type::ActivityStateEnum::UCI_ACTIVE_UNCONSTRAINED);
                     activity.Get().getTargetVolume().push_back(targetVolume);

                     mActivityMessagePtr->Get().getMessageData().getActivity().push_back(activity.Get());
                     SetCommandAccepted(az, el);

                     double currentAz, currentEl;
                     mSensorPtr->GetCurrentCuedOrientation(currentAz, currentEl);
                     mSensorPtr->SetCuedOrientation(az, el);

                     if (az == currentAz && el == currentEl)
                     {
                        OnSlewComplete(aSimTime);
                     }
                     else
                     {
                        SendActivityMessage(aSimTime, uci::type::MessageStateEnum::UCI_NEW);
                     }
                  }
                  else
                  {
                     SetRejectedOutOfLimits(az, el);
                  }
               }
               else
               {
                  SetRejectedFixedMode(tmpSensorPtr->GetCurrentModeName(), it->first);
               }
            }
            else
            {
               SetRejectedModeDisabled(tmpSensorPtr->GetCurrentModeName(), it->first);
            }
         }
         else
         {
            SetRejectedTypeUnavailable();
         }
      }
   }
   return handled;
}

//TODO Implement
bool wsf::UCI_AMTI_Component::HandleActivityCommand(double                                     aSimTime,
                                                    const uci::type::AMTI_ActivityCommandType& aActivityCommand)
{
   bool handled{ false };
   auto it = mActivityToCabability.find(aActivityCommand.getActivityID().getUUID().getValueAsString());
   if (it != mActivityToCabability.end())
   {
      auto modeIt = mUUID_ToModeIndex.find(it->second);
      if (modeIt != mUUID_ToModeIndex.end())
      {
         if (DebugEnabled())
         {
            auto debug = ut::log::debug() << "Command REJECTED.";
            debug.AddNote() << "Activity commands are currently unavailable.";
         }
         UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Activity commands unavailable");
         handled = true;
      }
   }
   return handled;
}

void wsf::UCI_AMTI_Component::SendActivityMessage(double                                              aSimTime,
                                                  const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         mActivityMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mActivityMessagePtr);
         PrintMessageSent(aSimTime, "AMTI_Activity");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "AMTI_Activity",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void UCI_AMTI_Component::SendCapabilityMessage(double                                              aSimTime,
                                               const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   // Send capability message
   try
   {
      if (CanHandleMessage())
      {
         mCapabilityMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mCapabilityMessagePtr);
         PrintMessageSent(aSimTime, "AMTI_Capability");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "AMTI_Capability",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void wsf::UCI_AMTI_Component::SendCapabilityStatusMessage(double                                              aSimTime,
                                                          const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         mCapabilityStatusMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mCapabilityStatusMessagePtr);
         PrintMessageSent(aSimTime, "AMTI_CapabilityStatus");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "AMTI_CapabilityStatus",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

bool wsf::UCI_AMTI_Component::ReceiveMessage(double            aSimTime,
                                             const WsfMessage& aMessage)
{
   bool received{ false };
   if (CanHandleMessage())
   {
      auto type = aMessage.GetType();

      if (type == UCI_AMTI_CommandMessage::GetTypeId())
      {
         const auto& message = dynamic_cast<const UCI_AMTI_CommandMessage&>(aMessage);
         received = HandleCommandMessage(message);
      }
      else if (type == UCI_AMTI_SettingsCommandMessage::GetTypeId())
      {
         const auto& message = dynamic_cast<const UCI_AMTI_SettingsCommandMessage&>(aMessage);
         received = HandleSettingsCommand(aSimTime, message);
      }
      else
      {
         received = UCI_SensorComponent::ReceiveMessage(aSimTime, aMessage);
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

void wsf::UCI_AMTI_Component::OnSlewComplete(double aSimTime)
{
   SlewComplete<UCI_AMTI_ActivityMessage>(aSimTime, mActivityMessagePtr.get());
}

// virtual
WsfComponent* UCI_AMTI_Component::CloneComponent() const
{
   return new UCI_AMTI_Component(*this);
}

// virtual
void* UCI_AMTI_Component::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_UCI_AMTI)
   {
      return this;
   }
   else
   {
      return UCI_SensorComponent::QueryInterface(aRole);
   }
}

} // wsf
