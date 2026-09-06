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

#ifndef UTSCRIPTEXECUTORPROGRAMCOUNTER_HPP
#define UTSCRIPTEXECUTORPROGRAMCOUNTER_HPP

#include <cstddef>
#include <cstdint>
class UtScript;
class UtScriptContext;

//! A class that maintains the program counter for the script executor.
struct UtScriptExecutorProgramCounter
{
   ptrdiff_t mFrameIndex;               // index of first element of top stack frame
   using script_code_t = uint_fast32_t; // should be atleast as wide as the script_code_t alias in UtScript.hpp
   script_code_t    mCodeIndex;
   UtScript*        mScriptPtr;
   size_t           mNumArgs;
   UtScriptContext* mInstancePtr;
};

#endif
