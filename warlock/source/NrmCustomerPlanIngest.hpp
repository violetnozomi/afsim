#ifndef NRM_CUSTOMER_PLAN_INGEST_HPP
#define NRM_CUSTOMER_PLAN_INGEST_HPP

// Keeps JSON decoding success separate from repository acceptance so customer
// acknowledgements cannot report ACCEPTED for a plan that was not installed.

#include "NrmCustomerJsonCodec.hpp"

#include "nrm/NetworkPlanRepository.hpp"

namespace WkNrm
{
inline std::string CustomerPlanErrorPath(const std::string& aField)
{
   if (aField.find("allocation") != std::string::npos)
      return "/data/allocations";
   if (aField.find("demand") != std::string::npos ||
       aField == "minimumPdrPercent")
      return "/data/demands";
   if (aField.empty()) return "/data";
   return "/data/" + aField;
}

inline bool AcceptDecodedNetworkPlan(
   CustomerJsonDecodeResult& aDecodeResult,
   const nrm::NetworkPlanDocument& aPlan,
   nrm::NetworkPlanRepository& aRepository)
{
   if (!aDecodeResult.valid) return false;
   if (aRepository.ReplaceDraft(aPlan)) return true;

   const nrm::PlanRepositoryResult& repositoryResult =
      aRepository.LastLoadResult();
   aDecodeResult.valid = false;
   aDecodeResult.errors.push_back(
      {"PLAN_REJECTED", CustomerPlanErrorPath(repositoryResult.field),
       nrm::ToString(repositoryResult.reason)});
   return false;
}
}

#endif
