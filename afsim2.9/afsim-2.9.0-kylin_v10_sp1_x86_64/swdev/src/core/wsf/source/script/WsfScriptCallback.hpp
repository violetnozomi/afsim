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

#ifndef WSFSCRIPTCALLBACK_HPP
#define WSFSCRIPTCALLBACK_HPP

#include "wsf_export.h"

#include <vector>

class UtInput;
class WsfPlatform;
class UtScriptClass;
class UtScriptMethod;

#include "WsfCallback.hpp"

//! WsfScriptCallback provides the ability to execute a
//! list of scripts.  Each script can specify where it will
//! execute, which can be either the platform itself or one
//! of its processors.
class WSF_EXPORT WsfScriptCallback : public WsfCallback
{
public:
   WsfScriptCallback() = default;
   WsfScriptCallback(const std::string& aScriptName);
   WsfScriptCallback(const std::string& aScriptName, const std::string& aProcessorName);
   WsfScriptCallback(const UtScriptMethod& aMethod);
   WsfScriptCallback& operator=(const WsfScriptCallback& aRhs) = delete;
   ~WsfScriptCallback() override;

   WsfCallback* Clone() const override;
   bool         Initialize(double aSimTime) override;
   bool         ProcessInput(UtInput& aInput) override;

   bool Execute(double aSimTime, WsfPlatform* aPlatformPtr) override;

protected:
   WsfScriptCallback(const WsfScriptCallback& aSrc);

private:
   void GetNextValue(UtInput& aInput, std::string& aCommand);

   struct ScriptAndSource
   {
      ScriptAndSource()
         : mScriptId(nullptr)
         , mProcId(nullptr)
         , mMethodPtr(nullptr)
      {
      }
      WsfStringId     mScriptId;
      WsfStringId     mProcId;
      UtScriptMethod* mMethodPtr;
   };

   UtScriptClass*               mPlatformScriptClassPtr{nullptr};
   std::vector<ScriptAndSource> mScriptList;
};

#endif
