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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <sstream>
#include <string>
#include <vector>


class WsfPlatform;
class WsfSensor;
#include "WsfSensorResult.hpp"
class WsfTrack;
class WsfWeaponEngagement;
class WsfWeapon;

namespace engage
{

class Simulation;

//! This represents all of the events the user can select for writing.
class Events
{
public:
   class Event
   {
   public:
      Event() = delete; // protect from creation
      Event(const std::string& aType)
         : mEventType(aType)
         , mEventTime(-1.0)
      {
      }

      virtual void Write(std::ostream& aStream, Simulation& aSimulation) const {}
      virtual ~Event() = default;

      std::string mEventType;
      double      mEventTime;
   };

   using EventList = std::vector<Event*>;

   Events();

   void       PushEvent(Event* aEvent) { mEventList.push_back(aEvent); }
   EventList& GetEventList() { return mEventList; }
   void       ClearAllEvents();

   class SensorDetectionAttempt : public Event
   {
   public:
      SensorDetectionAttempt() = delete;
      SensorDetectionAttempt(double aSimTime, WsfSensor* aSensorPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      const WsfSensor*      mSensorPtr;
      const WsfPlatform*    mTargetPtr;
      const WsfSensorResult mResult;
   };

   class SensorDetectionChanged : public Event
   {
   public:
      SensorDetectionChanged() = delete;
      SensorDetectionChanged(double aSimTime, WsfSensor* aSensorPtr, size_t aTargetIndex, WsfSensorResult& aResult);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      const WsfSensor*      mSensorPtr;
      const size_t          mTargetIndex;
      const WsfSensorResult mResult;
   };

   class SensorFrequencyChanged : public Event
   {
   public:
      SensorFrequencyChanged() = delete;
      SensorFrequencyChanged(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor*           mSensorPtr;
      const WsfSensorMode* mModePtr;
   };

   class SensorModeActivated : public Event
   {
   public:
      SensorModeActivated() = delete;
      SensorModeActivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor*           mSensorPtr;
      const WsfSensorMode* mModePtr;
   };

   class SensorModeDeactivated : public Event
   {
   public:
      SensorModeDeactivated() = delete;
      SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor*           mSensorPtr;
      const WsfSensorMode* mModePtr;
   };

   class SensorNonOperational : public Event
   {
   public:
      SensorNonOperational() = delete;
      SensorNonOperational(double aSimTime, WsfSensor* aSensorPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor* mSensorPtr;
   };

   class SensorOperational : public Event
   {
   public:
      SensorOperational() = delete;
      SensorOperational(double aSimTime, WsfSensor* aSensorPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor* mSensorPtr;
   };

   class SensorRequestCanceled : public Event
   {
   public:
      SensorRequestCanceled() = delete;
      SensorRequestCanceled(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor*      mSensorPtr;
      const WsfTrack* mTrackPtr;
   };

   class SensorRequestInitiated : public Event
   {
   public:
      SensorRequestInitiated() = delete;
      SensorRequestInitiated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, const WsfTrack* aTrackPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor*           mSensorPtr;
      const WsfSensorMode* mModePtr;
      WsfStringId          mTrackTargetId;
   };

   class SensorRequestUpdated : public Event
   {
   public:
      SensorRequestUpdated() = delete;
      SensorRequestUpdated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, const WsfTrack* aTrackPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor*           mSensorPtr;
      const WsfSensorMode* mModePtr;
      WsfStringId          mTargetId;
   };

   class SensorTrackCoasted : public Event
   {
   public:
      SensorTrackCoasted() = delete;
      SensorTrackCoasted(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor*      mSensorPtr;
      const WsfTrack* mTrackPtr;
   };

   class SensorTrackDropped : public Event
   {
   public:
      SensorTrackDropped() = delete;
      SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor*      mSensorPtr;
      const WsfTrack* mTrackPtr;
   };

   class SensorTrackInitiated : public Event
   {
   public:
      SensorTrackInitiated() = delete;
      SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor*      mSensorPtr;
      const WsfTrack* mTrackPtr;
   };

   class SensorTrackUpdated : public Event
   {
   public:
      SensorTrackUpdated() = delete;
      SensorTrackUpdated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor*      mSensorPtr;
      const WsfTrack* mTrackPtr;
   };

   class SensorTurnedOff : public Event
   {
   public:
      SensorTurnedOff() = delete;
      SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor* mSensorPtr;
   };

   class SensorTurnedOn : public Event
   {
   public:
      SensorTurnedOn() = delete;
      SensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfSensor* mSensorPtr;
   };

   class SimulationComplete : public Event
   {
   public:
      SimulationComplete() = delete;
      SimulationComplete(double aSimTime);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;
   };

   class WeaponFireAborted : public Event
   {
   public:
      WeaponFireAborted() = delete;
      WeaponFireAborted(double aSimTime, WsfWeapon* aWeaponPtr, const WsfTrack* aTargetTrackPtr, double aQuantity);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfWeapon*      mWeaponPtr;
      const WsfTrack* mTargetTrackPtr;
      double          mQuantity;
   };

   class WeaponFireRequested : public Event
   {
   public:
      WeaponFireRequested() = delete;
      WeaponFireRequested(double aSimTime, WsfWeapon* aWeaponPtr, const WsfTrack* aTargetTrackPtr, double aQuantity);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfWeapon*  mWeaponPtr;
      WsfStringId mTargetId;
      double      mQuantity;
   };

   class WeaponFired : public Event
   {
   public:
      WeaponFired() = delete;
      WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      const WsfWeaponEngagement* mEngagementPtr;
      WsfStringId                mTargetId;
   };

   class WeaponHit : public Event
   {
   public:
      WeaponHit() = delete;
      WeaponHit(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      const WsfWeaponEngagement* mEngagementPtr;
      WsfPlatform*               mTargetPtr;
   };

   class WeaponKilled : public Event
   {
   public:
      WeaponKilled() = delete;
      WeaponKilled(double aSimTime, WsfWeapon* aWeaponPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      WsfWeapon* mWeaponPtr;
   };

   class WeaponMissed : public Event
   {
   public:
      WeaponMissed() = delete;
      WeaponMissed(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      const WsfWeaponEngagement* mEngagementPtr;
      WsfPlatform*               mTargetPtr;
   };

   class WeaponTerminated : public Event
   {
   public:
      WeaponTerminated() = delete;
      WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);

      void Write(std::ostream& aStream, Simulation& aSimulation) const override;

   private:
      const WsfWeaponEngagement* mEngagementPtr;
   };

private:
   EventList mEventList;
};

} // namespace engage

#endif
