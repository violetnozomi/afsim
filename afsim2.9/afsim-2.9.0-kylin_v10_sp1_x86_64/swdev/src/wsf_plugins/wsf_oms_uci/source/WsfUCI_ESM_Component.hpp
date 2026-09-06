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

#ifndef WSFUCI_ESM_COMPONENT_HPP
#define WSFUCI_ESM_COMPONENT_HPP

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
class UCI_ESM_Component : public UCI_SensorComponent
{
   public:

      UCI_ESM_Component(WsfScenario& aScenario);
      UCI_ESM_Component(const UCI_ESM_Component& aSrc);
      virtual ~UCI_ESM_Component();

      //! @name Component infrastructure methods.
      //@{
      bool             Initialize2(double aSimTime) override;
      const int*       GetComponentRoles() const override;
      WsfComponent*    CloneComponent() const override;
      void*            QueryInterface(int aRole) override;
      bool             InjectConditionally(double       aSimTime,
                                           WsfPlatform& aPlatform) override;
      //@}

      uci::type::ESM_SubCapabilityEnum::EnumerationItem GetSubCapabilityEnum() const;

      // Framework methods.
      //@{
      WsfObject* Clone() const { return new UCI_ESM_Component(*this); }
      //@}

      // Observer methods (TODO implement for uci messages)
      //@{

      virtual void OnSensorTrackInitiated(double          aSimTime,
                                          WsfSensor*      aSensorPtr,
                                          const WsfTrack& aTrack) override;

      virtual void OnSensorTrackUpdated(double          aSimTime,
                                        WsfSensor*      aSensorPtr,
                                        const WsfTrack& aTrack) override;

      void OnSlewComplete(double aSimTime) override;

      // todo how to signify track drops?
      //virtual void OnSensorTrackDropped(double          aSimTime,
      //                                  WsfSensor*      aSensorPtr,
      //                                  const WsfTrack& aTrack) override;
      //@}

      bool ReceiveMessage(double            aSimTime,
                          const WsfMessage& aMessage) override;

   protected:

      virtual void SetOwnerProducer(const ISM_OwnerProducerType& aOwnerType) override;

   private:

      void ClearActivities() override;

      bool HandleCommandMessage(const UCI_ESM_CommandMessage& aMessage);

      bool HandleCapabilityCommand(double                           aSimTime,
                                   const uci::type::ESM_CapabilityCommandType& aCapabilityCommand);

      bool HandleActivityCommand(double                         aSimTime,
                                 const uci::type::ESM_ActivityCommandType& aActivityCommand);

      void SendActivityMessage(double                                   aSimTime,
                               const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void SendCapabilityMessage(double                                   aSimTime,
                                 const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void SendCapabilityStatusMessage(double                                   aSimTime,
                                       const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void UpdateAllMessages() override;

      void SetAllCapabilitiesAvailability(const uci::type::CapabilityAvailabilityEnum::EnumerationItem& aAvailability) override;

      bool HandleSettingsCommand(double                                aSimTime,
                                 const UCI_ESM_SettingsCommandMessage& aMessage);

      std::unique_ptr<UCI_ESM_ActivityMessage>         mActivityMessagePtr;
      std::unique_ptr<UCI_ESM_CapabilityMessage>       mCapabilityMessagePtr;
      std::unique_ptr<UCI_ESM_CapabilityStatusMessage> mCapabilityStatusMessagePtr;
};

}  // namespace wsf

enum
{
   cWSF_COMPONENT_UCI_ESM = 12346789
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::UCI_ESM_Component, cWSF_COMPONENT_UCI_ESM)

#endif
