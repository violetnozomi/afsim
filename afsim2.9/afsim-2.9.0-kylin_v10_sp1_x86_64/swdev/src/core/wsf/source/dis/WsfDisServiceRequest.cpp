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

#include "dis/WsfDisServiceRequest.hpp"

#include <iostream>

#include "DisEntityType.hpp"
#include "DisTransactionEnums.hpp"
#include "WsfExchange.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "dis/WsfDisExchange.hpp"
#include "dis/WsfDisExt.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"
#include "ext/WsfExtInterface.hpp"

// See header comment, this PDU applies equally to a request for goods or services!

WsfDisServiceRequest::WsfDisServiceRequest(WsfDisInterface* aInterfacePtr)
   : DisServiceRequest()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisServiceRequest::WsfDisServiceRequest(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisServiceRequest(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisServiceRequest::~WsfDisServiceRequest() {}

// virtual
int WsfDisServiceRequest::Process()
{
   // Incoming Commodity or Service Request PDU, sent from Originator/Requestor.
   // Callback to GetSimulation()->GetObserver().ExchangeQueried to inform the Simulation.

   bool            senderIsRequestor;
   bool            senderIsResponder;
   bool            requesterIsLocal;
   bool            responderIsLocal;
   WsfDisPlatform* requesterPtr;
   WsfDisPlatform* responderPtr;
   size_t          requestingIndex;
   size_t          respondingIndex;

   bool success = WsfDisUtil::DisToWsfPlayers(mInterfacePtr,
                                              GetOriginatingEntity(),
                                              GetRequestingEntityId(),
                                              GetServicingEntityId(),
                                              senderIsRequestor,
                                              senderIsResponder,
                                              requesterIsLocal,
                                              responderIsLocal,
                                              requesterPtr,
                                              responderPtr,
                                              requestingIndex,
                                              respondingIndex);

   if (!success)
   {
      if (WsfDisExchange::DebugEnabled())
      {
         ut::log::debug() << "WsfDisServiceRequest received, but not all players could be resolved. Ignored.";
      }
      return 1;
   }

   if (requesterIsLocal || !responderIsLocal)
   {
      // The WsfDisServiceRequest was received, but it was originally sent by one of our own
      // local platforms, which must have previously sent it out.  Do not issue an
      // ExchangeQueried callback, because this would cause a ping-pong race condition.
      // (This would be considered an abnormal occurrence.)
      if (WsfDisExchange::DebugEnabled())
      {
         ut::log::debug()
            << "WsfDisServiceRequest received from an internal player, or responder not external. Ignored.";
      }
      return 1;
   }

   // Okay, now we know the requestor is external, so we need to generate a callback to
   // ExchangeQueried so our local players (whether specifically addressed, or as yet unknown,
   // the target of a broadcast plea) can potentially respond with an "Offer".
   double simTime    = WsfDisUtil::GetSimTime(mInterfacePtr);
   bool   cIsService = (GetServiceType() == DisEnum::Logistics::Service::REPAIR) ? true : false;
   // bool cIsOfferor = false; // the caller/originator is a requestor, not a supplier.

   if (requesterPtr->GetQueryPtr() == nullptr)
   {
      // First time this remote player has ever requested goods or services.
      // Create a local bookkeeping struct, hang it on the DIS platform,
      // and link the requested Transactor with the requesting Transactor.
      requesterPtr->SetQueryPtr(ut::make_unique<WsfExchange::Query>(&WsfDisUtil::GetSimulation(mInterfacePtr)));
   }
   auto requestorQueryPtr = requesterPtr->GetQueryPtr();

   // This implementation presently cannot handle more than one good or service
   // exchanged at a time.  The first exchangeable item found is used,
   // any others following are ignored.  (This limitation may need lifted later.)
   std::vector<DisSupplyRecord*>::iterator supplyRecIter(GetSupplyRecordBegin());

   bool callbackCompleted = false;

   for (unsigned int i = 0; i != GetSupplyRecordCount(); ++i)
   {
      const DisSupplyRecord* recPtr = supplyRecIter[i];

      // Map from the incoming DisEntityType, to our StringId.
      // If we cannot recognize that commodity, then ignore it.
      WsfStringId commodityId =
         WsfDisUtil::GetExtInterface(mInterfacePtr)->GetStringId(WsfDisExt::ToExt(recPtr->GetSupplyTypeValue()));
      if (!commodityId)
      {
         // Do not know this commodity or service.  Ignore it, but try looking at the next one!  (no "break")
         if (WsfDisExchange::DebugEnabled())
         {
            auto out = ut::log::debug() << "WsfDisServiceRequest received with unknown ID Name. Ignored.";
            out.AddNote() << "T = " << simTime;
         }
      }
      else
      {
         // As noted above, the NameId and Quantity of the service must be updated:
         double quantity = static_cast<double>(recPtr->GetQuantity());

         if (quantity > 0.0)
         {
            if (WsfDisExchange::DebugEnabled())
            {
               auto out = ut::log::debug() << "WsfDisServiceRequest received.";
               out.AddNote() << "T = " << simTime;
               out.AddNote() << "ExchangeQueried callback generated.";
            }

            // Callback to inform the rest of the simulation:
            // (DIS Interface does not support a transaction rate, so we supply zero.)
            unsigned int rid       = WsfExchange::Transactor::NextSerialRequestId();
            bool         isOfferor = false;
            bool         ok        = requestorQueryPtr->FormRequest(requestingIndex,
                                                     requestingIndex,
                                                     respondingIndex,
                                                     quantity,
                                                     0.0,
                                                     rid,
                                                     commodityId,
                                                     isOfferor,
                                                     cIsService);

            if (ok)
            {
               WsfObserver::ExchangeQueried (&WsfDisUtil::GetSimulation(mInterfacePtr))(simTime, *requestorQueryPtr);
               callbackCompleted = true;
               // Only one good or service allowed per.
               break;
            }
         }
         else if (WsfDisExchange::DebugEnabled())
         {
            auto out = ut::log::debug() << "WsfDisServiceRequest received containing zero quantity. Ignored.";
            out.AddNote() << "T = " << simTime;
         }
      }
   }

   if (WsfDisExchange::DebugEnabled() && !callbackCompleted)
   {
      ut::log::debug() << "WsfDisServiceRequest received, but a callback could not be assembled. Ignored.";
   }

   return 1; // Non-zero => delete PDU, Zero = Keep
}
