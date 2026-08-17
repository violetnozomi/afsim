/**
 * @file EffectiveSnapshotAssembler.hpp
 * @brief Deterministic authority merge for AFSIM base and customer overlays.
 */

#ifndef NRM_EFFECTIVE_SNAPSHOT_ASSEMBLER_HPP
#define NRM_EFFECTIVE_SNAPSHOT_ASSEMBLER_HPP

#include <algorithm>

#include "nrm/CustomerSnapshotAssembler.hpp"
#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
class EffectiveSnapshotAssembler
{
public:
   static ResourceSnapshot Compose(const ResourceSnapshot* aAfsimBasePtr,
                                   const ResourceSnapshot* aCustomerPtr)
   {
      if (aAfsimBasePtr == nullptr)
         return aCustomerPtr == nullptr ? ResourceSnapshot() : *aCustomerPtr;

      ResourceSnapshot result = *aAfsimBasePtr;
      if (aCustomerPtr == nullptr) return result;

      // AFSIM owns live networks/endpoints/links. Customer navigation is a
      // per-platform overlay and environment is a per-subdomain overlay.
      MergeNavigation(result.navigation, aCustomerPtr->navigation);
      MergeEnvironment(result.environment, aCustomerPtr->environment);
      result.simTime = std::max(result.simTime, aCustomerPtr->simTime);
      return result;
   }

private:
   static void MergeNavigation(NavigationSnapshot& aEffective,
                               const NavigationSnapshot& aCustomer)
   {
      if (!aCustomer.valid) return;
      for (const NavigationSample& sample : aCustomer.platforms)
      {
         const std::string& platformId =
            CustomerSnapshotAssembler::NavigationPlatformId(sample);
         const auto found = std::find_if(
            aEffective.platforms.begin(), aEffective.platforms.end(),
            [&platformId](const NavigationSample& aExisting)
            {
               return CustomerSnapshotAssembler::NavigationPlatformId(aExisting) ==
                      platformId;
            });
         if (found == aEffective.platforms.end())
            aEffective.platforms.push_back(sample);
         else
            *found = sample;
      }
      aEffective.valid = true;
      aEffective.sampleTime = std::max(aEffective.sampleTime, aCustomer.sampleTime);
      aEffective.schemaVersion = aCustomer.schemaVersion;
      aEffective.packetFormat = aCustomer.packetFormat;
      aEffective.providerId = aCustomer.providerId;
      aEffective.origin = aCustomer.origin;
      aEffective.confidence = aCustomer.confidence;
   }

   static void MergeEnvironment(EnvironmentSnapshot& aEffective,
                                const EnvironmentSnapshot& aCustomer)
   {
      if (!aCustomer.valid) return;
      if (aCustomer.terrain.available)
         aEffective.terrain = aCustomer.terrain;
      if (aCustomer.weather.available)
         aEffective.weather = aCustomer.weather;
      if (aCustomer.celestial.available)
         aEffective.celestial = aCustomer.celestial;
      if (aCustomer.interference.available)
         aEffective.interference = aCustomer.interference;
      aEffective.valid = true;
      aEffective.sampleTime = std::max(aEffective.sampleTime, aCustomer.sampleTime);
      aEffective.schemaVersion = aCustomer.schemaVersion;
      if (!aCustomer.configVersion.empty())
         aEffective.configVersion = aCustomer.configVersion;
      aEffective.providerId = aCustomer.providerId;
      aEffective.origin = aCustomer.origin;
      aEffective.confidence = aCustomer.confidence;
   }
};
} // namespace nrm

#endif
