/**
 * @file ProtocolResourceModel.hpp
 * @brief Parameterized polling, timeslot, beam/channel, and CDL resource accounting.
 */

#ifndef NRM_PROTOCOL_RESOURCE_MODEL_HPP
#define NRM_PROTOCOL_RESOURCE_MODEL_HPP

#include <algorithm>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
struct ProtocolResourceDefaults
{
   std::size_t link11PollingUnits = 8;
   std::size_t link16Slots = 16;
   std::size_t satcomBeams = 4;
   std::size_t satcomChannelsPerBeam = 2;
   std::size_t cdlConcurrentLinks = 4;
   std::size_t cdlChannels = 8;

   static ProtocolResourceDefaults AcceptanceDefaults() { return ProtocolResourceDefaults(); }
};

enum class ProtocolResourceReason
{
   cNONE,
   cTYPE_UNKNOWN,
   cPOLLING_UNIT_EXHAUSTED,
   cTIMESLOT_EXHAUSTED,
   cSATCOM_RESOURCE_EXHAUSTED,
   cCDL_RESOURCE_EXHAUSTED
};

struct ProtocolResourceEvaluation
{
   ProtocolResourceState state;
   double addedDelayMs = 0.0;
   bool available = false;
   ProtocolResourceReason reason = ProtocolResourceReason::cNONE;
};

class ProtocolResourceModel
{
public:
   explicit ProtocolResourceModel(
      const ProtocolResourceDefaults& aDefaults = ProtocolResourceDefaults::AcceptanceDefaults())
      : mDefaults(aDefaults)
   {
   }

   ProtocolResourceEvaluation Evaluate(NetworkType aType, std::size_t aActiveOwners,
                                       double /*aRequiredBandwidthBps*/) const
   {
      ProtocolResourceEvaluation result;
      result.state.valid = true;
      switch (aType)
      {
      case NetworkType::cLINK11:
         result.state.kind = "POLLING_UNIT";
         result.state.capacity = mDefaults.link11PollingUnits;
         result.addedDelayMs = 125.0 * (aActiveOwners + 1);
         result.reason = ProtocolResourceReason::cPOLLING_UNIT_EXHAUSTED;
         break;
      case NetworkType::cLINK16:
         result.state.kind = "TIMESLOT";
         result.state.capacity = mDefaults.link16Slots;
         result.addedDelayMs = 7.8125 * (aActiveOwners + 1);
         result.reason = ProtocolResourceReason::cTIMESLOT_EXHAUSTED;
         break;
      case NetworkType::cSATCOM:
         result.state.kind = "BEAM_CHANNEL";
         result.state.capacity = mDefaults.satcomBeams * mDefaults.satcomChannelsPerBeam;
         result.addedDelayMs = 40.0;
         result.reason = ProtocolResourceReason::cSATCOM_RESOURCE_EXHAUSTED;
         break;
      case NetworkType::cCDL:
         result.state.kind = "CHANNEL";
         result.state.capacity = std::min(mDefaults.cdlConcurrentLinks, mDefaults.cdlChannels);
         result.addedDelayMs = 2.0;
         result.reason = ProtocolResourceReason::cCDL_RESOURCE_EXHAUSTED;
         break;
      case NetworkType::cUNKNOWN:
         result.state.valid = false;
         result.reason = ProtocolResourceReason::cTYPE_UNKNOWN;
         return result;
      }
      result.state.used = aActiveOwners + 1;
      result.state.remaining = result.state.capacity > result.state.used
                                  ? result.state.capacity - result.state.used : 0;
      result.available = result.state.capacity > 0 &&
                         result.state.used <= result.state.capacity;
      if (result.state.capacity > 0)
      {
         result.state.utilizationPercent.value = std::min(
            100.0, 100.0 * result.state.used / result.state.capacity);
         result.state.utilizationPercent.unit = "percent";
         result.state.utilizationPercent.valid = true;
         result.state.utilizationPercent.origin = DataOrigin::cPARAMETERIZED_MODEL;
         result.state.utilizationPercent.confidence = Confidence::cLOW;
         result.state.utilizationPercent.reason = MetricReason::cNONE;
      }
      if (result.available) result.reason = ProtocolResourceReason::cNONE;
      return result;
   }

private:
   ProtocolResourceDefaults mDefaults;
};
}

#endif
