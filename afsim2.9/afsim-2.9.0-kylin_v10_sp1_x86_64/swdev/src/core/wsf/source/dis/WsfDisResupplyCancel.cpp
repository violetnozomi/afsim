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

#include "dis/WsfDisResupplyCancel.hpp"

#include <iostream>

#include "DisSupplyRecord.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "WsfExchange.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisExchange.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisResupplyCancel::WsfDisResupplyCancel(WsfDisInterface* aInterfacePtr)
   : DisResupplyCancel()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisResupplyCancel::WsfDisResupplyCancel(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisResupplyCancel(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisResupplyCancel::~WsfDisResupplyCancel() {}

// virtual
int WsfDisResupplyCancel::Process()
{
   // Incoming Resupply Cancel PDU.  Callback to inform the Platform involved.
   // The Resupply Cancel may come from either the Requestor or the Supplier side,
   // but here the only Cancel we care about is coming from an external player we
   // we have already negotiated with, and who is now canceling an exchange we do
   // not yet know is canceled.  (If otherwise, ignore.)

   WsfDisPlatform* originatorPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetOriginatingEntity());
   if (originatorPtr == nullptr)
   {
      return 1;
   }

   WsfExchange::Query* origQueryPtr = originatorPtr->GetQueryPtr();
   if (origQueryPtr == nullptr)
   {
      return 1;
   }

   if (origQueryPtr->IsDone())
   {
      return 1;
   }

   origQueryPtr->EventId() = WsfExchange::sEVENT_CANCELLED;
   origQueryPtr->SetResult(WsfExchange::Result::cCANCELLED_REJECTED);

   double simTime = WsfDisUtil::GetSimulation(mInterfacePtr).GetSimTime();
   WsfObserver::ExchangeQueried (&WsfDisUtil::GetSimulation(mInterfacePtr))(simTime, *origQueryPtr);

   // Now we are completely done.  Reset the transaction.
   origQueryPtr->FinalReset();

   return 1; // Non-zero => delete PDU, Zero = Keep
}
