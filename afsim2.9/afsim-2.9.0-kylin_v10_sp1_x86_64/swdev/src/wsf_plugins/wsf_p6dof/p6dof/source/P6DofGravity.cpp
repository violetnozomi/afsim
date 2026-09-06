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

#include "P6DofGravity.hpp"

#include "P6DofUtils.hpp"
#include "UtEntity.hpp"
#include "UtVec3dX.hpp"

P6DofGravity* P6DofGravity::Clone() const
{
   return new P6DofGravity(*this);
}

UtVec3dX P6DofGravity::NormalizedGravitationalAccel(const double aLat,
                                                    const double aLon,
                                                    const double aAlt_m,
                                                    const bool   aUseSphericalEarth)
{
   // This will be the direction of gravity
   UtVec3dX unitGravityVec(0., 0., 0.);

   if (aUseSphericalEarth)
   {
      UtVec3dX pos_m;
      P6DofUtils::CalcSphericalPositionFromLatLonAlt(aLat, aLon, aAlt_m, pos_m);
      // We want to point "down", so negate the pos vec
      pos_m = -1.0 * pos_m;
      unitGravityVec.Set(pos_m);
   }
   else
   {
      UtEntity tempEntity;
      tempEntity.SetLocationLLA(aLat, aLon, aAlt_m);
      // We want to point "down", so use +1 in z
      double nedVec[3] = {0.0, 0.0, 1.0};
      tempEntity.SetVelocityNED(nedVec);
      double wcsVec[3];
      tempEntity.GetVelocityWCS(wcsVec);
      unitGravityVec.Set(wcsVec);
   }

   // We need a unit vector
   unitGravityVec.Normalize();

   // Get the gravity at the altitude
   double normalizedGravityMagnitude = NormalizedGravitationalAccel(aAlt_m);

   // Scale the unit vector
   unitGravityVec = normalizedGravityMagnitude * unitGravityVec;

   return unitGravityVec;
}

double P6DofGravity::NormalizedGravitationalAccel(const double aAlt_m)
{
   // Note: The AFSIM cEARTH_RADIUS = 6366707.0194937074958 (6366.7 km)
   // Typical earth radius is 6371 km -- note the 4.3 km discrepancy
   // Distances from points on the surface to the center range from 6,353 km to 6,384 km.
   // Several different ways of modeling the Earth as a sphere each yield a mean radius of 6,371 km.
   double earthRadius_m = P6DofUtils::GetSphericalEarthRadius_m();

   double re2 = earthRadius_m * earthRadius_m;
   double r   = earthRadius_m + aAlt_m;

   // At altitude, gravity becomes gFactor = (re^2/((re+alt)^2)
   return (re2 / (r * r));
}

UtVec3dX P6DofGravity::NormalizedGravitationalAccel(const UtVec3dX& aLocation_m, const bool aUseSphericalEarth)
{
   if (aUseSphericalEarth)
   {
      double aLat   = 0.0;
      double aLon   = 0.0;
      double aAlt_m = 0.0;
      P6DofUtils::CalcSphericalLatLonAlt(aLocation_m, aLat, aLon, aAlt_m);

      return NormalizedGravitationalAccel(aLat, aLon, aAlt_m, true);
   }
   else
   {
      UtEntity tempEntity;
      double   wcsPos[3];
      aLocation_m.Get(wcsPos);
      tempEntity.SetLocationWCS(wcsPos);

      double aLat   = 0.0;
      double aLon   = 0.0;
      double aAlt_m = 0.0;
      tempEntity.GetLocationLLA(aLat, aLon, aAlt_m);

      return NormalizedGravitationalAccel(aLat, aLon, aAlt_m, false);
   }
}
