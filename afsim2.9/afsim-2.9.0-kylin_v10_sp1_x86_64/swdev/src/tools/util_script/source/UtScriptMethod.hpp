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

#ifndef UTSCRIPTMETHOD_HPP
#define UTSCRIPTMETHOD_HPP

#include "util_script_export.h"

#include <memory>

class UtScript;
class UtScriptClass;
class UtScriptContext;
#include "UtScriptData.hpp"
class UtScriptExecutor;
class UtScriptMethodClass;
class UtScriptRef;
class UtScriptTypes;

//! Stores reference to a script object and method or function for invocation at a later time.
class UTIL_SCRIPT_EXPORT UtScriptMethod
{
public:
   UtScriptMethod(const std::string& aFunctionName);
   UtScriptMethod(UtScriptRef* aBaseObjectPtr, const std::string& aFunctionName);
   ~UtScriptMethod();
   UtScriptMethod(const UtScriptMethod& aSrc);

   bool operator==(const UtScriptMethod& aRhs) const;

   bool Load(UtScriptContext* aGlobalContextPtr, UtScriptContext*& aMethodContextPtr, UtScript*& aScriptPtr);

   bool Call(UtScriptExecutor*       aExecutorPtr,
             UtScriptData&           aReturnVal,
             const UtScriptDataList& aArgs,
             UtScriptContext*        aGlobalContextPtr,
             bool                    aArgsAreOptional = false);

   UtScriptRef*       GetBase() const { return mBasePtr; }
   const std::string& GetName() const { return mMethodName; }

   void Bind(int aParameterIndex, const UtScriptData& aData);

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static UtScriptMethodClass* sClassPtr;

protected:
   void BuildArgs(const UtScriptDataList& aInputArgs, UtScriptDataList& aFinalArgs);

   bool CallAppMethod(UtScriptExecutor*  aExecutorPtr,
                      UtScriptData&      aReturnVal,
                      UtScriptMethodArgs aArgs,
                      UtScriptContext*   aGlobalContext,
                      bool               aArgsAreOptional);

   void                      operator=(const UtScriptMethod& aRhs); // undefined
   UtScriptRef*              mBasePtr;
   std::string               mMethodName;
   std::vector<UtScriptData> mBoundParameters;
};

#endif
