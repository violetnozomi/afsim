// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTGOLDENSECTIONSEARCH_HPP
#define UTGOLDENSECTIONSEARCH_HPP

#include <cmath>
#include <functional>

//! Find the minimum of the given function over the given interval.
//! This routine finds the argument that gives the minimum of @p afunction.
//! The range of arguments from @p aLow to @p aHigh specifies the search
//! interval. Golden Section Search relies on the provided function being
//! unimodal on the given interval. If this assumption is violated,
//! then the result of this routine is undefined.
//!
//! @p aFunction is any callable object acting as a function accepting
//! a double as its single input and returning a double.
//!
//! @note Please note that this does not return the value of @p aFunction
//! at the minimum.
//!
//! @note Please note that for functions varying slowly at the minimum, this
//! routine can fail to reach an exceedingly fine tolerance.
//!
//! See https://en.wikipedia.org/wiki/Golden-section_search for details.
//! @param aFunction  - The callable entity under consideration.
//! @param aLow       - The low end of the interval to search.
//! @param aHigh      - The high end of the interval to search.
//! @param aTolerance - The tolerance for the minimum search.
//! @returns          - The argument at which @p aFunction is minimized.
template<typename Callable>
double UtGoldenSectionSearch(Callable aFunction, double aLow, double aHigh, double aTolerance)
{
   static double invPhi = 2.0 / (sqrt(5.0) + 1.0); // one over the Golden Ratio

   double tA = aLow;
   double tB = aHigh;
   double tC = tB - (tB - tA) * invPhi;
   double tD = tA + (tB - tA) * invPhi;

   double vC = aFunction(tC);
   double vD = aFunction(tD);

   while (fabs(tB - tA) > aTolerance)
   {
      if (vC < vD)
      {
         tB = tD;
         tD = tC;
         vD = vC;
         tC = tB - (tB - tA) * invPhi;
         vC = aFunction(tC);
      }
      else
      {
         tA = tC;
         tC = tD;
         vC = vD;
         tD = tA + (tB - tA) * invPhi;
         vD = aFunction(tD);
      }
   }

   return (tA + tB) / 2.0;
}


#endif // UTGOLDENSECTIONSEARCH_HPP