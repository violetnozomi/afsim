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

#include "WsfSensorCoverage.hpp"

#include "UtLog.hpp"
#include "WsfCoverageGrid.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace coverage
{

//! Connect this object to the appropriate sensor observers.
bool SensorCoverage::InitializeP(WsfSimulation& aSimulation)
{
   mCallbacks.Add(
      WsfObserver::SensorDetectionChanged(&aSimulation).Connect(&SensorCoverage::OnSensorDetectionChanged, this));
   return true;
}

bool SensorCoverage::AreNullDevicesAllowed(bool aGridAssetNull,
                                           bool aAllGridAssetsNull,
                                           bool aFreeAssetNull,
                                           bool aAllFreeAssetsNull) const
{
   bool retval{false};
   if (!aAllFreeAssetsNull && aFreeAssetNull)
   {
      // All free assets must be null, or none of them should be
      auto msg = ut::log::error() << "Device specification error in coverage definition.";
      msg.AddNote() << "If any free asset has device = '" << cNULL_COVERAGE_DEVICE_NAME << "', all free assets must.";
      msg.AddNote() << "Coverage: " << GetName();
   }
   else if (!aAllGridAssetsNull && aGridAssetNull)
   {
      // All grid assets must be null, or none of them should be
      auto msg = ut::log::error() << "Device specification error in coverage definition.";
      msg.AddNote() << "If any grid asset has device = '" << cNULL_COVERAGE_DEVICE_NAME << "', all grid assets must.";
      msg.AddNote() << "Coverage: " << GetName();
   }
   else if (!aAllGridAssetsNull && !aAllFreeAssetsNull)
   {
      auto msg = ut::log::error() << "Device specification error in coverage definition.";
      msg.AddNote() << "Either grid assets or free assets must have device = '" << cNULL_COVERAGE_DEVICE_NAME << "'.";
      msg.AddNote() << "Coverage: " << GetName();
   }
   else if (aAllGridAssetsNull && aAllFreeAssetsNull)
   {
      auto msg = ut::log::error() << "Device specification error in coverage definition.";
      msg.AddNote() << "Either grid assets or free assets must have a device = '" << cNULL_COVERAGE_DEVICE_NAME << "'.";
      msg.AddNote() << "Coverage: " << GetName();
   }
   else if ((aAllGridAssetsNull && !aFreeAssetNull) || (!aGridAssetNull && aAllFreeAssetsNull))
   {
      retval = true;
   }
   return retval;
}

//! Veryify that the given platform has a device relevant for sensor coverage.
//!
//! Determine if the given @p aPlatform has a device with the given @p aDeviceName that is
//! a sensor, and can thus be used in a sensor coverage computation.
bool SensorCoverage::VerifyDeviceP(const WsfPlatform& aPlatform, const std::string& aDeviceName) const
{
   return aPlatform.GetComponent<WsfSensor>(aDeviceName) != nullptr;
}

//! Verify that the given platform has a relevant device with the given name and type.
//!
//! Determine if the given @p aPlatform has a relevant device (WsfSensor) with the given
//! @p aDeviceName and given @p aDeviceType.
bool SensorCoverage::VerifyDeviceAndType(const WsfPlatform& aPlatform,
                                         const std::string& aDeviceName,
                                         const std::string& aDeviceType) const
{
   auto* sensorPtr = aPlatform.GetComponent<WsfSensor>(aDeviceName);
   return sensorPtr && sensorPtr->IsA_TypeOf(aDeviceType);
}

//! Set sensors in the simulation to appropriate categories.
//!
//! In the Coverage object, each platform in the simulation is added to
//! one of three categories indicating if it is a grid asset, a free asset
//! or a non-asset for this coverage. Here, these categories are used to
//! set the sensors on various objects to ignore platforms in other
//! appropriate categories.
//!
//! \param aSimulation - The simulation owning this coverage object.
void SensorCoverage::PendingStartP(WsfSimulation& aSimulation)
{
   if (!GetGrid()->CreatesNewPlatforms())
   {
      return;
   }

   std::string gridCat = GetGridAssetCategory();
   std::string nonCat{cUNIVERSAL_NON_ASSET_CATEGORY};
   std::string freeCat = GetFreeAssetCategory();

   // Grid asset sensors should ignore both non-assets and other grid assets.
   auto actionPtr = CreateGridAction(
      [&aSimulation, &gridCat, &nonCat, &freeCat](const Grid& aGrid, const Point& aPoint)
      {
         auto         ga      = aGrid.GetAsset(aPoint.GetID());
         WsfPlatform* platPtr = aSimulation.GetPlatformByName(ga.GetPlatformName());
         if (platPtr)
         {
            for (WsfPlatform::RoleIterator<WsfSensor> iter{*platPtr}; !iter.AtEnd(); ++iter)
            {
               iter->AddIgnoredCategory(gridCat);
               iter->AddIgnoredCategory(nonCat);
               // Further, if there is a sensor on the grid asset that is not the device in question,
               // it should ignore the free assets also
               if (iter->GetName() != ga.GetDeviceName())
               {
                  iter->AddIgnoredCategory(freeCat);
               }
            }
         }
      });
   GetGrid()->Invoke(*actionPtr);

   // Sensors on free assets that are not the device in question should ignore grid assets.
   for (auto& asset : mFreeAssets)
   {
      WsfPlatform* platPtr = aSimulation.GetPlatformByName(asset.GetPlatformName());
      if (platPtr)
      {
         for (WsfPlatform::RoleIterator<WsfSensor> iter{*platPtr}; !iter.AtEnd(); ++iter)
         {
            if (mFreeAssets.count(FreeAsset{asset.GetPlatformName(), iter->GetName()}) == 0)
            {
               iter->AddIgnoredCategory(gridCat);
            }
         }
      }
   }

   // Non-assets should ignore grid assets.
   for (std::size_t i = 0; i < aSimulation.GetPlatformCount(); ++i)
   {
      WsfPlatform* platPtr = aSimulation.GetPlatformEntry(i);
      if (platPtr->IsCategoryMember(nonCat))
      {
         for (WsfPlatform::RoleIterator<WsfSensor> iter{*platPtr}; !iter.AtEnd(); ++iter)
         {
            iter->AddIgnoredCategory(gridCat);
         }
      }
   }
}

//! Callback for the sensor detection changed event for this coverage object.
//!
//! After filtering out events that occur outside the coverage interval, this method
//! will determine if the interaction is between one free asset and one grid asset for
//! this coverage.
//!
//! \param aSimTime       - The simulation time of the interaction.
//! \param aDetectorPtr   - The detecting device.
//! \param aDetectedIndex - The platform index of the detected platform.
//! \param aResult        - The sensor result of the interaction.
void SensorCoverage::OnSensorDetectionChanged(double           aSimTime,
                                              WsfSensor*       aDetectorPtr,
                                              std::size_t      aDetectedIndex,
                                              WsfSensorResult& aResult)
{
   // Filter events not involving one free asset and one grid asset.
   WsfPlatform* detectorPlatformPtr = aDetectorPtr->GetPlatform();
   if (!IsAsset(detectorPlatformPtr))
   {
      return;
   }

   WsfSimulation* simulationPtr       = detectorPlatformPtr->GetSimulation();
   WsfPlatform*   detecteePlatformPtr = simulationPtr->GetPlatformByIndex(aDetectedIndex);
   if (!IsAsset(detecteePlatformPtr))
   {
      return;
   }

   // Make sure we have interaction between one free and one grid asset.
   if (IsFreeAsset(detectorPlatformPtr) && IsGridAsset(detecteePlatformPtr))
   {
      ProcessInterval(aResult.Detected(),
                      aSimTime,
                      detectorPlatformPtr->GetName(),
                      aDetectorPtr->GetName(),
                      detecteePlatformPtr->GetName(),
                      cNULL_COVERAGE_DEVICE_NAME);
   }
   else if (IsFreeAsset(detecteePlatformPtr) && IsGridAsset(detectorPlatformPtr))
   {
      ProcessInterval(aResult.Detected(),
                      aSimTime,
                      detecteePlatformPtr->GetName(),
                      cNULL_COVERAGE_DEVICE_NAME,
                      detectorPlatformPtr->GetName(),
                      aDetectorPtr->GetName());
   }
}

//! Assure that the given assets are valid and then add the interaction to the raw data.
//!
//! \param aIntervalStart         - Is the interaction the start of an interval.
//! \param aSimTime               - The simulation time of the interaction.
//! \param aFreeAssetPlatformName - The name of the free asset's platform.
//! \param aFreeAssetDeviceName   - The name of the free asset's device.
//! \param aGridAssetPlatformName - The name of the grid asset's platform.
//! \param aGridAssetDeviceName   - The name of the grid asset's device.
void SensorCoverage::ProcessInterval(bool               aIntervalStart,
                                     double             aSimTime,
                                     const std::string& aFreeAssetPlatformName,
                                     const std::string& aFreeAssetDeviceName,
                                     const std::string& aGridAssetPlatformName,
                                     const std::string& aGridAssetDeviceName)
{
   FreeAsset fa{aFreeAssetPlatformName, aFreeAssetDeviceName};
   if (mFreeAssets.find(fa) == mFreeAssets.end())
   {
      return;
   }

   GridAsset ga = GetGrid()->GetAsset(aGridAssetPlatformName);
   if (!ga.IsValid() || (ga.GetPlatformName() != aGridAssetPlatformName) || (ga.GetDeviceName() != aGridAssetDeviceName))
   {
      return;
   }

   if (aIntervalStart)
   {
      AddIntervalStartToData(ga, fa, aSimTime);
   }
   else
   {
      AddIntervalEndToData(ga, fa, aSimTime);
   }
}

} // namespace coverage
} // namespace wsf
