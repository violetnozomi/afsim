#ifndef NRM_NETWORK_PLAN_EVALUATION_SERVICE_HPP
#define NRM_NETWORK_PLAN_EVALUATION_SERVICE_HPP

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <set>
#include <sstream>

#include "nrm/CommunicationCapabilityService.hpp"
#include "nrm/NetworkPlanValidator.hpp"

namespace nrm
{
class NetworkPlanEvaluationService
{
public:
   explicit NetworkPlanEvaluationService(
      const NetworkProfileRepository& aProfiles,
      const EnvironmentEffectAdapter* aEnvironmentAdapterPtr = nullptr)
      : mProfiles(aProfiles)
      , mValidator(aProfiles)
      , mCapabilityService(aProfiles, aEnvironmentAdapterPtr)
   {
   }

   NetworkPlanEvaluationResult Evaluate(
      const ResourceSnapshot& aSnapshot,
      const NetworkPlanDocument& aPlan,
      const EnvironmentContext& aEnvironment = EnvironmentContext()) const
   {
      NetworkPlanEvaluationResult result;
      result.planId = aPlan.planId;
      result.revision = aPlan.revision;
      result.planFingerprint = network_plan_detail::PlanContentFingerprint(aPlan);
      result.snapshotVersion = aSnapshot.snapshotVersion;
      result.simTime = std::isfinite(aSnapshot.simTime) ? aSnapshot.simTime : 0.0;
      result.validation = mValidator.Validate(aSnapshot, aPlan);
      if (!result.validation.passed)
      {
         result.overallStatus = PlanEvaluationStatus::cDATA_INVALID;
         result.resultingState = NetworkPlanState::cREJECTED;
         for (const NetworkPlanDemand& demand : aPlan.demands)
         {
            PlanDemandEvaluation evaluation;
            evaluation.demandId = demand.demandId;
            evaluation.status = PlanEvaluationStatus::cNOT_EVALUATED;
            evaluation.reasons.push_back(PlanValidationReason::cVALIDATION_FAILED);
            evaluation.recommendations.push_back(
               "先修正规划校验问题，再执行只读推演。");
            result.demands.push_back(evaluation);
         }
         return result;
      }

      bool hasFailure = false;
      bool hasInvalid = false;
      for (const NetworkPlanDemand& demand : aPlan.demands)
      {
         PlanDemandEvaluation evaluation;
         evaluation.demandId = demand.demandId;
         const CapabilityRequest request = MapDemand(aPlan, demand);
         evaluation.capability =
            mCapabilityService.Query(aSnapshot, request, aEnvironment);
         EvaluateDemand(demand, evaluation);
         BuildRecommendations(demand, evaluation);
         hasFailure = hasFailure || evaluation.status == PlanEvaluationStatus::cFAIL;
         hasInvalid = hasInvalid || evaluation.status == PlanEvaluationStatus::cDATA_INVALID;
         result.demands.push_back(evaluation);
      }

      if (hasInvalid)
         result.overallStatus = PlanEvaluationStatus::cDATA_INVALID;
      else if (hasFailure)
         result.overallStatus = PlanEvaluationStatus::cFAIL;
      else
         result.overallStatus = PlanEvaluationStatus::cPASS;
      result.resultingState = result.overallStatus == PlanEvaluationStatus::cPASS
                                 ? NetworkPlanState::cVALIDATED
                                 : NetworkPlanState::cREJECTED;
      return result;
   }

private:
   static void AddReason(PlanDemandEvaluation& aEvaluation,
                         PlanValidationReason aReason)
   {
      if (std::find(aEvaluation.reasons.begin(), aEvaluation.reasons.end(), aReason) ==
          aEvaluation.reasons.end())
         aEvaluation.reasons.push_back(aReason);
   }

   static bool HasCapabilityReason(const CapabilityResult& aCapability,
                                   CapabilityReason aReason)
   {
      return std::find(aCapability.reasons.begin(), aCapability.reasons.end(), aReason) !=
             aCapability.reasons.end();
   }

   static bool HasReason(const PlanDemandEvaluation& aEvaluation,
                         PlanValidationReason aReason)
   {
      return std::find(aEvaluation.reasons.begin(), aEvaluation.reasons.end(), aReason) !=
             aEvaluation.reasons.end();
   }

   static void AddRecommendation(PlanDemandEvaluation& aEvaluation,
                                 const std::string& aRecommendation)
   {
      if (!aRecommendation.empty() &&
          std::find(aEvaluation.recommendations.begin(),
                    aEvaluation.recommendations.end(), aRecommendation) ==
             aEvaluation.recommendations.end())
         aEvaluation.recommendations.push_back(aRecommendation);
   }

   static std::string Number(double aValue, int aPrecision)
   {
      std::ostringstream output;
      output << std::fixed << std::setprecision(aPrecision) << aValue;
      return output.str();
   }

   static void BuildRecommendations(const NetworkPlanDemand& aDemand,
                                    PlanDemandEvaluation& aEvaluation)
   {
      if (aEvaluation.status == PlanEvaluationStatus::cPASS) return;

      if (HasReason(aEvaluation, PlanValidationReason::cNODE_OFFLINE))
      {
         AddRecommendation(
            aEvaluation, "恢复源/目的平台在线，或选择同网络中的在线替代节点。");
      }
      if (HasReason(aEvaluation, PlanValidationReason::cNO_PATH))
      {
         AddRecommendation(
            aEvaluation,
            "启用备用链路、增加中继节点，或改用任务允许的其他网络后重新推演。");
      }
      if (HasReason(aEvaluation, PlanValidationReason::cBANDWIDTH_NOT_MET))
      {
         std::string recommendation =
            "释放低优先级业务占用，或改用更高带宽的备用链路";
         if (aEvaluation.capability.transmissionRateBps.valid &&
             std::isfinite(aEvaluation.capability.transmissionRateBps.value))
         {
            const double gap = std::max(
               0.0, aDemand.requiredBandwidthBps -
                       aEvaluation.capability.transmissionRateBps.value);
            recommendation += "；当前至少缺少 " + Number(gap, 1) + " bit/s";
         }
         AddRecommendation(aEvaluation, recommendation + "。");
      }
      if (HasReason(aEvaluation, PlanValidationReason::cDELAY_NOT_MET))
      {
         std::string recommendation = "减少中继跳数，或选择预测时延更低的路由";
         if (aEvaluation.capability.transmissionDelayMs.valid &&
             std::isfinite(aEvaluation.capability.transmissionDelayMs.value))
         {
            const double gap = std::max(
               0.0, aEvaluation.capability.transmissionDelayMs.value -
                       aDemand.maximumDelayMs);
            recommendation += "；当前至少需降低 " + Number(gap, 3) + " ms";
         }
         AddRecommendation(aEvaluation, recommendation + "。");
      }
      if (HasReason(aEvaluation, PlanValidationReason::cPDR_NOT_MET))
      {
         std::string recommendation = "改用PDR更高的备用链路或质量更好的中继路径";
         if (aEvaluation.capability.packetLossPercent.valid &&
             std::isfinite(aEvaluation.capability.packetLossPercent.value))
         {
            const double currentPdr = 100.0 - aEvaluation.capability.packetLossPercent.value;
            const double gap = std::max(0.0, aDemand.minimumPdrPercent - currentPdr);
            recommendation += "；当前PDR至少需提升 " + Number(gap, 3) + " 个百分点";
         }
         AddRecommendation(aEvaluation, recommendation + "。");
      }
      if (HasReason(aEvaluation, PlanValidationReason::cDATA_INVALID))
      {
         AddRecommendation(
            aEvaluation,
            "检查节点映射、网络标签及带宽/时延/PDR测量；缺失字段可启用项目参数化默认值后重新推演。");
      }
      if (aEvaluation.recommendations.empty())
      {
         AddRecommendation(
            aEvaluation, "根据失败原因调整资源分配或业务约束后重新执行只读推演。");
      }
   }

   static CapabilityRequest MapDemand(const NetworkPlanDocument& aPlan,
                                      const NetworkPlanDemand& aDemand)
   {
      CapabilityRequest request;
      request.requestId = aDemand.demandId;
      request.sourcePlatform = aDemand.sourcePlatform;
      request.destinationPlatform = aDemand.destinationPlatform;
      request.businessType = aDemand.businessType;
      request.payloadBits = aDemand.payloadBits;
      request.requiredBandwidthBps = aDemand.requiredBandwidthBps;
      request.maximumDelayMs = aDemand.maximumDelayMs;
      request.minimumPdrPercent = aDemand.minimumPdrPercent;
      request.allowedNetworks = aDemand.allowedNetworks;
      request.allowParameterizedMetricFallback =
         aPlan.source == DataOrigin::cPARAMETERIZED_MODEL;
      if (request.allowedNetworks.empty())
      {
         std::set<NetworkType> allocatedTypes;
         for (const NetworkPlanAllocation& allocation : aPlan.allocations)
         {
            if (allocation.enabled && allocation.networkType != NetworkType::cUNKNOWN)
               allocatedTypes.insert(allocation.networkType);
         }
         request.allowedNetworks.assign(allocatedTypes.begin(), allocatedTypes.end());
      }
      return request;
   }

   static void EvaluateDemand(const NetworkPlanDemand& aDemand,
                              PlanDemandEvaluation& aEvaluation)
   {
      const CapabilityResult& capability = aEvaluation.capability;
      if (!capability.requestValid)
      {
         aEvaluation.status = PlanEvaluationStatus::cDATA_INVALID;
         AddReason(aEvaluation, PlanValidationReason::cDATA_INVALID);
         return;
      }
      if (!capability.pathAvailable)
      {
         if (HasCapabilityReason(capability, CapabilityReason::cPATH_METRIC_INVALID) ||
             HasCapabilityReason(capability, CapabilityReason::cPROFILE_CONFIG_INVALID) ||
             HasCapabilityReason(capability, CapabilityReason::cNON_FINITE_INPUT))
         {
            aEvaluation.status = PlanEvaluationStatus::cDATA_INVALID;
            AddReason(aEvaluation, PlanValidationReason::cDATA_INVALID);
         }
         else
         {
            aEvaluation.status = PlanEvaluationStatus::cFAIL;
            AddReason(aEvaluation,
                      HasCapabilityReason(capability, CapabilityReason::cNODE_OFFLINE)
                         ? PlanValidationReason::cNODE_OFFLINE
                         : PlanValidationReason::cNO_PATH);
         }
         return;
      }

      bool invalid = false;
      bool failed = false;
      if (aDemand.requiredBandwidthBps > 0.0)
      {
         if (!capability.transmissionRateBps.valid ||
             !std::isfinite(capability.transmissionRateBps.value))
            invalid = true;
         else if (capability.transmissionRateBps.value < aDemand.requiredBandwidthBps)
         {
            failed = true;
            AddReason(aEvaluation, PlanValidationReason::cBANDWIDTH_NOT_MET);
         }
      }
      if (aDemand.maximumDelayMs > 0.0)
      {
         if (!capability.transmissionDelayMs.valid ||
             !std::isfinite(capability.transmissionDelayMs.value))
            invalid = true;
         else if (capability.transmissionDelayMs.value > aDemand.maximumDelayMs)
         {
            failed = true;
            AddReason(aEvaluation, PlanValidationReason::cDELAY_NOT_MET);
         }
      }
      if (aDemand.minimumPdrPercent > 0.0)
      {
         if (!capability.packetLossPercent.valid ||
             !std::isfinite(capability.packetLossPercent.value))
            invalid = true;
         else if (capability.packetLossPercent.value >
                  100.0 - aDemand.minimumPdrPercent)
         {
            failed = true;
            AddReason(aEvaluation, PlanValidationReason::cPDR_NOT_MET);
         }
      }

      if (invalid)
      {
         aEvaluation.status = PlanEvaluationStatus::cDATA_INVALID;
         AddReason(aEvaluation, PlanValidationReason::cDATA_INVALID);
      }
      else if (failed)
      {
         aEvaluation.status = PlanEvaluationStatus::cFAIL;
         AddReason(aEvaluation, PlanValidationReason::cEVALUATION_FAILED);
      }
      else
      {
         aEvaluation.status = PlanEvaluationStatus::cPASS;
      }
   }

   NetworkProfileRepository mProfiles;
   NetworkPlanValidator mValidator;
   CommunicationCapabilityService mCapabilityService;
};
} // namespace nrm

#endif
