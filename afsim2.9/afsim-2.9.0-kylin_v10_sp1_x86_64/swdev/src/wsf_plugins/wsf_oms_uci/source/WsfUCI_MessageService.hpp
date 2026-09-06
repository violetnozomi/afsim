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

#ifndef WSFUCI_MESSAGESERVICE_HPP
#define WSFUCI_MESSAGESERVICE_HPP

#include "WsfUCI_ServiceBase.hpp"

#include "uci/base/AbstractServiceBusConnection.h"
#include "UtMemory.hpp"

#include "WsfUCI_FactoryService.hpp"
#include "WsfUCI_GenericListener.hpp"

namespace wsf
{
   class UCI_Interface;

   //! Define a generic service to enable creating, sending, and receiving any UCI message type.
   template< class MessageType,
             class FactoryType,
             class ReaderType,
             class WriterType,
             class ListenerType > // see definition of UCI_GenericListner.
   class UCI_MessageService : public UCI_ServiceBase
   {
      using GenericListenerType = UCI_GenericListener<MessageType, ListenerType>;

      public:
         UCI_MessageService(UCI_Interface&                           aInterface,
                            uci::base::AbstractServiceBusConnection* aAsbPtr, 
                            const char*                              aTopic)
            : UCI_ServiceBase(),
              mFactoryService(aAsbPtr),
              mReaderPtr(&mFactoryService.GetFactory().createReader(aTopic)),
              mWriterPtr(&mFactoryService.GetFactory().createWriter(aTopic)),
              mListenerPtr(ut::make_unique<GenericListenerType>(aInterface))
         {
            mReaderPtr->addListener(*mListenerPtr);
         }
         virtual ~UCI_MessageService() = default;
         UCI_MessageService() = delete;
         UCI_MessageService(const UCI_MessageService& aSrc) = delete;
         const UCI_MessageService& operator=(const UCI_MessageService& aRhs) = delete;

         const UCI_FactoryService<MessageType, FactoryType>& GetTypeFactory() const { return mFactoryService; }
         FactoryType& GetFactory() const { return mFactoryService.GetFactory(); }
         WriterType&  GetWriter()  const { return *mWriterPtr;  }
         ReaderType&  GetReader()  const { return *mReaderPtr;  }

         //! This method should be called sometime before object destruction to make sure that 
         //! any messages still held by the listener are delivered to still-existing objects.
         void Shutdown()
         {
            mReaderPtr->removeListener(*mListenerPtr); // ... but not deleted.
            mReaderPtr->close();
            mFactoryService.GetFactory().destroyReader(*mReaderPtr);
            mWriterPtr->close();
            mFactoryService.GetFactory().destroyWriter(*mWriterPtr);
            mFactoryService.Shutdown();
         }
      private:

         UCI_FactoryService<MessageType, FactoryType>  mFactoryService;
         ReaderType*                                   mReaderPtr   = nullptr;
         WriterType*                                   mWriterPtr   = nullptr;
         std::unique_ptr<GenericListenerType>          mListenerPtr = nullptr;
   };


//#define DEFINE_UCI_MESSAGE_SERVICE(WSF_MESSAGE_SERVICE_TYPE, UCI_MESSAGE_TYPE, WSF_TYPE_NAME, TOPIC_NAME)
//class WSF_MESSAGE_SERVICE_TYPE : public UCI_MessageService<uci::type::UCI_MESSAGE_TYPE,
//                                                uci::factory::UCI_MESSAGE_TYPE##Factory,
//                                                uci::reader::UCI_MESSAGE_TYPE##Reader,
//                                                uci::writer::UCI_MESSAGE_TYPE##Writer,
//                                                uci::reader::UCI_MESSAGE_TYPE##Listener>
//{
//   public:
//      explicit WSF_MESSAGE_SERVICE_TYPE(UCI_Interface&                           aInterface,
//                                        uci::base::AbstractServiceBusConnection* aAsbPtr,
//                                        const char*                              aTopic) : UCI_MessageService(aInterface, aAbsPtr, aTopic)  {}
//      explicit WSF_MESSAGE_SERVICE_TYPE(UCI_Interface& aInterface) : UCI_MessageType(aInterface)                                         { SetType(GetTypeId()); }
//      WSF_MESSAGE_SERVICE_TYPE(UCI_Interface& aInterface, const uci::type::UCI_MESSAGE_TYPE& aType) : UCI_MessageType(aInterface, aType) { SetType(GetTypeId()); }
//      WSF_MESSAGE_SERVICE_TYPE(const WSF_MESSAGE_SERVICE_TYPE& aSrc) = default;
//      ~WSF_MESSAGE_SERVICE_TYPE() override = default;
//      WsfMessage* Clone() const override { return new WSF_MESSAGE_SERVICE_TYPE(*this); }
//      const char* GetScriptClassName() const override { return #WSF_MESSAGE_SERVICE_TYPE; }
//      static WsfStringId GetTypeId() { return #WSF_TYPE_NAME; }
//};

}

#endif
