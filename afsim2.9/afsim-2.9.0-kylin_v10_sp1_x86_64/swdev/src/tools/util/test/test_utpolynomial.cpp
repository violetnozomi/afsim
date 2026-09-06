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
#include <math.h>

#include <gtest/gtest.h>

#include "UtPolynomial.hpp"

double Quartic(double aValue, double aCoeffs[5])
{
   double  x        = aValue;
   double* c        = aCoeffs;
   double  x2       = x * x;
   double  x3       = x2 * x;
   double  x4       = x3 * x;
   double  solution = c[0] + c[1] * x + c[2] * x2 + c[3] * x3 + c[4] * x4;
   return solution;
}

TEST(UtPolynomial, SolveQuartic)
{
   // -x^4 + 3x^2 + 2x + 1
   // (two roots)
   double c1[5] = {1.0, 2.0, 3.0, 0.0, -1.0};
   double s[4];
   int    num = UtPolynomial::SolveQuartic(c1, s);
   EXPECT_TRUE(num == 2);
   static const double cTOL = 1.0e-13;

   EXPECT_TRUE(fabs(Quartic(s[0], c1)) < cTOL);
   EXPECT_TRUE(fabs(Quartic(s[1], c1)) < cTOL);

   // (no roots)
   double c2[5] = {5.0, 2.0, 5.0, -3.0, 2.0};
   num          = UtPolynomial::SolveQuartic(c2, s);
   EXPECT_TRUE(num == 0);

   // (four)
   double c3[5] = {1, 5.0, -4.0, -12.0, 9.0};
   num          = UtPolynomial::SolveQuartic(c3, s);
   EXPECT_TRUE(num == 4);
   EXPECT_TRUE(fabs(Quartic(s[0], c3)) < cTOL);
   EXPECT_TRUE(fabs(Quartic(s[1], c3)) < cTOL);
   EXPECT_TRUE(fabs(Quartic(s[2], c3)) < cTOL);
   EXPECT_TRUE(fabs(Quartic(s[3], c3)) < cTOL);

   // (bi-quadratic special case, four roots)
   double c4[5] = {4.0, 0.0, -5.0, 0.0, 1.0};
   num          = UtPolynomial::SolveQuartic(c4, s);
   EXPECT_TRUE(num == 4);
   EXPECT_TRUE(fabs(Quartic(s[0], c4)) < cTOL);
   EXPECT_TRUE(fabs(Quartic(s[1], c4)) < cTOL);
   EXPECT_TRUE(fabs(Quartic(s[2], c4)) < cTOL);
   EXPECT_TRUE(fabs(Quartic(s[3], c4)) < cTOL);

   // (bi-quadratic special case, two roots)
   double c5[5] = {1, 2.0, 3.0 / 8.0, 1.0, 1.0};
   num          = UtPolynomial::SolveQuartic(c5, s);
   EXPECT_TRUE(num == 2);
   EXPECT_TRUE(fabs(Quartic(s[0], c5)) < cTOL);
   EXPECT_TRUE(fabs(Quartic(s[1], c5)) < cTOL);

   // (bi-quadratic, no roots)
   double c6[5] = {4.0, 0.0, 5.0, 0.0, 1.0};
   num          = UtPolynomial::SolveQuartic(c6, s);
   EXPECT_TRUE(num == 0);

   // (no constant term in reduced quartic; special case)
   double c7[5] = {3.0, 1.0, 1.0, 4.0, 1.0};
   num          = UtPolynomial::SolveQuartic(c7, s);
   EXPECT_TRUE(num == 2);
   EXPECT_TRUE(fabs(Quartic(s[0], c7)) < cTOL);
   EXPECT_TRUE(fabs(Quartic(s[1], c7)) < cTOL);

   // simple case
   double c8[5] = {0.0, 0.0, 0.0, 0.0, 19.0};
   num          = UtPolynomial::SolveQuartic(c8, s);
   EXPECT_TRUE(num == 1);
   EXPECT_TRUE(fabs(Quartic(s[0], c8)) < cTOL);

   // simple case
   double c9[5] = {0.0, 0.0, 1.0, 0.0, 1.0};
   num          = UtPolynomial::SolveQuartic(c9, s);
   EXPECT_TRUE(num == 1);
   EXPECT_TRUE(fabs(Quartic(s[0], c9)) < cTOL);
}
