/**
 * @file CustomerNrmAdapter.hpp
 * @brief Stable, in-process customer integration boundary for the NRM module.
 *
 * Customer-specific AFSIM objects are converted to the value types declared by
 * NRM before entering this class.  The adapter deliberately has no dependency
 * on Qt, JSON, sockets, or customer-private headers.
 */

#ifndef NRM_CUSTOMER_NRM_ADAPTER_HPP
#define NRM_CUSTOMER_NRM_ADAPTER_HPP

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>

#include "nrm/AssessmentTypes.hpp"
#include "nrm/CommunicationCapabilityTypes.hpp"
#include "nrm/CustomerIngestionState.hpp"
#include "nrm/CustomerSnapshotAssembler.hpp"
#include "nrm/NetworkPlanTypes.hpp"
#include "nrm/ResourceSnapshotValidator.hpp"
#include "nrm/ResourceDemandTypes.hpp"

namespace nrm
{
using CustomerIngestStatus = CustomerIngestionStatus;

struct CustomerCallContext
{
   std::string runId;
   std::string messageId;
   std::string providerId;
   double simTime = 0.0;
   bool hasSimTime = false;
};

enum class CustomerIngestReason
{
   cNONE,
   cCONTEXT_INVALID,
   cRESOURCE_INVALID,
   cNAVIGATION_INVALID,
   cENVIRONMENT_INVALID
};

inline const char* ToString(CustomerIngestReason aReason)
{
   switch (aReason)
   {
   case CustomerIngestReason::cNONE: return "NONE";
   case CustomerIngestReason::cCONTEXT_INVALID: return "CONTEXT_INVALID";
   case CustomerIngestReason::cRESOURCE_INVALID: return "RESOURCE_INVALID";
   case CustomerIngestReason::cNAVIGATION_INVALID: return "NAVIGATION_INVALID";
   case CustomerIngestReason::cENVIRONMENT_INVALID: return "ENVIRONMENT_INVALID";
   }
   return "CONTEXT_INVALID";
}

struct CustomerIngestResult
{
   CustomerIngestStatus status = CustomerIngestStatus::cREJECTED;
   CustomerIngestReason reason = CustomerIngestReason::cCONTEXT_INVALID;
   std::uint64_t snapshotVersion = 0;
   bool mutated = false;
};

struct CustomerPlanEvaluationResult
{
   bool accepted = false;
   PlanValidationResult validation;
   NetworkPlanEvaluationResult evaluation;
};

/**
 * Port implemented by the owning AFSIM/Warlock module.  It keeps lifecycle,
 * storage, UI notification, reporting, and model execution outside the
 * customer-facing adapter.
 */
class CustomerNrmHost
{
public:
   virtual ~CustomerNrmHost() = default;

   virtual const ResourceSnapshot& CurrentSnapshot() const = 0;
   // Implementations that maintain an AFSIM base must return only the
   // customer-owned overlay here.  The default preserves source compatibility
   // for customer-only/replay hosts.
   virtual const ResourceSnapshot& CurrentCustomerOverlay() const
   {
      return CurrentSnapshot();
   }
   virtual std::string ActiveConfigVersion() const = 0;
   virtual void PublishCustomerSnapshot(const ResourceSnapshot& aSnapshot) = 0;
   virtual void ApplyCustomerEnvironmentContext(
      const EnvironmentContext& aContext) = 0;
   virtual void BeginCustomerRun() = 0;
   virtual AssessmentResult RunCustomerAssessment(
      const AssessmentTask& aTask) = 0;
   virtual CustomerPlanEvaluationResult RunCustomerPlan(
      const NetworkPlanDocument& aPlan) = 0;
   virtual ResourceDemandBatchResult RunCustomerDemands(
      const ResourceDemandSet& aDemands) = 0;
};

/**
 * Public same-process API used after customer objects have been converted to
 * canonical NRM value types.  Duplicate, stale, and run-transition decisions
 * are made before any host mutation.
 */
class CustomerNrmAdapter
{
public:
   explicit CustomerNrmAdapter(CustomerNrmHost& aHost,
                               std::size_t aMaximumMessageIds = 4096)
      : mHost(aHost)
      , mOwnedIngestionState(aMaximumMessageIds)
      , mIngestionState(mOwnedIngestionState)
   {
   }

   CustomerNrmAdapter(CustomerNrmHost& aHost,
                      CustomerIngestionState& aIngestionState)
      : mHost(aHost)
      , mOwnedIngestionState(1)
      , mIngestionState(aIngestionState)
   {
   }

   CustomerNrmAdapter(const CustomerNrmAdapter&) = delete;
   CustomerNrmAdapter& operator=(const CustomerNrmAdapter&) = delete;

   CustomerIngestResult UpdateResources(
      const CustomerCallContext& aContext,
      const ResourceSnapshot& aSnapshot)
   {
      if (!ValidContext(aContext))
         return Rejected(CustomerIngestReason::cCONTEXT_INVALID);
      if (!std::isfinite(aSnapshot.simTime) || aSnapshot.simTime < 0.0)
         return Rejected(CustomerIngestReason::cRESOURCE_INVALID);
      if (!ResourceSnapshotValidator().Validate(aSnapshot).valid)
         return Rejected(CustomerIngestReason::cRESOURCE_INVALID);

      const CustomerIngestionDecision decision = Preview(
         aContext, CustomerMessageDomain::cRESOURCE);
      if (decision.status != CustomerIngestionStatus::cACCEPTED)
         return Ignored(decision.status);

      BeginRunIfNeeded(decision);
      ResourceSnapshot normalized = aSnapshot;
      if (aContext.hasSimTime) normalized.simTime = aContext.simTime;
      if (!aContext.providerId.empty())
         normalized.providerId = aContext.providerId;
      const ResourceSnapshot merged = CustomerSnapshotAssembler::MergeResources(
         mHost.CurrentCustomerOverlay(), normalized, decision.newRun);
      mHost.PublishCustomerSnapshot(merged);
      Commit(aContext, CustomerMessageDomain::cRESOURCE);
      return Accepted(mHost.CurrentSnapshot().snapshotVersion);
   }

   CustomerIngestResult UpdateNavigation(
      const CustomerCallContext& aContext,
      const NavigationSample& aSample)
   {
      if (!ValidContext(aContext))
         return Rejected(CustomerIngestReason::cCONTEXT_INVALID);
      if (!aSample.valid ||
          CustomerSnapshotAssembler::NavigationPlatformId(aSample).empty() ||
          !std::isfinite(aSample.sampleTime) || aSample.sampleTime < 0.0)
         return Rejected(CustomerIngestReason::cNAVIGATION_INVALID);

      const CustomerIngestionDecision decision = Preview(
         aContext, CustomerMessageDomain::cNAVIGATION);
      if (decision.status != CustomerIngestionStatus::cACCEPTED)
         return Ignored(decision.status);

      BeginRunIfNeeded(decision);
      NavigationSample normalized = aSample;
      if (aContext.hasSimTime) normalized.sampleTime = aContext.simTime;
      const std::string providerId = aContext.providerId.empty()
                                        ? mHost.CurrentCustomerOverlay().providerId
                                        : aContext.providerId;
      const ResourceSnapshot merged = CustomerSnapshotAssembler::UpsertNavigation(
         mHost.CurrentCustomerOverlay(), normalized, providerId, decision.newRun);
      mHost.PublishCustomerSnapshot(merged);
      Commit(aContext, CustomerMessageDomain::cNAVIGATION);
      return Accepted(mHost.CurrentSnapshot().snapshotVersion);
   }

   CustomerIngestResult UpdateEnvironment(
      const CustomerCallContext& aContext,
      const EnvironmentSnapshot& aSnapshot,
      const EnvironmentContext& aEnvironment)
   {
      if (!ValidContext(aContext))
         return Rejected(CustomerIngestReason::cCONTEXT_INVALID);
      if (!aSnapshot.valid || !std::isfinite(aSnapshot.sampleTime) ||
          aSnapshot.sampleTime < 0.0)
         return Rejected(CustomerIngestReason::cENVIRONMENT_INVALID);

      const CustomerIngestionDecision decision = Preview(
         aContext, CustomerMessageDomain::cENVIRONMENT);
      if (decision.status != CustomerIngestionStatus::cACCEPTED)
         return Ignored(decision.status);

      BeginRunIfNeeded(decision);
      EnvironmentSnapshot normalized = aSnapshot;
      EnvironmentContext normalizedContext = aEnvironment;
      if (normalizedContext.origin == DataOrigin::cCUSTOMER_MODULE &&
          normalizedContext.customerProvidedDomains.empty())
      {
         normalizedContext.customerProvidedDomains =
            ProvidedEnvironmentDomains(normalized);
      }
      if (aContext.hasSimTime)
      {
         normalized.sampleTime = aContext.simTime;
         normalizedContext.sampleTime = aContext.simTime;
      }
      if (!aContext.providerId.empty())
      {
         normalized.providerId = aContext.providerId;
         normalizedContext.providerId = aContext.providerId;
      }
      const ResourceSnapshot merged = CustomerSnapshotAssembler::MergeEnvironment(
         mHost.CurrentCustomerOverlay(), normalized, decision.newRun);
      mHost.ApplyCustomerEnvironmentContext(normalizedContext);
      mHost.PublishCustomerSnapshot(merged);
      Commit(aContext, CustomerMessageDomain::cENVIRONMENT);
      return Accepted(mHost.CurrentSnapshot().snapshotVersion);
   }

   AssessmentResult Evaluate(const AssessmentTask& aTask)
   {
      return mHost.RunCustomerAssessment(aTask);
   }

   CustomerPlanEvaluationResult EvaluatePlan(
      const NetworkPlanDocument& aPlan)
   {
      NetworkPlanDocument normalized = aPlan;
      normalized.configVersion = mHost.ActiveConfigVersion();
      return mHost.RunCustomerPlan(normalized);
   }

   ResourceDemandBatchResult EvaluateDemands(
      const ResourceDemandSet& aDemands)
   {
      ResourceDemandSet normalized = aDemands;
      normalized.configVersion = mHost.ActiveConfigVersion();
      return mHost.RunCustomerDemands(normalized);
   }

   const std::string& CurrentRunId() const
   {
      return mIngestionState.get().CurrentRunId();
   }

private:
   static std::vector<EnvironmentDomain> ProvidedEnvironmentDomains(
      const EnvironmentSnapshot& aSnapshot)
   {
      std::vector<EnvironmentDomain> domains;
      if (aSnapshot.terrain.available)
         domains.push_back(EnvironmentDomain::cTERRAIN);
      if (aSnapshot.weather.available)
         domains.push_back(EnvironmentDomain::cWEATHER);
      if (aSnapshot.celestial.available)
         domains.push_back(EnvironmentDomain::cCELESTIAL);
      if (aSnapshot.interference.available)
         domains.push_back(
            EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE);
      return domains;
   }

   static bool ValidContext(const CustomerCallContext& aContext)
   {
      return !aContext.messageId.empty() &&
             (!aContext.hasSimTime ||
              (std::isfinite(aContext.simTime) && aContext.simTime >= 0.0));
   }

   CustomerIngestionDecision Preview(
      const CustomerCallContext& aContext,
      CustomerMessageDomain aDomain) const
   {
      return mIngestionState.get().Preview(
         aContext.runId, aContext.messageId, aDomain,
         aContext.hasSimTime, aContext.simTime);
   }

   void Commit(const CustomerCallContext& aContext,
               CustomerMessageDomain aDomain)
   {
      mIngestionState.get().Commit(
         aContext.runId, aContext.messageId, aDomain,
         aContext.hasSimTime, aContext.simTime);
   }

   void BeginRunIfNeeded(const CustomerIngestionDecision& aDecision)
   {
      if (aDecision.newRun) mHost.BeginCustomerRun();
   }

   CustomerIngestResult Accepted(std::uint64_t aSnapshotVersion) const
   {
      CustomerIngestResult result;
      result.status = CustomerIngestStatus::cACCEPTED;
      result.reason = CustomerIngestReason::cNONE;
      result.snapshotVersion = aSnapshotVersion;
      result.mutated = true;
      return result;
   }

   CustomerIngestResult Ignored(CustomerIngestStatus aStatus) const
   {
      CustomerIngestResult result;
      result.status = aStatus;
      result.reason = CustomerIngestReason::cNONE;
      result.snapshotVersion = mHost.CurrentSnapshot().snapshotVersion;
      return result;
   }

   CustomerIngestResult Rejected(CustomerIngestReason aReason) const
   {
      CustomerIngestResult result;
      result.status = CustomerIngestStatus::cREJECTED;
      result.reason = aReason;
      result.snapshotVersion = mHost.CurrentSnapshot().snapshotVersion;
      return result;
   }

   CustomerNrmHost& mHost;
   CustomerIngestionState mOwnedIngestionState;
   std::reference_wrapper<CustomerIngestionState> mIngestionState;
};
} // namespace nrm

#endif
