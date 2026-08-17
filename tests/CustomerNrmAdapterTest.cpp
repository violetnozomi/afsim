#include "nrm/CustomerNrmAdapter.hpp"
#include "nrm/EffectiveSnapshotAssembler.hpp"

#include <cassert>

namespace
{
class Host final : public nrm::CustomerNrmHost
{
public:
   const nrm::ResourceSnapshot& CurrentSnapshot() const override { return snapshot; }
   const nrm::ResourceSnapshot& CurrentCustomerOverlay() const override
   {
      return customerOverlay;
   }
   std::string ActiveConfigVersion() const override { return "demo-0.7.0"; }
   void PublishCustomerSnapshot(const nrm::ResourceSnapshot& aSnapshot) override
   {
      customerOverlay = aSnapshot;
      snapshot = hasAfsimBase
                    ? nrm::EffectiveSnapshotAssembler::Compose(
                         &afsimBase, &customerOverlay)
                    : customerOverlay;
      ++published;
   }
   void ApplyCustomerEnvironmentContext(
      const nrm::EnvironmentContext& aContext) override
   {
      environment = aContext;
   }
   void BeginCustomerRun() override
   {
      environment = nrm::EnvironmentContext();
      const std::uint64_t previousVersion = snapshot.snapshotVersion;
      customerOverlay = nrm::ResourceSnapshot();
      customerOverlay.snapshotVersion = previousVersion;
      ++runTransitions;
   }
   nrm::AssessmentResult RunCustomerAssessment(
      const nrm::AssessmentTask& aTask) override
   {
      ++assessmentCalls;
      nrm::AssessmentResult result;
      result.taskId = aTask.taskId;
      result.snapshotVersion = snapshot.snapshotVersion;
      return result;
   }
   nrm::CustomerPlanEvaluationResult RunCustomerPlan(
      const nrm::NetworkPlanDocument& aPlan) override
   {
      ++planCalls;
      lastPlan = aPlan;
      nrm::CustomerPlanEvaluationResult result;
      result.accepted = true;
      result.evaluation.planId = aPlan.planId;
      return result;
   }
   nrm::ResourceDemandBatchResult RunCustomerDemands(
      const nrm::ResourceDemandSet& aDemands) override
   {
      ++demandCalls;
      lastDemands = aDemands;
      nrm::ResourceDemandBatchResult result;
      result.demandSetId = aDemands.demandSetId;
      result.totalCount = aDemands.demands.size();
      return result;
   }

   nrm::ResourceSnapshot snapshot;
   nrm::ResourceSnapshot afsimBase;
   nrm::ResourceSnapshot customerOverlay;
   bool hasAfsimBase = false;
   nrm::EnvironmentContext environment;
   nrm::NetworkPlanDocument lastPlan;
   nrm::ResourceDemandSet lastDemands;
   std::size_t published = 0;
   std::size_t runTransitions = 0;
   std::size_t assessmentCalls = 0;
   std::size_t planCalls = 0;
   std::size_t demandCalls = 0;
};

nrm::CustomerCallContext Context(const char* aRun, const char* aMessage,
                                 double aSimTime)
{
   nrm::CustomerCallContext context;
   context.runId = aRun;
   context.messageId = aMessage;
   context.providerId = "customer-afsim";
   context.simTime = aSimTime;
   context.hasSimTime = true;
   return context;
}
} // namespace

int main()
{
   Host host;
   nrm::CustomerNrmAdapter adapter(host, 8);

   nrm::NavigationSample invalidNavigation;
   const nrm::CustomerIngestResult rejected = adapter.UpdateNavigation(
      Context("run-invalid", "nav-invalid", 1.0), invalidNavigation);
   assert(rejected.status == nrm::CustomerIngestStatus::cREJECTED);
   assert(rejected.reason == nrm::CustomerIngestReason::cNAVIGATION_INVALID);
   assert(!rejected.mutated);
   assert(host.published == 0);
   assert(adapter.CurrentRunId().empty());

   nrm::ResourceSnapshot resources;
   resources.simTime = 10.0;
   resources.providerId = "customer-afsim";
   nrm::NetworkSnapshot network;
   network.networkId = "link16-primary";
   network.networkType = nrm::NetworkType::cLINK16;
   resources.networks.push_back(network);
   nrm::EndpointSnapshot endpoint;
   endpoint.endpointId = "endpoint-1";
   endpoint.platformId = "platform-1";
   endpoint.platformName = "platform-1";
   endpoint.networkId = "link16-primary";
   endpoint.networkType = nrm::NetworkType::cLINK16;
   resources.endpoints.push_back(endpoint);
   const nrm::CustomerIngestResult accepted =
      adapter.UpdateResources(Context("run-a", "resource-1", 10.0), resources);
   assert(accepted.status == nrm::CustomerIngestStatus::cACCEPTED);
   assert(accepted.mutated);
   assert(accepted.snapshotVersion == 1);
   assert(host.snapshot.networks.size() == 1);
   assert(host.snapshot.providerId == "customer-afsim");

   nrm::ResourceSnapshot invalidResources = resources;
   invalidResources.networks.push_back(network);
   const nrm::CustomerIngestResult invalidResourceResult =
      adapter.UpdateResources(
         Context("run-a", "resource-invalid", 10.5), invalidResources);
   assert(invalidResourceResult.status == nrm::CustomerIngestStatus::cREJECTED);
   assert(invalidResourceResult.reason ==
          nrm::CustomerIngestReason::cRESOURCE_INVALID);
   assert(!invalidResourceResult.mutated);
   assert(host.published == 1);

   const nrm::CustomerIngestResult duplicate =
      adapter.UpdateResources(Context("run-a", "resource-1", 10.0), resources);
   assert(duplicate.status == nrm::CustomerIngestStatus::cDUPLICATE);
   assert(!duplicate.mutated);
   assert(host.published == 1);

   nrm::NavigationSample firstNavigation;
   firstNavigation.platformId = "aircraft-01";
   firstNavigation.platformName = "aircraft-01";
   firstNavigation.sampleTime = 11.0;
   firstNavigation.valid = true;
   assert(adapter.UpdateNavigation(
      Context("run-a", "nav-1", 11.0), firstNavigation).mutated);
   nrm::NavigationSample secondNavigation = firstNavigation;
   secondNavigation.platformId = "aircraft-02";
   secondNavigation.platformName = "aircraft-02";
   secondNavigation.sampleTime = 12.0;
   assert(adapter.UpdateNavigation(
      Context("run-a", "nav-2", 12.0), secondNavigation).mutated);
   assert(host.snapshot.navigation.platforms.size() == 2);
   assert(host.snapshot.networks.size() == 1);
   assert(host.snapshot.navigation.providerId == "customer-afsim");

   const nrm::CustomerIngestResult stale = adapter.UpdateNavigation(
      Context("run-a", "nav-old", 10.5), firstNavigation);
   assert(stale.status == nrm::CustomerIngestStatus::cSTALE);
   assert(!stale.mutated);

   nrm::EnvironmentSnapshot environment;
   environment.sampleTime = 13.0;
   environment.valid = true;
   environment.weather.available = true;
   environment.weather.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
   nrm::EnvironmentContext environmentContext;
   environmentContext.valid = true;
   environmentContext.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
   environmentContext.applicationMode =
      nrm::EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT;
   environmentContext.applyParameterizedEffects = true;
   assert(adapter.UpdateEnvironment(
      Context("run-a", "env-1", 13.0), environment,
      environmentContext).mutated);
   assert(host.environment.applyParameterizedEffects);
   assert(host.environment.customerProvidedDomains.size() == 1);
   assert(host.environment.customerProvidedDomains.front() ==
          nrm::EnvironmentDomain::cWEATHER);
   assert(host.snapshot.navigation.platforms.size() == 2);
   assert(host.snapshot.networks.size() == 1);

   nrm::ResourceSnapshot refreshedResources = resources;
   refreshedResources.simTime = 14.0;
   assert(adapter.UpdateResources(
      Context("run-a", "resource-refresh", 14.0), refreshedResources).mutated);
   assert(host.snapshot.navigation.platforms.size() == 2);
   assert(host.snapshot.environment.valid);
   assert(host.environment.applicationMode ==
          nrm::EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT);

   nrm::AssessmentTask task;
   task.taskId = "task-1";
   assert(adapter.Evaluate(task).taskId == "task-1");
   assert(host.assessmentCalls == 1);

   nrm::NetworkPlanDocument plan;
   plan.planId = "plan-1";
   plan.revision = 1;
   assert(adapter.EvaluatePlan(plan).accepted);
   assert(host.planCalls == 1);
   assert(host.lastPlan.configVersion == "demo-0.7.0");

   nrm::ResourceDemandSet demands;
   demands.demandSetId = "demands-1";
   demands.revision = 1;
   demands.demands.push_back(nrm::ResourceDemand());
   assert(adapter.EvaluateDemands(demands).totalCount == 1);
   assert(host.demandCalls == 1);
   assert(host.lastDemands.configVersion == "demo-0.7.0");

   nrm::ResourceSnapshot nextRunResources;
   nextRunResources.simTime = 1.0;
   const nrm::CustomerIngestResult nextRun = adapter.UpdateResources(
      Context("run-b", "resource-2", 1.0), nextRunResources);
   assert(nextRun.mutated);
   assert(host.runTransitions == 1);
   assert(!host.environment.applyParameterizedEffects);
   assert(host.snapshot.navigation.platforms.empty());
   assert(host.snapshot.snapshotVersion == 6);

   // A customer update for Y must not copy the AFSIM sample for X into the
   // customer-owned overlay.  Later AFSIM X updates therefore stay visible,
   // while customer Y continues to override AFSIM Y.
   Host overlayHost;
   overlayHost.hasAfsimBase = true;
   overlayHost.afsimBase.navigation.valid = true;
   overlayHost.afsimBase.navigation.providerId = "afsim-navigation";
   nrm::NavigationSample afsimX;
   afsimX.platformId = "platform-x";
   afsimX.platformName = "platform-x";
   afsimX.valid = true;
   afsimX.sampleTime = 10.0;
   afsimX.truthLatitudeDeg.valid = true;
   afsimX.truthLatitudeDeg.value = 10.0;
   nrm::NavigationSample afsimY = afsimX;
   afsimY.platformId = "platform-y";
   afsimY.platformName = "platform-y";
   overlayHost.afsimBase.navigation.platforms = {afsimX, afsimY};
   overlayHost.snapshot = overlayHost.afsimBase;

   nrm::CustomerNrmAdapter overlayAdapter(overlayHost, 8);
   nrm::NavigationSample customerY = afsimY;
   customerY.origin = nrm::DataOrigin::cCUSTOMER_MODULE;
   customerY.sampleTime = 11.0;
   customerY.truthLatitudeDeg.value = 11.0;
   assert(overlayAdapter.UpdateNavigation(
      Context("run-overlay", "nav-y", 11.0), customerY).mutated);
   assert(overlayHost.customerOverlay.navigation.platforms.size() == 1);
   assert(overlayHost.customerOverlay.navigation.platforms.front().platformId ==
          "platform-y");

   overlayHost.afsimBase.navigation.platforms.at(0).sampleTime = 12.0;
   overlayHost.afsimBase.navigation.platforms.at(0).truthLatitudeDeg.value = 12.0;
   overlayHost.afsimBase.navigation.platforms.at(1).sampleTime = 12.0;
   overlayHost.afsimBase.navigation.platforms.at(1).truthLatitudeDeg.value = 12.0;
   overlayHost.snapshot = nrm::EffectiveSnapshotAssembler::Compose(
      &overlayHost.afsimBase, &overlayHost.customerOverlay);
   assert(overlayHost.snapshot.navigation.platforms.size() == 2);
   assert(overlayHost.snapshot.navigation.platforms.at(0).truthLatitudeDeg.value ==
          12.0);
   assert(overlayHost.snapshot.navigation.platforms.at(1).truthLatitudeDeg.value ==
          11.0);
   return 0;
}
