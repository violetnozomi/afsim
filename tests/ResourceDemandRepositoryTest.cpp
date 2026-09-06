#include "nrm/ResourceDemandRepository.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>

#include <sys/stat.h>
#include <unistd.h>

namespace
{
void WriteText(const std::string& aPath, const std::string& aText)
{
   std::ofstream output(aPath, std::ios::out | std::ios::trunc);
   assert(output);
   output << aText;
   assert(output.good());
}

std::string ReadAll(const std::string& aPath)
{
   std::ifstream input(aPath);
   std::ostringstream output;
   output << input.rdbuf();
   return output.str();
}

nrm::ResourceDemandSet ValidSet(std::uint64_t aRevision = 1)
{
   nrm::ResourceDemandSet set;
   set.demandSetId = "demand-set-alpha";
   set.revision = aRevision;
   set.configVersion = "demo-0.7.0";
   set.providerId = "internal-test";
   set.createdTime = "2026-08-01T20:00:00Z";
   set.source = nrm::DataOrigin::cCUSTOMER_MODULE;
   set.confidence = nrm::Confidence::cLOW;
   set.valid = true;

   nrm::ResourceDemand demand;
   demand.demandId = "demand-1";
   demand.demandSetId = set.demandSetId;
   demand.revision = set.revision;
   demand.missionStage = "INGRESS";
   demand.businessType = "C2";
   demand.sourcePlatform = "source";
   demand.destinationPlatform = "destination";
   demand.payloadBits = 1024;
   demand.businessTrafficBps = 500.0;
   demand.requiredBandwidthBps = 400.0;
   demand.maximumDelayMs = 100.0;
   demand.minimumPdrPercent = 90.0;
   demand.maximumDistanceM = 5000.0;
   demand.minimumNetworkSize = 2;
   demand.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   demand.source = set.source;
   demand.confidence = set.confidence;
   demand.valid = true;
   set.demands.push_back(demand);
   return set;
}

bool Equal(const nrm::ResourceDemandSet& aLeft,
           const nrm::ResourceDemandSet& aRight)
{
   if (aLeft.schemaVersion != aRight.schemaVersion ||
       aLeft.demandSetId != aRight.demandSetId ||
       aLeft.revision != aRight.revision ||
       aLeft.configVersion != aRight.configVersion ||
       aLeft.providerId != aRight.providerId ||
       aLeft.createdTime != aRight.createdTime ||
       aLeft.source != aRight.source ||
       aLeft.confidence != aRight.confidence ||
       aLeft.valid != aRight.valid ||
       aLeft.previousDemandSetId != aRight.previousDemandSetId ||
       aLeft.previousRevision != aRight.previousRevision ||
       aLeft.demands.size() != aRight.demands.size())
      return false;
   const nrm::ResourceDemand& left = aLeft.demands.front();
   const nrm::ResourceDemand& right = aRight.demands.front();
   return left.schemaVersion == right.schemaVersion &&
          left.demandId == right.demandId &&
          left.demandSetId == right.demandSetId &&
          left.revision == right.revision &&
          left.missionStage == right.missionStage &&
          left.businessType == right.businessType &&
          left.sourcePlatform == right.sourcePlatform &&
          left.destinationPlatform == right.destinationPlatform &&
          left.payloadBits == right.payloadBits &&
          left.businessTrafficBps == right.businessTrafficBps &&
          left.requiredBandwidthBps == right.requiredBandwidthBps &&
          left.maximumDelayMs == right.maximumDelayMs &&
          left.minimumPdrPercent == right.minimumPdrPercent &&
          left.maximumDistanceM == right.maximumDistanceM &&
          left.minimumNetworkSize == right.minimumNetworkSize &&
          left.allowedNetworks == right.allowedNetworks &&
          left.source == right.source &&
          left.confidence == right.confidence &&
          left.valid == right.valid;
}
} // namespace

int main()
{
   const std::string root = "/tmp/nrm-demand-repository-" +
      std::to_string(static_cast<long long>(getpid()));
   assert(mkdir(root.c_str(), 0700) == 0);
   const std::string inputPath = root + "/demand-r1.nrm";
   const std::string roundTripPath = root + "/demand-r1-copy.nrm";

   const nrm::ResourceDemandSet original = ValidSet();
   assert(nrm::ResourceDemandRepository::SaveDocumentAtomic(
      original, inputPath, true).success);

   nrm::ResourceDemandRepository repository;
   assert(repository.LoadFromFile(inputPath));
   assert(Equal(original, *repository.GetCurrentDemandSet()));
   assert(repository.SaveRevision(roundTripPath));
   nrm::ResourceDemandRepository roundTrip;
   assert(roundTrip.LoadFromFile(roundTripPath));
   assert(Equal(original, *roundTrip.GetCurrentDemandSet()));

   assert(!repository.LoadFromFile(inputPath));
   assert(repository.LastLoadResult().reason ==
          nrm::ResourceDemandReason::cDUPLICATE_DEMAND_REVISION);
   repository.Unload();
   assert(!repository.HasCurrentDemandSet());
   assert(repository.LoadFromFile(inputPath));

   const nrm::ResourceDemandSet beforeFailure = *repository.GetCurrentDemandSet();
   assert(!repository.LoadFromFile(root + "/missing.nrm"));
   assert(repository.LastLoadResult().reason ==
          nrm::ResourceDemandReason::cFILE_OPEN_FAILED);
   assert(Equal(beforeFailure, *repository.GetCurrentDemandSet()));

   nrm::ResourceDemandSet unchanged = ValidSet();
   assert(!repository.ReplaceDraft(unchanged));
   assert(repository.LastLoadResult().reason ==
          nrm::ResourceDemandReason::cREVISION_NOT_INCREMENTED);
   nrm::ResourceDemandSet revision2 = ValidSet(2);
   revision2.createdTime = "2026-08-01T20:01:00Z";
   assert(repository.ReplaceDraft(revision2));
   assert(repository.GetCurrentDemandSet()->previousDemandSetId ==
          original.demandSetId);
   assert(repository.GetCurrentDemandSet()->previousRevision == 1);

   const std::string preserved = ReadAll(roundTripPath);
   assert(!repository.SaveRevision(roundTripPath));
   assert(repository.LastSaveResult().reason ==
          nrm::ResourceDemandReason::cFILE_ALREADY_EXISTS);
   assert(ReadAll(roundTripPath) == preserved);

   const std::string badMagic = root + "/bad-magic.nrm";
   WriteText(badMagic, "WRONG_MAGIC \"set\" 1\n");
   assert(!repository.LoadFromFile(badMagic));
   assert(repository.LastLoadResult().reason ==
          nrm::ResourceDemandReason::cUNSUPPORTED_SCHEMA);

   const std::string unknown = root + "/unknown.nrm";
   WriteText(unknown,
      "NRM_RESOURCE_DEMAND_V1 \"set-unknown\" 1 \"demo-0.7.0\" "
      "\"provider\" \"2026-08-01T20:00:00Z\" CUSTOMER_MODULE LOW 1 \"\" 0\n"
      "UNKNOWN \"value\"\n");
   assert(!repository.LoadFromFile(unknown));
   assert(repository.LastLoadResult().reason ==
          nrm::ResourceDemandReason::cUNKNOWN_RECORD_TYPE);

   const std::string trailing = root + "/trailing.nrm";
   WriteText(trailing,
      "NRM_RESOURCE_DEMAND_V1 \"set-trailing\" 1 \"demo-0.7.0\" "
      "\"provider\" \"2026-08-01T20:00:00Z\" CUSTOMER_MODULE LOW 1 \"\" 0 extra\n");
   assert(!repository.LoadFromFile(trailing));
   assert(repository.LastLoadResult().reason ==
          nrm::ResourceDemandReason::cTRAILING_TOKEN);

   const std::string invalidNumeric = root + "/invalid-numeric.nrm";
   WriteText(invalidNumeric,
      "NRM_RESOURCE_DEMAND_V1 \"set-numeric\" 1 \"demo-0.7.0\" "
      "\"provider\" \"2026-08-01T20:00:00Z\" CUSTOMER_MODULE LOW 1 \"\" 0\n"
      "DEMAND \"d1\" \"INGRESS\" \"C2\" \"source\" \"destination\" "
      "1 nan 0 0 0 0 0\n");
   assert(!repository.LoadFromFile(invalidNumeric));

   nrm::ResourceDemandSet invalid = ValidSet(3);
   invalid.demands[0].businessTrafficBps =
      std::numeric_limits<double>::infinity();
   assert(!repository.ReplaceDraft(invalid));
   assert(repository.LastLoadResult().reason ==
          nrm::ResourceDemandReason::cNON_FINITE_VALUE);
   invalid = ValidSet(3);
   invalid.demands[0].requiredBandwidthBps = -1.0;
   assert(!repository.ReplaceDraft(invalid));
   assert(repository.LastLoadResult().reason ==
          nrm::ResourceDemandReason::cNEGATIVE_VALUE);
   invalid = ValidSet(3);
   invalid.demands[0].minimumPdrPercent = 100.1;
   assert(!repository.ReplaceDraft(invalid));
   assert(repository.LastLoadResult().reason ==
          nrm::ResourceDemandReason::cPDR_OUT_OF_RANGE);
   invalid = ValidSet(3);
   invalid.demands.push_back(invalid.demands.front());
   assert(!repository.ReplaceDraft(invalid));
   assert(repository.LastLoadResult().reason ==
          nrm::ResourceDemandReason::cDUPLICATE_DEMAND_ID);

   nrm::ResourceDemandRepository acceptanceRepository;
   const std::string acceptancePath =
      std::string(NRM_SOURCE_DIR) +
      "/data/resource_demands/contract_acceptance_37node-r1.demand";
   assert(acceptanceRepository.LoadFromFile(acceptancePath));
   const nrm::ResourceDemandSet& acceptanceSet =
      *acceptanceRepository.GetCurrentDemandSet();
   assert(acceptanceSet.demandSetId == "contract-acceptance-37node-demands");
   assert(acceptanceSet.demands.size() == 12);
   bool hasLink11 = false;
   bool hasLink16 = false;
   bool hasSatcom = false;
   bool hasCdl = false;
   for (const nrm::ResourceDemand& demand : acceptanceSet.demands)
   {
      for (nrm::NetworkType network : demand.allowedNetworks)
      {
         hasLink11 = hasLink11 || network == nrm::NetworkType::cLINK11;
         hasLink16 = hasLink16 || network == nrm::NetworkType::cLINK16;
         hasSatcom = hasSatcom || network == nrm::NetworkType::cSATCOM;
         hasCdl = hasCdl || network == nrm::NetworkType::cCDL;
      }
   }
   assert(hasLink11 && hasLink16 && hasSatcom && hasCdl);

   const std::string files[] = {
      inputPath, roundTripPath, badMagic, unknown, trailing, invalidNumeric};
   for (const std::string& path : files) std::remove(path.c_str());
   assert(rmdir(root.c_str()) == 0);
   return 0;
}
