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

#include <cmath>
#include <memory>

#include <gtest/gtest.h>

#include "UtEarth.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalState.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalEvent.hpp"

namespace // {anonymous}
{
class SetSpecificState : public UtOrbitalPropagator
{
public:
   SetSpecificState(const ut::OrbitalState& aState)
      : UtOrbitalPropagator{}
   {
      SetOrbitalState(aState);
   }
};

UtCalendar DecemberSolstice2020()
{
   UtCalendar epoch{};
   epoch.SetDate(2020, 12, 21);
   epoch.SetTime(10, 2, 0.0);
   return epoch;
}

std::unique_ptr<SetSpecificState> CreateTestPropagator()
{
   double           rMag = 8000.0 * 1000.0; // 8000 km
   UtVec3d          pos{rMag, 0.0, 0.0};
   auto             earth = ut::EarthEGM96{};
   double           vMag  = 2.0 * sqrt(earth.GetGravitationalParameter() / rMag); // gives an unbound orbit
   UtVec3d          vel{0.0, vMag, 0.0};
   ut::OrbitalState state{DecemberSolstice2020(),
                          earth,
                          ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                          ut::OrbitalState::ReferenceFrame::cECI,
                          ut::OrbitalStateVector{pos, vel}};
   return ut::make_unique<SetSpecificState>(state);
}

std::unique_ptr<SetSpecificState> CreateEllipticalPropagator(double aMeanAnomaly)
{
   UtOrbitalElements elem{DecemberSolstice2020(),
                          ut::EarthEGM96{},
                          10000000.0, // sma = 10,000 km
                          0.1,
                          aMeanAnomaly,
                          0.0,
                          0.0,
                          0.0};
   ut::OrbitalState state{ut::OrbitalState::CoordinateSystem::cEQUATORIAL, ut::OrbitalState::ReferenceFrame::cECI, elem};
   auto             retvalPtr = ut::make_unique<SetSpecificState>(state);
   retvalPtr->Initialize(state);
   return retvalPtr;
}

std::unique_ptr<SetSpecificState> CreateCircularPropagator(double aMeanAnomaly)
{
   UtOrbitalElements elem{DecemberSolstice2020(),
                          ut::EarthEGM96{},
                          10000000.0, // sma = 10,000 km
                          0.0,
                          aMeanAnomaly,
                          UtMath::cPI_OVER_4,
                          0.0,
                          0.0};
   ut::OrbitalState  state{ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                          ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE,
                          elem};
   return ut::make_unique<SetSpecificState>(state);
}

} // namespace

TEST(WsfSpaceOrbitalEventCondition, TestHyperbolic)
{
   constexpr double cTOLERANCE{1.0e-12};

   auto propPtr = CreateTestPropagator();

   {
      wsf::space::RelativeTimeCondition cond{};
      double                            tc;
      EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
      EXPECT_NEAR(tc, 0.0, cTOLERANCE);
   }

   {
      constexpr double                  cOFFSET_TIME{123.0};
      wsf::space::RelativeTimeCondition cond{};
      cond.SetOffsetTime(cOFFSET_TIME);
      double tc;
      EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
      EXPECT_NEAR(tc, cOFFSET_TIME, cTOLERANCE);

      // Orbit number should be ignored
      cond.SetOrbitNumber(5);
      EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
      EXPECT_NEAR(tc, cOFFSET_TIME, cTOLERANCE);
   }

   {
      wsf::space::PeriapsisCondition cond{};
      double                         tc;
      EXPECT_FALSE(cond.GetTimeToCondition(*propPtr, tc));
   }

   {
      wsf::space::ApoapsisCondition cond{};
      double                        tc;
      EXPECT_FALSE(cond.GetTimeToCondition(*propPtr, tc));
   }

   {
      wsf::space::AscendingNodeCondition cond{};
      double                             tc;
      EXPECT_FALSE(cond.GetTimeToCondition(*propPtr, tc));
   }

   {
      wsf::space::DescendingNodeCondition cond{};
      double                              tc;
      EXPECT_FALSE(cond.GetTimeToCondition(*propPtr, tc));
   }

   {
      wsf::space::AscendingRadiusCondition cond{};
      double                               tc;
      EXPECT_FALSE(cond.GetTimeToCondition(*propPtr, tc));
   }

   {
      wsf::space::DescendingRadiusCondition cond{};
      double                                tc;
      EXPECT_FALSE(cond.GetTimeToCondition(*propPtr, tc));
   }

   {
      wsf::space::NorthernIntersectionCondition cond{};
      double                                    tc;
      EXPECT_FALSE(cond.GetTimeToCondition(*propPtr, tc));
   }

   {
      wsf::space::SouthernIntersectionCondition cond{};
      double                                    tc;
      EXPECT_FALSE(cond.GetTimeToCondition(*propPtr, tc));
   }

   {
      wsf::space::EclipseEntryCondition cond{};
      double                            tc;
      EXPECT_FALSE(cond.GetTimeToCondition(*propPtr, tc));
   }

   {
      wsf::space::EclipseExitCondition cond{};
      double                           tc;
      EXPECT_FALSE(cond.GetTimeToCondition(*propPtr, tc));
   }
}

TEST(WsfSpaceOrbitalEventCondition, TestRelativeTimeCondition)
{
   constexpr double       cTOLERANCE{1.0e-12};
   constexpr double       cTIME_OFFSET{123.4};
   constexpr unsigned int cORBIT_NUMBER{2};
   auto                   propPtr = CreateEllipticalPropagator(0.0);
   double                 period  = propPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();

   wsf::space::RelativeTimeCondition cond{};
   cond.SetOffsetTime(UtTimeValue{cTIME_OFFSET});
   double tc;
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, cTIME_OFFSET, cTOLERANCE);

   cond.SetOrbitNumber(cORBIT_NUMBER);
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, cTIME_OFFSET + cORBIT_NUMBER * period, cTOLERANCE);
}

TEST(WsfSpaceOrbitalEventCondition, TestApoapsisCondition)
{
   constexpr double       cTOLERANCE{1.0e-4};
   constexpr unsigned int cORBIT_NUMBER{4};
   auto                   propPtr = CreateEllipticalPropagator(0.0); // Start at periapsis
   double                 period  = propPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();

   wsf::space::ApoapsisCondition cond{};
   double                        tc;
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period / 2.0, cTOLERANCE);

   cond.SetOrbitNumber(cORBIT_NUMBER);
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period / 2.0 + cORBIT_NUMBER * period, cTOLERANCE);
}

TEST(WsfSpaceOrbitalEventCondition, TestPeriapsisCondition)
{
   constexpr double       cTOLERANCE{1.0e-3};
   constexpr unsigned int cORBIT_NUMBER{4};
   auto                   propPtr = CreateEllipticalPropagator(UtMath::cPI); // Start at apoapsis
   double                 period  = propPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();

   wsf::space::PeriapsisCondition cond{};
   double                         tc;
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period / 2.0, cTOLERANCE);

   cond.SetOrbitNumber(cORBIT_NUMBER);
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period / 2.0 + cORBIT_NUMBER * period, cTOLERANCE);
}

TEST(WsfSpaceOrbitalEventCondition, TestAscendingNodeCondition)
{
   constexpr double       cTOLERANCE{1.0e-4};
   constexpr unsigned int cORBIT_NUMBER{1};
   auto                   propPtr = CreateCircularPropagator(UtMath::cPI); // Start at apoapsis
   double                 period  = propPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();

   wsf::space::AscendingNodeCondition cond{};
   double                             tc;
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period / 2.0, cTOLERANCE);

   cond.SetOrbitNumber(cORBIT_NUMBER);
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period / 2.0 + cORBIT_NUMBER * period, cTOLERANCE);
}

TEST(WsfSpaceOrbitalEventCondition, TestDescendingNodeCondition)
{
   constexpr double       cTOLERANCE{1.0e-3};
   constexpr unsigned int cORBIT_NUMBER{6};
   auto                   propPtr = CreateCircularPropagator(0.0); // Start at periapsis
   double                 period  = propPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();

   wsf::space::DescendingNodeCondition cond{};
   double                              tc;
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period / 2.0, cTOLERANCE);

   cond.SetOrbitNumber(cORBIT_NUMBER);
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period / 2.0 + cORBIT_NUMBER * period, cTOLERANCE);
}

TEST(WsfSpaceOrbitalEventCondition, TestAscendingRadiusCondition)
{
   constexpr double       cTOLERANCE{1.0e-8};
   constexpr unsigned int cORBIT_NUMBER{2};
   constexpr double       cRADIUS{10000000.0};                       // 10,000 km
   auto                   propPtr = CreateEllipticalPropagator(0.0); // Start at periapsis
   double                 sma     = propPtr->GetOrbitalState().GetOrbitalElements().GetSemiMajorAxis();
   double                 ecc     = propPtr->GetOrbitalState().GetOrbitalElements().GetEccentricity();
   double                 period  = propPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();

   // See Fundamentals of Astrodynamics and Applications, Vallado, 4th Ed
   double cosNu = sma * (1.0 - ecc * ecc) / (ecc * cRADIUS) - 1.0 / ecc; // Eq. (1-24) and (1-19)
   double sinNu = std::sqrt(1.0 - cosNu * cosNu);
   double sinE  = sinNu * std::sqrt(1.0 - ecc * ecc) / (1.0 + ecc * cosNu); // Eq. (2-9)
   double E     = std::asin(sinE);
   double M     = E - ecc * sinE; // Eq. (2-4)
   double frac  = M / UtMath::cTWO_PI;

   wsf::space::AscendingRadiusCondition cond{};
   cond.SetRadius(UtLengthValue{cRADIUS});
   double tc;
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period * frac, cTOLERANCE);

   cond.SetOrbitNumber(cORBIT_NUMBER);
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period * frac + cORBIT_NUMBER * period, cTOLERANCE);
}

TEST(WsfSpaceOrbitalEventCondition, TestDescendingRadiusCondition)
{
   constexpr double       cTOLERANCE{1.0e-10};
   constexpr unsigned int cORBIT_NUMBER{2};
   constexpr double       cRADIUS{10000000.0};                       // 10,000 km
   auto                   propPtr = CreateEllipticalPropagator(0.0); // Start at periapsis
   double                 sma     = propPtr->GetOrbitalState().GetOrbitalElements().GetSemiMajorAxis();
   double                 ecc     = propPtr->GetOrbitalState().GetOrbitalElements().GetEccentricity();
   double                 period  = propPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();

   // See Fundamentals of Astrodynamics and Applications, Vallado, 4th Ed
   double cosNu = sma * (1.0 - ecc * ecc) / (ecc * cRADIUS) - 1.0 / ecc; // Eq. (1-24) and (1-19)
   double sinNu = std::sqrt(1.0 - cosNu * cosNu);
   double sinE  = sinNu * std::sqrt(1.0 - ecc * ecc) / (1.0 + ecc * cosNu); // Eq. (2-9)
   double E     = std::asin(sinE);
   double M     = E - ecc * sinE; // Eq. (2-4)
   double frac  = 1.0 - M / UtMath::cTWO_PI;

   wsf::space::DescendingRadiusCondition cond{};
   cond.SetRadius(UtLengthValue{cRADIUS});
   double tc;
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period * frac, cTOLERANCE);

   cond.SetOrbitNumber(cORBIT_NUMBER);
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period * frac + cORBIT_NUMBER * period, cTOLERANCE);
}

UtVec3d OrbitNormal(double aInclination, double aRAAN)
{
   return UtVec3d{std::sin(aInclination) * std::sin(aRAAN),
                  -std::sin(aInclination) * std::cos(aRAAN),
                  std::cos(aInclination)};
}

TEST(WsfSpaceOrbitalEventCondition, TestNorthernIntersectionCondition)
{
   constexpr double       cTOLERANCE{1.0e-4};
   constexpr unsigned int cORBIT_NUMBER{1};
   constexpr double       cNEW_RAAN{UtMath::cPI_OVER_2};
   constexpr double       cNEW_INCLINATION{3.0 * UtMath::cPI / 8.0};
   auto                   propPtr = CreateCircularPropagator(0.0); // Start at ascending node
   double                 incl    = propPtr->GetOrbitalState().GetOrbitalElements().GetInclination();
   double                 raan    = propPtr->GetOrbitalState().GetOrbitalElements().GetRAAN();
   double                 period  = propPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();

   // compute normals before and after
   auto n1 = OrbitNormal(incl, raan);
   auto n2 = OrbitNormal(cNEW_INCLINATION, cNEW_RAAN);

   // The cross product of the normals gives the direction to the line of intersection between
   // the two orbital planes.
   UtVec3d l;
   l.CrossProduct(n1, n2).Normalize();

   // The angle between xHat and l gives the mean anomaly of the intersection point (for circular orbits).
   UtVec3d xHat = propPtr->GetOrbitalState().GetOrbitalStateVectorTOD().GetLocation();
   xHat.Normalize();
   double cosM = xHat.DotProduct(l);
   double M    = std::acos(cosM);
   double frac = M / UtMath::cTWO_PI;

   wsf::space::NorthernIntersectionCondition cond{};
   cond.SetIntersectionRAAN_Inclination(cNEW_RAAN, cNEW_INCLINATION);
   double tc;
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period * frac, cTOLERANCE);

   cond.SetOrbitNumber(cORBIT_NUMBER);
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period * frac + cORBIT_NUMBER * period, cTOLERANCE);
}

TEST(WsfSpaceOrbitalEventCondition, TestSouthernIntersectionCondition)
{
   constexpr double       cTOLERANCE{1.0e-3};
   constexpr unsigned int cORBIT_NUMBER{1};
   constexpr double       cNEW_RAAN{UtMath::cPI_OVER_2};
   constexpr double       cNEW_INCLINATION{3.0 * UtMath::cPI / 8.0};
   auto                   propPtr = CreateCircularPropagator(0.0); // Start at ascending node
   double                 incl    = propPtr->GetOrbitalState().GetOrbitalElements().GetInclination();
   double                 raan    = propPtr->GetOrbitalState().GetOrbitalElements().GetRAAN();
   double                 period  = propPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();

   // compute normals before and after
   auto n1 = OrbitNormal(incl, raan);
   auto n2 = OrbitNormal(cNEW_INCLINATION, cNEW_RAAN);

   // The cross product of the normals gives the direction to the line of intersection between
   // the two orbital planes.
   UtVec3d l;
   l.CrossProduct(n2, n1).Normalize();

   // The angle between xHat and l gives the mean anomaly of the intersection point (for circular orbits).
   UtVec3d xHat = propPtr->GetOrbitalState().GetOrbitalStateVectorTOD().GetLocation();
   xHat.Normalize();
   double cosM = xHat.DotProduct(l);
   double M    = std::acos(cosM);
   double frac = 1.0 - M / UtMath::cTWO_PI;

   wsf::space::SouthernIntersectionCondition cond{};
   cond.SetIntersectionRAAN_Inclination(cNEW_RAAN, cNEW_INCLINATION);
   double tc;
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period * frac, cTOLERANCE);

   cond.SetOrbitNumber(cORBIT_NUMBER);
   EXPECT_TRUE(cond.GetTimeToCondition(*propPtr, tc));
   EXPECT_NEAR(tc, period * frac + cORBIT_NUMBER * period, cTOLERANCE);
}
