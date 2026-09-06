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

#ifndef WSFSENSORCOVERAGE_HPP
#define WSFSENSORCOVERAGE_HPP

#include "WsfCoverage.hpp"

class WsfSensor;
class WsfSensorResult;

namespace wsf
{
namespace coverage
{

//! Compute coverage from sensor interactions.
class SensorCoverage : public Coverage
{
public:
   explicit SensorCoverage(const WsfScenario& aScenario)
      : Coverage(aScenario)
   {
      SetType(cTYPE);
   }

   SensorCoverage(const SensorCoverage& aOther) = default;
   ~SensorCoverage() override                   = default;

   // WsfObject overrides
   SensorCoverage* Clone() const override { return new SensorCoverage{*this}; }

   constexpr static const char* cTYPE{"WSF_SENSOR_COVERAGE"};

private:
   bool InitializeP(WsfSimulation& aSimulation) override;
   bool AreNullDevicesAllowed(bool aGridAssetNull,
                              bool aAllGridAssetsNull,
                              bool aFreeAssetNull,
                              bool aAllFreeAssetsNull) const override;
   bool VerifyDeviceP(const WsfPlatform& aPlatform, const std::string& aDeviceName) const override;
   bool VerifyDeviceAndType(const WsfPlatform& aPlatform,
                            const std::string& aDeviceName,
                            const std::string& aDeviceType) const override;
   void PendingStartP(WsfSimulation& aSimulation) override;

   void OnSensorDetectionChanged(double aSimTime, WsfSensor* aDetectorPtr, std::size_t aDetectedIndex, WsfSensorResult& aResult);
   void ProcessInterval(bool               aIntervalStart,
                        double             aSimTime,
                        const std::string& aFreeAssetPlatformName,
                        const std::string& aFreeAssetDeviceName,
                        const std::string& aGridAssetPlatformName,
                        const std::string& aGridAssetDeviceName);
};

} // namespace coverage
} // namespace wsf

#endif // WSFSENSORCOVERAGE_HPP
