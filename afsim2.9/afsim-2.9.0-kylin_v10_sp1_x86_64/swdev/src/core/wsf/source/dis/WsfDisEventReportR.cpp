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

#include "dis/WsfDisEventReportR.hpp"

#include "dis/WsfDisUtil.hpp"

WsfDisEventReportR::WsfDisEventReportR(WsfDisInterface* aInterfacePtr)
   : DisEventReportR()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisEventReportR::WsfDisEventReportR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisEventReportR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisEventReportR::~WsfDisEventReportR() {}

// virtual
int WsfDisEventReportR::Process()
{
   return 1; // Non-zero => delete PDU, Zero = Keep
}
