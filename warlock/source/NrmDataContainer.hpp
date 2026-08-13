#ifndef NRM_DATA_CONTAINER_HPP
#define NRM_DATA_CONTAINER_HPP

#include <memory>
#include <string>

#include <QObject>

#include "nrm/NetworkResourceTypes.hpp"
#include "nrm/AssessmentTypes.hpp"
#include "nrm/ConcurrentTaskAssessment.hpp"
#include "nrm/BuiltInEnvironmentEffectAdapter.hpp"
#include "nrm/ModelRegistry.hpp"
#include "nrm/ModelServiceFacade.hpp"
#include "nrm/NetworkPlanRepository.hpp"
#include "nrm/ResourceDemandRepository.hpp"
#include "NrmCustomerJsonCodec.hpp"

namespace WkNrm
{
class SnapshotReporter;

class DataContainer : public QObject
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
   const nrm::ResourceDemandSet* GetResourceDemandSet() const
   {
      return mDemandRepository.GetCurrentDemandSet();
   }
   const nrm::ResourceDemandBatchResult& GetDemandMatching() const
   {
      return mDemandMatching;
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
   nrm::NetworkPlanState GetNetworkPlanState() const;
   bool IsReportingHealthy() const;
   std::string GetReportingStatus() const;
   void SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot);
   void StoreAssessment(const nrm::AssessmentResult& aResult);
   nrm::CapabilityResult QueryCapability(
      const nrm::CapabilityRequest& aRequest,
      const nrm::EnvironmentContext& aEnvironment = nrm::EnvironmentContext());
   bool LoadNetworkPlan(const std::string& aPath);
   bool LoadCustomerJson(const std::string& aPath);
   const CustomerJsonDecodeResult& LastCustomerJsonResult() const
   {
      return mLastCustomerJsonResult;
   }
   bool ReplaceNetworkPlanDraft(const nrm::NetworkPlanDocument& aDocument);
   void UnloadNetworkPlan();
   bool SaveNetworkPlanRevision(const std::string& aPath = std::string());
   nrm::PlanValidationResult ValidateNetworkPlan();
   nrm::NetworkPlanEvaluationResult EvaluateNetworkPlan(
      const nrm::EnvironmentContext& aEnvironment = nrm::EnvironmentContext());
   nrm::DistributionPackageResult GenerateNetworkPlanPackage(
      const std::string& aOutputRoot = std::string());
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
   nrm::ModelServiceContext MakeModelServiceContext(
      nrm::ModelServiceOperation aOperation,
      std::uint64_t aSnapshotVersion);
   nrm::EnvironmentContext EffectiveEnvironment(
      const nrm::EnvironmentContext& aEnvironment) const;

   nrm::FrameworkSnapshot            mSnapshot;
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
   nrm::PlanRepositoryResult          mPlanOperation;
   nrm::PlanValidationResult          mPlanValidation;
   nrm::NetworkPlanEvaluationResult   mPlanEvaluation;
   nrm::ConcurrentAssessmentResult    mConcurrentAssessment;
   nrm::DistributionPackageResult     mDistributionPackage;
   bool                               mHasPlanValidation = false;
   bool                               mHasPlanEvaluation = false;
   bool                               mHasDistributionPackage = false;
   nrm::ResourceDemandRepository      mDemandRepository;
   nrm::ResourceDemandRepositoryResult mDemandOperation;
   nrm::ResourceDemandBatchResult     mDemandMatching;
   bool                               mHasDemandMatching = false;
   std::unique_ptr<SnapshotReporter> mReporterPtr;
   CustomerJsonCodec                 mCustomerJsonCodec;
   CustomerJsonDecodeResult          mLastCustomerJsonResult;
};
} // namespace WkNrm

#endif
