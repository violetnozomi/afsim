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

#include "WsfUCI_IRST_Component.hpp"

#include "UtLog.hpp"
#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"

#include "WsfEM_Antenna.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfDateTime.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEvent.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfTrack.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_MessageBaseType.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

#include "uci/type/EntityMDT.h"
#include "uci/type/KinematicsType.h"
#include "uci/type/DateTimeSigmaType.h"
#include "uci/type/EntityPositionType.h"
#include "uci/type/POST_Command.h"
#include "uci/type/RelativePositionType.h"
#include "uci/factory/DateTimeSigmaTypeFactory.h"

namespace wsf
{

UCI_IRST_Component::UCI_IRST_Component(WsfScenario& aScenario)
   : UCI_SensorComponent(aScenario,
                         cWSF_COMPONENT_UCI_IRST,
                         WsfSensor::cINFRARED | WsfSensor::cPASSIVE)
{
}

UCI_IRST_Component::UCI_IRST_Component(const UCI_IRST_Component& aSrc)
   : UCI_SensorComponent(aSrc)
{
}

// virtual
UCI_IRST_Component::~UCI_IRST_Component()
{
   Detach();
}

bool UCI_IRST_Component::Initialize2(double aSimTime)
{
   if (IsActive())
   {
      // uci type construction depends on availability of the platform uci interface, so it is here rather than Initialize.
      unsigned entry = 0;
      do
      {
         auto irstComponentPtr = static_cast<UCI_IRST_Component*>(GetComponentParent()->GetComponentEntryByRole(cWSF_COMPONENT_UCI_IRST, entry));
         if (irstComponentPtr != nullptr)
         {
            if (irstComponentPtr != this)
            {
               auto componentSensorName = irstComponentPtr->GetSensorName();
               if (mSensorName == componentSensorName)
               {
                  auto error = ut::log::error() << "Cannot have multiple UCI IRST components on sensor.";
                  error.AddNote() << "Sensor: " << componentSensorName << '.';
                  return false;
               }
            }
         }
         else
         {
            break;
         }
         ++entry;
      }
      while (true);

      try
      {
         mControlStatusMessagePtr = ut::make_unique<UCI_ControlStatusMessage>(GetComponentParent());
         mCapabilityMessagePtr = ut::make_unique<UCI_POST_CapabilityMessage>(GetComponentParent());
         mCapabilityStatusMessagePtr = ut::make_unique<UCI_POST_CapabilityStatusMessage>(GetComponentParent());
         mSubsystemStatusMessagePtr = ut::make_unique<UCI_SubsystemStatusMessage>(GetComponentParent());
         mSystemStatusMessagePtr = ut::make_unique<UCI_SystemStatusMessage>(GetComponentParent());
         mActivityMessagePtr = ut::make_unique<UCI_POST_ActivityMessage>(GetComponentParent());

         ISM_OwnerProducerType ownPro(GetComponentParent());
         ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
         SetOwnerProducer(ownPro);

         auto& capabilityData = mCapabilityMessagePtr->Get().getMessageData();
         auto& activityData = mActivityMessagePtr->Get().getMessageData();

         std::string label = UCI_Util::RemoveAngleBrackets(mSensorPtr->GetPlatform()->GetName() + ":" + mSensorPtr->GetName());

         //Needed to push control types to control status message
         mControlStatusMessagePtr->Get().getMessageData().getControlType().setControlStatusControlTypeChoiceOrdinal(
            uci::type::ControlStatusControlType::CONTROLSTATUSCONTROLTYPE_CHOICE_CAPABILITYCONTROL);
         std::set<uci::type::PointingTypeEnum::EnumerationItem> pointingCapabilities;
         //Loop through all modes
         for (size_t i = 0; i < mSensorPtr->GetModeCount(); ++i)
         {
            UCI_POST_CapabilityType capabilityType(GetComponentParent());
            UCI_POST_CapabilityStatusType status(GetComponentParent());
            capabilityType.Get().getCapabilityType().setValue(uci::type::POST_CapabilityEnum::UCI_SEARCH_VOLUME);

            InitializeModeData<UCI_POST_CapabilityType, UCI_POST_CapabilityStatusType>(capabilityType, status, i);

            //If sensor can be moved by LOS (az, el) add LOS pointing capability
            if (pointingCapabilities.find(uci::type::PointingTypeEnum::EnumerationItem::UCI_LOS) == pointingCapabilities.end() &&
                mSensorPtr->GetModeEntry(i)->GetCueMode() != WsfSensor::SlewMode::cSLEW_FIXED)
            {
               UCI_PointingCapabilityType pointingCapability(GetComponentParent());
               pointingCapability.Get().setValue(uci::type::PointingTypeEnum::EnumerationItem::UCI_LOS);
               capabilityData.getPointingCapability().push_back(pointingCapability.Get());
            }

            capabilityData.getCapability().push_back(capabilityType.Get());
            mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().push_back(status.Get());
         }

         //The sensor is the sub capability so IRST sensors' subcapabilities are Infrared
         UCI_POST_SubCapabilityType subCapability(GetComponentParent());
         subCapability.Get().getSubCapabilityID().setUUID(GetInterface()->GetConnection()->generateUUID());
         subCapability.Get().getSpectrum().setValue(uci::type::POST_SubCapabilityEnum::EnumerationItem::UCI_INFRARED_SPECTRUM);
         capabilityData.getSubCapability().push_back(subCapability.Get());

         if (mSubsystemUUID.empty())
         {
            mSubsystemUUID = GetInterface()->GetConnection()->getMySubsystemUUID().getValueAsString();
         }
         activityData.getSubsystemID().getUUID().setValue(mSubsystemUUID);
         RegisterCapabilitiesWithInterface();
      }
      catch (const uci::base::UCIException& ex)
      {
         ut::log::error() << ex.what() << ":" << ex.getErrorCode();
      }
   }

   return UCI_SensorComponent::Initialize2(aSimTime);
}

// ================================================================================================
//! Inject an IRST component for all IRST sensors
//! @param aSimTime The simulation time
//! @param aPlatform The platform to inject the components on to
//! @return Returns true if component is injected
bool UCI_IRST_Component::InjectConditionally(double       aSimTime,
                                             WsfPlatform& aPlatform)
{
   bool injected = false;
   unsigned entry = 0;
   do
   {
      WsfSensor* sensorPtr = static_cast<WsfSensor*>(aPlatform.GetComponentEntryByRole(cWSF_COMPONENT_SENSOR, entry));
      if (sensorPtr != nullptr)
      {
         if (sensorPtr->GetClass() == (WsfSensor::cINFRARED | WsfSensor::cPASSIVE)) // a type of IRST
         {
            std::unique_ptr<UCI_IRST_Component> IRST_Component{ ut::make_unique<UCI_IRST_Component>(*this) };
            IRST_Component->Attach(*sensorPtr);
            injected |= aPlatform.AddComponent(IRST_Component.release());
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

const int* UCI_IRST_Component::GetComponentRoles() const
{
   static const int roles[] = { cWSF_COMPONENT_UCI_SENSOR,
                                cWSF_COMPONENT_UCI_IRST,
                                cWSF_COMPONENT_UCI,
                                cWSF_COMPONENT_PLATFORM_PART,
                                cWSF_COMPONENT_NULL
                              };
   return roles;
}

void UCI_IRST_Component::OnSensorTrackInitiated(double          aSimTime,
                                                WsfSensor*      aSensorPtr,
                                                const WsfTrack& aTrack)
{

   SendEntityMessage(aSimTime,
                     aTrack,
                     uci::type::MessageStateEnum::UCI_NEW,
                     uci::type::EntitySourceEnum::UCI_CAPABILITY_PASSIVE_OPTICAL_SEARCH_AND_TRACK);
}

void UCI_IRST_Component::OnSensorTrackUpdated(double          aSimTime,
                                              WsfSensor*      aSensorPtr,
                                              const WsfTrack& aTrack)
{
   SendEntityMessage(aSimTime,
                     aTrack,
                     uci::type::MessageStateEnum::UCI_UPDATE,
                     uci::type::EntitySourceEnum::UCI_CAPABILITY_PASSIVE_OPTICAL_SEARCH_AND_TRACK);
}

void wsf::UCI_IRST_Component::OnSlewComplete(double aSimTime)
{
   SlewComplete<UCI_POST_ActivityMessage>(aSimTime, mActivityMessagePtr.get());
}

void wsf::UCI_IRST_Component::SetOwnerProducer(const ISM_OwnerProducerType& aOwnerType)
{
   mActivityMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   mCapabilityMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   mCapabilityStatusMessagePtr->Get().getSecurityAttributesGroup().getOwnerProducer().push_back(aOwnerType.Get());
   UCI_PartComponent::SetOwnerProducer(aOwnerType);
}

void wsf::UCI_IRST_Component::ClearActivities()
{
   mActivityMessagePtr->Get().getMessageData().getActivity().clear();
}

bool wsf::UCI_IRST_Component::HandleCommandMessage(double                         aSimTime,
                                                   const UCI_POST_CommandMessage& aMessage)
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

         if (command.getPOST_CommandTypeChoiceOrdinal() == uci::type::POST_CommandType::POST_COMMANDTYPE_CHOICE_CAPABILITY)
         {
            if (HandleCapabilityCommand(aSimTime, command.getCapability()))
            {
               if (i == 0)
               {
                  handled = true;
               }
               SendCommandStatusMessage<UCI_POST_CommandStatusMessage, uci::type::CapabilityCommandID_Type>(simTime, msgState.getValue(), command.getCapability().getCommandID());
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
               SendCommandStatusMessage<UCI_POST_CommandStatusMessage, uci::type::CapabilityCommandID_Type>(simTime, msgState.getValue(), command.getActivity().getCommandID());
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

//=================================================================
//! Handles POST_CapabilityCommands. Currently only accepts Line of Sight
//! (LOS) commands. LOS commands tell a sensor which azimuth and elevation
//! to cue/slew to, i.e where to point. If a sensor is commanded to
//! cue/slew past its min/max angles, the sensor will only go to the
//! min/max angle.
//!
//! This will only handle commands relevant to the corresponding sensor's
//! capabilities. If the current mode of the sensor is different than
//! the commanded mode, the sensor will switch to the commanded mode.
//!
//! @param aSimTime The current simulation time
//! @param aCapabilityCommand The given sensor command
//! @return Returns true if the sensor has the capability being commanded, false otherwise
bool wsf::UCI_IRST_Component::HandleCapabilityCommand(double                                       aSimTime,
                                                      const uci::type::POST_CapabilityCommandType& aCapabilityCommand)
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

      if (aCapabilityCommand.hasPointing())
      {
         //Is a LOS Command
         if (aCapabilityCommand.getPointing().getPointingTypeChoiceOrdinal() == uci::type::PointingType::POINTINGTYPE_CHOICE_LOSOPTION &&
             aCapabilityCommand.getPointing().getLOSOption().getLOS_D_TypeChoiceOrdinal() == uci::type::LOS_D_Type::LOS_D_TYPE_CHOICE_LOS)
         {
            auto& los = aCapabilityCommand.getPointing().getLOSOption().getLOS();
            double az = UtMath::NormalizeAngleMinusPi_Pi(los.getAzimuth());
            double el = los.getElevation();

            auto tmpSensorPtr = mSensorPtr;
            tmpSensorPtr->SelectMode(GetSimulation().GetSimTime(), tmpSensorPtr->GetModeName(it->second));
            auto newIndex = tmpSensorPtr->GetCurrentMode()->GetModeIndex();
            auto availability = mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().at(newIndex).getAvailabilityInfo().getAvailability().getValue();

            if (availability == uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE)
            {
               if (tmpSensorPtr->GetCueMode() != WsfArticulatedPart::SlewMode::cSLEW_FIXED &&
                   tmpSensorPtr->GetSlewMode() != WsfArticulatedPart::SlewMode::cSLEW_FIXED)
               {
                  if (los.getReference() == uci::type::LOS_ReferenceEnum::UCI_INERTIAL)
                  {
                     double ned[3] = { 0, 0, 0 };
                     tmpSensorPtr->GetAbsoluteCuedOrientation(az, el, ned);
                  }

                  if (UCI_Util::CheckLimits(tmpSensorPtr, az, el))
                  {
                     mSensorPtr->SelectMode(GetSimulation().GetSimTime(), mSensorPtr->GetModeName(it->second));

                     UCI_LOS_Type updatedLOS(GetComponentParent());
                     //Replace current activity in activity message
                     //! Activity message includes azimuth, elevation, and reference type (inertial or body).
                     //! The inertial type corresponds to AFSIM's absolute orientation.
                     ClearActivities();
                     UCI_POST_ActivityType activity(GetComponentParent());
                     activity.Get().getActivityID().setUUID(GetInterface()->GetConnection()->generateUUID());
                     mActivityToCabability.emplace(activity.Get().getActivityID().getUUID().getValueAsString(),
                        aCapabilityCommand.getCapabilityID().getUUID().getValueAsString());
                     activity.Get().enablePointingActivityReport();
                     activity.Get().getPointingActivityReport().setPointingTypeChoiceOrdinal(uci::type::PointingType::POINTINGTYPE_CHOICE_LOSOPTION);
                     activity.Get().getPointingActivityReport().getLOSOption().setLOS_D_TypeChoiceOrdinal(uci::type::LOS_D_Type::LOS_D_TYPE_CHOICE_LOS);
                     activity.Get().getPointingActivityReport().getLOSOption().setLOS(los);

                     activity.Get().getPointingActivityReport().getLOSOption().getLOS().setAzimuth(az);
                     activity.Get().getPointingActivityReport().getLOSOption().getLOS().setElevation(el);
                     auto simTime = UCI_Util::GetTimeStamp(GetSimulation(), aSimTime);
                     activity.Get().setActualStartTime(simTime);
                     activity.Get().setActualCompletionTime(simTime);
                     activity.Get().setActivityState(uci::type::ActivityStateEnum::UCI_ACTIVE_UNCONSTRAINED);
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
      else
      {
         SetRejectedTypeUnavailable();
      }
   }
   return handled;
}

bool wsf::UCI_IRST_Component::HandleActivityCommand(double                                     aSimTime,
                                                    const uci::type::POST_ActivityCommandType& aActivityCommand)
{
   bool handled{ false };
   auto it = mActivityToCabability.find(aActivityCommand.getActivityID().getUUID().getValueAsString());
   if (it != mActivityToCabability.end())
   {
      auto modeIt = mUUID_ToModeIndex.find(it->second);
      if (modeIt != mUUID_ToModeIndex.end())
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

//=================================================================
//! Send the current sensor activity message. Activities currently
//! only include Line of Sight (LOS) data. See HandleCapabilityCommand
//! documentation for more information on LOS.
//!
//! @param aSimTime The time to send the message
//! @param aMessageState The message state of the activity message (UCI_NEW/UCI_UPDATE)
void UCI_IRST_Component::SendActivityMessage(double                                              aSimTime,
                                             const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         mActivityMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mActivityMessagePtr);
         PrintMessageSent(aSimTime, "POST_Activity");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "POST_Activity",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

//====================================================================
//! Process a received UCI message. Handles POST_Command messages.
//!
//! @param aSimTime The current simulation time
//! @param aMessage The message being processed
//! @return Returns true if the message is processed; otherwise returns false.
bool wsf::UCI_IRST_Component::ProcessMessage(double            aSimTime,
                                             const WsfMessage& aMessage)
{
   bool processed{ false };
   if (CanHandleMessage())
   {
      auto type = aMessage.GetType();

      if (DebugEnabled())
      {
         auto basePtr = dynamic_cast<const UCI_MessageBaseType*>(&aMessage);
         if (basePtr != nullptr)
         {
            auto debug = ut::log::debug() << "Message received.";
            debug.AddNote() << "Component: " << GetName();
            debug.AddNote() << "Message Type: " << aMessage.GetType();
         }
      }

      if (type == UCI_POST_CommandMessage::GetTypeId())
      {
         const auto& message = dynamic_cast<const UCI_POST_CommandMessage&>(aMessage);
         processed = HandleCommandMessage(aSimTime, message);
      }
      else if (type == UCI_POST_SettingsCommandMessage::GetTypeId())
      {
         const auto& message = dynamic_cast<const UCI_POST_SettingsCommandMessage&>(aMessage);
         processed = HandleSettingsCommand(aSimTime, message);
      }
      else
      {
         processed = UCI_SensorComponent::ProcessMessage(aSimTime, aMessage);
      }
   }
   else if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Message not received.";
      debug.AddNote() << "Component is turned off and/or not operational.";
      debug.AddNote() << "Component: " << GetName();
   }

   return processed;
}

void UCI_IRST_Component::SendCapabilityMessage(double                                              aSimTime,
                                               const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   // Send capability message
   try
   {
      if (CanHandleMessage())
      {
         mCapabilityMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mCapabilityMessagePtr);
         PrintMessageSent(aSimTime, "POST_Capability");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "POST_Capability",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void wsf::UCI_IRST_Component::SendCapabilityStatusMessage(double                                              aSimTime,
                                                          const uci::type::MessageStateEnum::EnumerationItem& aMessageState)
{
   try
   {
      if (CanHandleMessage())
      {
         mCapabilityStatusMessagePtr->Get().setMessageState(aMessageState);
         SendMessage(aSimTime, *mCapabilityStatusMessagePtr);
         PrintMessageSent(aSimTime, "POST_CapabilityStatus");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "POST_CapabilityStatus",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

void wsf::UCI_IRST_Component::UpdateAllMessages()
{
   UpdateCapabilityStatusMessage<UCI_POST_CapabilityStatusMessage>(mCapabilityStatusMessagePtr.get());
   UCI_SensorComponent::UpdateAllMessages();
}

void wsf::UCI_IRST_Component::SetAllCapabilitiesAvailability(const uci::type::CapabilityAvailabilityEnum::EnumerationItem& aAvailability)
{
   size_t capSize = mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().size();
   for (size_t i = 0; i < capSize; ++i)
   {
      mCapabilityStatusMessagePtr->Get().getMessageData().getCapabilityStatus().at(i).getAvailabilityInfo().setAvailability(aAvailability);
   }
   //Whether we are enabling or disabling all modes, the "default" mode should be the lowest available
   mLowestAvailableCapability = 0;
}

bool wsf::UCI_IRST_Component::HandleSettingsCommand(double                                 aSimTime,
                                                    const UCI_POST_SettingsCommandMessage& aMessage)
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
      SendCommandStatusMessage<UCI_POST_SettingsCommandStatusMessage, uci::type::CapabilitySettingsCommandID_Type>(aSimTime, msgState.getValue(), data.getCommandID());
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

// virtual
WsfComponent* UCI_IRST_Component::CloneComponent() const
{
   return new UCI_IRST_Component(*this);
}

// virtual
void* UCI_IRST_Component::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_UCI_IRST)
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
