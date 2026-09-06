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

#include "dis/WsfDisRepairComplete.hpp"

#include <iostream>

#include "DisServiceRequest.hpp"
#include "DisTransactionEnums.hpp"
#include "WsfExchange.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisExchange.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisRepairComplete::WsfDisRepairComplete(WsfDisInterface* aInterfacePtr)
   : DisRepairComplete()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisRepairComplete::WsfDisRepairComplete(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisRepairComplete(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisRepairComplete::~WsfDisRepairComplete() {}

// virtual
int WsfDisRepairComplete::Process()
{
   // Incoming Repair Complete PDU.  Callback to Observer to inform the Simulation.
   // This PDU comes from the provider/supplier of service to the serviced entity
   // to let him know he is repaired.

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
         ut::log::debug() << "WsfDisRepairComplete received from an unknown supplier/servicer. Ignored.";
      }
      return 1;
   }

   WsfExchange::Query* responderQueryPtr = responderPtr->GetQueryPtr();
   if (responderQueryPtr == nullptr)
   {
      // What?  We are now receiving a RepairComplete from a player
      // we have not previously negotiated an exchange with?
      // I think not. Ignore.
      if (WsfDisExchange::DebugEnabled())
      {
         ut::log::debug() << "WsfDisRepairComplete received, but no previous negotiation took place. Ignored.";
      }
      return 1;
   }

   double simTime = WsfDisUtil::GetSimTime(mInterfacePtr);

   if (WsfDisExchange::DebugEnabled())
   {
      std::string requestorName = requesterPtr->GetPlatform()->GetName();
      std::string servicerName  = responderPtr->GetPlatform()->GetName();
      auto        out           = ut::log::debug() << "DisRepairComplete received from requester.";
      out.AddNote() << "T = " << simTime;
      out.AddNote() << "Requester: " << requestorName;
      out.AddNote() << "Servicer: " << servicerName;
      out.AddNote() << "ExchangeQueried callback being generated.";
   }

   // Callback to inform the rest of the simulation:
   responderQueryPtr->SetEvent(WsfExchange::sEVENT_OFFEROR_COMPLETED);
   WsfObserver::ExchangeQueried (&WsfDisUtil::GetSimulation(mInterfacePtr))(simTime, *responderQueryPtr);

   return 1; // Non-zero => delete PDU, Zero = Keep
}
