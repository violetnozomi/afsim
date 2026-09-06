// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPHYSICALSCANSENSORSCHEDULER_HPP
#define WSFPHYSICALSCANSENSORSCHEDULER_HPP

#include "wsf_export.h"

#include <functional>
#include <queue>
#include <vector>

#include "UtOptional.hpp"
#include "WsfSensorScheduler.hpp"
#include "WsfTrack.hpp"

class WSF_EXPORT WsfPhysicalScanSensorScheduler : public WsfSensorScheduler
{
   using SearchListSortFn = std::function<bool(size_t, size_t)>;

public:
   WsfPhysicalScanSensorScheduler() = default;
   WsfPhysicalScanSensorScheduler(std::string aTypeName, bool aRandomizeInitialHeading, SearchListSortFn aSearchListSort);
   ~WsfPhysicalScanSensorScheduler() override = default;

   static std::unique_ptr<WsfSensorScheduler> ObjectFactory(const std::string& aTypeName);

   WsfSensorScheduler* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorTracker* aTrackerPtr) override;

   void ModeDeselected(double aSimTime, WsfStringId aModeNameId) override;

   void ModeSelected(double aSimTime, WsfStringId aModeNameId) override;

   void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) override;

   void RemoveTarget(double aSimTime, size_t aTargetIndex) override;

   bool SelectTarget(double               aSimTime,
                     double&              aNextSimTime,
                     size_t&              aTargetIndex,
                     WsfTrackId&          aRequestId,
                     WsfSensor::Settings& aSettings) override;

   void TurnOff(double aSimTime) override;

   void TurnOn(double aSimTime) override;

   //! Returns the initial heading (in radians) of the sensor, either as specified via ProcessInput, or
   //! otherwise a random value chosen during Initialize. Do not call this before the scheduler is initialized!
   double GetInitialHeading() const { return mInitialHeading.value(); }

protected:
   WsfPhysicalScanSensorScheduler(const WsfPhysicalScanSensorScheduler& aSrc);
   WsfPhysicalScanSensorScheduler& operator=(const WsfPhysicalScanSensorScheduler&) = delete;

   void CheckSearchModeAvailability();

   void ResetSearchList();

   //! The pointers to the sensor modes, indexed by mode index.
   std::vector<WsfSensorMode*> mModeList;

   //! The last mode explicitly selected (i.e.: via WsfSensor::SelectMode).
   size_t mLastExplicitModeIndex{0};

   //! The mode index to be used for searching.
   //! If greater than the mode count then searching is disabled.
   std::vector<size_t> mSearchModeIndex{0};

   //! 'true' if search chances can be processed.
   bool mSearchAllowed{false};

   struct SensorTarget
   {
      SensorTarget()
         : SensorTarget(0, 0)
      {
      }

      SensorTarget(size_t aTargetIndex, size_t aModeIndex)
         : mTargetIndex(aTargetIndex)
         , mModeIndex(aModeIndex)
      {
      }

      size_t mTargetIndex;
      size_t mModeIndex;
   };

   //! A queue of platform indices representing targets in the sensor's current FOV angle.
   std::queue<SensorTarget> mSensorTargetQueue;

   //! The value of UpdateInterval defined by user at run-time in the input files.
   double mSensorUpdateInterval{0.0};

   struct SearchModeData
   {
      double mLastUpdate;           // TODO - not sure this is required per mode (sensor level)
      double mSensorPieSize;        //! The size of the angle centered around the sensor heading.
      double mCurrentSensorHeading; // TODO - not sure this is required per mode (sensor level)
   };

   std::map<size_t, SearchModeData> mSearchModeDataMap;

   //! The value of Sim Time during the last Update interval.  This is used
   //! by SelectTarget to know when to search through the list of
   //! platforms and create a list of targets to return.
   double mLastUpdate{-1.0};

   //! The current sensor heading.  This gets set to a random value during
   //! initialization so that the sensor doesn't always start out
   //! pointing north.
   // double                      mCurrentSensorHeading;

private:
   std::string          mTypeName{"physical_scan"};
   ut::optional<double> mInitialHeading{ut::nullopt}; // If empty, initial heading will be randomized during initialization
   SearchListSortFn     mSearchListSort;
};

#endif
