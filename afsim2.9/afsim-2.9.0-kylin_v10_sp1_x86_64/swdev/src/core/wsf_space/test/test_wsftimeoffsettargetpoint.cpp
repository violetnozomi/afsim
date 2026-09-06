// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagator.hpp"
#include "WsfPropagatorTargetPoint.hpp"
#include "WsfTimeOffsetTargetPoint.hpp"

constexpr double cORBIT_PERIOD{7200.0};

UtCalendar MakeTestEpoch()
{
   UtCalendar retval{};
   retval.SetDate(2021, 1, 20);
   retval.SetTime(12, 0, 0.0);
   return retval;
}

UtOrbitalElements MakeTestElements()
{
   UtOrbitalElements retval{ut::EarthEGM96{}};
   retval.SetEpoch(MakeTestEpoch());
   retval.SetInclination(UtMath::cPI / 2.0);
   retval.SetRAAN(0.0);
   retval.SetArgumentOfPeriapsis(0.0);
   retval.SetEccentricity(0.0);
   retval.SetTrueAnomaly(0.0);
   retval.SetMeanMotion(UtMath::cTWO_PI / cORBIT_PERIOD); // 2 hr orbit
   return retval;
}

std::unique_ptr<UtOrbitalPropagator> MakeTestPropagator()
{
   auto             retvalPtr = ut::make_unique<UtOrbitalPropagator>();
   ut::OrbitalState state{ut::OrbitalState::CoordinateSystem::cECLIPTIC,
                          ut::OrbitalState::ReferenceFrame::cECI,
                          MakeTestElements()};
   retvalPtr->SetInitialOrbitalState(state);
   retvalPtr->Initialize(MakeTestEpoch());
   return retvalPtr;
}

std::unique_ptr<wsf::space::OrbitalTargetPoint> MakeTestTargetPoint()
{
   return ut::make_unique<wsf::space::PropagatorTargetPoint>(MakeTestPropagator());
}

std::unique_ptr<wsf::space::TimeOffsetTargetPoint> MakeTimeOffsetTargetPoint(double aTimeOffset)
{
   return ut::make_unique<wsf::space::TimeOffsetTargetPoint>(MakeTestTargetPoint(), MakeTestPropagator(), aTimeOffset);
}

TEST(WsfTimeOffsetTargetPoint, ConstructionThrows)
{
   EXPECT_THROW({ wsf::space::TimeOffsetTargetPoint test(nullptr, nullptr, 0.0); }, std::runtime_error);
   EXPECT_THROW({ wsf::space::TimeOffsetTargetPoint test(MakeTestTargetPoint(), nullptr, 0.0); }, std::runtime_error);
   EXPECT_THROW({ wsf::space::TimeOffsetTargetPoint test(nullptr, MakeTestPropagator(), 0.0); }, std::runtime_error);
   EXPECT_NO_THROW({ wsf::space::TimeOffsetTargetPoint test(MakeTestTargetPoint(), MakeTestPropagator(), 0.0); });
}

// This tests that a time offset of 0 gives the same results as the base target point.
TEST(WsfTimeOffsetTargetPoint, ZeroOffset)
{
   auto base = MakeTestTargetPoint();
   auto tgt  = MakeTimeOffsetTargetPoint(0.0);
   EXPECT_NEAR(tgt->GetTimeOffset(), 0.0, 1.0e-12);

   // loop over one period, and then check that the two points give the same results
   for (double dt = 0.0; dt < cORBIT_PERIOD; dt += cORBIT_PERIOD / 20.0)
   {
      UtCalendar epoch = MakeTestEpoch();
      epoch.AdvanceTimeBy(dt);

      base->Update(epoch);
      tgt->Update(epoch);

      UtVec3d basePos = base->GetPositionECI();
      UtVec3d tgtPos  = tgt->GetPositionECI();
      UtVec3d baseVel = base->GetVelocityECI();
      UtVec3d tgtVel  = tgt->GetVelocityECI();

      for (int i = 0; i < 3; ++i)
      {
         EXPECT_NEAR(tgtPos[i], basePos[i], 1.0e-4);
         EXPECT_NEAR(tgtVel[i], baseVel[i], 1.0e-3);
      }
   }
}

// This tests that a positive time offset correctly lags the base target point.
TEST(WsfTimeOffsetTargetPoint, PositiveOffset)
{
   double delta = cORBIT_PERIOD / 20.0;
   auto   base  = MakeTestTargetPoint();
   auto   tgt   = MakeTimeOffsetTargetPoint(delta);
   EXPECT_NEAR(tgt->GetTimeOffset(), delta, 1.0e-12);

   // loop over one period, advancing, and comparing to the previous
   UtVec3d prevPos = base->GetPositionECI();
   UtVec3d prevVel = base->GetVelocityECI();
   for (double dt = delta; dt < cORBIT_PERIOD; dt += delta)
   {
      UtCalendar epoch = MakeTestEpoch();
      epoch.AdvanceTimeBy(dt);

      base->Update(epoch);
      tgt->Update(epoch);

      UtVec3d pos = tgt->GetPositionECI();
      UtVec3d vel = tgt->GetVelocityECI();

      for (int i = 0; i < 3; ++i)
      {
         EXPECT_NEAR(pos[i], prevPos[i], 1.0e-4);
         EXPECT_NEAR(vel[i], prevVel[i], 1.0e-3);
      }

      prevPos = base->GetPositionECI();
      prevVel = base->GetVelocityECI();
   }
}

// This tests that a positive time offset correctly leads the base target point.
TEST(WsfTimeOffsetTargetPoint, NegativeOffset)
{
   double delta = cORBIT_PERIOD / 20.0;
   auto   base  = MakeTestTargetPoint();
   auto   tgt   = MakeTimeOffsetTargetPoint(-delta);
   EXPECT_NEAR(tgt->GetTimeOffset(), -delta, 1.0e-12);

   // loop over one period, advancing, and comparing to the previous
   tgt->Update(MakeTestEpoch());
   UtVec3d nextPos = tgt->GetPositionECI();
   UtVec3d nextVel = tgt->GetVelocityECI();
   for (double dt = delta; dt < cORBIT_PERIOD; dt += delta)
   {
      UtCalendar epoch = MakeTestEpoch();
      epoch.AdvanceTimeBy(dt);

      base->Update(epoch);
      tgt->Update(epoch);

      UtVec3d pos = base->GetPositionECI();
      UtVec3d vel = base->GetVelocityECI();

      for (int i = 0; i < 3; ++i)
      {
         EXPECT_NEAR(pos[i], nextPos[i], 1.0e-4);
         EXPECT_NEAR(vel[i], nextVel[i], 1.0e-3);
      }

      nextPos = tgt->GetPositionECI();
      nextVel = tgt->GetVelocityECI();
   }
}
