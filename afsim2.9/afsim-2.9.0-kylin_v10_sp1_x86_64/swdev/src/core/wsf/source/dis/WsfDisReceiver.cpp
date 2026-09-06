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

#include "dis/WsfDisReceiver.hpp"

#include <iostream>

#include "dis/WsfDisUtil.hpp"

WsfDisReceiver::WsfDisReceiver(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisReceiver(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisReceiver::~WsfDisReceiver() {}

// virtual
int WsfDisReceiver::Process()
{
   return 1;
}
