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

#include "dis/WsfDisData.hpp"

#include <iostream>

#include "Dis.hpp"
#include "WsfDisObserver.hpp"
#include "dis/WsfDisUtil.hpp"

//! Constructor
//! Local Origination
//! Generate WsfDisData when NOT responding to an externally received DisDataQuery or DisSetData PDU.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisData::WsfDisData(WsfDisInterface* aInterfacePtr)
   : DisData()
   , mInterfacePtr(aInterfacePtr)
{
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

//! Constructor
//! Remote Origination
//! Generate WsfDisData in response to an externally received DisDataQuery or DisSetData PDU or
//! Generate WsfDisData while processing an externally received DisData PDU.
//! @param aPdu          A reference to the incoming Protocol Data Unit (PDU).
//! @param aGenI         A GenI helper reference used to retrieve PDU information from the buffer.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisData::WsfDisData(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisData(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisData::~WsfDisData() {}

// virtual
int WsfDisData::Process()
{
   WsfObserver::DisDataReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);
   return 1;
}
