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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTSCRIPTEXTENSION_HPP
#define UTSCRIPTEXTENSION_HPP

#include "util_script_export.h"

#include <string>

//--------------------------------------------------------------
//! UtScriptExtension is responsible for providing common
//! inheritance type class access to external class methods
//! add by external sources. Registering the derived class
//! with the script types provides common functions to add
//! external methods to script classes.
//--------------------------------------------------------------

class UtScriptTypes;

class UTIL_SCRIPT_EXPORT UtScriptExtension
{
public:
   virtual ~UtScriptExtension() = default;
   ;
   virtual bool AddExtClassMethods(const std::string& aClassName,
                                   const std::string& aBaseName,
                                   UtScriptTypes*     aScriptTypesPtr) = 0;
};

#endif
