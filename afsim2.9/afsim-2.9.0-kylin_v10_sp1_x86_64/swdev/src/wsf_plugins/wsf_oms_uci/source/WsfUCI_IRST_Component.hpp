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

#ifndef WSFUCI_IRST_COMPONENT_HPP
#define WSFUCI_IRST_COMPONENT_HPP

#include "wsf_oms_uci_export.h"

#include <iostream>
#include <string>

#include "uci/base/AbstractServiceBusConnection.h"
#include "uci/type/POST_CapabilityType.h"

#include "UtMemory.hpp"
#include "WsfDateTime.hpp"
#include "WsfSensor.hpp"
#include "WsfPlatform.hpp"

#include "WsfUCI_SensorComponent.hpp"
#include "WsfUCI_Messages.hpp"

#include "WsfProcessor.hpp"

namespace wsf
{

//! An example of a simple platform component (See WsfSimplePlatformCompoent).
//! UCI_ are meant to be single instances on platforms with only a defined quantity.
class UCI_IRST_Component : public UCI_SensorComponent
{
   public:

      UCI_IRST_Component(WsfScenario& aScenario);
      UCI_IRST_Component(const UCI_IRST_Component& aSrc);
      virtual ~UCI_IRST_Component();

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
      WsfObject* Clone() const { return new UCI_IRST_Component(*this); }
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

      bool ProcessMessage(double            aSimTime,
                          const WsfMessage& aMessage) override;

      // todo how to signify track drops?
      //virtual void OnSensorTrackDropped(double          aSimTime,
      //                                  WsfSensor*      aSensorPtr,
      //                                  const WsfTrack& aTrack) override;
      //@}

   protected:

      virtual void SetOwnerProducer(const ISM_OwnerProducerType& aOwnerType) override;

   private:

      void ClearActivities() override;

      bool HandleCommandMessage(double                         aSimTime,
                                const UCI_POST_CommandMessage& aMessage);

      bool HandleCapabilityCommand(double                                       aSimTime,
                                   const uci::type::POST_CapabilityCommandType& aCapabilityCommand);

      bool HandleActivityCommand(double                                     aSimTime,
                                 const uci::type::POST_ActivityCommandType& aActivityCommand);

      void SendActivityMessage(double                                              aSimTime,
                               const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void SendCapabilityMessage(double                                              aSimTime,
                                 const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void SendCapabilityStatusMessage(double                                              aSimTime,
                                       const uci::type::MessageStateEnum::EnumerationItem& aMessageState) override;

      void UpdateAllMessages() override;

      void SetAllCapabilitiesAvailability(const uci::type::CapabilityAvailabilityEnum::EnumerationItem& aAvailability) override;

      bool HandleSettingsCommand(double                                 aSimTime,
                                 const UCI_POST_SettingsCommandMessage& aMessage);

      std::unique_ptr<UCI_POST_ActivityMessage>         mActivityMessagePtr;
      std::unique_ptr<UCI_POST_CapabilityMessage>       mCapabilityMessagePtr;
      std::unique_ptr<UCI_POST_CapabilityStatusMessage> mCapabilityStatusMessagePtr;

};

}  // namespace wsf

enum
{
   cWSF_COMPONENT_UCI_IRST = 12345679
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::UCI_IRST_Component, cWSF_COMPONENT_UCI_IRST)

#endif
