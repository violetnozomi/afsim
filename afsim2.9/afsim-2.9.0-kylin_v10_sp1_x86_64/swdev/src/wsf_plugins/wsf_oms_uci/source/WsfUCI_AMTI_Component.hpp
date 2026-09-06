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

#ifndef WSFUCIRADARCOMPONENT_HPP
#define WSFUCIRADARCOMPONENT_HPP

#include "wsf_oms_uci_export.h"

#include <iostream>
#include <string>

#include "uci/base/AbstractServiceBusConnection.h"
#include "uci/type/POST_CapabilityType.h"

#include "UtMemory.hpp"
#include "WsfSensor.hpp"
#include "WsfPlatform.hpp"

#include "WsfSingleSensorObserver.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_SensorComponent.hpp"

#include "WsfProcessor.hpp"

namespace wsf
{

//! An example of a simple platform component (See WsfSimplePlatformCompoent).
//! UCI_ are meant to be single instances on platforms with only a defined quantity.
class UCI_AMTI_Component : public UCI_SensorComponent
{
   public:

      UCI_AMTI_Component(WsfScenario& aScenario);
      UCI_AMTI_Component(const UCI_AMTI_Component& aSrc);
      virtual ~UCI_AMTI_Component();

      //! @name Component infrastructure methods.
      //@{
      bool             Initialize2(double aSimTime) override;
      const int*       GetComponentRoles() const override;
      WsfComponent*    CloneComponent() const override;
      void*            QueryInterface(int aRole) override;
      bool             InjectConditionally(double       aSimTime,
                                           WsfPlatform& aPlatform) override;
      //@}

      // Framework methods.
      //@{
      WsfObject* Clone() const { return new UCI_AMTI_Component(*this); }
      //@}

      // Observer methods (TODO implement for uci messages)
      //@{

      virtual void OnSensorTrackInitiated(double          aSimTime,
                                          WsfSensor*      aSensorPtr,
                                          const WsfTrack& aTrack) override;

      virtual void OnSensorTrackUpdated(double          aSimTime,
                                        WsfSensor*      aSensorPtr,
                                        const WsfTrack& aTrack) override;

      // todo how to signify track drops?
      //virtual void OnSensorTrackDropped(double          aSimTime,
      //                                  WsfSensor*      aSensorPtr,
      //                                  const WsfTrack& aTrack) override;
      //@}

      bool ReceiveMessage(double            aSimTime,
                          const WsfMessage& aMessage) override;

      virtual void OnSlewComplete(double aSimTime) override;


   protected:

      virtual void SetOwnerProducer(const ISM_OwnerProducerType& aOwnerType) override;

   private:

      void ClearActivities() override;

      uci::type::AMTI_SubCapabilityEnum::EnumerationItem GetSubCapabilityEnum() const;

      bool HandleCommandMessage(const UCI_AMTI_CommandMessage& aMessage);

      bool HandleSettingsCommand(double                                 aSimTime,
                                 const UCI_AMTI_SettingsCommandMessage& aMessage);

      bool HandleCapabilityCommand(double                            aSimTime,
                                   const uci::type::AMTI_CapabilityCommandType& aCapabilityCommand);

      bool HandleActivityCommand(double                          aSimTime,
                                 const uci::type::AMTI_ActivityCommandType& aActivityCommand);

      void SendActivityMessage(double                                   aSimTime,
                               const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void SendCapabilityMessage(double                                   aSimTime,
                                 const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void SendCapabilityStatusMessage(double                                   aSimTime,
                                       const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void UpdateAllMessages() override;

      void SetAllCapabilitiesAvailability(const uci::type::CapabilityAvailabilityEnum::EnumerationItem& aAvailability) override;

      std::unique_ptr<UCI_AMTI_ActivityMessage>         mActivityMessagePtr;
      std::unique_ptr<UCI_AMTI_CapabilityMessage>       mCapabilityMessagePtr;
      std::unique_ptr<UCI_AMTI_CapabilityStatusMessage> mCapabilityStatusMessagePtr;
};

}  // namespace wsf

enum
{
   cWSF_COMPONENT_UCI_AMTI = 12356789
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::UCI_AMTI_Component, cWSF_COMPONENT_UCI_AMTI)

#endif
