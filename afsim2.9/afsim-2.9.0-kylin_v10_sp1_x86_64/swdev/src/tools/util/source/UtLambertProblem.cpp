// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtLambertProblem.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "UtEarth.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"


namespace // {anonymous}
{

//! Utility method used by LambertUniversal, below.
void FindC2_C3(double aZnew, double& aC2New, double& aC3New)
{
   static const double cSMALL = 1.0e-6;

   if (aZnew > cSMALL)
   {
      double sqrtZ    = sqrt(aZnew);
      double sinSqrtZ = sin(sqrtZ);
      double cosSqrtZ = cos(sqrtZ);
      aC2New          = (1.0 - cosSqrtZ) / aZnew;
      aC3New          = (sqrtZ - sinSqrtZ) / pow(sqrtZ, 3);
   }
   else if (aZnew < -cSMALL)
   {
      double sqrtZ = sqrt(-aZnew);
      aC2New       = (1.0 - cosh(sqrtZ)) / aZnew;
      aC3New       = (sinh(sqrtZ) - sqrtZ) / pow(sqrtZ, 3);
   }
   else
   {
      aC2New = 0.5;
      aC3New = 1.0 / 6.0;
   }
}

} // namespace

namespace UtLambertProblem
{

//! A version of Gauss' solution of the Lambert problem (given two locations and time, find the orbital velocities at
//! each of those two points).
//! @param aLocationInertial_1 The first provided satellite location in Inertial coordinates.
//! @param aLocationInertial_2 The second provided satellite location in Inertial coordinates.
//! @param aDt The elapsed time for the satellite to have gone from aLocInertial_1 to aLocInertial_2 (time between
//! observations).
//! @param aVelocityInertial_1 The computed Inertial velocity of the satellite at the first point.
//! @param aVelocityInertial_2 The computed Inertial velocity of the satellite at the second point.
//! @param aGravitationalParameter The gravitational parameter to be used in computing the solution.
//! @param aIsShortWay Whether the solution should be computed assuming aDt is for the satellite to go "the short way"
//! along the ellipse from one point to the other, or "the long way."
//! @param aConvergenceTolerance (optional) The numerical tolerance at which the iteration stops and a solution is
//! assumed "good enough."
bool Gauss(const UtVec3d& aLocationInertial_1,
           const UtVec3d& aLocationInertial_2,
           double         aDt,
           UtVec3d&       aVelocityInertial_1,
           UtVec3d&       aVelocityInertial_2,
           double         aGravitationalParameter,
           bool           aIsShortWay,
           double         aConvergenceTolerance /*= 1.0e-12*/)
{
   bool   solved = true;
   double r1     = aLocationInertial_1.Magnitude();
   double r2     = aLocationInertial_2.Magnitude();
   double cosDeltaNu =
      std::max(std::min(UtVec3d::DotProduct(aLocationInertial_1, aLocationInertial_2) / r1 / r2, 1.0), -1.0);
   double sinDeltaNu = sqrt(std::max(0.0, 1.0 - cosDeltaNu * cosDeltaNu));
   if (!aIsShortWay)
   {
      sinDeltaNu = -sinDeltaNu;
   }
   double              deltaNu    = acos(cosDeltaNu);
   double              parameter1 = sqrt(r1 * r2) * cos(deltaNu / 2.0);
   double              l          = (r1 + r2) / (4.0 * parameter1) - 0.5;
   double              m          = aGravitationalParameter * aDt * aDt / pow((2.0 * parameter1), 3);
   double              y          = 1; // initial guess
   double              lastY      = 10000.0;
   static const size_t sMaxIter   = 40;
   size_t              iteration  = 0;
   double              x1         = 0.0;

   while (true)
   {
      // Calculate x1
      x1 = m / y / y - l;

      // Calculate x2 by developing up to five terms
      double numeratorCoeff   = 6.0;
      double denominatorCoeff = 5.0;
      double coefficient      = 1.0;
      double x2Series         = 1.0;
      for (size_t i = 1; i < 8; ++i) // Number of terms in series arbitrary (todo make input)
      {
         coefficient *= numeratorCoeff / denominatorCoeff;
         x2Series += coefficient * pow(x1, i);
         numeratorCoeff += 2.0;
         denominatorCoeff += 2.0;
      }
      static const double sFourThirds = 4.0 / 3.0;
      double              x2          = sFourThirds * x2Series;

      y = 1.0 + x2 * (l + x1);
      if ((fabs(y - lastY) <= aConvergenceTolerance) || (iteration == sMaxIter))
      {
         break;
      }
      lastY = y;
      ++iteration;
   };
   if (iteration != sMaxIter)
   {
      // for now only handle eccentric orbits
      double cosDelE_Over2 = 1.0 - 2.0 * x1;
      double p             = r1 * r2 * (1.0 - cosDeltaNu) / (r1 + r2 - 2.0 * parameter1 * cosDelE_Over2);
      double f             = 1.0 - r2 / p * (1.0 - cosDeltaNu);
      double g             = r1 * r2 * sinDeltaNu / sqrt(p * aGravitationalParameter);
      assert(p != 0.0);
      // Fdot is not required for the provided computation of the second velocity.
      double  gDot        = 1.0 - r1 / p * (1.0 - cosDeltaNu);
      UtVec3d fr1         = f * aLocationInertial_1;
      aVelocityInertial_1 = aLocationInertial_2 - fr1;
      aVelocityInertial_1 = aVelocityInertial_1 / g;

      UtVec3d gDotr2      = gDot * aLocationInertial_2;
      aVelocityInertial_2 = gDotr2 - aLocationInertial_1;
      aVelocityInertial_2 = aVelocityInertial_2 / g;
   }
   else
   {
      ut::log::error() << "Max iterations (gauss-lambert).";
      solved = false;
   }

   return solved;
}

//! A version of the solution of the Lambert problem (given two locations and time, find the orbital velocities at each
//! of those two points), using the universal variables formulation (see Vallado, Fundamentals of Astrodynamics and
//! Applications, 4th ed., pp. 489-495, or Bate Mueller, and White, Fundamentals of Astrodynamics, pp. 191-212). This
//! version follows Vallado and is mostly a direct translation of his Fortran version.
//! @param aLocationInertial_1 The first provided satellite location in Inertial coordinates.
//! @param aLocationInertial_2 The second provided satellite location in Inertial coordinates.
//! @param aDt The elapsed time for the satellite to have gone from aLocInertial_1 to aLocInertial_2 (time between
//! observations).
//! @param aEllipsoid The central body ellipsoid to be used in computing the solution.
//! @param aIsShortWay Whether the solution should be computed assuming aDt is for the satellite to go "the short way"
//! along the ellipse from one point to the other, or "the long way."
//! @param aConvergenceTolerance (optional) The numerical tolerance at which the iteration stops and a solution is
//! assumed "good enough."
Result Universal(const UtVec3d&                  aLocationInertial_1,
                 const UtVec3d&                  aLocationInertial_2,
                 double                          aDt,
                 const ut::CentralBodyEllipsoid& aEllipsoid,
                 bool                            aShortWay,
                 double                          aConvergenceTolerance /*= 1.0e-12*/)
{
   bool   solved = true;
   double r1     = aLocationInertial_1.Magnitude();
   double r2     = aLocationInertial_2.Magnitude();
   double cosDeltaNu =
      std::max(std::min(UtVec3d::DotProduct(aLocationInertial_1, aLocationInertial_2) / r1 / r2, 1.0), -1.0);
   double  A = sqrt(r1 * r2 * (1 + cosDeltaNu));
   UtVec3d velocityInertial_1;
   UtVec3d velocityInertial_2;
   if (!aShortWay)
   {
      A = -A;
   }
   static const double cSMALL = 1.0e-6;
   if (fabs(A) > cSMALL)
   {
      // following is a translation of the Vallado fortran to C
      //  ---------------  Form Initial guesses   ---------------------
      double psiOld = 0.0;
      double psiNew = 0.0;
      double xOld   = 0.0;
      double c2New  = 0.5;
      double c3New  = 1.0 / 6.0;

      //  --------- Set up initial bounds for the bissection ----------
      double upper = UtMath::cTWO_PI * UtMath::cTWO_PI;
      // Lower bound is arbitrary, the present value covers all hyperbolic use cases encountered thus far.
      double lower = -30.0;
      double y     = 0;

      //  -------  Determine If  the orbit is possible at all ---------
      unsigned              loops     = 0;
      unsigned              yNegKtr   = 1; //  ! y neg ktr
      double                dtNew     = -10.0;
      static const unsigned cNUM_ITER = 50;
      while ((fabs(dtNew - aDt) >= 0.000001) && (loops < cNUM_ITER) && (yNegKtr < 10))
      {
         y = r1 + r2;
         if (fabs(c2New) > cSMALL)
         {
            y -= A * (1.0 - psiOld * c3New) / sqrt(c2New);
         }
         //  ----------- Check for negative values of y ----------
         if ((A > 0.0) && (y < 0.0))
         {
            yNegKtr = 1;
            while ((y < 0.0) && (yNegKtr < 10))
            {
               psiNew = (upper + psiOld) * 0.5;
               //  -------- Find C2 and C3 functions -----------
               FindC2_C3(psiNew, c2New, c3New);
               psiOld = psiNew;
               y      = r1 + r2;
               if (fabs(c2New) > cSMALL)
               {
                  y -= A * (1.0 - psiOld * c3New) / sqrt(c2New);
               }
               yNegKtr = yNegKtr + 1;
            }
         }
         if (yNegKtr < 10)
         {
            if (fabs(c2New) > cSMALL)
            {
               xOld = sqrt(y / c2New);
            }
            else
            {
               xOld = 0.0;
            }
            double xOldCubed = xOld * xOld * xOld;
            dtNew            = (xOldCubed * c3New + A * sqrt(y)) / sqrt(aEllipsoid.GetGravitationalParameter());

            //  ! --------  Readjust upper and lower bounds -------
            if (dtNew < aDt)
            {
               lower = psiOld;
            }
            if (dtNew > aDt)
            {
               upper = psiOld;
            }
            psiNew = (upper + lower) * 0.5;

            //  ------------- Find c2 and c3 functions ----------
            FindC2_C3(psiNew, c2New, c3New);
            psiOld = psiNew;
            loops  = loops + 1;

            //  --- Make sure the first guess isn't too close ---
            if ((fabs(dtNew - aDt) < cSMALL) && (loops == 1))
            {
               dtNew = aDt - 1.0;
            }
         } //  If  YNegKtr < 10
      }    //  Do While Loop

      if ((loops >= cNUM_ITER) || (yNegKtr >= 10))
      {
         // Error= 'GNotConv'
         if (yNegKtr >= 10)
         {
            // Error= 'Y negative'
            ut::log::error() << "Lambert Universal: y negative...";
         }
         solved = false;
      }
      else
      {
         //  --- Use F and G series to find Velocity Vectors -----
         double f    = 1.0 - y / r1;
         double gdot = 1.0 - y / r2;
         double g    = 1.0 / (A * sqrt(y / aEllipsoid.GetGravitationalParameter())); //  ! 1 over G
         for (size_t i = 0; i < 3; ++i)
         {
            velocityInertial_1[i] = (aLocationInertial_2[i] - f * aLocationInertial_1[i]) * g;
            velocityInertial_2[i] = (gdot * aLocationInertial_2[i] - aLocationInertial_1[i]) * g;
         }
      } //  If  the answer has converged
   }
   else
   {
      // Error= 'impos 180'
      solved = false;
   } //  If  Var A > 0.0

   Result retval{aDt, aLocationInertial_1, aLocationInertial_2, aEllipsoid};
   retval.SetSolution(solved, velocityInertial_1, velocityInertial_2);

   return retval;
}

//! Determine whether an intersect orbit will hit the ellipsoid.
//! This check is for a Keplerian orbit against a spherical body (using the mean radius of the ellipsoid) only.
//! @param aEllipsoid The ellipsoid against which we perform the intersection test.
//! @param aLocationInertial_1 The first provided satellite location in Inertial coordinates.
//! @param aLocationInertial_2 The second provided satellite location in Inertial coordinates.
//! @param aVelocityInertial_1 The computed Inertial velocity of the satellite at the first point.
//! @param aVelocityInertial_2 The computed Inertial velocity of the satellite at the second point.
//! @return Whether the object would hit the ellipsoid.
bool Hits(const ut::CentralBodyEllipsoid& aEllipsoid,
          const UtVec3d&                  aLocationInertial_1,
          const UtVec3d&                  aLocationInertial_2,
          const UtVec3d&                  aVelocityInertial_1,
          const UtVec3d&                  aVelocityInertial_2)
{
   bool   hits = false;
   double r1   = aLocationInertial_1.Magnitude();
   double f1   = aLocationInertial_1.DotProduct(aVelocityInertial_1);
   double r2   = aLocationInertial_2.Magnitude();
   double f2   = aLocationInertial_2.DotProduct(aVelocityInertial_2);
   if (f1 < 0.0 && (f2 > 0.0 || r2 > r1))
   {
      double  v2mag = aVelocityInertial_1.MagnitudeSquared();
      double  rmag  = aLocationInertial_1.Magnitude();
      double  sma   = 1.0 / (2.0 / rmag - v2mag / aEllipsoid.GetGravitationalParameter());
      UtVec3d h;
      h.CrossProduct(aLocationInertial_1, aVelocityInertial_1);
      double h2mag = h.MagnitudeSquared();
      double p     = h2mag / aEllipsoid.GetGravitationalParameter();
      double e     = sqrt((sma - p) / sma);
      double rp    = sma * (1.0 - e);
      hits         = (rp < aEllipsoid.GetMeanRadius());
   }
   return hits;
}

//! Determine whether a candidate orbit is hyperbolic.
//! @param aLocationInertial A location on the orbit at a given time.
//! @param aVelocityInertial A velocity on the orbit at a given time.
//! @return Whether the implied orbit is hyperbolic.
bool Hyperbolic(double aGravitationalPararmeter, const UtVec3d& aLocationInertial, const UtVec3d& aVelocityInertial)
{
   double vSquared = aVelocityInertial.MagnitudeSquared();
   double rMag     = aLocationInertial.Magnitude();
   return (vSquared > (2.0 * aGravitationalPararmeter / rMag));
}

bool Result::Assess(bool aAllowHyperbolic, bool aAllowHitsCentralBody) const
{
   return mIsSolution && (aAllowHyperbolic || !mIsHyperbolic) && (aAllowHitsCentralBody || !aAllowHitsCentralBody);
}

void Result::SetSolution(bool aIsSolution, const UtVec3d& aInitialVelocity, const UtVec3d& aFinalVelocity)
{
   mIsSolution = aIsSolution;
   if (aIsSolution)
   {
      mInitialVelocity = aInitialVelocity;
      mFinalVelocity   = aFinalVelocity;
      mIsHyperbolic =
         UtLambertProblem::Hyperbolic(mEllipsoidPtr->GetGravitationalParameter(), mInitialPosition, mInitialVelocity);
      mHitsCentralBody =
         UtLambertProblem::Hits(*mEllipsoidPtr, mInitialPosition, mFinalPosition, mInitialVelocity, mFinalVelocity);
   }
}

} // namespace UtLambertProblem
