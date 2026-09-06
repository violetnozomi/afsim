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

#ifndef WSFMILSCRIPTEXTENSIONS_HPP
#define WSFMILSCRIPTEXTENSIONS_HPP

#include "wsf_mil_export.h"
class UtScriptTypes;
#include "UtScriptExtension.hpp"

class WSF_MIL_EXPORT WsfMilScriptExtensions : public UtScriptExtension
{
public:
   ~WsfMilScriptExtensions() override = default;
   static void ExtendScriptClasses(UtScriptTypes& aTypes);
   bool        AddExtClassMethods(const std::string& aClassName, const std::string& aBaseName, UtScriptTypes* aScriptTypesPtr) override;
};

#endif
