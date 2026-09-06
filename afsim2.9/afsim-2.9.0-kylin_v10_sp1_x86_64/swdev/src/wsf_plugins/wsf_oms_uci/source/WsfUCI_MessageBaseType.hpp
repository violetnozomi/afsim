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

#ifndef WSFUCI_MESSAGEBASETYPE_HPP
#define WSFUCI_MESSAGEBASETYPE_HPP

#include "wsf_oms_uci_export.h"

#include "uci/type/HeaderType.h"

#include "WsfMessage.hpp"
class     WsfSimulation;

namespace wsf
{

class UCI_Interface;

class WSF_OMS_UCI_EXPORT UCI_MessageBaseType: public WsfMessage                                                 
{
   public:
      UCI_MessageBaseType(WsfSimulation& aSimulation,
                          UCI_Interface& aInterface)
         : WsfMessage(aSimulation),
           mInterfacePtr(&aInterface)          
      {
      }
      UCI_MessageBaseType(const UCI_MessageBaseType& aSrc) = default;
      ~UCI_MessageBaseType() override = default;

      //! Return this message as a uci::type::MessageType (common base class)
      virtual const uci::type::HeaderType* GetHeader() const = 0;

      //! Get the underlying UCI type.
      UCI_Interface& GetInterface() const { return *mInterfacePtr; }

      //! Send this message using the writer.
      virtual void Send(const uci::type::DateTimeTypeValue& aSimTime) const = 0;

   private:

      UCI_Interface*  mInterfacePtr  { nullptr };
};

} // namespace wsf

#endif
