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
#include "GenPipe.hpp"
#ifdef _WIN32
#include <winsock2.h>

#include <windows.h>

#include <fcntl.h>
#include <io.h>
#include <winbase.h>
#else
#ifdef __sun
#include <sys/filio.h> // for FIONBIO on sun
#endif
#include <sys/ioctl.h>
#include <unistd.h>
#endif

GenPipe::GenPipe(size_t aPipeHandle)
   : mPipeHandle(aPipeHandle)
{
}

//! Writes data to the pipe.
//! @return The number of bytes written to the pipe
int GenPipe::Write(const void* aBuffer, unsigned int aBytes)
{
#ifdef WIN32
   DWORD bytesWritten = 0;
   while (bytesWritten < aBytes)
   {
      unsigned long bytes;
      if (WriteFile((HANDLE)mPipeHandle, ((char*)aBuffer) + bytesWritten, aBytes - bytesWritten, &bytes, 0))
      {
         bytesWritten += bytes;
      }
      else
      {
         break;
      }
   }
   return aBytes - bytesWritten;
#else
   unsigned int bytesWritten = 0;
   while (bytesWritten < aBytes)
   {
      int bytes = write(mPipeHandle, ((char*)aBuffer) + bytesWritten, aBytes - bytesWritten);
      if (bytes >= 0)
      {
         bytesWritten += bytes;
      }
      else
      {
         break;
      }
   }
   return aBytes - bytesWritten;
#endif
}

//! Reads data from the pipe.
//! @return The number of bytes read from the pipe
int GenPipe::Read(void* aBuffer, unsigned int aBufferBytes)
{
#ifdef WIN32
   int           bytesRead = 0;
   unsigned long bytes;
   if (ReadFile((HANDLE)mPipeHandle, ((char*)aBuffer) + bytesRead, aBufferBytes - bytesRead, &bytes, 0))
   {
      bytesRead += bytes;
   }
   return bytesRead;
#else
   unsigned int bytesRead = 0;
   while (bytesRead != aBufferBytes)
   {
      int bytes = read(mPipeHandle, ((char*)aBuffer) + bytesRead, aBufferBytes - bytesRead);
      if (bytes >= 0)
      {
         bytesRead += bytes;
      }
      else
      {
         break;
      }
   }

   return bytesRead;
#endif
}

#ifdef WIN32
// static
//! Creates a one way pipe.
//! @param aReadPipe  The read end of the pipe.
//!                   The Write() method is not valid for this pipe.
//! @param aWritePipe The write end of the pipe.
//!                   The Read() method is not valid for this pipe.
bool GenPipe::Create(GenPipe& aReadPipe, GenPipe& aWritePipe)
{
   // Windows also supplies a _pipe() method, but does not allow
   // non-blocking reads/writes to that pipe.  With CreatePipe(),
   // non-blocking reads/writes can be done with ReadFile() and
   // WriteFile()
   bool ok = false;

   SECURITY_ATTRIBUTES security;
   security.bInheritHandle       = true;
   security.lpSecurityDescriptor = 0;
   security.nLength              = sizeof(SECURITY_ATTRIBUTES);

   HANDLE readHandle, writeHandle;
   if (CreatePipe(&readHandle, &writeHandle, &security, 0 /*default buffer size*/))
   {
      unsigned long mode = PIPE_NOWAIT;
      SetNamedPipeHandleState(readHandle, &mode, 0, 0);
      mode = PIPE_NOWAIT;
      SetNamedPipeHandleState(writeHandle, &mode, 0, 0);
      ok         = true;
      aReadPipe  = GenPipe(reinterpret_cast<size_t>(readHandle));
      aWritePipe = GenPipe(reinterpret_cast<size_t>(writeHandle));
   }
   return ok;
}
#else
// static
//! Creates a one way pipe.
//! @param aReadPipe  The read end of the pipe.
//!                   The Write() method is not valid for this pipe.
//! @param aWritePipe The write end of the pipe.
//!                   The Read() method is not valid for this pipe.
bool GenPipe::Create(GenPipe& aPipe1, GenPipe& aPipe2)
{
   bool ok = false;
   int fileDes[2];
   // if (0 == pipe(fileDes, 1024, O_BINARY))
   if (0 == pipe(fileDes))
   {
      // Set the pipe to non-blocking
      unsigned long onOff = 1;
      ioctl(fileDes[0], FIONBIO, &onOff);
      ioctl(fileDes[1], FIONBIO, &onOff);

      aPipe1 = GenPipe(fileDes[0]);
      aPipe2 = GenPipe(fileDes[1]);
      ok = true;
   }
   return ok;
}
#endif

//! Closes the pipe.
void GenPipe::Close()
{
   if (mPipeHandle != 0)
   {
#ifdef WIN32
      CloseHandle(reinterpret_cast<HANDLE>(mPipeHandle));
#else
      close(mPipeHandle);
#endif
   }
}
