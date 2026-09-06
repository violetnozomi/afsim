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

#ifndef UTSCANDIR_HPP
#define UTSCANDIR_HPP

#include "ut_export.h"

#include <string>
#include <vector>

#if defined(__GNUC__) || defined(sgi) || defined(sun)
struct dirent;
#endif

class UT_EXPORT UtScanDir
{
public:
   struct DirEntry
   {
      DirEntry()
         : mFilename()
         , mDirectory(false)
      {
      }

      DirEntry(const std::string& aFilename, bool aDirectory)
         : mFilename(aFilename)
         , mDirectory(aDirectory)
      {
      }

      bool operator==(const DirEntry& aRhs) const { return mFilename == aRhs.mFilename; }

      bool operator<(const DirEntry& aRhs) const { return mFilename < aRhs.mFilename; }

      std::string mFilename;
      bool        mDirectory;
   };

   using DirEntries = std::vector<DirEntry>;
   using StringList = std::vector<std::string>;

   struct Filter
   {
      StringList mPatternParts;
      size_t     mMinCharacters;
   };

   static int ScanDir(const std::string& aDirName, DirEntries& aDirEntries, const std::string& aFilter = "*");

   static int  ScanDirRecursive(const std::string& aDirName,
                                DirEntries&        aDirEntries,
                                const std::string& aFilter,
                                int                aMaxDepth = 1000);
   static char GetSlashChar();

   static void Test();

private:
#if defined(__GNUC__) || defined(sgi) || defined(sun)
   static int PatternMatch(dirent* aDirEnt);
   static int PatternMatch(const dirent* aDirEnt);
#endif

   static void CreateFilter(const std::string& aPattern, Filter& aFilter);

   static bool PatternMatch(const std::string& aFileName, Filter& aFilter);
};

#endif
