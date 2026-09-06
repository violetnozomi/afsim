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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfWeaponTrackProcessor.hpp"

#include <algorithm>
#include <cfloat>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtVec3.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfWeaponEngagement.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageHandler.hpp"

// =================================================================================================
WsfWeaponTrackProcessor::WsfWeaponTrackProcessor(WsfScenario& aScenario)
   : WsfProcessor(aScenario)
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), "WsfProcessor", "PROCESSOR"))
   , mMessageHandlerPtr(new WsfScriptMessageHandler(*mContextPtr))
   , mSensorList()
   , mEngagementPhase(cMID_COURSE)
   , mIgnoreUplinkTargetChange(false)
   , mUplinkRequired(false)
   , mRequireInterceptCalculation(false)
   , mCoastTime(0.0)
   , mSwitchTime(0.0)
   , mSwitchRange(0.0)
   , mCmdTrackPtr(nullptr)
   , mSnrTrackPtr(nullptr)
   , mTimeToGo(0.0)
   , mRangeToGo(0.0)
   , mRangeToTgt(0.0)
   , mLastCmdTrackUpdateTime(0.0)
   , mLastSnrTrackUpdateTime(0.0)
{
   SetUpdateInterval(1.0);
}

// =================================================================================================
// protected
WsfWeaponTrackProcessor::WsfWeaponTrackProcessor(const WsfWeaponTrackProcessor& aSrc)
   : WsfProcessor(aSrc)
   , mContextPtr(new WsfScriptContext(*(aSrc.mContextPtr)))
   , mMessageHandlerPtr(new WsfScriptMessageHandler(*(aSrc.mMessageHandlerPtr), *mContextPtr))
   , mSensorList(aSrc.mSensorList)
   , mEngagementPhase(aSrc.mEngagementPhase)
   , mIgnoreUplinkTargetChange(aSrc.mIgnoreUplinkTargetChange)
   , mUplinkRequired(aSrc.mUplinkRequired)
   , mRequireInterceptCalculation(aSrc.mRequireInterceptCalculation)
   , mCoastTime(aSrc.mCoastTime)
   , mSwitchTime(aSrc.mSwitchTime)
   , mSwitchRange(aSrc.mSwitchRange)
   , mCmdTrackPtr(nullptr)
   , mSnrTrackPtr(nullptr)
   , mTimeToGo(0.0)
   , mRangeToGo(0.0)
   , mRangeToTgt(0.0)
   , mLastCmdTrackUpdateTime(0.0)
   , mLastSnrTrackUpdateTime(0.0)
{
}

// =================================================================================================
// virtual
WsfWeaponTrackProcessor::~WsfWeaponTrackProcessor()
{
   delete mCmdTrackPtr;
   delete mSnrTrackPtr;
}

// =================================================================================================
// virtual
WsfProcessor* WsfWeaponTrackProcessor::Clone() const
{
   return new WsfWeaponTrackProcessor(*this);
}

// =================================================================================================
// virtual
bool WsfWeaponTrackProcessor::Initialize(double aSimTime)
{
   bool ok = WsfProcessor::Initialize(aSimTime);
   ok &= mContextPtr->Initialize(aSimTime, GetPlatform(), this);
   ok &= mMessageHandlerPtr->Initialize(aSimTime, GetPlatform());

   mRequireInterceptCalculation = false;
   if ((mSwitchTime > 0.0) || // switch_time <time>
       (mSwitchRange > 0.0))  // switch range <dist>
   {
      mRequireInterceptCalculation = true;
   }
   for (std::vector<Sensor>::iterator sli = mSensorList.begin(); sli != mSensorList.end(); ++sli)
   {
      Sensor& sensor    = *sli;
      sensor.mSensorPtr = GetPlatform()->GetComponent<WsfSensor>(sensor.mSensorName);
      if (sensor.mSensorPtr == nullptr)
      {
         auto out = ut::log::error() << "Sensor does not exist.";
         out.AddNote() << "Sensor: " << sensor.mSensorName;
         ok = false;
      }
      if ((sensor.mTurnOnTime > 0.0) || // turn_on_sensor <sensor> at_time <time> before_intercept
          (sensor.mTurnOnRange > 0.0))  // turn_on_sensor <sensor> at_range <dist> before_intercept
      {
         mRequireInterceptCalculation = true;
      }
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfWeaponTrackProcessor::Initialize2(double aSimTime)
{
   bool ok = WsfProcessor::Initialize2(aSimTime);
   ok &= mContextPtr->Initialize2(aSimTime);

   // Get the initial target from the hand-off.
   const WsfTrack* trackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
   if (trackPtr != nullptr)
   {
      trackPtr = GetPlatform()->GetMasterTrackList().FindTrack(trackPtr->GetTrackId());
      if (trackPtr != nullptr)
      {
         delete mCmdTrackPtr;
         mCmdTrackPtr            = trackPtr->Clone();
         mLastCmdTrackUpdateTime = std::max(mCmdTrackPtr->GetUpdateTime(), aSimTime);
      }
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfWeaponTrackProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "turn_on_sensor")
   {
      std::string sensorName;
      std::string dimension;
      double      turnOnTime(0.0);
      double      turnOnRange(0.0);
      bool        useTargetLocation(false);
      std::string turnOnCondition;

      aInput.ReadValue(sensorName);

      aInput.ReadValue(dimension);
      if (dimension == "at_range")
      {
         aInput.ReadValueOfType(turnOnRange, UtInput::cLENGTH);
         aInput.ValueGreater(turnOnRange, 0.0);
      }
      else
      {
         if (dimension != "at_time")
         {
            aInput.PushBack(dimension);
         }
         aInput.ReadValueOfType(turnOnTime, UtInput::cTIME);
         aInput.ValueGreater(turnOnTime, 0.0);
      }

      aInput.ReadValue(turnOnCondition);
      if (turnOnCondition == "after_launch")
      {
         if (turnOnRange > 0.0)
         {
            throw UtInput::BadValue(aInput, "Expected 'before_intercept' or 'to_target'");
         }
         turnOnTime = -turnOnTime;
      }
      else if (turnOnCondition == "before_intercept")
      {
      }
      else if (turnOnCondition == "to_target")
      {
         if (turnOnTime > 0.0)
         {
            throw UtInput::BadValue(aInput, "Expected 'before_intercept' or 'after_launch'");
         }
         useTargetLocation = true;
      }
      else
      {
         throw UtInput::BadValue(aInput,
                                 "'turn_on_sensor' must include 'after_launch' or 'before_intercept' or 'to_target'");
      }

      Sensor sensor;
      sensor.mSensorName        = sensorName;
      sensor.mSensorPtr         = nullptr;
      sensor.mTurnOnTime        = turnOnTime;
      sensor.mTurnOnRange       = turnOnRange;
      sensor.mUseTargetLocation = useTargetLocation;
      mSensorList.push_back(sensor);
   }
   else if (command == "coast_time")
   {
      aInput.ReadValueOfType(mCoastTime, UtInput::cTIME);
      aInput.ValueGreater(mCoastTime, 0.0);
   }
   else if (command == "switch_time")
   {
      aInput.ReadValueOfType(mSwitchTime, UtInput::cTIME);
      aInput.ValueGreater(mSwitchTime, 0.0);
   }
   else if (command == "switch_range")
   {
      aInput.ReadValueOfType(mSwitchRange, UtInput::cLENGTH);
      aInput.ValueGreater(mSwitchRange, 0.0);
   }
   else if (command == "ignore_uplink_target_change")
   {
      mIgnoreUplinkTargetChange = true;
   }
   else if (command == "uplink_required")
   {
      aInput.ReadValue(mUplinkRequired);
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
   }
   else if (mMessageHandlerPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfWeaponTrackProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool        messageProcessed = false;
   WsfStringId messageType      = aMessage.GetType();
   if (messageType == WsfTrackMessage::GetTypeId())
   {
      messageProcessed               = true;
      const WsfTrackMessage& message = dynamic_cast<const WsfTrackMessage&>(aMessage);
      ProcessTrackUpdate(aSimTime, *(message.GetTrack()));
   }
   else if (messageType == WsfTrackDropMessage::GetTypeId())
   {
      messageProcessed                   = true;
      const WsfTrackDropMessage& message = dynamic_cast<const WsfTrackDropMessage&>(aMessage);
      ProcessTrackDrop(aSimTime, message.GetTrackId());
   }

   // We always let the message handler have a check at the message - even if we have already processed it.
   messageProcessed |= mMessageHandlerPtr->ProcessMessage(aSimTime, aMessage);
   return messageProcessed;
}

// =================================================================================================
// virtual
void WsfWeaponTrackProcessor::Update(double aSimTime)
{
   UpdateTargetTrack(aSimTime, true);

   // Turn on any seekers that need to be activated.
   for (std::vector<Sensor>::iterator sli = mSensorList.begin(); sli != mSensorList.end(); ++sli)
   {
      Sensor& sensor = *sli;
      if (sensor.mSensorPtr->IsOperational() && (!sensor.mSensorPtr->IsTurnedOn()))
      {
         bool turnOn = false;
         if (sensor.mTurnOnTime > 0.0)
         {
            turnOn = (mTimeToGo < sensor.mTurnOnTime);
         }
         else if (sensor.mTurnOnTime < 0.0)
         {
            turnOn = ((aSimTime - GetPlatform()->GetCreationTime()) > (-sensor.mTurnOnTime));
         }
         else if (sensor.mTurnOnRange > 0.0)
         {
            if (sensor.mUseTargetLocation)
            {
               turnOn = (mRangeToTgt < sensor.mTurnOnRange);
            }
            else
            {
               turnOn = (mRangeToGo < sensor.mTurnOnRange);
            }
         }

         if (turnOn)
         {
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Turning on sensor.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Processor: " << GetName();
               out.AddNote() << "Sensor: " << sensor.mSensorPtr->GetName();
               if (sensor.mUseTargetLocation)
               {
                  out.AddNote() << "RTT: " << mRangeToTgt;
               }
               else
               {
                  out.AddNote() << "TTG: " << mTimeToGo;
                  out.AddNote() << "RTG: " << mRangeToGo;
               }
            }
            GetSimulation()->TurnPartOn(aSimTime, sensor.mSensorPtr);
            if (mCmdTrackPtr != nullptr)
            {
               // TODO may want to update the seeker tracking request as we get uplinks in case
               // the seeker is not yet locked on
               sensor.mSensorPtr->StartTracking(aSimTime, *mCmdTrackPtr, nullptr);
            }
         }
      }
   }

   // Self-destruct if data hasn't been received within the specified coast time.
   if (mCoastTime > 0.0)
   {
      WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*GetPlatform());
      if (engagementPtr != nullptr)
      {
         double lastTrackUpdateTime = std::max(mLastCmdTrackUpdateTime, mLastSnrTrackUpdateTime);
         if (mUplinkRequired)
         {
            lastTrackUpdateTime = mLastCmdTrackUpdateTime;
         }
         if ((aSimTime - lastTrackUpdateTime) > mCoastTime)
         {
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Terminating due to coast time exceeded.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Processor: " << GetName();
            }
            engagementPtr->SetExtendedResult("Coast time exceeded");
            engagementPtr->Terminate(aSimTime);
         }
      }
   }

   mContextPtr->Update(aSimTime); // Let the script 'on_update' have a chance.
}

// =================================================================================================
//! Determine minimum-time intercept point.
//!
//! Given a weapon location and speed and a target location and velocity vector,
//! determine the intercept location (and therefore the desired weapon heading
//! and the time-to-intercept). This method assumes the bodies move at a constant
//! speed and that the weapon can turn instantly to the best heading.
//!
//! @param aWpnLocWCS [input]  The WCS location of the weapon.
//! @param aWpnSpeed  [input]  The speed of the weapon.
//! @param aTgtLocWCS [input]  The WCS location of the target.
//! @param aTgtVelWCS [input]  The WCS velocity of the target.
//! @param aIntLocWCS [output] The WCS location of the intercept. (valid only if the
//!                            function return value is 'true')
//!
//! @returns 'true' if an intercept occurred or 'false' if not.
//!
//! @note: This routine will allow a target to run into a zero velocity weapon.

// protected static
bool WsfWeaponTrackProcessor::FindIntercept(const double aWpnLocWCS[3],
                                            double       aWpnSpeed,
                                            const double aTgtLocWCS[3],
                                            const double aTgtVelWCS[3],
                                            double       aIntLocWCS[3])
{
   // Compute the WCS location of the target with respect to the weapon.

   double wpnToTgtLocWCS[3];
   UtVec3d::Subtract(wpnToTgtLocWCS, aTgtLocWCS, aWpnLocWCS);

   // Assume a position vector from the weapon to the target.  Compute the radial
   // (along the vector) and tangential (perpendicular to the vector) components of
   // the target velocity.

   double radUnitVec[3];
   double wpnToTgtRange = UtVec3d::Normalize(radUnitVec, wpnToTgtLocWCS);
   if (wpnToTgtRange < 1.0)
   {
      // The weapon and target are so close.  Just set the intercept location to be the
      // target location.
      UtVec3d::Set(aIntLocWCS, aTgtLocWCS);
      return true;
   }

   double tgtRadVelVec[3];
   double tgtTanVelVec[3];
   double tgtRadVel = UtVec3d::DotProduct(aTgtVelWCS, radUnitVec);
   UtVec3d::Multiply(tgtRadVelVec, radUnitVec, tgtRadVel);
   UtVec3d::Subtract(tgtTanVelVec, aTgtVelWCS, tgtRadVelVec);

   // Compute the components of the weapon velocity vector.
   //
   // The weapon tangential velocity must match the target tangential velocity.
   // The weapon radial velocity is whatever part of the velocity that remains
   // after subtracting out the tangential velocity.

   double wpnTanVel = UtVec3d::Magnitude(tgtTanVelVec);
   if (wpnTanVel > aWpnSpeed)
   {
      // The required tangential speed is greater than or equal to the total weapon speed.
      // There is nothing left for the closing radial component.
      return false;
   }

   double wpnRadVel = sqrt((aWpnSpeed * aWpnSpeed) - (wpnTanVel * wpnTanVel));
   double wpnRadVelVec[3];
   UtVec3d::Multiply(wpnRadVelVec, radUnitVec, wpnRadVel);

   // Determine the rate of closure along the radial vector.

   double cloRadVelVec[3];
   UtVec3d::Subtract(cloRadVelVec, wpnRadVelVec, tgtRadVelVec);

   // Make sure the rate of closure is positive toward the target.

   double cloRadVel = UtVec3d::DotProduct(cloRadVelVec, radUnitVec);
   if (cloRadVel <= 0.0)
   {
      return false;
   }

   // Intercept is possible, so compute the intercept location.
   double timeToIntercept = wpnToTgtRange / cloRadVel;
   double tgtIncWCS[3];
   UtVec3d::Multiply(tgtIncWCS, aTgtVelWCS, timeToIntercept);
   UtVec3d::Add(aIntLocWCS, aTgtLocWCS, tgtIncWCS);
   return true;
}

// =================================================================================================
// virtual protected
void WsfWeaponTrackProcessor::GetInterceptData(double aSimTime, double& aTimeToGo, double& aRangeToGo, double& aRangeToTgt)
{
   // Assume no intercept.
   aTimeToGo   = 1.0E+20;
   aRangeToGo  = 1.0E+20;
   aRangeToTgt = 1.0E+20;

   const WsfTrack* targetPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
   if (targetPtr != nullptr)
   {
      double wpnLocWCS[3];
      double wpnVelWCS[3];
      double tgtLocWCS[3];
      double tgtVelWCS[3] = {0.0, 0.0, 0.0};

      GetPlatform()->GetLocationWCS(wpnLocWCS);
      GetPlatform()->GetVelocityWCS(wpnVelWCS);
      double wpnSpd = UtVec3d::Magnitude(wpnVelWCS);

      if (targetPtr->GetExtrapolatedLocationWCS(aSimTime, tgtLocWCS))
      {
         if (targetPtr->VelocityValid())
         {
            targetPtr->GetVelocityWCS(tgtVelWCS);
         }

         // Find the minimum-time intercept location.
         double intLocWCS[3];
         if (FindIntercept(wpnLocWCS, wpnSpd, tgtLocWCS, tgtVelWCS, intLocWCS))
         {
            // Compute the range and time to the intercept location.
            double wpnToIntLocWCS[3];
            UtVec3d::Subtract(wpnToIntLocWCS, intLocWCS, wpnLocWCS);
            aRangeToGo = UtVec3d::Magnitude(wpnToIntLocWCS);
            aTimeToGo  = aRangeToGo / std::max(1.0, wpnSpd);

            // Compute the range and time to the target location.
            double wpnToTgtLocWCS[3];
            UtVec3d::Subtract(wpnToTgtLocWCS, tgtLocWCS, wpnLocWCS);
            aRangeToTgt = UtVec3d::Magnitude(wpnToTgtLocWCS);
         }
      }
      else
      {
         if (DebugEnabled())
         {
            ut::log::warning() << "Unable to calculate intercept location. Target track may not be a valid 3-D track.";
         }
      }
   }
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Intercept data:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "TTG: " << aTimeToGo;
      out.AddNote() << "RTG: " << aRangeToGo;
   }
}

// =================================================================================================
//! Update the track list and, if necessary, the current target state.
//!
//! @param aSimTime [input] The current simulation time.
//! @param aTrack   [input] The updated track.
// virtual protected
void WsfWeaponTrackProcessor::ProcessTrackUpdate(double aSimTime, const WsfTrack& aTrack)
{
   size_t targetIndex = GetCurrentTargetIndex();
   if (mIgnoreUplinkTargetChange)
   {
      // Don't process tracks that aren't of the current target
      if (targetIndex != 0 && targetIndex != aTrack.GetTargetIndex())
      {
         return;
      }
   }

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Process track update:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "Update track: " << aTrack.GetTrackId();
      out.AddNote() << "Originator: " << aTrack.GetOriginatorNameId();
      if (aTrack.GetSensorNameId() != 0)
      {
         out.AddNote() << "Sensor: " << aTrack.GetSensorNameId();
         out.AddNote() << "Mode: " << aTrack.GetSensorModeId();
      }
      out.AddNote() << "Age: " << aSimTime - aTrack.GetUpdateTime();
   }

   // WsfLocalTrack* trackPtr = GetPlatform()->GetMasterTrackList().FindTrack(aTrack.GetTrackId());
   // if (trackPtr == 0)
   //{
   //    trackPtr = new WsfLocalTrack(aTrack);
   //    trackPtr->SetTrackManager(&GetPlatform()->GetTrackManager());
   //    GetPlatform()->GetMasterTrackList().AddTrack(trackPtr);
   //    WsfObserver::LocalTrackInitiated(aSimTime, GetPlatform(), trackPtr, &aTrack);
   // }
   // else
   //{
   //    trackPtr->UpdateFromMeasurement(aSimTime, aTrack);
   //    WsfObserver::LocalTrackUpdated(aSimTime, GetPlatform(), trackPtr, &aTrack);
   // }
   GetPlatform()->GetTrackManager().AddTrackReport(aSimTime, aTrack);
   WsfLocalTrack* trackPtr = GetPlatform()->GetTrackManager().FindCorrelatedTrack(aTrack.GetTrackId());


   // NOTE: When setting the respective 'last valid track update time', it is set to the MAXIMUM of
   // the track update time and the time the platform was created. The reason is that the incoming
   // track may be the hand-off track, which may be 'relatively' old because of launch delays, etc.
   // Otherwise we may get a coast time exceeded before we even get a chance for the first update!.

   // If the track is from the local sensor then set it as the current sensor track.
   // If it is the track from command guidance then set it as the current command track.

   // NOTE: Tracks that come in from the Link-16 J11 interface will always have the owning platform ID
   //       equal to this platform ID. These should be treated as command (uplink) tracks.
   //       (See l16/source/Interface_J11::InFlightTargetUpdate).

   WsfTrackId newTrackId = aTrack.GetTrackId();
   if ((aTrack.GetSensorNameId() != 0) && (newTrackId.GetOwningPlatformId() == GetPlatform()->GetNameId()))
   {
      // The incoming track is from a local sensor. It will be accepted as the new
      // sensor track as dictated by the rules in the following *if* block.

      bool   acceptTrack    = false;
      size_t cmdTargetIndex = (mCmdTrackPtr == nullptr) ? 0 : (mCmdTrackPtr->GetTargetIndex());
      if (cmdTargetIndex != 0)
      {
         // We have been told by the uplink track to purse a specific target.
         // Accept the incoming track if it is for the specified target.
         acceptTrack = (aTrack.GetTargetIndex() == cmdTargetIndex);
      }
      else if ((targetIndex == 0) || (aTrack.GetTargetIndex() == targetIndex))
      {
         // The incoming track refers to the same target as the 'current target' OR there is no
         // current target. Accept the track.
         acceptTrack = true;
      }

      if (acceptTrack)
      {
         delete mSnrTrackPtr;
         mSnrTrackPtr            = trackPtr->Clone();
         mLastSnrTrackUpdateTime = std::max(mLastSnrTrackUpdateTime, mSnrTrackPtr->GetUpdateTime());
         mLastSnrTrackUpdateTime = std::max(mLastSnrTrackUpdateTime, GetPlatform()->GetCreationTime());
      }
   }
   else if (trackPtr->GetTargetIndex() != GetPlatform()->GetIndex())
   {
      // Off-board track is not a track of me so we'll assume it is the command track.
      delete mCmdTrackPtr;
      mCmdTrackPtr            = trackPtr->Clone();
      mLastCmdTrackUpdateTime = std::max(mLastCmdTrackUpdateTime, mCmdTrackPtr->GetUpdateTime());
      mLastCmdTrackUpdateTime = std::max(mLastCmdTrackUpdateTime, GetPlatform()->GetCreationTime());
   }

   UpdateTargetTrack(aSimTime, false);
}

// =================================================================================================
//! Drop a track from the track list and update the current target state.
//!
//! @param aSimTime [input] The current simulation time.
//! @param aTrackId [input] The track ID of the track being dropped.
// virtual protected
void WsfWeaponTrackProcessor::ProcessTrackDrop(double aSimTime, const WsfTrackId& aTrackId)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Processing track drop.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "Dropped track: " << aTrackId;
   }

   // Remove the track from the master track list (but don't delete it yet).
   auto trackPtr = GetPlatform()->GetMasterTrackList().RemoveTrack(aTrackId);
   if (trackPtr != nullptr)
   {
      WsfObserver::LocalTrackDropped(GetSimulation())(aSimTime, GetPlatform(), trackPtr.get());
   }

   // Determine if the track was being used to formulate the current target track.

   bool updateTargetTrack = false;
   if (trackPtr.get() == mCmdTrackPtr)
   {
      mCmdTrackPtr      = nullptr;
      updateTargetTrack = true;
   }
   else if (trackPtr.get() == mSnrTrackPtr)
   {
      mSnrTrackPtr      = nullptr;
      updateTargetTrack = true;
   }

   // Clear the current target if the track being dropped was for the current target.

   if (aTrackId == GetPlatform()->GetTrackManager().GetCurrentTargetTrackId())
   {
      GetPlatform()->GetTrackManager().ClearCurrentTarget();
      updateTargetTrack = true;
   }

   if (updateTargetTrack)
   {
      UpdateTargetTrack(aSimTime, false);
   }
}

// =================================================================================================
//! Form the current perception of the target.
//!
//! @param aSimTime [input] The current simulation time.
//! @param aAdjustUpdateInterval [input] If 'true' and in mid-course, this method will
//! adjust the update interval (if necessary) to better resolve the time at which the
//! switch to terminal phase should occur.
// virtual protected
void WsfWeaponTrackProcessor::UpdateTargetTrack(double aSimTime, bool aAdjustUpdateInterval)
{
   double timeToGo  = 0.0;
   double rangeToGo = 0.0;

   if (mRequireInterceptCalculation)
   {
      double rangeToTgt = 0.0;
      GetInterceptData(aSimTime, timeToGo, rangeToGo, rangeToTgt);
      mTimeToGo   = timeToGo;
      mRangeToGo  = rangeToGo;
      mRangeToTgt = rangeToTgt;
   }

   // Determine if it is time to switch to terminal guidance.

   EngagementPhase lastEngagementPhase = mEngagementPhase;
   if ((mSwitchTime > 0.0) || (mSwitchRange > 0.0))
   {
      if (timeToGo < mSwitchTime)
      {
         mEngagementPhase = cTERMINAL;
      }
      else if (rangeToGo < mSwitchRange)
      {
         mEngagementPhase = cTERMINAL;
      }
      else if (aAdjustUpdateInterval)
      {
         // Not yet time to switch to terminal guidance and we're being called
         // from Update().  If we're getting close then decrease the update interval
         // so we get better resolution.

         double updateInterval = GetUpdateInterval();
         double timeToSwitch   = 1.0E+20;
         if (mSwitchTime > 0.0)
         {
            timeToSwitch = std::min(timeToSwitch, timeToGo - mSwitchTime);
         }
         if (mSwitchRange > 0.0)
         {
            double wpnVelWCS[3];
            GetPlatform()->GetVelocityWCS(wpnVelWCS);
            double wpnSpeed = std::max(UtVec3d::Magnitude(wpnVelWCS), 1.0);
            timeToSwitch    = std::min(timeToSwitch, (rangeToGo - mSwitchRange) / wpnSpeed);
         }
         if (timeToSwitch < (1.5 * updateInterval))
         {
            timeToSwitch = std::max(0.05, 0.5 * timeToSwitch);
            if (timeToSwitch < updateInterval)
            {
               SetUpdateInterval(timeToSwitch);
            }
         }
      }
   }
   else if (mSnrTrackPtr != nullptr)
   {
      // switch_range or switch_time not specified, but a local sensor is feeding us a track.
      // For now we'll treat this as though the seeker has acquired lock and automatically
      // switch to terminal guidance.

      mEngagementPhase = cTERMINAL;
   }

   if (DebugEnabled() && (mEngagementPhase != lastEngagementPhase))
   {
      if (mEngagementPhase == cTERMINAL)
      {
         auto out = ut::log::debug() << "Switched to terminal phase.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
      }
   }

   if (mEngagementPhase == cMID_COURSE)
   {
      if (mCmdTrackPtr != nullptr)
      {
         SetCurrentTarget(aSimTime, mCmdTrackPtr);
      }
   }
   else if (mEngagementPhase == cTERMINAL)
   {
      // TODO - need to put some blend logic in here...
      if (mSnrTrackPtr != nullptr)
      {
         SetCurrentTarget(aSimTime, mSnrTrackPtr);
      }
      else if (mCmdTrackPtr != nullptr)
      {
         SetCurrentTarget(aSimTime, mCmdTrackPtr);
      }
   }

   if ((mEngagementPhase == cTERMINAL) && DebugEnabled())
   {
      const WsfTrack* trackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
      if (trackPtr != nullptr)
      {
         // Truth location
         double       tgtLocWCS[3] = {0.0, 0.0, 0.0};
         WsfPlatform* tgtPtr       = GetSimulation()->GetPlatformByIndex(trackPtr->GetTargetIndex());
         if (tgtPtr != nullptr)
         {
            tgtPtr->GetLocationWCS(tgtLocWCS);
         }
         double lat;
         double lon;
         double alt;
         auto   out = ut::log::debug() << "Track update for target.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
         out.AddNote() << "Target: " << trackPtr->GetTargetName();
         { // RAII block
            auto note1 = out.AddNote() << "True LLA: ";
            if (UtVec3d::Equals(tgtLocWCS, 0.0))
            {
               note1 << "<UNKNOWN>";
            }
            else
            {
               UtEntity::ConvertWCSToLLA(tgtLocWCS, lat, lon, alt);
               note1.AddNote() << "Lat: " << lat;
               note1.AddNote() << "Lon: " << lon;
               note1.AddNote() << "Alt: " << alt;
            }
         }

         // Track location, if valid 3D
         double trkLocWCS[3];
         if (trackPtr->GetExtrapolatedLocationWCS(aSimTime, trkLocWCS))
         {
            UtEntity::ConvertWCSToLLA(trkLocWCS, lat, lon, alt);
         }
         else
         {
            double locationWCS[3];
            trackPtr->GetOriginatorLocationWCS(locationWCS);
            UtEntity originator;
            originator.SetLocationWCS(locationWCS);

            double trackLocNED[3] = {0.0, 0.0, 0.0};
            if (trackPtr->RangeValid() && trackPtr->BearingValid())
            {
               if (trackPtr->ElevationValid())
               {
                  double ne      = trackPtr->GetRange() * cos(trackPtr->GetElevation());
                  double d       = -trackPtr->GetRange() * sin(trackPtr->GetElevation());
                  trackLocNED[0] = ne * cos(trackPtr->GetBearing());
                  trackLocNED[1] = ne * sin(trackPtr->GetBearing());
                  trackLocNED[2] = d;
               }
               else
               {
                  double ne      = trackPtr->GetRange();
                  trackLocNED[0] = ne * cos(trackPtr->GetBearing());
                  trackLocNED[1] = ne * sin(trackPtr->GetBearing());
                  trackLocNED[2] = 0.0;
               }
            }

            originator.ConvertNEDToLLA(trackLocNED, lat, lon, alt);
         }

         auto note = out.AddNote() << "Track LLA: ";
         note.AddNote() << "Lat: " << lat;
         note.AddNote() << "Lon: " << lon;
         note.AddNote() << "Alt: " << alt;
         note.AddNote() << "Updated at T = " << trackPtr->GetUpdateTime();
      }
   }
}

// =================================================================================================
// private
size_t WsfWeaponTrackProcessor::GetCurrentTargetIndex()
{
   size_t          targetIndex = 0;
   const WsfTrack* trackPtr    = GetPlatform()->GetTrackManager().GetCurrentTarget();
   if (trackPtr != nullptr)
   {
      targetIndex = trackPtr->GetTargetIndex();
   }
   return targetIndex;
}

// =================================================================================================
// private
void WsfWeaponTrackProcessor::SetCurrentTarget(double aSimTime, WsfTrack* aTrackPtr)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Set curent target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "TrackId: " << aTrackPtr->GetTrackId();
      out.AddNote() << "Originator: " << aTrackPtr->GetOriginatorNameId();
      if (aTrackPtr->GetSensorNameId() != 0)
      {
         out.AddNote() << "Sensor: " << aTrackPtr->GetSensorNameId();
         out.AddNote() << "Mode: " << aTrackPtr->GetSensorModeId();
      }
      out.AddNote() << "Age: " << aSimTime - aTrackPtr->GetUpdateTime();
   }
   GetPlatform()->GetTrackManager().SetCurrentTarget(aSimTime, *aTrackPtr);
}

// =================================================================================================
void WsfWeaponTrackProcessor::SetPlatform(WsfPlatform* aPlatformPtr)
{
   WsfProcessor::SetPlatform(aPlatformPtr);
   // Set the context's parent as soon as possible.  This allows scripts
   // compiled later to use the parent's variables.
   mContextPtr->SetParent(&aPlatformPtr->GetScriptContext());
}
