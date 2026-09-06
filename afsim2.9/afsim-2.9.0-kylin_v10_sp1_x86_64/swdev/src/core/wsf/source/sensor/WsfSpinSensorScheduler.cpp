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

#include "WsfSpinSensorScheduler.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <string>

#include "UtEntity.hpp"
#include "UtGoldenSectionSearch.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfMover.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSimulation.hpp"
#include "WsfWaypointMover.hpp"

WsfSpinSensorScheduler::WsfSpinSensorScheduler(const WsfSpinSensorScheduler& aSrc)
   : WsfSensorScheduler(aSrc)
   , mScanPeriod(aSrc.mScanPeriod)
   , mScanDirection(aSrc.mScanDirection)
   , mInitialAngle(aSrc.mInitialAngle)
   , mRandomAzimuthPosition(aSrc.mRandomAzimuthPosition)
   , mRandomAzimuthPositionCheck(aSrc.mRandomAzimuthPositionCheck)
   , mModeList()
   , mLastExplicitModeIndex(aSrc.mLastExplicitModeIndex)
{
}

std::unique_ptr<WsfSensorScheduler> WsfSpinSensorScheduler::ObjectFactory(const std::string& aTypeName)
{
   if (aTypeName == "spin")
   {
      return ut::make_unique<WsfSpinSensorScheduler>();
   }
   return nullptr;
}

WsfSensorScheduler* WsfSpinSensorScheduler::Clone() const
{
   return new WsfSpinSensorScheduler(*this);
}

bool WsfSpinSensorScheduler::ProcessInput(UtInput& aInput)
{
   bool              myCommand = true;
   const std::string command(aInput.GetCommand());

   if (command == "scan_period")
   {
      aInput.ReadValueOfType(mScanPeriod, UtInput::cTIME);
   }
   else if (command == "clockwise")
   {
      bool direction = aInput.ReadBool();
      mScanDirection = direction ? 1.0f : -1.0f;
   }
   else if (command == "starting_beam_azimuth")
   {
      aInput.ReadValueOfType(mInitialAngle, UtInput::cANGLE);
      mRandomAzimuthPosition = false;
   }
   else if (command == "starting_azimuth_randomized")
   {
      mRandomAzimuthPosition      = true;
      mRandomAzimuthPositionCheck = true;
   }
   else if (WsfSensorScheduler::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool WsfSpinSensorScheduler::Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorTracker* aTrackerPtr)
{
   if (aSensorPtr->GetModeList() == nullptr)
   {
      ut::log::error() << "A mode list is required for WsfSpinSensorScheduler";
      return false;
   }

   bool ok = WsfSensorScheduler::Initialize(aSimTime, aSensorPtr, aTrackerPtr);

   if (ok && mRandomAzimuthPosition)
   {
      // If we appear to have a azimuth angle specified, let the user know so that we warn them of overriding a
      // potentially pre-defined az angle
      if (mInitialAngle != 0.0)
      {
         ut::log::warning() << "Initial azimuth angle set to " << mInitialAngle
                            << " while scheduler set to use randomized starting azimuth on sensor: "
                            << aSensorPtr->GetName() << " for platform: " << aSensorPtr->GetPlatform()->GetName()
                            << ". Possible loss of user data.";
      }

      mInitialAngle = GetSimulation()->GetRandom().Uniform(-UtMath::cPI, UtMath::cPI);
   }

   // If starting_azimuth_random was input, setting mRandomAzimuthPositionCheck to true, but mRandomAzimuthPosition is
   // false, it was clobbered by starting_beam_azimuth
   // NOTE: Typically AFSIM does not provide these kinds of warnings in case of clobbering, and assumes user is being
   // intentional with this choice.
   // Original behavior is kept to preserve possible EZJA use-case.
   if (mRandomAzimuthPositionCheck && !mRandomAzimuthPosition)
   {
      ut::log::warning()
         << "Initial azimuth angle set to randomized starting azimuth while scheduler was set to an initial angle of "
         << mInitialAngle
         << " while scheduler set to use randomized starting azimuth on sensor: " << aSensorPtr->GetName()
         << " for platform: " << aSensorPtr->GetPlatform()->GetName() << ". Possible loss of user data.";
   }

   // Reduce future dynamic casting by extracting derived class mode pointers.
   aSensorPtr->GetModeList()->GetDerivedModeList(mModeList);
   mLastExplicitModeIndex = static_cast<unsigned int>(mModeList.size());

   // Ensure that each mode has a frame time.
   for (const auto modePtr : mModeList)
   {
      if (modePtr->GetFrameTime() <= 0.0)
      {
         ut::log::error() << "Sensor " << mSensorPtr->GetName() << ", mode " << modePtr->GetName()
                          << ": frame_time not specified";
         ok = false;
      }
   }

   return ok;
}

void WsfSpinSensorScheduler::ModeDeselected(double aSimTime, WsfStringId aModeNameId)
{
   const size_t modeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
   if (mLastExplicitModeIndex == modeIndex)
   {
      mLastExplicitModeIndex = mModeList.size();
   }
}

void WsfSpinSensorScheduler::ModeSelected(double aSimTime, WsfStringId aModeNameId)
{
   mLastExplicitModeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
}

void WsfSpinSensorScheduler::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // When a platform is added, add it to the SearchList, calculate its detection time,
   // and register callbacks and add them to the holder and maps.
   auto index = aPlatformPtr->GetIndex();
   mSearchList.push_back(index);
   CalculateDetectionTime(index, aSimTime);
   mSensorPtr->SetNextUpdateTime(aSimTime, std::min(mDetectionTimes[index], mSensorPtr->GetUpdateInterval()));

   // We need a way to recalculate detection time when a target's path is changed due to
   // reactive maneuvering. However, the following way of scheduling callbacks slows down
   // runtime a LOT.
   std::unique_ptr<UtCallback> moverCallback =
      WsfObserver::MoverChanged(GetSimulation())
         .Connect([this, index](WsfPlatform* platform)
                  { CalculateDetectionTime(index, platform->GetSimulation()->GetSimTime()); });
   mMoverCallbackMap[index] = std::move(moverCallback);

   std::unique_ptr<UtCallback> routeCallback =
      WsfObserver::RouteChanged(GetSimulation())
         .Connect([this](WsfMover* mover)
                  { CalculateDetectionTime(mover->GetPlatform()->GetIndex(), mover->GetSimulation()->GetSimTime()); });
   mRouteCallbackMap[index] = std::move(routeCallback);

   if (DebugEnabled())
   {
      ut::log::debug() << "Spin Scheduler: Platform added. ";
   }
}

void WsfSpinSensorScheduler::RemoveTarget(double aSimTime, size_t aTargetIndex)
{
   mSearchList.erase(std::remove(mSearchList.begin(), mSearchList.end(), aTargetIndex), mSearchList.end());
   mDetectionTimes.erase(aTargetIndex);
   mMoverCallbackMap.erase(aTargetIndex);
   mRouteCallbackMap.erase(aTargetIndex);
}

double WsfSpinSensorScheduler::GetDirection(double aSimTime) const
{
   const auto angular_velocity = UtMath::cTWO_PI / mScanPeriod * mScanDirection;
   return UtMath::NormalizeAngle0_TwoPi(mInitialAngle + aSimTime * angular_velocity);
}

void WsfSpinSensorScheduler::ResetSearchList()
{
   mDetectionTimes.clear();
   mSearchList.clear();
   mMoverCallbackMap.clear();
   mRouteCallbackMap.clear();
}

void WsfSpinSensorScheduler::TurnOn(double aSimTime)
{
   if (DebugEnabled())
   {
      ut::log::debug() << "T=" << aSimTime << ' ' << mSensorPtr->GetPlatform()->GetName() << '.'
                       << mSensorPtr->GetName() << " scheduler TurnOn";
   }

   ResetSearchList();
   const WsfSimulation& sim           = *GetSimulation();
   const std::size_t    platformCount = sim.GetPlatformCount();
   for (std::size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* targetPtr = sim.GetPlatformEntry(platformEntry);

      if (targetPtr != mSensorPtr->GetPlatform())
      {
         mSensorPtr->PlatformAdded(aSimTime, targetPtr);
      }
   }
}

void WsfSpinSensorScheduler::TurnOff(double aSimTime)
{
   if (DebugEnabled())
   {
      ut::log::debug() << "T=" << aSimTime << ' ' << mSensorPtr->GetPlatform()->GetName() << '.'
                       << mSensorPtr->GetName() << " scheduler TurnOff";
   }

   ResetSearchList();
}

bool WsfSpinSensorScheduler::SelectTarget(double               aSimTime,
                                          double&              aNextSimTime,
                                          size_t&              aTargetIndex,
                                          WsfTrackId&          aRequestId,
                                          WsfSensor::Settings& aSettings)
{
   aTargetIndex = 0;
   aNextSimTime = std::numeric_limits<double>::max(); // first guess, in case no viable targets

   for (auto index : mSearchList)
   {
      // Loop does three things:
      // 1. Update detection times of events scheduled for strictly prior to aSimTime
      // 2. Indicate which target should be detected presently (detection time == aSimTime)
      // 3. Determine time for next scheduled detection

      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(index);

      // Check that the platform still exists.
      if (targetPtr == nullptr)
      {
         RemoveTarget(aSimTime, index);
         return false;
      }

      // 1. Update detection times of events scheduled for strictly prior to aSimTime
      const auto& time = mDetectionTimes[index];
      if (time < aSimTime)
      {
         CalculateDetectionTime(index, aSimTime);
      }

      // 2. Indicate which target should be detected presently (detection time == aSimTime)
      if (mSensorPtr->WithinFieldOfView(aSimTime, targetPtr))
      {
         if (std::abs(aSimTime - time) < 1.0e-5)
         {
            aTargetIndex = index;
            CalculateDetectionTime(index, aSimTime); // schedule next detection
            break;
         }
      }
   }

   // 3. Determine time for next scheduled detection
   using mp     = std::pair<size_t, double>;
   auto it      = std::min_element(mDetectionTimes.begin(),
                              mDetectionTimes.end(),
                              [](mp a, mp b) -> bool { return a.second < b.second; });
   aNextSimTime = (it != std::end(mDetectionTimes)) ? it->second : std::numeric_limits<double>::max();

   if (DebugEnabled())
   {
      if (aTargetIndex != 0 && aNextSimTime > aSimTime)
      {
         ut::log::debug() << "Spin Scheduler T=" << aSimTime << " " << mSensorPtr->GetPlatform()->GetName() << "."
                          << mSensorPtr->GetName() << " Selecting target" << aTargetIndex << " NT =" << aNextSimTime;
      }
   }

   return (aTargetIndex != 0 && aNextSimTime >= aSimTime);
}

void WsfSpinSensorScheduler::CalculateDetectionTime(size_t aTargetIndex, double aSimTime)
{
   auto targetPtr = GetSimulation()->GetPlatformByIndex(aTargetIndex);
   if (targetPtr == nullptr)
   {
      RemoveTarget(aSimTime, aTargetIndex);
      return;
   }

   // timeEpsilon defines convergence criterion for UtGoldenSelectionSearch, initial
   // value informed by reference EZJA implementation
   constexpr double timeEpsilon      = 0.002;
   const auto       angularFrequency = UtMath::cTWO_PI / mScanPeriod;

   const auto rangeToTarget  = targetPtr->GetLocationWCS().Subtract(mSensorPtr->GetLocationWCS());
   auto       referenceAngle = GetDirection(aSimTime);
   auto       angleToTarget  = CalculateAngle(rangeToTarget, referenceAngle);

   auto deltaTime = 0.0;

   // Compute time to advance scan to target at start time.
   // AngleToTarget is unsigned, so targets at equal angles clockwise and anti-clockwise
   if (mScanDirection == 1.0f)
   {
      deltaTime = angleToTarget / angularFrequency;
   }
   else
   {
      deltaTime = (UtMath::cTWO_PI - angleToTarget) / angularFrequency;
   }

   using namespace std::placeholders;
   auto f = std::bind(&WsfSpinSensorScheduler::AngularDistance, this, _1, targetPtr);

   auto timeNext = UtGoldenSectionSearch(f, aSimTime, aSimTime + deltaTime, timeEpsilon);

   // Update detection time in map.
   auto it = mDetectionTimes.find(aTargetIndex);
   // either not found, or calculated next detection time is sufficiently far ahead of schedule to note.
   if (it == std::end(mDetectionTimes) || timeNext - it->second > timeEpsilon)
   {
      mDetectionTimes[aTargetIndex] = timeNext;
   }
   else if (timeNext - aSimTime < timeEpsilon) // else, too close to consider, reschedule for next pass
   {
      mDetectionTimes[aTargetIndex] = timeNext + mScanPeriod;
   }
}

double WsfSpinSensorScheduler::CalculateAngle(const ut::coords::WCS& aRangeVector, double aAngle)
{
   double az, el;
   mSensorPtr->ComputeAspect(aRangeVector.GetData(), az, el);
   return UtMath::NormalizeAngle0_TwoPi(az - aAngle);
}

ut::coords::WCS WsfSpinSensorScheduler::FutureVec(double aSimTime, WsfSensor* aSensorPtr, const WsfPlatform* aTargetPtr)
{
   // Get target and sensor positions at a given future time.
   auto sensorLocation = aSensorPtr->GetLocationWCS();
   auto targetLocation = aTargetPtr->GetLocationWCS();

   auto sensorPlatform = aSensorPtr->GetPlatform();
   if (sensorPlatform)
   {
      auto sensorMover = sensorPlatform->GetMover();
      if (sensorMover)
      {
         if (!sensorMover->GetFutureLocationWCS(aSimTime, sensorLocation.GetData()))
         {
            aSensorPtr->GetLocationWCS(sensorLocation.GetData());
         }
      }
   }

   auto targetMover = aTargetPtr->GetMover();
   if (targetMover)
   {
      if (!targetMover->GetFutureLocationWCS(aSimTime, targetLocation.GetData()))
      {
         aTargetPtr->GetLocationWCS(targetLocation.GetData());
      }
   }

   // Compute sensor to target unit range vector and normalize.
   return targetLocation.Subtract(sensorLocation);
}

double WsfSpinSensorScheduler::AngularDistance(double aSimTime, const WsfPlatform* aTargetPtr)
{
   auto    angle                  = GetDirection(aSimTime);
   auto    future_position_vector = FutureVec(aSimTime, mSensorPtr, aTargetPtr);
   UtVec3d target_pcs;
   mSensorPtr->ConvertWCSVectorToPCS(target_pcs.GetData(), future_position_vector.GetData());
   target_pcs[2] = 0;
   target_pcs.Normalize();
   UtVec3d sensor_pcs = {std::cos(angle), std::sin(angle), 0};
   UtVec3d cross_product;
   cross_product.CrossProduct(sensor_pcs, target_pcs);
   return UtMath::NormalizeAngle0_TwoPi((std::signbit(mScanDirection) == std::signbit(cross_product[2]) ? 1.0 : -1.0) *
                                        sensor_pcs.AngleWith(target_pcs));
}
