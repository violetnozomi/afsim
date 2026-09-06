// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "VaUtils.hpp"

#include <algorithm>
#include <stdio.h>

#include "UtColor.hpp"
#include "UtPathParser.hpp"
#include "UtRunEnvManager.hpp"
#include "UtStringUtil.hpp"
#include "UtoObjectManager.hpp"

namespace vespa
{
std::vector<std::string> VaUtils::mApplicationDataPathList;
UtPath                   VaUtils::mSitePath;
UtPath                   VaUtils::mAppDataPath;
UtPath                   VaUtils::mUserPath;

//! Converts an openGL RGBA value to an UtoColor object.
//! @param aColor RGBA color array (openGL standard color format)
UtoColor VaUtils::glToUtoColor(const float* aColor)
{
   return UtoColor((unsigned char)(aColor[0] * 255),
                   (unsigned char)(aColor[1] * 255),
                   (unsigned char)(aColor[2] * 255),
                   (unsigned char)(aColor[3] * 255));
}

// Converts an openGL UtColor object to an UtoColor object.
//! @param aColor RGBA UtColor object (openGL standard color format)
UtoColor VaUtils::glToUtoColor(const UtColor& aColor)
{
   return UtoColor((unsigned char)(aColor[0] * 255),
                   (unsigned char)(aColor[1] * 255),
                   (unsigned char)(aColor[2] * 255),
                   (unsigned char)(aColor[3] * 255));
}

// Converts an Uto color object to an UtColor object in the openGL color format.
// @param aColor RGBA UtoColor object (Uto color format).
UtColor VaUtils::UtoToGlColor(const UtoColor& aColor)
{
   return UtColor((float)aColor.RedF(), (float)aColor.GreenF(), (float)aColor.BlueF(), (float)aColor.AlphaF());
}

// this method searches the current, home, site, and versioned etc directories for the requested
// file.  If found, it will return the full pathed file name for the first hit.  If not found in any directory, it will
// return an empty string.
// static
void VaUtils::GetPathPrioritizedFilename(const std::string& aInputFilename,
                                         std::string&       aPathAndFilename,
                                         const std::string& aEnvironmentVariable,
                                         const std::string& aKeyString)
{
#ifdef _WIN32
   static const std::string slash = "\\";
#else
   static const std::string slash = "/";
#endif

   std::string cmeRelBase = UtRunEnvManager::GetCmeRelBase();
   if (cmeRelBase == "")
   {
      cmeRelBase = ".";
   }

   std::string filename;


   if (aEnvironmentVariable != "")
   {
      // Next check the AFNES_TYPES environment variable.
      const char* envVarPtr = ::getenv(aEnvironmentVariable.c_str());
      if (envVarPtr != nullptr)
      {
         filename = std::string(envVarPtr);
      }

      if (UtPathParser::FileExists(filename))
      {
         aPathAndFilename = filename;
         return;
      }
   }

   if (aKeyString != "")
   {
      // try the run directory / cme_data / aKeyString
      filename = "cme_data" + slash + aKeyString + slash + aInputFilename;
      if (UtPathParser::FileExists(filename))
      {
         aPathAndFilename = filename;
         return;
      }
   }
   // try the run directory
   if (UtPathParser::FileExists(aInputFilename))
   {
      aPathAndFilename = aInputFilename;
      return;
   }
   std::string homeDir = UtRunEnvManager::GetHomePath();
   if (homeDir != "")
   {
      if (aKeyString != "")
      {
         // Next try the users home directory / cme_data / aKeyString
         filename = homeDir + slash + "cme_data" + slash + aKeyString + slash + aInputFilename;
         if (UtPathParser::FileExists(filename))
         {
            aPathAndFilename = filename;
            return;
         }
      }
      // Next try the users home directory / cme_data
      filename = homeDir + slash + "cme_data" + slash + aInputFilename;
      if (UtPathParser::FileExists(filename))
      {
         aPathAndFilename = filename;
         return;
      }
      // Next try the users home director
      filename = homeDir + slash + aInputFilename;
      if (UtPathParser::FileExists(filename))
      {
         aPathAndFilename = filename;
         return;
      }
   }

   // Next try the site directory
   filename = cmeRelBase + slash + "site" + slash + aInputFilename;
   if (UtPathParser::FileExists(filename))
   {
      aPathAndFilename = filename;
      return;
   }

   // try application provided 'site' directories
   std::vector<std::string>::iterator it;
   for (it = mApplicationDataPathList.begin(); it != mApplicationDataPathList.end(); ++it)
   {
      filename = *it + slash + aInputFilename;
      if (UtPathParser::FileExists(filename))
      {
         aPathAndFilename = filename;
         return;
      }
   }

   if (aKeyString != "")
   {
      // try cme_rel_base / classified / aKeyString
      filename = cmeRelBase + slash + "classified" + slash + aKeyString + slash + aInputFilename;
      if (UtPathParser::FileExists(filename))
      {
         aPathAndFilename = filename;
         return;
      }

      // try cme_rel_base / aKeyString
      filename = cmeRelBase + slash + aKeyString + slash + aInputFilename;
      if (UtPathParser::FileExists(filename))
      {
         aPathAndFilename = filename;
         return;
      }
   }

   // And try the data folder / vespa
   filename = UtRunEnvManager::GetCmeFolder("data") + slash + "vespa" + slash + aInputFilename;
   if (UtPathParser::FileExists(filename))
   {
      aPathAndFilename = filename;
      return;
   }

   // And try the etc folder / vespa
   filename = UtRunEnvManager::GetCmeFolder("etc") + slash + "vespa" + slash + aInputFilename;
   if (UtPathParser::FileExists(filename))
   {
      aPathAndFilename = filename;
      return;
   }

   // And try the data folder
   filename = UtRunEnvManager::GetCmeFolder("data") + slash + aInputFilename;
   if (UtPathParser::FileExists(filename))
   {
      aPathAndFilename = filename;
      return;
   }

   // And finally try the etc folder
   filename = UtRunEnvManager::GetCmeFolder("etc") + slash + aInputFilename;
   if (UtPathParser::FileExists(filename))
   {
      aPathAndFilename = filename;
      return;
   }
   aPathAndFilename = "";
}

void VaUtils::AddApplicationDataPath(const std::string& aPath)
{
   mApplicationDataPathList.push_back(aPath);
}

void VaUtils::SetSitePath(const UtPath& aPath)
{
   mSitePath = aPath;
}

void VaUtils::SetAppDataPath(const UtPath& aPath)
{
   mAppDataPath = aPath;
}

void VaUtils::SetUserPath(const UtPath& aPath)
{
   mUserPath = aPath;
}

UtPath& VaUtils::GetSitePath()
{
   return mSitePath;
}

UtPath& VaUtils::GetAppDataPath()
{
   return mAppDataPath;
}

UtPath& VaUtils::GetUserPath()
{
   return mUserPath;
}

std::string VaUtils::GetAbsolutePathFromSource(const std::string& aRelativePath, const std::string& aSource)
{
   UtPath wd = UtPath::WorkingDirectory();
   if (aSource == "user")
   {
      VaUtils::GetUserPath().SetWorkingDirectory();
   }
   else if (aSource == "site")
   {
      VaUtils::GetSitePath().SetWorkingDirectory();
   }
   else if (aSource == "app")
   {
      VaUtils::GetAppDataPath().SetWorkingDirectory();
   }
   UtPath file(aRelativePath);

   file.MakeFullPath();
   wd.SetWorkingDirectory();
   return file.GetSystemPath();
}

// static
void VaUtils::Cleanup()
{
   UtoObjectManager::Instance()->DestroyAllObjects(true);
}

void VaUtils::HSV_ToRGB(const float aHSV[3], float aRGB[3])
{
   float h, s, v, f, p, q, t;
   int   i;

   h = aHSV[0];
   s = aHSV[1];
   v = aHSV[2];
   if (0 == s)
   {
      if (-1 == h)
      {
         aRGB[0] = v;
         aRGB[1] = v;
         aRGB[2] = v;
      }
   }
   else
   {
      if (360.f == h)
         h = 0.f;
      h = h / 60.f;
      i = (int)h;       // integer part
      f = h - (float)i; // fractional part
      p = v * (1.f - s);
      q = v * (1.f - (s * f));
      t = v * (1.f - (s * (1.f - f)));
      switch (i)
      {
      case 0:
         aRGB[0] = v;
         aRGB[1] = t;
         aRGB[2] = p;
         break;
      case 1:
         aRGB[0] = q;
         aRGB[1] = v;
         aRGB[2] = p;
         break;
      case 2:
         aRGB[0] = p;
         aRGB[1] = v;
         aRGB[2] = t;
         break;
      case 3:
         aRGB[0] = p;
         aRGB[1] = q;
         aRGB[2] = v;
         break;
      case 4:
         aRGB[0] = t;
         aRGB[1] = p;
         aRGB[2] = v;
         break;
      case 5:
         aRGB[0] = v;
         aRGB[1] = p;
         aRGB[2] = q;
         break;
      }
   }
}

} // end namespace vespa
