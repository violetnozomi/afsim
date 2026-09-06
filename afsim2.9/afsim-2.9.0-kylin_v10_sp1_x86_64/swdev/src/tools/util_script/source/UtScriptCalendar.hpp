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

#ifndef UTSCRIPTCALENDAR_HPP
#define UTSCRIPTCALENDAR_HPP

#include "util_script_export.h"

#include <string>

#include "UtScriptClass.hpp"

class UTIL_SCRIPT_EXPORT UtScriptCalendar : public UtScriptClass
{
public:
   UtScriptCalendar(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void*       Create(const UtScriptContext& aContext) override;
   void        Destroy(void* aObjectPtr) override;
   void*       Clone(void* aObjectPtr) override;
   std::string ToString(void* aObjectPtr) const override;
};

#endif
