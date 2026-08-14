#ifndef NRM_REFERENCE_MODEL_ADAPTER_HPP
#define NRM_REFERENCE_MODEL_ADAPTER_HPP

// Minimal third-party integration example for another AFSIM-derived module.
// It proves the public registry boundary only; it is not a formal customer ABI.

#include <cmath>
#include <string>
#include <utility>

#include "nrm/ModelServiceTypes.hpp"

namespace nrm
{
class ReferenceModelAdapter
{
public:
   explicit ReferenceModelAdapter(std::string aProviderId)
      : mProviderId(std::move(aProviderId))
   {
   }

   ModelDescriptor GetDescriptor() const
   {
      ModelDescriptor descriptor;
      descriptor.modelId = "nrm.reference_model";
      descriptor.modelName = "NRM AFSIM Reference Model Adapter";
      descriptor.modelVersion = "1.0.0";
      descriptor.providerId = mProviderId;
      descriptor.supportedOperations = {
         ModelServiceOperation::cGET_HEALTH,
         ModelServiceOperation::cGET_DESCRIPTOR};
      descriptor.supportedRequestSchemas = {cMODEL_SERVICE_REQUEST_SCHEMA};
      descriptor.supportedResponseSchemas = {cMODEL_SERVICE_RESPONSE_SCHEMA};
      descriptor.source = DataOrigin::cCUSTOMER_MODULE;
      descriptor.confidence = Confidence::cLOW;
      descriptor.valid = !mProviderId.empty();
      return descriptor;
   }

   ModelHealthServiceResponse GetHealth(
      const ModelServiceContext& aContext) const
   {
      ModelHealthServiceResponse response = Base<ModelServiceHealth>(
         aContext, ModelServiceOperation::cGET_HEALTH);
      if (!ValidContext(aContext) || mProviderId.empty()) return response;
      const ModelDescriptor descriptor = GetDescriptor();
      response.result.modelId = descriptor.modelId;
      response.result.modelVersion = descriptor.modelVersion;
      response.result.profileRepositoryAvailable = true;
      response.result.serviceDependenciesAvailable = true;
      response.result.healthy = true;
      Complete(response);
      return response;
   }

   ModelDescriptorServiceResponse Describe(
      const ModelServiceContext& aContext) const
   {
      ModelDescriptorServiceResponse response = Base<ModelDescriptor>(
         aContext, ModelServiceOperation::cGET_DESCRIPTOR);
      if (!ValidContext(aContext) || mProviderId.empty()) return response;
      response.result = GetDescriptor();
      Complete(response);
      return response;
   }

private:
   static bool ValidContext(const ModelServiceContext& aContext)
   {
      return aContext.valid && !aContext.requestId.empty() &&
             std::isfinite(aContext.requestTime) && aContext.requestTime >= 0.0;
   }

   template<typename ResultType>
   static ModelServiceResponse<ResultType> Base(
      const ModelServiceContext& aContext, ModelServiceOperation aOperation)
   {
      ModelServiceResponse<ResultType> response;
      response.requestId = aContext.requestId;
      response.correlationId = aContext.correlationId;
      response.operation = aOperation;
      response.snapshotVersion = aContext.snapshotVersion;
      response.softwareVersion = "reference-1.0.0";
      response.status = ModelServiceStatus::cINVALID_REQUEST;
      response.reasons = {ModelServiceReason::cCONTEXT_INVALID};
      response.source = DataOrigin::cDERIVED;
      response.confidence = Confidence::cHIGH;
      return response;
   }

   template<typename ResultType>
   static void Complete(ModelServiceResponse<ResultType>& aResponse)
   {
      aResponse.status = ModelServiceStatus::cSUCCESS;
      aResponse.reasons = {ModelServiceReason::cNONE};
      aResponse.valid = true;
   }

   std::string mProviderId;
};
} // namespace nrm

#endif
