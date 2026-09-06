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

#include "script/WsfScriptEM_InteractionClass.hpp"

#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorResult.hpp"

namespace
{
double PrintableAngle(double aAngle)
{
   double angle = aAngle * UtMath::cDEG_PER_RAD;
   if (fabs(aAngle) < 1.0E-12)
   {
      angle = 0.0;
   }
   return angle;
}

void GetRelativeAngle(WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                      const WsfEM_Interaction::RelativeData& aRelData,
                      double                                 aUnitVec[3],
                      double&                                aAz,
                      double&                                aEl)
{
   double otherLocNED[3];
   aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aUnitVec, otherLocNED);
   double magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
   aAz                = atan2(otherLocNED[1], otherLocNED[0]);
   aEl                = atan2(-otherLocNED[2], magnitudeNE);
}

void GetRelativeAngleTgt(WsfPlatform*                           aPlatformPtr,
                         const WsfEM_Interaction::RelativeData& aRelData,
                         double                                 aUnitVec[3],
                         double&                                aAz,
                         double&                                aEl)
{
   double otherLocNED[3];
   aPlatformPtr->ConvertWCSVectorToNED(otherLocNED, aUnitVec);
   double magnitudeNE = sqrt(otherLocNED[0] * otherLocNED[0] + otherLocNED[1] * otherLocNED[1]);
   aAz                = atan2(otherLocNED[1], otherLocNED[0]);
   aEl                = atan2(-otherLocNED[2], magnitudeNE);
}

void GetRelativeAngleBeam(WsfEM_XmtrRcvr* aXmtrRcvrPtr, const WsfEM_Interaction::BeamData& aBeamData, double& aAz, double& aEl)
{
   double beamPointNED[3];
   aXmtrRcvrPtr->GetAntenna()->ConvertWCS_VectorToNED(aBeamData.mWCS_ToBeamTransform[0], beamPointNED);
   double magnitudeNE = sqrt(beamPointNED[0] * beamPointNED[0] + beamPointNED[1] * beamPointNED[1]);
   aAz                = atan2(beamPointNED[1], beamPointNED[0]);
   aEl                = atan2(-beamPointNED[2], magnitudeNE);
}
} // namespace

WsfScriptEM_InteractionClass::WsfScriptEM_InteractionClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfEM_Interaction");
   mIsScriptAccessible = true;

   // Add each of the method objects to the class.

   // Status
   AddMethod(ut::make_unique<Succeeded>());
   AddMethod(ut::make_unique<Failed>());
   AddMethod(ut::make_unique<FailedStatus>());

   // Xmtr
   AddMethod(ut::make_unique<XmtrPlatform>());
   AddMethod(ut::make_unique<XmtrPlatformName>());
   AddMethod(ut::make_unique<XmtrLocation>());
   AddMethod(ut::make_unique<Xmtr>());

   // Rcvr
   AddMethod(ut::make_unique<RcvrPlatform>());
   AddMethod(ut::make_unique<RcvrPlatformName>());
   AddMethod(ut::make_unique<RcvrLocation>());
   AddMethod(ut::make_unique<Rcvr>());

   // Target
   AddMethod(ut::make_unique<TargetPlatform>());
   AddMethod(ut::make_unique<TargetPlatformName>());
   AddMethod(ut::make_unique<TargetLocation>());

   // Location - Relative
   AddMethod(ut::make_unique<RcvrToTargetRange>());
   AddMethod(ut::make_unique<RcvrToTargetTrueAzimuth>());
   AddMethod(ut::make_unique<RcvrToTargetTrueElevation>());
   AddMethod(ut::make_unique<RcvrToTargetApparentAzimuth>());
   AddMethod(ut::make_unique<RcvrToTargetApparentElevation>());

   AddMethod(ut::make_unique<TargetToRcvrRange>());
   AddMethod(ut::make_unique<TargetToRcvrTrueAzimuth>());
   AddMethod(ut::make_unique<TargetToRcvrTrueElevation>());
   AddMethod(ut::make_unique<TargetToRcvrApparentAzimuth>());
   AddMethod(ut::make_unique<TargetToRcvrApparentElevation>());

   AddMethod(ut::make_unique<XmtrToTargetRange>());
   AddMethod(ut::make_unique<XmtrToTargetTrueAzimuth>());
   AddMethod(ut::make_unique<XmtrToTargetTrueElevation>());
   AddMethod(ut::make_unique<XmtrToTargetApparentAzimuth>());
   AddMethod(ut::make_unique<XmtrToTargetApparentElevation>());

   AddMethod(ut::make_unique<TargetToXmtrRange>());
   AddMethod(ut::make_unique<TargetToXmtrTrueAzimuth>());
   AddMethod(ut::make_unique<TargetToXmtrTrueElevation>());
   AddMethod(ut::make_unique<TargetToXmtrApparentAzimuth>());
   AddMethod(ut::make_unique<TargetToXmtrApparentElevation>());

   AddMethod(ut::make_unique<XmtrToRcvrRange>());
   AddMethod(ut::make_unique<XmtrToRcvrTrueAzimuth>());
   AddMethod(ut::make_unique<XmtrToRcvrTrueElevation>());
   AddMethod(ut::make_unique<XmtrToRcvrApparentAzimuth>());
   AddMethod(ut::make_unique<XmtrToRcvrApparentElevation>());

   AddMethod(ut::make_unique<RcvrToXmtrRange>());
   AddMethod(ut::make_unique<RcvrToXmtrTrueAzimuth>());
   AddMethod(ut::make_unique<RcvrToXmtrTrueElevation>());
   AddMethod(ut::make_unique<RcvrToXmtrApparentAzimuth>());
   AddMethod(ut::make_unique<RcvrToXmtrApparentElevation>());

   // Beam
   AddMethod(ut::make_unique<XmtrBeamBearing>());
   AddMethod(ut::make_unique<XmtrBeamElevation>());
   AddMethod(ut::make_unique<XmtrBeamTargetRelativeAzimuth>());
   AddMethod(ut::make_unique<XmtrBeamTargetRelativeElevation>());
   AddMethod(ut::make_unique<XmtrBeamGain>());
   AddMethod(ut::make_unique<XmtrBeamEBS_Azimuth>());
   AddMethod(ut::make_unique<XmtrBeamEBS_Elevation>());
   AddMethod(ut::make_unique<XmtrBeamEBS_Omega>());

   AddMethod(ut::make_unique<RcvrBeamBearing>());
   AddMethod(ut::make_unique<RcvrBeamElevation>());
   AddMethod(ut::make_unique<RcvrBeamTargetRelativeAzimuth>());
   AddMethod(ut::make_unique<RcvrBeamTargetRelativeElevation>());
   AddMethod(ut::make_unique<RcvrBeamGain>());
   AddMethod(ut::make_unique<RcvrBeamEBS_Azimuth>());
   AddMethod(ut::make_unique<RcvrBeamEBS_Elevation>());
   AddMethod(ut::make_unique<RcvrBeamEBS_Omega>());

   // Signature
   AddMethod(ut::make_unique<InfraredSignature>());
   AddMethod(ut::make_unique<InfraredSignatureAzimuth>());
   AddMethod(ut::make_unique<InfraredSignatureElevation>());

   AddMethod(ut::make_unique<RadarSignature>());
   AddMethod(ut::make_unique<RadarSignatureAzimuth>());
   AddMethod(ut::make_unique<RadarSignatureElevation>());

   AddMethod(ut::make_unique<OpticalSignature>());
   AddMethod(ut::make_unique<OpticalSignatureAzimuth>());
   AddMethod(ut::make_unique<OpticalSignatureElevation>());

   // Power
   AddMethod(ut::make_unique<XmtdPower>());
   AddMethod(ut::make_unique<PowerDensityAtTarget>());
   AddMethod(ut::make_unique<RcvdPower>());
   AddMethod(ut::make_unique<RcvrNoise>());
   AddMethod(ut::make_unique<ClutterPower>());
   AddMethod(ut::make_unique<InterferencePower>());

   // Signal to ...
   AddMethod(ut::make_unique<SignalToInterference>());
   AddMethod(ut::make_unique<SignalToNoise>());
   AddMethod(ut::make_unique<SignalToNoiseClutter>());
   AddMethod(ut::make_unique<SignalToNoiseClutterInterference>());

   // Others
   AddMethod(ut::make_unique<MaskingFactor>());
   AddMethod(ut::make_unique<PropagationFactor>());
   AddMethod(ut::make_unique<AbsorptionFactor>());
   AddMethod(ut::make_unique<BackgroundRadiantIntensity>());
   AddMethod(ut::make_unique<ContrastRadiantIntensity>());
   AddMethod(ut::make_unique<DetectionThreshold>());
   AddMethod(ut::make_unique<PixelCount>());
}

WsfScriptEM_InteractionClass::~WsfScriptEM_InteractionClass() {}

// bool success = <x>.Succeeded();
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, Succeeded, 0, "bool", "")
{
   bool success = ((aObjectPtr->mCheckedStatus != 0) && (aObjectPtr->mFailedStatus == 0));
   aReturnVal.SetBool(success);
}

// bool success = <x>.Failed();
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, Failed, 0, "bool", "")
{
   bool success = ((aObjectPtr->mCheckedStatus != 0) && (aObjectPtr->mFailedStatus == 0));
   aReturnVal.SetBool(!success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, FailedStatus, 0, "string", "")
{
   std::string failedStatusStr;

   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cXMTR_RANGE_LIMITS) != 0)
   {
      failedStatusStr += "Xmtr_Range_Limits_Exceeded";
   }
   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cRCVR_RANGE_LIMITS) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Rcvr_Range_Limits_Exceeded";
   }
   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cXMTR_ALTITUDE_LIMITS) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Xmtr_Altitude_Limits_Exceeded";
   }
   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cRCVR_ALTITUDE_LIMITS) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Rcvr_Altitude_Limits_Exceeded";
   }
   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cXMTR_ANGLE_LIMITS) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Xmtr_Angle_Limits_Exceeded";
   }
   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cRCVR_ANGLE_LIMITS) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Rcvr_Angle_Limits_Exceeded";
   }
   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cXMTR_HORIZON_MASKING) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Xmtr_Masked_By_Horizon";
   }
   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cRCVR_HORIZON_MASKING) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Rcvr_Masked_By_Horizon";
   }
   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cXMTR_TERRAIN_MASKING) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Xmtr_Masked_By_Terrain";
   }
   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cRCVR_TERRAIN_MASKING) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Rcvr_Masked_By_Terrain";
   }
   if ((aObjectPtr->mFailedStatus & WsfEM_Interaction::cSIGNAL_LEVEL) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Insufficient_Signal";
   }
   if ((aObjectPtr->mFailedStatus & WsfSensorResult::cCONCEALMENT) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Target_Concealed";
   }
   if ((aObjectPtr->mFailedStatus & WsfSensorResult::cDOPPLER_LIMITS) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Doppler_Limits_Exceeded";
   }
   if ((aObjectPtr->mFailedStatus & WsfSensorResult::cVELOCITY_LIMITS) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Velocity_Limits_Exceeded";
   }
   if ((aObjectPtr->mFailedStatus & WsfSensorResult::cTARGET_DELETED) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Target_Deleted";
   }
   if ((aObjectPtr->mFailedStatus & WsfSensorResult::cDETECTION_STOP) != 0)
   {
      if (!failedStatusStr.empty())
      {
         failedStatusStr += " ";
      }
      failedStatusStr += "Detection_Stopped";
   }

   aReturnVal.SetString(failedStatusStr);
}

// Xmtr
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrPlatform, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr = nullptr;
   WsfEM_Xmtr*  xmtrPtr     = aObjectPtr->GetTransmitter();
   if (xmtrPtr != nullptr)
   {
      platformPtr = xmtrPtr->GetPlatform();
   }
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrPlatformName, 0, "string", "")
{
   std::string name;
   WsfEM_Xmtr* xmtrPtr = aObjectPtr->GetTransmitter();
   if (xmtrPtr != nullptr)
   {
      name = xmtrPtr->GetPlatform()->GetName();
   }
   aReturnVal.SetString(name);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrLocation, 0, "WsfGeoPoint", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double locWCS[3] = {0.0};
   if (aObjectPtr->mXmtrLoc.mIsValid)
   {
      UtVec3d::Set(locWCS, aObjectPtr->mXmtrLoc.mLocWCS);
   }
   else
   {
      WsfEM_Xmtr* xmtrPtr = aObjectPtr->GetTransmitter();
      if (xmtrPtr != nullptr)
      {
         xmtrPtr->GetPlatform()->GetLocationWCS(locWCS);
      }
   }

   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, Xmtr, 0, "WsfEM_Xmtr", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetTransmitter(), aReturnClassPtr));
}

// Rcvr
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrPlatform, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr = nullptr;
   WsfEM_Rcvr*  rcvrPtr     = aObjectPtr->GetReceiver();
   if (rcvrPtr != nullptr)
   {
      platformPtr = rcvrPtr->GetPlatform();
   }
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrPlatformName, 0, "string", "")
{
   std::string name;
   WsfEM_Rcvr* rcvrPtr = aObjectPtr->GetReceiver();
   if (rcvrPtr != nullptr)
   {
      name = rcvrPtr->GetPlatform()->GetName();
   }
   aReturnVal.SetString(name);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrLocation, 0, "WsfGeoPoint", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double locWCS[3] = {0.0};
   if (aObjectPtr->mRcvrLoc.mIsValid)
   {
      UtVec3d::Set(locWCS, aObjectPtr->mRcvrLoc.mLocWCS);
   }
   else
   {
      WsfEM_Rcvr* rcvrPtr = aObjectPtr->GetReceiver();
      if (rcvrPtr != nullptr)
      {
         rcvrPtr->GetPlatform()->GetLocationWCS(locWCS);
      }
   }

   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, Rcvr, 0, "WsfEM_Rcvr", "")
{
   aReturnVal.SetPointer(UtScriptRef::Ref(aObjectPtr->GetReceiver(), aReturnClassPtr));
}

// Target
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetPlatform, 0, "WsfPlatform", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetTarget(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetPlatformName, 0, "string", "")
{
   std::string  name;
   WsfPlatform* targetPtr = aObjectPtr->GetTarget();
   if (targetPtr != nullptr)
   {
      name = targetPtr->GetName();
   }
   aReturnVal.SetString(name);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetLocation, 0, "WsfGeoPoint", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double locWCS[3] = {0.0};
   if (aObjectPtr->mTgtLoc.mIsValid)
   {
      UtVec3d::Set(locWCS, aObjectPtr->mTgtLoc.mLocWCS);
   }
   else
   {
      WsfPlatform* targetPtr = aObjectPtr->GetTarget();
      if (targetPtr != nullptr)
      {
         targetPtr->GetLocationWCS(locWCS);
      }
   }

   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// double rcvrToTargetRange = <x>.RcvrToTargetRange();           // m
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrToTargetRange, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double range = -999.0;
   if (aObjectPtr->mRcvrToTgt.mRange > 0.0)
   {
      range = aObjectPtr->mRcvrToTgt.mRange;
   }
   aReturnVal.SetDouble(range);
}

// double rcvrToTargetTrueAzimuth = <x>.RcvrToTargetTrueAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrToTargetTrueAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrToTgt.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetReceiver(),
                       aObjectPtr->mRcvrToTgt,
                       aObjectPtr->mRcvrToTgt.mTrueUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrToTargetTrueElevation = <x>.RcvrToTargetTrueElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrToTargetTrueElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrToTgt.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetReceiver(),
                       aObjectPtr->mRcvrToTgt,
                       aObjectPtr->mRcvrToTgt.mTrueUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrToTargetApparentAzimuth = <x>.RcvrToTargetApparentAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrToTargetApparentAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mRcvrToTgt.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetReceiver(),
                       aObjectPtr->mRcvrToTgt,
                       aObjectPtr->mRcvrToTgt.mUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrToTargetApparentElevation = <x>.RcvrToTargetApparentElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrToTargetApparentElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mRcvrToTgt.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetReceiver(),
                       aObjectPtr->mRcvrToTgt,
                       aObjectPtr->mRcvrToTgt.mUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double targetToRcvrRange = <x>.TargetToRcvrRange();           // m
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetToRcvrRange, 0, "double", "")
{
   double range = -999.0;
   if (aObjectPtr->mTgtToRcvr.mRange > 0.0)
   {
      range = aObjectPtr->mTgtToRcvr.mRange;
   }
   aReturnVal.SetDouble(range);
}

// double targetToRcvrTrueAzimuth = <x>.TargetToRcvrTrueAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetToRcvrTrueAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mTgtToRcvr.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleTgt(aObjectPtr->GetTarget(),
                          aObjectPtr->mTgtToRcvr,
                          aObjectPtr->mTgtToRcvr.mTrueUnitVecWCS,
                          azimuthAngle,
                          elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double targetToRcvrTrueElevation = <x>.TargetToRcvrTrueElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetToRcvrTrueElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mTgtToRcvr.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleTgt(aObjectPtr->GetTarget(),
                          aObjectPtr->mTgtToRcvr,
                          aObjectPtr->mTgtToRcvr.mTrueUnitVecWCS,
                          azimuthAngle,
                          elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double targetToRcvrApparentAzimuth = <x>.TargetToRcvrApparentAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetToRcvrApparentAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mTgtToRcvr.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleTgt(aObjectPtr->GetTarget(),
                          aObjectPtr->mTgtToRcvr,
                          aObjectPtr->mTgtToRcvr.mUnitVecWCS,
                          azimuthAngle,
                          elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double targetToRcvrApparentElevation = <x>.TargetToRcvrApparentElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetToRcvrApparentElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mTgtToRcvr.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleTgt(aObjectPtr->GetTarget(),
                          aObjectPtr->mTgtToRcvr,
                          aObjectPtr->mTgtToRcvr.mUnitVecWCS,
                          azimuthAngle,
                          elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrToTargetRange = <x>.XmtrToTargetRange();           // m
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrToTargetRange, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double range = -999.0;
   if (aObjectPtr->mXmtrToTgt.mRange > 0.0)
   {
      range = aObjectPtr->mXmtrToTgt.mRange;
   }
   aReturnVal.SetDouble(range);
}

// double xmtrToTargetTrueAzimuth = <x>.XmtrToTargetTrueAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrToTargetTrueAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrToTgt.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetTransmitter(),
                       aObjectPtr->mXmtrToTgt,
                       aObjectPtr->mXmtrToTgt.mTrueUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrToTargetTrueElevation = <x>.XmtrToTargetTrueElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrToTargetTrueElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrToTgt.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetTransmitter(),
                       aObjectPtr->mXmtrToTgt,
                       aObjectPtr->mXmtrToTgt.mTrueUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrToTargetApparentAzimuth = <x>.XmtrToTargetApparentAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrToTargetApparentAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mXmtrToTgt.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetTransmitter(),
                       aObjectPtr->mXmtrToTgt,
                       aObjectPtr->mXmtrToTgt.mUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrToTargetApparentElevation = <x>.XmtrToTargetApparentElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrToTargetApparentElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mXmtrToTgt.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetTransmitter(),
                       aObjectPtr->mXmtrToTgt,
                       aObjectPtr->mXmtrToTgt.mUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double targetToXmtrRange = <x>.TargetToXmtrRange();           // m
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetToXmtrRange, 0, "double", "")
{
   double range = -999.0;
   if (aObjectPtr->mTgtToXmtr.mRange > 0.0)
   {
      range = aObjectPtr->mTgtToXmtr.mRange;
   }
   aReturnVal.SetDouble(range);
}

// double targetToXmtrTrueAzimuth = <x>.TargetToXmtrTrueAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetToXmtrTrueAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mTgtToXmtr.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleTgt(aObjectPtr->GetTarget(),
                          aObjectPtr->mTgtToXmtr,
                          aObjectPtr->mTgtToXmtr.mTrueUnitVecWCS,
                          azimuthAngle,
                          elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double targetToXmtrTrueElevation = <x>.TargetToXmtrTrueElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetToXmtrTrueElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mTgtToXmtr.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleTgt(aObjectPtr->GetTarget(),
                          aObjectPtr->mTgtToXmtr,
                          aObjectPtr->mTgtToXmtr.mTrueUnitVecWCS,
                          azimuthAngle,
                          elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double targetToXmtrApparentAzimuth = <x>.TargetToXmtrApparentAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetToXmtrApparentAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mTgtToXmtr.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleTgt(aObjectPtr->GetTarget(),
                          aObjectPtr->mTgtToXmtr,
                          aObjectPtr->mTgtToXmtr.mUnitVecWCS,
                          azimuthAngle,
                          elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double targetToXmtrApparentElevation = <x>.TargetToXmtrApparentElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, TargetToXmtrApparentElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mTgtToXmtr.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleTgt(aObjectPtr->GetTarget(),
                          aObjectPtr->mTgtToXmtr,
                          aObjectPtr->mTgtToXmtr.mUnitVecWCS,
                          azimuthAngle,
                          elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrToRcvrRange = <x>.XmtrToRcvrRange();           // m
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrToRcvrRange, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double range = -999.0;
   if (aObjectPtr->mXmtrToRcvr.mRange > 0.0)
   {
      range = aObjectPtr->mXmtrToRcvr.mRange;
   }
   aReturnVal.SetDouble(range);
}

// double xmtrToRcvrTrueAzimuth = <x>.XmtrToRcvrTrueAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrToRcvrTrueAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrToRcvr.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetTransmitter(),
                       aObjectPtr->mXmtrToRcvr,
                       aObjectPtr->mXmtrToRcvr.mTrueUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrToRcvrTrueElevation = <x>.XmtrToRcvrTrueElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrToRcvrTrueElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrToRcvr.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetTransmitter(),
                       aObjectPtr->mXmtrToRcvr,
                       aObjectPtr->mXmtrToRcvr.mTrueUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrToRcvrApparentAzimuth = <x>.XmtrToRcvrApparentAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrToRcvrApparentAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mXmtrToRcvr.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetTransmitter(),
                       aObjectPtr->mXmtrToRcvr,
                       aObjectPtr->mXmtrToRcvr.mUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrToRcvrApparentElevation = <x>.XmtrToRcvrApparentElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrToRcvrApparentElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mXmtrToRcvr.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetTransmitter(),
                       aObjectPtr->mXmtrToRcvr,
                       aObjectPtr->mXmtrToRcvr.mUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrToXmtrRange = <x>.RcvrToXmtrRange();           // m
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrToXmtrRange, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double range = -999.0;
   if (aObjectPtr->mRcvrToXmtr.mRange > 0.0)
   {
      range = aObjectPtr->mRcvrToXmtr.mRange;
   }
   aReturnVal.SetDouble(range);
}

// double rcvrToXmtrTrueAzimuth = <x>.RcvrToXmtrTrueAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrToXmtrTrueAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrToXmtr.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetReceiver(),
                       aObjectPtr->mRcvrToXmtr,
                       aObjectPtr->mRcvrToXmtr.mTrueUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrToXmtrTrueElevation = <x>.RcvrToXmtrTrueElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrToXmtrTrueElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrToXmtr.mRange > 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetReceiver(),
                       aObjectPtr->mRcvrToXmtr,
                       aObjectPtr->mRcvrToXmtr.mTrueUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrToXmtrApparentAzimuth = <x>.RcvrToXmtrApparentAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrToXmtrApparentAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mRcvrToXmtr.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetReceiver(),
                       aObjectPtr->mRcvrToXmtr,
                       aObjectPtr->mRcvrToXmtr.mUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrToXmtrApparentElevation = <x>.RcvrToXmtrApparentElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrToXmtrApparentElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if ((aObjectPtr->mRcvrToXmtr.mRange > 0.0) && (aObjectPtr->mEarthRadiusScale != 1.0))
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngle(aObjectPtr->GetReceiver(),
                       aObjectPtr->mRcvrToXmtr,
                       aObjectPtr->mRcvrToXmtr.mUnitVecWCS,
                       azimuthAngle,
                       elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrBeamBearing = <x>.XmtrBeamBearing();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrBeamBearing, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrBeam.mGain >= 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleBeam(aObjectPtr->GetTransmitter(), aObjectPtr->mXmtrBeam, azimuthAngle, elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrBeamElevation = <x>.XmtrBeamElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrBeamElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrBeam.mGain >= 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleBeam(aObjectPtr->GetTransmitter(), aObjectPtr->mXmtrBeam, azimuthAngle, elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrBeamTargetRelativeAzimuth = <x>.XmtrBeamTargetRelativeAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrBeamTargetRelativeAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrBeam.mGain >= 0.0)
   {
      angle = PrintableAngle(aObjectPtr->mXmtrBeam.mAz);
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrBeamTargetRelativeElevation = <x>.XmtrBeamTargetRelativeElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrBeamTargetRelativeElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrBeam.mGain >= 0.0)
   {
      angle = PrintableAngle(aObjectPtr->mXmtrBeam.mEl);
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrBeamGain = <x>.XmtrBeamGain();                //dB
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrBeamGain, 0, "double", "")
{
   double gain = -300.0;
   if (aObjectPtr->mXmtrBeam.mGain > 0.0)
   {
      gain = UtMath::LinearToDB(aObjectPtr->mXmtrBeam.mGain);
   }
   aReturnVal.SetDouble(gain);
}

// double xmtrBeamEBS_Azimuth = <x>.XmtrBeamEBS_Azimuth();    //degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrBeamEBS_Azimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrBeam.mGain > 0.0)
   {
      if (aObjectPtr->GetTransmitter()->GetAntenna()->GetEBS_Mode() != WsfEM_Antenna::cEBS_NONE)
      {
         angle = PrintableAngle(aObjectPtr->mXmtrBeam.mEBS_Az);
      }
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrBeamEBS_Elevation = <x>.XmtrBeamEBS_Elevation();    //degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrBeamEBS_Elevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrBeam.mGain > 0.0)
   {
      if (aObjectPtr->GetTransmitter()->GetAntenna()->GetEBS_Mode() != WsfEM_Antenna::cEBS_NONE)
      {
         angle = PrintableAngle(aObjectPtr->mXmtrBeam.mEBS_El);
      }
   }
   aReturnVal.SetDouble(angle);
}

// double xmtrBeamEBS_Omega = <x>.XmtrBeamEBS_Omega();    //degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtrBeamEBS_Omega, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mXmtrBeam.mGain > 0.0)
   {
      if (aObjectPtr->GetTransmitter()->GetAntenna()->GetEBS_Mode() != WsfEM_Antenna::cEBS_NONE)
      {
         double cosTheta = cos(aObjectPtr->mXmtrBeam.mEBS_Az) * cos(aObjectPtr->mXmtrBeam.mEBS_El);
         cosTheta        = std::max(std::min(cosTheta, 1.0), -1.0); // For safety
         angle           = PrintableAngle(acos(cosTheta));
      }
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrBeamBearing = <x>.RcvrBeamBearing();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrBeamBearing, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrBeam.mGain >= 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleBeam(aObjectPtr->GetReceiver(), aObjectPtr->mRcvrBeam, azimuthAngle, elevationAngle);
      angle = PrintableAngle(UtMath::NormalizeAngle0_TwoPi(azimuthAngle));
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrBeamElevation = <x>.RcvrBeamElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrBeamElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrBeam.mGain >= 0.0)
   {
      double azimuthAngle   = -999.0;
      double elevationAngle = -999.0;
      GetRelativeAngleBeam(aObjectPtr->GetReceiver(), aObjectPtr->mXmtrBeam, azimuthAngle, elevationAngle);
      angle = PrintableAngle(elevationAngle);
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrBeamTargetRelativeAzimuth = <x>.RcvrBeamTargetRelativeAzimuth();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrBeamTargetRelativeAzimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrBeam.mGain >= 0.0)
   {
      angle = PrintableAngle(aObjectPtr->mRcvrBeam.mAz);
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrBeamTargetRelativeElevation = <x>.RcvrBeamTargetRelativeElevation();         // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrBeamTargetRelativeElevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrBeam.mGain >= 0.0)
   {
      angle = PrintableAngle(aObjectPtr->mRcvrBeam.mEl);
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrBeamGain = <x>.RcvrBeamGain();                //dB
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrBeamGain, 0, "double", "")
{
   double gain = -300.0;
   if (aObjectPtr->mRcvrBeam.mGain > 0.0)
   {
      gain = UtMath::LinearToDB(aObjectPtr->mRcvrBeam.mGain);
   }
   aReturnVal.SetDouble(gain);
}

// double rcvrBeamEBS_Azimuth = <x>.RcvrBeamEBS_Azimuth();    //degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrBeamEBS_Azimuth, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrBeam.mGain > 0.0)
   {
      if (aObjectPtr->GetReceiver()->GetAntenna()->GetEBS_Mode() != WsfEM_Antenna::cEBS_NONE)
      {
         angle = PrintableAngle(aObjectPtr->mRcvrBeam.mEBS_Az);
      }
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrBeamEBS_Elevation = <x>.RcvrBeamEBS_Elevation();    //degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrBeamEBS_Elevation, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrBeam.mGain >= 0.0)
   {
      if (aObjectPtr->GetReceiver()->GetAntenna()->GetEBS_Mode() != WsfEM_Antenna::cEBS_NONE)
      {
         angle = PrintableAngle(aObjectPtr->mRcvrBeam.mEBS_El);
      }
   }
   aReturnVal.SetDouble(angle);
}

// double rcvrBeamEBS_Omega = <x>.RcvrBeamEBS_Omega();    //degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrBeamEBS_Omega, 0, "double", "")
{
   aObjectPtr->ComputeUndefinedGeometry();
   double angle = -999.0;
   if (aObjectPtr->mRcvrBeam.mGain >= 0.0)
   {
      if (aObjectPtr->GetReceiver()->GetAntenna()->GetEBS_Mode() != WsfEM_Antenna::cEBS_NONE)
      {
         double cosTheta = cos(aObjectPtr->mRcvrBeam.mEBS_Az) * cos(aObjectPtr->mRcvrBeam.mEBS_El);
         cosTheta        = std::max(std::min(cosTheta, 1.0), -1.0); // For safety
         angle           = PrintableAngle(acos(cosTheta));
      }
   }
   aReturnVal.SetDouble(angle);
}

// double infraredSignature = <x>.InfraredSignature();       // m^2
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, InfraredSignature, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mInfraredSig);
}

// double infraredSignatureAz = <x>.InfraredSignatureAzimuth();     // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, InfraredSignatureAzimuth, 0, "double", "")
{
   double angle = -999.0;
   if (aObjectPtr->mInfraredSig > 0.0)
   {
      angle = PrintableAngle(aObjectPtr->mInfraredSigAz);
   }
   aReturnVal.SetDouble(angle);
}

// double infraredSignatureEl = <x>.InfraredSignatureElevation();     // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, InfraredSignatureElevation, 0, "double", "")
{
   double angle = -999.0;
   if (aObjectPtr->mInfraredSig > 0.0)
   {
      angle = PrintableAngle(aObjectPtr->mInfraredSigEl);
   }
   aReturnVal.SetDouble(angle);
}

// double radarSignature = <x>.RadarSignature();       // m^2
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RadarSignature, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mRadarSig);
}

// double radarSignatureAz = <x>.RadarSignatureAzimuth();     // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RadarSignatureAzimuth, 0, "double", "")
{
   double angle = -999.0;
   if (aObjectPtr->mRadarSig > 0.0)
   {
      angle = PrintableAngle(aObjectPtr->mRadarSigAz);
   }
   aReturnVal.SetDouble(angle);
}

// double radarSignatureEl = <x>.RadarSignatureElevation();     // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RadarSignatureElevation, 0, "double", "")
{
   double angle = -999.0;
   if (aObjectPtr->mRadarSig > 0.0)
   {
      angle = PrintableAngle(aObjectPtr->mRadarSigEl);
   }
   aReturnVal.SetDouble(angle);
}

// double opticalSignature = <x>.OpticalSignature();       // m^2
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, OpticalSignature, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mOpticalSig);
}

// double opticalSignatureAz = <x>.OpticalSignatureAzimuth();     // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, OpticalSignatureAzimuth, 0, "double", "")
{
   double angle = -999.0;
   if (aObjectPtr->mOpticalSig > 0.0)
   {
      angle = PrintableAngle(aObjectPtr->mOpticalSigAz);
   }
   aReturnVal.SetDouble(angle);
}

// double opticalSignatureEl = <x>.OpticalSignatureElevation();     // degrees
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, OpticalSignatureElevation, 0, "double", "")
{
   double angle = -999.0;
   if (aObjectPtr->mOpticalSig > 0.0)
   {
      angle = PrintableAngle(aObjectPtr->mOpticalSigEl);
   }
   aReturnVal.SetDouble(angle);
}

// double xmtdPower = <x>.XmtdPower();     // dBw
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, XmtdPower, 0, "double", "")
{
   double power = -300.0;
   if (aObjectPtr->mXmtdPower > 0.0)
   {
      power = UtMath::LinearToDB(aObjectPtr->mXmtdPower);
   }
   aReturnVal.SetDouble(power);
}

// double powerDensityAtTarget = <x>.PowerDensityAtTarget();     // dBw/m^2
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, PowerDensityAtTarget, 0, "double", "")
{
   double power = -300.0;
   if (aObjectPtr->mPowerDensityAtTarget > 0.0)
   {
      power = UtMath::LinearToDB(aObjectPtr->mPowerDensityAtTarget);
   }
   aReturnVal.SetDouble(power);
}

// double rcvdPower = <x>.RcvdPower();     // dBw
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvdPower, 0, "double", "")
{
   double power = -300.0;
   if (aObjectPtr->mRcvdPower > 0.0)
   {
      power = UtMath::LinearToDB(aObjectPtr->mRcvdPower);
   }
   aReturnVal.SetDouble(power);
}

// double rcvrNoise = <x>.RcvrNoise();     // dBw
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, RcvrNoise, 0, "double", "")
{
   double power = -300.0;
   if (aObjectPtr->mRcvrNoisePower > 0.0)
   {
      power = UtMath::LinearToDB(aObjectPtr->mRcvrNoisePower);
   }
   aReturnVal.SetDouble(power);
}

// double clutterPower = <x>.ClutterPower();     // dBw
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, ClutterPower, 0, "double", "")
{
   double power = -300.0;
   if (aObjectPtr->mClutterPower > 0.0)
   {
      power = UtMath::LinearToDB(aObjectPtr->mClutterPower);
   }
   aReturnVal.SetDouble(power);
}

// double noiseInterferencePower = <x>.NoiseInterferencePower();     // dBw
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, InterferencePower, 0, "double", "")
{
   double power = -300.0;
   if (aObjectPtr->mInterferencePower > 0.0)
   {
      power = UtMath::LinearToDB(aObjectPtr->mInterferencePower);
   }
   aReturnVal.SetDouble(power);
}

// double signalTo = <x>.SignalToInterference();            //dB
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, SignalToInterference, 0, "double", "")
{
   double returnValue = -300.0;
   if (aObjectPtr->mSignalToNoise > 0.0)
   {
      returnValue = UtMath::LinearToDB(aObjectPtr->mSignalToNoise);
   }
   aReturnVal.SetDouble(returnValue);
}

// double signalTo = <x>.SignalToNoise();            //dB
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, SignalToNoise, 0, "double", "")
{
   double returnValue = -300.0;
   if ((aObjectPtr->mSignalToNoise > 0.0) && (aObjectPtr->mRcvrNoisePower > 0.0))
   {
      double noisePower   = aObjectPtr->mRcvrNoisePower;
      double clutterPower = std::max(aObjectPtr->mClutterPower, 0.0);
      double intfPower    = std::max(aObjectPtr->mInterferencePower, 0.0);
      // Signal power used to be aResult.mRcvdPower, but that didn't always contain all the parts that were
      // considered 'signal' (depends on the sensor implementation). The following recovers the signal.
      double signalPower = aObjectPtr->mSignalToNoise * (noisePower + clutterPower + intfPower);
      returnValue        = UtMath::LinearToDB(signalPower / noisePower);
   }
   aReturnVal.SetDouble(returnValue);
}

// double signalTo = <x>.SignalToNoiseClutter();            //dB
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, SignalToNoiseClutter, 0, "double", "")
{
   double returnValue = -300.0;
   if ((aObjectPtr->mSignalToNoise > 0.0) && (aObjectPtr->mRcvrNoisePower > 0.0))
   {
      double noisePower   = aObjectPtr->mRcvrNoisePower;
      double clutterPower = std::max(aObjectPtr->mClutterPower, 0.0);
      double intfPower    = std::max(aObjectPtr->mInterferencePower, 0.0);
      // Signal power used to be aResult.mRcvdPower, but that didn't always contain all the parts that were
      // considered 'signal' (depends on the sensor implementation). The following recovers the signal.
      double signalPower = aObjectPtr->mSignalToNoise * (noisePower + clutterPower + intfPower);
      returnValue        = UtMath::LinearToDB(signalPower / (noisePower + clutterPower));
   }
   aReturnVal.SetDouble(returnValue);
}

// double signalTo = <x>.SignalToNoiseClutterInterference();            //dB
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, SignalToNoiseClutterInterference, 0, "double", "")
{
   double returnValue = -300.0;
   if ((aObjectPtr->mSignalToNoise > 0.0) && (aObjectPtr->mRcvrNoisePower > 0.0))
   {
      double noisePower   = aObjectPtr->mRcvrNoisePower;
      double clutterPower = std::max(aObjectPtr->mClutterPower, 0.0);
      double intfPower    = std::max(aObjectPtr->mInterferencePower, 0.0);
      // Signal power used to be aResult.mRcvdPower, but that didn't always contain all the parts that were
      // considered 'signal' (depends on the sensor implementation). The following recovers the signal.
      double signalPower = aObjectPtr->mSignalToNoise * (noisePower + clutterPower + intfPower);
      returnValue        = UtMath::LinearToDB(signalPower / (noisePower + clutterPower + intfPower));
   }
   aReturnVal.SetDouble(returnValue);
}

// double maskingFactor = <x>.MaskingFactor();                 // [0..1]
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, MaskingFactor, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mMaskingFactor);
}

// double propagationFactor = <x>.PropagationFactor();         // dB
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, PropagationFactor, 0, "double", "")
{
   double factor = 0.0;
   if (aObjectPtr->mPropagationFactor > 0.0)
   {
      factor = UtMath::LinearToDB(aObjectPtr->mPropagationFactor);
   }
   aReturnVal.SetDouble(factor);
}

// double absorptionFactor = <x>.AbsorptionFactor();         // dB
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, AbsorptionFactor, 0, "double", "")
{
   double factor = 0.0;
   if (aObjectPtr->mAbsorptionFactor > 0.0)
   {
      factor = UtMath::LinearToDB(aObjectPtr->mAbsorptionFactor);
   }
   aReturnVal.SetDouble(factor);
}

// double backgroundRadiantIntensity = <x>.BackgroundRadiantIntensity();         // w/sr
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, BackgroundRadiantIntensity, 0, "double", "")
{
   double intensity = 0.0;
   if ((aObjectPtr->mInfraredSig > 0.0) && (aObjectPtr->mSignalToNoise > 0.0))
   {
      intensity = aObjectPtr->mBackgroundRadiantIntensity;
   }
   aReturnVal.SetDouble(intensity);
}

// double contrastRadiantIntensity = <x>.ContrastRadiantIntensity();         // w/sr
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, ContrastRadiantIntensity, 0, "double", "")
{
   double intensity = 0.0;
   if ((aObjectPtr->mInfraredSig > 0.0) && (aObjectPtr->mSignalToNoise > 0.0))
   {
      intensity = aObjectPtr->mContrastRadiantIntensity;
   }
   aReturnVal.SetDouble(intensity);
}

// double detectionThreshold = <x>.DetectionThreshold();
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, DetectionThreshold, 0, "double", "")
{
   double threshold = -300.0;
   if (aObjectPtr->mDetectionThreshold > 0.0)
   {
      threshold = UtMath::LinearToDB(aObjectPtr->mDetectionThreshold);
   }
   aReturnVal.SetDouble(threshold);
}

// double pixelCount = <x>.PixelCount();
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_InteractionClass, WsfEM_Interaction, PixelCount, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->mPixelCount);
}
