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

#include "WsfSimpleCoverage.hpp"

#include "WsfCoverage.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageGrid.hpp"
#include "WsfCoverageGridAction.hpp"
#include "WsfCoverageGridPoint.hpp"

namespace wsf
{
namespace coverage
{

bool SimpleCoverage::InitializeP(Coverage& aCoverage)
{
   mCallbacks.Add(aCoverage.AccessIntervalComplete().Connect(&SimpleCoverage::OnAccessIntervalComplete, this));
   return true;
}

double SimpleCoverage::GetMeasuredValue(const GridAsset& aGridAsset) const
{
   double retval{0.0};
   auto   iter = mData.find(aGridAsset);
   if (iter != mData.end())
   {
      retval = iter->second;
   }
   return retval;
}

std::string SimpleCoverage::GetValueHeader() const
{
   return "Simple Coverage";
}

void SimpleCoverage::OnAccessIntervalComplete(const GridAsset& aGridAsset,
                                              const FreeAsset& /*aFreeAsset*/,
                                              const AccessInterval& /*aInterval*/)
{
   mData[aGridAsset] = 1.0;
}

} // namespace coverage
} // namespace wsf
