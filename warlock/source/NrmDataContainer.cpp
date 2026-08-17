#include "NrmDataContainer.hpp"
#include "NrmCustomerPlanIngest.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "NrmSnapshotReporter.hpp"
#include "nrm/ContractMetricEnricher.hpp"
#include "nrm/NavigationAccuracyModel.hpp"
#include "nrm/NetworkProfileRepository.hpp"
#include "nrm/Version.hpp"

namespace
{
nrm::NetworkProfileRepository LoadProfiles()
{
   nrm::NetworkProfileRepository profiles =
      nrm::NetworkProfileRepository::BuiltInDemo();
   const QByteArray profilePath = qgetenv("NRM_NETWORK_PROFILE_CONFIG");
   if (!profilePath.isEmpty())
   {
      profiles = nrm::NetworkProfileRepository();
      nrm::NetworkProfileValidation validation;
      profiles.LoadFromFile(profilePath.constData(), validation);
   }
   return profiles;
}

nrm::EnvironmentConfigRepository LoadEnvironmentConfig()
{
   nrm::EnvironmentConfigRepository config =
      nrm::EnvironmentConfigRepository::BuiltInDemo();
   const QByteArray configPath = qgetenv("NRM_ENVIRONMENT_CONFIG");
   if (!configPath.isEmpty())
   {
      nrm::EnvironmentConfigRepository external;
      nrm::EnvironmentConfigValidation validation;
      if (external.LoadFromFile(configPath.constData(), validation))
      {
         config = external;
      }
   }
   return config;
}

nrm::CustomerMessageDomain MessageDomain(const std::string& aSchema)
{
   if (aSchema == "nrm.customer.resource_report.v1")
      return nrm::CustomerMessageDomain::cRESOURCE;
   if (aSchema == "nrm.customer.navigation_report.v1")
      return nrm::CustomerMessageDomain::cNAVIGATION;
   if (aSchema == "nrm.customer.environment_report.v1")
      return nrm::CustomerMessageDomain::cENVIRONMENT;
   if (aSchema == "nrm.customer.network_plan.v1")
      return nrm::CustomerMessageDomain::cPLAN;
   if (aSchema == "nrm.customer.assessment_request.v1")
      return nrm::CustomerMessageDomain::cASSESSMENT;
   if (aSchema == "nrm.customer.resource_demand_request.v1")
      return nrm::CustomerMessageDomain::cDEMAND;
   if (aSchema == "nrm.customer.membership_request.v1")
      return nrm::CustomerMessageDomain::cMEMBERSHIP;
   return nrm::CustomerMessageDomain::cPROVIDER;
}
} // namespace

WkNrm::DataContainer::DataContainer(QObject* aParentPtr)
   : QObject(aParentPtr)
   , mProfiles(LoadProfiles())
   , mEnvironmentConfig(LoadEnvironmentConfig())
   , mEnvironmentAdapter(mEnvironmentConfig)
   , mModelServiceFacade(mProfiles, &mEnvironmentAdapter)
   , mCustomerNrmAdapter(*this, mCustomerIngestionState)
{
   mModelRegistration = mModelRegistry.Register(
      nrm::ModelServiceFacade::Descriptor());
   QByteArray outputDirectory = qgetenv("NRM_OUTPUT_DIR");
   if (outputDirectory.isEmpty())
   {
      outputDirectory = "/tmp/nrm-output";
   }
   QDir().mkpath(QString::fromLocal8Bit(outputDirectory));
   const std::string configVersion =
      mProfiles.ConfigVersion().empty() ? "invalid-external-profile"
                                        : mProfiles.ConfigVersion();
   mReporterPtr.reset(new SnapshotReporter(outputDirectory.constData(), configVersion));
}

WkNrm::DataContainer::~DataContainer() = default;

void WkNrm::DataContainer::PublishCustomerSnapshot(
   const nrm::ResourceSnapshot& aSnapshot)
{
   mCustomerOverlaySnapshot = aSnapshot;
   mHasCustomerOverlaySnapshot = true;
   RebuildEffectiveSnapshot();
}

void WkNrm::DataContainer::ApplyCustomerEnvironmentContext(
   const nrm::EnvironmentContext& aContext)
{
   mCustomerEnvironmentContext = aContext;
}

void WkNrm::DataContainer::BeginCustomerRun()
{
   mCustomerEnvironmentContext = nrm::EnvironmentContext();
   mCustomerOverlaySnapshot = nrm::FrameworkSnapshot();
   mHasCustomerOverlaySnapshot = false;
}

nrm::AssessmentResult WkNrm::DataContainer::RunCustomerAssessment(
   const nrm::AssessmentTask& aTask)
{
   return EvaluateAssessment(aTask);
}

nrm::CustomerPlanEvaluationResult WkNrm::DataContainer::RunCustomerPlan(
   const nrm::NetworkPlanDocument& aPlan)
{
   nrm::CustomerPlanEvaluationResult result;
   result.accepted = ReplaceNetworkPlanDraft(aPlan);
   if (!result.accepted) return result;
   result.validation = ValidateNetworkPlan();
   result.evaluation = EvaluateNetworkPlan();
   return result;
}

nrm::ResourceDemandBatchResult WkNrm::DataContainer::RunCustomerDemands(
   const nrm::ResourceDemandSet& aDemands)
{
   if (!ReplaceResourceDemandDraft(aDemands))
      return nrm::ResourceDemandBatchResult();
   return EvaluateResourceDemands();
}

void WkNrm::DataContainer::SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot)
{
   mAfsimBaseSnapshot = aSnapshot;
   mHasAfsimBaseSnapshot = true;
   RebuildEffectiveSnapshot();
}

void WkNrm::DataContainer::RebuildEffectiveSnapshot()
{
   const nrm::ResourceSnapshot* afsimPtr =
      mHasAfsimBaseSnapshot ? &mAfsimBaseSnapshot : nullptr;
   const nrm::ResourceSnapshot* customerPtr =
      mHasCustomerOverlaySnapshot ? &mCustomerOverlaySnapshot : nullptr;
   ApplyEffectiveSnapshot(
      nrm::EffectiveSnapshotAssembler::Compose(afsimPtr, customerPtr));
}

void WkNrm::DataContainer::ApplyEffectiveSnapshot(
   nrm::FrameworkSnapshot aSnapshot)
{
   const std::uint64_t nextVersion = mEffectiveSnapshotVersion + 1;
   aSnapshot.snapshotVersion = std::max(aSnapshot.snapshotVersion, nextVersion);
   mEffectiveSnapshotVersion = aSnapshot.snapshotVersion;
   mSnapshot = std::move(aSnapshot);
   const nrm::NavigationAccuracyModel navigationAccuracy;
   for (nrm::NavigationSample& sample : mSnapshot.navigation.platforms)
   {
      navigationAccuracy.Enrich(sample, sample.navigationType);
   }
   nrm::ContractMetricEnricher(mProfiles).Apply(mSnapshot);
   // Every derived result is tied to a snapshot version. Never present a
   // result computed from the previous graph as current after an update.
   mHasAssessment = false;
   mHasCapability = false;
   mHasPlanValidation = false;
   mHasPlanEvaluation = false;
   mHasDistributionPackage = false;
   mHasDemandMatching = false;
   mReporterPtr->Enqueue(mSnapshot);
   emit SnapshotChanged();
}

void WkNrm::DataContainer::StoreAssessment(const nrm::AssessmentResult& aResult)
{
   mAssessment = aResult;
   mHasAssessment = true;
   mReporterPtr->EnqueueAssessment(aResult);
   emit AssessmentChanged();
}

nrm::AssessmentResult WkNrm::DataContainer::EvaluateAssessment(
   const nrm::AssessmentTask& aTask)
{
   const nrm::AssessmentServiceResponse response =
      mModelServiceFacade.EvaluateAssessment(
         MakeModelServiceContext(
            nrm::ModelServiceOperation::cEVALUATE_ASSESSMENT,
            mSnapshot.snapshotVersion),
         mSnapshot, aTask, EffectiveEnvironment(nrm::EnvironmentContext()));
   if (response.valid)
   {
      StoreAssessment(response.result);
   }
   return response.result;
}

nrm::CapabilityResult WkNrm::DataContainer::QueryCapability(
   const nrm::CapabilityRequest& aRequest,
   const nrm::EnvironmentContext& aEnvironment)
{
   const nrm::CapabilityServiceResponse response =
      mModelServiceFacade.QueryCapability(
         MakeModelServiceContext(nrm::ModelServiceOperation::cQUERY_CAPABILITY,
                                 mSnapshot.snapshotVersion),
         mSnapshot, aRequest, EffectiveEnvironment(aEnvironment));
   mCapability = response.result;
   mHasCapability = response.valid;
   if (mHasCapability)
      mReporterPtr->EnqueueCapability(mCapability);
   emit CapabilityChanged();
   return mCapability;
}

namespace
{
nrm::PlanValidationResult NoCurrentPlanValidation()
{
   nrm::PlanValidationResult validation;
   nrm::PlanValidationIssue issue;
   issue.reason = nrm::PlanValidationReason::cNO_CURRENT_PLAN;
   issue.field = "plan";
   issue.severity = nrm::PlanIssueSeverity::cERROR;
   issue.description = "No network plan is loaded.";
   validation.issues.push_back(issue);
   return validation;
}

void AddPlanReason(nrm::PlanDemandEvaluation& aEvaluation,
                   nrm::PlanValidationReason aReason)
{
   if (std::find(aEvaluation.reasons.begin(), aEvaluation.reasons.end(), aReason) ==
       aEvaluation.reasons.end())
      aEvaluation.reasons.push_back(aReason);
}

void AddPlanRecommendation(nrm::PlanDemandEvaluation& aEvaluation,
                           const std::string& aRecommendation)
{
   if (!aRecommendation.empty() &&
       std::find(aEvaluation.recommendations.begin(),
                 aEvaluation.recommendations.end(), aRecommendation) ==
          aEvaluation.recommendations.end())
      aEvaluation.recommendations.push_back(aRecommendation);
}

void MergeConcurrentAssessment(
   nrm::NetworkPlanEvaluationResult& aEvaluation,
   const nrm::ConcurrentAssessmentResult& aConcurrent)
{
   if (!aConcurrent.valid) return;
   for (const nrm::ConcurrentTaskResult& task : aConcurrent.tasks)
   {
      if (task.allocated) continue;
      auto found = std::find_if(
         aEvaluation.demands.begin(), aEvaluation.demands.end(),
         [&task](const nrm::PlanDemandEvaluation& demand)
         {
            return demand.demandId == task.taskId;
         });
      if (found == aEvaluation.demands.end() ||
          found->status == nrm::PlanEvaluationStatus::cNOT_EVALUATED)
         continue;

      if (task.independent.canComplete && !task.concurrent.canComplete)
         AddPlanReason(*found, nrm::PlanValidationReason::cRESOURCE_CONFLICT);
      else
         AddPlanReason(*found, nrm::PlanValidationReason::cEVALUATION_FAILED);
      if (found->status != nrm::PlanEvaluationStatus::cDATA_INVALID)
         found->status = nrm::PlanEvaluationStatus::cFAIL;
      for (const std::string& recommendation : task.concurrent.recommendations)
         AddPlanRecommendation(*found, recommendation);
      if (found->recommendations.empty())
      {
         AddPlanRecommendation(
            *found,
            "当前任务未能分配并发资源；请按失败原因调整链路资源或业务约束后重新推演。");
      }
   }

   bool hasInvalid = false;
   bool hasFailure = false;
   for (const nrm::PlanDemandEvaluation& demand : aEvaluation.demands)
   {
      hasInvalid = hasInvalid ||
                   demand.status == nrm::PlanEvaluationStatus::cDATA_INVALID;
      hasFailure = hasFailure || demand.status == nrm::PlanEvaluationStatus::cFAIL;
   }
   if (hasInvalid)
      aEvaluation.overallStatus = nrm::PlanEvaluationStatus::cDATA_INVALID;
   else if (hasFailure)
      aEvaluation.overallStatus = nrm::PlanEvaluationStatus::cFAIL;
   else
      aEvaluation.overallStatus = nrm::PlanEvaluationStatus::cPASS;
   aEvaluation.resultingState =
      aEvaluation.overallStatus == nrm::PlanEvaluationStatus::cPASS
         ? nrm::NetworkPlanState::cVALIDATED
         : nrm::NetworkPlanState::cREJECTED;
}
}

bool WkNrm::DataContainer::LoadNetworkPlan(const std::string& aPath)
{
   const bool loaded = mPlanRepository.LoadFromFile(aPath);
   mPlanOperation = mPlanRepository.LastLoadResult();
   if (!loaded)
      mReporterPtr->ReportPlanError(mPlanOperation.reason, mPlanOperation.field);
   if (loaded)
   {
      mHasPlanValidation = false;
      mHasPlanEvaluation = false;
      mHasDistributionPackage = false;
      mHasDemandMatching = false;
   }
   emit NetworkPlanChanged();
   return loaded;
}

bool WkNrm::DataContainer::LoadCustomerJson(const std::string& aPath)
{
   mLastCustomerJsonResponse.clear();
   QFile input(QString::fromStdString(aPath));
   if (!input.open(QIODevice::ReadOnly))
   {
      mLastCustomerJsonResult = CustomerJsonDecodeResult();
      mLastCustomerJsonResult.errors.push_back(
         {"FILE_OPEN_FAILED", "/", "无法打开甲方JSON文件"});
   }
   else
   {
      const QByteArray json = input.readAll();
      mLastCustomerJsonResult = mCustomerJsonCodec.Inspect(json);
      if (mLastCustomerJsonResult.valid)
      {
         const std::string& schema = mLastCustomerJsonResult.envelope.schema;
         const nrm::CustomerMessageDomain domain = MessageDomain(schema);
         const nrm::CustomerIngestionDecision decision =
            mCustomerIngestionState.Preview(
               mLastCustomerJsonResult.envelope.runId,
               mLastCustomerJsonResult.envelope.messageId,
               domain,
               mLastCustomerJsonResult.envelope.hasSimTime,
               mLastCustomerJsonResult.envelope.simTime);
         const bool execute =
            decision.status == nrm::CustomerIngestionStatus::cACCEPTED;
         bool committed = false;
         const auto commit = [&]()
         {
            if (committed) return;
            mCustomerIngestionState.Commit(
               mLastCustomerJsonResult.envelope.runId,
               mLastCustomerJsonResult.envelope.messageId,
               domain, mLastCustomerJsonResult.envelope.hasSimTime,
               mLastCustomerJsonResult.envelope.simTime);
            committed = true;
         };
         if (!execute)
         {
            const bool duplicate =
               decision.status == nrm::CustomerIngestionStatus::cDUPLICATE;
            mLastCustomerJsonResponse = mCustomerJsonCodec.EncodeIngestAck(
               mLastCustomerJsonResult.envelope,
               duplicate ? CustomerIngestStatus::cDUPLICATE
                         : CustomerIngestStatus::cSTALE,
               duplicate ? "重复消息已忽略" : "陈旧消息已忽略");
         }
         else if (schema == "nrm.customer.network_plan.v1")
         {
            nrm::NetworkPlanDocument plan;
            mLastCustomerJsonResult = mCustomerJsonCodec.DecodeNetworkPlan(json, plan);
            if (mLastCustomerJsonResult.valid)
            {
               commit();
               plan.configVersion = mProfiles.ConfigVersion();
               const bool accepted = AcceptDecodedNetworkPlan(
                  mLastCustomerJsonResult, plan, mPlanRepository);
               mPlanOperation = mPlanRepository.LastLoadResult();
               if (accepted)
               {
                  mHasPlanValidation = false;
                  mHasPlanEvaluation = false;
                  mHasDistributionPackage = false;
                  mHasDemandMatching = false;
               }
               emit NetworkPlanChanged();
            }
         }
         else if (schema == "nrm.customer.navigation_report.v1")
         {
            nrm::NavigationSample sample;
            mLastCustomerJsonResult = mCustomerJsonCodec.DecodeNavigation(json, sample);
            if (mLastCustomerJsonResult.valid)
            {
               commit();
               if (decision.newRun)
                  mCustomerEnvironmentContext = nrm::EnvironmentContext();
               PublishCustomerSnapshot(nrm::CustomerSnapshotAssembler::UpsertNavigation(
                  mSnapshot, sample, mLastCustomerJsonResult.envelope.source,
                  decision.newRun));
            }
         }
         else if (schema == "nrm.customer.environment_report.v1")
         {
            nrm::EnvironmentSnapshot environment;
            nrm::EnvironmentContext context;
            mLastCustomerJsonResult = mCustomerJsonCodec.DecodeEnvironment(json, environment, context);
            if (mLastCustomerJsonResult.valid)
            {
               commit();
               mCustomerEnvironmentContext = context;
               PublishCustomerSnapshot(nrm::CustomerSnapshotAssembler::MergeEnvironment(
                  mSnapshot, environment, decision.newRun));
            }
         }
         else if (schema == "nrm.customer.resource_report.v1")
         {
            nrm::ResourceSnapshot snapshot;
            mLastCustomerJsonResult = mCustomerJsonCodec.DecodeResources(json, snapshot);
            if (mLastCustomerJsonResult.valid)
            {
               commit();
               if (decision.newRun)
                  mCustomerEnvironmentContext = nrm::EnvironmentContext();
               PublishCustomerSnapshot(nrm::CustomerSnapshotAssembler::MergeResources(
                  mSnapshot, snapshot, decision.newRun));
            }
         }
         else if (schema == "nrm.customer.assessment_request.v1")
         {
            nrm::AssessmentTask task;
            mLastCustomerJsonResult = mCustomerJsonCodec.DecodeAssessment(json, task);
            if (mLastCustomerJsonResult.valid)
            {
               commit();
               const nrm::AssessmentResult result =
                  mCustomerNrmAdapter.Evaluate(task);
               mLastCustomerJsonResponse = mCustomerJsonCodec.EncodeAssessment(
                  mLastCustomerJsonResult.envelope, result);
            }
         }
         else if (schema == "nrm.customer.resource_demand_request.v1")
         {
            nrm::ResourceDemandSet demandSet;
            mLastCustomerJsonResult =
               mCustomerJsonCodec.DecodeResourceDemands(json, demandSet);
            if (mLastCustomerJsonResult.valid)
            {
               const nrm::ResourceDemandBatchResult matching =
                  mCustomerNrmAdapter.EvaluateDemands(demandSet);
               if (mDemandOperation.success)
               {
                  commit();
                  mLastCustomerJsonResponse =
                     mCustomerJsonCodec.EncodeResourceDemandResult(
                        mLastCustomerJsonResult.envelope, matching);
               }
               else
               {
                  mLastCustomerJsonResult.valid = false;
                  mLastCustomerJsonResult.errors.push_back(
                     {"RESOURCE_DEMAND_REJECTED", "/data",
                      nrm::ToString(mDemandOperation.reason)});
               }
            }
         }
         else if (schema == "nrm.customer.provider_hello.v1")
         {
            CustomerProviderHello hello;
            mLastCustomerJsonResult = mCustomerJsonCodec.DecodeProviderHello(json, hello);
            if (mLastCustomerJsonResult.valid)
            {
               commit();
               mLastCustomerProvider = hello;
               mHasCustomerProvider = true;
            }
         }
         else if (schema == "nrm.customer.membership_request.v1")
         {
            nrm::NetworkPlanChange change;
            mLastCustomerJsonResult = mCustomerJsonCodec.DecodeMembership(json, change);
            const nrm::NetworkPlanDocument* current = mPlanRepository.GetCurrentPlan();
            if (mLastCustomerJsonResult.valid && current != nullptr)
            {
               commit();
               const nrm::PlanCoordinationResult coordination =
                  mPlanCoordinationService.ApplyMembership(*current, change);
               mPlanCoordination = nrm::PlanCoordinationEvidence();
               mPlanCoordination.operation =
                  change.changeType == nrm::PlanChangeType::cJOIN
                     ? "MEMBERSHIP_JOIN" : "MEMBERSHIP_LEAVE";
               mPlanCoordination.requestId = change.changeId;
               mPlanCoordination.planId = change.planId;
               mPlanCoordination.revision = coordination.success
                  ? coordination.revisedPlan.revision : current->revision;
               mPlanCoordination.success = coordination.success;
               mPlanCoordination.reason = coordination.reason;
               mHasPlanCoordination = true;
               mReporterPtr->EnqueuePlanningCoordination(mPlanCoordination);
               if (!coordination.success || !ReplaceNetworkPlanDraft(coordination.revisedPlan))
               {
                  mLastCustomerJsonResult.valid = false;
                  mLastCustomerJsonResult.errors.push_back(
                     {"MEMBERSHIP_REJECTED", "/data", ToString(coordination.reason)});
               }
            }
            else if (mLastCustomerJsonResult.valid)
            {
               mLastCustomerJsonResult.valid = false;
               mLastCustomerJsonResult.errors.push_back(
                  {"NO_CURRENT_PLAN", "/data/planId", "当前没有可变更的资源规划"});
            }
         }
         else
         {
            mLastCustomerJsonResult.valid = false;
            mLastCustomerJsonResult.errors.push_back(
               {"SCHEMA_UNSUPPORTED", "/schema", "该消息不能通过文件加载入口执行"});
         }
      }
   }
   if (mLastCustomerJsonResponse.isEmpty())
   {
      mLastCustomerJsonResponse = mLastCustomerJsonResult.valid
         ? mCustomerJsonCodec.EncodeIngestAck(
              mLastCustomerJsonResult.envelope,
              CustomerIngestStatus::cACCEPTED, "消息已接收并处理")
         : mCustomerJsonCodec.EncodeError(
              mLastCustomerJsonResult.envelope, mLastCustomerJsonResult.errors);
   }
   const CustomerJsonError error = mLastCustomerJsonResult.errors.empty()
                                      ? CustomerJsonError()
                                      : mLastCustomerJsonResult.errors.front();
   mReporterPtr->ReportCustomerInterfaceEvent(
      mLastCustomerJsonResult.envelope.schema,
      mLastCustomerJsonResult.envelope.messageId,
      QFileInfo(QString::fromStdString(aPath)).fileName().toStdString(),
      mLastCustomerJsonResult.valid, error.code, error.path);
   return mLastCustomerJsonResult.valid;
}

bool WkNrm::DataContainer::ReplaceNetworkPlanDraft(
   const nrm::NetworkPlanDocument& aDocument)
{
   const bool replaced = mPlanRepository.ReplaceDraft(aDocument);
   mPlanOperation = mPlanRepository.LastLoadResult();
   if (replaced)
   {
      mHasPlanValidation = false;
      mHasPlanEvaluation = false;
      mHasDistributionPackage = false;
      mHasDemandMatching = false;
   }
   emit NetworkPlanChanged();
   return replaced;
}

void WkNrm::DataContainer::UnloadNetworkPlan()
{
   mPlanRepository.Unload();
   mPlanOperation = nrm::PlanRepositoryResult();
   mPlanValidation = nrm::PlanValidationResult();
   mPlanEvaluation = nrm::NetworkPlanEvaluationResult();
   mDistributionPackage = nrm::DistributionPackageResult();
   mHasPlanValidation = false;
   mHasPlanEvaluation = false;
   mHasDistributionPackage = false;
   mHasDemandMatching = false;
   emit NetworkPlanChanged();
}

bool WkNrm::DataContainer::SaveNetworkPlanRevision(const std::string& aPath)
{
   const bool saved = mPlanRepository.SaveRevision(aPath);
   mPlanOperation = mPlanRepository.LastSaveResult();
   if (!saved)
      mReporterPtr->ReportPlanError(mPlanOperation.reason, mPlanOperation.field);
   emit NetworkPlanChanged();
   return saved;
}

nrm::PlanValidationResult WkNrm::DataContainer::ValidateNetworkPlan()
{
   const nrm::NetworkPlanDocument* planPtr = mPlanRepository.GetCurrentPlan();
   if (planPtr == nullptr)
   {
      mPlanValidation = NoCurrentPlanValidation();
      mHasPlanValidation = true;
   }
   else
   {
      const nrm::PlanValidationServiceResponse response =
         mModelServiceFacade.ValidatePlan(
            MakeModelServiceContext(nrm::ModelServiceOperation::cVALIDATE_PLAN,
                                    mSnapshot.snapshotVersion),
            mSnapshot, *planPtr);
      mPlanValidation = response.result;
      mHasPlanValidation = response.valid;
   }
   mHasPlanEvaluation = false;
   mHasDistributionPackage = false;
   mHasDemandMatching = false;
   if (mHasPlanValidation)
      mReporterPtr->EnqueuePlanValidation(mPlanValidation);
   emit NetworkPlanChanged();
   return mPlanValidation;
}

nrm::NetworkPlanEvaluationResult WkNrm::DataContainer::EvaluateNetworkPlan(
   const nrm::EnvironmentContext& aEnvironment)
{
   const nrm::NetworkPlanDocument* planPtr = mPlanRepository.GetCurrentPlan();
   if (planPtr == nullptr)
   {
      mPlanEvaluation = nrm::NetworkPlanEvaluationResult();
      mPlanEvaluation.overallStatus = nrm::PlanEvaluationStatus::cDATA_INVALID;
      mPlanEvaluation.resultingState = nrm::NetworkPlanState::cREJECTED;
      mPlanEvaluation.validation = NoCurrentPlanValidation();
      mHasPlanEvaluation = true;
   }
   else
   {
      const nrm::PlanEvaluationServiceResponse response =
         mModelServiceFacade.EvaluatePlan(
            MakeModelServiceContext(nrm::ModelServiceOperation::cEVALUATE_PLAN,
                                    mSnapshot.snapshotVersion),
            mSnapshot, *planPtr, EffectiveEnvironment(aEnvironment));
      mPlanEvaluation = response.result;
      mHasPlanEvaluation = response.valid;
      std::vector<nrm::AssessmentTask> tasks;
      for (const nrm::NetworkPlanDemand& demand : planPtr->demands)
      {
         nrm::AssessmentTask task;
         task.taskId = demand.demandId;
         task.sourcePlatform = demand.sourcePlatform;
         task.destinationPlatform = demand.destinationPlatform;
         task.businessType = demand.businessType;
         task.payloadBits = demand.payloadBits;
         task.requiredBandwidthBps = demand.requiredBandwidthBps;
         task.maximumDelayMs = demand.maximumDelayMs;
         task.minimumPdrPercent = demand.minimumPdrPercent;
         task.allowedNetworks = demand.allowedNetworks;
         tasks.push_back(task);
      }
      mConcurrentAssessment = nrm::ConcurrentTaskAssessment(mProfiles).Evaluate(
         mSnapshot, tasks);
      MergeConcurrentAssessment(mPlanEvaluation, mConcurrentAssessment);
   }
   mPlanValidation = mPlanEvaluation.validation;
   mHasPlanValidation = mHasPlanEvaluation;
   mHasDistributionPackage = false;
   mHasDemandMatching = false;
   if (mHasPlanEvaluation)
   {
      mReporterPtr->EnqueuePlanValidation(mPlanValidation);
      mReporterPtr->EnqueuePlanEvaluation(mPlanEvaluation);
   }
   emit NetworkPlanChanged();
   return mPlanEvaluation;
}

nrm::DistributionPackageResult WkNrm::DataContainer::GenerateNetworkPlanPackage(
   const std::string& aOutputRoot)
{
   const nrm::NetworkPlanDocument* planPtr = mPlanRepository.GetCurrentPlan();
   if (planPtr == nullptr)
   {
      mDistributionPackage = nrm::DistributionPackageResult();
      mDistributionPackage.reason = nrm::PlanValidationReason::cNO_CURRENT_PLAN;
   }
   else if (!mHasPlanValidation || !mHasPlanEvaluation)
   {
      mDistributionPackage = nrm::DistributionPackageResult();
      mDistributionPackage.planId = planPtr->planId;
      mDistributionPackage.revision = planPtr->revision;
      mDistributionPackage.reason = nrm::PlanValidationReason::cPLAN_NOT_VALIDATED;
   }
   else
   {
      const nrm::DistributionPackageServiceResponse response =
         mModelServiceFacade.GenerateDistributionPackage(
            MakeModelServiceContext(
               nrm::ModelServiceOperation::cGENERATE_DISTRIBUTION_PACKAGE,
               mPlanEvaluation.snapshotVersion),
            *planPtr, mPlanValidation, mPlanEvaluation, aOutputRoot);
      mDistributionPackage = response.result;
      mHasDistributionPackage = response.valid;
   }
   if (planPtr == nullptr || !mHasPlanValidation || !mHasPlanEvaluation)
      mHasDistributionPackage = true;
   if (mHasDistributionPackage && !mDistributionPackage.generated &&
       (mDistributionPackage.reason == nrm::PlanValidationReason::cFILE_WRITE_FAILED ||
        mDistributionPackage.reason == nrm::PlanValidationReason::cATOMIC_RENAME_FAILED ||
        mDistributionPackage.reason == nrm::PlanValidationReason::cOUTPUT_PATH_INVALID))
      mReporterPtr->ReportPlanError(mDistributionPackage.reason, "distributionPackage");
   emit NetworkPlanChanged();
   return mDistributionPackage;
}

bool WkNrm::DataContainer::AcknowledgeNetworkPlanPackage(
   const std::string& aAckPath)
{
   const nrm::PlanCoordinationResult result =
      mPlanCoordinationService.AcknowledgeDistribution(mDistributionPackage,
                                                       aAckPath);
   mPlanCoordination = nrm::PlanCoordinationEvidence();
   mPlanCoordination.operation = "DISTRIBUTION_ACK";
   mPlanCoordination.requestId = aAckPath;
   mPlanCoordination.planId = mDistributionPackage.planId;
   mPlanCoordination.revision = mDistributionPackage.revision;
   mPlanCoordination.packageId = mDistributionPackage.outputPath;
   mPlanCoordination.fingerprint = mDistributionPackage.planFingerprint;
   mPlanCoordination.success = result.success;
   mPlanCoordination.acknowledged = result.acknowledged;
   mPlanCoordination.reason = result.reason;
   mHasPlanCoordination = true;
   mReporterPtr->EnqueuePlanningCoordination(mPlanCoordination);
   emit NetworkPlanChanged();
   return result.success;
}

bool WkNrm::DataContainer::LoadResourceDemands(const std::string& aPath)
{
   const bool loaded = mDemandRepository.LoadFromFile(aPath);
   mDemandOperation = mDemandRepository.LastLoadResult();
   if (!loaded)
      mReporterPtr->ReportDemandError(mDemandOperation.reason,
                                      mDemandOperation.field);
   if (loaded)
   {
      mDemandMatching = nrm::ResourceDemandBatchResult();
      mHasDemandMatching = false;
   }
   emit ResourceDemandChanged();
   return loaded;
}

bool WkNrm::DataContainer::ReplaceResourceDemandDraft(
   const nrm::ResourceDemandSet& aDemandSet)
{
   const bool replaced = mDemandRepository.ReplaceDraft(aDemandSet);
   mDemandOperation = mDemandRepository.LastLoadResult();
   if (!replaced)
      mReporterPtr->ReportDemandError(mDemandOperation.reason,
                                      mDemandOperation.field);
   if (replaced)
   {
      mDemandMatching = nrm::ResourceDemandBatchResult();
      mHasDemandMatching = false;
   }
   emit ResourceDemandChanged();
   return replaced;
}

void WkNrm::DataContainer::UnloadResourceDemands()
{
   mDemandRepository.Unload();
   mDemandOperation = nrm::ResourceDemandRepositoryResult();
   mDemandMatching = nrm::ResourceDemandBatchResult();
   mHasDemandMatching = false;
   emit ResourceDemandChanged();
}

bool WkNrm::DataContainer::SaveResourceDemandRevision(const std::string& aPath)
{
   const bool saved = mDemandRepository.SaveRevision(aPath);
   mDemandOperation = mDemandRepository.LastSaveResult();
   if (!saved)
      mReporterPtr->ReportDemandError(mDemandOperation.reason,
                                      mDemandOperation.field);
   emit ResourceDemandChanged();
   return saved;
}

nrm::ResourceDemandBatchResult WkNrm::DataContainer::EvaluateResourceDemands(
   const nrm::EnvironmentContext& aEnvironment,
   const nrm::PlanningCandidateSet* aCandidatesPtr)
{
   const nrm::ResourceDemandSet* demandSetPtr =
      mDemandRepository.GetCurrentDemandSet();
   if (demandSetPtr == nullptr)
   {
      mDemandMatching = nrm::ResourceDemandBatchResult();
      mDemandOperation = nrm::ResourceDemandRepositoryResult();
      mDemandOperation.reason = nrm::ResourceDemandReason::cNO_CURRENT_DEMAND_SET;
      mHasDemandMatching = false;
      emit ResourceDemandChanged();
      return mDemandMatching;
   }

   const nrm::NetworkPlanDocument* planPtr = mPlanRepository.GetCurrentPlan();
   const nrm::NetworkPlanEvaluationResult* evaluationPtr =
      mHasPlanEvaluation ? &mPlanEvaluation : nullptr;
   const nrm::ResourceDemandServiceResponse response =
      mModelServiceFacade.MatchResourceDemands(
         MakeModelServiceContext(
            nrm::ModelServiceOperation::cMATCH_RESOURCE_DEMANDS,
            mSnapshot.snapshotVersion),
         mSnapshot, *demandSetPtr, planPtr, evaluationPtr,
         EffectiveEnvironment(aEnvironment),
         aCandidatesPtr);
   mDemandMatching = response.result;
   mHasDemandMatching = response.valid;
   if (mHasDemandMatching)
      mDemandFeedback = mDemandCoordinator.Record(*demandSetPtr, mDemandMatching);
   if (mHasDemandMatching)
   {
      mReporterPtr->EnqueueDemandResults(mDemandMatching);
      mReporterPtr->EnqueuePlanningRecommendations(mDemandMatching);
      mReporterPtr->EnqueueDemandFeedback(mDemandFeedback);
   }
   emit ResourceDemandChanged();
   return mDemandMatching;
}

nrm::ModelServiceContext WkNrm::DataContainer::MakeModelServiceContext(
   nrm::ModelServiceOperation aOperation,
   std::uint64_t aSnapshotVersion)
{
   nrm::ModelServiceContext context;
   context.requestId = std::string("warlock-") + nrm::ToString(aOperation) + "-" +
                       std::to_string(++mModelServiceRequestSequence);
   context.correlationId = context.requestId;
   context.callerId = "warlock-network-resource-manager";
   context.softwareVersion = nrm::cVERSION;
   context.snapshotVersion = aSnapshotVersion;
   context.requestTime = std::isfinite(mSnapshot.simTime) ? mSnapshot.simTime : 0.0;
   context.source = nrm::DataOrigin::cAFSIM_INTERNAL;
   context.confidence = nrm::Confidence::cHIGH;
   context.valid = true;
   return context;
}

nrm::EnvironmentContext WkNrm::DataContainer::EffectiveEnvironment(
   const nrm::EnvironmentContext& aEnvironment) const
{
   if (aEnvironment.valid) return aEnvironment;
   if (mCustomerEnvironmentContext.valid) return mCustomerEnvironmentContext;
   nrm::EnvironmentContext context;
   context.contextId = "afsim-environment-snapshot-" +
                       std::to_string(mSnapshot.snapshotVersion);
   context.schemaVersion = mSnapshot.environment.schemaVersion;
   context.providerId = mSnapshot.environment.providerId;
   context.sampleTime = mSnapshot.environment.sampleTime;
   context.origin = mSnapshot.environment.origin;
   context.confidence = mSnapshot.environment.confidence;
   context.valid = mSnapshot.environment.valid;
   return context;
}

nrm::NetworkPlanState WkNrm::DataContainer::GetNetworkPlanState() const
{
   const nrm::NetworkPlanDocument* planPtr = mPlanRepository.GetCurrentPlan();
   if (planPtr == nullptr) return nrm::NetworkPlanState::cDRAFT;
   if (mHasDistributionPackage && mDistributionPackage.generated &&
       mDistributionPackage.planId == planPtr->planId &&
       mDistributionPackage.revision == planPtr->revision)
      return nrm::NetworkPlanState::cREADY_FOR_DISTRIBUTION;
   if (mHasPlanEvaluation && mPlanEvaluation.planId == planPtr->planId &&
       mPlanEvaluation.revision == planPtr->revision)
      return mPlanEvaluation.resultingState;
   return planPtr->state;
}

bool WkNrm::DataContainer::IsReportingHealthy() const
{
   return mReporterPtr && mReporterPtr->GetStatus().healthy;
}

std::string WkNrm::DataContainer::GetReportingStatus() const
{
   if (!mReporterPtr)
   {
      return "REPORTER_NOT_INITIALIZED";
   }
   const ReporterStatus status = mReporterPtr->GetStatus();
   std::string result = status.healthy ? "OK" : "ERROR";
   if (!status.started)
   {
      result += " (starting)";
   }
   const std::uint64_t allDropped = status.droppedSnapshotCount +
                                    status.droppedAssessmentCount +
                                    status.droppedCapabilityCount +
                                    status.droppedPlanValidationCount +
                                    status.droppedPlanEvaluationCount +
                                    status.droppedDemandResultCount +
                                    status.droppedPlanningRecommendationCount +
                                    status.droppedDemandFeedbackCount +
                                    status.droppedPlanningCoordinationCount;
   if (allDropped > 0)
   {
      result += " dropped=" + std::to_string(allDropped);
   }
   if (status.writeErrorCount > 0)
   {
      result += " errors=" + std::to_string(status.writeErrorCount);
   }
   if (!status.lastError.empty())
   {
      result += " " + status.lastError;
   }
   return result;
}
