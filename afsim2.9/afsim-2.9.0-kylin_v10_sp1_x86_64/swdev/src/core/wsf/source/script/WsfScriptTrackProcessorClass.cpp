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


#include "script/WsfScriptTrackProcessorClass.hpp"

#include "UtMemory.hpp"
#include "WsfTrackProcessor.hpp"

// ================================================================================================
WsfScriptTrackProcessorClass::WsfScriptTrackProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTrackProcessor");

   AddMethod(ut::make_unique<TrackManager>());
   AddMethod(ut::make_unique<ReportsFusedTracks>());
   AddMethod(ut::make_unique<ReportsRawTracks>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackProcessorClass, WsfTrackProcessor, TrackManager, 0, "WsfTrackManager", "")
{
   WsfTrackManager& trackManager = aObjectPtr->GetTrackManager();
   aReturnVal.SetPointer(new UtScriptRef(&trackManager, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackProcessorClass, WsfTrackProcessor, ReportsFusedTracks, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetReportFusedTracks());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackProcessorClass, WsfTrackProcessor, ReportsRawTracks, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetReportRawTracks());
}
