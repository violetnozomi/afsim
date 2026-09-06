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

#ifndef WSFCONVEXHULL_HPP
#define WSFCONVEXHULL_HPP

#include "wsf_export.h"

#include <vector>

#include "UtVec2.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfTrackList.hpp"

class WSF_EXPORT WsfConvexHull
{
public:
   // uses the latitude & longitude of the tracks to compute the convex hull
   static WsfTrackList* ConvexHull(WsfTrackList& aList);
   // uses the latitude & longitude of the points to compute the convex hull
   static std::vector<WsfGeoPoint> ConvexHull(std::vector<WsfGeoPoint>& aList);
   // computes a convex hull
   static std::vector<UtVec2d> ConvexHull(std::vector<UtVec2d>& aList);
};

#endif
