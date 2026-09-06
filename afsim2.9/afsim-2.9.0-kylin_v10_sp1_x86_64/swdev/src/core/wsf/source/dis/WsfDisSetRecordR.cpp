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

#include "dis/WsfDisSetRecordR.hpp"

#include "Dis.hpp"
#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "UtLog.hpp"
#include "WsfDisObserver.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisRecordR.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisSetRecordR::WsfDisSetRecordR(WsfDisInterface* aInterfacePtr)
   : DisSetRecordR()
   , mInterfacePtr(aInterfacePtr)
{
   SetReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisSetRecordR::WsfDisSetRecordR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisSetRecordR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisSetRecordR::~WsfDisSetRecordR() {}

// virtual
void WsfDisSetRecordR::TimedOut()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisSetRecordR: Unhandled virtual TimedOut() call.";
   }
}

WsfDisRecordR* WsfDisSetRecordR::ProduceReply()
{
   WsfDisRecordR* replyPtr = new WsfDisRecordR(mInterfacePtr);

   replyPtr->SetRequestId(GetRequestId());
   replyPtr->SetReceivingEntity(GetOriginatingEntity());

   // Per the DIS Standard, the response shall NOT require acknowledgment.
   replyPtr->SetReliabilityService(DisEnum::Pdu::Reliability::Unacknowledged);

   PopulateReply(replyPtr);

   return replyPtr;
}

bool WsfDisSetRecordR::PopulateReply(WsfDisRecordR* aReplyPtr)
{
   bool processed = false;
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      // For now, our only response is to indicate we cannot respond.
      ut::log::warning() << "WsfDisSetRecordR has not yet implemented PopulateReply() function.";
      processed = true;
   }
   return processed;
}

// virtual
int WsfDisSetRecordR::Process()
{
   const int DELETE_PDU = 1;

   // bool actionNeeded = false;
   bool satisfied = false;
   bool repliedTo = false;

   WsfObserver::DisSetRecordRReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);

   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      double simTime2 = WsfDisUtil::GetSimTime(mInterfacePtr);
      double pduTime  = WsfDisUtil::GetTimeFromPdu(mInterfacePtr, *this);

      auto out = ut::log::debug() << "WsfDisSetRecordR: PDU received and processed.";
      out.AddNote() << "Sim T = " << simTime2;
      out.AddNote() << "PDU T = " << pduTime;

      if (satisfied)
      {
         out.AddNote() << "Satisfied.";
      }
      if (repliedTo)
      {
         out.AddNote() << "Reply sent.";
      }
      if (satisfied && repliedTo)
      {
         out.AddNote() << "Complete Transaction.";
      }
   }

   return DELETE_PDU;
}

bool WsfDisSetRecordR::ProcessSetRecordRequest()
{
   bool success = false;
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      // Our only action currently is to indicate we can't do this yet.
      ut::log::warning() << "WsfDisSetRecordR does not yet process the data in any way.";
      success = true;
   }
   return success;
}
