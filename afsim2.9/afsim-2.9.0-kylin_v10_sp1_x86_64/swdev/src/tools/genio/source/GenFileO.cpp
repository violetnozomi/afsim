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

// GenFileO - A derived class of GenO that does buffered output to a file.
//            Physical writes are performed only when required.

#include "GenFileO.hpp"

#include <cstdio>
#include <iostream>

#include "GenBufIO.hpp"
#include "UtLog.hpp"

GenFileO::GenFileO(const char* aFile, GenBuf::BufType aBufType)
   : GenFile(aFile, 1, aBufType)
   , GenO("GenFileO")
{
   SetGenBufXferO(GetGenBufIO());
}

GenFileO::GenFileO()
   : GenFile()
   , GenO("GenFileO")
{
   SetGenBufXferO(GetGenBufIO());
}

GenFileO::~GenFileO()
{
   if (mFilePtr != nullptr && mWriteMode)
   {
      WriteBuffer();
   }
}

int GenFileO::Send()
{
   ClearOutputErrorStatus();
   return static_cast<int>(WriteBuffer()); // Send buffered data on its way
}

unsigned long GenFileO::WriteDirect(void* aBufferPtr, unsigned long aBytesToWrite)
{
   // Do the actual write
   unsigned long bytesWritten = static_cast<unsigned long>(fwrite(aBufferPtr, 1, aBytesToWrite, mFilePtr));

   if (bytesWritten == aBytesToWrite)
   {
      mFilePos += bytesWritten;
      return bytesWritten;
   }
   else
   {
      auto out = ut::log::error() << "GenFile::WriteDirect: Write error on file.";
      out.AddNote() << "File: " << mFile;
      out.AddNote() << "Errno: " << std::strerror(errno);
      return 0;
   }
}

// Virtual
int GenFileO::HandlePutError(int aStatus, unsigned long aVariableSize)
{
   if (aStatus == GenBuf::TooMuchData)
   {
      if (WriteBuffer() > 0)
      {
         return 1;
      }
      else
      {
         return GenO::HandlePutError(aStatus, aVariableSize);
      }
   }
   else
   {
      return GenO::HandlePutError(aStatus, aVariableSize);
   }
}

//
// Private
//
unsigned long GenFileO::WriteBuffer()
{
   unsigned long status       = 0;
   unsigned long bytesToWrite = mBuf->GetPutOffset(GenBufXferO::FromBeg);
   if (bytesToWrite != 0)
   {
      // Let WriteDirect do the acutal reading
      status = WriteDirect(mBuf->GetBuffer(), bytesToWrite);
   }
   mBuf->Reset();
   return status;
}
