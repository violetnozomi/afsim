/**
 * @file NrmOperatorPresentation.hpp
 * @brief Converts internal planning data into concise operator-facing Chinese text.
 */

#ifndef NRM_OPERATOR_PRESENTATION_HPP
#define NRM_OPERATOR_PRESENTATION_HPP

#include <string>
#include <vector>

#include <QString>

#include "nrm/NetworkPlanTypes.hpp"
#include "nrm/NetworkProfileRepository.hpp"
#include "nrm/NetworkResourceTypes.hpp"
#include "nrm/ResourceDemandTypes.hpp"

namespace WkNrm
{
namespace OperatorPresentation
{
struct PlanAllocationSummary
{
   QString networkType;
   QString purpose;
   QString frequency;
   QString channelOrBeam;
   QString subnet;
   QString memberCount;
   QString routePolicy;
   QString state;
};

struct DemandRecommendationSummary
{
   QString demandId;
   QString task;
   QString conclusion;
   QString resourceAdvice;
   QString routeAdvice;
   QString explanation;
};

struct NetworkMetricSummary
{
   nrm::NetworkType networkTypeValue = nrm::NetworkType::cUNKNOWN;
   QString networkType;
   QString networkName;
   QString health;
   QString throughput;
   QString pdr;
   QString onlineRatio;
   QString transportDelay;
   QString referenceBandwidth;
   QString referenceUtilization;
   QString queueState;
};

struct ActiveLinkSummary
{
   nrm::NetworkType networkTypeValue = nrm::NetworkType::cUNKNOWN;
   QString networkType;
   QString source;
   QString destination;
   QString state;
   QString distance;
   QString throughput;
   QString referenceBandwidth;
   QString referenceUtilization;
   QString quality;
};

QString FormatFrequency(double aFrequencyHz);
QString FormatRate(double aRateBps);
QString FormatDistance(double aDistanceM);
QString RoutePolicyLabel(const std::string& aRoutePolicyId);
PlanAllocationSummary SummarizeAllocation(const nrm::NetworkPlanAllocation& aAllocation);
NetworkMetricSummary SummarizeNetworkMetric(
   const nrm::NetworkSnapshot& aNetwork,
   const nrm::NetworkProfile* aProfilePtr);
std::vector<ActiveLinkSummary> SummarizeActiveLinks(
   const nrm::FrameworkSnapshot& aSnapshot,
   const nrm::NetworkProfileRepository& aProfiles);
std::vector<DemandRecommendationSummary> SummarizeDemandRecommendations(
   const nrm::ResourceDemandSet& aDemandSet,
   const nrm::ResourceDemandBatchResult& aBatch);
}
} // namespace WkNrm

#endif
