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

#include "dis/WsfDisRemoveEntity.hpp"

#include "Dis.hpp"
#include "DisIO.hpp"
#include "UtLog.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisRemoveEntity::WsfDisRemoveEntity(WsfDisInterface* aInterfacePtr)
   : DisRemoveEntity()
   , mInterfacePtr(aInterfacePtr)
{
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisRemoveEntity::WsfDisRemoveEntity(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisRemoveEntity(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisRemoveEntity::~WsfDisRemoveEntity() {}

// virtual
int WsfDisRemoveEntity::Process()
{
   const int DELETE_PDU = 1;

   DisEntityId destId = GetReceivingEntity();
   DisUint16   site   = destId.GetSite();
   DisUint16   appl   = destId.GetApplication();
   // DisUint16   entity = destId.GetEntity();

   // Is this PDU intended for me?
   if ((site == Dis::GetSite()) && (appl == Dis::GetApplication()))
   {
      double simTime = WsfDisUtil::GetSimTime(mInterfacePtr);

      // Get dis Platform pointer
      WsfDisPlatform* lDisPlatform = WsfDisUtil::FindDisPlatform(mInterfacePtr, destId);

      if (lDisPlatform != nullptr)
      {
         auto out = ut::log::info() << "WsfDisRemoveEntity: Removing entity.";
         out.AddNote() << "Entity: " << destId;
         WsfDisUtil::GetSimulation(mInterfacePtr).DeletePlatform(simTime, lDisPlatform->GetPlatform());
      }

      if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         auto out = ut::log::debug() << "WsfDisRemoveEntity: PDU received.";
         out.AddNote() << "T = " << simTime;
      }
   }

   return DELETE_PDU;
}
