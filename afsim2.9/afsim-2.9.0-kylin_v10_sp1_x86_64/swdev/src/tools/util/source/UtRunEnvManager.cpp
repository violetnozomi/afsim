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

///////////////////////////////////////////////////////////////////////////////////////////
// Ut_RunEnvManager.cpp - Implementation of the Ut_RunEnvManager class
///////////////////////////////////////////////////////////////////////////////////////////

#include "UtRunEnvManager.hpp"

#include <climits>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "UtLog.hpp"
#include "UtPath.hpp"
#include "UtPathParser.hpp"

namespace // {anonymous}
{
bool        sSetupFlag = false;
std::string sHomePath;
std::string sRunPath;
std::string sUsername;
std::string sVersionNumber;
std::string mholdS;
std::string sCmeRelBase;
std::string sCmeRelSuffix;

//--------------------------------------------------------------------
// Determines the username based off of environment variables.
//--------------------------------------------------------------------
void SetUsername(std::string& aUsername)
{
#if defined(_WIN32)
   std::string env = "USERNAME";
#else
   std::string env = "USER";
#endif

   if (::getenv(env.c_str()) != nullptr)
   {
      aUsername = ::getenv(env.c_str());
   }
   else
   {
      aUsername = "Unknown";
   }
}

//--------------------------------------------------------------------
// Determines the run path based on argv[0]
//--------------------------------------------------------------------
void SetRunPath(std::string& aRunPath, const char* /*aArgvZero*/)
{
   UtPath exePath = UtPath::GetExePath();
   exePath.Up();
   aRunPath = exePath.GetSystemPath();
}

//--------------------------------------------------------------------
// Determines the home path based on environment variables.
//--------------------------------------------------------------------
void SetHomePath(std::string& aHomePath)
{
#if defined(_WIN32)
   if (getenv("HOMEDRIVE") != nullptr)
   {
      aHomePath = getenv("HOMEDRIVE");
      aHomePath += getenv("HOMEPATH");
   }
   else
   {
      aHomePath = "";
   }

#else
   if (getenv("HOME"))
   {
      aHomePath = getenv("HOME");
   }
   else
   {
      aHomePath = "";
   }
#endif
}

//--------------------------------------------------------------------
// Determines the location of the CME base directory and returns it.
//--------------------------------------------------------------------
void SetCmeRelBase(std::string& aCmeRelBase)
{
   // If CME_REL_BASE_DEVELOPER is defined, use it.
   const char* cmeRelBase1 = ::getenv("CME_REL_BASE_DEVELOPER");

   if (cmeRelBase1 != nullptr)
   {
      aCmeRelBase = cmeRelBase1;
   }
   else
   {
      // Since CME_REL_BASE_DEVELOPER is not defined, determine where
      //  it is based on the run path.
      aCmeRelBase = UtRunEnvManager::GetRunPath();

      // Strip off one directory level.
      aCmeRelBase = UtPathParser::GetPath(aCmeRelBase);
   }
}

//--------------------------------------------------------------------
// Determines the CME relative suffix (ex .v3.53) and returns it.
//--------------------------------------------------------------------
void SetCmeRelSuffix(std::string& aCmeRelSuffix)
{
   // If CME_REL_SUFFIX_DEVELOPER is defined, use it.
   const char* cmeRelSuffix = ::getenv("CME_REL_SUFFIX_DEVELOPER");
   if (cmeRelSuffix != nullptr)
   {
      aCmeRelSuffix = cmeRelSuffix;
   }
   else
   {
      // Since CME_REL_SUFFIX_DEVELOPER is not defined, determine it based
      //  on the run directory.
      aCmeRelSuffix = UtRunEnvManager::GetRunPath();

      // Remove the trailing "/" or "\" so UtPathParser will treat the
      // CME_REL_BASE_DEVELOPER/bin.vx.yz as filename.
      if (aCmeRelSuffix[aCmeRelSuffix.length() - 1] == '/' || aCmeRelSuffix[aCmeRelSuffix.length() - 1] == '\\')
      {
         aCmeRelSuffix.erase(aCmeRelSuffix.length() - 1);
      }

      // Get the extension on the run directory, which should
      //  be of the form bin.v3.53
      aCmeRelSuffix = UtPathParser::GetExtension(aCmeRelSuffix);
   }
}

//--------------------------------------------------------------------
// Determines the version (ex 3.53) based on the CME relative suffix.
//--------------------------------------------------------------------
void SetVersionNumber(std::string& aVersion)
{
   if (!sCmeRelSuffix.empty())
   {
      size_t pos = sCmeRelSuffix.find("v");
      if (pos != std::string::npos)
      {
         aVersion = sCmeRelSuffix.substr(pos + 1);
      }
   }
   else
   {
      aVersion = "";
   }
}

} // namespace

// This method determines the version and runpath by the arguments, and username and
// homepath by Environment variables;
void UtRunEnvManager::Setup(const char* aArgvZero, const char* aCME_REL_BASE)
{
   sSetupFlag = true;

   SetUsername(sUsername);

   SetRunPath(sRunPath, aArgvZero);

   SetHomePath(sHomePath);

   if (aCME_REL_BASE != nullptr)
   {
      sCmeRelBase = aCME_REL_BASE;
   }
   else
   {
      SetCmeRelBase(sCmeRelBase);
   }

   SetCmeRelSuffix(sCmeRelSuffix);

   // SetVersionNumber must happen after SetCmeRelSuffix, because
   // it uses sCmeRelSuffix to determine the version.
   SetVersionNumber(sVersionNumber);
}

// This method returns the homepath
const std::string& UtRunEnvManager::GetHomePath()
{
   if (!sSetupFlag)
   {
      Setup(nullptr);
   }
   return sHomePath;
}

// This method returns the runpath
const std::string& UtRunEnvManager::GetRunPath()
{
   if (!sSetupFlag)
   {
      Setup(nullptr);
   }
   return sRunPath;
}

// This method returns the version number
const std::string& UtRunEnvManager::GetVersionNumber()
{
   if (!sSetupFlag)
   {
      Setup(nullptr);
   }
   return sVersionNumber;
}

// This method returns the username
const std::string& UtRunEnvManager::GetUsername()
{
   if (!sSetupFlag)
   {
      Setup(nullptr);
   }
   return sUsername;
}

const std::string& UtRunEnvManager::GetCmeRelBase()
{
   // If sCmeRelBase hasn't been set, it means Setup hasn't been called.
   if (!sSetupFlag)
   {
      Setup(nullptr);
   }
   return sCmeRelBase;
}

const std::string& UtRunEnvManager::GetCmeRelSuffix()
{
   if (!sSetupFlag)
   {
      Setup(nullptr);
   }
   return sCmeRelSuffix;
}

std::string UtRunEnvManager::GetCmeFolder(const std::string& aFolderName)
{
   std::string cmeFolder = GetCmeRelBase() + GetPathSeparator() + aFolderName + GetCmeRelSuffix();
   return cmeFolder;
}

const std::string& UtRunEnvManager::GetPathSeparator()
{
#ifdef _WIN32
   static const std::string separator = "\\";
#else
   static const std::string separator = "/";
#endif
   return separator;
}

//#define TESTING_UT_CME_ENV_VAR

#ifdef TESTING_UT_CME_ENV_VAR
#include <iostream>

int main(int argc, char* argv[])
{
   UtRunEnvManager::Setup(argv[0]);
   auto out = ut::log::info() << "TESTING_UT_CME_ENV_VAR";
   out.AddNote() << "User name: " << UtRunEnvManager::GetUsername();
   out.AddNote() << "Run path: " << UtRunEnvManager::GetRunPath();
   out.AddNote() << "Home path: " << UtRunEnvManager::GetHomePath();
   out.AddNote() << "Version: " << UtRunEnvManager::GetVersionNumber();
   out.AddNote() << "CME_REL_BASE_DEVELOPER: " << UtRunEnvManager::GetCmeRelBase();
   out.AddNote() << "CME_REL_SUFFIX_DEVELOPER: " << UtRunEnvManager::GetCmeRelSuffix();

   return 0;
}
#endif
