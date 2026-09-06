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

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <iostream>
#include <sstream>

#include "UtDynamicLibrary.hpp"

UtDynamicLibrary::UtDynamicLibrary(void* aHandle)
   : mHandle(aHandle)
{
}

UtDynamicLibrary::~UtDynamicLibrary()
{
   if (mHandle != nullptr)
   {
#ifndef _WIN32
      ::dlclose(mHandle);
#else
      ::FreeLibrary((HMODULE)mHandle);
#endif
   }
}

UtDynamicLibrary* UtDynamicLibrary::Load(const std::string& aName, std::string& aErrorString)
{
   UtDynamicLibrary* dynamicLibraryPtr = nullptr;

   if (!aName.empty())
   {
      void* handlePtr = nullptr;

#if defined(_WIN32)
      // Avoid the 'Bad Image' message box
      UINT oldmode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
      handlePtr    = ::LoadLibraryA(aName.c_str());
      if (handlePtr == nullptr)
      {
         // If LoadLibrary failed, try an application specific search order using LOAD_LIBRARY_SEARCH_* flags. The
         // flags specified below will search directories in the following order:
         // 1. The directory that contains the DLL (LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR)
         // 2. The application directory
         // 3. Paths explicitly added with the AddDllDirectory or SetDllDirectory functions
         // 4. The System directory
         // Note that directories in the standard search path are NOT searched, including directories listed in the
         // PATH environment variable. If that becomes an issue, LOAD_WITH_ALTERED_SEARCH_PATH could be used instead.
         handlePtr =
            ::LoadLibraryExA(aName.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
      }

      if (handlePtr == nullptr)
      {
         DWORD             errorCode = ::GetLastError();
         std::stringstream ss;
         ss << std::string("LoadLibrary(") << aName << std::string(") Failed. errorCode: ") << errorCode;
         aErrorString = ss.str();
      }
      // Restore the error mode.
      SetErrorMode(oldmode);
#else
      handlePtr = ::dlopen(aName.c_str(), RTLD_NOW);
      if (handlePtr == nullptr)
      {
         std::string errorString;
         const char* errorStringPtr = ::dlerror();
         if (errorStringPtr != nullptr)
         {
            errorString = errorStringPtr;
         }
         aErrorString += "Failed to load \"" + aName + '"';
         if (errorString.size())
         {
            aErrorString += ": " + errorString;
         }
      }
#endif
      if (handlePtr != nullptr)
      {
         dynamicLibraryPtr = new UtDynamicLibrary(handlePtr);
      }
   }
   else
   {
      aErrorString = "Empty path.";
   }
   return dynamicLibraryPtr;
}

void* UtDynamicLibrary::GetSymbol(const std::string& aSymbol)
{
   void* symbolPtr = nullptr;
   if (mHandle != nullptr)
   {
#ifdef _WIN32
      symbolPtr = ::GetProcAddress((HMODULE)mHandle, aSymbol.c_str());
#else
      symbolPtr = ::dlsym(mHandle, aSymbol.c_str());
#endif
   }
   return symbolPtr;
}
