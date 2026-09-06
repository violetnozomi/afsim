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

#include "dis/WsfDisTransmitter.hpp"

#include "UtLog.hpp"
#include "WsfDisObserver.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisTransmitter::WsfDisTransmitter(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisTransmitter(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisTransmitter::~WsfDisTransmitter() {}

// virtual
int WsfDisTransmitter::Process()
{
   WsfObserver::DisTransmitterReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);
   return 1;
}
