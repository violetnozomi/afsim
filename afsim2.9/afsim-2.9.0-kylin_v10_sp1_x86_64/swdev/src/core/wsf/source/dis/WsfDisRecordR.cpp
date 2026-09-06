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

#include "dis/WsfDisRecordR.hpp"

#include <iostream>

#include "Dis.hpp"
#include "DisSetDataR.hpp"
#include "DisSetRecordR.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisRecordR::WsfDisRecordR(WsfDisInterface* aInterfacePtr)
   : DisRecordR()
   , mInterfacePtr(aInterfacePtr)
{
   SetReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisRecordR::WsfDisRecordR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisRecordR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisRecordR::~WsfDisRecordR() {}

// virtual
int WsfDisRecordR::Process()
{
   const int DELETE_PDU = 1;
   return DELETE_PDU;
}

bool WsfDisRecordR::ProcessResponseTo(DisPdu* aInitiatorPtr)
{
   bool satisfied = false;

   // This PDU can be a response to a SetDataR PDU, or a SetRecordR PDU
   DisSetDataR* setDataPduPtr = dynamic_cast<DisSetDataR*>(aInitiatorPtr);
   if (setDataPduPtr != nullptr)
   {
      satisfied = true;
   }

   if (!satisfied)
   {
      DisSetRecordR* setRecordPduPtr = dynamic_cast<DisSetRecordR*>(aInitiatorPtr);
      if (setRecordPduPtr != nullptr)
      {
         satisfied = true;
      }
   }

   return satisfied;
}
