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

#include "GenMemO.hpp"

// This class inherits from GenIO, and GenMem

GenMemO::GenMemO(GenBuf::BufType aBufType, unsigned long aSize)
   : GenO("GenMemO")
   , GenMem()
{
   // '1' means to allow buffer to grow
   mBuf = new GenBufOManaged(aBufType, aSize, 1);
   SetGenBufXferO(mBuf);
   SetGenBuf(mBuf);
}

GenMemO::~GenMemO()
{
   delete mBuf;
}

int GenMemO::Send()
{
   // Clear the buffer
   mBuf->Reset();

   ClearOutputErrorStatus();

   return 0;
}
