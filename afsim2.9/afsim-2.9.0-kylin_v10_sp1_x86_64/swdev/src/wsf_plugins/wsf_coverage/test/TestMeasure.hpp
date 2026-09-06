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

#ifndef WSFCOVERAGETESTTESTMEASURE_HPP
#define WSFCOVERAGETESTTESTMEASURE_HPP

#include <map>

#include "WsfCoverageAccessInterval.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageMeasure.hpp"

class TestMeasure : public wsf::coverage::Measure
{
public:
   explicit TestMeasure(const WsfScenario& aScenario)
      : wsf::coverage::Measure(aScenario)
   {
   }
   TestMeasure(const TestMeasure& aOther) = default;
   ~TestMeasure() override                = default;

   TestMeasure* Clone() const override { return new TestMeasure{*this}; }

   double GetMeasuredValue(const wsf::coverage::GridAsset& aGridAsset) const override;
   double GetDefaultValue() const override { return 0.0; }

   void CollectionStarting(wsf::coverage::Coverage& aCoverage, double aSimTime) override { mStarted = true; }

   std::string GetValueHeader() const override { return "Test Value"; }

   void ClearValues() { mData.clear(); }

   void SetValue(const wsf::coverage::GridAsset& aAsset, double aValue) { mData[aAsset] = aValue; }

   int NumOutputsOfType(const std::string& aOutputType) const;
   int GetNumIntervalStarts(const wsf::coverage::GridAsset& aGridAsset, const wsf::coverage::FreeAsset& aFreeAsset) const;
   int GetNumIntervalEnds(const wsf::coverage::GridAsset& aGridAsset, const wsf::coverage::FreeAsset& aFreeAsset) const;
   int GetNumIntervalCompletions(const wsf::coverage::GridAsset& aGridAsset,
                                 const wsf::coverage::FreeAsset& aFreeAsset) const;

   bool IsStarted() const { return mStarted; }
   bool IsCompleted() const { return mCompleted; }

private:
   bool InitializeP(wsf::coverage::Coverage& aCoverage) override;
   void CollectionCompletingP(wsf::coverage::Coverage& aCoverage, double aSimTime) override { mCompleted = true; }

   void OnIntervalStart(const wsf::coverage::GridAsset& aGridAsset,
                        const wsf::coverage::FreeAsset& aFreeAsset,
                        double                          aSimTime);
   void OnIntervalEnd(const wsf::coverage::GridAsset& aGridAsset, const wsf::coverage::FreeAsset& aFreeAsset, double aSimTime);
   void OnIntervalComplete(const wsf::coverage::GridAsset&      aGridAsset,
                           const wsf::coverage::FreeAsset&      aFreeAsset,
                           const wsf::coverage::AccessInterval& aAccessInterval);

   std::map<wsf::coverage::GridAsset, double> mData{};

   std::map<wsf::coverage::GridAsset, std::map<wsf::coverage::FreeAsset, int>> mIntervalStarts;
   std::map<wsf::coverage::GridAsset, std::map<wsf::coverage::FreeAsset, int>> mIntervalEnds;
   std::map<wsf::coverage::GridAsset, std::map<wsf::coverage::FreeAsset, int>> mIntervalCompletions;

   bool mStarted{false};
   bool mCompleted{false};
};

#endif // WSFCOVERAGETESTTESTMEASURE_HPP
