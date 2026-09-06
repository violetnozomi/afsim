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

#ifndef UTSCRIPTCORE_HPP
#define UTSCRIPTCORE_HPP

#include "util_script_export.h"

#include <mutex>

#include "UtScriptClass.hpp"

//! UtScriptCore is a set of built-in methods for script.
//! These methods are available from any script.
class UTIL_SCRIPT_EXPORT UtScriptCore : public UtScriptClass
{
public:
   static std::mutex  sWriteBufferMutex;
   static std::string sWriteBuffer;
   static bool        sAbortOnAssert;
   UtScriptCore(UtScriptTypes* aScriptTypesPtr);
   ~UtScriptCore() override;
};

#endif
