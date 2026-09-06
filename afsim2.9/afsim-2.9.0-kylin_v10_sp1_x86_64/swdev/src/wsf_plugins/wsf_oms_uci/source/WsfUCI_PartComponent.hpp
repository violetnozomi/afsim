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

#ifndef WSFUCI_PARTCOMPONENT_HPP
#define WSFUCI_PARTCOMPONENT_HPP

#include "wsf_oms_uci_export.h"

#include "UtCallbackHolder.hpp"
#include "UtLog.hpp"
#include "WsfEvent.hpp"
#include "WsfUCI_Component.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"

#include "uci/type/MessageStateEnum.h"

class     WsfScenario;
class     WsfSimulation;

namespace wsf
{

//! An example of a simple platform component (See WsfSimplePlatformCompoent).
//! UCI_ are meant to be single instances on platforms with only a defined quantity.
class UCI_PartComponent : public UCI_Component
{
   public:
      //! @name Definitions to support templated component methods.
      //@{

      // We must define the component role enum here as it is not a core value
      static const int cWSF_COMPONENT_UCI_PART = 1234321;  // See WsfComponentRoles
      //@}

      UCI_PartComponent() = delete;
      UCI_PartComponent(const WsfScenario& aScenario,
                          int              aPartType);
      UCI_PartComponent(const UCI_PartComponent& aSrc);
      virtual ~UCI_PartComponent() = default;
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
      bool             Initialize2(double aSimTime) override;
      const int*       GetComponentRoles() const override;
      // Send/Receive
      //@{
      virtual void SendUpdateMessages(double                                              aSimTime,
                                      const uci::type::MessageStateEnum::EnumerationItem& aMessageState);

      virtual bool ProcessMessage(double            aSimTime,
                                  const WsfMessage& aMessage) override;
      //@}

      virtual std::set<std::string> GetCapabilityUUIDs() const override;

   protected:      

      struct Controller
      {
         Controller() = default;
         Controller(const std::string& aSystemID,
                    const std::string& aServiceID)
            : mSystemID(aSystemID)
            , mServiceID(aServiceID) {};

         bool operator<(const Controller& aController) const
         {
            if (mSystemID != aController.mSystemID)
            {
               return mSystemID < aController.mSystemID;
            }
            return mServiceID < aController.mServiceID;
         }

         bool operator==(const Controller& aController) const
         {
            return mSystemID == aController.mSystemID &&
                   mServiceID == aController.mServiceID;
         }

         std::string mSystemID;
         std::string mServiceID;
      };

      bool IsCommandAccepted() const { return mCommandState == uci::type::CommandStateEnum::UCI_ACCEPTED; }

      virtual void SetOwnerProducer(const ISM_OwnerProducerType& aOwnerType);

      virtual void ClearActivities() = 0;

      virtual void Detach() {};

      bool CheckPartStatus(WsfArticulatedPart* aPartPtr);

      bool HandleControlRequestMessage(const UCI_ControlRequestMessage& aMessage);

      bool HasControl(const std::string&         aCapbilityUUID,
                      uci::type::ServiceID_Type& aServiceID,
                      uci::type::SystemID_Type&  aSystemID);

      virtual void SendActivityMessage(double                                              aSimTime,
                                       const uci::type::MessageStateEnum::EnumerationItem& aMessageState) = 0;

      virtual void SendCapabilityMessage(double                                              aSimTime,
                                         const uci::type::MessageStateEnum::EnumerationItem& aMessageState) = 0;

      virtual void SendCapabilityStatusMessage(double                                              aSimTime,
                                               const uci::type::MessageStateEnum::EnumerationItem& aMessageState) = 0;

      template <typename CommandStatusMessage,
                typename CommandID>
      inline void SendCommandStatusMessage(double                                              aSimTime,
                                           const uci::type::MessageStateEnum::EnumerationItem& aMessageState,
                                           const CommandID&                                    aCommandID)
      {
         try
         {
            CommandStatusMessage msg(GetComponentParent());
            msg.Get().setMessageState(aMessageState);

            ISM_OwnerProducerType ownPro(GetComponentParent());
            ownPro.Get().setValue(ismownpro::type::CVEnumISMOwnerProducerValues::ISMOWNPRO_USA);   //Arbitrary for now
            msg.Get().getSecurityAttributesGroup().getOwnerProducer().push_back(ownPro.Get());

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

      virtual void SendSubsystemStatusMessage(double                                              aSimTime,
                                              const uci::type::MessageStateEnum::EnumerationItem& aMessageState);

      virtual void SendSystemStatusMessage(double                                              aSimTime,
                                           const uci::type::MessageStateEnum::EnumerationItem& aMessageState);

      virtual void TurnOn(double aSimTime) override;

      virtual void TurnOff(double aSimTime) override;

      virtual void UpdateAllMessages() = 0;

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

      void UpdateSubsystemStatusMessage(uci::type::SubsystemStateEnum::EnumerationItem aState);

      virtual uci::type::ApprovalEnum::EnumerationItem GetApprovalType() const { return mApprovalType; }

      template <typename CapabilityStatusMessage>
      inline uci::type::CapabilityAvailabilityEnum::EnumerationItem GetCapabilityAvailability(const CapabilityStatusMessage* aMessagePtr,
                                                                                   const size_t                   aIndex) const
      {
         auto availability = uci::type::CapabilityAvailabilityEnum::UCI_AVAILABLE;
         auto& currentAvailability = aMessagePtr->Get().getMessageData().getCapabilityStatus().at(aIndex).getAvailabilityInfo().getAvailability();
         if (!CanHandleMessage())
         {
            availability = uci::type::CapabilityAvailabilityEnum::UCI_DISABLED;
         }
         else
         {
            availability = currentAvailability;
         }
         return availability;
      }


      virtual void SetAllCapabilitiesAvailability(const uci::type::CapabilityAvailabilityEnum::EnumerationItem& aAvailability) = 0;

      void SetRejectedCurrentlyCommanded(const std::string& aPartName);

      virtual void PrintMessageSent(double             aSimTime,
                                    const std::string& aMessageType);

      virtual void PrintMessageNotSent(double             aSimTime,
                                       const std::string& aMessageType,
                                       const std::string& aReason);

      std::unique_ptr<UCI_ControlStatusMessage>   mControlStatusMessagePtr;
      std::unique_ptr<UCI_SubsystemStatusMessage> mSubsystemStatusMessagePtr;
      std::unique_ptr<UCI_SystemStatusMessage>    mSystemStatusMessagePtr;

      std::map<std::string, std::string>          mCapabilityToUUID;
      double                                      mUpdateMessageTime{ 10 };
      std::map<std::string, std::string>          mActivityToCabability;
      uci::type::CommandStateEnum::EnumerationItem mCommandState{ uci::type::CommandStateEnum::UCI_ACCEPTED };
      std::string                                 mRemarks;
      bool                                        mInitialMessagesSent{ false };
      bool                                        mIsCommanded{ false };
      UtCallbackHolder                            mCallbacks;

      //Capability UUID to controller maps
      std::map<std::string, Controller>           mPrimaryController;
      std::map<std::string, std::set<Controller>> mSecondaryControllers;
      std::map<std::string, std::set<Controller>> mManagerControllers;
      
      uci::type::ApprovalEnum::EnumerationItem    mApprovalType;

   private:

      bool HandleAcquireMessage(const UCI_ControlRequestMessage& aMessage);

      bool HandleReleaseMessage(const UCI_ControlRequestMessage& aMessage);

      std::string GetApprovalTypeString(const uci::type::ApprovalEnum::EnumerationItem& aApprovalType);

};

class UCI_Message_Event : public WsfEvent
{
   public:
      UCI_Message_Event(UCI_PartComponent* aUCI_Component,
                        double             aSimTime,
                        size_t             aPlatformIndex,
                        double             aSendInterval = 10)
         : WsfEvent(aSimTime)
         , mUCI_ComponentPtr(aUCI_Component)
         , mSendInterval(aSendInterval)
         , mPlatformIndex(aPlatformIndex) {}

      ~UCI_Message_Event() override = default;
      WsfEvent::EventDisposition Execute() override;

   private:

      UCI_PartComponent* mUCI_ComponentPtr;
      double             mSendInterval{ 10 };
      size_t             mPlatformIndex;

};

} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::UCI_PartComponent, wsf::UCI_PartComponent::cWSF_COMPONENT_UCI_PART)

#endif
