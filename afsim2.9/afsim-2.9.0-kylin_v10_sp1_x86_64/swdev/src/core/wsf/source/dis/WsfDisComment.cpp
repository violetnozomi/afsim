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

#include "dis/WsfDisComment.hpp"

#include "UtLog.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisComment::WsfDisComment(WsfDisInterface* aInterfacePtr)
   : DisComment()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisComment::WsfDisComment(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisComment(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisComment::~WsfDisComment() {}

// virtual
int WsfDisComment::Process()
{
   return 1; // Non-zero => delete PDU, Zero = Keep
}
