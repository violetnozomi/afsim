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
#include "UtCircleTangents.hpp"

#include <algorithm>
#include <utility>

//! Computes the tangents between a circle and a point
//! @param aTangentPoint1 The point on the circle and the first tangent line.
//! @param aTangentPoint2 The point on the circle and the second tangent line.
bool UtCircleTangents::CirclePointTangents(const UtVec2d& aCenter,
                                           double         aRadius,
                                           const UtVec2d& aPoint,
                                           UtVec2d&       aTangentPoint1,
                                           UtVec2d&       aTangentPoint2)
{
   // http://mathworld.wolfram.com/CircleTangentLine.html
   // Compute t = +- acos( [-a * x0 +- y0 sqrt(x0^2 + y0^2 - a^2)] / [x0^2 + y0^2] )
   UtVec2d c = aCenter - aPoint;

   if (fabs(c[0]) < fabs(c[1])) // Results are not accurate when c[0] nears 0
   {
      bool ok = CirclePointTangents(UtVec2d(aCenter[1], aCenter[0]),
                                    aRadius,
                                    UtVec2d(aPoint[1], aPoint[0]),
                                    aTangentPoint1,
                                    aTangentPoint2);
      std::swap(aTangentPoint1[0], aTangentPoint1[1]);
      std::swap(aTangentPoint2[0], aTangentPoint2[1]);
      std::swap(aTangentPoint1, aTangentPoint2);
      return ok;
   }

   double range2 = c.MagnitudeSquared();
   double root   = range2 - aRadius * aRadius;
   if (root < 0)
      return false;
   double t1  = c[1] * sqrt(root);
   double t2  = -aRadius * c[0];
   double div = range2;

   double a1Cos = (t2 + t1) / div;
   a1Cos        = std::max(std::min(1.0, a1Cos), -1.0);
   double a1    = acos(a1Cos);
   double a2    = -a1;
   double a2Cos = (t2 - t1) / div;
   a2Cos        = std::max(std::min(1.0, a2Cos), -1.0);
   double a3    = acos(a2Cos);
   double a4    = -a3;

   UtVec2d p[4] = {c + UtVec2d(cos(a1) * aRadius, sin(a1) * aRadius),
                   c + UtVec2d(cos(a2) * aRadius, sin(a2) * aRadius),
                   c + UtVec2d(cos(a3) * aRadius, sin(a3) * aRadius),
                   c + UtVec2d(cos(a4) * aRadius, sin(a4) * aRadius)};

   // pick the tangent lines are which are at two of these points.
   // Their distance to the point should be equal
   int p1Index = 1;
   int p2Index = 2;

   // t1 near 0 means there are 2 pair of points with equal distance
   if (t1 < -0.0001 || t1 > 0.0001)
   {
      double d[4];
      for (int i = 0; i < 4; ++i)
      {
         d[i] = (p[i]).MagnitudeSquared();
      };

      typedef std::pair<double, int> DataPair;
      DataPair                       dd[4];
      for (int i = 0; i < 4; ++i)
      {
         dd[i] = DataPair(fabs(d[(i % 2) + 2] - d[i / 2]), i);
      }
      DataPair best = std::min(std::min(dd[0], dd[1]), std::min(dd[2], dd[3]));
      p1Index       = best.second / 2;
      p2Index       = (best.second % 2) + 2;
   }
   else if (t2 > 0) // use this criteria when t1 is near 0
   {
      p1Index = 0;
      p2Index = 3;
   }
   aTangentPoint1 = p[p1Index];
   aTangentPoint2 = p[p2Index];

   aTangentPoint1 += aPoint;
   aTangentPoint2 += aPoint;
   return true;
}

//! Finds the tangent to a circle which passes through a point.
//! @param aDirection The desired direction around the circle moving from the point to the tangent.
bool UtCircleTangents::CirclePointTangent(const UtVec2d& aCenter,
                                          double         aRadius,
                                          const UtVec2d& aPoint,
                                          int            aDirection,
                                          UtVec2d&       aTangentPoint)
{
   UtVec2d tp1;
   UtVec2d tp2;
   bool    ok = CirclePointTangents(aCenter, aRadius, aPoint, tp1, tp2);
   if (ok)
   {
      if (aDirection == cCW)
      {
         aTangentPoint = tp2;
      }
      else
      {
         aTangentPoint = tp1;
      }
   }
   return ok;
}

//! Computes a tangent line between two circles
//! @param aCenter1 The center of the first circle
//! @param aRadius1 The radius of the first circle
//! @param aDirection1 The direction going into the tangent on the first circle
//! @param aCenter2 The center of the second circle
//! @param aRadius2 The radius of the second circle
//! @param aDirection2 The direction going into the tangent on the second circle
//! @param aPoint1 The tangent point on the first circle
//! @param aPoint2 The tangent point on the second circle
//! @return 'true' if the tangent exists
bool UtCircleTangents::CircleCircleTangent(const UtVec2d& aCenter1,
                                           double         aRadius1,
                                           int            aDirection1,
                                           const UtVec2d& aCenter2,
                                           double         aRadius2,
                                           int            aDirection2,
                                           UtVec2d&       aPoint1,
                                           UtVec2d&       aPoint2)
{
   int     directionValue = aDirection1 | (aDirection2 << 1);
   UtVec2d t1;
   UtVec2d t2;
   bool    ok = false;
   switch (directionValue)
   {
      // CW, CW
   case 0:
      ok = OuterTangents(aCenter2, aRadius2, aCenter1, aRadius1, t1, t2, aPoint2, aPoint1);
      break;
      // CCW, CW
   case 1:
      ok = InnerTangents(aCenter2, aRadius2, aCenter1, aRadius1, aPoint2, aPoint1, t1, t2);
      break;
      // CW, CCW
   case 2:
      ok = InnerTangents(aCenter2, aRadius2, aCenter1, aRadius1, t1, t2, aPoint2, aPoint1);
      break;
      // CCW, CCW
   case 3:
      ok = OuterTangents(aCenter2, aRadius2, aCenter1, aRadius1, aPoint2, aPoint1, t1, t2);
      break;
   }
   return ok;
}

//! Computes the tangents between two circles which cross the line segment passing through
//! both circle's center-points.
bool UtCircleTangents::InnerTangents(const UtVec2d& aCenter1,
                                     double         aRadius1,
                                     const UtVec2d& aCenter2,
                                     double         aRadius2,
                                     UtVec2d&       aTangentA1,
                                     UtVec2d&       aTangentA2,
                                     UtVec2d&       aTangentB1,
                                     UtVec2d&       aTangentB2)
{
   //! The 'inner' tangents can be found similar to OuterTangents(), but instead constructing a circle
   //! with radius of aRadius1 + aRadius2

   // Should succeed unless the circles intersect
   double combinedRadius = aRadius1 + aRadius2;
   if ((aCenter2 - aCenter1).MagnitudeSquared() < combinedRadius * combinedRadius)
   {
      return false;
   }

   CirclePointTangents(aCenter1, combinedRadius, aCenter2, aTangentA1, aTangentB1);
   UtVec2d d1 = (aTangentA1 - aCenter1) * (1.0 / (aRadius1 + aRadius2));
   aTangentA1 = aCenter1 + d1 * aRadius1;
   aTangentA2 = aCenter2 + d1 * -aRadius2;
   UtVec2d d2 = (aTangentB1 - aCenter1) * (1.0 / (aRadius1 + aRadius2));
   aTangentB1 = aCenter1 + d2 * aRadius1;
   aTangentB2 = aCenter2 + d2 * -aRadius2;
   return true;
}

//! Computes the tangents between two circles which do not cross cross the line segment passing through
//! both circle's center-points.
bool UtCircleTangents::OuterTangents(const UtVec2d& aCenter1,
                                     double         aRadius1,
                                     const UtVec2d& aCenter2,
                                     double         aRadius2,
                                     UtVec2d&       aTangentA1,
                                     UtVec2d&       aTangentA2,
                                     UtVec2d&       aTangentB1,
                                     UtVec2d&       aTangentB2)
{
   // The 'outer' tangents can be calculated using the point-circle tangent algorithm as described
   // here: http://mathworld.wolfram.com/Circle-UtCircleTangents.html
   double radiusDiff = aRadius1 - aRadius2;

   // Should succeed unless one circle lies inside of another
   if ((aCenter2 - aCenter1).MagnitudeSquared() < radiusDiff * radiusDiff)
   {
      return false;
   }

   UtVec2d d1;
   UtVec2d d2;
   if (radiusDiff > 0.001 || radiusDiff < -0.001)
   {
      CirclePointTangents(aCenter1, aRadius1 - aRadius2, aCenter2, aTangentB1, aTangentA1);
      d1 = (aTangentA1 - aCenter1) * (1.0 / (aRadius1 - aRadius2));
      d2 = (aTangentB1 - aCenter1) * (1.0 / (aRadius1 - aRadius2));
   }
   else
   {
      d1 = aCenter2 - aCenter1;
      d1.RotateCW();
      d1.Normalize();
      d2 = -d1;
   }
   aTangentA1 = aCenter1 + d1 * aRadius1;
   aTangentA2 = aCenter2 + d1 * aRadius2;

   aTangentB1 = aCenter1 + d2 * aRadius1;
   aTangentB2 = aCenter2 + d2 * aRadius2;
   return true;
}
