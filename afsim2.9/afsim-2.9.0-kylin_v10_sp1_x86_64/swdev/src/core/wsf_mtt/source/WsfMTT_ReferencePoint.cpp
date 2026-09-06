// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfMTT_ReferencePoint.hpp"

#include "UtMath.hpp"

WsfMTT_ReferencePoint::WsfMTT_ReferencePoint()
{
   SetReferenceLocationLLA(0.0, 0.0, 0.0);
}

//! Define the reference location
void WsfMTT_ReferencePoint::SetReferenceLocationLLA(double aLat, double aLon, double aAlt)
{
   mCenterEntity.SetLocationLLA(aLat, aLon, aAlt);

   mCenterLat    = aLat * UtMath::cRAD_PER_DEG;
   mCenterLon    = aLon * UtMath::cRAD_PER_DEG;
   mCenterAlt    = aAlt;
   mCosCenterLat = cos(mCenterLat);
   mSinCenterLat = sin(mCenterLat);
   mCosCenterLon = cos(mCenterLon);
   mSinCenterLon = sin(mCenterLon);

   // from data block 15, offset 11 (ERADUS)
   mRadius = 6371221.3;
}

//! Convert a local flat-earth NED to LLA
void WsfMTT_ReferencePoint::ConvertNEDToLLA(const double aLocNED[3], double& aLat, double& aLon, double& aAlt) const
{
   double clacos = mCosCenterLat;
   double clasin = mSinCenterLat;
   double closin = mSinCenterLon;
   double clocos = mCosCenterLon;

   // convert from Local NED to Suppressor ENU
   double x = aLocNED[1];
   double y = aLocNED[0];
   double z = -aLocNED[2];

   double term  = sqrt((mRadius * mRadius) - (x * x) - (y * y));
   double xback = -(x * closin) - (y * clasin * clocos) + (term * clacos * clocos);
   double yback = (x * clocos) - (y * clasin * closin) + (term * clacos * closin);
   double zback = (y * clacos) + (term * clasin);
   aLat         = asin(zback / mRadius);
   aLat *= UtMath::cDEG_PER_RAD;
   aLon = atan2(yback, xback);
   aLon *= UtMath::cDEG_PER_RAD;
   aAlt = z + mCenterAlt;
}

// Convert a Local flat-earth NED coordinate to WGS-84 WCS
// See Suppressor routine chgc2s.f (Orthographic mode)
void WsfMTT_ReferencePoint::ConvertNEDToWCS(const double aLocNED[3], double aLocWCS[3]) const
{
   double lat;
   double lon;
   double alt;
   ConvertNEDToLLA(aLocNED, lat, lon, alt);
   mCenterEntity.ConvertLLAToWCS(lat, lon, alt, aLocWCS);
}

// Convert a WGS-84 WCS coordinate to a local flat-earth NED
// See Suppressor routine chgs2c.f (Orthographic mode)
void WsfMTT_ReferencePoint::ConvertWCSToNED(const double aLocWCS[3], double aLocNED[3]) const
{
   double lat = 0.0;
   double lon = 0.0;
   double alt = 0.0;
   mCenterEntity.ConvertWCSToLLA(aLocWCS, lat, lon, alt);

   // convert lat/lon/alt to NED
   lat *= UtMath::cRAD_PER_DEG;
   lon *= UtMath::cRAD_PER_DEG;
   double delong = lon - mCenterLon;
   double placos = cos(lat) * mRadius;
   double plasin = sin(lat) * mRadius;
   double x      = placos * sin(delong);
   double y      = (mCosCenterLat * plasin) - (mSinCenterLat * placos * cos(delong));

   aLocNED[0] = y;
   aLocNED[1] = x;
   aLocNED[2] = -(alt - mCenterAlt);
}

void WsfMTT_ReferencePoint::ConvertWCSVectorToNED(double aVectorNED[3], const double aVectorWCS[3]) const
{
   const_cast<UtEntity&>(mCenterEntity).ConvertWCSVectorToNED(aVectorNED, aVectorWCS);
}

void WsfMTT_ReferencePoint::ConvertNEDVectorToWCS(double aVectorWCS[3], const double aVectorNED[3]) const
{
   const_cast<UtEntity&>(mCenterEntity).ConvertNEDVectorToWCS(aVectorWCS, aVectorNED);
}
