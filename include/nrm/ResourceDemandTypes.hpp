#ifndef NRM_RESOURCE_DEMAND_TYPES_HPP
#define NRM_RESOURCE_DEMAND_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "nrm/CommunicationCapabilityTypes.hpp"
#include "nrm/NetworkPlanTypes.hpp"
#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
enum class DemandMatchStatus
{
   cSATISFIED,
   cUNSATISFIED,
   cDATA_INVALID
};

inline const char* ToString(DemandMatchStatus aStatus)
{
   switch (aStatus)
   {
   case DemandMatchStatus::cSATISFIED: return "SATISFIED";
   case DemandMatchStatus::cUNSATISFIED: return "UNSATISFIED";
   case DemandMatchStatus::cDATA_INVALID: return "DATA_INVALID";
   }
   return "DATA_INVALID";
}

enum class RequirementItemType
{
   cPATH,
   cNETWORK_SIZE,
   cDISTANCE,
   cBANDWIDTH,
   cTRAFFIC,
   cDELAY,
   cPDR,
   cBUSINESS_TYPE
};

inline const char* ToString(RequirementItemType aType)
{
   switch (aType)
   {
   case RequirementItemType::cPATH: return "PATH";
   case RequirementItemType::cNETWORK_SIZE: return "NETWORK_SIZE";
   case RequirementItemType::cDISTANCE: return "DISTANCE";
   case RequirementItemType::cBANDWIDTH: return "BANDWIDTH";
   case RequirementItemType::cTRAFFIC: return "TRAFFIC";
   case RequirementItemType::cDELAY: return "DELAY";
   case RequirementItemType::cPDR: return "PDR";
   case RequirementItemType::cBUSINESS_TYPE: return "BUSINESS_TYPE";
   }
   return "PATH";
}

enum class RecommendationType
{
   cFREQUENCY,
   cSTATION,
   cCHANNEL,
   cSUBNET,
   cTIMESLOT,
   cROUTE
};

inline const char* ToString(RecommendationType aType)
{
   switch (aType)
   {
   case RecommendationType::cFREQUENCY: return "FREQUENCY";
   case RecommendationType::cSTATION: return "STATION";
   case RecommendationType::cCHANNEL: return "CHANNEL";
   case RecommendationType::cSUBNET: return "SUBNET";
   case RecommendationType::cTIMESLOT: return "TIMESLOT";
   case RecommendationType::cROUTE: return "ROUTE";
   }
   return "ROUTE";
}

enum class RecommendationStatus
{
   cAVAILABLE,
   cUNAVAILABLE
};

inline const char* ToString(RecommendationStatus aStatus)
{
   return aStatus == RecommendationStatus::cAVAILABLE ? "AVAILABLE" : "UNAVAILABLE";
}

enum class ResourceDemandReason
{
   cNONE,
   cFILE_OPEN_FAILED,
   cFILE_WRITE_FAILED,
   cATOMIC_RENAME_FAILED,
   cOUTPUT_PATH_INVALID,
   cFILE_ALREADY_EXISTS,
   cPARSE_ERROR,
   cUNSUPPORTED_SCHEMA,
   cUNKNOWN_RECORD_TYPE,
   cTRAILING_TOKEN,
   cMISSING_REQUIRED_FIELD,
   cINVALID_DEMAND_SET_ID,
   cINVALID_DEMAND_ID,
   cINVALID_REVISION,
   cREVISION_NOT_INCREMENTED,
   cDUPLICATE_DEMAND_REVISION,
   cDUPLICATE_DEMAND_ID,
   cDUPLICATE_NETWORK_TYPE,
   cREFERENCE_NOT_FOUND,
   cNON_FINITE_VALUE,
   cNEGATIVE_VALUE,
   cPDR_OUT_OF_RANGE,
   cUNKNOWN_NETWORK_TYPE,
   cSOURCE_EQUALS_DESTINATION,
   cSNAPSHOT_INVALID,
   cENDPOINT_NOT_FOUND,
   cPLAN_EVIDENCE_MISMATCH,
   cCAPABILITY_REQUEST_INVALID,
   cPATH_UNAVAILABLE,
   cMETRIC_UNAVAILABLE,
   cNETWORK_SIZE_NOT_MET,
   cDISTANCE_NOT_MET,
   cBANDWIDTH_NOT_MET,
   cTRAFFIC_NOT_MET,
   cDELAY_NOT_MET,
   cPDR_NOT_MET,
   cBUSINESS_TYPE_NOT_SUPPORTED,
   cPROFILE_CONFIG_INVALID,
   cCANDIDATE_DATA_UNAVAILABLE,
   cCUSTOMER_RULE_UNAVAILABLE,
   cNO_FEASIBLE_CANDIDATE,
   cCANDIDATE_CONFLICT,
   cMEMBER_LIMIT_EXCEEDED,
   cROUTE_UNAVAILABLE,
   cNO_CURRENT_DEMAND_SET,
   cINTERFERENCE_CONFLICT
};

inline const char* ToString(ResourceDemandReason aReason)
{
   switch (aReason)
   {
   case ResourceDemandReason::cNONE: return "NONE";
   case ResourceDemandReason::cFILE_OPEN_FAILED: return "FILE_OPEN_FAILED";
   case ResourceDemandReason::cFILE_WRITE_FAILED: return "FILE_WRITE_FAILED";
   case ResourceDemandReason::cATOMIC_RENAME_FAILED: return "ATOMIC_RENAME_FAILED";
   case ResourceDemandReason::cOUTPUT_PATH_INVALID: return "OUTPUT_PATH_INVALID";
   case ResourceDemandReason::cFILE_ALREADY_EXISTS: return "FILE_ALREADY_EXISTS";
   case ResourceDemandReason::cPARSE_ERROR: return "PARSE_ERROR";
   case ResourceDemandReason::cUNSUPPORTED_SCHEMA: return "UNSUPPORTED_SCHEMA";
   case ResourceDemandReason::cUNKNOWN_RECORD_TYPE: return "UNKNOWN_RECORD_TYPE";
   case ResourceDemandReason::cTRAILING_TOKEN: return "TRAILING_TOKEN";
   case ResourceDemandReason::cMISSING_REQUIRED_FIELD: return "MISSING_REQUIRED_FIELD";
   case ResourceDemandReason::cINVALID_DEMAND_SET_ID: return "INVALID_DEMAND_SET_ID";
   case ResourceDemandReason::cINVALID_DEMAND_ID: return "INVALID_DEMAND_ID";
   case ResourceDemandReason::cINVALID_REVISION: return "INVALID_REVISION";
   case ResourceDemandReason::cREVISION_NOT_INCREMENTED: return "REVISION_NOT_INCREMENTED";
   case ResourceDemandReason::cDUPLICATE_DEMAND_REVISION:
      return "DUPLICATE_DEMAND_REVISION";
   case ResourceDemandReason::cDUPLICATE_DEMAND_ID: return "DUPLICATE_DEMAND_ID";
   case ResourceDemandReason::cDUPLICATE_NETWORK_TYPE:
      return "DUPLICATE_NETWORK_TYPE";
   case ResourceDemandReason::cREFERENCE_NOT_FOUND: return "REFERENCE_NOT_FOUND";
   case ResourceDemandReason::cNON_FINITE_VALUE: return "NON_FINITE_VALUE";
   case ResourceDemandReason::cNEGATIVE_VALUE: return "NEGATIVE_VALUE";
   case ResourceDemandReason::cPDR_OUT_OF_RANGE: return "PDR_OUT_OF_RANGE";
   case ResourceDemandReason::cUNKNOWN_NETWORK_TYPE: return "UNKNOWN_NETWORK_TYPE";
   case ResourceDemandReason::cSOURCE_EQUALS_DESTINATION:
      return "SOURCE_EQUALS_DESTINATION";
   case ResourceDemandReason::cSNAPSHOT_INVALID: return "SNAPSHOT_INVALID";
   case ResourceDemandReason::cENDPOINT_NOT_FOUND: return "ENDPOINT_NOT_FOUND";
   case ResourceDemandReason::cPLAN_EVIDENCE_MISMATCH:
      return "PLAN_EVIDENCE_MISMATCH";
   case ResourceDemandReason::cCAPABILITY_REQUEST_INVALID:
      return "CAPABILITY_REQUEST_INVALID";
   case ResourceDemandReason::cPATH_UNAVAILABLE: return "PATH_UNAVAILABLE";
   case ResourceDemandReason::cMETRIC_UNAVAILABLE: return "METRIC_UNAVAILABLE";
   case ResourceDemandReason::cNETWORK_SIZE_NOT_MET: return "NETWORK_SIZE_NOT_MET";
   case ResourceDemandReason::cDISTANCE_NOT_MET: return "DISTANCE_NOT_MET";
   case ResourceDemandReason::cBANDWIDTH_NOT_MET: return "BANDWIDTH_NOT_MET";
   case ResourceDemandReason::cTRAFFIC_NOT_MET: return "TRAFFIC_NOT_MET";
   case ResourceDemandReason::cDELAY_NOT_MET: return "DELAY_NOT_MET";
   case ResourceDemandReason::cPDR_NOT_MET: return "PDR_NOT_MET";
   case ResourceDemandReason::cBUSINESS_TYPE_NOT_SUPPORTED:
      return "BUSINESS_TYPE_NOT_SUPPORTED";
   case ResourceDemandReason::cPROFILE_CONFIG_INVALID:
      return "PROFILE_CONFIG_INVALID";
   case ResourceDemandReason::cCANDIDATE_DATA_UNAVAILABLE:
      return "CANDIDATE_DATA_UNAVAILABLE";
   case ResourceDemandReason::cCUSTOMER_RULE_UNAVAILABLE:
      return "CUSTOMER_RULE_UNAVAILABLE";
   case ResourceDemandReason::cNO_FEASIBLE_CANDIDATE:
      return "NO_FEASIBLE_CANDIDATE";
   case ResourceDemandReason::cCANDIDATE_CONFLICT: return "CANDIDATE_CONFLICT";
   case ResourceDemandReason::cMEMBER_LIMIT_EXCEEDED:
      return "MEMBER_LIMIT_EXCEEDED";
   case ResourceDemandReason::cROUTE_UNAVAILABLE: return "ROUTE_UNAVAILABLE";
   case ResourceDemandReason::cNO_CURRENT_DEMAND_SET: return "NO_CURRENT_DEMAND_SET";
   case ResourceDemandReason::cINTERFERENCE_CONFLICT: return "INTERFERENCE_CONFLICT";
   }
   return "PARSE_ERROR";
}

struct ResourceDemand
{
   std::string schemaVersion = "nrm.resource_demand.v1";
   std::string demandId;
   std::string demandSetId;
   std::uint64_t revision = 0;
   std::string missionStage;
   std::string businessType;
   std::string sourcePlatform;
   std::string destinationPlatform;
   std::uint64_t payloadBits = 0;
   double businessTrafficBps = 0.0;
   double requiredBandwidthBps = 0.0;
   double maximumDelayMs = 0.0;
   double minimumPdrPercent = 0.0;
   double maximumDistanceM = 0.0;
   std::size_t minimumNetworkSize = 0;
   std::vector<NetworkType> allowedNetworks;
   DataOrigin source = DataOrigin::cCUSTOMER_MODULE;
   Confidence confidence = Confidence::cLOW;
   bool valid = false;
};

struct ResourceDemandSet
{
   std::string schemaVersion = "nrm.resource_demand_set.v1";
   std::string demandSetId;
   std::uint64_t revision = 0;
   std::string configVersion;
   std::string providerId;
   std::string createdTime;
   std::string requestSource;
   std::string correlationId;
   DataOrigin source = DataOrigin::cCUSTOMER_MODULE;
   Confidence confidence = Confidence::cLOW;
   bool valid = false;
   std::string previousDemandSetId;
   std::uint64_t previousRevision = 0;
   std::vector<ResourceDemand> demands;
};

struct ResourceDemandRepositoryResult
{
   bool success = false;
   ResourceDemandReason reason = ResourceDemandReason::cNONE;
   std::string path;
   std::string demandSetId;
   std::uint64_t revision = 0;
   std::string field;
};

struct RequirementCheck
{
   RequirementItemType type = RequirementItemType::cPATH;
   bool applicable = false;
   bool passed = false;
   MetricValue<double> requiredValue;
   MetricValue<double> currentValue;
   MetricValue<double> margin;
   std::string requiredText;
   std::string currentText;
   ResourceDemandReason reason = ResourceDemandReason::cNONE;
};

struct PlanningResourceCandidate
{
   RecommendationType type = RecommendationType::cFREQUENCY;
   std::string candidateId;
   std::string value;
   std::string allocationId;
   std::string platformId;
   double frequencyHz = 0.0;
   double interferenceCenterHz = 0.0;
   double interferenceBandwidthHz = 0.0;
   double protectionBandwidthHz = 0.0;
   bool occupied = false;
   bool pathAvailable = false;
   DemandMatchStatus projectedStatus = DemandMatchStatus::cDATA_INVALID;
   MetricValue<double> minimumMargin;
   std::vector<std::string> supportedBusinessTypes;
   std::size_t currentMembers = 0;
   std::size_t maximumMembers = 0;
   DataOrigin source = DataOrigin::cCUSTOMER_MODULE;
   Confidence confidence = Confidence::cLOW;
   bool valid = false;
};

struct PlanningCandidateSet
{
   std::string candidateSetId;
   std::vector<PlanningResourceCandidate> candidates;
};

struct PlanningRecommendation
{
   RecommendationType type = RecommendationType::cFREQUENCY;
   RecommendationStatus status = RecommendationStatus::cUNAVAILABLE;
   std::string demandId;
   std::string candidateId;
   std::string value;
   std::size_t rank = 0;
   std::uint64_t snapshotVersion = 0;
   std::string planId;
   std::uint64_t planRevision = 0;
   std::string planFingerprint;
   std::vector<std::string> evidence;
   ResourceDemandReason reason = ResourceDemandReason::cCANDIDATE_DATA_UNAVAILABLE;
   DataOrigin source = DataOrigin::cCUSTOMER_MODULE;
   Confidence confidence = Confidence::cLOW;
};

struct ResourceDemandMatchResult
{
   std::string schemaVersion = "nrm.resource_demand_match.v1";
   std::string demandId;
   std::string demandSetId;
   std::uint64_t demandSetRevision = 0;
   std::uint64_t snapshotVersion = 0;
   std::string planId;
   std::uint64_t planRevision = 0;
   std::string planFingerprint;
   DemandMatchStatus status = DemandMatchStatus::cDATA_INVALID;
   CapabilityResult capability;
   std::vector<RequirementCheck> checks;
   std::vector<PlanningRecommendation> recommendations;
   std::vector<ResourceDemandReason> reasons;
};

struct ResourceDemandBatchResult
{
   std::string schemaVersion = "nrm.resource_demand_batch.v1";
   std::string demandSetId;
   std::uint64_t revision = 0;
   std::uint64_t snapshotVersion = 0;
   std::size_t totalCount = 0;
   std::size_t satisfiedCount = 0;
   std::size_t unsatisfiedCount = 0;
   std::size_t dataInvalidCount = 0;
   std::vector<ResourceDemandMatchResult> results;
};

struct ResourceDemandFeedback
{
   std::string schemaVersion = "nrm.resource_demand_feedback.v1";
   std::string requestSource;
   std::string correlationId;
   std::vector<std::string> classifications;
   ResourceDemandBatchResult batch;
   double historicalPassRatioPercent = 0.0;
   std::size_t historySampleCount = 0;
};
} // namespace nrm

#endif
