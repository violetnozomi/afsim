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

#include "WsfCompositeSensor.hpp"

#include <algorithm>
#include <cmath>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfFilter.hpp"
#include "WsfFilterTypes.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"

// =================================================================================================
WsfCompositeSensor::WsfCompositeSensor(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mOperatingMode(cOM_UNDEFINED)
   , mCompositeModeList()
   , mSensorPtrs()
   , mSensorNames()
   , mTrackQuality(0.0)
   , mFilterPtr(nullptr)
   , mTrackManagerPtr(nullptr)
   , mMutex()
   , mCallbacks()
   , mXmtrList()
   , mRcvrList()
{
   // Create the mode list with the sensor-specific mode template.
   // SetModeList(new WsfSensorModeList(new CompositeMode()));

   // Assign the default sensor scheduler and tracker
   // SetScheduler(new WsfDefaultSensorScheduler());
   // SetTracker(new WsfDefaultSensorTracker());
}

// =================================================================================================
WsfCompositeSensor::WsfCompositeSensor(const WsfCompositeSensor& aSrc)
   : WsfSensor(aSrc)
   , mOperatingMode(aSrc.mOperatingMode)
   , mCompositeModeList()
   , mSensorPtrs()
   , mSensorNames(aSrc.mSensorNames)
   , mTrackQuality(aSrc.mTrackQuality)
   , mFilterPtr(nullptr)
   , mTrackManagerPtr(nullptr)
   , mMutex()
   , mCallbacks()
   , mXmtrList()
   , mRcvrList()
{
   if (aSrc.mFilterPtr != nullptr)
   {
      mFilterPtr = aSrc.mFilterPtr->Clone();
   }
}

// =================================================================================================
// virtual
WsfCompositeSensor::~WsfCompositeSensor()
{
   delete mFilterPtr;
   delete mTrackManagerPtr;
}

// =================================================================================================
// virtual
WsfSensor* WsfCompositeSensor::Clone() const
{
   return new WsfCompositeSensor(*this);
}

// =================================================================================================
// virtual
bool WsfCompositeSensor::Initialize(double aSimTime)
{
   bool ok = true;

   if (mOperatingMode == cOM_UNDEFINED)
   {
      ut::log::error() << "'operating_mode' has not been defined.";
      return false;
   }

   // Build the list of the sub-sensors that make the composite

   if (mSensorNames.empty())
   {
      ut::log::error() << "No constituent sensors defined.";
      return false;
   }

   mSensorPtrs.reserve(mSensorNames.size());
   for (std::vector<WsfStringId>::const_iterator sni = mSensorNames.begin(); sni != mSensorNames.end(); ++sni)
   {
      WsfStringId nameId    = *sni;
      WsfSensor*  sensorPtr = GetPlatform()->GetComponent<WsfSensor>(nameId);
      if (sensorPtr == this)
      {
         auto out = ut::log::error() << "Constituent sensor cannot be the same as the composite sensor.";
         out.AddNote() << "Sensor: " << nameId;
      }
      else if (sensorPtr == nullptr)
      {
         auto out = ut::log::error() << "Constituent sensor was not defined";
         out.AddNote() << "Sensor: " << nameId;
         ok = false;
      }
      else
      {
         mSensorPtrs.push_back(sensorPtr);
      }
   }

   // Processing common to both operating modes that must occur prior to initialization.

   // Propagate the class from the first sensor to our sensor.
   SetClass(mSensorPtrs[0]->GetClass());

   // Processing for the independent operating mode that must occur prior to initialization.

   if (mOperatingMode == cOM_INDEPENDENT)
   {
      SetModeList(ut::make_unique<WsfSensorModeList>(new CompositeMode()));
      SetScheduler(nullptr);
      SetTracker(nullptr);
      SetIsSlave(aSimTime, true);
   }

   // Processing for the synchronous mode that must occur prior to initialization.

   if (mOperatingMode == cOM_SYNCHRONOUS)
   {
      // Verify that each constituent sensor has a mode list and the same mode structure.
      for (size_t i = 1; i < mSensorPtrs.size(); ++i)
      {
         if (mSensorPtrs[i]->GetModeList() == nullptr)
         {
            auto out = ut::log::error() << "Constituent sensor must have a mode list.";
            out.AddNote() << "Sensor: " << mSensorPtrs[i]->GetName();
            ok = false;
         }
      }
      if (!ok)
      {
         return false;
      }

      WsfSensorModeList* modeList0_Ptr = mSensorPtrs[0]->GetModeList();
      for (size_t i = 1; i < mSensorPtrs.size(); ++i)
      {
         WsfSensorModeList* modeListN_Ptr = mSensorPtrs[i]->GetModeList();
         if (modeList0_Ptr->GetModeCount() != modeListN_Ptr->GetModeCount())
         {
            ut::log::error()
               << "All constituent sensors must have the same number of modes when using 'operating mode synchronous'.";
            ok = false;
            continue;
         }
         for (size_t j = 0; j < modeList0_Ptr->GetModeCount(); ++j)
         {
            if (modeListN_Ptr->GetModeNameId(j) != modeList0_Ptr->GetModeNameId(j))
            {
               ut::log::error()
                  << "All constituent sensors must have the same mode names when using 'operating mode synchronous'.";
               ok = false;
               continue;
            }
         }
      }
      if (!ok)
      {
         return false;
      }

      // Copy the mode list from the first constituent sensor and use as our mode list.
      // We aren't really copying the modes. They are just placeholders with the correct name.

      std::vector<WsfSensorMode*> modeList;
      for (size_t i = 0; i < mSensorPtrs[0]->GetModeCount(); ++i)
      {
         WsfSensorMode* modePtr = static_cast<WsfSensorMode*>(mSensorPtrs[0]->GetModeList()->GetModeEntry(i));
         modeList.push_back(new CompositeMode(*modePtr));
      }
      WsfSensorMode* templatePtr = static_cast<WsfSensorMode*>(modeList0_Ptr->GetModeTemplate());
      SetModeList(ut::make_unique<WsfSensorModeList>(*modeList0_Ptr, new CompositeMode(*templatePtr), modeList));

      // Use the scheduler and tracker from the first constituent scheduler as the scheduler and tracker
      // for composite sensor. The scheduler and tracker are then removed from all of the constituents
      // as they are no longer needed.

      auto schedulerPtr = mSensorPtrs[0]->SwapScheduler(nullptr);
      auto trackerPtr   = mSensorPtrs[0]->SwapTracker(nullptr);
      if ((schedulerPtr == nullptr) || (trackerPtr == nullptr))
      {
         auto out = ut::log::error() << "The constituent sensor must have a scheduler and a tracker.";
         out.AddNote() << "Sensor: " << mSensorPtrs[0]->GetName();
         ok = false;
      }
      else
      {
         SetScheduler(std::move(schedulerPtr));
         SetTracker(std::move(trackerPtr));
         for (size_t i = 0; i < mSensorPtrs.size(); ++i)
         {
            WsfSensor* sensorPtr = mSensorPtrs[i];
            sensorPtr->SetScheduler(nullptr);
            sensorPtr->SetTracker(nullptr);
            sensorPtr->SetInitiallyTurnedOn(false);

            // Indicate that each of the constituent sensors is a slave. This prevents the simulation
            // from controlling the sensing chances of the constituent sensor. The simulation will
            // control this sensor and we will control the constituents.
            sensorPtr->SetIsSlave(aSimTime, true);

            // Mark the sensor as private so DIS/HLA will not send emissions PDU for the constituent.
            // (They will be combined into the composite).
            sensorPtr->SetIsPrivate(true);
         }
      }

      // Get the base-level message parameters.
      mSensorPtrs[0]->GetMessageParameters(mMessageLength, mMessagePriority);
   }

   if (!ok)
   {
      return false;
   }

   ok &= WsfSensor::Initialize(aSimTime);

   // Reduce future dynamic casting by extracting derived class mode pointers.
   if (mModeListPtr != nullptr)
   {
      mModeListPtr->GetDerivedModeList(mCompositeModeList);
   }

   if (mOperatingMode == cOM_INDEPENDENT)
   {
      // Ensure each of the constituent sensors are linked to this sensor.
      // Ignore any error indicating it has already been added.
      for (size_t i = 0; i < mSensorPtrs.size(); ++i)
      {
         if (!mSensorPtrs[i]->GetInternalLinks().AddRecipient(this))
         {
            auto out = ut::log::error() << "Failed to add internal link for sensor.";
            out.AddNote() << "Sensor: " << mSensorPtrs[i]->GetName();
            ok = false;
         }
      }

      // Initialize the track manager.
      mTrackManagerPtr = new WsfTrackManager(GetScenario());
      ok &= mTrackManagerPtr->Initialize(aSimTime, GetPlatform());

      // Register simulation observer callbacks.
      mCallbacks.Add(
         WsfObserver::SensorDetectionChanged(GetSimulation()).Connect(&WsfCompositeSensor::SensorDetectionChanged, this));
      mCallbacks.Add(WsfObserver::SensorTurnedOff(GetSimulation()).Connect(&WsfCompositeSensor::SensorTurnedOff, this));
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfCompositeSensor::Initialize2(double aSimTime)
{
   bool ok = true;
   if (mOperatingMode == cOM_INDEPENDENT)
   {
      ok = mTrackManagerPtr->Initialize2(aSimTime);
   }
   ok &= WsfSensor::Initialize2(aSimTime);
   return ok;
}

// =================================================================================================
// virtual
bool WsfCompositeSensor::ProcessInput(UtInput& aInput)
{
   WsfFilter*  filterPtr = nullptr;
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "operating_mode")
   {
      std::string operatingMode;
      aInput.ReadValue(operatingMode);
      if (operatingMode == "independent")
      {
         mOperatingMode = cOM_INDEPENDENT;
      }
      else if (operatingMode == "synchronous")
      {
         mOperatingMode = cOM_SYNCHRONOUS;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "sensor")
   {
      std::string name;
      aInput.ReadValue(name);
      WsfStringId nameId = name;
      if (std::find(mSensorNames.begin(), mSensorNames.end(), nameId) != mSensorNames.end())
      {
         throw UtInput::BadValue(aInput, "Duplicate constituent sensor '" + name + "'");
      }
      mSensorNames.push_back(nameId);
   }
   else if (command == "track_quality")
   {
      aInput.ReadValue(mTrackQuality);
      aInput.ValueInClosedRange(mTrackQuality, 0.0, 1.0);
   }
   else if ((mOperatingMode == cOM_INDEPENDENT) &&
            WsfFilterTypes::Get(WsfScenario::FromInput(aInput)).LoadInstance(aInput, filterPtr))
   {
      delete mFilterPtr;
      mFilterPtr = filterPtr;
   }
   else
   {
      myCommand = WsfSensor::ProcessInput(aInput);
   }
   return myCommand;
}


// =================================================================================================
// virtual
bool WsfCompositeSensor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   if (mOperatingMode != cOM_INDEPENDENT)
   {
      return false;
   }

   bool processed = false;

   WsfStringId messageType(aMessage.GetType());
   if (messageType == WsfTrackMessage::GetTypeId())
   {
      const WsfTrackMessage& message  = static_cast<const WsfTrackMessage&>(aMessage);
      WsfTrack*              trackPtr = message.GetTrack();
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Composite sensor received track update from sensor.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Composite Sensor: " << GetName();
         out.AddNote() << "Sensor: " << trackPtr->GetSensorNameId();
         out.AddNote() << "Mode: " << trackPtr->GetSensorModeId();
         out.AddNote() << "Track ID: " << trackPtr->GetTrackId();
         out.AddNote() << "Target: " << trackPtr->GetTargetName();
      }

      // Make sure the update is coming from one of the constituent sensors.
      WsfSensor* sensorPtr = GetPlatform()->GetComponent<WsfSensor>(trackPtr->GetSensorNameId());
      if (IsA_ConstituentSensor(sensorPtr))
      {
         UpdateCompositeTrack(aSimTime, *trackPtr);
         processed = true;
      }
   }
   else if (messageType == WsfTrackDropMessage::GetTypeId())
   {
      const WsfTrackDropMessage& message = static_cast<const WsfTrackDropMessage&>(aMessage);
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Composite sensor received track drop.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Composite Sensor: " << GetName();
         out.AddNote() << "Track ID: " << message.GetTrackId();
         out.AddNote() << "Target: " << GetSimulation()->GetPlatformNameId(message.GetTargetIndex());
      }
      processed = DropContributorTrack(aSimTime, message.GetTrackId());
   }

   return processed;
}

// =================================================================================================
// Note: This should only be called in synchronous mode.
// virtual
void WsfCompositeSensor::Update(double aSimTime)
{
   if (mOperatingMode == cOM_SYNCHRONOUS)
   {
      // Bypass updates if not time for an update.  This avoids unnecessary device updates.
      // (A little slop is allowed to make sure event-driven chances occur as scheduled)
      if (mNextUpdateTime <= (aSimTime + 1.0E-5))
      {
         WsfSensor::Update(aSimTime);
         PerformScheduledDetections(aSimTime); // Perform any required detection attempts
      }
   }
}

// =================================================================================================
// virtual
void WsfCompositeSensor::TurnOff(double aSimTime)
{
   WsfSensor::TurnOff(aSimTime);
   if (mOperatingMode == cOM_INDEPENDENT)
   {
      mTrackManagerPtr->GetTrackList().DeleteAllTracks();
      mTrackManagerPtr->GetRawTrackList().DeleteAllTracks();
   }
   else if (mOperatingMode == cOM_SYNCHRONOUS)
   {
      for (size_t i = 0; i < mSensorPtrs.size(); ++i)
      {
         mSensorPtrs[i]->TurnOff(aSimTime);
      }
   }
}

// =================================================================================================
// virtual
void WsfCompositeSensor::TurnOn(double aSimTime)
{
   WsfSensor::TurnOn(aSimTime);
   if (mOperatingMode == cOM_SYNCHRONOUS)
   {
      for (size_t i = 0; i < mSensorPtrs.size(); ++i)
      {
         mSensorPtrs[i]->TurnOn(aSimTime);
      }
   }
}

// =================================================================================================
// virtual
size_t WsfCompositeSensor::GetEM_RcvrCount() const
{
   if (mOperatingMode == cOM_SYNCHRONOUS)
   {
      return static_cast<unsigned int>(mRcvrList.size());
   }
   return WsfSensor::GetEM_RcvrCount();
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfCompositeSensor::GetEM_Rcvr(size_t aIndex) const
{
   if (mOperatingMode == cOM_SYNCHRONOUS)
   {
      if (aIndex < mRcvrList.size())
      {
         return *(mRcvrList[aIndex]);
      }
   }
   return WsfSensor::GetEM_Rcvr(aIndex);
}

// =================================================================================================
// virtual
size_t WsfCompositeSensor::GetEM_XmtrCount() const
{
   if (mOperatingMode == cOM_SYNCHRONOUS)
   {
      return mXmtrList.size();
   }
   return WsfSensor::GetEM_XmtrCount();
}

// =================================================================================================
// virtual
WsfEM_Xmtr& WsfCompositeSensor::GetEM_Xmtr(size_t aIndex) const
{
   if (mOperatingMode == cOM_SYNCHRONOUS)
   {
      if (aIndex < mXmtrList.size())
      {
         return *(mXmtrList[aIndex]);
      }
   }
   return WsfSensor::GetEM_Xmtr(aIndex);
}

// =================================================================================================
// virtual
bool WsfCompositeSensor::WithinFieldOfView(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool withinFieldOfView = false;
   if (mOperatingMode == cOM_SYNCHRONOUS)
   {
      for (size_t i = 0; i < mSensorPtrs.size(); ++i)
      {
         if (mSensorPtrs[i]->WithinFieldOfView(aSimTime, aPlatformPtr))
         {
            withinFieldOfView = true;
            break;
         }
      }
   }
   return withinFieldOfView;
}

// =================================================================================================
// virtual
bool WsfCompositeSensor::WithinFieldOfView(double aSimTime, WsfTrack* aTrackPtr)
{
   bool withinFieldOfView = false;
   if (mOperatingMode == cOM_SYNCHRONOUS)
   {
      for (size_t i = 0; i < mSensorPtrs.size(); ++i)
      {
         if (mSensorPtrs[i]->WithinFieldOfView(aSimTime, aTrackPtr))
         {
            withinFieldOfView = true;
            break;
         }
      }
   }
   return withinFieldOfView;
}

// =================================================================================================
//! Process the drop of a contributor track.
//!
//! @param aSimTime    The current simulation time.
//! @param aRawTrackId The track ID of the contributor being dropped.
//! @return true if the track was dropped.
//!
//! @note This is only used in independent mode.
// protected
bool WsfCompositeSensor::DropContributorTrack(double aSimTime, const WsfTrackId& aRawTrackId)
{
   // Search our composite track list to find the track with which this track is fused.

   WsfLocalTrackList& localTrackList = mTrackManagerPtr->GetTrackList();
   WsfTrackList&      rawTrackList   = mTrackManagerPtr->GetRawTrackList();

   WsfLocalTrack* fusedTrackPtr = nullptr;
   for (unsigned int entryIndex = 0; entryIndex < localTrackList.GetTrackCount(); ++entryIndex)
   {
      WsfLocalTrack* tempTrackPtr = localTrackList.GetTrackEntry(entryIndex);
      if (tempTrackPtr->IsCorrelatedWith(aRawTrackId))
      {
         fusedTrackPtr = tempTrackPtr;
         break;
      }
   }

   // Decorrelate the track from the composite track.
   // If there are no remaining correlations then also drop the composite track.

   if (fusedTrackPtr != nullptr)
   {
      fusedTrackPtr->Decorrelate(aRawTrackId);
      if (!fusedTrackPtr->IsCorrelated())
      {
         TrackDropped(aSimTime, fusedTrackPtr);
         localTrackList.DeleteTrack(fusedTrackPtr->GetTrackId());
      }
   }

   // And finally, get rid of the raw track.
   bool trackDropped = rawTrackList.DeleteTrack(aRawTrackId);
   return trackDropped;
}

// =================================================================================================
//! Update a composite track with a raw track from a constituent sensor.
//!
//! @param aSimTime  The current simulation.
//! @param aRawTrack The track update from a constituent sensor.
//!
//! @note This is only used in independent mode.
// protected
void WsfCompositeSensor::UpdateCompositeTrack(double aSimTime, const WsfTrack& aRawTrack)
{
   WsfLocalTrackList& localTrackList = mTrackManagerPtr->GetTrackList();
   WsfTrackList&      rawTrackList   = mTrackManagerPtr->GetRawTrackList();

   // Add or update the raw track list with the track from the constituent sensor.

   WsfTrack* rawTrackPtr = rawTrackList.FindTrack(aRawTrack.GetTrackId());
   if (rawTrackPtr != nullptr)
   {
      // Update existing raw track.
      (*rawTrackPtr) = aRawTrack;
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Composite sensor updating raw track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Composite Sensor: " << GetName();
         out.AddNote() << "Track ID: " << rawTrackPtr->GetTrackId();
         out.AddNote() << "Target: " << rawTrackPtr->GetTargetName();
         out.AddNote() << "Track Quality: " << rawTrackPtr->GetTrackQuality();
      }
   }
   else
   {
      // Add new raw track.
      rawTrackPtr = aRawTrack.Clone();
      rawTrackList.AddTrack(std::unique_ptr<WsfTrack>(rawTrackPtr));
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Composite sensor adding raw track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Composite Sensor: " << GetName();
         out.AddNote() << "Track ID: " << rawTrackPtr->GetTrackId();
         out.AddNote() << "Target: " << rawTrackPtr->GetTargetName();
         out.AddNote() << "Track Quality: " << rawTrackPtr->GetTrackQuality();
      }
   }

   // Use perfect correlation to locate the composite track.

   WsfLocalTrack* localTrackPtr = nullptr;
   for (unsigned int entryIndex = 0; entryIndex < localTrackList.GetTrackCount(); ++entryIndex)
   {
      WsfLocalTrack* tempTrackPtr = localTrackList.GetTrackEntry(entryIndex);
      if (tempTrackPtr->GetTargetName() == rawTrackPtr->GetTargetName())
      {
         localTrackPtr = tempTrackPtr;
         break;
      }
   }

   // If a composite track does exist then create one from the contributor track.

   if (localTrackPtr == nullptr)
   {
      auto localTrackUP = ut::make_unique<WsfLocalTrack>(*rawTrackPtr);
      localTrackPtr     = localTrackUP.get();
      localTrackPtr->SetSensorOriginator(this, GetPlatform());
      localTrackPtr->SetSensorModeId(GetCurrentModeName());
      localTrackPtr->Initialize(localTrackPtr->GetUpdateTime(), GetPlatform()->GetNextTrackId(), *GetSimulation());
      if (mFilterPtr != nullptr)
      {
         localTrackPtr->SetTrackType(WsfTrack::cFILTERED_SENSOR);
         localTrackPtr->SetFilter(mFilterPtr->Clone());
      }
      localTrackList.AddTrack(std::move(localTrackUP));
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Composite sensor creating composite track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Composite Sensor: " << GetName();
         out.AddNote() << "Track ID: " << localTrackPtr->GetTrackId();
         out.AddNote() << "Target: " << localTrackPtr->GetTargetName();
      }
   }
   if (!localTrackPtr->IsCorrelatedWith(rawTrackPtr->GetTrackId()))
   {
      localTrackPtr->Correlate(rawTrackPtr->GetTrackId());
   }

   // Determine if the update from this contributor should be used to update the composite track.

   bool                                 updateTrack = true;
   const WsfLocalTrack::RawTrackIdList& rawTrackIds = localTrackPtr->GetRawTrackIds();
   for (unsigned int index = 0; index < rawTrackIds.GetCount(); ++index)
   {
      WsfTrack* tempTrackPtr = rawTrackList.FindTrack(*(rawTrackIds.GetEntry(index)));
      if ((tempTrackPtr != nullptr) && (rawTrackPtr->GetTrackQuality() < tempTrackPtr->GetTrackQuality()))
      {
         updateTrack = false;
         break;
      }
   }

   // Update the composite track if this is an acceptable input.

   if (updateTrack)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Platform's Composite Sensor updating Composite Track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Composite Sensor: " << GetName();
         out.AddNote() << "Track ID: " << localTrackPtr->GetTrackId();
         out.AddNote() << "Target: " << localTrackPtr->GetTargetName();
      }

      localTrackPtr->UpdateFromMeasurement(rawTrackPtr->GetUpdateTime(), *rawTrackPtr);
      if (mTrackQuality > 0.0)
      {
         localTrackPtr->SetTrackQuality(mTrackQuality);
      }

      // NOTE: If the operating_mode is 'independent', there are no modes on this sensor. The message_length/message_priority
      //       will be those from the sensor-level.
      TrackUpdated(aSimTime, localTrackPtr);
   }
   else if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Composite sensor ignored update for composite track.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Composite Sensor: " << GetName();
      out.AddNote() << "Track ID: " << localTrackPtr->GetTrackId();
      out.AddNote() << "Target: " << localTrackPtr->GetTargetName();
   }
}

// =================================================================================================
//! Is the supplied sensor name on of my constituent sensors.
//! @param aSensorPtr The sensor to be tested.
//! @returns true if the sensor is one of my constituents.
//!
//! @note This is only used in independent mode.
bool WsfCompositeSensor::IsA_ConstituentSensor(WsfSensor* aSensorPtr)
{
   for (unsigned int i = 0; i < mSensorPtrs.size(); ++i)
   {
      if (aSensorPtr == mSensorPtrs[i])
      {
         return true;
      }
   }
   return false;
}

// =================================================================================================
//! Return the index of the sensor (on the platform) with the specified name (Id).
//! @returns the index of the sensor.
unsigned int WsfCompositeSensor::GetSensorIndex(WsfSensor* aSensorPtr) const
{
   unsigned int sensorCount = GetPlatform()->GetComponentCount<WsfSensor>();
   for (unsigned int sensorIndex = 0; sensorIndex < sensorCount; ++sensorIndex)
   {
      if (aSensorPtr == GetPlatform()->GetComponentEntry<WsfSensor>(sensorIndex))
      {
         return sensorIndex;
      }
   }
   return sensorCount;
}

// =================================================================================================
//! Simulation observer to handle the turning off of a sensor.
//!
//! @note This is only used in independent mode.
void WsfCompositeSensor::SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr)
{
   if ((!IsTurnedOn()) || (!IsA_ConstituentSensor(aSensorPtr)))
   {
      return;
   }

   std::lock_guard<std::recursive_mutex> lock(mMutex);

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Composite sensor notified of turn-off of sensor.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Composite Sensor: " << GetName();
      out.AddNote() << "Sensor: " << aSensorPtr->GetName();
   }

   // Drop all the contributor tracks that came from the sensor.
   //
   // For each composite track that contains a contributor from this sensor, the contributor will
   // be removed from the contributors to that track, which potentially updates the 'current best
   // contributor'.

   WsfTrackList& rawTrackList = mTrackManagerPtr->GetRawTrackList();
   unsigned int  index        = 0;
   while (index < rawTrackList.GetTrackCount())
   {
      WsfTrack* rawTrackPtr = rawTrackList.GetTrackEntry(index);
      if (rawTrackPtr->GetSensorNameId() == aSensorPtr->GetNameId())
      {
         DropContributorTrack(aSimTime, rawTrackPtr->GetTrackId());
      }
      else
      {
         ++index;
      }
   }
}

// =================================================================================================
//! Simulation observer to handle a change in detection status of a target.
//!
//! @note This is only used in independent mode.
void WsfCompositeSensor::SensorDetectionChanged(double           aSimTime,
                                                WsfSensor*       aSensorPtr,
                                                size_t           aTargetIndex,
                                                WsfSensorResult& aResult)
{
   // We are only interested in loss of detection. If changing from no-detect to detect we will get
   // a track update if it was already tracking.

   if ((aResult.mCheckedStatus != 0) && (aResult.mFailedStatus == 0))
   {
      return; // Transition from non-detectable to detectable.
   }

   // The constituent sensor test is done second because it takes longer.
   if ((!IsTurnedOn()) || (!IsA_ConstituentSensor(aSensorPtr)))
   {
      return;
   }

   std::lock_guard<std::recursive_mutex> lock(mMutex);

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Composite sensor notified of detection loss of target by sensor.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Composite Sensor: " << GetName();
      out.AddNote() << "Sensor: " << aSensorPtr->GetName();
      out.AddNote() << "Target " << GetSimulation()->GetPlatformNameId(aTargetIndex);
   }

   // Locate the contributor track that corresponds to this sensor/target and set its
   // quality to zero. If this was the track that was the current update source then
   // this will allow the next best source(s) to take over.

   WsfTrackList& rawTrackList = mTrackManagerPtr->GetRawTrackList();
   WsfTrack*     rawTrackPtr  = nullptr;
   for (unsigned index = 0; index < rawTrackList.GetTrackCount(); ++index)
   {
      WsfTrack* tempTrackPtr = rawTrackList.GetTrackEntry(index);
      if ((tempTrackPtr->GetSensorNameId() == aSensorPtr->GetNameId()) && (tempTrackPtr->GetTargetIndex() == aTargetIndex))
      {
         rawTrackPtr = tempTrackPtr;
         break;
      }
   }

   // If a track does not exist than it is still in the track formation stage (Hasn't met the M/N criteria).
   if (rawTrackPtr != nullptr)
   {
      rawTrackPtr->SetTrackQuality(-1.0);
   }
}

// =================================================================================================
//! Update the list of active transmitters and receivers.
//!
//! This is only used in synchronous mode.
void WsfCompositeSensor::UpdateXmtrRcvrLists()
{
   mXmtrList.clear();
   mRcvrList.clear();
   for (size_t sensorIndex = 0; sensorIndex < mSensorPtrs.size(); ++sensorIndex)
   {
      WsfSensor* sensorPtr = mSensorPtrs[sensorIndex];
      for (unsigned int i = 0; i < sensorPtr->GetEM_XmtrCount(); ++i)
      {
         mXmtrList.push_back(&(sensorPtr->GetEM_Xmtr(i)));
      }
      for (unsigned int i = 0; i < sensorPtr->GetEM_RcvrCount(); ++i)
      {
         mRcvrList.push_back(&(sensorPtr->GetEM_Rcvr(i)));
      }
   }
}

// =================================================================================================
// =================================================================================================
// Definition for the nested class that implements the mode of the sensor.
// =================================================================================================
// =================================================================================================

WsfCompositeSensor::CompositeMode::CompositeMode()
   : WsfSensorMode()
   , mMySensorPtr(nullptr)
   , mDetectModePtr(nullptr)
{
   // Force it to report something to prevent an error when the mode is initialized.
   // (What the mode reports is really a function of the constituent sensors).
   mReportingFlags.mReportsOther = true;
   SetErrorModel(nullptr); // No error model.
}

// =================================================================================================
WsfCompositeSensor::CompositeMode::CompositeMode(const CompositeMode& aSrc)
   : WsfSensorMode(aSrc)
   , mMySensorPtr(nullptr)
   , mDetectModePtr(nullptr)
{
}

// =================================================================================================
WsfCompositeSensor::CompositeMode& WsfCompositeSensor::CompositeMode::operator=(const CompositeMode& aRhs)
{
   if (this != &aRhs)
   {
      WsfSensorMode::operator=(aRhs);
      mMySensorPtr           = nullptr;
      mDetectModePtr         = nullptr;
   }
   return *this;
}

// =================================================================================================
WsfCompositeSensor::CompositeMode::CompositeMode(const WsfSensorMode& aSrc)
   : WsfSensorMode(aSrc)
   , mMySensorPtr(nullptr)
   , mDetectModePtr(nullptr)
{
   SetErrorModel(nullptr);
}

// =================================================================================================
WsfCompositeSensor::CompositeMode& WsfCompositeSensor::CompositeMode::operator=(const WsfSensorMode& aRhs)
{
   if (this != &aRhs)
   {
      WsfSensorMode::operator=(aRhs);
      mMySensorPtr           = nullptr;
      mDetectModePtr         = nullptr;
      SetErrorModel(nullptr);
   }
   return *this;
}

// =================================================================================================
// virtual
WsfMode* WsfCompositeSensor::CompositeMode::Clone() const
{
   return new CompositeMode(*this);
}

// =================================================================================================
// virtual
bool WsfCompositeSensor::CompositeMode::Initialize(double aSimTime)
{
   bool ok      = WsfSensorMode::Initialize(aSimTime);
   mMySensorPtr = dynamic_cast<WsfCompositeSensor*>(mSensorPtr);
   return ok;
}

// =================================================================================================
// virtual
bool WsfCompositeSensor::CompositeMode::ProcessInput(UtInput& aInput)
{
   bool myCommand = WsfSensorMode::ProcessInput(aInput);
   return myCommand;
}

// =================================================================================================
bool WsfCompositeSensor::CompositeMode::AttemptToDetect(double           aSimTime,
                                                        WsfPlatform*     aTargetPtr,
                                                        Settings&        aSettings,
                                                        WsfSensorResult& aResult)
{
   bool detected = false;
   GetSensor()->UpdatePosition(aSimTime); // Ensure my position is current
   aTargetPtr->Update(aSimTime);          // Ensure the target position is current

   if (GetSensor()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Composite sensor attempting to detect target.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Composite Sensor: " << GetSensor()->GetName();
      out.AddNote() << "Mode: " << GetName();
      out.AddNote() << "Target: " << aTargetPtr->GetName();
   }

   double          cueLocWCS[3];
   double          cueAz;
   double          cueEl;
   Settings        tempSettings(aSettings);
   WsfSensorResult tempResult;
   mDetectModePtr = nullptr;
   for (size_t i = 0; i < mMySensorPtr->mSensorPtrs.size(); ++i)
   {
      WsfSensor*     sensorPtr = mMySensorPtr->mSensorPtrs[i];
      WsfSensorMode* modePtr   = sensorPtr->GetModeEntry(GetModeIndex());

      // Propagate the cue from the composite sensor the constituent sensor.
      if (mMySensorPtr->GetTransientCuedLocationWCS(cueLocWCS))
      {
         sensorPtr->SetTransientCuedLocationWCS(cueLocWCS);
      }
      else if (mMySensorPtr->GetCuedLocationWCS(cueLocWCS))
      {
         sensorPtr->SetCuedLocationWCS(cueLocWCS);
      }
      else if (mMySensorPtr->GetCuedOrientation(cueAz, cueEl))
      {
         sensorPtr->SetCuedOrientation(cueAz, cueEl);
      }
      else
      {
         sensorPtr->ClearCueing();
         sensorPtr->ClearTransientCue();
      }

      // NOTE: Updating the position of the constituent sensor is not necessary as the detection
      // attempt will force a position update.

      // Update the settings with the mode index and the potentially mode-specific required Pd.

      tempSettings.mModeIndex = modePtr->GetModeIndex();
      if (aSettings.mRequiredPd <= 0.0)
      {
         tempSettings.mRequiredPd = WsfSensor::GetRequiredPd(modePtr);
      }

      // Perform the detection attempt. Note that the sensor-level AttemptToDetect is called and not
      // the mode-level form. The sensor-level form is then general interface and responsible for
      // vectoring to the mode-specific form after performing any required setup.

      if (i == 0)
      {
         sensorPtr->AttemptToDetect(aSimTime, aTargetPtr, tempSettings, aResult);
         mDetectModePtr = modePtr;
      }
      else
      {
         sensorPtr->AttemptToDetect(aSimTime, aTargetPtr, tempSettings, tempResult);

         // Determine if a signal has impinged on the sensor (even if not detectable).
         //
         // A signal impinges only if the signal level has been checked AND their are no failures
         // with the possible exception of signal level.

         bool cumHaveSignal = ((aResult.mCheckedStatus & WsfSensorResult::cSIGNAL_LEVEL) != 0) &&
                              ((aResult.mFailedStatus & (~WsfSensorResult::cSIGNAL_LEVEL)) == 0);

         bool nowHaveSignal = ((tempResult.mCheckedStatus & WsfSensorResult::cSIGNAL_LEVEL) != 0) &&
                              ((tempResult.mFailedStatus & (~WsfSensorResult::cSIGNAL_LEVEL)) == 0);

         bool updateResults = false;
         if (cumHaveSignal && nowHaveSignal)
         {
            // Some other constituent also had an impinging signal. If this has a higher Pd then choose it.
            // If Pd's match (as typical for a binary detection), then use signal to noise.
            if (tempResult.mPd > aResult.mPd)
            {
               updateResults = true;
            }
            else if (tempResult.mPd == aResult.mPd)
            {
               updateResults = (tempResult.mSignalToNoise > aResult.mSignalToNoise);
            }
         }
         else if (nowHaveSignal)
         {
            // This is the first constituent that had a signal that impinged on the receiver.
            updateResults = true;
         }

         if (updateResults)
         {
            aResult        = tempResult;
            mDetectModePtr = modePtr;
         }
      }
   }

   // A successful detection occurs only if the beam with the best signal-to-noise had no failing criteria.
   detected = WsfSensorComponent::PostAttemptToDetect(*GetSensor(), aSimTime, aTargetPtr, aResult);

   // The application of measurement errors must occur here because the angles used to compute the reported
   // location are relative to the sensor that did the detection. By doing it here we avoid having to go
   // through all sorts of machinations to figure that out.

   if (detected)
   {
      mDetectModePtr->ApplyMeasurementErrors(aResult);
   }

   // Set the beam index of the beam that is being returned.
   aResult.mBeamIndex = 0;
   for (unsigned int i = 0; i < mMySensorPtr->mRcvrList.size(); ++i)
   {
      if (mMySensorPtr->mRcvrList[i] == aResult.GetReceiver())
      {
         aResult.mBeamIndex = i;
         break;
      }
   }

   // Clear the transient cue on the constituents. This must be done AFTER the measurement error computation
   // as it requires the coordinate system at the time of the detection attempt.

   if (GetSensor()->TransientCueActive())
   {
      for (size_t i = 0; i < mMySensorPtr->mSensorPtrs.size(); ++i)
      {
         mMySensorPtr->mSensorPtrs[i]->ClearTransientCue();
      }
   }
   return detected;
}

// =================================================================================================
// virtual
void WsfCompositeSensor::CompositeMode::Deselect(double aSimTime)
{
   if (mMySensorPtr == nullptr)
   {
      return;
   }

   // Deselect the mode in each of the constituent sensors.

   for (size_t i = 0; i < mMySensorPtr->mSensorPtrs.size(); ++i)
   {
      WsfSensor*     sensorPtr = mMySensorPtr->mSensorPtrs[i];
      WsfSensorMode* modePtr   = sensorPtr->GetModeEntry(GetModeIndex());
      modePtr->Deselect(aSimTime);
   }
   mMySensorPtr->UpdateXmtrRcvrLists();
}

// =================================================================================================
// virtual
void WsfCompositeSensor::CompositeMode::Select(double aSimTime)
{
   if (mMySensorPtr == nullptr)
   {
      return;
   }

   // Select the mode in each of the constituent sensors

   for (size_t i = 0; i < mMySensorPtr->mSensorPtrs.size(); ++i)
   {
      WsfSensor*     sensorPtr = mMySensorPtr->mSensorPtrs[i];
      WsfSensorMode* modePtr   = sensorPtr->GetModeEntry(GetModeIndex());
      modePtr->Select(aSimTime);
   }
   mMySensorPtr->UpdateXmtrRcvrLists();
}

// =================================================================================================
// override
void WsfCompositeSensor::CompositeMode::ApplyMeasurementErrors(WsfSensorResult& aResult)
{
   // This is a dummy as errors should have already been applied (See CompositeMode::AttemptToDetect)
}

// =================================================================================================
//! Initialize the track.
//! @note The track must be initialized and updated using the data from the constituent mode definition,
//!       but must be identified as coming from the composition definition.
// virtual
void WsfCompositeSensor::CompositeMode::InitializeTrack(double aSimTime, WsfTrack*& aTrackPtr)
{
   if (mDetectModePtr != nullptr)
   {
      mDetectModePtr->InitializeTrack(aSimTime, aTrackPtr);

      // The track is identified as originating from the constituent. Change it so it originates from the composite.
      aTrackPtr->SetSensorOriginator(GetSensor()->GetNameId(), GetSensor()->GetTypeId());
   }
}

// =================================================================================================
//! Update the track.
//! @note The track must be initialized and updated using the data from the constituent mode definition,
//!       but must be identified as coming from the composition definition.
// virtual
void WsfCompositeSensor::CompositeMode::UpdateTrack(double           aSimTime,
                                                    WsfTrack*        aTrackPtr,
                                                    WsfPlatform*     aTargetPtr,
                                                    WsfSensorResult& aResult)
{
   if (mDetectModePtr != nullptr)
   {
      mDetectModePtr->UpdateTrack(aSimTime, aTrackPtr, aTargetPtr, aResult);
   }
}
