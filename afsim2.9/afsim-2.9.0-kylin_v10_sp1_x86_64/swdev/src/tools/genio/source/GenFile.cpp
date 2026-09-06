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

// GenFile - A base class for GenFileI, GenFileO, and GenFileIO.
//           Physical reads and writes are performed only when required.

#include "GenFile.hpp"

#include <memory>

#include "GenBufIO.hpp"
#include "UtLog.hpp"

GenFile::GenFile(const char* aFile, int aWriteMode, GenBuf::BufType aBufType)
   : mFile(aFile)
   , mFilePtr(nullptr)
   , mFilePos(0)
   , mWriteMode(aWriteMode)
   , mBufSize(1048576)
   , mBuf(nullptr)
{
   char* openMode;
   if (aWriteMode)
   {
      openMode = const_cast<char*>("w+b");
   }
   else
   {
      openMode = const_cast<char*>("rb");
   }

#if defined(_WIN32)
   mFilePtr = fopen(aFile, openMode);
#else
   mFilePtr = fopen64(aFile, openMode);
#endif

   if (!mFilePtr)
   {
      auto out = ut::log::error() << "GenFile: Cannot open file.";
      out.AddNote() << "File: " << aFile;
      out.AddNote() << "Errno: " << std::strerror(errno);
   }
   else
   {
      // Take the default to not let the buffer grow
      mBuf.reset(new GenBufIO(aBufType, mBufSize));
   }
}

GenFile::~GenFile()
{
   if (mFilePtr != nullptr)
   {
      fclose(mFilePtr);
   }
}

int GenFile::Seek(long long aFilePos)
{
   int result = 0;

   // Seek if needed
   if (mFilePos != aFilePos)
   {
#if defined(_WIN32)
      result = _fseeki64(mFilePtr, aFilePos, SEEK_SET);
#else
      result = fseeko64(mFilePtr, aFilePos, SEEK_SET);
#endif
      if (result != 0)
      {
         auto out = ut::log::error() << "GenFile::Seek error on file.";
         out.AddNote() << "File: " << mFile;
         out.AddNote() << "Errno: " << std::strerror(errno);
      }
      else
      {
         mFilePos = aFilePos;
      }
   }
   return result;
}

long long GenFile::Tell() const
{
#if defined(_WIN32)
   long long result = _ftelli64(mFilePtr);
#else
   long long result = ftello64(mFilePtr);
#endif

   if (result < 0)
   {
      auto out = ut::log::error() << "GenFile::Tell error on file.";
      out.AddNote() << "File: " << mFile;
      out.AddNote() << "Errno: " << std::strerror(errno);
   }

   return result;
}
