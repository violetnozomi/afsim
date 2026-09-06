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

#include "dis/WsfDisAcknowledge.hpp"

#include <iostream>

WsfDisAcknowledge::WsfDisAcknowledge(WsfDisInterface* aInterfacePtr)
   : DisAcknowledge()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisAcknowledge::WsfDisAcknowledge(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisAcknowledge(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisAcknowledge::~WsfDisAcknowledge() {}

// virtual
int WsfDisAcknowledge::Process()
{
   const int DELETE_PDU = 1;
   return DELETE_PDU; // Non-zero => delete PDU, Zero = Keep
}
