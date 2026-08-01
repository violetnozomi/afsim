#ifndef NRM_NETWORK_PLAN_REPOSITORY_HPP
#define NRM_NETWORK_PLAN_REPOSITORY_HPP

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

#include "nrm/NetworkPlanTypes.hpp"
#include "nrm/NetworkTypeUtils.hpp"

namespace nrm
{
namespace network_plan_detail
{
inline bool HasTrailingToken(std::istringstream& aInput)
{
   aInput >> std::ws;
   return !aInput.eof();
}

inline bool ReadUnsigned(std::istringstream& aInput, std::uint64_t& aValue)
{
   std::string token;
   if (!(aInput >> token) || token.empty())
   {
      return false;
   }
   std::uint64_t value = 0;
   for (char character : token)
   {
      if (character < '0' || character > '9')
      {
         return false;
      }
      const std::uint64_t digit = static_cast<std::uint64_t>(character - '0');
      if (value > (std::numeric_limits<std::uint64_t>::max() - digit) / 10)
      {
         return false;
      }
      value = value * 10 + digit;
   }
   aValue = value;
   return true;
}

inline bool ReadBool(std::istringstream& aInput, bool& aValue)
{
   std::string token;
   if (!(aInput >> token) || (token != "0" && token != "1"))
   {
      return false;
   }
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

inline bool ParseState(const std::string& aToken, NetworkPlanState& aState)
{
   if (aToken == "DRAFT") aState = NetworkPlanState::cDRAFT;
   else if (aToken == "VALIDATED") aState = NetworkPlanState::cVALIDATED;
   else if (aToken == "REJECTED") aState = NetworkPlanState::cREJECTED;
   else if (aToken == "READY_FOR_DISTRIBUTION")
      aState = NetworkPlanState::cREADY_FOR_DISTRIBUTION;
   else return false;
   return true;
}

inline bool ParseOrigin(const std::string& aToken, DataOrigin& aOrigin)
{
   if (aToken == "AFSIM_INTERNAL") aOrigin = DataOrigin::cAFSIM_INTERNAL;
   else if (aToken == "CUSTOMER_MODULE") aOrigin = DataOrigin::cCUSTOMER_MODULE;
   else if (aToken == "REPLAY") aOrigin = DataOrigin::cREPLAY;
   else if (aToken == "PARAMETERIZED_MODEL") aOrigin = DataOrigin::cPARAMETERIZED_MODEL;
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

inline bool ParseChangeType(const std::string& aToken, PlanChangeType& aType)
{
   if (aToken == "JOIN") aType = PlanChangeType::cJOIN;
   else if (aToken == "LEAVE") aType = PlanChangeType::cLEAVE;
   else return false;
   return true;
}

inline PlanRepositoryResult Failure(PlanValidationReason aReason,
                                    const std::string& aPath,
                                    const std::string& aField,
                                    const NetworkPlanDocument* aDocumentPtr = nullptr)
{
   PlanRepositoryResult result;
   result.reason = aReason;
   result.path = aPath;
   result.field = aField;
   if (aDocumentPtr != nullptr)
   {
      result.planId = aDocumentPtr->planId;
      result.revision = aDocumentPtr->revision;
   }
   return result;
}

inline PlanRepositoryResult Success(const std::string& aPath,
                                    const NetworkPlanDocument& aDocument)
{
   PlanRepositoryResult result;
   result.success = true;
   result.reason = PlanValidationReason::cNONE;
   result.path = aPath;
   result.planId = aDocument.planId;
   result.revision = aDocument.revision;
   return result;
}

inline bool FileExists(const std::string& aPath)
{
   struct stat info;
   return !aPath.empty() && stat(aPath.c_str(), &info) == 0;
}

inline bool IsSafePlanId(const std::string& aValue)
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

inline NetworkPlanAllocation* FindAllocation(NetworkPlanDocument& aDocument,
                                             const std::string& aAllocationId)
{
   for (NetworkPlanAllocation& allocation : aDocument.allocations)
   {
      if (allocation.allocationId == aAllocationId) return &allocation;
   }
   return nullptr;
}

inline NetworkPlanDemand* FindDemand(NetworkPlanDocument& aDocument,
                                     const std::string& aDemandId)
{
   for (NetworkPlanDemand& demand : aDocument.demands)
   {
      if (demand.demandId == aDemandId) return &demand;
   }
   return nullptr;
}

inline PlanRepositoryResult ValidateParsedDocument(const NetworkPlanDocument& aDocument,
                                                   const std::string& aPath)
{
   if (aDocument.schemaVersion != "nrm.network_plan.v1")
      return Failure(PlanValidationReason::cUNSUPPORTED_SCHEMA, aPath, "schemaVersion",
                     &aDocument);
   if (!IsSafePlanId(aDocument.planId))
      return Failure(PlanValidationReason::cINVALID_PLAN_ID, aPath, "planId", &aDocument);
   if (aDocument.revision == 0)
      return Failure(PlanValidationReason::cINVALID_REVISION, aPath, "revision", &aDocument);
   if (aDocument.configVersion.empty() || aDocument.providerId.empty() ||
       aDocument.createdTime.empty())
      return Failure(PlanValidationReason::cMISSING_REQUIRED_FIELD, aPath, "header",
                     &aDocument);

   std::set<std::string> allocationIds;
   for (const NetworkPlanAllocation& allocation : aDocument.allocations)
   {
      if (allocation.allocationId.empty())
         return Failure(PlanValidationReason::cMISSING_REQUIRED_FIELD, aPath,
                        "allocationId", &aDocument);
      if (!allocationIds.insert(allocation.allocationId).second)
         return Failure(PlanValidationReason::cDUPLICATE_ALLOCATION_ID, aPath,
                        "allocationId", &aDocument);
      if (!std::isfinite(allocation.frequencyHz))
         return Failure(PlanValidationReason::cNON_FINITE_VALUE, aPath,
                        "frequencyHz", &aDocument);
      if (allocation.frequencyHz < 0.0)
         return Failure(PlanValidationReason::cNEGATIVE_VALUE, aPath,
                        "frequencyHz", &aDocument);
   }

   std::set<std::string> demandIds;
   for (const NetworkPlanDemand& demand : aDocument.demands)
   {
      if (demand.demandId.empty())
         return Failure(PlanValidationReason::cMISSING_REQUIRED_FIELD, aPath,
                        "demandId", &aDocument);
      if (!demandIds.insert(demand.demandId).second)
         return Failure(PlanValidationReason::cDUPLICATE_DEMAND_ID, aPath,
                        "demandId", &aDocument);
      if (!std::isfinite(demand.requiredBandwidthBps) ||
          !std::isfinite(demand.maximumDelayMs) ||
          !std::isfinite(demand.minimumPdrPercent))
         return Failure(PlanValidationReason::cNON_FINITE_VALUE, aPath,
                        "demandNumeric", &aDocument);
      if (demand.requiredBandwidthBps < 0.0 || demand.maximumDelayMs < 0.0)
         return Failure(PlanValidationReason::cNEGATIVE_VALUE, aPath,
                        "demandNumeric", &aDocument);
      if (demand.minimumPdrPercent < 0.0 || demand.minimumPdrPercent > 100.0)
         return Failure(PlanValidationReason::cPDR_OUT_OF_RANGE, aPath,
                        "minimumPdrPercent", &aDocument);
   }

   std::set<std::string> changeIds;
   for (const NetworkPlanChange& change : aDocument.changes)
   {
      if (change.changeId.empty())
         return Failure(PlanValidationReason::cMISSING_REQUIRED_FIELD, aPath,
                        "changeId", &aDocument);
      if (!changeIds.insert(change.changeId).second)
         return Failure(PlanValidationReason::cDUPLICATE_CHANGE_ID, aPath,
                        "changeId", &aDocument);
   }
   return Success(aPath, aDocument);
}

inline void WriteDocument(std::ostream& aOutput, const NetworkPlanDocument& aDocument)
{
   aOutput << std::setprecision(std::numeric_limits<double>::max_digits10);
   aOutput << "NRM_NETWORK_PLAN_V1 " << std::quoted(aDocument.planId) << ' '
           << aDocument.revision << ' ' << std::quoted(aDocument.configVersion) << ' '
           << std::quoted(aDocument.providerId) << ' '
           << std::quoted(aDocument.createdTime) << ' ' << ToString(aDocument.state) << ' '
           << ToString(aDocument.source) << ' ' << ToString(aDocument.confidence) << ' '
           << (aDocument.valid ? 1 : 0) << ' ' << std::quoted(aDocument.previousPlanId)
           << ' ' << aDocument.previousRevision << '\n';
   for (const NetworkPlanAllocation& allocation : aDocument.allocations)
   {
      aOutput << "ALLOCATION " << std::quoted(allocation.allocationId) << ' '
              << std::quoted(allocation.networkName) << ' '
              << ToString(allocation.networkType) << ' '
              << std::quoted(allocation.profileId) << ' ' << allocation.frequencyHz << ' '
              << std::quoted(allocation.channelId) << ' '
              << std::quoted(allocation.subnetId) << ' '
              << std::quoted(allocation.routePolicyId) << ' '
              << (allocation.enabled ? 1 : 0) << '\n';
      for (const std::string& member : allocation.memberPlatformIds)
         aOutput << "MEMBER " << std::quoted(allocation.allocationId) << ' '
                 << std::quoted(member) << '\n';
      for (const std::string& slot : allocation.slotIds)
         aOutput << "SLOT " << std::quoted(allocation.allocationId) << ' '
                 << std::quoted(slot) << '\n';
   }
   for (const NetworkPlanDemand& demand : aDocument.demands)
   {
      aOutput << "DEMAND " << std::quoted(demand.demandId) << ' '
              << std::quoted(demand.businessType) << ' '
              << std::quoted(demand.sourcePlatform) << ' '
              << std::quoted(demand.destinationPlatform) << ' '
              << demand.payloadBits << ' ' << demand.requiredBandwidthBps << ' '
              << demand.maximumDelayMs << ' ' << demand.minimumPdrPercent << '\n';
      for (NetworkType networkType : demand.allowedNetworks)
         aOutput << "DEMAND_NETWORK " << std::quoted(demand.demandId) << ' '
                 << ToString(networkType) << '\n';
   }
   for (const NetworkPlanChange& change : aDocument.changes)
      aOutput << "CHANGE " << std::quoted(change.changeId) << ' '
              << ToString(change.changeType) << ' '
              << std::quoted(change.allocationId) << ' '
              << std::quoted(change.platformId) << '\n';
}

inline std::string PlanContentFingerprint(const NetworkPlanDocument& aDocument)
{
   NetworkPlanDocument canonical = aDocument;
   // Packaging advances lifecycle state without changing the reviewed plan content.
   canonical.state = NetworkPlanState::cDRAFT;
   std::ostringstream serialized;
   WriteDocument(serialized, canonical);

   std::uint64_t fingerprint = 14695981039346656037ULL;
   for (unsigned char byte : serialized.str())
   {
      fingerprint ^= static_cast<std::uint64_t>(byte);
      fingerprint *= 1099511628211ULL;
   }
   std::ostringstream output;
   output << std::hex << std::setw(16) << std::setfill('0') << fingerprint;
   return output.str();
}
} // namespace network_plan_detail

class NetworkPlanRepository
{
public:
   static constexpr const char* cSCHEMA_VERSION = "nrm.network_plan.v1";

   bool LoadFromFile(const std::string& aPath)
   {
      NetworkPlanDocument parsed;
      PlanRepositoryResult result = ParseFile(aPath, parsed);
      if (!result.success)
      {
         mLastLoad = result;
         return false;
      }
      const RevisionKey key(parsed.planId, parsed.revision);
      if (mHasCurrentPlan && mCurrentPlan.planId == parsed.planId &&
          mCurrentPlan.revision == parsed.revision)
      {
         mLastLoad = network_plan_detail::Failure(
            PlanValidationReason::cDUPLICATE_PLAN_REVISION, aPath, "revision", &parsed);
         return false;
      }
      mCurrentPlan = parsed;
      mHasCurrentPlan = true;
      mSeenRevisions.insert(key);
      mLastLoad = result;
      return true;
   }

   bool ReplaceDraft(const NetworkPlanDocument& aDocument)
   {
      NetworkPlanDocument draft = aDocument;
      draft.state = NetworkPlanState::cDRAFT;
      const PlanRepositoryResult syntax =
         network_plan_detail::ValidateParsedDocument(draft, std::string());
      if (!syntax.success)
      {
         mLastLoad = syntax;
         return false;
      }
      if (mHasCurrentPlan && draft.planId == mCurrentPlan.planId &&
          draft.revision <= mCurrentPlan.revision)
      {
         mLastLoad = network_plan_detail::Failure(
            PlanValidationReason::cREVISION_NOT_INCREMENTED, std::string(), "revision", &draft);
         return false;
      }
      const RevisionKey key(draft.planId, draft.revision);
      if (mSeenRevisions.count(key) != 0)
      {
         mLastLoad = network_plan_detail::Failure(
            PlanValidationReason::cDUPLICATE_PLAN_REVISION, std::string(), "revision", &draft);
         return false;
      }
      if (mHasCurrentPlan && draft.previousPlanId.empty())
      {
         draft.previousPlanId = mCurrentPlan.planId;
         draft.previousRevision = mCurrentPlan.revision;
      }
      mCurrentPlan = draft;
      mHasCurrentPlan = true;
      mSeenRevisions.insert(key);
      mLastLoad = network_plan_detail::Success(std::string(), draft);
      return true;
   }

   void Unload()
   {
      mCurrentPlan = NetworkPlanDocument();
      mHasCurrentPlan = false;
   }

   bool SaveRevision(const std::string& aPath = std::string())
   {
      if (!mHasCurrentPlan)
      {
         mLastSave = network_plan_detail::Failure(
            PlanValidationReason::cNO_CURRENT_PLAN, aPath, "plan");
         return false;
      }
      const std::string path = ResolveSavePath(aPath, mCurrentPlan);
      if (path.empty())
      {
         mLastSave = network_plan_detail::Failure(
            PlanValidationReason::cOUTPUT_PATH_INVALID, path, "path", &mCurrentPlan);
         return false;
      }
      mLastSave = SaveDocumentAtomic(mCurrentPlan, path, true);
      return mLastSave.success;
   }

   bool HasCurrentPlan() const { return mHasCurrentPlan; }
   const NetworkPlanDocument* GetCurrentPlan() const
   {
      return mHasCurrentPlan ? &mCurrentPlan : nullptr;
   }
   const PlanRepositoryResult& LastLoadResult() const { return mLastLoad; }
   const PlanRepositoryResult& LastSaveResult() const { return mLastSave; }

   static PlanRepositoryResult SaveDocumentAtomic(const NetworkPlanDocument& aDocument,
                                                  const std::string& aPath,
                                                  bool aRefuseOverwrite)
   {
      const PlanRepositoryResult syntax =
         network_plan_detail::ValidateParsedDocument(aDocument, aPath);
      if (!syntax.success) return syntax;
      if (aPath.empty())
         return network_plan_detail::Failure(
            PlanValidationReason::cOUTPUT_PATH_INVALID, aPath, "path", &aDocument);
      if (aRefuseOverwrite && network_plan_detail::FileExists(aPath))
         return network_plan_detail::Failure(
            PlanValidationReason::cDUPLICATE_PLAN_REVISION, aPath, "path", &aDocument);

      const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
      const std::string temporaryPath = aPath + ".tmp-" + std::to_string(stamp);
      {
         std::ofstream output(temporaryPath, std::ios::out | std::ios::trunc);
         if (!output)
            return network_plan_detail::Failure(
               PlanValidationReason::cFILE_WRITE_FAILED, aPath, "temporaryPath", &aDocument);
         network_plan_detail::WriteDocument(output, aDocument);
         output.flush();
         if (!output)
         {
            output.close();
            std::remove(temporaryPath.c_str());
            return network_plan_detail::Failure(
               PlanValidationReason::cFILE_WRITE_FAILED, aPath, "document", &aDocument);
         }
      }
      if (std::rename(temporaryPath.c_str(), aPath.c_str()) != 0)
      {
         std::remove(temporaryPath.c_str());
         return network_plan_detail::Failure(
            PlanValidationReason::cATOMIC_RENAME_FAILED, aPath, "path", &aDocument);
      }
      return network_plan_detail::Success(aPath, aDocument);
   }

private:
   using RevisionKey = std::pair<std::string, std::uint64_t>;

   struct PendingValue
   {
      std::string ownerId;
      std::string value;
      NetworkType networkType = NetworkType::cUNKNOWN;
   };

   static PlanRepositoryResult ParseFile(const std::string& aPath,
                                         NetworkPlanDocument& aDocument)
   {
      std::ifstream input(aPath);
      if (!input)
         return network_plan_detail::Failure(
            PlanValidationReason::cFILE_OPEN_FAILED, aPath, "path");

      NetworkPlanDocument parsed;
      bool headerRead = false;
      std::vector<PendingValue> members;
      std::vector<PendingValue> pendingSlots;
      std::vector<PendingValue> demandNetworks;
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
            if (recordType != "NRM_NETWORK_PLAN_V1")
               return network_plan_detail::Failure(
                  PlanValidationReason::cUNSUPPORTED_SCHEMA, aPath, field);
            std::string state;
            std::string origin;
            std::string confidence;
            if (!(record >> std::quoted(parsed.planId)) ||
                !network_plan_detail::ReadUnsigned(record, parsed.revision) ||
                !(record >> std::quoted(parsed.configVersion) >>
                  std::quoted(parsed.providerId) >> std::quoted(parsed.createdTime) >>
                  state >> origin >> confidence) ||
                !network_plan_detail::ReadBool(record, parsed.valid) ||
                !(record >> std::quoted(parsed.previousPlanId)) ||
                !network_plan_detail::ReadUnsigned(record, parsed.previousRevision))
               return network_plan_detail::Failure(
                  PlanValidationReason::cPARSE_ERROR, aPath, field);
            if (!network_plan_detail::ParseState(state, parsed.state) ||
                !network_plan_detail::ParseOrigin(origin, parsed.source) ||
                !network_plan_detail::ParseConfidence(confidence, parsed.confidence))
               return network_plan_detail::Failure(
                  PlanValidationReason::cPARSE_ERROR, aPath, field);
            if (network_plan_detail::HasTrailingToken(record))
               return network_plan_detail::Failure(
                  PlanValidationReason::cTRAILING_TOKEN, aPath, field);
            parsed.schemaVersion = cSCHEMA_VERSION;
            headerRead = true;
            continue;
         }

         if (recordType == "ALLOCATION")
         {
            NetworkPlanAllocation allocation;
            std::string networkType;
            if (!(record >> std::quoted(allocation.allocationId) >>
                  std::quoted(allocation.networkName) >> networkType >>
                  std::quoted(allocation.profileId) >> allocation.frequencyHz >>
                  std::quoted(allocation.channelId) >> std::quoted(allocation.subnetId) >>
                  std::quoted(allocation.routePolicyId)) ||
                !network_plan_detail::ReadBool(record, allocation.enabled) ||
                !network_plan_detail::ParseNetworkType(networkType, allocation.networkType))
               return network_plan_detail::Failure(
                  PlanValidationReason::cPARSE_ERROR, aPath, field, &parsed);
            if (network_plan_detail::HasTrailingToken(record))
               return network_plan_detail::Failure(
                  PlanValidationReason::cTRAILING_TOKEN, aPath, field, &parsed);
            parsed.allocations.push_back(allocation);
         }
         else if (recordType == "MEMBER" || recordType == "SLOT")
         {
            PendingValue pending;
            if (!(record >> std::quoted(pending.ownerId) >> std::quoted(pending.value)))
               return network_plan_detail::Failure(
                  PlanValidationReason::cPARSE_ERROR, aPath, field, &parsed);
            if (network_plan_detail::HasTrailingToken(record))
               return network_plan_detail::Failure(
                  PlanValidationReason::cTRAILING_TOKEN, aPath, field, &parsed);
            (recordType == "MEMBER" ? members : pendingSlots).push_back(pending);
         }
         else if (recordType == "DEMAND")
         {
            NetworkPlanDemand demand;
            if (!(record >> std::quoted(demand.demandId) >>
                  std::quoted(demand.businessType) >> std::quoted(demand.sourcePlatform) >>
                  std::quoted(demand.destinationPlatform)) ||
                !network_plan_detail::ReadUnsigned(record, demand.payloadBits) ||
                !(record >> demand.requiredBandwidthBps >> demand.maximumDelayMs >>
                  demand.minimumPdrPercent))
               return network_plan_detail::Failure(
                  PlanValidationReason::cPARSE_ERROR, aPath, field, &parsed);
            if (network_plan_detail::HasTrailingToken(record))
               return network_plan_detail::Failure(
                  PlanValidationReason::cTRAILING_TOKEN, aPath, field, &parsed);
            parsed.demands.push_back(demand);
         }
         else if (recordType == "DEMAND_NETWORK")
         {
            PendingValue pending;
            std::string networkType;
            if (!(record >> std::quoted(pending.ownerId) >> networkType) ||
                !network_plan_detail::ParseNetworkType(networkType, pending.networkType))
               return network_plan_detail::Failure(
                  PlanValidationReason::cPARSE_ERROR, aPath, field, &parsed);
            if (network_plan_detail::HasTrailingToken(record))
               return network_plan_detail::Failure(
                  PlanValidationReason::cTRAILING_TOKEN, aPath, field, &parsed);
            demandNetworks.push_back(pending);
         }
         else if (recordType == "CHANGE")
         {
            NetworkPlanChange change;
            std::string changeType;
            if (!(record >> std::quoted(change.changeId) >> changeType >>
                  std::quoted(change.allocationId) >> std::quoted(change.platformId)) ||
                !network_plan_detail::ParseChangeType(changeType, change.changeType))
               return network_plan_detail::Failure(
                  PlanValidationReason::cPARSE_ERROR, aPath, field, &parsed);
            if (network_plan_detail::HasTrailingToken(record))
               return network_plan_detail::Failure(
                  PlanValidationReason::cTRAILING_TOKEN, aPath, field, &parsed);
            parsed.changes.push_back(change);
         }
         else
         {
            return network_plan_detail::Failure(
               PlanValidationReason::cUNKNOWN_RECORD_TYPE, aPath, field, &parsed);
         }
      }
      if (!input.eof() && input.fail())
         return network_plan_detail::Failure(
            PlanValidationReason::cPARSE_ERROR, aPath, "stream", &parsed);
      if (!headerRead)
         return network_plan_detail::Failure(
            PlanValidationReason::cUNSUPPORTED_SCHEMA, aPath, "header", &parsed);

      for (const PendingValue& member : members)
      {
         NetworkPlanAllocation* allocation =
            network_plan_detail::FindAllocation(parsed, member.ownerId);
         if (allocation == nullptr)
            return network_plan_detail::Failure(
               PlanValidationReason::cREFERENCE_NOT_FOUND, aPath, "MEMBER", &parsed);
         allocation->memberPlatformIds.push_back(member.value);
      }
      for (const PendingValue& slot : pendingSlots)
      {
         NetworkPlanAllocation* allocation =
            network_plan_detail::FindAllocation(parsed, slot.ownerId);
         if (allocation == nullptr)
            return network_plan_detail::Failure(
               PlanValidationReason::cREFERENCE_NOT_FOUND, aPath, "SLOT", &parsed);
         allocation->slotIds.push_back(slot.value);
      }
      for (const PendingValue& allowed : demandNetworks)
      {
         NetworkPlanDemand* demand =
            network_plan_detail::FindDemand(parsed, allowed.ownerId);
         if (demand == nullptr)
            return network_plan_detail::Failure(
               PlanValidationReason::cREFERENCE_NOT_FOUND, aPath, "DEMAND_NETWORK", &parsed);
         demand->allowedNetworks.push_back(allowed.networkType);
      }

      const PlanRepositoryResult validation =
         network_plan_detail::ValidateParsedDocument(parsed, aPath);
      if (!validation.success) return validation;
      aDocument = parsed;
      return validation;
   }

   static std::string ResolveSavePath(const std::string& aPath,
                                      const NetworkPlanDocument& aDocument)
   {
      if (!aPath.empty()) return aPath;
      const char* directory = std::getenv("NRM_PLAN_STORE_DIR");
      if (directory == nullptr || directory[0] == '\0') return std::string();
      std::string result(directory);
      if (!result.empty() && result.back() != '/') result += '/';
      result += aDocument.planId + "-r" + std::to_string(aDocument.revision) + ".nrm";
      return result;
   }

   NetworkPlanDocument mCurrentPlan;
   bool mHasCurrentPlan = false;
   std::set<RevisionKey> mSeenRevisions;
   PlanRepositoryResult mLastLoad;
   PlanRepositoryResult mLastSave;
};
} // namespace nrm

#endif
