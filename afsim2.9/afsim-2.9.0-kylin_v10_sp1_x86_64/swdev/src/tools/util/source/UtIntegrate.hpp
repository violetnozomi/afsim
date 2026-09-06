// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTINTEGRATE_HPP
#define UTINTEGRATE_HPP

class UtIntegrand;


/**
 *  Newton-Cotes approximation routines for computing the definite
 *  integral of f(x) from a to b. Adapted from "Introduction to
 *  Scientific Computing (2nd ed.)" by C.F. Van Loan. Refer to Chapter 4.
 *
 *  fname is the function pointer to the integrand, which must
 *  take a double argument (the dummy variable) and return a double.
 *  There are two versions each of AdaptQNC and CompQNC. One version
 *  uses a pointer to member function of UtIntegrand to specify the
 *  integrand function. The other uses a static function.
 *
 *  aA and aB specify the endpoints of the interval over which you
 *  are integrating.
 *
 *  aM specifies the number of points in the quadrature rule, and must
 *  satisfy 2 <= aM <= 11
 */

namespace UtIntegrate
{
// Adaptive Newton-Cotes quadrature, a driver for CompQNC.
// Recursively computes the integral using a divide and conquer
// approach, stopping when error tolerance is satisfied. "Adaptive"
// refers to the fact that the subintervals are not necessarily
// uniformly spaced between aA and aB (subintervals are shorter
// where the integrand is ill-behaved for better accuracy).
double AdaptQNC(double (UtIntegrand::*fname)(double),
                UtIntegrand* aIntegrand,
                double       aA,
                double       aB,
                int          aM,
                double       aTol,
                int          aNumRecurs      = 15,
                bool         aResetMaxRecurs = true);

double
AdaptQNC(double (*fname)(double), double aA, double aB, int aM, double aTol, int aNumRecurs = 15, bool aResetMaxRecurs = true);

// Composite Newton-Cotes quadrature. Divide the interval aA to aB
// into aN equal length subintervals, solve each of the subintervals
// using an aM-point Newton-Cotes quadrature rule, and sum the results
double CompQNC(double (UtIntegrand::*fname)(double), UtIntegrand* aIntegrand, double aA, double aB, int aM, int aN);

double CompQNC(double (*fname)(double), double aA, double aB, int aM, int aN);

// row k is the weight vector for a (k+2)-point Newton-Cotes
// quadrature rule
const double NCWeights[10][11] = {
   {1.0 / 2, 1.0 / 2},
   {1.0 / 6, 4.0 / 6, 1.0 / 6},
   {1.0 / 8, 3.0 / 8, 3.0 / 8, 1.0 / 8},
   {7.0 / 90, 32.0 / 90, 12.0 / 90, 32.0 / 90, 7.0 / 90},
   {19.0 / 288, 75.0 / 288, 50.0 / 288, 50.0 / 288, 75.0 / 288, 19.0 / 288},
   {41.0 / 840, 216.0 / 840, 27.0 / 840, 272.0 / 840, 27.0 / 840, 216.0 / 840, 41.0 / 840},
   {751.0 / 17280, 3577.0 / 17280, 1323.0 / 17280, 2989.0 / 17280, 2989.0 / 17280, 1323.0 / 17280, 3577.0 / 17280, 751.0 / 17280},
   {989.0 / 28350,
    5888.0 / 28350,
    -928.0 / 28350,
    10496.0 / 28350,
    -4540.0 / 28350,
    10496.0 / 28350,
    -928.0 / 28350,
    5888.0 / 28350,
    989.0 / 28350},
   {2857.0 / 89600,
    15741.0 / 89600,
    1080.0 / 89600,
    19344.0 / 89600,
    5778.0 / 89600,
    5778.0 / 89600,
    19344.0 / 89600,
    1080.0 / 89600,
    15741.0 / 89600,
    2857.0 / 89600},
   {16067.0 / 598752,
    106300.0 / 598752,
    -48525.0 / 598752,
    272400.0 / 598752,
    -260550.0 / 598752,
    427368.0 / 598752,
    -260550.0 / 598752,
    272400.0 / 598752,
    -48525.0 / 598752,
    106300.0 / 598752,
    16067.0 / 598752}};
} // namespace UtIntegrate

#endif
