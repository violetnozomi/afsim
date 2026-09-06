// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SceneGenGeoPathGen.hpp"

#include <chrono>
#include <iostream>

namespace sceneGen
{
std::deque<SceneGenGeoCoord> SceneGenGeoPathGen::PathMidPoint(SceneGenGeoCoord& aStartPosition,
                                                              SceneGenGeoCoord& aDestination,
                                                              int               aHeight,
                                                              float             aMinSeperationDeg)
{
   std::deque<SceneGenGeoCoord> result;

   PathMidPointHelper(aStartPosition, aDestination, aHeight, aMinSeperationDeg, result);

   return result;
}

void SceneGenGeoPathGen::PathMidPointHelper(SceneGenGeoCoord&             aStartPosition,
                                            SceneGenGeoCoord&             aDestination,
                                            int                           aHeight,
                                            const float&                  aMinSeperationDeg,
                                            std::deque<SceneGenGeoCoord>& aPath)
{
   if (aHeight < 1 || DistanceInDegress(aStartPosition, aDestination) < aMinSeperationDeg)
   {
      aPath.push_back(aStartPosition);
      aPath.push_back(aDestination);
      return;
   }

   SceneGenGeoCoord mid = MidPoint(aStartPosition, aDestination);
   PathMidPointHelper(aStartPosition, mid, aHeight - 1, aMinSeperationDeg, aPath);
   PathMidPointHelper(mid, aDestination, aHeight - 1, aMinSeperationDeg, aPath);
}

} // namespace sceneGen
