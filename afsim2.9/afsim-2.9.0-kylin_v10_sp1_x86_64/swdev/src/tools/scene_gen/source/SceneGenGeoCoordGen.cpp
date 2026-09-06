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

#include "SceneGenGeoCoordGen.hpp"

#include "UtMath.hpp"

namespace sceneGen
{
static std::random_device               cRandonDevice;
static std::mt19937                     cRandEngine(cRandonDevice());
static std::uniform_real_distribution<> cDistribution(-1.0, 1.0);

float RandFloat(float aRange)
{
   return aRange * static_cast<float>(cDistribution(cRandEngine));
}

void SceneGenGeoCoordGen::SetCenter(float aLatitude, float aLongitude)
{
   mLatitude  = AdjustLat(aLatitude);
   mLongitude = AdjustLon(aLongitude);
}

SceneGenGeoCoord SceneGenGeoCoordGen::GetCenter() const
{
   return SceneGenGeoCoord(mLatitude, mLongitude);
}

std::vector<SceneGenGeoCoord> SceneGenGeoCoordGen::RandomCircularDistribAroundCenter(float aRangeDegrees, int aCount) const
{
   std::vector<SceneGenGeoCoord> list;
   list.reserve(aCount);

   for (int i = 0; i < aCount; ++i)
   {
      float theta = RandFloat(2.0f * static_cast<float>(UtMath::cPI));
      float lat   = mLatitude + RandFloat(aRangeDegrees) * sinf(theta);
      float lon   = mLongitude + RandFloat(aRangeDegrees) * cosf(theta);
      list.emplace_back(lat, lon);
   }

   return list;
}

std::vector<SceneGenGeoCoord> SceneGenGeoCoordGen::CircularDistribAroundCenter(float aRangeDegrees, int aCount) const
{
   std::vector<SceneGenGeoCoord> list;
   list.reserve(aCount);

   const float increment = ToRadians(360.0f) / aCount;
   float       degree    = 0.0f;

   for (int i = 0; i < aCount; ++i)
   {
      float lat = mLatitude + aRangeDegrees * sinf(degree);
      float lon = mLongitude + aRangeDegrees * cosf(degree);
      degree += increment;

      list.emplace_back(lat, lon);
   }

   return list;
}

} // namespace sceneGen
