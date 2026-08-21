/**
 * @file WsfNrmCrossDomainGatewayProcessor.hpp
 * @brief Runtime AFSIM processor for policy-controlled cross-domain forwarding.
 */

#ifndef WSF_NRM_CROSS_DOMAIN_GATEWAY_PROCESSOR_HPP
#define WSF_NRM_CROSS_DOMAIN_GATEWAY_PROCESSOR_HPP

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "WsfProcessor.hpp"
#include "nrm/GatewayTypes.hpp"
#include "nrm/NetworkResourceTypes.hpp"
#include "wsf_network_resource_manager_export.h"

class UtInput;
class WsfMessage;
class WsfScenario;

class WSF_NETWORK_RESOURCE_MANAGER_EXPORT WsfNrmCrossDomainGatewayProcessor final
   : public WsfProcessor
{
public:
   explicit WsfNrmCrossDomainGatewayProcessor(WsfScenario& aScenario);
   WsfNrmCrossDomainGatewayProcessor(
      const WsfNrmCrossDomainGatewayProcessor& aSource);

   WsfProcessor* Clone() const override;
   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;
   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

private:
   struct PendingForward
   {
      std::unique_ptr<WsfMessage> message;
      nrm::GatewayMessageContext context;
      nrm::GatewayPolicyDecision decision;
      std::uint64_t messageBits = 0;
      std::uint64_t inputSerialNumber = 0;
      double receiveTime = 0.0;
      int priority = 0;
   };

   nrm::GatewayResourceState* FindLocalCapability(
      const std::string& aCapabilityId);
   const nrm::GatewayResourceState* FindLocalCapability(
      const std::string& aCapabilityId) const;
   void StartNext(double aSimTime);
   void CompleteActive(double aSimTime);
   void RecordEvent(const PendingForward& aPending,
                    const std::string& aResult,
                    const std::string& aReason,
                    std::uint64_t aOutputSerialNumber,
                    double aCompletionTime);
   void RecordImmediateEvent(const nrm::GatewayMessageContext& aContext,
                             const nrm::GatewayPolicyDecision& aDecision,
                             const WsfMessage& aMessage,
                             const std::string& aResult,
                             const std::string& aReason,
                             double aSimTime);
   void PublishCapability(const nrm::GatewayResourceState& aCapability);
   void PruneDeduplication(double aSimTime);

   std::vector<nrm::GatewayResourceState> mCapabilities;
   std::vector<PendingForward> mQueue;
   std::unique_ptr<PendingForward> mActive;
   std::map<std::string, double> mSeenTransfers;
   double mDeduplicationWindowS = 60.0;
   std::size_t mRecentEventLimit = 100;
};

#endif
