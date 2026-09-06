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

#include "dis/WsfDisRecordQueryR.hpp"

#include "Dis.hpp"
#include "DisDatumEnums.hpp"
#include "DisPduEnums.hpp"
#include "UtLog.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisDataR.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisRecordQueryR::WsfDisRecordQueryR(WsfDisInterface* aInterfacePtr)
   : DisRecordQueryR()
   , mInterfacePtr(aInterfacePtr)
{
   SetReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisRecordQueryR::WsfDisRecordQueryR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisRecordQueryR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisRecordQueryR::~WsfDisRecordQueryR() {}

// virtual
void WsfDisRecordQueryR::TimedOut()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisRecordQueryR: Unhandled virtual TimedOut() call.";
   }
}

WsfDisDataR* WsfDisRecordQueryR::ProduceResponse(bool& aIsDone)
{
   WsfDisDataR* responsePtr = new WsfDisDataR(mInterfacePtr);

   responsePtr->SetRequestId(GetRequestId());
   responsePtr->SetReceivingEntity(GetOriginatingEntity());

   // Per the DIS Standard, the response shall NOT require acknowledgment.
   responsePtr->SetReliabilityService(DisEnum::Pdu::Reliability::Unacknowledged);

   aIsDone = PopulateResponse(responsePtr);

   return responsePtr;
}

// virtual
int WsfDisRecordQueryR::Process()
{
   const int DELETE_PDU = 1;
   return DELETE_PDU;
}

bool WsfDisRecordQueryR::PopulateResponse(WsfDisDataR* aReplyPduPtr)
{
   unsigned int requested = 0;
   unsigned int supplied  = 0;

   using namespace DisEnum::DatumTag;

   for (unsigned int i = 0; i != GetNumRecords(); ++i)
   {
      ++requested;
   }

   if (WsfDisUtil::DebugEnabled(mInterfacePtr) && (supplied != requested))
   {
      ut::log::warning() << "No data records were supplied in response to a RecordQueryR PDU.";
   }

   return supplied == requested;
}
