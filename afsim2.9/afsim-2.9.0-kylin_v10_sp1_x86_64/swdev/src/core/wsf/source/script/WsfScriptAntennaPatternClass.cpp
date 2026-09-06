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

#include "WsfScriptAntennaPatternClass.hpp"

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WsfAntennaPattern.hpp"

// =================================================================================================
WsfScriptAntennaPatternClass::WsfScriptAntennaPatternClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptObjectClass(aClassName, aTypesPtr)
{
   SetClassName("WsfAntennaPattern");

   mConstructible      = false;
   mCloneable          = false;
   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<AzimuthBeamwidth_1>("AzimuthBeamwidth"));
   AddMethod(ut::make_unique<AzimuthBeamwidth_2>("AzimuthBeamwidth"));
   AddMethod(ut::make_unique<ElevationBeamwidth_1>("ElevationBeamwidth"));
   AddMethod(ut::make_unique<ElevationBeamwidth_2>("ElevationBeamwidth"));
   AddMethod(ut::make_unique<MinimumGain>());
   AddMethod(ut::make_unique<PeakGain>());
   AddMethod(ut::make_unique<GainAdjustment>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptAntennaPatternClass, WsfAntennaPattern, AzimuthBeamwidth_1, 1, "double", "double")
{
   aReturnVal.SetDouble(aObjectPtr->GetAzimuthBeamwidth(aVarArgs[0].GetDouble(), 0, 0) * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptAntennaPatternClass, WsfAntennaPattern, AzimuthBeamwidth_2, 3, "double", "double, double, double")
{
   double azEBS = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   double elEBS = aVarArgs[2].GetDouble() * UtMath::cRAD_PER_DEG;
   aReturnVal.SetDouble(aObjectPtr->GetAzimuthBeamwidth(aVarArgs[0].GetDouble(), azEBS, elEBS) * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptAntennaPatternClass, WsfAntennaPattern, ElevationBeamwidth_1, 1, "double", "double")
{
   aReturnVal.SetDouble(aObjectPtr->GetElevationBeamwidth(aVarArgs[0].GetDouble(), 0, 0) * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptAntennaPatternClass, WsfAntennaPattern, ElevationBeamwidth_2, 3, "double", "double, double, double")
{
   double azEBS = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   double elEBS = aVarArgs[2].GetDouble() * UtMath::cRAD_PER_DEG;
   aReturnVal.SetDouble(aObjectPtr->GetElevationBeamwidth(aVarArgs[0].GetDouble(), azEBS, elEBS) * UtMath::cDEG_PER_RAD);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptAntennaPatternClass, WsfAntennaPattern, MinimumGain, 0, "double", "")
{
   aReturnVal.SetDouble(UtMath::LinearToDB(aObjectPtr->GetMinimumGain()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptAntennaPatternClass, WsfAntennaPattern, PeakGain, 1, "double", "double")
{
   aReturnVal.SetDouble(UtMath::LinearToDB(aObjectPtr->GetPeakGain(aVarArgs[0].GetDouble())));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptAntennaPatternClass, WsfAntennaPattern, GainAdjustment, 0, "double", "")
{
   aReturnVal.SetDouble(UtMath::LinearToDB(aObjectPtr->GetGainAdjustment()));
}
