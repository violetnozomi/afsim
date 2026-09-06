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
#include "GenPipeI.hpp"

#include "GenBufIO.hpp"

GenPipeI::GenPipeI(GenBuf::BufType aBufType, GenPipe& aPipe)
   : GenI("GenPipeI")
   , mPipe(aPipe)
{
   mBuffer = new GenBufIO(aBufType, 1024, 1);
   SetGenBufXferI(mBuffer);
}

// virtual
GenPipeI::~GenPipeI()
{
   mPipe.Close();
}

// virtual
int GenPipeI::Receive(int aWaitTimeInMicroSec)
{
   SetGetOffset(0);
   unsigned char* buffer = mBuffer->GetBuffer();
   int            bytes  = mPipe.Read(buffer, GetGenBufXferI()->GetTotalBytes());
   if (bytes > 0)
   {
      GetGenBufXferI()->SetNumValidBytes(bytes);
   }
   return 0;
}
