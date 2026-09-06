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

#include "dis/WsfDisCommentR.hpp"

#include <iostream>

#include "dis/WsfDisUtil.hpp"

WsfDisCommentR::WsfDisCommentR(WsfDisInterface* aInterfacePtr)
   : DisCommentR()
   , mInterfacePtr(aInterfacePtr)
{
}

WsfDisCommentR::WsfDisCommentR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisCommentR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisCommentR::~WsfDisCommentR() {}

void WsfDisCommentR::SetCommentId(WsfStringId aStringId)
{
   std::string comment = aStringId.GetString();
   GetData().SetData((unsigned char*)(comment.c_str()), static_cast<DisUint32>(comment.length() * 8));
}

// virtual
int WsfDisCommentR::Process()
{
   return 1; // Non-zero => delete PDU, Zero = Keep
}
