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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtSphericalEarth.hpp"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLLPos.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

//! Compute the transformation to convert between the global ECEF frame and
//! a local ENU frame at a specified reference point.
//! Input/Output values are in the following units:
//!
//! Latitude : -90 (South) <= lat <= 90 (North)
//! Longitude: -180 (West) <= lon <= 180 (East)
//! Heading  : 0 <= heading <= 360
//! Distance : meters
void UtSphericalEarth::ComputeENUTransform(double aRefLat,
                                           double aRefLon,
                                           double aRefAlt,
                                           double aTransECEF[3][3],
                                           double aRefECEF[3])
{
   // Calculate the ECEF to ENU transformation matrix

   double lat    = aRefLat * UtMath::cRAD_PER_DEG;
   double lon    = aRefLon * UtMath::cRAD_PER_DEG;
   double sinLat = sin(lat);
   double cosLat = cos(lat);
   double sinLon = sin(lon);
   double cosLon = cos(lon);

   aTransECEF[0][0] = -sinLon;
   aTransECEF[0][1] = cosLon;
   aTransECEF[0][2] = 0.0;

   aTransECEF[1][0] = -sinLat * cosLon;
   aTransECEF[1][1] = -sinLat * sinLon;
   aTransECEF[1][2] = cosLat;

   aTransECEF[2][0] = cosLat * cosLon;
   aTransECEF[2][1] = cosLat * sinLon;
   aTransECEF[2][2] = sinLat;

   // Compute the position of the reference point in ECEF

   aRefECEF[0] = (aRefAlt + cEARTH_RADIUS) * cosLat * cosLon;
   aRefECEF[1] = (aRefAlt + cEARTH_RADIUS) * cosLat * sinLon;
   aRefECEF[2] = (aRefAlt + cEARTH_RADIUS) * sinLat;
}

UT_EXPORT ut::coords::WCS UtSphericalEarth::ComputeENUTransform(const ut::coords::LLA& aRef, double aTransECEF[3][3])
{
   ut::coords::WCS refECEF;
   ComputeENUTransform(aRef.mLat, aRef.mLon, aRef.mAlt, aTransECEF, refECEF.GetData());
   return refECEF;
}

//! Compute the transformation to convert between the global ECEF frame and
//! a local ENU frame at a specified reference point.
//! @note This form computes only the transformation matrix.
void UtSphericalEarth::ComputeENUTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3])
{
   // Calculate the ECEF to ENU transformation matrix

   double lat    = aRefLat * UtMath::cRAD_PER_DEG;
   double lon    = aRefLon * UtMath::cRAD_PER_DEG;
   double sinLat = sin(lat);
   double cosLat = cos(lat);
   double sinLon = sin(lon);
   double cosLon = cos(lon);

   aTransECEF[0][0] = -sinLon;
   aTransECEF[0][1] = cosLon;
   aTransECEF[0][2] = 0.0;

   aTransECEF[1][0] = -sinLat * cosLon;
   aTransECEF[1][1] = -sinLat * sinLon;
   aTransECEF[1][2] = cosLat;

   aTransECEF[2][0] = cosLat * cosLon;
   aTransECEF[2][1] = cosLat * sinLon;
   aTransECEF[2][2] = sinLat;
}

//! Compute the transformation to convert between the global ECEF frame and
//! a local NED frame at a specified reference point.
void UtSphericalEarth::ComputeNEDTransform(double aRefLat,
                                           double aRefLon,
                                           double aRefAlt,
                                           double aTransECEF[3][3],
                                           double aRefECEF[3])
{
   // Calculate the ECEF to NED transformation matrix

   double lat    = aRefLat * UtMath::cRAD_PER_DEG;
   double lon    = aRefLon * UtMath::cRAD_PER_DEG;
   double sinLat = sin(lat);
   double cosLat = cos(lat);
   double sinLon = sin(lon);
   double cosLon = cos(lon);

   aTransECEF[0][0] = -sinLat * cosLon;
   aTransECEF[0][1] = -sinLat * sinLon;
   aTransECEF[0][2] = cosLat;

   aTransECEF[1][0] = -sinLon;
   aTransECEF[1][1] = cosLon;
   aTransECEF[1][2] = 0.0;

   aTransECEF[2][0] = -cosLat * cosLon;
   aTransECEF[2][1] = -cosLat * sinLon;
   aTransECEF[2][2] = -sinLat;

   // Compute the position of the reference point in ECEF

   aRefECEF[0] = (aRefAlt + cEARTH_RADIUS) * cosLat * cosLon;
   aRefECEF[1] = (aRefAlt + cEARTH_RADIUS) * cosLat * sinLon;
   aRefECEF[2] = (aRefAlt + cEARTH_RADIUS) * sinLat;
}


ut::coords::WCS UtSphericalEarth::ComputeNEDTransform(const ut::coords::LLA& aRef, double aTransECEF[3][3])
{
   ut::coords::WCS refECEF;
   ComputeNEDTransform(aRef.mLat, aRef.mLon, aRef.mAlt, aTransECEF, refECEF.GetData());
   return refECEF;
}

//! Compute the transformation to convert between the global ECEF frame and
//! a local NED frame at a specified reference point.
//! @note This form computes only the transformation matrix.
void UtSphericalEarth::ComputeNEDTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3])
{
   // Calculate the ECEF to NED transformation matrix

   double lat    = aRefLat * UtMath::cRAD_PER_DEG;
   double lon    = aRefLon * UtMath::cRAD_PER_DEG;
   double sinLat = sin(lat);
   double cosLat = cos(lat);
   double sinLon = sin(lon);
   double cosLon = cos(lon);

   aTransECEF[0][0] = -sinLat * cosLon;
   aTransECEF[0][1] = -sinLat * sinLon;
   aTransECEF[0][2] = cosLat;

   aTransECEF[1][0] = -sinLon;
   aTransECEF[1][1] = cosLon;
   aTransECEF[1][2] = 0.0;

   aTransECEF[2][0] = -cosLat * cosLon;
   aTransECEF[2][1] = -cosLat * sinLon;
   aTransECEF[2][2] = -sinLat;
}

//! Convert LLA to a coordinate in a local ENU or NED frame.
void UtSphericalEarth::ConvertLLAToLocal(const double aRefECEF[3],
                                         const double aTransECEF[3][3],
                                         double       aLat,
                                         double       aLon,
                                         double       aAlt,
                                         double       aLocal[3])
{
   double ecef[3];

   ConvertLLAToECEF(aLat, aLon, aAlt, ecef);
   ConvertECEFToLocal(aRefECEF, aTransECEF, ecef, aLocal);
}

//! Convert a coordinate in a local ENU or NED frame to LLA
void UtSphericalEarth::ConvertLocalToLLA(const double aRefECEF[3],
                                         const double aTransECEF[3][3],
                                         const double aLocal[3],
                                         double&      aLat,
                                         double&      aLon,
                                         double&      aAlt)
{
   double ecef[3];

   ConvertLocalToECEF(aRefECEF, aTransECEF, aLocal, ecef);
   ConvertECEFToLLA(ecef, aLat, aLon, aAlt);
}

ut::coords::LLA UtSphericalEarth::ConvertLocalToLLA(const ut::coords::WCS& aRefECEF,
                                                    const double           aTransECEF[3][3],
                                                    const ut::coords::NED& aLocal)
{
   ut::coords::LLA out;
   ConvertLocalToLLA(aRefECEF.GetData(), aTransECEF, aLocal.GetData(), out.mLat, out.mLon, out.mAlt);
   return out;
}

ut::coords::LLA UtSphericalEarth::ConvertLocalToLLA(const ut::coords::WCS& aRefECEF,
                                                    const double           aTransECEF[3][3],
                                                    const ut::coords::ENU& aLocal)
{
   ut::coords::LLA out;
   ConvertLocalToLLA(aRefECEF.GetData(), aTransECEF, aLocal.GetData(), out.mLat, out.mLon, out.mAlt);
   return out;
}

ut::coords::NED UtSphericalEarth::ConvertLLAToNED(const ut::coords::WCS& aRefECEF,
                                                  const double           aTransECEF[3][3],
                                                  const ut::coords::LLA& aLLA)
{
   ut::coords::NED out;
   ConvertLLAToLocal(aRefECEF.GetData(), aTransECEF, aLLA.mLat, aLLA.mLon, aLLA.mAlt, out.GetData());
   return out;
}

ut::coords::ENU UtSphericalEarth::ConvertLLAToENU(const ut::coords::WCS& aRefECEF,
                                                  const double           aTransECEF[3][3],
                                                  const ut::coords::LLA& aLLA)
{
   ut::coords::ENU out;
   ConvertLLAToLocal(aRefECEF.GetData(), aTransECEF, aLLA.mLat, aLLA.mLon, aLLA.mAlt, out.GetData());
   return out;
}

//! Convert a POSITION in a local ENU or NED frame to the global ECEF frame.
void UtSphericalEarth::ConvertLocalToECEF(const double aRefECEF[3],
                                          const double aTransECEF[3][3],
                                          const double aLocal[3],
                                          double       aECEF[3])
{
   double tempECEF[3];

   UtMat3d::InverseTransform(tempECEF, aTransECEF, aLocal); // ECEF' = T(inv) * Local
   UtVec3d::Add(aECEF, tempECEF, aRefECEF);                 // ECEF  = ECEF' + ECEFref
}

ut::coords::WCS UtSphericalEarth::ConvertLocalToECEF(const ut::coords::WCS& aRefECEF,
                                                     const double           aTransECEF[3][3],
                                                     const ut::coords::NED& aLocal)
{
   ut::coords::WCS out;
   ConvertLocalToECEF(aTransECEF, aLocal.GetData(), out.GetData());
   return out;
}

ut::coords::WCS UtSphericalEarth::ConvertLocalToECEF(const ut::coords::WCS& aRefECEF,
                                                     const double           aTransECEF[3][3],
                                                     const ut::coords::ENU& aLocal)
{
   ut::coords::WCS out;
   ConvertLocalToECEF(aTransECEF, aLocal.GetData(), out.GetData());
   return out;
}

//! Convert a POSITION in a local ENU or NED frame to the global ECEF frame.
ut::coords::NED UtSphericalEarth::ConvertECEFToNED(const ut::coords::WCS& aRefECEF,
                                                   const double           aTransECEF[3][3],
                                                   const ut::coords::WCS& aECEF)
{
   ut::coords::NED out;
   ConvertECEFToLocal(aRefECEF.GetData(), aTransECEF, aECEF.GetData(), out.GetData());
   return out;
}

ut::coords::ENU UtSphericalEarth::ConvertECEFToENU(const ut::coords::WCS& aRefECEF,
                                                   const double           aTransECEF[3][3],
                                                   const ut::coords::WCS& aECEF)
{
   ut::coords::ENU out;
   ConvertECEFToLocal(aRefECEF.GetData(), aTransECEF, aECEF.GetData(), out.GetData());
   return out;
}

ut::coords::ENU UtSphericalEarth::ConvertECEFToENU(const double aTransECEF[3][3], const ut::coords::WCS& aECEF)
{
   ut::coords::ENU out;
   ConvertECEFToLocal(aTransECEF, aECEF.GetData(), out.GetData());
   return out;
}

//! Convert a POSITION in the global ECEF frame to a local ENU or NED frame.
void UtSphericalEarth::ConvertECEFToLocal(const double aRefECEF[3],
                                          const double aTransECEF[3][3],
                                          const double aECEF[3],
                                          double       aLocal[3])
{
   double tempECEF[3];

   UtVec3d::Subtract(tempECEF, aECEF, aRefECEF);     // ECEF' = ECEF - aRefECEF
   UtMat3d::Transform(aLocal, aTransECEF, tempECEF); // Local = T x ECEF'
}

//! Convert a VECTOR in a local ENU or NED frame to the global ECEF frame.
//! This is just like the 4-operand version with a reference of (0,0,0).
void UtSphericalEarth::ConvertLocalToECEF(const double aTransECEF[3][3], const double aLocal[3], double aECEF[3])
{
   UtMat3d::InverseTransform(aECEF, aTransECEF, aLocal); // ECEF = T(inv) * Local
}

//! Convert a VECTOR in a local ENU or NED frame to the global ECEF frame.
//! This is just like the 3-operand version with a reference of (0,0,0).
ut::coords::WCS UtSphericalEarth::ConvertLocalToECEF(const double aTransECEF[3][3], const ut::coords::NED& aLocal)
{
   ut::coords::WCS out;
   ConvertLocalToECEF(aTransECEF, aLocal.GetData(), out.GetData());
   return out;
}

//! Convert a VECTOR in the global ECEF frame to a local ENU or NED frame.
//! This is just like the 4-operand version with a reference of (0,0,0).
void UtSphericalEarth::ConvertECEFToLocal(const double aTransECEF[3][3], const double aECEF[3], double aLocal[3])
{
   UtMat3d::Transform(aLocal, aTransECEF, aECEF); // Local = T x ECEF
}

//! Convert a VECTOR in the global ECEF frame to a local ENU or NED frame.
//! This is just like the 4-operand version with a reference of (0,0,0).
ut::coords::NED UtSphericalEarth::ConvertECEFToNED(const double aTransECEF[3][3], const ut::coords::WCS& aECEF)
{
   ut::coords::NED out;
   ConvertECEFToLocal(aTransECEF, aECEF.GetData(), out.GetData());
   return out;
}

//! Given a latitude, longitude and altitude, return the ECEF XYZ coordinates.
void UtSphericalEarth::ConvertLLAToECEF(double aLat, double aLon, double aAlt, double aECEF[])
{
   double lat    = aLat * UtMath::cRAD_PER_DEG;
   double lon    = aLon * UtMath::cRAD_PER_DEG;
   double sinLat = sin(lat);
   double cosLat = cos(lat);
   double sinLon = sin(lon);
   double cosLon = cos(lon);

   // Compute the position of the given point in ECEF coordinates

   double altRadius = aAlt + cEARTH_RADIUS;
   aECEF[0]         = altRadius * cosLat * cosLon;
   aECEF[1]         = altRadius * cosLat * sinLon;
   aECEF[2]         = altRadius * sinLat;
}

ut::coords::WCS UtSphericalEarth::ConvertLLAToECEF(const ut::coords::LLA& aLLA)
{
   ut::coords::WCS out;
   ConvertLLAToECEF(aLLA.mLat, aLLA.mLon, aLLA.mAlt, out.GetData());
   return out;
}

//! Given ECEF XYZ coordinates, return the latitude, longitude and altitude.
void UtSphericalEarth::ConvertECEFToLLA(const double aECEF[], double& aLat, double& aLon, double& aAlt)
{
   double latrad = atan2(aECEF[2], (sqrt((aECEF[0] * aECEF[0]) + (aECEF[1] * aECEF[1]))));
   double lonrad = atan2(aECEF[1], aECEF[0]);

   aLat         = latrad * UtMath::cDEG_PER_RAD;
   aLon         = lonrad * UtMath::cDEG_PER_RAD;
   double range = sqrt((aECEF[0] * aECEF[0]) + (aECEF[1] * aECEF[1]) + (aECEF[2] * aECEF[2]));
   aAlt         = range - cEARTH_RADIUS;
}

ut::coords::LLA UtSphericalEarth::ConvertECEFToLLA(const ut::coords::WCS& aECEF)
{
   ut::coords::LLA out;
   ConvertECEFToLLA(aECEF.GetData(), out.mLat, out.mLon, out.mAlt);
   return out;
}

//! Given latitudes and longitudes of two points, compute the initial
//! heading and distance between them.
//! @note The caller is responsible for ensuring the input values are within range.
//!       Latitudes should be [-90, 90], Longitudes [-180, 180] or [0, 360].
void UtSphericalEarth::GreatCircleHeadingAndDistance(double  aBegLat,
                                                     double  aBegLon,
                                                     double  aEndLat,
                                                     double  aEndLon,
                                                     double& aHeading,
                                                     double& aDistance)
{
   if ((fabs(aBegLat) >= 90.0) || (fabs(aEndLat) >= 90.0))
   {
      // Path originates or terminates at one of the poles.
      aDistance = fabs(aEndLat - aBegLat);
      aHeading  = 0.0; // Assume northerly heading
      if (aBegLat >= 90.0)
      {
         aHeading = 180.0;
      }
   }
   else if (aBegLon == aEndLon)
   {
      // Longitudes are the same so the path goes north/south without passing
      // over a pole.

      aDistance = fabs(aEndLat - aBegLat);
      aHeading  = 0.0; // Assume northerly heading
      if (aEndLat < aBegLat)
      {
         aHeading = 180.0;
      }
   }
   else if (fabs(aEndLon - aBegLon) == 180.0)
   {
      // Longitudes are 180 degrees apart so path goes over the pole.

      // Compute the change in latitude going over the north and south pole.
      // We'll take the shortest path.

      double northAngle = (90.0 - aEndLat) + (90.0 - aBegLat);
      double southAngle = (90.0 + aEndLat) + (90.0 + aBegLat);
      if (northAngle <= southAngle)
      {
         // Go over the north pole
         aDistance = northAngle;
         aHeading  = 0.0;
      }
      else
      {
         // Go over the south pole
         aDistance = southAngle;
         aHeading  = 180.0;
      }
   }
   else
   {
      // Use the law of cosines for spherical triangles to get the distance
      // between the start and end points.  Let:
      //
      // a: The length of the side of the triangle formed by the meridian
      //    from the pole to the starting point, measured in radians (known).
      // b: The length of the side of the triangle formed by the great circle
      //    from the starting point to the ending point, measured in radians
      //    (unknown).  This is the distance - which is what we need.
      // c: The length of the side of the triangle formed by the meridian
      //    from the pole to the ending point, measured in radians (known).
      // B: The angle opposite side b (formed by sides a and c).
      //    This is just the difference in longitude (known).

      double a     = (90.0 - aBegLat) * UtMath::cRAD_PER_DEG;
      double cos_a = cos(a);
      double sin_a = sin(a);
      double c     = (90.0 - aEndLat) * UtMath::cRAD_PER_DEG;
      double cos_c = cos(c);
      double sin_c = sin(c);
      // TODO - Do we need to worry about the dateline here?
      double B     = (aEndLon - aBegLon) * UtMath::cRAD_PER_DEG;
      double cos_B = cos(B);
      double cos_b = cos_c * cos_a + sin_c * sin_a * cos_B;
      double b     = acos(std::min(std::max(cos_b, -1.0), 1.0));
      double sin_b = sin(b);

      aDistance = b * UtMath::cDEG_PER_RAD;
      if (aDistance != 0.0)
      {
         // Use the law of cosines again to get the heading (angle C, opposite side c)
         double cos_C = (cos_c - cos_a * cos_b) / (sin_a * sin_b);
         double C     = acos(std::min(std::max(cos_C, -1.0), 1.0));
         aHeading     = C * UtMath::cDEG_PER_RAD;
      }
      else
      {
         // Distance can be 0 due to loss of precision, this prevents divide by 0.
         aHeading = 0.0;
      }

      // Make sure the heading is in the proper quadrant...

      if (sin((aEndLon - aBegLon) * UtMath::cRAD_PER_DEG) < 0.0)
      {
         aHeading = 360.0 - aHeading;
      }
   }
   // Convert distance from degrees to nautical miles to meters
   aDistance *= 60.0 * UtMath::cM_PER_NM;
}

double UtSphericalEarth::GreatCircleHeadingAndDistance(ut::coords::LL aBeg, ut::coords::LL aEnd, double& aHeading)
{
   double out;
   GreatCircleHeadingAndDistance(aBeg.mLat, aBeg.mLon, aEnd.mLat, aEnd.mLon, aHeading, out);
   return out;
}


//! Given a starting latitude and longitude, an initial heading and a
//! distance, compute the final latitude and longitude.
void UtSphericalEarth::ExtrapolateGreatCirclePosition(double  aBegLat,
                                                      double  aBegLon,
                                                      double  aHeading,
                                                      double  aDistance,
                                                      double& aEndLat,
                                                      double& aEndLon)
{
   double heading  = UtMath::NormalizeAngle0_360(aHeading);
   double distance = aDistance / UtMath::cM_PER_NM / 60.0; // Distance in degrees

   // Perform the trivial cases...

   if (aDistance == 0.0)
   {
      aEndLat = aBegLat;
      aEndLon = aBegLon;
   }
   else if (heading == 0.0)
   {
      aEndLat = aBegLat + distance;
      aEndLon = aBegLon;
      if (aEndLat > 90.0)
      {
         aEndLat = 180.0 - aEndLat;
         aEndLon = UtMath::NormalizeAngleMinus180_180(aEndLon + 180.0);
      }
   }
   else if (heading == 180.0)
   {
      aEndLat = aBegLat - distance;
      aEndLon = aBegLon;
      if (aEndLat < -90.0)
      {
         aEndLat = -180.0 - aEndLat;
         aEndLon = UtMath::NormalizeAngleMinus180_180(aEndLon + 180.0);
      }
   }
   else
   {
      // The following fix (or at least the start of it) was put in by Carol Brickman to
      // correct a problem at the poles (divide by zero). This needs to be evaluated more.
      if (aBegLat >= 90.0)
      {
         aBegLat = 89.99;
      }
      else if (aBegLat <= -90.0)
      {
         aBegLat = -89.99;
      }

      // Use the law of cosines for spherical triangles to get the latitude
      // of the destination. Let:
      //
      // a: The length of the side of the triangle formed by the meridian
      //    from the pole to the starting point, measured in radians (known).
      // b: The length of the side of the triangle formed by the great circle
      //    from the starting point to the ending point, measured in radians
      //    (known).
      // c: The length of the side of the triangle formed by the meridian
      //    from the pole to the ending point, measured in radians (unknown).
      // C: The angle opposite side c (formed by sides a and b).
      //    This is just the heading (known).

      // Distance is in degrees... convert to radians to get the length of side b.

      double b     = distance * UtMath::cRAD_PER_DEG;
      double cos_b = cos(b);
      double sin_b = sin(b);
      double a     = (90.0 - aBegLat) * UtMath::cRAD_PER_DEG;
      double cos_a = cos(a);
      double sin_a = sin(a);
      double C     = heading * UtMath::cRAD_PER_DEG;
      double cos_C = cos(C);

      // Use the law of cosines for spherical triangles to get the latitude.

      double cos_c = cos_a * cos_b + sin_a * sin_b * cos_C;
      double c     = acos(std::min(std::max(cos_c, -1.0), 1.0));
      double sin_c = sin(c);
      aEndLat      = 90.0 - (c * UtMath::cDEG_PER_RAD);

      // Use law of cosines to get the longitude delta.  The law of sines
      // could be used, but I had difficulty getting it into the proper
      // quadrant.

      double cos_B = (cos_b - cos_c * cos_a) / (sin_c * sin_a);
      double B     = acos(std::min(std::max(cos_B, -1.0), 1.0));
      if (C > UtMath::cPI) // Get the proper sign based on the heading
      {
         B = -B;
      }
      aEndLon = UtMath::NormalizeAngleMinus180_180(aBegLon + B * UtMath::cDEG_PER_RAD);
   }
}

ut::coords::LL UtSphericalEarth::ExtrapolateGreatCirclePosition(ut::coords::LL aBeg, double aHeading, double aDistance)
{
   ut::coords::LL out;
   ExtrapolateGreatCirclePosition(aBeg.mLat, aBeg.mLon, aHeading, aDistance, out.mLat, out.mLon);
   return out;
}

//! Does the horizon mask (obscure) the view between to objects?  The
//! return value is 'true' if the view is obscured and 'false' if not.
//!
//! Altitudes are in meters above MSL and should be greater than 0.  If
//! either value is less than 0 then 'false' will be returned since one
//! of the objects is 'under ground'.
//!
//! aRadiusScale is a factor which can be used to model diffraction
//! effects.  The earth radius is multiplied by this value to get an
//! 'effective' earth radius.  An example value for radar would be 4/3.
//!
//! This method assumes a spherical bald earth and does not consider
//! terrain.  it should be used as a filter before performing expensive
//! an terrain line-of-sight determination.
//!
//! The equations are not limited by altitude or distance between the objects.
bool UtSphericalEarth::MaskedByHorizon(double aLat1,
                                       double aLon1,
                                       double aAlt1,
                                       double aLat2,
                                       double aLon2,
                                       double aAlt2,
                                       double aRadiusScale)
{
   // If either object is 'below ground' then the view is obscured.
   // A little fuzz is used here to allow for numerical conversion issues.

   if ((aAlt1 < -1.0) || (aAlt2 < -1.0))
   {
      return true;
   }
   double alt1 = std::max(aAlt1, 0.0);
   double alt2 = std::max(aAlt2, 0.0);

   bool maskedByHorizon = false;

   // Compute the slant range between the objects.  Note that this is a very
   // close approximation.  We get the Cartesian coordinates in the unscaled
   // coordinate system and compute the slant range from that.  We can't used
   // the scaled system because that would introduce distortion.

   double xyz1[3];
   double xyz2[3];

   ConvertLLAToECEF(aLat1, aLon1, alt1, xyz1);
   ConvertLLAToECEF(aLat2, aLon2, alt2, xyz2);
   double xyzDelta[3];
   UtVec3d::Subtract(xyzDelta, xyz2, xyz1);
   double range12Squared = UtVec3d::MagnitudeSquared(xyzDelta);

   // Determine the maximum possible slant range to the horizon.  Note that we
   // use the maximum of the two altitudes to determine this range.  (If A can
   // see B then B can see A).

   // Note: To compute ranges we note that the slant range to the horizon for
   //       altitude 'h' and an (effective) earth radius 'R' is:
   //
   //       D = sqrt((R + h)^2 - R^2)
   //         = sqrt(R^2 + 2Rh + h^2 - R^2)
   //         = sqrt(h^2 + 2Rh)

   double maxAlt              = std::max(alt1, alt2);
   double radius              = UtSphericalEarth::cEARTH_RADIUS * aRadiusScale;
   double horizonRangeSquared = (maxAlt * maxAlt) + (2.0 * radius * maxAlt);

   // If the slant range between the points is less than or equal to the slant
   // range to the horizon then the horizon cannot obscure the view.  Note that
   // we could have eliminated this test and just done what is in the 'if', but
   // this first test eliminates 3 sqrt calls for many cases.

   if (range12Squared > horizonRangeSquared)
   {
      // Recalculate the slant range based on the scaled earth radius.
      // This is generally slightly less than the non-scaled range.
      // We hold the ground range constant.

      if (aRadiusScale != 1.0)
      {
         double lenSideA         = alt1 + cEARTH_RADIUS;
         double lenSideC         = alt2 + cEARTH_RADIUS;
         double lenSideB_Squared = range12Squared;

         // Use law of cosines to compute angle B (the angle between the two position vectors)

         double cosAngleB =
            ((lenSideC * lenSideC) + (lenSideA * lenSideA) - lenSideB_Squared) / (2.0 * lenSideC * lenSideA);
         cosAngleB     = std::max(-1.0, std::min(cosAngleB, 1.0));
         double angleB = acos(cosAngleB);

         double scaledEarthRadius = aRadiusScale * UtSphericalEarth::cEARTH_RADIUS;
         double lenSideAP         = alt1 + scaledEarthRadius;
         double lenSideCP         = alt2 + scaledEarthRadius;
         double angleBP           = angleB / aRadiusScale;

         // Use the law of cosines to solve for side B (the slant range)

         double lenSideBP_Squared =
            (lenSideCP * lenSideCP) + (lenSideAP * lenSideAP) - (2.0 * lenSideCP * lenSideAP * cos(angleBP));
         range12Squared = lenSideBP_Squared;
      }

      // The slant range between the points is greater than the slant range to
      // the horizon.  Determine if it is above or below the horizon.

      // Calculate the slant range from each point to their respective horizon.
      double horizon1Range = sqrt((alt1 * alt1) + (2.0 * radius * alt1));
      double horizon2Range = sqrt((alt2 * alt2) + (2.0 * radius * alt2));
      double range12       = sqrt(range12Squared);
      if (range12 > (horizon1Range + horizon2Range))
      {
         maskedByHorizon = true;
      }
   }
   return maskedByHorizon;
}

bool UtSphericalEarth::MaskedByHorizon(const ut::coords::LLA& aLLA1, const ut::coords::LLA& aLLA2, double aRadiusScale)
{
   return MaskedByHorizon(aLLA1.mLat, aLLA1.mLon, aLLA1.mAlt, aLLA2.mLat, aLLA2.mLon, aLLA2.mAlt, aRadiusScale);
}

//! Returns the point of intersection of two great circle segments
//! defined by the segments (aSeg1End1) to (aSeg1End2) and
//! (aSeg2End1) to (aSeg2End2).
//! return: up to 2 valid lat/lon points of intersection that lie on both
//!         segments. if the intersection point is not valid, the lat/lon values
//!         are (DBL_MAX, DBL_MAX)
void UtSphericalEarth::GreatCircleIntersection(UtLLPos&       aLLIntersect1,
                                               UtLLPos&       aLLIntersect2,
                                               const UtLLPos& aSeg1End1,
                                               const UtLLPos& aSeg1End2,
                                               const UtLLPos& aSeg2End1,
                                               const UtLLPos& aSeg2End2)
{
   // Convert from Lat/Lon to XYZ
   double seg1End1ECEF[3];
   double seg1End2ECEF[3];
   double seg2End1ECEF[3];
   double seg2End2ECEF[3];
   ConvertLLAToECEF(aSeg1End1.GetLat(), aSeg1End1.GetLon(), 0.0, seg1End1ECEF);

   ConvertLLAToECEF(aSeg1End2.GetLat(), aSeg1End2.GetLon(), 0.0, seg1End2ECEF);

   ConvertLLAToECEF(aSeg2End1.GetLat(), aSeg2End1.GetLon(), 0.0, seg2End1ECEF);

   ConvertLLAToECEF(aSeg2End2.GetLat(), aSeg2End2.GetLon(), 0.0, seg2End2ECEF);

   double cross1[3];
   UtVec3d::CrossProduct(cross1, seg1End1ECEF, seg1End2ECEF);
   UtVec3d::Normalize(cross1);
   double cross2[3];
   UtVec3d::CrossProduct(cross2, seg2End1ECEF, seg2End2ECEF);
   UtVec3d::Normalize(cross2);

   double i1[3];
   UtVec3d::CrossProduct(i1, cross1, cross2);
   UtVec3d::Normalize(i1);
   double i1lat, i1lon, i1alt;
   ConvertECEFToLLA(i1, i1lat, i1lon, i1alt);

   // antipode
   double i2[3];
   UtVec3d::Multiply(i2, i1, -1.0);
   double i2lat, i2lon, i2alt;
   ConvertECEFToLLA(i2, i2lat, i2lon, i2alt);

   // check if the point of intersection lies on both segments
   // length of seg1
   double d1;
   double heading; // not used
   GreatCircleHeadingAndDistance(aSeg1End1.GetLat(), aSeg1End1.GetLon(), aSeg1End2.GetLat(), aSeg1End2.GetLon(), heading, d1);

   // length of seg2
   double d2;
   GreatCircleHeadingAndDistance(aSeg2End1.GetLat(), aSeg2End1.GetLon(), aSeg2End2.GetLat(), aSeg2End2.GetLon(), heading, d2);

   // between seg1 endpoints and first point of intersection
   double d111;
   GreatCircleHeadingAndDistance(aSeg1End1.GetLat(), aSeg1End1.GetLon(), i1lat, i1lon, heading, d111);
   double d121;
   GreatCircleHeadingAndDistance(aSeg1End2.GetLat(), aSeg1End2.GetLon(), i1lat, i1lon, heading, d121);

   // between seg1 endpoints and second point of intersection
   double d112;
   GreatCircleHeadingAndDistance(aSeg1End1.GetLat(), aSeg1End1.GetLon(), i2lat, i2lon, heading, d112);
   double d122;
   GreatCircleHeadingAndDistance(aSeg1End2.GetLat(), aSeg1End2.GetLon(), i2lat, i2lon, heading, d122);

   // between seg2 endpoints and first point of intersection
   double d211;
   GreatCircleHeadingAndDistance(aSeg2End1.GetLat(), aSeg2End1.GetLon(), i1lat, i1lon, heading, d211);
   double d221;
   GreatCircleHeadingAndDistance(aSeg2End2.GetLat(), aSeg2End2.GetLon(), i1lat, i1lon, heading, d221);

   // between seg2 endpoints and second point of intersection
   double d212;
   GreatCircleHeadingAndDistance(aSeg2End1.GetLat(), aSeg2End1.GetLon(), i2lat, i2lon, heading, d212);
   double d222;
   GreatCircleHeadingAndDistance(aSeg2End2.GetLat(), aSeg2End2.GetLon(), i2lat, i2lon, heading, d222);

   aLLIntersect1.Set(DBL_MAX, DBL_MAX);
   aLLIntersect2.Set(DBL_MAX, DBL_MAX);

   // check if first point of intersection lies on both segments
   if (d1 >= d111 && d1 >= d121 && d2 >= d211 && d2 >= d221)
   {
      aLLIntersect1.Set(i1lat, i1lon);
   }
   // check if second point of intersection lies on both segments
   if (d1 >= d112 && d1 >= d122 && d2 >= d212 && d2 >= d222)
   {
      aLLIntersect2.Set(i2lat, i2lon);
   }
}


//! Computes the point closest point on a great-circle to another point
void UtSphericalEarth::GreatCircleClosestPoint(double  aLat1,
                                               double  aLon1,
                                               double  aHeading,
                                               double  aLat2,
                                               double  aLon2,
                                               double& aClosestPointLat,
                                               double& aClosestPointLon)
{
   // Compute a point at an arbitrary great-circle distance
   double endPtLat, endPtLon;
   ExtrapolateGreatCirclePosition(aLat1, aLon1, aHeading, 50000.0, endPtLat, endPtLon);

   // Compute the normal of the great circle
   UtVec3d pt1ECEF, pt2ECEF;
   ConvertLLAToECEF(aLat1, aLon1, 0.0, pt1ECEF.GetData());
   ConvertLLAToECEF(endPtLat, endPtLon, 0.0, pt2ECEF.GetData());
   UtVec3d normal;
   normal.CrossProduct(pt1ECEF, pt2ECEF - pt1ECEF);
   normal.Normalize();

   // Project vector to the other point onto the great circle plane.
   UtVec3d otherPt;
   ConvertLLAToECEF(aLat2, aLon2, 0.0, otherPt.GetData());
   UtVec3d projectedPt = otherPt - normal * otherPt.DotProduct(normal);

   // Set on earth surface
   double projectPointMagnitude = projectedPt.Magnitude();
   if (projectPointMagnitude < 1.0)
   {
      // Divide by zero indicates any point will do
      aClosestPointLat = aLat1;
      aClosestPointLon = aLon1;
   }
   else
   {
      projectedPt *= cEARTH_RADIUS / projectPointMagnitude;
      double nearZero;
      ConvertECEFToLLA(projectedPt.GetData(), aClosestPointLat, aClosestPointLon, nearZero);
   }
}

//! Convert celestial right ascension and declination to a WCS vector, given the current time.
//! @param aRA The right ascension to be converted.
//! @param aDec The declination to be converted.
//! @param aTime The time reference, which is used to convert to WCS / ECEF.
//! @param aVecWCS The WCS unit vector, pointing in the direction of the celestial point.
//! @note The WCS vector always points in the same direction, regardless of the coordinate reference,
//!   because the point is assumed to be at "infinity."
void UtSphericalEarth::ConvertRA_DecToECEF(double aRA, double aDec, const UtCalendar& aTime, double aVecWCS[3])
{
   // get R.A., dec of mean sun.
   double hourAngle = aTime.GetEarthAngleApprox() - aRA;
   double sinDel    = sin(aDec);
   double cosDel    = cos(aDec);
   double sinH      = sin(hourAngle);
   double cosH      = cos(hourAngle);
   double sinEl     = sinDel;
   double cosEl     = cosDel;
   double cosAz     = cosH * cosDel / cosEl;
   double sinAz     = sinH * cosDel / cosEl;
   double az        = atan2(sinAz, cosAz);
   sinAz            = sin(az);
   cosAz            = cos(az);

   aVecWCS[0] = cosAz * cosEl;
   aVecWCS[1] = -sinAz * cosEl; // This is -sinAz to account for az progressing in the opposite direction
                                // in this coordinate system.
   aVecWCS[2] = sinEl;
}

//! Convert celestial right ascension and declination to an ECI vector.
//! @param aRA The right ascension to be converted.
//! @param aDec The declination to be converted.
//! @param aVecECI The ECI unit vector, pointing in the direction of the celestial point.
//! @note The ECI vector always points in the same direction, regardless of the coordinate reference,
//!   because the point is assumed to be at "infinity."
void UtSphericalEarth::ConvertRA_DecToECI(double aRA, double aDec, double aVecECI[3])
{
   double sinEl = sin(aDec);
   double cosEl = cos(aDec);
   double az    = -aRA;
   double sinAz = sin(az);
   double cosAz = cos(az);

   aVecECI[0] = cosAz * cosEl;
   aVecECI[1] = -sinAz * cosEl; // This is -sinAz to account for az progressing in the opposite direction
                                // in this coordinate system.
   aVecECI[2] = sinEl;
}

//! Convert a vector in Earth-Centered Inertial coordinates (ECI) to right-ascension
//! and declination.
//! @param aLocationECI The ECI location to convert.
//! @param aRA The converted right ascension.
//! @param aDec The converted declination.
void UtSphericalEarth::ConvertECIToRA_Dec(const double aLocationECI[3], double& aRA, double& aDec)
{
   aRA = atan2(aLocationECI[1], aLocationECI[0]);
   if (aRA < 0.0)
   {
      aRA += UtMath::cTWO_PI;
   }
   aDec = asin(aLocationECI[2] / UtVec3d::Magnitude(aLocationECI));
}

//! Convert celestial right ascension and declination to a locally-referenced azimuth and elevation.
//! @param aRA The right ascension to be converted.
//! @param aDec The declination to be converted.
//! @param aLat The latitude of the observer (degrees).
//! @param aLon The longitude of the observer (degrees).
//! @param aTime The time reference, which is used to convert to WCS / ECEF.
//! @param aAz The local azimuth of the celestial coordinate.
//! @param aEl The local elevation of the celestial coordinate.
void UtSphericalEarth::ConvertRA_DecToAzEl(double            aRA,
                                           double            aDec,
                                           double            aLat,
                                           double            aLon,
                                           const UtCalendar& aTime,
                                           double&           aAz,
                                           double&           aEl)
{
   // Compute the hour angle.
   double hourAngle = aTime.GetEarthAngleApprox() - aRA + aLon * UtMath::cRAD_PER_DEG;

   // solve for altitude (el)
   double sinDec       = sin(aDec);
   double cosDec       = cos(aDec);
   double lat          = aLat * UtMath::cRAD_PER_DEG;
   double sinLat       = sin(lat);
   double cosLat       = cos(lat);
   double sinHourAngle = sin(hourAngle);
   double cosHourAngle = cos(hourAngle);
   aEl                 = asin(cosHourAngle * cosDec * cosLat + sinDec * sinLat);
   double cosEl        = cos(aEl);

   // solve for azimuth
   double sinAz = sinHourAngle * cosDec / cosEl;
   double cosAz = (cosHourAngle * cosDec * sinLat - sinDec * cosLat) / cosEl;
   aAz          = atan2(sinAz, cosAz) + UtMath::cPI;
}

//! Convert celestial right ascension and declination to a locally-referenced azimuth and elevation.
//! @param aRA The right ascension to be converted.
//! @param aDec The declination to be converted.
//! @param aLat The latitude of the observer (degrees).
//! @param aLon The longitude of the observer (degrees).
//! @param aTime The time reference, which is used to convert to WCS / ECEF.
//! @param aLocal The locally referenced unit vector pointing in the direction of the celestial coordinates.
void UtSphericalEarth::ConvertRA_DecToLocal(double            aRA,
                                            double            aDec,
                                            double            aLat,
                                            double            aLon,
                                            const UtCalendar& aTime,
                                            double            aLocal[3])
{
   // Compute the hour angle.
   double hourAngle = aTime.GetEarthAngleApprox() - aRA + aLon * UtMath::cRAD_PER_DEG;

   // solve for altitude (el)
   double sinDec       = sin(aDec);
   double cosDec       = cos(aDec);
   double lat          = aLat * UtMath::cRAD_PER_DEG;
   double sinLat       = sin(lat);
   double cosLat       = cos(lat);
   double sinHourAngle = sin(hourAngle);
   double cosHourAngle = cos(hourAngle);
   double sinEl        = cosHourAngle * cosDec * cosLat + sinDec * sinLat;
   double el           = asin(sinEl);
   double cosEl        = cos(el);

   double sinAz = sinHourAngle * cosDec / cosEl;
   double cosAz = (cosHourAngle * cosDec * sinLat - sinDec * cosLat) / cosEl;

   aLocal[0] = -cosAz * cosEl;
   aLocal[1] = -sinAz * cosEl;
   aLocal[2] = -sinEl;
}

void UtSphericalEarth::ConvertSphericalToEllipsoidal(const double aSphericalLocWCS[3], double aEllipsoidalLocWCS[3])
{
   double lat;
   double lon;
   double alt;
   ConvertECEFToLLA(aSphericalLocWCS, lat, lon, alt);
   UtEllipsoidalEarth::ConvertLLAToECEF(lat, lon, alt, aEllipsoidalLocWCS);
}

void UtSphericalEarth::ConvertSphericalToEllipsoidalLLA(double  aSphericalLat,
                                                        double  aSphericalLon,
                                                        double  aSphericalAlt,
                                                        double& aOutLat,
                                                        double& aOutLon,
                                                        double& aOutAlt)
{
   double ecef[3];
   UtSphericalEarth::ConvertLLAToECEF(aSphericalLat, aSphericalLon, aSphericalAlt, ecef);
   UtEllipsoidalEarth::ConvertECEFToLLA(ecef, aOutLat, aOutLon, aOutAlt);
}

ut::coords::WCS UtSphericalEarth::ConvertSphericalToEllipsoidal(const ut::coords::WCS& aSphericalLocWCS)
{
   ut::coords::WCS out;
   ConvertSphericalToEllipsoidal(aSphericalLocWCS.GetData(), out.GetData());
   return out;
}

//! Determine the declination associated with a given azimuth, elevation, and latitude.
//!
//! @param aAz The azimuth to use in calculating the declination (radians).
//! @param aEl The elevation to use in calculating the declination (radians).
//! @param aLat The telescope/sensor platform latitude (degrees).
//! @return The declination in radians.
//!
//! @note The declination calculated this way is relative to the current equator; i.e., it may not match an official
//! astronomical declination value (usually referenced to J2000), but should match what an equatorial-mounted
//! telescope/sensor platform would call declination from its own measurements without referring to another reference
//! system, since it's fixed to the earth and thus has its equatorial axis match the current equator.
double UtSphericalEarth::ConvertAzElToDeclination(double aAz, double aEl, double aLat)
{
   const double lat    = aLat * UtMath::cRAD_PER_DEG;
   const double sinDec = sin(aEl) * sin(lat) + cos(aEl) * cos(aAz) * cos(lat);
   return asin(sinDec);
}

//! Determine the local hour angle associated with a given azimuth, elevation, and latitude.
//!
//! @param aAz The azimuth to use in calculating the declination (radians).
//! @param aEl The elevation to use in calculating the declination (radians).
//! @param aLat The telescope/sensor platform latitude (degrees).
//! @return The local hour angle in radians.
double UtSphericalEarth::ConvertAzElToHourAngle(double aAz, double aEl, double aLat)
{
   const double lat = aLat * UtMath::cRAD_PER_DEG;

   // Handle pointing at either north or south celestial pole separately, since that's essentially atan2(0,0)
   // and can be unstable if the arguments aren't exactly zero.
   if (std::abs(aAz) < 1E-15 && std::abs(aEl - lat) < 1E-15) // north pole
   {
      return 0;
   }
   if (std::abs(aAz - UtMath::cPI) < 1E-15 && std::abs(aEl + lat) < 1E-15) // south pole
   {
      return 0;
   }

   const double adjustedAzimuth = aAz - UtMath::cPI;
   return atan2(sin(adjustedAzimuth), cos(adjustedAzimuth) * sin(lat) + tan(aEl) * cos(lat));
}

//! Calculate the parallactic angle for a given azimuth, elevation, and latitude.
//!
//! @param aAz The azimuth to use in calculating the declination (radians).
//! @param aEl The elevation to use in calculating the declination (radians).
//! @param aLat The telescope/sensor platform latitude (degrees).
//! @return The parallactic angle in radians.
double UtSphericalEarth::ConvertAzElToParallacticAngle(double aAz, double aEl, double aLat)
{
   const double declinationRad = ConvertAzElToDeclination(aAz, aEl, aLat);
   const double hourAngleRad   = ConvertAzElToHourAngle(aAz, aEl, aLat);

   return atan2(sin(hourAngleRad),
                cos(declinationRad) * tan(aLat * UtMath::cRAD_PER_DEG) - sin(declinationRad) * cos(hourAngleRad));
}

// Following is a small 'main' program for testing this class...

#ifdef UNIT_TEST

#include <cstdlib>

#include "UtLatPos.hpp"
#include "UtLonPos.hpp"

void TestGreatCircle(double aBegLat, double aBegLon, double aBegHeading, double aDistance)
{
   double begLat     = aBegLat;
   double begLon     = aBegLon;
   double begHeading = aBegHeading;
   double endLat;
   double endLon;
   // double endHeading;
   // double chkLat;
   // double chkLon;
   // double chkHeading;
   // double revHeading;
   double heading;
   double distance;
   {
      // RAII block
      auto out = ut::log::info() << "UtSphericalEarth Unit Test:";
      {
         // RAII block
         auto noteBegin = out.AddNote() << "Begin:";
         noteBegin.AddNote() << "Lat DM: " << UtLatPos(begLat, UtAngle::FmtDM + 4);
         noteBegin.AddNote() << "Lon DM: " << UtLonPos(begLon, UtAngle::FmtDM + 4);
         noteBegin.AddNote() << "Lat Deg: " << UtLatPos(begLat, UtAngle::FmtD + 6);
         noteBegin.AddNote() << "Lon Deg: " << UtLonPos(begLon, UtAngle::FmtD + 6);
         noteBegin.AddNote() << "Hdg: " << begHeading;
         noteBegin.AddNote() << "Dst: " << aDistance;
      }
      UtSphericalEarth::ExtrapolateGreatCirclePosition(begLat, begLon, begHeading, aDistance, endLat, endLon);
      {
         // RAII block
         auto noteEnd = out.AddNote() << "End:";
         noteEnd.AddNote() << "Lat DM: " << UtLatPos(endLat, UtAngle::FmtDM + 4);
         noteEnd.AddNote() << "Lon DM: " << UtLonPos(endLon, UtAngle::FmtDM + 4);
         noteEnd.AddNote() << "Lat Deg: " << UtLatPos(endLat, UtAngle::FmtD + 6);
         noteEnd.AddNote() << "Lon Deg: " << UtLonPos(endLon, UtAngle::FmtD + 6);
      }
   }

   // Verify forward route.

   UtSphericalEarth::GreatCircleHeadingAndDistance(begLat, begLon, endLat, endLon, heading, distance);

   if ((fabs(heading - aBegHeading) > 0.0000001) || (fabs(distance - aDistance) > 0.0000001))
   {
      {
         // RAII block
         auto out  = ut::log::fatal("test_fail") << "Great Circle Heading and Distance:";
         auto note = out.AddNote() << "Fwd:";
         note.AddNote() << "Heading: " << heading;
         note.AddNote() << "Distance: " << distance;
      }
      exit(1);
   }

   // Verify reverse route.

   UtSphericalEarth::GreatCircleHeadingAndDistance(endLat, endLon, begLat, begLon, heading, distance);

   if (fabs(distance - aDistance) > 0.0000001)
   {
      {
         // RAII block
         auto out  = ut::log::fatal("test_fail") << "Great Circle Heading and Distance:";
         auto note = out.AddNote() << "Rev:";
         note.AddNote() << "Heading: " << heading;
         note.AddNote() << "Distance: " << distance;
      }
      exit(1);
   }

   UtSphericalEarth::ExtrapolateGreatCirclePosition(endLat, endLon, heading, distance, begLat, begLon);

   if ((fabs(begLat - aBegLat) > 0.000001) || (fabs(begLon - aBegLon) > 0.000001))
   {
      {
         // RAII block
         auto out  = ut::log::fatal("test_fail") << "Extrapolate Great Circle Position:";
         auto note = out.AddNote() << "Chk:";
         note.AddNote() << "Lat DM: " << UtLatPos(begLat, UtAngle::FmtDM + 4);
         note.AddNote() << "Lon DM: " << UtLonPos(begLon, UtAngle::FmtDM + 4);
         note.AddNote() << "Lat Deg: " << UtLatPos(begLat, UtAngle::FmtD + 6);
         note.AddNote() << "Lon Deg: " << UtLonPos(begLon, UtAngle::FmtD + 6);
      }
      exit(1);
   }
}

int main(int argc, char* argv[])
{
#if 1
   double distance = 2.0 * 60.0 * UtMath::cM_PER_NM;

   double begLatTab[] = {-85.0, -45.0, 0.0, 45.0, 85.0};
   double begLonTab[] = {-178.0, -90.0, 0.0, 90.0, 178.0};

   unsigned int numLat = sizeof(begLatTab) / sizeof(begLatTab[0]);
   unsigned int numLon = sizeof(begLonTab) / sizeof(begLonTab[0]);
   for (unsigned iLat = 0; iLat < numLat; ++iLat)
   {
      double begLat = begLatTab[iLat];
      for (unsigned iLon = 0; iLon < numLon; ++iLon)
      {
         double begLon = begLonTab[iLon];
         for (unsigned int iHead = 0; iHead < 360; ++iHead)
         {
            double begHeading = iHead;
            TestGreatCircle(begLat, begLon, begHeading, distance);
         }
      }
   }
#else
   TestGreatCircle(/* Init Latitude  */ 85.0,
                   /* Init Longitude */ 0.0,
                   /* Init Heading   */ 1.0,
                   /* Distance (m)   */ 1200.0 * 1852.0);
   TestGreatCircle(/* Init Latitude  */ -85.0,
                   /* Init Longitude */ 0.0,
                   /* Init Heading   */ 179.0,
                   /* Distance (m)   */ 1200.0 * 1852.0);
#endif
   return 0;
}

#endif
