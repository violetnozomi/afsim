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

#include "dis/WsfDisStopFreezeR.hpp"

#include <iostream>

#include "Dis.hpp"
#include "DisControlEnums.hpp"
#include "DisIO.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisAcknowledgeR.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisStopFreezeR::WsfDisStopFreezeR(WsfDisInterface* aInterfacePtr)
   : DisStopFreezeR()
   , mInterfacePtr(aInterfacePtr)
{
   SetReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisStopFreezeR::WsfDisStopFreezeR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisStopFreezeR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisStopFreezeR::~WsfDisStopFreezeR() = default;

// virtual
DisPdu* WsfDisStopFreezeR::ProduceResponse(bool aIsDone)
{
   WsfDisAcknowledgeR* responsePtr = new WsfDisAcknowledgeR(mInterfacePtr);
   responsePtr->SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
   responsePtr->SetReceivingEntity(GetOriginatingEntity());
   responsePtr->SetAcknowledgeFlag(DisEnum::Control::ResponseToCmdFlag::STOP_FREEZE);
   responsePtr->SetRequestId(GetRequestId());
   DisEnum16 compliance = DisEnum::Control::ResponseCompliance::UNABLE_TO_COMPLY;
   if (aIsDone)
   {
      compliance = DisEnum::Control::ResponseCompliance::ABLE_TO_COMPLY;
   }
   responsePtr->SetResponseFlag(compliance);
   return responsePtr;
}

// virtual
void WsfDisStopFreezeR::TimedOut()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisStopFreezeR: Unhandled virtual TimedOut() call.";
   }
}

bool WsfDisStopFreezeR::ActionIsForMe() const
{
   DisEntityId destId = GetReceivingEntity();
   DisUint16   site   = destId.GetSite();
   DisUint16   appl   = destId.GetApplication();
   DisUint16   entity = destId.GetEntity();

   // See if I should process this PDU.

   // The check for 0xFF is due to a mis-interpretation by Seattle.
   // These are supposed to be 16 bit fields, not 8 bit values.

   if (((site == DisEntityId::NO_SITES) || (site == DisEntityId::ALL_SITES) || (site == 0xFF) || (site == Dis::GetSite())) &&
       ((appl == DisEntityId::NO_APPLIC) || (appl == DisEntityId::ALL_APPLIC) || (appl == 0xFF) ||
        (appl == Dis::GetApplication())))
   {
      // For now we only process requests for non-specific entities

      if ((entity == DisEntityId::NO_ENTITY) || (entity == DisEntityId::ALL_ENTITIES) || (entity == 0xFF))
      {
         return true;
      }
   }
   return false;
}

// virtual
int WsfDisStopFreezeR::Process()
{
   if (ActionIsForMe())
   {
      bool actionNeeded = false;
      bool actedUpon    = false;
      bool repliedTo    = false;

      if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         double simTime2 = WsfDisUtil::GetSimTime(mInterfacePtr);
         double pduTime  = WsfDisUtil::GetTimeFromPdu(mInterfacePtr, *this);

         auto out = ut::log::debug() << "WsfDisStopFreezeR: PDU received and processed.";
         out.AddNote() << "Sim T = " << simTime2;
         out.AddNote() << "PDU T = " << pduTime;
         if (!actionNeeded)
         {
            out.AddNote() << "No further action needed.";
         }
         else
         {
            if (actedUpon)
            {
               out.AddNote() << "Acted upon.";
            }
            if (repliedTo)
            {
               out.AddNote() << "Reply sent.";
            }
            if (actedUpon && repliedTo)
            {
               out.AddNote() << "Complete transaction.";
            }
         }
      }
   }
   return DisPdu::Process();
}

bool WsfDisStopFreezeR::ProcessStopFreezeRequest()
{
   bool actedUpon = false;

   DisEnum8 reason = GetReason();
   if (reason == DisEnum::Control::Reason::RECESS)
   {
      WsfDisUtil::GetSimulation(mInterfacePtr).Pause();
      actedUpon = true;
   }
   else if ((reason == DisEnum::Control::Reason::STOP_FOR_RESET) || (reason == DisEnum::Control::Reason::STOP_FOR_RESTART) ||
            (reason == DisEnum::Control::Reason::ENTITY_RECONSTITUTION))
   {
      // 'ENTITY_RECONSTITUTION' is per Dave Peden (Seattle).
      // They used it for reset prior to the existence of STOP_FOR_RESET and STOP_FOR_RESTART.
      WsfDisUtil::GetSimulation(mInterfacePtr).RequestReset();
      actedUpon = true;
   }
   else if (reason == DisEnum::Control::Reason::TERMINATION)
   {
      WsfDisUtil::GetSimulation(mInterfacePtr).RequestTermination();
      actedUpon = true;
   }

   return actedUpon;
}
