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

#ifndef UTSCRIPTMATH_HPP
#define UTSCRIPTMATH_HPP

#include "util_script_export.h"

#include "UtOptimalAssignment.hpp"
#include "UtRandom.hpp"
#include "UtScriptClass.hpp"

class UTIL_SCRIPT_EXPORT UtScriptMath : public UtScriptClass
{
public:
   UtScriptMath(UtScriptTypes* aScriptTypesPtr);

   ut::Random&          GetRandom() { return mRandom; }
   UtOptimalAssignment& GetOptimalAssigner() { return mOptimalAssigner; }

private:
   ut::Random          mRandom;
   UtOptimalAssignment mOptimalAssigner;
};

#endif
