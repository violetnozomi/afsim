// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTSCRIPTFORMAT_HPP
#define UTSCRIPTFORMAT_HPP

#include "util_script_export.h"

#include "UtScriptClass.hpp"

class UTIL_SCRIPT_EXPORT UtScriptFormat : public UtScriptClass
{
public:
   UtScriptFormat(UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aContext) override;
   void  Destroy(void* aObjectPtr) override;
};

#endif
