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

#ifndef UTDYNAMICLIBRARY_HPP
#define UTDYNAMICLIBRARY_HPP

#include "ut_export.h"

#include <string>

class UT_EXPORT UtDynamicLibrary
{
public:
   static UtDynamicLibrary* Load(const std::string& aPath, std::string& aErrorString);
   UtDynamicLibrary(const UtDynamicLibrary& aSrc) = delete;
   UtDynamicLibrary& operator=(const UtDynamicLibrary&) = delete;
   ~UtDynamicLibrary();

   void* GetSymbol(const std::string& aSymbol);

private:
   UtDynamicLibrary(void* aHandle);

   void* mHandle;
};

#endif
