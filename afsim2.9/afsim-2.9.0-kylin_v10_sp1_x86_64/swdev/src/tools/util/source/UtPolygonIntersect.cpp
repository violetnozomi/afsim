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
#include "UtPolygonIntersect.hpp"

#include <algorithm>
#include <cassert>
#include <functional>

bool UtPolygonIntersect::IntersectLines(const UtVec2d& aA,
                                        const UtVec2d& aB,
                                        const UtVec2d& aC,
                                        const UtVec2d& aD,
                                        UtVec2d&       aPosition,
                                        double         aEpsilon /*= 1.0E-6*/)
{
   // solve Ax + dx1 * t = Cx + dx2 * s
   //       Ay + dy1 * t = Cy + dy2 * s
   double dx1 = aB[0] - aA[0];
   double dy1 = aB[1] - aA[1];
   double dx2 = aD[0] - aC[0];
   double dy2 = aD[1] - aC[1];
   //|dx1  -dx2| |t| = |Cx - Ax|
   //|dy1  -dy2| |s| = |Cy - Ay|
   double det = dx1 * -dy2 + dx2 * dy1;
   if (det == 0)
   {
      // parallel
      return false;
   }
   else
   {
      double Cx     = aC[0] - aA[0];
      double Cy     = aC[1] - aA[1];
      double detInv = 1.0 / det;
      // |t| = 1/det |-dy2  dx2| |Cx - Ax|
      // |s| =       |-dy1  dx1| |Cy - Ay|
      double t = Cx * (detInv * -dy2) + Cy * (detInv * dx2);
      double s = Cx * (detInv * -dy1) + Cy * (detInv * dx1);
      // t and s should be in range [0,1] if the segments intersect
      bool intersected = (t >= -aEpsilon && t <= 1.0 + aEpsilon && s >= -aEpsilon && s <= 1.0 + aEpsilon);
      if (intersected)
      {
         aPosition[0] = aA[0] + t * dx1;
         aPosition[1] = aA[1] + t * dy1;
      }
      return intersected;
   }
}

//! Finds all intersections between segments in aSegmentList.  Segments are defined
//! as between every two pairs points.
void UtPolygonIntersect::IntersectSegments(const VertexList& aSegmentList,
                                           IntersectionList& aIntersections,
                                           double            aEpsilon /*= 1.0E-6*/)
{
   // A simplified version of the sweep-line algorithm for intersecting segments
   // An intersection test is done between very pair of lines which overlap
   // in the y-coordinate.
   using EdgeStart = std::pair<double, int>;
   std::vector<EdgeStart> starts(aSegmentList.size() / 2 + 1);
   for (size_t i = 0; i < aSegmentList.size(); i += 2)
   {
      EdgeStart& edge = starts[i / 2];
      edge.first      = std::min(aSegmentList[i][0], aSegmentList[i + 1][0]);
      edge.second     = static_cast<int>(i);
   }
   starts[aSegmentList.size() / 2].first = 1.0E30;
   std::sort(starts.begin(), starts.end());
   std::vector<EdgeStart> stops;
   stops.emplace_back(1.0E31, -1);
   int    startIndex = 0;
   double x          = -1.0E30;
   while (x < 1.0E30)
   {
      EdgeStart start = starts[startIndex];
      EdgeStart stop  = stops[0];
      if (start.first < stop.first)
      {
         double stopPos = std::max(aSegmentList[start.second][0], aSegmentList[start.second + 1][0]);
         stops.emplace_back(stopPos, start.second);
         std::push_heap(stops.begin(), stops.end(), std::greater<EdgeStart>());
         ++startIndex;
         x = starts[startIndex].first;
      }
      else
      {
         const UtVec2d& a = aSegmentList[stop.second];
         const UtVec2d& b = aSegmentList[stop.second + 1];
         UtVec2d        iPos;
         std::pop_heap(stops.begin(), stops.end(), std::greater<EdgeStart>());
         stops.pop_back();
         for (auto& otherStop : stops)
         {
            int otherSegment = otherStop.second;
            if (otherSegment != -1 &&
                IntersectLines(a, b, aSegmentList[otherSegment], aSegmentList[otherSegment + 1], iPos, aEpsilon))
            {
               Intersection intersection;
               intersection.mSegments[0] = stop.second;
               intersection.mSegments[1] = otherSegment;
               intersection.mPoint       = iPos;
               aIntersections.push_back(intersection);
            }
         }
      }
   }
}

//! Returns the intersection between two polygons.
//! For now, the preferred method is IntersectSegments(), but this is
//! provided for backward compatibility
void UtPolygonIntersect::IntersectPolygons(const VertexList& aPoly1, const VertexList& aPoly2, IntersectionList& aIntersections)
{
   // Build segment list
   VertexList segments((aPoly1.size() + aPoly2.size()) * 2);
   int        idx = 0;
   for (size_t i = 0; i < aPoly1.size(); ++i)
   {
      segments[idx++] = aPoly1[i];
      segments[idx++] = aPoly1[(i + 1) % aPoly1.size()];
   }
   int poly1Max = idx;
   for (size_t i = 0; i < aPoly2.size(); ++i)
   {
      segments[idx++] = aPoly2[i];
      segments[idx++] = aPoly2[(i + 1) % aPoly2.size()];
   }
   IntersectionList intersections;
   IntersectSegments(segments, intersections);
   // Filter out self-intersections
   for (Intersection& isec : intersections)
   {
      if (isec.mSegments[1] < isec.mSegments[0])
      {
         std::swap(isec.mSegments[0], isec.mSegments[1]);
      }

      if (isec.mSegments[0] < poly1Max && isec.mSegments[1] >= poly1Max)
      {
         Intersection pInt;
         pInt.mSegments[0] = isec.mSegments[0] / 2;
         pInt.mSegments[1] = (isec.mSegments[1] - poly1Max) / 2;
         pInt.mPoint       = isec.mPoint;
         aIntersections.push_back(pInt);
      }
   }
}
