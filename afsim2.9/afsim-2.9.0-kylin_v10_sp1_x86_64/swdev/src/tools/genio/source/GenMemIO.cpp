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

#include "GenMemIO.hpp"

// This class inherits from GenIO, and GenMem

GenMemIO::GenMemIO(GenBuf::BufType aBufType, unsigned long aSize)
   : GenIO("GenMemIO", "GenMemIO")
   , GenMem()
{
   // '1' means to allow buffer to grow
   mBuf = new GenBufIO(aBufType, aSize, 1);
   SetGenBufXferI(mBuf);
   SetGenBufXferO(mBuf);
   SetGenBuf(mBuf);
}

GenMemIO::GenMemIO(unsigned char* aUserBuffer, unsigned long aBufferSize, GenBuf::BufType aBufType, unsigned long aNumValidBytes)
   : GenIO("GenMemIO", "GenMemIO")
   , GenMem()
{
   mBuf = new GenBufIO(aBufType, aUserBuffer, aBufferSize, aNumValidBytes);
   SetGenBufXferI(mBuf);
   SetGenBufXferO(mBuf);
   SetGenBuf(mBuf);
}

GenMemIO::~GenMemIO()
{
   delete mBuf;
}


int GenMemIO::Receive(int /* aWaitTimeInMicroSec */)
{
   // Clear the buffer
   mBuf->Reset();

   ClearInputErrorStatus();

   return 0;
}

int GenMemIO::Send()
{
   // Clear the buffer
   mBuf->Reset();

   ClearOutputErrorStatus();

   return 0;
}
