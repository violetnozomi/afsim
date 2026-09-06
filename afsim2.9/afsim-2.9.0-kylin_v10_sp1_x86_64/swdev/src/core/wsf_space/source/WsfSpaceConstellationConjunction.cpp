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

#include "WsfSpaceConstellationConjunction.hpp"

#include "UtMath.hpp"
#include "UtVec3.hpp"

namespace // {anonymous}
{

constexpr double cCOLLISION_TOLERANCE{1.0e-6};

UtVec3d NormalVector(double aInclination, double aRAAN)
{
   return UtVec3d{sin(aInclination) * sin(aRAAN), -sin(aInclination) * cos(aRAAN), cos(aInclination)};
}

UtVec3d IntersectionVector(const UtVec3d& aNormal1, const UtVec3d& aNormal2)
{
   UtVec3d retval;
   retval.CrossProduct(aNormal1, aNormal2).Normalize();
   return retval;
}

UtVec3d AscendingNode(double aRAAN)
{
   return UtVec3d{cos(aRAAN), sin(aRAAN), 0.0};
}

double AngleBetweenVectors(const UtVec3d& aVectorA, const UtVec3d& aVectorB)
{
   return acos(aVectorA.DotProduct(aVectorB));
}

} // namespace

namespace wsf
{
namespace space
{

//! Determine if the specified constellation parameters would lead to a conjunction.
//!
//! This will return true if the provided constellation parameters would lead to a conjunction
//! between one member of the constellation and another.
//!
//! \param aNumPlanes    - the number of orbital planes in the constellation.
//! \param aSatsPerPlane - the number of satellites per orbital plane.
//! \param aInclination  - the inclination in radians of the orbit.
//! \param aRAAN_Range   - the range of RAAN over which the orbital planes are spread, in radians.
//! \param aAnomalyAlias - the inter-plane phasing, in radians.
//! \returns             - true is a conjuntion is expected for the given design; false otherwise.
bool ConstellationConjunction::Assess(int aNumPlanes, int aSatsPerPlane, double aInclination, double aRAAN_Range, double aAnomalyAlias)
{
   bool retval{false};

   // Compute plane 0 vectors
   UtVec3d plane0_Normal = NormalVector(aInclination, 0.0);
   UtVec3d plane0_Node   = AscendingNode(0.0);

   // Loop over all other planes
   for (int plane = 1; plane < aNumPlanes; ++plane)
   {
      // Compute raan of the given plane
      double raan = plane * aRAAN_Range / aNumPlanes;

      // Compute plane N vectors
      UtVec3d planeN_Normal = NormalVector(aInclination, raan);
      UtVec3d planeN_Node   = AscendingNode(raan);

      // Compute intersection and resulting angles
      UtVec3d intersection = IntersectionVector(plane0_Normal, planeN_Normal);
      double  theta0       = AngleBetweenVectors(plane0_Node, intersection);
      double  thetaN       = AngleBetweenVectors(planeN_Node, intersection);

      // Compute the satellite number of the satellite that would be in exactly the
      // right place to hit the intersection point when the plane 0 satellite 0 hits
      // the intersection.
      double numerator = theta0 - thetaN - aAnomalyAlias * plane;
      double denom     = UtMath::cTWO_PI / aSatsPerPlane;
      double satIndex  = numerator / denom;
      double integralPart;
      double fractionalPart = fabs(modf(satIndex, &integralPart));
      if (fractionalPart < cCOLLISION_TOLERANCE || (1.0 - fractionalPart) < cCOLLISION_TOLERANCE)
      {
         // If the fractional part is nearly zero, then there is a likely conjunction.
         retval = true;
         break;
      }
   }

   return retval;
}

} // namespace space
} // namespace wsf
