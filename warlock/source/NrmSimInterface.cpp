#include "NrmSimInterface.hpp"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <map>
#include <set>
#include <string>

#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommGraph.hpp"
#include "WsfCommNetwork.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommResult.hpp"
#include "WsfEnvironment.hpp"
#include "WsfMessage.hpp"
#include "WsfNavigationErrors.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "UtMath.hpp"
#include "nrm/NetworkProfileRepository.hpp"
#include "nrm/NetworkTypeUtils.hpp"

namespace
{
std::string EndpointId(const wsf::comm::Comm* aCommPtr)
{
   return aCommPtr == nullptr ? std::string() : aCommPtr->GetAddress().GetAddress();
}

nrm::ResourceState CommState(const wsf::comm::Comm* aCommPtr)
{
   if (aCommPtr == nullptr)
   {
      return nrm::ResourceState::cUNKNOWN;
   }
   if (aCommPtr->IsBroken())
   {
      return nrm::ResourceState::cFAILED;
   }
   if (!aCommPtr->IsTurnedOn())
   {
      return nrm::ResourceState::cDISABLED;
   }
   if (!aCommPtr->IsOperational())
   {
      return nrm::ResourceState::cOFFLINE;
   }
   return nrm::ResourceState::cONLINE;
}

void SetPositionMetric(nrm::MetricValue<double>& aMetric,
                       double                    aValue,
                       const char*               aUnit,
                       double                    aSimTime)
{
   aMetric.value      = aValue;
   aMetric.unit       = aUnit;
   aMetric.valid      = true;
   aMetric.origin     = nrm::DataOrigin::cAFSIM_INTERNAL;
   aMetric.confidence = nrm::Confidence::cHIGH;
   aMetric.sampleTime = aSimTime;
   aMetric.reason     = nrm::MetricReason::cNONE;
}

void SetDirectMetric(nrm::MetricValue<double>& aMetric,
                     double                    aValue,
                     const char*               aUnit,
                     double                    aSimTime)
{
   aMetric.value      = aValue;
   aMetric.unit       = aUnit;
   aMetric.valid      = true;
   aMetric.origin     = nrm::DataOrigin::cAFSIM_INTERNAL;
   aMetric.confidence = nrm::Confidence::cHIGH;
   aMetric.sampleTime = aSimTime;
   aMetric.reason     = nrm::MetricReason::cNONE;
}

std::string LinkId(const wsf::comm::Comm* aSourcePtr, const wsf::comm::Comm* aDestinationPtr)
{
   return EndpointId(aSourcePtr) + "->" + EndpointId(aDestinationPtr);
}

const double cWINDOWS_S[] = {1.0, 10.0, 60.0};

nrm::NavigationMode NavigationModeFromStatus(WsfNavigationErrors::GPS_Status aStatus)
{
   switch (aStatus)
   {
   case WsfNavigationErrors::cGPS_PERFECT: return nrm::NavigationMode::cPERFECT;
   case WsfNavigationErrors::cGPS_ACTIVE: return nrm::NavigationMode::cGPS_ACTIVE;
   case WsfNavigationErrors::cGPS_DEGRADED: return nrm::NavigationMode::cGPS_DEGRADED;
   case WsfNavigationErrors::cGPS_EXTERNAL: return nrm::NavigationMode::cGPS_EXTERNAL;
   case WsfNavigationErrors::cGPS_INACTIVE: return nrm::NavigationMode::cINS;
   }
   return nrm::NavigationMode::cUNKNOWN;
}

std::string RawNavigationStatus(WsfNavigationErrors::GPS_Status aStatus)
{
   switch (aStatus)
   {
   case WsfNavigationErrors::cGPS_PERFECT: return "PERFECT";
   case WsfNavigationErrors::cGPS_ACTIVE: return "GPS1";
   case WsfNavigationErrors::cGPS_DEGRADED: return "GPS2";
   case WsfNavigationErrors::cGPS_EXTERNAL: return "GPS3";
   case WsfNavigationErrors::cGPS_INACTIVE: return "INS1";
   }
   return "UNKNOWN";
}
} // namespace

WkNrm::SimInterface::SimInterface(const QString& aPluginName)
   : warlock::SimInterfaceT<SimEvent>(aPluginName)
{
}

void WkNrm::SimInterface::SimulationInitializing(const WsfSimulation& aSimulation)
{
   mCallbacks.Clear();
   mSnapshot = nrm::ResourceSnapshot();
   nrm::NetworkProfileRepository profiles =
      nrm::NetworkProfileRepository::BuiltInDemo();
   const char* profilePath = std::getenv("NRM_NETWORK_PROFILE_CONFIG");
   if (profilePath != nullptr && profilePath[0] != '\0')
   {
      profiles = nrm::NetworkProfileRepository();
      nrm::NetworkProfileValidation validation;
      profiles.LoadFromFile(profilePath, validation);
   }
   mSnapshot.configVersion = profiles.ConfigVersion();
   for (const nrm::NetworkProfile& profile : profiles.Profiles())
   {
      if (profile.valid)
      {
         mSnapshot.profileIds.push_back(profile.profileId);
      }
   }
   mEnvironmentConfig = nrm::EnvironmentConfigRepository::BuiltInDemo();
   const char* environmentPath = std::getenv("NRM_ENVIRONMENT_CONFIG");
   if (environmentPath != nullptr && environmentPath[0] != '\0')
   {
      nrm::EnvironmentConfigRepository external;
      nrm::EnvironmentConfigValidation validation;
      if (external.LoadFromFile(environmentPath, validation))
         mEnvironmentConfig = external;
   }
   mMessagesByNetwork.clear();
   mMetricsByNetwork.clear();
   mMetricsByLink.clear();
   mRadioByLink.clear();
   mQueuedTimes.clear();
   mTransmittedTimes.clear();
   mLastEndpointStates.clear();
   mLastLinkStates.clear();
   mLifecycleTracker.Clear();
   mEventLedger.Clear();
   mLastPublishTime = -1.0;
   RegisterCallbacks(aSimulation);

   PublishSnapshot(aSimulation, nrm::RuntimeState::cINITIALIZING);
}

void WkNrm::SimInterface::SimulationStarting(const WsfSimulation& aSimulation)
{
   PublishSnapshot(aSimulation, nrm::RuntimeState::cRUNNING);
}

void WkNrm::SimInterface::SimulationClockRead(const WsfSimulation& aSimulation)
{
   if (mLastPublishTime < 0.0 || aSimulation.GetSimTime() - mLastPublishTime >= 0.5)
   {
      PublishSnapshot(aSimulation, nrm::RuntimeState::cRUNNING);
   }
}

void WkNrm::SimInterface::SimulationComplete(const WsfSimulation& aSimulation)
{
   PublishSnapshot(aSimulation, nrm::RuntimeState::cCOMPLETE);
   mCallbacks.Clear();
}

void WkNrm::SimInterface::PublishSnapshot(const WsfSimulation& aSimulation, nrm::RuntimeState aState)
{
   ++mSnapshot.snapshotVersion;
   mSnapshot.simTime      = aSimulation.GetSimTime();
   mSnapshot.runtimeState = aState;
   mSnapshot.origin       = nrm::DataOrigin::cAFSIM_INTERNAL;
   mSnapshot.providerId   = "afsim-internal";
   mLifecycleTracker.Prune(mSnapshot.simTime);
   BuildResourceState(aSimulation);
   PruneCorrelations(mSnapshot.simTime);
   mLastPublishTime = mSnapshot.simTime;

   AddSimEvent(ut::make_unique<SnapshotEvent>(mSnapshot));
}

void WkNrm::SimInterface::RegisterCallbacks(const WsfSimulation& aSimulation)
{
   mCallbacks.Add(WsfObserver::MessageQueued(&aSimulation)
                     .Connect([this](double             aSimTime,
                                     wsf::comm::Comm*   aCommPtr,
                                     const WsfMessage& aMessage,
                                     size_t             aQueueDepth)
                              {
                                 nrm::MessageLifecycleRecord record;
                                 record.messageId = aMessage.GetSerialNumber();
                                 record.networkId = aCommPtr == nullptr ? "" : aCommPtr->GetNetwork();
                                 record.sourceEndpointId = EndpointId(aCommPtr);
                                 record.destinationEndpointId =
                                    aMessage.GetDstAddr().GetAddress();
                                 record.bits = static_cast<std::uint64_t>(
                                    std::max(0, aMessage.GetSizeBits()));
                                 record.queuedTime = aSimTime;
                                 if (mLifecycleTracker.RecordQueued(record))
                                 {
                                    CountMessage(aCommPtr, &nrm::MessageStatistics::queued);
                                 }
                                 mQueuedTimes[aMessage.GetSerialNumber()] = aSimTime;
                                 mSnapshot.messages.queueDepth = aQueueDepth;
                                 if (aCommPtr != nullptr)
                                 {
                                    mMessagesByNetwork[aCommPtr->GetNetwork()].queueDepth = aQueueDepth;
                                 }
                              }));
   mCallbacks.Add(WsfObserver::MessageTransmitted(&aSimulation)
                     .Connect([this](double             aSimTime,
                                     wsf::comm::Comm*   aCommPtr,
                                     const WsfMessage& aMessage)
                              {
                                 const std::uint64_t bits = static_cast<std::uint64_t>(
                                    std::max(0, aMessage.GetSizeBits()));
                                 if (mLifecycleTracker.RecordTransmitted(
                                        aMessage.GetSerialNumber(), aSimTime,
                                        aCommPtr == nullptr ? "" : aCommPtr->GetNetwork(),
                                        EndpointId(aCommPtr), bits))
                                 {
                                    CountMessage(aCommPtr, &nrm::MessageStatistics::transmitted);
                                 }
                                 mQueuedTimes.erase(aMessage.GetSerialNumber());
                                 mTransmittedTimes[aMessage.GetSerialNumber()] = aSimTime;
                              }));
   mCallbacks.Add(WsfObserver::MessageReceived(&aSimulation)
                     .Connect([this](double             aSimTime,
                                     wsf::comm::Comm*   aTransmitterPtr,
                                     wsf::comm::Comm*   aReceiverPtr,
                                     const WsfMessage& aMessage,
                                     wsf::comm::Result& aResult)
                              {
                                 // AFSIM publishes MessageReceived as (transmitter, receiver).
                                 // Keep these names aligned with WsfComm::ReceiveActions: swapping
                                 // them prevents successful delivery from reaching a terminal state
                                 // and later makes the lifecycle timeout look like a discard.
                                 const bool isFinalDestination =
                                    aReceiverPtr != nullptr &&
                                    aMessage.GetDstAddr() == aReceiverPtr->GetAddress();
                                 const bool firstTerminal =
                                    isFinalDestination && mLifecycleTracker.RecordTerminal(
                                       aMessage.GetSerialNumber(),
                                       nrm::MessageTerminalState::cDELIVERED,
                                       aSimTime,
                                       EndpointId(aReceiverPtr));
                                 if (firstTerminal)
                                 {
                                    CountMessage(aReceiverPtr, &nrm::MessageStatistics::received);
                                 }
                                 double transportDelayMs = -1.0;
                                 const auto transmittedIt =
                                    mTransmittedTimes.find(aMessage.GetSerialNumber());
                                 if (transmittedIt != mTransmittedTimes.end())
                                 {
                                    transportDelayMs = 1000.0 * (aSimTime - transmittedIt->second);
                                    mTransmittedTimes.erase(transmittedIt);
                                 }
                                 const std::uint64_t bits = static_cast<std::uint64_t>(
                                    std::max(0, aMessage.GetSizeBits()));
                                 if (aTransmitterPtr != nullptr && aReceiverPtr != nullptr)
                                 {
                                    const std::string linkId =
                                       LinkId(aTransmitterPtr, aReceiverPtr);
                                    nrm::RollingMetrics& linkMetrics = mMetricsByLink[linkId];
                                    linkMetrics.SetTransmitObservationAvailable(false);
                                    linkMetrics.RecordReceive(aSimTime, bits, transportDelayMs);
                                    LinkRadioState& radio = mRadioByLink[linkId];
                                    if (aResult.mDataRate > 0.0)
                                    {
                                       SetDirectMetric(
                                          radio.bandwidthBps, aResult.mDataRate, "bit/s", aSimTime);
                                    }
                                    if (aResult.mRcvdPower > 0.0)
                                    {
                                       SetDirectMetric(radio.rssiDbm,
                                                       10.0 * std::log10(aResult.mRcvdPower * 1000.0),
                                                       "dBm", aSimTime);
                                    }
                                    if (aResult.mSignalToNoise > 0.0)
                                    {
                                       SetDirectMetric(radio.snrDb,
                                                       10.0 * std::log10(aResult.mSignalToNoise),
                                                       "dB", aSimTime);
                                    }
                                    if (aResult.mBitErrorRate >= 0.0)
                                    {
                                       SetDirectMetric(radio.ber, aResult.mBitErrorRate,
                                                       "ratio", aSimTime);
                                    }
                                    if (aResult.mInterferencePower > 0.0)
                                    {
                                       SetDirectMetric(
                                          radio.interferencePowerDbm,
                                          10.0 * std::log10(aResult.mInterferencePower * 1000.0),
                                          "dBm", aSimTime);
                                    }
                                    if (aResult.mInterferenceFactor >= 0.0)
                                    {
                                       SetDirectMetric(radio.interferenceFactorPercent,
                                                       100.0 * aResult.mInterferenceFactor,
                                                       "percent", aSimTime);
                                    }
                                    if (aResult.mAbsorptionFactor > 0.0)
                                    {
                                       SetDirectMetric(radio.atmosphericTransmittancePercent,
                                                       100.0 * aResult.mAbsorptionFactor,
                                                       "percent", aSimTime);
                                    }
                                 }
                              }));
   mCallbacks.Add(WsfObserver::MessageHop(&aSimulation)
                     .Connect([this](double, wsf::comm::Comm* aReceiverPtr,
                                     wsf::comm::Comm*, const WsfMessage&)
                              { CountMessage(aReceiverPtr, &nrm::MessageStatistics::hops); }));
   mCallbacks.Add(WsfObserver::MessageDiscarded(&aSimulation)
                     .Connect([this](double             aSimTime,
                                     wsf::comm::Comm*   aCommPtr,
                                     const WsfMessage& aMessage,
                                     const std::string&)
                              {
                                 if (mLifecycleTracker.RecordTerminal(
                                        aMessage.GetSerialNumber(),
                                        nrm::MessageTerminalState::cDISCARDED,
                                        aSimTime))
                                 {
                                    CountMessage(aCommPtr, &nrm::MessageStatistics::discarded);
                                 }
                              }));
   mCallbacks.Add(WsfObserver::MessageFailedRouting(&aSimulation)
                     .Connect([this](double             aSimTime,
                                     wsf::comm::Comm*   aCommPtr,
                                     WsfPlatform*,
                                     const WsfMessage& aMessage)
                              {
                                 if (mLifecycleTracker.RecordTerminal(
                                        aMessage.GetSerialNumber(),
                                        nrm::MessageTerminalState::cROUTING_FAILED,
                                        aSimTime))
                                 {
                                    CountMessage(aCommPtr, &nrm::MessageStatistics::routingFailed);
                                 }
                              }));
}

void WkNrm::SimInterface::CountMessage(wsf::comm::Comm*                         aCommPtr,
                                       std::uint64_t nrm::MessageStatistics::*aCounter)
{
   ++(mSnapshot.messages.*aCounter);
   if (aCommPtr != nullptr)
   {
      ++(mMessagesByNetwork[aCommPtr->GetNetwork()].*aCounter);
   }
}

void WkNrm::SimInterface::PruneCorrelations(double aSimTime)
{
   const double oldestAllowed = aSimTime - nrm::RollingMetrics::cMAX_WINDOW_S;
   for (auto it = mQueuedTimes.begin(); it != mQueuedTimes.end();)
   {
      it = it->second <= oldestAllowed ? mQueuedTimes.erase(it) : std::next(it);
   }
   for (auto it = mTransmittedTimes.begin(); it != mTransmittedTimes.end();)
   {
      it = it->second <= oldestAllowed ? mTransmittedTimes.erase(it) : std::next(it);
   }
}

nrm::NetworkType WkNrm::SimInterface::GetNetworkType(const wsf::comm::Comm* aCommPtr)
{
   if (aCommPtr == nullptr)
   {
      return nrm::NetworkType::cUNKNOWN;
   }
   return nrm::ClassifyNetwork(aCommPtr->GetNetwork(), aCommPtr->GetType());
}

void WkNrm::SimInterface::BuildResourceState(const WsfSimulation& aSimulation)
{
   mSnapshot.networks.clear();
   mSnapshot.endpoints.clear();
   mSnapshot.links.clear();
   mSnapshot.navigation = nrm::NavigationSnapshot();
   mSnapshot.navigation.sampleTime = mSnapshot.simTime;
   for (std::size_t platformIndex = 0;
        platformIndex < aSimulation.GetPlatformCount(); ++platformIndex)
   {
      WsfPlatform* platformPtr = aSimulation.GetPlatformEntry(platformIndex);
      if (platformPtr == nullptr) continue;
      WsfNavigationErrors* navigationPtr = WsfNavigationErrors::Find(*platformPtr);
      if (navigationPtr == nullptr) continue;
      const WsfNavigationErrors::GPS_Status status = navigationPtr->GetGPS_Status();
      const double updateTime = navigationPtr->GetLastUpdateTime();
      if (updateTime < 0.0 && status != WsfNavigationErrors::cGPS_PERFECT) continue;

      nrm::NavigationSample sample;
      sample.platformName = platformPtr->GetName();
      sample.rawStatus = RawNavigationStatus(status);
      sample.mode = NavigationModeFromStatus(status);
      sample.navigationType = sample.mode == nrm::NavigationMode::cINS
                                 ? "INS"
                                 : (sample.mode == nrm::NavigationMode::cPERFECT
                                       ? "INTEGRATED"
                                       : "GNSS");
      sample.statusCode = static_cast<int>(status);
      sample.valid = sample.mode != nrm::NavigationMode::cUNKNOWN;
      sample.sampleTime = updateTime >= 0.0 ? updateTime : mSnapshot.simTime;
      double truthLat = 0.0;
      double truthLon = 0.0;
      double truthAlt = 0.0;
      double perceivedLat = 0.0;
      double perceivedLon = 0.0;
      double perceivedAlt = 0.0;
      platformPtr->GetLocationLLA(truthLat, truthLon, truthAlt);
      navigationPtr->GetPerceivedLocationLLA(
         perceivedLat, perceivedLon, perceivedAlt);
      SetDirectMetric(sample.truthLatitudeDeg, truthLat, "deg", sample.sampleTime);
      SetDirectMetric(sample.truthLongitudeDeg, truthLon, "deg", sample.sampleTime);
      SetDirectMetric(sample.truthAltitudeM, truthAlt, "m", sample.sampleTime);
      SetDirectMetric(sample.perceivedLatitudeDeg, perceivedLat, "deg", sample.sampleTime);
      SetDirectMetric(sample.perceivedLongitudeDeg, perceivedLon, "deg", sample.sampleTime);
      SetDirectMetric(sample.perceivedAltitudeM, perceivedAlt, "m", sample.sampleTime);
      double heading = 0.0;
      double pitch = 0.0;
      double roll = 0.0;
      platformPtr->GetOrientationNED(heading, pitch, roll);
      SetDirectMetric(sample.headingDeg,
                      UtMath::NormalizeAngle0_360(heading * UtMath::cDEG_PER_RAD),
                      "deg", sample.sampleTime);
      const ut::coords::RSCS error = navigationPtr->GetLocationErrorRSCS();
      SetDirectMetric(sample.inTrackErrorM, error[0], "m", sample.sampleTime);
      SetDirectMetric(sample.crossTrackErrorM, error[1], "m", sample.sampleTime);
      SetDirectMetric(sample.verticalErrorM, error[2], "m", sample.sampleTime);
      SetDirectMetric(sample.totalPositionErrorM, error.Magnitude(), "m", sample.sampleTime);
      mSnapshot.navigation.platforms.push_back(sample);
   }
   mSnapshot.navigation.valid = !mSnapshot.navigation.platforms.empty();
   mSnapshot.environment = nrm::EnvironmentSnapshot();
   mSnapshot.environment.sampleTime = mSnapshot.simTime;
   mSnapshot.environment.providerId = "afsim-internal";
   mSnapshot.environment.configVersion = mEnvironmentConfig.ConfigVersion();
   mSnapshot.environment.valid = true;

   WsfEnvironment& environment = aSimulation.GetEnvironment();
   nrm::WeatherEnvironmentState& weather = mSnapshot.environment.weather;
   weather.available = true;
   SetDirectMetric(weather.windSpeedMps, environment.GetWindSpeed(),
                   "m/s", mSnapshot.simTime);
   SetDirectMetric(weather.windDirectionDeg,
                   environment.GetWindDirection() * UtMath::cDEG_PER_RAD,
                   "deg", mSnapshot.simTime);
   SetDirectMetric(weather.rainRateMmPerHour,
                   environment.GetRainRate() * 3600000.0,
                   "mm/h", mSnapshot.simTime);
   SetDirectMetric(weather.rainUpperAltitudeM, environment.GetRainUpperLevel(),
                   "m", mSnapshot.simTime);
   double cloudLower = 0.0;
   double cloudUpper = 0.0;
   environment.GetCloudLevel(cloudLower, cloudUpper);
   SetDirectMetric(weather.cloudLowerAltitudeM, cloudLower, "m", mSnapshot.simTime);
   SetDirectMetric(weather.cloudUpperAltitudeM, cloudUpper, "m", mSnapshot.simTime);
   SetDirectMetric(weather.cloudWaterDensityKgPerM3,
                   environment.GetCloudWaterDensity(), "kg/m^3", mSnapshot.simTime);
   SetDirectMetric(weather.dustVisibilityM, environment.GetDustStormVisibility(),
                   "m", mSnapshot.simTime);

   nrm::CelestialEnvironmentState& celestial = mSnapshot.environment.celestial;
   celestial.available = true;
   celestial.usesSystemTime = aSimulation.GetDateTime().UsingSystemTime();
   SetDirectMetric(celestial.julianDate,
                   aSimulation.GetDateTime().GetStartJulianDate() +
                      mSnapshot.simTime / 86400.0,
                   "julian_day", mSnapshot.simTime);

   nrm::TerrainEnvironmentState& terrain = mSnapshot.environment.terrain;
   wsf::TerrainInterface* terrainPtr = aSimulation.GetTerrainInterface();
   terrain.available = terrainPtr != nullptr;
   terrain.enabled = terrainPtr != nullptr && terrainPtr->IsEnabled();
   mSnapshot.environment.interference.available = true;
   const std::uint64_t hopCount = mSnapshot.messages.hops;
   const std::size_t queueDepth = mSnapshot.messages.queueDepth;
   mSnapshot.messages = mLifecycleTracker.Cumulative();
   mSnapshot.messages.hops = hopCount;
   mSnapshot.messages.queueDepth = queueDepth;

   wsf::comm::NetworkManager* networkManagerPtr = aSimulation.GetCommNetworkManager();
   if (networkManagerPtr == nullptr)
   {
      return;
   }

   std::map<std::string, std::size_t> networkIndexes;
   for (const std::string& networkName : networkManagerPtr->GetManagedNetworks())
   {
      wsf::comm::Network* networkPtr = networkManagerPtr->GetNetwork(networkName);
      nrm::NetworkSnapshot network;
      network.networkId   = networkPtr == nullptr ? networkName : networkPtr->GetAddress().GetAddress();
      network.networkName = networkName.empty() ? "default" : networkName;
      network.modelType   = networkPtr == nullptr ? "" : networkPtr->GetScriptClassName();
      network.networkType = nrm::ClassifyNetwork(network.networkName, network.modelType);
      network.messages = mLifecycleTracker.Cumulative(networkName);
      const auto messageIt = mMessagesByNetwork.find(networkName);
      if (messageIt != mMessagesByNetwork.end())
      {
         network.messages.hops = messageIt->second.hops;
         network.messages.queueDepth = messageIt->second.queueDepth;
      }
      networkIndexes[networkName] = mSnapshot.networks.size();
      mSnapshot.networks.emplace_back(network);
   }

   std::map<std::string, std::size_t> endpointIndexes;
   for (const wsf::comm::Address& address : networkManagerPtr->GetComms())
   {
      wsf::comm::Comm* commPtr = networkManagerPtr->GetComm(address);
      if (commPtr == nullptr)
      {
         continue;
      }

      nrm::EndpointSnapshot endpoint;
      endpoint.endpointId  = EndpointId(commPtr);
      endpoint.address     = endpoint.endpointId;
      endpoint.commName    = commPtr->GetName();
      endpoint.commType    = commPtr->GetType();
      endpoint.networkName = commPtr->GetNetwork();
      endpoint.networkType = GetNetworkType(commPtr);
      endpoint.state       = CommState(commPtr);
      endpoint.canSend     = commPtr->CanSend();
      endpoint.canReceive  = commPtr->CanReceive();
      const auto previousState = mLastEndpointStates.find(endpoint.endpointId);
      if (previousState == mLastEndpointStates.end() || previousState->second != endpoint.state)
      {
         nrm::ResourceEvent event;
         event.kind = nrm::ResourceEventKind::cENDPOINT_STATE;
         event.simTime = mSnapshot.simTime;
         event.endpointId = endpoint.endpointId;
         event.networkId = endpoint.networkName;
         event.previousState = previousState == mLastEndpointStates.end()
                                  ? nrm::ResourceState::cUNKNOWN : previousState->second;
         event.currentState = endpoint.state;
         event.reasonCode = "AFSIM_COMM_STATE_CHANGED";
         mEventLedger.Record(event);
         mLastEndpointStates[endpoint.endpointId] = endpoint.state;
      }
      const nrm::ResourceStateMetrics endpointMetrics =
         mEventLedger.EndpointMetrics(endpoint.endpointId, mSnapshot.simTime, 60.0);
      endpoint.currentOfflineDurationS = endpointMetrics.currentOfflineDurationS;
      endpoint.windowOfflineDurationS = endpointMetrics.windowOfflineDurationS;
      endpoint.endpointOnlineRatioPercent = endpointMetrics.endpointOnlineRatioPercent;
      if (commPtr->GetPlatform() != nullptr)
      {
         endpoint.platformName = commPtr->GetPlatform()->GetName();
         double latitude = 0.0;
         double longitude = 0.0;
         double altitude = 0.0;
         commPtr->GetPlatform()->GetLocationLLA(latitude, longitude, altitude);
         SetPositionMetric(endpoint.latitudeDeg, latitude, "deg", mSnapshot.simTime);
         SetPositionMetric(endpoint.longitudeDeg, longitude, "deg", mSnapshot.simTime);
         SetPositionMetric(endpoint.altitudeM, altitude, "m", mSnapshot.simTime);
      }

      endpointIndexes[endpoint.endpointId] = mSnapshot.endpoints.size();
      mSnapshot.endpoints.emplace_back(endpoint);

      const auto networkIt = networkIndexes.find(commPtr->GetNetwork());
      if (networkIt != networkIndexes.end())
      {
         nrm::NetworkSnapshot& network = mSnapshot.networks[networkIt->second];
         ++network.endpointCount;
         if (endpoint.state == nrm::ResourceState::cONLINE)
         {
            ++network.onlineCount;
         }
      }
   }

   for (auto networkIt = mSnapshot.networks.begin(); networkIt != mSnapshot.networks.end();)
   {
      if (networkIt->endpointCount == 0 && networkIt->networkType == nrm::NetworkType::cUNKNOWN)
      {
         networkIt = mSnapshot.networks.erase(networkIt);
      }
      else
      {
         ++networkIt;
      }
   }
   networkIndexes.clear();
   for (std::size_t index = 0; index < mSnapshot.networks.size(); ++index)
   {
      networkIndexes[mSnapshot.networks[index].networkName] = index;
      nrm::NetworkSnapshot& network = mSnapshot.networks[index];
      const double onlineRatio =
         network.endpointCount == 0 ? 0.0 : static_cast<double>(network.onlineCount) / network.endpointCount;
      nrm::RollingMetrics& metrics = mMetricsByNetwork[network.networkName];
      if (mSnapshot.runtimeState != nrm::RuntimeState::cINITIALIZING && mSnapshot.simTime > 0.0)
      {
         metrics.RecordOnlineRatio(mSnapshot.simTime, onlineRatio);
      }
      for (double windowS : cWINDOWS_S)
      {
         nrm::WindowMetrics window =
            mLifecycleTracker.Snapshot(mSnapshot.simTime, windowS, network.networkName);
         const nrm::WindowMetrics stateWindow =
            metrics.Snapshot(mSnapshot.simTime, windowS);
         window.onlineRatioPercent = stateWindow.onlineRatioPercent;
         network.windows.emplace_back(window);
      }
   }

   wsf::comm::graph::Graph& graph = networkManagerPtr->GetGraph();
   for (const wsf::comm::graph::Node* nodePtr : graph.GetNodes())
   {
      if (nodePtr == nullptr)
      {
         continue;
      }
      for (const wsf::comm::graph::Edge* edgePtr : graph.GetOutgoingNodeEdges(nodePtr))
      {
         if (edgePtr == nullptr)
         {
            continue;
         }
         wsf::comm::Comm* sourcePtr = networkManagerPtr->GetComm(edgePtr->GetSourceAddress());
         wsf::comm::Comm* destinationPtr = networkManagerPtr->GetComm(edgePtr->GetDestinationAddress());
         if (sourcePtr == nullptr || destinationPtr == nullptr)
         {
            continue;
         }

         nrm::LinkSnapshot link;
         link.sourceEndpointId      = EndpointId(sourcePtr);
         link.destinationEndpointId = EndpointId(destinationPtr);
         link.linkId                = link.sourceEndpointId + "->" + link.destinationEndpointId;
         link.sourcePlatform        = sourcePtr->GetPlatform() == nullptr ? "" : sourcePtr->GetPlatform()->GetName();
         link.destinationPlatform =
            destinationPtr->GetPlatform() == nullptr ? "" : destinationPtr->GetPlatform()->GetName();
         link.networkName = sourcePtr->GetNetwork();
         link.networkType = GetNetworkType(sourcePtr);
         link.state       = edgePtr->IsEnabled() ? nrm::ResourceState::cONLINE : nrm::ResourceState::cDISABLED;
         const auto previousState = mLastLinkStates.find(link.linkId);
         if (previousState == mLastLinkStates.end() || previousState->second != link.state)
         {
            nrm::ResourceEvent event;
            event.kind = nrm::ResourceEventKind::cLINK_STATE;
            event.simTime = mSnapshot.simTime;
            event.linkId = link.linkId;
            event.networkId = link.networkName;
            event.sourceEndpointId = link.sourceEndpointId;
            event.destinationEndpointId = link.destinationEndpointId;
            event.previousState = previousState == mLastLinkStates.end()
                                     ? nrm::ResourceState::cUNKNOWN : previousState->second;
            event.currentState = link.state;
            event.reasonCode = "AFSIM_GRAPH_EDGE_STATE_CHANGED";
            mEventLedger.Record(event);
            mLastLinkStates[link.linkId] = link.state;
         }
         const nrm::ResourceStateMetrics linkMetrics =
            mEventLedger.LinkMetrics(link.linkId, mSnapshot.simTime, 60.0);
         link.currentOfflineDurationS = linkMetrics.currentOfflineDurationS;
         link.windowOfflineDurationS = linkMetrics.windowOfflineDurationS;
         link.serviceAvailabilityPercent = linkMetrics.serviceAvailabilityPercent;
         link.establishmentAttempts = linkMetrics.establishmentAttempts;
         link.establishmentSuccessRatioPercent =
            linkMetrics.establishmentSuccessRatioPercent;
         link.averageEstablishmentDelayMs = linkMetrics.averageEstablishmentDelayMs;

         if (sourcePtr->GetPlatform() != nullptr && destinationPtr->GetPlatform() != nullptr)
         {
            double sourceWcs[3] = {0.0, 0.0, 0.0};
            double destinationWcs[3] = {0.0, 0.0, 0.0};
            sourcePtr->GetPlatform()->GetLocationWCS(sourceWcs);
            destinationPtr->GetPlatform()->GetLocationWCS(destinationWcs);
            const double dx = sourceWcs[0] - destinationWcs[0];
            const double dy = sourceWcs[1] - destinationWcs[1];
            const double dz = sourceWcs[2] - destinationWcs[2];
            SetPositionMetric(link.distanceM, std::sqrt(dx * dx + dy * dy + dz * dz), "m", mSnapshot.simTime);
            link.distanceM.origin = nrm::DataOrigin::cDERIVED;
         }

         const auto radioIt = mRadioByLink.find(link.linkId);
         double capacityBps = -1.0;
         if (radioIt != mRadioByLink.end())
         {
            link.bandwidthBps = radioIt->second.bandwidthBps;
            link.rssiDbm      = radioIt->second.rssiDbm;
            link.snrDb        = radioIt->second.snrDb;
            link.ber          = radioIt->second.ber;
            link.interferencePowerDbm = radioIt->second.interferencePowerDbm;
            link.interferenceFactorPercent = radioIt->second.interferenceFactorPercent;
            link.atmosphericTransmittancePercent =
               radioIt->second.atmosphericTransmittancePercent;
            if (link.bandwidthBps.valid)
            {
               capacityBps = link.bandwidthBps.value;
            }
         }

         if (terrain.enabled && sourcePtr->GetPlatform() != nullptr &&
             destinationPtr->GetPlatform() != nullptr)
         {
            double sourceLat = 0.0;
            double sourceLon = 0.0;
            double sourceAlt = 0.0;
            double destinationLat = 0.0;
            double destinationLon = 0.0;
            double destinationAlt = 0.0;
            sourcePtr->GetPlatform()->GetLocationLLA(sourceLat, sourceLon, sourceAlt);
            destinationPtr->GetPlatform()->GetLocationLLA(
               destinationLat, destinationLon, destinationAlt);
            const bool blocked = terrainPtr->MaskedByTerrain(
               sourceLat, sourceLon, sourceAlt, destinationLat, destinationLon,
               destinationAlt, 0.0);
            SetDirectMetric(link.terrainBlockedFlag, blocked ? 1.0 : 0.0,
                            "boolean", mSnapshot.simTime);
            ++terrain.evaluatedLinkCount;
            if (blocked) ++terrain.blockedLinkCount;
         }

         nrm::InterferenceEnvironmentState& interference =
            mSnapshot.environment.interference;
         if (link.interferencePowerDbm.valid || link.interferenceFactorPercent.valid)
         {
            ++interference.observedLinkCount;
         }
         if (link.interferencePowerDbm.valid)
         {
            if (!interference.maximumPowerDbm.valid ||
                link.interferencePowerDbm.value > interference.maximumPowerDbm.value)
               interference.maximumPowerDbm = link.interferencePowerDbm;
         }
         if (link.interferenceFactorPercent.valid &&
             (!interference.maximumFactorPercent.valid ||
              link.interferenceFactorPercent.value >
                 interference.maximumFactorPercent.value))
            interference.maximumFactorPercent = link.interferenceFactorPercent;
         const auto metricsIt = mMetricsByLink.find(link.linkId);
         for (double windowS : cWINDOWS_S)
         {
            link.windows.emplace_back(metricsIt == mMetricsByLink.end()
                                         ? nrm::WindowMetrics()
                                         : metricsIt->second.Snapshot(mSnapshot.simTime, windowS, capacityBps));
            link.windows.back().windowS = windowS;
         }

         mSnapshot.links.emplace_back(link);
         const auto networkIt = networkIndexes.find(link.networkName);
         if (networkIt != networkIndexes.end() && link.state == nrm::ResourceState::cONLINE)
         {
            ++mSnapshot.networks[networkIt->second].activeLinks;
         }
      }
   }
}
