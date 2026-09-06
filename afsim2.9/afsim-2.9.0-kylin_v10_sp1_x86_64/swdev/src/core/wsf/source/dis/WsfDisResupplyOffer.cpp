// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "dis/WsfDisResupplyOffer.hpp"

#include <string>

#include "DisTransactionEnums.hpp"
#include "UtLog.hpp"
#include "WsfExchange.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "dis/WsfDisExchange.hpp"
#include "dis/WsfDisExt.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisResupplyOffer::WsfDisResupplyOffer(WsfDisInterface* aInterfacePtr)
   : DisResupplyOffer()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisResupplyOffer::WsfDisResupplyOffer(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisResupplyOffer(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisResupplyOffer::~WsfDisResupplyOffer() {}

// virtual
int WsfDisResupplyOffer::Process()
{
   // Apparently we have made a request to an external Entity to supply one of our platforms
   // with some product or service.  He is responding back to begin a negotiation for resupply.
   bool            senderIsRequester;
   bool            senderIsResponder;
   bool            requesterIsLocal;
   bool            responderIsLocal;
   WsfDisPlatform* requesterPtr;
   WsfDisPlatform* responderPtr;
   size_t          requestingIndex;
   size_t          respondingIndex;

   bool ok = WsfDisUtil::DisToWsfPlayers(mInterfacePtr,
                                         GetOriginatingEntity(),
                                         GetRequestingEntityId(),
                                         GetServicingEntityId(),
                                         senderIsRequester,
                                         senderIsResponder,
                                         requesterIsLocal,
                                         responderIsLocal,
                                         requesterPtr,
                                         responderPtr,
                                         requestingIndex,
                                         respondingIndex);

   if (!ok)
   {
      if (WsfDisExchange::DebugEnabled())
      {
         ut::log::debug() << "WsfDisResupplyOffer received, but could not resolve players. Ignored.";
      }
      return 1;
   }

   if ((!responderPtr->GetPlatform()->IsExternallyControlled()) || (requesterPtr->GetPlatform()->IsExternallyControlled()))
   {
      if (WsfDisExchange::DebugEnabled())
      {
         ut::log::debug()
            << "WsfDisResupplyOffer received, but was not a Local Receiver with Remote Supplier. Ignored.";
      }
      return 1;
   }

   double simTime = WsfDisUtil::GetSimTime(mInterfacePtr);

   if (WsfDisExchange::DebugEnabled())
   {
      std::string requesterName = requesterPtr->GetPlatform()->GetName();
      std::string servicerName  = responderPtr->GetPlatform()->GetName();
      auto        out           = ut::log::debug() << "WsfDisResupplyOffer received from supplier.";
      out.AddNote() << "T = " << simTime;
      out.AddNote() << "Requester: " << requesterName;
      out.AddNote() << "Supplier: " << servicerName;
   }

   // Incoming Supply Offer PDU, supposedly in response to our outgoing Request.
   // WHEN WE FIRST SENT THE REQUEST, WE SHOULD HAVE ATTACHED A QUERY TO THE EXPECTED RESPONDER.
   // Generate a callback to Observer to inform the Simulation.
   WsfExchange::Query* responderQueryPtr = responderPtr->GetQueryPtr();
   if (responderQueryPtr == nullptr)
   {
      if (WsfDisExchange::DebugEnabled())
      {
         ut::log::debug() << "WsfDisResupplyOffer received, but expected the Remote Supplier to have a Query. Ignored.";
      }
      return 1;
   }

   bool callbackAccomplished = false;

   std::vector<DisSupplyRecord*>::iterator supplyRecIter(GetSupplyRecordBegin());
   for (unsigned int i = 0; i != GetSupplyRecordCount(); ++i)
   {
      const DisSupplyRecord* recPtr = supplyRecIter[i];

      // Map from the incoming DisEntityType, to our StringId.
      // If we cannot recognize that commodity, then ignore it.
      WsfStringId commodityId =
         WsfDisUtil::GetExtInterface(mInterfacePtr)->GetStringId(WsfDisExt::ToExt(recPtr->GetSupplyTypeValue()));
      double quantity = static_cast<double>(recPtr->GetQuantity());

      // If we do not know what the goods are, ignore, and go on to the next one.
      if (commodityId != 0)
      {
         // We should have pre-populated almost everything into this query earlier
         // when our original request was made.  Just supply the missing info:
         // How much are you willing to give me, and what is this event?
         responderQueryPtr->SetEvent(WsfExchange::sEVENT_OFFERED);
         responderQueryPtr->SetOfferedQuantity(quantity);
         responderQueryPtr->SetNegotiatedQuantity(quantity);
         assert(commodityId == responderQueryPtr->ItemId());

         if (WsfDisExchange::DebugEnabled())
         {
            auto out = ut::log::debug() << "WsfDisResupplyOffer received.";
            out.AddNote() << "T = " << simTime;
            responderQueryPtr->DebugDump(out);
            out.AddNote() << "ExchangeQueried callback generated.";
         }

         // Generate callback to the rest of the simulation:
         WsfObserver::ExchangeQueried (&WsfDisUtil::GetSimulation(mInterfacePtr))(simTime, *responderQueryPtr);

         callbackAccomplished = true;

         // We only allow one item per exchange, currently.
         break;
      }
   }

   if (!callbackAccomplished && WsfDisExchange::DebugEnabled())
   {
      ut::log::debug() << "WsfDisResupplyOffer received, but Commodity ID was not found. Ignored.";
   }

   return 1; // Non-zero => delete PDU, Zero = Keep
}
