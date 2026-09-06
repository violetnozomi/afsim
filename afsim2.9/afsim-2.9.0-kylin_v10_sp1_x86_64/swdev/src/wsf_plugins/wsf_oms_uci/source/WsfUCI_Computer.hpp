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

#ifndef WSFUCI_COMPUTER_HPP
#define WSFUCI_COMPUTER_HPP

#include "wsf_oms_uci_export.h"

#include <memory>

#include "UtCallbackHolder.hpp"

#include "WsfUCI_Component.hpp"
#include "WsfUCI_MessageFactory.hpp"

namespace wsf
{
   //! The OMS / UCI Interface Object to the UCI middleware.
   class WSF_OMS_UCI_EXPORT UCI_Computer : public UCI_Component
   {
      public:
         UCI_Computer(WsfScenario& aScenario);
         UCI_Computer(const UCI_Computer& aSrc);
         virtual ~UCI_Computer() = default;

         bool          PreInitialize(double aSimTime) override;
         bool          Initialize(double aSimTime) override;
         WsfObject*    Clone() const { return new UCI_Computer(*this); }
         const int*    GetComponentRoles() const override;
         WsfComponent* CloneComponent() const override;
         void*         QueryInterface(int aRole) override;

         bool ProcessMessage(double aSimTime,
                             const WsfMessage& aMessage) override;

         void ReceiveUCI_Message(const uci::type::MessageType* aMessagePtr) override;

      private:

         std::unique_ptr<UCI_MessageFactory>    mMessageFactoryPtr { nullptr };
   };
}

enum
{
   cWSF_COMPONENT_UCI_COMPUTER = 12345681
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::UCI_Computer, cWSF_COMPONENT_UCI_COMPUTER)

#endif
