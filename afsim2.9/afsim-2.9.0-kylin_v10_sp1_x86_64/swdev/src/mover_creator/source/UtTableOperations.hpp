// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTTABLEOPERATIONS_HPP
#define UTTABLEOPERATIONS_HPP

#include <map>
#include <QPoint>

namespace Designer
{
   namespace util
   {
      enum class ExtrapolationMethod
      {
         cCLAMP,
         cZERO,
         cEXTRAPOLATE
      };

      // Calculate slope between two points
      double CalcSlope(double aValueMin,
                       double aValueMax,
                       double aRangeMin,
                       double aRangeMax);

      // Find the value corresponding to aKey in aMap, using interpolation/extrapolation
      double LookupValue(const std::map<double, double>& aMap,
                         double aKey,
                         ExtrapolationMethod aForwardMethod = ExtrapolationMethod::cZERO,
                         ExtrapolationMethod aReverseMethod = ExtrapolationMethod::cZERO);
   }
}

#endif