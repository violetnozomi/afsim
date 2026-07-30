#include "NrmSimInterface.hpp"

#include <cmath>
#include <map>
#include <set>
#include <string>

#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommGraph.hpp"
#include "WsfCommNetwork.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
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
}
} // namespace

WkNrm::SimInterface::SimInterface(const QString& aPluginName)
   : warlock::SimInterfaceT<SimEvent>(aPluginName)
{
}

void WkNrm::SimInterface::SimulationInitializing(const WsfSimulation& aSimulation)
{
   mCallbacks.Clear();
   mSnapshot         = nrm::ResourceSnapshot();
   mMessagesByNetwork.clear();
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
   BuildResourceState(aSimulation);
   mLastPublishTime = mSnapshot.simTime;

   AddSimEvent(ut::make_unique<SnapshotEvent>(mSnapshot));
}

void WkNrm::SimInterface::RegisterCallbacks(const WsfSimulation& aSimulation)
{
   mCallbacks.Add(WsfObserver::MessageQueued(&aSimulation)
                     .Connect([this](double, wsf::comm::Comm* aCommPtr, const WsfMessage&, size_t aQueueDepth)
                              {
                                 CountMessage(aCommPtr, &nrm::MessageStatistics::queued);
                                 mSnapshot.messages.queueDepth = aQueueDepth;
                                 if (aCommPtr != nullptr)
                                 {
                                    mMessagesByNetwork[aCommPtr->GetNetwork()].queueDepth = aQueueDepth;
                                 }
                              }));
   mCallbacks.Add(WsfObserver::MessageTransmitted(&aSimulation)
                     .Connect([this](double, wsf::comm::Comm* aCommPtr, const WsfMessage&)
                              { CountMessage(aCommPtr, &nrm::MessageStatistics::transmitted); }));
   mCallbacks.Add(WsfObserver::MessageReceived(&aSimulation)
                     .Connect([this](double,
                                     wsf::comm::Comm* aReceiverPtr,
                                     wsf::comm::Comm*,
                                     const WsfMessage&,
                                     wsf::comm::Result&)
                              { CountMessage(aReceiverPtr, &nrm::MessageStatistics::received); }));
   mCallbacks.Add(WsfObserver::MessageHop(&aSimulation)
                     .Connect([this](double, wsf::comm::Comm* aReceiverPtr, wsf::comm::Comm*, const WsfMessage&)
                              { CountMessage(aReceiverPtr, &nrm::MessageStatistics::hops); }));
   mCallbacks.Add(WsfObserver::MessageDiscarded(&aSimulation)
                     .Connect([this](double, wsf::comm::Comm* aCommPtr, const WsfMessage&, const std::string&)
                              { CountMessage(aCommPtr, &nrm::MessageStatistics::discarded); }));
   mCallbacks.Add(WsfObserver::MessageFailedRouting(&aSimulation)
                     .Connect([this](double, wsf::comm::Comm* aCommPtr, WsfPlatform*, const WsfMessage&)
                              { CountMessage(aCommPtr, &nrm::MessageStatistics::routingFailed); }));
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
      const auto messageIt = mMessagesByNetwork.find(networkName);
      if (messageIt != mMessagesByNetwork.end())
      {
         network.messages = messageIt->second;
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

         mSnapshot.links.emplace_back(link);
         const auto networkIt = networkIndexes.find(link.networkName);
         if (networkIt != networkIndexes.end() && link.state == nrm::ResourceState::cONLINE)
         {
            ++mSnapshot.networks[networkIt->second].activeLinks;
         }
      }
   }
}
