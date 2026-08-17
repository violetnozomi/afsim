/**
 * @file EffectiveSnapshotAssembler.hpp
 * @brief Deterministic authority merge for AFSIM base and customer overlays.
 */

#ifndef NRM_EFFECTIVE_SNAPSHOT_ASSEMBLER_HPP
#define NRM_EFFECTIVE_SNAPSHOT_ASSEMBLER_HPP

#include <algorithm>

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

      // AFSIM owns live networks/endpoints/links. Customer navigation and
      // environment fill domains that the customer module explicitly owns.
      if (aCustomerPtr->navigation.valid)
         result.navigation = aCustomerPtr->navigation;
      if (aCustomerPtr->environment.valid)
         result.environment = aCustomerPtr->environment;
      result.simTime = std::max(result.simTime, aCustomerPtr->simTime);
      return result;
   }
};
} // namespace nrm

#endif
