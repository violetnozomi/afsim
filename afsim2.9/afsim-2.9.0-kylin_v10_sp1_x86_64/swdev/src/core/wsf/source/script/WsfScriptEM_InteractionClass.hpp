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

#ifndef WSFSCRIPTEM_INTERACTIONCLASS_HPP
#define WSFSCRIPTEM_INTERACTIONCLASS_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"

//! Define script methods for WsfEM_Interaction.,
class WSF_EXPORT WsfScriptEM_InteractionClass : public UtScriptClass
{
public:
   WsfScriptEM_InteractionClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~WsfScriptEM_InteractionClass() override;

   // EM Interaction methods

   // Status
   UT_DECLARE_SCRIPT_METHOD(Succeeded);
   UT_DECLARE_SCRIPT_METHOD(Failed);
   UT_DECLARE_SCRIPT_METHOD(FailedStatus);

   // Xmtr
   UT_DECLARE_SCRIPT_METHOD(XmtrPlatform);
   UT_DECLARE_SCRIPT_METHOD(XmtrPlatformName);
   UT_DECLARE_SCRIPT_METHOD(XmtrLocation);
   UT_DECLARE_SCRIPT_METHOD(Xmtr);

   // Rcvr
   UT_DECLARE_SCRIPT_METHOD(RcvrPlatform);
   UT_DECLARE_SCRIPT_METHOD(RcvrPlatformName);
   UT_DECLARE_SCRIPT_METHOD(RcvrLocation);
   UT_DECLARE_SCRIPT_METHOD(Rcvr);

   // Target
   UT_DECLARE_SCRIPT_METHOD(TargetPlatform);
   UT_DECLARE_SCRIPT_METHOD(TargetPlatformName);
   UT_DECLARE_SCRIPT_METHOD(TargetLocation);

   // Location - Relative
   UT_DECLARE_SCRIPT_METHOD(RcvrToTargetRange);
   UT_DECLARE_SCRIPT_METHOD(RcvrToTargetTrueAzimuth);
   UT_DECLARE_SCRIPT_METHOD(RcvrToTargetTrueElevation);
   UT_DECLARE_SCRIPT_METHOD(RcvrToTargetApparentAzimuth);
   UT_DECLARE_SCRIPT_METHOD(RcvrToTargetApparentElevation);

   UT_DECLARE_SCRIPT_METHOD(TargetToRcvrRange);
   UT_DECLARE_SCRIPT_METHOD(TargetToRcvrTrueAzimuth);
   UT_DECLARE_SCRIPT_METHOD(TargetToRcvrTrueElevation);
   UT_DECLARE_SCRIPT_METHOD(TargetToRcvrApparentAzimuth);
   UT_DECLARE_SCRIPT_METHOD(TargetToRcvrApparentElevation);

   UT_DECLARE_SCRIPT_METHOD(XmtrToTargetRange);
   UT_DECLARE_SCRIPT_METHOD(XmtrToTargetTrueAzimuth);
   UT_DECLARE_SCRIPT_METHOD(XmtrToTargetTrueElevation);
   UT_DECLARE_SCRIPT_METHOD(XmtrToTargetApparentAzimuth);
   UT_DECLARE_SCRIPT_METHOD(XmtrToTargetApparentElevation);

   UT_DECLARE_SCRIPT_METHOD(TargetToXmtrRange);
   UT_DECLARE_SCRIPT_METHOD(TargetToXmtrTrueAzimuth);
   UT_DECLARE_SCRIPT_METHOD(TargetToXmtrTrueElevation);
   UT_DECLARE_SCRIPT_METHOD(TargetToXmtrApparentAzimuth);
   UT_DECLARE_SCRIPT_METHOD(TargetToXmtrApparentElevation);

   UT_DECLARE_SCRIPT_METHOD(XmtrToRcvrRange);
   UT_DECLARE_SCRIPT_METHOD(XmtrToRcvrTrueAzimuth);
   UT_DECLARE_SCRIPT_METHOD(XmtrToRcvrTrueElevation);
   UT_DECLARE_SCRIPT_METHOD(XmtrToRcvrApparentAzimuth);
   UT_DECLARE_SCRIPT_METHOD(XmtrToRcvrApparentElevation);

   UT_DECLARE_SCRIPT_METHOD(RcvrToXmtrRange);
   UT_DECLARE_SCRIPT_METHOD(RcvrToXmtrTrueAzimuth);
   UT_DECLARE_SCRIPT_METHOD(RcvrToXmtrTrueElevation);
   UT_DECLARE_SCRIPT_METHOD(RcvrToXmtrApparentAzimuth);
   UT_DECLARE_SCRIPT_METHOD(RcvrToXmtrApparentElevation);

   // Beam
   UT_DECLARE_SCRIPT_METHOD(XmtrBeamBearing);
   UT_DECLARE_SCRIPT_METHOD(XmtrBeamElevation);
   UT_DECLARE_SCRIPT_METHOD(XmtrBeamTargetRelativeAzimuth);
   UT_DECLARE_SCRIPT_METHOD(XmtrBeamTargetRelativeElevation);
   UT_DECLARE_SCRIPT_METHOD(XmtrBeamGain);
   UT_DECLARE_SCRIPT_METHOD(XmtrBeamEBS_Azimuth);
   UT_DECLARE_SCRIPT_METHOD(XmtrBeamEBS_Elevation);
   UT_DECLARE_SCRIPT_METHOD(XmtrBeamEBS_Omega);

   UT_DECLARE_SCRIPT_METHOD(RcvrBeamBearing);
   UT_DECLARE_SCRIPT_METHOD(RcvrBeamElevation);
   UT_DECLARE_SCRIPT_METHOD(RcvrBeamTargetRelativeAzimuth);
   UT_DECLARE_SCRIPT_METHOD(RcvrBeamTargetRelativeElevation);
   UT_DECLARE_SCRIPT_METHOD(RcvrBeamGain);
   UT_DECLARE_SCRIPT_METHOD(RcvrBeamEBS_Azimuth);
   UT_DECLARE_SCRIPT_METHOD(RcvrBeamEBS_Elevation);
   UT_DECLARE_SCRIPT_METHOD(RcvrBeamEBS_Omega);

   // Signature
   UT_DECLARE_SCRIPT_METHOD(InfraredSignature);
   UT_DECLARE_SCRIPT_METHOD(InfraredSignatureAzimuth);
   UT_DECLARE_SCRIPT_METHOD(InfraredSignatureElevation);

   UT_DECLARE_SCRIPT_METHOD(RadarSignature);
   UT_DECLARE_SCRIPT_METHOD(RadarSignatureAzimuth);
   UT_DECLARE_SCRIPT_METHOD(RadarSignatureElevation);

   UT_DECLARE_SCRIPT_METHOD(OpticalSignature);
   UT_DECLARE_SCRIPT_METHOD(OpticalSignatureAzimuth);
   UT_DECLARE_SCRIPT_METHOD(OpticalSignatureElevation);

   // Power
   UT_DECLARE_SCRIPT_METHOD(XmtdPower);
   UT_DECLARE_SCRIPT_METHOD(PowerDensityAtTarget);
   UT_DECLARE_SCRIPT_METHOD(RcvdPower);
   UT_DECLARE_SCRIPT_METHOD(RcvrNoise);
   UT_DECLARE_SCRIPT_METHOD(ClutterPower);
   UT_DECLARE_SCRIPT_METHOD(InterferencePower);

   // Signal to ...
   UT_DECLARE_SCRIPT_METHOD(SignalToInterference);
   UT_DECLARE_SCRIPT_METHOD(SignalToNoise);
   UT_DECLARE_SCRIPT_METHOD(SignalToNoiseClutter);
   UT_DECLARE_SCRIPT_METHOD(SignalToNoiseClutterInterference);

   // Others
   UT_DECLARE_SCRIPT_METHOD(MaskingFactor);
   UT_DECLARE_SCRIPT_METHOD(PropagationFactor);
   UT_DECLARE_SCRIPT_METHOD(AbsorptionFactor);
   UT_DECLARE_SCRIPT_METHOD(BackgroundRadiantIntensity);
   UT_DECLARE_SCRIPT_METHOD(ContrastRadiantIntensity);
   UT_DECLARE_SCRIPT_METHOD(DetectionThreshold);
   UT_DECLARE_SCRIPT_METHOD(PixelCount);
};

#endif
