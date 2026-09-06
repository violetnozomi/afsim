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

#include "xio_sim/WsfXIO_Simulation.hpp"

#include <cassert>
#include <memory>

#include "UtEllipsoidalEarth.hpp"
#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptDataPack.hpp"
#include "UtScriptTypes.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfCSV_EventOutput.hpp"
#include "WsfClockSource.hpp"
#include "WsfComm.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEventOutput.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfProcessor.hpp"
#include "WsfRealTimeClockSource.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfTaskProcessor.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWaypointMover.hpp"
#include "dis/WsfDisInterface.hpp"
#include "ext/WsfExtInterface.hpp"
#include "script/WsfScriptManager.hpp"
#include "xio/WsfXIO_Connection.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio_sim/WsfXIO_DisExtension.hpp"
#include "xio_sim/WsfXIO_DrawService.hpp"
#include "xio_sim/WsfXIO_EventResults.hpp"
#include "xio_sim/WsfXIO_MessageService.hpp"
#include "xio_sim/WsfXIO_PlatformInfoService.hpp"
#include "xio_sim/WsfXIO_PlatformListService.hpp"
#include "xio_sim/WsfXIO_ScriptClass.hpp"
#include "xio_sim/WsfXIO_ScriptSerialize.hpp"
#include "xio_sim/WsfXIO_ScriptService.hpp"
#include "xio_sim/WsfXIO_SensorService.hpp"
#include "xio_sim/WsfXIO_SimPacketRegistry.hpp"
#include "xio_sim/WsfXIO_SimTimeService.hpp"
#include "xio_sim/WsfXIO_TaskService.hpp"
#include "xio_sim/WsfXIO_TrackService.hpp"

namespace
{
// An instance of this is registered with the simulation to get the time from the simulation.
class ClockSource : public WsfXIO_Interface::ClockSource
{
public:
   ClockSource(const WsfSimulation& aSimulation)
      : mSimulation(aSimulation)
   {
   }
   double               GetSimTime() const override { return mSimulation.GetSimTime(); }
   const WsfSimulation& mSimulation;
};

constexpr double cTHRESHOLD_SEND_TIME_BEHIND    = 0.5;
constexpr double cTHRESHOLD_CATCHUP_TIME_BEHIND = 0.0;

using EventOutput = wsf::event::output::SimulationExtension;
//! Helper function for registering xio events with event output
//! See WsfEventOutputBase.hpp for more details
template<typename RESULT, typename... Args>
void AddEvent(EventOutput& aEventOutput, const std::string& aEventName, UtCallbackListN<void(Args...)>& aCallbackList)
{
   auto fn = [&aEventOutput](Args&&... args)
   {
      double                  simTime = aEventOutput.GetSimulation().GetSimTime();
      EventOutput::EventGuard guard(aEventOutput);
      RESULT                  result(simTime, std::forward<Args>(args)..., aEventOutput.GetSettings());
      aEventOutput.PrintEvent(result);
   };
   aEventOutput.AddEvent(aEventName, aCallbackList.Connect(fn));
}

void Register_event_output(const WsfXIO_Simulation& aXIO_Simulation, EventOutput& aEventOutput)
{
   auto interfacePtr = aXIO_Simulation.GetInterface();

   using namespace wsf::event;
   AddEvent<HandleConnect>(aEventOutput, "XIO_CONNECT", interfacePtr->OnConnected);
   AddEvent<HandleDisconnect>(aEventOutput, "XIO_DISCONNECT", interfacePtr->OnDisconnected);
   if (!aXIO_Simulation.GetSimulation().GetApplication().IsTestingEnabled())
   {
      // The non-determinism of the XIO_BANDWIDTH event makes it unsuitable
      // for usage in automated testing environments.
      AddEvent<HandleBandwidthData>(aEventOutput, "XIO_BANDWIDTH", interfacePtr->BandwidthDataEvent);
   }
}
} // namespace

// =================================================================================================
WsfXIO_Simulation::WsfXIO_Simulation(WsfXIO_Interface* aInterfacePtr, WsfSimulation& aSimulation)
   : mInterfacePtr(aInterfacePtr)
   , mSimulation(aSimulation)
   , mDisInterfacePtr(nullptr)
{
   SetParentOfComponents(this); // Components will have me as their parent.

   WsfXIO_SimPacketRegistry::RegisterPackets(*mInterfacePtr);

   // Tell the interface to get the simulation time from me.
   aInterfacePtr->SetClockSource(new ClockSource(aSimulation));

   mDisInterfacePtr = WsfDisInterface::Find(aSimulation);
   assert(mDisInterfacePtr != nullptr);

   mCallbacks += mInterfacePtr->Connect(&WsfXIO_Simulation::HandleInformationRequest, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_Simulation::HandleDeletePlatform, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_Simulation::HandleStateChangeRequest, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_Simulation::HandleReRoutePkt, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_Simulation::HandleTrackManagerStateChangeRequest, this);
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_Simulation::HandleSimTimeCommand, this);

   // Create the service providers
   mTrackServicePtr            = new WsfXIO_TrackService(*this);
   mPlatformListServicePtr     = new WsfXIO_PlatformListService(*this);
   mPlatformInfoServicePtr     = new WsfXIO_PlatformInfoService(*this);
   mPlatformPartInfoServicePtr = new WsfXIO_PlatformPartInfoService(*this);
   mTaskServicePtr             = new WsfXIO_TaskService(*this);
   mMessageServicePtr          = new WsfXIO_MessageService(*this);
   mSimTimeServicePtr          = new WsfXIO_SimTimeService(*this);
   mScriptServicePtr           = new WsfXIO_ScriptService(*this);
   mDrawServicePtr             = new WsfXIO_DrawService(*this);
   mSensorServicePtr           = new WsfXIO_SensorService(*this);

   // If the 'event_output' extension exists then add our event processor.
   WsfEventOutput* eventOutputPtr = WsfEventOutput::Find(aSimulation);
   if (eventOutputPtr != nullptr)
   {
      Register_event_output(*this, *eventOutputPtr);
   }

   // If the 'csv_event_output' extension exists then add our csv event processor.
   WsfCSV_EventOutput* csvEventOutputPtr = WsfCSV_EventOutput::Find(aSimulation);
   if (csvEventOutputPtr != nullptr)
   {
      WsfCSV_EventOutputData::AddDataTags("XIO_CONNECT",
                                          {"time<time>",
                                           "event<string>",
                                           "reliability<string>",
                                           "app_id<string>",
                                           "app_name<string>",
                                           "app_type<int>",
                                           "address<string>",
                                           "port<int>"});
      WsfCSV_EventOutputData::AddDataTags(
         "XIO_DISCONNECT",
         {"time<time>", "event<string>", "reliability<string>", "app_id<string>", "app_name<string>", "app_type<int>"});
      WsfCSV_EventOutputData::AddDataTags("XIO_BANDWIDTH",
                                          {"time<time>",
                                           "event<string>",
                                           "total_bytes_sent<int>",
                                           "total_bytes_received<int>",
                                           "delta_bytes_sent<int>",
                                           "delta_bytes_received<int>",
                                           "send_rate<int>",
                                           "receive_rate<int>"});
      Register_event_output(*this, *csvEventOutputPtr);
   }

   WsfXIO_ScriptSerialize::Initialize();
}

// =================================================================================================
WsfXIO_Simulation::~WsfXIO_Simulation()
{
   delete mSensorServicePtr;
   delete mDrawServicePtr;
   delete mScriptServicePtr;
   delete mSimTimeServicePtr;
   delete mMessageServicePtr;
   delete mTaskServicePtr;
   delete mPlatformPartInfoServicePtr;
   delete mPlatformInfoServicePtr;
   delete mPlatformListServicePtr;
   delete mTrackServicePtr;
}

// =================================================================================================
bool WsfXIO_Simulation::Initialize()
{
   // Cause the WsfXIO_Interface to advance in time whenever the WSF simulation advances in time.
   mCallbacks += WsfObserver::AdvanceTime(&GetSimulation()).Connect(&WsfXIO_Interface::AdvanceTime, mInterfacePtr);
   mCallbacks +=
      WsfObserver::SimulationTimeBehind(&GetSimulation()).Connect(&WsfXIO_Simulation::SimulationTimeBehind, this);

   // TODO_XIO need to comment this
   if (mInterfacePtr->mPauseForConnection)
   {
      GetSimulation().SetIsExternallyStarted(true);
   }
   return true;
}

// =================================================================================================
//                                         PACKET HANDLERS
// =================================================================================================

void WsfXIO_Simulation::HandleInformationRequest(WsfXIO_InformationRequestPkt& aPkt)
{
   if (aPkt.mType == WsfXIO_InformationRequestPkt::cPLATFORM_LIST)
   {
      WsfXIO_PlatformListPkt pkt;
      for (unsigned int i = 0; i < GetSimulation().GetPlatformCount(); ++i)
      {
         WsfPlatform* platformPtr = GetSimulation().GetPlatformEntry(i);
         if (!platformPtr->IsExternallyControlled())
         {
            WsfXIO_PlatformListPkt::PlatformInfo info;
            info.mName  = platformPtr->GetName();
            info.mIndex = static_cast<int>(platformPtr->GetIndex());
            if (mDisInterfacePtr != nullptr)
            {
               DisEntityId entityId;
               mDisInterfacePtr->GetEntityIdFromIndex(info.mIndex, entityId);
               info.mEntityId = entityId;
            }
            pkt.mPlatformData.push_back(info);
         }
      }
      mInterfacePtr->GetSender(aPkt)->Send(pkt);
   }
   else if (aPkt.mType == WsfXIO_InformationRequestPkt::cPLATFORM_TYPE_LIST)
   {
      WsfXIO_PlatformTypeListPkt        pkt;
      auto&                             platTypes = WsfPlatformTypes::Get(GetSimulation().GetScenario());
      WsfObjectTypeListBase::TypeIdList types;
      platTypes.GetTypeIds(types);
      pkt.mPlatformTypes.resize(types.size());
      for (size_t i = 0; i < pkt.mPlatformTypes.size(); ++i)
      {
         pkt.mPlatformTypes[i].mTypeName = types[i];
         std::string  typeName           = types[i];
         WsfPlatform* typePtr            = platTypes.Find(typeName);
         if (typePtr != nullptr)
         {
            pkt.mPlatformTypes[i].mIcon = typePtr->GetIcon();
         }
      }
      mInterfacePtr->GetSender(aPkt)->Send(pkt);
   }
}

void WsfXIO_Simulation::HandleDeletePlatform(WsfXIO_DeletePlatformPkt& aPkt)
{
   for (int index : aPkt.mPlatformIndices)
   {
      WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(index);
      if (platformPtr != nullptr)
      {
         GetSimulation().DeletePlatform(GetSimulation().GetSimTime(), platformPtr);
      }
   }
}

void WsfXIO_Simulation::HandleStateChangeRequest(WsfXIO_PartStateChangeRequestPkt& aPkt)
{
   double       simTime     = GetSimulation().GetSimTime();
   WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(aPkt.mPlatformIndex);
   if (platformPtr != nullptr)
   {
      int         commandType    = aPkt.mStateChange & WsfXIO_PartStateChangeRequestPkt::cCOMMAND_MASK;
      int         subCommandType = aPkt.mStateChange & ~WsfXIO_PartStateChangeRequestPkt::cCOMMAND_MASK;
      WsfEM_Rcvr* rcvrPtr        = nullptr;
      WsfEM_Xmtr* xmtrPtr        = nullptr;
      if (aPkt.mPartType == cCOMPONENT_ROLE<WsfSensor>())
      {
         WsfSensor* sensorPtr = platformPtr->GetComponent<WsfSensor>(aPkt.mPartName);
         if (sensorPtr != nullptr)
         {
            if (aPkt.mSelectMode != 0)
            {
               if (subCommandType == WsfXIO_PartStateChangeRequestPkt::cDESELECT_MODE)
               {
                  sensorPtr->DeselectMode(simTime, aPkt.mSelectMode);
               }
               else
               {
                  sensorPtr->SelectMode(simTime, aPkt.mSelectMode);
               }
            }
            if (commandType == WsfXIO_PartStateChangeRequestPkt::cTRACKING_REQUEST_COMMAND)
            {
               if (aPkt.mStateChange == WsfXIO_PartStateChangeRequestPkt::cSTART_TRACKING)
               {
                  WsfTrack* trackPtr = nullptr;
                  if (aPkt.mTrackHostIndex == 0)
                  {
                     WsfPlatform* owningPlatformPtr =
                        GetSimulation().GetPlatformByName(aPkt.mTrackId.GetOwningPlatformId());
                     if (owningPlatformPtr != nullptr)
                     {
                        trackPtr = owningPlatformPtr->GetMasterTrackList().FindTrack(aPkt.mTrackId);
                     }
                  }
                  else
                  {
                     WsfPlatform* owningPlatformPtr = GetSimulation().GetPlatformByIndex(aPkt.mTrackHostIndex);
                     if (owningPlatformPtr != nullptr)
                     {
                        trackPtr = owningPlatformPtr->GetMasterRawTrackList().FindTrack(aPkt.mTrackId);
                     }
                  }
                  if (trackPtr != nullptr)
                  {
                     sensorPtr->StartTracking(simTime, *trackPtr, aPkt.mSelectMode);
                  }
               }
               else if (aPkt.mStateChange == WsfXIO_PartStateChangeRequestPkt::cSTOP_TRACKING)
               {
                  sensorPtr->StopTracking(simTime, aPkt.mTrackId);
               }
            }
            if (commandType == WsfXIO_PartStateChangeRequestPkt::cXMTR_RCVR_COMMAND)
            {
               if (aPkt.mXmtrIndex < (int)sensorPtr->GetEM_XmtrCount())
               {
                  xmtrPtr = &sensorPtr->GetEM_Xmtr(aPkt.mXmtrIndex);
                  if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cCHANGE_RCVR)
                  {
                     rcvrPtr = xmtrPtr->GetLinkedReceiver();
                  }
               }
            }
         }
      }
      else
      {
         // Not handled explicitly above, so try the components.
         const ComponentList& componentList = *this;
         for (ComponentList::Iterator it = componentList.Begin(); it != componentList.End(); ++it)
         {
            if ((*it)->HandleStateChangeRequest(simTime, aPkt, platformPtr, xmtrPtr, rcvrPtr))
            {
               break;
            }
         }
      }
      if (rcvrPtr != nullptr)
      {
         if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cSET_BANDWIDTH)
         {
            rcvrPtr->SetBandwidth(aPkt.mBandwidth);
         }
         else if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cSET_FREQUENCY)
         {
            rcvrPtr->SetFrequency(aPkt.mFrequency);
         }
         else if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cSET_DETECTION_THRESHOLD)
         {
            rcvrPtr->SetDetectionThreshold(aPkt.mDetectionThreshold);
         }
         else if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cSET_NOISE_FIGURE)
         {
            rcvrPtr->SetNoiseFigure(aPkt.mNoiseFigure);
         }
      }
      else if (xmtrPtr != nullptr)
      {
         if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cSET_POWER)
         {
            xmtrPtr->SetPower(aPkt.mPower);
         }
         else if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cSET_BANDWIDTH)
         {
            xmtrPtr->SetBandwidth(aPkt.mBandwidth);
         }
         else if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cSET_FREQUENCY)
         {
            xmtrPtr->SetFrequency(aPkt.mFrequency);
         }
      }
      if (aPkt.mStateChange != WsfXIO_PartStateChangeRequestPkt::cNO_CHANGE)
      {
         WsfComponent*    componentPtr = platformPtr->FindComponent(aPkt.mPartName, aPkt.mPartType);
         WsfPlatformPart* partPtr(nullptr);
         componentPtr->QueryInterfaceT(partPtr);
         if (commandType == WsfXIO_PartStateChangeRequestPkt::cSTATE_COMMAND)
         {
            bool turnOn = 0 != (subCommandType & WsfXIO_PartStateChangeRequestPkt::cTURN_ON);
            if ((partPtr != nullptr) && (partPtr->IsTurnedOn() != turnOn))
            {
               if (turnOn)
               {
                  GetSimulation().TurnPartOn(simTime, partPtr);
               }
               else
               {
                  GetSimulation().TurnPartOff(simTime, partPtr);
               }
            }
         }
         if (commandType == WsfXIO_PartStateChangeRequestPkt::cDEBUG_COMMAND)
         {
            bool debug = (0 != (subCommandType & WsfXIO_PartStateChangeRequestPkt::cTURN_DEBUG_ON));
            if ((partPtr != nullptr) && (debug != partPtr->DebugEnabled()))
            {
               partPtr->SetDebugEnabled(debug);
               mPlatformInfoServicePtr->PartDebugChange(partPtr);
            }
         }
         if (commandType == WsfXIO_PartStateChangeRequestPkt::cCUE_COMMAND)
         {
            WsfArticulatedPart* articulatedPartPtr = dynamic_cast<WsfArticulatedPart*>(partPtr);
            if (articulatedPartPtr != nullptr)
            {
               if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cCUE_AZ_EL)
               {
                  double az(0), el(0);
                  articulatedPartPtr->GetCuedOrientation(az, el);
                  if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cCUE_AZ)
                  {
                     az = UtMath::NormalizeAngleMinusPi_Pi(aPkt.mCueAz);
                  }
                  if (subCommandType & WsfXIO_PartStateChangeRequestPkt::cCUE_EL)
                  {
                     el = UtMath::NormalizeAngleMinusPi_Pi(aPkt.mCueEl);
                  }
                  articulatedPartPtr->SetCuedOrientation(az, el);
               }
               if (aPkt.mStateChange == WsfXIO_PartStateChangeRequestPkt::cCUE_WCS)
               {
                  articulatedPartPtr->SetCuedLocationWCS(aPkt.mCueWCS.GetData());
               }
            }
         }
      }
   }
}

void WsfXIO_Simulation::HandleTrackManagerStateChangeRequest(WsfXIO_TrackManagerStateChangeRequestPkt& aPkt)
{
   WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(aPkt.mPlatformIndex);
   if (platformPtr != nullptr)
   {
      // only update the correlation and / or fusion strategy if new values were provided.
      // The track manager will handle how it reacts to the strategy or parses
      // bad values.

      WsfTrackManager& mgrRef = platformPtr->GetTrackManager();
      if (aPkt.IsCorrelationStrategyChanged())
      {
         mgrRef.SetCorrelationStrategy(aPkt.mCorrelationStrategy);
      }

      if (aPkt.IsFusionStrategyChanged())
      {
         mgrRef.SetFusionStrategy(aPkt.mFusionStrategy);
      }
   }
}

void WsfXIO_Simulation::HandleReRoutePkt(WsfXIO_ReRoutePlatformPkt& aPkt)
{
   double       simTime     = GetSimulation().GetSimTime();
   WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(aPkt.mPlatformIndex);
   if (platformPtr != nullptr)
   {
      WsfMover* moverPtr = platformPtr->GetMover();
      if (moverPtr != nullptr)
      {
         if (aPkt.mType == WsfXIO_ReRoutePlatformPkt::cGO_TO_SPEED)
         {
            moverPtr->GoToSpeed(simTime, aPkt.mSpeed, aPkt.mRateOfChange, aPkt.mKeepRoute);
         }
         else if (aPkt.mType == WsfXIO_ReRoutePlatformPkt::cGO_TO_HEADING)
         {
            double radialAccel = 1.0e6;
            if (aPkt.mRateOfChange != 0.0)
            {
               radialAccel = aPkt.mRateOfChange;
            }
            else
            {
               WsfWaypointMover* wpMoverPtr = dynamic_cast<WsfWaypointMover*>(moverPtr);
               if (wpMoverPtr != nullptr)
               {
                  radialAccel = wpMoverPtr->GetMoverConstraints()->mMaxRadialAccel;
               }
            }

            if (aPkt.mRelativeHeading)
            {
               moverPtr->TurnToRelativeHeading(simTime, aPkt.mHeading, radialAccel, WsfPath::cTURN_DIR_SHORTEST);
            }
            else
            {
               moverPtr->TurnToHeading(simTime, aPkt.mHeading, radialAccel, WsfPath::cTURN_DIR_SHORTEST);
            }
         }
         else if (aPkt.mType == WsfXIO_ReRoutePlatformPkt::cGO_TO_ALTITUDE)
         {
            moverPtr->GoToAltitude(simTime, aPkt.mAltitude, aPkt.mRateOfChange, aPkt.mKeepRoute);
         }
         else if (aPkt.mType == WsfXIO_ReRoutePlatformPkt::cFOLLOW_ROUTE && aPkt.mRoutePtr != nullptr)
         {
            if (aPkt.mRoutePtr->GetSize() > 0)
            {
               moverPtr->UpdateRoute(simTime, *aPkt.mRoutePtr);
            }
         }
         else if (aPkt.mType == WsfXIO_ReRoutePlatformPkt::cUPDATE_DEFAULT_ROUTE)
         {
            moverPtr->UpdateDefaultRoute();
         }
         else if (aPkt.mType == WsfXIO_ReRoutePlatformPkt::cRETURN_TO_ROUTE)
         {
            moverPtr->ReturnToRoute(simTime);
         }
         else if (aPkt.mType == WsfXIO_ReRoutePlatformPkt::cGO_TO_LOCATION)
         {
            double lat, lon, alt;
            UtEllipsoidalEarth::ConvertECEFToLLA(aPkt.mLocationWCS, lat, lon, alt);
            moverPtr->GoToLocation(simTime, lat, lon, alt);
         }
      }
      if (aPkt.mType == WsfXIO_ReRoutePlatformPkt::cSET_LOCATION)
      {
         // Set the location while preserving the NED orientation.
         double heading, pitch, roll;
         platformPtr->GetOrientationNED(heading, pitch, roll);
         platformPtr->SetLocationWCS(aPkt.mLocationWCS);
         platformPtr->SetOrientationNED(heading, pitch, roll);

         // If the platform has a mover, it may need to be told that the position has changed.
         // Note: This does not support all movers
         // Currently, this supports WsfRouteMover and WsfP6DOF_Mover
         if (moverPtr != nullptr)
         {
            moverPtr->ResetPosition(simTime);
         }
      }
   }
}

void WsfXIO_Simulation::HandleSimTimeCommand(WsfXIO_SimTimeCommandPkt& aPkt)
{
   if (GetSimulation().IsFlexibleRealtime() &&
       (aPkt.mCommandType == WsfXIO_SimTimeCommandPkt::cPAUSE || aPkt.mCommandType == WsfXIO_SimTimeCommandPkt::cRESUME))
   {
      // SetMaximumClock() is called elsewhere to advance time quickly.
      // If this is the case, pausing the sim can confuse the advance time logic.
      // Therefore, set the clock to the current sim time prior to pausing
      WsfClockSource* clockSourcePtr = (WsfClockSource*)GetSimulation().GetClockSource();
      if (clockSourcePtr != nullptr)
      {
         double simTime = GetSimulation().GetSimTime();
         clockSourcePtr->SetClock(simTime);
      }
   }
   switch (aPkt.mCommandType)
   {
   case WsfXIO_SimTimeCommandPkt::cSET_MAX_SIM_TIME:
   {
      WsfClockSource* clockSourcePtr = (WsfClockSource*)GetSimulation().GetClockSource();
      if (clockSourcePtr != nullptr)
      {
         clockSourcePtr->SetMaximumClock(aPkt.mSimTime);
         GetSimulation().SetFlexibleRealtime();
      }
      break;
   }
   case WsfXIO_SimTimeCommandPkt::cREQUEST_TIME_MANAGEMENT: // not yet implemented
      break;
   case WsfXIO_SimTimeCommandPkt::cSET_CLOCK_RATE:
      GetSimulation().SetClockRate(aPkt.mSimTime);
      break;
   case WsfXIO_SimTimeCommandPkt::cPAUSE:
      GetSimulation().Pause();
      break;
   case WsfXIO_SimTimeCommandPkt::cRESUME:
      GetSimulation().Resume();
      break;
   case WsfXIO_SimTimeCommandPkt::cADVANCE_TIME:
   {
      // Unsure how you can advance time in a constructive setting, this is geared toward a real-time clock source.
      // Simply change the clock and let the sim catch up.
      WsfClockSource*         clockSourcePtr   = const_cast<WsfClockSource*>(GetSimulation().GetClockSource());
      WsfRealTimeClockSource* rtClockSourcePtr = dynamic_cast<WsfRealTimeClockSource*>(clockSourcePtr);
      if (rtClockSourcePtr != nullptr)
      {
         rtClockSourcePtr->SetClock(rtClockSourcePtr->GetClock(1.0E20) + aPkt.mSimTime);
      }
   }
   break;
   case WsfXIO_SimTimeCommandPkt::cSET_END_TIME:
      GetSimulation().SetEndTime(aPkt.mSimTime);
      break;
   case WsfXIO_SimTimeCommandPkt::cSET_NON_REALTIME:
      GetSimulation().SetRealtime(GetSimulation().GetSimTime(), false);
      break;
   case WsfXIO_SimTimeCommandPkt::cSET_REALTIME:
      GetSimulation().SetRealtime(GetSimulation().GetSimTime(), true);
      break;
   }
}

void WsfXIO_Simulation::SendTimeBehind(bool aIsBehind)
{
   WsfXIO_TimeBehindPkt pkt;
   pkt.mIsBehind      = aIsBehind;
   pkt.mApplicationId = mInterfacePtr->GetApplicationId();
   // used TCP instead of UDP because WsfConnection's GetApplicationId() function has comment saying only valid for TCP connections
   mInterfacePtr->SendToAllTCP(pkt);
}

void WsfXIO_Simulation::SimulationTimeBehind(double aTimeBehind)
{
   // If we are more than a specified amount behind, send message out
   if (!mBehindRealTime && aTimeBehind > mInterfacePtr->mFallingBehindThreshold.value_or(cTHRESHOLD_SEND_TIME_BEHIND))
   {
      SendTimeBehind(true);
      mBehindRealTime = true;
   }
   else if (mBehindRealTime && aTimeBehind <= cTHRESHOLD_CATCHUP_TIME_BEHIND)
   {
      SendTimeBehind(false);
      mBehindRealTime = false;
   }
}

//////////////////////////////////////////////////////////////////////////
// WsfXIO_ScriptPublishKeyClass
//////////////////////////////////////////////////////////////////////////


class WsfXIO_ScriptPublishKeyClass : public UtScriptClass
{
public:
   WsfXIO_ScriptPublishKeyClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~WsfXIO_ScriptPublishKeyClass() override;

   // See the base class for a description of Create, Clone, and Destroy.

   void*       Clone(void* aObjectPtr) override;
   void        Destroy(void* aObjectPtr) override;
   std::string ToString(void* aObjectPtr) const override;

   UT_DECLARE_SCRIPT_METHOD(Field);
   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);
   UT_DECLARE_SCRIPT_METHOD(Construct_3);
   UT_DECLARE_SCRIPT_METHOD(Construct_4);
   UT_DECLARE_SCRIPT_METHOD(Match);
};

WsfXIO_ScriptPublishKeyClass::WsfXIO_ScriptPublishKeyClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfXIO_PublishKey");
   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<Field>());
   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_3>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_4>("Construct"));
   AddMethod(ut::make_unique<Match>());
}
WsfXIO_ScriptPublishKeyClass::~WsfXIO_ScriptPublishKeyClass() {}

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfXIO_PublishKey::CreateScriptClass(const std::string& aClassName,
                                                                    UtScriptTypes*     aScriptTypesPtr)
{
   auto scriptClass = ut::make_unique<WsfXIO_ScriptPublishKeyClass>(aClassName, aScriptTypesPtr);
   sScriptClassPtr  = scriptClass.get();
   return std::move(scriptClass);
}

namespace
{
WsfXIO_PublishKey* ConstructKey(UtScriptMethodArgs aVarArgs)
{
   WsfXIO_PublishKey::Field args[4];
   for (size_t i = 0; i < aVarArgs.size(); ++i)
   {
      const UtScriptData& arg = aVarArgs[i];
      switch (arg.GetType())
      {
      case ut::script::Data::Type::cBOOL:
         args[i] = (int)arg.GetBool();
         break;
      case ut::script::Data::Type::cINT:
         args[i] = arg.GetInt();
         break;
      case ut::script::Data::Type::cDOUBLE:
         args[i] = (int)arg.GetDouble();
         break;
      case ut::script::Data::Type::cSTRING:
         args[i] = arg.GetString();
         break;
      case ut::script::Data::Type::cPOINTER:
      {
         void* appObjectPtr = arg.GetPointer()->GetAppObject();
         args[i]            = WsfXIO_PublishKey::Field((size_t)appObjectPtr);
         break;
      }
      default:
         break;
      }
   }
   return new WsfXIO_PublishKey(args[0], args[1], args[2], args[3]);
}
} // namespace

// virtual
void* WsfXIO_ScriptPublishKeyClass::Clone(void* aObjectPtr)
{
   return new WsfXIO_PublishKey(*(WsfXIO_PublishKey*)aObjectPtr);
}

// virtual
void WsfXIO_ScriptPublishKeyClass::Destroy(void* aObjectPtr)
{
   delete ((WsfXIO_PublishKey*)aObjectPtr);
}

// virtual
std::string WsfXIO_ScriptPublishKeyClass::ToString(void* aObjectPtr) const
{
   auto ptr = static_cast<WsfXIO_PublishKey*>(aObjectPtr);
   return ptr->ToString();
}

UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptPublishKeyClass, WsfXIO_PublishKey, Field, 1, "string", "int")
{
   aReturnVal.SetString((*aObjectPtr)[aVarArgs[0].GetInt()].GetString());
}
UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptPublishKeyClass, WsfXIO_PublishKey, Construct_1, 1, "WsfXIO_PublishKey", "Object")
{
   aReturnVal.SetPointer(new UtScriptRef(ConstructKey(aVarArgs), aReturnClassPtr, UtScriptRef::cMANAGE));
}
UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptPublishKeyClass, WsfXIO_PublishKey, Construct_2, 2, "WsfXIO_PublishKey", "Object,Object")
{
   aReturnVal.SetPointer(new UtScriptRef(ConstructKey(aVarArgs), aReturnClassPtr, UtScriptRef::cMANAGE));
}
UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptPublishKeyClass, WsfXIO_PublishKey, Construct_3, 3, "WsfXIO_PublishKey", "Object,Object,Object")
{
   aReturnVal.SetPointer(new UtScriptRef(ConstructKey(aVarArgs), aReturnClassPtr, UtScriptRef::cMANAGE));
}
UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptPublishKeyClass, WsfXIO_PublishKey, Construct_4, 4, "WsfXIO_PublishKey", "Object,Object,Object,Object")
{
   aReturnVal.SetPointer(new UtScriptRef(ConstructKey(aVarArgs), aReturnClassPtr, UtScriptRef::cMANAGE));
}
UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptPublishKeyClass, WsfXIO_PublishKey, Match, 1, "bool", "WsfXIO_PublishKey")
{
   WsfXIO_PublishKey* keyPtr = (WsfXIO_PublishKey*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->Match(*keyPtr));
}

//////////////////////////////////////////////////////////////////////////
// WsfXIO_ScriptConnectionClass
//////////////////////////////////////////////////////////////////////////


class WsfXIO_ScriptConnectionClass : public UtScriptClass
{
public:
   WsfXIO_ScriptConnectionClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   bool AddStaticMethodP(std::unique_ptr<InterfaceMethod> aInterfaceMethod)
   {
      return UtScriptClass::AddStaticMethod(std::move(aInterfaceMethod));
   }

   UT_DECLARE_SCRIPT_METHOD(Execute);
   UT_DECLARE_SCRIPT_METHOD(Name);
   UT_DECLARE_SCRIPT_METHOD(Index);

protected:
};

std::unique_ptr<UtScriptClass> WsfXIO_Connection::CreateScriptClass(UtScriptTypes* aScriptTypesPtr)
{
   auto scriptClassPtr = ut::make_unique<WsfXIO_ScriptConnectionClass>("WsfXIO_Connection", aScriptTypesPtr);
   mScriptClassPtr     = scriptClassPtr.get();
   return std::move(scriptClassPtr);
}

WsfXIO_ScriptConnectionClass::WsfXIO_ScriptConnectionClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   WsfXIO_Connection::mScriptClassPtr = this;

   SetClassName("WsfXIO_Connection");
   AddMethod(ut::make_unique<Execute>());
   AddMethod(ut::make_unique<Name>());
   AddMethod(ut::make_unique<Index>());
}

// Execute a script function on another application
UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptConnectionClass, WsfXIO_Connection, Execute, 2, "void", "string, Array<Object>")
{
   WsfXIO_ScriptExecutePkt pkt;
   pkt.mIsApplicationMethod = false;
   pkt.mPlatformIndex       = 0;
   std::string                scriptName;
   std::vector<UtScriptData>* argListPtr = nullptr;
   UtScriptDataUnpack(aVarArgs, scriptName, argListPtr);
   pkt.mScriptName = scriptName;
   if (argListPtr != nullptr)
   {
      pkt.mArgumentList = *argListPtr;
   }
   pkt.mBaseObject = nullptr;
   aObjectPtr->Send(pkt);
}

UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptConnectionClass, WsfXIO_Connection, Name, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetApplicationName());
}

UT_DEFINE_SCRIPT_METHOD(WsfXIO_ScriptConnectionClass, WsfXIO_Connection, Index, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetConnectionId());
}
