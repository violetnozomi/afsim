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

#include <array>
#include <tuple>

#include <gtest/gtest.h>

#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfIntercept.hpp"

class WsfInterceptTest : public ::testing::TestWithParam<std::tuple<double, double, double, double, bool>>
{
public:
   void SetUp() override
   {
      aBody.SetLocationLLA(0.0, 0.0, 0.0);
      aBody.SetOrientationNED(90.0 * UtMath::cRAD_PER_DEG, 0.0, 0.0);
   }

protected:
   UtEntity aBody;
};

TEST_P(WsfInterceptTest, Intercept)
{
   double aBodySpeed       = std::get<0>(GetParam());
   double aTgtRange        = std::get<1>(GetParam());
   double aTgtSpeed        = std::get<2>(GetParam());
   double aTgtHeading      = std::get<3>(GetParam());
   bool   aExpectIntercept = std::get<4>(GetParam());

   std::array<double, 3> bodyVelNED = {0.0, aBodySpeed, 0.0};
   aBody.SetVelocityNED(bodyVelNED.data());

   // The target is in the same plane as the body
   std::array<double, 3> tgtLocNED = {0.0, aTgtRange, 0.0};
   std::array<double, 3> tgtLocWCS;
   aBody.ConvertNEDToWCS(tgtLocNED.data(), tgtLocWCS.data());
   UtEntity tgt;
   tgt.SetLocationWCS(tgtLocWCS.data());
   std::array<double, 3> tgtVelNED = {aTgtSpeed * cos(aTgtHeading * UtMath::cRAD_PER_DEG),
                                      aTgtSpeed * sin(aTgtHeading * UtMath::cRAD_PER_DEG),
                                      0.0};

   // The first form keeps the target velocity vector in the same plane as the body
   // The second form puts it in the body local frame.
   // The first form is useful for looking at the results in reference to a 'sheet of paper'.
#if 1
   std::array<double, 3> tgtVelWCS;
   aBody.ConvertNEDVectorToWCS(tgtVelWCS.data(), tgtVelNED.data());
   tgt.SetVelocityWCS(tgtVelWCS.data());
   tgt.GetVelocityNED(tgtVelNED.data());
#else
   tgt.SetVelocityNED(tgtVelNED.data());
   std::array<double, 3> tgtVelWCS;
   tgt.GetVelocityWCS(tgtVelWCS.data());
#endif

   aBody.GetVelocityNED(bodyVelNED.data());

   std::array<double, 3> bodyLocWCS;
   aBody.GetLocationWCS(bodyLocWCS.data());
   std::array<double, 3> intLocWCS;
   double                intTime =
      WsfIntercept::Intercept(bodyLocWCS.data(), aBodySpeed, tgtLocWCS.data(), tgtVelWCS.data(), intLocWCS.data());
   std::array<double, 3> intLocNED;
   aBody.ConvertWCSToNED(intLocWCS.data(), intLocNED.data());

   if (intTime >= 0.0)
   {
      // Make sure the returned location and time are consistent (i.e. - that each object
      // reaches the projected intercept point at the correct time.

      std::array<double, 3> bodyToIntWCS;
      UtVec3d::Subtract(bodyToIntWCS.data(), intLocWCS.data(), bodyLocWCS.data());
      double bodyToIntTime = UtVec3d::Magnitude(bodyToIntWCS.data()) / aBodySpeed;

      std::array<double, 3> tgtToIntWCS;
      UtVec3d::Subtract(tgtToIntWCS.data(), intLocWCS.data(), tgtLocWCS.data());
      double tgtToIntTime = UtVec3d::Magnitude(tgtToIntWCS.data()) / aTgtSpeed;

      EXPECT_TRUE(aExpectIntercept);

      static const double cTIME_TOLERANCE = 1.0E-3;
      EXPECT_NEAR(bodyToIntTime, intTime, cTIME_TOLERANCE);
      EXPECT_NEAR(tgtToIntTime, intTime, cTIME_TOLERANCE);
   }
   else
   {
      EXPECT_FALSE(aExpectIntercept);
      EXPECT_TRUE(UtVec3d::Equals(intLocWCS.data(), tgtLocWCS.data()));
   }
}

INSTANTIATE_TEST_CASE_P(PureTail,
                        WsfInterceptTest,
                        ::testing::ValuesIn(std::array<std::tuple<double, double, double, double, bool>, 3>{
                           std::make_tuple(200.0, 20000.0, 150.0, 90.0, true),
                           std::make_tuple(200.0, 20000.0, 200.0, 90.0, false),
                           std::make_tuple(200.0, 20000.0, 250.0, 90.0, false)}));

INSTANTIATE_TEST_CASE_P(PureHeadOn,
                        WsfInterceptTest,
                        ::testing::ValuesIn(std::array<std::tuple<double, double, double, double, bool>, 3>{
                           std::make_tuple(200.0, 20000.0, 150.0, -90.0, true),
                           std::make_tuple(200.0, 20000.0, 200.0, -90.0, true),
                           std::make_tuple(200.0, 20000.0, 250.0, -90.0, true)}));

INSTANTIATE_TEST_CASE_P(PureRightAngle,
                        WsfInterceptTest,
                        ::testing::ValuesIn(std::array<std::tuple<double, double, double, double, bool>, 2>{
                           std::make_tuple(200.0, 20000.0, 100.0, 0.0, true),
                           std::make_tuple(200.0, 20000.0, 100.0, 180.0, true)}));

INSTANTIATE_TEST_CASE_P(Front,
                        WsfInterceptTest,
                        ::testing::ValuesIn(std::array<std::tuple<double, double, double, double, bool>, 2>{
                           std::make_tuple(200.0, 20000.0, 100.0, 315.0, true),
                           std::make_tuple(200.0, 20000.0, 100.0, 225.0, true)}));

INSTANTIATE_TEST_CASE_P(Rear,
                        WsfInterceptTest,
                        ::testing::ValuesIn(std::array<std::tuple<double, double, double, double, bool>, 2>{
                           std::make_tuple(200.0, 20000.0, 100.0, 45.0, true),
                           std::make_tuple(200.0, 20000.0, 100.0, 135.0, true)}));
