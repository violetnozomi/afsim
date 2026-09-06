// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSPINSENSORSCHEDULER_HPP
#define WSFSPINSENSORSCHEDULER_HPP

#include "wsf_export.h"

#include <queue>
#include <vector>

class UtInput;
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfSensorScheduler.hpp"
#include "WsfTrack.hpp"

//! The WsfSpinSensorScheduler provides a sensor scanning function that aims to capture
//! the behavior of a spinning radar system in a temporally accurate manner.
//! No explicit scanning or slewing of the sensor beam is visible in Mystic, as the
//! WsfSpinSensorScheduler assumes the sensor it is scheduling has a 360 degree field of view.
class WSF_EXPORT WsfSpinSensorScheduler : public WsfSensorScheduler
{
public:
   WsfSpinSensorScheduler()           = default;
   ~WsfSpinSensorScheduler() override = default;

   //! Factory method for WsfSensorSchedulerTypes to determine if a scheduler
   //! represented by this class is being requested.
   static std::unique_ptr<WsfSensorScheduler> ObjectFactory(const std::string& aTypeName);

   //! @name Common infrastructure methods.
   //@{
   //! Clone this object.
   //! @return a pointer to the new object.
   WsfSensorScheduler* Clone() const override;

   bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorTracker* aTrackerPtr) override;
   bool ProcessInput(UtInput& aInput) override;
   //@}

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

protected:
   WsfSpinSensorScheduler(const WsfSpinSensorScheduler& aSrc);
   WsfSpinSensorScheduler& operator=(const WsfSpinSensorScheduler&) = delete;

private:
   //! Reset the state of various private data objects.
   //! Side effects: mModeList, mDetectionTimes, mMoverCallbackMap, mRouteCallbackMap
   void ResetSearchList();

   //! Calculate when the beam will be pointing at a certain target. This result is stored in mDetectionTimes[@param
   //! aTargetIndex].
   //! @param aTargetIndex the platform index of the target. Requires aTargetIndex > 0.
   //! @param aSimTime the reference time after which the next detection attempt will be scheduled.
   //!        Thus aTargetIndex will next attempt to be detected in the interval (aSimTime, aSimTime + mScanPeriod].
   void CalculateDetectionTime(size_t aTargetIndex, double aSimTime);

   //! Returns a value in [0, 2pi) describing the aAngle between current sensor look direction and the target
   double CalculateAngle(const ut::coords::WCS& aRangeVector, double aAngle);

   //! Returns the direction of the sensor "beam" relative to the positive x-axis of the part, at time aSimTime
   //! signed based on mScanDirection. An angle of +30 (deg, yaw) off the +x-axis at time aSimTime will correspond
   //! to a return value of 30 degrees if mScanDirection is 1.0f (clockwise) and 330 degrees otherwise.
   double GetDirection(double aSimTime) const;

   //! Returns a vector (WCS) in the direction from aSensorPtr to aTargetPtr at time aSimTime
   //! In the event that the future location of aSensorPtr or aTargetPtr is unavailable, the current location
   //! of that system will be used instead.
   ut::coords::WCS FutureVec(double aSimTime, WsfSensor* aSensorPtr, const WsfPlatform* aTargetPtr);

   //! Returns angular distance at time aSimTime between sensor pointing direction and aTargetPtr
   //! @param aSimTime  >= 0
   //! @param aTargetPtr != nullptr
   double AngularDistance(double aSimTime, const WsfPlatform* aTargetPtr);

   //! The scan period of the sensor (seconds/revolution).
   double mScanPeriod{10.0};

   //! -1.0f if beam scan direction is counter-clockwise, 1.0f else
   float mScanDirection{1.0f};

   //! The initial angle of the scheduler relative to PCS +x axis.
   //! [0, 2*pi), radians
   double mInitialAngle{0.0};

   //! Flag to denote when the user wishes the initial position to be randomized
   bool mRandomAzimuthPosition{false};

   //! Flag to indicate starting_azimuth_random is used
   bool mRandomAzimuthPositionCheck{false};

   //! Track next detection time (value) for each target (key)
   std::map<std::size_t, double> mDetectionTimes;

   //! A callback holder and maps to hold callbacks when the mover is changed and
   //! when a platform's route is changed. When these callbacks are called, the
   //! detection time of that platform will update.
   std::map<size_t, std::unique_ptr<UtCallback>> mMoverCallbackMap;
   std::map<size_t, std::unique_ptr<UtCallback>> mRouteCallbackMap;

   //! The pointers to the sensor modes, indexed by mode index.
   std::vector<WsfSensorMode*> mModeList;

   //! The last mode explicitly selected (i.e.: via WsfSensor::SelectMode).
   size_t mLastExplicitModeIndex{0};
};

#endif
