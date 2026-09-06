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

#ifndef UTSCRIPTSYSTEM_HPP
#define UTSCRIPTSYSTEM_HPP

#include "util_script_export.h"

#include "UtScriptClass.hpp"

//! Script Class for system calls
class UTIL_SCRIPT_EXPORT UtScriptSystem : public UtScriptClass
{
public:
   UtScriptSystem(UtScriptTypes* aScriptTypesPtr);
};

#endif
