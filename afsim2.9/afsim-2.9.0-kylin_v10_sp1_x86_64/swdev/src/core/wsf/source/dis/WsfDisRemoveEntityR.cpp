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

#include "dis/WsfDisRemoveEntityR.hpp"

#include "Dis.hpp"
#include "DisControlEnums.hpp"
#include "UtLog.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisAcknowledgeR.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisRemoveEntityR::WsfDisRemoveEntityR(WsfDisInterface* aInterfacePtr)
   : DisRemoveEntityR()
   , mInterfacePtr(aInterfacePtr)
{
   SetReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisRemoveEntityR::WsfDisRemoveEntityR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisRemoveEntityR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisRemoveEntityR::~WsfDisRemoveEntityR() {}

// virtual
int WsfDisRemoveEntityR::Process()
{
   const int DELETE_PDU = 1;
   return DELETE_PDU;
}

bool WsfDisRemoveEntityR::ProcessRemoveEntityRequest()
{
   bool success = false;
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisRemoveEntityR: Cannot yet comply with the Remove request.";
      success = true;
   }
   return success;
}

// virtual
void WsfDisRemoveEntityR::TimedOut()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisRemoveEntityR: Unhandled virtual TimedOut() call.";
   }
}

// virtual
DisPdu* WsfDisRemoveEntityR::ProduceResponse(bool aIsDone)
{
   WsfDisAcknowledgeR* resultPtr = new WsfDisAcknowledgeR(mInterfacePtr);
   resultPtr->SetReceivingEntity(GetOriginatingEntity());
   resultPtr->SetRequestId(GetRequestId());
   resultPtr->SetAcknowledgeFlag(DisEnum::Control::ResponseToCmdFlag::REMOVE_ENTITY);
   DisEnum32 compliance = DisEnum::Control::ResponseCompliance::UNABLE_TO_COMPLY;
   if (aIsDone)
   {
      compliance = DisEnum::Control::ResponseCompliance::ABLE_TO_COMPLY;
   }
   resultPtr->SetResponseFlag(compliance);
   return resultPtr;
}
