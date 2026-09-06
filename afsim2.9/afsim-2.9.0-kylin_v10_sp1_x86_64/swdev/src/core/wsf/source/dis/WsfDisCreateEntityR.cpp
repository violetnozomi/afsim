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

#include "dis/WsfDisCreateEntityR.hpp"

#include "Dis.hpp"
#include "DisControlEnums.hpp"
#include "DisPduEnums.hpp"
#include "UtLog.hpp"
#include "dis/WsfDisAcknowledgeR.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisCreateEntityR::WsfDisCreateEntityR(WsfDisInterface* aInterfacePtr)
   : DisCreateEntityR()
   , mInterfacePtr(aInterfacePtr)
{
   SetReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}
WsfDisCreateEntityR::WsfDisCreateEntityR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisCreateEntityR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisCreateEntityR::~WsfDisCreateEntityR() = default;

bool WsfDisCreateEntityR::ProcessCreateEntityRequest()
{
   bool actedUpon = false;
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      // For the moment, our only response is to say that we cannot respond.
      ut::log::warning() << "WsfDisCreateEntityR: Cannot yet comply with ProcessCreateEntityRequest().";
      actedUpon = true;
   }

   return actedUpon;
}

// virtual
void WsfDisCreateEntityR::TimedOut()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisCreateEntityR: Unhandled virtual TimedOut() call.";
   }
}

DisPdu* WsfDisCreateEntityR::ProduceResponse(bool aIsDone) const
{
   WsfDisAcknowledgeR* responsePtr = new WsfDisAcknowledgeR(mInterfacePtr);
   responsePtr->SetReceivingEntity(GetOriginatingEntity());
   responsePtr->SetRequestId(GetRequestId());
   responsePtr->SetAcknowledgeFlag(DisEnum::Control::ResponseToCmdFlag::CREATE_ENTITY);
   DisEnum16 response = DisEnum::Control::ResponseCompliance::UNABLE_TO_COMPLY;
   if (aIsDone)
   {
      response = DisEnum::Control::ResponseCompliance::ABLE_TO_COMPLY;
   }
   responsePtr->SetResponseFlag(response);
   return responsePtr;
}
