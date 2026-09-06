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

#include "BrawlerCoordinateConversion.hpp"

#include "UtMath.hpp"

// static
bool   BrawlerCoordinateConversion::mReferenceSet = false;
double BrawlerCoordinateConversion::mRefLat       = 0.0;
double BrawlerCoordinateConversion::mRefLon       = 0.0;

// 364812.76313253408783362183046679
#define cDegrees2Feet 364812.76313

// static
UtVec3d BrawlerCoordinateConversion::ConvertLLAtoNED(UtVec3d& lla)
{
   // just in case
   RegisterLocationLLA(lla);

   // convert
   UtVec3d ned((lla[0] - mRefLat) * cDegrees2Feet,
               (lla[1] - mRefLon) * cDegrees2Feet * cos(mRefLat * UtMath::cRAD_PER_DEG),
               -lla[2] * UtMath::cFT_PER_M);
   return ned;
}

// static
UtVec3d BrawlerCoordinateConversion::ConvertNEDtoLLA(UtVec3d& ned)
{
   if (mReferenceSet == false)
   {
      return UtVec3d();
   }

   // convert
   UtVec3d lla(mRefLat + (ned[0] / cDegrees2Feet),
               mRefLon + (ned[1] / (cDegrees2Feet * cos(mRefLat * UtMath::cRAD_PER_DEG))),
               -ned[2] / UtMath::cFT_PER_M);
   return lla;
}

// static
UtVec3d BrawlerCoordinateConversion::GetPositionNED(UtEntity& ref)
{
   UtVec3d lla;
   ref.GetLocationLLA(lla[0], lla[1], lla[2]);
   return ConvertLLAtoNED(lla);
}

// static
UtVec3d BrawlerCoordinateConversion::GetVelocityNED(UtEntity& ref)
{
   UtVec3d vNED;
   ref.GetVelocityNED(vNED.GetData());
   vNED *= UtMath::cFT_PER_M;
   return vNED;
}

// static
UtVec3d BrawlerCoordinateConversion::GetAccelerationNED(UtEntity& ref)
{
   UtVec3d ned;
   ref.GetAccelerationNED(ned.GetData());
   ned *= UtMath::cFT_PER_M;
   return ned;
}

// static
UtVec3d BrawlerCoordinateConversion::GetOrientationNED(UtEntity& ref)
{
   UtVec3d ori;
   ref.GetOrientationNED(ori[0], ori[1], ori[2]);
   return ori;
}

// static
UtVec3d BrawlerCoordinateConversion::RelativePositionNED(UtEntity& ref, UtEntity& tgt)
{
   // should use each converted NED position, and just take difference

   // UtVec3d pNED2;
   // ref.GetRelativeLocationNED(&aTgt, pNED2.GetData());
   // pNED2 *= UtMath::cFT_PER_M;
   // return pNED2;

   // double rlat, rlon, ralt;
   // ref.GetLocationLLA(rlat, rlon, ralt);
   // double lat, lon, alt;
   // tgt.GetLocationLLA(lat, lon, alt);
   // UtVec3d pNED((lat - rlat) * cDegrees2Feet,
   //              (lon - rlon) * cDegrees2Feet * cos(rlat*UtMath::cRAD_PER_DEG),
   //              -(alt-ralt) * UtMath::cFT_PER_M);
   // return pNED;

   UtVec3d nedRef = GetPositionNED(ref);
   UtVec3d nedTgt = GetPositionNED(tgt);
   nedTgt -= nedRef;
   return nedTgt;
}

// static
UtVec3d BrawlerCoordinateConversion::RelativePositionNED(UtVec3d& llaRef, UtVec3d& llaTgt)
{
   UtVec3d nedRef = ConvertLLAtoNED(llaRef);
   UtVec3d nedTgt = ConvertLLAtoNED(llaTgt);
   nedTgt -= nedRef;
   return nedTgt;
}

// static
UtVec3d BrawlerCoordinateConversion::RelativeVelocityNED(UtEntity& ref, UtEntity& tgt)
{
   UtVec3d vRef = GetVelocityNED(ref);
   UtVec3d vTgt = GetVelocityNED(tgt);
   vTgt -= vRef;
   return vTgt;
}

// static
void BrawlerCoordinateConversion::RegisterLocationLLA(UtVec3d& lla)
{
   if (mReferenceSet == false)
   {
      mRefLat       = lla[0];
      mRefLon       = lla[1];
      mReferenceSet = true;
   }
}

// static
void BrawlerCoordinateConversion::RegisterLocationEntity(UtEntity& ref)
{
   if (mReferenceSet == false)
   {
      double alt;
      ref.GetLocationLLA(mRefLat, mRefLon, alt);
      mReferenceSet = true;
   }
}
