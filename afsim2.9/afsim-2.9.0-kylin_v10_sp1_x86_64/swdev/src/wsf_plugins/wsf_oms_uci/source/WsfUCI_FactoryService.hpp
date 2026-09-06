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

#ifndef WSFUCI_FACTORYSERVICE_HPP
#define WSFUCI_FACTORYSERVICE_HPP

#include "WsfUCI_ServiceBase.hpp"

#include "uci/base/AbstractServiceBusConnection.h"

namespace wsf
{
   //! Define a generic factory service that can be parameterized with any factory-enabled UCI type.
   template< class MessageType,
             class FactoryType >
   class UCI_FactoryService : public UCI_ServiceBase
   {
      public:
         explicit UCI_FactoryService(uci::base::AbstractServiceBusConnection* aAsbPtr)
            : UCI_ServiceBase(),
              mFactoryPtr(FactoryType::getFactory(aAsbPtr))
         {
         }
         virtual ~UCI_FactoryService()
         {
         }
         UCI_FactoryService() = delete;
         UCI_FactoryService(const UCI_FactoryService& aSrc) = delete;
         const UCI_FactoryService& operator=(const UCI_FactoryService& aRhs) = delete;

         FactoryType& GetFactory() const { return *mFactoryPtr; }
         void Shutdown()
         {
            FactoryType::destroyFactory(mFactoryPtr);
         }

      protected:

         FactoryType* mFactoryPtr = nullptr;
   };

}

#endif
