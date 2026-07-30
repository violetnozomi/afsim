#include "NrmSnapshotReporter.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

#include "nrm/NetworkTypeUtils.hpp"

namespace
{
std::string EscapeJson(const std::string& aValue)
{
   std::ostringstream output;
   for (char character : aValue)
   {
      switch (character)
      {
      case '\\':
         output << "\\\\";
         break;
      case '"':
         output << "\\\"";
         break;
      case '\n':
         output << "\\n";
         break;
      case '\r':
         output << "\\r";
         break;
      case '\t':
         output << "\\t";
         break;
      default:
         output << character;
         break;
      }
   }
   return output.str();
}

void WriteJsonSnapshot(std::ostream& aOutput, const nrm::ResourceSnapshot& aSnapshot)
{
   aOutput << "{\"schema\":\"nrm.snapshot.v1\",\"snapshot_version\":" << aSnapshot.snapshotVersion
           << ",\"sim_time\":" << std::fixed << std::setprecision(3) << aSnapshot.simTime
           << ",\"provider_id\":\"" << EscapeJson(aSnapshot.providerId) << "\",\"network_count\":"
           << aSnapshot.networks.size() << ",\"endpoint_count\":" << aSnapshot.endpoints.size()
           << ",\"link_count\":" << aSnapshot.links.size() << ",\"messages\":{\"queued\":"
           << aSnapshot.messages.queued << ",\"transmitted\":" << aSnapshot.messages.transmitted
           << ",\"received\":" << aSnapshot.messages.received << ",\"hops\":" << aSnapshot.messages.hops
           << ",\"discarded\":" << aSnapshot.messages.discarded << ",\"routing_failed\":"
           << aSnapshot.messages.routingFailed << "},\"networks\":[";

   for (std::size_t index = 0; index < aSnapshot.networks.size(); ++index)
   {
      const nrm::NetworkSnapshot& network = aSnapshot.networks[index];
      if (index != 0)
      {
         aOutput << ',';
      }
      aOutput << "{\"name\":\"" << EscapeJson(network.networkName) << "\",\"type\":\""
              << nrm::ToString(network.networkType) << "\",\"members\":" << network.endpointCount
              << ",\"online\":" << network.onlineCount << ",\"active_links\":" << network.activeLinks
              << ",\"transmitted\":" << network.messages.transmitted << ",\"received\":"
              << network.messages.received << '}';
   }
   aOutput << "],\"endpoints\":[";
   for (std::size_t index = 0; index < aSnapshot.endpoints.size(); ++index)
   {
      const nrm::EndpointSnapshot& endpoint = aSnapshot.endpoints[index];
      if (index != 0)
      {
         aOutput << ',';
      }
      aOutput << "{\"id\":\"" << EscapeJson(endpoint.endpointId) << "\",\"platform\":\""
              << EscapeJson(endpoint.platformName) << "\",\"comm\":\"" << EscapeJson(endpoint.commName)
              << "\",\"network\":\"" << EscapeJson(endpoint.networkName) << "\",\"network_type\":\""
              << nrm::ToString(endpoint.networkType) << "\",\"state\":\"" << nrm::ToString(endpoint.state)
              << "\",\"can_send\":" << (endpoint.canSend ? "true" : "false") << ",\"can_receive\":"
              << (endpoint.canReceive ? "true" : "false") << ",\"position\":{\"latitude_deg\":"
              << endpoint.latitudeDeg.value << ",\"longitude_deg\":" << endpoint.longitudeDeg.value
              << ",\"altitude_m\":" << endpoint.altitudeM.value << ",\"valid\":"
              << (endpoint.latitudeDeg.valid && endpoint.longitudeDeg.valid && endpoint.altitudeM.valid ? "true"
                                                                                                        : "false")
              << "}}";
   }
   aOutput << "],\"links\":[";
   for (std::size_t index = 0; index < aSnapshot.links.size(); ++index)
   {
      const nrm::LinkSnapshot& link = aSnapshot.links[index];
      if (index != 0)
      {
         aOutput << ',';
      }
      aOutput << "{\"id\":\"" << EscapeJson(link.linkId) << "\",\"source\":\""
              << EscapeJson(link.sourceEndpointId) << "\",\"destination\":\""
              << EscapeJson(link.destinationEndpointId) << "\",\"network_type\":\""
              << nrm::ToString(link.networkType) << "\",\"state\":\"" << nrm::ToString(link.state)
              << "\",\"distance_m\":{\"value\":" << link.distanceM.value << ",\"valid\":"
              << (link.distanceM.valid ? "true" : "false") << ",\"source\":\""
              << nrm::ToString(link.distanceM.origin) << "\",\"confidence\":\""
              << nrm::ToString(link.distanceM.confidence) << "\"},\"rssi_dbm\":{\"value\":"
              << link.rssiDbm.value << ",\"valid\":" << (link.rssiDbm.valid ? "true" : "false")
              << "},\"snr_db\":{\"value\":" << link.snrDb.value << ",\"valid\":"
              << (link.snrDb.valid ? "true" : "false") << "},\"ber\":{\"value\":" << link.ber.value
              << ",\"valid\":" << (link.ber.valid ? "true" : "false") << "}}";
   }
   aOutput << "]}\n";
}
} // namespace

WkNrm::SnapshotReporter::SnapshotReporter(const std::string& aOutputDirectory)
   : mOutputDirectory(aOutputDirectory)
   , mThread(&SnapshotReporter::Run, this)
{
}

WkNrm::SnapshotReporter::~SnapshotReporter()
{
   {
      std::lock_guard<std::mutex> lock(mMutex);
      mStopping = true;
   }
   mCondition.notify_one();
   if (mThread.joinable())
   {
      mThread.join();
   }
}

void WkNrm::SnapshotReporter::Enqueue(const nrm::ResourceSnapshot& aSnapshot)
{
   {
      std::lock_guard<std::mutex> lock(mMutex);
      if (mQueue.size() >= cMAX_QUEUE_SIZE)
      {
         mQueue.pop_front();
      }
      mQueue.push_back(aSnapshot);
   }
   mCondition.notify_one();
}

void WkNrm::SnapshotReporter::Run()
{
   std::ofstream jsonOutput(mOutputDirectory + "/resource_snapshots.jsonl", std::ios::out | std::ios::trunc);
   std::ofstream csvOutput(mOutputDirectory + "/network_summary.csv", std::ios::out | std::ios::trunc);
   csvOutput << "snapshot_version,sim_time,network_type,network_name,members,online,active_links,"
                "transmitted,received,discarded,routing_failed\n";

   while (true)
   {
      nrm::ResourceSnapshot snapshot;
      {
         std::unique_lock<std::mutex> lock(mMutex);
         mCondition.wait(lock, [this] { return mStopping || !mQueue.empty(); });
         if (mQueue.empty() && mStopping)
         {
            break;
         }
         snapshot = mQueue.front();
         mQueue.pop_front();
      }

      WriteJsonSnapshot(jsonOutput, snapshot);
      for (const nrm::NetworkSnapshot& network : snapshot.networks)
      {
         csvOutput << snapshot.snapshotVersion << ',' << std::fixed << std::setprecision(3) << snapshot.simTime << ','
                   << nrm::ToString(network.networkType) << ",\"" << network.networkName << "\","
                   << network.endpointCount << ',' << network.onlineCount << ',' << network.activeLinks << ','
                   << network.messages.transmitted << ',' << network.messages.received << ','
                   << network.messages.discarded << ',' << network.messages.routingFailed << '\n';
      }
      jsonOutput.flush();
      csvOutput.flush();
   }
}
