#include "NrmDataContainer.hpp"

#include <cmath>

#include <QByteArray>
#include <QDir>

#include "NrmSnapshotReporter.hpp"
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
} // namespace

WkNrm::DataContainer::DataContainer(QObject* aParentPtr)
   : QObject(aParentPtr)
   , mProfiles(LoadProfiles())
   , mEnvironmentConfig(LoadEnvironmentConfig())
   , mEnvironmentAdapter(mEnvironmentConfig)
   , mModelServiceFacade(mProfiles, &mEnvironmentAdapter)
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

void WkNrm::DataContainer::SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot)
{
   mSnapshot = aSnapshot;
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
   {
      mReporterPtr->EnqueueDemandResults(mDemandMatching);
      mReporterPtr->EnqueuePlanningRecommendations(mDemandMatching);
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
                                    status.droppedPlanningRecommendationCount;
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
