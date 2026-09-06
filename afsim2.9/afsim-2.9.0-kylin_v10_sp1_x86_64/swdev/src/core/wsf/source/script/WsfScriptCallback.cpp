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

#include "script/WsfScriptCallback.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtScriptMethod.hpp"
#include "UtScriptTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptProcessor.hpp"

WsfScriptCallback::WsfScriptCallback(const std::string& aScriptName)
   : mPlatformScriptClassPtr(nullptr)
{
   ScriptAndSource callback;
   callback.mScriptId = aScriptName;
   mScriptList.push_back(callback);
}

WsfScriptCallback::WsfScriptCallback(const std::string& aScriptName, const std::string& aProcessorName)
   : mPlatformScriptClassPtr(nullptr)
{
   ScriptAndSource callback;
   callback.mScriptId = aScriptName;
   callback.mProcId   = aProcessorName;
   mScriptList.push_back(callback);
}

WsfScriptCallback::WsfScriptCallback(const UtScriptMethod& aMethod)
   : mPlatformScriptClassPtr(nullptr)
{
   ScriptAndSource callback;
   callback.mMethodPtr = new UtScriptMethod(aMethod);
   mScriptList.push_back(callback);
}

// virtual
WsfScriptCallback::~WsfScriptCallback()
{
   for (auto& script : mScriptList)
   {
      delete script.mMethodPtr;
   }
}

// protected
WsfScriptCallback::WsfScriptCallback(const WsfScriptCallback& aSrc)
   : WsfCallback(aSrc)
   , mPlatformScriptClassPtr(aSrc.mPlatformScriptClassPtr)
   , mScriptList(aSrc.mScriptList)
{
   for (auto& script : mScriptList)
   {
      UtScriptMethod*& methodPtr = script.mMethodPtr;
      if (methodPtr != nullptr)
      {
         methodPtr = new UtScriptMethod(*methodPtr);
      }
   }
}

// virtual
WsfCallback* WsfScriptCallback::Clone() const
{
   return new WsfScriptCallback(*this);
}

bool WsfScriptCallback::Initialize(double aSimTime)
{
   bool success(true);
   for (auto& script : mScriptList)
   {
      bool ok(true);
      if (script.mMethodPtr == nullptr)
      {
         std::string errorStr;
         if (!script.mProcId.Empty())
         {
            WsfScriptProcessor* procPtr =
               dynamic_cast<WsfScriptProcessor*>(GetComponentParent()->GetComponent<WsfProcessor>(script.mProcId));
            if (procPtr)
            {
               ok       = procPtr->GetScriptContext().FindScript(script.mScriptId);
               errorStr = "Cannot find callback script on processor.";
            }
            else
            {
               errorStr = "Cannot find callback processor on platform.";
               ok       = false;
            }
         }
         else
         {
            ok       = GetComponentParent()->GetScriptContext().FindScript(script.mScriptId);
            errorStr = "Cannot find callback script on platform.";
         }
         if (!ok)
         {
            auto out = ut::log::error() << errorStr;
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetComponentParent()->GetName();
            if (!script.mProcId.Empty())
            {
               out.AddNote() << "Processor: " << script.mProcId.GetString();
            }
            out.AddNote() << "Callback: " << script.mScriptId.GetString();
         }
      }
      success &= ok;
   }

   return success;
}

//! Processes the input for a list of script calls and
//! where they will execute (platform or one of its processors).

// virtual
bool WsfScriptCallback::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if (command == "execute")
   {
      ScriptAndSource scriptAndSource;

      std::string scriptName;
      GetNextValue(aInput, scriptName);
      scriptAndSource.mScriptId = scriptName;

      aInput.ReadCommand(command);
      if (command == "in")
      {
         std::string processorName;
         GetNextValue(aInput, processorName);
         scriptAndSource.mProcId = processorName;
      }
      else
      {
         aInput.PushBack(command);
      }
      mScriptList.push_back(scriptAndSource);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Reads the next command and throws an error if the command
//! is the end of the callback.

// private
void WsfScriptCallback::GetNextValue(UtInput& aInput, std::string& aCommand)
{
   aInput.ReadCommand(aCommand);
   if (aCommand == "end_callback")
   {
      throw UtInput::BadValue(aInput, "Still expecting callback arguments, but encountered \'end_callback\'");
   }
}

//! Executes the list of scripts in the appropriate contexts.
//! The context can either be the platform itself or one of
//! its processors.

// virtual
bool WsfScriptCallback::Execute(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool success(true);
   for (auto& script : mScriptList)
   {
      bool ok(false);
      if (script.mMethodPtr == nullptr)
      {
         std::string errorStr;
         if (!script.mProcId.Empty())
         {
            WsfScriptProcessor* procPtr =
               dynamic_cast<WsfScriptProcessor*>(aPlatformPtr->GetComponent<WsfProcessor>(script.mProcId));
            if (procPtr)
            {
               ok       = procPtr->ExecuteScript(aSimTime, script.mScriptId);
               errorStr = "Cannot execute callback script on processor.";
            }
            else
            {
               errorStr = "Cannot find callback processor on platform.";
               ok       = false;
            }
         }
         else
         {
            ok       = aPlatformPtr->ExecuteScript(aSimTime, script.mScriptId);
            errorStr = "Cannot execute callback script on platform.";
         }
         if (!ok)
         {
            auto out = ut::log::error() << errorStr;
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << aPlatformPtr->GetName();
            if (!script.mProcId.Empty())
            {
               out.AddNote() << "Processor: " << script.mProcId.GetString();
            }
            out.AddNote() << "Callback: " << script.mScriptId.GetString();
         }
      }
      else
      {
         if (mPlatformScriptClassPtr == nullptr)
         {
            mPlatformScriptClassPtr = aPlatformPtr->GetScriptContext().GetContext().GetTypes()->GetClass("WsfPlatform");
         }
         UtScriptData     returnValue;
         UtScriptDataList args(1);
         args[0].SetPointer(new UtScriptRef(aPlatformPtr, mPlatformScriptClassPtr));
         ok = script.mMethodPtr->Call(&aPlatformPtr->GetSimulation()->GetScriptExecutor(),
                                      returnValue,
                                      args,
                                      &aPlatformPtr->GetScriptContext().GetContext(),
                                      true);
         if (!ok)
         {
            auto out = ut::log::error() << "Cannot execute callback method.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << aPlatformPtr->GetName();
            if (!script.mProcId.Empty())
            {
               out.AddNote() << "Processor: " << script.mProcId.GetString();
            }
            out.AddNote() << "Callback: " << script.mScriptId.GetString();
         }
      }

      success &= ok;
   }

   return success;
}
