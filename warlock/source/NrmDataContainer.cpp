#include "NrmDataContainer.hpp"

#include <QByteArray>
#include <QDir>

#include "NrmSnapshotReporter.hpp"
#include "nrm/NetworkProfileRepository.hpp"

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
} // namespace

WkNrm::DataContainer::DataContainer(QObject* aParentPtr)
   : QObject(aParentPtr)
   , mProfiles(LoadProfiles())
   , mCapabilityService(mProfiles)
   , mPlanValidator(mProfiles)
   , mPlanEvaluationService(mProfiles)
{
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
   mCapability = mCapabilityService.Query(mSnapshot, aRequest, aEnvironment);
   mHasCapability = true;
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
   mPlanValidation = planPtr == nullptr
                        ? NoCurrentPlanValidation()
                        : mPlanValidator.Validate(mSnapshot, *planPtr);
   mHasPlanValidation = true;
   mHasPlanEvaluation = false;
   mHasDistributionPackage = false;
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
   }
   else
   {
      mPlanEvaluation =
         mPlanEvaluationService.Evaluate(mSnapshot, *planPtr, aEnvironment);
   }
   mPlanValidation = mPlanEvaluation.validation;
   mHasPlanValidation = true;
   mHasPlanEvaluation = true;
   mHasDistributionPackage = false;
   mReporterPtr->EnqueuePlanValidation(mPlanValidation);
   mReporterPtr->EnqueuePlanEvaluation(mPlanEvaluation);
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
      mDistributionPackage = mPlanDistributionService.Generate(
         *planPtr, mPlanValidation, mPlanEvaluation, aOutputRoot);
   }
   mHasDistributionPackage = true;
   if (!mDistributionPackage.generated &&
       (mDistributionPackage.reason == nrm::PlanValidationReason::cFILE_WRITE_FAILED ||
        mDistributionPackage.reason == nrm::PlanValidationReason::cATOMIC_RENAME_FAILED ||
        mDistributionPackage.reason == nrm::PlanValidationReason::cOUTPUT_PATH_INVALID))
      mReporterPtr->ReportPlanError(mDistributionPackage.reason, "distributionPackage");
   emit NetworkPlanChanged();
   return mDistributionPackage;
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
                                    status.droppedPlanEvaluationCount;
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
