// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef SIMULATIONEXTENSION_HPP
#define SIMULATIONEXTENSION_HPP

#include "UtCallbackHolder.hpp"
class WsfPlatform;
#include "observer/WsfPlatformObserver.hpp"
class WsfSensor;
class WsfSensorMode;
class WsfSensorResult;
#include "WsfSimulationExtension.hpp"
class WsfTrack;
class WsfWeapon;
class WsfWeaponEngagement;


namespace engage
{

class Events;
class Simulation;
class Task;
class TaskOutput;

//! The simulation extension for ENGAGE.
//!
//! This contains the simulation observers for events of interest.
class SimulationExtension : public WsfSimulationExtension
{
public:
   SimulationExtension();
   ~SimulationExtension() override;

   void AddedToSimulation() override;
   bool Initialize() override;

#define TODO_TEMPORARY_OBSERVERS
#ifdef TODO_TEMPORARY_OBSERVERS
   // TODO-TEMPORARAY OBSERVERS
   void FrameStarting(double aSimTime);
   void PlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr);
#endif

   //! @name Simulation observers
   //@{
   void AdvanceTime(double aSimTime);

   void FrameComplete(double aSimTime);

   void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr);

   void PlatformAppearanceChanged(double aSimTime, WsfPlatform* aPlatformPtr, WsfObserver::AppearanceType aAppearanceType);

   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   void PlatformKilled(double aSimTime, WsfPlatform* aPlatformPtr);

   void SensorDetectionAttempt(double aSimTime, WsfSensor* aSensorPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

   void SensorDetectionChanged(double aSimTime, WsfSensor* aSensorPtr, size_t aTargetIndex, WsfSensorResult& aResult);

   void SensorFrequencyChanged(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void SensorModeActivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

   void SensorNonOperational(double aSimTime, WsfSensor* aSensorPtr);

   void SensorOperational(double aSimTime, WsfSensor* aSensorPtr);

   void SensorRequestCanceled(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorRequestInitiated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, const WsfTrack* aTrackPtr);

   void SensorRequestUpdated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, const WsfTrack* aTrackPtr);

   void SensorTrackCoasted(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorTrackUpdated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

   void SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr);

   void SensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr);

   void SimulationComplete(double aSimTime);

   void WeaponFireAborted(double aSimTime, WsfWeapon* aWeaponPtr, const WsfTrack* aTargetTrackPtr, double aQuantity);

   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr);

   void WeaponFireRequested(double aSimTime, WsfWeapon* aWeaponPtr, const WsfTrack* aTargetTrackPtr, double aQuantity);


   void WeaponHit(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPtr);

   void WeaponKilled(double aSimTime, WsfWeapon* aWeaponPtr);

   void WeaponMissed(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPtr);

   void WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);
   //@}

private:
   UtCallbackHolder mCallbacks;

   Task*       mTaskPtr;
   TaskOutput* mTaskOutputPtr; // Ease of use, gotten from mTaskPtr
   Events*     mEventsPtr;     // Ease of use, gotten from mTaskOutputPtr

   //! The WeaponTerminated callback has been encountered.
   bool mWeaponTerminated;

   //! The simulation is 'logically' complete and no further output should occur.
   //! See the observers for WeaponTerminated and FrameComplete for more information.
   bool mSimulationComplete;
};

} // namespace engage

#endif
