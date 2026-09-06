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

#include "UtScanDir.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <cstdio>
#include <cstdlib>

#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
namespace
{
// static variable required for nix scandir()
UtScanDir::Filter* sCurrentFilter(nullptr);
} // namespace
#endif

// static
int UtScanDir::ScanDir(const std::string& aDirName, DirEntries& aDirEntries, const std::string& aFilter /* = "*"*/)
{
   aDirEntries.clear();

#if defined(__GNUC__) || defined(sgi) || defined(sun)
   sCurrentFilter = new Filter();
   CreateFilter(aFilter, *sCurrentFilter);
   struct stat st;
   dirent**    dirList  = nullptr;
   const int   dirCount = scandir(aDirName.c_str(), &dirList, PatternMatch, nullptr);
   for (int i = 0; i < dirCount; ++i)
   {
      char path[256];
      if (stat(strcat(strcpy(path, std::string(aDirName + GetSlashChar()).c_str()), dirList[i]->d_name), &st) != -1)
      {
         aDirEntries.push_back(DirEntry(dirList[i]->d_name, S_ISDIR(st.st_mode)));
      }
      free(dirList[i]);
   }
   free(dirList);
   delete sCurrentFilter;
   sCurrentFilter = nullptr;
#else
   WIN32_FIND_DATA wfd;
   HANDLE          hf;
   std::string     path;

   path        = aDirName;
   const int s = (int)aDirName.size();
   if ((s > 0) && (aDirName[s - 1] != '\\') && (aDirName[s - 1] != '/'))
   {
      path += "\\";
   }
   path += "*.*";

   Filter filter;
   CreateFilter(aFilter, filter);
   hf = FindFirstFile(path.c_str(), &wfd);
   do
   {
      if (hf == INVALID_HANDLE_VALUE)
      {
         return -1;
      }

      if (PatternMatch(wfd.cFileName, filter))
      {
         aDirEntries.push_back(DirEntry(wfd.cFileName, (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0));
      }
   } while (FindNextFile(hf, &wfd));

   FindClose(hf);
#endif

   // Sort the array.
   sort(aDirEntries.begin(), aDirEntries.end());

   return (int)aDirEntries.size();
}

// static
int UtScanDir::ScanDirRecursive(const std::string& aDirName, DirEntries& aDirEntries, const std::string& aFilter, int aMaxDepth)
{
   char separator = '/';
#ifdef _WIN32
   separator = '\\';
#endif
   DirEntries filterEntries;
   ScanDir(aDirName, filterEntries, aFilter);
   for (size_t i = 0; i < filterEntries.size(); ++i)
   {
      filterEntries[i].mFilename = aDirName + separator + filterEntries[i].mFilename;
      aDirEntries.push_back(filterEntries[i]);
   }
   DirEntries subdirs;
   ScanDir(aDirName, subdirs, "*");

   if (aMaxDepth >= 0)
   {
      for (size_t i = 0; i < subdirs.size(); ++i)
      {
         if (subdirs[i].mDirectory && subdirs[i].mFilename[0] != '.')
         {
            ScanDirRecursive(aDirName + separator + subdirs[i].mFilename, aDirEntries, aFilter, aMaxDepth - 1);
         }
      }
   }
   return (int)aDirEntries.size();
}

//! Break the search pattern into a token list which is separated by *'s
//! Return the minimum number of characters a match must have
void UtScanDir::CreateFilter(const std::string& aPattern, Filter& aFilter)
{
   aFilter.mMinCharacters = 0;
   std::string pattern    = aPattern;
   if (pattern.empty())
   {
      pattern = "*";
   }

   while (!pattern.empty())
   {
      size_t offset = pattern.find('*');
      if (offset == std::string::npos)
      {
         aFilter.mPatternParts.push_back(pattern);
         aFilter.mMinCharacters += pattern.size();
         pattern.erase(0, std::string::npos);
      }
      else
      {
         aFilter.mPatternParts.push_back(pattern.substr(0, offset));
         aFilter.mMinCharacters += offset;
         pattern.erase(0, offset + 1);
         if (pattern.empty())
         {
            aFilter.mPatternParts.push_back("");
         }
      }
   }
}

namespace
{
struct WildcardComparor
{
   bool operator()(char a, char b) const { return a == b || a == '?' || b == '?'; }
};
} // namespace

//! Return true if the pattern matches the pattern.
bool UtScanDir::PatternMatch(const std::string& aFileName, Filter& aFilter)
{
   if (aFilter.mPatternParts.size() == 1)
   {
      return aFileName == aFilter.mPatternParts[0];
   }
   if (aFilter.mMinCharacters > aFileName.size())
   {
      return false;
   }
   const std::string& first     = aFilter.mPatternParts[0];
   const std::string& last      = aFilter.mPatternParts[aFilter.mPatternParts.size() - 1];
   int                firstSize = static_cast<int>(first.size());
   int                lastSize  = static_cast<int>(last.size());

   bool match = false;
   // Check beginning and end of file name
   if (std::equal(first.begin(), first.begin() + firstSize, aFileName.begin(), WildcardComparor()) &&
       std::equal(last.begin(), last.begin() + lastSize, aFileName.end() - last.size(), WildcardComparor()))
   {
      std::string           nameRemaining = aFileName.substr(firstSize, aFileName.length() - firstSize - lastSize);
      std::string::iterator curr          = nameRemaining.begin();

      // Search for all tokens separated by *'s
      for (size_t i = 1; i < aFilter.mPatternParts.size() - 1; ++i)
      {
         const std::string& part = aFilter.mPatternParts[i];
         curr                    = std::search(curr, nameRemaining.end(), part.begin(), part.end(), WildcardComparor());
         if (curr == nameRemaining.end())
         {
            return false;
         }
         curr += part.size();
      }
      return true;
   }
   return match;
}

#if defined(__GNUC__) || defined(sgi) || defined(sun)
// static private
int UtScanDir::PatternMatch(dirent* aDirEnt)
{
   int success = 0;
   if (PatternMatch(aDirEnt->d_name, *sCurrentFilter))
   {
      success = true;
   }
   return success;
}

// static private
int UtScanDir::PatternMatch(const dirent* aDirEnt)
{
   int success = 0;
   if (PatternMatch(aDirEnt->d_name, *sCurrentFilter))
   {
      success = true;
   }
   return success;
}
#endif

char UtScanDir::GetSlashChar()
{
#ifdef _WIN32
   return '\\';
#else
   return '/';
#endif
}
