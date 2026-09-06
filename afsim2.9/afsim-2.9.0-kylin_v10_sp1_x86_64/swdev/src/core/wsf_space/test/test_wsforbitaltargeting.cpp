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
#include <memory>

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalState.hpp"
#include "UtTwoLineElement.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalTargeting.hpp"
#include "WsfPropagatorTargetPoint.hpp"

UtOrbitalPropagator ObjectFromKinematics(const UtVec3d& aLocationECI, const UtVec3d& aVelocityECI, const UtCalendar& aEpoch)
{
   UtOrbitalPropagator retval{};
   ut::OrbitalState    state(ut::EarthWGS84(),
                          ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                          ut::OrbitalState::ReferenceFrame::cECI);
   state.Set(aEpoch, ut::OrbitalState::Vector(aLocationECI, aVelocityECI));
   retval.Initialize(state);
   return retval;
}

UtVec3d LocationForAngle(double aRadius, double aTheta)
{
   return UtVec3d(aRadius * cos(aTheta), aRadius * sin(aTheta), 0.0);
}

UtVec3d VelocityForAngle(double aSpeed, double aTheta)
{
   return UtVec3d(-aSpeed * sin(aTheta), aSpeed * cos(aTheta), 0.0);
}

UtOrbitalPropagator
ObjectAtAngle(double aThetaDegrees, const UtCalendar& aEpoch, double aRadius = 8000000.0, double aSpeed = 8000.0)
{
   double thetaRadians = aThetaDegrees * UtMath::cRAD_PER_DEG;
   return ObjectFromKinematics(LocationForAngle(aRadius, thetaRadians), VelocityForAngle(aSpeed, thetaRadians), aEpoch);
}

void TestVectorEqualTolerance(const UtVec3d& aFirst, const UtVec3d& aSecond, double aTolerance, const std::string& aMessage)
{
   for (int i = 0; i < 3; ++i)
   {
      EXPECT_NEAR(aFirst[i], aSecond[i], aTolerance) << aMessage << " in component " << i;
   }
}

void TestObjectsIntersectionAtTime(UtOrbitalPropagator& aFirst,
                                   UtOrbitalPropagator& aSecond,
                                   const UtCalendar&    aTestTime,
                                   const std::string&   aMessage)
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
   TestVectorEqualTolerance(firstLoc, secondLoc, relVelMag, locationMessage);
}

void TestObjectsRelativeVelocity(UtOrbitalPropagator& aFirst,
                                 UtOrbitalPropagator& aSecond,
                                 const UtCalendar&    aTestTime,
                                 const UtVec3d&       aRelativeVelocity,
                                 const std::string&   aMessage)
{
   aFirst.Update(aTestTime);
   aSecond.Update(aTestTime);

   UtVec3d firstVel                 = aFirst.GetOrbitalState().GetOrbitalStateVector().GetVelocity();
   UtVec3d secondVel                = aSecond.GetOrbitalState().GetOrbitalStateVector().GetVelocity();
   UtVec3d measuredRelativeVelocity = firstVel - secondVel;

   // 1 cm/s tolerance!
   double tolerance = 0.01;

   TestVectorEqualTolerance(measuredRelativeVelocity, aRelativeVelocity, tolerance, aMessage);
}

void TestSimpleTargeting(bool aDoRendezvous, bool aMinimizeDeltaV)
{
   UtCalendar startTime{};
   startTime.SetDate(2018, 3, 20);
   startTime.SetTime(0.0);

   // Set up two objects
   UtOrbitalPropagator               primary   = ObjectAtAngle(0.0, startTime);
   UtOrbitalPropagator               secondary = ObjectAtAngle(30.0, startTime);
   wsf::space::PropagatorTargetPoint target{ut::clone(secondary)};

   // Do the optimization problem
   UtVec3d             location = primary.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d             velocity = primary.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
   WsfOrbitalTargeting targ{startTime, primary, target};

   double                  maxDeltaV{5000.0}; // 5 km/s
   double                  maxDeltaT{7200.0}; // 2 hrs
   double                  solutionDeltaV{-1.0};
   double                  solutionDeltaT{-1.0};
   bool                    solutionFound{false};
   static constexpr double cTIME_TOLERANCE = 1.0E-4;
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
   UtOrbitalPropagator interceptor = ObjectFromKinematics(location, velocity + deltaVIntercept, startTime);

   TestObjectsIntersectionAtTime(primary, interceptor, startTime, "Primary v Interceptor intersection failure in");

   UtCalendar finalTime{startTime};
   finalTime.AdvanceTimeBy(solutionDeltaT);
   TestObjectsIntersectionAtTime(interceptor, secondary, finalTime, "Interceptor v Secondary intersection failure in");

   if (aDoRendezvous)
   {
      TestObjectsRelativeVelocity(secondary,
                                  interceptor,
                                  finalTime,
                                  deltaVRendezvous,
                                  "Interceptor v Secondary relative velocity failure");
   }
}

TEST(WsfOrbitalTargeting, SimpleInterceptDV)
{
   TestSimpleTargeting(false, true);
}

TEST(WsfOrbitalTargeting, SimpleRendezvousDV)
{
   TestSimpleTargeting(true, true);
}

TEST(WsfOrbitalTargeting, SimpleInterceptDT)
{
   TestSimpleTargeting(false, false);
}

TEST(WsfOrbitalTargeting, SimpleRendezvousDT)
{
   TestSimpleTargeting(true, false);
}

TEST(WsfOrbitalTargeting, IdenticalOrbits)
{
   UtCalendar startTime{};
   startTime.SetDate(2018, 3, 20);
   startTime.SetTime(0.0);

   UtOrbitalPropagator               primary   = ObjectAtAngle(0.0, startTime);
   UtOrbitalPropagator               secondary = ObjectAtAngle(0.0, startTime);
   wsf::space::PropagatorTargetPoint target{ut::clone(secondary)};

   WsfOrbitalTargeting targ{startTime, primary, target};

   // Loop over a range of deltaT to test the solution. We limit to 120 degrees of orbit because eventually the
   // solution will be retrograde and this test will no longer work.
   double maxDeltaT = 120.0 * UtMath::cRAD_PER_DEG / primary.GetInitialOrbitalState().GetOrbitalElements().GetMeanMotion();
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

struct HohmannSolution
{
   double mV_Initial;
   double mV_Final;
   double mNuInitial;
   double mDeltaT;
   double mDeltaV;
};

HohmannSolution ComputeHohmannValues(double aR_Initial, double aR_Final)
{
   HohmannSolution retval{};

   double mu         = UtEarth::cGRAVITATIONAL_PARAMETER_WGS84;
   retval.mV_Initial = sqrt(mu / aR_Initial);
   retval.mV_Final   = sqrt(mu / aR_Final);

   double aTrans = (aR_Final + aR_Initial) / 2.0;
   double v1     = sqrt(2.0 * mu / aR_Initial - mu / aTrans);
   double v2     = sqrt(2.0 * mu / aR_Final - mu / aTrans);

   retval.mDeltaT    = UtMath::cPI * sqrt(aTrans * aTrans * aTrans / mu);
   retval.mNuInitial = UtMath::cPI * UtMath::cDEG_PER_RAD * (1.0 - pow(aTrans / aR_Final, 3.0 / 2.0));
   retval.mDeltaV    = (v1 - retval.mV_Initial) + (retval.mV_Final - v2);

   return retval;
}

TEST(WsfOrbitalTargeting, HohmannTest)
{
   UtCalendar startTime{};
   startTime.SetDate(2018, 3, 20);
   startTime.SetTime(0.0);

   double          innerRadius{8000000.0};
   double          outerRadius{12000000.0};
   HohmannSolution transfer = ComputeHohmannValues(innerRadius, outerRadius);

   // NOTE: These radii and speeds were computed externally to give us the right orientation.
   // TODO: Upgrade this to being the result of a computation given the input radii.
   UtOrbitalPropagator primary   = ObjectAtAngle(0.0, startTime, innerRadius, transfer.mV_Initial);
   UtOrbitalPropagator secondary = ObjectAtAngle(transfer.mNuInitial, startTime, outerRadius, transfer.mV_Final);
   wsf::space::PropagatorTargetPoint target{ut::clone(secondary)};

   WsfOrbitalTargeting targ{startTime, primary, target};

   // Do the min delta V rendezvous - should give us the Hohmann
   double                  solutionDeltaT{-1.0};
   double                  solutionDeltaV{-1.0};
   static constexpr double cTIME_TOLERANCE = 1.0E-6;
   bool                    solutionFound =
      targ
         .MinimizeDeltaV(transfer.mDeltaT * 2.0, transfer.mDeltaV * 2.0, true, cTIME_TOLERANCE, solutionDeltaT, solutionDeltaV)
         .IsSolution();
   EXPECT_TRUE(solutionFound);
   EXPECT_NEAR(transfer.mDeltaT, solutionDeltaT, 0.4); // TODO MJM revisit why this exceeded 0.1 s
   EXPECT_NEAR(transfer.mDeltaV, solutionDeltaV, 0.1);
}
