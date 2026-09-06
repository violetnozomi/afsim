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

#ifndef WSFSCRIPTTRACKPROCESSORCLASS_HPP
#define WSFSCRIPTTRACKPROCESSORCLASS_HPP

#include "wsf_export.h"

#include <string>

#include "script/WsfScriptProcessorClass.hpp"

//! Define script methods for WsfProcessor.
class WSF_EXPORT WsfScriptTrackProcessorClass : public WsfScriptProcessorClass
{
public:
   WsfScriptTrackProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(TrackManager);
   UT_DECLARE_SCRIPT_METHOD(ReportsFusedTracks); // NO_DOC | INTERNAL USE
   UT_DECLARE_SCRIPT_METHOD(ReportsRawTracks);   // NO_DOC | INTERNAL USE
};

#endif
