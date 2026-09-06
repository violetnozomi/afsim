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

#include "UtScriptMoon.hpp"

#include "UtCalendar.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtMoon.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtScriptVector.hpp"

UtScriptMoon::UtScriptMoon(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("Moon", aScriptTypesPtr)
{
   UT_SCRIPT_NONWRAPPING_CLASS(Moon);

   UT_SCRIPT_CONSTEXPR_VALUE(MEAN_RADIUS, UtMoon::cMEAN_RADIUS);
   UT_SCRIPT_CONSTEXPR_VALUE(GRAVITATIONAL_PARAMETER, UtMoon::cGRAVITATIONAL_PARAMETER);

   UT_SCRIPT_STATIC_METHOD(double, Phase, (const UtCalendar& aEpoch))
   {
      return UtMoon::GetPhase(aEpoch) * UtMath::cDEG_PER_RAD;
   };

   UT_SCRIPT_STATIC_METHOD(double, Illumination, (const UtCalendar& aEpoch))
   {
      return UtMoon::GetIllumination(aEpoch);
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, LocationECI, (const UtCalendar& aEpoch))
   {
      auto moonLocECI = ut::make_unique<UtVec3d>();
      UtMoon::GetLocationECI(aEpoch, *moonLocECI);
      return moonLocECI;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, LocationWCS, (const UtCalendar& aEpoch))
   {
      auto moonLocWCS = ut::make_unique<UtVec3d>();
      UtMoon::GetLocationWCS(aEpoch, *moonLocWCS);
      return moonLocWCS;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, VelocityECI, (const UtCalendar& aEpoch))
   {
      auto moonVelECI = ut::make_unique<UtVec3d>();
      UtMoon::GetVelocityECI(aEpoch, *moonVelECI);
      return moonVelECI;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, UnitVecWCS, (const UtCalendar& aEpoch))
   {
      UtVec3d moonLocECI;
      UtMoon::GetLocationECI(aEpoch, moonLocECI);
      auto moonLocWCS = ut::make_unique<UtVec3d>();
      UtEllipsoidalEarth::ConvertTODToECEF(aEpoch.GetEarthAngleApprox(), moonLocECI.GetData(), moonLocWCS->GetData());
      moonLocWCS->Normalize();
      return moonLocWCS;
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, UnitVecECI, (const UtCalendar& aEpoch))
   {
      auto moonLocECI = ut::make_unique<UtVec3d>();
      UtMoon::GetLocationECI(aEpoch, *moonLocECI);
      moonLocECI->Normalize();
      return moonLocECI;
   };

   UT_SCRIPT_STATIC_METHOD(UtScriptVector<double>, LocationRA_Dec, (const UtCalendar& aEpoch))
   {
      UtScriptVector<double> ra_dec{aContext};
      double                 ra, dec;
      UtMoon::GetLocationRA_Dec(aEpoch, ra, dec);
      ra_dec.emplace_back(ra * UtMath::cDEG_PER_RAD);
      ra_dec.emplace_back(dec * UtMath::cDEG_PER_RAD);
      return ra_dec;
   };

   UT_SCRIPT_STATIC_METHOD(bool,
                           LineOfSight,
                           (const UtCalendar& aEpoch, const UtVec3d& aLocationECI_1, const UtVec3d& aLocationECI_2))
   {
      UtVec3d moonLocECI;
      UtMoon::GetLocationECI(aEpoch, moonLocECI);
      return ut::CentralBody::LineOfSight(aLocationECI_1, aLocationECI_2, moonLocECI, UtMoon::cA);
   };
}
