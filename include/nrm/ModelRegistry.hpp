#ifndef NRM_MODEL_REGISTRY_HPP
#define NRM_MODEL_REGISTRY_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "nrm/ModelServiceTypes.hpp"

namespace nrm
{
enum class ModelRegistryReason
{
   cNONE,
   cINVALID_DESCRIPTOR,
   cINVALID_MODEL_VERSION,
   cDUPLICATE_MODEL_VERSION,
   cMODEL_NOT_FOUND
};

inline const char* ToString(ModelRegistryReason aReason)
{
   switch (aReason)
   {
   case ModelRegistryReason::cNONE: return "NONE";
   case ModelRegistryReason::cINVALID_DESCRIPTOR: return "INVALID_DESCRIPTOR";
   case ModelRegistryReason::cINVALID_MODEL_VERSION:
      return "INVALID_MODEL_VERSION";
   case ModelRegistryReason::cDUPLICATE_MODEL_VERSION:
      return "DUPLICATE_MODEL_VERSION";
   case ModelRegistryReason::cMODEL_NOT_FOUND: return "MODEL_NOT_FOUND";
   }
   return "INVALID_DESCRIPTOR";
}

enum class ModelSchemaDirection
{
   cREQUEST,
   cRESPONSE
};

struct ModelRegistryResult
{
   bool success = false;
   ModelRegistryReason reason = ModelRegistryReason::cNONE;
   std::string modelId;
   std::string modelVersion;
};

class ModelRegistry
{
public:
   ModelRegistryResult Register(const ModelDescriptor& aDescriptor)
   {
      ModelRegistryReason validationReason = ModelRegistryReason::cNONE;
      if (!ValidateDescriptor(aDescriptor, validationReason))
         return Result(false, validationReason, aDescriptor.modelId,
                       aDescriptor.modelVersion);
      ModelDescriptor existing;
      if (Find(aDescriptor.modelId, aDescriptor.modelVersion, existing))
         return Result(false, ModelRegistryReason::cDUPLICATE_MODEL_VERSION,
                       aDescriptor.modelId, aDescriptor.modelVersion);

      mDescriptors.push_back(aDescriptor);
      std::sort(mDescriptors.begin(), mDescriptors.end(), DescriptorLess);
      return Result(true, ModelRegistryReason::cNONE,
                    aDescriptor.modelId, aDescriptor.modelVersion);
   }

   ModelRegistryResult Unregister(const std::string& aModelId,
                                  const std::string& aModelVersion)
   {
      const auto iterator = std::find_if(
         mDescriptors.begin(), mDescriptors.end(),
         [&aModelId, &aModelVersion](const ModelDescriptor& aDescriptor)
         {
            return aDescriptor.modelId == aModelId &&
                   aDescriptor.modelVersion == aModelVersion;
         });
      if (iterator == mDescriptors.end())
         return Result(false, ModelRegistryReason::cMODEL_NOT_FOUND,
                       aModelId, aModelVersion);
      mDescriptors.erase(iterator);
      return Result(true, ModelRegistryReason::cNONE, aModelId, aModelVersion);
   }

   bool Find(
      const std::string& aModelId,
      const std::string& aModelVersion,
      ModelDescriptor& aDescriptor) const
   {
      const auto iterator = std::find_if(
         mDescriptors.begin(), mDescriptors.end(),
         [&aModelId, &aModelVersion](const ModelDescriptor& aDescriptor)
         {
            return aDescriptor.modelId == aModelId &&
                   aDescriptor.modelVersion == aModelVersion;
         });
      if (iterator == mDescriptors.end()) return false;
      aDescriptor = *iterator;
      return true;
   }

   std::vector<ModelDescriptor> List() const { return mDescriptors; }

   bool SupportsOperation(const std::string& aModelId,
                          const std::string& aModelVersion,
                          ModelServiceOperation aOperation) const
   {
      ModelDescriptor descriptor;
      return Find(aModelId, aModelVersion, descriptor) &&
             std::find(descriptor.supportedOperations.begin(),
                       descriptor.supportedOperations.end(), aOperation) !=
                descriptor.supportedOperations.end();
   }

   bool SupportsSchema(const std::string& aModelId,
                       const std::string& aModelVersion,
                       const std::string& aSchema,
                       ModelSchemaDirection aDirection) const
   {
      ModelDescriptor descriptor;
      if (!Find(aModelId, aModelVersion, descriptor)) return false;
      const std::vector<std::string>& schemas =
         aDirection == ModelSchemaDirection::cREQUEST
            ? descriptor.supportedRequestSchemas
            : descriptor.supportedResponseSchemas;
      return std::find(schemas.begin(), schemas.end(), aSchema) != schemas.end();
   }

private:
   using VersionParts = std::array<std::uint64_t, 3>;

   static ModelRegistryResult Result(bool aSuccess,
                                     ModelRegistryReason aReason,
                                     const std::string& aModelId,
                                     const std::string& aModelVersion)
   {
      ModelRegistryResult result;
      result.success = aSuccess;
      result.reason = aReason;
      result.modelId = aModelId;
      result.modelVersion = aModelVersion;
      return result;
   }

   static bool ParseVersion(const std::string& aVersion,
                            VersionParts& aParts)
   {
      std::size_t start = 0;
      for (std::size_t index = 0; index < aParts.size(); ++index)
      {
         const std::size_t end = aVersion.find('.', start);
         if ((index + 1 < aParts.size() && end == std::string::npos) ||
             (index + 1 == aParts.size() && end != std::string::npos))
            return false;
         const std::string part = aVersion.substr(
            start, end == std::string::npos ? std::string::npos : end - start);
         if (part.empty() || (part.size() > 1 && part.front() == '0')) return false;

         std::uint64_t value = 0;
         for (char character : part)
         {
            if (character < '0' || character > '9') return false;
            const std::uint64_t digit =
               static_cast<std::uint64_t>(character - '0');
            if (value > (std::numeric_limits<std::uint64_t>::max() - digit) / 10)
               return false;
            value = value * 10 + digit;
         }
         aParts[index] = value;
         start = end == std::string::npos ? aVersion.size() : end + 1;
      }
      return start == aVersion.size();
   }

   static bool HasEmptyOrDuplicateSchema(
      const std::vector<std::string>& aSchemas)
   {
      if (aSchemas.empty()) return true;
      std::set<std::string> unique;
      for (const std::string& schema : aSchemas)
      {
         if (schema.empty() || !unique.insert(schema).second) return true;
      }
      return false;
   }

   static bool ValidateDescriptor(const ModelDescriptor& aDescriptor,
                                  ModelRegistryReason& aReason)
   {
      VersionParts version{};
      if (!ParseVersion(aDescriptor.modelVersion, version))
      {
         aReason = ModelRegistryReason::cINVALID_MODEL_VERSION;
         return false;
      }
      if (!aDescriptor.valid || aDescriptor.modelId.empty() ||
          aDescriptor.modelName.empty() || aDescriptor.providerId.empty() ||
          aDescriptor.supportedOperations.empty() ||
          HasEmptyOrDuplicateSchema(aDescriptor.supportedRequestSchemas) ||
          HasEmptyOrDuplicateSchema(aDescriptor.supportedResponseSchemas))
      {
         aReason = ModelRegistryReason::cINVALID_DESCRIPTOR;
         return false;
      }
      std::set<ModelServiceOperation> operations;
      for (ModelServiceOperation operation : aDescriptor.supportedOperations)
      {
         if (!IsKnownOperation(operation) ||
             !operations.insert(operation).second)
         {
            aReason = ModelRegistryReason::cINVALID_DESCRIPTOR;
            return false;
         }
      }
      aReason = ModelRegistryReason::cNONE;
      return true;
   }

   static bool IsKnownOperation(ModelServiceOperation aOperation)
   {
      switch (aOperation)
      {
      case ModelServiceOperation::cQUERY_CAPABILITY:
      case ModelServiceOperation::cVALIDATE_PLAN:
      case ModelServiceOperation::cEVALUATE_PLAN:
      case ModelServiceOperation::cGENERATE_DISTRIBUTION_PACKAGE:
      case ModelServiceOperation::cMATCH_RESOURCE_DEMANDS:
      case ModelServiceOperation::cGET_HEALTH:
      case ModelServiceOperation::cGET_DESCRIPTOR:
         return true;
      }
      return false;
   }

   static bool DescriptorLess(const ModelDescriptor& aLeft,
                              const ModelDescriptor& aRight)
   {
      if (aLeft.modelId != aRight.modelId)
         return aLeft.modelId < aRight.modelId;
      VersionParts leftVersion{};
      VersionParts rightVersion{};
      ParseVersion(aLeft.modelVersion, leftVersion);
      ParseVersion(aRight.modelVersion, rightVersion);
      if (leftVersion != rightVersion) return leftVersion < rightVersion;
      if (aLeft.providerId != aRight.providerId)
         return aLeft.providerId < aRight.providerId;
      return aLeft.modelVersion < aRight.modelVersion;
   }

   std::vector<ModelDescriptor> mDescriptors;
};
} // namespace nrm

#endif
