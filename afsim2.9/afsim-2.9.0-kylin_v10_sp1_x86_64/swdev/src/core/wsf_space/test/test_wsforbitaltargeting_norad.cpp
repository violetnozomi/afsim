// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <cmath>
#include <utility>

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtTwoLineElement.hpp"
#include "UtVec3.hpp"
#include "WsfNORAD_OrbitalPropagator.hpp"
#include "WsfOrbitalTargeting.hpp"
#include "WsfPropagatorTargetPoint.hpp"

WsfNORAD_OrbitalPropagator NORAD_ObjectFromKinematics(const UtVec3d&    aLocationECI,
                                                      const UtVec3d&    aVelocityECI,
                                                      const UtCalendar& aEpoch)
{
   WsfNORAD_OrbitalPropagator retval{};
   ut::OrbitalState           state(ut::EarthWGS84(),
                          ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                          ut::OrbitalState::ReferenceFrame::cECI);
   state.Set(aEpoch, ut::OrbitalState::Vector(aLocationECI, aVelocityECI));
   retval.Initialize(state);
   return retval;
}

UtVec3d NORAD_LocationForAngle(double aRadius, double aTheta)
{
   return UtVec3d(aRadius * cos(aTheta), aRadius * sin(aTheta), 0.0);
}

UtVec3d NORAD_VelocityForAngleAndInclination(double aSpeed, double aTheta, double aInclination)
{
   return UtVec3d(-aSpeed * sin(aTheta) * cos(aInclination),
                  aSpeed * cos(aTheta) * cos(aInclination),
                  aSpeed * sin(aInclination));
}

WsfNORAD_OrbitalPropagator
NORAD_ObjectAtAngle(double aThetaDegrees, const UtCalendar& aEpoch, double aRadius = 8000000.0, double aSpeed = 8000.0)
{
   double thetaRadians = aThetaDegrees * UtMath::cRAD_PER_DEG;
   double inclination  = 10.0 * UtMath::cRAD_PER_DEG;
   return NORAD_ObjectFromKinematics(NORAD_LocationForAngle(aRadius, thetaRadians),
                                     NORAD_VelocityForAngleAndInclination(aSpeed, thetaRadians, inclination),
                                     aEpoch);
}

void NORAD_TestVectorEqualTolerance(const UtVec3d& aFirst, const UtVec3d& aSecond, double aTolerance, const std::string& aMessage)
{
   for (int i = 0; i < 3; ++i)
   {
      EXPECT_NEAR(aFirst[i], aSecond[i], aTolerance) << aMessage << " in component " << i;
   }
}

void NORAD_TestObjectsIntersectionAtTime(WsfNORAD_OrbitalPropagator& aFirst,
                                         WsfNORAD_OrbitalPropagator& aSecond,
                                         const UtCalendar&           aTestTime,
                                         const std::string&          aMessage)
{
   aFirst.Update(aTestTime);
   aSecond.Update(aTestTime);

   UtVec3d firstLoc  = aFirst.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d secondLoc = aSecond.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d firstVel  = aFirst.GetOrbitalState().GetOrbitalStateVector().GetVelocity();
   UtVec3d secondVel = aSecond.GetOrbitalState().GetOrbitalStateVector().GetVelocity();

   UtVec3d relVel    = firstVel - secondVel;
   double  relVelMag = relVel.Magnitude();

   // Use a 1 second at the relative velocity tolerance (see WsfOrbitalTargetingSearch tolerance)
   std::string locationMessage = aMessage + std::string(" location");
   NORAD_TestVectorEqualTolerance(firstLoc, secondLoc, relVelMag, locationMessage);
}

void NORAD_TestObjectsRelativeVelocity(WsfNORAD_OrbitalPropagator& aFirst,
                                       WsfNORAD_OrbitalPropagator& aSecond,
                                       const UtCalendar&           aTestTime,
                                       const UtVec3d&              aRelativeVelocity,
                                       const std::string&          aMessage)
{
   aFirst.Update(aTestTime);
   aSecond.Update(aTestTime);

   UtVec3d firstVel                 = aFirst.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
   UtVec3d secondVel                = aSecond.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
   UtVec3d measuredRelativeVelocity = firstVel - secondVel;

   // 1 cm/s tolerance!
   double tolerance = 0.01;

   NORAD_TestVectorEqualTolerance(measuredRelativeVelocity, aRelativeVelocity, tolerance, aMessage);
}

void NORAD_TestSimpleTargeting(bool aDoRendezvous, bool aMinimizeDeltaV)
{
   UtCalendar startTime{};
   startTime.SetDate(2018, 3, 20);
   startTime.SetTime(0.0);

   // Set up two objects
   WsfNORAD_OrbitalPropagator primary = NORAD_ObjectAtAngle(0.0, startTime);
   primary.Initialize(startTime);
   WsfNORAD_OrbitalPropagator secondary = NORAD_ObjectAtAngle(30.0, startTime);
   secondary.Initialize(startTime);
   auto targetPropPtr = ut::clone(secondary);
   targetPropPtr->Initialize(startTime);
   wsf::space::PropagatorTargetPoint target{std::move(targetPropPtr)};

   // Do the optimization problem
   UtVec3d             location = primary.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d             velocity = primary.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
   WsfOrbitalTargeting targ{startTime, primary, target};

   double                  maxDeltaV{5000.0};  // 5 km/s
   double                  maxDeltaT{10800.0}; // 3 hrs
   double                  solutionDeltaV{-1.0};
   double                  solutionDeltaT{-1.0};
   static constexpr double cTIME_TOLERANCE = 1.0E-4;
   bool                    solutionFound{false};
   if (aMinimizeDeltaV)
   {
      solutionFound =
         targ.MinimizeDeltaV(maxDeltaT, maxDeltaV, aDoRendezvous, cTIME_TOLERANCE, solutionDeltaT, solutionDeltaV).IsSolution();
   }
   else
   {
      solutionFound =
         targ.MinimizeDeltaT(maxDeltaT, maxDeltaV, aDoRendezvous, cTIME_TOLERANCE, solutionDeltaT, solutionDeltaV).IsSolution();
   }
   EXPECT_TRUE(solutionFound);
   EXPECT_LE(solutionDeltaT, maxDeltaT);
   EXPECT_LE(solutionDeltaV, maxDeltaV);

   // Now solve for the actual delta V values
   UtVec3d deltaVIntercept;
   UtVec3d deltaVRendezvous;
   targ.Solve(solutionDeltaT, deltaVIntercept, deltaVRendezvous);
   double deltaV_Magnitude = deltaVIntercept.Magnitude();
   if (aDoRendezvous)
   {
      deltaV_Magnitude += deltaVRendezvous.Magnitude();
   }
   EXPECT_DOUBLE_EQ(deltaV_Magnitude, solutionDeltaV);

   // Create the intercepting object
   WsfNORAD_OrbitalPropagator interceptor = NORAD_ObjectFromKinematics(location, velocity + deltaVIntercept, startTime);
   interceptor.Initialize(startTime);

   NORAD_TestObjectsIntersectionAtTime(primary, interceptor, startTime, "Primary v Interceptor intersection failure in");

   UtCalendar finalTime{startTime};
   finalTime.AdvanceTimeBy(solutionDeltaT);
   NORAD_TestObjectsIntersectionAtTime(interceptor, secondary, finalTime, "Interceptor v Secondary intersection failure in");

   if (aDoRendezvous)
   {
      NORAD_TestObjectsRelativeVelocity(secondary,
                                        interceptor,
                                        finalTime,
                                        deltaVRendezvous,
                                        "Interceptor v Secondary relative velocity failure");
   }
}

TEST(WsfOrbitalTargeting, NORAD_SimpleInterceptDV)
{
   NORAD_TestSimpleTargeting(false, true);
}

TEST(WsfOrbitalTargeting, NORAD_SimpleRendezvousDV)
{
   NORAD_TestSimpleTargeting(true, true);
}

TEST(WsfOrbitalTargeting, NORAD_SimpleInterceptDT)
{
   NORAD_TestSimpleTargeting(false, false);
}

TEST(WsfOrbitalTargeting, NORAD_SimpleRendezvousDT)
{
   NORAD_TestSimpleTargeting(true, false);
}

TEST(WsfOrbitalTargeting, NORAD_IdenticalOrbits)
{
   UtCalendar startTime{};
   startTime.SetDate(2018, 3, 20);
   startTime.SetTime(0.0);

   WsfNORAD_OrbitalPropagator primary = NORAD_ObjectAtAngle(0.0, startTime);
   UtTwoLineElement           tle     = primary.GetOrbitalState().GetTwoLineElement();
   tle.SetEccentricity(0.02); // Set a nonzero eccentricity (zero cases need further study).
   primary.Initialize(
      ut::OrbitalState(ut::OrbitalState::CoordinateSystem::cEQUATORIAL, ut::OrbitalState::ReferenceFrame::cTEME, tle));
   WsfNORAD_OrbitalPropagator secondary = primary;
   secondary.Initialize(startTime);
   auto targetPropPtr = ut::clone(secondary);
   targetPropPtr->Initialize(startTime);
   wsf::space::PropagatorTargetPoint target{std::move(targetPropPtr)};

   WsfOrbitalTargeting targ{startTime, primary, target};

   // Loop over a range of deltaT to test the solution. We limit to 120 degrees of orbit because eventually the
   // solution will be retrograde and this test will no longer work.
   double  maxDeltaT   = 120.0 * UtMath::cRAD_PER_DEG / primary.GetOrbitalState().GetOrbitalElements().GetMeanMotion();
   double  deltaDeltaT = maxDeltaT / 10.0;
   UtVec3d interceptDeltaV{};
   UtVec3d rendezvousDeltaV{};
   for (double deltaT = deltaDeltaT; deltaT < maxDeltaT; deltaT += deltaDeltaT)
   {
      bool solutionFound = targ.Solve(deltaT, interceptDeltaV, rendezvousDeltaV).IsSolution();
      EXPECT_TRUE(solutionFound) << "For deltaT = " << deltaT;
      EXPECT_NEAR(0.0, interceptDeltaV.Magnitude(), 1.0e-3) << "For deltaT = " << deltaT;
      EXPECT_NEAR(0.0, rendezvousDeltaV.Magnitude(), 1.0e-3) << "For deltaT = " << deltaT;
   }
}
