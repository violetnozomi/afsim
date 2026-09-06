// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfDisTransferOwnership.hpp"

#include "WsfDisObserver.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisTransferOwnership::WsfDisTransferOwnership(WsfDisInterface* aInterfacePtr)
   : DisTransferOwnership()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisTransferOwnership::WsfDisTransferOwnership(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisTransferOwnership(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

int WsfDisTransferOwnership::Process()
{
   const int DELETE_PDU = 1;
   WsfObserver::DisTransferOwnershipReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);
   return DELETE_PDU;
}
