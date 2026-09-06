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

#ifndef UTSCRIPTFUNCTION_HPP
#define UTSCRIPTFUNCTION_HPP

#include "util_script_export.h"

#include <string>
#include <utility>
#include <vector>

class UtScriptClass;
#include "UtStringId.hpp"

//! A function prototype, defines a script signature
class UTIL_SCRIPT_EXPORT UtScriptFunction
{
public:
   enum Flags
   {
      cAPP_METHOD = 1,
      cSTATIC     = 2
   };

   UtScriptFunction();

   bool operator<(const UtScriptFunction& aRhs) const;

   bool operator==(const UtScriptFunction& aRhs) const { return !((*this) < aRhs) && !(aRhs < (*this)); }

   UtStringId SpecializeType(UtStringId aThisType, std::pair<UtScriptClass*, UtScriptClass*> aArgs, UtStringId aType);

   void Specialize(UtStringId aThisType, std::pair<UtScriptClass*, UtScriptClass*> aTempArgs);

   int                     mFlags;
   UtStringId              mReturnType;
   UtScriptClass*          mReturnClassPtr;
   std::vector<UtStringId> mArgs;
};

#endif
