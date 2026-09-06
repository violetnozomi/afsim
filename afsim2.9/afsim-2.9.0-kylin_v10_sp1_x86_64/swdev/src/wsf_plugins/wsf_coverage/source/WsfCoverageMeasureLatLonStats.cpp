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

#include "WsfCoverageMeasureLatLonStats.hpp"

#include <map>
#include <utility>

#include "WsfCoverage.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageGrid.hpp"
#include "WsfCoverageGridPoint.hpp"
#include "WsfCoverageMeasure.hpp"

namespace wsf
{
namespace coverage
{

MeasureLatLonStats::MeasureLatLonStats(const Coverage& aCoverage,
                                       const Measure&  aMeasure,
                                       Projection      aProjection,
                                       double          aMinimumBinSize)
   : mProjection{aProjection}
   , mMinimumBinSize{aMinimumBinSize}
{
   // Select projector function
   ProjectorFunction projector{nullptr};
   if (mProjection == Projection::cLATITUDE)
   {
      projector = &ProjectLatitude;
   }
   else
   {
      projector = &ProjectLongitude;
   }

   // Accumulate into scratch space
   std::map<int, BinData> accumulator{};
   auto                   actionPtr = CreateGridAction(
      [this, &accumulator, projector, &aMeasure](const Grid& aGrid, const Point& aPoint)
      {
         double value     = aMeasure.GetMeasuredValue(aGrid.GetAsset(aPoint.GetID()));
         double proj      = projector(aPoint);
         int    projIndex = BinIndexFromValue(proj);
         double binValue  = BinValueFromIndex(projIndex);

         BinData datum{binValue, value};

         auto iter = accumulator.find(projIndex);
         if (iter == accumulator.end())
         {
            accumulator.emplace(std::make_pair(projIndex, datum));
         }
         else
         {
            iter->second.Accumulate(datum);
         }
      });
   aCoverage.GetGrid()->Invoke(*actionPtr);

   // Form mean from accumulated value and add it to data vector.
   for (auto& iter : accumulator)
   {
      iter.second.Finalize();
      mData.emplace_back(iter.second);
   }
}

std::string MeasureLatLonStats::GetProjectionDescription() const
{
   std::string retval;
   if (mProjection == Projection::cLATITUDE)
   {
      retval = "Latitude";
   }
   else
   {
      retval = "Longitude";
   }
   return retval;
}

int MeasureLatLonStats::BinIndexFromValue(double aValue) const
{
   return static_cast<int>((aValue + (mMinimumBinSize * 0.5)) / mMinimumBinSize);
}

double MeasureLatLonStats::BinValueFromIndex(int aIndex) const
{
   return aIndex * mMinimumBinSize;
}

double MeasureLatLonStats::ProjectLatitude(const Point& aPoint)
{
   return aPoint.GetLatitude();
}

double MeasureLatLonStats::ProjectLongitude(const Point& aPoint)
{
   return aPoint.GetLongitude();
}

void MeasureLatLonStats::BinData::Accumulate(const BinData& aOther)
{
   mMean += aOther.mCount * aOther.mMean;
   if (aOther.mMaximum > mMaximum)
   {
      mMaximum = aOther.mMaximum;
   }
   if (aOther.mMinimum < mMinimum)
   {
      mMinimum = aOther.mMinimum;
   }
   mCount += aOther.mCount;
}

void MeasureLatLonStats::BinData::Finalize()
{
   if (mCount > 0)
   {
      mMean /= mCount;
   }
}

} // namespace coverage
} // namespace wsf
