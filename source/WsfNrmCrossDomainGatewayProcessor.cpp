/**
 * @file WsfNrmCrossDomainGatewayProcessor.cpp
 * @brief Executes bounded, auditable cross-domain forwarding in simulation time.
 */

#include "WsfNrmCrossDomainGatewayProcessor.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>
#include <utility>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfEvent.hpp"
#include "WsfMessage.hpp"
#include "WsfNrmGatewayExtension.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "nrm/GatewayPolicyEngine.hpp"

namespace
{
std::vector<std::string> SplitTrace(const std::string& aTrace)
{
   std::vector<std::string> result;
   std::istringstream stream(aTrace);
   std::string value;
   while (std::getline(stream, value, ','))
   {
      if (!value.empty())
      {
         result.push_back(value);
      }
   }
   return result;
}

std::string JoinTrace(const std::vector<std::string>& aTrace)
{
   std::ostringstream stream;
   for (std::size_t index = 0; index < aTrace.size(); ++index)
   {
      if (index != 0)
      {
         stream << ',';
      }
      stream << aTrace[index];
   }
   return stream.str();
}

bool ReadStringAux(const WsfMessage& aMessage,
                   const std::string& aName,
                   std::string& aValue)
{
   return WsfUtil::GetAuxValue(aMessage, aName, aValue);
}

int ReadIntAux(const WsfMessage& aMessage,
               const std::string& aName,
               int aDefault)
{
   int value = aDefault;
   WsfUtil::GetAuxValue(aMessage, aName, value);
   return value;
}
} // namespace

WsfNrmCrossDomainGatewayProcessor::WsfNrmCrossDomainGatewayProcessor(
   WsfScenario& aScenario)
   : WsfProcessor(aScenario)
{
}

WsfNrmCrossDomainGatewayProcessor::WsfNrmCrossDomainGatewayProcessor(
   const WsfNrmCrossDomainGatewayProcessor& aSource)
   : WsfProcessor(aSource)
   , mCapabilities(aSource.mCapabilities)
   , mDeduplicationWindowS(aSource.mDeduplicationWindowS)
   , mRecentEventLimit(aSource.mRecentEventLimit)
{
   for (nrm::GatewayResourceState& capability : mCapabilities)
   {
      capability.platformId.clear();
      capability.valid = false;
      capability.queuedMessages = 0;
      capability.queuedBits = 0;
      capability.receivedCount = 0;
      capability.forwardedCount = 0;
      capability.rejectedCount = 0;
      capability.droppedCount = 0;
      capability.forwardedBits = 0;
      capability.recentEvents.clear();
      capability.reasonCodes.clear();
   }
}

WsfProcessor* WsfNrmCrossDomainGatewayProcessor::Clone() const
{
   return new WsfNrmCrossDomainGatewayProcessor(*this);
}

bool WsfNrmCrossDomainGatewayProcessor::ProcessInput(UtInput& aInput)
{
   const std::string command = aInput.GetCommand();
   if (command == "deduplication_window")
   {
      aInput.ReadValue(mDeduplicationWindowS);
      if (!std::isfinite(mDeduplicationWindowS) || mDeduplicationWindowS <= 0.0)
      {
         throw UtInput::BadValue(aInput, "deduplication_window must be positive");
      }
      return true;
   }
   if (command == "recent_event_limit")
   {
      aInput.ReadValue(mRecentEventLimit);
      if (mRecentEventLimit == 0)
      {
         throw UtInput::BadValue(aInput, "recent_event_limit must be positive");
      }
      return true;
   }
   if (command != "capability")
   {
      return WsfProcessor::ProcessInput(aInput);
   }

   nrm::GatewayResourceState capability;
   capability.origin = nrm::DataOrigin::cAFSIM_INTERNAL;
   capability.confidence = nrm::Confidence::cHIGH;
   UtInputBlock block(aInput, "end_capability");
   std::string blockCommand;
   while (block.ReadCommand(blockCommand))
   {
      if (blockCommand == "gateway_id")
      {
         aInput.ReadValue(capability.gatewayId);
      }
      else if (blockCommand == "ingress_network_id")
      {
         aInput.ReadValue(capability.ingressNetworkId);
      }
      else if (blockCommand == "egress_network_id")
      {
         aInput.ReadValue(capability.egressNetworkId);
      }
      else if (blockCommand == "ingress_comm")
      {
         aInput.ReadValue(capability.ingressCommName);
      }
      else if (blockCommand == "egress_comm")
      {
         aInput.ReadValue(capability.egressCommName);
      }
      else if (blockCommand == "allowed_source_platform")
      {
         std::string value;
         aInput.ReadValue(value);
         capability.allowedSourcePlatformIds.push_back(value);
      }
      else if (blockCommand == "allowed_destination_platform")
      {
         std::string value;
         aInput.ReadValue(value);
         capability.allowedDestinationPlatformIds.push_back(value);
      }
      else if (blockCommand == "allowed_message_type")
      {
         std::string value;
         aInput.ReadValue(value);
         capability.allowedMessageTypes.push_back(value);
      }
      else if (blockCommand == "priority")
      {
         aInput.ReadValue(capability.priority);
      }
      else if (blockCommand == "processing_delay_ms")
      {
         aInput.ReadValue(capability.processingDelayMs);
      }
      else if (blockCommand == "forwarding_rate_bps")
      {
         aInput.ReadValue(capability.forwardingRateBps);
      }
      else if (blockCommand == "max_queue_messages")
      {
         aInput.ReadValue(capability.maxQueueMessages);
      }
      else if (blockCommand == "max_queue_bits")
      {
         aInput.ReadValue(capability.maxQueueBits);
      }
      else if (blockCommand == "enabled")
      {
         aInput.ReadValue(capability.enabled);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   mCapabilities.push_back(capability);
   return true;
}

bool WsfNrmCrossDomainGatewayProcessor::Initialize(double aSimTime)
{
   bool valid = WsfProcessor::Initialize(aSimTime);
   WsfNrmGatewaySimulationExtension* extension =
      WsfNrmGatewaySimulationExtension::Find(*GetSimulation());
   if (extension == nullptr || mCapabilities.empty())
   {
      ut::log::error() << "NRM_GATEWAY processor initialization failed: missing "
                          "gateway extension or capability";
      return false;
   }

   for (nrm::GatewayResourceState& capability : mCapabilities)
   {
      capability.platformId = GetPlatform()->GetName();
      capability.sampleTime = aSimTime;
      capability.valid = true;
      capability.reasonCodes.clear();
      wsf::comm::Comm* ingress =
         GetPlatform()->GetComponent<wsf::comm::Comm>(capability.ingressCommName);
      wsf::comm::Comm* egress =
         GetPlatform()->GetComponent<wsf::comm::Comm>(capability.egressCommName);
      if (ingress == nullptr)
      {
         capability.valid = false;
         capability.reasonCodes.push_back("GATEWAY_INGRESS_REFERENCE_INVALID");
         ut::log::error() << "NRM_GATEWAY INIT_INGRESS_INVALID capability="
                          << capability.gatewayId << " comm="
                          << capability.ingressCommName << " expected_network="
                          << capability.ingressNetworkId << " actual_network=<missing>";
      }
      if (egress == nullptr)
      {
         capability.valid = false;
         capability.reasonCodes.push_back("GATEWAY_EGRESS_REFERENCE_INVALID");
         ut::log::error() << "NRM_GATEWAY INIT_EGRESS_INVALID capability="
                          << capability.gatewayId << " comm="
                          << capability.egressCommName << " expected_network="
                          << capability.egressNetworkId << " actual_network=<missing>";
      }
      if (!capability.valid)
      {
         valid = false;
      }
      extension->UpdateCapability(capability);
   }
   return valid;
}

nrm::GatewayResourceState*
WsfNrmCrossDomainGatewayProcessor::FindLocalCapability(
   const std::string& aCapabilityId)
{
   const auto found = std::find_if(
      mCapabilities.begin(), mCapabilities.end(),
      [&aCapabilityId](const nrm::GatewayResourceState& aCapability)
      { return aCapability.gatewayId == aCapabilityId; });
   return found == mCapabilities.end() ? nullptr : &*found;
}

const nrm::GatewayResourceState*
WsfNrmCrossDomainGatewayProcessor::FindLocalCapability(
   const std::string& aCapabilityId) const
{
   const auto found = std::find_if(
      mCapabilities.begin(), mCapabilities.end(),
      [&aCapabilityId](const nrm::GatewayResourceState& aCapability)
      { return aCapability.gatewayId == aCapabilityId; });
   return found == mCapabilities.end() ? nullptr : &*found;
}

bool WsfNrmCrossDomainGatewayProcessor::ProcessMessage(
   double aSimTime,
   const WsfMessage& aMessage)
{
   nrm::GatewayMessageContext context;
   if (!ReadStringAux(aMessage, "nrm_gateway_route_id", context.routeId))
   {
      return false;
   }
   ReadStringAux(aMessage, "nrm_gateway_transfer_id", context.transferId);
   ReadStringAux(aMessage, "nrm_original_source_platform", context.sourcePlatformId);
   ReadStringAux(aMessage, "nrm_final_destination_platform",
                 context.destinationPlatformId);
   ReadStringAux(aMessage, "nrm_final_destination_comm",
                 context.destinationCommName);
   context.messageType = aMessage.GetType().GetString();
   context.actualOriginatorPlatformId = aMessage.GetOriginator().GetString();

   const int routeIndex =
      ReadIntAux(aMessage, "nrm_gateway_route_index", 0);
   const int hopCount = ReadIntAux(aMessage, "nrm_gateway_hop_count", 0);
   context.routeIndex = routeIndex < 0
                           ? std::numeric_limits<std::size_t>::max()
                           : static_cast<std::size_t>(routeIndex);
   context.hopCount = hopCount < 0
                         ? std::numeric_limits<std::size_t>::max()
                         : static_cast<std::size_t>(hopCount);
   context.ttl = ReadIntAux(aMessage, "nrm_gateway_ttl", 8);
   std::string encodedTrace;
   ReadStringAux(aMessage, "nrm_gateway_trace", encodedTrace);
   context.trace = SplitTrace(encodedTrace);
   wsf::comm::NetworkManager* networkManager =
      wsf::comm::NetworkManager::Find(*GetSimulation());
   wsf::comm::Comm* ingressComm =
      networkManager == nullptr ? nullptr : networkManager->GetComm(aMessage.GetDstAddr());
   if (ingressComm != nullptr)
   {
      context.actualIngressNetworkId = ingressComm->GetNetwork();
      context.actualIngressCommName = ingressComm->GetName();
   }

   WsfNrmGatewaySimulationExtension* extension =
      WsfNrmGatewaySimulationExtension::Find(*GetSimulation());
   if (extension == nullptr)
   {
      return false;
   }
   nrm::GatewayPolicyEngine engine;
   const nrm::GatewayPolicyDecision decision = engine.Evaluate(
      extension->GetCapabilities(), extension->GetRoutes(), context,
      GetPlatform()->GetName());
   if (decision.action != nrm::GatewayPolicyAction::cFORWARD)
   {
      RecordImmediateEvent(context, decision, aMessage, "REJECTED",
                           decision.reasonCode, aSimTime);
      return true;
   }

   nrm::GatewayResourceState* capability =
      FindLocalCapability(decision.capabilityId);
   if (capability == nullptr)
   {
      RecordImmediateEvent(context, decision, aMessage, "REJECTED",
                           "GATEWAY_LOCAL_CAPABILITY_NOT_FOUND", aSimTime);
      return true;
   }
   PruneDeduplication(aSimTime);
   const std::string deduplicationKey =
      context.transferId + "#" + std::to_string(context.routeIndex);
   if (mSeenTransfers.find(deduplicationKey) != mSeenTransfers.end())
   {
      RecordImmediateEvent(context, decision, aMessage, "DROPPED",
                           "GATEWAY_DUPLICATE_TRANSFER", aSimTime);
      return true;
   }

   const std::uint64_t messageBits = aMessage.GetSizeBits() <= 0
                                        ? 0U
                                        : static_cast<std::uint64_t>(aMessage.GetSizeBits());
   if (capability->queuedMessages >= capability->maxQueueMessages ||
       messageBits > capability->maxQueueBits -
                        std::min(capability->queuedBits, capability->maxQueueBits))
   {
      RecordImmediateEvent(context, decision, aMessage, "DROPPED",
                           "GATEWAY_QUEUE_CAPACITY_EXCEEDED", aSimTime);
      return true;
   }

   mSeenTransfers[deduplicationKey] = aSimTime;
   ++capability->receivedCount;
   capability->sampleTime = aSimTime;
   PendingForward pending;
   pending.message.reset(aMessage.Clone());
   pending.context = context;
   pending.decision = decision;
   pending.messageBits = messageBits;
   pending.inputSerialNumber = aMessage.GetSerialNumber();
   pending.receiveTime = aSimTime;
   pending.priority = capability->priority + aMessage.GetPriority();
   const auto route = std::find_if(
      extension->GetRoutes().begin(), extension->GetRoutes().end(),
      [&context](const nrm::GatewayRouteTemplate& aRoute)
      { return aRoute.routeId == context.routeId; });
   if (route != extension->GetRoutes().end())
   {
      pending.priority += route->priority;
   }
   ++capability->queuedMessages;
   capability->queuedBits += messageBits;
   PublishCapability(*capability);
   mQueue.push_back(std::move(pending));
   StartNext(aSimTime);
   return true;
}

void WsfNrmCrossDomainGatewayProcessor::StartNext(double aSimTime)
{
   if (mActive != nullptr || mQueue.empty())
   {
      return;
   }
   std::stable_sort(
      mQueue.begin(), mQueue.end(),
      [](const PendingForward& aLeft, const PendingForward& aRight)
      {
         if (aLeft.priority != aRight.priority)
         {
            return aLeft.priority > aRight.priority;
         }
         if (aLeft.receiveTime != aRight.receiveTime)
         {
            return aLeft.receiveTime < aRight.receiveTime;
         }
         return aLeft.context.transferId < aRight.context.transferId;
      });
   mActive = std::make_unique<PendingForward>(std::move(mQueue.front()));
   mQueue.erase(mQueue.begin());
   const nrm::GatewayResourceState* capability =
      FindLocalCapability(mActive->decision.capabilityId);
   if (capability == nullptr)
   {
      CompleteActive(aSimTime);
      return;
   }
   const double serviceTime = capability->processingDelayMs / 1000.0 +
                              static_cast<double>(mActive->messageBits) /
                                 capability->forwardingRateBps;
   GetSimulation()->AddEvent(ut::make_unique<WsfOneShotEvent>(
      aSimTime + serviceTime,
      [this, aSimTime, serviceTime]()
      { CompleteActive(aSimTime + serviceTime); }));
}

void WsfNrmCrossDomainGatewayProcessor::CompleteActive(double aSimTime)
{
   if (mActive == nullptr)
   {
      return;
   }
   PendingForward& pending = *mActive;
   nrm::GatewayResourceState* capability =
      FindLocalCapability(pending.decision.capabilityId);
   if (capability == nullptr)
   {
      RecordEvent(pending, "DROPPED", "GATEWAY_LOCAL_CAPABILITY_NOT_FOUND", 0,
                  aSimTime);
      mActive.reset();
      StartNext(aSimTime);
      return;
   }

   wsf::comm::Comm* outgoing =
      GetPlatform()->GetComponent<wsf::comm::Comm>(capability->egressCommName);
   wsf::comm::Comm* target = nullptr;
   WsfNrmGatewaySimulationExtension* extension =
      WsfNrmGatewaySimulationExtension::Find(*GetSimulation());
   if (outgoing != nullptr && extension != nullptr && !pending.decision.finalHop)
   {
      const nrm::GatewayResourceState* next =
         extension->FindCapability(pending.decision.nextCapabilityId);
      if (next != nullptr)
      {
         WsfPlatform* nextPlatform =
            GetSimulation()->GetPlatformByName(WsfStringId(next->platformId));
         if (nextPlatform != nullptr)
         {
            target = nextPlatform->GetComponent<wsf::comm::Comm>(next->ingressCommName);
         }
      }
   }
   else if (outgoing != nullptr && extension != nullptr)
   {
      WsfPlatform* destination = GetSimulation()->GetPlatformByName(
         WsfStringId(pending.context.destinationPlatformId));
      std::string destinationCommName = pending.context.destinationCommName;
      const auto route = std::find_if(
         extension->GetRoutes().begin(), extension->GetRoutes().end(),
         [&pending](const nrm::GatewayRouteTemplate& aRoute)
         { return aRoute.routeId == pending.context.routeId; });
      if (destinationCommName.empty() && route != extension->GetRoutes().end())
      {
         destinationCommName = route->destinationCommName;
      }
      if (destination != nullptr)
      {
         target = destination->GetComponent<wsf::comm::Comm>(destinationCommName);
      }
   }

   if (outgoing == nullptr || target == nullptr ||
       outgoing->GetNetwork() != capability->egressNetworkId ||
       target->GetNetwork() != capability->egressNetworkId)
   {
      if (capability->queuedMessages > 0)
      {
         --capability->queuedMessages;
      }
      capability->queuedBits -=
         std::min(capability->queuedBits, pending.messageBits);
      ++capability->droppedCount;
      RecordEvent(pending, "DROPPED", "GATEWAY_FORWARD_TARGET_INVALID", 0,
                  aSimTime);
      PublishCapability(*capability);
      mActive.reset();
      StartNext(aSimTime);
      return;
   }

   pending.context.trace.push_back(pending.decision.capabilityId);
   WsfAttributeContainer& aux = pending.message->GetAuxData();
   aux.Assign("nrm_gateway_transfer_id", pending.context.transferId);
   aux.Assign("nrm_gateway_route_id", pending.context.routeId);
   aux.Assign("nrm_gateway_route_index",
              static_cast<int>(pending.decision.nextRouteIndex));
   aux.Assign("nrm_gateway_hop_count",
              static_cast<int>(pending.context.hopCount + 1U));
   aux.Assign("nrm_gateway_ttl", pending.context.ttl - 1);
   aux.Assign("nrm_gateway_trace", JoinTrace(pending.context.trace));
   aux.Assign("nrm_original_source_platform", pending.context.sourcePlatformId);
   aux.Assign("nrm_final_destination_platform",
              pending.context.destinationPlatformId);
   aux.Assign("nrm_final_destination_comm", pending.context.destinationCommName);
   const unsigned int outputSerialNumber =
      GetSimulation()->NextMessageSerialNumber();
   pending.message->SetSerialNumber(outputSerialNumber);
   const bool sent = outgoing->Send(aSimTime, std::move(pending.message),
                                    target->GetAddress());

   if (capability->queuedMessages > 0)
   {
      --capability->queuedMessages;
   }
   capability->queuedBits -= std::min(capability->queuedBits, pending.messageBits);
   if (sent)
   {
      ++capability->forwardedCount;
      capability->forwardedBits += pending.messageBits;
      RecordEvent(pending, "FORWARDED", "FORWARD_ALLOWED", outputSerialNumber,
                  aSimTime);
      ut::log::info() << "NRM_GATEWAY FORWARDED transfer="
                      << pending.context.transferId << " route="
                      << pending.context.routeId << " capability="
                      << pending.decision.capabilityId << " platform="
                      << GetPlatform()->GetName() << " input_serial="
                      << pending.inputSerialNumber << " output_serial="
                      << outputSerialNumber;
   }
   else
   {
      ++capability->droppedCount;
      RecordEvent(pending, "DROPPED", "GATEWAY_SEND_FAILED", outputSerialNumber,
                  aSimTime);
   }
   capability->sampleTime = aSimTime;
   PublishCapability(*capability);
   mActive.reset();
   StartNext(aSimTime);
}

void WsfNrmCrossDomainGatewayProcessor::RecordEvent(
   const PendingForward& aPending,
   const std::string& aResult,
   const std::string& aReason,
   std::uint64_t aOutputSerialNumber,
   double aCompletionTime)
{
   nrm::GatewayResourceState* capability =
      FindLocalCapability(aPending.decision.capabilityId);
   if (capability == nullptr)
   {
      return;
   }
   nrm::GatewayForwardingEvent event;
   event.transferId = aPending.context.transferId;
   event.routeId = aPending.context.routeId;
   event.gatewayCapabilityId = aPending.decision.capabilityId;
   event.platformId = GetPlatform()->GetName();
   event.sourcePlatformId = aPending.context.sourcePlatformId;
   event.destinationPlatformId = aPending.context.destinationPlatformId;
   event.destinationCommName = aPending.context.destinationCommName;
   event.ingressNetworkId = capability->ingressNetworkId;
   event.egressNetworkId = capability->egressNetworkId;
   event.messageType = aPending.context.messageType;
   event.result = aResult;
   event.reasonCode = aReason;
   event.inputSerialNumber = aPending.inputSerialNumber;
   event.outputSerialNumber = aOutputSerialNumber;
   event.messageBits = aPending.messageBits;
   event.routeIndex = aPending.context.routeIndex;
   event.hopCount = aPending.context.hopCount;
   event.receiveTime = aPending.receiveTime;
   event.completionTime = aCompletionTime;
   capability->recentEvents.push_back(event);
   if (capability->recentEvents.size() > mRecentEventLimit)
   {
      capability->recentEvents.erase(
         capability->recentEvents.begin(),
         capability->recentEvents.begin() +
            static_cast<std::ptrdiff_t>(capability->recentEvents.size() -
                                        mRecentEventLimit));
   }
}

void WsfNrmCrossDomainGatewayProcessor::RecordImmediateEvent(
   const nrm::GatewayMessageContext& aContext,
   const nrm::GatewayPolicyDecision& aDecision,
   const WsfMessage& aMessage,
   const std::string& aResult,
   const std::string& aReason,
   double aSimTime)
{
   nrm::GatewayResourceState* capability =
      FindLocalCapability(aDecision.capabilityId);
   if (capability == nullptr)
   {
      const auto fallback = std::find_if(
         mCapabilities.begin(), mCapabilities.end(),
         [&aContext](const nrm::GatewayResourceState& aCandidate)
         {
            return aCandidate.ingressNetworkId == aContext.actualIngressNetworkId &&
                   aCandidate.ingressCommName == aContext.actualIngressCommName;
         });
      if (fallback != mCapabilities.end())
      {
         capability = &*fallback;
      }
   }
   if (capability == nullptr)
   {
      return;
   }
   ++capability->receivedCount;
   if (aResult == "DROPPED")
   {
      ++capability->droppedCount;
   }
   else
   {
      ++capability->rejectedCount;
   }
   PendingForward pending;
   pending.context = aContext;
   pending.decision = aDecision;
   pending.decision.capabilityId = capability->gatewayId;
   pending.messageBits = aMessage.GetSizeBits() <= 0
                            ? 0U
                            : static_cast<std::uint64_t>(aMessage.GetSizeBits());
   pending.inputSerialNumber = aMessage.GetSerialNumber();
   pending.receiveTime = aSimTime;
   RecordEvent(pending, aResult, aReason, 0, aSimTime);
   capability->sampleTime = aSimTime;
   PublishCapability(*capability);
   ut::log::warning() << "NRM_GATEWAY " << aResult << " transfer="
                      << aContext.transferId << " route=" << aContext.routeId
                      << " capability=" << capability->gatewayId << " reason="
                      << aReason;
}

void WsfNrmCrossDomainGatewayProcessor::PublishCapability(
   const nrm::GatewayResourceState& aCapability)
{
   WsfNrmGatewaySimulationExtension* extension =
      WsfNrmGatewaySimulationExtension::Find(*GetSimulation());
   if (extension != nullptr)
   {
      extension->UpdateCapability(aCapability);
   }
}

void WsfNrmCrossDomainGatewayProcessor::PruneDeduplication(double aSimTime)
{
   for (auto entry = mSeenTransfers.begin(); entry != mSeenTransfers.end();)
   {
      if (aSimTime - entry->second > mDeduplicationWindowS)
      {
         entry = mSeenTransfers.erase(entry);
      }
      else
      {
         ++entry;
      }
   }
}
