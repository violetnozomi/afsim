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

#ifndef UTSCRIPTCOLOR_HPP
#define UTSCRIPTCOLOR_HPP

#include "util_script_export.h"

#include <string>

class UtColor;
#include "UtScriptClass.hpp"
class UtScriptTypes;

//! Define script methods for UtColor.
class UTIL_SCRIPT_EXPORT UtScriptColor : public UtScriptClass
{
public:
   UtScriptColor(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static UtScriptRef* Create(const UtColor& aColor);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   bool EqualTo(void* aLHS, void* aRHS) override;
};

#endif
