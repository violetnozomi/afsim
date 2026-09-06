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

#include "dis/WsfDisStopFreeze.hpp"

#include "Dis.hpp"
#include "DisControlEnums.hpp"
#include "UtLog.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisStopFreeze::WsfDisStopFreeze(WsfDisInterface* aInterfacePtr)
   : DisStopFreeze()
   , mInterfacePtr(aInterfacePtr)
{
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisStopFreeze::WsfDisStopFreeze(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisStopFreeze(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisStopFreeze::~WsfDisStopFreeze() {}

// virtual
int WsfDisStopFreeze::Process()
{
   const int DELETE_PDU = 1;

   bool actedUpon = false;

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
         DisEnum8 reason = GetReason();
         if (reason == DisEnum::Control::Reason::RECESS)
         {
            WsfDisUtil::GetSimulation(mInterfacePtr).Pause();
            actedUpon = true;
         }
         else if ((reason == DisEnum::Control::Reason::STOP_FOR_RESET) ||
                  (reason == DisEnum::Control::Reason::STOP_FOR_RESTART) ||
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
      }
   }

   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      double      simTime  = WsfDisUtil::GetSimTime(mInterfacePtr);
      DisEntityId sourceId = GetOriginatingEntity();

      auto out = ut::log::debug() << "WsfDisStopFreeze: PDU received and processed.";
      out.AddNote() << "T = " << simTime;
      out.AddNote() << "Source: " << sourceId.GetSite() << ":" << sourceId.GetApplication();
      if (actedUpon)
      {
         out.AddNote() << "Acted upon. No reply required.";
      }
      else
      {
         out.AddNote() << "Probably not acted upon due to non-specific Reason(), or addressed to a specific entity.";
      }
   }

   return DELETE_PDU;
}
