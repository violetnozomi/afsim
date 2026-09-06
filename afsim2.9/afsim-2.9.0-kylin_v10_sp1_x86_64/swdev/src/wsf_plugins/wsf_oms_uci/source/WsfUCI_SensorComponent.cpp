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

#include "WsfUCI_SensorComponent.hpp"

#include <string>
#include <sstream>

#include "UtLog.hpp"
#include "UtInput.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtMemory.hpp"
#include "WsfDateTime.hpp"
#include "WsfESM_Sensor.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisInterface.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfTrack.hpp"
#include "WsfUCI_Utils.hpp"

#include "uci/type/EntityMDT.h"
#include "uci/type/ControlRequestID_Type.h"

namespace wsf
{

UCI_SensorComponent::UCI_SensorComponent(const WsfScenario& aScenario,
                                         int                aPartType,
                                         const int          aClass)
   : UCI_PartComponent(aScenario, aPartType)
   , mClass(aClass)
{
}

UCI_SensorComponent::UCI_SensorComponent(const UCI_SensorComponent& aSrc)
   : UCI_PartComponent(aSrc)
   , mSensorName(aSrc.mSensorName)
   , mClass(aSrc.mClass)
   , mUUID_ToModeIndex(aSrc.mUUID_ToModeIndex)
{
}

bool UCI_SensorComponent::ProcessInput(UtInput& aInput)
{
   auto command = aInput.GetCommand();
   bool myCommand{ true };
   if (command == "sensor")
   {
      aInput.ReadValue(mSensorName);
   }
   else if (command == "mode_uuid")
   {
      std::string uuid;
      std::string capability;
      aInput.ReadValue(capability);
      //UUID should be in the format ########-####-####-####-############
      //where # is [A-Z0-9]
      aInput.ReadValue(uuid);
      mCapabilityToUUID[capability] = uuid;
   }
   else if (command == "default_mode_uuid")
   {
      std::string uuid;
      //UUID should be in the format ########-####-####-####-############
      //where # is [A-Z0-9]
      aInput.ReadValue(uuid);
      mUUID_ToModeIndex[uuid] = 0;
   }
   else if (command == "capability_uuid")
   {
      std::string uuid;
      std::string capability;
      aInput.ReadValue(capability);
      //UUID should be in the format ########-####-####-####-############
      //where # is [A-Z0-9]
      aInput.ReadValue(uuid);
      mCapabilityToUUID[capability] = uuid;
   }
   else
   {
      myCommand = UCI_PartComponent::ProcessInput(aInput);
   }
   return myCommand;
}

const int* UCI_SensorComponent::GetComponentRoles() const
{
   static const int roles[] = { cWSF_COMPONENT_UCI_SENSOR,
                                cWSF_COMPONENT_UCI_PART,
                                cWSF_COMPONENT_UCI,
                                cWSF_COMPONENT_PLATFORM_PART,
                                cWSF_COMPONENT_NULL
                              };
   return roles;
}

// virtual
void* UCI_SensorComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_UCI_SENSOR)
   {
      return this;
   }
   else
   {
      return UCI_PartComponent::QueryInterface(aRole);
   }
}

bool UCI_SensorComponent::PreInitialize(double aSimTime)
{
   bool ok = UCI_Component::PreInitialize(aSimTime);
   if (IsActive())
   {
      if (mSensorName.empty())
      {
         auto error = ut::log::error() << "Component must have a sensor associated with it.";
         error.AddNote() << "Add the \"sensor <sensor_name>\" command to the component.";
         return false;
      }
      unsigned entry = 0;
      do
      {
         auto sensorPtr = static_cast<WsfSensor*>(GetComponentParent()->GetComponentEntryByRole(cWSF_COMPONENT_SENSOR, entry));
         if (sensorPtr != nullptr)
         {
            if (sensorPtr->GetClass() == mClass &&
                sensorPtr->GetName() == mSensorName)
            {
               Attach(*sensorPtr);
               break;
            }
         }
         else
         {
            break;
         }
         ++entry;
      }
      while (true);
   }
   return ok;
}

bool wsf::UCI_SensorComponent::Initialize(double aSimTime)
{
   if (mSensorPtr == nullptr)
   {
      auto error = ut::log::error() << "Sensor was not attached to uci_component.";
      error.AddNote() << "Make sure that a corresponding sensor with the name given in the \"sensor\" command exists.";
      error.AddNote() << "Component: " << GetName();
      error.AddNote() << "Sensor: " << mSensorName;
      return false;
   }
   return UCI_Component::Initialize(aSimTime);
}

bool wsf::UCI_SensorComponent::Initialize2(double aSimTime)
{
   try
   {
      if (IsActive())
      {
         auto& subsystemData = mSubsystemStatusMessagePtr->Get().getMessageData();
         if (mSubsystemLabel.empty())
         {
            mSubsystemLabel = UCI_Util::RemoveAngleBrackets(GetPlatform()->GetName() + ":" + mSensorPtr->GetName());
         }
         subsystemData.getSubsystemState().setValue(UCI_Util::GetSubsystemState(mSensorPtr));
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }

   return UCI_PartComponent::Initialize2(aSimTime);
}

// ================================================================================================
//! Attaches the UCI component to the given sensor
//! @param aSensor The sensor to attach observer to
void UCI_SensorComponent::Attach(WsfSensor& aSensor)
{
   if (mSensorPtr != nullptr)
   {
      Detach();
   }

   aSensor.AttachObserver(this);
   mSensorPtr = &aSensor;
   mCallbacks.Add(WsfObserver::SensorModeActivated(&GetSimulation()).Connect(&UCI_SensorComponent::SensorModeActivated, this));
   mCallbacks.Add(WsfObserver::SensorModeDeactivated(&GetSimulation()).Connect(&UCI_SensorComponent::SensorModeDeactivated, this));
   mCallbacks.Add(mSensorPtr->SlewComplete.Connect(&UCI_SensorComponent::OnSlewComplete, this));

   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Component attached to sensor.";
      debug.AddNote() << "Component: " << GetName();
      debug.AddNote() << "Sensor: " << mSensorPtr->GetName();
   }
}

// ================================================================================================
//! Detaches the component observer from the sensor
void UCI_SensorComponent::Detach()
{
   if (mSensorPtr != nullptr)
   {
      mSensorPtr->DetachObserver(this);
      mCallbacks.Clear();
      if (DebugEnabled())
      {
         auto debug = ut::log::debug() << "Component detached from sensor.";
         debug.AddNote() << "Component: " << GetName();
         debug.AddNote() << "Sensor: " << mSensorPtr->GetName();
      }

      mSensorPtr = nullptr;
   }
}

void wsf::UCI_SensorComponent::UpdateAllMessages()
{
   UpdateSubsystemStatusMessage(UCI_Util::GetSubsystemState(mSensorPtr));
}

void UCI_SensorComponent::SendControlRequestStatusMessage(double                                  aSimTime,
                                                          const uci::type::ControlRequestID_Type& aRequestID)
{
   try
   {
      if (CanHandleMessage())
      {
         UCI_ControlRequestStatusMessage msg(GetComponentParent());
         auto& data = msg.Get().getMessageData();
         data.setControlRequestID(aRequestID);
         auto approvalType = GetApprovalType();
         data.setApprovalType(approvalType);
         data.enableRemarks();
         data.setRemarks("Always approved");
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
void UCI_SensorComponent::SendControlStatusMessage(double                                              aSimTime,
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

//==================================================================
//! Sends Entity messages based on WsfTrack data. Data includes:
//! track creation time, source and kinematic information, and, if applicable, IFF, pulse,
//! pulsewidth, and frequency data.
//!
//! @param aSimTime The current simulation time
//! @param aTrack The track of the entity being described in this message
//! @param aMessageState The message state of the message (UCI_NEW/UCI_UPDATE)
//! @param aSourceType The source of the message
void UCI_SensorComponent::SendEntityMessage(double                                              aSimTime,
                                            const WsfTrack&                                     aTrack,
                                            const uci::type::MessageStateEnum::EnumerationItem& aMessageState,
                                            const uci::type::EntitySourceEnum::EnumerationItem& aSourceType)
{
   try
   {
      if (CanHandleMessage())
      {
         UCI_EntityMessage eMsg(GetComponentParent());
         auto& msg = eMsg.Get();

         ISM_OwnerProducerType ownPro(GetComponentParent());
         ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
         msg.getSecurityAttributesGroup().getOwnerProducer().push_back(ownPro.Get());

         auto& data = msg.getMessageData();
         msg.setMessageState(aMessageState);
         data.getCreationTimestamp().setDateTime(UCI_Util::GetTimeStamp(GetSimulation(), aSimTime));
         UCI_EntityID_Type entityID(GetComponentParent());
         GetEntityId(aTrack.GetTrackId(), entityID);
         data.setEntityID(entityID.Get());
         auto& source = data.getSource();
         source.getSystemID().setUUID(GetInterface()->GetConnection()->getMySystemUUID());
         source.setSourceType(aSourceType);
         source.enableSourceTypeID();
         source.getSourceTypeID().setEntitySourceIdentifierTypeChoiceOrdinal(uci::type::EntitySourceIdentifierType::ENTITYSOURCEIDENTIFIERTYPE_CHOICE_CAPABILITY);
         auto index = mSensorPtr->GetCurrentMode()->GetModeIndex();
         for (const auto& uuid : mUUID_ToModeIndex)
         {
            if (uuid.second == index)
            {
               UCI_CapabilityID_Type capabilityId(GetComponentParent());
               capabilityId.Get().getUUID().setValue(uuid.first);
               source.getSourceTypeID().getCapability().setCapabilityID(capabilityId.Get());
               break;
            }
         }

         UCI_KinematicsType kinematics(GetComponentParent());
         auto updateTime = UCI_Util::GetTimeStamp(GetSimulation(), aTrack.GetUpdateTime());
         UCI_Util::GetKinematics(updateTime, kinematics.Get(), mSensorPtr, aTrack);
         data.setKinematics(kinematics.Get());
         data.getIdentity().setIdentityTimestamp(updateTime);
         if (mSensorPtr->GetCurrentMode()->ReportsIFF_Status())
         {
            UCI_StandardIdentityConfidenceType standard(GetComponentParent());
            standard.Get().setStandardIdentity(UCI_Util::GetStandardIdentity(aTrack));
            standard.Get().setConfidence(100);
            data.getIdentity().getStandard().push_back(standard.Get());
         }

         auto signalListPtr = aTrack.GetSignalList();
         if (signalListPtr != nullptr)
         {
            bool reportsPW = mSensorPtr->GetCurrentMode()->ReportsPulseWidth();
            bool reportsPRI = mSensorPtr->GetCurrentMode()->ReportsPulseRepititionInterval();
            bool reportsFrequency = mSensorPtr->GetCurrentMode()->ReportsFrequency();
            for (const auto& signal : signalListPtr->mSignalList)
            {
               UCI_EntitySignalSummaryType summary(GetComponentParent());
               auto& description = summary.Get().getSignalDescription();
               if (reportsPW)
               {
                  description.enablePulseWidthAverage();
                  description.enablePulseWidthMax();
                  description.enablePulseWidthMin();

                  auto pulseWidth = static_cast<uci::type::DurationTypeValue>(signal.mPulseWidth * 1e9);
                  description.setPulseWidthAverage(pulseWidth);
                  description.setPulseWidthMax(pulseWidth);
                  description.setPulseWidthMin(pulseWidth);
               }

               if (reportsPRI)
               {
                  description.enablePRI_Average();
                  description.enablePRI_Max();
                  description.enablePRI_Min();
                  description.enablePRI_Type();

                  auto pri = static_cast<uci::type::DurationTypeValue>(signal.mPulseRepetitionInterval * 1e9);
                  description.setPRI_Average(pri);
                  description.setPRI_Max(pri);
                  description.setPRI_Min(pri);
                  description.setPRI_Type(uci::type::PRI_TypeEnum::UCI_FIXED_PRI);
               }

               if (reportsFrequency)
               {
                  description.enableFrequencyAverage();
                  description.setFrequencyAverage((signal.mUpperFrequency + signal.mLowerFrequency) / 2);
               }

               summary.Get().enableSignalID();
               //emmiter type id list
               UCI_SignalID_Type signalId(GetComponentParent());
               GetSignalId(signal.mEmitterTruthId, signalId);
               summary.Get().setSignalID(signalId.Get());
               data.getSignalSummary().push_back(summary.Get());
            }
         }

         //todo find out where to put emitter data
         /*for (size_t i = 0; i < aTrack.GetEmitterTypeIdCount(); ++i)
         {
            WsfTrack::EmitterTypeData emitterType;
            aTrack.GetEmitterTypeIdEntry(i, emitterType);
            emitterType.
         }*/

         SendMessage(aSimTime, eMsg);

         PrintMessageSent(aSimTime, "Entity");
      }
      else
      {
         PrintMessageNotSent(aSimTime,
                             "Entity",
                             "Component " + GetName() + " is turned off and/or not operational.");
      }
   }
   catch (const uci::base::UCIException& ex)
   {
      ut::log::error() << ex.what() << ":" << ex.getErrorCode();
   }
}

// ================================================================================================
//! Get the corresponding entity ID from the track ID. If one doesn't already exist, create one and
//! add it to the map.
//! @param aTrackId The track ID
//! @param aEntityId The returned entity ID
void UCI_SensorComponent::GetEntityId(const WsfTrackId&  aTrackId,
                                      UCI_EntityID_Type& aEntityID)
{
   auto iter = mTrackToEntityIdMap.find(aTrackId);
   if (iter != mTrackToEntityIdMap.end())
   {
      aEntityID.Get().setUUID(iter->second.Get().getUUID());
      if (iter->second.Get().hasDescriptiveLabel())
      {
         aEntityID.Get().setDescriptiveLabel(iter->second.Get().getDescriptiveLabel());
      }
   }
   else
   {
      aEntityID.Get().setUUID(GetInterface()->GetConnection()->generateUUID());
      WsfDisInterface* disInterface = WsfDisInterface::Find(GetSimulation());
      bool useTrackId = disInterface == nullptr;
      std::string id;
      if (!useTrackId)
      {
         auto targetPlatform = GetSimulation().GetPlatformByName(aTrackId.GetOwningPlatformId());

         auto disPlatform = disInterface->GetDisPlatform(targetPlatform->GetIndex());
         if (disPlatform != nullptr)
         {
            auto application = disPlatform->GetEntityId().GetApplication();
            auto entity = disPlatform->GetEntityId().GetEntity();
            auto site = disPlatform->GetEntityId().GetSite();
            id = std::to_string(site) + ":" + std::to_string(application) + ":" + std::to_string(entity);
         }
         else
         {
            useTrackId = true;
         }
      }
      if (useTrackId)
      {
         std::stringstream ss;
         ss << aTrackId;
         id = UCI_Util::RemoveAngleBrackets(ss.str());
      }

      aEntityID.Get().setDescriptiveLabel(id);
      mTrackToEntityIdMap.emplace(aTrackId, aEntityID);
   }
}

void UCI_SensorComponent::GetSignalId(const WsfStringId& aEmitterId,
                                      UCI_SignalID_Type& aSignalID)
{
   auto iter = mSignalIdMap.find(aEmitterId);
   if (iter != mSignalIdMap.end())
   {
      aSignalID.Get().setUUID(iter->second.Get().getUUID());
      if (iter->second.Get().hasDescriptiveLabel())
      {
         aSignalID.Get().setDescriptiveLabel(iter->second.Get().getDescriptiveLabel());
      }
   }
   else
   {
      aSignalID.Get().setUUID(GetInterface()->GetConnection()->generateUUID());
      aSignalID.Get().setDescriptiveLabel(aEmitterId);
      mSignalIdMap.emplace(aEmitterId, aSignalID);
   }
}

uci::type::ActivityStateEnum::EnumerationItem wsf::UCI_SensorComponent::GetActivityState() const
{
   auto activityState = uci::type::ActivityStateEnum::UCI_ENABLED;
   if (UCI_Util::GetSubsystemState(mSensorPtr) == uci::type::SubsystemStateEnum::UCI_OFF)
   {
      activityState = uci::type::ActivityStateEnum::UCI_DISABLED;
   }
   return activityState;
}

void UCI_SensorComponent::OnSensorDeleting(WsfSensor* aSensorPtr)
{
}

void UCI_SensorComponent::OnSensorDeleted(WsfSensor* aSensorPtr)
{
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Sensor deleted.";
      debug.AddNote() << "Sensor: " << aSensorPtr->GetName();
   }

   Detach();
}

void UCI_SensorComponent::OnSensorTurnedOff(double     aSimTime,
                                            WsfSensor* aSensorPtr)
{
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Sensor turned off.";
      debug.AddNote() << "T= " << aSimTime;
      debug.AddNote() << "Sensor: " << aSensorPtr->GetName();
   }

   mIsCommanded = false;
   uci::type::MessageStateEnum::EnumerationItem messageState = uci::type::MessageStateEnum::UCI_UPDATE;
   ClearActivities();
   //Not sure if the activity message needs to be sent at this point
   UpdateAllMessages();
   SetAllCapabilitiesAvailability(uci::type::CapabilityAvailabilityEnum::UCI_DISABLED);
   SendActivityMessage(aSimTime, messageState);
   SendCapabilityStatusMessage(aSimTime, messageState);
   SendSubsystemStatusMessage(aSimTime, messageState);
   SendSystemStatusMessage(aSimTime, messageState);
}

void UCI_SensorComponent::OnSensorTurnedOn(double     aSimTime,
                                           WsfSensor* aSensorPtr)
{
   mIsCommanded = false;
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Sensor turned on.";
      debug.AddNote() << "T= " << aSimTime;
      debug.AddNote() << "Sensor: " << aSensorPtr->GetName();
   }
   uci::type::MessageStateEnum::EnumerationItem messageState = uci::type::MessageStateEnum::UCI_UPDATE;
   //Not sure if the activity message needs to be sent at this point
   UpdateAllMessages();
   SetAllCapabilitiesAvailability(uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE);
   SendActivityMessage(aSimTime, messageState);
   SendCapabilityStatusMessage(aSimTime, messageState);
   SendSubsystemStatusMessage(aSimTime, messageState);
   SendSystemStatusMessage(aSimTime, messageState);
}

void UCI_SensorComponent::SensorModeActivated(double         aSimTime,
                                              WsfSensor*     aSensorPtr,
                                              WsfSensorMode* aModePtr)
{
   if (aSensorPtr == mSensorPtr)
   {
      if (!mInitialMessagesSent)
      {
         mInitialMessagesSent = true;
      }
      //Not sure if the activity message needs to be sent at this point
      UpdateAllMessages();
   }
}

void UCI_SensorComponent::SensorModeDeactivated(double         aSimTime,
                                                WsfSensor*     aSensorPtr,
                                                WsfSensorMode* aModePtr)
{
   if (aSensorPtr == mSensorPtr)
   {
      auto messageState = uci::type::MessageStateEnum::UCI_UPDATE;
      if (!mInitialMessagesSent)
      {
         messageState = uci::type::MessageStateEnum::UCI_NEW;
         mInitialMessagesSent = true;
      }
      //Not sure if the activity message needs to be sent at this point
      UpdateAllMessages();
      SendActivityMessage(aSimTime, messageState);
      SendCapabilityStatusMessage(aSimTime, messageState);
   }
}

void wsf::UCI_SensorComponent::SetCommandAccepted(double aAz,
                                                  double aEl)
{
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Command ACCEPTED.";
      debug.AddNote() << "Scan center set.";
      debug.AddNote() << "Az: " << aAz;
      debug.AddNote() << "El: " << aEl;
   }
   UpdateCommandResults(uci::type::CommandStateEnum::UCI_ACCEPTED, "");
   mIsCommanded = true;
}

void wsf::UCI_SensorComponent::SetRejectedOutOfLimits(double aAz,
                                                      double aEl)
{
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Command REJECTED.";
      debug.AddNote() << "Desired center out of cue limits.";
      debug.AddNote() << "Desired Az: " << aAz;
      debug.AddNote() << "Desired El: " << aEl;
      debug.AddNote() << "minAzCue: " << mSensorPtr->GetMinAzCue();
      debug.AddNote() << "minElCue: " << mSensorPtr->GetMinElCue();
      debug.AddNote() << "maxAzCue: " << mSensorPtr->GetMaxAzCue();
      debug.AddNote() << "maxElCue: " << mSensorPtr->GetMaxElCue();
   }
   UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Desired center out of cue limits");
}

void wsf::UCI_SensorComponent::SetRejectedFixedMode(const std::string& aModeName,
                                                    const std::string& aCapabilityUUID)
{
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Command REJECTED.";
      debug.AddNote() << "Sensor mode is set to a fixed cue and slew mode.";
      debug.AddNote() << "Sensor: " << mSensorName;
      debug.AddNote() << "Mode name: " << aModeName;
      debug.AddNote() << "Mode UUID: " << aCapabilityUUID;
   }
   UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Fixed sensor/mode");
}

void wsf::UCI_SensorComponent::SetRejectedModeDisabled(const std::string& aModeName,
                                                       const std::string& aCapabilityUUID)
{
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Command REJECTED.";
      debug.AddNote() << "Capability is disabled.";
      debug.AddNote() << "Sensor: " << mSensorName;
      debug.AddNote() << "Mode name: " << aModeName;
      debug.AddNote() << "Mode UUID: " << aCapabilityUUID;
   }
   UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Capability is disabled");
}

void wsf::UCI_SensorComponent::SetRejectedTypeUnavailable()
{
   if (DebugEnabled())
   {
      auto debug = ut::log::debug() << "Command REJECTED.";
      debug.AddNote() << "Command type unavailable.";
   }
   UpdateCommandResults(uci::type::CommandStateEnum::UCI_REJECTED, "Command type unavailable");
}

void wsf::UCI_SensorComponent::SelectDefaultMode(double aSimTime)
{
   if (mSensorPtr->IsTurnedOn() &&
       mSensorPtr->IsOperational() &&
       mSensorPtr->GetCurrentMode()->GetModeIndex() != mLowestAvailableCapability)
   {
      mSensorPtr->SelectMode(aSimTime, mSensorPtr->GetModeName(mLowestAvailableCapability));
   }
}

// ================================================================================================
//! Resets the sensor to its default mode if the corresponding platform still exists
//! in the simulation.
WsfEvent::EventDisposition wsf::UCI_SetDefaultModeEvent::Execute()
{
   if (GetSimulation()->GetPlatformByIndex(mPlatformIndex) != nullptr &&
       mSensorPtr->IsTurnedOn() &&
       mSensorPtr->IsOperational() &&
       mSensorPtr->GetCurrentMode()->GetModeIndex() != mLowestModeIndex)
   {
      mSensorPtr->SelectMode(GetTime(), mSensorPtr->GetModeName(mLowestModeIndex));
   }
   return WsfEvent::cDELETE;
}

} // wsf
