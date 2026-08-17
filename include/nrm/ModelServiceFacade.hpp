#ifndef NRM_MODEL_SERVICE_FACADE_HPP
#define NRM_MODEL_SERVICE_FACADE_HPP

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <utility>

#include "nrm/AssessmentEvaluator.hpp"
#include "nrm/CommunicationCapabilityService.hpp"
#include "nrm/ModelServiceTypes.hpp"
#include "nrm/NetworkPlanDistributionService.hpp"
#include "nrm/NetworkPlanEvaluationService.hpp"
#include "nrm/NetworkPlanRepository.hpp"
#include "nrm/NetworkPlanValidator.hpp"
#include "nrm/ResourceDemandMatchingService.hpp"
#include "nrm/Version.hpp"

namespace nrm
{
class CommunicationCapabilityServicePort
{
public:
   virtual ~CommunicationCapabilityServicePort() = default;

   virtual CapabilityResult Query(
      const ResourceSnapshot& aSnapshot,
      const CapabilityRequest& aRequest,
      const EnvironmentContext& aEnvironment) const = 0;
};

class AssessmentServicePort
{
public:
   virtual ~AssessmentServicePort() = default;

   virtual AssessmentResult Evaluate(
      const ResourceSnapshot& aSnapshot,
      const AssessmentTask& aTask) const = 0;
};

class PlanValidationServicePort
{
public:
   virtual ~PlanValidationServicePort() = default;

   virtual PlanValidationResult Validate(
      const ResourceSnapshot& aSnapshot,
      const NetworkPlanDocument& aPlan) const = 0;
};

class PlanEvaluationServicePort
{
public:
   virtual ~PlanEvaluationServicePort() = default;

   virtual NetworkPlanEvaluationResult Evaluate(
      const ResourceSnapshot& aSnapshot,
      const NetworkPlanDocument& aPlan,
      const EnvironmentContext& aEnvironment) const = 0;
};

class PlanDistributionServicePort
{
public:
   virtual ~PlanDistributionServicePort() = default;

   virtual DistributionPackageResult Generate(
      const NetworkPlanDocument& aPlan,
      const PlanValidationResult& aValidation,
      const NetworkPlanEvaluationResult& aEvaluation,
      const std::string& aOutputRoot) const = 0;
};

class ResourceDemandMatchingServicePort
{
public:
   virtual ~ResourceDemandMatchingServicePort() = default;

   virtual ResourceDemandBatchResult Evaluate(
      const ResourceSnapshot& aSnapshot,
      const ResourceDemandSet& aDemandSet,
      const NetworkPlanDocument* aPlanPtr,
      const NetworkPlanEvaluationResult* aPlanEvaluationPtr,
      const EnvironmentContext& aEnvironment,
      const PlanningCandidateSet* aCandidatesPtr) const = 0;
};

namespace model_service_detail
{
class DefaultAssessmentServicePort final : public AssessmentServicePort
{
public:
   explicit DefaultAssessmentServicePort(
      const NetworkProfileRepository& aProfiles)
      : mService(aProfiles)
   {
   }

   AssessmentResult Evaluate(
      const ResourceSnapshot& aSnapshot,
      const AssessmentTask& aTask) const override
   {
      return mService.Evaluate(aSnapshot, aTask);
   }

private:
   AssessmentEvaluator mService;
};

class DefaultCommunicationCapabilityServicePort final
   : public CommunicationCapabilityServicePort
{
public:
   DefaultCommunicationCapabilityServicePort(
      const NetworkProfileRepository& aProfiles,
      const EnvironmentEffectAdapter* aEnvironmentAdapterPtr)
      : mService(aProfiles, aEnvironmentAdapterPtr)
   {
   }

   CapabilityResult Query(
      const ResourceSnapshot& aSnapshot,
      const CapabilityRequest& aRequest,
      const EnvironmentContext& aEnvironment) const override
   {
      return mService.Query(aSnapshot, aRequest, aEnvironment);
   }

private:
   CommunicationCapabilityService mService;
};

class DefaultPlanValidationServicePort final : public PlanValidationServicePort
{
public:
   explicit DefaultPlanValidationServicePort(
      const NetworkProfileRepository& aProfiles)
      : mService(aProfiles)
   {
   }

   PlanValidationResult Validate(
      const ResourceSnapshot& aSnapshot,
      const NetworkPlanDocument& aPlan) const override
   {
      return mService.Validate(aSnapshot, aPlan);
   }

private:
   NetworkPlanValidator mService;
};

class DefaultPlanEvaluationServicePort final : public PlanEvaluationServicePort
{
public:
   DefaultPlanEvaluationServicePort(
      const NetworkProfileRepository& aProfiles,
      const EnvironmentEffectAdapter* aEnvironmentAdapterPtr)
      : mService(aProfiles, aEnvironmentAdapterPtr)
   {
   }

   NetworkPlanEvaluationResult Evaluate(
      const ResourceSnapshot& aSnapshot,
      const NetworkPlanDocument& aPlan,
      const EnvironmentContext& aEnvironment) const override
   {
      return mService.Evaluate(aSnapshot, aPlan, aEnvironment);
   }

private:
   NetworkPlanEvaluationService mService;
};

class DefaultPlanDistributionServicePort final
   : public PlanDistributionServicePort
{
public:
   DistributionPackageResult Generate(
      const NetworkPlanDocument& aPlan,
      const PlanValidationResult& aValidation,
      const NetworkPlanEvaluationResult& aEvaluation,
      const std::string& aOutputRoot) const override
   {
      return mService.Generate(aPlan, aValidation, aEvaluation, aOutputRoot);
   }

private:
   NetworkPlanDistributionService mService;
};

class DefaultResourceDemandMatchingServicePort final
   : public ResourceDemandMatchingServicePort
{
public:
   DefaultResourceDemandMatchingServicePort(
      const NetworkProfileRepository& aProfiles,
      const EnvironmentEffectAdapter* aEnvironmentAdapterPtr)
      : mService(aProfiles, aEnvironmentAdapterPtr)
   {
   }

   ResourceDemandBatchResult Evaluate(
      const ResourceSnapshot& aSnapshot,
      const ResourceDemandSet& aDemandSet,
      const NetworkPlanDocument* aPlanPtr,
      const NetworkPlanEvaluationResult* aPlanEvaluationPtr,
      const EnvironmentContext& aEnvironment,
      const PlanningCandidateSet* aCandidatesPtr) const override
   {
      return mService.Evaluate(aSnapshot, aDemandSet, aPlanPtr,
                               aPlanEvaluationPtr, aEnvironment,
                               aCandidatesPtr);
   }

private:
   ResourceDemandMatchingService mService;
};
} // namespace model_service_detail

class ModelServiceFacade
{
public:
   using CapabilityServicePtr =
      std::shared_ptr<const CommunicationCapabilityServicePort>;
   using ValidationServicePtr =
      std::shared_ptr<const PlanValidationServicePort>;
   using EvaluationServicePtr =
      std::shared_ptr<const PlanEvaluationServicePort>;
   using DistributionServicePtr =
      std::shared_ptr<const PlanDistributionServicePort>;
   using DemandServicePtr =
      std::shared_ptr<const ResourceDemandMatchingServicePort>;
   using AssessmentServicePtr =
      std::shared_ptr<const AssessmentServicePort>;

   explicit ModelServiceFacade(
      const NetworkProfileRepository& aProfiles,
      const EnvironmentEffectAdapter* aEnvironmentAdapterPtr = nullptr)
      : ModelServiceFacade(
           aProfiles,
           std::make_shared<
              model_service_detail::DefaultCommunicationCapabilityServicePort>(
                 aProfiles, aEnvironmentAdapterPtr),
           std::make_shared<
              model_service_detail::DefaultPlanValidationServicePort>(aProfiles),
           std::make_shared<
              model_service_detail::DefaultPlanEvaluationServicePort>(
                 aProfiles, aEnvironmentAdapterPtr),
           std::make_shared<
              model_service_detail::DefaultPlanDistributionServicePort>(),
           std::make_shared<
              model_service_detail::DefaultResourceDemandMatchingServicePort>(
                 aProfiles, aEnvironmentAdapterPtr),
           std::make_shared<
              model_service_detail::DefaultAssessmentServicePort>(aProfiles))
   {
   }

   ModelServiceFacade(
      const NetworkProfileRepository& aProfiles,
      CapabilityServicePtr aCapabilityService,
      ValidationServicePtr aValidationService,
      EvaluationServicePtr aEvaluationService,
      DistributionServicePtr aDistributionService,
      DemandServicePtr aDemandService)
      : ModelServiceFacade(
           aProfiles, std::move(aCapabilityService),
           std::move(aValidationService), std::move(aEvaluationService),
           std::move(aDistributionService), std::move(aDemandService),
           std::make_shared<
              model_service_detail::DefaultAssessmentServicePort>(aProfiles))
   {
   }

   ModelServiceFacade(
      const NetworkProfileRepository& aProfiles,
      CapabilityServicePtr aCapabilityService,
      ValidationServicePtr aValidationService,
      EvaluationServicePtr aEvaluationService,
      DistributionServicePtr aDistributionService,
      DemandServicePtr aDemandService,
      AssessmentServicePtr aAssessmentService)
      : mProfiles(aProfiles)
      , mCapabilityService(std::move(aCapabilityService))
      , mValidationService(std::move(aValidationService))
      , mEvaluationService(std::move(aEvaluationService))
      , mDistributionService(std::move(aDistributionService))
      , mDemandService(std::move(aDemandService))
      , mAssessmentService(std::move(aAssessmentService))
   {
   }

   AssessmentServiceResponse EvaluateAssessment(
      const ModelServiceContext& aContext,
      const ResourceSnapshot& aSnapshot,
      const AssessmentTask& aTask,
      const EnvironmentContext& aEnvironment = EnvironmentContext()) const
   {
      AssessmentServiceResponse response = NewResponse<AssessmentResult>(
         aContext, ModelServiceOperation::cEVALUATE_ASSESSMENT,
         aSnapshot.snapshotVersion);
      if (!ValidateContext(aContext, &aSnapshot.snapshotVersion, response) ||
          !ValidateService(mAssessmentService, response) ||
          (UsesCandidateAdjustment(aEnvironment) &&
           !ValidateService(mCapabilityService, response)))
         return response;
      try
      {
         response.result = mAssessmentService->Evaluate(aSnapshot, aTask);
         if (UsesCandidateAdjustment(aEnvironment))
         {
            const CapabilityResult capability = mCapabilityService->Query(
               aSnapshot, CapabilityRequestFrom(aTask), aEnvironment);
            ApplyEnvironmentCapability(aTask, capability, response.result);
         }
         Complete(response);
      }
      catch (...)
      {
         InternalError(response);
      }
      return response;
   }

   CapabilityServiceResponse QueryCapability(
      const ModelServiceContext& aContext,
      const ResourceSnapshot& aSnapshot,
      const CapabilityRequest& aRequest,
      const EnvironmentContext& aEnvironment = EnvironmentContext()) const
   {
      CapabilityServiceResponse response = NewResponse<CapabilityResult>(
         aContext, ModelServiceOperation::cQUERY_CAPABILITY,
         aSnapshot.snapshotVersion);
      if (!ValidateContext(aContext, &aSnapshot.snapshotVersion, response) ||
          !ValidateService(mCapabilityService, response))
         return response;
      try
      {
         response.result =
            mCapabilityService->Query(aSnapshot, aRequest, aEnvironment);
         Complete(response);
      }
      catch (...)
      {
         InternalError(response);
      }
      return response;
   }

   PlanValidationServiceResponse ValidatePlan(
      const ModelServiceContext& aContext,
      const ResourceSnapshot& aSnapshot,
      const NetworkPlanDocument& aPlan) const
   {
      PlanValidationServiceResponse response = NewResponse<PlanValidationResult>(
         aContext, ModelServiceOperation::cVALIDATE_PLAN,
         aSnapshot.snapshotVersion);
      if (!ValidateContext(aContext, &aSnapshot.snapshotVersion, response) ||
          !ValidateService(mValidationService, response))
         return response;
      try
      {
         response.result = mValidationService->Validate(aSnapshot, aPlan);
         Complete(response);
      }
      catch (...)
      {
         InternalError(response);
      }
      return response;
   }

   PlanEvaluationServiceResponse EvaluatePlan(
      const ModelServiceContext& aContext,
      const ResourceSnapshot& aSnapshot,
      const NetworkPlanDocument& aPlan,
      const EnvironmentContext& aEnvironment = EnvironmentContext()) const
   {
      PlanEvaluationServiceResponse response =
         NewResponse<NetworkPlanEvaluationResult>(
            aContext, ModelServiceOperation::cEVALUATE_PLAN,
            aSnapshot.snapshotVersion);
      if (!ValidateContext(aContext, &aSnapshot.snapshotVersion, response) ||
          !ValidateService(mEvaluationService, response))
         return response;
      try
      {
         response.result =
            mEvaluationService->Evaluate(aSnapshot, aPlan, aEnvironment);
         Complete(response);
      }
      catch (...)
      {
         InternalError(response);
      }
      return response;
   }

   DistributionPackageServiceResponse GenerateDistributionPackage(
      const ModelServiceContext& aContext,
      const NetworkPlanDocument& aPlan,
      const PlanValidationResult& aValidation,
      const NetworkPlanEvaluationResult& aEvaluation,
      const std::string& aOutputRoot = std::string()) const
   {
      DistributionPackageServiceResponse response =
         NewResponse<DistributionPackageResult>(
            aContext,
            ModelServiceOperation::cGENERATE_DISTRIBUTION_PACKAGE,
            aEvaluation.snapshotVersion);
      if (!ValidateContext(aContext, &aEvaluation.snapshotVersion, response) ||
          !ValidatePlanEvidence(aPlan, aValidation, aEvaluation, response) ||
          !ValidateService(mDistributionService, response))
         return response;
      try
      {
         response.result = mDistributionService->Generate(
            aPlan, aValidation, aEvaluation, aOutputRoot);
         Complete(response);
      }
      catch (...)
      {
         InternalError(response);
      }
      return response;
   }

   ResourceDemandServiceResponse MatchResourceDemands(
      const ModelServiceContext& aContext,
      const ResourceSnapshot& aSnapshot,
      const ResourceDemandSet& aDemandSet,
      const NetworkPlanDocument* aPlanPtr = nullptr,
      const NetworkPlanEvaluationResult* aPlanEvaluationPtr = nullptr,
      const EnvironmentContext& aEnvironment = EnvironmentContext(),
      const PlanningCandidateSet* aCandidatesPtr = nullptr) const
   {
      ResourceDemandServiceResponse response =
         NewResponse<ResourceDemandBatchResult>(
            aContext, ModelServiceOperation::cMATCH_RESOURCE_DEMANDS,
            aSnapshot.snapshotVersion);
      if (!ValidateContext(aContext, &aSnapshot.snapshotVersion, response) ||
          !ValidateOptionalPlanEvidence(aSnapshot, aPlanPtr,
                                        aPlanEvaluationPtr, response) ||
          !ValidateService(mDemandService, response))
         return response;
      try
      {
         response.result = mDemandService->Evaluate(
            aSnapshot, aDemandSet, aPlanPtr, aPlanEvaluationPtr,
            aEnvironment, aCandidatesPtr);
         Complete(response);
      }
      catch (...)
      {
         InternalError(response);
      }
      return response;
   }

   ModelHealthServiceResponse GetHealth(
      const ModelServiceContext& aContext) const
   {
      ModelHealthServiceResponse response = NewResponse<ModelServiceHealth>(
         aContext, ModelServiceOperation::cGET_HEALTH,
         aContext.snapshotVersion);
      if (!ValidateContext(aContext, nullptr, response)) return response;
      response.result.modelId = "nrm.model_service.facade";
      response.result.modelVersion = cVERSION;
      response.result.profileRepositoryAvailable = mProfiles.Valid();
      response.result.serviceDependenciesAvailable = DependenciesAvailable();
      response.result.healthy = response.result.profileRepositoryAvailable &&
                                response.result.serviceDependenciesAvailable;
      Complete(response);
      return response;
   }

   ModelDescriptorServiceResponse GetDescriptor(
      const ModelServiceContext& aContext) const
   {
      ModelDescriptorServiceResponse response = NewResponse<ModelDescriptor>(
         aContext, ModelServiceOperation::cGET_DESCRIPTOR,
         aContext.snapshotVersion);
      if (!ValidateContext(aContext, nullptr, response)) return response;
      response.result = Descriptor();
      Complete(response);
      return response;
   }

   static ModelDescriptor Descriptor()
   {
      ModelDescriptor descriptor;
      descriptor.modelId = "nrm.model_service.facade";
      descriptor.modelName = "AFSIM Network Resource Manager Model Service";
      descriptor.modelVersion = cVERSION;
      descriptor.providerId = "afsim-network-resource-manager";
      descriptor.supportedOperations = {
         ModelServiceOperation::cQUERY_CAPABILITY,
         ModelServiceOperation::cEVALUATE_ASSESSMENT,
         ModelServiceOperation::cVALIDATE_PLAN,
         ModelServiceOperation::cEVALUATE_PLAN,
         ModelServiceOperation::cGENERATE_DISTRIBUTION_PACKAGE,
         ModelServiceOperation::cMATCH_RESOURCE_DEMANDS,
         ModelServiceOperation::cGET_HEALTH,
         ModelServiceOperation::cGET_DESCRIPTOR};
      descriptor.supportedRequestSchemas = {
         cMODEL_SERVICE_REQUEST_SCHEMA,
         "nrm.assessment_task.v1",
         "nrm.network_plan.v1",
         "nrm.resource_demand_set.v1"};
      descriptor.supportedResponseSchemas = {
         cMODEL_SERVICE_RESPONSE_SCHEMA,
         "nrm.assessment.v3",
         "nrm.capability.v1",
         "nrm.network_plan_validation.v1",
         "nrm.network_plan_evaluation.v1",
         "nrm.network_plan_distribution.v1",
         "nrm.resource_demand_batch.v1",
         "nrm.model_service.health.v1"};
      descriptor.source = DataOrigin::cDERIVED;
      descriptor.confidence = Confidence::cHIGH;
      descriptor.valid = true;
      return descriptor;
   }

private:
   static bool UsesCandidateAdjustment(const EnvironmentContext& aEnvironment)
   {
      if (!aEnvironment.valid ||
          aEnvironment.applicationMode ==
             EnvironmentApplicationMode::cALREADY_INCLUDED)
         return false;
      if (aEnvironment.applicationMode ==
          EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT)
         return true;
      return aEnvironment.applyParameterizedEffects;
   }

   static CapabilityRequest CapabilityRequestFrom(const AssessmentTask& aTask)
   {
      CapabilityRequest request;
      request.requestId = aTask.taskId;
      request.sourcePlatform = aTask.sourcePlatform;
      request.destinationPlatform = aTask.destinationPlatform;
      request.businessType = aTask.businessType;
      request.payloadBits = aTask.payloadBits;
      request.requiredBandwidthBps = aTask.requiredBandwidthBps;
      request.maximumDelayMs = aTask.maximumDelayMs;
      request.minimumPdrPercent = aTask.minimumPdrPercent;
      request.kShortestPaths = aTask.kShortestPaths;
      request.maximumHops = aTask.maximumHops;
      request.allowedNetworks = aTask.allowedNetworks;
      return request;
   }

   static bool HasReason(const CapabilityResult& aResult,
                         CapabilityReason aReason)
   {
      return std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) !=
             aResult.reasons.end();
   }

   static void AddAssessmentReason(AssessmentResult& aResult,
                                   AssessmentReason aReason)
   {
      if (std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) ==
          aResult.reasons.end())
         aResult.reasons.push_back(aReason);
   }

   static bool HasApplicableEnvironmentEffect(const CapabilityResult& aCapability)
   {
      for (const EnvironmentEffect& effect : aCapability.environmentEffects)
      {
         if (effect.valid && effect.origin != DataOrigin::cAFSIM_INTERNAL)
            return true;
      }
      return HasReason(aCapability, CapabilityReason::cENVIRONMENT_HARD_BLOCKED);
   }

   static void SetDerivedMargin(MetricValue<double>& aMargin,
                                double aValue,
                                const MetricValue<double>& aEvidence,
                                const char* aUnit)
   {
      aMargin = aEvidence;
      aMargin.value = aValue;
      aMargin.unit = aUnit;
      aMargin.valid = std::isfinite(aValue);
      aMargin.reason = aMargin.valid ? MetricReason::cNONE
                                     : MetricReason::cINVALID_INPUT;
   }

   static void ApplyEnvironmentCapability(const AssessmentTask& aTask,
                                          const CapabilityResult& aCapability,
                                          AssessmentResult& aResult)
   {
      if (!HasApplicableEnvironmentEffect(aCapability)) return;
      if (!aCapability.requestValid)
      {
         aResult.canComplete = false;
         aResult.stable = false;
         AddAssessmentReason(aResult, AssessmentReason::cDATA_INVALID);
         return;
      }

      const bool hardBlocked =
         HasReason(aCapability, CapabilityReason::cENVIRONMENT_HARD_BLOCKED);
      if (hardBlocked)
      {
         aResult.canEstablish = false;
         aResult.canComplete = false;
         aResult.stable = false;
         AddAssessmentReason(
            aResult, AssessmentReason::cENVIRONMENT_HARD_BLOCKED);
         aResult.recommendations.push_back(
            "环境约束阻断了当前候选路径，建议更换链路、时段或中继节点后重评估。");
         return;
      }

      bool meetsConstraints = aCapability.pathAvailable;
      if (aCapability.transmissionRateBps.valid)
      {
         aResult.bottleneckBandwidthBps = aCapability.transmissionRateBps;
         SetDerivedMargin(aResult.bandwidthMarginBps,
                          aCapability.transmissionRateBps.value -
                             aTask.requiredBandwidthBps,
                          aCapability.transmissionRateBps, "bit/s");
         if (aResult.bandwidthMarginBps.value < 0.0)
         {
            meetsConstraints = false;
            AddAssessmentReason(
               aResult, AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE);
         }
      }
      else
      {
         meetsConstraints = false;
         AddAssessmentReason(aResult, AssessmentReason::cDATA_INVALID);
      }

      if (aCapability.transmissionDelayMs.valid)
      {
         aResult.predictedDelayMs = aCapability.transmissionDelayMs;
         SetDerivedMargin(aResult.delayMarginMs,
                          aTask.maximumDelayMs -
                             aCapability.transmissionDelayMs.value,
                          aCapability.transmissionDelayMs, "ms");
         if (aTask.maximumDelayMs > 0.0 && aResult.delayMarginMs.value < 0.0)
         {
            meetsConstraints = false;
            AddAssessmentReason(aResult, AssessmentReason::cDELAY_MARGIN_NEGATIVE);
         }
      }
      else if (aTask.requireDelayMetricForFeasibility)
      {
         meetsConstraints = false;
         AddAssessmentReason(aResult, AssessmentReason::cDATA_INVALID);
      }

      if (aCapability.packetLossPercent.valid)
      {
         aResult.estimatedPdrPercent = aCapability.packetLossPercent;
         aResult.estimatedPdrPercent.value =
            100.0 - aCapability.packetLossPercent.value;
         aResult.estimatedPdrPercent.unit = "percent";
         SetDerivedMargin(aResult.reliabilityMarginPercent,
                          aResult.estimatedPdrPercent.value -
                             aTask.minimumPdrPercent,
                          aResult.estimatedPdrPercent, "percent");
         if (aResult.reliabilityMarginPercent.value < 0.0)
         {
            meetsConstraints = false;
            AddAssessmentReason(
               aResult, AssessmentReason::cRELIABILITY_MARGIN_NEGATIVE);
         }
      }
      else
      {
         meetsConstraints = false;
         AddAssessmentReason(aResult, AssessmentReason::cDATA_INVALID);
      }

      aResult.canComplete = aResult.canComplete && meetsConstraints;
      aResult.stable = aResult.stable && aResult.canComplete;
      if (!aResult.canComplete)
         aResult.recommendations.push_back(
            "候选环境修正后任务约束不再满足，请按负裕量调整资源或业务门限。");
   }

   template<typename ResultType>
   static ModelServiceResponse<ResultType> NewResponse(
      const ModelServiceContext& aContext,
      ModelServiceOperation aOperation,
      std::uint64_t aSnapshotVersion)
   {
      ModelServiceResponse<ResultType> response;
      response.requestId = aContext.requestId;
      response.correlationId = aContext.correlationId;
      response.operation = aOperation;
      response.softwareVersion = cVERSION;
      response.snapshotVersion = aSnapshotVersion;
      return response;
   }

   template<typename ResultType>
   static bool ValidateContext(
      const ModelServiceContext& aContext,
      const std::uint64_t* aEvidenceSnapshotVersionPtr,
      ModelServiceResponse<ResultType>& aResponse)
   {
      if (aContext.schemaVersion != cMODEL_SERVICE_REQUEST_SCHEMA)
      {
         Reject(aResponse, ModelServiceStatus::cUNSUPPORTED_SCHEMA,
                ModelServiceReason::cUNSUPPORTED_SCHEMA);
         return false;
      }
      if (aContext.requestId.empty())
      {
         Reject(aResponse, ModelServiceStatus::cINVALID_REQUEST,
                ModelServiceReason::cREQUEST_ID_EMPTY);
         return false;
      }
      if (!aContext.valid)
      {
         Reject(aResponse, ModelServiceStatus::cINVALID_REQUEST,
                ModelServiceReason::cCONTEXT_INVALID);
         return false;
      }
      if (!std::isfinite(aContext.requestTime))
      {
         Reject(aResponse, ModelServiceStatus::cINVALID_REQUEST,
                ModelServiceReason::cREQUEST_TIME_INVALID);
         return false;
      }
      if (aEvidenceSnapshotVersionPtr != nullptr &&
          aContext.snapshotVersion != *aEvidenceSnapshotVersionPtr)
      {
         Reject(aResponse, ModelServiceStatus::cEVIDENCE_MISMATCH,
                ModelServiceReason::cSNAPSHOT_VERSION_MISMATCH);
         return false;
      }
      return true;
   }

   template<typename ServiceType, typename ResultType>
   static bool ValidateService(
      const std::shared_ptr<const ServiceType>& aService,
      ModelServiceResponse<ResultType>& aResponse)
   {
      if (aService) return true;
      Reject(aResponse, ModelServiceStatus::cSERVICE_UNAVAILABLE,
             ModelServiceReason::cSERVICE_DEPENDENCY_UNAVAILABLE);
      return false;
   }

   template<typename ResultType>
   static bool ValidatePlanEvidence(
      const NetworkPlanDocument& aPlan,
      const PlanValidationResult& aValidation,
      const NetworkPlanEvaluationResult& aEvaluation,
      ModelServiceResponse<ResultType>& aResponse)
   {
      const std::string fingerprint =
         network_plan_detail::PlanContentFingerprint(aPlan);
      const bool matches =
         !fingerprint.empty() &&
         aValidation.planId == aPlan.planId &&
         aValidation.revision == aPlan.revision &&
         aValidation.planFingerprint == fingerprint &&
         aEvaluation.planId == aPlan.planId &&
         aEvaluation.revision == aPlan.revision &&
         aEvaluation.planFingerprint == fingerprint &&
         aEvaluation.validation.planId == aPlan.planId &&
         aEvaluation.validation.revision == aPlan.revision &&
         aEvaluation.validation.planFingerprint == fingerprint;
      if (matches) return true;
      Reject(aResponse, ModelServiceStatus::cEVIDENCE_MISMATCH,
             ModelServiceReason::cPLAN_EVIDENCE_MISMATCH);
      return false;
   }

   template<typename ResultType>
   static bool ValidateOptionalPlanEvidence(
      const ResourceSnapshot& aSnapshot,
      const NetworkPlanDocument* aPlanPtr,
      const NetworkPlanEvaluationResult* aEvaluationPtr,
      ModelServiceResponse<ResultType>& aResponse)
   {
      if (aEvaluationPtr == nullptr) return true;
      if (aPlanPtr == nullptr)
      {
         Reject(aResponse, ModelServiceStatus::cEVIDENCE_MISMATCH,
                ModelServiceReason::cPLAN_EVIDENCE_MISMATCH);
         return false;
      }
      const PlanValidationResult& validation = aEvaluationPtr->validation;
      if (aEvaluationPtr->snapshotVersion != aSnapshot.snapshotVersion ||
          !ValidatePlanEvidence(*aPlanPtr, validation, *aEvaluationPtr, aResponse))
      {
         if (aResponse.reasons.empty())
         {
            Reject(aResponse, ModelServiceStatus::cEVIDENCE_MISMATCH,
                   ModelServiceReason::cSNAPSHOT_VERSION_MISMATCH);
         }
         return false;
      }
      return true;
   }

   template<typename ResultType>
   static void Complete(ModelServiceResponse<ResultType>& aResponse)
   {
      aResponse.status = ModelServiceStatus::cSUCCESS;
      aResponse.valid = true;
      aResponse.reasons.clear();
   }

   template<typename ResultType>
   static void Reject(ModelServiceResponse<ResultType>& aResponse,
                      ModelServiceStatus aStatus,
                      ModelServiceReason aReason)
   {
      aResponse.status = aStatus;
      aResponse.valid = false;
      aResponse.reasons.clear();
      aResponse.reasons.push_back(aReason);
   }

   template<typename ResultType>
   static void InternalError(ModelServiceResponse<ResultType>& aResponse)
   {
      Reject(aResponse, ModelServiceStatus::cINTERNAL_ERROR,
             ModelServiceReason::cINTERNAL_EXCEPTION);
   }

   bool DependenciesAvailable() const
   {
      return mCapabilityService && mValidationService && mEvaluationService &&
             mDistributionService && mDemandService && mAssessmentService;
   }

   NetworkProfileRepository mProfiles;
   CapabilityServicePtr mCapabilityService;
   ValidationServicePtr mValidationService;
   EvaluationServicePtr mEvaluationService;
   DistributionServicePtr mDistributionService;
   DemandServicePtr mDemandService;
   AssessmentServicePtr mAssessmentService;
};
} // namespace nrm

#endif
