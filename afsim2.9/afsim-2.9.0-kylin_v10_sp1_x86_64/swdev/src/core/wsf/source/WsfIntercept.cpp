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

#include "WsfIntercept.hpp"

#include <algorithm>

#include "UtVec3.hpp"

//! A low level routine to calculate the intercept point and the time to intercept.
//! This routine ignores the heading of the intercepting body and assumes it can turn instantly.
//!
//! @param aBodyLoc          The location of the body doing the intercepting.
//! @param aBodySpd          The speed of the body doing the intercepting. (assumed constant)
//! @param aTgtLoc           The 'target' location.
//! @param aTgtVel           The 'target' velocity vector.
//! @param aIntLoc           [output] The intercept location.
//! @param aPredictAheadTime The predict ahead time to add/remove additional time to intercept.
//! @returns The time to the intercept point. If the value is less than zero then
//! intercept is not possible and aIntLoc will be set to the value of aTgtLoc.
//!
//! @note The input vectors must all have the same reference frame, i.e. they must
//! all be WCS or must all refer to the same NED frame.
double WsfIntercept::Intercept(const double aBodyLoc[3],
                               double       aBodySpd,
                               const double aTgtLoc[3],
                               const double aTgtVel[3],
                               double       aIntLoc[3],
                               double       aPredictAheadTime /* =0.0*/)
{
   // Assume target and body on coincident.
   double intTime = -1.0;
   UtVec3d::Set(aIntLoc, aTgtLoc);

   // Trivial failure if no bullet velocity
   if (aBodySpd <= 0.0)
   {
      return intTime;
   }

   // Target speed and unit vector
   double tgtVelHat[3]; // dirT
   double tgtSpd = UtVec3d::Normalize(tgtVelHat, aTgtVel);

   // Compute the target-to-body range and unit vector;
   double tgtToBodyLoc[3];
   double tgtToBodyLocHat[3];
   UtVec3d::Subtract(tgtToBodyLoc, aTgtLoc, aBodyLoc);
   /*double tgtToBodyRng = */ UtVec3d::Normalize(tgtToBodyLocHat, tgtToBodyLoc);

   double bodyToTgtLoc[3];
   double bodyToTgtLocHat[3];
   UtVec3d::Subtract(bodyToTgtLoc, aBodyLoc, aTgtLoc);
   double bodyToTgtRng = UtVec3d::Normalize(bodyToTgtLocHat, bodyToTgtLoc);

   // Trivial success if Target isn't moving.
   if (tgtSpd <= 0.0)
   {
      intTime = bodyToTgtRng / aBodySpd;
      return intTime;
   }

   if (bodyToTgtRng <= 1.0)
   {
      intTime = 0.0;
      return intTime;
   }

   // The quadratic formula tends to break down when speed of the bullet and
   // target are equal. Mathematically, this is occurring because the
   // coefficient 'a' is near 0. We treat this case separately.
   if (aBodySpd == tgtSpd)
   {
      // Trivial failure if angle between the bullet and the the targets
      // trajectory is greater than zero.
      // (bodyPos - tgtPos) dot (tgtVel)
      double scalar = UtVec3d::DotProduct(bodyToTgtLocHat, tgtVelHat);
      if (scalar < 0.0)
      {
         return intTime;
      }

      // When aBodySpd == tgtSpd, we have reflection and the shotDir will
      // mirror that of velocityT. To calculate the shotDir, we project
      // velocityT onto the normalized positionDiff vector. We multiply by
      // two for ease of reflection.
      double projected[3];
      UtVec3d::Multiply(projected, bodyToTgtLocHat, 2 * scalar);
      double shotDir[3];
      UtVec3d::Subtract(shotDir, tgtVelHat, projected);

      // In most games, the shotDir would probably be sufficient, but for
      // the method, we calculate the time and intercept point. The time is
      // figured out by determining the minimal distance (at this point, we
      // already know shotDir and the speed of the bullet).
      double shotVel[3];
      UtVec3d::Multiply(shotVel, shotDir, aBodySpd);
      double velDiff[3];
      UtVec3d::Subtract(velDiff, aTgtVel, shotVel);

      double num[3];
      UtVec3d::Multiply(num, bodyToTgtLoc, velDiff);
      double den[3];
      UtVec3d::Multiply(den, velDiff, velDiff);
      intTime = (UtVec3d::Magnitude(num) / UtVec3d::Magnitude(den)) + aPredictAheadTime;

      // double num = UtVec3d::DotProduct(bodyToTgtLoc, velDiff);
      // double den = UtVec3d::MagnitudeSquared(velDiff);
      // intTime = num / den;

      double deltaLoc[3];
      UtVec3d::Multiply(deltaLoc, aTgtVel, intTime);
      UtVec3d::Add(aIntLoc, aTgtLoc, deltaLoc);
      return intTime;
   }

   // Solve using quadratic formula
   double a = UtVec3d::MagnitudeSquared(aTgtVel) - aBodySpd * aBodySpd;
   // Quadratic will not work at 0
   if (a == 0)
   {
      return intTime;
   }

   double b        = 2 * UtVec3d::DotProduct(aTgtVel, tgtToBodyLoc);
   double c        = UtVec3d::MagnitudeSquared(tgtToBodyLoc);
   double radicand = b * b - 4 * a * c;

   // No solution possible
   if (radicand < 0)
   {
      return intTime;
   }

   double sqrtRadicand = sqrt(radicand);
   double denominator  = 2 * a;
   double time0        = (-b + sqrtRadicand) / denominator;
   double time1        = (-b - sqrtRadicand) / denominator;

   if (time0 < time1)
   {
      // not a valid direction
      if (time1 < 0.0f)
      {
         return intTime;
      }

      if (time0 >= 0.0f)
      {
         intTime = time0;
      }
      else
      {
         intTime = time1;
      }
   }
   else
   {
      // not a valid direction
      if (time0 < 0.0f)
      {
         return intTime;
      }

      if (time1 >= 0.0f)
      {
         intTime = time1;
      }
      else
      {
         intTime = time0;
      }
   }

   // Add in additional time
   intTime += aPredictAheadTime;

   double deltaLoc[3];
   UtVec3d::Multiply(deltaLoc, aTgtVel, intTime);
   UtVec3d::Add(aIntLoc, aTgtLoc, deltaLoc);
   return intTime;
}
