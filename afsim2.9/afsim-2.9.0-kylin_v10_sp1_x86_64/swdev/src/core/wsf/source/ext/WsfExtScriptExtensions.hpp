// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFEXTSCRIPTEXTENSIONS_HPP
#define WSFEXTSCRIPTEXTENSIONS_HPP

#include "wsf_export.h"

#include "UtScriptExtension.hpp"

namespace wsf
{
namespace ext
{
class WSF_EXPORT ScriptExtensions : public UtScriptExtension
{
public:
   static void ExtendScriptClasses(UtScriptTypes& aTypes);
   bool        AddExtClassMethods(const std::string& aClassName, const std::string& aBaseName, UtScriptTypes* aScriptTypesPtr) override;
};
} // namespace ext
} // namespace wsf

#endif
