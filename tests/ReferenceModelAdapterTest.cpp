#include "nrm/ModelRegistry.hpp"
#include "nrm/ReferenceModelAdapter.hpp"

#include <cassert>

int main()
{
   const nrm::ReferenceModelAdapter adapter("customer-afsim-reference");
   const nrm::ModelDescriptor descriptor = adapter.GetDescriptor();
   assert(descriptor.valid);
   assert(descriptor.providerId == "customer-afsim-reference");

   nrm::ModelRegistry registry;
   assert(registry.Register(descriptor).success);
   assert(registry.SupportsOperation(
      descriptor.modelId, descriptor.modelVersion,
      nrm::ModelServiceOperation::cGET_HEALTH));
   assert(registry.SupportsSchema(
      descriptor.modelId, descriptor.modelVersion,
      nrm::cMODEL_SERVICE_REQUEST_SCHEMA,
      nrm::ModelSchemaDirection::cREQUEST));

   nrm::ModelServiceContext context;
   context.requestId = "request-01";
   context.correlationId = "correlation-01";
   context.requestTime = 10.0;
   context.valid = true;
   const nrm::ModelHealthServiceResponse health = adapter.GetHealth(context);
   assert(health.valid);
   assert(health.status == nrm::ModelServiceStatus::cSUCCESS);
   assert(health.requestId == context.requestId);
   assert(health.correlationId == context.correlationId);
   assert(health.result.healthy);

   const nrm::ModelDescriptorServiceResponse described =
      adapter.Describe(context);
   assert(described.valid);
   assert(described.result.modelId == descriptor.modelId);

   assert(registry.Unregister(descriptor.modelId,
                              descriptor.modelVersion).success);
   nrm::ModelDescriptor absent;
   assert(!registry.Find(descriptor.modelId, descriptor.modelVersion, absent));
   return 0;
}
