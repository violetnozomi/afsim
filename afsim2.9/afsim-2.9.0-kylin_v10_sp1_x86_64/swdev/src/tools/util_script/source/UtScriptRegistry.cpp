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

#include "UtScriptRegistry.hpp"

#include "UtScript.hpp"

UtScriptRegistry::UtScriptRegistry(UtScriptEnvironment* aEnvironmentPtr)
   : mEnvironmentPtr(aEnvironmentPtr)
{
   mIndexRequested = false;
}

void UtScriptRegistry::RegisterScript(UtScript* aScriptPtr)
{
   mRegisteredScripts[aScriptPtr];
   aScriptPtr->Registered(mEnvironmentPtr);
   if (mIndexRequested)
   {
      mFileScriptIndex.insert(IndexMap::value_type(GetScriptPosition(aScriptPtr), aScriptPtr));
   }
}

void UtScriptRegistry::UnregisterScript(UtScript* aScriptPtr)
{
   mRegisteredScripts.erase(aScriptPtr);
   if (mIndexRequested)
   {
      mFileScriptIndex.erase(GetScriptPosition(aScriptPtr));
   }
}

UtScript* UtScriptRegistry::FindScriptAtLine(const std::string& aFilePath, size_t aLineNumber)
{
   if (!mIndexRequested)
   {
      InitIndex();
   }
   ScriptPosition     pos(aFilePath, aLineNumber);
   IndexMap::iterator iter = mFileScriptIndex.lower_bound(pos);
   if (iter != mFileScriptIndex.end())
   {
      if (PositionIsInScript(iter->second, pos))
      {
         return iter->second;
      }
   }
   while (iter != mFileScriptIndex.begin())
   {
      --iter;
      UtScript* scriptPtr = iter->second;
      if (scriptPtr->mSourceFilePath != aFilePath)
      {
         break;
      }
      if (PositionIsInScript(scriptPtr, pos))
      {
         return scriptPtr;
      }
   }
   return nullptr;
}

void UtScriptRegistry::InitIndex()
{
   mIndexRequested = true;
   for (UtScriptPointerMap::iterator i = mRegisteredScripts.begin(); i != mRegisteredScripts.end(); ++i)
   {
      UtScript* scriptPtr = i->first;
      mFileScriptIndex.insert(IndexMap::value_type(GetScriptPosition(scriptPtr), scriptPtr));
   }
}

UtScriptRegistry::ScriptPosition UtScriptRegistry::GetScriptPosition(UtScript* aScriptPtr)
{
   size_t firstLine, lastLine;
   aScriptPtr->GetSourceLineRange(firstLine, lastLine);
   return ScriptPosition(aScriptPtr->mSourceFilePath, firstLine);
}

bool UtScriptRegistry::PositionIsInScript(UtScript* aScriptPtr, ScriptPosition aPosition)
{
   if (aScriptPtr->mSourceFilePath == aPosition.first)
   {
      size_t firstLine, lastLine;
      aScriptPtr->GetSourceLineRange(firstLine, lastLine);
      return aPosition.second >= firstLine && aPosition.second <= lastLine;
   }
   return false;
}
