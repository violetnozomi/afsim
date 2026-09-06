// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERSCRIPTEXTENSIONS_HPP
#define WSFCYBERSCRIPTEXTENSIONS_HPP

#include "wsf_cyber_export.h"

#include "UtScriptExtension.hpp"
class UtScriptTypes;

namespace wsf
{

namespace cyber
{

//! A convenience class referenced by the application extension and via base class (UtScriptExtension)
//! to register all of the script classes and methods, to include derived types
class WSF_CYBER_EXPORT ScriptExtensions : public UtScriptExtension
{
public:
   static void ExtendScriptClasses(UtScriptTypes& aTypes);
   bool        AddExtClassMethods(const std::string& aClassName, const std::string& aBaseName, UtScriptTypes* aScriptTypesPtr) override;
};

} // namespace cyber
} // namespace wsf

#endif
