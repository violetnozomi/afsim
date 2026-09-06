// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtoMapProjection.hpp"

#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"

const double UtoMapProjection::msEqrFactor = 111318.163;

UtoMapProjection::UtoMapProjection()
   : projection(UtoMapProjection::Geocentric)
{
   // unused in geocentric, but needed for comparison
   longitudeOffset = 0.0;
}

void UtoMapProjection::ConvertFromLLA(const double aLLA[3], double aXYZ[3]) const
{
   ConvertFromLLA(aLLA[0], aLLA[1], aLLA[2], aXYZ);
}

void UtoMapProjection::ConvertFromLLA(const double aLat, const double aLon, const double aAlt, double aXYZ[3]) const
{
   if (projection == Geocentric)
   {
      UtEllipsoidalEarth::ConvertLLAToECEF(aLat, aLon, aAlt, aXYZ);
   }
   else if (projection == Equirectangular)
   {
      aXYZ[1] = aLat * msEqrFactor;
      aXYZ[0] = (UtMath::NormalizeAngleMinus180_180(aLon - longitudeOffset)) * msEqrFactor;
      aXYZ[2] = aAlt;
   }
}

void UtoMapProjection::ConvertFromECEF(const double aXYZin[3], double aXYZout[3]) const
{
   if (projection == Geocentric)
   {
      aXYZout[0] = aXYZin[0];
      aXYZout[1] = aXYZin[1];
      aXYZout[2] = aXYZin[2];
   }
   else if (projection == Equirectangular)
   {
      double lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(aXYZin, lla[0], lla[1], lla[2]);
      ConvertFromLLA(lla, aXYZout);
   }
}

void UtoMapProjection::ConvertFromECEF(double aXYZinout[3]) const
{
   if (projection == Geocentric)
   {
      return;
   }
   else if (projection == Equirectangular)
   {
      double lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(aXYZinout, lla[0], lla[1], lla[2]);
      ConvertFromLLA(lla, aXYZinout);
   }
}

void UtoMapProjection::ConvertToLLA(const double aXYZ[3], double aLLA[3]) const
{
   if (projection == Geocentric)
   {
      UtEllipsoidalEarth::ConvertECEFToLLA(aXYZ, aLLA[0], aLLA[1], aLLA[2]);
   }
   else if (projection == Equirectangular)
   {
      aLLA[0] = aXYZ[1] / msEqrFactor;
      aLLA[1] = UtMath::NormalizeAngleMinus180_180(aXYZ[0] / msEqrFactor + longitudeOffset);
      aLLA[2] = aXYZ[2];
   }
}

bool UtoMapProjection::operator==(const UtoMapProjection& aRHS) const
{
   return (projection == aRHS.projection) && (longitudeOffset == aRHS.longitudeOffset);
}

bool UtoMapProjection::operator!=(const UtoMapProjection& aRHS) const
{
   return (projection != aRHS.projection) || (longitudeOffset != aRHS.longitudeOffset);
}

void UtoMapProjection::Convert(UtEntity& aEntity, double aXYZ[3]) const
{
   if (projection == Geocentric)
   {
      aEntity.GetLocationWCS(aXYZ);
   }
   else if (projection == Equirectangular) // JONCSFIX - ADD THE LONGITUDE OFFSET
   {
      aEntity.GetLocationLLA(aXYZ[1], aXYZ[0], aXYZ[2]);
      aXYZ[0] = (UtMath::NormalizeAngleMinus180_180(aXYZ[0] - longitudeOffset)) * msEqrFactor;
      aXYZ[1] *= msEqrFactor;
      aXYZ[0] += m_Offset[0];
      aXYZ[1] += m_Offset[1];
      aXYZ[2] += m_Offset[2];
   }
}

void UtoMapProjection::ConvertNoOffset(UtEntity& aEntity, double aXYZ[3]) const
{
   if (projection == Geocentric)
   {
      aEntity.GetLocationWCS(aXYZ);
   }
   else if (projection == Equirectangular) // JONCSFIX - ADD THE LONGITUDE OFFSET
   {
      aEntity.GetLocationLLA(aXYZ[1], aXYZ[0], aXYZ[2]);
      aXYZ[0] = (UtMath::NormalizeAngleMinus180_180(aXYZ[0] - longitudeOffset)) * msEqrFactor;
      aXYZ[1] *= msEqrFactor;
   }
}

void UtoMapProjection::Convert(UtEntity& aEntity, double aXYZ[3], double aRPY[3]) const
{
   if (projection == Geocentric)
   {
      aEntity.GetLocationWCS(aXYZ);
      aEntity.GetOrientationWCS(aRPY[2], aRPY[1], aRPY[0]);
      aRPY[0] = aRPY[0] * UtMath::cDEG_PER_RAD;
      aRPY[1] = aRPY[1] * UtMath::cDEG_PER_RAD;
      aRPY[2] = aRPY[2] * UtMath::cDEG_PER_RAD;
   }
   else if (projection == Equirectangular)
   {
      aEntity.GetLocationLLA(aXYZ[1], aXYZ[0], aXYZ[2]);
      aXYZ[0] = (UtMath::NormalizeAngleMinus180_180(aXYZ[0] - longitudeOffset)) * msEqrFactor;
      aXYZ[1] *= msEqrFactor;
      aXYZ[0] += m_Offset[0];
      aXYZ[1] += m_Offset[1];
      aXYZ[2] += m_Offset[2];
      aEntity.GetOrientationNED(aRPY[2], aRPY[1], aRPY[0]);
      aRPY[0] = 180 + aRPY[0] * UtMath::cDEG_PER_RAD;
      aRPY[1] = aRPY[1] * UtMath::cDEG_PER_RAD;
      aRPY[2] = 90 + -aRPY[2] * UtMath::cDEG_PER_RAD;
   }
}
void UtoMapProjection::ConvertNoOffset(UtEntity& aEntity, double aXYZ[3], double aRPY[3]) const
{
   if (projection == Geocentric)
   {
      aEntity.GetLocationWCS(aXYZ);
      aEntity.GetOrientationWCS(aRPY[2], aRPY[1], aRPY[0]);
      aRPY[0] = aRPY[0] * UtMath::cDEG_PER_RAD;
      aRPY[1] *= UtMath::cDEG_PER_RAD;
      aRPY[2] = aRPY[2] * UtMath::cDEG_PER_RAD;
   }
   else if (projection == Equirectangular)
   {
      aEntity.GetLocationLLA(aXYZ[1], aXYZ[0], aXYZ[2]);
      aXYZ[0] = (UtMath::NormalizeAngleMinus180_180(aXYZ[0] - longitudeOffset)) * msEqrFactor;
      aXYZ[1] *= msEqrFactor;
      aEntity.GetOrientationNED(aRPY[2], aRPY[1], aRPY[0]);
      aRPY[0] = 180 + aRPY[0] * UtMath::cDEG_PER_RAD;
      aRPY[1] *= UtMath::cDEG_PER_RAD;
      aRPY[2] = 90 + -aRPY[2] * UtMath::cDEG_PER_RAD;
   }
}
