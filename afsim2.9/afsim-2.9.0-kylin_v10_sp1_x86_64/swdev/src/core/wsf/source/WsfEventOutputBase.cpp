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
#include "WsfEventOutputBase.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfBehaviorObserver.hpp"
#include "WsfCommObserver.hpp"
#include "WsfEventResults.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfFuelObserver.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfProcessorObserver.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfSystemLog.hpp"
#include "WsfTaskObserver.hpp"
#include "WsfTrackObserver.hpp"

namespace wsf
{
namespace event
{
namespace output
{

bool Data::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command;
   aInput.GetCommand(command);
   if (command == "disable" || command == "enable")
   {
      std::string eventName;
      aInput.ReadValue(eventName);
      if (eventName == "all")
      {
         mToggledEvents.clear();
      }
      else if (eventName == "PLATFORM_KILLED")
      {
         auto out = ut::log::warning() << "The PLATFORM_KILLED event has been deprecated and will be removed "
                                       << "in a future release. Use PLATFORM_BROKEN instead.";
         out.AddNote() << aInput.GetLocation();
      }
      // NOTE: Unfortunately because of the way things were restructured in 1.8, it is impossible to know
      //       if the specified event name is valid.  The registered event names aren't known until all
      //       simulation extensions have registered. We will eventually issue a message - we just won't
      //       be able to point the user to the location.
      mToggledEvents[eventName] = (command == "enable");
   }
   else if (command == "file")
   {
      aInput.ReadValueQuoted(mFileName);
      mFileName = aInput.SubstitutePathVariables(mFileName);
   }
   else if (command == "flush_output")
   {
      aInput.ReadValue(mFlushOutput);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool SimulationExtension::PrepareExtension()
{
   // Verify that every toggle event is actually the name of a valid event or event alias.
   bool ok = true;
   for (const auto& toggledEvent : mData.mToggledEvents)
   {
      std::string eventName(toggledEvent.first);
      if (eventName != "all")
      {
         if ((mEvents.find(eventName) == mEvents.end()) && (mEventAliases.find(eventName) == mEventAliases.end()))
         {
            auto out = ut::log::error() << "Invalid event name for " << GetExtensionName();
            out.AddNote() << "Event: " << eventName;
            ok = false;
         }
      }
   }

   if (!ok)
   {
      return false;
   }

   mIsInitialized = true;
   mIsInitialized = OpenFile(mData.mFileName);

   bool allEnabled = mData.mToggledEvents["all"];
   // replace aliased events
   // Note: If a user tries to use two event names that are aliased to the same event, e.g.:
   // enable AUTONOMY_LEVEL_CHANGED    disable OPERATING_LEVEL_CHANGED
   // or
   // disable AUTONOMY_LEVEL_CHANGED   enable OPERATING_LEVEL_CHANGED
   //    There is no way know at this point which command was processed last
   for (auto i = mData.mToggledEvents.begin(); i != mData.mToggledEvents.end();)
   {
      if (i->first != "all")
      {
         if (mEventAliases.find(i->first) != mEventAliases.end())
         {
            mData.mToggledEvents[mEventAliases[i->first]] = i->second;
            mData.mToggledEvents.erase(i++);
         }
         else
         {
            ++i;
         }
      }
      else
      {
         ++i;
      }
   }

   for (auto& evnt : mEvents)
   {
      bool isToggled    = mData.mToggledEvents.find(evnt.first) != mData.mToggledEvents.end();
      bool shouldEnable = (allEnabled && (!isToggled || mData.mToggledEvents[evnt.first])) ||
                          (!allEnabled && isToggled && mData.mToggledEvents[evnt.first]);
      EnableOrDisableEvent(evnt.first, shouldEnable);
   }

   return mIsInitialized;
}

// =================================================================================================
//! Opens a file for outputting events.
//! If a file is already open, it will be closed first before opening a new file.
bool SimulationExtension::OpenFile(const std::string& aFileName)
{
   bool ok         = true;
   mData.mFileName = aFileName;
   if (mIsInitialized)
   {
      if (mCurrentStream == &mFileStream)
      {
         mFileStream.close();
         mCurrentStream = nullptr;
      }

      if (mData.mFileName == "STDOUT")
      {
         SetStream(&std::cout);
      }
      else if (mData.mFileName == "NULL")
      {
         SetStream(nullptr);
      }
      else if (!mData.mFileName.empty())
      {
         std::string fileName = GetSimulation().SubstituteOutputFileVariables(mData.mFileName);
         mFileStream.open(fileName.c_str());
         if (!mFileStream)
         {
            SetStream(nullptr);
            auto out = ut::log::error() << "Unable to open " << GetExtensionName() << " file.";
            out.AddNote() << "File: " << fileName;
            ok = false;
         }
         else
         {
            if (!GetScenario().GetClassificationString().empty())
            {
               mFileStream << "Classification: " << GetScenario().GetClassificationString() << std::endl;
            }
            GetScenario().GetSystemLog().WriteOutputLogEntry("Event", fileName);
            SetStream(&mFileStream);
         }
      }
   }
   return ok;
}

bool SimulationExtension::IsEnabled(const std::string& aEventName) const
{
   return mEvents.at(aEventName).mEnabled;
}

// =================================================================================================
//! Adds a new output event.
//! @param aEventName The name of the event.  This name is used to reference the event in the input file.
//! @param aCallbackPtr A callback that is already connected to the event source.
//!                     UtCallback::Block() will be called if the event is disabled.
void SimulationExtension::AddEvent(const std::string& aEventName, std::unique_ptr<UtCallback> aCallbackPtr)
{
   EventData& data   = mEvents[aEventName];
   data.mCallbackPtr = std::move(aCallbackPtr);
   data.mCallbackPtr->Block(true);
   data.mEnabled = false;
}

// =================================================================================================
//! Adds an alternative name for an event.  The new name is usable from the input file.
//! @param aAliasEventName The new event alias name.
//! @param aCurrentEventName The name of the existing event to be aliased.
void SimulationExtension::AddEventAlias(const std::string& aAliasEventName, const std::string& aCurrentEventName)
{
   if (mEvents.find(aCurrentEventName) != mEvents.end())
   {
      mEventAliases[aAliasEventName] = aCurrentEventName;
   }
}

// =================================================================================================
//! Perform initial processing upon entry into processing an event.
//! All registered callback observers that issue output to the event stream should call this method
//! prior to issuing any output for the event.
void SimulationExtension::EventEntry()
{
   if (GetSimulation().MultiThreadingActive())
   {
      mMutex.lock();
   }
}

// =================================================================================================
//! Perform final processing upon exit from processing an event.
//! Registered callback observers that call EventEntry should call this method just prior to
//! leaving the method.
void SimulationExtension::EventExit()
{
   if (StreamIsOpen() && mData.mFlushOutput)
   {
      StreamRef().flush();
   }

   if (GetSimulation().MultiThreadingActive())
   {
      mMutex.unlock();
   }
}

// =================================================================================================
bool SimulationExtension::EnableOrDisableEvent(const std::string& aEventName, bool aEnable)
{
   std::string eventName(aEventName);
   if (mEventAliases.find(aEventName) != mEventAliases.end())
   {
      eventName = mEventAliases[aEventName];
   }

   bool enable = aEnable && StreamIsOpen();
   bool ok     = false;
   if (eventName == "all")
   {
      ok = true;
      for (auto& evnt : mEvents)
      {
         if (mCurrentStream != nullptr)
         {
            evnt.second.mCallbackPtr->Block(!enable);
         }
         evnt.second.mEnabled = aEnable;

         EnableOrDisableEventP(evnt.first, enable);
      }
   }
   else
   {
      auto iter = mEvents.find(eventName);
      if (iter != mEvents.end())
      {
         if (mCurrentStream != nullptr)
         {
            iter->second.mCallbackPtr->Block(!enable);
         }
         iter->second.mEnabled = aEnable;
         ok                    = true;

         EnableOrDisableEventP(eventName, enable);
      }
   }
   return ok;
}

// =================================================================================================
void SimulationExtension::RegisterMessagePrinter(WsfStringId aMessageType, const Settings::MessagePrintFunction& aFunction)
{
   mData.mSettings.RegisterMessagePrinter(aMessageType, aFunction);
}

// =================================================================================================
//! Set the stream used to output the events.  Allows derived
//! classes to output to other stream types.
//! @param aStreamPtr The new output stream.  If nullptr events will not be output.
// private
void SimulationExtension::SetStream(std::ostream* aStreamPtr)
{
   if (!mIsInitialized)
   {
      mData.mFileName.clear();
   }

   mCurrentStream = aStreamPtr;
   bool enable    = (mCurrentStream != nullptr);
   for (auto& evnt : mEvents)
   {
      if (evnt.second.mEnabled)
      {
         evnt.second.mCallbackPtr->Block(!enable);
      }
   }
}

// ===================================================================================================
void SimulationExtension::AddedToSimulation()
{
   WsfSimulation* simPtr = &GetSimulation();

   AddEvent<BehaviorTreeNodeChildren>("BTREE_NODE_CHILDREN", WsfObserver::BehaviorTreeNodeChildren(simPtr));
   AddEvent<BehaviorTreeNodeExec>("BTREE_NODE_EXEC", WsfObserver::BehaviorTreeNodeExec(simPtr));
   AddEvent<CommAddedToManager>("COMM_ADDED_TO_MANAGER", WsfObserver::CommAddedToManager(simPtr));
   AddEvent<CommRemovedFromManager>("COMM_REMOVED_FROM_MANAGER", WsfObserver::CommRemovedFromManager(simPtr));
   AddEvent<CommAddedToLocal>("COMM_ADDED_TO_LOCAL", WsfObserver::CommAddedToLocal(simPtr));
   AddEvent<CommRemovedFromLocal>("COMM_REMOVED_FROM_LOCAL", WsfObserver::CommRemovedFromLocal(simPtr));
   AddEvent<CommBroken>("COMM_BROKEN", WsfObserver::CommBroken(simPtr));
   AddEvent<CommFrequencyChanged>("COMM_FREQUENCY_CHANGED", WsfObserver::CommFrequencyChanged(simPtr));
   AddEvent<CommNonOperational>("COMM_NON_OPERATIONAL", WsfObserver::CommNonOperational(simPtr));
   AddEvent<CommOperational>("COMM_OPERATIONAL", WsfObserver::CommOperational(simPtr));
   AddEvent<CommTurnedOff>("COMM_TURNED_OFF", WsfObserver::CommTurnedOff(simPtr));
   AddEvent<CommTurnedOn>("COMM_TURNED_ON", WsfObserver::CommTurnedOn(simPtr));
   AddEvent<Comment>("COMMENT", WsfObserver::Comment(simPtr));
   AddEvent<CrashedIntoGround>("CRASHED_INTO_GROUND", WsfObserver::CrashedIntoGround(simPtr));
   AddEvent<ExchangeCompleted>("EXCHANGE_COMPLETED", WsfObserver::ExchangeCompleted(simPtr));
   AddEvent<ExchangeNegotiated>("EXCHANGE_NEGOTIATED", WsfObserver::ExchangeNegotiated(simPtr));
   AddEvent<ExchangeQueried>("EXCHANGE_QUERIED", WsfObserver::ExchangeQueried(simPtr));
   AddEvent<ExchangeRequestFailed>("EXCHANGE_REQUEST_FAILED", WsfObserver::ExchangeRequestFailed(simPtr));
   AddEvent<ExecuteCallback>("EXECUTE_CALLBACK", WsfObserver::ExecuteCallback(simPtr));
   AddEvent<FuelEvent>("FUEL_EVENT", WsfObserver::FuelEvent(simPtr));
   AddEvent<ImageCreated>("IMAGE_CREATED", WsfObserver::ImageCreated(simPtr));
   AddEvent<LinkAddedToManager>("LINK_ADDED_TO_MANAGER", WsfObserver::LinkAddedToManager(simPtr));
   AddEvent<LinkRemovedFromManager>("LINK_REMOVED_FROM_MANAGER", WsfObserver::LinkRemovedFromManager(simPtr));
   AddEvent<LinkEnabledOnManager>("LINK_ENABLED_ON_MANAGER", WsfObserver::LinkEnabledOnManager(simPtr));
   AddEvent<LinkDisabledOnManager>("LINK_DISABLED_ON_MANAGER", WsfObserver::LinkDisabledOnManager(simPtr));
   AddEvent<LinkAddedToLocal>("LINK_ADDED_TO_LOCAL", WsfObserver::LinkAddedToLocal(simPtr));
   AddEvent<LinkRemovedFromLocal>("LINK_REMOVED_FROM_LOCAL", WsfObserver::LinkRemovedFromLocal(simPtr));
   AddEvent<LinkEnabledOnLocal>("LINK_ENABLED_ON_LOCAL", WsfObserver::LinkEnabledOnLocal(simPtr));
   AddEvent<LinkDisabledOnLocal>("LINK_DISABLED_ON_LOCAL", WsfObserver::LinkDisabledOnLocal(simPtr));
   AddEvent<LocalTrackCorrelation>("LOCAL_TRACK_CORRELATION", WsfObserver::LocalTrackCorrelation(simPtr));
   AddEvent<LocalTrackDecorrelation>("LOCAL_TRACK_DECORRELATION", WsfObserver::LocalTrackDecorrelation(simPtr));
   AddEvent<LocalTrackDropped>("LOCAL_TRACK_DROPPED", WsfObserver::LocalTrackDropped(simPtr));
   AddEvent<LocalTrackInitiated>("LOCAL_TRACK_INITIATED", WsfObserver::LocalTrackInitiated(simPtr));
   AddEvent<LocalTrackUpdated>("LOCAL_TRACK_UPDATED", WsfObserver::LocalTrackUpdated(simPtr));
   AddEvent<MessageDeliveryAttempt>("MESSAGE_DELIVERY_ATTEMPT", WsfObserver::MessageDeliveryAttempt(simPtr));
   AddEvent<MessageDiscarded>("MESSAGE_DISCARDED", WsfObserver::MessageDiscarded(simPtr));
   AddEvent<MessageFailedRouting>("MESSAGE_FAILED_ROUTING", WsfObserver::MessageFailedRouting(simPtr));
   AddEvent<MessageHop>("MESSAGE_HOP", WsfObserver::MessageHop(simPtr));
   AddEvent<MessageQueued>("MESSAGE_QUEUED", WsfObserver::MessageQueued(simPtr));
   AddEvent<MessageReceived>("MESSAGE_RECEIVED", WsfObserver::MessageReceived(simPtr));
   AddEvent<MessageTransmitted>("MESSAGE_TRANSMITTED", WsfObserver::MessageTransmitted(simPtr));
   AddEvent<MessageTransmitEnded>("MESSAGE_TRANSMIT_ENDED", WsfObserver::MessageTransmitEnded(simPtr));
   AddEvent<MessageTransmittedHeartbeat>("MESSAGE_TRANSMITTED_HEARTBEAT", WsfObserver::MessageTransmittedHeartbeat(simPtr));
   AddEvent<MessageUpdated>("MESSAGE_UPDATED", WsfObserver::MessageUpdated(simPtr));
   AddEvent<MoverBroken>("MOVER_BROKEN", WsfObserver::MoverBroken(simPtr));
   AddEvent<MoverBurnedOut>("MOVER_BURNED_OUT", WsfObserver::MoverBurnedOut(simPtr));
   AddEvent<MoverNonOperational>("MOVER_NON_OPERATIONAL", WsfObserver::MoverNonOperational(simPtr));
   AddEvent<MoverOperational>("MOVER_OPERATIONAL", WsfObserver::MoverOperational(simPtr));
   AddEvent<MoverStaged>("MOVER_STAGED", WsfObserver::MoverStaged(simPtr));
   AddEvent<MoverTurnedOff>("MOVER_TURNED_OFF", WsfObserver::MoverTurnedOff(simPtr));
   AddEvent<MoverTurnedOn>("MOVER_TURNED_ON", WsfObserver::MoverTurnedOn(simPtr));
   AddEvent<NavigationStatusChanged>("NAVIGATION_STATUS_CHANGED", WsfObserver::NavigationStatusChanged(simPtr));
   AddEvent<NetworkAdded>("NETWORK_ADDED", WsfObserver::NetworkAdded(simPtr));
   AddEvent<NetworkRemoved>("NETWORK_REMOVED", WsfObserver::NetworkRemoved(simPtr));
   AddEvent<OperatingLevelChanged>("OPERATING_LEVEL_CHANGED", WsfObserver::OperatingLevelChanged(simPtr));
   AddEvent<PlatformAdded>("PLATFORM_ADDED", WsfObserver::PlatformAdded(simPtr));
   AddEvent<PlatformAppearanceChanged>("PLATFORM_APPEARANCE_CHANGED", WsfObserver::PlatformAppearanceChanged(simPtr));
   AddEvent<PlatformBroken>("PLATFORM_BROKEN", WsfObserver::PlatformBroken(simPtr));
   AddEvent<PlatformCapabilityChanged>("PLATFORM_CAPABILITY_CHANGED", WsfObserver::PlatformCapabilityChanged(simPtr));
   AddEvent<PlatformDeleted>("PLATFORM_DELETED", WsfObserver::PlatformDeleted(simPtr));
   AddEvent<PlatformInitialized>("PLATFORM_INITIALIZED", WsfObserver::PlatformInitialized(simPtr));
   AddEvent<PlatformOmitted>("PLATFORM_OMITTED", WsfObserver::PlatformOmitted(simPtr));
   AddEvent<ProcessorBroken>("PROCESSOR_BROKEN", WsfObserver::ProcessorBroken(simPtr));
   AddEvent<ProcessorNonOperational>("PROCESSOR_NON_OPERATIONAL", WsfObserver::ProcessorNonOperational(simPtr));
   AddEvent<ProcessorOperational>("PROCESSOR_OPERATIONAL", WsfObserver::ProcessorOperational(simPtr));
   AddEvent<ProcessorTurnedOff>("PROCESSOR_TURNED_OFF", WsfObserver::ProcessorTurnedOff(simPtr));
   AddEvent<ProcessorTurnedOn>("PROCESSOR_TURNED_ON", WsfObserver::ProcessorTurnedOn(simPtr));
   AddEvent<RouterBroken>("ROUTER_BROKEN", WsfObserver::RouterBroken(simPtr));
   AddEvent<RouterNonOperational>("ROUTER_NON_OPERATIONAL", WsfObserver::RouterNonOperational(simPtr));
   AddEvent<RouterOperational>("ROUTER_OPERATIONAL", WsfObserver::RouterOperational(simPtr));
   AddEvent<RouterTurnedOff>("ROUTER_TURNED_OFF", WsfObserver::RouterTurnedOff(simPtr));
   AddEvent<RouterTurnedOn>("ROUTER_TURNED_ON", WsfObserver::RouterTurnedOn(simPtr));
   AddEvent<SensorBroken>("SENSOR_BROKEN", WsfObserver::SensorBroken(simPtr));
   AddEvent<SensorDetectionAttempt>("SENSOR_DETECTION_ATTEMPT", WsfObserver::SensorDetectionAttempt(simPtr));
   AddEvent<SensorDetectionChanged>("SENSOR_DETECTION_CHANGED", WsfObserver::SensorDetectionChanged(simPtr));
   AddEvent<SensorFrequencyChanged>("SENSOR_FREQUENCY_CHANGED", WsfObserver::SensorFrequencyChanged(simPtr));
   AddEvent<SensorModeActivated>("SENSOR_MODE_ACTIVATED", WsfObserver::SensorModeActivated(simPtr));
   AddEvent<SensorModeDeactivated>("SENSOR_MODE_DEACTIVATED", WsfObserver::SensorModeDeactivated(simPtr));
   AddEvent<SensorNonOperational>("SENSOR_NON_OPERATIONAL", WsfObserver::SensorNonOperational(simPtr));
   AddEvent<SensorOperational>("SENSOR_OPERATIONAL", WsfObserver::SensorOperational(simPtr));
   AddEvent<SensorRequestCanceled>("SENSOR_REQUEST_CANCELED", WsfObserver::SensorRequestCanceled(simPtr));
   AddEvent<SensorRequestInitiated>("SENSOR_REQUEST_INITIATED", WsfObserver::SensorRequestInitiated(simPtr));
   AddEvent<SensorRequestUpdated>("SENSOR_REQUEST_UPDATED", WsfObserver::SensorRequestUpdated(simPtr));
   AddEvent<SensorTrackCoasted>("SENSOR_TRACK_COASTED", WsfObserver::SensorTrackCoasted(simPtr));
   AddEvent<SensorTrackDropped>("SENSOR_TRACK_DROPPED", WsfObserver::SensorTrackDropped(simPtr));
   AddEvent<SensorTrackInitiated>("SENSOR_TRACK_INITIATED", WsfObserver::SensorTrackInitiated(simPtr));
   AddEvent<SensorTrackUpdated>("SENSOR_TRACK_UPDATED", WsfObserver::SensorTrackUpdated(simPtr));
   AddEvent<SensorTurnedOff>("SENSOR_TURNED_OFF", WsfObserver::SensorTurnedOff(simPtr));
   AddEvent<SensorTurnedOn>("SENSOR_TURNED_ON", WsfObserver::SensorTurnedOn(simPtr));
   AddEvent("SIMULATION_STARTING",
            WsfObserver::SimulationStarting(simPtr).Connect(&SimulationExtension::SimulationStarting, this));
   AddEvent("SIMULATION_COMPLETE",
            WsfObserver::SimulationComplete(simPtr).Connect(&SimulationExtension::SimulationComplete, this));
   AddEvent<StateEntry>("STATE_ENTRY", WsfObserver::StateEntry(simPtr));
   AddEvent<StateExit>("STATE_EXIT", WsfObserver::StateExit(simPtr));
   AddEvent<TankingEvent>("TANKING_EVENT", WsfObserver::TankingEvent(simPtr));
   AddEvent<TaskAssigned>("TASK_ASSIGNED", WsfObserver::TaskAssigned(simPtr));
   AddEvent<TaskCanceled>("TASK_CANCELED", WsfObserver::TaskCanceled(simPtr));
   AddEvent<TaskCompleted>("TASK_COMPLETED", WsfObserver::TaskCompleted(simPtr));
   AddEvent<PlatformAddedTeamName>("TEAM_NAME_DEFINITION", WsfObserver::PlatformAdded(simPtr));

   // Included for backward compatibility
   AddEventAlias("AUTONOMY_LEVEL_CHANGED", "OPERATING_LEVEL_CHANGED");
   AddEventAlias("RAN_OUT_OF_FUEL", "FUEL_EVENT");
}

// ----------------------------------------------------------------------------
//                 Start of simulation observer callbacks
// ----------------------------------------------------------------------------

// ===================================================================================================
void SimulationExtension::SimulationComplete(double aSimTime)
{
   EventGuard                     guard(*this);
   wsf::event::SimulationComplete wsfevent(aSimTime, GetSimulation(), mData.mSettings);
   PrintEvent(wsfevent);

   // Notify the derived class before we shut the event output system down.
   OnSimulationComplete(aSimTime);

   size_t entryCount = GetSimulation().GetPlatformCount();
   if (mCurrentStream && IsEnabled("PLATFORM_DELETED"))
   {
      for (size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex)
      {
         ObserverCallback<PlatformDeleted>(aSimTime, GetSimulation().GetPlatformEntry(entryIndex));
      }
   }
   mFileStream.close();
   SetStream(nullptr);
}

// ===================================================================================================
void SimulationExtension::SimulationStarting()
{
   EventGuard                     guard(*this);
   wsf::event::SimulationStarting wsfevent(0, GetSimulation(), mData.mSettings);
   PrintEvent(wsfevent);
}

} // namespace output
} // namespace event
} // namespace wsf
