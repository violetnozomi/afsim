/**
 * @file CustomerSnapshotAssembler.hpp
 * @brief Domain-aware immutable-style merge helpers for customer snapshots.
 */

#ifndef NRM_CUSTOMER_SNAPSHOT_ASSEMBLER_HPP
#define NRM_CUSTOMER_SNAPSHOT_ASSEMBLER_HPP

#include <algorithm>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
class CustomerSnapshotAssembler
{
public:
   static const std::string& NavigationPlatformId(const NavigationSample& aSample)
   {
      return aSample.platformId.empty() ? aSample.platformName : aSample.platformId;
   }

   static ResourceSnapshot MergeResources(const ResourceSnapshot& aCurrent,
                                          const ResourceSnapshot& aIncoming,
                                          bool aNewRun)
   {
      ResourceSnapshot result = aIncoming;
      result.snapshotVersion = aCurrent.snapshotVersion + 1;
      if (!aNewRun)
      {
         result.simTime = std::max(aCurrent.simTime, aIncoming.simTime);
         result.navigation = aCurrent.navigation;
         result.environment = aCurrent.environment;
      }
      return result;
   }

   static ResourceSnapshot UpsertNavigation(const ResourceSnapshot& aCurrent,
                                            const NavigationSample& aSample,
                                            const std::string& aProviderId,
                                            bool aNewRun)
   {
      ResourceSnapshot result = aNewRun ? ResourceSnapshot() : aCurrent;
      result.snapshotVersion = aCurrent.snapshotVersion + 1;
      result.simTime = std::max(result.simTime, aSample.sampleTime);
      result.navigation.valid = true;
      result.navigation.providerId = aProviderId;
      result.navigation.origin = aSample.origin;
      result.navigation.sampleTime =
         std::max(result.navigation.sampleTime, aSample.sampleTime);
      auto found = std::find_if(result.navigation.platforms.begin(),
                                result.navigation.platforms.end(),
                                [&aSample](const NavigationSample& aExisting)
                                {
                                   return NavigationPlatformId(aExisting) ==
                                          NavigationPlatformId(aSample);
                                });
      if (found == result.navigation.platforms.end())
         result.navigation.platforms.push_back(aSample);
      else
         *found = aSample;
      return result;
   }

   static ResourceSnapshot MergeEnvironment(const ResourceSnapshot& aCurrent,
                                             const EnvironmentSnapshot& aEnvironment,
                                             bool aNewRun)
   {
      ResourceSnapshot result = aNewRun ? ResourceSnapshot() : aCurrent;
      result.snapshotVersion = aCurrent.snapshotVersion + 1;
      result.simTime = std::max(result.simTime, aEnvironment.sampleTime);
      result.environment = aEnvironment;
      return result;
   }
};
} // namespace nrm

#endif
