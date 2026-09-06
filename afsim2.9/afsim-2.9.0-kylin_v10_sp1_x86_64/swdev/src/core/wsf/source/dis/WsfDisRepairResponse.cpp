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

#include "dis/WsfDisRepairResponse.hpp"

#include "DisServiceRequest.hpp"
#include "DisSupplyRecord.hpp"
#include "DisTransactionEnums.hpp"
#include "UtLog.hpp"
#include "WsfExchange.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisExchange.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisRepairResponse::WsfDisRepairResponse(WsfDisInterface* aInterfacePtr)
   : DisRepairResponse()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisRepairResponse::WsfDisRepairResponse(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisRepairResponse(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisRepairResponse::~WsfDisRepairResponse() {}

// virtual
int WsfDisRepairResponse::Process()
{
   // Incoming Repair Response PDU.  Callback to Observer to inform the Simulation.
   // This PDU comes from the requestor of service to the servicing entity
   // to let him know he is fully repaired, and acknowledging the repair.
   // (This is probably a no-op on the part of the receiver, but we give him
   //  the option of doign something in response...
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
         ut::log::debug() << "WsfDisRepairResponse received from an unknown requester. Ignored.";
      }
      return 1;
   }

   double simTime = WsfDisUtil::GetSimTime(mInterfacePtr);
   if (WsfDisExchange::DebugEnabled())
   {
      std::string requestorName = requesterPtr->GetPlatform()->GetName();
      std::string servicerName  = responderPtr->GetPlatform()->GetName();

      auto out = ut::log::debug() << "DisRepairResponse received from requester, acknowledging repairs received.";
      out.AddNote() << "T = " << simTime;
      out.AddNote() << "Requester: " << requestorName;
      out.AddNote() << "Servicer: " << servicerName;
   }

   WsfExchange::Query* requesterQueryPtr = requesterPtr->GetQueryPtr();
   if (requesterQueryPtr != nullptr)
   {
      // See DisTransactionEnums.hpp
      // Note:  DisEnum::Logistics::Repair::Result = { OTHER, ENDED, INVALID, INTERRUPTED, CANCELED };
      DisEnum8    disResult = GetServiceResult();
      WsfStringId wsfResult = WsfExchange::sEVENT_CANCELLED;
      if (disResult == DisEnum::Logistics::Repair::Result::ENDED)
      {
         wsfResult = WsfExchange::sEVENT_REQUESTOR_RESPONDED; // We assume this is success.
      }

      if (WsfDisExchange::DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfDisRepairResponse received.";
         out.AddNote() << "ExchangeQueried sEVENT_REQUESTOR_RESPONDED callback generated.";
      }

      WsfObserver::ExchangeQueried (&WsfDisUtil::GetSimulation(mInterfacePtr))(simTime, *requesterQueryPtr);
   }
   return 1; // Non-zero => delete PDU, Zero = Keep
}
