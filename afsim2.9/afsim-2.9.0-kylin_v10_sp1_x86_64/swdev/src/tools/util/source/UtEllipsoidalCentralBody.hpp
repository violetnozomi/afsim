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

#ifndef UtEllipsoidalCentralBody_HPP
#define UtEllipsoidalCentralBody_HPP

#include "ut_export.h"
namespace ut
{
class CentralBodyEllipsoid;
}

//! Utility routines for an ellipsoidal central body (usually, utilizing an Earth ellipsoid).
//! The ECEF coordinate system (Earth-centered, Earth-fixed; ECEF
//! [or Ellipsoid-centered, Ellipsoid-fixed when the central body is not the Earth]) is defined as follows:
//! X-axis runs through the body at 0N 0E, the y-axis runs through
//! the body at 0N 90E, and the z-axis runs through the north pole.
//! Input/Output values are in the following units:
//!
//! \li Latitude : -90 (South) <= lat <= 90 (North)
//! \li Longitude: -180 (West) <= lon <= 180 (East)
//! \li Altitude : meters
//! \li Heading  : 0 <= heading <= 360
//! \li Distance : meters
//! \li ECEF     : meters
//!
//! @note The algorithm for EllipsoidalCentralBody::ConvertECEFToLLA is a direct,
//!       yet not exact solution that provides centimeter accuracy for heights < 1000km
//!
//! See http://www.colorado.edu/geograpy/gcraft/notes/gps/gif/xyzllh.gif
//!
//! Which refers to:
//!
//!   Bowring, B. 1976. Transformation for spatial to geographical coordinates.
//!                     Survey Review, XXIII: pg 323-327
//!
//! There is an iterative solution that works for all cases.
namespace ut
{

namespace EllipsoidalCentralBody
{
UT_EXPORT void ComputeENUTransform(double                          aRefLat,
                                   double                          aRefLon,
                                   double                          aRefAlt,
                                   const ut::CentralBodyEllipsoid& aEllipsoid,
                                   double                          aTransECEF[3][3],
                                   double                          aRefECEF[3]);

UT_EXPORT void ComputeENUTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3]);

UT_EXPORT void ComputeNEDTransform(double                          aRefLat,
                                   double                          aRefLon,
                                   double                          aRefAlt,
                                   const ut::CentralBodyEllipsoid& aEllipsoid,
                                   double                          aTransECEF[3][3],
                                   double                          aRefECEF[3]);

UT_EXPORT void ComputeNEDTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3]);

UT_EXPORT void ConvertLocalToLLA(const double                    aRefECEF[3],
                                 const double                    aTransECEF[3][3],
                                 const double                    aLocal[3],
                                 const ut::CentralBodyEllipsoid& aEllipsoid,
                                 double&                         aLat,
                                 double&                         aLon,
                                 double&                         aAlt);

UT_EXPORT void ConvertLLAToLocal(const double                    aRefECEF[3],
                                 const double                    aTransECEF[3][3],
                                 double                          aLat,
                                 double                          aLon,
                                 double                          aAlt,
                                 const ut::CentralBodyEllipsoid& aEllipsoid,
                                 double                          aLocal[3]);

UT_EXPORT void
ConvertLocalToECEF(const double aRefECEF[3], const double aTransECEF[3][3], const double aLocal[3], double aECEF[3]);

UT_EXPORT void
ConvertECEFToLocal(const double aRefECEF[3], const double aTransECEF[3][3], const double aECEF[3], double aLocal[3]);

UT_EXPORT void ConvertLocalToECEF(const double aTransECEF[3][3], const double aLocal[3], double aECEF[3]);

UT_EXPORT void ConvertECEFToLocal(const double aTransECEF[3][3], const double aECEF[3], double aLocal[3]);

UT_EXPORT void
ConvertLLAToECEF(double aLat, double aLon, double aAlt, const ut::CentralBodyEllipsoid& aEllipsoid, double aECEF[]);

UT_EXPORT void ConvertECEFToLLA(const double                    aECEF[],
                                const ut::CentralBodyEllipsoid& aEllipsoid,
                                double&                         aLat,
                                double&                         aLon,
                                double&                         aAlt);

UT_EXPORT void ExtrapolateGreatEllipsePosition(double                          aBegLat,
                                               double                          aBegLon,
                                               double                          aHeading,
                                               double                          aDistance,
                                               const ut::CentralBodyEllipsoid& aEllipsoid,
                                               double&                         aEndLat,
                                               double&                         aEndLon);

UT_EXPORT double GetVincentyDistance(double                          aBegLat,
                                     double                          aBegLon,
                                     double                          aEndLat,
                                     double                          aEndLon,
                                     const ut::CentralBodyEllipsoid& aEllipsoid,
                                     double&                         aHdgStart,
                                     double&                         aHdgEnd);

UT_EXPORT double MetersPerDegreeLat(double aLatitude, const ut::CentralBodyEllipsoid& aEllipsoid);

UT_EXPORT double MetersPerDegreeLon(double aLongitude, const ut::CentralBodyEllipsoid& aEllipsoid);
} // namespace EllipsoidalCentralBody

} // namespace ut

#endif
