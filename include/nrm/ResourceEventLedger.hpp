#ifndef NRM_RESOURCE_EVENT_LEDGER_HPP
#define NRM_RESOURCE_EVENT_LEDGER_HPP

#include <algorithm>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "nrm/ResourceEventTypes.hpp"

namespace nrm
{
class ResourceEventLedger
{
public:
   explicit ResourceEventLedger(std::size_t aMaximumEvents = 8192)
      : mMaximumEvents(aMaximumEvents > 0 ? aMaximumEvents : 1)
   {
   }

   void Clear()
   {
      mEvents.clear();
      mEventKeys.clear();
      mDuplicateEventCount = 0;
      mOutOfOrderEventCount = 0;
      mDroppedEventCount = 0;
   }

   bool Record(const ResourceEvent& aEvent)
   {
      const std::string key = EventKey(aEvent);
      if (!mEventKeys.insert(key).second)
      {
         ++mDuplicateEventCount;
         return false;
      }
      if (!mEvents.empty() && aEvent.simTime < mEvents.back().simTime)
      {
         ++mOutOfOrderEventCount;
      }
      mEvents.push_back(aEvent);
      std::stable_sort(mEvents.begin(), mEvents.end(), EventLess);
      while (mEvents.size() > mMaximumEvents)
      {
         mEventKeys.erase(EventKey(mEvents.front()));
         mEvents.erase(mEvents.begin());
         ++mDroppedEventCount;
      }
      return true;
   }

   ResourceStateMetrics EndpointMetrics(const std::string& aEndpointId,
                                        double aSimTime,
                                        double aWindowS) const
   {
      const std::vector<ResourceEvent> events =
         StateEvents(ResourceEventKind::cENDPOINT_STATE, aEndpointId, aSimTime);
      ResourceStateMetrics output;
      PopulateStateMetrics(events, aSimTime, aWindowS,
                           output.currentOfflineDurationS,
                           output.windowOfflineDurationS,
                           output.endpointOnlineRatioPercent);
      output.serviceAvailabilityPercent.reason = MetricReason::cMISSING_STATE_HISTORY;
      PopulateEstablishment(std::string(), aSimTime, aWindowS, output);
      return output;
   }

   ResourceStateMetrics LinkMetrics(const std::string& aLinkId,
                                    double aSimTime,
                                    double aWindowS) const
   {
      const std::vector<ResourceEvent> events =
         StateEvents(ResourceEventKind::cLINK_STATE, aLinkId, aSimTime);
      ResourceStateMetrics output;
      PopulateStateMetrics(events, aSimTime, aWindowS,
                           output.currentOfflineDurationS,
                           output.windowOfflineDurationS,
                           output.serviceAvailabilityPercent);
      output.endpointOnlineRatioPercent.reason = MetricReason::cMISSING_STATE_HISTORY;
      PopulateEstablishment(aLinkId, aSimTime, aWindowS, output);
      return output;
   }

   std::size_t Size() const { return mEvents.size(); }
   std::uint64_t DuplicateEventCount() const { return mDuplicateEventCount; }
   std::uint64_t OutOfOrderEventCount() const { return mOutOfOrderEventCount; }
   std::uint64_t DroppedEventCount() const { return mDroppedEventCount; }
   const std::vector<ResourceEvent>& Events() const { return mEvents; }

private:
   static bool EventLess(const ResourceEvent& aLeft, const ResourceEvent& aRight)
   {
      if (aLeft.simTime != aRight.simTime)
      {
         return aLeft.simTime < aRight.simTime;
      }
      return EventKey(aLeft) < EventKey(aRight);
   }

   static std::string EventKey(const ResourceEvent& aEvent)
   {
      if (!aEvent.eventId.empty())
      {
         return "id:" + aEvent.eventId;
      }
      return std::to_string(static_cast<int>(aEvent.kind)) + "|" +
             std::to_string(aEvent.simTime) + "|" + aEvent.networkId + "|" +
             aEvent.linkId + "|" + aEvent.endpointId + "|" + aEvent.correlationId + "|" +
             std::to_string(static_cast<int>(aEvent.currentState));
   }

   std::vector<ResourceEvent> StateEvents(ResourceEventKind aKind,
                                          const std::string& aEntityId,
                                          double aSimTime) const
   {
      std::vector<ResourceEvent> output;
      for (const ResourceEvent& event : mEvents)
      {
         const std::string& eventEntity =
            aKind == ResourceEventKind::cENDPOINT_STATE ? event.endpointId : event.linkId;
         if (event.kind == aKind && eventEntity == aEntityId && event.simTime <= aSimTime)
         {
            output.push_back(event);
         }
      }
      return output;
   }

   static bool IsOnline(ResourceState aState)
   {
      return aState == ResourceState::cONLINE;
   }

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

   static void PopulateStateMetrics(const std::vector<ResourceEvent>& aEvents,
                                    double aSimTime,
                                    double aWindowS,
                                    MetricValue<double>& aCurrentOffline,
                                    MetricValue<double>& aWindowOffline,
                                    MetricValue<double>& aOnlineRatio)
   {
      aCurrentOffline.reason = MetricReason::cMISSING_STATE_HISTORY;
      aWindowOffline.reason = MetricReason::cMISSING_STATE_HISTORY;
      aOnlineRatio.reason = MetricReason::cMISSING_STATE_HISTORY;
      if (aEvents.empty() || aWindowS <= 0.0)
      {
         return;
      }

      const ResourceEvent& current = aEvents.back();
      double currentOfflineS = 0.0;
      if (!IsOnline(current.currentState))
      {
         currentOfflineS = std::max(0.0, aSimTime - current.simTime);
      }
      SetDerived(aCurrentOffline, currentOfflineS, "s", aSimTime, 0.0);

      const double windowStart = aSimTime - aWindowS;
      ResourceState state = ResourceState::cUNKNOWN;
      double cursor = windowStart;
      for (const ResourceEvent& event : aEvents)
      {
         if (event.simTime <= windowStart)
         {
            state = event.currentState;
            continue;
         }
         break;
      }
      if (state == ResourceState::cUNKNOWN)
      {
         return;
      }

      double onlineDuration = 0.0;
      double offlineDuration = 0.0;
      for (const ResourceEvent& event : aEvents)
      {
         if (event.simTime <= windowStart)
         {
            continue;
         }
         const double end = std::min(aSimTime, event.simTime);
         if (end > cursor)
         {
            if (IsOnline(state))
            {
               onlineDuration += end - cursor;
            }
            else
            {
               offlineDuration += end - cursor;
            }
         }
         cursor = end;
         state = event.currentState;
         if (event.simTime >= aSimTime)
         {
            break;
         }
      }
      if (cursor < aSimTime)
      {
         if (IsOnline(state))
         {
            onlineDuration += aSimTime - cursor;
         }
         else
         {
            offlineDuration += aSimTime - cursor;
         }
      }
      SetDerived(aWindowOffline, offlineDuration, "s", aSimTime, aWindowS);
      SetDerived(aOnlineRatio, 100.0 * onlineDuration / aWindowS, "percent", aSimTime, aWindowS);
   }

   void PopulateEstablishment(const std::string& aLinkId,
                              double aSimTime,
                              double aWindowS,
                              ResourceStateMetrics& aOutput) const
   {
      aOutput.establishmentSuccessRatioPercent.reason =
         MetricReason::cMISSING_ESTABLISHMENT_EVENTS;
      aOutput.averageEstablishmentDelayMs.reason =
         MetricReason::cMISSING_ESTABLISHMENT_EVENTS;
      if (aWindowS <= 0.0)
      {
         return;
      }
      const double start = aSimTime - aWindowS;
      std::map<std::string, double> attemptTimes;
      std::vector<double> delays;
      for (const ResourceEvent& event : mEvents)
      {
         if ((!aLinkId.empty() && event.linkId != aLinkId) ||
             event.simTime <= start || event.simTime > aSimTime)
         {
            continue;
         }
         if (event.kind == ResourceEventKind::cESTABLISHMENT_ATTEMPTED)
         {
            ++aOutput.establishmentAttempts;
            attemptTimes[event.correlationId] = event.simTime;
         }
         else if (event.kind == ResourceEventKind::cESTABLISHMENT_SUCCEEDED)
         {
            const auto attempt = attemptTimes.find(event.correlationId);
            if (attempt != attemptTimes.end() && event.simTime >= attempt->second)
            {
               ++aOutput.establishmentSuccesses;
               delays.push_back(1000.0 * (event.simTime - attempt->second));
            }
         }
      }
      if (aOutput.establishmentAttempts == 0)
      {
         return;
      }
      SetDerived(aOutput.establishmentSuccessRatioPercent,
                 100.0 * static_cast<double>(aOutput.establishmentSuccesses) /
                    static_cast<double>(aOutput.establishmentAttempts),
                 "percent", aSimTime, aWindowS);
      if (!delays.empty())
      {
         double total = 0.0;
         for (double delay : delays)
         {
            total += delay;
         }
         SetDerived(aOutput.averageEstablishmentDelayMs,
                    total / static_cast<double>(delays.size()),
                    "ms", aSimTime, aWindowS);
      }
   }

   std::vector<ResourceEvent> mEvents;
   std::set<std::string> mEventKeys;
   std::size_t mMaximumEvents;
   std::uint64_t mDuplicateEventCount = 0;
   std::uint64_t mOutOfOrderEventCount = 0;
   std::uint64_t mDroppedEventCount = 0;
};
} // namespace nrm

#endif
