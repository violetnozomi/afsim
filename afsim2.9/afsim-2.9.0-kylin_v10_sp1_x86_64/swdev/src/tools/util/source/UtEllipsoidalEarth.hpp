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

#ifndef UTELLIPSOIDALEARTH_HPP
#define UTELLIPSOIDALEARTH_HPP

#include "ut_export.h"

#include <string>

#include "UtCoords.hpp"

namespace UtEllipsoidalEarth
{
UT_EXPORT void
ComputeENUTransform(double aRefLat, double aRefLon, double aRefAlt, double aTransECEF[3][3], double aRefECEF[3]);

UT_EXPORT ut::coords::WCS ComputeENUTransform(const ut::coords::LLA& aRef, double aTransECEF[3][3]);

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

UT_EXPORT void ConvertLLAToLocal(const double aRefECEF[3],
                                 const double aTransECEF[3][3],
                                 double       aLat,
                                 double       aLon,
                                 double       aAlt,
                                 double       aLocal[3]);

UT_EXPORT ut::coords::LLA ConvertLocalToLLA(const ut::coords::WCS& aRefECEF,
                                            const double           aTransECEF[3][3],
                                            const ut::coords::NED& aLocal);

UT_EXPORT ut::coords::LLA ConvertLocalToLLA(const ut::coords::WCS& aRefECEF,
                                            const double           aTransECEF[3][3],
                                            const ut::coords::ENU& aLocal);

UT_EXPORT ut::coords::NED ConvertLLAToNED(const ut::coords::WCS& aRefECEF,
                                          const double           aTransECEF[3][3],
                                          const ut::coords::LLA& aLLA);

UT_EXPORT ut::coords::ENU ConvertLLAToENU(const ut::coords::WCS& aRefECEF,
                                          const double           aTransECEF[3][3],
                                          const ut::coords::LLA& aLLA);

UT_EXPORT void
ConvertLocalToECEF(const double aRefECEF[3], const double aTransECEF[3][3], const double aLocal[3], double aECEF[3]);

UT_EXPORT void
ConvertECEFToLocal(const double aRefECEF[3], const double aTransECEF[3][3], const double aECEF[3], double aLocal[3]);


UT_EXPORT ut::coords::WCS ConvertLocalToECEF(const ut::coords::WCS& aRefECEF,
                                             const double           aTransECEF[3][3],
                                             const ut::coords::NED& aLocal);

UT_EXPORT ut::coords::WCS ConvertLocalToECEF(const ut::coords::WCS& aRefECEF,
                                             const double           aTransECEF[3][3],
                                             const ut::coords::ENU& aLocal);

UT_EXPORT ut::coords::NED ConvertECEFToNED(const ut::coords::WCS& aRefECEF,
                                           const double           aTransECEF[3][3],
                                           const ut::coords::WCS& aECEF);

UT_EXPORT ut::coords::ENU ConvertECEFToENU(const ut::coords::WCS& aRefECEF,
                                           const double           aTransECEF[3][3],
                                           const ut::coords::WCS& aECEF);

UT_EXPORT void ConvertLocalToECEF(const double aTransECEF[3][3], const double aLocal[3], double aECEF[3]);

UT_EXPORT void ConvertECEFToLocal(const double aTransECEF[3][3], const double aECEF[3], double aLocal[3]);

UT_EXPORT ut::coords::NED ConvertECEFToNED(const double aTransECEF[3][3], const ut::coords::WCS& aECEF);

UT_EXPORT ut::coords::ENU ConvertECEFToENU(const double aTransECEF[3][3], const ut::coords::WCS& aECEF);

UT_EXPORT void ConvertLLAToECEF(double aLat, double aLon, double aAlt, double aECEF[]);

UT_EXPORT ut::coords::WCS ConvertLLAToECEF(const ut::coords::LLA& aLLA);

UT_EXPORT void ConvertECEFToLLA(const double aECEF[], double& aLat, double& aLon, double& aAlt);

UT_EXPORT ut::coords::LLA ConvertECEFToLLA(const ut::coords::WCS& aECEF);

UT_EXPORT void ConvertUTMToLL(const unsigned int aZone,
                              const double       aEastings,
                              const double       aNorthings,
                              const bool         aNorthernHemisphere,
                              double&            aLat,
                              double&            aLon);

UT_EXPORT void ConvertLLToUTM(const double  aLat,
                              const double  aLon,
                              unsigned int& aLonZone,
                              bool&         aNorthernHemisphere,
                              double&       aEastings,
                              double&       aNorthings);

UT_EXPORT void ConvertLLToMGRS(const double aLat, const double aLon, std::string& aLocationMGRS);

UT_EXPORT bool ConvertMGRSToLL(const std::string& aLocationMGRS, double& aLat, double& aLon);

UT_EXPORT void ExtrapolateGreatEllipsePosition(double  aBegLat,
                                               double  aBegLon,
                                               double  aHeading,
                                               double  aDistance,
                                               double& aEndLat,
                                               double& aEndLon);

UT_EXPORT ut::coords::LL ExtrapolateGreatEllipsePosition(ut::coords::LL aBeg, double aHeading, double aDistance);

UT_EXPORT double
GetVincentyDistance(double aBegLat, double aBegLon, double aEndLat, double aEndLon, double& aHdgStart, double& aHdgEnd);

UT_EXPORT double GetVincentyDistance(ut::coords::LL aBeg, ut::coords::LL aEnd, double& aHdgStart, double& aHdgEnd);

UT_EXPORT void ConvertTODToECEF(double aEarthAngle, const double aLocationTOD[3], double aLocationECEF[3]);

UT_EXPORT void ConvertECEFToTOD(double aEarthAngle, const double aLocationECEF[3], double aLocationTOD[3]);

UT_EXPORT double MetersPerDegreeLat(double aLatitude);

UT_EXPORT double MetersPerDegreeLon(double aLongitude);

UT_EXPORT void ConvertEllipsoidalToSpherical(const double aEllipsoidalLocWCS[3], double aSphericalLocWCS[3]);

UT_EXPORT void ConvertEllipsoidalToSphericalLLA(double  aEllipsoidalLat,
                                                double  aEllipsoidalLon,
                                                double  aEllipsoidalAlt,
                                                double& aOutLat,
                                                double& aOutLon,
                                                double& aOutAlt);

UT_EXPORT ut::coords::WCS ConvertEllipsoidalToSpherical(const ut::coords::WCS& aEllipsoidalLocWCS);

}; // namespace UtEllipsoidalEarth

#endif
