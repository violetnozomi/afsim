// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SOSM_Utility.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>

#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <direct.h>
#endif

#include "SOSM_Selector.hpp"

namespace
{
SOSM_Utility::SpectralPrintFormat sSpectralPrintFormat = SOSM_Utility::cSPF_WAVENUMBER;
}

// ================================================================================================
void SOSM_Utility::SetSpectralPrintFormat(SpectralPrintFormat aSpectralPrintFormat)
{
   sSpectralPrintFormat = aSpectralPrintFormat;
}

// ================================================================================================
//! Print a vector that represents the spectral product of something (target model, atmosphere, etc.)
void SOSM_Utility::PrintSpectralVector(ut::log::MessageStream&   aStream,
                                       const SOSM_Selector&      aSelector,
                                       const std::vector<float>& aVector,
                                       const std::string&        aHeader)
{
   auto out = aStream.AddNote() << aHeader << ":";
   if (sSpectralPrintFormat == cSPF_WAVENUMBER)
   {
      float maxWavenumber = aSelector.Origin() + ((aSelector.Count() - 1) * aSelector.Increment());
      auto  logLoop       = out.AddNote();
      for (int i = 0; i < aSelector.Count(); ++i)
      {
         if ((i % 7) == 0)
         {
            logLoop.Send();
            float lowerWavenumber = aSelector.Origin() + (i * aSelector.Increment());
            float upperWavenumber = aSelector.Origin() + ((i + 6) * aSelector.Increment());
            upperWavenumber       = std::min(upperWavenumber, maxWavenumber);
            logLoop << "Wave Numbers " << std::fixed;
            logLoop << std::setprecision(0) << lowerWavenumber << '-';
            logLoop << upperWavenumber << ':';
         }
         logLoop.AddNote() << std::scientific << std::setprecision(5) << aVector[i + aSelector.OutputStartIndex()];
      }
   }
   else if (sSpectralPrintFormat == cSPF_WAVELENGTH)
   {
      float minWavenumber = aSelector.Origin();
      auto  logLoop       = out.AddNote();
      for (int j = 0; j < aSelector.Count(); ++j)
      {
         int i = aSelector.Count() - j - 1;
         if ((j % 7) == 0)
         {
            logLoop.Send();
            float lowerWavenumber = aSelector.Origin() + ((i - 6) * aSelector.Increment());
            float upperWavenumber = aSelector.Origin() + (i * aSelector.Increment());
            lowerWavenumber       = std::max(lowerWavenumber, minWavenumber);
            float lowerWavelength = 10000.0F / upperWavenumber;
            float upperWavelength = 10000.0F / lowerWavenumber;
            logLoop << "Wave Lengths " << std::fixed << std::showpoint;
            logLoop << std::setprecision(4) << lowerWavelength << '-';
            logLoop << std::setprecision(4) << upperWavelength << ':';
            logLoop << std::noshowpoint;
         }
         logLoop.AddNote() << std::scientific << std::setprecision(5) << aVector[i + aSelector.OutputStartIndex()];
      }
   }
}

// =================================================================================================
//! A C++ equivalent of the Unix 'basename' command, without the option to remove the suffix.
std::string SOSM_Utility::BaseName(const std::string& aPath)
{
   std::string pathPrefix;
   std::string pathName;
   StripPathPrefix(aPath, pathPrefix, pathName);
   std::string baseName;

   // find the last '/' or '\'.
   while (true)
   {
      std::string::size_type lastSep = pathName.find_last_of("/\\");
      if (lastSep == std::string::npos)
      {
         // Bare word
         baseName = pathName;
         break;
      }
      else if ((lastSep + 1) < pathName.size())
      {
         // Last character is not a path separator
         baseName = pathName.substr(lastSep + 1);
         break;
      }
      else if ((lastSep == 0) && (pathName.size() == 1))
      {
         // Bare separator (root path)
         baseName = pathName;
         break;
      }
      // Trailing '/' - remove and repeat.
      pathName = pathName.substr(0, lastSep);
   }
   return baseName;
}

// =================================================================================================
//! A C++ equivalent of the Unix 'dirname'.
//! The returned value will NOT have a trailing '/'
std::string SOSM_Utility::DirName(const std::string& aPath)
{
   std::string pathPrefix;
   std::string pathName;
   StripPathPrefix(aPath, pathPrefix, pathName);
   std::string dirName;
   while (true)
   {
      std::string::size_type lastSep = pathName.find_last_of("/\\");
      if (lastSep == std::string::npos)
      {
         // Bare word
         dirName = ".";
         break;
      }
      else if (lastSep == 0)
      {
         dirName = pathName[lastSep];
         break;
      }
      else if ((lastSep + 1) < pathName.size())
      {
         // Last character is not a path separator
         dirName = pathName.substr(0, lastSep);
         break;
      }
      // Trailing '/' - remove and repeat.
      pathName = pathName.substr(0, lastSep);
   }
   if ((dirName == ".") && (!pathPrefix.empty()))
   {
      dirName = pathPrefix;
   }
   else
   {
      dirName = pathPrefix + dirName;
   }
   return dirName;
}

// =================================================================================================
//! Strip off Windows/DOS path prefix data.
void SOSM_Utility::StripPathPrefix(const std::string& aPrefixedPath, std::string& aPrefix, std::string& aPath)
{
   aPrefix = "";
   aPath   = aPrefixedPath;
#if _WIN32
   if ((aPrefixedPath.size() > 3) && ((aPrefixedPath[0] == '\\') || (aPrefixedPath[0] == '/')) &&
       (aPrefixedPath[1] == aPrefixedPath[0]))
   {
      // "\\server\volume\..." (Window network share or UNC format)

      std::string::size_type serverEnd = aPrefixedPath.find_first_of("\\/", 2);
      if ((serverEnd != std::string::npos) && ((serverEnd + 1) != aPrefixedPath.size()))
      {
         std::string::size_type volumeEnd = aPrefixedPath.find_first_of("\\/", serverEnd + 1);
         if (volumeEnd != std::string::npos)
         {
            aPrefix = aPrefixedPath.substr(0, volumeEnd + 1);
            if ((volumeEnd + 1) < aPrefixedPath.size())
            {
               aPath = aPrefixedPath.substr(volumeEnd + 1);
            }
            else
            {
               aPath = ".";
            }
         }
         else
         {
            aPrefix = aPrefixedPath;
            aPath   = ".";
         }
      }
   }
   else if ((aPrefixedPath.size() >= 2) && (aPrefixedPath[1] == ':'))
   {
      aPrefix = aPrefixedPath.substr(0, 2);
      if (aPrefixedPath.size() > 2)
      {
         aPath = aPrefixedPath.substr(2);
      }
   }
#endif
}

// =================================================================================================
//! Like Unix 'mkdir -p'
bool SOSM_Utility::MakePath(const std::string& aPath)
{
   struct stat            statBuf;
   std::list<std::string> dirStack;
   std::string            curPath(aPath);

   // An empty path is treated as '.', which should exist.
   if (aPath.empty())
   {
      return true;
   }

   // Build the stack of directories that need to be created.

   while (true)
   {
      if (stat(curPath.c_str(), &statBuf) == 0)
      {
         if ((statBuf.st_mode & S_IFDIR) == 0)
         {
            return false; // found a file in the path...
         }
         break;
      }
      dirStack.push_front(curPath);
      std::string nextPath = SOSM_Utility::DirName(curPath);
      if (curPath == nextPath)
      {
         break;
      }
      curPath = nextPath;
   }

   bool ok = true;
   for (std::list<std::string>::const_iterator dsi = dirStack.begin(); dsi != dirStack.end(); ++dsi)
   {
      if (!CreateDirectory(*dsi))
      {
         ok = false;
         break;
      }
   }
   return ok;
}

// =================================================================================================
bool SOSM_Utility::CreateDirectory(const std::string& aDirName)
{
   bool        created = false;
   struct stat statBuf;

   if (stat(aDirName.c_str(), &statBuf) == 0)
   {
      created = ((statBuf.st_mode & S_IFDIR) != 0);
   }
   else
   {
#ifdef _WIN32
      if (_mkdir(aDirName.c_str()) == 0)
#else
      if (mkdir(aDirName.c_str(), 0777) == 0)
#endif
      {
         created = true;
      }
   }
   return created;
}

// =================================================================================================
bool SOSM_Utility::DirectoryExists(const std::string& aDirName)
{
   bool        exists = false;
   struct stat statBuf;

   if (stat(aDirName.c_str(), &statBuf) == 0)
   {
      exists = ((statBuf.st_mode & S_IFDIR) != 0);
   }
   return exists;
}

// =================================================================================================
bool SOSM_Utility::FileExists(const std::string& aFileName)
{
   bool        exists = false;
   struct stat statBuf;

   if (stat(aFileName.c_str(), &statBuf) == 0)
   {
      exists = ((statBuf.st_mode & S_IFREG) != 0);
   }
   return exists;
}

// =================================================================================================
//! Is the target file current with respect to the source file?
//!
//! This is similar to the check performed by the 'make' utility. A target file is considered current with
//! respect to the source if the target and source files exist and the modification date of the target is
//! newer than that of the source.
//!
//! @param aTargetFile The file to be tested to see if it is 'current'.
//! @param aSourceFile The reference (source) file to be used the comparison.
//! @returns true if aTargetFile and aSourceFile exist and the modification data of the target file is newer
//! that the modification data of the source file.
bool SOSM_Utility::FileIsCurrent(const std::string& aTargetFile, const std::string& aSourceFile)
{
   bool        isCurrent = false;
   struct stat sourceStatBuf;
   struct stat targetStatBuf;

   if (stat(aTargetFile.c_str(), &targetStatBuf) == 0)
   {
      if (stat(aSourceFile.c_str(), &sourceStatBuf) == 0)
      {
         isCurrent = (targetStatBuf.st_mtime >= sourceStatBuf.st_mtime);
      }
   }
   return isCurrent;
}

// =================================================================================================
//! Return the timestamp when the indicated file was last modified.
//! @returns The timestamp of when the file was last modified. A negative value is returned if the
//!          file does not exist.
double SOSM_Utility::FileModificationTime(const std::string& aFile)
{
   double      modificationTime = -1.0;
   struct stat statBuf;

   if (stat(aFile.c_str(), &statBuf) == 0)
   {
      modificationTime = static_cast<double>(statBuf.st_mtime);
   }
   return modificationTime;
}
