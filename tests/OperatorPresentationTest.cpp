/**
 * @file OperatorPresentationTest.cpp
 * @brief Verifies concise Chinese operator-facing plan and demand summaries.
 */

#include "NrmOperatorPresentation.hpp"

#include <cassert>

#include "nrm/NetworkProfileRepository.hpp"

namespace
{
void SetMetric(nrm::MetricValue<double>& aMetric,
               double                    aValue,
               const char*               aUnit,
               double                    aWindowS = 10.0)
{
   aMetric.value = aValue;
   aMetric.unit = aUnit;
   aMetric.valid = true;
   aMetric.window = aWindowS;
}
}

int main()
{
   using namespace WkNrm::OperatorPresentation;

   assert(FormatFrequency(225000000.0) == QString::fromUtf8("225 MHz"));
   assert(FormatFrequency(1000000000.0) == QString::fromUtf8("1 GHz"));
   assert(FormatRate(614.4) == QString::fromUtf8("614.4 bit/s"));
   assert(FormatRate(45000000.0) == QString::fromUtf8("45 Mbit/s"));
   assert(FormatDistance(35786536.0) == QString::fromUtf8("35,786.5 km"));
   assert(RoutePolicyLabel("controlled-polling") ==
          QString::fromUtf8("受控轮询"));

   const nrm::NetworkProfileRepository profiles =
      nrm::NetworkProfileRepository::BuiltInDemo();
   nrm::NetworkSnapshot link11Network;
   link11Network.networkId = "nrm_link11_coordination";
   link11Network.networkName = "nrm_link11_coordination";
   link11Network.networkType = nrm::NetworkType::cLINK11;
   link11Network.endpointCount = 7;
   link11Network.onlineCount = 7;
   link11Network.activeLinks = 12;
   nrm::WindowMetrics networkWindow;
   networkWindow.windowS = 10.0;
   SetMetric(networkWindow.deliveredThroughputBps, 614.4, "bps");
   SetMetric(networkWindow.deliveryRatioPercent, 100.0, "percent");
   SetMetric(networkWindow.onlineRatioPercent, 100.0, "percent");
   SetMetric(networkWindow.averageTransportDelayMs, 0.082, "ms");
   link11Network.windows.push_back(networkWindow);

   const NetworkMetricSummary networkSummary = SummarizeNetworkMetric(
      link11Network, profiles.Find(nrm::NetworkType::cLINK11));
   assert(networkSummary.networkType == QString::fromUtf8("Link-11"));
   assert(networkSummary.networkName == QString::fromUtf8("Link-11 协调网"));
   assert(networkSummary.health == QString::fromUtf8("正常"));
   assert(networkSummary.throughput == QString::fromUtf8("614.4 bit/s"));
   assert(networkSummary.pdr == QString::fromUtf8("100.0%"));
   assert(networkSummary.transportDelay == QString::fromUtf8("0.082 ms"));
   assert(networkSummary.referenceBandwidth ==
          QString::fromUtf8("2.4 kbit/s（参考）"));
   assert(networkSummary.referenceUtilization ==
          QString::fromUtf8("25.6%（参考）"));
   assert(networkSummary.queueState == QString::fromUtf8("未建模"));

   nrm::FrameworkSnapshot snapshot;
   nrm::LinkSnapshot activeSatcom;
   activeSatcom.linkId = "regional_command->sat_relay_1";
   activeSatcom.sourcePlatform = "regional_command";
   activeSatcom.destinationPlatform = "sat_relay_1";
   activeSatcom.networkType = nrm::NetworkType::cSATCOM;
   activeSatcom.state = nrm::ResourceState::cONLINE;
   SetMetric(activeSatcom.distanceM, 35786536.0, "m");
   nrm::WindowMetrics activeWindow;
   activeWindow.windowS = 10.0;
   activeWindow.messages.transmitted = 1;
   activeWindow.messages.received = 1;
   SetMetric(activeWindow.deliveredThroughputBps, 1638.4, "bps");
   SetMetric(activeWindow.deliveryRatioPercent, 100.0, "percent");
   activeSatcom.windows.push_back(activeWindow);
   snapshot.links.push_back(activeSatcom);

   nrm::LinkSnapshot inactive = activeSatcom;
   inactive.linkId = "idle-link";
   inactive.sourcePlatform = "sat_relay_2";
   inactive.destinationPlatform = "backup_processing_center";
   inactive.windows.front().messages = nrm::MessageStatistics();
   inactive.windows.front().deliveredThroughputBps.value = 0.0;
   snapshot.links.push_back(inactive);

   const std::vector<ActiveLinkSummary> activeLinks =
      SummarizeActiveLinks(snapshot, profiles);
   assert(activeLinks.size() == 1);
   assert(activeLinks.front().source == QString::fromUtf8("区域指挥中心"));
   assert(activeLinks.front().destination == QString::fromUtf8("卫星中继1"));
   assert(activeLinks.front().distance == QString::fromUtf8("35,786.5 km"));
   assert(activeLinks.front().throughput == QString::fromUtf8("1.638 kbit/s"));
   assert(activeLinks.front().referenceBandwidth ==
          QString::fromUtf8("5 Mbit/s（参考）"));
   assert(activeLinks.front().referenceUtilization ==
          QString::fromUtf8("0.03%（参考）"));
   assert(activeLinks.front().quality == QString::fromUtf8("PDR 100.0%"));

   nrm::NetworkPlanAllocation allocation;
   allocation.networkType = nrm::NetworkType::cLINK11;
   allocation.frequencyHz = 225000000.0;
   allocation.channelId = "L11-CH-N";
   allocation.subnetId = "L11-NORTH";
   allocation.routePolicyId = "controlled-polling";
   allocation.memberPlatformIds = {"control", "relay", "member"};
   allocation.enabled = true;
   const PlanAllocationSummary allocationSummary = SummarizeAllocation(allocation);
   assert(allocationSummary.frequency == QString::fromUtf8("225 MHz"));
   assert(allocationSummary.memberCount == QString::fromUtf8("3个平台"));
   assert(allocationSummary.routePolicy == QString::fromUtf8("受控轮询"));
   assert(allocationSummary.state == QString::fromUtf8("已启用"));

   nrm::ResourceDemandSet demandSet;
   nrm::ResourceDemand demand;
   demand.demandId = "d01-l11-command";
   demand.businessType = "COMMAND";
   demand.sourcePlatform = "link11_control_station";
   demand.destinationPlatform = "link11_station_north";
   demandSet.demands.push_back(demand);

   nrm::ResourceDemandMatchResult result;
   result.demandId = demand.demandId;
   result.status = nrm::DemandMatchStatus::cUNSATISFIED;
   result.reasons.push_back(nrm::ResourceDemandReason::cPDR_NOT_MET);

   nrm::PlanningRecommendation frequency;
   frequency.demandId = demand.demandId;
   frequency.type = nrm::RecommendationType::cFREQUENCY;
   frequency.status = nrm::RecommendationStatus::cAVAILABLE;
   frequency.candidateId = "plan:l11-north:FREQUENCY";
   frequency.value = "225000000";
   frequency.evidence.push_back("candidateSource=parameterized");
   result.recommendations.push_back(frequency);

   nrm::PlanningRecommendation route;
   route.demandId = demand.demandId;
   route.type = nrm::RecommendationType::cROUTE;
   route.status = nrm::RecommendationStatus::cAVAILABLE;
   route.candidateId = "plan:l11-north:ROUTE";
   route.value = "link11_control_station -> link11_relay_north -> link11_station_north";
   result.recommendations.push_back(route);

   nrm::ResourceDemandBatchResult batch;
   batch.results.push_back(result);
   const std::vector<DemandRecommendationSummary> summaries =
      SummarizeDemandRecommendations(demandSet, batch);

   assert(summaries.size() == 1);
   assert(summaries.front().task ==
          QString::fromUtf8("指挥控制：Link-11控制站 → Link-11北部站"));
   assert(summaries.front().conclusion == QString::fromUtf8("需要调整"));
   assert(summaries.front().resourceAdvice.contains(QString::fromUtf8("频率 225 MHz")));
   assert(summaries.front().routeAdvice.contains(
      QString::fromUtf8("Link-11控制站 → Link-11北部中继 → Link-11北部站")));
   assert(summaries.front().explanation.contains(QString::fromUtf8("PDR")));
   assert(!summaries.front().resourceAdvice.contains("plan:l11-north"));
   assert(!summaries.front().explanation.contains("candidateSource"));
   return 0;
}
