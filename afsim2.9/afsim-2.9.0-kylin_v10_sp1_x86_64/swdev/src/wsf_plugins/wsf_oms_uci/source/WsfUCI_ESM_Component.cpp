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

#include "WsfUCI_ESM_Component.hpp"

#include "UtLog.hpp"
#include "UtInput.hpp"
#include "UtMemory.hpp"

#include "WsfDateTime.hpp"
#include "WsfESM_Sensor.hpp"
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

UCI_ESM_Component::UCI_ESM_Component(WsfScenario& aScenario)
   : UCI_SensorComponent(aScenario,
                         cWSF_COMPONENT_UCI_ESM,
                         WsfSensor::cRADIO | WsfSensor::cPASSIVE)
{
}

UCI_ESM_Component::UCI_ESM_Component(const UCI_ESM_Component& aSrc)
   : UCI_SensorComponent(aSrc)
{
}

// virtual
UCI_ESM_Component::~UCI_ESM_Component()
{
   Detach();
}

bool UCI_ESM_Component::Initialize2(double aSimTime)
{
   // uci type construction depends on availability of the platform uci interface, so it is here rather than Initialize.
   unsigned entry = 0;
   do
   {
      auto esmComponentPtr = static_cast<UCI_ESM_Component*>(GetComponentParent()->GetComponentEntryByRole(cWSF_COMPONENT_UCI_ESM, entry));
      if (esmComponentPtr != nullptr &&
          esmComponentPtr != this)
      {
         auto componentSensorName = esmComponentPtr->GetSensorName();
         if (mSensorName == componentSensorName)
         {
            auto error = ut::log::error() << "Cannot have multiple UCI ESM components on sensor.";
            error.AddNote() << "Sensor: " << componentSensorName;
            return false;
         }
      }
      else
      {
         break;
      }
      ++entry;
   }
   while (true);

   mControlStatusMessagePtr = ut::make_unique<UCI_ControlStatusMessage>(GetComponentParent());
   mCapabilityMessagePtr = ut::make_unique<UCI_ESM_CapabilityMessage>(GetComponentParent());
   mActivityMessagePtr = ut::make_unique<UCI_ESM_ActivityMessage>(GetComponentParent());
   mCapabilityStatusMessagePtr = ut::make_unique<UCI_ESM_CapabilityStatusMessage>(GetComponentParent());
   mSubsystemStatusMessagePtr = ut::make_unique<UCI_SubsystemStatusMessage>(GetComponentParent());
   mSystemStatusMessagePtr = ut::make_unique<UCI_SystemStatusMessage>(GetComponentParent());

   ISM_OwnerProducerType ownPro(GetComponentParent());
   ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
   SetOwnerProducer(ownPro);

   auto& capabilityData = mCapabilityMessagePtr->Get().getMessageData();
   auto& activityData = mActivityMessagePtr->Get().getMessageData();

   mControlStatusMessagePtr->Get().getMessageData().getControlType().setControlStatusControlTypeChoiceOrdinal(
      uci::type::ControlStatusControlType::CONTROLSTATUSCONTROLTYPE_CHOICE_CAPABILITYCONTROL);
   //Loop through all modes
   for (size_t i = 0; i < mSensorPtr->GetModeCount(); ++i)
   {
      UCI_ESM_CapabilityType capabilityType(GetComponentParent());
      UCI_CapabilityStatusType status(GetComponentParent());
      capabilityType.Get().getCapabilityType().setValue(uci::type::CapabilityInitiationEnum::EnumerationItem::UCI_AUTO);

      InitializeModeData<UCI_ESM_CapabilityType, UCI_CapabilityStatusType>(capabilityType, status, i);

      capabilityType.Get().getCapabilityOptions().setInterruptOtherActivities(false);
      UCI_SubCapabilityEnumType subCapability(GetComponentParent());
      subCapability.Get().setValue(GetSubCapabilityEnum());
      capabilityType.Get().getSubCapabilityType().push_back(subCapability.Get());
      capabilityData.getCapability().push_back(capabilityType.Get());
      mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().push_back(status.Get());
   }

   if (mSubsystemUUID.empty())
   {
      mSubsystemUUID = GetInterface()->GetConnection()->getMySubsystemUUID().getValueAsString();
   }
   activityData.getSubsystemID().getUUID().setValue(mSubsystemUUID);

   RegisterCapabilitiesWithInterface();

   return UCI_SensorComponent::Initialize2(aSimTime);;
}

// ================================================================================================
//! Inject an ESM component for all ESM sensors
//! @param aSimTime The simulation time
//! @param aPlatform The platform to inject the components on to
bool UCI_ESM_Component::InjectConditionally(double       aSimTime,
                                            WsfPlatform& aPlatform)
{
   bool injected = false;
   unsigned entry = 0;
   do
   {
      WsfSensor* sensorPtr = static_cast<WsfSensor*>(aPlatform.GetComponentEntryByRole(cWSF_COMPONENT_SENSOR, entry));
      if (sensorPtr != nullptr)
      {
         if (sensorPtr->GetClass() == (WsfSensor::cRADIO | WsfSensor::cPASSIVE)) // a type of ESM
         {
            std::unique_ptr<UCI_ESM_Component> ESM_Component{ ut::make_unique<UCI_ESM_Component>(*this) };
            ESM_Component->Attach(*sensorPtr);
            injected |= aPlatform.AddComponent(ESM_Component.release());
         }
      }
      else
      {
         break;
      }
      ++entry;
   }
   while (true);

   return injected;
}

//TODO - Filler, need to implement
uci::type::ESM_SubCapabilityEnum::EnumerationItem wsf::UCI_ESM_Component::GetSubCapabilityEnum() const
{
   return uci::type::ESM_SubCapabilityEnum::UCI_PULSE_DATA_COLLECTION;
}

const int* UCI_ESM_Component::GetComponentRoles() const
{
   static const int roles[] = { cWSF_COMPONENT_UCI_SENSOR,
                                cWSF_COMPONENT_UCI_ESM,
                                cWSF_COMPONENT_UCI,
                                cWSF_COMPONENT_PLATFORM_PART,
                                cWSF_COMPONENT_NULL,
                              };
   return roles;
}

void UCI_ESM_Component::OnSensorTrackInitiated(double          aSimTime,
                                               WsfSensor*      aSensorPtr,
                                               const WsfTrack& aTrack)
{
   SendEntityMessage(aSimTime,
                     aTrack,
                     uci::type::MessageStateEnum::UCI_NEW,
                     uci::type::EntitySourceEnum::UCI_CAPABILITY_ESM);
}

void UCI_ESM_Component::OnSensorTrackUpdated(double          aSimTime,
                                             WsfSensor*      aSensorPtr,
                                             const WsfTrack& aTrack)
{
   SendEntityMessage(aSimTime,
                     aTrack,
                     uci::type::MessageStateEnum::UCI_UPDATE,
                     uci::type::EntitySourceEnum::UCI_CAPABILITY_ESM);
}

void wsf::UCI_ESM_Component::ClearActivities()
{
   mActivityMessagePtr->Get().getMessageData().getActivity().clear();
}

bool wsf::UCI_ESM_Component::HandleCommandMessage(const UCI_ESM_CommandMessage& aMessage)
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
         if (command.getESM_CommandTypeChoiceOrdinal() == uci::type::ESM_CommandType::ESM_COMMANDTYPE_CHOICE_CAPABILITY)
         {
            if (HandleCapabilityCommand(simTime, command.getCapability()))
            {
               if (i == 0)
               {
                  handled = true;
               }
               SendCommandStatusMessage<UCI_ESM_CommandStatusMessage, uci::type::CapabilityCommandID_Type>(simTime, msgState.getValue(), command.getCapability().getCommandID());
            }
            else
            {
               handled = true;
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
               SendCommandStatusMessage<UCI_ESM_CommandStatusMessage, 
                                        uci::type::CapabilityCommandID_Type>(simTime,
                                                                             msgState.getValue(),
                                                                             command.getActivity().getCommandID());
            }
            else
            {
               handled = true;
            }
         }
      }
   }
   return handled;
}

bool wsf::UCI_ESM_Component::HandleCapabilityCommand(double                                      aSimTime,
                                                     const uci::type::ESM_CapabilityCommandType& aCapabilityCommand)
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

      for (size_t i = 0; i < aCapabilityCommand.getSubCapabilitySelection().size(); ++i)
      {
         auto& selection = aCapabilityCommand.getSubCapabilitySelection().at(i);
         for (size_t j = 0; j < selection.getSubCapabilityDetails().size(); ++j)
         {
            auto&  details = selection.getSubCapabilityDetails().at(j);
            if (details.hasESM_Location() &&
                details.getESM_Location().getESM_LocationTypeChoiceOrdinal() ==
                uci::type::ESM_LocationType::ESM_LOCATIONTYPE_CHOICE_ESM_AIRVOLUME)
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
                     auto& airVolume = details.getESM_Location().getESM_AirVolume();
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
                        UCI_ESM_ActivityType activity(GetComponentParent());
                        activity.Get().getActivityID().setUUID(GetInterface()->GetConnection()->generateUUID());
                        mActivityToCabability.emplace(activity.Get().getActivityID().getUUID().getValueAsString(),
                                                      aCapabilityCommand.getCapabilityID().getUUID().getValueAsString());
                        activity.Get().getCapabilityID().push_back(aCapabilityCommand.getCapabilityID());
                        activity.Get().getActivityRank().setPrecedence(0);
                        activity.Get().getActivityRank().setPriority(0);
                        activity.Get().setInteractive(false);
                        activity.Get().setActivityState(uci::type::ActivityStateEnum::UCI_ACTIVE_UNCONSTRAINED);
                        activity.Get().getSubCapabilityDetails().push_back(details);
                        auto simTime = UCI_Util::GetTimeStamp(GetSimulation(), aSimTime);
                        activity.Get().setActualStartTime(simTime);
                        activity.Get().setActualEndTime(simTime);
                        activity.Get().setSubCapability(selection.getSubCapability());
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
   }
   return handled;
}

//TODO Implement
bool wsf::UCI_ESM_Component::HandleActivityCommand(double                         aSimTime,
                                                   const uci::type::ESM_ActivityCommandType& aActivityCommand)
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

void wsf::UCI_ESM_Component::SendActivityMessage(double                                              aSimTime,
                                                 const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         mActivityMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mActivityMessagePtr);
         PrintMessageSent(aSimTime, "ESM_Activity");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "ESM_Activity",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void UCI_ESM_Component::SendCapabilityMessage(double                                              aSimTime,
                                              const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   // Send capability message
   try
   {
      if (CanHandleMessage())
      {
         mCapabilityMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mCapabilityMessagePtr);
         PrintMessageSent(aSimTime, "ESM_Capability");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "ESM_Capability",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void wsf::UCI_ESM_Component::SendCapabilityStatusMessage(double                                              aSimTime,
                                                         const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         mCapabilityStatusMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mCapabilityStatusMessagePtr);
         PrintMessageSent(aSimTime, "ESM_CapabilityStatus");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "ESM_CapabilityStatus",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

bool wsf::UCI_ESM_Component::ReceiveMessage(double            aSimTime,
                                            const WsfMessage& aMessage)
{
   bool received{ false };
   if (CanHandleMessage())
   {
      auto type = aMessage.GetType();

      if (type == UCI_ESM_CommandMessage::GetTypeId())
      {
         const auto& message = dynamic_cast<const UCI_ESM_CommandMessage&>(aMessage);
         received = HandleCommandMessage(message);
      }
      else if (type == UCI_ESM_SettingsCommandMessage::GetTypeId())
      {
         const auto& message = dynamic_cast<const UCI_ESM_SettingsCommandMessage&>(aMessage);
         received = HandleSettingsCommand(aSimTime, message);
      }
      else
      {
         received = UCI_SensorComponent::ReceiveMessage(aSimTime, aMessage);
      }
   }
   return received;
}

void wsf::UCI_ESM_Component::SetOwnerProducer(const ISM_OwnerProducerType& aOwnerType)
{
   mActivityMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   mCapabilityMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   mCapabilityStatusMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   UCI_PartComponent::SetOwnerProducer(aOwnerType);
}

void wsf::UCI_ESM_Component::UpdateAllMessages()
{
   UpdateCapabilityStatusMessage<UCI_ESM_CapabilityStatusMessage>(mCapabilityStatusMessagePtr.get());
   UCI_SensorComponent::UpdateAllMessages();
}

void wsf::UCI_ESM_Component::SetAllCapabilitiesAvailability(const uci::type::CapabilityAvailabilityEnum::EnumerationItem& aAvailability)
{
   size_t capSize = mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().size();
   for (size_t i = 0; i < capSize; ++i)
   {
      mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().at(i).getAvailabilityInfo().setAvailability(aAvailability);
   }
   //Whether we are enabling or disabling all modes, the "default" mode should be the lowest available
   mLowestAvailableCapability = 0;
}

bool wsf::UCI_ESM_Component::HandleSettingsCommand(double                                aSimTime,
                                                   const UCI_ESM_SettingsCommandMessage& aMessage)
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
      SendCommandStatusMessage<UCI_ESM_SettingsCommandStatusMessage, uci::type::CapabilitySettingsCommandID_Type>(aSimTime, msgState.getValue(), data.getCommandID());
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

void wsf::UCI_ESM_Component::OnSlewComplete(double aSimTime)
{
   SlewComplete<UCI_ESM_ActivityMessage>(aSimTime, mActivityMessagePtr.get());
}


// virtual
WsfComponent* UCI_ESM_Component::CloneComponent() const
{
   return new UCI_ESM_Component(*this);
}

// virtual
void* UCI_ESM_Component::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_UCI_ESM)
   {
      return this;
   }
   else if (aRole == cWSF_COMPONENT_UCI_SENSOR)
   {
      return static_cast<UCI_SensorComponent*>(this);
   }
   else if (aRole == cWSF_COMPONENT_UCI)
   {
      return static_cast<UCI_Component*>(this);
   }
   else if (aRole == cWSF_COMPONENT_PLATFORM_PART)
   {
      return static_cast<WsfPlatformPart*>(this);
   }
   else
   {
      return 0;
   }
}

} // wsf
