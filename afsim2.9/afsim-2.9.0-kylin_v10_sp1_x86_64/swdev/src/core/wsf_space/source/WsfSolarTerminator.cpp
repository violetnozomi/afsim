// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSolarTerminator.hpp"

#include <algorithm>
#include <cassert>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"
#include "WsfDateTime.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace WsfSolarTerminator
{
namespace
{
// The following definition is taken from Fundamentals of Astrodynamics and
// Applications, 4th Ed. p. 281.
const double cCOS_TWILIGHT_BEGIN = cos(1.5853407372281827); //  90 deg 50'

// One 1 mm of tolerance for checking if a location is inside the Earth.
constexpr double cMASKED_BY_EARTH_TOLERANCE = 0.05;

//! Return the inner product of the two vectors scaled by the ellipsoidal semi-axes.
//! \param aVectorA_WCS An ECEF position vector.
//! \param aVectorB_WCS Another ECEF position vector.
//! \return             The phase of the day at the given surface location.
double EllipsoidalInnerProduct(const double aVectorA_WCS[3], const double aVectorB_WCS[3])
{
   return (aVectorA_WCS[0] * aVectorB_WCS[0] + aVectorA_WCS[1] * aVectorB_WCS[1]) / (UtEarth::cA * UtEarth::cA) +
          (aVectorA_WCS[2] * aVectorB_WCS[2]) / (UtEarth::cB * UtEarth::cB);
}

//! Return the value of the ellipsoidal function for the given location.
//! The ellipsoidal function is zero on the surface of the ellipsoid, negative at points inside,
//! and positive at points outside.
//! \param aLocationWCS The location of interest.
//! \return             The value of the ellipsoidal function.
double EllipsoidalFunction(const double aLocationWCS[3])
{
   return EllipsoidalInnerProduct(aLocationWCS, aLocationWCS) - 1.0;
}

//! Return the displacement from the given location to the indicated limb of the Sun.
//! \param aLocationWCS  The location from which to compute the Sun's limb.
//! \param aTime         The time in question.
//! \param aUpperLimbWCS The resulting WCS displacement of the solar limb farthest from the Earth's horizon.
//! \param aLowerLimbWCS The resulting WCS displacement of the other solar limb.
void GetDisplacementToSolarLimbs(const double      aLocationWCS[3],
                                 const UtCalendar& aTime,
                                 double            aUpperLimbWCS[3],
                                 double            aLowerLimbWCS[3])
{
   double sunLoc[3];
   UtSun::GetSunLocationWCS(aTime, sunLoc);
   double sunHat[3];
   std::copy(sunLoc, sunLoc + 3, sunHat);
   UtVec3d::Normalize(sunHat);

   double locHat[3];
   std::copy(aLocationWCS, aLocationWCS + 3, locHat);
   UtVec3d::Normalize(locHat);

   double upVec[3];
   UtVec3d::CrossProduct(upVec, locHat, sunHat);
   double scale = UtVec3d::Magnitude(upVec);
   if (scale < 1.0e-6) // corresponds to about 1 second of arc
   {
      // In this case, the Earth, Sun and Observer Location are in a line.
      // So pick any direction orthogonal to sunHat to be 'up'.
      std::copy(sunHat, sunHat + 3, upVec);
      double* maxIter        = std::max_element(upVec, upVec + 3);
      double* minIter        = std::min_element(upVec, upVec + 3);
      double  maxReplacement = -*minIter;
      double  minReplacement = *maxIter;
      std::fill(upVec, upVec + 3, 0.0);
      *maxIter = maxReplacement;
      *minIter = minReplacement;
   }
   UtVec3d::Normalize(upVec);

   double limbVec[3];
   UtVec3d::CrossProduct(limbVec, sunHat, upVec);

   UtVec3d::Multiply(limbVec, UtSun::cMEAN_RADIUS);
   UtVec3d::Add(aUpperLimbWCS, sunLoc, limbVec);
   UtVec3d::Subtract(aLowerLimbWCS, sunLoc, limbVec);
}
} // namespace

//! Return the given location's state with respect to the solar terminator.
//! \param aLatDegrees   The latitude of interest.
//! \param aLonDegrees   The longitude of interest.
//! \param aTime         The time of interest.
//! \param aLimitDegrees The angle limit giving the end of twilight.
//! \return              The phase of the day at the given surface location.
PhaseOfDay GetPhaseOfDay(double aLatDegrees, double aLonDegrees, const UtCalendar& aTime, double aLimitDegrees)
{
   UtVec3d vecNED;
   UtSun::GetSunVecNED(aLatDegrees, aLonDegrees, aTime, vecNED);
   double cosTheta = -vecNED.Get(2);
   double cosLimit = cos(aLimitDegrees * UtMath::cRAD_PER_DEG);

   PhaseOfDay retval{PhaseOfDay::cDAY};
   if (cosTheta > cCOS_TWILIGHT_BEGIN)
   {
      retval = PhaseOfDay::cDAY;
   }
   else if (cosTheta > cosLimit)
   {
      retval = PhaseOfDay::cTWILIGHT;
   }
   else
   {
      retval = PhaseOfDay::cNIGHT;
   }
   return retval;
}

//! Return if the given target location is masked by the horizon when viewed from the observer location.
//! Note that this does not take terrain into account.
//! \param aObserverWCS The observer's location.
//! \param aTargetWCS   The target's location.
//! \return              true if the target is not visible from the observer; false otherwise
bool MaskedByHorizon(const double aObserverWCS[3], const double aTargetWCS[3])
{
   // The basic approach is to define the equation of the Earth ellipsoid, and a line going from the observer to the
   // target and search for intersections. This amounts to looking for solutions to a quadratic.
   double TminusO[3];
   UtVec3d::Subtract(TminusO, aTargetWCS, aObserverWCS);
   double lambdaTarget = UtVec3d::Normalize(TminusO);

   // The EllipsoidalInnerProduct is positive definite, so aCoeff is going to be positive (unless the
   // observer is at the center of the Sun).
   double aCoeff = EllipsoidalInnerProduct(TminusO, TminusO);
   double bCoeff = 2.0 * EllipsoidalInnerProduct(aObserverWCS, TminusO);
   double cCoeff = EllipsoidalFunction(aObserverWCS);

   bool   retval{false};
   double discrim = bCoeff * bCoeff - 4.0 * aCoeff * cCoeff;
   if (discrim >= 0.0)
   {
      double sqrtDiscrim = sqrt(discrim);
      // Because aCoeff is positive, we know solnOne >= solnTwo.
      double solnOne = (-bCoeff + sqrtDiscrim) / (2.0 * aCoeff);
      double solnTwo = (-bCoeff - sqrtDiscrim) / (2.0 * aCoeff);

      // Cases (excluding those for which solnTwo > solnOne, which is impossible):
      // 1: solnOne > 0,  solnTwo > 0  --> Line to limb passes through Earth.
      // 2: solnTwo > lambdaTarget     --> First intersection with Earth is on other side of target.
      // 3: solnOne > 0,  solnTwo == 0 --> Line to limb starts on dark side surface and passes through Earth.
      // 4: solnOne > 0,  solnTwo < 0  --> Line to limb starts inside Earth.
      // 5: solnOne == 0, solnTwo == 0 --> Marginal case where line from observer to limb starts precisely on the
      //                                   terminator with respect to that limb.
      // 6: solnOne == 0, solnTwo < 0  --> Line to limb starts on the light side surface of the Earth.
      // 7: solnOne < 0,  solnTwo < 0  --> Line to limb starts above the light side surface of the Earth.
      //
      // For cases 1, 3, 4, this method should return true (i.e., masked);
      // for cases 2, 5-7, this methos should return false (i.e., not masked).

      // Numerically, for positions right on the surface of the Earth, solnOne may be within a small margin
      // of zero. cMASKED_BY_EARTH_TOLERANCE allows a slight deviation from zero.
      retval = solnOne > cMASKED_BY_EARTH_TOLERANCE && solnTwo + cMASKED_BY_EARTH_TOLERANCE < lambdaTarget;
   }

   return retval;
}

//! Return qualitatively how much illumination the given platform is receiving from the sun.
//! This will determine based on the location of the platform as of its most recent update
//! if the platform is illuminated, in the Earth's penumbra, or in the Earth's umbra.
//! This method will only given valid results for a platform that is part of a simulation.
//! \note This method does not currently account for refraction through Earth's atmosphere.
//! \param aPlatformPtr The platform in question.
//! \return             The illumination by the Sun of the given platform.
PlatformSolarIllumination GetPlatformSolarIllumination(WsfPlatform* aPlatformPtr)
{
   // Platform must be part of a simulation to use this method.
   if (aPlatformPtr->GetSimulation() == nullptr)
   {
      return PlatformSolarIllumination::cINVALID_PLATFORM;
   }

   double platformLocation[3];
   aPlatformPtr->GetLocationWCS(platformLocation);

   double       upperLimb[3];
   double       lowerLimb[3];
   UtCalendar   time{};
   WsfDateTime& dateTime = aPlatformPtr->GetSimulation()->GetDateTime();
   dateTime.GetCurrentTime(aPlatformPtr->GetLastUpdateTime(), time);
   GetDisplacementToSolarLimbs(platformLocation, time, upperLimb, lowerLimb);

   bool upperMasked = MaskedByHorizon(platformLocation, upperLimb);
   bool lowerMasked = MaskedByHorizon(platformLocation, lowerLimb);

   PlatformSolarIllumination retval{PlatformSolarIllumination::cINVALID_PLATFORM};
   if (upperMasked && lowerMasked)
   {
      retval = PlatformSolarIllumination::cEARTH_UMBRA;
   }
   else if (!upperMasked && lowerMasked)
   {
      retval = PlatformSolarIllumination::cEARTH_PENUMBRA;
   }
   else if (upperMasked && !lowerMasked)
   {
      assert(0 && "The upper limb should never be masked if the lower is not.");
   }
   else
   {
      retval = PlatformSolarIllumination::cILLUMINATED;
   }

   return retval;
}

} // namespace WsfSolarTerminator
