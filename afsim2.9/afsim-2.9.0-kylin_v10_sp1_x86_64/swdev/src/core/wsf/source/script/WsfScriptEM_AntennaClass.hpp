// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTEM_ANTENNACLASS_HPP
#define WSFSCRIPTEM_ANTENNACLASS_HPP

#include "wsf_export.h"

#include <string>

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

//! The script class for the WsfEM_Antenna object.
class WSF_EXPORT WsfScriptEM_AntennaClass : public UtScriptClass
{
public:
   WsfScriptEM_AntennaClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(ArticulatedPart);

   UT_DECLARE_SCRIPT_METHOD(Height);
   UT_DECLARE_SCRIPT_METHOD(Pitch);
   UT_DECLARE_SCRIPT_METHOD(FieldOfView);
   UT_DECLARE_SCRIPT_METHOD(AzimuthFieldOfView);
   UT_DECLARE_SCRIPT_METHOD(ElevationFieldOfView);
   UT_DECLARE_SCRIPT_METHOD(MaximumRange);
   UT_DECLARE_SCRIPT_METHOD(MinimumRange);
   UT_DECLARE_SCRIPT_METHOD(MaximumAltitude);
   UT_DECLARE_SCRIPT_METHOD(MinimumAltitude);
   UT_DECLARE_SCRIPT_METHOD(EBS_Mode);
   UT_DECLARE_SCRIPT_METHOD(EBS_AzimuthLossExponent);
   UT_DECLARE_SCRIPT_METHOD(EBS_ElevationLossExponent);
   UT_DECLARE_SCRIPT_METHOD(EBS_AzimuthCosineSteeringLimit);
   UT_DECLARE_SCRIPT_METHOD(EBS_ElevationCosineSteeringLimit);
   UT_DECLARE_SCRIPT_METHOD(ScanMode);
   UT_DECLARE_SCRIPT_METHOD(ScanStabilization);
   UT_DECLARE_SCRIPT_METHOD(MaximumAzimuthScanLimit);
   UT_DECLARE_SCRIPT_METHOD(MinimumAzimuthScanLimit);
   UT_DECLARE_SCRIPT_METHOD(MaximumElevationScanLimit);
   UT_DECLARE_SCRIPT_METHOD(MinimumElevationScanLimit);


   UT_DECLARE_SCRIPT_METHOD(SetHeight);
   UT_DECLARE_SCRIPT_METHOD(SetPitch);
   UT_DECLARE_SCRIPT_METHOD(SetFieldOfView);
   UT_DECLARE_SCRIPT_METHOD(SetAzimuthFieldOfView);
   UT_DECLARE_SCRIPT_METHOD(SetElevationFieldOfView);
   UT_DECLARE_SCRIPT_METHOD(SetMaximumRange);
   UT_DECLARE_SCRIPT_METHOD(SetMinimumRange);
   UT_DECLARE_SCRIPT_METHOD(SetMaximumAltitude);
   UT_DECLARE_SCRIPT_METHOD(SetMinimumAltitude);
   UT_DECLARE_SCRIPT_METHOD(SetEBS_Mode);
   UT_DECLARE_SCRIPT_METHOD(SetEBS_AzimuthLossExponent);
   UT_DECLARE_SCRIPT_METHOD(SetEBS_ElevationLossExponent);
   UT_DECLARE_SCRIPT_METHOD(SetEBS_AzimuthCosineSteeringLimit);
   UT_DECLARE_SCRIPT_METHOD(SetEBS_ElevationCosineSteeringLimit);
   UT_DECLARE_SCRIPT_METHOD(SetScanMode);
   UT_DECLARE_SCRIPT_METHOD(SetScanStabilization);
   UT_DECLARE_SCRIPT_METHOD(SetMaximumAzimuthScanLimit);
   UT_DECLARE_SCRIPT_METHOD(SetMinimumAzimuthScanLimit);
   UT_DECLARE_SCRIPT_METHOD(SetMaximumElevationScanLimit);
   UT_DECLARE_SCRIPT_METHOD(SetMinimumElevationScanLimit);
};

#endif
