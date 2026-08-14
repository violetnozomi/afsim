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

      RecommendFrequency(aPlanPtr, aCandidatesPtr, recommendations[0]);
      RecommendStation(aCandidatesPtr, recommendations[1]);
      RecommendUnoccupied(RecommendationType::cCHANNEL, aCandidatesPtr,
                          recommendations[2]);
      RecommendSubnet(aDemand, aCandidatesPtr, recommendations[3]);
      RecommendUnoccupied(RecommendationType::cTIMESLOT, aCandidatesPtr,
                          recommendations[4]);
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
