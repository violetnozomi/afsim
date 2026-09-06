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

#include "DisSystemId.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

DisSystemId::DisSystemId()
   : mSystemType(0)
   , mSystemName(0)
   , mSystemMode(0)
   , mChangeOptions(0)
{
}

DisSystemId::~DisSystemId() {}

void DisSystemId::Get(GenI& aGenI)
{
   aGenI >> mSystemType;
   aGenI >> mSystemName;
   aGenI >> mSystemMode;
   aGenI >> mChangeOptions;
}

void DisSystemId::Put(GenO& aGenO) const
{
   aGenO << mSystemType;
   aGenO << mSystemName;
   aGenO << mSystemMode;
   aGenO << mChangeOptions;
}

bool DisSystemId::IsValid() const
{
   return true;
}
