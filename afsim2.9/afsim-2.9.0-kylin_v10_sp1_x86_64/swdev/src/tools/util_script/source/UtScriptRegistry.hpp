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

#ifndef UTSCRIPTREGISTRY_HPP
#define UTSCRIPTREGISTRY_HPP

#include <map>
#include <string>
#include <unordered_map>

class UtScript;
class UtScriptEnvironment;
class UtScriptTypes;

//! Maintains a searchable list of all scripts
class UtScriptRegistry
{
public:
   typedef std::pair<std::string, size_t> ScriptPosition;

   UtScriptRegistry(UtScriptEnvironment* aEnvironmentPtr);

   void      RegisterScript(UtScript* aScriptPtr);
   void      UnregisterScript(UtScript* aScriptPtr);
   UtScript* FindScriptAtLine(const std::string& aFilePath, size_t aLineNumber);

protected:
   bool           PositionIsInScript(UtScript* aScriptPtr, ScriptPosition aPosition);
   void           InitIndex();
   ScriptPosition GetScriptPosition(UtScript* aScriptPtr);

   using UtScriptPointerMap = std::unordered_map<UtScript*, bool>;
   using IndexMap           = std::multimap<ScriptPosition, UtScript*>;

   UtScriptEnvironment* mEnvironmentPtr;
   UtScriptPointerMap   mRegisteredScripts;
   IndexMap             mFileScriptIndex;
   bool                 mIndexRequested;
};

#endif
