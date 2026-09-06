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

#ifndef WSFSCRIPTCONSTELLATIONMAKER_HPP
#define WSFSCRIPTCONSTELLATIONMAKER_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WsfScriptConstellationMaker : public UtScriptClass
{
public:
   explicit WsfScriptConstellationMaker(UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptConstellationMaker() override = default;

   void Destroy(void* aObjectPtr) override;

   // Static construction methods
   UT_DECLARE_SCRIPT_METHOD(CreateWalkerDelta);
   UT_DECLARE_SCRIPT_METHOD(CreateWalkerStar);
   UT_DECLARE_SCRIPT_METHOD(CreateGeneral);

   // Main Action Methods
   UT_DECLARE_SCRIPT_METHOD(WriteToFile);
   UT_DECLARE_SCRIPT_METHOD(Create_1);
   UT_DECLARE_SCRIPT_METHOD(Create_2);
   UT_DECLARE_SCRIPT_METHOD(DetectConjunction);

   // Gets
   UT_DECLARE_SCRIPT_METHOD(IsSetup);
   UT_DECLARE_SCRIPT_METHOD(Validate);
   UT_DECLARE_SCRIPT_METHOD(IsWalkerDelta);
   UT_DECLARE_SCRIPT_METHOD(IsWalkerStar);
   UT_DECLARE_SCRIPT_METHOD(IsGeneral);
   UT_DECLARE_SCRIPT_METHOD(TotalSatellites);
   UT_DECLARE_SCRIPT_METHOD(Planes);
   UT_DECLARE_SCRIPT_METHOD(SatellitesPerPlane);
   UT_DECLARE_SCRIPT_METHOD(WalkerF);
   UT_DECLARE_SCRIPT_METHOD(AnomalyAlias);
   UT_DECLARE_SCRIPT_METHOD(RAAN_Range);

   UT_DECLARE_SCRIPT_METHOD(SemiMajorAxis);
   UT_DECLARE_SCRIPT_METHOD(Altitude);
   UT_DECLARE_SCRIPT_METHOD(RevolutionsPerDay);
   UT_DECLARE_SCRIPT_METHOD(Period);
   UT_DECLARE_SCRIPT_METHOD(Inclination);
   UT_DECLARE_SCRIPT_METHOD(InitialRAAN);
   UT_DECLARE_SCRIPT_METHOD(InitialAnomaly);
   UT_DECLARE_SCRIPT_METHOD(ConstellationName);
   UT_DECLARE_SCRIPT_METHOD(PlatformType);
   UT_DECLARE_SCRIPT_METHOD(BasePath);

   // Sets
   UT_DECLARE_SCRIPT_METHOD(SetSemiMajorAxis);
   UT_DECLARE_SCRIPT_METHOD(SetAltitude);
   UT_DECLARE_SCRIPT_METHOD(SetRevolutionsPerDay);
   UT_DECLARE_SCRIPT_METHOD(SetPeriod);
   UT_DECLARE_SCRIPT_METHOD(SetInclination);
   UT_DECLARE_SCRIPT_METHOD(SetInitialRAAN);
   UT_DECLARE_SCRIPT_METHOD(SetInitialAnomaly);
   UT_DECLARE_SCRIPT_METHOD(SetConstellationName);
   UT_DECLARE_SCRIPT_METHOD(SetPlatformType);
   UT_DECLARE_SCRIPT_METHOD(SetBasePath);
};

#endif // WSFSCRIPTCONSTELLATIONMAKER_HPP
