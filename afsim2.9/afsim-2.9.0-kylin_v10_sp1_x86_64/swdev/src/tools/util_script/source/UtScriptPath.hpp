// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTSCRIPTPATH_HPP
#define UTSCRIPTPATH_HPP

#include "util_script_export.h"

#include "UtScriptClass.hpp"

class UtPath;

class UTIL_SCRIPT_EXPORT UtScriptPath : public UtScriptClass
{
public:
   UtScriptPath(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   static UtScriptRef* Create(const UtPath& aPath);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   std::string ToString(void* aObjectPtr) const override;
   bool        EqualTo(void* aLHS, void* aRHS) override;
};

#endif
