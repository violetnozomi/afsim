#ifndef NRM_NETWORK_PLAN_TYPES_HPP
#define NRM_NETWORK_PLAN_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "nrm/CommunicationCapabilityTypes.hpp"
#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
enum class NetworkPlanState
{
   cDRAFT,
   cVALIDATED,
   cREJECTED,
   cREADY_FOR_DISTRIBUTION
};

inline const char* ToString(NetworkPlanState aState)
{
   switch (aState)
   {
   case NetworkPlanState::cDRAFT: return "DRAFT";
   case NetworkPlanState::cVALIDATED: return "VALIDATED";
   case NetworkPlanState::cREJECTED: return "REJECTED";
   case NetworkPlanState::cREADY_FOR_DISTRIBUTION: return "READY_FOR_DISTRIBUTION";
   }
   return "REJECTED";
}

enum class PlanValidationReason
{
   cNONE,
   cFILE_OPEN_FAILED,
   cFILE_WRITE_FAILED,
   cATOMIC_RENAME_FAILED,
   cPARSE_ERROR,
   cUNSUPPORTED_SCHEMA,
   cUNKNOWN_RECORD_TYPE,
   cMISSING_REQUIRED_FIELD,
   cTRAILING_TOKEN,
   cDUPLICATE_PLAN_REVISION,
   cDUPLICATE_ALLOCATION_ID,
   cDUPLICATE_DEMAND_ID,
   cDUPLICATE_CHANGE_ID,
   cREVISION_NOT_INCREMENTED,
   cINVALID_PLAN_ID,
   cINVALID_REVISION,
   cINVALID_PROVIDER_ID,
   cCONFIG_VERSION_MISMATCH,
   cNON_FINITE_VALUE,
   cNEGATIVE_VALUE,
   cPDR_OUT_OF_RANGE,
   cUNKNOWN_NETWORK_TYPE,
   cSOURCE_EQUALS_DESTINATION,
   cPROFILE_NOT_FOUND,
   cFREQUENCY_NOT_SUPPORTED,
   cPLATFORM_NOT_FOUND,
   cREFERENCE_NOT_FOUND,
   cALLOWED_NETWORK_NOT_ALLOCATED,
   cBUSINESS_TYPE_NOT_SUPPORTED,
   cMEMBER_LIMIT_EXCEEDED,
   cDUPLICATE_MEMBER,
   cRESOURCE_CONFLICT,
   cCHANGE_CONFLICT,
   cALREADY_MEMBER,
   cNOT_MEMBER,
   cCUSTOMER_RULE_UNAVAILABLE,
   cVALIDATION_FAILED,
   cEVALUATION_FAILED,
   cDATA_INVALID,
   cNO_PATH,
   cNODE_OFFLINE,
   cBANDWIDTH_NOT_MET,
   cDELAY_NOT_MET,
   cPDR_NOT_MET,
   cPLAN_NOT_VALIDATED,
   cPLAN_IDENTITY_MISMATCH,
   cOUTPUT_PATH_INVALID,
   cPACKAGE_ALREADY_EXISTS,
   cNO_CURRENT_PLAN,
   cADAPTER_UNAVAILABLE
};

inline const char* ToString(PlanValidationReason aReason)
{
   switch (aReason)
   {
   case PlanValidationReason::cNONE: return "NONE";
   case PlanValidationReason::cFILE_OPEN_FAILED: return "FILE_OPEN_FAILED";
   case PlanValidationReason::cFILE_WRITE_FAILED: return "FILE_WRITE_FAILED";
   case PlanValidationReason::cATOMIC_RENAME_FAILED: return "ATOMIC_RENAME_FAILED";
   case PlanValidationReason::cPARSE_ERROR: return "PARSE_ERROR";
   case PlanValidationReason::cUNSUPPORTED_SCHEMA: return "UNSUPPORTED_SCHEMA";
   case PlanValidationReason::cUNKNOWN_RECORD_TYPE: return "UNKNOWN_RECORD_TYPE";
   case PlanValidationReason::cMISSING_REQUIRED_FIELD: return "MISSING_REQUIRED_FIELD";
   case PlanValidationReason::cTRAILING_TOKEN: return "TRAILING_TOKEN";
   case PlanValidationReason::cDUPLICATE_PLAN_REVISION: return "DUPLICATE_PLAN_REVISION";
   case PlanValidationReason::cDUPLICATE_ALLOCATION_ID: return "DUPLICATE_ALLOCATION_ID";
   case PlanValidationReason::cDUPLICATE_DEMAND_ID: return "DUPLICATE_DEMAND_ID";
   case PlanValidationReason::cDUPLICATE_CHANGE_ID: return "DUPLICATE_CHANGE_ID";
   case PlanValidationReason::cREVISION_NOT_INCREMENTED: return "REVISION_NOT_INCREMENTED";
   case PlanValidationReason::cINVALID_PLAN_ID: return "INVALID_PLAN_ID";
   case PlanValidationReason::cINVALID_REVISION: return "INVALID_REVISION";
   case PlanValidationReason::cINVALID_PROVIDER_ID: return "INVALID_PROVIDER_ID";
   case PlanValidationReason::cCONFIG_VERSION_MISMATCH: return "CONFIG_VERSION_MISMATCH";
   case PlanValidationReason::cNON_FINITE_VALUE: return "NON_FINITE_VALUE";
   case PlanValidationReason::cNEGATIVE_VALUE: return "NEGATIVE_VALUE";
   case PlanValidationReason::cPDR_OUT_OF_RANGE: return "PDR_OUT_OF_RANGE";
   case PlanValidationReason::cUNKNOWN_NETWORK_TYPE: return "UNKNOWN_NETWORK_TYPE";
   case PlanValidationReason::cSOURCE_EQUALS_DESTINATION: return "SOURCE_EQUALS_DESTINATION";
   case PlanValidationReason::cPROFILE_NOT_FOUND: return "PROFILE_NOT_FOUND";
   case PlanValidationReason::cFREQUENCY_NOT_SUPPORTED: return "FREQUENCY_NOT_SUPPORTED";
   case PlanValidationReason::cPLATFORM_NOT_FOUND: return "PLATFORM_NOT_FOUND";
   case PlanValidationReason::cREFERENCE_NOT_FOUND: return "REFERENCE_NOT_FOUND";
   case PlanValidationReason::cALLOWED_NETWORK_NOT_ALLOCATED:
      return "ALLOWED_NETWORK_NOT_ALLOCATED";
   case PlanValidationReason::cBUSINESS_TYPE_NOT_SUPPORTED:
      return "BUSINESS_TYPE_NOT_SUPPORTED";
   case PlanValidationReason::cMEMBER_LIMIT_EXCEEDED: return "MEMBER_LIMIT_EXCEEDED";
   case PlanValidationReason::cDUPLICATE_MEMBER: return "DUPLICATE_MEMBER";
   case PlanValidationReason::cRESOURCE_CONFLICT: return "RESOURCE_CONFLICT";
   case PlanValidationReason::cCHANGE_CONFLICT: return "CHANGE_CONFLICT";
   case PlanValidationReason::cALREADY_MEMBER: return "ALREADY_MEMBER";
   case PlanValidationReason::cNOT_MEMBER: return "NOT_MEMBER";
   case PlanValidationReason::cCUSTOMER_RULE_UNAVAILABLE: return "CUSTOMER_RULE_UNAVAILABLE";
   case PlanValidationReason::cVALIDATION_FAILED: return "VALIDATION_FAILED";
   case PlanValidationReason::cEVALUATION_FAILED: return "EVALUATION_FAILED";
   case PlanValidationReason::cDATA_INVALID: return "DATA_INVALID";
   case PlanValidationReason::cNO_PATH: return "NO_PATH";
   case PlanValidationReason::cNODE_OFFLINE: return "NODE_OFFLINE";
   case PlanValidationReason::cBANDWIDTH_NOT_MET: return "BANDWIDTH_NOT_MET";
   case PlanValidationReason::cDELAY_NOT_MET: return "DELAY_NOT_MET";
   case PlanValidationReason::cPDR_NOT_MET: return "PDR_NOT_MET";
   case PlanValidationReason::cPLAN_NOT_VALIDATED: return "PLAN_NOT_VALIDATED";
   case PlanValidationReason::cPLAN_IDENTITY_MISMATCH: return "PLAN_IDENTITY_MISMATCH";
   case PlanValidationReason::cOUTPUT_PATH_INVALID: return "OUTPUT_PATH_INVALID";
   case PlanValidationReason::cPACKAGE_ALREADY_EXISTS: return "PACKAGE_ALREADY_EXISTS";
   case PlanValidationReason::cNO_CURRENT_PLAN: return "NO_CURRENT_PLAN";
   case PlanValidationReason::cADAPTER_UNAVAILABLE: return "ADAPTER_UNAVAILABLE";
   }
   return "PARSE_ERROR";
}

enum class PlanChangeType
{
   cJOIN,
   cLEAVE
};

inline const char* ToString(PlanChangeType aType)
{
   return aType == PlanChangeType::cJOIN ? "JOIN" : "LEAVE";
}

enum class PlanEvaluationStatus
{
   cNOT_EVALUATED,
   cPASS,
   cFAIL,
   cDATA_INVALID
};

inline const char* ToString(PlanEvaluationStatus aStatus)
{
   switch (aStatus)
   {
   case PlanEvaluationStatus::cNOT_EVALUATED: return "NOT_EVALUATED";
   case PlanEvaluationStatus::cPASS: return "PASS";
   case PlanEvaluationStatus::cFAIL: return "FAIL";
   case PlanEvaluationStatus::cDATA_INVALID: return "DATA_INVALID";
   }
   return "DATA_INVALID";
}

enum class PlanIssueSeverity
{
   cWARNING,
   cERROR
};

inline const char* ToString(PlanIssueSeverity aSeverity)
{
   return aSeverity == PlanIssueSeverity::cERROR ? "ERROR" : "WARNING";
}

struct NetworkPlanAllocation
{
   std::string allocationId;
   std::string networkName;
   NetworkType networkType = NetworkType::cUNKNOWN;
   std::string profileId;
   double frequencyHz = 0.0;
   std::string channelId;
   std::string subnetId;
   std::vector<std::string> slotIds;
   std::vector<std::string> memberPlatformIds;
   std::string routePolicyId;
   bool enabled = true;
};

struct NetworkPlanDemand
{
   std::string demandId;
   std::string businessType;
   std::string sourcePlatform;
   std::string destinationPlatform;
   std::uint64_t payloadBits = 0;
   double requiredBandwidthBps = 0.0;
   double maximumDelayMs = 0.0;
   double minimumPdrPercent = 0.0;
   std::vector<NetworkType> allowedNetworks;
};

struct NetworkPlanChange
{
   std::string changeId;
   PlanChangeType changeType = PlanChangeType::cJOIN;
   std::string allocationId;
   std::string platformId;
};

struct NetworkPlanDocument
{
   std::string schemaVersion = "nrm.network_plan.v1";
   std::string planId;
   std::uint64_t revision = 0;
   std::string configVersion;
   std::string providerId;
   std::string createdTime;
   NetworkPlanState state = NetworkPlanState::cDRAFT;
   std::vector<NetworkPlanAllocation> allocations;
   std::vector<NetworkPlanDemand> demands;
   std::vector<NetworkPlanChange> changes;
   DataOrigin source = DataOrigin::cCUSTOMER_MODULE;
   Confidence confidence = Confidence::cLOW;
   bool valid = false;
   std::string previousPlanId;
   std::uint64_t previousRevision = 0;
};

struct PlanValidationIssue
{
   PlanValidationReason reason = PlanValidationReason::cNONE;
   std::string field;
   std::string recordId;
   PlanIssueSeverity severity = PlanIssueSeverity::cERROR;
   std::string description;
};

struct PlanValidationResult
{
   std::string schemaVersion = "nrm.network_plan_validation.v1";
   std::string planId;
   std::uint64_t revision = 0;
   std::string planFingerprint;
   bool passed = false;
   std::vector<PlanValidationIssue> issues;
};

struct PlanRepositoryResult
{
   bool success = false;
   PlanValidationReason reason = PlanValidationReason::cNONE;
   std::string path;
   std::string planId;
   std::uint64_t revision = 0;
   std::string field;
};

struct PlanDemandEvaluation
{
   std::string demandId;
   PlanEvaluationStatus status = PlanEvaluationStatus::cNOT_EVALUATED;
   CapabilityResult capability;
   std::vector<PlanValidationReason> reasons;
};

struct NetworkPlanEvaluationResult
{
   std::string schemaVersion = "nrm.network_plan_evaluation.v1";
   std::string planId;
   std::uint64_t revision = 0;
   std::string planFingerprint;
   std::uint64_t snapshotVersion = 0;
   double simTime = 0.0;
   PlanEvaluationStatus overallStatus = PlanEvaluationStatus::cNOT_EVALUATED;
   NetworkPlanState resultingState = NetworkPlanState::cDRAFT;
   PlanValidationResult validation;
   std::vector<PlanDemandEvaluation> demands;
};

struct DistributionPackageResult
{
   std::string schemaVersion = "nrm.network_plan_distribution.v1";
   bool generated = false;
   std::string outputPath;
   std::string planId;
   std::uint64_t revision = 0;
   NetworkPlanState resultingState = NetworkPlanState::cVALIDATED;
   PlanValidationReason reason = PlanValidationReason::cPLAN_NOT_VALIDATED;
};

class NetworkPlanAdapter
{
public:
   virtual ~NetworkPlanAdapter() = default;

   virtual PlanRepositoryResult LoadExternal(
      const std::string& aPath,
      NetworkPlanDocument& aDocument) const = 0;
};
} // namespace nrm

#endif
