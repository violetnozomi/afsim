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

#include "UtLineSegment.hpp"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <limits>

namespace
{
template<typename T>
T clamp(T val, T low, T hi)
{
   return std::max(std::min(val, hi), low);
}
} // namespace

/** Default constructor */
UtLineSegment::UtLineSegment()
{
   UtVec3d::Set(mLoc1, 0.0);
   UtVec3d::Set(mLoc2, 1.0, 0.0, 0.0);
   mDirectionSet = false;
}

/** (Point, Point) constructor */
UtLineSegment::UtLineSegment(const double aPoint1[3], const double aPoint2[3])
{
   UtVec3d::Set(mLoc1, aPoint1);
   UtVec3d::Set(mLoc2, aPoint2);
   mDirectionSet = false;
}

/** Copy constructor */
UtLineSegment::UtLineSegment(const UtLineSegment& aSrc)
{
   UtVec3d::Set(mLoc1, aSrc.mLoc1);
   UtVec3d::Set(mLoc2, aSrc.mLoc2);
   mDirectionSet = aSrc.mDirectionSet;
}

/** Operator Overloading: */
UtLineSegment& UtLineSegment::operator=(const UtLineSegment& aRHS)
{
   UtVec3d::Set(mLoc1, aRHS.mLoc1);
   UtVec3d::Set(mLoc2, aRHS.mLoc2);
   return *this;
}

/** Saves having to do a sqrt() for Magnitude. */
double UtLineSegment::MagnitudeSquared() const
{
   double diff[3];
   UtVec3d::Subtract(diff, mLoc2, mLoc1);
   return UtVec3d::MagnitudeSquared(diff);
}

/** Provides the length of the segment. */
double UtLineSegment::Magnitude() const
{
   return std::sqrt(MagnitudeSquared());
}

/** Populates a vector of three doubles, the beginning point in the segment. */
void UtLineSegment::GetBeginPoint(double aBeginLoc[]) const
{
   UtVec3d::Set(aBeginLoc, mLoc1);
}

/** Populates a vector of three doubles, the ending point in the segment. */
void UtLineSegment::GetEndPoint(double aEndLoc[]) const
{
   UtVec3d::Set(aEndLoc, mLoc2);
}

double UtLineSegment::DistanceBetween(const double aPoint[3], const bool aLimitToEndpoints) const
{
   double p[3];
   double t;
   PointOfClosestApproach(aPoint, p, t, aLimitToEndpoints);
   const UtVec3d a(aPoint);
   const UtVec3d b(p);
   const UtVec3d d = b - a;
   return d.Magnitude();
}

bool UtLineSegment::DistanceBetween(const UtLineSegment& aOtherLine, double& aDistance, const bool aLimitToEndpoints) const
{
   aDistance = DistanceBetween(aOtherLine, aLimitToEndpoints);
   return true;
}

double UtLineSegment::DistanceBetween(const UtLineSegment& aOtherLine, const bool aLimitToEndpoints) const
{
   double s1[] = {0.0, 0.0, 0.0};
   double s2[] = {0.0, 0.0, 0.0};
   double s    = 0.0;
   double t    = 0.0;
   PointOfClosestApproach(aOtherLine, s1, s, s2, t, aLimitToEndpoints);
   UtVec3d v1(s1);
   UtVec3d v2(s2);
   UtVec3d d = v1 - v2;
   return std::sqrt(UtVec3d::DotProduct(d, d));
}

bool UtLineSegment::PointOfClosestApproach(const UtLineSegment& aOtherLine,
                                           double               aThisClosestPoint[3],
                                           double&              aThisModulus,
                                           double               aOtherClosestPoint[3],
                                           double&              aOtherModulus,
                                           const bool           aLimitToEndpoints) const
{
   // This algorithm is derived from the textbook algorithm published in:
   // "Real Time Collision Detection", by Christer Ericson,
   // (c) 2005 by Elsevier Inc.  ISBN: 1-55860-732-3
   // 5.1.9 "Closet Points of Two Line Segments", p 148-151

   const UtVec3d p1(mLoc1);
   const UtVec3d q1(mLoc2);
   const UtVec3d p2(aOtherLine.mLoc1);
   const UtVec3d q2(aOtherLine.mLoc2);

   const UtVec3d d1 = q1 - p1; // Direction vector of segment S1
   const UtVec3d d2 = q2 - p2; // Direction vector of segment S2
   const UtVec3d r  = p1 - p2;

   const double a = UtVec3d::DotProduct(d1, d1); // Squared Length of S1
   const double e = UtVec3d::DotProduct(d2, d2); // Squared Length of S2
   const double f = UtVec3d::DotProduct(d2, r);

   double s = 0.0;
   double t = 0.0;

   // Check if either or both segments degenerate into points
   if (a <= std::numeric_limits<double>::epsilon() && e <= std::numeric_limits<double>::epsilon())
   {
      // Both segments degenerate into points
      p1.Get(aThisClosestPoint);
      p2.Get(aOtherClosestPoint);
      aThisModulus = aOtherModulus = 0.0;
      return true;
   }
   if (a <= std::numeric_limits<double>::epsilon())
   {
      // First segment degenerates into a point
      t = clamp(f / e, 0.0, 1.0); // s = 0 => t = (b*s + f) / e = f / e
   }
   else
   {
      const double c = UtVec3d::DotProduct(d1, r);
      if (e <= std::numeric_limits<double>::epsilon())
      {
         // Second segment degenerates into a point
         s = -c / a; // t = 0 => s = (b*t -c / a = -c / a
         if (aLimitToEndpoints)
         {
            s = clamp(s, 0.0, 1.0);
         }
      }
      else
      {
         // The general nondegenerate case starts here
         const double b     = UtVec3d::DotProduct(d1, d2);
         const double denom = a * e - b * b; // Always nonnegative

         // If the segments are not parallel, compute closest point on L1 to L2
         // and clamp to segment S1, Else, keep arbitrary s(0.0)
         if (denom != 0.0)
         {
            s = (b * f - c * e) / denom;
            if (aLimitToEndpoints)
            {
               s = clamp(s, 0.0, 1.0);
            }
         }

         // If t in [0,1] done, else clamp t, recompute s for the new value
         // of t using s = Dot((P2 + D2*t) - P1, D1) / Dot(D1, D1)
         //              = (t*b - c) / a
         const double tnom = b * s + f;
         if (aLimitToEndpoints && tnom < 0.0)
         {
            s = clamp(-c / a, 0.0, 1.0);
         }
         else if (aLimitToEndpoints && tnom > e)
         {
            t = 1.0;
            s = clamp((b - c) / a, 0.0, 1.0);
         }
         else
         {
            t = tnom / e;
         }
      }
   }

   const UtVec3d c1 = p1 + d1 * s;
   const UtVec3d c2 = p2 + d2 * t;

   aThisModulus  = s;
   aOtherModulus = t;
   c1.Get(aThisClosestPoint);
   c2.Get(aOtherClosestPoint);

   return true;
}

bool UtLineSegment::PointOfClosestApproach(const double aOtherPoint[3],
                                           double       aClosestPoint[3],
                                           double&      aModulus,
                                           const bool   aLimitToEndpoints) const
{
   // This algorithm is derived from the textbook algorithm published in:
   // "Real Time Collision Detection", by Christer Ericson,
   // (c) 2005 by Elsevier Inc.  ISBN: 1-55860-732-3
   // 5.1.2 "Closet Point on Line Segment to Point", p 127-129

   const UtVec3d a(mLoc1);
   const UtVec3d b(mLoc2);
   const UtVec3d c(aOtherPoint);

   const UtVec3d ab = b - a;
   double        t  = UtVec3d::DotProduct(c - a, ab);
   UtVec3d       d(0.0, 0.0, 0.0);

   if (aLimitToEndpoints)
   {
      if (aLimitToEndpoints && t <= 0.0)
      {
         // c projects outside the [a,b] interval, on the a side; clamp to a
         t = 0;
         d = a;
      }
      else
      {
         double denom = UtVec3d::DotProduct(ab, ab);
         if (aLimitToEndpoints && t >= denom)
         {
            // c projects outside the [a,b] interval, on the b side; clamp to b
            t = 1.0;
            d = b;
         }
         else
         {
            t = t / denom;
            d = a + t * ab;
         }
      }
   }
   aModulus         = t;
   aClosestPoint[0] = d[0];
   aClosestPoint[1] = d[1];
   aClosestPoint[2] = d[2];

   return true;
}


bool UtLineSegment::PointOnLineSegment(const double aPoint[3], const bool aLimitToEndpoints) const
{
   // Scale epsilon to proper floating point range
   double epsilon_scale =
      (std::abs(mLoc1[0]) + std::abs(mLoc1[1]) + std::abs(mLoc1[2]) + std::abs(mLoc2[0]) + std::abs(mLoc2[1]) +
       std::abs(mLoc2[2]) + std::abs(aPoint[0]) + std::abs(aPoint[1]) + std::abs(aPoint[2]) + 1.0);

   double distBetween = DistanceBetween(aPoint, aLimitToEndpoints);

   return distBetween < (std::numeric_limits<double>::epsilon() * epsilon_scale);
}

/**
   Populates data for a 3-D point in space located at a specified modulus value
   from the beginning (modulus = 0.0) to the end (modulus = 1.0) of the segment.
   Modulus value can be specified as < 0.0 or > 1.0, which yields a point
   beyond the segment endpoints.
*/
void UtLineSegment::GetPointAtModulus(double aModulus, double aPoint[3]) const
{
   double diff[3];
   UtVec3d::Subtract(diff, mLoc2, mLoc1);
   UtVec3d::Multiply(diff, aModulus);
   UtVec3d::Add(aPoint, mLoc1, diff);
}

/**
   This method allows the segment to be propagated forward in 3-D space to a new
   location. It swaps the (previous) end point to the beginning of the segment,
   and the specified new location as the current end point.  (Propagates forward
   from the beginning point toward end point.)
*/
void UtLineSegment::StepTo(const double aNewPoint[3])
{
   UtVec3d::Set(mLoc1, mLoc2);
   UtVec3d::Set(mLoc2, aNewPoint);
}

/**
Set the end, or forward-most point on the line segment.
*/
void UtLineSegment::SetEndPoint(const double aLoc[3])
{
   UtVec3d::Set(mLoc2, aLoc);
}

/**
Set the begin, or rearward-most point on the line segment.
*/
void UtLineSegment::SetBeginPoint(const double aLoc[3])
{
   UtVec3d::Set(mLoc1, aLoc);
}

void UtLineSegment::SetDirection(const double aLocA[3], const double aLocB[3])
{
   UtVec3d::Subtract(mDirectionVector, aLocA, aLocB);
   mDirectionSet = true;
}
void UtLineSegment::SetNormal()
{
   if (!mDirectionSet)
   {
      SetDirection(mLoc2, mLoc1);
   }
   UtVec3d::Normalize(mNormal, mDirectionVector);
}

/** Populates a vector of three doubles, the ending point in the segment. */
void UtLineSegment::GetNormal(double aNormal[]) const
{
   UtVec3d::Set(aNormal, mNormal);
}

// given two lines check for an intersection between them and return the relationship between them.
// If the lines do intersect, return the intersection point as well
UtLineSegment::LINE_CLASSIFICATION UtLineSegment::Intersects(const UtLineSegment* aOtherLinePtr,
                                                             UtVec3d*             aIntersectionPointPtr) const
{
   double otherLineBeginPoint[3];
   double otherLineEndPoint[3];
   aOtherLinePtr->GetBeginPoint(otherLineBeginPoint);
   aOtherLinePtr->GetEndPoint(otherLineEndPoint);

   double Ay_minus_Cy = mLoc1[1] - otherLineEndPoint[1];
   double Dx_minus_Cx = otherLineEndPoint[0] - otherLineBeginPoint[0];
   double Ax_minus_Cx = mLoc1[0] - otherLineBeginPoint[0];
   double Dy_minus_Cy = otherLineEndPoint[1] - otherLineBeginPoint[1];
   double Bx_minus_Ax = mLoc2[0] - mLoc1[0];
   double By_minus_Ay = mLoc2[1] - mLoc1[1];

   double numerator1   = (Ay_minus_Cy * Dx_minus_Cx) - (Ax_minus_Cx * Dy_minus_Cy);
   double denominator1 = (Bx_minus_Ax * Dy_minus_Cy) - (By_minus_Ay * Dx_minus_Cx);


   double x1 = mLoc1[0];
   double y1 = mLoc1[1];
   double x2 = mLoc2[0];
   double y2 = mLoc2[1];

   double x3 = otherLineBeginPoint[0];
   double y3 = otherLineBeginPoint[1];
   double x4 = otherLineEndPoint[0];
   double y4 = otherLineEndPoint[1];

   double numerator   = ((y1 - y3) * (x4 - x3)) - ((x1 - x3) * (y4 - y3));
   double denominator = ((x2 - x1) * (y4 - y3)) - ((y2 - y1) * (x4 - x3));
   double numeratorCD = ((y1 - y3) * (x2 - x1)) - ((x1 - x3) * (y2 - y1));


   // if lines do not intersect, return now
   if (denominator1 == 0.0)
   {
      if (numerator1 == 0.0)
      {
         return COLLINEAR;
      }

      return PARALLEL;
   }

   double FactorAB = numerator / denominator;
   double FactorCD = numeratorCD / denominator;

   // if an intersection point was provided, fill it in now
   if (aIntersectionPointPtr != nullptr)
   {
      aIntersectionPointPtr->Set(0, mLoc1[0] + (FactorAB * Bx_minus_Ax));
      aIntersectionPointPtr->Set(1, mLoc1[1] + (FactorAB * By_minus_Ay));
      aIntersectionPointPtr->Set(2, 0);
   }

   if (aOtherLinePtr->PointOnLineSegment(mLoc1))
   {
      return LINES_INTERSECT;
   }
   if (aOtherLinePtr->PointOnLineSegment(mLoc2))
   {
      return SEGMENTS_INTERSECT;
   }

   // now determine the type of intersection
   if ((FactorAB >= 0.0F) && (FactorAB <= 1.0F) && (FactorCD >= 0.0F) && (FactorCD <= 1.0F))
   {
      return (SEGMENTS_INTERSECT);
   }
   else if ((FactorCD >= 0.0F) && (FactorCD <= 1.0F))
   {
      // make sure this point is actually in the direction we are heading towards..
      // create a vector from the starting point to the intersection point.
      double intersectVec[3];
      double motionVec[3];
      UtVec3d::Subtract(intersectVec, aIntersectionPointPtr->GetData(), mLoc1);
      UtVec3d::Subtract(motionVec, mLoc2, mLoc1);
      double dotProd = UtVec3d::DotProduct(intersectVec, motionVec);
      if (dotProd > 0.0)
      {
         return (A_BISECTS_B);
      }
      else
      {
         return LINES_INTERSECT;
      }
   }
   else if ((FactorAB >= 0.0F) && (FactorAB <= 1.0F))
   {
      return (B_BISECTS_A);
   }

   return LINES_INTERSECT;
}

// return true if both lines are equal
bool UtLineSegment::operator==(const UtLineSegment& rhs) const
{
   bool matchOneFound = false;
   bool matchTwoFound = false;

   double tmpBegin[3] = {0, 0, 0};
   double tmpEnd[3]   = {0, 0, 0};
   rhs.GetBeginPoint(tmpBegin);
   rhs.GetEndPoint(tmpEnd);

   if (UtVec3d::Equals(mLoc1, tmpBegin) || UtVec3d::Equals(mLoc1, tmpEnd))
   {
      matchOneFound = true;
   }

   if (UtVec3d::Equals(mLoc2, tmpBegin) || UtVec3d::Equals(mLoc2, tmpEnd))
   {
      matchTwoFound = true;
   }

   return (matchOneFound && matchTwoFound);
}

bool UtLineSegment::IsCollinear(const UtLineSegment& aOtherLine, const bool aLimitToEndpoints)
{
   if (aLimitToEndpoints)
   {
      if (MagnitudeSquared() >= aOtherLine.MagnitudeSquared())
      {
         return (PointOnLineSegment(aOtherLine.mLoc1) && PointOnLineSegment(aOtherLine.mLoc2));
      }
      return (aOtherLine.PointOnLineSegment(mLoc1) && aOtherLine.PointOnLineSegment(mLoc2));
   }
   return (PointOnLineSegment(aOtherLine.mLoc1, false) && PointOnLineSegment(aOtherLine.mLoc2, false));
}
