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

#ifndef WSFCOVERAGENASSETCOVERAGE_HPP
#define WSFCOVERAGENASSETCOVERAGE_HPP

#include "wsf_coverage_export.h"

#include <map>
#include <set>
#include <vector>

#include "WsfCoverageAccessInterval.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageMeasure.hpp"

namespace wsf
{
namespace coverage
{

//! An MOE that measures the number of free assets that a grid point can access at a time.
class WSF_COVERAGE_EXPORT N_AssetCoverage : public Measure
{
public:
   explicit N_AssetCoverage(const WsfScenario& aScenario)
      : Measure{aScenario}
   {
      SetType(cTYPE);
   }
   N_AssetCoverage(const N_AssetCoverage& aOther) = default;
   ~N_AssetCoverage() override                    = default;

   // Measure overrides
   N_AssetCoverage* Clone() const override { return new N_AssetCoverage{*this}; }
   bool             ProcessInput(UtInput& aInput) override;
   double           GetDefaultValue() const override;
   double           GetMeasuredValue(const GridAsset& aGridAsset) const override;
   void             CollectionStarting(Coverage& aCoverage, double aSimTime) override;
   std::string      GetValueHeader() const override;

   constexpr static const char* cTYPE{"WSF_N_ASSET_COVERAGE_MOE"};

   //! The available sub-types for this MOE
   enum class SubType
   {
      cMEAN,
      cMAX,
      cMIN,
      cPERCENT_ABOVE,
      cUNIQUE,
      cUNKNOWN
   };

   //! Returns the MoE's subtype.
   SubType GetSubType() const { return mSubType; }

   //! Returns the parameter passed to percent_above, or -100.0 if a different subtype was used.
   double GetParameter() const { return mParameter * 100.0; }

private:
   struct GridData
   {
      // The set of all free assets that this grid point has accessed.
      std::set<FreeAsset> mAssetCount;

      // The ordered map of times at which the number of accesses increases or decreases.
      // When iterated over, the number of accesses and the times between changes can be accumulated.
      std::map<double, int> mCountFrames;

      // The grid point's output.
      double mData;
   };
   bool   InitializeP(Coverage& aCoverage) override;
   void   CollectionCompletingP(Coverage& aCoverage, double aSimTime) override;
   double ComputeValue(const GridData& aIntervals) const;
   double ComputeMean(const std::map<double, int>& aFrames) const;
   double ComputeMax(const std::map<double, int>& aFrames) const;
   double ComputeMin(const std::map<double, int>& aFrames) const;
   double ComputePercentAbove(const std::map<double, int>& aFrames) const;
   double ComputeUnique(const std::set<FreeAsset>& aAssets) const;

   void OnAccessIntervalComplete(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval);

   static SubType GetSubtypeFromIdentifier(const std::string& aIdentifier);
   static bool    SubtypeHasParameter(SubType aSubType);

   //! The coverage intervals.
   std::map<GridAsset, GridData> mData{};

   //! The coverage interval.
   AccessInterval mCoverageInterval{};

   //! The selected sub-type.
   SubType mSubType{SubType::cUNKNOWN};

   //! The parameter passed to percent_above.
   double mParameter{-1.0};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEN_ASSET_COVERAGE_HPP
