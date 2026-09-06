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

#ifndef WSFCOVERAGESIMPLECOVERAGE_HPP
#define WSFCOVERAGESIMPLECOVERAGE_HPP

#include "wsf_coverage_export.h"

#include <map>

#include "WsfCoverageAsset.hpp"
#include "WsfCoverageGridPoint.hpp"
#include "WsfCoverageMeasure.hpp"

namespace wsf
{
namespace coverage
{

class AccessInterval;

//! An MOE that measures if a grid asset has had any interaction with a free asset.
class WSF_COVERAGE_EXPORT SimpleCoverage : public Measure
{
public:
   explicit SimpleCoverage(const WsfScenario& aScenario)
      : Measure{aScenario}
   {
      SetType(cTYPE);
   }
   SimpleCoverage(const SimpleCoverage& aOther) = default;
   ~SimpleCoverage() override                   = default;

   // Measure overrides
   SimpleCoverage* Clone() const override { return new SimpleCoverage{*this}; }
   double          GetMeasuredValue(const GridAsset& aGridAsset) const override;
   double          GetDefaultValue() const override { return 0.0; }
   std::string     GetValueHeader() const override;

   constexpr static const char* cTYPE{"WSF_SIMPLE_COVERAGE_MOE"};

private:
   bool InitializeP(Coverage& aCoverage) override;

   void OnAccessIntervalComplete(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval);

   std::map<GridAsset, double> mData{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGESIMPLECOVERAGE_HPP
