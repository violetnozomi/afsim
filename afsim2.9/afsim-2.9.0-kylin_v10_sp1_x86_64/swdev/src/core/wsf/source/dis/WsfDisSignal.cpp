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

#include "dis/WsfDisSignal.hpp"

#include <iostream>

#include "WsfDisObserver.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisSignal::WsfDisSignal(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisSignal(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisSignal::~WsfDisSignal() {}

// virtual
int WsfDisSignal::Process()
{
   WsfObserver::DisSignalReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);

   return 1;
}
