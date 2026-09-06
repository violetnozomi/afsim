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

#ifndef WSFSCRIPTCONSTELLATION_HPP
#define WSFSCRIPTCONSTELLATION_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WsfScriptConstellation : public UtScriptClass
{
public:
   explicit WsfScriptConstellation(UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptConstellation() override = default;

   void Destroy(void* aObjectPtr);

   // Static methods
   UT_DECLARE_SCRIPT_METHOD(FindConstellation);
   UT_DECLARE_SCRIPT_METHOD(AllConstellations);

   // Queries
   UT_DECLARE_SCRIPT_METHOD(IsGeneral);
   UT_DECLARE_SCRIPT_METHOD(IsWalkerDelta);
   UT_DECLARE_SCRIPT_METHOD(IsWalkerStar);
   UT_DECLARE_SCRIPT_METHOD(TotalSats);
   UT_DECLARE_SCRIPT_METHOD(NumPlanes);
   UT_DECLARE_SCRIPT_METHOD(SatsPerPlane);
   UT_DECLARE_SCRIPT_METHOD(WalkerF);
   UT_DECLARE_SCRIPT_METHOD(AnomalyAlias);
   UT_DECLARE_SCRIPT_METHOD(RAAN_Range);
   UT_DECLARE_SCRIPT_METHOD(ConstellationName);
   UT_DECLARE_SCRIPT_METHOD(PlatformType);
   UT_DECLARE_SCRIPT_METHOD(SemiMajorAxis);
   UT_DECLARE_SCRIPT_METHOD(CircularAltitude);
   UT_DECLARE_SCRIPT_METHOD(OrbitalPeriod);
   UT_DECLARE_SCRIPT_METHOD(RevolutionsPerDay);
   UT_DECLARE_SCRIPT_METHOD(Inclination);
   UT_DECLARE_SCRIPT_METHOD(InitialRAAN);
   UT_DECLARE_SCRIPT_METHOD(InitialAnomaly);

   UT_DECLARE_SCRIPT_METHOD(NextSatelliteInPlane);
   UT_DECLARE_SCRIPT_METHOD(PreviousSatelliteInPlane);
   UT_DECLARE_SCRIPT_METHOD(NextPlane);
   UT_DECLARE_SCRIPT_METHOD(PreviousPlane);

   // Member-related
   UT_DECLARE_SCRIPT_METHOD(MemberName);
   UT_DECLARE_SCRIPT_METHOD(Member);
   UT_DECLARE_SCRIPT_METHOD(Apply);
};

#endif // WSFSCRIPTCONSTELLATION_HPP
