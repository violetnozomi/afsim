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

#include "dis/WsfDisExchange.hpp"

#include "Dis.hpp"
#include "DisRepairComplete.hpp"
#include "DisRepairResponse.hpp"
#include "DisResupplyCancel.hpp"
#include "DisResupplyOffer.hpp"
#include "DisResupplyReceived.hpp"
#include "DisServiceRequest.hpp"
#include "DisTransactionEnums.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisExt.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"
#include "ext/WsfExtInterface.hpp"

// ============================================================================
// static
bool WsfDisExchange::sDebug = false;

// ============================================================================
WsfDisExchange::WsfDisExchange()
   : mDisInterfacePtr(nullptr)
   , mCallbacks()
{
}

// ============================================================================
WsfDisExchange::~WsfDisExchange()
{
   mCallbacks.Clear();
}

// ============================================================================
bool WsfDisExchange::Initialize(WsfDisInterface* aDisInterfacePtr)
{
   mDisInterfacePtr = aDisInterfacePtr;
   mCallbacks.Add(
      WsfObserver::ExchangeQueried(&mDisInterfacePtr->GetSimulation()).Connect(&WsfDisExchange::ExchangeQueried, this));
   return true;
}

// ============================================================================
bool WsfDisExchange::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command   = aInput.GetCommand();
   if (command == "dis_exchange")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         command = aInput.GetCommand();
         if (command == "debug")
         {
            sDebug = true;
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      myCommand = true;
   }
   return myCommand;
}

// ============================================================================
void WsfDisExchange::FillInPduHeader(double aSimTime, DisPdu* aPduPtr)
{
   aPduPtr->SetExerciseIdentifier(Dis::GetExercise());
   aPduPtr->SetProtocolVersion(Dis::GetProtocolVersion());
   aPduPtr->SetTime(aSimTime);
}

// ============================================================================
void WsfDisExchange::ExchangeQueried(double aSimTime, const WsfExchange::Query& aQuery)
{
   // Cannot send anything out if we do not have a connection to do so.
   if (!mDisInterfacePtr->HasDevice())
   {
      return;
   }

   // Do we need to send out an external notification of this event?
   // The purpose of this method is to determine whether a DIS PDU needs
   // to go out to initiate an exchange Transaction with an external player.
   // If we already know that both parties are internal, then do not
   // broadcast any further PDUs to an agnostic, dis-interested DIS world.

   bool originatorIsRequester;
   bool originatorIsResponder;
   bool requesterIsLocal;
   bool responderIsLocal;

   WsfDisPlatform* requesterPtr = nullptr;
   WsfDisPlatform* responderPtr = nullptr;

   DisEntityId originatorId;
   DisEntityId requesterId;
   DisEntityId responderId;

   // This is a bit tricky.  We can check for who the requestor and responders are,
   // but since this is a two-way Transactor, each player may be the originator or NOT.
   size_t owningIndex     = aQuery.OwningIndex();
   size_t requestingIndex = aQuery.RequestingIndex();
   size_t respondingIndex = aQuery.RespondingIndex();

   // assert(owningIndex != 0);
   // assert(requestingIndex != 0);
   // assert(respondingIndex != 0);

   bool ok = WsfDisUtil::WsfToDisPlayers(mDisInterfacePtr,
                                         owningIndex,
                                         requestingIndex,
                                         respondingIndex,
                                         originatorIsRequester,
                                         originatorIsResponder,
                                         requesterIsLocal,
                                         responderIsLocal,
                                         requesterPtr,
                                         responderPtr,
                                         originatorId,
                                         requesterId,
                                         responderId);

   if (!ok)
   {
      // Both players are local, or both players are remote.  No reason for our DIS to get involved.
      if (sDebug)
      {
         ut::log::debug() << "WsfDisExchange: ExchangeQueried(): Did nothing, as players could not be resolved.";
      }
      return;
   }

   if (requesterIsLocal == responderIsLocal)
   {
      // Both players are local, or both players are remote.  No reason for our DIS to get involved.
      if (sDebug)
      {
         ut::log::debug()
            << "WsfDisExchange: ExchangeQueried(): Did nothing, as both requester and responder are local or remote.";
      }
      return;
   }

   // The only reason we will need to send out a PDU is if the originator is local,
   // and the intended recipient is not local (external):

   bool originatorIsLocal = (originatorIsRequester && requesterIsLocal) || (originatorIsResponder && responderIsLocal);
   bool recipientIsRemote = (originatorIsRequester && !responderIsLocal) || (originatorIsResponder && !requesterIsLocal);
   if (!(originatorIsLocal && recipientIsRemote))
   {
      // Both players are local, or both players are remote.  No reason for our DIS to get involved.
      if (sDebug)
      {
         ut::log::debug() << "WsfDisExchange: ExchangeQueried(): Did nothing, as the intended recipient was local.";
      }
      return;
   }

   // Map to the outgoing DisEntityType (if any).
   WsfExtEntityType commodityOrServiceType;
   if (!mDisInterfacePtr->GetExtInterface()->GetEntityType(aQuery.ItemId(), commodityOrServiceType))
   {
      // A repair complete notification does not have to supply the repair type.
      if (aQuery.EventId() != WsfExchange::sEVENT_OFFEROR_COMPLETED)
      {
         // If we have not been informed of a commodity ID to coordinate upon
         // externally, there is no common medium of exchange.  Give up.
         if (sDebug)
         {
            auto out = ut::log::warning()
                       << "WsfDisExchange: ExchangeQueried(): DIS Interface entity_type mapping not found.";
            out.AddNote() << "Type: " << aQuery.ItemId().GetString();
            out.AddNote() << "Type will be ignored.";
         }
         return;
      }
      assert(aQuery.EventId() == WsfExchange::sEVENT_OFFEROR_COMPLETED);
   }

   // =============== PRELIMINARIES RESOLVED.  Now respond uniquely to each event type... ===============

   if (aQuery.EventId() == WsfExchange::sEVENT_REQUESTED) //================================
   {
      // One of our platforms is asking for some commodity or service.
      // If there is no pre-existing Query attached to the remote player,
      // attach one now:
      if (responderPtr->GetQueryPtr() == nullptr)
      {
         responderPtr->SetQueryPtr(ut::make_unique<WsfExchange::Query>(&WsfDisUtil::GetSimulation(mDisInterfacePtr)));
      }
      auto respondingQueryPtr = responderPtr->GetQueryPtr();

      // Very soon now, we hope (expect) to get a callback from this player, RESPONDING
      // to our own outbound request.  Since we make the original request, we can pre-
      // fill in most of the details for him.  The one thing we MUST fill in is the
      // RequestId, which he will not have from the DIS interface...
      // We do supply Zeros for Offered Quantity and Offered Rate, as he must fill those in later.
      // (Actually, over DIS, the Rate will always be zero.)
      unsigned int requestId    = aQuery.RequestId();
      size_t       respondIndex = aQuery.RespondingIndex();
      bool         isOfferor    = true;

      // WsfExchange::Result::Value hisValue = aQuery.GetResultValue();
      // WsfExchange::Result::Value myValue  = respondingQueryPtr->GetResultValue();
      respondingQueryPtr->FormOffer(respondIndex,
                                    aQuery.RequestingIndex(),
                                    respondIndex,
                                    aQuery.DesiredQuantity(),
                                    aQuery.DesiredRate(),
                                    0.0,
                                    0.0,
                                    requestId,
                                    aQuery.ItemId(),
                                    isOfferor,
                                    aQuery.IsService());
      respondingQueryPtr->SetResult(WsfExchange::Result::cPENDING);

      auto pduPtr = ut::make_unique<DisServiceRequest>();
      FillInPduHeader(aSimTime, pduPtr.get());
      pduPtr->SetRequestingEntityId(requesterId);
      pduPtr->SetServicingEntityId(responderId);

      if (aQuery.IsService())
      {
         pduPtr->SetServiceType(DisEnum::Logistics::Service::REPAIR);
      }
      else
      {
         pduPtr->SetServiceType(DisEnum::Logistics::Service::RESUPPLY);
      }

      // Place a record in the PDU, indicating what we are asking for,
      // and how much.  We do not indicate a rate.
      DisSupplyRecord* recordPtr = new DisSupplyRecord();
      recordPtr->SetSupplyTypeValue(WsfDisExt::ToDis(commodityOrServiceType));
      DisFloat32 amount = static_cast<DisFloat32>(aQuery.DesiredQuantity());
      recordPtr->SetQuantity(amount);
      pduPtr->AddSupplyRecord(recordPtr);

      if (sDebug)
      {
         auto out = ut::log::debug()
                    << "WsfDisExchange: ExchangeQueried(): sEVENT_REQUESTED externally sending a DisServiceRequest.";

         if (aQuery.IsService())
         {
            out.AddNote() << "Requesting " << amount << " '" << aQuery.ItemId() << "' Services.";
         }
         else
         {
            out.AddNote() << "Requesting " << amount << " '" << aQuery.ItemId() << "' Goods.";
         }
      }

      mDisInterfacePtr->PutPdu(aSimTime, std::move(pduPtr));
   }
   else if (aQuery.EventId() == WsfExchange::sEVENT_OFFERED) // ==================================
   {
      // Assumedly, earlier we received a request for goods/services from a remote player.
      // We have now decided to Offer that external DIS Entity some of what we have.
      // Send out a PDU informing him of that fact.  We _should_ already have a Query from him.
      if ((requesterPtr == nullptr) || (requesterPtr->GetQueryPtr() == nullptr))
      {
         if (sDebug)
         {
            ut::log::debug() << "WsfDisExchange: ExchangeQueried(): sEVENT_OFFERED to unknown player. Ignored.";
         }
         return;
      }

      auto pduPtr = ut::make_unique<DisResupplyOffer>();
      FillInPduHeader(aSimTime, pduPtr.get());
      pduPtr->SetRequestingEntityId(requesterId);
      pduPtr->SetServicingEntityId(responderId); // We are the Originator
      DisSupplyRecord* recordPtr = new DisSupplyRecord();
      bool             isService = aQuery.IsService();
      recordPtr->SetSupplyTypeValue(WsfDisExt::ToDis(commodityOrServiceType));
      DisFloat32 amount = static_cast<DisFloat32>(aQuery.OfferedQuantity());
      recordPtr->SetQuantity(amount);
      pduPtr->AddSupplyRecord(recordPtr);

      if (sDebug)
      {
         std::string theType = "Goods";
         if (isService)
         {
            theType = "Services";
         }
         auto out = ut::log::debug()
                    << "WsfDisExchange: ExchangeQueried(): sEVENT_OFFERED externally sending a DisResupplyOffer.";
         out.AddNote() << "Offering " << amount << " of " << aQuery.ItemId() << " " << theType << ".";
      }

      mDisInterfacePtr->PutPdu(aSimTime, std::move(pduPtr));
   }
   else if (aQuery.EventId() == WsfExchange::sEVENT_ACCEPTED) // =============================
   {
      // One of our local platforms accepted commodities or services from an external platform.
      // Now assure that the respondant is made aware of his acceptance.

      WsfExchange::Query* responderQueryPtr = responderPtr->GetQueryPtr();
      if (responderQueryPtr == nullptr)
      {
         if (sDebug)
         {
            ut::log::debug() << "WsfDisExchange: ExchangeQueried(): sEVENT_ACCEPTED to unknown player. Ignored.";
         }
         return;
      }

      auto pduPtr = ut::make_unique<DisResupplyReceived>();
      FillInPduHeader(aSimTime, pduPtr.get());
      pduPtr->SetRequestingEntityId(requesterId);
      pduPtr->SetServicingEntityId(responderId);
      DisSupplyRecord* recordPtr = new DisSupplyRecord();
      recordPtr->SetSupplyTypeValue(WsfDisExt::ToDis(commodityOrServiceType));
      recordPtr->SetQuantity(static_cast<DisFloat32>(aQuery.DesiredQuantity()));
      pduPtr->AddSupplyRecord(recordPtr);

      if (sDebug)
      {
         auto out =
            ut::log::debug() << "WsfDisExchange: ExchangeQueried(): cEVENT_ACCEPT sending a DisResupplyReceived.";
         out.AddNote() << "Item: " << aQuery.ItemId();
      }

      mDisInterfacePtr->PutPdu(aSimTime, std::move(pduPtr));
   }
   else if (aQuery.EventId() == WsfExchange::sEVENT_SUPPLIED)
   {
      // This event has completed the transaction.  No external response is made.
      if (!sDebug)
      {
         return;
      }

      if ((requesterPtr == nullptr) || (requesterPtr->GetQueryPtr() == nullptr))
      {
         ut::log::warning() << "WsfDisExchange: ExchangeQueried(): sEVENT_SUPPLIED to unknown player.";
      }
      else
      {
         ut::log::debug() << "WsfDisExchange: ExchangeQueried(): sEVENT_SUPPLIED is a no-op.";
      }
   }
   else if (aQuery.EventId() == WsfExchange::sEVENT_OFFEROR_COMPLETED)
   {
      // One of our platforms has completed repairs on an external entity.
      // Send him a PDU to indicate:

      if ((requesterPtr == nullptr) || (requesterPtr->GetQueryPtr() == nullptr))
      {
         if (sDebug)
         {
            ut::log::debug()
               << "WsfDisExchange: ExchangeQueried(): sEVENT_OFFEROR_COMPLETED to unknown player. Ignored.";
         }
         return;
      }

      auto pduPtr = ut::make_unique<DisRepairComplete>();
      FillInPduHeader(aSimTime, pduPtr.get());
      pduPtr->SetRequestingEntityId(requesterId);
      pduPtr->SetServicingEntityId(responderId);
      pduPtr->SetRepair(DisEnum::Logistics::Repair::Completed::ALL); // SUCCESS

      if (sDebug)
      {
         auto out =
            ut::log::debug()
            << "WsfDisExchange: ExchangeQueried(): sEVENT_OFFEROR_COMPLETED externally sent a DisRepairComplete.";
         out.AddNote() << "Item: " << aQuery.ItemId();
      }

      mDisInterfacePtr->PutPdu(aSimTime, std::move(pduPtr));
   }
   else if (aQuery.EventId() == WsfExchange::sEVENT_REQUESTOR_RESPONDED)
   {
      // One of our platforms previously requested repair from an external entity.
      // The negotiation was completed on what needed to be done, and he consented
      // that he could do the required repairs.  After looking at the bill we did
      // agree that the estimate was acceptable, and authorized the work to be done.
      // Now we have become aware from him that the repairs are complete.  Now I
      // respond to him that we acknowledge the repair.  NOW WHAT?  Consult DIS Std.

      WsfExchange::Query* responderQueryPtr = responderPtr->GetQueryPtr();
      if (responderQueryPtr == nullptr)
      {
         if (sDebug)
         {
            ut::log::debug()
               << "WsfDisExchange: ExchangeQueried(): sEVENT_REQUESTOR_RESPONDED to unknown player. Ignored.";
         }
         return;
      }

      auto pduPtr = ut::make_unique<DisRepairResponse>();
      FillInPduHeader(aSimTime, pduPtr.get());
      pduPtr->SetRequestingEntityId(requesterId);
      pduPtr->SetServicingEntityId(responderId);
      pduPtr->SetServiceResult(DisEnum::Logistics::Repair::Result::ENDED); // SUCCESS

      if (sDebug)
      {
         auto out =
            ut::log::debug()
            << "WsfDisExchange: ExchangeQueried(): sEVENT_REQUESTOR_RESPONDED externally sent a DisRepairResponse.";
         out.AddNote() << "Item: " << aQuery.ItemId();
      }

      mDisInterfacePtr->PutPdu(aSimTime, std::move(pduPtr));
   }
   else if (aQuery.EventId() == WsfExchange::sEVENT_CANCELLED)
   {
      // Inform the other guy that we canceled on him.
      WsfExchange::Query* requesterQueryPtr = nullptr;
      if (requesterPtr != nullptr)
      {
         requesterQueryPtr = requesterPtr->GetQueryPtr();
      }

      if (requesterQueryPtr == nullptr)
      {
         if (sDebug)
         {
            ut::log::debug() << "WsfDisExchange: ExchangeQueried(): sEVENT_CANCELLED to unknown player. Ignored.";
         }
         return;
      }

      // One of our platforms is canceling a request for some commodity or service.
      // This notification is at risk for a ping-pong race condition, because he
      // might also cancel in response to our Cancel (?).  Do not send
      // if the receiver's state is known to be COMPLETE, as he does not need
      // to know of the cancellation.  Also, if his RequestID is not zero, send the
      // cancel PDU, but then immediately set his RequestID to zero so we do not
      // do this again
      if (!requesterQueryPtr->IsDone())
      {
         auto pduPtr = ut::make_unique<DisResupplyCancel>();
         FillInPduHeader(aSimTime, pduPtr.get());

         pduPtr->SetRequestingEntityId(requesterId);
         pduPtr->SetServicingEntityId(responderId);

         if (sDebug)
         {
            auto out = ut::log::debug() << "WsfDisExchange: ExchangeQueried(): Sending sEVENT_CANCELLED.";
            out.AddNote() << "Item: " << aQuery.ItemId();
         }

         mDisInterfacePtr->PutPdu(aSimTime, std::move(pduPtr));

         requesterQueryPtr->SetResult(WsfExchange::Result::cCANCELLED_REJECTED);
      }
   }
   else
   {
      if (sDebug)
      {
         ut::log::debug() << "WsfDisExchange: ExchangeQueried(): Unknown event type. Ignored.";
      }
   }
}
