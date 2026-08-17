#ifndef NRM_MODEL_SERVICE_TYPES_HPP
#define NRM_MODEL_SERVICE_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "nrm/AssessmentTypes.hpp"
#include "nrm/NetworkPlanTypes.hpp"
#include "nrm/ResourceDemandTypes.hpp"

namespace nrm
{
constexpr const char* cMODEL_SERVICE_REQUEST_SCHEMA =
   "nrm.model_service.request.v1";
constexpr const char* cMODEL_SERVICE_RESPONSE_SCHEMA =
   "nrm.model_service.response.v1";

enum class ModelServiceOperation
{
   cQUERY_CAPABILITY,
   cEVALUATE_ASSESSMENT,
   cVALIDATE_PLAN,
   cEVALUATE_PLAN,
   cGENERATE_DISTRIBUTION_PACKAGE,
   cMATCH_RESOURCE_DEMANDS,
   cGET_HEALTH,
   cGET_DESCRIPTOR
};

inline const char* ToString(ModelServiceOperation aOperation)
{
   switch (aOperation)
   {
   case ModelServiceOperation::cQUERY_CAPABILITY: return "QUERY_CAPABILITY";
   case ModelServiceOperation::cEVALUATE_ASSESSMENT:
      return "EVALUATE_ASSESSMENT";
   case ModelServiceOperation::cVALIDATE_PLAN: return "VALIDATE_PLAN";
   case ModelServiceOperation::cEVALUATE_PLAN: return "EVALUATE_PLAN";
   case ModelServiceOperation::cGENERATE_DISTRIBUTION_PACKAGE:
      return "GENERATE_DISTRIBUTION_PACKAGE";
   case ModelServiceOperation::cMATCH_RESOURCE_DEMANDS:
      return "MATCH_RESOURCE_DEMANDS";
   case ModelServiceOperation::cGET_HEALTH: return "GET_HEALTH";
   case ModelServiceOperation::cGET_DESCRIPTOR: return "GET_DESCRIPTOR";
   }
   return "GET_HEALTH";
}

enum class ModelServiceStatus
{
   cSUCCESS,
   cINVALID_REQUEST,
   cUNSUPPORTED_SCHEMA,
   cEVIDENCE_MISMATCH,
   cSERVICE_UNAVAILABLE,
   cINTERNAL_ERROR
};

inline const char* ToString(ModelServiceStatus aStatus)
{
   switch (aStatus)
   {
   case ModelServiceStatus::cSUCCESS: return "SUCCESS";
   case ModelServiceStatus::cINVALID_REQUEST: return "INVALID_REQUEST";
   case ModelServiceStatus::cUNSUPPORTED_SCHEMA: return "UNSUPPORTED_SCHEMA";
   case ModelServiceStatus::cEVIDENCE_MISMATCH: return "EVIDENCE_MISMATCH";
   case ModelServiceStatus::cSERVICE_UNAVAILABLE: return "SERVICE_UNAVAILABLE";
   case ModelServiceStatus::cINTERNAL_ERROR: return "INTERNAL_ERROR";
   }
   return "INTERNAL_ERROR";
}

enum class ModelServiceReason
{
   cNONE,
   cCONTEXT_INVALID,
   cREQUEST_ID_EMPTY,
   cREQUEST_TIME_INVALID,
   cUNSUPPORTED_SCHEMA,
   cSNAPSHOT_VERSION_MISMATCH,
   cPLAN_EVIDENCE_MISMATCH,
   cSERVICE_DEPENDENCY_UNAVAILABLE,
   cINTERNAL_EXCEPTION
};

inline const char* ToString(ModelServiceReason aReason)
{
   switch (aReason)
   {
   case ModelServiceReason::cNONE: return "NONE";
   case ModelServiceReason::cCONTEXT_INVALID: return "CONTEXT_INVALID";
   case ModelServiceReason::cREQUEST_ID_EMPTY: return "REQUEST_ID_EMPTY";
   case ModelServiceReason::cREQUEST_TIME_INVALID: return "REQUEST_TIME_INVALID";
   case ModelServiceReason::cUNSUPPORTED_SCHEMA: return "UNSUPPORTED_SCHEMA";
   case ModelServiceReason::cSNAPSHOT_VERSION_MISMATCH:
      return "SNAPSHOT_VERSION_MISMATCH";
   case ModelServiceReason::cPLAN_EVIDENCE_MISMATCH:
      return "PLAN_EVIDENCE_MISMATCH";
   case ModelServiceReason::cSERVICE_DEPENDENCY_UNAVAILABLE:
      return "SERVICE_DEPENDENCY_UNAVAILABLE";
   case ModelServiceReason::cINTERNAL_EXCEPTION: return "INTERNAL_EXCEPTION";
   }
   return "INTERNAL_EXCEPTION";
}

struct ModelServiceContext
{
   std::string schemaVersion = cMODEL_SERVICE_REQUEST_SCHEMA;
   std::string requestId;
   std::string correlationId;
   std::string callerId;
   std::string softwareVersion;
   std::uint64_t snapshotVersion = 0;
   double requestTime = 0.0;
   DataOrigin source = DataOrigin::cCUSTOMER_MODULE;
   Confidence confidence = Confidence::cLOW;
   bool valid = false;
};

struct ModelDescriptor
{
   std::string modelId;
   std::string modelName;
   std::string modelVersion;
   std::string providerId;
   std::vector<ModelServiceOperation> supportedOperations;
   std::vector<std::string> supportedRequestSchemas;
   std::vector<std::string> supportedResponseSchemas;
   DataOrigin source = DataOrigin::cCUSTOMER_MODULE;
   Confidence confidence = Confidence::cLOW;
   bool valid = false;
};

struct ModelServiceHealth
{
   std::string schemaVersion = "nrm.model_service.health.v1";
   std::string modelId;
   std::string modelVersion;
   bool profileRepositoryAvailable = false;
   bool serviceDependenciesAvailable = false;
   bool healthy = false;
};

template<typename ResultType>
struct ModelServiceResponse
{
   std::string schemaVersion = cMODEL_SERVICE_RESPONSE_SCHEMA;
   std::string requestId;
   std::string correlationId;
   ModelServiceOperation operation = ModelServiceOperation::cGET_HEALTH;
   ModelServiceStatus status = ModelServiceStatus::cINVALID_REQUEST;
   std::string softwareVersion;
   std::uint64_t snapshotVersion = 0;
   ResultType result;
   std::vector<ModelServiceReason> reasons;
   DataOrigin source = DataOrigin::cDERIVED;
   Confidence confidence = Confidence::cHIGH;
   bool valid = false;
};

using CapabilityServiceResponse = ModelServiceResponse<CapabilityResult>;
using AssessmentServiceResponse = ModelServiceResponse<AssessmentResult>;
using PlanValidationServiceResponse = ModelServiceResponse<PlanValidationResult>;
using PlanEvaluationServiceResponse =
   ModelServiceResponse<NetworkPlanEvaluationResult>;
using DistributionPackageServiceResponse =
   ModelServiceResponse<DistributionPackageResult>;
using ResourceDemandServiceResponse =
   ModelServiceResponse<ResourceDemandBatchResult>;
using ModelHealthServiceResponse = ModelServiceResponse<ModelServiceHealth>;
using ModelDescriptorServiceResponse = ModelServiceResponse<ModelDescriptor>;

struct CapabilityModelServiceRequest
{
   ResourceSnapshot snapshot;
   CapabilityRequest request;
   EnvironmentContext environment;
};

struct AssessmentModelServiceRequest
{
   ResourceSnapshot snapshot;
   AssessmentTask task;
};

struct ValidatePlanModelServiceRequest
{
   ResourceSnapshot snapshot;
   NetworkPlanDocument plan;
};

struct EvaluatePlanModelServiceRequest
{
   ResourceSnapshot snapshot;
   NetworkPlanDocument plan;
   EnvironmentContext environment;
};

struct DistributionPackageModelServiceRequest
{
   NetworkPlanDocument plan;
   PlanValidationResult validation;
   NetworkPlanEvaluationResult evaluation;
   std::string outputRoot;
};

struct ResourceDemandModelServiceRequest
{
   ResourceSnapshot snapshot;
   ResourceDemandSet demandSet;
   bool hasPlan = false;
   NetworkPlanDocument plan;
   bool hasPlanEvaluation = false;
   NetworkPlanEvaluationResult planEvaluation;
   EnvironmentContext environment;
   bool hasCandidates = false;
   PlanningCandidateSet candidates;
};

struct ModelHealthRequest
{
};

struct ModelDescriptorRequest
{
};

} // namespace nrm

#endif
