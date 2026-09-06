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

#ifndef UTJUPITER_HPP
#define UTJUPITER_HPP

#include "ut_export.h"

#include "UtCentralBody.hpp"
#include "UtCloneablePtr.hpp"
#include "UtSolarSystem.hpp"

namespace ut
{

// [1] Archinal, B.A. et al. 2018. "Report of the IAU Working Group on Cartographic Coordinates
//     and Rotational Elements: 2015" Celest Mech Dyn Astr (2018) 130:22.
// [2] Astrodynamic Constants, https://ssd.jpl.nasa.gov/?constants, as of 21 April 2020.

class UT_EXPORT JupiterEllipsoid : public CentralBodyEllipsoid
{
public:
   static constexpr const char* GetNameId() { return "jupiter"; }
   const char*                  GetName() const override { return GetNameId(); }
   double                       GetGravitationalParameter() const override { return 1.267127678e17; } // [2]
   double                       GetRotationRate() const override { return 1.75853e-4; }               // [1]
   double                       GetSemiMajorAxis() const override { return 71492.0e3; }               // [1]
   double                       GetSemiMinorAxis() const override { return 66854.0e3; }               // [1]
   double                       GetMeanRadius() const override { return 69911.0e3; }                  // [1]
   double                       GetFlatteningReciprocal() const override
   {
      return GetSemiMajorAxis() / (GetSemiMajorAxis() - GetSemiMinorAxis());
   }
   double GetFirstEccentricitySquared() const override
   {
      return 1.0 - GetSemiMinorAxis() * GetSemiMinorAxis() / (GetSemiMajorAxis() * GetSemiMajorAxis());
   }
   double GetSecondEccentricitySquared() const override
   {
      return GetFirstEccentricitySquared() / (1.0 - GetFirstEccentricitySquared());
   }

   JupiterEllipsoid* Clone() const override { return new JupiterEllipsoid{}; }
};

class UT_EXPORT Jupiter : public CentralBody
{
public:
   Jupiter()
      : CentralBody{JupiterEllipsoid{}}
      , mPlanetPtr{UtSolarSystem::GetPlanet(UtSolarSystem::Planet::cJUPITER)}
   {
   }

   Jupiter*                     Clone() const override { return new Jupiter{*this}; }
   static constexpr const char* GetNameId() { return JupiterEllipsoid::GetNameId(); }
   UtVec3d                      GetLocationECI(const UtCalendar& aEpoch) const override;
   double GetSiderealOffsetAngle(const UtCalendar& aEpoch) const override { return 4.9733; } // [1]
   double GetNorthPoleRA(const UtCalendar& aEpoch) const override { return 4.67847016; }     // [1]
   double GetNorthPoleDec(const UtCalendar& aEpoch) const override { return 1.1256554; }     // [1]

private:
   ut::CloneablePtr<UtSolarSystem::Planet> mPlanetPtr;
};

} // namespace ut

#endif // UTJUPITER_HPP
