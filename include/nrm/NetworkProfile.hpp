#ifndef NRM_NETWORK_PROFILE_HPP
#define NRM_NETWORK_PROFILE_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
enum class NetworkProfileValidationReason
{
   cNONE,
   cFILE_OPEN_FAILED,
   cPARSE_ERROR,
   cMISSING_SCHEMA_VERSION,
   cUNSUPPORTED_SCHEMA_VERSION,
   cMISSING_CONFIG_VERSION,
   cMISSING_PROFILE_ID,
   cUNKNOWN_NETWORK_TYPE,
   cNEGATIVE_VALUE,
   cPDR_OUT_OF_RANGE,
   cDUPLICATE_PROFILE_ID,
   cMISSING_REQUIRED_FIELD
};

inline const char* ToString(NetworkProfileValidationReason aReason)
{
   switch (aReason)
   {
   case NetworkProfileValidationReason::cNONE: return "NONE";
   case NetworkProfileValidationReason::cFILE_OPEN_FAILED: return "FILE_OPEN_FAILED";
   case NetworkProfileValidationReason::cPARSE_ERROR: return "PARSE_ERROR";
   case NetworkProfileValidationReason::cMISSING_SCHEMA_VERSION: return "MISSING_SCHEMA_VERSION";
   case NetworkProfileValidationReason::cUNSUPPORTED_SCHEMA_VERSION: return "UNSUPPORTED_SCHEMA_VERSION";
   case NetworkProfileValidationReason::cMISSING_CONFIG_VERSION: return "MISSING_CONFIG_VERSION";
   case NetworkProfileValidationReason::cMISSING_PROFILE_ID: return "MISSING_PROFILE_ID";
   case NetworkProfileValidationReason::cUNKNOWN_NETWORK_TYPE: return "UNKNOWN_NETWORK_TYPE";
   case NetworkProfileValidationReason::cNEGATIVE_VALUE: return "NEGATIVE_VALUE";
   case NetworkProfileValidationReason::cPDR_OUT_OF_RANGE: return "PDR_OUT_OF_RANGE";
   case NetworkProfileValidationReason::cDUPLICATE_PROFILE_ID: return "DUPLICATE_PROFILE_ID";
   case NetworkProfileValidationReason::cMISSING_REQUIRED_FIELD: return "MISSING_REQUIRED_FIELD";
   }
   return "PARSE_ERROR";
}

struct NetworkProfileValidation
{
   bool valid = false;
   NetworkProfileValidationReason reason = NetworkProfileValidationReason::cMISSING_REQUIRED_FIELD;
   std::string field;
   std::string profileId;
};

struct NetworkProfile
{
   std::string profileId;
   std::string schemaVersion;
   std::string configVersion;
   NetworkType networkType = NetworkType::cUNKNOWN;
   std::string protocolModel;
   std::vector<double> frequenciesHz;
   double maximumRangeM = -1.0;
   double establishmentDelayMs = -1.0;
   double candidatePdrPercent = -1.0;
   double serviceCapacityBps = -1.0;
   std::string propagationModelId;
   std::size_t maximumMembers = 0;
   std::vector<std::string> supportedBusinessTypes;
   std::string providerId;
   DataOrigin source = DataOrigin::cPARAMETERIZED_MODEL;
   Confidence confidence = Confidence::cLOW;
   bool valid = false;
};
} // namespace nrm

#endif
