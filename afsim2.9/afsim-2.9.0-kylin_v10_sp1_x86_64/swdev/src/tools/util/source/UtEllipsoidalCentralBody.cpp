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

#include "UtEllipsoidalCentralBody.hpp"

#include <cmath>
#include <limits>

#include "UtCentralBodyEllipsoid.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"

// Input/Output values are in the following units:
//
// Latitude : -90 (South) <= lat <= 90 (North)
// Longitude: -180 (West) <= lon <= 180 (East)
// Heading  : 0 <= heading <= 360
// Distance : meters

namespace ut
{

//! Compute the transformation to convert between the global ECEF frame and
//! a local ENU frame at the specified reference point.
void EllipsoidalCentralBody::ComputeENUTransform(double                      aRefLat,
                                                 double                      aRefLon,
                                                 double                      aRefAlt,
                                                 const CentralBodyEllipsoid& aEllipsoid,
                                                 double                      aTransECEF[3][3],
                                                 double                      aRefECEF[3])
{
   // Calculate the ECEF to ENU transformation matrix
   ComputeENUTransform(aRefLat, aRefLon, aRefAlt, aTransECEF);

   // Compute the position of the reference point in ECEF
   ConvertLLAToECEF(aRefLat, aRefLon, aRefAlt, aEllipsoid, aRefECEF);
}

//! Compute the transformation to convert between the global ECEF frame and
//! a local ENU frame at the specified reference point.
//! @note This form only produces the transformation matrix.
void EllipsoidalCentralBody::ComputeENUTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3])
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
void EllipsoidalCentralBody::ComputeNEDTransform(double                      aRefLat,
                                                 double                      aRefLon,
                                                 double                      aRefAlt,
                                                 const CentralBodyEllipsoid& aEllipsoid,
                                                 double                      aTransECEF[3][3],
                                                 double                      aRefECEF[3])
{
   // Calculate the ECEF to NED transformation matrix
   ComputeNEDTransform(aRefLat, aRefLon, aRefAlt, aTransECEF);

   // Compute the position of the reference point in ECEF
   ConvertLLAToECEF(aRefLat, aRefLon, aRefAlt, aEllipsoid, aRefECEF);
}

//! Compute the transformation to convert between the global ECEF frame and
//! a local NED frame at a specified reference point.
//! @note This form only produces the transformation matrix.
void EllipsoidalCentralBody::ComputeNEDTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3])
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
void EllipsoidalCentralBody::ConvertLLAToLocal(const double                aRefECEF[3],
                                               const double                aTransECEF[3][3],
                                               double                      aLat,
                                               double                      aLon,
                                               double                      aAlt,
                                               const CentralBodyEllipsoid& aEllipsoid,
                                               double                      aLocal[3])
{
   double ecef[3];

   ConvertLLAToECEF(aLat, aLon, aAlt, aEllipsoid, ecef);
   ConvertECEFToLocal(aRefECEF, aTransECEF, ecef, aLocal);
}

//! Convert a coordinate in a local ENU or NED frame to LLA
void EllipsoidalCentralBody::ConvertLocalToLLA(const double                aRefECEF[3],
                                               const double                aTransECEF[3][3],
                                               const double                aLocal[3],
                                               const CentralBodyEllipsoid& aEllipsoid,
                                               double&                     aLat,
                                               double&                     aLon,
                                               double&                     aAlt)
{
   double ecef[3];

   ConvertLocalToECEF(aRefECEF, aTransECEF, aLocal, ecef);
   ConvertECEFToLLA(ecef, aEllipsoid, aLat, aLon, aAlt);
}

//! Convert a POSITION in a local ENU or NED frame to the global ECEF frame.
void EllipsoidalCentralBody::ConvertLocalToECEF(const double aRefECEF[3],
                                                const double aTransECEF[3][3],
                                                const double aLocal[3],
                                                double       aECEF[3])
{
   double tempECEF[3];

   UtMat3d::InverseTransform(tempECEF, aTransECEF, aLocal); // ECEF' = T(inv) * Local
   UtVec3d::Add(aECEF, tempECEF, aRefECEF);                 // ECEF  = ECEF' + ECEFref
}

//! Convert a POSITION in the global ECEF frame to a local ENU or NED frame.
void EllipsoidalCentralBody::ConvertECEFToLocal(const double aRefECEF[3],
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
void EllipsoidalCentralBody::ConvertLocalToECEF(const double aTransECEF[3][3], const double aLocal[3], double aECEF[3])
{
   UtMat3d::InverseTransform(aECEF, aTransECEF, aLocal); // ECEF = T(inv) * Local
}

//! Convert a VECTOR in the global ECEF frame to a local ENU or NED frame.
//! This is just like the 4-operand version with a reference of (0,0,0).
void EllipsoidalCentralBody::ConvertECEFToLocal(const double aTransECEF[3][3], const double aECEF[3], double aLocal[3])
{
   UtMat3d::Transform(aLocal, aTransECEF, aECEF); // Local = T x ECEF
}

//! Given a latitude, longitude and altitude, return the ECEF XYZ coordinates.
void EllipsoidalCentralBody::ConvertLLAToECEF(double                      aLat,
                                              double                      aLon,
                                              double                      aAlt,
                                              const CentralBodyEllipsoid& aEllipsoid,
                                              double                      aECEF[])
{
   double lat    = aLat * UtMath::cRAD_PER_DEG;
   double lon    = aLon * UtMath::cRAD_PER_DEG;
   double sinLat = sin(lat);
   double cosLat = cos(lat);
   double sinLon = sin(lon);
   double cosLon = cos(lon);

   // R_N = Radius of curvature in the prime vertical.
   // Otherwise known as the normal to the ellipsoid terminating at the minor axis.

   double R_N = aEllipsoid.GetSemiMajorAxis() / sqrt(1.0 - aEllipsoid.GetFirstEccentricitySquared() * sinLat * sinLat);
   double temp1 = (R_N + aAlt) * cosLat;
   aECEF[0]     = temp1 * cosLon;
   aECEF[1]     = temp1 * sinLon;
   aECEF[2]     = ((1.0 - aEllipsoid.GetFirstEccentricitySquared()) * R_N + aAlt) * sinLat;
}

//! Given ECEF XYZ coordinates, return the latitude, longitude and altitude.
void EllipsoidalCentralBody::ConvertECEFToLLA(const double                aECEF[],
                                              const CentralBodyEllipsoid& aEllipsoid,
                                              double&                     aLat,
                                              double&                     aLon,
                                              double&                     aAlt)
{
   // NOTE: This is a direct, yet not exact solution that provides centimeter
   //       accuracy for heights < 1000km
   //
   // See http://www.colorado.edu/geograpy/gcraft/notes/gps/gif/xyzllh.gif
   //
   // Which refers to:
   //
   //   Bowring, B. 1976. Transformation for spatial to geographical coordinates.
   //                     Survey Review, XXIII: pg 323-327
   //
   // There is an iterative solution that works for all cases.

   double x = aECEF[0];
   double y = aECEF[1];
   double z = aECEF[2];

   // If we're on the Z axis then some of the denominators go to zero.  If this
   // is the case (or if we're very close), we treat it special...

   double p = sqrt((x * x) + (y * y));
   if (p > 1.0e-8)
   {
      double theta      = atan((z * aEllipsoid.GetSemiMajorAxis()) / (p * aEllipsoid.GetSemiMinorAxis()));
      double sinTheta   = sin(theta);
      double cosTheta   = cos(theta);
      double sinTheta_3 = sinTheta * sinTheta * sinTheta;
      double cosTheta_3 = cosTheta * cosTheta * cosTheta;

      double phi = atan2((z + aEllipsoid.GetSecondEccentricitySquared() * aEllipsoid.GetSemiMinorAxis() * sinTheta_3),
                         (p - aEllipsoid.GetFirstEccentricitySquared() * aEllipsoid.GetSemiMajorAxis() * cosTheta_3));
      double lambda = atan2(y, x);
      double cosPhi = cos(phi);
      double sinPhi = sin(phi);
      double R_N = aEllipsoid.GetSemiMajorAxis() / sqrt(1.0 - aEllipsoid.GetFirstEccentricitySquared() * sinPhi * sinPhi);
      double h   = (p / cosPhi) - R_N;

      aLat = phi * UtMath::cDEG_PER_RAD;
      aLon = lambda * UtMath::cDEG_PER_RAD;
      aAlt = h;
   }
   else
   {
      // Special case where the point is 'on' the Z axis

      if (z >= 0.0)
      {
         aLat = 90.0;
      }
      else
      {
         aLat = -90.0;
      }
      aLon = 0.0;
      aAlt = fabs(z) - aEllipsoid.GetSemiMinorAxis();
   }
}

void EllipsoidalCentralBody::ExtrapolateGreatEllipsePosition(double                      aBegLat,
                                                             double                      aBegLon,
                                                             double                      aHeading,
                                                             double                      aDistance,
                                                             const CentralBodyEllipsoid& aEllipsoid,
                                                             double&                     aEndLat,
                                                             double&                     aEndLon)
{
   if (aDistance == 0.0)
   {
      aEndLat = aBegLat;
      aEndLon = aBegLon;
      return;
   }

   double startLat = aBegLat * UtMath::cRAD_PER_DEG;
   double startLon = aBegLon * UtMath::cRAD_PER_DEG;
   double heading  = aHeading * UtMath::cRAD_PER_DEG;
   double distance = aDistance / UtMath::cM_PER_NM;

   double EPS = 0.00000000005;

   // Equatorial radius in nm
   double a = 6378.137 / 1.852;

   // Inverse of the flattening (1/f)
   double flattening = 1.0 / aEllipsoid.GetFlatteningReciprocal();
   double r          = 1 - flattening;

   double tu = r * tan(startLat);

   double sinHeading = sin(heading);
   double cosHeading = cos(heading);

   double beta = 0.0;
   if (cosHeading != 0)
   {
      beta = 2.0 * atan2(tu, cosHeading);
   }

   double cu  = 1.0 / sqrt(1 + tu * tu);
   double su  = tu * cu;
   double sa  = cu * sinHeading;
   double c2a = 1.0 - sa * sa;

   double x = 1.0 + sqrt(1.0 + c2a * (1.0 / (r * r) - 1.0));
   x        = (x - 2.0) / x;
   double c = 1.0 - x;
   c        = (x * x / 4. + 1.) / c;

   double d = (0.375 * x * x - 1.) * x;
   tu       = distance / (r * a * c);
   double y = tu;
   c        = y + 1;

   double e  = 0.0;
   double sy = 0.0;
   double cy = 0.0;
   double cz = 0.0;

   while (fabs(y - c) > EPS)
   {
      sy = sin(y);
      cy = cos(y);
      cz = cos(beta + y);

      e = 2.0 * cz * cz - 1.0;
      c = y;
      x = e * cy;
      y = e + e - 1.0;
      y = (((sy * sy * 4. - 3.) * y * cz * d / 6. + x) * d / 4. - cz) * sy * d + tu;
   }

   beta = cu * cy * cosHeading - su * sy;
   c    = r * sqrt(sa * sa + beta * beta);
   d    = su * cy + cu * sy * cosHeading;

   aEndLat = fmod(atan2(d, c) + UtMath::cPI_OVER_2, UtMath::cTWO_PI) - UtMath::cPI_OVER_2;

   c = cu * cy - su * sy * cosHeading;
   x = atan2(sy * sinHeading, c);
   c = ((-3. * c2a + 4.) * flattening + 4.) * c2a * flattening / 16.0;
   d = ((e * cy * c + cz) * sy * c + y) * sa;

   aEndLon = fmod((startLon + x - (1.0 - c) * d * flattening) + UtMath::cPI, UtMath::cTWO_PI) - UtMath::cPI;

   aEndLon *= UtMath::cDEG_PER_RAD;
   aEndLat *= UtMath::cDEG_PER_RAD;
}

//! Return distance in meters from lat, lon to lat, lon.
//! @param aBegLat The beginning latitude in degrees.
//! @param aBegLon The beginning longitude in degrees.
//! @param aEndLat The ending latitude in degrees.
//! @param aEndLon The ending longitude in degrees.
//! @param aHdgStart The start heading in degrees.
//! @param aHdgEnd The final heading in degrees.
//! @return Distance from beginning lat, lon to ending lat, lon in meters
//! @note Algorithm from "Survey Review" by T. Vincenty, April, 1975 Published by Directorate of
//! Overseas Surveys of the Ministry of Overseas Development, Kingston Road, Tolworth, Surrey
double EllipsoidalCentralBody::GetVincentyDistance(double                      aBegLat,
                                                   double                      aBegLon,
                                                   double                      aEndLat,
                                                   double                      aEndLon,
                                                   const CentralBodyEllipsoid& aEllipsoid,
                                                   double&                     aHdgStart,
                                                   double&                     aHdgEnd)
{
   if (fabs(aBegLat - aEndLat) < std::numeric_limits<double>::epsilon() &&
       fabs(aBegLon - aEndLon) < std::numeric_limits<double>::epsilon()) // same point, no distance
   {
      aHdgStart = 0.0;
      aHdgEnd   = 0.0;
      return 0.0;
   }

   static const double f_ = 1.0 / aEllipsoid.GetFlatteningReciprocal();

   const double L     = (aEndLon - aBegLon) * UtMath::cRAD_PER_DEG;
   const double U1    = atan((1.0 - f_) * tan(aBegLat * UtMath::cRAD_PER_DEG));
   const double cosU1 = cos(U1);
   const double sinU1 = sin(U1);
   const double U2    = atan((1.0 - f_) * tan(aEndLat * UtMath::cRAD_PER_DEG));
   const double cosU2 = cos(U2);
   const double sinU2 = sin(U2);

   double lambda    = L;
   double sinLambda = sin(lambda);
   double cosLambda = cos(lambda);

   double diffL = 1.0; // Force at least one pass

   double cosSqAlpha = 0.0;
   double cos2SigmaM = 0.0;
   double sigma      = 0.0;
   double sinSigma   = 0.0;
   double cosSigma   = 0.0;

   short loop_cnt = 0;
   while (fabs(diffL) > 1.0E-12 && loop_cnt < 24)
   {
      ++loop_cnt;
      double lambdaP    = lambda;
      double sinSqSigma = (cosU2 * cosU2 * sinLambda * sinLambda) +
                          (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) * (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda);
      if (fabs(sinSqSigma) < std::numeric_limits<double>::epsilon()) // Co-incident points Bail
      {
         aHdgStart = 0.0;
         aHdgEnd   = 0.0;
         return 0.0;
      }
      sinSigma = sqrt(sinSqSigma);
      cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
      sigma    = atan2(sinSigma, cosSigma);

      double sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
      // Trig Identity cos^2(ang) = 1 - sin^2(ang)
      cosSqAlpha = 1.0 - sinAlpha * sinAlpha;
      cos2SigmaM = cosSigma - 2.0 * sinU1 * sinU2 / cosSqAlpha;
      // Equatorial line: cosSqAlpha = 0
      if (cosSqAlpha == 0.0)
         cos2SigmaM = 0;
      double C = f_ / 16.0 * cosSqAlpha * (4.0 + f_ * (4.0 - 3.0 * cosSqAlpha));
      lambda   = L + (1.0 - C) * f_ * sinAlpha *
                      (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1.0 + 2.0 * cos2SigmaM * cos2SigmaM)));
      sinLambda = sin(lambda);
      cosLambda = cos(lambda);
      diffL     = lambda - lambdaP;
   }
   const double uSq        = cosSqAlpha * aEllipsoid.GetSecondEccentricitySquared();
   const double A          = 1.0 + uSq / 256.0 * (64.0 + uSq * (-12.0 + 5 * uSq));
   const double B          = uSq / 512.0 * (128.0 + uSq * (-64.0 + 37.0 * uSq));
   const double deltaSigma = B * sinSigma * (cos2SigmaM + 0.25 * B * cosSigma * (-1.0 + cos2SigmaM * cos2SigmaM));
   const double distance   = aEllipsoid.GetSemiMinorAxis() * A * (sigma - deltaSigma);

   aHdgStart = atan2((cosU2 * sinLambda), (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda)) * UtMath::cDEG_PER_RAD;
   aHdgEnd   = atan2((cosU1 * sinLambda), (-sinU1 * cosU2 + cosU1 * sinU2 * cosLambda)) * UtMath::cDEG_PER_RAD;

   return distance;
}

double EllipsoidalCentralBody::MetersPerDegreeLat(double aLatitude, const CentralBodyEllipsoid& aEllipsoid)
{
   // Compute R_N, the radius of curvature in the prime vertical

   double sinLat = sin(aLatitude * UtMath::cRAD_PER_DEG);
   double R_N = aEllipsoid.GetSemiMajorAxis() / sqrt(1.0 - aEllipsoid.GetFirstEccentricitySquared() * sinLat * sinLat);

   return ((2.0 * UtMath::cPI * R_N) / 360.0);
}

double EllipsoidalCentralBody::MetersPerDegreeLon(double aLongitude, const CentralBodyEllipsoid& aEllipsoid)
{
   // Compute R_N, the radius of curvature in the prime vertical

   double sinLat = sin(aLongitude * UtMath::cRAD_PER_DEG);
   double R_N = aEllipsoid.GetSemiMajorAxis() / sqrt(1.0 - aEllipsoid.GetFirstEccentricitySquared() * sinLat * sinLat);

   // Compute the radius of the circle of constant latitude

   double RLat = R_N * cos(aLongitude * UtMath::cRAD_PER_DEG);

   return ((2.0 * UtMath::cPI * RLat) / 360.0);
}

} // namespace ut
