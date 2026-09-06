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

#include "script/WsfScriptArticulatedPartClass.hpp"

#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_Util.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptContext.hpp"

namespace
{
void UpdateSensorCueingLimits(WsfArticulatedPart* aPartPtr)
{
   if (aPartPtr->GetPartType() == cCOMPONENT_ROLE<WsfSensor>())
   {
      WsfSensor* sensorPtr = dynamic_cast<WsfSensor*>(aPartPtr);
      if (sensorPtr != nullptr)
      {
         WsfSensorMode* modePtr = sensorPtr->GetCurrentMode();
         if (modePtr != nullptr)
         {
            modePtr->UpdateSensorCueingLimits();
         }
      }
   }
}
} // namespace

// =================================================================================================

WsfScriptArticulatedPartClass::WsfScriptArticulatedPartClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptPlatformPartClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfArticulatedPart");

   AddMethod(ut::make_unique<Pitch>());
   AddMethod(ut::make_unique<SetPitch>());
   AddMethod(ut::make_unique<Roll>());
   AddMethod(ut::make_unique<SetRoll>());
   AddMethod(ut::make_unique<Yaw>());
   AddMethod(ut::make_unique<SetYaw>());
   AddMethod(ut::make_unique<X>());
   AddMethod(ut::make_unique<Y>());
   AddMethod(ut::make_unique<Z>());
   AddMethod(ut::make_unique<DX>());
   AddMethod(ut::make_unique<DY>());
   AddMethod(ut::make_unique<DZ>());
   AddMethod(ut::make_unique<SetXYZ>());
   AddMethod(ut::make_unique<SetDXYZ>());
   AddMethod(ut::make_unique<Location>());

   AddMethod(ut::make_unique<CanSlewInAzimuth>());
   AddMethod(ut::make_unique<CanSlewInElevation>());
   AddMethod(ut::make_unique<SlewMinimumAzimuth>());
   AddMethod(ut::make_unique<SlewMaximumAzimuth>());
   AddMethod(ut::make_unique<SlewMinimumElevation>());
   AddMethod(ut::make_unique<SlewMaximumElevation>());
   AddMethod(ut::make_unique<IsSlewing>());

   AddMethod(ut::make_unique<CanCueInAzimuth>());
   AddMethod(ut::make_unique<CanCueInElevation>());
   AddMethod(ut::make_unique<CueMinimumAzimuth>());
   AddMethod(ut::make_unique<CueMaximumAzimuth>());
   AddMethod(ut::make_unique<CueMinimumElevation>());
   AddMethod(ut::make_unique<CueMaximumElevation>());

   AddMethod(ut::make_unique<ActualCuedAzimuth>());
   AddMethod(ut::make_unique<ActualCuedElevation>());
   AddMethod(ut::make_unique<CurrentCuedAzimuth>());
   AddMethod(ut::make_unique<CurrentCuedElevation>());

   AddMethod(ut::make_unique<ApparentLocationOf>());
   AddMethod(ut::make_unique<RelativeAzimuthOf>());
   AddMethod(ut::make_unique<RelativeElevationOf>());

   AddMethod(ut::make_unique<CueToLLA>());
   AddMethod(ut::make_unique<CueToWCS>());
   AddMethod(ut::make_unique<CueToWCS_2>("CueToWCS"));
   AddMethod(ut::make_unique<CueToPoint>());
   AddMethod(ut::make_unique<CueToTrack>());
   AddMethod(ut::make_unique<CueToAbsoluteAzEl>());
   AddMethod(ut::make_unique<CueToRelativeAzEl>());
   AddMethod(ut::make_unique<CueToRelativeAzEl>("CueToAzEl"));
   AddMethod(ut::make_unique<ClearCueing>());
   AddMethod(ut::make_unique<IsCued>());

   AddMethod(ut::make_unique<TimeToCueToLLA>());
   AddMethod(ut::make_unique<TimeToCueToWCS>());
   AddMethod(ut::make_unique<TimeToCueToWCS_2>("TimeToCueToWCS"));
   AddMethod(ut::make_unique<TimeToCueToPoint>());
   AddMethod(ut::make_unique<TimeToCueToTrack>());
   AddMethod(ut::make_unique<TimeToCueToAbsoluteAzEl>());
   AddMethod(ut::make_unique<TimeToCueToRelativeAzEl>());
   AddMethod(ut::make_unique<TimeToCueToRelativeAzEl>("TimeToCueToAzEl"));

   AddMethod(ut::make_unique<MaskingPatternState>());
   AddMethod(ut::make_unique<SetMaskingPatternState>());

   AddMethod(ut::make_unique<EnableArticulationUpdates>());
   AddMethod(ut::make_unique<DisableArticulationUpdates>());

   AddMethod(ut::make_unique<ConvertPCSVectorToWCS>());
   AddMethod(ut::make_unique<ConvertWCSVectorToPCS>());

   AddMethod(ut::make_unique<ConvertPCSVectorToECS>());
   AddMethod(ut::make_unique<ConvertECSVectorToPCS>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, Pitch, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   aReturnVal.SetDouble(aObjectPtr->GetPitch() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, SetPitch, 1, "void", "double")
{
   aObjectPtr->SetPitch(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, Roll, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   aReturnVal.SetDouble(aObjectPtr->GetRoll() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, SetRoll, 1, "void", "double")
{
   aObjectPtr->SetRoll(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, Yaw, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   aReturnVal.SetDouble(aObjectPtr->GetYaw() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, SetYaw, 1, "void", "double")
{
   aObjectPtr->SetYaw(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, X, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double p[3];
   aObjectPtr->GetLocation(p);
   aReturnVal.SetDouble(p[0]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, Y, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double p[3];
   aObjectPtr->GetLocation(p);
   aReturnVal.SetDouble(p[1]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, Z, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double p[3];
   aObjectPtr->GetLocation(p);
   aReturnVal.SetDouble(p[2]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, DX, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDX());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, DY, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDY());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, DZ, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDZ());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, SetXYZ, 3, "void", "double, double, double")
{
   double xyz[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   aObjectPtr->SetLocation(xyz);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, SetDXYZ, 3, "void", "double, double, double")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double dx = aVarArgs[0].GetDouble();
   double dy = aVarArgs[1].GetDouble();
   double dz = aVarArgs[2].GetDouble();
   aObjectPtr->SetPositionRateECS(dx, dy, dz);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, Location, 0, "Vec3", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double locationECS[3];
   aObjectPtr->GetLocation(locationECS);
   aReturnVal.SetPointer(UtScriptVec3::Create(locationECS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CanSlewInAzimuth, 0, "bool", "")
{
   bool canSlew = ((aObjectPtr->GetSlewMode() & WsfArticulatedPart::cSLEW_AZ) != 0);
   aReturnVal.SetBool(canSlew);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CanSlewInElevation, 0, "bool", "")
{
   bool canSlew = ((aObjectPtr->GetSlewMode() & WsfArticulatedPart::cSLEW_EL) != 0);
   aReturnVal.SetBool(canSlew);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, SlewMinimumAzimuth, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMinAzSlew() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, SlewMaximumAzimuth, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaxAzSlew() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, SlewMinimumElevation, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMinElSlew() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, SlewMaximumElevation, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaxElSlew() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, IsSlewing, 0, "bool", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   aReturnVal.SetBool(aObjectPtr->IsSlewing());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CanCueInAzimuth, 0, "bool", "")
{
   UpdateSensorCueingLimits(aObjectPtr);
   bool canCue = ((aObjectPtr->GetCueMode() & WsfArticulatedPart::cSLEW_AZ) != 0);
   aReturnVal.SetBool(canCue);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CanCueInElevation, 0, "bool", "")
{
   UpdateSensorCueingLimits(aObjectPtr);
   bool canCue = ((aObjectPtr->GetCueMode() & WsfArticulatedPart::cSLEW_EL) != 0);
   aReturnVal.SetBool(canCue);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueMinimumAzimuth, 0, "double", "")
{
   UpdateSensorCueingLimits(aObjectPtr);
   double value = 0.0;
   if ((aObjectPtr->GetCueMode() & WsfArticulatedPart::cSLEW_AZ) != 0)
   {
      value = std::max(aObjectPtr->GetMinAzCue(), aObjectPtr->GetMinAzSlew()) * UtMath::cDEG_PER_RAD;
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueMaximumAzimuth, 0, "double", "")
{
   UpdateSensorCueingLimits(aObjectPtr);
   double value = 0.0;
   if ((aObjectPtr->GetCueMode() & WsfArticulatedPart::cSLEW_AZ) != 0)
   {
      value = std::min(aObjectPtr->GetMaxAzCue(), aObjectPtr->GetMaxAzSlew()) * UtMath::cDEG_PER_RAD;
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueMinimumElevation, 0, "double", "")
{
   UpdateSensorCueingLimits(aObjectPtr);
   double value = 0.0;
   if ((aObjectPtr->GetCueMode() & WsfArticulatedPart::cSLEW_EL) != 0)
   {
      value = std::max(aObjectPtr->GetMinElCue(), aObjectPtr->GetMinElSlew()) * UtMath::cDEG_PER_RAD;
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueMaximumElevation, 0, "double", "")
{
   UpdateSensorCueingLimits(aObjectPtr);
   double value = 0.0;
   if ((aObjectPtr->GetCueMode() & WsfArticulatedPart::cSLEW_EL) != 0)
   {
      value = std::min(aObjectPtr->GetMaxElCue(), aObjectPtr->GetMaxElSlew()) * UtMath::cDEG_PER_RAD;
   }
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, ActualCuedAzimuth, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double cuedAz, cuedEl;
   aObjectPtr->GetActualCuedOrientation(cuedAz, cuedEl);
   aReturnVal.SetDouble(cuedAz * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, ActualCuedElevation, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double cuedAz, cuedEl;
   aObjectPtr->GetActualCuedOrientation(cuedAz, cuedEl);
   aReturnVal.SetDouble(cuedEl * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CurrentCuedAzimuth, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double cuedAz, cuedEl;
   aObjectPtr->GetCurrentCuedOrientation(cuedAz, cuedEl);
   aReturnVal.SetDouble(cuedAz * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CurrentCuedElevation, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double cuedAz, cuedEl;
   aObjectPtr->GetCurrentCuedOrientation(cuedAz, cuedEl);
   aReturnVal.SetDouble(cuedEl * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, ApparentLocationOf, 2, "WsfGeoPoint", "WsfGeoPoint, double")
{
   WsfGeoPoint* pointPtr         = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       earthRadiusScale = aVarArgs[1].GetDouble();
   double       srcLocWCS[3];
   aObjectPtr->GetLocationWCS(srcLocWCS);
   double tgtLocWCS[3];
   pointPtr->GetLocationWCS(tgtLocWCS);
   WsfEM_Util::ComputeApparentPosition(earthRadiusScale, srcLocWCS, tgtLocWCS, srcLocWCS, tgtLocWCS);
   pointPtr = new WsfGeoPoint(tgtLocWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// Compute azimuth of a point relative to the current cue.
UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, RelativeAzimuthOf, 1, "double", "WsfGeoPoint")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double       otherLocWCS[3];
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   pointPtr->GetLocationWCS(otherLocWCS);
   double relLocWCS[3];
   aObjectPtr->GetRelativeLocationWCS(otherLocWCS, relLocWCS);
   double relLocPCS[3];
   aObjectPtr->ConvertWCSVectorToPCS(relLocPCS, relLocWCS);

   // From UtEntity::ComputeAzimuthAndElevation (azimuth part only)
   double x       = relLocPCS[0];
   double y       = relLocPCS[1];
   double azimuth = atan2(y, x);
   aReturnVal.SetDouble(azimuth * UtMath::cDEG_PER_RAD);
}

// Compute elevation of a point relative to the current cue.
UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, RelativeElevationOf, 1, "double", "WsfGeoPoint")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdatePosition(simTime);
   double       otherLocWCS[3];
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   pointPtr->GetLocationWCS(otherLocWCS);
   double relLocWCS[3];
   aObjectPtr->GetRelativeLocationWCS(otherLocWCS, relLocWCS);
   double relLocPCS[3];
   aObjectPtr->ConvertWCSVectorToPCS(relLocPCS, relLocWCS);

   // From UtEntity::ComputeAzimuthAndElevation (elevation part only)
   double x         = relLocPCS[0];
   double y         = relLocPCS[1];
   double z         = relLocPCS[2];
   double elevation = 0.0;
   double xy        = sqrt((x * x) + (y * y));
   if (xy != 0.0)
   {
      elevation = -atan2(z, xy);
   }
   else
   {
      elevation = UtMath::cPI_OVER_2;
      if (z > 0.0)
      {
         elevation = -UtMath::cPI_OVER_2;
      }
   }
   aReturnVal.SetDouble(elevation * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueToLLA, 3, "bool", "double, double, double")
{
   double lat = aVarArgs[0].GetDouble();
   double lon = aVarArgs[1].GetDouble();
   double alt = aVarArgs[2].GetDouble();
   double cueWCS[3];
   double relLocWCS[3];
   UtEntity::ConvertLLAToWCS(lat, lon, alt, cueWCS);
   double azimuth   = 0.0;
   double elevation = 0.0;


   aObjectPtr->SetCuedLocationWCS(cueWCS);

   aObjectPtr->GetRelativeLocationWCS(cueWCS, relLocWCS);
   aObjectPtr->ComputeAspect(relLocWCS, azimuth, elevation);
   aReturnVal.SetBool(aObjectPtr->WithinSlewLimits(azimuth, elevation));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueToWCS, 3, "bool", "double, double, double")
{
   double cueWCS[3];
   double relLocWCS[3];
   double azimuth   = 0.0;
   double elevation = 0.0;
   cueWCS[0]        = aVarArgs[0].GetDouble();
   cueWCS[1]        = aVarArgs[1].GetDouble();
   cueWCS[2]        = aVarArgs[2].GetDouble();

   aObjectPtr->SetCuedLocationWCS(cueWCS);

   aObjectPtr->GetRelativeLocationWCS(cueWCS, relLocWCS);
   aObjectPtr->ComputeAspect(relLocWCS, azimuth, elevation);
   aReturnVal.SetBool(aObjectPtr->WithinSlewLimits(azimuth, elevation));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueToWCS_2, 1, "bool", "Vec3")
{
   UtVec3d* vec3Ptr = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->SetCuedLocationWCS(vec3Ptr->GetData());
   double relLocWCS[3];
   double azimuth   = 0.0;
   double elevation = 0.0;

   aObjectPtr->GetRelativeLocationWCS(vec3Ptr->GetData(), relLocWCS);
   aObjectPtr->ComputeAspect(relLocWCS, azimuth, elevation);
   aReturnVal.SetBool(aObjectPtr->WithinSlewLimits(azimuth, elevation));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueToPoint, 1, "bool", "WsfGeoPoint")
{
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       locationWCS[3];
   double       relLocWCS[3];
   double       azimuth   = 0.0;
   double       elevation = 0.0;
   pointPtr->GetLocationWCS(locationWCS);

   aObjectPtr->SetCuedLocationWCS(locationWCS);

   aObjectPtr->GetRelativeLocationWCS(locationWCS, relLocWCS);
   aObjectPtr->ComputeAspect(relLocWCS, azimuth, elevation);
   aReturnVal.SetBool(aObjectPtr->WithinSlewLimits(azimuth, elevation));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueToTrack, 1, "bool", "WsfTrack")
{
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   double    locationWCS[3];
   double    relLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, locationWCS);
   double azimuth   = 0.0;
   double elevation = 0.0;

   aObjectPtr->SetCuedLocationWCS(locationWCS);
   aObjectPtr->GetRelativeLocationWCS(locationWCS, relLocWCS);
   aObjectPtr->ComputeAspect(relLocWCS, azimuth, elevation);
   aReturnVal.SetBool(aObjectPtr->WithinSlewLimits(azimuth, elevation));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueToAbsoluteAzEl, 2, "bool", "double, double")
{
   double az = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   double el = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   double heading, pitch, roll;
   aObjectPtr->GetPlatform()->GetOrientationNED(heading, pitch, roll);
   double ned[3] = {heading, 0, 0};
   aObjectPtr->GetAbsoluteCuedOrientation(az, el, ned);
   aObjectPtr->SetCuedOrientation(az, el);
   aReturnVal.SetBool(aObjectPtr->WithinSlewLimits(az, el));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, CueToRelativeAzEl, 2, "bool", "double, double")
{
   double relAz = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   double relEl = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   aObjectPtr->SetCuedOrientation(relAz, relEl);
   aReturnVal.SetBool(aObjectPtr->WithinSlewLimits(relAz, relEl));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, ClearCueing, 0, "void", "")
{
   aObjectPtr->ClearCueing();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, IsCued, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsCued());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, MaskingPatternState, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetMaskingPatternState());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, SetMaskingPatternState, 1, "void", "string")
{
   aObjectPtr->SetMaskingPatternState(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, EnableArticulationUpdates, 1, "void", "double")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->EnableArticulationUpdates(simTime, aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, DisableArticulationUpdates, 0, "void", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->DisableArticulationUpdates(simTime);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, ConvertPCSVectorToWCS, 1, "Vec3", "Vec3")
{
   UtVec3d* vec3Ptr = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   double   vecWCS[3];
   aObjectPtr->ConvertPCSVectorToWCS(vecWCS, vec3Ptr->GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(vecWCS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, ConvertWCSVectorToPCS, 1, "Vec3", "Vec3")
{
   UtVec3d* vec3Ptr = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   double   vecPCS[3];
   aObjectPtr->ConvertWCSVectorToPCS(vecPCS, vec3Ptr->GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(vecPCS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, ConvertPCSVectorToECS, 1, "Vec3", "Vec3")
{
   UtVec3d* vec3Ptr = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   double   vecECS[3];
   aObjectPtr->ConvertPCSVectorToECS(vecECS, vec3Ptr->GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(vecECS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, ConvertECSVectorToPCS, 1, "Vec3", "Vec3")
{
   UtVec3d* vec3Ptr = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   double   vecPCS[3];
   aObjectPtr->ConvertECSVectorToPCS(vecPCS, vec3Ptr->GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(vecPCS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, TimeToCueToLLA, 3, "double", "double, double, double")
{
   double  lat = aVarArgs[0].GetDouble();
   double  lon = aVarArgs[1].GetDouble();
   double  alt = aVarArgs[2].GetDouble();
   UtVec3d cueWCS;
   UtVec3d relLocWCS;
   double  azimuth   = 0.0;
   double  elevation = 0.0;

   UtEntity::ConvertLLAToWCS(lat, lon, alt, cueWCS.GetData());

   aObjectPtr->GetRelativeLocationWCS(cueWCS.GetData(), relLocWCS.GetData());
   aObjectPtr->ComputeAspect(relLocWCS.GetData(), azimuth, elevation);

   double timeToCue(-1.0);
   if (aObjectPtr->WithinSlewLimits(azimuth, elevation))
   {
      timeToCue = aObjectPtr->TimeToCue(azimuth, elevation);
   }

   aReturnVal.SetDouble(timeToCue);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, TimeToCueToWCS, 3, "double", "double, double, double")
{
   double cueWCS[3];
   double relLocWCS[3];
   double azimuth   = 0.0;
   double elevation = 0.0;
   cueWCS[0]        = aVarArgs[0].GetDouble();
   cueWCS[1]        = aVarArgs[1].GetDouble();
   cueWCS[2]        = aVarArgs[2].GetDouble();

   aObjectPtr->GetRelativeLocationWCS(cueWCS, relLocWCS);
   aObjectPtr->ComputeAspect(relLocWCS, azimuth, elevation);

   double timeToCue(-1.0);
   if (aObjectPtr->WithinSlewLimits(azimuth, elevation))
   {
      timeToCue = aObjectPtr->TimeToCue(azimuth, elevation);
   }

   aReturnVal.SetDouble(timeToCue);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, TimeToCueToWCS_2, 1, "double", "Vec3")
{
   auto   vec3Ptr = aVarArgs[0].GetPointer()->GetAppObject<UtVec3d>();
   double relLocWCS[3];
   double azimuth   = 0.0;
   double elevation = 0.0;

   aObjectPtr->GetRelativeLocationWCS(vec3Ptr->GetData(), relLocWCS);
   aObjectPtr->ComputeAspect(relLocWCS, azimuth, elevation);

   double timeToCue(-1.0);
   if (aObjectPtr->WithinSlewLimits(azimuth, elevation))
   {
      timeToCue = aObjectPtr->TimeToCue(azimuth, elevation);
   }

   aReturnVal.SetDouble(timeToCue);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, TimeToCueToPoint, 1, "double", "WsfGeoPoint")
{
   auto   pointPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfGeoPoint>();
   double locationWCS[3];
   double relLocWCS[3];
   double azimuth   = 0.0;
   double elevation = 0.0;
   pointPtr->GetLocationWCS(locationWCS);

   aObjectPtr->GetRelativeLocationWCS(locationWCS, relLocWCS);
   aObjectPtr->ComputeAspect(relLocWCS, azimuth, elevation);

   double timeToCue(-1.0);
   if (aObjectPtr->WithinSlewLimits(azimuth, elevation))
   {
      timeToCue = aObjectPtr->TimeToCue(azimuth, elevation);
   }

   aReturnVal.SetDouble(timeToCue);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, TimeToCueToTrack, 1, "double", "WsfTrack")
{
   auto   trackPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfTrack>();
   double simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   double locationWCS[3];
   double relLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, locationWCS);
   double azimuth   = 0.0;
   double elevation = 0.0;

   aObjectPtr->GetRelativeLocationWCS(locationWCS, relLocWCS);
   aObjectPtr->ComputeAspect(relLocWCS, azimuth, elevation);

   double timeToCue(-1.0);
   if (aObjectPtr->WithinSlewLimits(azimuth, elevation))
   {
      timeToCue = aObjectPtr->TimeToCue(azimuth, elevation);
   }

   aReturnVal.SetDouble(timeToCue);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, TimeToCueToAbsoluteAzEl, 2, "double", "double, double")
{
   double azimuth   = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   double elevation = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   double heading, pitch, roll;
   aObjectPtr->GetPlatform()->GetOrientationNED(heading, pitch, roll);
   double ned[3] = {heading, 0, 0};
   aObjectPtr->GetAbsoluteCuedOrientation(azimuth, elevation, ned);

   double timeToCue(-1.0);
   if (aObjectPtr->WithinSlewLimits(azimuth, elevation))
   {
      timeToCue = aObjectPtr->TimeToCue(azimuth, elevation);
   }

   aReturnVal.SetDouble(timeToCue);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptArticulatedPartClass, WsfArticulatedPart, TimeToCueToRelativeAzEl, 2, "double", "double, double")
{
   double azimuth   = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   double elevation = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;

   double timeToCue(-1.0);

   if (aObjectPtr->WithinSlewLimits(azimuth, elevation))
   {
      timeToCue = aObjectPtr->TimeToCue(azimuth, elevation);
   }

   aReturnVal.SetDouble(timeToCue);
}
