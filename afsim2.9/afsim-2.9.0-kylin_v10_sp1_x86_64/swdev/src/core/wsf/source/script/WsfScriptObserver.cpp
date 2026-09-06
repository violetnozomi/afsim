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

#include "WsfScriptObserver.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfCallback.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetwork.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommRouterProtocolInterface.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfFuelObserver.hpp"
#include "WsfFuelingOperation.hpp"
#include "WsfImage.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMover.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfProcessor.hpp"
#include "WsfProcessorObserver.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSignatureList.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfTankedFuel.hpp"
#include "WsfTaskObserver.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackObserver.hpp"
#include "script/WsfScriptContext.hpp"

namespace
{

inline void ExecuteScriptList(double aSimTime, WsfScriptContext& aGlobalContext, const std::vector<UtScript*>& aScriptList)
{
   for (auto script : aScriptList)
   {
      aGlobalContext.ExecuteScript(aSimTime, script);
   }
}

inline void ExecuteScriptList(double                        aSimTime,
                              WsfScriptContext&             aGlobalContext,
                              const std::vector<UtScript*>& aScriptList,
                              UtScriptData&                 aScriptRetVal,
                              UtScriptDataList&             aScriptArgs)
{
   for (auto script : aScriptList)
   {
      aGlobalContext.ExecuteScript(aSimTime, script, aScriptRetVal, aScriptArgs);
   }
}

// Helper execution functions which find the global context from any other context
inline void ExecuteScriptList(double aSimTime, UtScriptContext& aThisContext, const std::vector<UtScript*>& aScriptList)
{
   WsfScriptContext& globalContext = WsfScriptContext::GetSIMULATION(aThisContext)->GetScriptContext();
   ExecuteScriptList(aSimTime, globalContext, aScriptList);
}

inline void ExecuteScriptList(double                        aSimTime,
                              UtScriptContext&              aThisContext,
                              const std::vector<UtScript*>& aScriptList,
                              UtScriptData&                 aScriptRetVal,
                              UtScriptDataList&             aScriptArgs)
{
   WsfScriptContext& globalContext = WsfScriptContext::GetSIMULATION(aThisContext)->GetScriptContext();
   ExecuteScriptList(aSimTime, globalContext, aScriptList, aScriptRetVal, aScriptArgs);
}
} // namespace

// ****************************************************************************
// Public
WsfScriptObserver* WsfScriptObserver::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfScriptObserver*>(aSimulation.FindExtension("script_observer"));
}

// ============================================================================
bool WsfScriptObserver::Initialize()
{
   mInitializeSuccess = true;
   for (const auto& enabledEvent : mExtensionPtr->mEnabledEvents)
   {
      std::string callbackName = enabledEvent.first;
      for (const std::string& scriptName : enabledEvent.second)
      {
         mInitializeSuccess = EnableOrDisable(true, callbackName, scriptName) && mInitializeSuccess;
      }
   }

   if (!mInitializeSuccess)
   {
      return false;
   }
   // This method is called through GetSimulation().GetObserver().Initialize during simulation initialization,
   // so invoke any registered script observers for simulation initialization.

   ExecuteScriptList(0.0, GetScriptList(GetEventIndex("SIMULATION_INITIALIZING")));
   return mInitializeSuccess;
}

void WsfScriptObserver::Start()
{
   ExecuteScriptList(0.0, GetScriptList(GetEventIndex("SIMULATION_STARTING")));
}

// ============================================================================
int WsfScriptObserver::AddEventP(const std::string&          aEventName,
                                 std::unique_ptr<UtCallback> aCallbackPtr,
                                 const std::string&          aDefaultScriptName,
                                 const std::string&          aExpectedArgumentTypes)
{
   int index                 = static_cast<int>(mEvents.size());
   mEventToIndex[aEventName] = index;

   EventData data;
   data.mCallbackPtr = aCallbackPtr.get();
   if (data.mCallbackPtr != nullptr)
   {
      data.mCallbackPtr->Block(true);
   }
   data.mEnabled               = false;
   data.mDefaultScriptName     = aDefaultScriptName;
   data.mExpectedArgumentTypes = aExpectedArgumentTypes;
   mEvents.push_back(data);
   mCallbacks += std::move(aCallbackPtr);

   return index;
}

// ============================================================================
int WsfScriptObserver::GetEventIndex(const std::string& aEventName) const
{
   int  index = -1;
   auto it    = mEventToIndex.find(aEventName);
   if (it != mEventToIndex.end())
   {
      index = it->second;
   }
   return index;
}

// =================================================================================================
//! Perform initial processing upon entry into processing an event.
//! All registered callback observers that issue output to the event stream should call this method
//! prior to issuing any output for the event.
void WsfScriptObserver::EventEntry()
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
void WsfScriptObserver::EventExit()
{
   if (GetSimulation().MultiThreadingActive())
   {
      mMutex.unlock();
   }
}

namespace
{
template<typename PART>
void PlatformPartPacker(UtScriptDataPacker& aScriptArgs, PART* aPlatformPartPtr)
{
   aScriptArgs << aPlatformPartPtr->GetPlatform() << aPlatformPartPtr;
}

// ============================================================================
void CommLocalPacker(UtScriptDataPacker&                   aScriptArgs,
                     wsf::comm::router::Router*            aLocalRouterPtr,
                     wsf::comm::router::ProtocolInterface* aProtocolPtr,
                     wsf::comm::Address*                   aAddressPtr)
{
   aScriptArgs << aLocalRouterPtr << aProtocolPtr->GetType() << aAddressPtr;
}

// ============================================================================
void ExecuteCallbackPacker(UtScriptDataPacker& aScriptArgs, WsfPlatform* aPlatformPtr, const WsfCallback& aCallback)
{
   aScriptArgs << aPlatformPtr << aCallback.GetName();
}

// ============================================================================
void FuelEventPacker(UtScriptDataPacker& aScriptArgs, WsfFuel* aFuelPtr, WsfStringId aEventNameId)
{
   aScriptArgs << aFuelPtr->GetPlatform() << aFuelPtr << aEventNameId;
}

// ============================================================================
void LinkLocalPacker(UtScriptDataPacker&                   aScriptArgs,
                     wsf::comm::router::Router*            aLocalRouterPtr,
                     wsf::comm::router::ProtocolInterface* aProtocolPtr,
                     wsf::comm::Address*                   aSourceAddressPtr,
                     wsf::comm::Address*                   aDestinationAddressPtr)
{
   aScriptArgs << aLocalRouterPtr << aProtocolPtr->GetType() << aSourceAddressPtr << aDestinationAddressPtr;
}

// ============================================================================
void OperatingLevelChangedPacker(UtScriptDataPacker& aScriptArgs, WsfProcessor* aProcessorPtr, WsfStringId aName, int aLevel)
{
   aScriptArgs << aProcessorPtr->GetPlatform() << aProcessorPtr << aName << aLevel;
}

void PlatformAppearanceChangedPacker(UtScriptDataPacker&         aScriptArgs,
                                     WsfPlatform*                aPlatformPtr,
                                     WsfObserver::AppearanceType aAppearanceType)
{
   std::string appearanceType("unknown");
   if (aAppearanceType == WsfObserver::cAPPEARANCE_WORD)
   {
      appearanceType = "appearance_word";
   }
   else if (aAppearanceType == WsfObserver::cCONCEALMENT_FACTOR)
   {
      appearanceType = "concealment_factor";
   }
   else if ((aAppearanceType >= WsfObserver::cSIGNATURE_STATE_START) &&
            (aAppearanceType <= WsfObserver::cSIGNATURE_STATE_LIMIT))
   {
      WsfSignatureList& sigList(aPlatformPtr->GetSignatureList());
      size_t            sigIndex(static_cast<size_t>(aAppearanceType - WsfObserver::cSIGNATURE_STATE_START));
#undef GetClassName
      appearanceType = sigList.GetClassName(sigIndex);
   }
   else if ((aAppearanceType >= WsfObserver::cSIGNATURE_SCALE_FACTOR_START) &&
            (aAppearanceType <= WsfObserver::cSIGNATURE_SCALE_FACTOR_LIMIT))
   {
      WsfSignatureList& sigList(aPlatformPtr->GetSignatureList());
      size_t            sigIndex(static_cast<size_t>(aAppearanceType - WsfObserver::cSIGNATURE_SCALE_FACTOR_START));
#undef GetClassName
      appearanceType = sigList.GetClassName(sigIndex);
   }
   aScriptArgs << aPlatformPtr << appearanceType;
}

// ============================================================================
void SensorDetectionAttemptPacker(UtScriptDataPacker& aScriptArgs,
                                  WsfSensor*          aSensorPtr,
                                  WsfPlatform*        aTargetPtr,
                                  WsfSensorResult&    aResult)
{
   aScriptArgs << aSensorPtr->GetPlatform() << aSensorPtr << aTargetPtr << aResult;
}

// ============================================================================
void SensorDetectionChangedPacker(UtScriptDataPacker& aScriptArgs,
                                  WsfSensor*          aSensorPtr,
                                  size_t              aTargetIndex,
                                  WsfSensorResult&    aResult)
{
   aScriptArgs << aSensorPtr->GetPlatform() << aSensorPtr << aTargetIndex << aResult;
}

// ============================================================================
void SensorModePacker(UtScriptDataPacker& aScriptArgs, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr)
{
   aScriptArgs << aSensorPtr->GetPlatform() << aSensorPtr;
}

// ============================================================================
void SensorRequestCanceledPacker(UtScriptDataPacker& aScriptArgs, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   aScriptArgs << aSensorPtr->GetPlatform() << aSensorPtr << aTrackPtr;
}

// ============================================================================
void SensorRequestInitiatedPacker(UtScriptDataPacker& aScriptArgs,
                                  WsfSensor*          aSensorPtr,
                                  WsfSensorMode*      aModePtr,
                                  const WsfTrack*     aTrackPtr)
{
   aScriptArgs << aSensorPtr->GetPlatform() << aSensorPtr << aTrackPtr;
}

// ============================================================================
void SensorRequestUpdatedPacker(UtScriptDataPacker& aScriptArgs,
                                WsfSensor*          aSensorPtr,
                                WsfSensorMode*      aModePtr,
                                const WsfTrack*     aTrackPtr)
{
   aScriptArgs << aSensorPtr->GetPlatform() << aSensorPtr << aTrackPtr;
}

// ============================================================================
void SensorTrackPacker(UtScriptDataPacker& aScriptArgs, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   // Ignore pseudo-sensor tracks used for visualization (kludge)
   if (aTrackPtr->GetTrackType() == WsfTrack::cPSEUDO_SENSOR)
   {
      return;
   }

   aScriptArgs << aSensorPtr->GetPlatform() << aSensorPtr << aTrackPtr;
}

// ============================================================================
void StatePacker(UtScriptDataPacker& aScriptArgs, WsfProcessor* aProcessorPtr, const WsfTrack* aTrackPtr, WsfStringId aStateNameId)
{
   aScriptArgs << aProcessorPtr->GetPlatform() << aProcessorPtr << aTrackPtr << aStateNameId;
}

// ============================================================================
void TankingEventPacker(UtScriptDataPacker& aScriptArgs, WsfFuelingOperation* aFuelingOpPtr, WsfStringId aEventNameId)
{
   aScriptArgs << aFuelingOpPtr->SupplyTankPtr()->GetPlatform() << aFuelingOpPtr->ReceiveTankPtr()->GetPlatform()
               << aEventNameId;
}

} // namespace

// ****************************************************************************
// Private

// ============================================================================
WsfScriptObserver::WsfScriptObserver(WsfSimulation& aSimulation, const WsfScriptObserverExtension& aExtension)
   : WsfSimulationExtension()
   , mEvents()
   , mExtensionPtr(&aExtension)
{
}

// ============================================================================
void WsfScriptObserver::AddedToSimulation()
{
   AddEvent("COMMENT", WsfObserver::Comment(&GetSimulation()), "Comment", "WsfPlatform, string");
   AddEvent("COMM_FREQUENCY_CHANGED", WsfObserver::CommFrequencyChanged(&GetSimulation()), "CommFrequencyChanged", "WsfComm");
   AddEvent("COMM_TURNED_OFF", WsfObserver::CommTurnedOff(&GetSimulation()), "CommTurnedOff", "WsfComm");
   AddEvent("COMM_TURNED_ON", WsfObserver::CommTurnedOn(&GetSimulation()), "CommTurnedOn", "WsfComm");
   AddEvent("COMM_ADDED_TO_MANAGER", WsfObserver::CommAddedToManager(&GetSimulation()), "CommAddedToManager", "WsfComm");
   AddEvent("COMM_REMOVED_FROM_MANAGER",
            WsfObserver::CommRemovedFromManager(&GetSimulation()),
            "CommRemovedFromManager",
            "WsfComm");
   AddEvent("COMM_ADDED_TO_LOCAL",
            WsfObserver::CommAddedToLocal(&GetSimulation()),
            "CommAddedToLocal",
            "WsfCommRouter, string, WsfAddress",
            UtStd::Bind(&CommLocalPacker));
   AddEvent("COMM_REMOVED_FROM_LOCAL",
            WsfObserver::CommRemovedFromLocal(&GetSimulation()),
            "CommRemovedFromLocal",
            "WsfCommRouter, string, WsfAddress",
            UtStd::Bind(&CommLocalPacker));
   AddEvent("CRASHED_INTO_GROUND", WsfObserver::CrashedIntoGround(&GetSimulation()), "CrashedIntoGround", "WsfPlatform");
   AddEvent("EXECUTE_CALLBACK",
            WsfObserver::ExecuteCallback(&GetSimulation()),
            "ExecuteCallback",
            "WsfPlatform, string",
            UtStd::Bind(&ExecuteCallbackPacker));
   AddEvent("FUEL_EVENT",
            WsfObserver::FuelEvent(&GetSimulation()),
            "FuelEvent",
            "WsfPlatform, WsfFuel, string",
            UtStd::Bind(&FuelEventPacker));
   AddEvent("IMAGE_CREATED", WsfObserver::ImageCreated(&GetSimulation()), "ImageCreated", "WsfSensor, WsfImage");
   AddEvent("LINK_ADDED_TO_MANAGER", WsfObserver::LinkAddedToManager(&GetSimulation()), "LinkAddedToManager", "WsfComm, WsfComm");
   AddEvent("LINK_REMOVED_FROM_MANAGER",
            WsfObserver::LinkRemovedFromManager(&GetSimulation()),
            "LinkRemovedFromManager",
            "WsfComm, WsfComm");
   AddEvent("LINK_ENABLED_ON_MANAGER",
            WsfObserver::LinkEnabledOnManager(&GetSimulation()),
            "LinkEnabledOnManager",
            "WsfComm, WsfComm");
   AddEvent("LINK_DISABLED_ON_MANAGER",
            WsfObserver::LinkDisabledOnManager(&GetSimulation()),
            "LinkDisabledOnManager",
            "WsfComm, WsfComm");
   AddEvent("LINK_ADDED_TO_LOCAL",
            WsfObserver::LinkAddedToLocal(&GetSimulation()),
            "LinkAddedToLocal",
            "WsfCommRouter, string, WsfAddress, WsfAddress",
            UtStd::Bind(&LinkLocalPacker));
   AddEvent("LINK_REMOVED_FROM_LOCAL",
            WsfObserver::LinkRemovedFromLocal(&GetSimulation()),
            "LinkRemovedFromLocal",
            "WsfCommRouter, string, WsfAddress, WsfAddress",
            UtStd::Bind(&LinkLocalPacker));
   AddEvent("LINK_ENABLED_ON_LOCAL",
            WsfObserver::LinkEnabledOnLocal(&GetSimulation()),
            "LinkEnabledOnLocal",
            "WsfCommRouter, string, WsfAddress, WsfAddress",
            UtStd::Bind(&LinkLocalPacker));
   AddEvent("LINK_DISABLED_ON_LOCAL",
            WsfObserver::LinkDisabledOnLocal(&GetSimulation()),
            "LinkDisabledOnLocal",
            "WsfCommRouter, string, WsfAddress, WsfAddress",
            UtStd::Bind(&LinkLocalPacker));
   AddEvent("LOCAL_TRACK_CORRELATION",
            WsfObserver::LocalTrackCorrelation(&GetSimulation()),
            "LocalTrackCorrelation",
            "WsfPlatform, WsfTrackId, WsfTrackId");
   AddEvent("LOCAL_TRACK_DECORRELATION",
            WsfObserver::LocalTrackDecorrelation(&GetSimulation()),
            "LocalTrackDecorrelation",
            "WsfPlatform, WsfTrackId, WsfTrackId");
   AddEvent("LOCAL_TRACK_DROPPED",
            WsfObserver::LocalTrackDropped(&GetSimulation()),
            "LocalTrackDropped",
            "WsfPlatform, WsfLocalTrack");
   AddEvent("LOCAL_TRACK_INITIATED",
            WsfObserver::LocalTrackInitiated(&GetSimulation()),
            "LocalTrackInitiated",
            "WsfPlatform, WsfLocalTrack, WsfTrack");
   AddEvent("LOCAL_TRACK_UPDATED",
            WsfObserver::LocalTrackUpdated(&GetSimulation()),
            "LocalTrackUpdated",
            "WsfPlatform, WsfLocalTrack, WsfTrack");
   AddEvent("MESSAGE_DELIVERY_ATTEMPT",
            WsfObserver::MessageDeliveryAttempt(&GetSimulation()),
            "MessageDeliveryAttempt",
            "WsfComm, WsfComm, WsfMessage, WsfCommInteraction");
   AddEvent("MESSAGE_DISCARDED",
            WsfObserver::MessageDiscarded(&GetSimulation()),
            "MessageDiscarded",
            "WsfComm, WsfMessage, string");
   AddEvent("MESSAGE_FAILED_ROUTING",
            WsfObserver::MessageFailedRouting(&GetSimulation()),
            "MessageFailedRouting",
            "WsfComm, WsfPlatform, WsfMessage");
   AddEvent("MESSAGE_HOP", WsfObserver::MessageHop(&GetSimulation()), "MessageHop", "WsfComm, WsfComm, WsfMessage");
   AddEvent("MESSAGE_QUEUED", WsfObserver::MessageQueued(&GetSimulation()), "MessageQueued", "WsfComm, WsfMessage, int");
   AddEvent("MESSAGE_RECEIVED",
            WsfObserver::MessageReceived(&GetSimulation()),
            "MessageReceived",
            "WsfComm, WsfComm, WsfMessage, WsfCommInteraction");
   AddEvent("MESSAGE_TRANSMITTED",
            WsfObserver::MessageTransmitted(&GetSimulation()),
            "MessageTransmitted",
            "WsfComm, WsfMessage");
   AddEvent("MESSAGE_TRANSMIT_ENDED",
            WsfObserver::MessageTransmitEnded(&GetSimulation()),
            "MessageTransmitEnded",
            "WsfComm, WsfMessage");
   AddEvent("MESSAGE_TRANSMITTED_HEARTBEAT",
            WsfObserver::MessageTransmittedHeartbeat(&GetSimulation()),
            "MessageTransmittedHeartbeat",
            "WsfComm, WsfMessage");
   AddEvent("MESSAGE_UPDATED",
            WsfObserver::MessageUpdated(&GetSimulation()),
            "MessageUpdated",
            "WsfComm, WsfMessage, WsfMessage");
   AddEvent("MOVER_BROKEN",
            WsfObserver::MoverBroken(&GetSimulation()),
            "MoverBroken",
            "WsfPlatform, WsfMover",
            UtStd::Bind(&PlatformPartPacker<WsfMover>));
   AddEvent("MOVER_BURNED_OUT",
            WsfObserver::MoverBurnedOut(&GetSimulation()),
            "MoverBurnedOut",
            "WsfPlatform, WsfMover",
            UtStd::Bind(&PlatformPartPacker<WsfMover>));
   AddEvent("MOVER_STAGED",
            WsfObserver::MoverStaged(&GetSimulation()),
            "MoverStaged",
            "WsfPlatform, WsfMover",
            UtStd::Bind(&PlatformPartPacker<WsfMover>));
   AddEvent("MOVER_UPDATED",
            WsfObserver::MoverUpdated(&GetSimulation()),
            "MoverUpdated",
            "WsfPlatform, WsfMover",
            UtStd::Bind(&PlatformPartPacker<WsfMover>));
   AddEvent("NETWORK_ADDED", WsfObserver::NetworkAdded(&GetSimulation()), "NetworkAdded", "WsfNetwork");
   AddEvent("NETWORK_REMOVED", WsfObserver::NetworkRemoved(&GetSimulation()), "NetworkRemoved", "WsfNetwork");
   AddEvent("OPERATING_LEVEL_CHANGED",
            WsfObserver::OperatingLevelChanged(&GetSimulation()),
            "OperatingLevelChanged",
            "WsfPlatform, WsfProcessor, string, int",
            UtStd::Bind(&OperatingLevelChangedPacker));
   AddEvent("PLATFORM_ADDED", WsfObserver::PlatformAdded(&GetSimulation()), "PlatformAdded", "WsfPlatform");
   AddEvent("PLATFORM_APPEARANCE_CHANGED",
            WsfObserver::PlatformAppearanceChanged(&GetSimulation()),
            "PlatformAppearanceChanged",
            "WsfPlatform, string",
            UtStd::Bind(&PlatformAppearanceChangedPacker));
   AddEvent("PLATFORM_BROKEN", WsfObserver::PlatformBroken(&GetSimulation()), "PlatformBroken", "WsfPlatform");
   AddEvent("PLATFORM_CAPABILITY_CHANGED",
            WsfObserver::PlatformCapabilityChanged(&GetSimulation()),
            "PlatformCapabilityChanged",
            "WsfPlatform, string, bool");
   AddEvent("PLATFORM_DELETED", WsfObserver::PlatformDeleted(&GetSimulation()), "PlatformDeleted", "WsfPlatform");
   AddEvent("PLATFORM_INITIALIZED", WsfObserver::PlatformInitialized(&GetSimulation()), "PlatformInitialized", "WsfPlatform");
   AddEvent("PLATFORM_OMITTED", WsfObserver::PlatformOmitted(&GetSimulation()), "PlatformOmitted", "WsfPlatform");
   AddEvent("PROCESSOR_TURNED_OFF",
            WsfObserver::ProcessorTurnedOff(&GetSimulation()),
            "ProcessorTurnedOff",
            "WsfPlatform, WsfProcessor",
            UtStd::Bind(&PlatformPartPacker<WsfProcessor>));
   AddEvent("PROCESSOR_TURNED_ON",
            WsfObserver::ProcessorTurnedOn(&GetSimulation()),
            "ProcessorTurnedOn",
            "WsfPlatform, WsfProcessor",
            UtStd::Bind(&PlatformPartPacker<WsfProcessor>));
   AddEvent("ROUTER_TURNED_OFF",
            WsfObserver::RouterTurnedOff(&GetSimulation()),
            "RouterTurnedOff",
            "WsfPlatform, WsfCommRouter",
            UtStd::Bind(&PlatformPartPacker<wsf::comm::router::Router>));
   AddEvent("ROUTER_TURNED_ON",
            WsfObserver::RouterTurnedOn(&GetSimulation()),
            "RouterTurnedOn",
            "WsfPlatform, WsfCommRouter",
            UtStd::Bind(&PlatformPartPacker<wsf::comm::router::Router>));
   AddEvent<true>("SENSOR_DETECTION_ATTEMPT",
                  WsfObserver::SensorDetectionAttempt(&GetSimulation()),
                  "SensorDetectionAttempt",
                  "WsfPlatform, WsfSensor, WsfPlatform, WsfSensorInteraction",
                  UtStd::Bind(&SensorDetectionAttemptPacker));
   AddEvent<true>("SENSOR_DETECTION_CHANGED",
                  WsfObserver::SensorDetectionChanged(&GetSimulation()),
                  "SensorDetectionChanged",
                  "WsfPlatform, WsfSensor, int, WsfSensorInteraction",
                  UtStd::Bind(&SensorDetectionChangedPacker));
   AddEvent("SENSOR_FREQUENCY_CHANGED",
            WsfObserver::SensorFrequencyChanged(&GetSimulation()),
            "SensorFrequencyChanged",
            "WsfPlatform, WsfSensor",
            UtStd::Bind(&SensorModePacker));
   AddEvent("SENSOR_MODE_ACTIVATED",
            WsfObserver::SensorModeActivated(&GetSimulation()),
            "SensorModeActivated",
            "WsfPlatform, WsfSensor",
            UtStd::Bind(&SensorModePacker));
   AddEvent("SENSOR_MODE_DEACTIVATED",
            WsfObserver::SensorModeDeactivated(&GetSimulation()),
            "SensorModeDeactivated",
            "WsfPlatform, WsfSensor",
            UtStd::Bind(&SensorModePacker));
   AddEvent("SENSOR_REQUEST_CANCELED",
            WsfObserver::SensorRequestCanceled(&GetSimulation()),
            "SensorRequestCanceled",
            "WsfPlatform, WsfSensor, WsfTrack",
            UtStd::Bind(&SensorRequestCanceledPacker));
   AddEvent("SENSOR_REQUEST_INITIATED",
            WsfObserver::SensorRequestInitiated(&GetSimulation()),
            "SensorRequestInitiated",
            "WsfPlatform, WsfSensor, WsfTrack",
            UtStd::Bind(&SensorRequestInitiatedPacker));
   AddEvent("SENSOR_REQUEST_UPDATED",
            WsfObserver::SensorRequestUpdated(&GetSimulation()),
            "SensorRequestUpdated",
            "WsfPlatform, WsfSensor, WsfTrack",
            UtStd::Bind(&SensorRequestUpdatedPacker));
   AddEvent<true>("SENSOR_TRACK_COASTED",
                  WsfObserver::SensorTrackCoasted(&GetSimulation()),
                  "SensorTrackCoasted",
                  "WsfPlatform, WsfSensor, WsfTrack",
                  UtStd::Bind(&SensorTrackPacker));
   AddEvent<true>("SENSOR_TRACK_DROPPED",
                  WsfObserver::SensorTrackDropped(&GetSimulation()),
                  "SensorTrackDropped",
                  "WsfPlatform, WsfSensor, WsfTrack",
                  UtStd::Bind(&SensorTrackPacker));
   AddEvent<true>("SENSOR_TRACK_INITIATED",
                  WsfObserver::SensorTrackInitiated(&GetSimulation()),
                  "SensorTrackInitiated",
                  "WsfPlatform, WsfSensor, WsfTrack",
                  UtStd::Bind(&SensorTrackPacker));
   AddEvent<true>("SENSOR_TRACK_UPDATED",
                  WsfObserver::SensorTrackUpdated(&GetSimulation()),
                  "SensorTrackUpdated",
                  "WsfPlatform, WsfSensor, WsfTrack",
                  UtStd::Bind(&SensorTrackPacker));
   AddEvent("SENSOR_TURNED_OFF",
            WsfObserver::SensorTurnedOff(&GetSimulation()),
            "SensorTurnedOff",
            "WsfPlatform, WsfSensor",
            UtStd::Bind(&PlatformPartPacker<WsfSensor>));
   AddEvent("SENSOR_TURNED_ON",
            WsfObserver::SensorTurnedOn(&GetSimulation()),
            "SensorTurnedOn",
            "WsfPlatform, WsfSensor",
            UtStd::Bind(&PlatformPartPacker<WsfSensor>));
   AddEvent("SIMULATION_COMPLETE", WsfObserver::SimulationComplete(&GetSimulation()), "SimulationComplete", "");
   AddEventP("SIMULATION_INITIALIZING", nullptr, "SimulationInitializing", "");
   AddEventP("SIMULATION_STARTING", nullptr, "SimulationStarting", "");
   AddEvent("STATE_ENTRY",
            WsfObserver::StateEntry(&GetSimulation()),
            "StateEntry",
            "WsfPlatform, WsfProcessor, WsfTrack, string",
            UtStd::Bind(&StatePacker));
   AddEvent("STATE_EXIT",
            WsfObserver::StateExit(&GetSimulation()),
            "StateExit",
            "WsfPlatform, WsfProcessor, WsfTrack, string",
            UtStd::Bind(&StatePacker));
   AddEvent("TANKING_EVENT",
            WsfObserver::TankingEvent(&GetSimulation()),
            "TankingEvent",
            "WsfPlatform, WsfPlatform, string",
            UtStd::Bind(&TankingEventPacker));
   AddEvent("TASK_ASSIGNED", WsfObserver::TaskAssigned(&GetSimulation()), "TaskAssigned", "WsfTask, WsfTrack");
   AddEvent("TASK_CANCELED", WsfObserver::TaskCanceled(&GetSimulation()), "TaskCanceled", "WsfTask");
   AddEvent("TASK_COMPLETED", WsfObserver::TaskCompleted(&GetSimulation()), "TaskCompleted", "WsfTask, string");
   AddEvent("TEAM_NAME_DEFINITION", WsfObserver::PlatformAdded(&GetSimulation()), "TeamNameDefinition", "WsfPlatform");
}

// ============================================================================
//! Enable or disable the callback, specified by event name, with the Observer.
//! @param aEnable      'true' indicates the callback should be enabled.
//! @param aEventName   The name of the callback to register.
//! @param aScriptName  The name of the script to be associated with the callback.
// private
bool WsfScriptObserver::EnableOrDisable(bool aEnable, const std::string& aEventName, const std::string& aScriptName)
{
   bool ok(false);

   int eventIndex = GetEventIndex(aEventName);
   if (eventIndex != -1)
   {
      EventData& eventData(mEvents[eventIndex]);
      ok = EnableOrDisable(aEnable, aEventName, aScriptName, eventData);
   }
   else
   {
      auto out = ut::log::error() << "Unknown callback name.";
      out.AddNote() << "Callback: " << aEventName;
      ok = false;
   }
   return ok;
}

// ============================================================================
bool WsfScriptObserver::EnableOrDisable(bool               aEnable,
                                        const std::string& aEventName,
                                        const std::string& aScriptName,
                                        EventData&         aEventData)
{
   bool ok(false);

   // If a script name was provided then use it, otherwise use the default name.
   std::string scriptName(aScriptName);
   if (aScriptName.empty())
   {
      scriptName = aEventData.mDefaultScriptName;
   }

   if (aEnable)
   {
      // Enable a script WsfObserver::
      // Locate the script to execute and make sure it has the required argument list.

      UtScript* scriptPtr = GetSimulation().GetScriptContext().FindScript(scriptName);
      if (scriptPtr == nullptr)
      {
         auto out = ut::log::error() << "Unable to find script for callback.";
         out.AddNote() << "Script: " << aScriptName;
         out.AddNote() << "Callback: " << aEventName;
      }
      else if (GetSimulation().GetScriptContext().ValidateScript(scriptPtr, "void", aEventData.mExpectedArgumentTypes))
      {
         // Don't add to the list if already in the list (but not an error if a duplicate)
         if (std::find(aEventData.mScriptList.begin(), aEventData.mScriptList.end(), scriptPtr) ==
             aEventData.mScriptList.end())
         {
            aEventData.mScriptList.push_back(scriptPtr);
         }

         // If we have a script, enable the callback.
         if (!aEventData.mScriptList.empty())
         {
            if (aEventData.mCallbackPtr != nullptr)
            {
               aEventData.mCallbackPtr->Block(false);
            }
            aEventData.mEnabled = true;
         }
         ok = true;
      }
   }
   else
   {
      // Disable a script WsfObserver::
      // Note that disabling a non-enabled observer is silently ignored.

      for (auto sli = aEventData.mScriptList.begin(); sli != aEventData.mScriptList.end(); ++sli)
      {
         UtScript* scriptPtr = *sli;
         if (scriptName == scriptPtr->GetName())
         {
            aEventData.mScriptList.erase(sli);
            break;
         }
      }

      // If there are no scripts, disable the callback.
      if (aEventData.mScriptList.empty())
      {
         if (aEventData.mCallbackPtr != nullptr)
         {
            aEventData.mCallbackPtr->Block(true);
         }
         aEventData.mEnabled = false;
      }
      ok = true;
   }
   return ok;
}

// ============================================================================
void WsfScriptObserver::ExecuteScriptList(double aSimTime, const std::vector<UtScript*>& aScriptList)
{
   ::ExecuteScriptList(aSimTime, GetSimulation().GetScriptContext(), aScriptList);
}

// ============================================================================
void WsfScriptObserver::ExecuteScriptList(double                        aSimTime,
                                          const std::vector<UtScript*>& aScriptList,
                                          UtScriptData&                 aScriptRetVal,
                                          UtScriptDataList&             aScriptArgs)
{
   ::ExecuteScriptList(aSimTime, GetSimulation().GetScriptContext(), aScriptList, aScriptRetVal, aScriptArgs);
}

// ============================================================================
void WsfScriptObserverExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfScriptObserver>(aSimulation, *this));
}

// ============================================================================
//! Process the script input for the scriptObserver.
//! @param aInput The input object to be parsed.
//! @return True if the command is handled by this ProcessInput.
bool WsfScriptObserverExtension::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if (command == "observer")
   {
      UtInputBlock observerBlock(aInput);
      while (observerBlock.ReadCommand(command))
      {
         auto readCallback = [&aInput]()
         {
            std::string callbackName; // The name of the callback as defined in WsfObserver.
            aInput.ReadValue(callbackName);
            if (callbackName == "PLATFORM_KILLED")
            {
               auto out = ut::log::warning() << "The PLATFORM_KILLED event has been deprecated and will be removed "
                                             << "in a future release. Use PLATFORM_BROKEN instead.";
               out.AddNote() << aInput.GetLocation();
            }
            std::string scriptName; // The name of the script to be invoked.
            aInput.ReadValue(scriptName);
            if ((scriptName == "end_observer") || (scriptName == "enable") || (scriptName == "disable"))
            {
               aInput.PushBack(scriptName);
               scriptName.clear();
            }
            return std::make_pair(callbackName, scriptName);
         };

         if (command == "enable")
         {
            auto callback = readCallback();
            mEnabledEvents[callback.first].insert(callback.second);
         }
         else if (command == "disable")
         {
            auto callback = readCallback();
            mEnabledEvents[callback.first].erase(callback.second);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}
