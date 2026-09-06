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

#include "UtInput.hpp"
#include "Utils.hpp"
#include "WsfOrbitalTargetingCost.hpp"
#include "WsfOrbitalTargetingCostTypes.hpp"

// Test that instances can be created for the built-in type, and that other
// type names do not return a non-null object.
TEST(OrbitalTargetingCostTypes, CreateInstance)
{
   wsf::space::OrbitalTargetingCostTypes types{};

   auto costPtr = types.CreateInstance("blended");
   EXPECT_TRUE(costPtr);
   auto blendedCostPtr = dynamic_cast<wsf::space::BlendedCost*>(costPtr.get());
   EXPECT_TRUE(blendedCostPtr);

   auto failedCostPtr = types.CreateInstance("not_a_type");
   EXPECT_FALSE(failedCostPtr);
}

// A cost to use during testing.
class TestCost : public wsf::space::OrbitalTargetingCost
{
public:
   TestCost()                       = default;
   TestCost(const TestCost& aOther) = default;

   static constexpr const char* cTYPE_NAME = "test_cost";

   TestCost* Clone() const override { return new TestCost{*this}; }

   bool ProcessInput(UtInput& aInput) override
   {
      aInput.ReadValue(mLambda);
      aInput.ValueInClosedRange(mLambda, 0.0, 1.0);
      return true;
   }

   double operator()(double aDeltaT, double aDeltaV) const override
   {
      return mLambda * aDeltaT + (1.0 - mLambda) * aDeltaV;
   }

   const char* GetScriptClassName() const override { return "TestCost"; }

   bool IsLeastTime() const override { return mLambda == 1.0; }

   bool IsValid() const override { return true; }

   std::string GetConfigurationRequirements() const override { return std::string{}; }

   double GetLambda() const { return mLambda; }

private:
   double mLambda{0.5};
};

// To be sure we have the ProcessInput working as it should for later tests,
// we test TestCost.
TEST(TestCost, Functionality)
{
   static constexpr double cDBL_TOLERANCE{1.0e-12};

   TestCost cost{};
   EXPECT_NEAR(cost.GetLambda(), 0.5, cDBL_TOLERANCE);

   TestBadValueThrow(cost, "-1.0");
   TestBadValueThrow(cost, "10.0");

   static constexpr double cLAMBDA{0.75};
   static constexpr double cONE_MINUS_LAMBDA{1.0 - cLAMBDA};

   UtInput input{};
   input.PushInputString("0.75");
   EXPECT_TRUE(cost.ProcessInput(input));

   for (double dt = 0.0; dt < 1.0; dt += 0.1)
   {
      for (double dv = 0.0; dv < 1.0; dv += 0.1)
      {
         EXPECT_NEAR(cost(dt, dv), cLAMBDA * dt + cONE_MINUS_LAMBDA * dv, cDBL_TOLERANCE);
      }
   }
}

// A factory function that will produce TestCost intances.
std::unique_ptr<wsf::space::OrbitalTargetingCost> TestCostFactory(const std::string& aTypeName)
{
   if (aTypeName == TestCost::cTYPE_NAME)
   {
      return ut::make_unique<TestCost>();
   }
   return nullptr;
}

// This assures that the TestCostFactory works as intended.
TEST(TestCostFactory, Functionality)
{
   auto costPtr = TestCostFactory(TestCost::cTYPE_NAME);
   EXPECT_TRUE(costPtr);

   auto testCostPtr = dynamic_cast<TestCost*>(costPtr.get());
   EXPECT_TRUE(testCostPtr);

   auto failedCostPtr = TestCostFactory("anything_else");
   EXPECT_FALSE(failedCostPtr);
}

// Test the addition of the new object factory.
TEST(OrbitalTargetingCostTypes, AddObjectFactory)
{
   wsf::space::OrbitalTargetingCostTypes types{};

   // First, assure that the type is not constructable initially.
   auto failedCostPtr = types.CreateInstance(TestCost::cTYPE_NAME);
   EXPECT_FALSE(failedCostPtr);

   types.AddObjectFactory(TestCostFactory);

   // Second, assure that the type can be constructed after addition to the factory.
   auto costPtr = types.CreateInstance(TestCost::cTYPE_NAME);
   EXPECT_TRUE(costPtr);
   auto testCostPtr = dynamic_cast<TestCost*>(costPtr.get());
   EXPECT_TRUE(testCostPtr);
}

// Test the full loading of instances, which both creates and uses the cost's
// ProcessInput to setup the type.
TEST(OrbitalTargetingCostTypes, LoadInstance)
{
   wsf::space::OrbitalTargetingCostTypes types{};
   types.AddObjectFactory(TestCostFactory);

   UtInput input{};
   input.PushInputString("optimize_cost blended 1.0 2.0 3.0\n"
                         "optimize_cost test_cost 0.33\n"
                         "optimize_cost not_a_cost not_an_argument");

   std::unique_ptr<wsf::space::OrbitalTargetingCost> costPtr;

   EXPECT_TRUE(types.LoadInstance(input, costPtr));
   EXPECT_TRUE(dynamic_cast<wsf::space::BlendedCost*>(costPtr.get()));

   std::string command;
   EXPECT_TRUE(input.ReadCommand(command));
   EXPECT_TRUE(types.LoadInstance(input, costPtr));
   EXPECT_TRUE(dynamic_cast<TestCost*>(costPtr.get()));

   EXPECT_TRUE(input.ReadCommand(command));
   EXPECT_THROW(types.LoadInstance(input, costPtr), UtInput::BadValue);
}
