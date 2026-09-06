// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTECLIPSE_HPP
#define UTECLIPSE_HPP

#include "ut_export.h"

class UtCalendar;
class UtOrbitalElements;
#include "UtVec3.hpp"

//! A set of utilities to determine eclipsed states.
namespace UtEclipse
{
//! The eclipsed state returned by the GetEclipsedState method.
//! @note Currently eclipsing is only valid for earth, but this restriction
//! could be overcome by substituting other central object radii.
enum EclipsedState
{
   cECLIPSED_NONE      = 0,
   cECLIPSED_PENUMBRAL = 1,
   cECLIPSED_UMBRAL    = 2
};

UT_EXPORT EclipsedState GetEclipsedState(const UtCalendar& aTime, const UtVec3d& aLocationECI);

UT_EXPORT bool
GetEclipseTimes(const UtCalendar& aTime, const UtOrbitalElements& aElements, double& aTimeToEntry, double& aTimeToExit);

UT_EXPORT bool GetEclipseTimes(const UtOrbitalElements& aElements, double& aTimeToEntry, double& aTimeToExit);

UT_EXPORT bool GetEclipseAnomalies(const UtCalendar&        aTime,
                                   const UtOrbitalElements& aElements,
                                   double&                  aEntryAnomaly,
                                   double&                  aExitAnomaly);

UT_EXPORT bool GetEclipseAnomalies(const UtOrbitalElements& aElements, double& aEntryAnomaly, double& aExitAnomaly);

UT_EXPORT double GetShadowFunction(const UtCalendar& aTime, const UtOrbitalElements& aElements);

UT_EXPORT double GetUmbralFraction(const UtVec3d& aLocationPrimary,
                                   double         aRadiusPrimary,
                                   const UtVec3d& aLocationSecondary,
                                   double         aRadiusSecondary,
                                   const UtVec3d& aViewingLocation);
}; // namespace UtEclipse

#endif
