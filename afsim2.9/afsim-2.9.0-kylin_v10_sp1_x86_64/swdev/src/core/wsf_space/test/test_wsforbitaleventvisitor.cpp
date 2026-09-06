// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "SpaceTestService.hpp"
#include "UtLocator.hpp"
#include "UtMemory.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfOrbitalScriptEvent.hpp"
#include "WsfOrbitalStagingEvent.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeEccentricity.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeInclination.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeRAAN.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeRAAN_Inclination.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeSemiMajorAxis.hpp"
#include "maneuvers/WsfOrbitalManeuversCircularize.hpp"
#include "maneuvers/WsfOrbitalManeuversCompound.hpp"
#include "maneuvers/WsfOrbitalManeuversHohmannTransfer.hpp"
#include "maneuvers/WsfOrbitalManeuversIntercept.hpp"
#include "maneuvers/WsfOrbitalManeuversMatchVelocity.hpp"
#include "maneuvers/WsfOrbitalManeuversNormal.hpp"
#include "maneuvers/WsfOrbitalManeuversRendezvous.hpp"
#include "maneuvers/WsfOrbitalManeuversTangent.hpp"
#include "maneuvers/WsfOrbitalManeuversTarget.hpp"

class SimpleVisitor : public WsfOrbitalEventVisitor
{
public:
   void VisitChangeEccentricity(WsfOrbitalManeuvers::ChangeEccentricity* aEventPtr) override { ++mEccentricityCount; }
   void VisitChangeInclination(WsfOrbitalManeuvers::ChangeInclination* aEventPtr) override { ++mInclinationCount; }
   void VisitChangeRAAN(WsfOrbitalManeuvers::ChangeRAAN* aEventPtr) override { ++mRAAN_Count; }
   void VisitChangeRAAN_Inclination(WsfOrbitalManeuvers::ChangeRAAN_Inclination* aEventPtr) override
   {
      ++mRAAN_InclinationCount;
   }
   void VisitChangeSemiMajorAxis(WsfOrbitalManeuvers::ChangeSemiMajorAxis* aEventPtr) override
   {
      ++mSemiMajorAxisCount;
   }
   void VisitCircularize(WsfOrbitalManeuvers::Circularize* aEventPtr) override { ++mCircularizeCount; }
   void VisitCompoundManeuver(WsfOrbitalManeuvers::CompoundManeuver* aEventPtr) override { ++mCompoundCount; }
   void VisitHohmannTransfer(WsfOrbitalManeuvers::HohmannTransfer* aEventPtr) override { ++mHohmannCount; }
   void VisitIntercept(WsfOrbitalManeuvers::Intercept* aEventPtr) override { ++mInterceptCount; }
   void VisitMatchVelocity(WsfOrbitalManeuvers::MatchVelocity* aEventPtr) override { ++mMatchVelocityCount; }
   void VisitNormal(WsfOrbitalManeuvers::Normal* aEventPtr) override { ++mNormalCount; }
   void VisitRendezvous(WsfOrbitalManeuvers::Rendezvous* aEventPtr) override { ++mRendezvousCount; }
   void VisitTangent(WsfOrbitalManeuvers::Tangent* aEventPtr) override { ++mTangentCount; }
   void VisitTarget(WsfOrbitalManeuvers::Target* aEventPtr) override { ++mTargetCount; }
   void VisitOrbitalScriptEvent(WsfOrbitalScriptEvent* aEventPtr) override { ++mScriptCount; }
   void VisitOrbitalStagingEvent(WsfOrbitalStagingEvent* aEventPtr) override { ++mStagingCount; }

   int GetEccentricityCount() const { return mEccentricityCount; }
   int GetInclinationCount() const { return mInclinationCount; }
   int GetRAAN_Count() const { return mRAAN_Count; }
   int GetRAAN_InclinationCount() const { return mRAAN_InclinationCount; }
   int GetSemiMajorAxisCount() const { return mSemiMajorAxisCount; }
   int GetCircularizeCount() const { return mCircularizeCount; }
   int GetCompoundCount() const { return mCompoundCount; }
   int GetHohmannCount() const { return mHohmannCount; }
   int GetInterceptCount() const { return mInterceptCount; }
   int GetMatchVelocityCount() const { return mMatchVelocityCount; }
   int GetNormalCount() const { return mNormalCount; }
   int GetRendezvousCount() const { return mRendezvousCount; }
   int GetTargetCount() const { return mTargetCount; }
   int GetTangentCount() const { return mTangentCount; }
   int GetScriptCount() const { return mScriptCount; }
   int GetStagingCount() const { return mStagingCount; }

private:
   int mEccentricityCount     = 0;
   int mInclinationCount      = 0;
   int mRAAN_Count            = 0;
   int mRAAN_InclinationCount = 0;
   int mSemiMajorAxisCount    = 0;
   int mCircularizeCount      = 0;
   int mCompoundCount         = 0;
   int mHohmannCount          = 0;
   int mInterceptCount        = 0;
   int mMatchVelocityCount    = 0;
   int mNormalCount           = 0;
   int mRendezvousCount       = 0;
   int mTargetCount           = 0;
   int mTangentCount          = 0;
   int mScriptCount           = 0;
   int mStagingCount          = 0;
};

std::vector<std::unique_ptr<WsfOrbitalEvent>> CreateTestEvents(const WsfScenario& aScenario)
{
   std::vector<std::unique_ptr<WsfOrbitalEvent>> retval{};
   retval.emplace_back(ut::make_unique<WsfOrbitalManeuvers::Normal>(aScenario, 120.0));
   retval.emplace_back(ut::make_unique<WsfOrbitalManeuvers::Circularize>(aScenario));
   retval.emplace_back(ut::make_unique<WsfOrbitalManeuvers::ChangeEccentricity>(aScenario, 0.9));
   retval.emplace_back(ut::make_unique<WsfOrbitalManeuvers::ChangeInclination>(aScenario, 0.0));
   retval.emplace_back(ut::make_unique<WsfOrbitalManeuvers::ChangeRAAN>(aScenario, 0.0));
   retval.emplace_back(ut::make_unique<WsfOrbitalManeuvers::ChangeRAAN_Inclination>(aScenario, 0.0, 0.0));
   retval.emplace_back(ut::make_unique<WsfOrbitalManeuvers::ChangeSemiMajorAxis>(aScenario, 1.0e6));
   retval.emplace_back(ut::make_unique<WsfOrbitalManeuvers::Tangent>(aScenario, 10.0));
   retval.emplace_back(ut::make_unique<WsfOrbitalStagingEvent>(aScenario));

   return retval;
}

TEST(WsfOrbitalEventVisitor, BasicVisitor)
{
   auto& testDataService = ut::Locator<SpaceTestService>::get();
   auto  events          = CreateTestEvents(testDataService.GetScenario());

   SimpleVisitor simple{};

   for (auto& elem : events)
   {
      elem->Accept(&simple);
   }

   EXPECT_EQ(simple.GetEccentricityCount(), 1);
   EXPECT_EQ(simple.GetInclinationCount(), 1);
   EXPECT_EQ(simple.GetRAAN_Count(), 1);
   EXPECT_EQ(simple.GetRAAN_InclinationCount(), 1);
   EXPECT_EQ(simple.GetSemiMajorAxisCount(), 1);
   EXPECT_EQ(simple.GetCircularizeCount(), 1);
   EXPECT_EQ(simple.GetCompoundCount(), 0);
   EXPECT_EQ(simple.GetHohmannCount(), 0);
   EXPECT_EQ(simple.GetInterceptCount(), 0);
   EXPECT_EQ(simple.GetMatchVelocityCount(), 0);
   EXPECT_EQ(simple.GetNormalCount(), 1);
   EXPECT_EQ(simple.GetRendezvousCount(), 0);
   EXPECT_EQ(simple.GetTargetCount(), 0);
   EXPECT_EQ(simple.GetTangentCount(), 1);
   EXPECT_EQ(simple.GetScriptCount(), 0);
   EXPECT_EQ(simple.GetStagingCount(), 1);
}
