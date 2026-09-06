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

#ifndef UTSCRIPTSIGNAL_HPP
#define UTSCRIPTSIGNAL_HPP

#include "util_script_export.h"

#include <memory>

class UtScriptMethod;
class UtScriptContext;
class UtScriptClass;
#include "UtScriptData.hpp"
class UtScriptTypes;
class UtScriptExecutor;

//! Stores a list of script methods for invoking
class UTIL_SCRIPT_EXPORT UtScriptSignal
{
public:
   UtScriptSignal() = default;
   ~UtScriptSignal();
   UtScriptSignal(const UtScriptSignal& aSrc);

   void             Clear();
   void             Connect(UtScriptMethod* aMethodPtr);
   void             Disconnect(const UtScriptMethod& aMethod);
   UtScriptDataList Call(UtScriptExecutor* aExecutorPtr, const UtScriptDataList& aArgs, UtScriptContext* aContextPtr);
   const std::vector<UtScriptMethod*>& GetMethods() { return mMethodList; }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

protected:
   std::vector<UtScriptMethod*> mMethodList;
};

#endif
