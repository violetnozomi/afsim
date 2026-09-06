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
//*
//* PURPOSE:
//*   The WsfConvexHull class is used to find the convex hull of a given
//*   WsfTrackList, list of WsfGeoPoint, or list of UtVec2d.
//*
//***************************************************************************

#include "WsfConvexHull.hpp"

#include <map>
#include <vector>

#include "UtConvexHull.hpp"
#include "UtVec2.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"

WsfTrackList* WsfConvexHull::ConvexHull(WsfTrackList& aList)
{
   // convert input aList into a std::vector<UtVec2d>
   std::vector<UtVec2d>                         points;
   std::map<UtVec2d, std::unique_ptr<WsfTrack>> mapVecTrack;
   for (unsigned int i = 0; i < aList.GetTrackCount(); ++i)
   {
      UtVec2d vec;
      double  alt;
      auto    trackPtr = ut::clone(aList.GetTrackEntry(i));
      trackPtr->GetLocationLLA(vec[0], vec[1], alt);
      points.push_back(vec);
      mapVecTrack[vec] = std::move(trackPtr); // save this off for later
   }

   // now get convex hull
   points = UtConvexHull::ConvexHull(points);

   // now convert back to tracks that the hull is comprised of
   auto outputPtr = ut::make_unique<WsfTrackList>();
   for (const auto& point : points)
   {
      outputPtr->AddTrack(ut::clone(mapVecTrack[point]));
   }

   mapVecTrack.clear();
   points.clear();
   return outputPtr.release();
}

std::vector<WsfGeoPoint> WsfConvexHull::ConvexHull(std::vector<WsfGeoPoint>& aList)
{
   // convert input aList into a std::vector<UtVec2d>
   std::vector<UtVec2d>            points;
   std::map<UtVec2d, WsfGeoPoint*> mapVecPoint;
   for (unsigned int i = 0; i < aList.size(); ++i)
   {
      UtVec2d      vec;
      double       alt;
      WsfGeoPoint* geoPointPtr = &aList[i];
      geoPointPtr->GetLocationLLA(vec[0], vec[1], alt);
      points.push_back(vec);
      mapVecPoint[vec] = geoPointPtr; // just store pointers for now (clone later if needed)
   }

   // now get convex hull
   points = UtConvexHull::ConvexHull(points);

   // now convert back to geopoints that the hull is comprised of
   std::vector<WsfGeoPoint> output;
   for (unsigned int j = 0; j < points.size(); ++j)
   {
      WsfGeoPoint* geoPointPtr = mapVecPoint[points[j]];
      output.push_back(*geoPointPtr->Clone()); // clone the point here, its uses as output
   }
   mapVecPoint.clear(); // just delete pointers, not the members of the input list
   points.clear();
   return output;
}

std::vector<UtVec2d> WsfConvexHull::ConvexHull(std::vector<UtVec2d>& aList)
{
   return UtConvexHull::ConvexHull(aList);
}
