#include "NrmSnapshotReporter.hpp"

#include <atomic>
#include <cerrno>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "nrm/NetworkTypeUtils.hpp"
#include "nrm/Version.hpp"

namespace
{
std::string EscapeJson(const std::string& aValue)
{
   std::ostringstream output;
   for (char character : aValue)
   {
      switch (character)
      {
      case '\\': output << "\\\\"; break;
      case '"': output << "\\\""; break;
      case '\n': output << "\\n"; break;
      case '\r': output << "\\r"; break;
      case '\t': output << "\\t"; break;
      default: output << character; break;
      }
   }
   return output.str();
}

std::string UtcTimestamp(bool aCompact)
{
   const std::time_t now = std::time(nullptr);
   std::tm utc{};
   gmtime_r(&now, &utc);
   std::ostringstream output;
   output << std::put_time(&utc, aCompact ? "%Y%m%dT%H%M%SZ" : "%Y-%m-%dT%H:%M:%SZ");
   return output.str();
}

std::string GenerateRunId()
{
   static std::atomic<unsigned long> counter{0};
   const long long milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(
         std::chrono::system_clock::now().time_since_epoch()).count();
   std::ostringstream output;
   output << "run-" << UtcTimestamp(true) << '-' << getpid() << '-'
          << milliseconds << '-' << counter.fetch_add(1);
   return output.str();
}

bool MakeDirectories(const std::string& aPath)
{
   if (aPath.empty())
   {
      return false;
   }
   std::string current;
   std::size_t position = 0;
   if (aPath[0] == '/')
   {
      current = "/";
      position = 1;
   }
   while (position <= aPath.size())
   {
      const std::size_t slash = aPath.find('/', position);
      const std::string part =
         aPath.substr(position, slash == std::string::npos ? std::string::npos : slash - position);
      if (!part.empty())
      {
         if (!current.empty() && current.back() != '/')
         {
            current += '/';
         }
         current += part;
         if (mkdir(current.c_str(), 0750) != 0 && errno != EEXIST)
         {
            return false;
         }
      }
      if (slash == std::string::npos)
      {
         break;
      }
      position = slash + 1;
   }
   return true;
}

void WriteMetric(std::ostream& aOutput, const nrm::MetricValue<double>& aMetric)
{
   aOutput << "{\"value\":" << aMetric.value << ",\"unit\":\""
           << EscapeJson(aMetric.unit) << "\",\"valid\":"
           << (aMetric.valid ? "true" : "false") << ",\"source\":\""
           << nrm::ToString(aMetric.origin) << "\",\"confidence\":\""
           << nrm::ToString(aMetric.confidence) << "\",\"reasonCode\":\""
           << nrm::ToString(aMetric.reason) << "\",\"sampleTime\":"
           << aMetric.sampleTime << ",\"window\":" << aMetric.window << '}';
}

void WriteWindows(std::ostream& aOutput, const std::vector<nrm::WindowMetrics>& aWindows)
{
   aOutput << '[';
   for (std::size_t index = 0; index < aWindows.size(); ++index)
   {
      const nrm::WindowMetrics& window = aWindows[index];
      if (index != 0) aOutput << ',';
      aOutput << "{\"windowS\":" << window.windowS
              << ",\"transmitted\":" << window.messages.transmitted
              << ",\"received\":" << window.messages.received
              << ",\"discarded\":" << window.messages.discarded
              << ",\"routingFailed\":" << window.messages.routingFailed
              << ",\"offeredBits\":" << window.transmittedBits
              << ",\"deliveredBits\":" << window.deliveredBits
              << ",\"offeredLoadBps\":";
      WriteMetric(aOutput, window.offeredLoadBps);
      aOutput << ",\"deliveredThroughputBps\":";
      WriteMetric(aOutput, window.deliveredThroughputBps);
      aOutput << ",\"deliveryRatioPercent\":";
      WriteMetric(aOutput, window.deliveryRatioPercent);
      aOutput << ",\"throughputBps\":";
      WriteMetric(aOutput, window.throughputBps);
      aOutput << ",\"pdrPercent\":";
      WriteMetric(aOutput, window.pdrPercent);
      aOutput << ",\"averageQueueDelayMs\":";
      WriteMetric(aOutput, window.averageQueueDelayMs);
      aOutput << ",\"p50QueueDelayMs\":";
      WriteMetric(aOutput, window.p50QueueDelayMs);
      aOutput << ",\"p95QueueDelayMs\":";
      WriteMetric(aOutput, window.p95QueueDelayMs);
      aOutput << ",\"averageTransportDelayMs\":";
      WriteMetric(aOutput, window.averageTransportDelayMs);
      aOutput << ",\"p50TransportDelayMs\":";
      WriteMetric(aOutput, window.p50TransportDelayMs);
      aOutput << ",\"p95TransportDelayMs\":";
      WriteMetric(aOutput, window.p95TransportDelayMs);
      aOutput << ",\"onlineRatioPercent\":";
      WriteMetric(aOutput, window.onlineRatioPercent);
      aOutput << ",\"utilizationPercent\":";
      WriteMetric(aOutput, window.utilizationPercent);
      // Preserve the v0.6 window contract while v0.7 consumers migrate to camelCase.
      aOutput << ",\"window_s\":" << window.windowS
              << ",\"routing_failed\":" << window.messages.routingFailed
              << ",\"transmitted_bits\":" << window.transmittedBits
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

void WriteStringArray(std::ostream& aOutput, const std::vector<std::string>& aValues)
{
   aOutput << '[';
   for (std::size_t index = 0; index < aValues.size(); ++index)
   {
      if (index != 0) aOutput << ',';
      aOutput << '"' << EscapeJson(aValues[index]) << '"';
   }
   aOutput << ']';
}

void WriteAssessment(std::ostream& aOutput,
                     const nrm::AssessmentResult& aResult,
                     const std::string& aRunId,
                     const std::string& aDefaultConfigVersion)
{
   const std::string configVersion =
      aResult.configVersion.empty() ? aDefaultConfigVersion : aResult.configVersion;
   aOutput << "{\"schema\":\"nrm.assessment.v3\",\"schemaVersion\":\"nrm.assessment.v3\""
           << ",\"runId\":\"" << EscapeJson(aRunId)
           << "\",\"configVersion\":\"" << EscapeJson(configVersion)
           << "\",\"profileProviderId\":\"" << EscapeJson(aResult.profileProviderId)
           << "\",\"profileIds\":";
   WriteStringArray(aOutput, aResult.profileIds);
   aOutput << ",\"task_id\":\"" << EscapeJson(aResult.taskId)
           << "\",\"snapshot_version\":" << aResult.snapshotVersion
           << ",\"sim_time\":" << aResult.simTime
           << ",\"reachable\":" << (aResult.reachable ? "true" : "false")
           << ",\"can_establish\":" << (aResult.canEstablish ? "true" : "false")
           << ",\"can_complete\":" << (aResult.canComplete ? "true" : "false")
           << ",\"stable\":" << (aResult.stable ? "true" : "false")
           << ",\"consideredPathCount\":" << aResult.consideredPathCount
           << ",\"selectedPathRank\":" << aResult.selectedPathRank
           << ",\"disjointnessType\":\"" << EscapeJson(aResult.disjointnessType)
           << "\",\"primary_route\":";
   WriteStringArray(aOutput, aResult.primaryRoute);
   aOutput << ",\"primary_route_uses_candidate\":"
           << (aResult.primaryRouteUsesCandidate ? "true" : "false")
           << ",\"backup_route\":";
   WriteStringArray(aOutput, aResult.backupRoute);
   aOutput << ",\"backup_route_uses_candidate\":"
           << (aResult.backupRouteUsesCandidate ? "true" : "false")
           << ",\"network_sequence\":[";
   for (std::size_t index = 0; index < aResult.networkSequence.size(); ++index)
   {
      if (index != 0) aOutput << ',';
      aOutput << '"' << nrm::ToString(aResult.networkSequence[index]) << '"';
   }
   aOutput << ']'
           << ",\"failedConstraints\":";
   WriteStringArray(aOutput, aResult.failedConstraints);
   aOutput << ",\"predicted_delay_ms\":";
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
      if (index != 0) aOutput << ',';
      aOutput << '"' << nrm::ToString(aResult.reasons[index]) << '"';
   }
   aOutput << "],\"recommendations\":";
   WriteStringArray(aOutput, aResult.recommendations);
   aOutput << "}\n";
}

void WriteJsonSnapshot(std::ostream& aOutput,
                       const nrm::ResourceSnapshot& aSnapshot,
                       const std::string& aRunId,
                       const std::string& aDefaultConfigVersion)
{
   const std::string configVersion =
      aSnapshot.configVersion.empty() ? aDefaultConfigVersion : aSnapshot.configVersion;
   aOutput << "{\"schema\":\"nrm.snapshot.v2\",\"schemaVersion\":\""
           << EscapeJson(aSnapshot.schemaVersion) << "\",\"runId\":\""
           << EscapeJson(aRunId) << "\",\"configVersion\":\""
           << EscapeJson(configVersion) << "\",\"profileIds\":";
   WriteStringArray(aOutput, aSnapshot.profileIds);
   aOutput << ",\"snapshot_version\":" << aSnapshot.snapshotVersion
           << ",\"sim_time\":" << std::fixed << std::setprecision(3)
           << aSnapshot.simTime << ",\"provider_id\":\""
           << EscapeJson(aSnapshot.providerId) << "\",\"network_count\":"
           << aSnapshot.networks.size() << ",\"endpoint_count\":"
           << aSnapshot.endpoints.size() << ",\"link_count\":"
           << aSnapshot.links.size() << ",\"messages\":{\"queued\":"
           << aSnapshot.messages.queued << ",\"transmitted\":"
           << aSnapshot.messages.transmitted << ",\"received\":"
           << aSnapshot.messages.received << ",\"hops\":"
           << aSnapshot.messages.hops << ",\"discarded\":"
           << aSnapshot.messages.discarded << ",\"routing_failed\":"
           << aSnapshot.messages.routingFailed << "},\"networks\":[";

   for (std::size_t index = 0; index < aSnapshot.networks.size(); ++index)
   {
      const nrm::NetworkSnapshot& network = aSnapshot.networks[index];
      if (index != 0) aOutput << ',';
      aOutput << "{\"name\":\"" << EscapeJson(network.networkName)
              << "\",\"type\":\"" << nrm::ToString(network.networkType)
              << "\",\"members\":" << network.endpointCount
              << ",\"online\":" << network.onlineCount
              << ",\"active_links\":" << network.activeLinks
              << ",\"transmitted\":" << network.messages.transmitted
              << ",\"received\":" << network.messages.received
              << ",\"discarded\":" << network.messages.discarded
              << ",\"routing_failed\":" << network.messages.routingFailed
              << ",\"windows\":";
      WriteWindows(aOutput, network.windows);
      aOutput << '}';
   }
   aOutput << "],\"endpoints\":[";
   for (std::size_t index = 0; index < aSnapshot.endpoints.size(); ++index)
   {
      const nrm::EndpointSnapshot& endpoint = aSnapshot.endpoints[index];
      if (index != 0) aOutput << ',';
      aOutput << "{\"id\":\"" << EscapeJson(endpoint.endpointId)
              << "\",\"platform\":\"" << EscapeJson(endpoint.platformName)
              << "\",\"comm\":\"" << EscapeJson(endpoint.commName)
              << "\",\"network\":\"" << EscapeJson(endpoint.networkName)
              << "\",\"network_type\":\"" << nrm::ToString(endpoint.networkType)
              << "\",\"state\":\"" << nrm::ToString(endpoint.state)
              << "\",\"can_send\":" << (endpoint.canSend ? "true" : "false")
              << ",\"can_receive\":" << (endpoint.canReceive ? "true" : "false")
              << ",\"position\":{\"latitude_deg\":" << endpoint.latitudeDeg.value
              << ",\"longitude_deg\":" << endpoint.longitudeDeg.value
              << ",\"altitude_m\":" << endpoint.altitudeM.value
              << ",\"valid\":"
              << (endpoint.latitudeDeg.valid && endpoint.longitudeDeg.valid && endpoint.altitudeM.valid
                     ? "true" : "false") << '}'
              << ",\"currentOfflineDurationS\":";
      WriteMetric(aOutput, endpoint.currentOfflineDurationS);
      aOutput << ",\"windowOfflineDurationS\":";
      WriteMetric(aOutput, endpoint.windowOfflineDurationS);
      aOutput << ",\"endpointOnlineRatioPercent\":";
      WriteMetric(aOutput, endpoint.endpointOnlineRatioPercent);
      aOutput << '}';
   }
   aOutput << "],\"links\":[";
   for (std::size_t index = 0; index < aSnapshot.links.size(); ++index)
   {
      const nrm::LinkSnapshot& link = aSnapshot.links[index];
      if (index != 0) aOutput << ',';
      aOutput << "{\"id\":\"" << EscapeJson(link.linkId)
              << "\",\"source\":\"" << EscapeJson(link.sourceEndpointId)
              << "\",\"destination\":\"" << EscapeJson(link.destinationEndpointId)
              << "\",\"network_type\":\"" << nrm::ToString(link.networkType)
              << "\",\"state\":\"" << nrm::ToString(link.state)
              << "\",\"distance_m\":";
      WriteMetric(aOutput, link.distanceM);
      aOutput << ",\"bandwidth_bps\":";
      WriteMetric(aOutput, link.bandwidthBps);
      aOutput << ",\"rssi_dbm\":";
      WriteMetric(aOutput, link.rssiDbm);
      aOutput << ",\"snr_db\":";
      WriteMetric(aOutput, link.snrDb);
      aOutput << ",\"ber\":";
      WriteMetric(aOutput, link.ber);
      aOutput << ",\"serviceAvailabilityPercent\":";
      WriteMetric(aOutput, link.serviceAvailabilityPercent);
      aOutput << ",\"establishmentAttempts\":" << link.establishmentAttempts
              << ",\"establishmentSuccessRatioPercent\":";
      WriteMetric(aOutput, link.establishmentSuccessRatioPercent);
      aOutput << ",\"averageEstablishmentDelayMs\":";
      WriteMetric(aOutput, link.averageEstablishmentDelayMs);
      aOutput << ",\"windows\":";
      WriteWindows(aOutput, link.windows);
      aOutput << '}';
   }
   aOutput << "]}\n";
}
} // namespace

WkNrm::SnapshotReporter::SnapshotReporter(const std::string& aOutputDirectory,
                                          const std::string& aConfigVersion,
                                          std::size_t aMaximumQueueSize,
                                          bool aAutoStart)
   : mOutputDirectory(aOutputDirectory)
   , mConfigVersion(aConfigVersion)
   , mMaximumQueueSize(aMaximumQueueSize > 0 ? aMaximumQueueSize : 1)
   , mStartTimeUtc(UtcTimestamp(false))
{
   mStatus.runId = GenerateRunId();
   mStatus.runDirectory = mOutputDirectory + "/" + mStatus.runId;
   if (!MakeDirectories(mOutputDirectory) || !MakeDirectories(mStatus.runDirectory))
   {
      mStatus.healthy = false;
      mStatus.lastError = "REPORT_OUTPUT_DIRECTORY_CREATE_FAILED";
      ++mStatus.writeErrorCount;
   }
   else
   {
      WriteManifest(false);
   }
   if (aAutoStart)
   {
      Start();
   }
}

WkNrm::SnapshotReporter::~SnapshotReporter()
{
   Start();
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

void WkNrm::SnapshotReporter::Start()
{
   std::lock_guard<std::mutex> lock(mMutex);
   if (mStarted)
   {
      return;
   }
   mStarted = true;
   mStatus.started = true;
   mThread = std::thread(&SnapshotReporter::Run, this);
}

void WkNrm::SnapshotReporter::Enqueue(const nrm::ResourceSnapshot& aSnapshot)
{
   {
      std::lock_guard<std::mutex> lock(mMutex);
      if (mQueue.size() >= mMaximumQueueSize)
      {
         mQueue.pop_front();
         ++mStatus.droppedSnapshotCount;
      }
      mQueue.push_back(aSnapshot);
   }
   mCondition.notify_one();
}

void WkNrm::SnapshotReporter::EnqueueAssessment(const nrm::AssessmentResult& aResult)
{
   {
      std::lock_guard<std::mutex> lock(mMutex);
      if (mAssessmentQueue.size() >= mMaximumQueueSize)
      {
         mAssessmentQueue.pop_front();
         ++mStatus.droppedAssessmentCount;
      }
      mAssessmentQueue.push_back(aResult);
   }
   mCondition.notify_one();
}

WkNrm::ReporterStatus WkNrm::SnapshotReporter::GetStatus() const
{
   std::lock_guard<std::mutex> lock(mMutex);
   return mStatus;
}

std::string WkNrm::SnapshotReporter::GetRunDirectory() const
{
   std::lock_guard<std::mutex> lock(mMutex);
   return mStatus.runDirectory;
}

void WkNrm::SnapshotReporter::RecordError(const std::string& aComponent,
                                          nrm::MetricReason aReason,
                                          const std::string& aField)
{
   std::string runDirectory;
   {
      std::lock_guard<std::mutex> lock(mMutex);
      mStatus.healthy = false;
      ++mStatus.writeErrorCount;
      mStatus.lastError = nrm::ToString(aReason);
      runDirectory = mStatus.runDirectory;
   }
   std::ofstream errorOutput(runDirectory + "/error.log", std::ios::out | std::ios::app);
   if (errorOutput)
   {
      errorOutput << "{\"time\":\"" << UtcTimestamp(false)
                  << "\",\"component\":\"" << EscapeJson(aComponent)
                  << "\",\"reasonCode\":\"" << nrm::ToString(aReason)
                  << "\",\"field\":\"" << EscapeJson(aField) << "\"}\n";
   }
}

void WkNrm::SnapshotReporter::WriteManifest(bool aComplete)
{
   const ReporterStatus status = GetStatus();
   std::ofstream output(status.runDirectory + "/manifest.json",
                        std::ios::out | std::ios::trunc);
   if (!output)
   {
      RecordError("SnapshotReporter", nrm::MetricReason::cOUTPUT_OPEN_FAILED,
                  "manifest.json");
      return;
   }
   output << "{\"schemaVersion\":\"nrm.run_manifest.v1\",\"runId\":\""
          << EscapeJson(status.runId) << "\",\"softwareVersion\":\""
          << nrm::cVERSION << "\",\"configVersion\":\""
          << EscapeJson(mConfigVersion) << "\",\"startTime\":\""
          << EscapeJson(mStartTimeUtc) << "\",\"complete\":"
          << (aComplete ? "true" : "false")
          << ",\"healthy\":" << (status.healthy ? "true" : "false")
          << ",\"droppedSnapshotCount\":" << status.droppedSnapshotCount
          << ",\"droppedAssessmentCount\":" << status.droppedAssessmentCount
          << ",\"writeErrorCount\":" << status.writeErrorCount
          << ",\"files\":[\"resource_snapshots.jsonl\",\"network_summary.csv\","
             "\"assessment_results.jsonl\",\"error.log\"]}\n";
   output.flush();
   if (!output)
   {
      RecordError("SnapshotReporter", nrm::MetricReason::cOUTPUT_WRITE_FAILED,
                  "manifest.json");
   }
}

void WkNrm::SnapshotReporter::Run()
{
   const ReporterStatus initialStatus = GetStatus();
   std::ofstream jsonOutput(initialStatus.runDirectory + "/resource_snapshots.jsonl",
                            std::ios::out | std::ios::trunc);
   std::ofstream csvOutput(initialStatus.runDirectory + "/network_summary.csv",
                           std::ios::out | std::ios::trunc);
   std::ofstream assessmentOutput(initialStatus.runDirectory + "/assessment_results.jsonl",
                                  std::ios::out | std::ios::trunc);
   bool jsonHealthy = jsonOutput.is_open();
   bool csvHealthy = csvOutput.is_open();
   bool assessmentHealthy = assessmentOutput.is_open();
   if (!jsonHealthy)
      RecordError("SnapshotReporter", nrm::MetricReason::cOUTPUT_OPEN_FAILED,
                  "resource_snapshots.jsonl");
   if (!csvHealthy)
      RecordError("SnapshotReporter", nrm::MetricReason::cOUTPUT_OPEN_FAILED,
                  "network_summary.csv");
   if (!assessmentHealthy)
      RecordError("SnapshotReporter", nrm::MetricReason::cOUTPUT_OPEN_FAILED,
                  "assessment_results.jsonl");

   if (csvHealthy)
   {
      csvOutput << "run_id,config_version,snapshot_version,sim_time,network_type,"
                   "network_name,members,online,active_links,window_s,offered_load_bps,"
                   "delivered_throughput_bps,delivery_ratio_percent,online_ratio_percent,"
                   "average_queue_delay_ms,p50_queue_delay_ms,p95_queue_delay_ms,"
                   "average_transport_delay_ms,p50_transport_delay_ms,p95_transport_delay_ms\n";
   }

   while (true)
   {
      nrm::ResourceSnapshot snapshot;
      nrm::AssessmentResult assessment;
      bool hasSnapshot = false;
      bool hasAssessment = false;
      {
         std::unique_lock<std::mutex> lock(mMutex);
         mCondition.wait(lock, [this]
         {
            return mStopping || !mQueue.empty() || !mAssessmentQueue.empty();
         });
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
         else if (!mQueue.empty())
         {
            snapshot = mQueue.front();
            mQueue.pop_front();
            hasSnapshot = true;
         }
      }

      if (hasAssessment && assessmentHealthy)
      {
         WriteAssessment(assessmentOutput, assessment, initialStatus.runId, mConfigVersion);
         assessmentOutput.flush();
         if (!assessmentOutput)
         {
            assessmentHealthy = false;
            RecordError("SnapshotReporter", nrm::MetricReason::cOUTPUT_WRITE_FAILED,
                        "assessment_results.jsonl");
         }
      }
      if (hasSnapshot)
      {
         if (jsonHealthy)
         {
            WriteJsonSnapshot(jsonOutput, snapshot, initialStatus.runId, mConfigVersion);
            jsonOutput.flush();
            if (!jsonOutput)
            {
               jsonHealthy = false;
               RecordError("SnapshotReporter", nrm::MetricReason::cOUTPUT_WRITE_FAILED,
                           "resource_snapshots.jsonl");
            }
         }
         if (csvHealthy)
         {
            const std::string configVersion =
               snapshot.configVersion.empty() ? mConfigVersion : snapshot.configVersion;
            for (const nrm::NetworkSnapshot& network : snapshot.networks)
            {
               for (const nrm::WindowMetrics& window : network.windows)
               {
                  csvOutput << initialStatus.runId << ',' << configVersion << ','
                            << snapshot.snapshotVersion << ',' << std::fixed
                            << std::setprecision(3) << snapshot.simTime << ','
                            << nrm::ToString(network.networkType) << ",\""
                            << network.networkName << "\"," << network.endpointCount << ','
                            << network.onlineCount << ',' << network.activeLinks << ','
                            << window.windowS << ',';
                  const nrm::MetricValue<double>* metrics[] = {
                     &window.offeredLoadBps, &window.deliveredThroughputBps,
                     &window.deliveryRatioPercent, &window.onlineRatioPercent,
                     &window.averageQueueDelayMs, &window.p50QueueDelayMs,
                     &window.p95QueueDelayMs, &window.averageTransportDelayMs,
                     &window.p50TransportDelayMs, &window.p95TransportDelayMs};
                  for (std::size_t index = 0; index < 10; ++index)
                  {
                     if (metrics[index]->valid) csvOutput << metrics[index]->value;
                     if (index != 9) csvOutput << ',';
                  }
                  csvOutput << '\n';
               }
            }
            csvOutput.flush();
            if (!csvOutput)
            {
               csvHealthy = false;
               RecordError("SnapshotReporter", nrm::MetricReason::cOUTPUT_WRITE_FAILED,
                           "network_summary.csv");
            }
         }
      }
   }
   WriteManifest(true);
}
