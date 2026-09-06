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

#ifndef WSFSCRIPTTHREATPROCESSORCLASS_HPP
#define WSFSCRIPTTHREATPROCESSORCLASS_HPP

#include "wsf_mil_export.h"

#include "script/WsfScriptProcessorClass.hpp"

//! Define script methods for WsfProcessor.
class WSF_MIL_EXPORT WsfScriptThreatProcessorClass : public WsfScriptProcessorClass
{
public:
   WsfScriptThreatProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Threats);
   UT_DECLARE_SCRIPT_METHOD(IsThreatening);
   UT_DECLARE_SCRIPT_METHOD(NearestThreat);
};

#endif
