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

#ifndef WSFCOVERAGECOVERAGE_HPP
#define WSFCOVERAGECOVERAGE_HPP

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "UtCalendar.hpp"
#include "UtCallback.hpp"
#include "UtCloneablePtr.hpp"
#include "WsfCoverageAccessInterval.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageAssetSpecification.hpp"
#include "WsfCoverageIntervalConstraint.hpp"
#include "WsfCoverageMeasure.hpp"
#include "WsfCoverageRawDataOutput.hpp"
#include "WsfObject.hpp"
#include "WsfScenario.hpp"
class WsfSimulation;

#include "wsf_coverage_export.h"

namespace wsf
{
namespace coverage
{

class Grid;

//! Compute coverage of various assets over a specified grid.
//!
//! A coverage computation observes interactions between platforms in the
//! simulation and computes various measures of effectiveness (denoted MOE
//! or Measure) from the interaction data. The platforms whose interactions
//! are observed are either from a set of platforms instantiated to be at
//! the locations of the points in the grid (these are called grid assets),
//! or from platforms that exist in the simulation (these are called free
//! assets, or just assets).
//!
//! Each coverage object will specify the set of assets to use in the
//! computation, a grid over which to compute, and a set of MOEs to
//! compute for those assets over that grid.
class WSF_COVERAGE_EXPORT Coverage : public WsfObject
{
public:
   explicit Coverage(const WsfScenario& aScenario)
      : mScenario(aScenario)
   {
      mStartEpoch.SetDate(1900, 1, 1);
      mEndEpoch.SetDate(1900, 1, 1);
   }
   Coverage(const Coverage& aOther) = default;
   ~Coverage() override             = default;

   // WsfObject overrides
   Coverage* Clone() const override = 0;
   bool      ProcessInput(UtInput& aInput) override;

   // Main coverage methods
   bool Initialize(WsfSimulation& aSimulation);
   bool ResolveAssets(WsfSimulation& aSimulation);
   void CategorizePlatforms(const WsfSimulation& aSimulation);
   void PendingStart(WsfSimulation& aSimulation);
   void Finalize(double aSimTime, const UtCalendar& aSimStartEpoch);

   // The observable event signatures
   using AccessIntervalStartCallback    = UtCallbackListN<void(const GridAsset&, const FreeAsset&, double)>;
   using AccessIntervalEndCallback      = UtCallbackListN<void(const GridAsset&, const FreeAsset&, double)>;
   using AccessIntervalCompleteCallback = UtCallbackListN<void(const GridAsset&, const FreeAsset&, const AccessInterval&)>;

   //! Return the callback list that is triggered at the start of a relevant access interval.
   //!
   //! The Measure objects defined on a coverage may observe these events so that they
   //! might include this information in their calculation. The subscribers to the access interval
   //! start event are always notified of the event, even if the complete interval is ultimately
   //! filtered out of the data.
   AccessIntervalStartCallback& AccessIntervalStart() { return mIntervalStart; }

   //! Return the callback list that is triggered at the end of a relevant access interval.
   //! Subscribers to this event are always notified even in the event that the complete access
   //! interval is filtered out of the data.
   AccessIntervalEndCallback& AccessIntervalEnd() { return mIntervalEnd; }

   //! Return the callback list that is triggered when a complete access interval is formed.
   //!
   //! Unlike for the start and end of an access interval, subscribers to the event are only
   //! notified if the access interval passes any filtering provided by the IntervalConstraints
   //! defined on this object.
   AccessIntervalCompleteCallback& AccessIntervalComplete() { return mIntervalComplete; }

   //! Return the grid object this object will compute over.
   //!
   //! This method returns the grid object this coverage object will compute over.
   //! If this is called before this object is initialized, this will return nullptr.
   //!
   //! \returns - The grid over which this coverage computes if this object is initialized;
   //!            nullptr if this object is not initialized.
   const Grid* GetGrid() const { return mGridPtr; }

   //! Return the output directory for this objects data.
   //!
   //! Any output files that are produced by this object will be placed in this
   //! folder.
   const std::string& GetOutputDir() const { return mOutputDir; }

   //! Does the given simulation time fall inside this object's coverage interval?
   bool InCoverageInterval(double aSimTime) const { return mCoverageInterval.Contains(aSimTime); }

   //! Return the simulation time of the start of the coverage interval.
   double GetStartTime() const { return mCoverageInterval.GetStart(); }

   //! Return the simulation time of the end of the coverage interval.
   double GetEndTime() const { return mCoverageInterval.GetEnd(); }

   //! Return the date and time of the start of the coverage interval.
   const UtCalendar& GetStartEpoch() const { return mStartEpoch; }

   //! Has the start epoch been given a value?
   bool IsStartEpochDefined() const { return mStartEpoch.GetYear() > 1900; }

   //! Return the date and time of the end of the coverage interval.
   const UtCalendar& GetEndEpoch() const { return mEndEpoch; }

   //! Has the end epoch been given a value?
   bool IsEndEpochDefined() const { return mEndEpoch.GetYear() > 1900; }

   //! Return if the given @p aPlatformPtr is either a grid or free asset for this coverage.
   bool IsAsset(const WsfPlatform* aPlatformPtr) const
   {
      return IsFreeAsset(aPlatformPtr) || IsGridAsset(aPlatformPtr);
   }

   //! Return if the given @p aPlatformPtr is a free asset for this coverage.
   bool IsFreeAsset(const WsfPlatform* aPlatformPtr) const
   {
      return aPlatformPtr && aPlatformPtr->IsCategoryMember(mFreeAssetCategory);
   }

   //! Return if the given @p aPlatformPtr is a grid asset for this coverage.
   bool IsGridAsset(const WsfPlatform* aPlatformPtr) const
   {
      return aPlatformPtr && aPlatformPtr->IsCategoryMember(mGridAssetCategory);
   }

   //! Return if the platform with the given @p aPlatformName has the given @p aPlatformType.
   bool IsPlatformOfType(const std::string& aPlatformName, const std::string& aPlatformType) const;

   //! Return if the platform with the given @p aPlatformName has a device with the given @p aDeviceName that has the
   //! given @p aDeviceType.
   bool IsPlatformDeviceOfType(const std::string& aPlatformName,
                               const std::string& aDeviceName,
                               const std::string& aDeviceType) const;

protected:
   //! Set the grid for this coverage object.
   //!
   //! This sets the grid for this coverage object to be the given @p aGridPtr.
   //!
   //! \note This is intended for use in unit tests. This should not be used
   //!       by general clients of this class.
   void SetGrid(const Grid* aGridPtr) { mGridPtr = aGridPtr; }

   void AddIntervalStartToData(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, double aSimTime);
   void AddIntervalEndToData(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, double aSimTime);

   //! Return the category assigned to grid asset platforms for this coverage.
   const std::string& GetGridAssetCategory() const { return mGridAssetCategory; }

   //! Return the category assigned to free asset platform for this coverage.
   const std::string& GetFreeAssetCategory() const { return mFreeAssetCategory; }

   //! Free assets for this coverage
   std::set<FreeAsset> mFreeAssets{};

   //! This is intended to hold all the callbacks that derived classes use to
   //! connect to simulation events.
   UtCallbackHolder mCallbacks{};

private:
   //! Subclass specific intialization.
   //!
   //! This method is called from the template pattern method Initialize, and it
   //! is intended to perform any subclass specific initialization. At a minimum,
   //! every subclass will need to connect to simulation observes to capture the
   //! relevant events for this object.
   //!
   //! \param aSimulation - The simulation in which this object is participating.
   //! \returns           - true if initialization is successful; false otherwise.
   virtual bool InitializeP(WsfSimulation& aSimulation) = 0;

   //! Allow subclass to determine if null devices are allowed.
   //!
   //! Some coverage types only require one asset in the interaction to have a device.
   //! This method reports to the subclass which assets, if any, have null devices
   //! specified, and allows the subclass to respond to that information by writing out
   //! any relevant errors. @p aGridAssetNone will be true if the grid asset has
   //! a null device specified. @p aFreeAssetNull will be true if any free asset has
   //! a null device specified. @p aAllFreeAssetsNull will be true if all free assets
   //! have a null device specified.
   //!
   //! \param aGridAssetNull     - Does any grid assets have a null device.
   //! \param aAllGridAssetsNull - Do all the grid assets have a null device.
   //! \param aFreeAssetNull     - Does any free asset have a null device.
   //! \param aAllFreeAssetsNull - Do all free assets have a null device.
   //! \returns                  - true if the detected null devices are allowed; false otherwise.
   virtual bool AreNullDevicesAllowed(bool aGridAssetNull,
                                      bool aAllGridAssetsNull,
                                      bool aFreeAssetNull,
                                      bool aAllFreeAssetsNull) const = 0;

   //! Verify the relevance of the given device.
   //!
   //! Subclasses will use this method to verify that the given @p aDeviceName on
   //! the given @p aPlatform is of the correct type for use in that coveage computation.
   //! For example, a mover is not a valid device when computing sensor coverage.
   //!
   //! \param aPlatform   - The platform whose device is being verified.
   //! \param aDeviceName - The name of the device on the given @p aPlatform to verify.
   //! \returns           - true if the device is relevant for this coverage object; false otherwise.
   virtual bool VerifyDeviceP(const WsfPlatform& aPlatform, const std::string& aDeviceName) const = 0;

   //! Verify the device and type of device.
   //!
   //! This method is used when applying IntervalConstraints to the access intervals collected
   //! by this Coverage object. This will return true if the given @p aPlatform has a device
   //! of the subclass specific type (e.g., WsfSensor for SensorCoverage) with the name @p aDeviceName
   //! and given @p aDeviceType.
   //!
   //! \param aPlatform   - The platform whose device is being verified.
   //! \param aDeviceName - The name of the device on the given @p aPlatform to verify.
   //! \param aDeviceType - The type of the device being verified.
   //! \returns           - true if the given platform has a device with the given name and given type; false otherwise.
   virtual bool VerifyDeviceAndType(const WsfPlatform& aPlatform,
                                    const std::string& aDeviceName,
                                    const std::string& aDeviceType) const = 0;

   //! Subclass specific final actions before simulation starts.
   //!
   //! This method, called from the template pattern method PendingStart, allows
   //! subclasses to specify actions that must be performed after platforms are
   //! initialized, but before the simulation begins.
   //!
   //! \param aSimulation - The simulation in which this object is participating.
   virtual void PendingStartP(WsfSimulation& aSimulation) = 0;

   bool FindGrid(const WsfSimulation& aSimulation);
   bool NullDeviceCheck();
   bool InitializeCoverageInterval(const WsfSimulation& aSimulation);
   bool InitializeMeasures();
   bool AssureFilterFileUniqueness() const;
   bool VerifyDevice(const WsfPlatform& aPlatform, const std::string& aDeviceName) const;
   bool ResolvePlatformFreeAsset(const WsfSimulation& aSimulation, const AssetSpecification::Asset& aAsset);
   bool ResolvePlatformTypeFreeAsset(const WsfSimulation& aSimulation, const AssetSpecification::Asset& aAsset);
   bool ResolveCategoryFreeAssets(const WsfSimulation& aSimulation, const AssetSpecification::Asset& aAsset);
   bool ResolveGroupFreeAssets(const WsfSimulation& aSimulation, const AssetSpecification::Asset& aAsset);
   void StartInterval(double aSimTime);
   void CloseAllOpenIntervals(double aSimTime);
   void ProcessInterval(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval);
   void CategorizeAssets(const WsfSimulation& aSimulation);
   void WriteOverlayData() const;

   // Needed to get at the Measure types
   const WsfScenario&   mScenario;
   const WsfSimulation* mSimulationPtr{nullptr};

   // Some basic data
   std::string                                      mGridName{};
   AssetSpecification                               mAssetSpecification{};
   std::map<std::string, ut::CloneablePtr<Measure>> mMeasures{};

   // Coverage interval - saved as calendar objects, or sim times
   UtCalendar     mStartEpoch{};
   UtCalendar     mEndEpoch{};
   AccessInterval mCoverageInterval{};

   // Output related options
   std::string                     mOutputDir{};
   ut::CloneablePtr<RawDataOutput> mRawData{nullptr};
   std::string                     mOverlayOutputFileName{};

   // reference to the grid
   const Grid* mGridPtr{nullptr};
   std::string mGridAssetCategory{};
   std::string mFreeAssetCategory{};

   // Interval events to which Measures can subscribe
   AccessIntervalStartCallback    mIntervalStart{};
   AccessIntervalEndCallback      mIntervalEnd{};
   AccessIntervalCompleteCallback mIntervalComplete{};

   // The intervals as they are built.
   std::map<GridAsset, std::map<FreeAsset, AccessInterval>> mData{};

   // Constraints
   std::vector<IntervalConstraint> mConstraints{};

   bool mFinalized{false};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGECOVERAGE_HPP
