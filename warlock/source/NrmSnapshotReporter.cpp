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

void WriteMetric(std::ostream& aOutput, const nrm::MetricValue<double>& aMetric)
{
   aOutput << "{\"value\":" << aMetric.value << ",\"unit\":\"" << EscapeJson(aMetric.unit)
           << "\",\"valid\":" << (aMetric.valid ? "true" : "false") << ",\"source\":\""
           << nrm::ToString(aMetric.origin) << "\",\"confidence\":\""
           << nrm::ToString(aMetric.confidence) << "\",\"sample_time\":" << aMetric.sampleTime
           << ",\"window\":" << aMetric.window << '}';
}

void WriteWindows(std::ostream& aOutput, const std::vector<nrm::WindowMetrics>& aWindows)
{
   aOutput << '[';
   for (std::size_t index = 0; index < aWindows.size(); ++index)
   {
      const nrm::WindowMetrics& window = aWindows[index];
      if (index != 0)
      {
         aOutput << ',';
      }
      aOutput << "{\"window_s\":" << window.windowS << ",\"transmitted\":"
              << window.messages.transmitted << ",\"received\":" << window.messages.received
              << ",\"discarded\":" << window.messages.discarded << ",\"routing_failed\":"
              << window.messages.routingFailed << ",\"transmitted_bits\":" << window.transmittedBits
              << ",\"throughput_bps\":";
      WriteMetric(aOutput, window.throughputBps);
      aOutput << ",\"pdr_percent\":";
      WriteMetric(aOutput, window.pdrPercent);
      aOutput << ",\"online_ratio_percent\":";
      WriteMetric(aOutput, window.onlineRatioPercent);
      aOutput << ",\"average_queue_delay_ms\":";
      WriteMetric(aOutput, window.averageQueueDelayMs);
      aOutput << ",\"average_transport_delay_ms\":";
      WriteMetric(aOutput, window.averageTransportDelayMs);
      aOutput << ",\"utilization_percent\":";
      WriteMetric(aOutput, window.utilizationPercent);
      aOutput << '}';
   }
   aOutput << ']';
}

void WriteAssessment(std::ostream& aOutput, const nrm::AssessmentResult& aResult)
{
   aOutput << "{\"schema\":\"nrm.assessment.v2\",\"task_id\":\"" << EscapeJson(aResult.taskId)
           << "\",\"snapshot_version\":" << aResult.snapshotVersion << ",\"sim_time\":"
           << aResult.simTime << ",\"reachable\":" << (aResult.reachable ? "true" : "false")
           << ",\"can_establish\":" << (aResult.canEstablish ? "true" : "false")
           << ",\"can_complete\":" << (aResult.canComplete ? "true" : "false")
           << ",\"stable\":" << (aResult.stable ? "true" : "false") << ",\"primary_route\":[";
   for (std::size_t index = 0; index < aResult.primaryRoute.size(); ++index)
   {
      if (index != 0)
      {
         aOutput << ',';
      }
      aOutput << '"' << EscapeJson(aResult.primaryRoute[index]) << '"';
   }
   aOutput << "],\"primary_route_uses_candidate\":"
           << (aResult.primaryRouteUsesCandidate ? "true" : "false")
           << ",\"backup_route\":[";
   for (std::size_t index = 0; index < aResult.backupRoute.size(); ++index)
   {
      if (index != 0)
      {
         aOutput << ',';
      }
      aOutput << '"' << EscapeJson(aResult.backupRoute[index]) << '"';
   }
   aOutput << "],\"backup_route_uses_candidate\":"
           << (aResult.backupRouteUsesCandidate ? "true" : "false")
           << ",\"network_sequence\":[";
   for (std::size_t index = 0; index < aResult.networkSequence.size(); ++index)
   {
      if (index != 0)
      {
         aOutput << ',';
      }
      aOutput << '"' << nrm::ToString(aResult.networkSequence[index]) << '"';
   }
   aOutput << "],\"predicted_delay_ms\":";
   WriteMetric(aOutput, aResult.predictedDelayMs);
   aOutput << ",\"estimated_pdr_percent\":";
   WriteMetric(aOutput, aResult.estimatedPdrPercent);
   aOutput << ",\"bottleneck_bandwidth_bps\":";
   WriteMetric(aOutput, aResult.bottleneckBandwidthBps);
   aOutput << ",\"bandwidth_margin_bps\":";
   WriteMetric(aOutput, aResult.bandwidthMarginBps);
   aOutput << ",\"delay_margin_ms\":";
   WriteMetric(aOutput, aResult.delayMarginMs);
   aOutput << ",\"reliability_margin_percent\":";
   WriteMetric(aOutput, aResult.reliabilityMarginPercent);
   aOutput << ",\"reason_codes\":[";
   for (std::size_t index = 0; index < aResult.reasons.size(); ++index)
   {
      if (index != 0)
      {
         aOutput << ',';
      }
      aOutput << '"' << nrm::ToString(aResult.reasons[index]) << '"';
   }
   aOutput << "],\"recommendations\":[";
   for (std::size_t index = 0; index < aResult.recommendations.size(); ++index)
   {
      if (index != 0)
      {
         aOutput << ',';
      }
      aOutput << '"' << EscapeJson(aResult.recommendations[index]) << '"';
   }
   aOutput << "]}\n";
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
              << network.messages.received << ",\"windows\":";
      WriteWindows(aOutput, network.windows);
      aOutput << '}';
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
              << "\",\"distance_m\":";
      WriteMetric(aOutput, link.distanceM);
      aOutput << ",\"rssi_dbm\":";
      WriteMetric(aOutput, link.rssiDbm);
      aOutput << ",\"snr_db\":";
      WriteMetric(aOutput, link.snrDb);
      aOutput << ",\"ber\":";
      WriteMetric(aOutput, link.ber);
      aOutput << ",\"bandwidth_bps\":";
      WriteMetric(aOutput, link.bandwidthBps);
      aOutput << ",\"windows\":";
      WriteWindows(aOutput, link.windows);
      aOutput << '}';
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

void WkNrm::SnapshotReporter::EnqueueAssessment(const nrm::AssessmentResult& aResult)
{
   {
      std::lock_guard<std::mutex> lock(mMutex);
      if (mAssessmentQueue.size() >= cMAX_QUEUE_SIZE)
      {
         mAssessmentQueue.pop_front();
      }
      mAssessmentQueue.push_back(aResult);
   }
   mCondition.notify_one();
}

void WkNrm::SnapshotReporter::Run()
{
   std::ofstream jsonOutput(mOutputDirectory + "/resource_snapshots.jsonl", std::ios::out | std::ios::trunc);
   std::ofstream csvOutput(mOutputDirectory + "/network_summary.csv", std::ios::out | std::ios::trunc);
   std::ofstream assessmentOutput(mOutputDirectory + "/assessment_results.jsonl",
                                  std::ios::out | std::ios::trunc);
   csvOutput << "snapshot_version,sim_time,network_type,network_name,members,online,active_links,"
                "transmitted,received,discarded,routing_failed,window_s,throughput_bps,pdr_percent,"
                "online_ratio_percent,average_queue_delay_ms,average_transport_delay_ms\n";

   while (true)
   {
      nrm::ResourceSnapshot snapshot;
      nrm::AssessmentResult assessment;
      bool hasSnapshot = false;
      bool hasAssessment = false;
      {
         std::unique_lock<std::mutex> lock(mMutex);
         mCondition.wait(lock, [this] { return mStopping || !mQueue.empty() || !mAssessmentQueue.empty(); });
         if (mQueue.empty() && mAssessmentQueue.empty() && mStopping)
         {
            break;
         }
         if (!mAssessmentQueue.empty())
         {
            assessment = mAssessmentQueue.front();
            mAssessmentQueue.pop_front();
            hasAssessment = true;
         }
         else
         {
            snapshot = mQueue.front();
            mQueue.pop_front();
            hasSnapshot = true;
         }
      }

      if (hasAssessment)
      {
         WriteAssessment(assessmentOutput, assessment);
         assessmentOutput.flush();
      }
      if (hasSnapshot)
      {
         WriteJsonSnapshot(jsonOutput, snapshot);
         for (const nrm::NetworkSnapshot& network : snapshot.networks)
         {
            for (const nrm::WindowMetrics& window : network.windows)
            {
               csvOutput << snapshot.snapshotVersion << ',' << std::fixed << std::setprecision(3)
                         << snapshot.simTime << ',' << nrm::ToString(network.networkType) << ",\""
                         << network.networkName << "\"," << network.endpointCount << ',' << network.onlineCount
                         << ',' << network.activeLinks << ',' << network.messages.transmitted << ','
                         << network.messages.received << ',' << network.messages.discarded << ','
                         << network.messages.routingFailed << ',' << window.windowS << ',';
               if (window.throughputBps.valid)
               {
                  csvOutput << window.throughputBps.value;
               }
               csvOutput << ',';
               if (window.pdrPercent.valid)
               {
                  csvOutput << window.pdrPercent.value;
               }
               csvOutput << ',';
               if (window.onlineRatioPercent.valid)
               {
                  csvOutput << window.onlineRatioPercent.value;
               }
               csvOutput << ',';
               if (window.averageQueueDelayMs.valid)
               {
                  csvOutput << window.averageQueueDelayMs.value;
               }
               csvOutput << ',';
               if (window.averageTransportDelayMs.valid)
               {
                  csvOutput << window.averageTransportDelayMs.value;
               }
               csvOutput << '\n';
            }
         }
         jsonOutput.flush();
         csvOutput.flush();
      }
   }
}
