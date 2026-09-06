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

#include "OrthoProjection.hpp"

#include <cmath>

#include "UtMath.hpp"

OrthoProjection::OrthoProjection()
{
   SetCenter(0.0, 0.0);
}

// virtual
OrthoProjection::~OrthoProjection() {}

// Convert at Lat/Lon to an X/Y.

// virtual
bool OrthoProjection::ConvertLatLonToXY(double aLat, double aLon, double& aX, double& aY) const
{
   double ptlat  = aLat * UtMath::cRAD_PER_DEG;
   double ptlon  = aLon * UtMath::cRAD_PER_DEG;
   double delong = ptlon - mCenterLon;
   double placos = cos(ptlat) * mRadius;
   double plasin = sin(ptlat) * mRadius;
   aX            = placos * sin(delong);
   aY            = (mCosCenterLat * plasin) - (mSinCenterLat * placos * cos(delong));
   return true;
}

//*****************************************************************************

// Convert an X/Y coordinate to a Lat/Lon.

// virtual
bool OrthoProjection::ConvertXYToLatLon(double aX, double aY, double& aLat, double& aLon) const
{
   double clacos = mCosCenterLat;
   double clasin = mSinCenterLat;
   double closin = mSinCenterLon;
   double clocos = mCosCenterLon;
   double term   = sqrt((mRadius * mRadius) - (aX * aX) - (aY * aY));
   double xback  = -(aX * closin) - (aY * clasin * clocos) + (term * clacos * clocos);
   double yback  = (aX * clocos) - (aY * clasin * closin) + (term * clacos * closin);
   double zback  = (aY * clacos) + (term * clasin);
   aLat          = asin(zback / mRadius);
   aLat *= UtMath::cDEG_PER_RAD;
   aLon = atan2(yback, xback);
   aLon *= UtMath::cDEG_PER_RAD;
   return true;
}

//*****************************************************************************

// Calculate the ellipsoid radius at the center of the scenario.

// virtual
void OrthoProjection::SetCenter(double aCenterLat, double aCenterLon)
{
   mCenterLat = aCenterLat * UtMath::cRAD_PER_DEG;
   mCenterLon = aCenterLon * UtMath::cRAD_PER_DEG;

   mCosCenterLat = cos(mCenterLat);
   mSinCenterLat = sin(mCenterLat);
   mCosCenterLon = cos(mCenterLon);
   mSinCenterLon = sin(mCenterLon);

   mRadius = 6371221.3;
}
