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

#include "WsfCoverageMeasureGridStats.hpp"

#include "WsfCoverage.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageGrid.hpp"
#include "WsfCoverageMeasure.hpp"

namespace wsf
{
namespace coverage
{

MeasureGridStats::MeasureGridStats(const Coverage& aCoverage, const Measure& aMeasure)
{
   auto actionPtr = CreateGridAction(
      [this, &aMeasure](const Grid& aGrid, const Point& aPoint)
      {
         double value = aMeasure.GetMeasuredValue(aGrid.GetAsset(aPoint.GetID()));
         if (mMinValue > value)
         {
            mMinValue = value;
         }
         if (mMaxValue < value)
         {
            mMaxValue = value;
         }
         mMeanValue += value;
         ++mCount;
      });
   aCoverage.GetGrid()->Invoke(*actionPtr);
   if (mCount)
   {
      mMeanValue /= mCount;
   }
}

} // namespace coverage
} // namespace wsf
