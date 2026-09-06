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

#include "UtScriptSystem.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "UtScriptMethodDefine.hpp"
#include "UtWallClock.hpp"

UtScriptSystem::UtScriptSystem(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("System", aScriptTypesPtr)
{
   mConstructible = false;

   UT_SCRIPT_NONWRAPPING_CLASS(System);

   UT_SCRIPT_STATIC_METHOD(int, Command, (const char* cmd)) { return system(cmd); };

   UT_SCRIPT_STATIC_METHOD(std::string, EnvironmentVariable, (const char* aVarName))
   {
      const char* envVar = getenv(aVarName);
      if (envVar != nullptr)
      {
         return envVar;
      }
      return "";
   };

   UT_SCRIPT_STATIC_METHOD(double, Time, ())
   {
      static UtWallClock sClock(UtWallClock::cSYSTEM_TIME);
      return sClock.GetRawClock();
   };

   UT_SCRIPT_DEPRECATED("Renamed to 'EnvironmentVariable'")
   UT_SCRIPT_STATIC_METHOD(std::string, GetEnvironmentVariable, (const char* aVarName)) // NO_DOC | DEPRECATED
   {
      const char* envVar = getenv(aVarName);
      if (envVar != nullptr)
      {
         return envVar;
      }
      return "";
   };
}
