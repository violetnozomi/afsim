// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtMemoryMappedFile.hpp"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <stdexcept>

// OS-specific
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// Windows
#include <windows.h>
#else
// Linux
// enable large file support on 32 bit systems
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifdef _FILE_OFFSET_BITS
#undef _FILE_OFFSET_BITS
#endif
#define _FILE_OFFSET_BITS 64
// and include needed headers
#include <errno.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

/// get OS page size (for ReMap)
int UtMemMap::GetPageSize()
{
#ifdef _WIN32
   SYSTEM_INFO sysInfo;
   GetSystemInfo(&sysInfo);
   return static_cast<int>(sysInfo.dwAllocationGranularity);
#else
   return sysconf(_SC_PAGESIZE); //::getpagesize();
#endif
}


/// open file, aNumBytesToMap = 0 maps the whole file
UtMemoryMappedFile::UtMemoryMappedFile(const std::string& aFilename, size_t aNumBytesToMap, CacheHint aHint)
   : mHint(aHint)
   , mMappedBytes(aNumBytesToMap)
{
   Open(aFilename, aNumBytesToMap, aHint);
}

UtMemoryMappedFile::UtMemoryMappedFile(UtMemoryMappedFile&& aMov) noexcept
   : mFileSize(aMov.mFileSize)
   , mHint(aMov.mHint)
   , mMappedBytes(aMov.mMappedBytes)
#ifdef _WIN32
   , mMappedFilePtr(aMov.mMappedFilePtr)
#endif
   , mFileHandle(aMov.mFileHandle)
   , mMappedViewPtr(aMov.mMappedViewPtr)
{
   aMov.mFileSize    = 0;
   aMov.mMappedBytes = 0;
#ifdef _WIN32
   aMov.mMappedFilePtr = nullptr;
#endif
   aMov.mFileHandle    = 0;
   aMov.mMappedViewPtr = nullptr;
}

UtMemoryMappedFile& UtMemoryMappedFile::operator=(UtMemoryMappedFile&& aMov) noexcept
{
   if (&aMov == this)
   {
      return *this;
   }

   mFileSize    = aMov.mFileSize;
   mHint        = aMov.mHint;
   mMappedBytes = aMov.mMappedBytes;
#ifdef _WIN32
   mMappedFilePtr = aMov.mMappedFilePtr;
#endif
   mFileHandle    = aMov.mFileHandle;
   mMappedViewPtr = aMov.mMappedViewPtr;


   aMov.mFileSize    = 0;
   aMov.mMappedBytes = 0;
#ifdef _WIN32
   aMov.mMappedFilePtr = nullptr;
#endif
   aMov.mFileHandle    = 0;
   aMov.mMappedViewPtr = nullptr;
   return *this;
}


/// close file (see Close() )
UtMemoryMappedFile::~UtMemoryMappedFile()
{
   Close();
}


/// open file
bool UtMemoryMappedFile::Open(const std::string& aFilename, size_t aNumBytesToMap, CacheHint aHint)
{
   // already open ?
   if (IsValid())
   {
      return false;
   }

   mFileHandle = 0;
   mFileSize   = 0;
   mHint       = aHint;
#ifdef _WIN32
   mMappedFilePtr = nullptr;
#endif
   mMappedViewPtr = nullptr;

#ifdef _WIN32
   // Windows

   DWORD winHint = 0;
   switch (mHint)
   {
   case CacheHint::Normal:
      winHint = FILE_ATTRIBUTE_NORMAL;
      break;
   case CacheHint::SequentialScan:
      winHint = FILE_FLAG_SEQUENTIAL_SCAN;
      break;
   case CacheHint::RandomAccess:
      winHint = FILE_FLAG_RANDOM_ACCESS;
      break;
   default:
      break;
   }

   // open file
   mFileHandle = ::CreateFileA(aFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, winHint, nullptr);
   if (mFileHandle == nullptr || mFileHandle == INVALID_HANDLE_VALUE)
   {
      mFileHandle = nullptr;
      return false;
   }

   // file size
   LARGE_INTEGER result;
   if (!GetFileSizeEx(mFileHandle, &result))
      return false;
   mFileSize = static_cast<uint64_t>(result.QuadPart);

   // convert to mapped mode
   mMappedFilePtr = ::CreateFileMapping(mFileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
   if (mMappedFilePtr == nullptr || mMappedFilePtr == INVALID_HANDLE_VALUE)
   {
      ::CloseHandle(mFileHandle);
      mFileHandle    = nullptr;
      mMappedFilePtr = nullptr;
      return false;
   }

#else

   // Linux

   // open file
   mFileHandle = ::open(aFilename.c_str(), O_RDONLY | O_LARGEFILE);
   if (mFileHandle == -1)
   {
      mFileHandle = 0;
      return false;
   }

   // file size
   struct stat64 statInfo;
   if (fstat64(mFileHandle, &statInfo) < 0)
   {
      return false;
   }

   mFileSize = statInfo.st_size;
#endif

   // initial mapping
   ReMap(0, aNumBytesToMap);

   if (!mMappedViewPtr)
   {
      return false;
   }

   // everything's fine
   return true;
}


/// close file
void UtMemoryMappedFile::Close()
{
   // kill pointer
   if (mMappedViewPtr)
   {
#ifdef _WIN32
      ::UnmapViewOfFile(mMappedViewPtr);
#else
      ::munmap(mMappedViewPtr, mMappedBytes);
#endif
      mMappedViewPtr = nullptr;
   }

#ifdef _WIN32
   if (mMappedFilePtr)
   {
      ::CloseHandle(mMappedFilePtr);
      mMappedFilePtr = nullptr;
   }
#endif

   // close underlying file
   if (mFileHandle)
   {
#ifdef _WIN32
      ::CloseHandle(mFileHandle);
#else
      ::close(mFileHandle);
#endif
      mFileHandle = 0;
   }

   mFileSize = 0;
}


/// access position, no range checking (faster)
unsigned char UtMemoryMappedFile::operator[](size_t aOffset) const
{
   return static_cast<unsigned char*>(mMappedViewPtr)[aOffset];
}


/// access position, including range checking
unsigned char UtMemoryMappedFile::At(size_t aOffset) const
{
   // checks
   if (!mMappedViewPtr)
      throw std::invalid_argument("No view mapped");
   if (aOffset >= mFileSize)
      throw std::out_of_range("View is not large enough");

   return operator[](aOffset);
}


/// raw access
const unsigned char* UtMemoryMappedFile::GetData() const
{
   return static_cast<const unsigned char*>(mMappedViewPtr);
}


/// true, if file successfully opened
bool UtMemoryMappedFile::IsValid() const
{
   return mMappedViewPtr != nullptr;
}


/// get file size
uint64_t UtMemoryMappedFile::FileSize() const
{
   return mFileSize;
}


/// get number of actually mapped bytes
size_t UtMemoryMappedFile::MappedSize() const
{
   return mMappedBytes;
}


/// replace mapping by a new one of the same file, aOffset MUST be a multiple of the page size
bool UtMemoryMappedFile::ReMap(uint64_t aOffset, size_t aNumBytesToMap)
{
   assert(aOffset % UtMemMap::GetPageSize() == 0); // precondition invariant

   if (!mFileHandle)
   {
      return false;
   }

   if (aNumBytesToMap == UtMemMap::MapWholeFile)
   {
      aNumBytesToMap = mFileSize;
   }

   // close old mapping
   if (mMappedViewPtr)
   {
#ifdef _WIN32
      ::UnmapViewOfFile(mMappedViewPtr);
#else
      ::munmap(mMappedViewPtr, mMappedBytes);
#endif
      mMappedViewPtr = nullptr;
   }

   // don't go further than end of file
   if (aOffset > mFileSize)
   {
      return false;
   }
   if (aOffset + aNumBytesToMap > mFileSize)
   {
      aNumBytesToMap = size_t(mFileSize - aOffset);
   }

#ifdef _WIN32
   // Windows

   DWORD offsetLow  = DWORD(aOffset & 0xFFFFFFFF);
   DWORD offsetHigh = DWORD(aOffset >> 32);
   mMappedBytes     = aNumBytesToMap;

   // get memory address
   mMappedViewPtr = ::MapViewOfFile(mMappedFilePtr, FILE_MAP_READ, offsetHigh, offsetLow, aNumBytesToMap);

   if (mMappedViewPtr == nullptr)
   {
      mMappedBytes   = 0;
      mMappedViewPtr = nullptr;
      return false;
   }

   return true;

#else

   // Linux
   // new mapping
   mMappedViewPtr = ::mmap64(nullptr, aNumBytesToMap, PROT_READ, MAP_SHARED, mFileHandle, aOffset);
   if (mMappedViewPtr == MAP_FAILED)
   {
      mMappedBytes   = 0;
      mMappedViewPtr = nullptr;
      return false;
   }

   mMappedBytes = aNumBytesToMap;

   // tweak performance
   int linuxHint = 0;
   switch (mHint)
   {
   case CacheHint::Normal:
      linuxHint = MADV_NORMAL;
      break;
   case CacheHint::SequentialScan:
      linuxHint = MADV_SEQUENTIAL;
      break;
   case CacheHint::RandomAccess:
      linuxHint = MADV_RANDOM;
      break;
   default:
      break;
   }
   // assume that file will be accessed soon,
   // meaning the kernal may load pages ahead of time.
   // linuxHint |= MADV_WILLNEED;
   // assume large chunks of the file will be accessed at a time.
   // normally this means changing page size from 4 KB to 2 MB.
   // linuxHint |= MADV_HUGEPAGE;

   ::madvise(mMappedViewPtr, mMappedBytes, linuxHint);

   return true;
#endif
}

// ==========================================================================
// ==========================================================================

UtMemMapWriter::UtMemMapWriter(const std::string& aFilename, size_t aSizeOfFileToCreate)
   : mFileSize(aSizeOfFileToCreate)
{
   CreateFileMap(aFilename, aSizeOfFileToCreate);
}

UtMemMapWriter::UtMemMapWriter(UtMemMapWriter&& aMov) noexcept
   : mFileSize(aMov.mFileSize)
#ifdef _WIN32
   , mMappedFilePtr(aMov.mMappedFilePtr)
#endif
   , mFileHandle(aMov.mFileHandle)
   , mMappedViewPtr(aMov.mMappedViewPtr)
{
   aMov.mFileSize = 0;
#ifdef _WIN32
   aMov.mMappedFilePtr = nullptr;
#endif
   aMov.mFileHandle    = 0;
   aMov.mMappedViewPtr = nullptr;
}

UtMemMapWriter& UtMemMapWriter::operator=(UtMemMapWriter&& aMov) noexcept
{
   if (&aMov == this)
   {
      return *this;
   }

   mFileSize = aMov.mFileSize;
#ifdef _WIN32
   mMappedFilePtr = aMov.mMappedFilePtr;
#endif
   mFileHandle    = aMov.mFileHandle;
   mMappedViewPtr = aMov.mMappedViewPtr;


   aMov.mFileSize = 0;
#ifdef _WIN32
   aMov.mMappedFilePtr = nullptr;
#endif
   aMov.mFileHandle    = 0;
   aMov.mMappedViewPtr = nullptr;
   return *this;
}

/// close file (see Close() )
UtMemMapWriter::~UtMemMapWriter()
{
   Close();
}

/// open file
bool UtMemMapWriter::CreateFileMap(const std::string& aFilename, size_t aSizeOfFileToCreate)
{
   // already open ?
   if (IsValid())
   {
      return false;
   }

   mFileHandle = 0;
   mFileSize   = 0;
#ifdef _WIN32
   mMappedFilePtr = nullptr;
#endif
   mMappedViewPtr = nullptr;
#ifdef _WIN32
   // Windows

   // open file
   // read write acces
   mFileHandle =
      ::CreateFileA(aFilename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
   if (mFileHandle == nullptr || mFileHandle == INVALID_HANDLE_VALUE)
   {
      mFileHandle = nullptr;
      return false;
   }

   // convert to mapped mode
   DWORD sizeLow  = static_cast<DWORD>(aSizeOfFileToCreate & 0xFFFFFFFF);
   DWORD sizeHigh = static_cast<DWORD>(aSizeOfFileToCreate >> 32);

   mMappedFilePtr = ::CreateFileMapping(mFileHandle, nullptr, PAGE_READWRITE, sizeHigh, sizeLow, nullptr);
   if (mMappedFilePtr == nullptr || mMappedFilePtr == INVALID_HANDLE_VALUE)
   {
      ::CloseHandle(mFileHandle);
      mFileHandle    = nullptr;
      mMappedFilePtr = nullptr;
      return false;
   }


#else

   // Linux

   // open file
   mFileHandle = ::open(aFilename.c_str(), O_RDWR | O_CREAT | O_TRUNC | O_LARGEFILE, static_cast<mode_t>(0600));
   if (mFileHandle == -1)
   {
      mFileHandle = 0;
      return false;
   }

   // stretch the file size
   int result = lseek(mFileHandle, aSizeOfFileToCreate - 1, SEEK_SET);
   if (result == -1)
   {
      close(mFileHandle);
      mFileHandle = 0;
      return false;
   }

   // write to file to aplly stretch to the file.
   result = write(mFileHandle, "", 1);
   if (result != 1)
   {
      close(mFileHandle);
      mFileHandle = 0;
      return false;
   }

#endif
   mFileSize = aSizeOfFileToCreate;


#ifdef _WIN32
   // Windows

   // get memory address
   mMappedViewPtr = ::MapViewOfFile(mMappedFilePtr, FILE_MAP_WRITE, 0, 0, mFileSize);

   if (mMappedViewPtr == nullptr)
   {
      mMappedViewPtr = nullptr;
      return false;
   }

#else

   // Linux
   // new mapping
   mMappedViewPtr = ::mmap64(nullptr, mFileSize, PROT_WRITE, MAP_SHARED, mFileHandle, 0);
   if (mMappedViewPtr == MAP_FAILED)
   {
      mMappedViewPtr = nullptr;
      return false;
   }
#endif

   if (!mMappedViewPtr)
   {
      return false;
   }

   // everything's fine
   return true;
}


/// close file
void UtMemMapWriter::Close()
{
   // kill pointer
   if (mMappedViewPtr)
   {
#ifdef _WIN32
      ::FlushViewOfFile(mMappedViewPtr, mFileSize);
      ::UnmapViewOfFile(mMappedViewPtr);
#else
      ::munmap(mMappedViewPtr, mFileSize);
#endif
      mMappedViewPtr = nullptr;
   }

#ifdef _WIN32
   if (mMappedFilePtr)
   {
      ::CloseHandle(mMappedFilePtr);
      mMappedFilePtr = nullptr;
   }
#endif

   // close underlying file
   if (mFileHandle)
   {
#ifdef _WIN32
      ::CloseHandle(mFileHandle);
#else
      ::close(mFileHandle);
#endif
      mFileHandle = 0;
   }

   mFileSize = 0;
}

void UtMemMapWriter::Flush()
{
   if (mMappedViewPtr)
   {
#ifdef _WIN32
      ::FlushViewOfFile(mMappedViewPtr, mFileSize);
#else
      ::msync(mMappedViewPtr, mFileSize, MS_SYNC);
#endif
   }
}

void UtMemMapWriter::Write(size_t aOffset, const void* aData, size_t aNumBytes)
{
   memcpy(&GetData()[aOffset], aData, aNumBytes);
}

/// raw access
unsigned char* UtMemMapWriter::GetData()
{
   return static_cast<unsigned char*>(mMappedViewPtr);
}


/// true, if file successfully opened
bool UtMemMapWriter::IsValid() const
{
   return mMappedViewPtr != nullptr;
}


/// get file size
uint64_t UtMemMapWriter::FileSize() const
{
   return mFileSize;
}
