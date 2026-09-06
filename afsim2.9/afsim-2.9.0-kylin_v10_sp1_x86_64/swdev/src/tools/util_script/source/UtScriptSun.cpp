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

#include "UtScriptSun.hpp"

#include "UtMath.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtScriptVector.hpp"
#include "UtSolarSystem.hpp"
#include "UtSphericalEarth.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"

UtScriptSun::UtScriptSun(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("Sun", aScriptTypesPtr)
{
   UT_SCRIPT_NONWRAPPING_CLASS(Sun);

   mConstructible = true;

   UT_SCRIPT_CONSTEXPR_VALUE(GRAVITATIONAL_PARAMETER, UtSun::cGRAVITATIONAL_PARAMETER);
   UT_SCRIPT_CONSTEXPR_VALUE(MEAN_RADIUS, UtSun::cMEAN_RADIUS);

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, UnitVecWCS, (const UtCalendar& aTime))
   {
      auto sunVecWCS = ut::make_unique<UtVec3d>();
      UtSun::GetSunVecWCS(aTime, *sunVecWCS);
      return sunVecWCS;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, UnitVecECI, (const UtCalendar& aTime))
   {
      auto sunVecECI = ut::make_unique<UtVec3d>();
      UtSun::GetSunVecECI(aTime, *sunVecECI);
      sunVecECI->Normalize();
      return sunVecECI;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, LocationECI, (const UtCalendar& aEpoch))
   {
      auto sunLocECI = ut::make_unique<UtVec3d>();
      UtSolarSystem::GetSunLocationECI(aEpoch, *sunLocECI);
      return sunLocECI;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, LocationWCS, (const UtCalendar& aEpoch))
   {
      auto sunLocWCS = ut::make_unique<UtVec3d>();
      UtSun::GetSunLocationWCS(aEpoch, *sunLocWCS);
      return sunLocWCS;
   };

   UT_SCRIPT_STATIC_METHOD(UtScriptVector<double>, LocationRA_Dec, (const UtCalendar& aEpoch))
   {
      double ra, dec;
      UtSun::GetSunLocationRA_Dec(aEpoch, ra, dec);

      UtScriptVector<double> ra_dec{aContext};
      ra_dec.push_back(ra * UtMath::cDEG_PER_RAD);
      ra_dec.push_back(dec * UtMath::cDEG_PER_RAD);
      return ra_dec;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, UnitVecNED, (const UtCalendar& aEpoch, double lat, double lon))
   {
      auto vecNED = ut::make_unique<UtVec3d>();
      UtSun::GetSunVecNED(lat, lon, aEpoch, *vecNED);
      vecNED->Normalize();
      return vecNED;
   };

   UT_SCRIPT_STATIC_METHOD(bool,
                           LineOfSight,
                           (const UtCalendar& aEpoch, const UtVec3d& aLocationECI_1, const UtVec3d& aLocationECI_2))
   {
      UtVec3d sunLocECI;
      UtSolarSystem::GetSunLocationECI(aEpoch, sunLocECI);
      return ut::CentralBody::LineOfSight(aLocationECI_1, aLocationECI_2, sunLocECI, UtSun::cMEAN_RADIUS);
   };

   UT_SCRIPT_STATIC_METHOD(double, Elevation, (const UtCalendar& aEpoch, double aLat, double aLon))
   {
      UtVec3d vecNED;
      UtSun::GetSunVecNED(aLat, aLon, aEpoch, vecNED);
      vecNED.Normalize();
      double r       = std::sqrt(vecNED[0] * vecNED[0] + vecNED[1] * vecNED[1]); // hypotenuse
      double elevRad = std::atan2(-vecNED[2], r);
      return elevRad * UtMath::cDEG_PER_RAD;
   };

   UT_SCRIPT_STATIC_METHOD(double, Azimuth, (const UtCalendar& aEpoch, double aLat, double aLon))
   {
      UtVec3d vecNED;
      UtSun::GetSunVecNED(aLat, aLon, aEpoch, vecNED);
      vecNED.Normalize();
      if (UtMath::NearlyZero(vecNED[0]) && UtMath::NearlyZero(vecNED[1]))
      {
         return 0.0;
      }
      double azRad = std::atan2(vecNED[1], vecNED[0]);
      azRad        = UtMath::NormalizeAngle0_TwoPi(azRad);
      return azRad * UtMath::cDEG_PER_RAD;
   };

   //! Test methods.

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>,
                           VecWCS,
                           (const UtCalendar& aEpoch,
                            double            ra,
                            double            dec)) // NO_DOC | FOR_TEST_ONLY
   {
      ra *= UtMath::cRAD_PER_DEG;
      dec *= UtMath::cRAD_PER_DEG;
      auto vecWCS = ut::make_unique<UtVec3d>();
      UtSphericalEarth::ConvertRA_DecToECEF(ra, dec, aEpoch, vecWCS->GetData());
      return vecWCS;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, VecECI, (double ra,
                                                              double dec)) // NO_DOC | FOR_TEST_ONLY
   {
      ra *= UtMath::cRAD_PER_DEG;
      dec *= UtMath::cRAD_PER_DEG;
      auto vecECI = ut::make_unique<UtVec3d>();
      UtSphericalEarth::ConvertRA_DecToECI(ra, dec, vecECI->GetData());
      return vecECI;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>,
                           VecNED,
                           (const UtCalendar& aEpoch,
                            double            ra,
                            double            dec,
                            double            lat,
                            double            lon)) // NO_DOC | FOR_TEST_ONLY
   {
      ra *= UtMath::cRAD_PER_DEG;
      dec *= UtMath::cRAD_PER_DEG;
      auto vecNED = ut::make_unique<UtVec3d>();
      UtSphericalEarth::ConvertRA_DecToLocal(ra, dec, lat, lon, aEpoch, vecNED->GetData());
      return vecNED;
   };
}

// virtual
void* UtScriptSun::Create(const UtScriptContext& /*aContext*/)
{
   // Since this acts as a static class, we just return the current instance.
   return this;
}

// virtual
void UtScriptSun::Destroy(void* aObjectPtr)
{
   // Do nothing.  This is a static class.
}
