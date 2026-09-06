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

#ifndef UTSPHERICALEARTH_HPP
#define UTSPHERICALEARTH_HPP

#include "ut_export.h"

#include "UtCoords.hpp"

class UtCalendar;
class UtLLPos;

//! Utility routines for the 'standard' spherical earth.  Using the definition
//! the nautical mile, in which 1 minute of arc at the earth's surface
//! equals exactly 1852 meters, the circumference of the earth is:
//!
//!    360.0 * 60.0 * 1852.0 meters = 40003200.0
//!
//! The earth-centered/earth-fixed coordinate system is defined as follows:
//!
//! X-axis runs through the earth at 0N 0E, the y-axis runs through
//! the earth at 0N 90E, and the z-axis runs through the north pole.

namespace UtSphericalEarth
{
//! Definition of Earth's radius based on the definition of the nautical mile.
//! 360.0 * 60.0 * 1852.0 / (2.0 * PI)
constexpr double cEARTH_RADIUS = 6366707.0194937074958;

//! Definition of the Earth's radius based on the volumetric mean radius
//!(radius at which the volume of the ellipsoid is the same as the
//! spherical volume)
constexpr double cEARTH_MEAN_RADIUS = 6371000.0;

UT_EXPORT void
ComputeENUTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3], double aRefECEF[3]);

UT_EXPORT ut::coords::WCS ComputeENUTransform(const ut::coords::LLA& aRef, double aTransECEF[3][3]);

UT_EXPORT void ComputeENUTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3]);

UT_EXPORT void
ComputeNEDTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3], double aRefECEF[3]);

UT_EXPORT ut::coords::WCS ComputeNEDTransform(const ut::coords::LLA& aRef, double aTransECEF[3][3]);

UT_EXPORT void ComputeNEDTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3]);

UT_EXPORT void ConvertLocalToLLA(const double aRefECEF[3],
                                 const double aTransECEF[3][3],
                                 const double aLocal[3],
                                 double&      aLat,
                                 double&      aLon,
                                 double&      aAlt);

UT_EXPORT ut::coords::LLA ConvertLocalToLLA(const ut::coords::WCS& aRefECEF,
                                            const double           aTransECEF[3][3],
                                            const ut::coords::NED& aLocal);

UT_EXPORT ut::coords::LLA ConvertLocalToLLA(const ut::coords::WCS& aRefECEF,
                                            const double           aTransECEF[3][3],
                                            const ut::coords::ENU& aLocal);

UT_EXPORT void ConvertLLAToLocal(const double aRefECEF[3],
                                 const double aTransECEF[3][3],
                                 double       aLat,
                                 double       aLon,
                                 double       aAlt,
                                 double       aLocal[3]);

UT_EXPORT ut::coords::NED ConvertLLAToNED(const ut::coords::WCS& aRefECEF,
                                          const double           aTransECEF[3][3],
                                          const ut::coords::LLA& aLLA);

UT_EXPORT ut::coords::ENU ConvertLLAToENU(const ut::coords::WCS& aRefECEF,
                                          const double           aTransECEF[3][3],
                                          const ut::coords::LLA& aLLA);

UT_EXPORT void
ConvertLocalToECEF(const double aRefECEF[3], const double aTransECEF[3][3], const double aLocal[3], double aECEF[3]);

UT_EXPORT ut::coords::WCS ConvertLocalToECEF(const ut::coords::WCS& aRefECEF,
                                             const double           aTransECEF[3][3],
                                             const ut::coords::NED& aLocal);

UT_EXPORT ut::coords::WCS ConvertLocalToECEF(const ut::coords::WCS& aRefECEF,
                                             const double           aTransECEF[3][3],
                                             const ut::coords::ENU& aLocal);

UT_EXPORT void
ConvertECEFToLocal(const double aRefECEF[3], const double aTransECEF[3][3], const double aECEF[3], double aLocal[3]);

UT_EXPORT ut::coords::NED ConvertECEFToNED(const ut::coords::WCS& aRefECEF,
                                           const double           aTransECEF[3][3],
                                           const ut::coords::WCS& aECEF);

UT_EXPORT ut::coords::ENU ConvertECEFToENU(const ut::coords::WCS& aRefECEF,
                                           const double           aTransECEF[3][3],
                                           const ut::coords::WCS& aECEF);

UT_EXPORT void ConvertLocalToECEF(const double aTransECEF[3][3], const double aLocal[3], double aECEF[3]);

UT_EXPORT ut::coords::WCS ConvertLocalToECEF(const double aTransECEF[3][3], const ut::coords::NED& aLocal);

UT_EXPORT void ConvertECEFToLocal(const double aTransECEF[3][3], const double aECEF[3], double aLocal[3]);

UT_EXPORT ut::coords::NED ConvertECEFToNED(const double aTransECEF[3][3], const ut::coords::WCS& aECEF);

UT_EXPORT ut::coords::ENU ConvertECEFToENU(const double aTransECEF[3][3], const ut::coords::WCS& aECEF);

UT_EXPORT void ConvertLLAToECEF(double aLat, double aLon, double aAlt, double aECEF[]);

UT_EXPORT ut::coords::WCS ConvertLLAToECEF(const ut::coords::LLA& aLLA);

UT_EXPORT void ConvertECEFToLLA(const double aECEF[], double& aLat, double& aLon, double& aAlt);

UT_EXPORT ut::coords::LLA ConvertECEFToLLA(const ut::coords::WCS& aECEF);

UT_EXPORT void GreatCircleHeadingAndDistance(double  aBegLat,
                                             double  aBegLon,
                                             double  aEndLat,
                                             double  aEndLon,
                                             double& aHeading,
                                             double& aDistance);

UT_EXPORT double GreatCircleHeadingAndDistance(ut::coords::LL aBeg, ut::coords::LL aEnd, double& aHeading);

UT_EXPORT void ExtrapolateGreatCirclePosition(double  aBegLat,
                                              double  aBegLon,
                                              double  aHeading,
                                              double  aDistance,
                                              double& aEndLat,
                                              double& aEndLon);

UT_EXPORT ut::coords::LL ExtrapolateGreatCirclePosition(ut::coords::LL aBeg, double aHeading, double aDistance);

UT_EXPORT bool
MaskedByHorizon(double aLat1, double aLon1, double aAlt1, double aLat2, double aLon2, double aAlt2, double aRadiusScale);

UT_EXPORT bool MaskedByHorizon(const ut::coords::LLA& aLLA1, const ut::coords::LLA& aLLA2, double aRadiusScale);

UT_EXPORT void GreatCircleIntersection(UtLLPos&       aLLIntersect1,
                                       UtLLPos&       aLLIntersect2,
                                       const UtLLPos& aSeg1End1,
                                       const UtLLPos& aSeg1End2,
                                       const UtLLPos& aSeg2End1,
                                       const UtLLPos& aSeg2End2);

UT_EXPORT void GreatCircleClosestPoint(double  aLat1,
                                       double  aLon1,
                                       double  aHeading,
                                       double  aLat2,
                                       double  aLon2,
                                       double& aClosestPointLat,
                                       double& aClosestPointLon);

UT_EXPORT void ConvertRA_DecToECEF(double aRA, double aDec, const UtCalendar& aTime, double aVecECEF[3]);

UT_EXPORT void ConvertRA_DecToECI(double aRA, double aDec, double aVecECI[3]);

UT_EXPORT void ConvertECIToRA_Dec(const double aLocationECI[3], double& aRA, double& aDec);

UT_EXPORT void
ConvertRA_DecToAzEl(double aRA, double aDec, double aLat, double aLon, const UtCalendar& aTime, double& aAz, double& aEl);

UT_EXPORT void
ConvertRA_DecToLocal(double aRA, double aDec, double aLat, double aLon, const UtCalendar& aTime, double aLocal[3]);

UT_EXPORT void ConvertSphericalToEllipsoidal(const double aSphericalLocWCS[3], double aEllipsoidalLocWCS[3]);

UT_EXPORT void ConvertSphericalToEllipsoidalLLA(double  aSphericalLat,
                                                double  aSphericalLon,
                                                double  aSphericalAlt,
                                                double& aOutLat,
                                                double& aOutLon,
                                                double& aOutAlt);

UT_EXPORT ut::coords::WCS ConvertSphericalToEllipsoidal(const ut::coords::WCS& aSphericalLocWCS);

UT_EXPORT double ConvertAzElToDeclination(double aAz, double aEl, double aLat);

UT_EXPORT double ConvertAzElToHourAngle(double aAz, double aEl, double aLat);

UT_EXPORT double ConvertAzElToParallacticAngle(double aAz, double aEl, double aLat);

}; // namespace UtSphericalEarth

#endif
