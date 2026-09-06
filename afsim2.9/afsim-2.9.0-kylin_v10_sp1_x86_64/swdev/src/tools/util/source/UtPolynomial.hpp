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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTPOLYNOMIAL_HPP
#define UTPOLYNOMIAL_HPP

#include "ut_export.h"

//! Contains methods to solve polynomials up to degree 4.
class UT_EXPORT UtPolynomial
{
public:
   static int  SolveLinear(const double aCoefficients[2], double aSolutions[1]);
   static int  SolveQuadric(const double aCoefficients[3], double aSolutions[2]);
   static bool SolveQuadricMinPos(const double aCoefficients[3], double& aResult);
   static int  SolveCubic(const double aCoefficients[4], double aSolutions[3]);
   static int  SolveQuartic(const double aCoefficients[5], double aSolutions[4]);
};

#endif
