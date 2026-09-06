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

// GenFileIO - A derived class of GenIO that does buffered I/O on a file.
//             Physical reads and writes are performed only when required.

#include "GenFileIO.hpp"

#include <cstdio>
#include <iostream>

#include "GenBufIO.hpp"
#include "UtLog.hpp"

GenFileIO::GenFileIO(const char* aFile, GenBuf::BufType aBufType, unsigned int aReadOnly)
   : GenFile(aFile, !aReadOnly, aBufType)
   , GenIO("GenFileIO", "GenFileIO")
{
   SetGenBufXferI(GetGenBufIO());
   SetGenBufXferO(GetGenBufIO());
}

GenFileIO::~GenFileIO() {}

int GenFileIO::Receive(int /* aWaitTimeInMicroSec */)
{
   ClearInputErrorStatus();

   // Move any unread data to the front of the buffer and then refill the buffer
   // Maybe this routine should return a long someday
   return static_cast<int>(ReadBuffer());
}

unsigned long GenFileIO::ReadDirect(void* aBufferPtr, unsigned long aBytesToRead)
{
   unsigned long bytesRead = (unsigned long)fread(aBufferPtr, 1, aBytesToRead, mFilePtr);

   // 99% of the time this if should be true after the test bytesRead == bytesToRead
   if (bytesRead == aBytesToRead || (bytesRead < aBytesToRead && feof(mFilePtr) != 0))
   {
      mFilePos += bytesRead;
      return bytesRead;
   }
   else
   {
      auto out = ut::log::error() << "GenFile::ReadDirect: Read error on file.";
      out.AddNote() << "File: " << mFile;
      out.AddNote() << "Errno: " << std::strerror(errno);
      return 0;
   }
}

int GenFileIO::Send()
{
   ClearOutputErrorStatus();
   return static_cast<int>(WriteBuffer()); // Send buffered data on its way
}

unsigned long GenFileIO::WriteDirect(void* aBufferPtr, unsigned long aBytesToWrite)
{
   // Do the actual write
   unsigned long bytesWritten = (unsigned long)fwrite(aBufferPtr, 1, aBytesToWrite, mFilePtr);

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


//
// Protected
//

// Virtual
int GenFileIO::HandleGetError(int aStatus, unsigned long aVariableSize)
{
   if (aStatus == GenBuf::NotEnoughData)
   {
      if (ReadBuffer() > 0)
      {
         return 1;
      }
      else
      {
         return GenI::HandleGetError(aStatus, aVariableSize);
      }
   }
   else
   {
      return GenI::HandleGetError(aStatus, aVariableSize);
   }
}

// Virtual
int GenFileIO::HandlePutError(int aStatus, unsigned long aVariableSize)
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
//  Private
//
unsigned long GenFileIO::ReadBuffer()
{
   // Shift any existing data up to make room if there is still data left in buffer.
   unsigned char* bufAddr    = mBuf->GetBuffer();
   unsigned long  bytesInBuf = mBuf->GetGetOffset(GenBuf::FromDataEnd);
   if (bytesInBuf != 0)
   {
      long int getOffset = mBuf->GetGetOffset(GenBuf::FromBeg);
      memcpy(bufAddr, bufAddr + getOffset, bytesInBuf);

      mBuf->SetNumValidBytes(bytesInBuf);
      mBuf->SetGetOffset(0, GenBuf::FromBeg);
      mBuf->SetPutOffset(0, GenBuf::FromBeg);
   }
   else
   {
      mBuf->Reset();
   }

   unsigned long bytesToRead = mBufSize - bytesInBuf;

   // Let ReadDirect do the acutal reading
   unsigned long bytesRead = ReadDirect(bufAddr + bytesInBuf, bytesToRead);

   mBuf->SetNumValidBytes(bytesInBuf + bytesRead);
   mBuf->SetPutOffset(bytesInBuf + bytesRead, GenBuf::FromBeg);
   return bytesInBuf + bytesRead;
}

unsigned long GenFileIO::WriteBuffer()
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
