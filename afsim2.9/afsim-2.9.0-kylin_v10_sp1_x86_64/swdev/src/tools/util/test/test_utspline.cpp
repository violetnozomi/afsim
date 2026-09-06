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

#include <gtest/gtest.h>

#include "UtSpline.hpp"

template<unsigned int N>
constexpr double Monomial(double aTime)
{
   return Monomial<N - 1>(aTime) * aTime;
}

template<>
constexpr double Monomial<0>(double aTime)
{
   return 1.0;
}

template<unsigned int N>
constexpr double MonomialPrime(double aTime)
{
   return Monomial<N - 1>(aTime) * N;
}

template<>
constexpr double MonomialPrime<0>(double aTime)
{
   return 0.0;
}

template<unsigned int N>
constexpr double MonomialDoublePrime(double aTime)
{
   return Monomial<N - 2>(aTime) * N * (N - 1);
}

template<>
constexpr double MonomialDoublePrime<0>(double aTime)
{
   return 0.0;
}

template<>
constexpr double MonomialDoublePrime<1u>(double aTime)
{
   return 0.0;
}

TEST(UtSpline, TwoPointCubic)
{
   constexpr double cSTART     = 0.0;
   constexpr double cEND       = 1.0;
   constexpr double cTESTTIME  = 1.0 / 1.6180339887;
   constexpr double cTOLERANCE = 1.0e-14;

   // Should reproduce constants.
   auto constant = UtCubicSpline::TwoPoint(cSTART,
                                           Monomial<0>(cSTART),
                                           MonomialPrime<0>(cSTART),
                                           cEND,
                                           Monomial<0>(cEND),
                                           MonomialPrime<0>(cEND));
   EXPECT_NEAR(constant(cTESTTIME), Monomial<0>(cTESTTIME), cTOLERANCE);

   // Should reproduce linear
   auto linear = UtCubicSpline::TwoPoint(cSTART,
                                         Monomial<1>(cSTART),
                                         MonomialPrime<1>(cSTART),
                                         cEND,
                                         Monomial<1>(cEND),
                                         MonomialPrime<1>(cEND));
   EXPECT_NEAR(linear(cTESTTIME), Monomial<1>(cTESTTIME), cTOLERANCE);

   // Should reproduce quadratic
   auto quad = UtCubicSpline::TwoPoint(cSTART,
                                       Monomial<2>(cSTART),
                                       MonomialPrime<2>(cSTART),
                                       cEND,
                                       Monomial<2>(cEND),
                                       MonomialPrime<2>(cEND));
   EXPECT_NEAR(quad(cTESTTIME), Monomial<2>(cTESTTIME), cTOLERANCE);

   // Should reproduce cubic
   auto cubic = UtCubicSpline::TwoPoint(cSTART,
                                        Monomial<3>(cSTART),
                                        MonomialPrime<3>(cSTART),
                                        cEND,
                                        Monomial<3>(cEND),
                                        MonomialPrime<3>(cEND));
   EXPECT_NEAR(cubic(cTESTTIME), Monomial<3>(cTESTTIME), cTOLERANCE);

   // Should NOT reproduce quartic
   auto quartic = UtCubicSpline::TwoPoint(cSTART,
                                          Monomial<4>(cSTART),
                                          MonomialPrime<4>(cSTART),
                                          cEND,
                                          Monomial<4>(cEND),
                                          MonomialPrime<4>(cEND));
   EXPECT_FALSE(fabs(quartic(cTESTTIME) - Monomial<4>(cTESTTIME)) < cTOLERANCE);
}

TEST(UtSpline, FourPointCubic)
{
   constexpr double cT0        = -1.0;
   constexpr double cT1        = 0.0;
   constexpr double cT2        = 1.0;
   constexpr double cT3        = 2.0;
   constexpr double cTESTTIME  = 1.6180339887;
   constexpr double cTOLERANCE = 1.0e-14;

   // Should reproduce a constant
   auto constant =
      UtCubicSpline::FourPoint(cT0, Monomial<0>(cT0), cT1, Monomial<0>(cT1), cT2, Monomial<0>(cT2), cT3, Monomial<0>(cT3));
   EXPECT_NEAR(constant(cTESTTIME), Monomial<0>(cTESTTIME), cTOLERANCE);

   // Should reproduce a linear
   auto linear =
      UtCubicSpline::FourPoint(cT0, Monomial<1>(cT0), cT1, Monomial<1>(cT1), cT2, Monomial<1>(cT2), cT3, Monomial<1>(cT3));
   EXPECT_NEAR(linear(cTESTTIME), Monomial<1>(cTESTTIME), cTOLERANCE);

   // Should reproduce a quadratic
   auto quad =
      UtCubicSpline::FourPoint(cT0, Monomial<2>(cT0), cT1, Monomial<2>(cT1), cT2, Monomial<2>(cT2), cT3, Monomial<2>(cT3));
   EXPECT_NEAR(quad(cTESTTIME), Monomial<2>(cTESTTIME), cTOLERANCE);

   // Should reproduce a cubic
   auto cubic =
      UtCubicSpline::FourPoint(cT0, Monomial<3>(cT0), cT1, Monomial<3>(cT1), cT2, Monomial<3>(cT2), cT3, Monomial<3>(cT3));
   EXPECT_NEAR(cubic(cTESTTIME), Monomial<3>(cTESTTIME), cTOLERANCE);

   // Should NOT reproduce a quartic
   auto quartic =
      UtCubicSpline::FourPoint(cT0, Monomial<4>(cT0), cT1, Monomial<4>(cT1), cT2, Monomial<4>(cT2), cT3, Monomial<4>(cT3));
   EXPECT_FALSE(fabs(quartic(cTESTTIME) - Monomial<4>(cTESTTIME)) < cTOLERANCE);
}

TEST(UtSpline, TwoPointQuintic)
{
   constexpr double cSTART     = 0.0;
   constexpr double cEND       = 1.0;
   constexpr double cTESTTIME  = 1.0 / 1.6180339887;
   constexpr double cTOLERANCE = 1.0e-14;

   // Should reproduce a constant
   auto constant = UtQuinticSpline::TwoPoint(cSTART,
                                             Monomial<0>(cSTART),
                                             MonomialPrime<0>(cSTART),
                                             MonomialDoublePrime<0>(cSTART),
                                             cEND,
                                             Monomial<0>(cEND),
                                             MonomialPrime<0>(cEND),
                                             MonomialDoublePrime<0>(cEND));
   EXPECT_NEAR(constant(cTESTTIME), Monomial<0>(cTESTTIME), cTOLERANCE);

   // Should reproduce a linear
   auto linear = UtQuinticSpline::TwoPoint(cSTART,
                                           Monomial<1>(cSTART),
                                           MonomialPrime<1>(cSTART),
                                           MonomialDoublePrime<1>(cSTART),
                                           cEND,
                                           Monomial<1>(cEND),
                                           MonomialPrime<1>(cEND),
                                           MonomialDoublePrime<1>(cEND));
   EXPECT_NEAR(linear(cTESTTIME), Monomial<1>(cTESTTIME), cTOLERANCE);

   // Should reproduce a quadratic
   auto quad = UtQuinticSpline::TwoPoint(cSTART,
                                         Monomial<2>(cSTART),
                                         MonomialPrime<2>(cSTART),
                                         MonomialDoublePrime<2>(cSTART),
                                         cEND,
                                         Monomial<2>(cEND),
                                         MonomialPrime<2>(cEND),
                                         MonomialDoublePrime<2>(cEND));
   EXPECT_NEAR(quad(cTESTTIME), Monomial<2>(cTESTTIME), cTOLERANCE);

   // Should reproduce a cubic
   auto cubic = UtQuinticSpline::TwoPoint(cSTART,
                                          Monomial<3>(cSTART),
                                          MonomialPrime<3>(cSTART),
                                          MonomialDoublePrime<3>(cSTART),
                                          cEND,
                                          Monomial<3>(cEND),
                                          MonomialPrime<3>(cEND),
                                          MonomialDoublePrime<3>(cEND));
   EXPECT_NEAR(cubic(cTESTTIME), Monomial<3>(cTESTTIME), cTOLERANCE);

   // Should reproduce a quartic
   auto quartic = UtQuinticSpline::TwoPoint(cSTART,
                                            Monomial<4>(cSTART),
                                            MonomialPrime<4>(cSTART),
                                            MonomialDoublePrime<4>(cSTART),
                                            cEND,
                                            Monomial<4>(cEND),
                                            MonomialPrime<4>(cEND),
                                            MonomialDoublePrime<4>(cEND));
   EXPECT_NEAR(quartic(cTESTTIME), Monomial<4>(cTESTTIME), cTOLERANCE);

   // Should reproduce a quintic
   auto quintic = UtQuinticSpline::TwoPoint(cSTART,
                                            Monomial<5>(cSTART),
                                            MonomialPrime<5>(cSTART),
                                            MonomialDoublePrime<5>(cSTART),
                                            cEND,
                                            Monomial<5>(cEND),
                                            MonomialPrime<5>(cEND),
                                            MonomialDoublePrime<5>(cEND));
   EXPECT_NEAR(quintic(cTESTTIME), Monomial<5>(cTESTTIME), cTOLERANCE);

   // Should NOT reproduce a sextic
   auto sextic = UtQuinticSpline::TwoPoint(cSTART,
                                           Monomial<6>(cSTART),
                                           MonomialPrime<6>(cSTART),
                                           MonomialDoublePrime<6>(cSTART),
                                           cEND,
                                           Monomial<6>(cEND),
                                           MonomialPrime<6>(cEND),
                                           MonomialDoublePrime<6>(cEND));
   EXPECT_FALSE(fabs(sextic(cTESTTIME) - Monomial<6>(cTESTTIME)) < cTOLERANCE);
}
