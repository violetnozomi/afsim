#ifndef NRM_MESSAGE_LIFECYCLE_TRACKER_HPP
#define NRM_MESSAGE_LIFECYCLE_TRACKER_HPP

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "nrm/MessageLifecycleTypes.hpp"
#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
class MessageLifecycleTracker
{
public:
   explicit MessageLifecycleTracker(double aTimeoutS = 60.0, std::size_t aMaximumRecords = 4096)
      : mTimeoutS(aTimeoutS > 0.0 ? aTimeoutS : 60.0)
      , mMaximumRecords(aMaximumRecords > 0 ? aMaximumRecords : 1)
   {
   }

   void Clear()
   {
      mRecords.clear();
      mArchived.clear();
      mDuplicateEventCount = 0;
      mOutOfOrderEventCount = 0;
      mUncorrelatedTerminalCount = 0;
      mExpiredCount = 0;
   }

   bool RecordQueued(const MessageLifecycleRecord& aRecord)
   {
      if (mRecords.count(aRecord.messageId) != 0)
      {
         ++mDuplicateEventCount;
         return false;
      }
      MessageLifecycleRecord record = aRecord;
      record.transmittedTime = -1.0;
      record.terminalTime = -1.0;
      record.terminalState = MessageTerminalState::cPENDING;
      mRecords[record.messageId] = record;
      EnforceBound();
      return true;
   }

   bool RecordTransmitted(std::uint64_t aMessageId,
                          double aSimTime,
                          const std::string& aNetworkId = std::string(),
                          const std::string& aSourceEndpointId = std::string(),
                          std::uint64_t aBits = 0)
   {
      MessageLifecycleRecord& record = mRecords[aMessageId];
      record.messageId = aMessageId;
      if (record.transmittedTime >= 0.0)
      {
         ++mDuplicateEventCount;
         return false;
      }
      if (record.terminalState != MessageTerminalState::cPENDING)
      {
         ++mOutOfOrderEventCount;
         return false;
      }
      record.transmittedTime = aSimTime;
      if (!aNetworkId.empty())
      {
         record.networkId = aNetworkId;
      }
      if (!aSourceEndpointId.empty())
      {
         record.sourceEndpointId = aSourceEndpointId;
      }
      if (aBits > 0)
      {
         record.bits = aBits;
      }
      EnforceBound();
      return true;
   }

   bool RecordTerminal(std::uint64_t aMessageId,
                       MessageTerminalState aState,
                       double aSimTime,
                       const std::string& aDestinationEndpointId = std::string())
   {
      const auto it = mRecords.find(aMessageId);
      if (it == mRecords.end() || it->second.transmittedTime < 0.0)
      {
         ++mUncorrelatedTerminalCount;
         return false;
      }
      MessageLifecycleRecord& record = it->second;
      if (record.terminalState != MessageTerminalState::cPENDING)
      {
         ++mDuplicateEventCount;
         return false;
      }
      if (aSimTime < record.transmittedTime)
      {
         ++mOutOfOrderEventCount;
         return false;
      }
      record.terminalState = aState;
      record.terminalTime = aSimTime;
      if (!aDestinationEndpointId.empty())
      {
         record.destinationEndpointId = aDestinationEndpointId;
      }
      return true;
   }

   void Prune(double aSimTime)
   {
      for (auto& entry : mRecords)
      {
         MessageLifecycleRecord& record = entry.second;
         const double referenceTime = record.transmittedTime >= 0.0 ? record.transmittedTime : record.queuedTime;
         if (record.terminalState == MessageTerminalState::cPENDING && referenceTime >= 0.0 &&
             aSimTime - referenceTime >= mTimeoutS)
         {
            record.terminalState = MessageTerminalState::cEXPIRED;
            record.terminalTime = aSimTime;
            ++mExpiredCount;
         }
      }
      for (auto it = mRecords.begin(); it != mRecords.end();)
      {
         const MessageLifecycleRecord& record = it->second;
         if (record.terminalTime >= 0.0 && aSimTime - record.terminalTime > mTimeoutS)
         {
            Archive(record);
            it = mRecords.erase(it);
         }
         else
         {
            ++it;
         }
      }
   }

   WindowMetrics Snapshot(double aSimTime,
                          double aWindowS,
                          const std::string& aNetworkId = std::string()) const
   {
      WindowMetrics output;
      output.windowS = aWindowS;
      if (aWindowS <= 0.0)
      {
         return output;
      }
      const double start = aSimTime - aWindowS;
      std::vector<double> queueDelays;
      std::vector<double> transportDelays;
      for (const auto& entry : mRecords)
      {
         const MessageLifecycleRecord& record = entry.second;
         if ((!aNetworkId.empty() && record.networkId != aNetworkId) ||
             record.transmittedTime <= start || record.transmittedTime > aSimTime)
         {
            continue;
         }
         ++output.messages.transmitted;
         output.transmittedBits += record.bits;
         if (record.queuedTime >= 0.0 && record.transmittedTime >= record.queuedTime)
         {
            queueDelays.push_back(1000.0 * (record.transmittedTime - record.queuedTime));
         }
         switch (record.terminalState)
         {
         case MessageTerminalState::cDELIVERED:
            ++output.messages.received;
            output.deliveredBits += record.bits;
            if (record.terminalTime >= record.transmittedTime)
            {
               transportDelays.push_back(1000.0 * (record.terminalTime - record.transmittedTime));
            }
            break;
         case MessageTerminalState::cDISCARDED:
         case MessageTerminalState::cEXPIRED:
            ++output.messages.discarded;
            break;
         case MessageTerminalState::cROUTING_FAILED:
            ++output.messages.routingFailed;
            break;
         case MessageTerminalState::cPENDING:
            break;
         }
      }

      SetDerived(output.offeredLoadBps,
                 static_cast<double>(output.transmittedBits) / aWindowS,
                 "bit/s", aSimTime, aWindowS);
      SetDerived(output.deliveredThroughputBps,
                 static_cast<double>(output.deliveredBits) / aWindowS,
                 "bit/s", aSimTime, aWindowS);
      output.throughputBps = output.deliveredThroughputBps;
      if (output.messages.transmitted > 0)
      {
         SetDerived(output.deliveryRatioPercent,
                    100.0 * output.messages.received / output.messages.transmitted,
                    "percent", aSimTime, aWindowS);
         output.pdrPercent = output.deliveryRatioPercent;
      }
      else
      {
         MarkInvalid(output.deliveryRatioPercent, MetricReason::cNO_SAMPLES);
         MarkInvalid(output.pdrPercent, MetricReason::cNO_SAMPLES);
      }
      PopulateDelays(queueDelays,
                     output.averageQueueDelayMs,
                     output.p50QueueDelayMs,
                     output.p95QueueDelayMs,
                     aSimTime,
                     aWindowS);
      PopulateDelays(transportDelays,
                     output.averageTransportDelayMs,
                     output.p50TransportDelayMs,
                     output.p95TransportDelayMs,
                     aSimTime,
                     aWindowS);
      return output;
   }

   MessageStatistics Cumulative(const std::string& aNetworkId = std::string()) const
   {
      MessageStatistics output;
      if (aNetworkId.empty())
      {
         for (const auto& archived : mArchived)
         {
            Accumulate(output, archived.second);
         }
      }
      else
      {
         const auto archived = mArchived.find(aNetworkId);
         if (archived != mArchived.end())
         {
            Accumulate(output, archived->second);
         }
      }
      for (const auto& entry : mRecords)
      {
         const MessageLifecycleRecord& record = entry.second;
         if (!aNetworkId.empty() && record.networkId != aNetworkId)
         {
            continue;
         }
         if (record.queuedTime >= 0.0)
         {
            ++output.queued;
         }
         if (record.transmittedTime >= 0.0)
         {
            ++output.transmitted;
         }
         if (record.terminalState == MessageTerminalState::cDELIVERED)
         {
            ++output.received;
         }
         else if (record.terminalState == MessageTerminalState::cDISCARDED ||
                  record.terminalState == MessageTerminalState::cEXPIRED)
         {
            ++output.discarded;
         }
         else if (record.terminalState == MessageTerminalState::cROUTING_FAILED)
         {
            ++output.routingFailed;
         }
      }
      return output;
   }

   std::size_t Size() const { return mRecords.size(); }
   std::uint64_t DuplicateEventCount() const { return mDuplicateEventCount; }
   std::uint64_t OutOfOrderEventCount() const { return mOutOfOrderEventCount; }
   std::uint64_t UncorrelatedTerminalCount() const { return mUncorrelatedTerminalCount; }
   std::uint64_t ExpiredCount() const { return mExpiredCount; }

private:
   static void SetDerived(MetricValue<double>& aMetric,
                          double aValue,
                          const char* aUnit,
                          double aSimTime,
                          double aWindowS)
   {
      aMetric.value = aValue;
      aMetric.unit = aUnit;
      aMetric.valid = true;
      aMetric.origin = DataOrigin::cDERIVED;
      aMetric.confidence = Confidence::cHIGH;
      aMetric.sampleTime = aSimTime;
      aMetric.window = aWindowS;
      aMetric.reason = MetricReason::cNONE;
   }

   static void MarkInvalid(MetricValue<double>& aMetric, MetricReason aReason)
   {
      aMetric.valid = false;
      aMetric.reason = aReason;
   }

   static double Percentile(const std::vector<double>& aSorted, double aPercentile)
   {
      const std::size_t index = static_cast<std::size_t>(
         std::ceil(aPercentile * static_cast<double>(aSorted.size()))) - 1;
      return aSorted[std::min(index, aSorted.size() - 1)];
   }

   static void PopulateDelays(std::vector<double> aValues,
                              MetricValue<double>& aAverage,
                              MetricValue<double>& aP50,
                              MetricValue<double>& aP95,
                              double aSimTime,
                              double aWindowS)
   {
      if (aValues.empty())
      {
         return;
      }
      std::sort(aValues.begin(), aValues.end());
      double total = 0.0;
      for (double value : aValues)
      {
         total += value;
      }
      SetDerived(aAverage, total / aValues.size(), "ms", aSimTime, aWindowS);
      SetDerived(aP50, Percentile(aValues, 0.50), "ms", aSimTime, aWindowS);
      SetDerived(aP95, Percentile(aValues, 0.95), "ms", aSimTime, aWindowS);
   }

   void EnforceBound()
   {
      while (mRecords.size() > mMaximumRecords)
      {
         auto oldest = mRecords.begin();
         for (auto it = mRecords.begin(); it != mRecords.end(); ++it)
         {
            const double candidateTime = it->second.transmittedTime >= 0.0
                                            ? it->second.transmittedTime : it->second.queuedTime;
            const double oldestTime = oldest->second.transmittedTime >= 0.0
                                         ? oldest->second.transmittedTime : oldest->second.queuedTime;
            if (candidateTime < oldestTime)
            {
               oldest = it;
            }
         }
         if (oldest->second.terminalState == MessageTerminalState::cPENDING)
         {
            ++mExpiredCount;
            oldest->second.terminalState = MessageTerminalState::cEXPIRED;
            oldest->second.terminalTime = oldest->second.transmittedTime >= 0.0
                                             ? oldest->second.transmittedTime
                                             : oldest->second.queuedTime;
         }
         Archive(oldest->second);
         mRecords.erase(oldest);
      }
   }

   static void Accumulate(MessageStatistics& aTarget,
                          const MessageStatistics& aSource)
   {
      aTarget.queued += aSource.queued;
      aTarget.transmitted += aSource.transmitted;
      aTarget.received += aSource.received;
      aTarget.discarded += aSource.discarded;
      aTarget.routingFailed += aSource.routingFailed;
   }

   void Archive(const MessageLifecycleRecord& aRecord)
   {
      MessageStatistics& statistics = mArchived[aRecord.networkId];
      if (aRecord.queuedTime >= 0.0) ++statistics.queued;
      if (aRecord.transmittedTime >= 0.0) ++statistics.transmitted;
      if (aRecord.terminalState == MessageTerminalState::cDELIVERED)
         ++statistics.received;
      else if (aRecord.terminalState == MessageTerminalState::cDISCARDED ||
               aRecord.terminalState == MessageTerminalState::cEXPIRED)
         ++statistics.discarded;
      else if (aRecord.terminalState == MessageTerminalState::cROUTING_FAILED)
         ++statistics.routingFailed;
   }

   std::map<std::uint64_t, MessageLifecycleRecord> mRecords;
   std::map<std::string, MessageStatistics> mArchived;
   double mTimeoutS;
   std::size_t mMaximumRecords;
   std::uint64_t mDuplicateEventCount = 0;
   std::uint64_t mOutOfOrderEventCount = 0;
   std::uint64_t mUncorrelatedTerminalCount = 0;
   std::uint64_t mExpiredCount = 0;
};
} // namespace nrm

#endif
