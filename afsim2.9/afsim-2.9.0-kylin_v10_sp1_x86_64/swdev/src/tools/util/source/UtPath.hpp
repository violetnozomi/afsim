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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#ifndef UTPATH_HPP
#define UTPATH_HPP

#include "ut_export.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

//! Encapsulates a file or directory path.
class UT_EXPORT UtPath
{
public:
   // Points to a part of another string.
   // Using this instead of std::string is more efficient
   struct Substring
   {
      Substring(const char* a, const char* b)
         : begin(a)
         , end(b)
      {
      }
      Substring(const char* aStrPtr)
      {
         begin = aStrPtr;
         end   = begin + strlen(aStrPtr);
      }
      Substring()
         : begin(nullptr)
         , end(nullptr)
      {
      }
      size_t length() const { return end - begin; }
      char   operator[](int offset) const { return begin[offset]; }
      bool   operator==(const Substring& aRhs) const
      {
         return length() == aRhs.length() && std::equal(begin, end, aRhs.begin);
      }
      bool        operator!=(const Substring& aRhs) const { return !operator==(aRhs); }
      const char* begin;
      const char* end;
   };
   using SubstringList = std::vector<Substring>;

   static UtPath WorkingDirectory();
   static UtPath GetExePath();

   UtPath(const std::string& aPath);
   UtPath() = default;
   UtPath& operator+=(const UtPath& aPath);
   UtPath& operator+=(const std::string& aPath) { return operator+=(UtPath(aPath)); }
   UtPath  operator+(const UtPath& aPath) const;
   UtPath  operator+(const std::string& aPath) const { return operator+(UtPath(aPath)); }

   bool               SetWorkingDirectory() const;
   const std::string& GetNormalizedPath() const { return mPathString; }
   std::string        GetSystemPath() const;
   std::string        GetFileName(bool aWithExtension = true) const;
   std::string        GetFileExtension() const;
   bool               Mkdir(bool aCreateAll = true) const;
   bool               RemoveDir() const;
   bool               RemoveTree() const;
   bool               RemoveFile() const;

   bool   Rename(const std::string& aNewName) const;
   UtPath GetRelativePath(const UtPath& aReferencedDirectory) const;
   bool   IsAncestorOf(const UtPath& aOtherPath) const;
   void   MakeFullPath(const UtPath& aReferencePath = UtPath());
   UtPath GetRealPath() const;
   void   ParsePath(std::string& aPrefix, SubstringList& aDirectoryStack) const;

   bool ListDir(std::vector<std::string>& aFiles, std::vector<std::string>& aDirectories) const;

   bool operator==(const UtPath& aPath) const { return mPathString == aPath.mPathString; }
   bool operator!=(const UtPath& aPath) const { return mPathString != aPath.mPathString; }
   bool operator<(const UtPath& aRhs) const { return mPathString < aRhs.mPathString; }

   //! @brief modify the path to refer to the parent path
   //!
   //! @return the updated UtPath object (enables function chaining)
   UtPath& Up();

   enum StatType
   {
      cSTAT_ERROR,
      cFILE,
      cDIRECTORY
   };

   enum PermissionBits
   {
      cPB_OWNER_READ    = 1,
      cPB_OWNER_WRITE   = 2,
      cPB_OWNER_EXECUTE = 4
   };

   struct StatData
   {
      StatType       mStatType;
      size_t         mFileSizeBytes;
      time_t         mModifiedDate;
      unsigned short mPermissionBits;
   };

   StatType Stat() const;
   bool     Stat(StatData& aData) const;

   bool IsNull() const { return mPathString.empty(); }

   bool IsFile() const { return Stat() == cFILE; }

   bool IsDirectory() const { return Stat() == cDIRECTORY; }

   bool Exists() const { return Stat() != cSTAT_ERROR; }

   void Swap(UtPath& aRhs) noexcept { std::swap(mPathString, aRhs.mPathString); }
   void Tokenize(std::string& aPrefix, std::vector<std::string>& aTokens) const;

private:
   static void MakeNormalizedPath(std::string& aPath);

   static void CleanDirectoryStack(SubstringList& aDirStack);

   static void ParsePathP(const std::string& aPathString, std::string& aPrefix, SubstringList& aDirectoryStack);

   static std::string BuildPath(const std::string& aPrefix, SubstringList& aDirectoryStack);

   static void NormalizeSlashes(std::string& aPath);

   std::string mPathString;
};

namespace std
{

template<>
struct hash<UtPath>
{
   std::size_t operator()(const UtPath& aRhs) const noexcept
   {
      return std::hash<std::string>{}(aRhs.GetNormalizedPath());
   }
};

inline void swap(UtPath& aLhs, UtPath& aRhs) noexcept
{
   aLhs.Swap(aRhs);
}

} // namespace std

#endif
