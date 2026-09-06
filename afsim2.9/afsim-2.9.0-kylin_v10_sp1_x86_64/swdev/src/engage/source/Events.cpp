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

#include "Events.hpp"

#include "Platform.hpp"
#include "Simulation.hpp"
#include "UtAtmosphere.hpp"
#include "UtEarth.hpp"
#include "UtStringUtil.hpp"
#include "UtTime.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfTrack.hpp"
#include "WsfWeaponEngagement.hpp"
#include "observer/WsfWeaponObserver.hpp"

namespace engage
{

// =================================================================================================
// Define accessors to retrieve each known event type from where ever it is stored.

namespace
{
// Helper methods
UtTime PrintTime(double aSimTime, int aTimeFormat = UtTime::FmtS + 5 + UtTime::FmtShowPoint + UtTime::FmtNoSuffix)
{
   return UtTime(aSimTime, aTimeFormat);
}
} // namespace

// =================================================================================================
Events::Events()
   : mEventList()
{
}

// =================================================================================================
void Events::ClearAllEvents()
{
   for (auto& evt : mEventList)
   {
      delete evt;
   }
   mEventList.clear();
}

// =================================================================================================
Events::SensorDetectionAttempt::SensorDetectionAttempt(double           aSimTime,
                                                       WsfSensor*       aSensorPtr,
                                                       WsfPlatform*     aTargetPtr,
                                                       WsfSensorResult& aResult)
   : Event("SENSOR_DETECTION_ATTEMPT")
   , mSensorPtr(aSensorPtr)
   , mTargetPtr(aTargetPtr)
   , mResult(aResult)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorDetectionAttempt::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Target=" << mTargetPtr->GetName();
   if (mSensorPtr->GetModeCount() > 0)
   {
      aStream << " Mode=" << mSensorPtr->GetModeName(mResult.mModeIndex);
   }
   else
   {
      aStream << " Mode=<none>";
   }
   aStream << " Beam=" << mResult.mBeamIndex + 1;
   aStream << " Pd=" << mResult.mPd << " RequiredPd=" << mResult.mRequiredPd << " Detected=" << mResult.Detected();
}

// =================================================================================================
Events::SensorDetectionChanged::SensorDetectionChanged(double           aSimTime,
                                                       WsfSensor*       aSensorPtr,
                                                       size_t           aTargetIndex,
                                                       WsfSensorResult& aResult)
   : Event("SENSOR_DETECTION_CHANGED")
   , mSensorPtr(aSensorPtr)
   , mTargetIndex(aTargetIndex)
   , mResult(aResult)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorDetectionChanged::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Target=";
   WsfStringId nameId = aSimulation.GetPlatformNameId(mTargetIndex);
   if (nameId != 0)
   {
      aStream << nameId;
   }
   else
   {
      aStream << "UNKNOWN-TARGET";
   }

   if (mSensorPtr->GetModeCount() > 0)
   {
      aStream << " Mode=" << mSensorPtr->GetModeName(mResult.mModeIndex);
   }
   else
   {
      aStream << " Mode=<none>";
   }
   aStream << " Beam=" << mResult.mBeamIndex + 1;
   aStream << " Pd=" << mResult.mPd << " RequiredPd=" << mResult.mRequiredPd << " Detected=" << mResult.Detected();
}

// =================================================================================================
Events::SensorFrequencyChanged::SensorFrequencyChanged(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
   : Event("SENSOR_FREQUENCY_CHANGED")
   , mSensorPtr(aSensorPtr)
   , mModePtr(aModePtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorFrequencyChanged::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Mode=" << mModePtr->GetName();

   // Frequency details for each beam/xmtr configured for this mode
   size_t modeIndex = mModePtr->GetModeIndex();
   size_t xmtrCount = mSensorPtr->GetEM_XmtrCount(modeIndex);
   for (unsigned int index = 0; index < xmtrCount; ++index)
   {
      auto& xmtrRef = mSensorPtr->GetEM_Xmtr(modeIndex, index);
      aStream << "  Beam=" << (xmtrRef.GetIndex() + 1);
      aStream << " Frequency Id=" << xmtrRef.GetCurrentAlternateFrequencyId();
      double frequency = xmtrRef.GetFrequency();
      if (frequency >= 1.0E+9)
      {
         aStream << " Frequency: " << frequency * 1.0E-9 << " ghz";
      }
      else if (frequency >= 1.0E+6)
      {
         aStream << " Frequency: " << frequency * 1.0E-6 << " mhz";
      }
      else if (frequency >= 1.0E+3)
      {
         aStream << " Frequency: " << frequency * 1.0E-3 << " khz";
      }
      else
      {
         aStream << " Frequency: " << frequency << " hz";
      }
   }
}

// =================================================================================================
Events::SensorModeActivated::SensorModeActivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
   : Event("SENSOR_MODE_ACTIVATED")
   , mSensorPtr(aSensorPtr)
   , mModePtr(aModePtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorModeActivated::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Mode=" << mModePtr->GetName();
}

// =================================================================================================
Events::SensorModeDeactivated::SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
   : Event("SENSOR_MODE_DEACTIVATED")
   , mSensorPtr(aSensorPtr)
   , mModePtr(aModePtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorModeDeactivated::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Mode=" << mModePtr->GetName();
}

// =================================================================================================
Events::SensorNonOperational::SensorNonOperational(double aSimTime, WsfSensor* aSensorPtr)
   : Event("SENSOR_NON_OPERATIONAL")
   , mSensorPtr(aSensorPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorNonOperational::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
}

// =================================================================================================
Events::SensorOperational::SensorOperational(double aSimTime, WsfSensor* aSensorPtr)
   : Event("SENSOR_OPERATIONAL")
   , mSensorPtr(aSensorPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorOperational::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
}

// =================================================================================================
Events::SensorRequestCanceled::SensorRequestCanceled(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
   : Event("SENSOR_REQUEST_CANCELED")
   , mSensorPtr(aSensorPtr)
   , mTrackPtr(aTrackPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorRequestCanceled::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Track=" << mTrackPtr->GetTargetName().GetString();
}

// =================================================================================================
Events::SensorRequestInitiated::SensorRequestInitiated(double          aSimTime,
                                                       WsfSensor*      aSensorPtr,
                                                       WsfSensorMode*  aModePtr,
                                                       const WsfTrack* aTrackPtr)
   : Event("SENSOR_REQUEST_INITIATED")
   , mSensorPtr(aSensorPtr)
   , mModePtr(aModePtr)
   , mTrackTargetId(aTrackPtr->GetTargetName())
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorRequestInitiated::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Mode=" << mModePtr->GetName();
   aStream << " Track=" << mTrackTargetId.GetString();
}

// =================================================================================================
Events::SensorRequestUpdated::SensorRequestUpdated(double          aSimTime,
                                                   WsfSensor*      aSensorPtr,
                                                   WsfSensorMode*  aModePtr,
                                                   const WsfTrack* aTrackPtr)
   : Event("SENSOR_REQUEST_UPDATED")
   , mSensorPtr(aSensorPtr)
   , mModePtr(aModePtr)
   , mTargetId(aTrackPtr->GetTargetName())
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorRequestUpdated::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Mode=" << mModePtr->GetName();
   aStream << " Track=" << mTargetId;
}

// =================================================================================================
Events::SensorTrackCoasted::SensorTrackCoasted(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
   : Event("SENSOR_TRACK_COASTED")
   , mSensorPtr(aSensorPtr)
   , mTrackPtr(aTrackPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorTrackCoasted::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Track=" << mTrackPtr->GetTargetName().GetString();
}

// =================================================================================================
Events::SensorTrackDropped::SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
   : Event("SENSOR_TRACK_DROPPED")
   , mSensorPtr(aSensorPtr)
   , mTrackPtr(aTrackPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorTrackDropped::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Track=" << mTrackPtr->GetTargetName().GetString();
}

// =================================================================================================
Events::SensorTrackInitiated::SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
   : Event("SENSOR_TRACK_INITIATED")
   , mSensorPtr(aSensorPtr)
   , mTrackPtr(aTrackPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorTrackInitiated::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Track=" << mTrackPtr->GetTargetName().GetString();
}

// =================================================================================================
Events::SensorTrackUpdated::SensorTrackUpdated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
   : Event("SENSOR_TRACK_UPDATED")
   , mSensorPtr(aSensorPtr)
   , mTrackPtr(aTrackPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorTrackUpdated::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
   aStream << " Track=" << mTrackPtr->GetTargetName().GetString();
}

// =================================================================================================
Events::SensorTurnedOff::SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr)
   : Event("SENSOR_TURNED_OFF")
   , mSensorPtr(aSensorPtr)
{
   mEventTime = aSimTime;
}
// =================================================================================================
void Events::SensorTurnedOff::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
}

// =================================================================================================
Events::SensorTurnedOn::SensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr)
   : Event("SENSOR_TURNED_ON")
   , mSensorPtr(aSensorPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SensorTurnedOn::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Sensor=" << mSensorPtr->GetName();
}

// =================================================================================================
Events::SimulationComplete::SimulationComplete(double aSimTime)
   : Event("SIMULATION_COMPLETE")
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::SimulationComplete::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
}

// =================================================================================================
Events::WeaponFireAborted::WeaponFireAborted(double          aSimTime,
                                             WsfWeapon*      aWeaponPtr,
                                             const WsfTrack* aTargetTrackPtr,
                                             double          aQuantity)
   : Event("WEAPON_FIRE_ABORTED")
   , mWeaponPtr(aWeaponPtr)
   , mTargetTrackPtr(aTargetTrackPtr)
   , mQuantity(aQuantity)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::WeaponFireAborted::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Weapon=" << mWeaponPtr->GetName();
   aStream << " TargetTrack=" << mTargetTrackPtr->GetTargetName();
   aStream << " Quality=" << mQuantity;
}

// =================================================================================================
Events::WeaponFireRequested::WeaponFireRequested(double          aSimTime,
                                                 WsfWeapon*      aWeaponPtr,
                                                 const WsfTrack* aTargetTrackPtr,
                                                 double          aQuantity)
   : Event("WEAPON_FIRE_REQUESTED")
   , mWeaponPtr(aWeaponPtr)
   , mTargetId(aTargetTrackPtr->GetTargetName())
   , mQuantity(aQuantity)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::WeaponFireRequested::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Weapon=" << mWeaponPtr->GetName();
   aStream << " TargetTrack=" << mTargetId;
   aStream << " Quality=" << mQuantity;
}

// =================================================================================================
Events::WeaponKilled::WeaponKilled(double aSimTime, WsfWeapon* aWeaponPtr)
   : Event("WEAPON_KILLED")
   , mWeaponPtr(aWeaponPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::WeaponKilled::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << mEventTime;
   aStream << ' ' << mEventType;
   aStream << " Weapon=" << mWeaponPtr->GetName();
}

// =================================================================================================
Events::WeaponFired::WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr)
   : Event("WEAPON_FIRED")
   , mEngagementPtr(aEngagementPtr)
   , mTargetId(aTargetTrackPtr->GetTargetName())
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::WeaponFired::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << PrintTime(mEngagementPtr->GetStartTime());
   aStream << ' ' << mEventType;
   aStream << " Missile=" << mEngagementPtr->GetWeaponSystemName();
   //<< " Track=" << mTargetTrackPtr->GetTrackId()
   aStream << " Target=" << mTargetId;
}

// =================================================================================================
Events::WeaponMissed::WeaponMissed(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPtr)
   : Event("WEAPON_MISSED")
   , mEngagementPtr(aEngagementPtr)
   , mTargetPtr(aTargetPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::WeaponMissed::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << PrintTime(mEngagementPtr->GetCompletionTime());
   aStream << ' ' << mEventType;
   aStream << " Missile=" << mEngagementPtr->GetWeaponSystemName();
   aStream << " Target=" << mTargetPtr->GetName();
   aStream << " Miss_Distance=" << mEngagementPtr->GetMissDistance(mTargetPtr) << " m";
}

// =================================================================================================
Events::WeaponHit::WeaponHit(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, WsfPlatform* aTargetPtr)
   : Event("WEAPON_HIT")
   , mEngagementPtr(aEngagementPtr)
   , mTargetPtr(aTargetPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::WeaponHit::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << PrintTime(mEngagementPtr->GetCompletionTime());
   aStream << ' ' << mEventType;
   aStream << " Missile=" << mEngagementPtr->GetWeaponSystemName();
   aStream << " Target=" << mTargetPtr->GetName();
   aStream << " Miss_Distance=" << mEngagementPtr->GetMissDistance(mTargetPtr) << " m";
}

// =================================================================================================
Events::WeaponTerminated::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
   : Event("WEAPON_TERMINATED")
   , mEngagementPtr(aEngagementPtr)
{
   mEventTime = aSimTime;
}

// =================================================================================================
void Events::WeaponTerminated::Write(std::ostream& aStream, Simulation& aSimulation) const
{
   aStream << "T=" << PrintTime(mEngagementPtr->GetCompletionTime());
   aStream << ' ' << mEventType;
   aStream << " Missile=" << mEngagementPtr->GetWeaponSystemName();
   aStream << " Miss_Distance=" << mEngagementPtr->GetMissDistance(mEngagementPtr->GetTargetPlatform()) << " m";
   aStream << " Intercept Pk=" << mEngagementPtr->GetWeaponEffects()->GetInterceptPk();
}

} // namespace engage
