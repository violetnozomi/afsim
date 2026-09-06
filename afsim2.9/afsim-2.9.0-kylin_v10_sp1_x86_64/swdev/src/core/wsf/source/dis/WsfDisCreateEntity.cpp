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

#include "dis/WsfDisCreateEntity.hpp"

#include <iostream>

#include "Dis.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisCreateEntity::WsfDisCreateEntity(WsfDisInterface* aInterfacePtr)
   : DisCreateEntity()
   , mInterfacePtr(aInterfacePtr)
{
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisCreateEntity::WsfDisCreateEntity(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisCreateEntity(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisCreateEntity::~WsfDisCreateEntity() {}

// virtual
int WsfDisCreateEntity::Process()
{
   return 1;
}
