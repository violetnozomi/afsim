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

#include "UtScriptEarth.hpp"

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtECI_Conversion.hpp"
#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtMoon.hpp"
#include "UtScriptException.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtSphericalEarth.hpp"
#include "UtSun.hpp"

UtScriptEarth::UtScriptEarth(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("Earth", aScriptTypesPtr)
{
   UT_SCRIPT_NONWRAPPING_CLASS(Earth);

   mConstructible = true;
   UT_SCRIPT_CONSTEXPR_VALUE(ACCEL_OF_GRAVITY, UtEarth::cACCEL_OF_GRAVITY);
   UT_SCRIPT_CONSTEXPR_VALUE(SSL_AIR_DENSITY, UtEarth::cSSL_AIR_DENSITY);
   UT_SCRIPT_CONSTEXPR_VALUE(GRAVITATIONAL_PARAMETER, UtEarth::cGRAVITATIONAL_PARAMETER_WGS84);
   UT_SCRIPT_CONSTEXPR_VALUE(EQUATORIAL_RADIUS, UtEarth::cA);
   UT_SCRIPT_CONSTEXPR_VALUE(POLAR_RADIUS, UtEarth::cB);
   UT_SCRIPT_CONSTEXPR_VALUE(MEAN_RADIUS, UtSphericalEarth::cEARTH_MEAN_RADIUS);
   UT_SCRIPT_CONSTEXPR_VALUE(ROTATION_RATE, UtEarth::cOMEGA * UtMath::cDEG_PER_RAD);
   UT_SCRIPT_CONSTEXPR_VALUE(MASS, UtEarth::cMASS);
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(double, GEOSTATIONARY_RADIUS, ())
   {
      return ut::EarthEGM96().GetSynchronousOrbitSemiMajorAxis();
   };

   UT_SCRIPT_STATIC_METHOD(bool, LineOfSight, (const UtVec3d& aLocationECI_1, const UtVec3d& aLocationECI_2))
   {
      UtCalendar now{};
      return ut::CentralBody::LineOfSight(aLocationECI_1, aLocationECI_2, ut::EarthEGM96().GetLocationECI(now), UtEarth::cA);
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, SubSolarPoint, (const UtCalendar& aEpoch))
   {
      auto sunVecPtr = ut::make_unique<UtVec3d>();
      UtSun::GetSunLocationWCS(aEpoch, *sunVecPtr);
      double lat;
      double lon;
      double alt;
      auto   earth = ut::EarthEGM96{};
      earth.ConvertWCS_ToLLA(*sunVecPtr, lat, lon, alt);
      alt = 0.0;
      earth.ConvertLLA_ToWCS(lat, lon, alt, *sunVecPtr);
      return sunVecPtr;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, SubLunarPoint, (const UtCalendar& aEpoch))
   {
      auto moonVecPtr = ut::make_unique<UtVec3d>();
      UtMoon::GetLocationWCS(aEpoch, *moonVecPtr);
      double lat;
      double lon;
      double alt;
      auto   earth = ut::EarthEGM96{};
      earth.ConvertWCS_ToLLA(*moonVecPtr, lat, lon, alt);
      alt = 0.0;
      earth.ConvertLLA_ToWCS(lat, lon, alt, *moonVecPtr);
      return moonVecPtr;
   };

   UT_SCRIPT_STATIC_METHOD(double, CATE_Angle, (const UtVec3d& aObserver, const UtVec3d& aTarget))
   {
      UtVec3d obsToTarget = aTarget - aObserver;
      // NOTE: The Earth's position in ECI is the zero vector by definition.
      double dotprod     = obsToTarget.DotProduct(aTarget);
      double teMagnitude = aTarget.Magnitude();
      if (teMagnitude == 0.0)
      {
         UT_SCRIPT_ABORT("The CATE angle is undefined if the target and Earth are at the same position.");
      }
      double catMagnitude = obsToTarget.Magnitude();
      if (catMagnitude == 0.0)
      {
         UT_SCRIPT_ABORT("The CATE angle is undefined if the target and the observers are at the same position.");
      }
      double denom = teMagnitude * catMagnitude;
      double theta = acos(dotprod / denom);
      return theta * UtMath::cDEG_PER_RAD;
   };

   UT_SCRIPT_STATIC_METHOD(UtVec3d, ConvertECI_ToWCS, (const UtCalendar& aEpoch, const UtVec3d& aECI))
   {
      UtECI_Conversion conv{aEpoch, ut::EarthEGM96{}};
      conv.SetLocationECI(aECI);
      return conv.GetLocationWCS();
   };

   UT_SCRIPT_STATIC_METHOD(UtVec3d, ConvertWCS_ToECI, (const UtCalendar& aEpoch, const UtVec3d& aWCS))
   {
      UtECI_Conversion conv{aEpoch, ut::EarthEGM96{}};
      conv.SetLocationWCS(aWCS);
      return conv.GetLocationECI();
   };
}

// virtual
void* UtScriptEarth::Create(const UtScriptContext& /*aContext*/)
{
   // Since this acts as a static class, we just return the current instance.
   return this;
}

// virtual
void UtScriptEarth::Destroy(void* aObjectPtr)
{
   // Do nothing.  This is a static class, so don't delete
   // the single instance.
}