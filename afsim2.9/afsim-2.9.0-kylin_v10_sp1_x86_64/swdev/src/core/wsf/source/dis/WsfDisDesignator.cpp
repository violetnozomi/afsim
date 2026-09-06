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

#include "dis/WsfDisDesignator.hpp"

#include "WsfDisObserver.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisDesignator::WsfDisDesignator(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisDesignator(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisDesignator::~WsfDisDesignator() {}

// virtual
int WsfDisDesignator::Process()
{
   // Give observers a chance to react to the PDU.
   WsfObserver::DisDesignatorReceived (&WsfDisUtil::GetSimulation(mInterfacePtr))(mInterfacePtr, *this);
   return 1;
}
