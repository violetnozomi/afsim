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

#ifndef WSFSCRIPTDEFS_HPP
#define WSFSCRIPTDEFS_HPP

#include "UtScriptClassDefine.hpp"
#include "UtScriptExecutorProgramCounter.hpp"
#include "WsfScriptContext.hpp"

// This file provides Preprocessor definitions for use inside script implementations.
// These definitions mimic variables available in script
#define TIME_NOW WsfScriptContext::GetTIME_NOW(aContext)
#define PLATFORM WsfScriptContext::GetPLATFORM(aContext)
#define PROCESSOR WsfScriptContext::GetPROCESSOR(aContext)
#define MESSAGE WsfScriptContext::GetMESSAGE(aContext)
#define TRACK WsfScriptContext::GetTRACK(aContext)
#define SIMULATION WsfScriptContext::GetSIMULATION(aContext)
#define SCENARIO WsfScriptContext::GetSCENARIO(aContext)

//! This macro can be used inside a script implementation to print a warning if
//! the simulation is initializing. Various script methods are non-functional
//! during simulation initialization (e.g. comm script methods prior to comm
//! framework initialization), such that calls to these methods during
//! platform/platform-type initialization blocks shouldn't be made.
#define WSF_SCRIPT_WARN_INIT                                              \
   UT_SCRIPT_WARN(SIMULATION->GetState() == WsfSimulation::cINITIALIZING, \
                  "Invalid method call during simulation initialization")

#define WSF_SCRIPT_WARN_INIT_NOT_INIT2                                                                            \
   if (SIMULATION->GetState() == WsfSimulation::cINITIALIZING)                                                    \
   {                                                                                                              \
      auto callstack = aExecutorPtr->GetCallstack();                                                              \
      for (auto entry : callstack)                                                                                \
      {                                                                                                           \
         UT_SCRIPT_WARN(                                                                                          \
            entry->mScriptPtr && (entry->mScriptPtr->GetName() == "on_initialize"),                               \
            "Invalid method call during simulation initialization - method may be called during initialization2") \
      }                                                                                                           \
   }

#endif
