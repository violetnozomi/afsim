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

#include <gtest/gtest.h>

#include "UtEarth.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagator.hpp"

namespace
{
double longitude(const UtVec3d& locationWCS)
{
   double lat, lon, alt;
   ut::EarthWGS84().ConvertWCS_ToLLA(locationWCS, lat, lon, alt);
   return lon;
}
} // namespace

TEST(UtOrbitalPropagator, UpdateAndReset)
{
   // Make a GEO orbit for easy WCS comparisons.
   const UtOrbitalElements initialElements(ut::EarthWGS84(), 42164000.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   const ut::OrbitalState  initialState(ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                       ut::OrbitalState::ReferenceFrame::cECI,
                                       initialElements);

   UtOrbitalPropagator propagator;
   propagator.SetInitialOrbitalState(initialState);

   const UtCalendar firstTime  = UtCalendar::ParseISO8601("2020-06-01T12:00:00.000Z");
   const UtCalendar secondTime = UtCalendar::ParseISO8601("2020-06-03T12:00:00.000Z");
   const UtCalendar thirdTime  = UtCalendar::ParseISO8601("2020-06-05T09:00:00.000Z");

   propagator.Initialize(firstTime);

   propagator.Update(secondTime);
   const UtVec3d secondLocationWCS = propagator.GetOrbitalState().GetOrbitalStateVectorWCS().GetLocation();
   const double  secondLon         = longitude(secondLocationWCS);
   propagator.Update(thirdTime);
   const UtVec3d thirdLocationWCS = propagator.GetOrbitalState().GetOrbitalStateVectorWCS().GetLocation();
   const double  thirdLon         = longitude(thirdLocationWCS);
   propagator.Update(secondTime);
   const UtVec3d secondLocationWCSAfterUpdate = propagator.GetOrbitalState().GetOrbitalStateVectorWCS().GetLocation();
   const double  secondLonAfterUpdate         = longitude(secondLocationWCSAfterUpdate);

   ASSERT_EQ(secondLon, secondLonAfterUpdate);
   ASSERT_EQ(secondLocationWCS, secondLocationWCSAfterUpdate);
}
