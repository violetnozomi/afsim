#ifndef NRM_PLANNING_RECOMMENDATION_ENGINE_HPP
#define NRM_PLANNING_RECOMMENDATION_ENGINE_HPP

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "nrm/NetworkPlanRepository.hpp"
#include "nrm/NetworkProfileRepository.hpp"
#include "nrm/ResourceDemandTypes.hpp"
#include "nrm/OperationalEnvironmentEvaluator.hpp"

namespace nrm
{
class PlanningRecommendationEngine
{
public:
   explicit PlanningRecommendationEngine(const NetworkProfileRepository& aProfiles)
      : mProfiles(aProfiles)
   {
   }

   std::vector<PlanningRecommendation> Evaluate(
      const ResourceSnapshot& aSnapshot,
      const ResourceDemand& aDemand,
      const ResourceDemandMatchResult& aMatch,
      const NetworkPlanDocument* aPlanPtr = nullptr,
      const PlanningCandidateSet* aCandidatesPtr = nullptr) const
   {
      std::vector<PlanningRecommendation> recommendations;
      recommendations.reserve(6);
      recommendations.push_back(NewRecommendation(RecommendationType::cFREQUENCY, aMatch));
      recommendations.push_back(NewRecommendation(RecommendationType::cSTATION, aMatch));
      recommendations.push_back(NewRecommendation(RecommendationType::cCHANNEL, aMatch));
      recommendations.push_back(NewRecommendation(RecommendationType::cSUBNET, aMatch));
      recommendations.push_back(NewRecommendation(RecommendationType::cTIMESLOT, aMatch));
      recommendations.push_back(NewRecommendation(RecommendationType::cROUTE, aMatch));

      if (HasMatchReason(aMatch, ResourceDemandReason::cPLAN_EVIDENCE_MISMATCH) ||
          !EvidenceIdentityValid(aSnapshot, aMatch, aPlanPtr))
      {
         SetAllUnavailable(recommendations,
                           ResourceDemandReason::cPLAN_EVIDENCE_MISMATCH);
         return recommendations;
      }
      if (aMatch.status == DemandMatchStatus::cDATA_INVALID)
      {
         SetAllUnavailable(recommendations, ResourceDemandReason::cMETRIC_UNAVAILABLE);
         return recommendations;
      }

      if (aCandidatesPtr == nullptr && aPlanPtr != nullptr)
      {
         // A separately supplied candidate set has higher authority. When it
         // is absent, expose only resources explicitly present in the loaded
         // plan as low-confidence suggestions; never invent an alternative.
         RecommendFromPlan(aDemand, *aPlanPtr, recommendations);
      }
      else
      {
         RecommendFrequency(aPlanPtr, aCandidatesPtr, recommendations[0]);
         RecommendStation(aCandidatesPtr, recommendations[1]);
         RecommendUnoccupied(RecommendationType::cCHANNEL, aCandidatesPtr,
                             recommendations[2]);
         RecommendSubnet(aDemand, aCandidatesPtr, recommendations[3]);
         RecommendUnoccupied(RecommendationType::cTIMESLOT, aCandidatesPtr,
                             recommendations[4]);
      }
      RecommendRoute(aMatch, recommendations[5]);
      return recommendations;
   }

private:
   static bool HasMatchReason(const ResourceDemandMatchResult& aMatch,
                              ResourceDemandReason aReason)
   {
      return std::find(aMatch.reasons.begin(), aMatch.reasons.end(), aReason) !=
             aMatch.reasons.end();
   }

   static PlanningRecommendation NewRecommendation(
      RecommendationType aType,
      const ResourceDemandMatchResult& aMatch)
   {
      PlanningRecommendation recommendation;
      recommendation.type = aType;
      recommendation.demandId = aMatch.demandId;
      recommendation.snapshotVersion = aMatch.snapshotVersion;
      recommendation.planId = aMatch.planId;
      recommendation.planRevision = aMatch.planRevision;
      recommendation.planFingerprint = aMatch.planFingerprint;
      return recommendation;
   }

   static bool EvidenceIdentityValid(const ResourceSnapshot& aSnapshot,
                                     const ResourceDemandMatchResult& aMatch,
                                     const NetworkPlanDocument* aPlanPtr)
   {
      if (aMatch.snapshotVersion != aSnapshot.snapshotVersion)
         return false;
      if (aMatch.planId.empty())
         return aPlanPtr == nullptr;
      if (aPlanPtr == nullptr)
         return false;
      return aMatch.planId == aPlanPtr->planId &&
             aMatch.planRevision == aPlanPtr->revision &&
             aMatch.planFingerprint ==
                network_plan_detail::PlanContentFingerprint(*aPlanPtr);
   }

   static void SetAllUnavailable(
      std::vector<PlanningRecommendation>& aRecommendations,
      ResourceDemandReason aReason)
   {
      for (PlanningRecommendation& recommendation : aRecommendations)
         recommendation.reason = aReason;
   }

   static std::vector<const PlanningResourceCandidate*> CandidatesOfType(
      RecommendationType aType,
      const PlanningCandidateSet* aCandidatesPtr)
   {
      std::vector<const PlanningResourceCandidate*> candidates;
      if (aCandidatesPtr == nullptr)
         return candidates;
      for (const PlanningResourceCandidate& candidate : aCandidatesPtr->candidates)
      {
         if (candidate.type == aType)
            candidates.push_back(&candidate);
      }
      return candidates;
   }

   static const NetworkPlanAllocation* FindAllocation(
      const NetworkPlanDocument& aPlan,
      const std::string& aAllocationId)
   {
      for (const NetworkPlanAllocation& allocation : aPlan.allocations)
      {
         if (allocation.allocationId == aAllocationId)
            return &allocation;
      }
      return nullptr;
   }

   const NetworkProfile* FindProfile(const NetworkPlanAllocation& aAllocation) const
   {
      for (const NetworkProfile& profile : mProfiles.Profiles())
      {
         if (profile.valid && profile.profileId == aAllocation.profileId &&
             profile.networkType == aAllocation.networkType)
            return &profile;
      }
      return nullptr;
   }

   static bool FrequencySupported(const NetworkProfile& aProfile, double aFrequencyHz)
   {
      return std::find(aProfile.frequenciesHz.begin(), aProfile.frequenciesHz.end(),
                       aFrequencyHz) != aProfile.frequenciesHz.end();
   }

   static std::string Number(double aValue)
   {
      std::ostringstream output;
      output << std::setprecision(17) << aValue;
      return output.str();
   }

   static bool NetworkAllowed(const ResourceDemand& aDemand,
                              NetworkType aNetworkType)
   {
      return aDemand.allowedNetworks.empty() ||
             std::find(aDemand.allowedNetworks.begin(),
                       aDemand.allowedNetworks.end(), aNetworkType) !=
                aDemand.allowedNetworks.end();
   }

   static bool ContainsMember(const NetworkPlanAllocation& aAllocation,
                              const std::string& aPlatformId)
   {
      return std::find(aAllocation.memberPlatformIds.begin(),
                       aAllocation.memberPlatformIds.end(), aPlatformId) !=
             aAllocation.memberPlatformIds.end();
   }

   const NetworkPlanAllocation* FindPlanAllocation(
      const ResourceDemand& aDemand,
      const NetworkPlanDocument& aPlan) const
   {
      const NetworkPlanAllocation* selected = nullptr;
      int selectedScore = -1;
      for (const NetworkPlanAllocation& allocation : aPlan.allocations)
      {
         if (!allocation.enabled || !NetworkAllowed(aDemand, allocation.networkType))
            continue;
         const NetworkProfile* profile = FindProfile(allocation);
         if (profile == nullptr ||
             !NetworkProfileRepository::SupportsBusiness(*profile,
                                                         aDemand.businessType))
            continue;
         const int score =
            (ContainsMember(allocation, aDemand.sourcePlatform) ? 1 : 0) +
            (ContainsMember(allocation, aDemand.destinationPlatform) ? 1 : 0);
         if (selected == nullptr || score > selectedScore ||
             (score == selectedScore &&
              allocation.allocationId < selected->allocationId))
         {
            selected = &allocation;
            selectedScore = score;
         }
      }
      return selected;
   }

   static void SelectPlanValue(RecommendationType aType,
                               const std::string& aValue,
                               const NetworkPlanDocument& aPlan,
                               const NetworkPlanAllocation& aAllocation,
                               const NetworkProfile& aProfile,
                               PlanningRecommendation& aRecommendation)
   {
      aRecommendation.status = RecommendationStatus::cAVAILABLE;
      aRecommendation.candidateId =
         "plan:" + aAllocation.allocationId + ":" + ToString(aType);
      aRecommendation.value = aValue;
      aRecommendation.rank = 1;
      aRecommendation.reason = ResourceDemandReason::cNONE;
      aRecommendation.source = aPlan.source;
      // This is a documented plan-derived fallback, not a measured free-resource
      // scan. Keep confidence low even when the source document says otherwise.
      aRecommendation.confidence = Confidence::cLOW;
      aRecommendation.evidence.push_back("candidateSource=LOADED_PLAN");
      aRecommendation.evidence.push_back("allocationId=" +
                                         aAllocation.allocationId);
      aRecommendation.evidence.push_back("profileId=" + aProfile.profileId);
   }

   void RecommendFromPlan(
      const ResourceDemand& aDemand,
      const NetworkPlanDocument& aPlan,
      std::vector<PlanningRecommendation>& aRecommendations) const
   {
      if (!mProfiles.Valid())
      {
         for (std::size_t index : {std::size_t(0), std::size_t(2),
                                   std::size_t(3), std::size_t(4)})
            aRecommendations[index].reason =
               ResourceDemandReason::cPROFILE_CONFIG_INVALID;
         aRecommendations[1].reason =
            ResourceDemandReason::cCANDIDATE_DATA_UNAVAILABLE;
         return;
      }
      const NetworkPlanAllocation* allocation =
         FindPlanAllocation(aDemand, aPlan);
      const NetworkProfile* profile =
         allocation == nullptr ? nullptr : FindProfile(*allocation);
      if (allocation == nullptr || profile == nullptr)
      {
         for (std::size_t index = 0; index < 5; ++index)
            aRecommendations[index].reason =
               ResourceDemandReason::cCANDIDATE_DATA_UNAVAILABLE;
         return;
      }

      if (std::isfinite(allocation->frequencyHz) &&
          allocation->frequencyHz > 0.0 &&
          FrequencySupported(*profile, allocation->frequencyHz))
      {
         SelectPlanValue(RecommendationType::cFREQUENCY,
                         Number(allocation->frequencyHz), aPlan, *allocation,
                         *profile, aRecommendations[0]);
      }
      else
      {
         aRecommendations[0].reason =
            ResourceDemandReason::cPROFILE_CONFIG_INVALID;
      }

      // A station recommendation requires an evaluated alternative path, which
      // the plan document alone cannot prove.
      aRecommendations[1].reason =
         ResourceDemandReason::cCANDIDATE_DATA_UNAVAILABLE;

      if (!allocation->channelId.empty())
         SelectPlanValue(RecommendationType::cCHANNEL, allocation->channelId,
                         aPlan, *allocation, *profile, aRecommendations[2]);
      if (!allocation->subnetId.empty())
         SelectPlanValue(RecommendationType::cSUBNET, allocation->subnetId,
                         aPlan, *allocation, *profile, aRecommendations[3]);
      if (!allocation->slotIds.empty() && !allocation->slotIds.front().empty())
         SelectPlanValue(RecommendationType::cTIMESLOT,
                         allocation->slotIds.front(), aPlan, *allocation,
                         *profile, aRecommendations[4]);
   }

   static void Select(const PlanningResourceCandidate& aCandidate,
                      const std::string& aValue,
                      PlanningRecommendation& aRecommendation)
   {
      aRecommendation.status = RecommendationStatus::cAVAILABLE;
      aRecommendation.candidateId = aCandidate.candidateId;
      aRecommendation.value = aValue;
      aRecommendation.rank = 1;
      aRecommendation.reason = ResourceDemandReason::cNONE;
      aRecommendation.source = aCandidate.source;
      aRecommendation.confidence = aCandidate.confidence;
   }

   void RecommendFrequency(const NetworkPlanDocument* aPlanPtr,
                           const PlanningCandidateSet* aCandidatesPtr,
                           PlanningRecommendation& aRecommendation) const
   {
      std::vector<const PlanningResourceCandidate*> candidates =
         CandidatesOfType(RecommendationType::cFREQUENCY, aCandidatesPtr);
      if (candidates.empty())
      {
         aRecommendation.reason = ResourceDemandReason::cCANDIDATE_DATA_UNAVAILABLE;
         return;
      }
      if (aPlanPtr == nullptr)
      {
         aRecommendation.reason = ResourceDemandReason::cCUSTOMER_RULE_UNAVAILABLE;
         return;
      }
      if (!mProfiles.Valid())
      {
         aRecommendation.reason = ResourceDemandReason::cPROFILE_CONFIG_INVALID;
         return;
      }

      std::sort(candidates.begin(), candidates.end(),
                [](const PlanningResourceCandidate* aLeft,
                   const PlanningResourceCandidate* aRight)
                {
                   const bool leftFinite = std::isfinite(aLeft->frequencyHz);
                   const bool rightFinite = std::isfinite(aRight->frequencyHz);
                   if (leftFinite != rightFinite) return leftFinite;
                   if (leftFinite && aLeft->frequencyHz != aRight->frequencyHz)
                      return aLeft->frequencyHz < aRight->frequencyHz;
                   return aLeft->candidateId < aRight->candidateId;
                });
      bool sawConflict = false;
      bool sawInterference = false;
      bool sawProfileFailure = false;
      for (const PlanningResourceCandidate* candidate : candidates)
      {
         if (!candidate->valid || candidate->candidateId.empty() ||
             candidate->allocationId.empty() ||
             !std::isfinite(candidate->frequencyHz) || candidate->frequencyHz <= 0.0)
            continue;
         if (candidate->occupied)
         {
            sawConflict = true;
            continue;
         }
         if (candidate->interferenceCenterHz > 0.0 &&
             OperationalEnvironmentEvaluator::BandsOverlap(
                candidate->frequencyHz, candidate->protectionBandwidthHz,
                candidate->interferenceCenterHz,
                candidate->interferenceBandwidthHz))
         {
            sawInterference = true;
            continue;
         }
         const NetworkPlanAllocation* allocation =
            FindAllocation(*aPlanPtr, candidate->allocationId);
         const NetworkProfile* profile =
            allocation == nullptr ? nullptr : FindProfile(*allocation);
         if (allocation == nullptr || !allocation->enabled || profile == nullptr ||
             !FrequencySupported(*profile, candidate->frequencyHz))
         {
            sawProfileFailure = true;
            continue;
         }
         Select(*candidate,
                candidate->value.empty() ? Number(candidate->frequencyHz)
                                         : candidate->value,
                aRecommendation);
         aRecommendation.evidence.push_back("allocationId=" + candidate->allocationId);
         aRecommendation.evidence.push_back("profileId=" + profile->profileId);
         aRecommendation.evidence.push_back("frequencyHz=" +
                                            Number(candidate->frequencyHz));
         if (candidate->interferenceCenterHz > 0.0)
         {
            const double margin = std::abs(candidate->frequencyHz -
                                           candidate->interferenceCenterHz) -
               (candidate->protectionBandwidthHz +
                candidate->interferenceBandwidthHz) / 2.0;
            aRecommendation.evidence.push_back(
               "interferenceNonOverlapMarginHz=" + Number(margin));
         }
         return;
      }
      if (sawProfileFailure)
         aRecommendation.reason = ResourceDemandReason::cPROFILE_CONFIG_INVALID;
      else if (sawInterference)
         aRecommendation.reason = ResourceDemandReason::cINTERFERENCE_CONFLICT;
      else if (sawConflict)
         aRecommendation.reason = ResourceDemandReason::cCANDIDATE_CONFLICT;
      else
         aRecommendation.reason = ResourceDemandReason::cNO_FEASIBLE_CANDIDATE;
   }

   static int ProjectedStatusRank(DemandMatchStatus aStatus)
   {
      if (aStatus == DemandMatchStatus::cSATISFIED) return 0;
      if (aStatus == DemandMatchStatus::cUNSATISFIED) return 1;
      return 2;
   }

   static void RecommendStation(const PlanningCandidateSet* aCandidatesPtr,
                                PlanningRecommendation& aRecommendation)
   {
      std::vector<const PlanningResourceCandidate*> candidates =
         CandidatesOfType(RecommendationType::cSTATION, aCandidatesPtr);
      if (candidates.empty())
      {
         aRecommendation.reason = ResourceDemandReason::cCANDIDATE_DATA_UNAVAILABLE;
         return;
      }
      std::sort(candidates.begin(), candidates.end(),
                [](const PlanningResourceCandidate* aLeft,
                   const PlanningResourceCandidate* aRight)
                {
                   const int leftRank = ProjectedStatusRank(aLeft->projectedStatus);
                   const int rightRank = ProjectedStatusRank(aRight->projectedStatus);
                   if (leftRank != rightRank) return leftRank < rightRank;
                   const double leftMargin =
                      aLeft->minimumMargin.valid &&
                            std::isfinite(aLeft->minimumMargin.value)
                         ? aLeft->minimumMargin.value
                         : -1.0e300;
                   const double rightMargin =
                      aRight->minimumMargin.valid &&
                            std::isfinite(aRight->minimumMargin.value)
                         ? aRight->minimumMargin.value
                         : -1.0e300;
                   if (leftMargin != rightMargin) return leftMargin > rightMargin;
                   return aLeft->candidateId < aRight->candidateId;
                });
      for (const PlanningResourceCandidate* candidate : candidates)
      {
         if (!candidate->valid || candidate->candidateId.empty() ||
             candidate->platformId.empty() || !candidate->pathAvailable ||
             candidate->projectedStatus != DemandMatchStatus::cSATISFIED ||
             !candidate->minimumMargin.valid ||
             !std::isfinite(candidate->minimumMargin.value) ||
             candidate->minimumMargin.value < 0.0)
            continue;
         Select(*candidate,
                candidate->value.empty() ? candidate->platformId : candidate->value,
                aRecommendation);
         aRecommendation.evidence.push_back("platformId=" + candidate->platformId);
         aRecommendation.evidence.push_back("projectedStatus=SATISFIED");
         aRecommendation.evidence.push_back("minimumMargin=" +
                                            Number(candidate->minimumMargin.value));
         return;
      }
      aRecommendation.reason = ResourceDemandReason::cNO_FEASIBLE_CANDIDATE;
   }

   static void RecommendUnoccupied(
      RecommendationType aType,
      const PlanningCandidateSet* aCandidatesPtr,
      PlanningRecommendation& aRecommendation)
   {
      std::vector<const PlanningResourceCandidate*> candidates =
         CandidatesOfType(aType, aCandidatesPtr);
      if (candidates.empty())
      {
         aRecommendation.reason = ResourceDemandReason::cCANDIDATE_DATA_UNAVAILABLE;
         return;
      }
      std::sort(candidates.begin(), candidates.end(),
                [](const PlanningResourceCandidate* aLeft,
                   const PlanningResourceCandidate* aRight)
                {
                   return aLeft->candidateId < aRight->candidateId;
                });
      bool sawConflict = false;
      for (const PlanningResourceCandidate* candidate : candidates)
      {
         if (!candidate->valid || candidate->candidateId.empty() ||
             candidate->value.empty())
            continue;
         if (candidate->occupied)
         {
            sawConflict = true;
            continue;
         }
         Select(*candidate, candidate->value, aRecommendation);
         aRecommendation.evidence.push_back("occupied=false");
         return;
      }
      aRecommendation.reason = sawConflict
                                  ? ResourceDemandReason::cCANDIDATE_CONFLICT
                                  : ResourceDemandReason::cNO_FEASIBLE_CANDIDATE;
   }

   static bool SupportsBusiness(const PlanningResourceCandidate& aCandidate,
                                const std::string& aBusinessType)
   {
      return std::find(aCandidate.supportedBusinessTypes.begin(),
                       aCandidate.supportedBusinessTypes.end(), "*") !=
                aCandidate.supportedBusinessTypes.end() ||
             std::find(aCandidate.supportedBusinessTypes.begin(),
                       aCandidate.supportedBusinessTypes.end(), aBusinessType) !=
                aCandidate.supportedBusinessTypes.end();
   }

   static void RecommendSubnet(const ResourceDemand& aDemand,
                               const PlanningCandidateSet* aCandidatesPtr,
                               PlanningRecommendation& aRecommendation)
   {
      std::vector<const PlanningResourceCandidate*> candidates =
         CandidatesOfType(RecommendationType::cSUBNET, aCandidatesPtr);
      if (candidates.empty())
      {
         aRecommendation.reason = ResourceDemandReason::cCANDIDATE_DATA_UNAVAILABLE;
         return;
      }
      std::sort(candidates.begin(), candidates.end(),
                [](const PlanningResourceCandidate* aLeft,
                   const PlanningResourceCandidate* aRight)
                {
                   return aLeft->candidateId < aRight->candidateId;
                });
      bool sawMemberLimit = false;
      for (const PlanningResourceCandidate* candidate : candidates)
      {
         if (!candidate->valid || candidate->candidateId.empty() ||
             candidate->value.empty() ||
             !SupportsBusiness(*candidate, aDemand.businessType))
            continue;
         if (candidate->maximumMembers == 0 ||
             candidate->currentMembers >= candidate->maximumMembers)
         {
            sawMemberLimit = true;
            continue;
         }
         Select(*candidate, candidate->value, aRecommendation);
         aRecommendation.evidence.push_back("businessType=" + aDemand.businessType);
         aRecommendation.evidence.push_back(
            "members=" + std::to_string(candidate->currentMembers) + "/" +
            std::to_string(candidate->maximumMembers));
         return;
      }
      aRecommendation.reason = sawMemberLimit
                                  ? ResourceDemandReason::cMEMBER_LIMIT_EXCEEDED
                                  : ResourceDemandReason::cNO_FEASIBLE_CANDIDATE;
   }

   static void RecommendRoute(const ResourceDemandMatchResult& aMatch,
                              PlanningRecommendation& aRecommendation)
   {
      if (!aMatch.capability.pathAvailable || aMatch.capability.route.empty())
      {
         aRecommendation.reason = ResourceDemandReason::cROUTE_UNAVAILABLE;
         return;
      }
      std::ostringstream route;
      for (std::size_t index = 0; index < aMatch.capability.route.size(); ++index)
      {
         if (index != 0) route << " -> ";
         route << aMatch.capability.route[index];
      }
      aRecommendation.status = RecommendationStatus::cAVAILABLE;
      aRecommendation.value = route.str();
      aRecommendation.rank = 1;
      aRecommendation.reason = ResourceDemandReason::cNONE;
      aRecommendation.source = DataOrigin::cDERIVED;
      aRecommendation.confidence =
         aMatch.capability.transmissionRateBps.valid
            ? aMatch.capability.transmissionRateBps.confidence
            : Confidence::cLOW;
      aRecommendation.evidence.push_back("capabilityRequestId=" +
                                         aMatch.capability.requestId);
   }

   NetworkProfileRepository mProfiles;
};
} // namespace nrm

#endif
