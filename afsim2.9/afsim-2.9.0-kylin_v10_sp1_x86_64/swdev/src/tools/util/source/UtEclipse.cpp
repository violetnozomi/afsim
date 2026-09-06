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

#include "UtEclipse.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <math.h>
#include <stdexcept>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtPolynomial.hpp"
#include "UtSphericalEarth.hpp"
#include "UtSun.hpp"
#include "UtVec2.hpp"

namespace
{
//! Determine the eclipsed state.
//! This algorithm is from Vallado, Fundamentals of Astrodynamics and Applications, 4th ed., p. 301-302.
UtEclipse::EclipsedState GetEclipsedStateP(UtVec3d& aSunLocECI, const UtVec3d& aLocationECI)
{
   UtEclipse::EclipsedState es         = UtEclipse::cECLIPSED_NONE;
   double                   r          = aLocationECI.Magnitude();
   double                   rsun       = aSunLocECI.Magnitude();
   double                   r_dot_rsun = UtVec3d::DotProduct(aLocationECI, aSunLocECI);
   if (r_dot_rsun < 0.0)
   {
      double              cosAngle      = std::min(std::max(-r_dot_rsun / r / rsun, -1.0), 1.0);
      double              sinAngle      = sin(acos(cosAngle));
      double              satHorizontal = r * cosAngle;
      double              satVertical   = r * sinAngle;
      static const double rp            = UtSphericalEarth::cEARTH_MEAN_RADIUS;
      static const double sinAlphaPen   = (UtSun::cMEAN_RADIUS + rp) / rsun;
      static const double tanAlphaPen   = tan(asin(sinAlphaPen));
      double              x             = rp / sinAlphaPen;
      double              Penvert       = tanAlphaPen * (x + satHorizontal);
      if (satVertical <= Penvert)
      {
         es                              = UtEclipse::cECLIPSED_PENUMBRAL;
         static const double sinAlphaUmb = (UtSun::cMEAN_RADIUS - rp) / rsun;
         static const double tanAlphaUmb = tan(asin(sinAlphaUmb));
         double              y           = rp / sinAlphaUmb;
         double              Umbvert     = tanAlphaUmb * (y - satHorizontal);
         if (satVertical <= Umbvert)
         {
            es = UtEclipse::cECLIPSED_UMBRAL;
         }
      }
   }

   return es;
}

//! Computes the derivate of the shadow function with respect to the true anomaly,
//! in order to determine if an eclipse solution is an entry solution (derivative > 0),
//! or an exit solution (derivative < 0.0).
double GetShadowFunctionDerivative(double aSLR, double aEcc, double aSinTA, double aCosTA, double aBeta1, double aBeta2)
{
   static const double Re         = UtSphericalEarth::cEARTH_MEAN_RADIUS;
   double              a          = Re / aSLR;
   double              derivative = -2.0 * aEcc * a * a * aSinTA * (1.0 + aEcc * aCosTA) +
                       2.0 * (aBeta1 * aCosTA + aBeta2 * aSinTA) * (-aBeta1 * aSinTA + aBeta2 * aCosTA);
   return derivative;
}

double GetShadowFunctionP(double aSLR, double aEcc, double aSinTA, double aCosTA, double aBeta1, double aBeta2)
{
   static const double Re = UtSphericalEarth::cEARTH_MEAN_RADIUS;
   double              a  = Re / aSLR;
   double              S  = a * a * pow((1 + aEcc * aCosTA), 2) + pow((aBeta1 * aCosTA + aBeta2 * aSinTA), 2) - 1.0;
   return S;
}

double GetTimeToAnomalyPassage(const UtOrbitalElements& aElements, double aTargetTrueAnomaly)
{
   double currentMeanAnomaly = aElements.GetMeanAnomaly();
   double targetMeanAnomaly  = UtOrbitalElements::ComputeMeanAnomaly(aTargetTrueAnomaly, aElements.GetEccentricity());
   double anomalyDiff        = targetMeanAnomaly - currentMeanAnomaly;
   if (anomalyDiff < 0.0)
   {
      anomalyDiff += UtMath::cTWO_PI;
   }
   return (anomalyDiff / aElements.GetMeanMotion());
}
} // end anonymous namespace

//! Determine whether a given point in ECI coordinates is eclipsed by the Earth.
//! @param aTime The time to use for the eclipse calculation.
//! @param aLocationECI The location
UtEclipse::EclipsedState UtEclipse::GetEclipsedState(const UtCalendar& aTime, const UtVec3d& aLocationECI)
{
   UtVec3d sunLocECI;
   UtSun::GetSunLocationECI(aTime, sunLocECI.GetData());
   return GetEclipsedStateP(sunLocECI, aLocationECI);
}

//! Compute the points in a satellite's orbit at which it enters and exits the earth's shadow.
//! @param aTime The evaluation time, used to fix the sun's location.
//! @param aElements The orbital elements used to represent the orbit used in the eclipse calculations.
//! @param aEntryAnomaly The true anomaly of the satellite when it enters earth's shadow.
//! @param aExitAnomaly The true anomaly of the satellite when it exits earth's shadow.
//! @return Whether the satellite is eclipsed on its current orbit.
//! @note These results are approximate as they do not take into account the motion of the earth during the orbit.
//! This algorithm also does not compute penumbral eclipsing.
bool UtEclipse::GetEclipseAnomalies(const UtCalendar&        aTime,
                                    const UtOrbitalElements& aElements,
                                    double&                  aEntryAnomaly,
                                    double&                  aExitAnomaly)
{
   UtVec3d        Pperifocal(1.0, 0.0, 0.0);
   UtVec3d        Qperifocal(0.0, 1.0, 0.0);
   UtVec3d        Peci;
   UtVec3d        Qeci;
   const UtMat3d& perifocalToInertialTransform = aElements.GetPerifocalToInertialTransform();
   perifocalToInertialTransform.TransposeMultiply(Peci, Pperifocal);
   perifocalToInertialTransform.TransposeMultiply(Qeci, Qperifocal);

   UtVec3d sunLoc;
   UtSun::GetSunLocationECI(aTime, sunLoc.GetData());
   double sunDistance = sunLoc.Magnitude();
   double beta1       = sunLoc.DotProduct(Peci) / sunDistance; // see Vallado, p. 303
   double beta2       = sunLoc.DotProduct(Qeci) / sunDistance;

   // Compute the shadow function according to Vallado p.304 (eq. 5-5)
   double                  e               = aElements.GetEccentricity();
   double                  e2              = e * e;
   double                  semiLatusRectum = aElements.GetSemiMajorAxis() * (1.0 - e2);
   double                  alpha           = UtSphericalEarth::cEARTH_MEAN_RADIUS / semiLatusRectum;
   double                  a2              = alpha * alpha;
   double                  a4              = a2 * a2;
   double                  b1_2            = beta1 * beta1;
   double                  b2_2            = beta2 * beta2;
   double                  oneMinus_b2_2   = 1.0 - b2_2;
   double                  b2_2_Minus_b1_2 = b2_2 - b1_2;
   double                  solutions[4];
   size_t                  numSolutions              = 0;
   static constexpr double cCIRCULAR_ECCENTRIC_LIMIT = 1.0e-12; // needs to be tested for a range of values.
   if (aElements.GetEccentricity() > cCIRCULAR_ECCENTRIC_LIMIT) // standard case (elliptical)
   {
      double coeff[5] = {a4 - 2.0 * a2 * oneMinus_b2_2 + pow(oneMinus_b2_2, 2),
                         4.0 * e * a2 * (a2 - oneMinus_b2_2),
                         2.0 * a2 * e2 * (3.0 * a2 - oneMinus_b2_2) + 2.0 * b2_2_Minus_b1_2 * (oneMinus_b2_2 - a2) -
                            4.0 * b2_2 * b1_2,
                         4.0 * e * a2 * (a2 * e2 - b2_2_Minus_b1_2),
                         a2 * e2 * (a2 * e2 - 2.0 * b2_2_Minus_b1_2) + pow((b1_2 + b2_2), 2)};
      numSolutions    = UtPolynomial::SolveQuartic(coeff, solutions);
   }
   else // circular case; set eccentricity to exactly zero to prevent problems with roundoff error in SolveQuartic
   {
      double coeff[5] = {a4 - 2.0 * a2 * oneMinus_b2_2 + pow(oneMinus_b2_2, 2),
                         0.0,
                         2.0 * b2_2_Minus_b1_2 * (oneMinus_b2_2 - a2) - 4.0 * b2_2 * b1_2,
                         0.0,
                         pow((b1_2 + b2_2), 2)};
      numSolutions    = UtPolynomial::SolveQuartic(coeff, solutions); // This becomes a bi-quadratic solution
   }

   bool foundEntry = false;
   bool foundExit  = false;

   if (numSolutions >= 2) // The returned solutions should include both the entry and exit points.
   {
      for (size_t solutionNum = 0; solutionNum < numSolutions; ++solutionNum)
      {
         double cosTA = solutions[solutionNum];
         double ta    = acos(cosTA);
         double sinTA = sin(ta);

         static const double sSHADOW_FUNCTION_THRESHOLD_TOLERANCE = 1.0e-7;
         static const double sBETA_THRESHOLD_TOLERANCE            = 1.0e-9;

         double cosSigma = 0.0;
         if (fabs(beta1) > sBETA_THRESHOLD_TOLERANCE) // standard case
         {
            // This may be a valid solution but may also be in the wrong quadrant.
            // Check with the original function; it should be zero.
            double shadowFunction = GetShadowFunctionP(semiLatusRectum, e, sinTA, cosTA, beta1, beta2);

            // If not valid, move to the correct quadrant.
            if (fabs(shadowFunction) >
                sSHADOW_FUNCTION_THRESHOLD_TOLERANCE) // Shadow function not equal to zero shows quadrant problem.
            {
               ta    = UtMath::cTWO_PI - ta;
               sinTA = -sinTA;
            }

            cosSigma = beta1 * cosTA + beta2 * sinTA;
         }
         else // When beta1 is zero, the shadow function is identically zero, so we cannot use the a shadow function test.
         {
            // There will be only two valid solutions, so we change quadrant directly based on cosSigma value.
            cosSigma = beta2 * sinTA;
            if (cosSigma >= 0.0)
            {
               ta       = UtMath::cTWO_PI - ta;
               sinTA    = -sinTA;
               cosSigma = -cosSigma;
            }
         }

         // Check whether we are in earth's shadow or in front of the earth (another quadrant check).
         if (cosSigma < 0.0) // In earth's shadow.
         {
            if (fabs(beta2) > sBETA_THRESHOLD_TOLERANCE) // standard case
            {
               // If the shadow function derivative is negative at the specific anomaly, we are going into shadow;
               // otherwise we are exiting.
               double shadowDerivative = GetShadowFunctionDerivative(semiLatusRectum, e, sinTA, cosTA, beta1, beta2);
               if (shadowDerivative > 0.0) // going into shadow
               {
                  aEntryAnomaly = ta;
                  foundEntry    = true;
               }
               else
               {
                  aExitAnomaly = ta;
                  foundExit    = true;
               }
            }
            else // Special case; beta2 is zero; P-axis aligned with sun axis.
            {
               // If the shadow function derivative is negative at the specific anomaly, we are going into shadow;
               // otherwise we are exiting.
               double shadowDerivative = GetShadowFunctionDerivative(semiLatusRectum, e, sinTA, cosTA, beta1, beta2);
               if (shadowDerivative > 0.0) // going into shadow
               {
                  aEntryAnomaly = ta;
                  aExitAnomaly  = UtMath::cTWO_PI - ta;
               }
               else
               {
                  aExitAnomaly  = ta;
                  aEntryAnomaly = UtMath::cTWO_PI - ta;
               }
               assert(GetShadowFunctionP(semiLatusRectum, e, sinTA, cosTA, beta1, beta2) <
                      sSHADOW_FUNCTION_THRESHOLD_TOLERANCE);
               assert(GetShadowFunctionP(semiLatusRectum, e, -sinTA, cosTA, beta1, beta2) <
                      sSHADOW_FUNCTION_THRESHOLD_TOLERANCE);
               foundEntry = true;
               foundExit  = true;
            }
         }
         if (foundEntry && foundExit)
         {
            break;
         }
      }
   }
   if (foundEntry != foundExit)
   {
      auto out = ut::log::error() << "GetEclipseAnomaliesP did not find both solutions.";
      out.AddNote() << "Found Entry: " << foundEntry;
      out.AddNote() << "Found Exit: " << foundExit;
      out.AddNote() << "No solutions will be returned.";
   }

   return foundEntry && foundExit;
}

//! Compute the points in a satellite's orbit at which it enters and exits the earth's shadow.  The time of
//! evaluation (used to fix the sun's position) is assumed to be the epoch associated with the orbital elements.
//! @param aElements The orbital elements used to represent the orbit used in the eclipse calculations.
//! @param aEntryAnomaly The true anomaly of the satellite when it enters earth's shadow.
//! @param aExitAnomaly The true anomaly of the satellite when it exits earth's shadow.
//! @return Whether the satellite is eclipsed on its current orbit.
//! @note These results are approximate as they do not take into account the motion of the earth during the orbit.
//! This algorithm also does not compute penumbral eclipsing.
bool UtEclipse::GetEclipseAnomalies(const UtOrbitalElements& aElements, double& aEntryAnomaly, double& aExitAnomaly)
{
   return GetEclipseAnomalies(aElements.GetEpoch(), aElements, aEntryAnomaly, aExitAnomaly);
}

//! Compute the "shadow function" (e.g., Vallado, Fundamentals of Astrodynamics and Applications, 4th ed., p. 303, eq. 5-4.)
//! @param aTime The time that is used to compute the earth-sun geometry.
//! @param aElements The orbital elements used to compute the shadow geometry.
//! @return The shadow function, which is zero at eclipse entry / exit points, positive within the shadow, and negative
//! outside the shadow.
double UtEclipse::GetShadowFunction(const UtCalendar& aTime, const UtOrbitalElements& aElements)
{
   UtVec3d        Pperifocal(1.0, 0.0, 0.0);
   UtVec3d        Qperifocal(0.0, 1.0, 0.0);
   UtVec3d        Peci;
   UtVec3d        Qeci;
   const UtMat3d& perifocalToInertialTransform = aElements.GetPerifocalToInertialTransform();
   perifocalToInertialTransform.TransposeMultiply(Peci, Pperifocal);
   perifocalToInertialTransform.TransposeMultiply(Qeci, Qperifocal);

   UtVec3d sunLoc;
   UtSun::GetSunLocationECI(aTime, sunLoc.GetData());
   double sunDistance = sunLoc.Magnitude();
   double beta1       = sunLoc.DotProduct(Peci) / sunDistance; // see Vallado, p. 303
   double beta2       = sunLoc.DotProduct(Qeci) / sunDistance;

   // Compute the shadow function according to Vallado p.304 (eq. 5-5)
   double semiLatusRectum =
      aElements.GetSemiMajorAxis() * (1.0 - aElements.GetEccentricity() * aElements.GetEccentricity());
   double sinTA = sin(aElements.GetTrueAnomaly());
   double cosTA = cos(aElements.GetTrueAnomaly());
   return GetShadowFunctionP(semiLatusRectum, aElements.GetEccentricity(), sinTA, cosTA, beta1, beta2);
}

//! Get the times in seconds from the propagator's current time to the times of next eclipse entry into, and exit from,
//! Earth's shadow.
//! @param aTime The evaluation time, used to fix the sun's location.
//! @param aElements The orbital elements used to represent the orbit used in the eclipse calculations.
//! @param aTimeToEntry The output time of entry of the satellite into Earth's shadow.
//! @param aTimeToExit The output time of exit of the satellite from Earth's shadow.
//! @return Whether the satellite is eclipsed on its current orbit.
bool UtEclipse::GetEclipseTimes(const UtCalendar&        aTime,
                                const UtOrbitalElements& aElements,
                                double&                  aTimeToEntry,
                                double&                  aTimeToExit)
{
   double entry;
   double exit;
   bool   found = GetEclipseAnomalies(aTime, aElements, entry, exit);
   if (found)
   {
      aTimeToEntry = GetTimeToAnomalyPassage(aElements, entry);
      aTimeToExit  = GetTimeToAnomalyPassage(aElements, exit);
   }
   return found;
}
//! Get the times in seconds from the propagator's current time to the times of next eclipse entry into, and exit from,
//! Earth's shadow. The time of evaluation (used to fix the sun's position) is assumed to be the epoch associated with
//! the orbital elements.
//! @param aTimeToEntry The output time of entry of the satellite into Earth's shadow.
//! @param aTimeToExit The output time of exit of the satellite from Earth's shadow.
//! @return Whether the satellite is eclipsed on its current orbit.
bool UtEclipse::GetEclipseTimes(const UtOrbitalElements& aElements, double& aTimeToEntry, double& aTimeToExit)
{
   return GetEclipseTimes(aElements.GetEpoch(), aElements, aTimeToEntry, aTimeToExit);
}

//! Return the fraction of the secondary body covered by the primary body from the given viewing location.
//! This method computes the fraction of the secondary object (given as a @p aLocationSecondary and @p
//! aRadiusSecondary) that appears covered by the primary object (given as a @p aLocationPrimary and
//! @p aRadiusPrimary) from the vantage point given, @p aViewingLocation. This will return a value between
//! 0 and 1, where zero means there is no overlap of the secondary by the primary, and 1 meaning that the
//! secondary is completely covered. The input positions need to all be in the same reference frame.
//! If the @p aViewingLocation is inside the primary object, this will return 1.0; if it is inside the
//! secondary, this will return 0.0. If either of the provided @p aRadiusPrimary or @p aRadiusSecondary
//! are not positive, this will throw a std::domain_error. This will also throw a std::domain_error if
//! the primary and secondary object are closer than the sum of their radii (i.e., they are intersecting).
//! @param aLocationPrimary The location of the obscuring object.
//! @param aRadiusPrimary The radius of the obscuring object.
//! @param aLocationSecondary The location of the obscured object.
//! @param aRadiusSecondary The radius of the obscured object.
//! @param aViewingLocation The location from which the potential obscuration is being observed.
double UtEclipse::GetUmbralFraction(const UtVec3d& aLocationPrimary,
                                    double         aRadiusPrimary,
                                    const UtVec3d& aLocationSecondary,
                                    double         aRadiusSecondary,
                                    const UtVec3d& aViewingLocation)
{
   // This method imagines projecting the two spherical objects (primary and secondary) onto
   // a unit sphere centered on the viewer. The limbs of the two objects will project onto
   // a 'little circle' on the projection surface (that is, they are not necessarily great
   // circles). From there, it is a matter of some spherical trigonometry to determine the
   // areas of overlap. Denote the center of the secondary object point A, the center of
   // the primary object point B, the two points of intersection of the little circles of
   // the limbs of primary and secondary objects C and D.
   //
   // This routine makes heavy use of the spherical law of cosines, the area of a spherical
   // triangle, and an expression for the area of a little circle on a sphere.

   if (aRadiusPrimary <= 0.0)
   {
      throw std::domain_error{"UtEclipse::GetUmbralFraction: radius of primary out of bounds."};
   }
   if (aRadiusSecondary <= 0.0)
   {
      throw std::domain_error{"UtEclipse::GetUmbralFraction: radius of secondary out of bounds."};
   }
   if ((aLocationPrimary - aLocationSecondary).Magnitude() < aRadiusPrimary + aRadiusSecondary)
   {
      throw std::domain_error{"UtEclipse::GetUmbralFraction: primary and secondary objects intersect."};
   }

   // Get unit vectors and distances from viewer to primary.
   UtVec3d ellP = aLocationPrimary - aViewingLocation;
   double  lP   = ellP.Normalize();
   if (lP < aRadiusPrimary)
   {
      // Inside the primary object; totally obscured.
      return 1.0;
   }

   // Get unit vectors and distances from viewer to secondary.
   UtVec3d ellS = aLocationSecondary - aViewingLocation;
   double  lS   = ellS.Normalize();
   if (lS < aRadiusSecondary)
   {
      // Inside the secondary; totally visible.
      return 0.0;
   }

   // Compute the angle from the center of the primary to the limb of the primary.
   double sinRp = aRadiusPrimary / lP;
   double rP    = std::asin(sinRp);
   double cosRp = std::cos(rP);

   // Compute the angle from the center of the secondary to the limb of the secondary.
   double sinRs         = aRadiusSecondary / lS;
   double rS            = std::asin(sinRs);
   double cosRs         = std::cos(rS);
   double areaSecondary = UtMath::cTWO_PI * (1.0 - cosRs);

   // Compute the angle between the centers of the projection of the primary and secondary objects.
   double cosCp = UtVec3d::DotProduct(ellP, ellS);
   double cP    = std::acos(cosCp);
   double sinCp = std::sin(cP);

   // A few simple to detect cases.
   if (cP - rP >= rS)
   {
      // No overlap; totally visible.
      return 0.0;
   }
   else if (cP - rP <= -rS)
   {
      // Complete overlap; totally obscured.
      return 1.0;
   }
   else if (UtMath::NearlyZero(sinCp))
   {
      // The two object are aligned, and the secondary will not be totally covered by
      // the primary. Obscuration fraction is the ratio of areas.
      double areaPrimary = UtMath::cTWO_PI * (1.0 - cosRp);
      return areaPrimary / areaSecondary;
   }

   // Find azimuth to intersection of the two little circles.
   double cosPhiC = (cosRp - cosCp * cosRs) / (sinCp * sinRs);
   double phiC    = std::acos(cosPhiC);

   double areaObscured{0.0};

   if (cosPhiC > 0.0)
   {
      // The CD side passes through spherical triangle ABC

      // m is the side from C to D.
      double cosM = cosRs * cosRs + sinRs * sinRs * std::cos(2.0 * phiC);
      double m    = std::acos(cosM);
      double sinM = std::sin(m);

      // lambdaS is the angle from AC to CD (and AD to CD).
      double cosLambdaS = (cosRs - cosRs * cosM) / (sinRs * sinM);
      double lambdaS    = std::acos(cosLambdaS);

      // lambdaB is the angle from BC to BD.
      double cosLambdaB = (cosM - cosRp * cosRp) / (sinRp * sinRp);
      double lambdaB    = std::acos(cosLambdaB);

      // lambdaP is the angle from BC to CD (and BD to CD);
      double cosLambdaP = (cosRp - cosRp * cosM) / (sinRp * sinM);
      double lambdaP    = std::acos(cosLambdaP);

      // Now compute areas.
      double areaTriangleACD = 2.0 * phiC + 2.0 * lambdaS - UtMath::cPI;
      double areaSectorACD   = 2.0 * phiC * (1.0 - cosRs);
      double areaTriangleBCD = lambdaB + 2.0 * lambdaP - UtMath::cPI;
      double areaSectorBCD   = lambdaB * (1.0 - cosRp);
      areaObscured           = areaSectorBCD - areaTriangleBCD + areaSectorACD - areaTriangleACD;
   }
   else if (cosPhiC < 0.0)
   {
      // The CD side does not pass through spherical triangle ABC

      // omegaI is the angle between CA and CB (or DA and DB).
      double cosOmegaI = (cosCp - cosRs * cosRp) / (sinRs * sinRp);
      double omegaI    = std::acos(cosOmegaI);

      // omegaP is the angle between AB and BC (or AB and BD).
      double cosOmegaP = (cosRs - cosCp * cosRp) / (sinCp * sinRp);
      double omegaP    = std::acos(cosOmegaP);

      // m is the side from C to D.
      double cosM = cosRp * cosRp + sinRp * sinRp * std::cos(2.0 * omegaP);
      double m    = std::acos(cosM);
      double sinM = std::sin(m);

      // omegaT is the angle between AC and CD.
      double cosOmegaI_PlusOmegaT = (cosRp - cosM * cosRp) / (sinM * sinRp);
      double omegaT               = std::acos(cosOmegaI_PlusOmegaT) - omegaI;

      // omegaC is the angle between AC and AD.
      double omegaC = UtMath::cTWO_PI - 2.0 * phiC;

      // Now compute areas
      double areaSectorBCD   = 2.0 * omegaP * (1.0 - cosRp);
      double areaTriangleBCD = 2.0 * omegaP + 2.0 * omegaI + 2.0 * omegaT - UtMath::cPI;
      double areaSectorACD   = 2.0 * phiC * (1.0 - cosRs);
      double areaTriangleACD = omegaC + 2.0 * omegaT - UtMath::cPI;
      areaObscured           = areaSectorBCD - areaTriangleBCD + areaSectorACD + areaTriangleACD;
   }
   else
   {
      // The CD side is along the AC side of spherical triangle ABC

      // lambdaB is the angle between BC and BD
      double cosLambdaB = (std::cos(2.0 * rS) - cosRp * cosRp) / (sinRp * sinRp);
      double lambdaB    = std::acos(cosLambdaB);

      // lambdaC is the angle between AC and CB
      double cosLambdaC = (cosCp - cosRs * cosRp) / (sinRs * sinRp);
      double lambdaC    = std::acos(cosLambdaC);

      // Compute areas
      double areaSectorACD   = areaSecondary / 2.0;
      double areaSectorBCD   = lambdaB * (1.0 - cosRp);
      double areaTriangleBCD = lambdaB + 2.0 * lambdaC - UtMath::cPI;
      areaObscured           = areaSectorACD + areaSectorBCD - areaTriangleBCD;
   }

   // Return the ratio of obscured to secondary area to get the obscured fraction
   return areaObscured / areaSecondary;
}
