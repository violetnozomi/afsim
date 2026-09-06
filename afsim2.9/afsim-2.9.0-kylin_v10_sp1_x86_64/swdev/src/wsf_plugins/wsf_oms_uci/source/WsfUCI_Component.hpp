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

#ifndef WSFUCI_COMPONENT_HPP
#define WSFUCI_COMPONENT_HPP

#include "wsf_oms_uci_export.h"

#include <string>

#include "uci/base/Accessor.h"
#include "uci/base/UUID.h"
#include "uci/factory/CapabilityID_TypeFactory.h"
#include "uci/type/MessageStateEnum.h"
#include "uci/type/DateTimeType.h"
#include "uci/type/MessageType.h"

#include "WsfComponent.hpp"
class     WsfMessage;
#include "WsfObject.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfUCI_Interface.hpp"

class     WsfScenario;
class     WsfSimulation;

namespace wsf
{

   //! An example of a simple platform component (See WsfSimplePlatformCompoent).
   //! UCI_ are meant to be single instances on platforms with only a defined quantity.
   class UCI_Component : public WsfPlatformPart
   {
      public:
         //! @name Definitions to support templated component methods.
         //@{

         // We must define the component role enum here as it is not a core value
         static const int cWSF_COMPONENT_UCI = 1234568;  // See WsfComponentRoles
         //@}

         UCI_Component(const WsfScenario& aScenario,
                       int                aPartType);
         UCI_Component(const UCI_Component& aSrc);
         virtual ~UCI_Component();

         UCI_Interface* GetInterface();

         double GetUpdateInterval() const override;
         void SetUpdateInterval(double aUpdateInterval) override;

         virtual std::set<std::string> GetCapabilityUUIDs() const { return std::set<std::string>(); }

         //! @name Component infrastructure methods.
         //@{
         WsfStringId      GetComponentName() const override { return GetNameId(); }
         const int*       GetComponentRoles() const override;
         
         void*            QueryInterface(int aRole) override;
         virtual bool     InjectConditionally(double       aSimTime,
                                              WsfPlatform& aPlatform) { return false; }
         bool             Initialize2(double aSimTime) override;

         bool ProcessInput(UtInput& aInput) override;

         //! Is this interface active (i.e., it has a valid corresponding UCI interface; otherwise it does nothing).
         bool             IsActive() { return (GetInterface() != nullptr); }

         bool             RegisterWithInterface(double aSimTime);
         void             DeregisterWithInterface();

         void             RegisterCapabilitiesWithInterface();
         void             DeregisterCapabilitiesWithInterface();

         //! Receive a raw UCI message.
         //! By default do nothing.
         virtual void ReceiveUCI_Message(const uci::type::MessageType* aMessagePtr) {};

         //! Convenience Methods
         //@{
         //! Get the simulation of which this component is a part.
         //! @note This must be called after component initialization.
         WsfSimulation& GetSimulation() const { return *GetComponentParent()->GetSimulation(); }
         //@}

         bool CanHandleMessage() const { return IsTurnedOn() && IsOperational(); }

      protected:
         std::string           mSubsystemUUID;
         std::string           mSubsystemLabel;

      private:
         bool                  mIsInitialized{ false };
         bool                  mIsRegistered     { false };
         UCI_Interface*        mUCI_InterfacePtr = nullptr;
   };

} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::UCI_Component, wsf::UCI_Component::cWSF_COMPONENT_UCI)

#endif
