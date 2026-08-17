#include "nrm/ContractInterfaceAdapter.hpp"
#include "nrm/ModelRegistry.hpp"
#include "nrm/ModelServiceFacade.hpp"

#include <cassert>
#include <string>
#include <type_traits>
#include <vector>

static_assert(std::is_abstract<nrm::ContractInterfaceAdapter>::value,
              "The contract interface must remain an abstract boundary.");

namespace
{
nrm::ModelDescriptor Descriptor(const char* aId,
                                const char* aVersion,
                                const char* aProvider)
{
   nrm::ModelDescriptor descriptor;
   descriptor.modelId = aId;
   descriptor.modelName = std::string("Model ") + aId;
   descriptor.modelVersion = aVersion;
   descriptor.providerId = aProvider;
   descriptor.supportedOperations = {
      nrm::ModelServiceOperation::cGET_HEALTH,
      nrm::ModelServiceOperation::cGET_DESCRIPTOR};
   descriptor.supportedRequestSchemas = {
      nrm::cMODEL_SERVICE_REQUEST_SCHEMA};
   descriptor.supportedResponseSchemas = {
      nrm::cMODEL_SERVICE_RESPONSE_SCHEMA};
   descriptor.source = nrm::DataOrigin::cCUSTOMER_MODULE;
   descriptor.confidence = nrm::Confidence::cMEDIUM;
   descriptor.valid = true;
   return descriptor;
}
} // namespace

int main()
{
   nrm::ModelRegistry registry;
   const nrm::ModelDescriptor nrmDescriptor = nrm::ModelServiceFacade::Descriptor();
   const nrm::ModelRegistryResult registered = registry.Register(nrmDescriptor);
   assert(registered.success);
   assert(registered.reason == nrm::ModelRegistryReason::cNONE);

   nrm::ModelDescriptor found;
   assert(registry.Find(nrmDescriptor.modelId, nrmDescriptor.modelVersion,
                        found));
   assert(found.providerId == nrmDescriptor.providerId);
   assert(!registry.Find(nrmDescriptor.modelId, "9.9.9", found));
   assert(registry.SupportsOperation(
      nrmDescriptor.modelId, nrmDescriptor.modelVersion,
      nrm::ModelServiceOperation::cQUERY_CAPABILITY));
   assert(registry.SupportsOperation(
      nrmDescriptor.modelId, nrmDescriptor.modelVersion,
      nrm::ModelServiceOperation::cEVALUATE_ASSESSMENT));
   assert(!registry.SupportsOperation(
      nrmDescriptor.modelId, nrmDescriptor.modelVersion,
      static_cast<nrm::ModelServiceOperation>(99)));
   assert(registry.SupportsSchema(
      nrmDescriptor.modelId, nrmDescriptor.modelVersion,
      nrm::cMODEL_SERVICE_REQUEST_SCHEMA,
      nrm::ModelSchemaDirection::cREQUEST));
   assert(registry.SupportsSchema(
      nrmDescriptor.modelId, nrmDescriptor.modelVersion,
      nrm::cMODEL_SERVICE_RESPONSE_SCHEMA,
      nrm::ModelSchemaDirection::cRESPONSE));

   const std::size_t countBeforeDuplicate = registry.List().size();
   const nrm::ModelRegistryResult duplicate = registry.Register(nrmDescriptor);
   assert(!duplicate.success);
   assert(duplicate.reason ==
          nrm::ModelRegistryReason::cDUPLICATE_MODEL_VERSION);
   assert(registry.List().size() == countBeforeDuplicate);

   nrm::ModelDescriptor invalid = Descriptor("", "1.0.0", "provider");
   const nrm::ModelRegistryResult emptyId = registry.Register(invalid);
   assert(!emptyId.success);
   assert(emptyId.reason == nrm::ModelRegistryReason::cINVALID_DESCRIPTOR);
   assert(registry.List().size() == countBeforeDuplicate);

   invalid = Descriptor("invalid-version", "1.0", "provider");
   const nrm::ModelRegistryResult invalidVersion = registry.Register(invalid);
   assert(!invalidVersion.success);
   assert(invalidVersion.reason ==
          nrm::ModelRegistryReason::cINVALID_MODEL_VERSION);
   assert(registry.List().size() == countBeforeDuplicate);

   invalid = Descriptor("no-operations", "1.0.0", "provider");
   invalid.supportedOperations.clear();
   assert(!registry.Register(invalid).success);
   assert(registry.List().size() == countBeforeDuplicate);

   invalid = Descriptor("unknown-operation", "1.0.0", "provider");
   invalid.supportedOperations.push_back(
      static_cast<nrm::ModelServiceOperation>(99));
   assert(!registry.Register(invalid).success);
   assert(registry.List().size() == countBeforeDuplicate);

   const nrm::ModelDescriptor versionOne =
      Descriptor("same-model", "1.0.0", "provider-b");
   const nrm::ModelDescriptor versionTen =
      Descriptor("same-model", "1.10.0", "provider-a");
   const nrm::ModelDescriptor versionTwo =
      Descriptor("same-model", "1.2.0", "provider-c");
   assert(registry.Register(versionTen).success);
   assert(registry.Register(versionOne).success);
   assert(registry.Register(versionTwo).success);
   assert(registry.Find("same-model", "1.0.0", found));
   assert(registry.Find("same-model", "1.2.0", found));
   assert(registry.Find("same-model", "1.10.0", found));

   assert(registry.Register(Descriptor("z-model", "2.0.0", "provider-z")).success);
   assert(registry.Register(Descriptor("a-model", "3.0.0", "provider-a")).success);
   const std::vector<nrm::ModelDescriptor> listed = registry.List();
   assert(listed.front().modelId == "a-model");
   std::size_t sameModelIndex = 0;
   while (sameModelIndex < listed.size() &&
          listed[sameModelIndex].modelId != "same-model")
      ++sameModelIndex;
   assert(sameModelIndex + 2 < listed.size());
   assert(listed[sameModelIndex].modelVersion == "1.0.0");
   assert(listed[sameModelIndex + 1].modelVersion == "1.2.0");
   assert(listed[sameModelIndex + 2].modelVersion == "1.10.0");
   assert(listed.back().modelId == "z-model");

   const nrm::ModelRegistryResult removed =
      registry.Unregister("same-model", "1.2.0");
   assert(removed.success);
   assert(!registry.Find("same-model", "1.2.0", found));
   assert(registry.Find("same-model", "1.0.0", found));
   assert(registry.Find("same-model", "1.10.0", found));
   const nrm::ModelRegistryResult missing =
      registry.Unregister("same-model", "9.9.9");
   assert(!missing.success);
   assert(missing.reason == nrm::ModelRegistryReason::cMODEL_NOT_FOUND);
   return 0;
}
