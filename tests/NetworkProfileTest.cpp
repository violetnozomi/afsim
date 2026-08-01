#include "nrm/NetworkProfileRepository.hpp"

#include <iostream>
#include <limits>
#include <vector>

#define CHECK(condition) \
   do { if (!(condition)) { std::cerr << "CHECK failed at line " << __LINE__ << "\n"; return 1; } } while (false)

int main()
{
   nrm::NetworkProfileRepository builtIn =
      nrm::NetworkProfileRepository::BuiltInDemo();
   CHECK(builtIn.Valid());
   CHECK(builtIn.ConfigVersion() == "demo-0.7.0");
   CHECK(builtIn.Profiles().size() == 4);
   const nrm::NetworkProfile* link16 =
      builtIn.Find(nrm::NetworkType::cLINK16);
   CHECK(link16 != nullptr);
   CHECK(link16->maximumRangeM == 500000.0);
   CHECK(link16->establishmentDelayMs == 20.0);
   CHECK(link16->candidatePdrPercent == 95.0);
   CHECK(link16->serviceCapacityBps == 238000.0);
   CHECK(link16->source == nrm::DataOrigin::cPARAMETERIZED_MODEL);
   CHECK(link16->confidence == nrm::Confidence::cLOW);

   nrm::NetworkProfileRepository external;
   nrm::NetworkProfileValidation validation;
   CHECK(external.LoadFromFile(
      std::string(NRM_SOURCE_DIR) + "/config/network_profiles.nrm", validation));
   CHECK(validation.valid);
   CHECK(external.Find(nrm::NetworkType::cCDL)->serviceCapacityBps == 45000000.0);

   std::vector<nrm::NetworkProfile> profiles = builtIn.Profiles();
   profiles[0].candidatePdrPercent = 101.0;
   nrm::NetworkProfileRepository invalid;
   CHECK(!invalid.Replace("bad-v1", "test", profiles, validation));
   CHECK(validation.reason == nrm::NetworkProfileValidationReason::cPDR_OUT_OF_RANGE);
   CHECK(!invalid.Valid());
   CHECK(invalid.Find(nrm::NetworkType::cLINK11) == nullptr);

   profiles = builtIn.Profiles();
   profiles[1].profileId = profiles[0].profileId;
   CHECK(!invalid.Replace("bad-v2", "test", profiles, validation));
   CHECK(validation.reason == nrm::NetworkProfileValidationReason::cDUPLICATE_PROFILE_ID);

   profiles = builtIn.Profiles();
   profiles[0].maximumRangeM = -1.0;
   CHECK(!invalid.Replace("bad-v3", "test", profiles, validation));
   CHECK(validation.reason == nrm::NetworkProfileValidationReason::cNEGATIVE_VALUE);

   profiles = builtIn.Profiles();
   profiles[0].serviceCapacityBps = std::numeric_limits<double>::quiet_NaN();
   CHECK(!invalid.Replace("bad-v4", "test", profiles, validation));
   CHECK(validation.reason == nrm::NetworkProfileValidationReason::cNEGATIVE_VALUE);

   CHECK(!invalid.LoadFromFile("/path/that/does/not/exist", validation));
   CHECK(validation.reason == nrm::NetworkProfileValidationReason::cFILE_OPEN_FAILED);
   return 0;
}
