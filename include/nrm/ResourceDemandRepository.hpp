#ifndef NRM_RESOURCE_DEMAND_REPOSITORY_HPP
#define NRM_RESOURCE_DEMAND_REPOSITORY_HPP

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <sys/stat.h>

#include "nrm/NetworkTypeUtils.hpp"
#include "nrm/ResourceDemandTypes.hpp"

namespace nrm
{
namespace resource_demand_detail
{
inline bool HasTrailingToken(std::istringstream& aInput)
{
   aInput >> std::ws;
   return !aInput.eof();
}

inline bool ReadUnsigned(std::istringstream& aInput, std::uint64_t& aValue)
{
   std::string token;
   if (!(aInput >> token) || token.empty()) return false;
   std::uint64_t value = 0;
   for (char character : token)
   {
      if (character < '0' || character > '9') return false;
      const std::uint64_t digit = static_cast<std::uint64_t>(character - '0');
      if (value > (std::numeric_limits<std::uint64_t>::max() - digit) / 10)
         return false;
      value = value * 10 + digit;
   }
   aValue = value;
   return true;
}

inline bool ReadBool(std::istringstream& aInput, bool& aValue)
{
   std::string token;
   if (!(aInput >> token) || (token != "0" && token != "1")) return false;
   aValue = token == "1";
   return true;
}

inline bool ParseNetworkType(const std::string& aToken, NetworkType& aType)
{
   if (aToken == "LINK11") aType = NetworkType::cLINK11;
   else if (aToken == "LINK16") aType = NetworkType::cLINK16;
   else if (aToken == "SATCOM") aType = NetworkType::cSATCOM;
   else if (aToken == "CDL") aType = NetworkType::cCDL;
   else return false;
   return true;
}

inline bool ParseOrigin(const std::string& aToken, DataOrigin& aOrigin)
{
   if (aToken == "AFSIM_INTERNAL") aOrigin = DataOrigin::cAFSIM_INTERNAL;
   else if (aToken == "CUSTOMER_MODULE") aOrigin = DataOrigin::cCUSTOMER_MODULE;
   else if (aToken == "REPLAY") aOrigin = DataOrigin::cREPLAY;
   else if (aToken == "PARAMETERIZED_MODEL")
      aOrigin = DataOrigin::cPARAMETERIZED_MODEL;
   else if (aToken == "ESTIMATED") aOrigin = DataOrigin::cESTIMATED;
   else if (aToken == "DERIVED") aOrigin = DataOrigin::cDERIVED;
   else return false;
   return true;
}

inline bool ParseConfidence(const std::string& aToken, Confidence& aConfidence)
{
   if (aToken == "LOW") aConfidence = Confidence::cLOW;
   else if (aToken == "MEDIUM") aConfidence = Confidence::cMEDIUM;
   else if (aToken == "HIGH") aConfidence = Confidence::cHIGH;
   else return false;
   return true;
}

inline bool IsSafeId(const std::string& aValue)
{
   if (aValue.empty()) return false;
   for (unsigned char character : aValue)
   {
      const bool allowed = (character >= 'a' && character <= 'z') ||
                           (character >= 'A' && character <= 'Z') ||
                           (character >= '0' && character <= '9') ||
                           character == '-' || character == '_' || character == '.';
      if (!allowed) return false;
   }
   return aValue != "." && aValue != "..";
}

inline bool FileExists(const std::string& aPath)
{
   struct stat info;
   return !aPath.empty() && stat(aPath.c_str(), &info) == 0;
}

inline ResourceDemandRepositoryResult Failure(
   ResourceDemandReason aReason,
   const std::string& aPath,
   const std::string& aField,
   const ResourceDemandSet* aSetPtr = nullptr)
{
   ResourceDemandRepositoryResult result;
   result.reason = aReason;
   result.path = aPath;
   result.field = aField;
   if (aSetPtr != nullptr)
   {
      result.demandSetId = aSetPtr->demandSetId;
      result.revision = aSetPtr->revision;
   }
   return result;
}

inline ResourceDemandRepositoryResult Success(const std::string& aPath,
                                               const ResourceDemandSet& aSet)
{
   ResourceDemandRepositoryResult result;
   result.success = true;
   result.reason = ResourceDemandReason::cNONE;
   result.path = aPath;
   result.demandSetId = aSet.demandSetId;
   result.revision = aSet.revision;
   return result;
}

inline ResourceDemand* FindDemand(ResourceDemandSet& aSet,
                                  const std::string& aDemandId)
{
   for (ResourceDemand& demand : aSet.demands)
   {
      if (demand.demandId == aDemandId) return &demand;
   }
   return nullptr;
}

inline ResourceDemandRepositoryResult ValidateDocument(
   const ResourceDemandSet& aSet,
   const std::string& aPath)
{
   if (aSet.schemaVersion != "nrm.resource_demand_set.v1")
      return Failure(ResourceDemandReason::cUNSUPPORTED_SCHEMA,
                     aPath, "schemaVersion", &aSet);
   if (!IsSafeId(aSet.demandSetId))
      return Failure(ResourceDemandReason::cINVALID_DEMAND_SET_ID,
                     aPath, "demandSetId", &aSet);
   if (aSet.revision == 0)
      return Failure(ResourceDemandReason::cINVALID_REVISION,
                     aPath, "revision", &aSet);
   if (aSet.configVersion.empty() || aSet.providerId.empty() ||
       aSet.createdTime.empty())
      return Failure(ResourceDemandReason::cMISSING_REQUIRED_FIELD,
                     aPath, "header", &aSet);
   if (aSet.previousDemandSetId.empty() != (aSet.previousRevision == 0))
      return Failure(ResourceDemandReason::cREFERENCE_NOT_FOUND,
                     aPath, "previousRevision", &aSet);
   if (aSet.demands.empty())
      return Failure(ResourceDemandReason::cMISSING_REQUIRED_FIELD,
                     aPath, "demands", &aSet);

   std::set<std::string> demandIds;
   for (const ResourceDemand& demand : aSet.demands)
   {
      if (demand.schemaVersion != "nrm.resource_demand.v1")
         return Failure(ResourceDemandReason::cUNSUPPORTED_SCHEMA,
                        aPath, "demand.schemaVersion", &aSet);
      if (!IsSafeId(demand.demandId))
         return Failure(ResourceDemandReason::cINVALID_DEMAND_ID,
                        aPath, "demandId", &aSet);
      if (!demandIds.insert(demand.demandId).second)
         return Failure(ResourceDemandReason::cDUPLICATE_DEMAND_ID,
                        aPath, "demandId", &aSet);
      if (demand.demandSetId != aSet.demandSetId ||
          demand.revision != aSet.revision)
         return Failure(ResourceDemandReason::cREFERENCE_NOT_FOUND,
                        aPath, "demandSetIdentity", &aSet);
      if (demand.missionStage.empty() || demand.businessType.empty() ||
          demand.sourcePlatform.empty() || demand.destinationPlatform.empty())
         return Failure(ResourceDemandReason::cMISSING_REQUIRED_FIELD,
                        aPath, "demand", &aSet);
      if (demand.sourcePlatform == demand.destinationPlatform)
         return Failure(ResourceDemandReason::cSOURCE_EQUALS_DESTINATION,
                        aPath, "sourcePlatform", &aSet);
      if (!std::isfinite(demand.businessTrafficBps) ||
          !std::isfinite(demand.requiredBandwidthBps) ||
          !std::isfinite(demand.maximumDelayMs) ||
          !std::isfinite(demand.minimumPdrPercent) ||
          !std::isfinite(demand.maximumDistanceM))
         return Failure(ResourceDemandReason::cNON_FINITE_VALUE,
                        aPath, "demandNumeric", &aSet);
      if (demand.businessTrafficBps < 0.0 ||
          demand.requiredBandwidthBps < 0.0 ||
          demand.maximumDelayMs < 0.0 ||
          demand.maximumDistanceM < 0.0)
         return Failure(ResourceDemandReason::cNEGATIVE_VALUE,
                        aPath, "demandNumeric", &aSet);
      if (demand.minimumPdrPercent < 0.0 ||
          demand.minimumPdrPercent > 100.0)
         return Failure(ResourceDemandReason::cPDR_OUT_OF_RANGE,
                        aPath, "minimumPdrPercent", &aSet);
      std::set<NetworkType> networkTypes;
      for (NetworkType networkType : demand.allowedNetworks)
      {
         if (networkType == NetworkType::cUNKNOWN)
            return Failure(ResourceDemandReason::cUNKNOWN_NETWORK_TYPE,
                           aPath, "allowedNetworks", &aSet);
         if (!networkTypes.insert(networkType).second)
            return Failure(ResourceDemandReason::cDUPLICATE_NETWORK_TYPE,
                           aPath, "allowedNetworks", &aSet);
      }
   }
   return Success(aPath, aSet);
}

inline ResourceDemandSet Normalize(const ResourceDemandSet& aSet)
{
   ResourceDemandSet normalized = aSet;
   normalized.schemaVersion = "nrm.resource_demand_set.v1";
   for (ResourceDemand& demand : normalized.demands)
   {
      demand.schemaVersion = "nrm.resource_demand.v1";
      demand.demandSetId = normalized.demandSetId;
      demand.revision = normalized.revision;
      demand.source = normalized.source;
      demand.confidence = normalized.confidence;
      demand.valid = normalized.valid;
   }
   return normalized;
}

inline void WriteDocument(std::ostream& aOutput, const ResourceDemandSet& aSet)
{
   aOutput << std::setprecision(std::numeric_limits<double>::max_digits10);
   aOutput << "NRM_RESOURCE_DEMAND_V1 " << std::quoted(aSet.demandSetId) << ' '
           << aSet.revision << ' ' << std::quoted(aSet.configVersion) << ' '
           << std::quoted(aSet.providerId) << ' ' << std::quoted(aSet.createdTime)
           << ' ' << ToString(aSet.source) << ' ' << ToString(aSet.confidence) << ' '
           << (aSet.valid ? 1 : 0) << ' ' << std::quoted(aSet.previousDemandSetId)
           << ' ' << aSet.previousRevision << '\n';
   for (const ResourceDemand& demand : aSet.demands)
   {
      aOutput << "DEMAND " << std::quoted(demand.demandId) << ' '
              << std::quoted(demand.missionStage) << ' '
              << std::quoted(demand.businessType) << ' '
              << std::quoted(demand.sourcePlatform) << ' '
              << std::quoted(demand.destinationPlatform) << ' '
              << demand.payloadBits << ' ' << demand.businessTrafficBps << ' '
              << demand.requiredBandwidthBps << ' ' << demand.maximumDelayMs << ' '
              << demand.minimumPdrPercent << ' ' << demand.maximumDistanceM << ' '
              << demand.minimumNetworkSize << '\n';
      for (NetworkType networkType : demand.allowedNetworks)
         aOutput << "DEMAND_NETWORK " << std::quoted(demand.demandId) << ' '
                 << ToString(networkType) << '\n';
   }
}
} // namespace resource_demand_detail

class ResourceDemandRepository
{
public:
   bool LoadFromFile(const std::string& aPath)
   {
      ResourceDemandSet parsed;
      ResourceDemandRepositoryResult result = ParseFile(aPath, parsed);
      if (!result.success)
      {
         mLastLoad = result;
         return false;
      }
      if (mHasCurrentSet && mCurrentSet.demandSetId == parsed.demandSetId &&
          mCurrentSet.revision == parsed.revision)
      {
         mLastLoad = resource_demand_detail::Failure(
            ResourceDemandReason::cDUPLICATE_DEMAND_REVISION,
            aPath, "revision", &parsed);
         return false;
      }
      mCurrentSet = parsed;
      mHasCurrentSet = true;
      mSeenRevisions.insert(RevisionKey(parsed.demandSetId, parsed.revision));
      mLastLoad = result;
      return true;
   }

   bool ReplaceDraft(const ResourceDemandSet& aSet)
   {
      ResourceDemandSet draft = resource_demand_detail::Normalize(aSet);
      const ResourceDemandRepositoryResult validation =
         resource_demand_detail::ValidateDocument(draft, std::string());
      if (!validation.success)
      {
         mLastLoad = validation;
         return false;
      }
      if (mHasCurrentSet && draft.demandSetId == mCurrentSet.demandSetId &&
          draft.revision <= mCurrentSet.revision)
      {
         mLastLoad = resource_demand_detail::Failure(
            ResourceDemandReason::cREVISION_NOT_INCREMENTED,
            std::string(), "revision", &draft);
         return false;
      }
      const RevisionKey key(draft.demandSetId, draft.revision);
      if (mSeenRevisions.count(key) != 0)
      {
         mLastLoad = resource_demand_detail::Failure(
            ResourceDemandReason::cDUPLICATE_DEMAND_REVISION,
            std::string(), "revision", &draft);
         return false;
      }
      if (mHasCurrentSet && draft.previousDemandSetId.empty())
      {
         draft.previousDemandSetId = mCurrentSet.demandSetId;
         draft.previousRevision = mCurrentSet.revision;
      }
      mCurrentSet = draft;
      mHasCurrentSet = true;
      mSeenRevisions.insert(key);
      mLastLoad = resource_demand_detail::Success(std::string(), draft);
      return true;
   }

   void Unload()
   {
      mCurrentSet = ResourceDemandSet();
      mHasCurrentSet = false;
   }

   bool SaveRevision(const std::string& aPath = std::string())
   {
      if (!mHasCurrentSet)
      {
         mLastSave = resource_demand_detail::Failure(
            ResourceDemandReason::cNO_CURRENT_DEMAND_SET, aPath, "demandSet");
         return false;
      }
      const std::string path = ResolveSavePath(aPath, mCurrentSet);
      if (path.empty())
      {
         mLastSave = resource_demand_detail::Failure(
            ResourceDemandReason::cOUTPUT_PATH_INVALID,
            path, "path", &mCurrentSet);
         return false;
      }
      mLastSave = SaveDocumentAtomic(mCurrentSet, path, true);
      return mLastSave.success;
   }

   bool HasCurrentDemandSet() const { return mHasCurrentSet; }

   const ResourceDemandSet* GetCurrentDemandSet() const
   {
      return mHasCurrentSet ? &mCurrentSet : nullptr;
   }

   const ResourceDemandRepositoryResult& LastLoadResult() const
   {
      return mLastLoad;
   }

   const ResourceDemandRepositoryResult& LastSaveResult() const
   {
      return mLastSave;
   }

   static ResourceDemandRepositoryResult SaveDocumentAtomic(
      const ResourceDemandSet& aSet,
      const std::string& aPath,
      bool aRefuseOverwrite)
   {
      const ResourceDemandSet normalized = resource_demand_detail::Normalize(aSet);
      const ResourceDemandRepositoryResult validation =
         resource_demand_detail::ValidateDocument(normalized, aPath);
      if (!validation.success) return validation;
      if (aPath.empty())
         return resource_demand_detail::Failure(
            ResourceDemandReason::cOUTPUT_PATH_INVALID, aPath, "path", &normalized);
      if (aRefuseOverwrite && resource_demand_detail::FileExists(aPath))
         return resource_demand_detail::Failure(
            ResourceDemandReason::cFILE_ALREADY_EXISTS, aPath, "path", &normalized);

      const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
      const std::string temporaryPath = aPath + ".tmp-" + std::to_string(stamp);
      {
         std::ofstream output(temporaryPath, std::ios::out | std::ios::trunc);
         if (!output)
            return resource_demand_detail::Failure(
               ResourceDemandReason::cFILE_WRITE_FAILED,
               aPath, "temporaryPath", &normalized);
         resource_demand_detail::WriteDocument(output, normalized);
         output.flush();
         if (!output)
         {
            output.close();
            std::remove(temporaryPath.c_str());
            return resource_demand_detail::Failure(
               ResourceDemandReason::cFILE_WRITE_FAILED,
               aPath, "document", &normalized);
         }
      }
      if (std::rename(temporaryPath.c_str(), aPath.c_str()) != 0)
      {
         std::remove(temporaryPath.c_str());
         return resource_demand_detail::Failure(
            ResourceDemandReason::cATOMIC_RENAME_FAILED,
            aPath, "path", &normalized);
      }
      return resource_demand_detail::Success(aPath, normalized);
   }

private:
   using RevisionKey = std::pair<std::string, std::uint64_t>;

   struct PendingNetwork
   {
      std::string demandId;
      NetworkType networkType = NetworkType::cUNKNOWN;
   };

   static ResourceDemandRepositoryResult ParseFile(
      const std::string& aPath,
      ResourceDemandSet& aSet)
   {
      std::ifstream input(aPath);
      if (!input)
         return resource_demand_detail::Failure(
            ResourceDemandReason::cFILE_OPEN_FAILED, aPath, "path");

      ResourceDemandSet parsed;
      bool headerRead = false;
      std::vector<PendingNetwork> pendingNetworks;
      std::string line;
      std::size_t lineNumber = 0;
      while (std::getline(input, line))
      {
         ++lineNumber;
         std::istringstream record(line);
         record >> std::ws;
         if (record.eof() || record.peek() == '#') continue;

         std::string recordType;
         if (!(record >> recordType)) continue;
         const std::string field = "line:" + std::to_string(lineNumber);
         if (!headerRead)
         {
            if (recordType != "NRM_RESOURCE_DEMAND_V1")
               return resource_demand_detail::Failure(
                  ResourceDemandReason::cUNSUPPORTED_SCHEMA, aPath, field);
            std::string origin;
            std::string confidence;
            if (!(record >> std::quoted(parsed.demandSetId)) ||
                !resource_demand_detail::ReadUnsigned(record, parsed.revision) ||
                !(record >> std::quoted(parsed.configVersion) >>
                  std::quoted(parsed.providerId) >> std::quoted(parsed.createdTime) >>
                  origin >> confidence) ||
                !resource_demand_detail::ReadBool(record, parsed.valid) ||
                !(record >> std::quoted(parsed.previousDemandSetId)) ||
                !resource_demand_detail::ReadUnsigned(record, parsed.previousRevision))
               return resource_demand_detail::Failure(
                  ResourceDemandReason::cPARSE_ERROR, aPath, field);
            if (!resource_demand_detail::ParseOrigin(origin, parsed.source) ||
                !resource_demand_detail::ParseConfidence(confidence,
                                                         parsed.confidence))
               return resource_demand_detail::Failure(
                  ResourceDemandReason::cPARSE_ERROR, aPath, field);
            if (resource_demand_detail::HasTrailingToken(record))
               return resource_demand_detail::Failure(
                  ResourceDemandReason::cTRAILING_TOKEN, aPath, field);
            parsed.schemaVersion = "nrm.resource_demand_set.v1";
            headerRead = true;
            continue;
         }

         if (recordType == "DEMAND")
         {
            ResourceDemand demand;
            std::uint64_t minimumNetworkSize = 0;
            if (!(record >> std::quoted(demand.demandId) >>
                  std::quoted(demand.missionStage) >>
                  std::quoted(demand.businessType) >>
                  std::quoted(demand.sourcePlatform) >>
                  std::quoted(demand.destinationPlatform)) ||
                !resource_demand_detail::ReadUnsigned(record, demand.payloadBits) ||
                !(record >> demand.businessTrafficBps >>
                  demand.requiredBandwidthBps >> demand.maximumDelayMs >>
                  demand.minimumPdrPercent >> demand.maximumDistanceM) ||
                !resource_demand_detail::ReadUnsigned(record, minimumNetworkSize) ||
                minimumNetworkSize > std::numeric_limits<std::size_t>::max())
               return resource_demand_detail::Failure(
                  ResourceDemandReason::cPARSE_ERROR, aPath, field, &parsed);
            if (resource_demand_detail::HasTrailingToken(record))
               return resource_demand_detail::Failure(
                  ResourceDemandReason::cTRAILING_TOKEN, aPath, field, &parsed);
            demand.minimumNetworkSize =
               static_cast<std::size_t>(minimumNetworkSize);
            demand.demandSetId = parsed.demandSetId;
            demand.revision = parsed.revision;
            demand.source = parsed.source;
            demand.confidence = parsed.confidence;
            demand.valid = parsed.valid;
            parsed.demands.push_back(demand);
         }
         else if (recordType == "DEMAND_NETWORK")
         {
            PendingNetwork pending;
            std::string networkType;
            if (!(record >> std::quoted(pending.demandId) >> networkType) ||
                !resource_demand_detail::ParseNetworkType(
                   networkType, pending.networkType))
               return resource_demand_detail::Failure(
                  ResourceDemandReason::cPARSE_ERROR, aPath, field, &parsed);
            if (resource_demand_detail::HasTrailingToken(record))
               return resource_demand_detail::Failure(
                  ResourceDemandReason::cTRAILING_TOKEN, aPath, field, &parsed);
            pendingNetworks.push_back(pending);
         }
         else
         {
            return resource_demand_detail::Failure(
               ResourceDemandReason::cUNKNOWN_RECORD_TYPE,
               aPath, field, &parsed);
         }
      }
      if (!input.eof() && input.fail())
         return resource_demand_detail::Failure(
            ResourceDemandReason::cPARSE_ERROR, aPath, "stream", &parsed);
      if (!headerRead)
         return resource_demand_detail::Failure(
            ResourceDemandReason::cUNSUPPORTED_SCHEMA, aPath, "header", &parsed);

      for (const PendingNetwork& pending : pendingNetworks)
      {
         ResourceDemand* demand =
            resource_demand_detail::FindDemand(parsed, pending.demandId);
         if (demand == nullptr)
            return resource_demand_detail::Failure(
               ResourceDemandReason::cREFERENCE_NOT_FOUND,
               aPath, "DEMAND_NETWORK", &parsed);
         demand->allowedNetworks.push_back(pending.networkType);
      }
      parsed = resource_demand_detail::Normalize(parsed);
      const ResourceDemandRepositoryResult validation =
         resource_demand_detail::ValidateDocument(parsed, aPath);
      if (!validation.success) return validation;
      aSet = parsed;
      return validation;
   }

   static std::string ResolveSavePath(const std::string& aPath,
                                      const ResourceDemandSet& aSet)
   {
      if (!aPath.empty()) return aPath;
      const char* directory = std::getenv("NRM_DEMAND_STORE_DIR");
      if (directory == nullptr || directory[0] == '\0') return std::string();
      std::string result(directory);
      if (result.back() != '/') result += '/';
      result += aSet.demandSetId + "-r" + std::to_string(aSet.revision) + ".nrm";
      return result;
   }

   ResourceDemandSet mCurrentSet;
   bool mHasCurrentSet = false;
   std::set<RevisionKey> mSeenRevisions;
   ResourceDemandRepositoryResult mLastLoad;
   ResourceDemandRepositoryResult mLastSave;
};
} // namespace nrm

#endif
