// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTSCRIPTEXCEPTION_HPP
#define UTSCRIPTEXCEPTION_HPP

#include "util_script_export.h"

#include "UtException.hpp"

class UTIL_SCRIPT_EXPORT UtScriptException : public UtException
{
public:
   // pull in constructors from UtException.
   using UtException::UtException;
};

//! Call while inside a script implementation to return from the script, printing an error
//! The return value is set negatively.  An assert is thrown for debug mode.
#define UT_SCRIPT_ABORT(MESSAGE_) throw UtScriptException{MESSAGE_};
#define UT_SCRIPT_ASSERT(CONDITION_)        \
   if (!(CONDITION_))                       \
   {                                        \
      throw UtScriptException{#CONDITION_}; \
   }

#endif