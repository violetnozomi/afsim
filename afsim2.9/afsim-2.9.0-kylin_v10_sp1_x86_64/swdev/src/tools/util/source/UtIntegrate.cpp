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
#include "UtIntegrate.hpp"

#include <cassert>
#include <iostream>

#include "UtIntegrand.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"


/**
 *  Compute (recursively) the definite integral of a function using the
 *  adaptive Newton-Cotes Quadrature
 *
 *  aTol is a positive real defining the error tolerance.
 *
 *  aNumRecurs is the maximum number of recursive calls allowed, and
 *  defaults to 15. This prevents infinite recursion, which could be
 *  caused by, e.g., too small an error tolerance.
 *
 *  aResetMaxRecurs is a boolean used to distinguish between external
 *  calls and recursive calls, for the purposes of resetting the
 *  maxRecursReached flag. This allows the error message to be printed
 *  at most once per external call.
 */
double UtIntegrate::AdaptQNC(double (UtIntegrand::*fname)(double),
                             UtIntegrand* aIntegrand,
                             double       aA,
                             double       aB,
                             int          aM,
                             double       aTol,
                             int          aNumRecurs,
                             bool         aResetMaxRecurs)
{
   static bool maxRecursReached = false;
   if (aResetMaxRecurs)
   {
      maxRecursReached = false;
   }

   double A1    = CompQNC(fname, aIntegrand, aA, aB, aM, 1);
   double A2    = CompQNC(fname, aIntegrand, aA, aB, aM, 2);
   int    d     = 2 * ((aM - 1) / 2) + 1;
   double error = (A2 - A1) / ((2 << d) - 1);

   if (fabs(error) <= aTol)
   {
      // A2 is acceptable
      return (A2 + error);
   }
   else if (aNumRecurs < 0)
   {
      if (!maxRecursReached) // only want to print message once
      {
         auto out = ut::log::error()
                    << "UtIntegrate: Maximum number of recursive calls to AdaptQNC has been reached (Quitting).";
         out.AddNote() << "Error Magnitude: " << fabs(error);
         out.AddNote() << "Required Tolerance: " << aTol;
         maxRecursReached = true;
      }
      return (A2 + error);
   }
   else
   {
      // Sum left and right subintegrals, recurse
      double mid = (aA + aB) / 2.0;
      return AdaptQNC(fname, aIntegrand, aA, mid, aM, aTol / 2.0, aNumRecurs - 1, false) +
             AdaptQNC(fname, aIntegrand, mid, aB, aM, aTol / 2.0, aNumRecurs - 1, false);
   }
}

double UtIntegrate::AdaptQNC(double (*fname)(double), double aA, double aB, int aM, double aTol, int aNumRecurs, bool aResetMaxRecurs)
{
   static bool maxRecursReached = false;
   if (aResetMaxRecurs)
   {
      maxRecursReached = false;
   }

   double A1    = CompQNC(fname, aA, aB, aM, 1);
   double A2    = CompQNC(fname, aA, aB, aM, 2);
   int    d     = 2 * ((aM - 1) / 2) + 1;
   double error = (A2 - A1) / ((2 << d) - 1);

   if (fabs(error) <= aTol)
   {
      // A2 is acceptable
      return (A2 + error);
   }
   else if (aNumRecurs < 0)
   {
      if (!maxRecursReached) // only want to print message once
      {
         auto out = ut::log::error()
                    << "UtIntegrate: Maximum number of recursive calls to AdaptQNC has been reached (Quitting).";
         out.AddNote() << "Error Magnitude: " << fabs(error);
         out.AddNote() << "Required Tolerance: " << aTol;
         maxRecursReached = true;
      }
      return (A2 + error);
   }
   else
   {
      // Sum left and right subintegrals, recurse
      double mid = (aA + aB) / 2.0;
      return AdaptQNC(fname, aA, mid, aM, aTol / 2.0, aNumRecurs - 1, false) +
             AdaptQNC(fname, mid, aB, aM, aTol / 2.0, aNumRecurs - 1, false);
   }
}

/**
 *  Returns the composite aM-point Newton-Cotes approximation of the
 *  integral of fname from aA to aB with aN equal length subintervals
 */
double UtIntegrate::CompQNC(double (UtIntegrand::*fname)(double), UtIntegrand* aIntegrand, double aA, double aB, int aM, int aN)
{
   assert((aM >= 2) && (aM <= 11));
   double delta  = (aB - aA) / aN;   // length of subinterval
   double h      = delta / (aM - 1); // distance b/w points in subinterval
   double result = 0;

   for (int i = 0; i < aN; ++i) // for each of N subintervals
   {
      // for each of M points in subinterval i
      for (int j = 0; j < aM; ++j)
      {
         double x = aA + (delta * i) + (h * j);
         // multiply weight by f(x) and add to result
         result += NCWeights[aM - 2][j] * (aIntegrand->*fname)(x);
      }
   }

   return (delta * result);
}

double UtIntegrate::CompQNC(double (*fname)(double), double aA, double aB, int aM, int aN)
{
   assert((aM >= 2) && (aM <= 11));
   double delta  = (aB - aA) / aN;   // length of subinterval
   double h      = delta / (aM - 1); // distance b/w points in subinterval
   double result = 0;

   for (int i = 0; i < aN; ++i) // for each of N subintervals
   {
      // for each of M points in subinterval i
      for (int j = 0; j < aM; ++j)
      {
         double x = aA + (delta * i) + (h * j);
         // multiply weight by f(x) and add to result
         result += NCWeights[aM - 2][j] * (*fname)(x);
      }
   }

   return (delta * result);
}

/**
 *  Pass in an m-vector and this function puts in aVec
 *  the weights for the m-point Newton-Cotes quadrature method
 *  The size of the vector must satisy 2 <= m <= 11
 */
/*
void UtIntegrate::NCWeights(vector<double>& aVec)
{
   unsigned int m = aVec.size();
   assert((m >= 2) && (m <= 11));

   switch(m)
   {
      case 2:
         aVec[0] = aVec[1] = 1.0/2;
         break;

      case 3:
         aVec[0] = aVec[2] = 1.0/6;
         aVec[1] = 4.0/6;
         break;

      case 4:
         aVec[0] = aVec[3] = 1.0/8;
         aVec[1] = aVec[2] = 3.0/8;
         break;

      case 5:
         aVec[0] = aVec[4] = 7.0/90;
         aVec[1] = aVec[3] = 32.0/90;
         aVec[2] = 12.0/90;
         break;

      case 6:
         aVec[0] = aVec[5] = 19.0/288;
         aVec[1] = aVec[4] = 75.0/288;
         aVec[2] = aVec[3] = 50.0/288;
         break;

      case 7:
         aVec[0] = aVec[6] = 41.0/840;
         aVec[1] = aVec[5] = 216.0/840;
         aVec[2] = aVec[4] = 27.0/840;
         aVec[3] = 272.0/840;
         break;

      case 8:
         aVec[0] = aVec[7] = 751.0/17280;
         aVec[1] = aVec[6] = 3577.0/17280;
         aVec[2] = aVec[5] = 1323.0/17280;
         aVec[3] = aVec[4] = 2989.0/17280;
         break;

      case 9:
         aVec[0] = aVec[8] = 989.0/28350;
         aVec[1] = aVec[7] = 5888.0/28350;
         aVec[2] = aVec[6] = -928.0/28350;
         aVec[3] = aVec[5] = 10496.0/28350;
         aVec[4] = -4540.0/28350;
         break;

      case 10:
         aVec[0] = aVec[9] = 2857.0/89600;
         aVec[1] = aVec[8] = 15741.0/89600;
         aVec[2] = aVec[7] = 1080.0/89600;
         aVec[3] = aVec[6] = 19344.0/89600;
         aVec[4] = aVec[5] = 5778.0/89600;
         break;

      case 11:
         aVec[0] = aVec[10] = 16067.0/598752;
         aVec[1] = aVec[9] = 106300.0/598752;
         aVec[2] = aVec[8] = -48525.0/598752;
         aVec[3] = aVec[7] = 272400.0/598752;
         aVec[4] = aVec[6] = -260550.0/598752;
         aVec[5] = 427368.0/598752;
         break;
   }
}
*/
