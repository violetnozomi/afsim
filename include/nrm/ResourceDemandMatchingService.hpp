#ifndef NRM_RESOURCE_DEMAND_MATCHING_SERVICE_HPP
#define NRM_RESOURCE_DEMAND_MATCHING_SERVICE_HPP

#include <algorithm>
#include <cmath>
#include <set>
#include <string>
#include <vector>

#include "nrm/CommunicationCapabilityService.hpp"
#include "nrm/NetworkPlanRepository.hpp"
#include "nrm/PlanningRecommendationEngine.hpp"
#include "nrm/ResourceDemandTypes.hpp"

namespace nrm
{
class ResourceDemandCapabilityQuery
{
public:
   virtual ~ResourceDemandCapabilityQuery() = default;

   virtual CapabilityResult Query(
      const ResourceSnapshot& aSnapshot,
      const CapabilityRequest& aRequest,
      const EnvironmentContext& aEnvironment = EnvironmentContext()) const = 0;
};

class ResourceDemandMatchingService
{
public:
   explicit ResourceDemandMatchingService(
      const NetworkProfileRepository& aProfiles,
      const EnvironmentEffectAdapter* aEnvironmentAdapterPtr = nullptr,
      const ResourceDemandCapabilityQuery* aCapabilityQueryPtr = nullptr)
      : mProfiles(aProfiles)
      , mCapabilityService(aProfiles, aEnvironmentAdapterPtr)
      , mCapabilityQueryPtr(aCapabilityQueryPtr)
   {
   }

   ResourceDemandBatchResult Evaluate(
      const ResourceSnapshot& aSnapshot,
      const ResourceDemandSet& aDemandSet,
      const NetworkPlanDocument* aPlanPtr = nullptr,
      const NetworkPlanEvaluationResult* aPlanEvaluationPtr = nullptr,
      const EnvironmentContext& aEnvironment = EnvironmentContext(),
      const PlanningCandidateSet* aCandidatesPtr = nullptr) const
   {
      ResourceDemandBatchResult batch;
      batch.demandSetId = aDemandSet.demandSetId;
      batch.revision = aDemandSet.revision;
      batch.snapshotVersion = aSnapshot.snapshotVersion;
      batch.totalCount = aDemandSet.demands.size();

      const ResourceDemandReason setReason = ValidateSet(aDemandSet);
      const ResourceDemandReason snapshotReason = ValidateSnapshot(aSnapshot);
      const bool planEvidenceValid =
         ValidatePlanEvidence(aSnapshot, aPlanPtr, aPlanEvaluationPtr);
      std::set<std::string> demandIds;

      for (const ResourceDemand& demand : aDemandSet.demands)
      {
         ResourceDemandMatchResult result;
         InitializeResult(aSnapshot, aDemandSet, demand, aPlanPtr, result);

         ResourceDemandReason reason = setReason;
         if (reason == ResourceDemandReason::cNONE)
            reason = snapshotReason;
         if (reason == ResourceDemandReason::cNONE &&
             !demandIds.insert(demand.demandId).second)
            reason = ResourceDemandReason::cDUPLICATE_DEMAND_ID;
         if (reason == ResourceDemandReason::cNONE)
            reason = ValidateDemand(aSnapshot, aDemandSet, demand);
         if (reason == ResourceDemandReason::cNONE && !planEvidenceValid)
            reason = ResourceDemandReason::cPLAN_EVIDENCE_MISMATCH;

         if (reason != ResourceDemandReason::cNONE)
         {
            PopulateInvalidChecks(aSnapshot, demand, reason, result);
            AddReason(result, reason);
         }
         else
         {
            const CapabilityRequest request = MapRequest(demand);
            result.capability = QueryCapability(aSnapshot, request, aEnvironment);
            PopulateChecks(aSnapshot, demand, result);
            FinalizeStatus(result);
         }

         const PlanningRecommendationEngine recommendationEngine(mProfiles);
         result.recommendations = recommendationEngine.Evaluate(
            aSnapshot, demand, result, aPlanPtr, aCandidatesPtr);

         Count(result.status, batch);
         batch.results.push_back(result);
      }
      return batch;
   }

private:
   static ResourceDemandReason ValidateSet(const ResourceDemandSet& aDemandSet)
   {
      if (aDemandSet.schemaVersion != "nrm.resource_demand_set.v1")
         return ResourceDemandReason::cUNSUPPORTED_SCHEMA;
      if (!aDemandSet.valid)
         return ResourceDemandReason::cMISSING_REQUIRED_FIELD;
      if (aDemandSet.demandSetId.empty())
         return ResourceDemandReason::cINVALID_DEMAND_SET_ID;
      if (aDemandSet.revision == 0)
         return ResourceDemandReason::cINVALID_REVISION;
      if (aDemandSet.configVersion.empty() || aDemandSet.providerId.empty())
         return ResourceDemandReason::cMISSING_REQUIRED_FIELD;
      return ResourceDemandReason::cNONE;
   }

   static ResourceDemandReason ValidateSnapshot(const ResourceSnapshot& aSnapshot)
   {
      if (aSnapshot.schemaVersion != "nrm.snapshot.v2" ||
          !std::isfinite(aSnapshot.simTime))
         return ResourceDemandReason::cSNAPSHOT_INVALID;
      for (const NetworkSnapshot& network : aSnapshot.networks)
      {
         if (network.onlineCount > network.endpointCount)
            return ResourceDemandReason::cSNAPSHOT_INVALID;
      }
      return ResourceDemandReason::cNONE;
   }

   static bool HasPlatform(const ResourceSnapshot& aSnapshot,
                           const std::string& aPlatform)
   {
      return std::find_if(
                aSnapshot.endpoints.begin(), aSnapshot.endpoints.end(),
                [&aPlatform](const EndpointSnapshot& aEndpoint)
                {
                   return aEndpoint.platformName == aPlatform;
                }) != aSnapshot.endpoints.end();
   }

   static ResourceDemandReason ValidateDemand(const ResourceSnapshot& aSnapshot,
                                              const ResourceDemandSet& aDemandSet,
                                              const ResourceDemand& aDemand)
   {
      if (aDemand.schemaVersion != "nrm.resource_demand.v1")
         return ResourceDemandReason::cUNSUPPORTED_SCHEMA;
      if (!aDemand.valid)
         return ResourceDemandReason::cMISSING_REQUIRED_FIELD;
      if (aDemand.demandId.empty())
         return ResourceDemandReason::cINVALID_DEMAND_ID;
      if (aDemand.demandSetId != aDemandSet.demandSetId ||
          aDemand.revision != aDemandSet.revision)
         return ResourceDemandReason::cREFERENCE_NOT_FOUND;
      if (aDemand.missionStage.empty() || aDemand.businessType.empty() ||
          aDemand.sourcePlatform.empty() || aDemand.destinationPlatform.empty())
         return ResourceDemandReason::cMISSING_REQUIRED_FIELD;
      if (aDemand.sourcePlatform == aDemand.destinationPlatform)
         return ResourceDemandReason::cSOURCE_EQUALS_DESTINATION;

      std::set<NetworkType> networkTypes;
      for (NetworkType type : aDemand.allowedNetworks)
      {
         if (type == NetworkType::cUNKNOWN)
            return ResourceDemandReason::cUNKNOWN_NETWORK_TYPE;
         if (!networkTypes.insert(type).second)
            return ResourceDemandReason::cDUPLICATE_NETWORK_TYPE;
      }

      if (!std::isfinite(aDemand.businessTrafficBps) ||
          !std::isfinite(aDemand.requiredBandwidthBps) ||
          !std::isfinite(aDemand.maximumDelayMs) ||
          !std::isfinite(aDemand.minimumPdrPercent) ||
          !std::isfinite(aDemand.maximumDistanceM))
         return ResourceDemandReason::cNON_FINITE_VALUE;
      if (aDemand.businessTrafficBps < 0.0 ||
          aDemand.requiredBandwidthBps < 0.0 ||
          aDemand.maximumDelayMs < 0.0 || aDemand.maximumDistanceM < 0.0)
         return ResourceDemandReason::cNEGATIVE_VALUE;
      if (aDemand.minimumPdrPercent < 0.0 ||
          aDemand.minimumPdrPercent > 100.0)
         return ResourceDemandReason::cPDR_OUT_OF_RANGE;
      if (!HasPlatform(aSnapshot, aDemand.sourcePlatform) ||
          !HasPlatform(aSnapshot, aDemand.destinationPlatform))
         return ResourceDemandReason::cENDPOINT_NOT_FOUND;
      return ResourceDemandReason::cNONE;
   }

   static bool ValidatePlanEvidence(
      const ResourceSnapshot& aSnapshot,
      const NetworkPlanDocument* aPlanPtr,
      const NetworkPlanEvaluationResult* aEvaluationPtr)
   {
      if (aEvaluationPtr == nullptr)
         return true;
      if (aPlanPtr == nullptr)
         return false;

      const std::string fingerprint =
         network_plan_detail::PlanContentFingerprint(*aPlanPtr);
      return aEvaluationPtr->planId == aPlanPtr->planId &&
             aEvaluationPtr->revision == aPlanPtr->revision &&
             aEvaluationPtr->planFingerprint == fingerprint &&
             aEvaluationPtr->snapshotVersion == aSnapshot.snapshotVersion &&
             aEvaluationPtr->validation.planId == aPlanPtr->planId &&
             aEvaluationPtr->validation.revision == aPlanPtr->revision &&
             aEvaluationPtr->validation.planFingerprint == fingerprint;
   }

   static CapabilityRequest MapRequest(const ResourceDemand& aDemand)
   {
      CapabilityRequest request;
      request.requestId = aDemand.demandId;
      request.sourcePlatform = aDemand.sourcePlatform;
      request.destinationPlatform = aDemand.destinationPlatform;
      request.businessType = aDemand.businessType;
      request.payloadBits = aDemand.payloadBits;
      request.requiredBandwidthBps =
         std::max(aDemand.businessTrafficBps, aDemand.requiredBandwidthBps);
      request.maximumDelayMs = aDemand.maximumDelayMs;
      request.minimumPdrPercent = aDemand.minimumPdrPercent;
      request.allowedNetworks = aDemand.allowedNetworks;
      return request;
   }

   CapabilityResult QueryCapability(
      const ResourceSnapshot& aSnapshot,
      const CapabilityRequest& aRequest,
      const EnvironmentContext& aEnvironment) const
   {
      if (mCapabilityQueryPtr != nullptr)
         return mCapabilityQueryPtr->Query(aSnapshot, aRequest, aEnvironment);
      return mCapabilityService.Query(aSnapshot, aRequest, aEnvironment);
   }

   static void InitializeResult(const ResourceSnapshot& aSnapshot,
                                const ResourceDemandSet& aDemandSet,
                                const ResourceDemand& aDemand,
                                const NetworkPlanDocument* aPlanPtr,
                                ResourceDemandMatchResult& aResult)
   {
      aResult.demandId = aDemand.demandId;
      aResult.demandSetId = aDemandSet.demandSetId;
      aResult.demandSetRevision = aDemandSet.revision;
      aResult.snapshotVersion = aSnapshot.snapshotVersion;
      if (aPlanPtr != nullptr)
      {
         aResult.planId = aPlanPtr->planId;
         aResult.planRevision = aPlanPtr->revision;
         aResult.planFingerprint =
            network_plan_detail::PlanContentFingerprint(*aPlanPtr);
      }
   }

   static RequirementCheck NewCheck(RequirementItemType aType,
                                    bool aApplicable,
                                    const char* aUnit,
                                    const ResourceSnapshot& aSnapshot)
   {
      RequirementCheck check;
      check.type = aType;
      check.applicable = aApplicable;
      InitializeMetric(check.requiredValue, aUnit, aSnapshot);
      InitializeMetric(check.currentValue, aUnit, aSnapshot);
      InitializeMetric(check.margin, aUnit, aSnapshot);
      return check;
   }

   static void InitializeMetric(MetricValue<double>& aMetric,
                                const char* aUnit,
                                const ResourceSnapshot& aSnapshot)
   {
      aMetric.unit = aUnit;
      aMetric.sampleTime = std::isfinite(aSnapshot.simTime) ? aSnapshot.simTime : 0.0;
      aMetric.reason = MetricReason::cNO_SAMPLES;
   }

   static void SetMetric(MetricValue<double>& aMetric,
                         double aValue,
                         const char* aUnit,
                         const ResourceSnapshot& aSnapshot,
                         DataOrigin aOrigin,
                         Confidence aConfidence)
   {
      aMetric.value = aValue;
      aMetric.unit = aUnit;
      aMetric.valid = true;
      aMetric.origin = aOrigin;
      aMetric.confidence = aConfidence;
      aMetric.sampleTime = std::isfinite(aSnapshot.simTime) ? aSnapshot.simTime : 0.0;
      aMetric.reason = MetricReason::cNONE;
   }

   static Confidence MinimumConfidence(Confidence aLeft, Confidence aRight)
   {
      return static_cast<int>(aLeft) < static_cast<int>(aRight) ? aLeft : aRight;
   }

   static void SetRequired(RequirementCheck& aCheck,
                           double aValue,
                           const ResourceDemand& aDemand,
                           const ResourceSnapshot& aSnapshot)
   {
      const std::string unit = aCheck.requiredValue.unit;
      SetMetric(aCheck.requiredValue, aValue, unit.c_str(),
                aSnapshot, aDemand.source, aDemand.confidence);
   }

   static void SetCurrentAndMargin(RequirementCheck& aCheck,
                                   double aCurrent,
                                   double aMargin,
                                   DataOrigin aOrigin,
                                   Confidence aConfidence,
                                   const ResourceSnapshot& aSnapshot)
   {
      const std::string unit = aCheck.currentValue.unit;
      SetMetric(aCheck.currentValue, aCurrent, unit.c_str(), aSnapshot,
                aOrigin, aConfidence);
      SetMetric(aCheck.margin, aMargin, unit.c_str(), aSnapshot,
                DataOrigin::cDERIVED,
                MinimumConfidence(aCheck.requiredValue.confidence, aConfidence));
   }

   static std::vector<RequirementCheck> InitializeChecks(
      const ResourceSnapshot& aSnapshot,
      const ResourceDemand& aDemand)
   {
      std::vector<RequirementCheck> checks;
      checks.push_back(NewCheck(RequirementItemType::cPATH, true, "boolean", aSnapshot));
      checks.push_back(NewCheck(RequirementItemType::cNETWORK_SIZE,
                                aDemand.minimumNetworkSize > 0, "member", aSnapshot));
      checks.push_back(NewCheck(RequirementItemType::cDISTANCE,
                                std::isfinite(aDemand.maximumDistanceM) &&
                                   aDemand.maximumDistanceM > 0.0,
                                "m", aSnapshot));
      checks.push_back(NewCheck(RequirementItemType::cBANDWIDTH,
                                std::isfinite(aDemand.requiredBandwidthBps) &&
                                   aDemand.requiredBandwidthBps > 0.0,
                                "bit/s", aSnapshot));
      checks.push_back(NewCheck(RequirementItemType::cTRAFFIC,
                                std::isfinite(aDemand.businessTrafficBps) &&
                                   aDemand.businessTrafficBps > 0.0,
                                "bit/s", aSnapshot));
      checks.push_back(NewCheck(RequirementItemType::cDELAY,
                                std::isfinite(aDemand.maximumDelayMs) &&
                                   aDemand.maximumDelayMs > 0.0,
                                "ms", aSnapshot));
      checks.push_back(NewCheck(RequirementItemType::cPDR,
                                std::isfinite(aDemand.minimumPdrPercent) &&
                                   aDemand.minimumPdrPercent > 0.0,
                                "percent", aSnapshot));
      checks.push_back(NewCheck(RequirementItemType::cBUSINESS_TYPE,
                                true, "boolean", aSnapshot));
      return checks;
   }

   static double RequiredNumeric(const ResourceDemand& aDemand,
                                 RequirementItemType aType)
   {
      switch (aType)
      {
      case RequirementItemType::cPATH: return 1.0;
      case RequirementItemType::cNETWORK_SIZE:
         return static_cast<double>(aDemand.minimumNetworkSize);
      case RequirementItemType::cDISTANCE: return aDemand.maximumDistanceM;
      case RequirementItemType::cBANDWIDTH: return aDemand.requiredBandwidthBps;
      case RequirementItemType::cTRAFFIC: return aDemand.businessTrafficBps;
      case RequirementItemType::cDELAY: return aDemand.maximumDelayMs;
      case RequirementItemType::cPDR: return aDemand.minimumPdrPercent;
      case RequirementItemType::cBUSINESS_TYPE: return 1.0;
      }
      return 0.0;
   }

   static void PopulateInvalidChecks(const ResourceSnapshot& aSnapshot,
                                     const ResourceDemand& aDemand,
                                     ResourceDemandReason aReason,
                                     ResourceDemandMatchResult& aResult)
   {
      aResult.checks = InitializeChecks(aSnapshot, aDemand);
      for (RequirementCheck& check : aResult.checks)
      {
         if (!check.applicable)
            continue;
         const double required = RequiredNumeric(aDemand, check.type);
         if (std::isfinite(required) && required >= 0.0)
            SetRequired(check, required, aDemand, aSnapshot);
         check.reason = aReason;
         check.currentValue.reason = MetricReason::cINVALID_INPUT;
         check.margin.reason = MetricReason::cINVALID_INPUT;
      }
   }

   static bool HasCapabilityReason(const CapabilityResult& aCapability,
                                   CapabilityReason aReason)
   {
      return std::find(aCapability.reasons.begin(), aCapability.reasons.end(), aReason) !=
             aCapability.reasons.end();
   }

   static ResourceDemandReason PathInvalidReason(const CapabilityResult& aCapability)
   {
      if (!aCapability.requestValid)
         return ResourceDemandReason::cCAPABILITY_REQUEST_INVALID;
      if (HasCapabilityReason(aCapability, CapabilityReason::cPROFILE_CONFIG_INVALID))
         return ResourceDemandReason::cPROFILE_CONFIG_INVALID;
      if (HasCapabilityReason(aCapability, CapabilityReason::cNON_FINITE_INPUT) ||
          HasCapabilityReason(aCapability, CapabilityReason::cNODE_NOT_FOUND))
         return ResourceDemandReason::cMETRIC_UNAVAILABLE;
      return ResourceDemandReason::cNONE;
   }

   static void EvaluatePath(const ResourceSnapshot& aSnapshot,
                            const ResourceDemand& aDemand,
                            const CapabilityResult& aCapability,
                            RequirementCheck& aCheck)
   {
      SetRequired(aCheck, 1.0, aDemand, aSnapshot);
      const ResourceDemandReason invalidReason = PathInvalidReason(aCapability);
      if (invalidReason != ResourceDemandReason::cNONE)
      {
         aCheck.reason = invalidReason;
         aCheck.currentValue.reason = MetricReason::cINVALID_INPUT;
         aCheck.margin.reason = MetricReason::cINVALID_INPUT;
         return;
      }
      const double current = aCapability.pathAvailable ? 1.0 : 0.0;
      SetCurrentAndMargin(aCheck, current, current - 1.0,
                          DataOrigin::cDERIVED, Confidence::cHIGH, aSnapshot);
      aCheck.passed = aCapability.pathAvailable;
      aCheck.reason = aCheck.passed ? ResourceDemandReason::cNONE
                                    : ResourceDemandReason::cPATH_UNAVAILABLE;
   }

   static std::size_t OnlineNetworkSize(const ResourceSnapshot& aSnapshot,
                                        const ResourceDemand& aDemand)
   {
      std::set<std::string> onlinePlatforms;
      for (const EndpointSnapshot& endpoint : aSnapshot.endpoints)
      {
         if (endpoint.state != ResourceState::cONLINE || endpoint.platformName.empty())
            continue;
         if (!aDemand.allowedNetworks.empty() &&
             std::find(aDemand.allowedNetworks.begin(), aDemand.allowedNetworks.end(),
                       endpoint.networkType) == aDemand.allowedNetworks.end())
            continue;
         onlinePlatforms.insert(endpoint.platformName);
      }
      return onlinePlatforms.size();
   }

   static void EvaluateLowerBound(const ResourceSnapshot& aSnapshot,
                                  const ResourceDemand& aDemand,
                                  double aRequired,
                                  const MetricValue<double>& aCurrent,
                                  ResourceDemandReason aFailureReason,
                                  RequirementCheck& aCheck)
   {
      SetRequired(aCheck, aRequired, aDemand, aSnapshot);
      if (!aCurrent.valid || !std::isfinite(aCurrent.value) || aCurrent.value < 0.0)
      {
         aCheck.reason = ResourceDemandReason::cMETRIC_UNAVAILABLE;
         aCheck.currentValue.reason = aCurrent.reason;
         aCheck.margin.reason = aCurrent.reason;
         return;
      }
      SetCurrentAndMargin(aCheck, aCurrent.value, aCurrent.value - aRequired,
                          aCurrent.origin, aCurrent.confidence, aSnapshot);
      aCheck.passed = aCurrent.value >= aRequired;
      aCheck.reason = aCheck.passed ? ResourceDemandReason::cNONE : aFailureReason;
   }

   static void EvaluateUpperBound(const ResourceSnapshot& aSnapshot,
                                  const ResourceDemand& aDemand,
                                  double aRequired,
                                  const MetricValue<double>& aCurrent,
                                  ResourceDemandReason aFailureReason,
                                  RequirementCheck& aCheck)
   {
      SetRequired(aCheck, aRequired, aDemand, aSnapshot);
      if (!aCurrent.valid || !std::isfinite(aCurrent.value) || aCurrent.value < 0.0)
      {
         aCheck.reason = ResourceDemandReason::cMETRIC_UNAVAILABLE;
         aCheck.currentValue.reason = aCurrent.reason;
         aCheck.margin.reason = aCurrent.reason;
         return;
      }
      SetCurrentAndMargin(aCheck, aCurrent.value, aRequired - aCurrent.value,
                          aCurrent.origin, aCurrent.confidence, aSnapshot);
      aCheck.passed = aCurrent.value <= aRequired;
      aCheck.reason = aCheck.passed ? ResourceDemandReason::cNONE : aFailureReason;
   }

   void EvaluateBusiness(const ResourceSnapshot& aSnapshot,
                         const ResourceDemand& aDemand,
                         const CapabilityResult& aCapability,
                         RequirementCheck& aCheck) const
   {
      SetRequired(aCheck, 1.0, aDemand, aSnapshot);
      aCheck.requiredText = aDemand.businessType;
      if (!mProfiles.Valid())
      {
         aCheck.reason = ResourceDemandReason::cPROFILE_CONFIG_INVALID;
         aCheck.currentValue.reason = MetricReason::cINVALID_INPUT;
         aCheck.margin.reason = MetricReason::cINVALID_INPUT;
         return;
      }

      std::vector<NetworkType> types = aCapability.networkSequence;
      if (types.empty())
         types = aDemand.allowedNetworks;

      const auto typeSupport = [this, &aDemand](NetworkType aType,
                                               bool& aHasProfile)
      {
         bool supported = false;
         aHasProfile = false;
         for (const NetworkProfile& profile : mProfiles.Profiles())
         {
            if (!profile.valid || profile.networkType != aType)
               continue;
            aHasProfile = true;
            supported = supported || NetworkProfileRepository::SupportsBusiness(
                                        profile, aDemand.businessType);
         }
         return supported;
      };

      bool hasProfile = false;
      bool supported = false;
      if (!aCapability.networkSequence.empty())
      {
         supported = true;
         std::set<NetworkType> uniqueTypes(aCapability.networkSequence.begin(),
                                           aCapability.networkSequence.end());
         for (NetworkType type : uniqueTypes)
         {
            bool typeHasProfile = false;
            const bool typeSupported = typeSupport(type, typeHasProfile);
            if (!typeHasProfile)
            {
               hasProfile = false;
               supported = false;
               break;
            }
            hasProfile = true;
            supported = supported && typeSupported;
         }
      }
      else if (types.empty())
      {
         for (const NetworkProfile& profile : mProfiles.Profiles())
         {
            if (!profile.valid)
               continue;
            hasProfile = true;
            supported = supported || NetworkProfileRepository::SupportsBusiness(
                                        profile, aDemand.businessType);
         }
      }
      else
      {
         std::set<NetworkType> uniqueTypes(types.begin(), types.end());
         for (NetworkType type : uniqueTypes)
         {
            bool typeHasProfile = false;
            const bool typeSupported = typeSupport(type, typeHasProfile);
            hasProfile = hasProfile || typeHasProfile;
            supported = supported || typeSupported;
         }
      }

      if (!hasProfile)
      {
         aCheck.reason = ResourceDemandReason::cPROFILE_CONFIG_INVALID;
         aCheck.currentValue.reason = MetricReason::cINVALID_INPUT;
         aCheck.margin.reason = MetricReason::cINVALID_INPUT;
         return;
      }
      SetCurrentAndMargin(aCheck, supported ? 1.0 : 0.0,
                          supported ? 0.0 : -1.0,
                          DataOrigin::cDERIVED, Confidence::cHIGH, aSnapshot);
      aCheck.currentText = supported ? aDemand.businessType : std::string();
      aCheck.passed = supported;
      aCheck.reason = supported ? ResourceDemandReason::cNONE
                                : ResourceDemandReason::cBUSINESS_TYPE_NOT_SUPPORTED;
   }

   void PopulateChecks(const ResourceSnapshot& aSnapshot,
                       const ResourceDemand& aDemand,
                       ResourceDemandMatchResult& aResult) const
   {
      aResult.checks = InitializeChecks(aSnapshot, aDemand);
      EvaluatePath(aSnapshot, aDemand, aResult.capability, aResult.checks[0]);

      if (aResult.checks[1].applicable)
      {
         SetRequired(aResult.checks[1],
                     static_cast<double>(aDemand.minimumNetworkSize),
                     aDemand, aSnapshot);
         const double current = static_cast<double>(OnlineNetworkSize(aSnapshot, aDemand));
         SetCurrentAndMargin(aResult.checks[1], current,
                             current - static_cast<double>(aDemand.minimumNetworkSize),
                             DataOrigin::cDERIVED, Confidence::cHIGH, aSnapshot);
         aResult.checks[1].passed = current >= aDemand.minimumNetworkSize;
         aResult.checks[1].reason =
            aResult.checks[1].passed ? ResourceDemandReason::cNONE
                                     : ResourceDemandReason::cNETWORK_SIZE_NOT_MET;
      }
      if (aResult.checks[2].applicable)
         EvaluateUpperBound(aSnapshot, aDemand, aDemand.maximumDistanceM,
                            aResult.capability.communicationDistanceM,
                            ResourceDemandReason::cDISTANCE_NOT_MET, aResult.checks[2]);
      if (aResult.checks[3].applicable)
         EvaluateLowerBound(aSnapshot, aDemand, aDemand.requiredBandwidthBps,
                            aResult.capability.transmissionRateBps,
                            ResourceDemandReason::cBANDWIDTH_NOT_MET, aResult.checks[3]);
      if (aResult.checks[4].applicable)
         EvaluateLowerBound(aSnapshot, aDemand, aDemand.businessTrafficBps,
                            aResult.capability.transmissionRateBps,
                            ResourceDemandReason::cTRAFFIC_NOT_MET, aResult.checks[4]);
      if (aResult.checks[5].applicable)
         EvaluateUpperBound(aSnapshot, aDemand, aDemand.maximumDelayMs,
                            aResult.capability.transmissionDelayMs,
                            ResourceDemandReason::cDELAY_NOT_MET, aResult.checks[5]);
      if (aResult.checks[6].applicable)
      {
         MetricValue<double> pdr = aResult.capability.packetLossPercent;
         if (pdr.valid && std::isfinite(pdr.value) && pdr.value >= 0.0 &&
             pdr.value <= 100.0)
            pdr.value = 100.0 - pdr.value;
         else
            pdr.valid = false;
         EvaluateLowerBound(aSnapshot, aDemand, aDemand.minimumPdrPercent, pdr,
                            ResourceDemandReason::cPDR_NOT_MET, aResult.checks[6]);
      }
      EvaluateBusiness(aSnapshot, aDemand, aResult.capability, aResult.checks[7]);
   }

   static void AddReason(ResourceDemandMatchResult& aResult,
                         ResourceDemandReason aReason)
   {
      if (aReason != ResourceDemandReason::cNONE &&
          std::find(aResult.reasons.begin(), aResult.reasons.end(), aReason) ==
             aResult.reasons.end())
         aResult.reasons.push_back(aReason);
   }

   static void FinalizeStatus(ResourceDemandMatchResult& aResult)
   {
      bool invalid = false;
      bool failed = false;
      for (const RequirementCheck& check : aResult.checks)
      {
         if (!check.applicable)
            continue;
         if (!check.currentValue.valid || !check.margin.valid)
            invalid = true;
         else if (!check.passed)
            failed = true;
         AddReason(aResult, check.reason);
      }
      if (invalid)
         aResult.status = DemandMatchStatus::cDATA_INVALID;
      else if (failed)
         aResult.status = DemandMatchStatus::cUNSATISFIED;
      else
         aResult.status = DemandMatchStatus::cSATISFIED;
   }

   static void Count(DemandMatchStatus aStatus, ResourceDemandBatchResult& aBatch)
   {
      if (aStatus == DemandMatchStatus::cSATISFIED)
         ++aBatch.satisfiedCount;
      else if (aStatus == DemandMatchStatus::cUNSATISFIED)
         ++aBatch.unsatisfiedCount;
      else
         ++aBatch.dataInvalidCount;
   }

   NetworkProfileRepository mProfiles;
   CommunicationCapabilityService mCapabilityService;
   const ResourceDemandCapabilityQuery* mCapabilityQueryPtr = nullptr;
};
} // namespace nrm

#endif
