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

#ifndef WSFUCI_SENSORCOMPONENT_HPP
#define WSFUCI_SENSORCOMPONENT_HPP

#include "wsf_oms_uci_export.h"

#include <iostream>
#include <string>

#include "uci/base/Accessor.h"
#include "uci/type/EntityMDT.h"
#include "uci/base/UUID.h"
#include "uci/factory/CapabilityID_TypeFactory.h"
#include "uci/type/ActivityStateEnum.h"
#include "uci/type/ApprovalEnum.h"
#include "uci/type/MessageStateEnum.h"
#include "uci/type/DateTimeType.h"
#include "uci/type/KinematicsType.h"

#include "UtCallbackHolder.hpp"
#include "UtLog.hpp"
#include "WsfDateTime.hpp"
#include "WsfComponent.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfSensor.hpp"
#include "WsfEvent.hpp"
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSingleSensorObserver.hpp"
#include "WsfUCI_PartComponent.hpp"
#include "WsfUCI_Component.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

class     WsfScenario;
class     WsfSimulation;

namespace wsf
{
class UCI_ControlRequestMessage;

class UCI_SetDefaultModeEvent : public WsfEvent
{
   public:
      UCI_SetDefaultModeEvent(WsfSensor* aSensorPtr,
                              double     aSimTime,
                              size_t     aPlatformIndex,
                              size_t     aLowestModeIndex)
         : WsfEvent(aSimTime)
         , mSensorPtr(aSensorPtr)
         , mPlatformIndex(aPlatformIndex)
         , mLowestModeIndex(aLowestModeIndex) {}

      ~UCI_SetDefaultModeEvent() override = default;
      WsfEvent::EventDisposition Execute() override;

   private:

      WsfSensor* mSensorPtr;
      size_t     mPlatformIndex;
      size_t     mLowestModeIndex;

};

//! An example of a simple platform component (See WsfSimplePlatformCompoent).
//! UCI_ are meant to be single instances on platforms with only a defined quantity.
class UCI_SensorComponent : public UCI_PartComponent,
                            public WsfSingleSensorObserver
{
   public:
      //! @name Definitions to support templated component methods.
      //@{

      // We must define the component role enum here as it is not a core value
      static const int cWSF_COMPONENT_UCI_SENSOR = 1234578;  // See WsfComponentRoles
      //@}

      UCI_SensorComponent() = delete;
      UCI_SensorComponent(const WsfScenario& aScenario,
                          int                aPartType,
                          const int          aClass);
      UCI_SensorComponent(const UCI_SensorComponent& aSrc);
      virtual ~UCI_SensorComponent() = default;
      bool ProcessInput(UtInput& aInput) override;

      void             SetUpdateMessageTime(double aUpdateTime) { mUpdateMessageTime = aUpdateTime; }
      double           GetUpdateMessageTime() const { return mUpdateMessageTime; }

      void ClearRemarks() { mRemarks.clear(); }
      std::string GetRemarks() const { return mRemarks; }
      uci::type::CommandStateEnum::EnumerationItem GetCommandState() const { return mCommandState; }

      void UpdateCommandResults(const uci::type::CommandStateEnum::EnumerationItem& aState,
                                const std::string&                                  aRemark)
      {
         mCommandState = aState;
         mRemarks = aRemark;
      }

      void*            QueryInterface(int aRole) override;
      bool             PreInitialize(double aSimTime) override;
      bool             Initialize(double aSimTime) override;
      bool             Initialize2(double aSimTime) override;
      const int*       GetComponentRoles() const override;
      WsfSensor*       GetSensor() const { return mSensorPtr; }

      const std::string& GetSensorName() const { return mSensorPtr->GetName(); }

      virtual void OnSensorDeleting(WsfSensor* aSensorPtr) override;

      virtual void OnSensorDeleted(WsfSensor* aSensorPtr) override;

      virtual void OnSensorTurnedOff(double     aSimTime,
                                     WsfSensor* aSensorPtr) override;

      virtual void OnSensorTurnedOn(double     aSimTime,
                                    WsfSensor* aSensorPtr) override;

      virtual void OnSlewComplete(double aSimTime) {};

      void SelectDefaultMode(double aSimTime);

      void SensorModeActivated(double         aSimTime,
                               WsfSensor*     aSensorPtr,
                               WsfSensorMode* aModePtr);

      void SensorModeDeactivated(double         aSimTime,
                                 WsfSensor*     aSensorPtr,
                                 WsfSensorMode* aModePtr);

   protected:

      void Attach(WsfSensor& aSensor);

      virtual void ClearActivities() = 0;

      void Detach() override;

      //!=============================================================
      //! Initializes common mode data such as the CapabilityID,
      //! ControlInterface,
      template< typename CapabilityType,
                typename CapabilityStatusType>
      inline void InitializeModeData(CapabilityType&       aCapabilityType,
                                     CapabilityStatusType& aStatusType,
                                     const size_t&         aIndex)
      {
         try
         {
            std::string label = UCI_Util::RemoveAngleBrackets(mSensorPtr->GetPlatform()->GetName() + ":" + mSensorPtr->GetName());
            auto modeName = mSensorPtr->GetModeName(aIndex);
            auto it = mCapabilityToUUID.find(modeName);
            std::string capabilityUUID = GetInterface()->GetConnection()->generateUUID().getValueAsString();
            bool defaultSet{ false };
            if (aIndex == 0)
            {
               //This should loop 0-1 times
               for (const auto& uuid : mUUID_ToModeIndex)
               {
                  if (uuid.second == 0)
                  {
                     defaultSet = true;
                     capabilityUUID = uuid.first;
                     break;
                  }
               }
            }
            if (it != mCapabilityToUUID.end() && !defaultSet)
            {
               capabilityUUID = it->second;
            }
            aCapabilityType.Get().getCapabilityID().getUUID().setValue(capabilityUUID);
            aCapabilityType.Get().getCapabilityID().setDescriptiveLabel(label);
            mSubsystemStatusMessagePtr->Get().getMessageData().getConfiguration().getCapabilityID().push_back(aCapabilityType.Get().getCapabilityID());
            mUUID_ToModeIndex.emplace(capabilityUUID, aIndex);
            UCI_CapabilityControlInterfaceEnumType controlInterfaceEnum(GetComponentParent());
            if (mSensorPtr->GetModeEntry(aIndex)->GetCueMode() == WsfSensor::SlewMode::cSLEW_FIXED)
            {
               controlInterfaceEnum.Get().setValue(uci::type::CapabilityControlInterfacesEnum::UCI_AUTO_MDF);
            }
            else
            {
               controlInterfaceEnum.Get().setValue(uci::type::CapabilityControlInterfacesEnum::UCI_CAPABILITY_COMMAND);
            }
            aCapabilityType.Get().getAcceptedInterface().push_back(controlInterfaceEnum.Get());

            aStatusType.Get().setCapabilityID(aCapabilityType.Get().getCapabilityID());
            aStatusType.Get().getAvailabilityInfo().setAvailability(uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE);

            UCI_ControlStatusCapabilityControlType capabilityControl(GetComponentParent());
            capabilityControl.Get().setCapabilityID(aCapabilityType.Get().getCapabilityID());
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
         catch (const uci::base::UCIException& ex)
         {
            ut::log::error() << ex.what() << ":" << ex.getErrorCode();
         }
      }

      template <typename CommandStatusMessage,
                typename CommandID>
      inline void SendCommandStatusMessage(double                                              aSimTime,
                                           const uci::type::MessageStateEnum::EnumerationItem& aMessageState,
                                           const CommandID&                                    aCommandID)
      {
         try
         {
            CommandStatusMessage msg(GetComponentParent());

            ISM_OwnerProducerType ownPro(GetComponentParent());
            ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
            msg.Get().getSecurityAttributesGroup().getOwnerProducer().push_back(ownPro.Get());

            msg.Get().setMessageState(aMessageState);
            auto& data = msg.Get().getMessageData();
            data.setCommandID(aCommandID);
            data.setState(GetCommandState());
            SendMessage(aSimTime, msg);
            PrintMessageSent(aSimTime, "CommandStatus");
         }
         catch (const uci::base::UCIException& ex)
         {
            ut::log::error() << ex.what() << ":" << ex.getErrorCode();
         }
      }

      virtual void SendControlRequestStatusMessage(double                                  aSimTime,
                                                   const uci::type::ControlRequestID_Type& aRequestID);

      virtual void SendControlStatusMessage(double                                              aSimTime,
                                            const uci::type::MessageStateEnum::EnumerationItem& aMessageState);

      void SendEntityMessage(double                                              aSimTime,
                             const WsfTrack&                                     aTrack,
                             const uci::type::MessageStateEnum::EnumerationItem& aMessageState,
                             const uci::type::EntitySourceEnum::EnumerationItem& aSourceType);

      virtual void UpdateAllMessages() override;

      template <typename CapabilityStatusMessage>
      inline void UpdateCapabilityStatusMessage(CapabilityStatusMessage* aMessagePtr)
      {
         try
         {
            auto& data = aMessagePtr->Get().getMessageData();
            for (size_t i = 0; i < data.getCapabilityStatus().size(); ++i)
            {
               auto availability = uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE;
               auto& currentAvailability = data.getCapabilityStatus().at(i).getAvailabilityInfo().getAvailability();
               if (!CanHandleMessage() ||
                  currentAvailability == uci::type::CapabilityAvailabilityEnum::UCI_DISABLED)
               {
                  availability = uci::type::CapabilityAvailabilityEnum::UCI_DISABLED;
               }
               data.getCapabilityStatus().at(i).getAvailabilityInfo().setAvailability(availability);
            }
         }
         catch (const uci::base::UCIException& ex)
         {
            ut::log::error() << ex.what() << ":" << ex.getErrorCode();
         }
      }

      template <typename CapabilityStatusMessage>
      inline uci::type::CapabilityAvailabilityEnum::EnumerationItem GetCapabilityAvailability(const CapabilityStatusMessage* aMessagePtr,
                                                                                              const size_t aIndex) const
      {
         auto availability = uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE;
         auto& currentAvailability = aMessagePtr->Get().getMessageData().getCapabilityStatus().at(aIndex).getAvailabilityInfo().getAvailability();
         if (!CanHandleMessage() ||
            currentAvailability == uci::type::CapabilityAvailabilityEnum::UCI_DISABLED)
         {
            availability = uci::type::CapabilityAvailabilityEnum::UCI_DISABLED;
         }
         return availability;
      }

      template<typename ActivityMessage>
      inline void SlewComplete(double           aSimTime,
                               ActivityMessage* aMessagePtr)
      {
         if (mIsCommanded)
         {
            mIsCommanded = false;
            auto& activity = aMessagePtr->Get().getMessageData().getActivity();
            auto& lastActivity = activity.at(activity.size() - 1);
            lastActivity.setActivityState(uci::type::ActivityStateEnum::UCI_COMPLETED);
            SendActivityMessage(aSimTime, uci::type::MessageStateEnum::UCI_UPDATE);

            double eventTime{ aSimTime + mSensorPtr->GetCurrentMode()->GetFrameTime() };
            size_t platformIndex{ mSensorPtr->GetPlatform()->GetIndex() };
            auto resetModeEvent = ut::make_unique<UCI_SetDefaultModeEvent>(mSensorPtr,
                                                                           eventTime,
                                                                           platformIndex,
                                                                           mLowestAvailableCapability);
            GetSimulation().AddEvent(std::move(resetModeEvent));
         }
      }

      virtual void SetAllCapabilitiesAvailability(const uci::type::CapabilityAvailabilityEnum::EnumerationItem& aAvailability) = 0;

      virtual void SetCommandAccepted(double aAz, double aEl);

      virtual void SetRejectedOutOfLimits(double aAz, double aEl);

      virtual void SetRejectedFixedMode(const std::string& aModeName,
                                        const std::string& aCapabilityUUID);

      virtual void SetRejectedModeDisabled(const std::string& aModeName, const std::string& aCapabilityUUID);

      virtual void SetRejectedTypeUnavailable();

      WsfSensor*                                  mSensorPtr{ nullptr };
      std::string                                 mSensorName;
      int                                         mClass;
      std::map<std::string, size_t>               mUUID_ToModeIndex;
      size_t                                      mLowestAvailableCapability{ 0 };

   private:

      void GetEntityId(const WsfTrackId& aTrackId,
                       UCI_EntityID_Type& aEntityID);

      void GetSignalId(const WsfStringId& aEmmiterId,
                       UCI_SignalID_Type& aSignalID);


      virtual uci::type::ActivityStateEnum::EnumerationItem GetActivityState() const;

      std::map<WsfTrackId, UCI_EntityID_Type>  mTrackToEntityIdMap;
      std::map<std::string, UCI_SignalID_Type> mSignalIdMap;
      UtCallbackHolder                         mCallbacks;
};

} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::UCI_SensorComponent, wsf::UCI_SensorComponent::cWSF_COMPONENT_UCI_SENSOR)

#endif
