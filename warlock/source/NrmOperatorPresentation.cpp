/**
 * @file NrmOperatorPresentation.cpp
 * @brief Implements the operator-facing presentation adapter.
 */

#include "NrmOperatorPresentation.hpp"

#include <algorithm>
#include <cmath>
#include <map>

#include <QLocale>
#include <QStringList>

#include "NrmUiText.hpp"
#include "nrm/NetworkTypeUtils.hpp"

namespace
{
QString CompactNumber(double aValue)
{
   const double rounded = std::round(aValue);
   if (std::abs(aValue - rounded) < 0.0005)
      return QString::number(rounded, 'f', 0);
   QString text = QString::number(aValue, 'f', 3);
   while (text.endsWith('0')) text.chop(1);
   if (text.endsWith('.')) text.chop(1);
   return text;
}

QString BusinessTypeLabel(const std::string& aBusinessType)
{
   static const std::map<std::string, QString> cLABELS = {
      {"COMMAND", QString::fromUtf8("指挥控制")},
      {"C2", QString::fromUtf8("指挥控制")},
      {"TRACK", QString::fromUtf8("态势航迹")},
      {"SITUATION", QString::fromUtf8("态势航迹")},
      {"SERVICE", QString::fromUtf8("综合业务")},
      {"SUPPORT", QString::fromUtf8("保障业务")},
      {"MONITOR", QString::fromUtf8("监视业务")},
      {"ISR_VIDEO", QString::fromUtf8("侦察视频")},
      {"VIDEO", QString::fromUtf8("视频传输")},
      {"FILE", QString::fromUtf8("文件传输")},
      {"VOICE", QString::fromUtf8("语音通信")},
      {"TELEMETRY", QString::fromUtf8("遥测业务")},
      {"NETWORK_MANAGEMENT", QString::fromUtf8("网络管理")}};
   const auto found = cLABELS.find(aBusinessType);
   return found == cLABELS.end() ? QString::fromStdString(aBusinessType)
                                 : found->second;
}

QString PlatformLabel(const std::string& aPlatformId)
{
   static const std::map<std::string, QString> cLABELS = {
      {"airborne_relay", QString::fromUtf8("空中中继机")},
      {"backup_processing_center", QString::fromUtf8("备份数据中心")},
      {"data_processing_center", QString::fromUtf8("数据处理中心")},
      {"link11_air_relay", QString::fromUtf8("Link-11空中中继")},
      {"link11_control_station", QString::fromUtf8("Link-11控制站")},
      {"link11_relay_north", QString::fromUtf8("Link-11北部中继")},
      {"link11_relay_south", QString::fromUtf8("Link-11南部中继")},
      {"link11_sensor_hub", QString::fromUtf8("Link-11传感器中心")},
      {"link11_service_node", QString::fromUtf8("Link-11业务节点")},
      {"link11_station_north", QString::fromUtf8("Link-11北部站")},
      {"link11_station_south", QString::fromUtf8("Link-11南部站")},
      {"mission_aircraft_1", QString::fromUtf8("任务飞机1")},
      {"mission_aircraft_2", QString::fromUtf8("任务飞机2")},
      {"mobile_command", QString::fromUtf8("机动指挥节点")},
      {"network_control_center", QString::fromUtf8("网络控制中心")},
      {"regional_command", QString::fromUtf8("区域指挥中心")},
      {"relay_aircraft_1", QString::fromUtf8("中继飞机1")},
      {"relay_aircraft_2", QString::fromUtf8("中继飞机2")},
      {"sat_relay_1", QString::fromUtf8("卫星中继1")},
      {"sat_relay_2", QString::fromUtf8("卫星中继2")},
      {"spectrum_monitor_aircraft", QString::fromUtf8("频谱监测飞机")},
      {"support_aircraft", QString::fromUtf8("保障飞机")},
      {"survey_uav_1", QString::fromUtf8("巡查无人机1")},
      {"survey_uav_2", QString::fromUtf8("巡查无人机2")},
      {"survey_uav_3", QString::fromUtf8("巡查无人机3")}};
   const auto found = cLABELS.find(aPlatformId);
   return found == cLABELS.end() ? QString::fromStdString(aPlatformId)
                                 : found->second;
}

QString RouteLabel(const std::string& aRoute)
{
   const QString raw = QString::fromStdString(aRoute);
   const QStringList hops = raw.split(" -> ", QString::SkipEmptyParts);
   if (hops.size() < 2) return raw;
   QStringList labels;
   for (const QString& hop : hops)
      labels.push_back(PlatformLabel(hop.trimmed().toStdString()));
   return labels.join(QString::fromUtf8(" → "));
}

QString PurposeLabel(const nrm::NetworkPlanAllocation& aAllocation)
{
   const QString joined =
      (QString::fromStdString(aAllocation.channelId) + " " +
       QString::fromStdString(aAllocation.subnetId) + " " +
       QString::fromStdString(aAllocation.routePolicyId)).toUpper();
   if (joined.contains("BACKUP")) return QString::fromUtf8("备份资源");
   if (joined.contains("SUPPORT")) return QString::fromUtf8("保障资源");
   if (joined.contains("PRIMARY")) return QString::fromUtf8("主用资源");
   return QString::fromUtf8("常规资源");
}

QString ConclusionLabel(nrm::DemandMatchStatus aStatus)
{
   switch (aStatus)
   {
   case nrm::DemandMatchStatus::cSATISFIED:
      return QString::fromUtf8("满足，可执行");
   case nrm::DemandMatchStatus::cUNSATISFIED:
      return QString::fromUtf8("需要调整");
   case nrm::DemandMatchStatus::cDATA_INVALID:
      return QString::fromUtf8("数据不足");
   }
   return QString::fromUtf8("数据不足");
}

QString RecommendationValue(const nrm::PlanningRecommendation& aRecommendation)
{
   if (aRecommendation.type == nrm::RecommendationType::cFREQUENCY)
   {
      bool ok = false;
      const double value = QString::fromStdString(aRecommendation.value).toDouble(&ok);
      return ok ? WkNrm::OperatorPresentation::FormatFrequency(value)
                : QString::fromStdString(aRecommendation.value);
   }
   if (aRecommendation.type == nrm::RecommendationType::cROUTE)
      return RouteLabel(aRecommendation.value);
   if (aRecommendation.type == nrm::RecommendationType::cSTATION)
      return PlatformLabel(aRecommendation.value);
   return QString::fromStdString(aRecommendation.value);
}

QString ReasonSummary(const nrm::ResourceDemandMatchResult& aResult)
{
   if (aResult.status == nrm::DemandMatchStatus::cSATISFIED)
      return QString::fromUtf8("当前路径满足全部通信约束");
   if (aResult.status == nrm::DemandMatchStatus::cDATA_INVALID)
      return QString::fromUtf8("缺少必要测量数据，请补充后重新匹配");

   QStringList reasons;
   for (const nrm::ResourceDemandReason reason : aResult.reasons)
   {
      const QString text = QString::fromStdString(
         WkNrm::UiText::TranslateCode(nrm::ToString(reason)));
      if (!text.isEmpty() && !reasons.contains(text)) reasons.push_back(text);
   }
   return reasons.isEmpty() ? QString::fromUtf8("当前方案未满足通信约束")
                            : reasons.join(QString::fromUtf8("、"));
}

const nrm::ResourceDemand* FindDemand(const nrm::ResourceDemandSet& aSet,
                                      const std::string& aDemandId)
{
   const auto found = std::find_if(
      aSet.demands.begin(), aSet.demands.end(),
      [&aDemandId](const nrm::ResourceDemand& aDemand)
      {
         return aDemand.demandId == aDemandId;
      });
   return found == aSet.demands.end() ? nullptr : &*found;
}

const nrm::WindowMetrics* FindWindow(const std::vector<nrm::WindowMetrics>& aWindows,
                                     double                                 aWindowS)
{
   const auto found = std::find_if(
      aWindows.begin(), aWindows.end(),
      [aWindowS](const nrm::WindowMetrics& aWindow)
      {
         return std::abs(aWindow.windowS - aWindowS) < 0.01;
      });
   return found == aWindows.end() ? nullptr : &*found;
}

const nrm::MetricValue<double>* ThroughputMetric(const nrm::WindowMetrics& aWindow)
{
   if (aWindow.deliveredThroughputBps.valid)
      return &aWindow.deliveredThroughputBps;
   return aWindow.throughputBps.valid ? &aWindow.throughputBps : nullptr;
}

const nrm::MetricValue<double>* PdrMetric(const nrm::WindowMetrics& aWindow)
{
   if (aWindow.deliveryRatioPercent.valid)
      return &aWindow.deliveryRatioPercent;
   return aWindow.pdrPercent.valid ? &aWindow.pdrPercent : nullptr;
}

QString Percent(double aValue, int aPrecision = 1)
{
   return QString::number(aValue, 'f', aPrecision) + "%";
}

QString NetworkName(const nrm::NetworkSnapshot& aNetwork)
{
   switch (aNetwork.networkType)
   {
   case nrm::NetworkType::cLINK11: return QString::fromUtf8("Link-11 协调网");
   case nrm::NetworkType::cLINK16: return QString::fromUtf8("Link-16 战术网");
   case nrm::NetworkType::cSATCOM: return QString::fromUtf8("卫星通信网");
   case nrm::NetworkType::cCDL: return QString::fromUtf8("CDL 侦察数据网");
   case nrm::NetworkType::cUNKNOWN: break;
   }
   return aNetwork.networkName.empty() ? QString::fromUtf8("未分类网络")
                                       : QString::fromStdString(aNetwork.networkName);
}

QString HealthText(const nrm::WindowMetrics* aWindowPtr)
{
   if (aWindowPtr == nullptr) return QString::fromUtf8("暂无10秒样本");
   const nrm::MetricValue<double>* pdrPtr = PdrMetric(*aWindowPtr);
   if (aWindowPtr->onlineRatioPercent.valid &&
       aWindowPtr->onlineRatioPercent.value < 90.0)
      return QString::fromUtf8("异常");
   if (pdrPtr != nullptr && pdrPtr->value < 90.0)
      return QString::fromUtf8("异常");
   if ((aWindowPtr->onlineRatioPercent.valid &&
        aWindowPtr->onlineRatioPercent.value < 99.0) ||
       (pdrPtr != nullptr && pdrPtr->value < 98.0))
      return QString::fromUtf8("需关注");
   return (pdrPtr != nullptr || aWindowPtr->onlineRatioPercent.valid)
             ? QString::fromUtf8("正常")
             : QString::fromUtf8("数据不足");
}

QString StateText(nrm::ResourceState aState)
{
   switch (aState)
   {
   case nrm::ResourceState::cONLINE: return QString::fromUtf8("在线");
   case nrm::ResourceState::cOFFLINE: return QString::fromUtf8("离线");
   case nrm::ResourceState::cDISABLED: return QString::fromUtf8("已停用");
   case nrm::ResourceState::cFAILED: return QString::fromUtf8("故障");
   case nrm::ResourceState::cUNKNOWN: return QString::fromUtf8("未知");
   }
   return QString::fromUtf8("未知");
}

QString ReferenceUtilization(double aThroughputBps,
                             const nrm::NetworkProfile* aProfilePtr)
{
   if (aProfilePtr == nullptr || !aProfilePtr->valid ||
       !std::isfinite(aProfilePtr->serviceCapacityBps) ||
       aProfilePtr->serviceCapacityBps <= 0.0 ||
       !std::isfinite(aThroughputBps) || aThroughputBps < 0.0)
      return QString::fromUtf8("未配置");
   const double utilization = 100.0 * aThroughputBps /
                              aProfilePtr->serviceCapacityBps;
   const int precision = utilization < 1.0 ? 2 : 1;
   return Percent(utilization, precision) + QString::fromUtf8("（参考）");
}
}

QString WkNrm::OperatorPresentation::FormatFrequency(double aFrequencyHz)
{
   if (!std::isfinite(aFrequencyHz) || aFrequencyHz <= 0.0)
      return QString::fromUtf8("—");
   if (aFrequencyHz >= 1000000000.0)
      return CompactNumber(aFrequencyHz / 1000000000.0) + " GHz";
   if (aFrequencyHz >= 1000000.0)
      return CompactNumber(aFrequencyHz / 1000000.0) + " MHz";
   if (aFrequencyHz >= 1000.0)
      return CompactNumber(aFrequencyHz / 1000.0) + " kHz";
   return CompactNumber(aFrequencyHz) + " Hz";
}

QString WkNrm::OperatorPresentation::FormatRate(double aRateBps)
{
   if (!std::isfinite(aRateBps) || aRateBps < 0.0)
      return QString::fromUtf8("—");
   if (aRateBps >= 1000000000.0)
      return CompactNumber(aRateBps / 1000000000.0) + " Gbit/s";
   if (aRateBps >= 1000000.0)
      return CompactNumber(aRateBps / 1000000.0) + " Mbit/s";
   if (aRateBps >= 1000.0)
      return CompactNumber(aRateBps / 1000.0) + " kbit/s";
   return CompactNumber(aRateBps) + " bit/s";
}

QString WkNrm::OperatorPresentation::FormatDistance(double aDistanceM)
{
   if (!std::isfinite(aDistanceM) || aDistanceM < 0.0)
      return QString::fromUtf8("—");
   if (aDistanceM >= 1000.0)
   {
      const int precision = aDistanceM >= 100000.0 ? 1 : 2;
      return QLocale(QLocale::English).toString(aDistanceM / 1000.0, 'f', precision) +
             " km";
   }
   return CompactNumber(aDistanceM) + " m";
}

QString WkNrm::OperatorPresentation::RoutePolicyLabel(
   const std::string& aRoutePolicyId)
{
   static const std::map<std::string, QString> cLABELS = {
      {"controlled-polling", QString::fromUtf8("受控轮询")},
      {"lowest-delay", QString::fromUtf8("最低时延路由")},
      {"stable-relay", QString::fromUtf8("稳定中继路由")},
      {"satellite-primary", QString::fromUtf8("卫星主链路")},
      {"satellite-backup", QString::fromUtf8("卫星备份链路")},
      {"direct-high-rate", QString::fromUtf8("高速直连")},
      {"direct-backup", QString::fromUtf8("直连备份")}};
   const auto found = cLABELS.find(aRoutePolicyId);
   return found == cLABELS.end()
             ? (aRoutePolicyId.empty() ? QString::fromUtf8("未指定")
                                       : QString::fromUtf8("自定义策略"))
             : found->second;
}

WkNrm::OperatorPresentation::PlanAllocationSummary
WkNrm::OperatorPresentation::SummarizeAllocation(
   const nrm::NetworkPlanAllocation& aAllocation)
{
   PlanAllocationSummary result;
   result.networkType = QString::fromStdString(
      WkNrm::UiText::TranslateCode(nrm::ToString(aAllocation.networkType)));
   result.purpose = PurposeLabel(aAllocation);
   result.frequency = FormatFrequency(aAllocation.frequencyHz);
   result.channelOrBeam = aAllocation.channelId.empty()
                            ? QString::fromUtf8("—")
                            : QString::fromStdString(aAllocation.channelId);
   result.subnet = aAllocation.subnetId.empty()
                      ? QString::fromUtf8("—")
                      : QString::fromStdString(aAllocation.subnetId);
   result.memberCount = QString::fromUtf8("%1个平台").arg(aAllocation.memberPlatformIds.size());
   result.routePolicy = RoutePolicyLabel(aAllocation.routePolicyId);
   result.state = aAllocation.enabled ? QString::fromUtf8("已启用")
                                      : QString::fromUtf8("已停用");
   return result;
}

WkNrm::OperatorPresentation::NetworkMetricSummary
WkNrm::OperatorPresentation::SummarizeNetworkMetric(
   const nrm::NetworkSnapshot& aNetwork,
   const nrm::NetworkProfile* aProfilePtr)
{
   NetworkMetricSummary result;
   result.networkTypeValue = aNetwork.networkType;
   result.networkType = QString::fromStdString(
      WkNrm::UiText::TranslateCode(nrm::ToString(aNetwork.networkType)));
   result.networkName = NetworkName(aNetwork);
   const nrm::WindowMetrics* windowPtr = FindWindow(aNetwork.windows, 10.0);
   result.health = HealthText(windowPtr);
   if (windowPtr == nullptr)
   {
      result.throughput = QString::fromUtf8("暂无样本");
      result.pdr = QString::fromUtf8("暂无样本");
      result.onlineRatio = QString::fromUtf8("暂无样本");
      result.transportDelay = QString::fromUtf8("暂无样本");
      result.referenceUtilization = QString::fromUtf8("暂无样本");
      result.queueState = QString::fromUtf8("未建模");
   }
   else
   {
      const nrm::MetricValue<double>* throughputPtr = ThroughputMetric(*windowPtr);
      const nrm::MetricValue<double>* pdrPtr = PdrMetric(*windowPtr);
      result.throughput = throughputPtr == nullptr
                             ? QString::fromUtf8("暂无业务")
                             : FormatRate(throughputPtr->value);
      result.pdr = pdrPtr == nullptr ? QString::fromUtf8("暂无样本")
                                     : Percent(pdrPtr->value);
      result.onlineRatio = windowPtr->onlineRatioPercent.valid
                              ? Percent(windowPtr->onlineRatioPercent.value)
                              : QString::fromUtf8("暂无样本");
      result.transportDelay = windowPtr->averageTransportDelayMs.valid
                                 ? QString::number(
                                      windowPtr->averageTransportDelayMs.value, 'f', 3) + " ms"
                                 : QString::fromUtf8("暂无样本");
      result.referenceUtilization = throughputPtr == nullptr
                                       ? QString::fromUtf8("暂无业务")
                                       : ReferenceUtilization(
                                            throughputPtr->value, aProfilePtr);
      result.queueState = windowPtr->queueUtilizationPercent.valid
                             ? Percent(windowPtr->queueUtilizationPercent.value)
                             : QString::fromUtf8("未建模");
   }
   result.referenceBandwidth =
      (aProfilePtr != nullptr && aProfilePtr->valid &&
       aProfilePtr->serviceCapacityBps > 0.0)
         ? FormatRate(aProfilePtr->serviceCapacityBps) +
              QString::fromUtf8("（参考）")
         : QString::fromUtf8("未配置");
   return result;
}

std::vector<WkNrm::OperatorPresentation::ActiveLinkSummary>
WkNrm::OperatorPresentation::SummarizeActiveLinks(
   const nrm::FrameworkSnapshot& aSnapshot,
   const nrm::NetworkProfileRepository& aProfiles)
{
   std::vector<ActiveLinkSummary> summaries;
   for (const nrm::LinkSnapshot& link : aSnapshot.links)
   {
      const nrm::WindowMetrics* windowPtr = FindWindow(link.windows, 10.0);
      if (windowPtr == nullptr) continue;
      const nrm::MetricValue<double>* throughputPtr = ThroughputMetric(*windowPtr);
      const bool hasTraffic = windowPtr->messages.queued > 0 ||
                              windowPtr->messages.transmitted > 0 ||
                              windowPtr->messages.received > 0 ||
                              windowPtr->messages.hops > 0 ||
                              (throughputPtr != nullptr && throughputPtr->value > 0.0);
      if (!hasTraffic) continue;

      const nrm::NetworkProfile* profilePtr = aProfiles.Find(link.networkType);
      ActiveLinkSummary summary;
      summary.networkTypeValue = link.networkType;
      summary.networkType = QString::fromStdString(
         WkNrm::UiText::TranslateCode(nrm::ToString(link.networkType)));
      summary.source = PlatformLabel(link.sourcePlatform);
      summary.destination = PlatformLabel(link.destinationPlatform);
      summary.state = StateText(link.state);
      summary.distance = link.distanceM.valid ? FormatDistance(link.distanceM.value)
                                              : QString::fromUtf8("接口未提供");
      summary.throughput = throughputPtr == nullptr
                              ? QString::fromUtf8("暂无业务")
                              : FormatRate(throughputPtr->value);
      summary.referenceBandwidth =
         (profilePtr != nullptr && profilePtr->valid &&
          profilePtr->serviceCapacityBps > 0.0)
            ? FormatRate(profilePtr->serviceCapacityBps) +
                 QString::fromUtf8("（参考）")
            : QString::fromUtf8("未配置");
      summary.referenceUtilization = throughputPtr == nullptr
                                        ? QString::fromUtf8("暂无业务")
                                        : ReferenceUtilization(
                                             throughputPtr->value, profilePtr);
      if (link.communicationQualityPercent.valid)
      {
         summary.quality = QString::fromUtf8("综合质量 %1")
                              .arg(Percent(link.communicationQualityPercent.value));
      }
      else if (link.snrDb.valid)
      {
         summary.quality = QString::fromUtf8("SNR %1 dB")
                              .arg(QString::number(link.snrDb.value, 'f', 1));
      }
      else
      {
         const nrm::MetricValue<double>* pdrPtr = PdrMetric(*windowPtr);
         summary.quality = pdrPtr == nullptr
                              ? QString::fromUtf8("射频接口未提供")
                              : QString::fromUtf8("PDR %1").arg(Percent(pdrPtr->value));
      }
      summaries.push_back(summary);
   }
   return summaries;
}

std::vector<WkNrm::OperatorPresentation::DemandRecommendationSummary>
WkNrm::OperatorPresentation::SummarizeDemandRecommendations(
   const nrm::ResourceDemandSet& aDemandSet,
   const nrm::ResourceDemandBatchResult& aBatch)
{
   std::vector<DemandRecommendationSummary> summaries;
   summaries.reserve(aBatch.results.size());
   for (const nrm::ResourceDemandMatchResult& match : aBatch.results)
   {
      DemandRecommendationSummary summary;
      summary.demandId = QString::fromStdString(match.demandId);
      const nrm::ResourceDemand* demandPtr = FindDemand(aDemandSet, match.demandId);
      summary.task = demandPtr == nullptr
                        ? summary.demandId
                        : QString::fromUtf8("%1：%2 → %3")
                             .arg(BusinessTypeLabel(demandPtr->businessType),
                                  PlatformLabel(demandPtr->sourcePlatform),
                                  PlatformLabel(demandPtr->destinationPlatform));
      summary.conclusion = ConclusionLabel(match.status);
      summary.explanation = ReasonSummary(match);

      QStringList resources;
      QStringList routes;
      for (const nrm::PlanningRecommendation& recommendation : match.recommendations)
      {
         if (recommendation.status != nrm::RecommendationStatus::cAVAILABLE) continue;
         const QString value = RecommendationValue(recommendation);
         if (value.isEmpty()) continue;
         if (recommendation.type == nrm::RecommendationType::cROUTE)
         {
            if (!routes.contains(value)) routes.push_back(value);
            continue;
         }
         const QString label = QString::fromStdString(
            WkNrm::UiText::TranslateCode(nrm::ToString(recommendation.type)));
         const QString item = label + " " + value;
         if (!resources.contains(item)) resources.push_back(item);
      }
      summary.resourceAdvice = resources.isEmpty()
                                  ? QString::fromUtf8("暂无可用资源调整项")
                                  : resources.join(QString::fromUtf8("；"));
      summary.routeAdvice = routes.isEmpty()
                               ? QString::fromUtf8("暂无可用替代路由")
                               : routes.join(QString::fromUtf8("；"));
      summaries.push_back(summary);
   }
   return summaries;
}
