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

#ifndef WSFUCI_GENERICLISTENER_HPP
#define WSFUCI_GENERICLISTENER_HPP

#include "wsf_oms_uci_export.h"

#include <iostream>
#include <stdexcept>

#include "UtLog.hpp"
#include "uci/base/UCIException.h"
#include "uci/type/MessageType.h"

namespace wsf
{
   class UCI_Interface;

   //! Provide a method to call the UCI_Interface::ReceiveMessage method, to be defined in the implementation file.
   //! @note Because This file is included from WsfUCI_Interface.hpp, we can only forward declare UCI_Interface;
   //! UCI_Interface methods cannot be called from the inline template code for this reason.
   void UCI_GenericListener_ReceiveMessage(UCI_Interface&                aInterface,
                                           const uci::type::MessageType* aMessagePtr);

   //! Define a generic interface that passes received messages to the interface to dispatch to UCI components.
   template< class MessageType,
             class ListenerBaseType >
   class UCI_GenericListener : public ListenerBaseType
   {
      public:
         explicit UCI_GenericListener(UCI_Interface& aInterface)
            : ListenerBaseType(),
              mInterface(aInterface)
         {
         }
         virtual ~UCI_GenericListener() = default;
         UCI_GenericListener(const UCI_GenericListener& aSrc) = delete;
         const UCI_GenericListener& operator=(const UCI_GenericListener& aRhs) = delete;

         void handleMessage(const MessageType& aMessage)
            throw(uci::base::UCIException) override;

      private:

         UCI_Interface&    mInterface;
   };

   template<class MessageType, class ListenerBaseType>
   inline void UCI_GenericListener<MessageType, ListenerBaseType>::handleMessage(const MessageType& aMessage)
      throw(uci::base::UCIException)
   {
      try
      {
         UCI_GenericListener_ReceiveMessage(mInterface, &aMessage);
      }
      catch (const std::runtime_error& aException)
      {
         ut::log::error() << aException.what();
      }
   }
}
#endif
