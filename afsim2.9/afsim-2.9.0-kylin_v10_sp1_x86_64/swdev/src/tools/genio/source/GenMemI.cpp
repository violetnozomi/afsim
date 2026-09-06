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

#include "GenMemI.hpp"

// This class inherits from GenI, and GenMem

GenMemI::GenMemI(const unsigned char* aUserBuffer, unsigned long aBufferSize, GenBuf::BufType aBufType, unsigned long aNumValidBytes)
   : GenI("GenMemI")
   , GenMem()
{
   mBuf = new GenBufIUser(aBufType, aUserBuffer, aBufferSize, aNumValidBytes);
   SetGenBufXferI(mBuf);
   SetGenBuf(mBuf);
}

GenMemI::~GenMemI()
{
   delete mBuf;
}


int GenMemI::Receive(int /* aWaitTimeInMicroSec */)
{
   // Clear the input buffer
   mBuf->Reset();

   ClearInputErrorStatus();

   return 0;
}
