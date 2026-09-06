// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSOLARTERMINATOR_HPP
#define WSFSOLARTERMINATOR_HPP

#include "wsf_space_export.h"

class UtCalendar;
class WsfPlatform;

//! A set of utilities relating to the Solar terminator.
namespace WsfSolarTerminator
{
// The following definitions are in degrees, and are taken from
// Fundamentals of Astrodynamics and Applications, 4th Ed.
// p. 281.
constexpr double cTWILIGHT_CIVIL        = 96.0;
constexpr double cTWILIGHT_NAUTICAL     = 102.0;
constexpr double cTWILIGHT_ASTRONOMICAL = 108.0;

enum class PhaseOfDay
{
   cDAY      = 1,
   cTWILIGHT = 2,
   cNIGHT    = 3
};

WSF_SPACE_EXPORT PhaseOfDay GetPhaseOfDay(double            aLatDegrees,
                                          double            aLonDegrees,
                                          const UtCalendar& aTime,
                                          double            aLimitDegrees = cTWILIGHT_CIVIL);

WSF_SPACE_EXPORT bool MaskedByHorizon(const double aObserverWCS[3], const double aTargetWCS[3]);

enum class PlatformSolarIllumination
{
   cINVALID_PLATFORM = 0,
   cILLUMINATED      = 1,
   cEARTH_PENUMBRA   = 2,
   cEARTH_UMBRA      = 3
};

WSF_SPACE_EXPORT PlatformSolarIllumination GetPlatformSolarIllumination(WsfPlatform* aPlatformPtr);
} // namespace WsfSolarTerminator

#endif // WSFSOLARTERMINATOR_HPP
