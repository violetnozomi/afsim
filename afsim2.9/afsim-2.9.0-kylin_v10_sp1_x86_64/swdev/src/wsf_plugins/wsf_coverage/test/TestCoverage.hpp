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

#ifndef WSFCOVERAGETESTTESTCOVERAGE_HPP
#define WSFCOVERAGETESTTESTCOVERAGE_HPP

#include <string>

#include "WsfCoverage.hpp"

class TestCoverage : public wsf::coverage::Coverage
{
public:
   TestCoverage(const WsfScenario& aScenario, const wsf::coverage::Grid* aGridPtr, const std::string& aName)
      : wsf::coverage::Coverage(aScenario)
   {
      SetName(aName);
      SetGrid(aGridPtr);
   }
   TestCoverage(const TestCoverage& aOther) = default;
   ~TestCoverage() override                 = default;

   TestCoverage* Clone() const override { return new TestCoverage{*this}; }

   // Expose ability to start and end intervals by hand
   void InjectIntervalStart(const wsf::coverage::GridAsset& aGridAsset,
                            const wsf::coverage::FreeAsset& aFreeAsset,
                            double                          aSimTime);
   void InjectIntervalEnd(const wsf::coverage::GridAsset& aGridAsset,
                          const wsf::coverage::FreeAsset& aFreeAsset,
                          double                          aSimTime);
   void InjectInterval(const wsf::coverage::GridAsset& aGridAsset,
                       const wsf::coverage::FreeAsset& aFreeAsset,
                       double                          aStartTime,
                       double                          aEndTime);

private:
   bool InitializeP(WsfSimulation& aSimulation) override { return true; }
   bool AreNullDevicesAllowed(bool aGridAssetNull, bool aAllGridAssetsNull, bool aFreeAssetNull, bool aAllFreeAssetsNull) const override
   {
      return true;
   }
   bool VerifyDeviceP(const WsfPlatform& aPlatform, const std::string& aDeviceName) const { return true; }
   bool VerifyDeviceAndType(const WsfPlatform& aPlatform,
                            const std::string& aDeviceName,
                            const std::string& aDeviceType) const override
   {
      return true;
   }
   void PendingStartP(WsfSimulation& aSimulation) override {}
};

#endif // WSFCOVERAGETESTTESTCOVERAGE_HPP
