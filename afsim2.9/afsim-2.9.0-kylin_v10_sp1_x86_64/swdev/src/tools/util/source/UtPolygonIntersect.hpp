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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#ifndef UTPOLYGONINTERSECT_HPP
#define UTPOLYGONINTERSECT_HPP

#include "ut_export.h"

#include <vector>

#include "UtVec2.hpp"

//! Computes the intersection between polygons and line segment lists
class UT_EXPORT UtPolygonIntersect
{
public:
   using VertexList = std::vector<UtVec2d>;
   //! Defines an intersection location
   struct Intersection
   {
      //! Index of the first points of the two intersecting line segments.
      //! Segment ID 's' refers to the edge (s, (s+1)%size)
      //! mSegments[0] is on polygon 1, mSegments[1] is on polygon2
      int     mSegments[2];
      UtVec2d mPoint;
   };
   using IntersectionList = std::vector<Intersection>;

   static bool IntersectLines(const UtVec2d& aA,
                              const UtVec2d& aB,
                              const UtVec2d& aC,
                              const UtVec2d& aD,
                              UtVec2d&       aIntersectPosition,
                              double         aEpsilon = 1.0E-6);

   static void IntersectSegments(const VertexList& aSegmentList, IntersectionList& aIntersections, double aEpsilon = 1.0E-6);

   static void IntersectPolygons(const VertexList& aPoly1, const VertexList& aPoly2, IntersectionList& aIntersections);
};

#endif
