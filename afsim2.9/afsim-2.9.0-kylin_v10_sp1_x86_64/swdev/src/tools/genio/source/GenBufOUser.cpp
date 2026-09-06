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

#include "GenBufOUser.hpp"

GenBufOUser::GenBufOUser(BufType aBufType, unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes)
   : GenBuf(aUserBuffer, aSizeInBytes, aNumValidBytes)
   , GenBufXferO(aBufType)
{
}


GenBufOUser::~GenBufOUser() {}
