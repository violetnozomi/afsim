#include "nrm/MessageLifecycleTracker.hpp"

#include <cmath>
#include <iostream>

#define CHECK(condition) \
   do { if (!(condition)) { std::cerr << "CHECK failed at line " << __LINE__ << "\n"; return 1; } } while (false)

int main()
{
   nrm::MessageLifecycleTracker tracker(60.0, 64);
   for (std::uint64_t id = 1; id <= 10; ++id)
   {
      nrm::MessageLifecycleRecord record;
      record.messageId = id;
      record.networkId = "network-A";
      record.sourceEndpointId = "source";
      record.destinationEndpointId = "destination";
      record.businessType = "command";
      record.bits = 100;
      record.queuedTime = static_cast<double>(id);
      CHECK(tracker.RecordQueued(record));
      CHECK(tracker.RecordTransmitted(id, static_cast<double>(id) + 0.1));
   }
   for (std::uint64_t id = 1; id <= 8; ++id)
   {
      CHECK(tracker.RecordTerminal(id, nrm::MessageTerminalState::cDELIVERED,
                                   static_cast<double>(id) + 0.2, "destination"));
   }
   CHECK(tracker.RecordTerminal(9, nrm::MessageTerminalState::cDISCARDED, 9.2));
   CHECK(tracker.RecordTerminal(10, nrm::MessageTerminalState::cROUTING_FAILED, 10.2));
   CHECK(!tracker.RecordTerminal(1, nrm::MessageTerminalState::cDELIVERED, 1.3));
   CHECK(tracker.DuplicateEventCount() == 1);

   const nrm::WindowMetrics metrics = tracker.Snapshot(20.0, 20.0, "network-A");
   CHECK(metrics.messages.transmitted == 10);
   CHECK(metrics.messages.received == 8);
   CHECK(metrics.messages.discarded == 1);
   CHECK(metrics.messages.routingFailed == 1);
   CHECK(metrics.deliveryRatioPercent.valid);
   CHECK(std::abs(metrics.deliveryRatioPercent.value - 80.0) < 1.0e-9);
   CHECK(metrics.offeredLoadBps.value == 50.0);
   CHECK(metrics.deliveredThroughputBps.value == 40.0);
   CHECK(metrics.throughputBps.value == metrics.deliveredThroughputBps.value);
   CHECK(metrics.p50QueueDelayMs.valid);
   CHECK(metrics.p95TransportDelayMs.valid);

   nrm::MessageLifecycleTracker uncorrelated;
   CHECK(!uncorrelated.RecordTerminal(
      99, nrm::MessageTerminalState::cDELIVERED, 1.0));
   CHECK(uncorrelated.UncorrelatedTerminalCount() == 1);
   CHECK(!uncorrelated.Snapshot(2.0, 1.0).deliveryRatioPercent.valid);

   nrm::MessageLifecycleTracker late;
   CHECK(late.RecordTransmitted(1, 5.0, "network-A", "source", 100));
   CHECK(late.RecordTerminal(1, nrm::MessageTerminalState::cDELIVERED, 12.0));
   CHECK(late.Snapshot(15.0, 20.0).deliveryRatioPercent.value == 100.0);
   CHECK(!late.Snapshot(15.0, 5.0).deliveryRatioPercent.valid);

   nrm::MessageLifecycleTracker expiry(5.0, 2);
   CHECK(expiry.RecordTransmitted(1, 0.0));
   expiry.Prune(5.0);
   CHECK(expiry.ExpiredCount() == 1);
   CHECK(expiry.Cumulative().discarded == 1);
   CHECK(expiry.RecordTransmitted(2, 6.0));
   CHECK(expiry.RecordTransmitted(3, 7.0));
   CHECK(expiry.Size() <= 2);
   CHECK(expiry.Cumulative().discarded == 1);

   nrm::MessageLifecycleTracker bounded(60.0, 1);
   CHECK(bounded.RecordTransmitted(1, 1.0));
   CHECK(bounded.RecordTransmitted(2, 2.0));
   CHECK(bounded.Size() == 1);
   CHECK(bounded.ExpiredCount() == 1);
   CHECK(bounded.Cumulative().discarded == 1);

   return 0;
}
