#ifndef NRM_DATA_CONTAINER_HPP
#define NRM_DATA_CONTAINER_HPP

#include <memory>
#include <string>

#include <QObject>

#include "nrm/NetworkResourceTypes.hpp"
#include "nrm/AssessmentTypes.hpp"
#include "nrm/ConcurrentTaskAssessment.hpp"
#include "nrm/BuiltInEnvironmentEffectAdapter.hpp"
#include "nrm/CustomerIngestionState.hpp"
#include "nrm/CustomerNrmAdapter.hpp"
#include "nrm/CustomerSnapshotAssembler.hpp"
#include "nrm/EffectiveSnapshotAssembler.hpp"
#include "nrm/ModelRegistry.hpp"
#include "nrm/ModelServiceFacade.hpp"
#include "nrm/NetworkPlanRepository.hpp"
#include "nrm/NetworkPlanCoordinationService.hpp"
#include "nrm/ResourceDemandRepository.hpp"
#include "nrm/ResourceDemandCoordinator.hpp"
#include "NrmCustomerJsonCodec.hpp"

namespace WkNrm
{
class SnapshotReporter;

class DataContainer : public QObject, private nrm::CustomerNrmHost
{
   Q_OBJECT

public:
   explicit DataContainer(QObject* aParentPtr = nullptr);
   ~DataContainer() override;

   const nrm::FrameworkSnapshot& GetSnapshot() const { return mSnapshot; }
   const nrm::AssessmentResult& GetAssessment() const { return mAssessment; }
   const nrm::CapabilityResult& GetCapability() const { return mCapability; }
   bool HasAssessment() const { return mHasAssessment; }
   bool HasCapability() const { return mHasCapability; }
   bool HasNetworkPlan() const { return mPlanRepository.HasCurrentPlan(); }
   bool HasPlanValidation() const { return mHasPlanValidation; }
   bool HasPlanEvaluation() const { return mHasPlanEvaluation; }
   bool HasDistributionPackage() const { return mHasDistributionPackage; }
   bool HasPlanCoordination() const { return mHasPlanCoordination; }
   bool HasResourceDemandSet() const { return mDemandRepository.HasCurrentDemandSet(); }
   bool HasDemandMatching() const { return mHasDemandMatching; }
   const nrm::NetworkPlanDocument* GetNetworkPlan() const
   {
      return mPlanRepository.GetCurrentPlan();
   }
   const nrm::PlanValidationResult& GetPlanValidation() const
   {
      return mPlanValidation;
   }
   const nrm::NetworkPlanEvaluationResult& GetPlanEvaluation() const
   {
      return mPlanEvaluation;
   }
   const nrm::ConcurrentAssessmentResult& GetConcurrentAssessment() const
   {
      return mConcurrentAssessment;
   }
   const nrm::DistributionPackageResult& GetDistributionPackage() const
   {
      return mDistributionPackage;
   }
   const nrm::PlanRepositoryResult& GetPlanOperation() const
   {
      return mPlanOperation;
   }
   const nrm::PlanCoordinationEvidence& GetPlanCoordination() const
   {
      return mPlanCoordination;
   }
   const nrm::ResourceDemandSet* GetResourceDemandSet() const
   {
      return mDemandRepository.GetCurrentDemandSet();
   }
   const nrm::ResourceDemandBatchResult& GetDemandMatching() const
   {
      return mDemandMatching;
   }
   const nrm::ResourceDemandFeedback& GetDemandFeedback() const
   {
      return mDemandFeedback;
   }
   const nrm::ResourceDemandRepositoryResult& GetDemandOperation() const
   {
      return mDemandOperation;
   }
   const nrm::ModelServiceFacade& GetModelServiceFacade() const
   {
      return mModelServiceFacade;
   }
   const nrm::ModelRegistry& GetModelRegistry() const { return mModelRegistry; }
   const nrm::ModelRegistryResult& GetModelRegistration() const
   {
      return mModelRegistration;
   }
   nrm::CustomerNrmAdapter& GetCustomerNrmAdapter()
   {
      return mCustomerNrmAdapter;
   }
   const nrm::CustomerNrmAdapter& GetCustomerNrmAdapter() const
   {
      return mCustomerNrmAdapter;
   }
   nrm::NetworkPlanState GetNetworkPlanState() const;
   bool IsReportingHealthy() const;
   std::string GetReportingStatus() const;
   void SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot);
   void StoreAssessment(const nrm::AssessmentResult& aResult);
   nrm::AssessmentResult EvaluateAssessment(const nrm::AssessmentTask& aTask);
   nrm::CapabilityResult QueryCapability(
      const nrm::CapabilityRequest& aRequest,
      const nrm::EnvironmentContext& aEnvironment = nrm::EnvironmentContext());
   bool LoadNetworkPlan(const std::string& aPath);
   bool LoadCustomerJson(const std::string& aPath);
   const CustomerJsonDecodeResult& LastCustomerJsonResult() const
   {
      return mLastCustomerJsonResult;
   }
   const QByteArray& LastCustomerJsonResponse() const
   {
      return mLastCustomerJsonResponse;
   }
   bool HasCustomerProvider() const { return mHasCustomerProvider; }
   const CustomerProviderHello& LastCustomerProvider() const
   {
      return mLastCustomerProvider;
   }
   bool ReplaceNetworkPlanDraft(const nrm::NetworkPlanDocument& aDocument);
   void UnloadNetworkPlan();
   bool SaveNetworkPlanRevision(const std::string& aPath = std::string());
   nrm::PlanValidationResult ValidateNetworkPlan();
   nrm::NetworkPlanEvaluationResult EvaluateNetworkPlan(
      const nrm::EnvironmentContext& aEnvironment = nrm::EnvironmentContext());
   nrm::DistributionPackageResult GenerateNetworkPlanPackage(
      const std::string& aOutputRoot = std::string());
   bool AcknowledgeNetworkPlanPackage(const std::string& aAckPath);
   bool LoadResourceDemands(const std::string& aPath);
   bool ReplaceResourceDemandDraft(const nrm::ResourceDemandSet& aDemandSet);
   void UnloadResourceDemands();
   bool SaveResourceDemandRevision(const std::string& aPath = std::string());
   nrm::ResourceDemandBatchResult EvaluateResourceDemands(
      const nrm::EnvironmentContext& aEnvironment = nrm::EnvironmentContext(),
      const nrm::PlanningCandidateSet* aCandidatesPtr = nullptr);

signals:
   void SnapshotChanged();
   void AssessmentChanged();
   void CapabilityChanged();
   void NetworkPlanChanged();
   void ResourceDemandChanged();

private:
   const nrm::ResourceSnapshot& CurrentSnapshot() const override
   {
      return mSnapshot;
   }
   std::string ActiveConfigVersion() const override
   {
      return mProfiles.ConfigVersion();
   }
   void PublishCustomerSnapshot(
      const nrm::ResourceSnapshot& aSnapshot) override;
   void ApplyCustomerEnvironmentContext(
      const nrm::EnvironmentContext& aContext) override;
   void BeginCustomerRun() override;
   nrm::AssessmentResult RunCustomerAssessment(
      const nrm::AssessmentTask& aTask) override;
   nrm::CustomerPlanEvaluationResult RunCustomerPlan(
      const nrm::NetworkPlanDocument& aPlan) override;
   nrm::ResourceDemandBatchResult RunCustomerDemands(
      const nrm::ResourceDemandSet& aDemands) override;

   nrm::ModelServiceContext MakeModelServiceContext(
      nrm::ModelServiceOperation aOperation,
      std::uint64_t aSnapshotVersion);
   nrm::EnvironmentContext EffectiveEnvironment(
      const nrm::EnvironmentContext& aEnvironment) const;
   void RebuildEffectiveSnapshot();
   void ApplyEffectiveSnapshot(nrm::FrameworkSnapshot aSnapshot);

   nrm::FrameworkSnapshot            mSnapshot;
   nrm::FrameworkSnapshot            mAfsimBaseSnapshot;
   nrm::FrameworkSnapshot            mCustomerOverlaySnapshot;
   bool                              mHasAfsimBaseSnapshot = false;
   bool                              mHasCustomerOverlaySnapshot = false;
   std::uint64_t                     mEffectiveSnapshotVersion = 0;
   nrm::AssessmentResult             mAssessment;
   nrm::CapabilityResult             mCapability;
   bool                              mHasAssessment = false;
   bool                              mHasCapability = false;
   nrm::NetworkProfileRepository     mProfiles;
   nrm::EnvironmentConfigRepository  mEnvironmentConfig;
   nrm::BuiltInEnvironmentEffectAdapter mEnvironmentAdapter;
   nrm::ModelServiceFacade            mModelServiceFacade;
   nrm::ModelRegistry                 mModelRegistry;
   nrm::ModelRegistryResult           mModelRegistration;
   std::uint64_t                      mModelServiceRequestSequence = 0;
   nrm::NetworkPlanRepository         mPlanRepository;
   nrm::NetworkPlanCoordinationService mPlanCoordinationService;
   nrm::PlanRepositoryResult          mPlanOperation;
   nrm::PlanValidationResult          mPlanValidation;
   nrm::NetworkPlanEvaluationResult   mPlanEvaluation;
   nrm::ConcurrentAssessmentResult    mConcurrentAssessment;
   nrm::DistributionPackageResult     mDistributionPackage;
   nrm::PlanCoordinationEvidence       mPlanCoordination;
   bool                               mHasPlanValidation = false;
   bool                               mHasPlanEvaluation = false;
   bool                               mHasDistributionPackage = false;
   bool                               mHasPlanCoordination = false;
   nrm::ResourceDemandRepository      mDemandRepository;
   nrm::ResourceDemandRepositoryResult mDemandOperation;
   nrm::ResourceDemandBatchResult     mDemandMatching;
   nrm::ResourceDemandCoordinator     mDemandCoordinator;
   nrm::ResourceDemandFeedback        mDemandFeedback;
   bool                               mHasDemandMatching = false;
   std::unique_ptr<SnapshotReporter> mReporterPtr;
   CustomerJsonCodec                 mCustomerJsonCodec;
   CustomerJsonDecodeResult          mLastCustomerJsonResult;
   QByteArray                        mLastCustomerJsonResponse;
   nrm::CustomerIngestionState       mCustomerIngestionState;
   nrm::CustomerNrmAdapter           mCustomerNrmAdapter;
   nrm::EnvironmentContext           mCustomerEnvironmentContext;
   CustomerProviderHello             mLastCustomerProvider;
   bool                              mHasCustomerProvider = false;
};
} // namespace WkNrm

#endif
