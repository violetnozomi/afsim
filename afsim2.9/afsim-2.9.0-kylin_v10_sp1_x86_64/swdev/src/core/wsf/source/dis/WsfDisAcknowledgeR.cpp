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

#include "dis/WsfDisAcknowledgeR.hpp"

#include "Dis.hpp"
#include "UtLog.hpp"
#include "WsfDisObserver.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisAcknowledgeR::WsfDisAcknowledgeR(WsfDisInterface* aInterfacePtr)
   : DisAcknowledgeR()
   , mInterfacePtr(aInterfacePtr)
{
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisAcknowledgeR::WsfDisAcknowledgeR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisAcknowledgeR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisAcknowledgeR::~WsfDisAcknowledgeR() {}

// virtual
void WsfDisAcknowledgeR::TimedOut()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisAcknowledgeR: Unhandled virtual TimedOut() call.";
   }
}

// virtual
int WsfDisAcknowledgeR::Process()
{
   const int DELETE_PDU = 1;
   WsfObserver::DisAcknowledgeRReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);
   return DELETE_PDU;
}
