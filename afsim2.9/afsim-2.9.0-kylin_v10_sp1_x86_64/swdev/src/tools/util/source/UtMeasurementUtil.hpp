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

#ifndef UTMEASUREMENTUTIL_HPP
#define UTMEASUREMENTUTIL_HPP

#include "ut_export.h"

template<typename T>
class UtMatrix;

class UtEntity;
class UtMeasurementData;

namespace UtMeasurementUtil
{
void UT_EXPORT RangeBearingElevation(const double aLocationNED[3], double& aRange, double& aBearing, double& aElevation);
void UT_EXPORT RangeBearingElevation(const double aOriginLocationWCS[3],
                                     const double aDestinationLocationWCS[3],
                                     double&      aRange,
                                     double&      aBearing,
                                     double&      aElevation);


void UT_EXPORT   LocationNED(const double aOriginLocationWCS[3],
                             const double aDestinationLocationWCS[3],
                             double       aLocationNED[3]);
double UT_EXPORT Elevation(const double aOriginLocationWCS[3], const double& aRange, const double& aAltitude);
void UT_EXPORT   LocationWCS(const double  aOriginLocationWCS[3],
                             const double& aRange,
                             const double& aBearing,
                             const double& aElevation,
                             double        aLocationWCS[3]);

double UT_EXPORT GetQuantitativeMeasurementQuality(double aSimTime, const UtMeasurementData& aMeasurement);

double UT_EXPORT GetQuantitativeMeasurementQuality(double                  aSimTime,
                                                   double                  aMeasurementUpdateTime,
                                                   const UtMatrix<double>& aCovar,
                                                   double                  aSpeed       = 0.0,
                                                   bool                    aNormalizeIt = true);

double UT_EXPORT GetExpectedLocationError(double aMeasurementQuality);

double UT_EXPORT GroundRange(const double aPoint1LocWCS[3], const double aPoint2LocWCS[3]);

double UT_EXPORT SlantRange(const double aPoint1LocWCS[3], const double aPoint2LocWCS[3]);
} // namespace UtMeasurementUtil

#endif
