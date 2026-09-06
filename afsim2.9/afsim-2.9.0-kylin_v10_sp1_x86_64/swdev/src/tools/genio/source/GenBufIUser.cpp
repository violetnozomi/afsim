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

#include "GenBufIUser.hpp"

// It is OK to cast away the const here, because none of the base
// classes will change the buffer and we control those base clasees.
GenBufIUser::GenBufIUser(BufType aBufType, const unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes)
   : GenBuf(const_cast<unsigned char*>(aUserBuffer), aSizeInBytes, aNumValidBytes)
   , GenBufXferI(aBufType)
{
}


GenBufIUser::~GenBufIUser() {}
