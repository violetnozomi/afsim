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
#include "GenPipeO.hpp"

GenPipeO::GenPipeO(GenBuf::BufType aBufType, GenPipe& aPipe)
   : GenO("GenPipeO")
   , mPipe(aPipe)
{
   mBuffer = new GenBufOManaged(aBufType, 1024, 1);
   SetGenBufXferO(mBuffer);
}

// virtual
GenPipeO::~GenPipeO()
{
   mPipe.Close();
}

// virtual
int GenPipeO::Send()
{
   int bytesLeft = GetGenBufXferO()->GetPutOffset();
   if (bytesLeft > 0)
   {
      mPipe.Write(mBuffer->GetBuffer(), bytesLeft);
      GetGenBufXferO()->SetPutOffset(0, GenBuf::FromBeg);
      mBuffer->Reset();
      ClearOutputErrorStatus();
   }
   return bytesLeft;
}
