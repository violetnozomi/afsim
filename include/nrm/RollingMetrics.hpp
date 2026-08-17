/**
 * @file RollingMetrics.hpp
 * @brief Bounded, simulation-time sliding-window communication metrics.
 */

#ifndef NRM_ROLLING_METRICS_HPP
#define NRM_ROLLING_METRICS_HPP

#include <algorithm>
#include <cstdint>
#include <deque>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
class RollingMetrics
{
public:
   static constexpr double cMAX_WINDOW_S = 60.0;

   void Clear() { mSamples.clear(); }

   void SetTransmitObservationAvailable(bool aAvailable)
   {
      mTransmitObservationAvailable = aAvailable;
   }

   void RecordTransmit(double aSimTime, std::uint64_t aBits, double aQueueDelayMs = -1.0)
   {
      Add({aSimTime, Kind::cTRANSMIT, aBits, aQueueDelayMs});
   }

   void RecordReceive(double aSimTime, std::uint64_t aBits, double aTransportDelayMs = -1.0)
   {
      Add({aSimTime, Kind::cRECEIVE, aBits, aTransportDelayMs});
   }

   void RecordDiscard(double aSimTime) { Add({aSimTime, Kind::cDISCARD, 0, 0.0}); }

   void RecordRoutingFailure(double aSimTime) { Add({aSimTime, Kind::cROUTING_FAILURE, 0, 0.0}); }

   void RecordOnlineRatio(double aSimTime, double aRatio)
   {
      Add({aSimTime, Kind::cONLINE_RATIO, 0, std::max(0.0, std::min(1.0, aRatio))});
   }

   WindowMetrics Snapshot(double aSimTime, double aWindowS, double aCapacityBps = -1.0) const
   {
      WindowMetrics output;
      output.windowS = aWindowS;
      if (aWindowS <= 0.0)
      {
         return output;
      }

      double      queueDelayTotalMs     = 0.0;
      double      transportDelayTotalMs = 0.0;
      double      onlineRatioTotal      = 0.0;
      std::uint64_t deliveredBits       = 0;
      std::size_t queueDelayCount       = 0;
      std::size_t transportDelayCount   = 0;
      std::size_t onlineRatioCount      = 0;
      const double windowStart          = aSimTime - aWindowS;

      for (const Sample& sample : mSamples)
      {
         if (sample.simTime <= windowStart || sample.simTime > aSimTime)
         {
            continue;
         }
         switch (sample.kind)
         {
         case Kind::cTRANSMIT:
            ++output.messages.transmitted;
            output.transmittedBits += sample.bits;
            if (sample.value >= 0.0)
            {
               queueDelayTotalMs += sample.value;
               ++queueDelayCount;
            }
            break;
         case Kind::cRECEIVE:
            ++output.messages.received;
            deliveredBits += sample.bits;
            if (sample.value >= 0.0)
            {
               transportDelayTotalMs += sample.value;
               ++transportDelayCount;
            }
            break;
         case Kind::cDISCARD:
            ++output.messages.discarded;
            break;
         case Kind::cROUTING_FAILURE:
            ++output.messages.routingFailed;
            break;
         case Kind::cONLINE_RATIO:
            onlineRatioTotal += sample.value;
            ++onlineRatioCount;
            break;
         }
      }

      output.deliveredBits = deliveredBits;
      if (mTransmitObservationAvailable)
      {
         SetDerived(output.offeredLoadBps,
                    static_cast<double>(output.transmittedBits) / aWindowS,
                    "bit/s",
                    aSimTime,
                    aWindowS);
      }
      else
      {
         MarkInvalid(output.offeredLoadBps, MetricReason::cMISSING_TRANSMIT_DENOMINATOR);
      }
      SetDerived(output.deliveredThroughputBps,
                 static_cast<double>(output.deliveredBits) / aWindowS,
                 "bit/s",
                 aSimTime,
                 aWindowS);
      output.throughputBps = output.deliveredThroughputBps;
      if (mTransmitObservationAvailable && output.messages.transmitted > 0)
      {
         const double ratio =
            std::min(1.0, static_cast<double>(output.messages.received) /
                           static_cast<double>(output.messages.transmitted));
         SetDerived(output.deliveryRatioPercent, ratio * 100.0, "percent", aSimTime, aWindowS);
         output.pdrPercent = output.deliveryRatioPercent;
      }
      else
      {
         const MetricReason reason = mTransmitObservationAvailable
                                        ? MetricReason::cNO_SAMPLES
                                        : MetricReason::cMISSING_TRANSMIT_DENOMINATOR;
         MarkInvalid(output.deliveryRatioPercent, reason);
         MarkInvalid(output.pdrPercent, reason);
      }
      if (queueDelayCount > 0)
      {
         SetDerived(
            output.averageQueueDelayMs,
            queueDelayTotalMs / static_cast<double>(queueDelayCount),
            "ms", aSimTime, aWindowS);
      }
      if (transportDelayCount > 0)
      {
         SetDerived(output.averageTransportDelayMs,
                    transportDelayTotalMs / static_cast<double>(transportDelayCount),
                    "ms",
                    aSimTime,
                    aWindowS);
      }
      if (onlineRatioCount > 0)
      {
         SetDerived(output.onlineRatioPercent,
                    100.0 * onlineRatioTotal / static_cast<double>(onlineRatioCount),
                    "percent",
                    aSimTime,
                    aWindowS);
      }
      if (aCapacityBps > 0.0)
      {
         SetDerived(output.utilizationPercent,
                    std::min(100.0, 100.0 * output.deliveredThroughputBps.value / aCapacityBps),
                    "percent",
                    aSimTime,
                    aWindowS);
      }
      return output;
   }

private:
   enum class Kind
   {
      cTRANSMIT,
      cRECEIVE,
      cDISCARD,
      cROUTING_FAILURE,
      cONLINE_RATIO
   };

   struct Sample
   {
      double        simTime = 0.0;
      Kind          kind    = Kind::cTRANSMIT;
      std::uint64_t bits    = 0;
      double        value   = 0.0;
   };

   static void SetDerived(MetricValue<double>& aMetric,
                          double               aValue,
                          const char*          aUnit,
                          double               aSimTime,
                          double               aWindowS)
   {
      aMetric.value      = aValue;
      aMetric.unit       = aUnit;
      aMetric.valid      = true;
      aMetric.origin     = DataOrigin::cDERIVED;
      aMetric.confidence = Confidence::cHIGH;
      aMetric.sampleTime = aSimTime;
      aMetric.window     = aWindowS;
      aMetric.reason     = MetricReason::cNONE;
   }

   static void MarkInvalid(MetricValue<double>& aMetric, MetricReason aReason)
   {
      aMetric.valid  = false;
      aMetric.reason = aReason;
   }

   void Add(const Sample& aSample)
   {
      mSamples.push_back(aSample);
      const double oldestAllowed = aSample.simTime - cMAX_WINDOW_S;
      while (!mSamples.empty() && mSamples.front().simTime <= oldestAllowed)
      {
         mSamples.pop_front();
      }
   }

   std::deque<Sample> mSamples;
   bool mTransmitObservationAvailable = true;
};
} // namespace nrm

#endif
