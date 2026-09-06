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

#ifndef WSFCOVERAGEACCESSDURATION_HPP
#define WSFCOVERAGEACCESSDURATION_HPP

#include "wsf_coverage_export.h"

#include <map>
#include <vector>

#include "WsfCoverageAccessInterval.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageMeasure.hpp"

namespace wsf
{
namespace coverage
{

//! An MOE that measures the durations of individual free asset accesses in coverage.
class WSF_COVERAGE_EXPORT AccessDuration : public Measure
{
public:
   explicit AccessDuration(const WsfScenario& aScenario)
      : Measure{aScenario}
   {
      SetType(cTYPE);
   }
   AccessDuration(const AccessDuration& aOther) = default;
   ~AccessDuration() override                   = default;

   // Measure overrides
   AccessDuration* Clone() const override { return new AccessDuration{*this}; }
   bool            ProcessInput(UtInput& aInput) override;
   double          GetDefaultValue() const override;
   double          GetMeasuredValue(const GridAsset& aGridAsset) const override;
   void            CollectionStarting(Coverage& aCoverage, double aSimTime) override;
   std::string     GetValueHeader() const override;

   constexpr static const char* cTYPE{"WSF_ACCESS_DURATION_MOE"};

   //! The available sub-types for this MOE
   enum class SubType
   {
      cMEAN,
      cMIN,
      cMAX,
      cPERCENT_ABOVE,
      cSTDDEV,
      cSUM,
      cUNKNOWN
   };

   //! Get the MoE's subtype enum.
   SubType GetSubType() const { return mSubType; }

   //! Get the parameter passed if the subtype is percent_above, or -100.0 otherwise.
   double GetParameter() const { return mParameter * 100.0; }

private:
   bool   InitializeP(Coverage& aCoverage) override;
   void   CollectionCompletingP(Coverage& aCoverage, double aSimTime) override;
   double ComputeValue(std::vector<AccessInterval>& aIntervals) const;
   double ComputeMean(std::vector<AccessInterval>& aIntervals) const;
   double ComputeMin(std::vector<AccessInterval>& aIntervals) const;
   double ComputeMax(std::vector<AccessInterval>& aIntervals) const;
   double ComputePercentAbove(std::vector<AccessInterval>& aIntervals) const;
   double ComputeStddev(std::vector<AccessInterval>& aIntervals) const;
   double ComputeSum(std::vector<AccessInterval>& aIntervals) const;

   void SortIntervalsByDuration(std::vector<AccessInterval>& aIntervals) const;

   void OnAccessIntervalComplete(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval);

   static SubType GetSubtypeFromIdentifier(const std::string& aIdentifier);
   static bool    SubtypeHasParameter(SubType aSubType);

   //! The coverage intervals.
   mutable std::map<GridAsset, std::vector<AccessInterval>> mIntervals{};

   //! The processed data. The value has a different meaning based on the sub-type selected.
   std::map<GridAsset, double> mData{};

   //! The coverage interval.
   AccessInterval mCoverageInterval{};

   //! The selected sub-type.
   SubType mSubType{SubType::cUNKNOWN};

   //! The parameter for subtypes that need it (for this MoE, only Percent Above)
   double mParameter{-1.0};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEACCESS_DURATION_HPP
