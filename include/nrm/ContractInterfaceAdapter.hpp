#ifndef NRM_CONTRACT_INTERFACE_ADAPTER_HPP
#define NRM_CONTRACT_INTERFACE_ADAPTER_HPP

#include <string>
#include <vector>

#include "nrm/ModelServiceTypes.hpp"

namespace nrm
{
class ContractInterfaceRequest
{
public:
   virtual ~ContractInterfaceRequest() = default;
};

class ContractInterfaceResponse
{
public:
   virtual ~ContractInterfaceResponse() = default;
};

enum class ContractAdapterStatus
{
   cSUCCESS,
   cINVALID_REQUEST,
   cUNSUPPORTED_SCHEMA,
   cCONVERSION_FAILED
};

inline const char* ToString(ContractAdapterStatus aStatus)
{
   switch (aStatus)
   {
   case ContractAdapterStatus::cSUCCESS: return "SUCCESS";
   case ContractAdapterStatus::cINVALID_REQUEST: return "INVALID_REQUEST";
   case ContractAdapterStatus::cUNSUPPORTED_SCHEMA: return "UNSUPPORTED_SCHEMA";
   case ContractAdapterStatus::cCONVERSION_FAILED: return "CONVERSION_FAILED";
   }
   return "CONVERSION_FAILED";
}

enum class ContractAdapterReason
{
   cNONE,
   cADAPTER_INVALID,
   cREQUEST_TYPE_UNSUPPORTED,
   cREQUEST_SCHEMA_UNSUPPORTED,
   cRESPONSE_SCHEMA_UNSUPPORTED,
   cREQUEST_CONVERSION_FAILED,
   cRESPONSE_CONVERSION_FAILED
};

inline const char* ToString(ContractAdapterReason aReason)
{
   switch (aReason)
   {
   case ContractAdapterReason::cNONE: return "NONE";
   case ContractAdapterReason::cADAPTER_INVALID: return "ADAPTER_INVALID";
   case ContractAdapterReason::cREQUEST_TYPE_UNSUPPORTED:
      return "REQUEST_TYPE_UNSUPPORTED";
   case ContractAdapterReason::cREQUEST_SCHEMA_UNSUPPORTED:
      return "REQUEST_SCHEMA_UNSUPPORTED";
   case ContractAdapterReason::cRESPONSE_SCHEMA_UNSUPPORTED:
      return "RESPONSE_SCHEMA_UNSUPPORTED";
   case ContractAdapterReason::cREQUEST_CONVERSION_FAILED:
      return "REQUEST_CONVERSION_FAILED";
   case ContractAdapterReason::cRESPONSE_CONVERSION_FAILED:
      return "RESPONSE_CONVERSION_FAILED";
   }
   return "ADAPTER_INVALID";
}

struct ContractAdapterResult
{
   ContractAdapterStatus status = ContractAdapterStatus::cINVALID_REQUEST;
   ContractAdapterReason reason = ContractAdapterReason::cADAPTER_INVALID;
   bool valid = false;
};

class ContractInterfaceAdapter
{
public:
   virtual ~ContractInterfaceAdapter() = default;

   virtual std::string AdapterId() const = 0;
   virtual std::string AdapterVersion() const = 0;
   virtual std::vector<std::string> SupportedRequestSchemas() const = 0;
   virtual std::vector<std::string> SupportedResponseSchemas() const = 0;

   virtual ContractAdapterResult Validate(
      const ContractInterfaceRequest& aExternalRequest) const = 0;

   virtual ContractAdapterResult DecodeNavigationReport(
      const ContractInterfaceRequest& aExternalRequest,
      NavigationSample& aSample) const = 0;

   virtual ContractAdapterResult DecodeEnvironmentReport(
      const ContractInterfaceRequest& aExternalRequest,
      EnvironmentSnapshot& aSnapshot,
      EnvironmentContext& aContext) const = 0;

   virtual ContractAdapterResult DecodeResourceReport(
      const ContractInterfaceRequest& aExternalRequest,
      ResourceSnapshot& aSnapshot) const = 0;

   virtual ContractAdapterResult DecodeCapabilityRequest(
      const ContractInterfaceRequest& aExternalRequest,
      ModelServiceContext& aContext,
      CapabilityModelServiceRequest& aRequest) const = 0;

   virtual ContractAdapterResult DecodeValidatePlanRequest(
      const ContractInterfaceRequest& aExternalRequest,
      ModelServiceContext& aContext,
      ValidatePlanModelServiceRequest& aRequest) const = 0;

   virtual ContractAdapterResult DecodeEvaluatePlanRequest(
      const ContractInterfaceRequest& aExternalRequest,
      ModelServiceContext& aContext,
      EvaluatePlanModelServiceRequest& aRequest) const = 0;

   virtual ContractAdapterResult DecodeDistributionPackageRequest(
      const ContractInterfaceRequest& aExternalRequest,
      ModelServiceContext& aContext,
      DistributionPackageModelServiceRequest& aRequest) const = 0;

   virtual ContractAdapterResult DecodeResourceDemandRequest(
      const ContractInterfaceRequest& aExternalRequest,
      ModelServiceContext& aContext,
      ResourceDemandModelServiceRequest& aRequest) const = 0;

   virtual ContractAdapterResult DecodeHealthRequest(
      const ContractInterfaceRequest& aExternalRequest,
      ModelServiceContext& aContext,
      ModelHealthRequest& aRequest) const = 0;

   virtual ContractAdapterResult DecodeDescriptorRequest(
      const ContractInterfaceRequest& aExternalRequest,
      ModelServiceContext& aContext,
      ModelDescriptorRequest& aRequest) const = 0;

   virtual ContractAdapterResult EncodeCapabilityResponse(
      const CapabilityServiceResponse& aResponse,
      ContractInterfaceResponse& aExternalResponse) const = 0;

   virtual ContractAdapterResult EncodePlanValidationResponse(
      const PlanValidationServiceResponse& aResponse,
      ContractInterfaceResponse& aExternalResponse) const = 0;

   virtual ContractAdapterResult EncodePlanEvaluationResponse(
      const PlanEvaluationServiceResponse& aResponse,
      ContractInterfaceResponse& aExternalResponse) const = 0;

   virtual ContractAdapterResult EncodeDistributionPackageResponse(
      const DistributionPackageServiceResponse& aResponse,
      ContractInterfaceResponse& aExternalResponse) const = 0;

   virtual ContractAdapterResult EncodeResourceDemandResponse(
      const ResourceDemandServiceResponse& aResponse,
      ContractInterfaceResponse& aExternalResponse) const = 0;

   virtual ContractAdapterResult EncodeHealthResponse(
      const ModelHealthServiceResponse& aResponse,
      ContractInterfaceResponse& aExternalResponse) const = 0;

   virtual ContractAdapterResult EncodeDescriptorResponse(
      const ModelDescriptorServiceResponse& aResponse,
      ContractInterfaceResponse& aExternalResponse) const = 0;
};
} // namespace nrm

#endif
