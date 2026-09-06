// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSensorProcessor.hpp"

#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtScriptTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackObserver.hpp"
#include "script/WsfScriptContext.hpp"


// =================================================================================================
WsfSensorProcessor::WsfSensorProcessor(WsfScenario& aScenario)
   : WsfProcessor(aScenario)
   , mIncomingMutex()
   , mSensors()
   , mMessageLength(0)
   , mMessagePriority(0)
{
}

// =================================================================================================
WsfSensorProcessor::WsfSensorProcessor(const WsfSensorProcessor& aSrc)
   : WsfProcessor(aSrc)
   , mIncomingMutex()
   , mSensors(aSrc.mSensors)
   , mMessageLength(aSrc.mMessageLength)
   , mMessagePriority(aSrc.mMessagePriority)
{
}

// =================================================================================================
bool WsfSensorProcessor::Initialize(double aSimTime)
{
   bool ok = WsfProcessor::Initialize(aSimTime);

   for (auto& sensor : mSensors)
   {
      if (sensor.first.IsNull())
      {
         sensor.first = GetPlatform()->GetNameId();
         AddSensor(sensor.first, sensor.second);
      }
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfSensorProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "sensors")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "sensor")
         {
            WsfStringId sensorId;
            aInput.ReadValue(sensorId);
            std::pair<WsfStringId, WsfStringId> pltfrmSnsrId(nullptr, sensorId);
            if (find(mSensors.begin(), mSensors.end(), pltfrmSnsrId) == mSensors.end())
            {
               mSensors.push_back(pltfrmSnsrId);
            }
         }
         else if (command == "platform_sensor")
         {
            WsfStringId platformId;
            aInput.ReadValue(platformId);
            WsfStringId sensorId;
            aInput.ReadValue(sensorId);
            std::pair<WsfStringId, WsfStringId> pltfrmSnsrId(platformId, sensorId);
            if (find(mSensors.begin(), mSensors.end(), pltfrmSnsrId) == mSensors.end())
            {
               mSensors.push_back(pltfrmSnsrId);
            }
         }
      }
   }
   else if (command == "message_length")
   {
      aInput.ReadValueOfType(mMessageLength, UtInput::cDATA_SIZE);
      aInput.ValueGreaterOrEqual(mMessageLength, 0);
   }
   else if (command == "message_priority")
   {
      aInput.ReadValue(mMessagePriority);
      aInput.ValueGreaterOrEqual(mMessagePriority, 0);
   }
   else
   {
      myCommand = WsfProcessor::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
void WsfSensorProcessor::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   for (auto& sensor : mSensors)
   {
      if (aPlatformPtr->GetNameId() == sensor.first)
      {
         AddSensor(sensor.first, sensor.second);
      }
   }
}

// =================================================================================================
void WsfSensorProcessor::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   for (auto& sensor : mSensors)
   {
      if (aPlatformPtr->GetNameId() == sensor.first)
      {
         RemoveSensor(sensor.first, sensor.second);
      }
   }
}

// =================================================================================================
bool WsfSensorProcessor::AddSensor(WsfStringId aPlatformNameId, WsfStringId aSensorNameId)
{
   bool added(false);

   // If aPlatformNameId is not set then assume own platform
   WsfStringId pltfrmNameId(aPlatformNameId);
   if (pltfrmNameId.IsNull())
   {
      pltfrmNameId = GetPlatform()->GetNameId();
   }

   // If not in input list then add it, maybe for use later even.
   // Note: The platform/sensor may be added later, even if platform/sensor is not currently valid.
   //       This add may cause issues though when cloning or copy constructing and my need a more thorough
   //       look in the future if issues arise.
   std::pair<WsfStringId, WsfStringId> pltfrmSnsrId(pltfrmNameId, aSensorNameId);
   if (find(mSensors.begin(), mSensors.end(), pltfrmSnsrId) == mSensors.end())
   {
      mSensors.push_back(pltfrmSnsrId);
   }

   // Check to see if the platform and sensor are valid and add sensor
   WsfPlatform* platformPtr = GetSimulation()->GetPlatformByName(pltfrmNameId);
   if (platformPtr != nullptr)
   {
      WsfSensor* sensorPtr = platformPtr->GetComponent<WsfSensor>(aSensorNameId);
      if (sensorPtr != nullptr)
      {
         added = SensorAdded(sensorPtr); // Call virtual function to notify inheriting classes;
      }
   }


   if (DebugEnabled())
   {
      if (added)
      {
         auto out = ut::log::debug() << "Sensor processor attached sensor.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
         out.AddNote() << "Sensor Platform: " << pltfrmNameId;
         out.AddNote() << "Sensor: " << aSensorNameId;
      }
      else
      {
         auto out = ut::log::warning() << "Sensor processor could not find sensor to attach.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
         out.AddNote() << "Sensor Platform: " << pltfrmNameId;
         out.AddNote() << "Sensor: " << aSensorNameId;
      }
   }

   return added;
}

// =================================================================================================
bool WsfSensorProcessor::RemoveSensor(WsfStringId aPlatformNameId, WsfStringId aSensorNameId)
{
   bool removed(false);

   WsfStringId pltfrmNameId(aPlatformNameId);
   if (pltfrmNameId.IsNull())
   {
      pltfrmNameId = GetPlatform()->GetNameId();
   }

   std::pair<WsfStringId, WsfStringId>                        pltfrmSnsrId(pltfrmNameId, aSensorNameId);
   std::vector<std::pair<WsfStringId, WsfStringId>>::iterator mpsi = find(mSensors.begin(), mSensors.end(), pltfrmSnsrId);
   if (mpsi != mSensors.end())
   {
      WsfPlatform* platformPtr = GetSimulation()->GetPlatformByName(pltfrmNameId);
      if (platformPtr != nullptr)
      {
         WsfSensor* sensorPtr = platformPtr->GetComponent<WsfSensor>(aSensorNameId);
         if (sensorPtr != nullptr)
         {
            SensorRemoved(sensorPtr); // Call virtual function to notify inheriting classes
         }
      }
      removed = true;
   }

   if (DebugEnabled())
   {
      if (removed)
      {
         auto out = ut::log::debug() << "Sensor processor removed sensor.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
         out.AddNote() << "Sensor Platform: " << pltfrmNameId;
         out.AddNote() << "Sensor: " << aSensorNameId;
      }
      else
      {
         auto out = ut::log::warning() << "Sensor processor could not find sensor to remove.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
         out.AddNote() << "Sensor Platform: " << pltfrmNameId;
         out.AddNote() << "Sensor: " << aSensorNameId;
      }
   }

   return removed;
}

// =================================================================================================
//! The sensor tracker has updated a track.
//! This method is a hook that is called when a derived sensor (or one of it's constituents such as the tracker)
//! has updated a track. The default method simply calls SendTrackUpdateMessage inform simulation observers
//! and send a message to attached processors.
//!
//! This method is often provided by derived classes that want to use an existing sensor and simply want to
//! change how information is reported.
//!
//! @param aSimTime   [input] The current simulation time.
//! @param aSensorPtr [input] Pointer to the sensor.
//! @param aTrackPtr  [input] A pointer to the track being updated.
// virtual
void WsfSensorProcessor::TrackUpdated(double aSimTime, WsfSensor* aSensorPtr, WsfTrack* aTrackPtr)
{
   if (!IsExternallyControlled())
   {
      // Issue the sensor track initiate/update observer calls if the sensor still exists.
      if (aSensorPtr != nullptr)
      {
         if (aTrackPtr->GetUpdateCount() > 0)
         {
            WsfObserver::SensorTrackUpdated(GetSimulation())(aSimTime, aSensorPtr, aTrackPtr);
         }
         else
         {
            WsfObserver::SensorTrackInitiated(GetSimulation())(aSimTime, aSensorPtr, aTrackPtr);
         }
      }

      SendTrackUpdateMessage(aSimTime, aSensorPtr, aTrackPtr);
   }
}

// =================================================================================================
//! Hook to notify a derived class that a track representing a target being dropped.
//!
//! @param aSimTime The current simulation time.
//! @param aTrack   The track being dropped.
// protected virtual
void WsfSensorProcessor::TrackDropped(double aSimTime, const WsfTrack& aTrack) {}

// =================================================================================================
//! A common method to inform interested parties that the track has been updated.
//! This will inform simulation observers and attached processors that a track has been updated.
//! @param aSimTime   [input] The current simulation time.
//! @param aSensorPtr [input] Pointer to the sensor.
//! @param aTrackPtr  [input] A pointer to the track being updated.
// virtual
void WsfSensorProcessor::SendTrackUpdateMessage(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   // Send the track to all attached objects.
   // Note: Generally message suppression will not be enabled, but
   // it is allowed.
   // if (!mSuppressMessage)
   //{
   WsfTrackMessage message(GetPlatform(), *aTrackPtr);
   message.SetSizeBits(mMessageLength);
   message.SetPriority(mMessagePriority);
   SendMessage(aSimTime, message);
   //}
}


// =================================================================================================
// Script Interface
// =================================================================================================

namespace
{
UT_DEFINE_SCRIPT_METHOD_EXT(WsfProcessor, AddSensor_1, 1, "void", "string")
{
   WsfSensorProcessor* procPtr = dynamic_cast<WsfSensorProcessor*>(aObjectPtr);
   if (procPtr != nullptr)
   {
      procPtr->AddSensor(nullptr, aVarArgs[0].GetString());
   }
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfProcessor, AddSensor_2, 2, "void", "string, string")
{
   WsfSensorProcessor* procPtr = dynamic_cast<WsfSensorProcessor*>(aObjectPtr);
   if (procPtr != nullptr)
   {
      procPtr->AddSensor(aVarArgs[0].GetString(), aVarArgs[1].GetString());
   }
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfProcessor, RemoveSensor_1, 1, "void", "string")
{
   WsfSensorProcessor* procPtr = dynamic_cast<WsfSensorProcessor*>(aObjectPtr);
   if (procPtr != nullptr)
   {
      procPtr->RemoveSensor(nullptr, aVarArgs[0].GetString());
   }
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfProcessor, RemoveSensor_2, 2, "void", "string, string")
{
   WsfSensorProcessor* procPtr = dynamic_cast<WsfSensorProcessor*>(aObjectPtr);
   if (procPtr != nullptr)
   {
      procPtr->RemoveSensor(aVarArgs[0].GetString(), aVarArgs[1].GetString());
   }
}

} // end of anonymous namespace

// =================================================================================================
//! Register the script methods associated with this class.
//! @note The WsfPlatform script class must be defined before calling this method.
// static
void WsfSensorProcessor::RegisterScriptMethods(UtScriptTypes& aScriptTypes)
{
   aScriptTypes.AddClassMethod("WsfProcessor", ut::make_unique<AddSensor_1>("AddSensor"));
   aScriptTypes.AddClassMethod("WsfProcessor", ut::make_unique<AddSensor_2>("AddSensor"));
   aScriptTypes.AddClassMethod("WsfProcessor", ut::make_unique<RemoveSensor_1>("RemoveSensor"));
   aScriptTypes.AddClassMethod("WsfProcessor", ut::make_unique<RemoveSensor_2>("RemoveSensor"));
   aScriptTypes.RegisterExtension(ut::make_unique<WsfSensorProcessorScriptExtensions>());
}


// =================================================================================================

bool WsfSensorProcessorScriptExtensions::AddExtClassMethods(const std::string& aClassName,
                                                            const std::string& aBaseName,
                                                            UtScriptTypes*     aScriptTypesPtr)
{
   if (aBaseName == aClassName)
   {
      // Should add base classes manually prior to initialization phase
   }
   else if (aBaseName == "WsfProcessor")
   {
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<AddSensor_1>("AddSensor"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<AddSensor_2>("AddSensor"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<RemoveSensor_1>("RemoveSensor"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<RemoveSensor_2>("RemoveSensor"));
   }
   else
   {
      return false;
   }
   return true;
}
