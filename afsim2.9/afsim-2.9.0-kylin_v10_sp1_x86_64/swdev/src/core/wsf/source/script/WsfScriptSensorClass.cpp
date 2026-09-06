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

#include "script/WsfScriptSensorClass.hpp"

#include <algorithm>
#include <limits>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfFieldOfView.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfMode.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSensorTypes.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptContext.hpp"

namespace
{
void GetScanLimits(WsfSensor* aSensorPtr, double& aMinAz, double& aMaxAz, double& aMinEl, double& aMaxEl)
{
   aMinAz = std::numeric_limits<double>::max();
   aMaxAz = -aMinAz;
   aMinEl = std::numeric_limits<double>::max();
   aMaxEl = -aMinEl;

   double minValue, maxValue;
   size_t rcvrCount = aSensorPtr->GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = aSensorPtr->GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr)
      {
         WsfEM_Antenna::ScanMode scanMode = antennaPtr->GetScanMode();

         minValue = maxValue = 0.0;
         if ((scanMode & WsfEM_Antenna::cSCAN_AZ) != 0)
         {
            antennaPtr->GetAzimuthScanLimits(minValue, maxValue);
         }
         aMinAz = std::min(minValue, aMinAz);
         aMaxAz = std::max(maxValue, aMaxAz);

         minValue = maxValue = 0.0;
         if ((scanMode & WsfEM_Antenna::cSCAN_EL) != 0)
         {
            antennaPtr->GetElevationScanLimits(minValue, maxValue);
         }
         aMinEl = std::min(minValue, aMinEl);
         aMaxEl = std::max(maxValue, aMaxEl);
      }
   }
}

int GetScanMode(WsfSensor* aSensorPtr)
{
   int    scanMode  = 0;
   size_t rcvrCount = aSensorPtr->GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = aSensorPtr->GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr)
      {
         scanMode |= antennaPtr->GetScanMode();
      }
   }
   return scanMode;
}

void GetFOV_Limits(WsfSensor* aSensorPtr, double& aMinAz, double& aMaxAz, double& aMinEl, double& aMaxEl)
{
   aMinAz = std::numeric_limits<double>::max();
   aMaxAz = -aMinAz;
   aMinEl = std::numeric_limits<double>::max();
   aMaxEl = -aMinEl;

   double minValue, maxValue;
   size_t rcvrCount = aSensorPtr->GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = aSensorPtr->GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr != nullptr)
      {
         antennaPtr->GetAzimuthFieldOfView(minValue, maxValue);
         aMinAz = std::min(minValue, aMinAz);
         aMaxAz = std::max(maxValue, aMaxAz);
         antennaPtr->GetElevationFieldOfView(minValue, maxValue);
         aMinEl = std::min(minValue, aMinEl);
         aMaxEl = std::max(maxValue, aMaxEl);
      }
   }
}

void GetRangeLimits(WsfSensor* aSensorPtr, double& aMinRange, double& aMaxRange)
{
   aMinRange = std::numeric_limits<double>::max();
   aMaxRange = -aMinRange;

   double minValue, maxValue;
   size_t rcvrCount = aSensorPtr->GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = aSensorPtr->GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr != nullptr)
      {
         antennaPtr->GetRangeLimits(minValue, maxValue);
         aMinRange = std::min(minValue, aMinRange);
         aMaxRange = std::max(maxValue, aMaxRange);
      }
   }
}
} // namespace

WsfScriptSensorClass::WsfScriptSensorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptArticulatedPartClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSensor");

   AddStaticMethod(ut::make_unique<IsA_TypeOf>());

   AddMethod(ut::make_unique<TurnOff>());
   AddMethod(ut::make_unique<TurnOn>());

   AddMethod(ut::make_unique<ModeCount>());
   AddMethod(ut::make_unique<ModeName>());
   AddMethod(ut::make_unique<CurrentMode>());
   AddMethod(ut::make_unique<SelectMode>());
   AddMethod(ut::make_unique<DeselectMode>());
   AddMethod(ut::make_unique<BeamCount_1>("BeamCount"));
   AddMethod(ut::make_unique<BeamCount_2>("BeamCount"));
   AddMethod(ut::make_unique<BeamCount_3>("BeamCount"));

   AddMethod(ut::make_unique<FOV_1>("FOV"));
   AddMethod(ut::make_unique<FOV_2>("FOV"));
   AddMethod(ut::make_unique<FOV_3>("FOV"));
   AddMethod(ut::make_unique<FOV_MinimumAzimuth>());
   AddMethod(ut::make_unique<FOV_MaximumAzimuth>());
   AddMethod(ut::make_unique<FOV_MinimumElevation>());
   AddMethod(ut::make_unique<FOV_MaximumElevation>());
   AddMethod(ut::make_unique<FOV_MinimumRange>());
   AddMethod(ut::make_unique<FOV_MaximumRange>());

   AddMethod(ut::make_unique<SetFOV_1>("SetFOV"));
   AddMethod(ut::make_unique<SetFOV_2>("SetFOV"));
   AddMethod(ut::make_unique<SetFOV_3>("SetFOV"));
   AddMethod(ut::make_unique<SetFOV_Azimuth>());
   AddMethod(ut::make_unique<SetFOV_Elevation>());
   AddMethod(ut::make_unique<SetFOV_Range>());

   AddMethod(ut::make_unique<CanScanInAzimuth>());
   AddMethod(ut::make_unique<CanScanInElevation>());
   AddMethod(ut::make_unique<ScanMinimumAzimuth>());
   AddMethod(ut::make_unique<ScanMaximumAzimuth>());
   AddMethod(ut::make_unique<ScanMinimumElevation>());
   AddMethod(ut::make_unique<ScanMaximumElevation>());

   AddMethod(ut::make_unique<WithinFieldOfView>());

   AddMethod(ut::make_unique<ActiveTrackCount>());
   AddMethod(ut::make_unique<MaximumTrackCount>());
   AddMethod(ut::make_unique<TrackQuality_1>("TrackQuality"));
   AddMethod(ut::make_unique<TrackQuality_2>("TrackQuality"));
   AddMethod(ut::make_unique<TrackQuality_3>("TrackQuality"));

   AddMethod(ut::make_unique<ActiveRequestCount_1>("ActiveRequestCount"));
   AddMethod(ut::make_unique<ActiveRequestCount_2>("ActiveRequestCount"));
   AddMethod(ut::make_unique<MaximumRequestCount_1>("MaximumRequestCount"));
   AddMethod(ut::make_unique<MaximumRequestCount_2>("MaximumRequestCount"));
   AddMethod(ut::make_unique<HaveRequestFor>());
   AddMethod(ut::make_unique<StartTracking>());
   AddMethod(ut::make_unique<StopTracking>());
   AddMethod(ut::make_unique<FrameTime>());

   AddMethod(ut::make_unique<Xmtr_1>("Xmtr"));
   AddMethod(ut::make_unique<Xmtr_2>("Xmtr"));
   AddMethod(ut::make_unique<XmtrCount_1>("XmtrCount"));
   AddMethod(ut::make_unique<XmtrCount_2>("XmtrCount"));

   AddMethod(ut::make_unique<Rcvr_1>("Rcvr"));
   AddMethod(ut::make_unique<Rcvr_2>("Rcvr"));
   AddMethod(ut::make_unique<RcvrCount_1>("RcvrCount"));
   AddMethod(ut::make_unique<RcvrCount_2>("RcvrCount"));
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfScriptSensorClass, IsA_TypeOf, 2, "bool", "string, string")
{
   bool       isA_TypeOf = false;
   WsfSensor* objectPtr  = WsfSensorTypes::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (objectPtr != nullptr)
   {
      isA_TypeOf = objectPtr->IsA_TypeOf(aVarArgs[1].GetString());
   }
   aReturnVal.SetBool(isA_TypeOf);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptSensorClass, WsfSensor, TurnOff, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOff(simTime, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptSensorClass, WsfSensor, TurnOn, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOn(simTime, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, ModeCount, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetModeCount()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, ModeName, 1, "string", "int")
{
   aReturnVal.SetString(aObjectPtr->GetModeName(aVarArgs[0].GetInt()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, CurrentMode, 0, "string", "")
{
   WsfSensorModeList* modeListPtr = aObjectPtr->GetModeList();
   if ((modeListPtr != nullptr) && modeListPtr->CanMultiSelect())
   {
      aReturnVal.SetString("<multi_select>");
   }
   else
   {
      aReturnVal.SetString(aObjectPtr->GetCurrentModeName().GetString());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, SelectMode, 1, "void", "string")
{
   double simTime = TIME_NOW;
   aObjectPtr->SelectMode(simTime, aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, DeselectMode, 1, "void", "string")
{
   double simTime = TIME_NOW;
   aObjectPtr->DeselectMode(simTime, aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, BeamCount_1, 1, "int", "int")
{
   int                  modeNum   = aVarArgs[0].GetInt();
   const WsfSensorMode* modePtr   = aObjectPtr->GetModeEntry(modeNum);
   int                  beamCount = 0;
   if (modePtr != nullptr)
   {
      beamCount = static_cast<int>(modePtr->GetBeamCount());
   }
   aReturnVal.SetInt(beamCount);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, BeamCount_2, 1, "int", "string")
{
   std::string          modeName  = aVarArgs[0].GetString();
   const WsfSensorMode* modePtr   = aObjectPtr->GetModeEntry(aObjectPtr->GetModeIndex(modeName));
   int                  beamCount = 0;
   if (modePtr != nullptr)
   {
      beamCount = static_cast<int>(modePtr->GetBeamCount());
   }
   aReturnVal.SetInt(beamCount);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, BeamCount_3, 0, "int", "")
{
   const WsfSensorMode* modePtr   = aObjectPtr->GetCurrentMode();
   int                  beamCount = 0;
   if (modePtr)
   {
      beamCount = static_cast<int>(modePtr->GetBeamCount());
   }
   aReturnVal.SetInt(beamCount);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, FOV_1, 0, "WsfFieldOfView", "")
{
   auto fovPtr = aObjectPtr->GetEM_Rcvr(0).GetAntenna()->GetFieldOfView();
   if (fovPtr != nullptr)
   {
      aReturnVal.SetPointer(new UtScriptRef(fovPtr->Clone(), aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      UT_SCRIPT_ABORT("No field of view is defined for this sensor");
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, FOV_2, 1, "WsfFieldOfView", "int")
{
   auto rcvrNum = static_cast<size_t>(aVarArgs[0].GetInt());
   if (rcvrNum < aObjectPtr->GetEM_RcvrCount())
   {
      auto fovPtr = aObjectPtr->GetEM_Rcvr(rcvrNum).GetAntenna()->GetFieldOfView();
      if (fovPtr != nullptr)
      {
         aReturnVal.SetPointer(new UtScriptRef(fovPtr->Clone(), aReturnClassPtr, UtScriptRef::cMANAGE));
      }
      else
      {
         UT_SCRIPT_ABORT("No field of view is defined for this sensor");
      }
   }
   else
   {
      UT_SCRIPT_ABORT("The receiver number is invalid");
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, FOV_3, 2, "WsfFieldOfView", "int, int")
{
   auto modeNum = static_cast<size_t>(aVarArgs[0].GetInt());
   if (modeNum < aObjectPtr->GetModeCount())
   {
      auto rcvrNum = static_cast<size_t>(aVarArgs[1].GetInt());
      if (rcvrNum < aObjectPtr->GetEM_RcvrCount(modeNum))
      {
         auto fovPtr = aObjectPtr->GetEM_Rcvr(modeNum, rcvrNum).GetAntenna()->GetFieldOfView();
         if (fovPtr != nullptr)
         {
            aReturnVal.SetPointer(new UtScriptRef(fovPtr->Clone(), aReturnClassPtr, UtScriptRef::cMANAGE));
         }
         else
         {
            UT_SCRIPT_ABORT("No field of view is defined for this sensor");
         }
      }
      else
      {
         UT_SCRIPT_ABORT("The receiver number is invalid");
      }
   }
   else
   {
      UT_SCRIPT_ABORT("The mode number is invalid");
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, FOV_MinimumAzimuth, 0, "double", "")
{
   double minAz, maxAz, minEl, maxEl;
   GetFOV_Limits(aObjectPtr, minAz, maxAz, minEl, maxEl);
   aReturnVal.SetDouble(minAz * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, FOV_MaximumAzimuth, 0, "double", "")
{
   double minAz, maxAz, minEl, maxEl;
   GetFOV_Limits(aObjectPtr, minAz, maxAz, minEl, maxEl);
   aReturnVal.SetDouble(maxAz * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, FOV_MinimumElevation, 0, "double", "")
{
   double minAz, maxAz, minEl, maxEl;
   GetFOV_Limits(aObjectPtr, minAz, maxAz, minEl, maxEl);
   aReturnVal.SetDouble(minEl * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, FOV_MaximumElevation, 0, "double", "")
{
   double minAz, maxAz, minEl, maxEl;
   GetFOV_Limits(aObjectPtr, minAz, maxAz, minEl, maxEl);
   aReturnVal.SetDouble(maxEl * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, FOV_MinimumRange, 0, "double", "")
{
   double minR, maxR;
   GetRangeLimits(aObjectPtr, minR, maxR);
   aReturnVal.SetDouble(minR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, FOV_MaximumRange, 0, "double", "")
{
   double minR, maxR;
   GetRangeLimits(aObjectPtr, minR, maxR);
   aReturnVal.SetDouble(maxR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, SetFOV_1, 1, "void", "WsfFieldOfView")
{
   auto   fovPtr    = aVarArgs[0].GetPointer()->GetAppObject<WsfFieldOfView>();
   size_t rcvrCount = aObjectPtr->GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = aObjectPtr->GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr != nullptr)
      {
         antennaPtr->SetFieldOfView(ut::clone(fovPtr));
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, SetFOV_2, 2, "void", "WsfFieldOfView, int")
{
   auto fovPtr  = aVarArgs[0].GetPointer()->GetAppObject<WsfFieldOfView>();
   auto rcvrNum = static_cast<size_t>(aVarArgs[1].GetInt());
   if (rcvrNum < aObjectPtr->GetEM_RcvrCount())
   {
      WsfEM_Antenna* antennaPtr = aObjectPtr->GetEM_Rcvr(rcvrNum).GetAntenna();
      if (antennaPtr != nullptr)
      {
         antennaPtr->SetFieldOfView(ut::clone(fovPtr));
      }
   }
   else
   {
      UT_SCRIPT_ABORT("The receiver number is invalid");
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, SetFOV_3, 3, "void", "WsfFieldOfView, int, int")
{
   auto fovPtr  = aVarArgs[0].GetPointer()->GetAppObject<WsfFieldOfView>();
   auto modeNum = static_cast<size_t>(aVarArgs[1].GetInt());
   if (modeNum < aObjectPtr->GetModeCount())
   {
      auto rcvrNum = static_cast<size_t>(aVarArgs[2].GetInt());
      if (rcvrNum < aObjectPtr->GetEM_RcvrCount(modeNum))
      {
         WsfEM_Antenna* antennaPtr = aObjectPtr->GetEM_Rcvr(modeNum, rcvrNum).GetAntenna();
         if (antennaPtr != nullptr)
         {
            antennaPtr->SetFieldOfView(ut::clone(fovPtr));
         }
      }
      else
      {
         UT_SCRIPT_ABORT("The receiver number is invalid");
      }
   }
   else
   {
      UT_SCRIPT_ABORT("The mode number is invalid");
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, SetFOV_Azimuth, 2, "void", "double, double")
{
   double minAz = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   double maxAz = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   if (minAz > maxAz)
   {
      std::swap(minAz, maxAz);
   }
   size_t rcvrCount = aObjectPtr->GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = aObjectPtr->GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr != nullptr)
      {
         antennaPtr->SetAzimuthFieldOfView(minAz, maxAz);
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, SetFOV_Elevation, 2, "void", "double, double")
{
   double minEl = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   double maxEl = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   if (minEl > maxEl)
   {
      std::swap(minEl, maxEl);
   }
   size_t rcvrCount = aObjectPtr->GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = aObjectPtr->GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr)
      {
         antennaPtr->SetElevationFieldOfView(minEl, maxEl);
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, SetFOV_Range, 2, "void", "double, double")
{
   double minRange = aVarArgs[0].GetDouble();
   double maxRange = aVarArgs[1].GetDouble();
   if (minRange > maxRange)
   {
      std::swap(minRange, maxRange);
   }
   size_t rcvrCount = aObjectPtr->GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = aObjectPtr->GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr)
      {
         antennaPtr->SetRangeLimits(minRange, maxRange);
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, CanScanInAzimuth, 0, "bool", "")
{
   int scanMode = GetScanMode(aObjectPtr);
   aReturnVal.SetBool((scanMode & WsfEM_Antenna::cSCAN_AZ) != 0);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, CanScanInElevation, 0, "bool", "")
{
   int scanMode = GetScanMode(aObjectPtr);
   aReturnVal.SetBool((scanMode & WsfEM_Antenna::cSCAN_EL) != 0);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, ScanMinimumAzimuth, 0, "double", "")
{
   double minAz, maxAz, minEl, maxEl;
   GetScanLimits(aObjectPtr, minAz, maxAz, minEl, maxEl);
   aReturnVal.SetDouble(minAz * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, ScanMaximumAzimuth, 0, "double", "")
{
   double minAz, maxAz, minEl, maxEl;
   GetScanLimits(aObjectPtr, minAz, maxAz, minEl, maxEl);
   aReturnVal.SetDouble(maxAz * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, ScanMinimumElevation, 0, "double", "")
{
   double minAz, maxAz, minEl, maxEl;
   GetScanLimits(aObjectPtr, minAz, maxAz, minEl, maxEl);
   aReturnVal.SetDouble(minEl * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, ScanMaximumElevation, 0, "double", "")
{
   double minAz, maxAz, minEl, maxEl;
   GetScanLimits(aObjectPtr, minAz, maxAz, minEl, maxEl);
   aReturnVal.SetDouble(maxEl * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, WithinFieldOfView, 1, "bool", "WsfGeoPoint")
{
   double simTime = TIME_NOW;
   aObjectPtr->UpdatePosition(simTime);

   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       tgtLocWCS[3];
   pointPtr->GetLocationWCS(tgtLocWCS);

   double earthRadiusMultiplier = 1.0;
   if (aObjectPtr->GetEM_XmtrCount() > 0)
   {
      earthRadiusMultiplier = aObjectPtr->GetEM_Xmtr(0).GetEarthRadiusMultiplier();
   }

   // This was copied from WsfSensor::WithinFieldOfView.
   bool   withinFieldOfView(false);
   size_t rcvrCount = aObjectPtr->GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = aObjectPtr->GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr != nullptr)
      {
         // Compute the range to the target
         double thisLocWCS[3];
         double thisToTgtLocWCS[3];
         antennaPtr->GetLocationWCS(thisLocWCS);
         UtVec3d::Subtract(thisToTgtLocWCS, tgtLocWCS, thisLocWCS);
         double range = UtVec3d::Magnitude(thisToTgtLocWCS);
         if (antennaPtr->WithinRange(range))
         {
            // WithinFieldOfView output
            double trueThisToTgtAz(0.0);
            double trueThisToTgtEl(0.0);
            double apparentThisToTgtLocWCS[3] = {0.0};
            double apparentThisToTgtAz(0.0);
            double apparentThisToTgtEl(0.0);
            double apparentTgtToThisLocWCS[3] = {0.0};

            if (antennaPtr->WithinFieldOfView(tgtLocWCS,
                                              earthRadiusMultiplier,
                                              trueThisToTgtAz,
                                              trueThisToTgtEl,
                                              apparentThisToTgtLocWCS,
                                              apparentThisToTgtAz,
                                              apparentThisToTgtEl,
                                              apparentTgtToThisLocWCS))
            {
               withinFieldOfView = true;
               break;
            }
         }
      }
   }
   aReturnVal.SetBool(withinFieldOfView);
}

//! int count = ActiveTrackCount();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, ActiveTrackCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetActiveTrackCount());
}

//! int count = MaximumTrackCount();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, MaximumTrackCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetMaximumTrackCount());
}

//! double quality = TrackQuality();  uses the current mode
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, TrackQuality_1, 0, "double", "")
{
   const WsfSensorMode* modePtr      = aObjectPtr->GetCurrentMode();
   double               trackQuality = -1.0;
   if (modePtr)
   {
      trackQuality = modePtr->GetTrackQuality();
   }
   aReturnVal.SetDouble(trackQuality);
}

//! double quality = TrackQuality(string aModeName);  uses the mode with the given name
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, TrackQuality_2, 1, "double", "string")
{
   std::string          modeName     = aVarArgs[0].GetString();
   const WsfSensorMode* modePtr      = aObjectPtr->GetModeEntry(aObjectPtr->GetModeIndex(modeName));
   double               trackQuality = -1.0;
   if (modePtr)
   {
      trackQuality = modePtr->GetTrackQuality();
   }
   aReturnVal.SetDouble(trackQuality);
}

//! double quality = TrackQuality(int aModeIndex);  uses the mode with the given index
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, TrackQuality_3, 1, "double", "int")
{
   int                  modeNum      = aVarArgs[0].GetInt();
   const WsfSensorMode* modePtr      = aObjectPtr->GetModeEntry(modeNum);
   double               trackQuality = -1.0;
   if (modePtr)
   {
      trackQuality = modePtr->GetTrackQuality();
   }
   aReturnVal.SetDouble(trackQuality);
}

//! int count = ActiveRequestCount(string aModeName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, ActiveRequestCount_1, 1, "int", "string")
{
   aReturnVal.SetInt(aObjectPtr->GetActiveRequestCount(aVarArgs[0].GetString()));
}

//! int count = ActiveRequestCount();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, ActiveRequestCount_2, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetActiveRequestCount());
}

//! int count = MaximumRequestCount(string aModeName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, MaximumRequestCount_1, 1, "int", "string")
{
   aReturnVal.SetInt(aObjectPtr->GetMaximumRequestCount(aVarArgs[0].GetString()));
}

//! int count = MaximumRequestCount();
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, MaximumRequestCount_2, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetMaximumRequestCount());
}

//! bool haveTrackRequest = HaveRequestFor(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, HaveRequestFor, 1, "bool", "WsfTrackId")
{
   WsfTrackId* trackIdPtr = (WsfTrackId*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->HaveRequestFor(*trackIdPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, StartTracking, 2, "bool", "WsfTrack, string")
{
   // Argument 0: the initial track for cuing the sensor
   // Argument 1: the sensor mode to be used
   WsfTrack*   trackPtr        = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfStringId modeNameId      = aVarArgs[1].GetString();
   double      simTime         = TIME_NOW;
   bool        startedTracking = aObjectPtr->StartTracking(simTime, *trackPtr, modeNameId);
   aReturnVal.SetBool(startedTracking);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, StopTracking, 1, "bool", "WsfTrackId")
{
   // Argument 0: The ID of the track.
   WsfTrackId* trackIdPtr      = (WsfTrackId*)aVarArgs[0].GetPointer()->GetAppObject();
   double      simTime         = TIME_NOW;
   bool        stoppedTracking = aObjectPtr->StopTracking(simTime, *trackIdPtr);
   aReturnVal.SetBool(stoppedTracking);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, FrameTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCurrentMode()->GetFrameTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, Xmtr_1, 1, "WsfEM_Xmtr", "int")
{
   int index = aVarArgs[0].GetInt();
   aReturnVal.SetPointer(new UtScriptRef(&aObjectPtr->GetEM_Xmtr(index), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, Xmtr_2, 2, "WsfEM_Xmtr", "int, int")
{
   int modeIndex = aVarArgs[0].GetInt();
   int index     = aVarArgs[1].GetInt();
   aReturnVal.SetPointer(new UtScriptRef(&aObjectPtr->GetEM_Xmtr(modeIndex, index), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, XmtrCount_1, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetEM_XmtrCount()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, XmtrCount_2, 1, "int", "int")
{
   int modeIndex = aVarArgs[0].GetInt();
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetEM_XmtrCount(modeIndex)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, Rcvr_1, 1, "WsfEM_Rcvr", "int")
{
   auto index   = ut::cast_to_size_t(aVarArgs[0].GetInt());
   auto rcvrPtr = &aObjectPtr->GetEM_Rcvr(index);
   aReturnVal.SetPointer(UtScriptRef::Ref(rcvrPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, Rcvr_2, 2, "WsfEM_Rcvr", "int, int")
{
   auto modeIndex = ut::cast_to_size_t(aVarArgs[0].GetInt());
   auto index     = ut::cast_to_size_t(aVarArgs[1].GetInt());
   auto rcvrPtr   = &aObjectPtr->GetEM_Rcvr(modeIndex, index);
   aReturnVal.SetPointer(UtScriptRef::Ref(rcvrPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, RcvrCount_1, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetEM_RcvrCount()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSensorClass, WsfSensor, RcvrCount_2, 1, "int", "int")
{
   auto modeIndex = ut::cast_to_size_t(aVarArgs[0].GetInt());
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetEM_RcvrCount(modeIndex)));
}
