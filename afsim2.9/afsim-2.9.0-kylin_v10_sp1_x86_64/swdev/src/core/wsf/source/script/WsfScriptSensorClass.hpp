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

#ifndef WSFSCRIPTSENSORCLASS_HPP
#define WSFSCRIPTSENSORCLASS_HPP

#include "wsf_export.h"

#include "script/WsfScriptArticulatedPartClass.hpp"

//! Define script methods for WsfSensor.
class WSF_EXPORT WsfScriptSensorClass : public WsfScriptArticulatedPartClass
{
public:
   WsfScriptSensorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(IsA_TypeOf); // static IsA_TypeOf("derived-type", "base-type")

   UT_DECLARE_SCRIPT_METHOD(TurnOff);
   UT_DECLARE_SCRIPT_METHOD(TurnOn);

   UT_DECLARE_SCRIPT_METHOD(ModeCount);
   UT_DECLARE_SCRIPT_METHOD(ModeName);
   UT_DECLARE_SCRIPT_METHOD(CurrentMode);
   UT_DECLARE_SCRIPT_METHOD(SelectMode);
   UT_DECLARE_SCRIPT_METHOD(DeselectMode);
   UT_DECLARE_SCRIPT_METHOD(BeamCount_1);
   UT_DECLARE_SCRIPT_METHOD(BeamCount_2);
   UT_DECLARE_SCRIPT_METHOD(BeamCount_3);

   UT_DECLARE_SCRIPT_METHOD(FOV_1);
   UT_DECLARE_SCRIPT_METHOD(FOV_2);
   UT_DECLARE_SCRIPT_METHOD(FOV_3);
   UT_DECLARE_SCRIPT_METHOD(FOV_MinimumAzimuth);
   UT_DECLARE_SCRIPT_METHOD(FOV_MaximumAzimuth);
   UT_DECLARE_SCRIPT_METHOD(FOV_MinimumElevation);
   UT_DECLARE_SCRIPT_METHOD(FOV_MaximumElevation);
   UT_DECLARE_SCRIPT_METHOD(FOV_MinimumRange);
   UT_DECLARE_SCRIPT_METHOD(FOV_MaximumRange);

   UT_DECLARE_SCRIPT_METHOD(SetFOV_1);
   UT_DECLARE_SCRIPT_METHOD(SetFOV_2);
   UT_DECLARE_SCRIPT_METHOD(SetFOV_3);
   UT_DECLARE_SCRIPT_METHOD(SetFOV_Azimuth);
   UT_DECLARE_SCRIPT_METHOD(SetFOV_Elevation);
   UT_DECLARE_SCRIPT_METHOD(SetFOV_Range);

   UT_DECLARE_SCRIPT_METHOD(CanScanInAzimuth);
   UT_DECLARE_SCRIPT_METHOD(CanScanInElevation);
   UT_DECLARE_SCRIPT_METHOD(ScanMinimumAzimuth);
   UT_DECLARE_SCRIPT_METHOD(ScanMaximumAzimuth);
   UT_DECLARE_SCRIPT_METHOD(ScanMinimumElevation);
   UT_DECLARE_SCRIPT_METHOD(ScanMaximumElevation);

   UT_DECLARE_SCRIPT_METHOD(WithinFieldOfView);

   UT_DECLARE_SCRIPT_METHOD(ActiveTrackCount);
   UT_DECLARE_SCRIPT_METHOD(MaximumTrackCount);
   UT_DECLARE_SCRIPT_METHOD(TrackQuality_1);
   UT_DECLARE_SCRIPT_METHOD(TrackQuality_2);
   UT_DECLARE_SCRIPT_METHOD(TrackQuality_3);

   UT_DECLARE_SCRIPT_METHOD(ActiveRequestCount_1);
   UT_DECLARE_SCRIPT_METHOD(ActiveRequestCount_2);
   UT_DECLARE_SCRIPT_METHOD(MaximumRequestCount_1);
   UT_DECLARE_SCRIPT_METHOD(MaximumRequestCount_2);
   UT_DECLARE_SCRIPT_METHOD(HaveRequestFor);
   UT_DECLARE_SCRIPT_METHOD(StartTracking);
   UT_DECLARE_SCRIPT_METHOD(StopTracking);
   UT_DECLARE_SCRIPT_METHOD(FrameTime);

   UT_DECLARE_SCRIPT_METHOD(Xmtr_1);
   UT_DECLARE_SCRIPT_METHOD(Xmtr_2);
   UT_DECLARE_SCRIPT_METHOD(XmtrCount_1);
   UT_DECLARE_SCRIPT_METHOD(XmtrCount_2);

   UT_DECLARE_SCRIPT_METHOD(Rcvr_1);
   UT_DECLARE_SCRIPT_METHOD(Rcvr_2);
   UT_DECLARE_SCRIPT_METHOD(RcvrCount_1);
   UT_DECLARE_SCRIPT_METHOD(RcvrCount_2);
};

#endif
