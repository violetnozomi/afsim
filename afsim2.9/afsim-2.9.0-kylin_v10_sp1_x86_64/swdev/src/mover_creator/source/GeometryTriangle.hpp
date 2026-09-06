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

#ifndef GEOMETRY_TRIANGLE_HPP
#define GEOMETRY_TRIANGLE_HPP

#include <string>

#include "UtMath.hpp"
#include "UtVec3dX.hpp"

// ****************************************************************************
// GeometryTriangle is a simple class to assist with triangle calculations.
// Although it uses 3D calculations, it assumes that the triangle is in the
// xy plane (z = zero).
// ****************************************************************************

namespace Designer
{
   class GeometryTriangle
   {
      public:
         GeometryTriangle();
         virtual ~GeometryTriangle();

         void SetPoints(UtVec3dX& aPt1, UtVec3dX& aPt2, UtVec3dX& aPt3);
         bool CalcCentroid();
         UtVec3dX GetCentroid_ft() { return mCentroid_ft; }

      protected:
         // Returns true if the line segments intersect. The argument aIntersectionPt
         // provides the intersection point via reference. This performs calculations
         // only using x and y (z is assumed zero).
         bool CalcLineSegmentIntersection(UtVec3dX& aSegment1Pt1,
                                          UtVec3dX& aSegment1Pt2,
                                          UtVec3dX& aSegment2Pt1,
                                          UtVec3dX& aSegment2Pt2,
                                          UtVec3dX& aIntersectionPt);
         UtVec3dX mPt1;
         UtVec3dX mPt2;
         UtVec3dX mPt3;
         UtVec3dX mCentroid_ft;
   };
}
#endif // !GEOMETRY_TRIANGLE_HPP
