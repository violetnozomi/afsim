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

#ifndef WSFSCRIPTMANAGER_HPP
#define WSFSCRIPTMANAGER_HPP

#include "wsf_export.h"

#include <memory>

class UtInput;
class UtScriptClass;
class UtScriptContext;
class UtScriptEnvironment;
class UtScriptParser;
class UtScriptTypes;
class WsfScenario;
class WsfScriptContext;
#include "WsfStringId.hpp"

//! Provides the overall management of the scripting capability within the application and scenarios.
class WSF_EXPORT WsfScriptManager
{
public:
   static void RegisterTypes(UtScriptTypes* aTypePtr);

   WsfScriptManager(WsfScenario* aScenarioPtr, UtScriptEnvironment* aScriptEnvironmentPtr);
   WsfScriptManager(const WsfScriptManager& aSrc);

   ~WsfScriptManager();

   bool ProcessInput(UtInput& aInput);

   bool Add(std::unique_ptr<UtScriptClass> aClassPtr);

   bool ExecuteScript(double aSimTime, WsfStringId aScriptId);

   static UtScriptTypes& GetTypes();

   WsfScriptContext& GetContext();

   // bool ScriptSilentModeEnabled() const;

   bool ScriptListingEnabled() const;

   bool ScriptWritesEnabled() const;

private:
   WsfScriptManager& operator=(const WsfScriptManager& aSrc);

   WsfScriptContext* mContextPtr; //!< The 'global' script context.

   static UtScriptTypes* mTypesPtr;
};

#endif
