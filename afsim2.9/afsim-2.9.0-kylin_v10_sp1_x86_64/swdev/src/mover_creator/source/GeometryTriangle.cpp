// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "GeometryTriangle.hpp"

#include "UtDCM.hpp"

namespace Designer
{

GeometryTriangle::GeometryTriangle()
{
}

GeometryTriangle::~GeometryTriangle()
{
}

void GeometryTriangle::SetPoints(UtVec3dX& aPt1, UtVec3dX& aPt2, UtVec3dX& aPt3)
{
   mPt1 = aPt1;
   mPt2 = aPt2;
   mPt3 = aPt3;
}

bool GeometryTriangle::CalcCentroid()
{
   UtVec3dX vec21 = mPt2 - mPt1;
   UtVec3dX vec32 = mPt3 - mPt2;

   double length21 = vec21.Magnitude();
   double length32 = vec32.Magnitude();

   vec21.Normalize();
   vec32.Normalize();

   double midlength21 = length21 * 0.5;
   double midlength32 = length32 * 0.5;

   UtVec3dX midVec21 = vec21 * midlength21;
   UtVec3dX midVec32 = vec32 * midlength32;

   UtVec3dX mid21 = mPt1 + midVec21;
   UtVec3dX mid32 = mPt2 + midVec32;

   UtVec3dX segment1Pt1 = mid21;
   UtVec3dX segment1Pt2 = mPt3;
   UtVec3dX segment2Pt1 = mid32;
   UtVec3dX segment2Pt2 = mPt1;
   UtVec3dX intersectionPt;

   if (CalcLineSegmentIntersection(segment1Pt1, segment1Pt2, segment2Pt1, segment2Pt2, intersectionPt))
   {
      // We have an intercept, so we have a centroid
      mCentroid_ft = intersectionPt;
      return true;
   }
   else
   {
      return false;
   }
}

bool GeometryTriangle::CalcLineSegmentIntersection(UtVec3dX& aSegment1Pt1,
                                                   UtVec3dX& aSegment1Pt2,
                                                   UtVec3dX& aSegment2Pt1,
                                                   UtVec3dX& aSegment2Pt2,
                                                   UtVec3dX& aIntersectionPt)
{
   double p0x = aSegment1Pt1.X();
   double p0y = aSegment1Pt1.Y();
   double p1x = aSegment1Pt2.X();
   double p1y = aSegment1Pt2.Y();
   double p2x = aSegment2Pt1.X();
   double p2y = aSegment2Pt1.Y();
   double p3x = aSegment2Pt2.X();
   double p3y = aSegment2Pt2.Y();
   double s1x = p1x - p0x;
   double s1y = p1y - p0y;
   double s2x = p3x - p2x;
   double s2y = p3y - p2y;

   double denom = (-s2x * s1y) + (s1x * s2y);

   if (std::abs(denom) < std::numeric_limits<double>::epsilon())
   {
      return false;
   }

   double s = (-s1y * (p0x - p2x) + s1x * (p0y - p2y)) / denom;
   if ((s >= 0.0) && (s <= 1.0))
   {
      double t = (s2x * (p0y - p2y) - s2y * (p0x - p2x)) / denom;
      if ((t >= 0.0) && (t <= 1.0))
      {
         // Intercept point
         aIntersectionPt.Set(p0x + (t * s1x),
                             p0y + (t * s1y),
                             0.0);
         return true;
      }
   }

   return false;
}

} // namespace Designer
