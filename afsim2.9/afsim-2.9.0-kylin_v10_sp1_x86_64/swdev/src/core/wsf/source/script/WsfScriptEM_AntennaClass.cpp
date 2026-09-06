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

#include "WsfScriptEM_AntennaClass.hpp"

#include <vector>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtStringUtil.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfFieldOfView.hpp"

// =================================================================================================
WsfScriptEM_AntennaClass::WsfScriptEM_AntennaClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfEM_Antenna");
   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<ArticulatedPart>());

   AddMethod(ut::make_unique<Height>());
   AddMethod(ut::make_unique<Pitch>());
   AddMethod(ut::make_unique<FieldOfView>());
   AddMethod(ut::make_unique<AzimuthFieldOfView>());
   AddMethod(ut::make_unique<ElevationFieldOfView>());
   AddMethod(ut::make_unique<MaximumRange>());
   AddMethod(ut::make_unique<MinimumRange>());
   AddMethod(ut::make_unique<MaximumAltitude>());
   AddMethod(ut::make_unique<MinimumAltitude>());
   AddMethod(ut::make_unique<EBS_Mode>());
   AddMethod(ut::make_unique<EBS_AzimuthLossExponent>());
   AddMethod(ut::make_unique<EBS_ElevationLossExponent>());
   AddMethod(ut::make_unique<EBS_AzimuthCosineSteeringLimit>());
   AddMethod(ut::make_unique<EBS_ElevationCosineSteeringLimit>());
   AddMethod(ut::make_unique<ScanMode>());
   AddMethod(ut::make_unique<ScanStabilization>());
   AddMethod(ut::make_unique<MaximumAzimuthScanLimit>());
   AddMethod(ut::make_unique<MinimumAzimuthScanLimit>());
   AddMethod(ut::make_unique<MaximumElevationScanLimit>());
   AddMethod(ut::make_unique<MinimumElevationScanLimit>());

   AddMethod(ut::make_unique<SetHeight>());
   AddMethod(ut::make_unique<SetPitch>());
   AddMethod(ut::make_unique<SetFieldOfView>());
   AddMethod(ut::make_unique<SetAzimuthFieldOfView>());
   AddMethod(ut::make_unique<SetElevationFieldOfView>());
   AddMethod(ut::make_unique<SetMaximumRange>());
   AddMethod(ut::make_unique<SetMinimumRange>());
   AddMethod(ut::make_unique<SetMaximumAltitude>());
   AddMethod(ut::make_unique<SetMinimumAltitude>());
   AddMethod(ut::make_unique<SetEBS_Mode>());
   AddMethod(ut::make_unique<SetEBS_AzimuthLossExponent>());
   AddMethod(ut::make_unique<SetEBS_ElevationLossExponent>());
   AddMethod(ut::make_unique<SetEBS_AzimuthCosineSteeringLimit>());
   AddMethod(ut::make_unique<SetEBS_ElevationCosineSteeringLimit>());
   AddMethod(ut::make_unique<SetScanMode>());
   AddMethod(ut::make_unique<SetScanStabilization>());
   AddMethod(ut::make_unique<SetMaximumAzimuthScanLimit>());
   AddMethod(ut::make_unique<SetMinimumAzimuthScanLimit>());
   AddMethod(ut::make_unique<SetMaximumElevationScanLimit>());
   AddMethod(ut::make_unique<SetMinimumElevationScanLimit>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, ArticulatedPart, 0, "WsfArticulatedPart", "")
{
   aReturnVal.SetPointer(UtScriptRef::Ref(aObjectPtr->GetArticulatedPart(), aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, Height, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHeight());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, Pitch, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPitch() * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, FieldOfView, 0, "WsfFieldOfView", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetFieldOfView()->Clone(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, AzimuthFieldOfView, 0, "Array<double>", "")
{
   double min, max;
   aObjectPtr->GetAzimuthFieldOfView(min, max);
   auto data = ut::make_unique<std::vector<UtScriptData>>();
   data->reserve(2);
   data->emplace_back(min * UtMath::cDEG_PER_RAD);
   data->emplace_back(max * UtMath::cDEG_PER_RAD);
   auto scriptRef = ut::make_unique<UtScriptRef>(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(scriptRef.release());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, ElevationFieldOfView, 0, "Array<double>", "")
{
   double min, max;
   aObjectPtr->GetElevationFieldOfView(min, max);
   auto data = ut::make_unique<std::vector<UtScriptData>>();
   data->reserve(2);
   data->emplace_back(min * UtMath::cDEG_PER_RAD);
   data->emplace_back(max * UtMath::cDEG_PER_RAD);
   auto scriptRef = ut::make_unique<UtScriptRef>(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(scriptRef.release());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, MaximumRange, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaximumRange());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, MinimumRange, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMinimumRange());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, MaximumAltitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaximumAltitude());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, MinimumAltitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMinimumAltitude());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, EBS_Mode, 0, "string", "")
{
   auto mode = aObjectPtr->GetEBS_Mode();
   switch (mode)
   {
   case WsfEM_Antenna::EBS_Mode::cEBS_NONE:
   {
      aReturnVal.SetString("none");
      break;
   }
   case WsfEM_Antenna::EBS_Mode::cEBS_AZIMUTH:
   {
      aReturnVal.SetString("azimuth");
      break;
   }
   case WsfEM_Antenna::EBS_Mode::cEBS_ELEVATION:
   {
      aReturnVal.SetString("elevation");
      break;
   }
   case WsfEM_Antenna::EBS_Mode::cEBS_BOTH:
   {
      aReturnVal.SetString("both");
      break;
   }
   default:
   {
      UT_SCRIPT_ABORT("Invalid EBS_Mode");
   }
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, EBS_AzimuthLossExponent, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEBS_AzLossExponent());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, EBS_ElevationLossExponent, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEBS_ElLossExponent());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, EBS_AzimuthCosineSteeringLimit, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEBS_AzCosSteeringLimit());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, EBS_ElevationCosineSteeringLimit, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEBS_ElCosSteeringLimit());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, ScanMode, 0, "string", "")
{
   auto mode = aObjectPtr->GetScanMode();
   switch (mode)
   {
   case WsfEM_Antenna::ScanMode::cSCAN_FIXED:
   {
      aReturnVal.SetString("fixed");
      break;
   }
   case WsfEM_Antenna::ScanMode::cSCAN_AZ:
   {
      aReturnVal.SetString("azimuth");
      break;
   }
   case WsfEM_Antenna::ScanMode::cSCAN_EL:
   {
      aReturnVal.SetString("elevation");
      break;
   }
   case WsfEM_Antenna::ScanMode::cSCAN_AZ_EL:
   {
      aReturnVal.SetString("both");
      break;
   }
   default:
   {
      UT_SCRIPT_ABORT("Invalid Scan Mode");
   }
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, ScanStabilization, 0, "string", "")
{
   auto mode = aObjectPtr->GetScanStabilization();
   switch (mode)
   {
   case WsfEM_Antenna::ScanStabilization::cSS_NONE:
   {
      aReturnVal.SetString("none");
      break;
   }
   case WsfEM_Antenna::ScanStabilization::cSS_PITCH:
   {
      aReturnVal.SetString("pitch");
      break;
   }
   case WsfEM_Antenna::ScanStabilization::cSS_ROLL:
   {
      aReturnVal.SetString("roll");
      break;
   }
   case WsfEM_Antenna::ScanStabilization::cSS_BOTH:
   {
      aReturnVal.SetString("pitch_and_roll");
      break;
   }
   default:
   {
      UT_SCRIPT_ABORT("Invalid Scan Stabilization");
   }
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, MaximumAzimuthScanLimit, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaximumAzimuthScanLimit() * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, MinimumAzimuthScanLimit, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMinimumAzimuthScanLimit() * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, MaximumElevationScanLimit, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaximumElevationScanLimit() * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, MinimumElevationScanLimit, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMinimumElevationScanLimit() * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetHeight, 1, "void", "double")
{
   aObjectPtr->SetHeight(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetPitch, 1, "void", "double")
{
   aObjectPtr->SetPitch(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetFieldOfView, 1, "void", "WsfFieldOfView")
{
   auto fovPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfFieldOfView>();
   aObjectPtr->SetFieldOfView(ut::clone(fovPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetAzimuthFieldOfView, 2, "void", "double, double")
{
   aObjectPtr->SetAzimuthFieldOfView(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG,
                                     aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetElevationFieldOfView, 2, "void", "double, double")
{
   aObjectPtr->SetElevationFieldOfView(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG,
                                       aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetMaximumRange, 1, "void", "double")
{
   aObjectPtr->SetMaximumRange(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetMinimumRange, 1, "void", "double")
{
   aObjectPtr->SetMinimumRange(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetMaximumAltitude, 1, "void", "double")
{
   aObjectPtr->SetMaximumAltitude(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetMinimumAltitude, 1, "void", "double")
{
   aObjectPtr->SetMinimumAltitude(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetEBS_Mode, 1, "bool", "string")
{
   auto mode = aVarArgs[0].GetString();
   UtStringUtil::ToLower(mode);
   aReturnVal.SetBool(true);

   if (mode == "none")
   {
      aObjectPtr->SetEBS_Mode(WsfEM_Antenna::EBS_Mode::cEBS_NONE);
   }
   else if (mode == "azimuth")
   {
      aObjectPtr->SetEBS_Mode(WsfEM_Antenna::EBS_Mode::cEBS_AZIMUTH);
   }
   else if (mode == "elevation")
   {
      aObjectPtr->SetEBS_Mode(WsfEM_Antenna::EBS_Mode::cEBS_ELEVATION);
   }
   else if (mode == "both")
   {
      aObjectPtr->SetEBS_Mode(WsfEM_Antenna::EBS_Mode::cEBS_BOTH);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetEBS_AzimuthLossExponent, 1, "void", "double")
{
   aObjectPtr->SetEBS_AzLossExponent(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetEBS_ElevationLossExponent, 1, "void", "double")
{
   aObjectPtr->SetEBS_ElLossExponent(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetEBS_AzimuthCosineSteeringLimit, 1, "void", "double")
{
   aObjectPtr->SetEBS_AzCosSteeringLimit(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetEBS_ElevationCosineSteeringLimit, 1, "void", "double")
{
   aObjectPtr->SetEBS_ElCosSteeringLimit(aVarArgs[0].GetDouble());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetScanMode, 1, "bool", "string")
{
   auto mode = aVarArgs[0].GetString();
   UtStringUtil::ToLower(mode);
   aReturnVal.SetBool(true);

   if (mode == "fixed")
   {
      aObjectPtr->SetScanMode(WsfEM_Antenna::ScanMode::cSCAN_FIXED);
   }
   else if (mode == "azimuth")
   {
      aObjectPtr->SetScanMode(WsfEM_Antenna::ScanMode::cSCAN_AZ);
   }
   else if (mode == "elevation")
   {
      aObjectPtr->SetScanMode(WsfEM_Antenna::ScanMode::cSCAN_EL);
   }
   else if (mode == "both")
   {
      aObjectPtr->SetScanMode(WsfEM_Antenna::ScanMode::cSCAN_AZ_EL);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetScanStabilization, 1, "bool", "string")
{
   auto mode = aVarArgs[0].GetString();
   UtStringUtil::ToLower(mode);
   aReturnVal.SetBool(true);

   if (mode == "none")
   {
      aObjectPtr->SetScanStabilization(WsfEM_Antenna::ScanStabilization::cSS_NONE);
   }
   else if (mode == "pitch")
   {
      aObjectPtr->SetScanStabilization(WsfEM_Antenna::ScanStabilization::cSS_PITCH);
   }
   else if (mode == "roll")
   {
      aObjectPtr->SetScanStabilization(WsfEM_Antenna::ScanStabilization::cSS_ROLL);
   }
   else if (mode == "pitch_and_roll")
   {
      aObjectPtr->SetScanStabilization(WsfEM_Antenna::ScanStabilization::cSS_BOTH);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetMaximumAzimuthScanLimit, 1, "void", "double")
{
   aObjectPtr->SetMaximumAzimuthScanLimit(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetMinimumAzimuthScanLimit, 1, "void", "double")
{
   aObjectPtr->SetMinimumAzimuthScanLimit(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetMaximumElevationScanLimit, 1, "void", "double")
{
   aObjectPtr->SetMaximumElevationScanLimit(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEM_AntennaClass, WsfEM_Antenna, SetMinimumElevationScanLimit, 1, "void", "double")
{
   aObjectPtr->SetMinimumElevationScanLimit(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}
