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

#ifndef WSFSCRIPTARTICULATEDPARTCLASS_HPP
#define WSFSCRIPTARTICULATEDPARTCLASS_HPP

#include "wsf_export.h"

#include "script/WsfScriptPlatformPartClass.hpp"

//! Define script methods for WsfArticulatedPart.
class WSF_EXPORT WsfScriptArticulatedPartClass : public WsfScriptPlatformPartClass
{
public:
   WsfScriptArticulatedPartClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Pitch);
   UT_DECLARE_SCRIPT_METHOD(SetPitch);
   UT_DECLARE_SCRIPT_METHOD(Roll);
   UT_DECLARE_SCRIPT_METHOD(SetRoll);
   UT_DECLARE_SCRIPT_METHOD(Yaw);
   UT_DECLARE_SCRIPT_METHOD(SetYaw);
   UT_DECLARE_SCRIPT_METHOD(X);
   UT_DECLARE_SCRIPT_METHOD(Y);
   UT_DECLARE_SCRIPT_METHOD(Z);
   UT_DECLARE_SCRIPT_METHOD(DX);
   UT_DECLARE_SCRIPT_METHOD(DY);
   UT_DECLARE_SCRIPT_METHOD(DZ);
   UT_DECLARE_SCRIPT_METHOD(SetXYZ);
   UT_DECLARE_SCRIPT_METHOD(SetDXYZ);
   UT_DECLARE_SCRIPT_METHOD(Location);

   UT_DECLARE_SCRIPT_METHOD(CanSlewInAzimuth);
   UT_DECLARE_SCRIPT_METHOD(CanSlewInElevation);
   UT_DECLARE_SCRIPT_METHOD(SlewMinimumAzimuth);
   UT_DECLARE_SCRIPT_METHOD(SlewMaximumAzimuth);
   UT_DECLARE_SCRIPT_METHOD(SlewMinimumElevation);
   UT_DECLARE_SCRIPT_METHOD(SlewMaximumElevation);
   UT_DECLARE_SCRIPT_METHOD(IsSlewing);

   UT_DECLARE_SCRIPT_METHOD(CanCueInAzimuth);
   UT_DECLARE_SCRIPT_METHOD(CanCueInElevation);
   UT_DECLARE_SCRIPT_METHOD(CueMinimumAzimuth);
   UT_DECLARE_SCRIPT_METHOD(CueMaximumAzimuth);
   UT_DECLARE_SCRIPT_METHOD(CueMinimumElevation);
   UT_DECLARE_SCRIPT_METHOD(CueMaximumElevation);

   UT_DECLARE_SCRIPT_METHOD(ActualCuedAzimuth);
   UT_DECLARE_SCRIPT_METHOD(ActualCuedElevation);
   UT_DECLARE_SCRIPT_METHOD(CurrentCuedAzimuth);
   UT_DECLARE_SCRIPT_METHOD(CurrentCuedElevation);

   UT_DECLARE_SCRIPT_METHOD(ApparentLocationOf);
   UT_DECLARE_SCRIPT_METHOD(RelativeAzimuthOf);
   UT_DECLARE_SCRIPT_METHOD(RelativeElevationOf);

   UT_DECLARE_SCRIPT_METHOD(CueToLLA);
   UT_DECLARE_SCRIPT_METHOD(CueToWCS);
   UT_DECLARE_SCRIPT_METHOD(CueToWCS_2);
   UT_DECLARE_SCRIPT_METHOD(CueToPoint);
   UT_DECLARE_SCRIPT_METHOD(CueToTrack);
   UT_DECLARE_SCRIPT_METHOD(CueToAbsoluteAzEl);
   UT_DECLARE_SCRIPT_METHOD(CueToRelativeAzEl);
   UT_DECLARE_SCRIPT_METHOD(ClearCueing);
   UT_DECLARE_SCRIPT_METHOD(IsCued);

   UT_DECLARE_SCRIPT_METHOD(TimeToCueToLLA);
   UT_DECLARE_SCRIPT_METHOD(TimeToCueToWCS);
   UT_DECLARE_SCRIPT_METHOD(TimeToCueToWCS_2);
   UT_DECLARE_SCRIPT_METHOD(TimeToCueToPoint);
   UT_DECLARE_SCRIPT_METHOD(TimeToCueToTrack);
   UT_DECLARE_SCRIPT_METHOD(TimeToCueToAbsoluteAzEl);
   UT_DECLARE_SCRIPT_METHOD(TimeToCueToRelativeAzEl);

   UT_DECLARE_SCRIPT_METHOD(MaskingPatternState);
   UT_DECLARE_SCRIPT_METHOD(SetMaskingPatternState);
   UT_DECLARE_SCRIPT_METHOD(EnableArticulationUpdates);
   UT_DECLARE_SCRIPT_METHOD(DisableArticulationUpdates);

   // Script Methods for UtEntityPart
   UT_DECLARE_SCRIPT_METHOD(ConvertPCSVectorToWCS);
   UT_DECLARE_SCRIPT_METHOD(ConvertWCSVectorToPCS);
   UT_DECLARE_SCRIPT_METHOD(ConvertPCSVectorToECS);
   UT_DECLARE_SCRIPT_METHOD(ConvertECSVectorToPCS);
};

#endif
