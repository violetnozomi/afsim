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

#include "dis/WsfDisStartResume.hpp"

#include "Dis.hpp"
#include "DisIO.hpp"
#include "UtLog.hpp"
#include "WsfDisObserver.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisStartResume::WsfDisStartResume(WsfDisInterface* aInterfacePtr)
   : DisStartResume()
   , mInterfacePtr(aInterfacePtr)
{
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisStartResume::WsfDisStartResume(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisStartResume(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisStartResume::~WsfDisStartResume() {}

// virtual
int WsfDisStartResume::Process()
{
   // Allow other WSF level applications to handle DisStartResume PDU
   WsfObserver::DisStartResumeReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);

   const int DELETE_PDU = 1;

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
         WsfDisUtil::GetSimulation(mInterfacePtr).Resume();

         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            double      simTime  = WsfDisUtil::GetSimTime(mInterfacePtr);
            DisEntityId sourceId = GetOriginatingEntity();
            auto        out      = ut::log::debug() << "WsfDisStartResume: PDU received and processed.";
            out.AddNote() << "T = " << simTime;
            out.AddNote() << "Source: " << sourceId.GetSite() << ":" << sourceId.GetApplication();
            out.AddNote() << "Contained time parameters are currently unused and ignored.";
         }
      }
   }

   return DELETE_PDU;
}
