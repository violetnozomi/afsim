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

#include "EquidistantProjection.hpp"

#include <cmath>

#include "UtMath.hpp"

EquidistantProjection::EquidistantProjection(double aRadius)
   : mRadius(aRadius)
{
   // Assume 'standard' earth radius if none provided.
   if (mRadius <= 0.0)
   {
      mRadius = 360.0 * 60.0 * 1852.0 / UtMath::cTWO_PI; // 6266707.019 m
   }
   SetCenter(0.0, 0.0);
}

// virtual
EquidistantProjection::~EquidistantProjection() {}

// virtual
bool EquidistantProjection::ConvertLatLonToXY(double aLat, double aLon, double& aX, double& aY) const
{
   aX = (aLon - mCenterLon) * mMetersPerDegreeLon;
   aY = (aLat - mCenterLat) * mMetersPerDegreeLat;
   return true;
}

// virtual
bool EquidistantProjection::ConvertXYToLatLon(double aX, double aY, double& aLat, double& aLon) const
{
   aLon = mCenterLon + (aX / mMetersPerDegreeLon);
   aLat = mCenterLat + (aY / mMetersPerDegreeLat);
   return true;
}

// virtual
void EquidistantProjection::SetCenter(double aCenterLat, double aCenterLon)
{
   mCenterLat          = aCenterLat;
   mCenterLon          = aCenterLon;
   mMetersPerDegreeLat = mRadius * UtMath::cTWO_PI / 360.0;
   mMetersPerDegreeLon = mMetersPerDegreeLat * cos(mCenterLat * UtMath::cRAD_PER_DEG);
}
