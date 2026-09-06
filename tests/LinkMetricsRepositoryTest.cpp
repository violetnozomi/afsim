/**
 * @file LinkMetricsRepositoryTest.cpp
 * @brief Regression tests for per-hop transmit/receive metric correlation.
 */

#include <cassert>
#include <cmath>

#include "nrm/LinkMetricsRepository.hpp"

namespace
{
bool NearlyEqual(double aLeft, double aRight, double aTolerance = 1.0e-9)
{
   return std::fabs(aLeft - aRight) <= aTolerance;
}
}

int main()
{
   nrm::LinkMetricsRepository repository;

   // Two delivery attempts on A->B, only one successful receive.  A physical
   // delivery attempt is the denominator; a final receive is the numerator.
   repository.RecordDeliveryAttempt("A->B", 41U, 1.000, 8000U);
   repository.RecordReceive("A->B", 41U, 1.125, 8000U);
   repository.RecordDeliveryAttempt("A->B", 42U, 1.500, 4000U);

   const nrm::RollingMetrics* metrics = repository.Find("A->B");
   assert(metrics != nullptr);
   const nrm::WindowMetrics window = metrics->Snapshot(2.0, 2.0, 64000.0);
   assert(window.messages.transmitted == 2U);
   assert(window.messages.received == 1U);
   assert(window.offeredLoadBps.valid);
   assert(window.deliveredThroughputBps.valid);
   assert(window.pdrPercent.valid);
   assert(window.averageTransportDelayMs.valid);
   assert(window.utilizationPercent.valid);
   assert(NearlyEqual(window.offeredLoadBps.value, 6000.0));
   assert(NearlyEqual(window.deliveredThroughputBps.value, 4000.0));
   assert(NearlyEqual(window.pdrPercent.value, 50.0));
   assert(NearlyEqual(window.averageTransportDelayMs.value, 125.0));
   assert(NearlyEqual(window.utilizationPercent.value, 6.25));

   // A message with the same serial number on another hop must not consume
   // A->B's correlation or contaminate its statistics.
   repository.RecordDeliveryAttempt("B->C", 41U, 2.100, 8000U);
   repository.RecordReceive("B->C", 41U, 2.300, 8000U);
   const nrm::RollingMetrics* secondHop = repository.Find("B->C");
   assert(secondHop != nullptr);
   const nrm::WindowMetrics secondWindow = secondHop->Snapshot(2.5, 1.0, 64000.0);
   assert(secondWindow.messages.transmitted == 1U);
   assert(secondWindow.messages.received == 1U);
   assert(secondWindow.pdrPercent.valid);
   assert(NearlyEqual(secondWindow.pdrPercent.value, 100.0));
   assert(NearlyEqual(secondWindow.averageTransportDelayMs.value, 200.0));

   repository.Prune(63.0);
   repository.RecordReceive("A->B", 42U, 63.1, 4000U);
   const nrm::WindowMetrics expired = repository.Find("A->B")->Snapshot(63.2, 1.0, 64000.0);
   assert(expired.messages.received == 1U);
   assert(!expired.averageTransportDelayMs.valid);

   repository.Clear();
   assert(repository.Find("A->B") == nullptr);
   return 0;
}
