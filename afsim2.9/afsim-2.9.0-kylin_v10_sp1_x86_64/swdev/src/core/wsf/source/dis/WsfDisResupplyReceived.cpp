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

#include "dis/WsfDisResupplyReceived.hpp"

#include "DisServiceRequest.hpp"
#include "UtLog.hpp"
#include "WsfExchange.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisExchange.hpp"
#include "dis/WsfDisExt.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisResupplyReceived::WsfDisResupplyReceived(WsfDisInterface* aInterfacePtr)
   : DisResupplyReceived()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisResupplyReceived::WsfDisResupplyReceived(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisResupplyReceived(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
   // SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
}

// virtual
WsfDisResupplyReceived::~WsfDisResupplyReceived() {}

// virtual
int WsfDisResupplyReceived::Process()
{
   // Incoming PDU notification of Commodity Received PDU, a confirmation of receipt from the
   // requester back to the supplier.  In order for this to have happened, the requester will
   // have already sent us a ResupplyRequest, we will have responded with a ResupplyOffer, and
   // this current message is the result.  So the Query should already be set.  Set up a call-
   // back to observers so that the supplier Platform can know.  (Here, the Query will be
   // attached to the external Receiver/Requester platform, querying back to the local Supplier
   // /Responder.)

   WsfDisPlatform* requestorPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetRequestingEntityId());
   if (requestorPtr == nullptr)
   {
      if (WsfDisExchange::DebugEnabled())
      {
         ut::log::debug() << "WsfDisResupplyReceived received from an unknown requester. Ignored.";
      }
      return 1;
   }

   WsfDisPlatform* respondingPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetServicingEntityId());
   if (respondingPtr == nullptr)
   {
      if (WsfDisExchange::DebugEnabled())
      {
         ut::log::debug() << "WsfDisResupplyReceived did not know the responder/supplier. Ignored.";
      }
      return 1;
   }

   if (respondingPtr->GetPlatform()->IsExternallyControlled() || !requestorPtr->GetPlatform()->IsExternallyControlled())
   {
      if (WsfDisExchange::DebugEnabled())
      {
         ut::log::debug()
            << "WsfDisResupplyReceived did not expect requester to be external, or responder to be local. Ignored.";
      }
      return 1;
   }

   double simTime = WsfDisUtil::GetSimTime(mInterfacePtr);

   if (WsfDisExchange::DebugEnabled())
   {
      std::string requestorName = requestorPtr->GetPlatform()->GetName();
      std::string servicerName  = respondingPtr->GetPlatform()->GetName();

      auto out = ut::log::debug() << "DisResupplyReceived incoming from requester to (hopefully) servicer.";
      out.AddNote() << "T = " << simTime;
      out.AddNote() << "Requester: " << requestorName;
      out.AddNote() << "Servicer: " << servicerName;
   }

   WsfExchange::Query* requestorQueryPtr = requestorPtr->GetQueryPtr();
   if (requestorQueryPtr == nullptr)
   {
      // The transactor should have already been attached to the requestor.  Fail.
      if (WsfDisExchange::DebugEnabled())
      {
         ut::log::debug() << "WsfDisResupplyReceived expected a valid Query to be allocated. Ignored.";
      }
      return 1;
   }

   std::vector<DisSupplyRecord*>::iterator supplyRecIter(GetSupplyRecordBegin());

   for (unsigned int i = 0; i != GetSupplyRecordCount(); ++i)
   {
      const DisSupplyRecord* recPtr = supplyRecIter[i];

      // Map from the incoming DisEntityType, to our StringId.
      // If we cannot recognize that commodity, then ignore it.
      WsfStringId itemId =
         WsfDisUtil::GetExtInterface(mInterfacePtr)->GetStringId(WsfDisExt::ToExt(recPtr->GetSupplyTypeValue()));
      if (!itemId.IsNull())
      {
         double quantity = static_cast<double>(recPtr->GetQuantity());
         requestorQueryPtr->ConstrainTo(quantity);
         requestorQueryPtr->SetEvent(WsfExchange::sEVENT_RECEIVED);

         if (WsfDisExchange::DebugEnabled())
         {
            auto out = ut::log::debug() << "WsfDisResupplyReceived received.";
            out.AddNote() << "ExchangeQueried callback generated.";
         }

         // Callback to inform the rest of the simulation that the external player accepted:
         WsfObserver::ExchangeQueried (&WsfDisUtil::GetSimulation(mInterfacePtr))(simTime, *requestorQueryPtr);

         // Only allow one trans per exchange.
         break;
      }
   }

   return 1; // Non-zero => delete PDU, Zero = Keep
}
