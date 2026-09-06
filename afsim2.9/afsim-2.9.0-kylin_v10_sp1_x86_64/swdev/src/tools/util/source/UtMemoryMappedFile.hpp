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

#ifndef UTMEMORYMAPPEDFILE_HPP
#define UTMEMORYMAPPEDFILE_HPP

#include "ut_export.h"

#include <cstdint> // define fixed size integer types
#include <string>

namespace UtMemMap
{
/// hint to operating system to tweak performance
/// of page loading/unloading.
enum class CacheHint
{
   Normal,         ///< good overall performance
   SequentialScan, ///< read file only once with few seeks
   RandomAccess    ///< unpredictable jumps around
};

constexpr size_t MapWholeFile = 0; ///< map everything ... be careful when file is larger than size_t

/// get OS page size (for remap of mem-mapped files)
UT_EXPORT int GetPageSize();
} // namespace UtMemMap
//! Portable read-only memory mapping (Windows and Linux)
//! Filesize limited by size_t, usually 2^32 or 2^64.
//!
//! Memory mapping is used to map a file into RAM without actually reading the file.
//! The operating system will take care of reading individual pages from the file as
//! they are accessed.
//! This allows large files to be used as data tables without suffering system memory/performance issues
//! of reading in entire files.
//!
//! see also: https://en.wikipedia.org/wiki/Memory-mapped_file
class UT_EXPORT UtMemoryMappedFile
{
public:
   using CacheHint = UtMemMap::CacheHint;

   /// do nothing, must use Open()
   UtMemoryMappedFile() = default;
   /// open file, mappedBytes = 0 maps the whole file
   UtMemoryMappedFile(const std::string& aFilename,
                      size_t             aNumBytesToMap = UtMemMap::MapWholeFile,
                      CacheHint          aHint          = CacheHint::Normal);
   /// don't copy object
   UtMemoryMappedFile(const UtMemoryMappedFile&) = delete;

   UtMemoryMappedFile(UtMemoryMappedFile&& aMov) noexcept;

   /// don't copy object
   UtMemoryMappedFile& operator=(const UtMemoryMappedFile&) = delete;

   UtMemoryMappedFile& operator=(UtMemoryMappedFile&& aMov) noexcept;

   /// close file (see Close() )
   ~UtMemoryMappedFile();

   /// open file, mappedBytes = 0 maps the whole file
   bool Open(const std::string& aFilename,
             size_t             aNumBytesToMap = UtMemMap::MapWholeFile,
             CacheHint          aHint          = CacheHint::Normal);
   /// close file
   void Close();

   /// access position, no range checking (faster)
   unsigned char operator[](size_t aOffset) const;
   /// access position, including range checking
   unsigned char At(size_t aOffset) const;

   /// raw access
   const unsigned char* GetData() const;

   /// true, if file successfully opened and mapped
   bool IsValid() const;

   /// get file size
   uint64_t FileSize() const;
   /// get number of actually mapped bytes
   size_t MappedSize() const;

   /// replace mapping by a new one of the same file, aOffset MUST be a multiple of the page size
   bool ReMap(uint64_t aOffset, size_t aNumBytesToMap);

private:
   /// file size
   uint64_t mFileSize{0};
   /// caching strategy
   CacheHint mHint{CacheHint::Normal};
   /// mapped size
   size_t mMappedBytes{0};

/// define handle
#ifdef _WIN32
   using FileHandle = void*;
   /// Windows handle to memory mapping of mFileHandle
   void* mMappedFilePtr{nullptr};
#else
   using FileHandle = int;
#endif

   /// file handle
   FileHandle mFileHandle{};
   /// pointer to the file contents mapped into memory
   void* mMappedViewPtr{nullptr};
};

//! Portable write-only memory mapping (Windows and Linux)
//! Filesize limited by size_t, usually 2^32 or 2^64.
//!
//! see also: https://en.wikipedia.org/wiki/Memory-mapped_file
class UT_EXPORT UtMemMapWriter
{
public:
   /// do nothing, must use Open()
   UtMemMapWriter() = default;
   /// open file, mappedBytes = 0 maps the whole file
   UtMemMapWriter(const std::string& aFilename, size_t aSizeOfFileToCreate);
   /// don't copy object
   UtMemMapWriter(const UtMemMapWriter&) = delete;

   UtMemMapWriter(UtMemMapWriter&& aMov) noexcept;

   /// don't copy object
   UtMemMapWriter& operator=(const UtMemMapWriter&) = delete;

   UtMemMapWriter& operator=(UtMemMapWriter&& aMov) noexcept;

   /// close file (see Close() )
   ~UtMemMapWriter();

   /// open file, mappedBytes = 0 maps the whole file
   bool CreateFileMap(const std::string& aFilename, size_t aSizeOfFileToCreate);
   /// close file
   void Close();

   /// write data, no range checking
   void Write(size_t aOffset, const void* aData, size_t aNumBytes);

   /// blocking call that flushes all writes back to disk.
   void Flush();

   /// raw access
   unsigned char* GetData();

   /// true, if file successfully opened and mapped
   bool IsValid() const;

   /// get file size
   uint64_t FileSize() const;


private:
   /// file size
   uint64_t mFileSize{0};

/// define handle
#ifdef _WIN32
   using FileHandle = void*;
   /// Windows handle to memory mapping of mFileHandle
   void* mMappedFilePtr{nullptr};
#else
   using FileHandle = int;
#endif

   /// file handle
   FileHandle mFileHandle{};
   /// pointer to the file contents mapped into memory
   void* mMappedViewPtr{nullptr};
};
#endif
