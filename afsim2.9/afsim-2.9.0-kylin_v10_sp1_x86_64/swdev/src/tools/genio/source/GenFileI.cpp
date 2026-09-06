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

// GenFileI  - A derived class of GenI that does buffered input from a file.
//             Physical reads are performed only when required.

#include "GenFileI.hpp"

#include "GenBufIO.hpp"
#include "UtLog.hpp"

GenFileI::GenFileI(const char* aFile, GenBuf::BufType aBufType)
   : GenFile(aFile, 0, aBufType)
   , GenI("GenFileI")
   , mBufferOffset(0)
{
   SetGenBufXferI(GetGenBufIO());
}

GenFileI::GenFileI()
   : GenFile()
   , GenI("GenFileI")
   , mBufferOffset(0)
{
   SetGenBufXferI(GetGenBufIO());
}

GenFileI::~GenFileI() {}

int GenFileI::Receive(int /* aWaitTimeInMicroSec */)
{
   ClearInputErrorStatus();

   // Move any unread data to the front of the buffer and then refill the buffer
   // Maybe this routine should return a long someday
   return static_cast<int>(ReadBuffer());
}

unsigned long GenFileI::ReadDirect(void* aBufferPtr, unsigned long aBytesToRead)
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

// Virtual
int GenFileI::HandleGetError(int aStatus, unsigned long aVariableSize)
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

//
// Private
//
unsigned long GenFileI::ReadBuffer()
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

   // Let ReadDirect do the actual reading
   mBufferOffset           = bytesInBuf + ftell(mFilePtr);
   unsigned long bytesRead = ReadDirect(bufAddr + bytesInBuf, bytesToRead);

   mBuf->SetNumValidBytes(bytesInBuf + bytesRead);
   mBuf->SetPutOffset(bytesInBuf + bytesRead, GenBuf::FromBeg);
   return bytesInBuf + bytesRead;
}

long long GenFileI::TellBufferOffset() const
{
   return mBufferOffset;
}
