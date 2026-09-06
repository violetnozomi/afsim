#include "NrmUiText.hpp"

#include "nrm/NetworkTypeUtils.hpp"

#include <cctype>
#include <sstream>
#include <unordered_map>

namespace
{
const std::unordered_map<std::string, std::string>& Translations()
{
   static const std::unordered_map<std::string, std::string> cVALUES = {
      {"NONE", "无"}, {"UNKNOWN", "未知"}, {"SUCCESS", "成功"},
      {"FAILED", "失败"}, {"FAIL", "失败"}, {"PASS", "通过"},
      {"WARNING", "警告"}, {"ERROR", "错误"}, {"PENDING", "待处理"},
      {"RUNNING", "运行中"}, {"STOPPED", "已停止"},
      {"VALID", "有效"}, {"INVALID", "无效"}, {"ENABLED", "已启用"},
      {"DRAFT", "草案"}, {"VALIDATED", "已校验"},
      {"READY_FOR_DISTRIBUTION", "可分发"}, {"REJECTED", "已拒绝"},
      {"NOT_EVALUATED", "未评估"}, {"SATISFIED", "满足"},
      {"UNSATISFIED", "不满足"}, {"AVAILABLE", "可用"},
      {"UNAVAILABLE", "不可用"}, {"ONLINE", "在线"},
      {"OFFLINE", "离线"}, {"DISABLED", "已禁用"},
      {"AFSIM_INTERNAL", "仿真实时数据"},
      {"CUSTOMER_MODULE", "系统接口数据"},
      {"REPLAY", "历史回放数据"},
      {"PARAMETERIZED_MODEL", "参数化估算"},
      {"PARAMETERIZED_CANDIDATE", "参数化候选链路"},
      {"DERIVED", "计算结果"}, {"ESTIMATED", "估算结果"},
      {"HIGH", "高可信"}, {"MEDIUM", "一般可信"}, {"LOW", "参考级"},
      {"L0_TOPOLOGY", "L0 拓扑降级"}, {"L1_LINK", "L1 链路降级"},
      {"L2_QUALITY", "L2 质量降级"}, {"L3_RESOURCE", "L3 资源降级"},
      {"LINK11", "Link-11"}, {"LINK16", "Link-16"},
      {"SATCOM", "SATCOM"}, {"CDL", "CDL"},
      {"NET_CONTROL_STATION", "网络控制站"},
      {"COMMAND_NODE", "指挥节点"},
      {"RELAY_GATEWAY", "中继网关"},
      {"SENSOR_SOURCE", "传感器信源"},
      {"SATCOM_TERMINAL", "SATCOM终端"},
      {"NETWORK_MEMBER", "网络成员"},
      {"POLLING_UNIT", "轮询单元"},
      {"BEAM_CHANNEL", "波束信道"},
      {"FORWARDED", "已转发"},
      {"CURRENT_LINK", "当前链路"}, {"CANDIDATE_LINK", "候选链路"},
      {"GATEWAY_TRANSITION", "网关转换"},
      {"GPS_ACTIVE", "卫星导航正常"}, {"GPS_DEGRADED", "卫星导航降级"},
      {"GPS_EXTERNAL", "外部卫星导航"}, {"INS", "惯性导航"},
      {"PERFECT", "理想导航"},
      {"BANDWIDTH", "带宽"}, {"DELAY", "时延"}, {"PDR", "分组投递率"},
      {"DISTANCE", "通信距离"}, {"NETWORK_SIZE", "网络规模"},
      {"TRAFFIC", "业务流量"}, {"BUSINESS_TYPE", "业务类型"},
      {"FREQUENCY", "频率"}, {"CHANNEL", "信道"}, {"SUBNET", "子网"},
      {"TIMESLOT", "时隙"}, {"ROUTE", "路由"},
      {"PATH", "路径"}, {"STATION", "站点调整"},
      {"AIRBORNE", "空中网络规划"}, {"GROUND", "地面网络规划"},
      {"JOINT", "联合网络规划"},
      {"CONNECTIVITY", "连通性"}, {"PERFORMANCE", "通信性能"},
      {"WEATHER", "气象"}, {"TERRAIN", "地形"},
      {"CELESTIAL", "天象"}, {"ELECTROMAGNETIC_INTERFERENCE", "电磁干扰"},
      {"BANDWIDTH_NOT_MET", "带宽要求不满足"},
      {"DELAY_NOT_MET", "时延要求不满足"},
      {"PDR_NOT_MET", "PDR要求不满足"},
      {"DISTANCE_NOT_MET", "距离要求不满足"},
      {"NETWORK_SIZE_NOT_MET", "网络规模要求不满足"},
      {"TRAFFIC_NOT_MET", "业务流量要求不满足"},
      {"BUSINESS_TYPE_NOT_SUPPORTED", "不支持该业务类型"},
      {"NO_CURRENT_PATH", "当前无可用路径"},
      {"NO_FEASIBLE_CANDIDATE", "无可行候选方案"},
      {"PATH_UNAVAILABLE", "路径不可用"}, {"ROUTE_UNAVAILABLE", "路由不可用"},
      {"NO_PATH", "没有可用路径"}, {"NODE_NOT_FOUND", "未找到节点"},
      {"NODE_OFFLINE", "节点离线"},
      {"NETWORK_TYPE_UNKNOWN", "网络类型未知"},
      {"UNKNOWN_NETWORK_TYPE", "未知网络类型"},
      {"LINK_NOT_ESTABLISHABLE", "链路无法建立"},
      {"PATH_SEARCH_LIMIT_REACHED", "路径搜索达到上限"},
      {"PATH_METRIC_INVALID", "路径指标无效"},
      {"PROFILE_CONFIG_INVALID", "网络配置无效"},
      {"ENVIRONMENT_HARD_BLOCKED", "环境硬约束阻断"},
      {"OPERATIONAL_AREA_OUTSIDE", "超出作业区域"},
      {"BANDWIDTH_MARGIN_NEGATIVE", "带宽裕量不足"},
      {"DELAY_MARGIN_NEGATIVE", "时延裕量不足"},
      {"RELIABILITY_MARGIN_NEGATIVE", "可靠性裕量不足"},
      {"ACCESS_DENOMINATOR_ZERO", "接入率计算分母为零"},
      {"THROUGHPUT_NOT_OBSERVED", "尚未观测到吞吐量"},
      {"ATTITUDE_MISMATCH", "平台姿态不匹配"},
      {"CANDIDATE_ADJUSTMENT", "候选链路参数调整"},
      {"INFORMATION_ONLY", "仅供参考"},
      {"ALREADY_INCLUDED", "已纳入计算"},
      {"INVALID_REQUEST", "请求无效"}, {"NON_FINITE_INPUT", "输入不是有限数值"},
      {"FORWARD_ALLOWED", "允许转发"},
      {"GATEWAY_LOOP_DETECTED", "检测到网关转发环路"},
      {"GATEWAY_QUEUE_CAPACITY_EXCEEDED", "网关队列容量不足"},
      {"GATEWAY_DUPLICATE_TRANSFER", "网关重复报文已丢弃"},
      {"GATEWAY_TTL_EXCEEDED", "跨域转发跳数已耗尽"},
      {"ROUTE_DESTINATION_COMM_MISMATCH", "最终通信端点与授权路由不一致"},
      {"GATEWAY_HOP_COUNT_MISMATCH", "网关路由跳号不一致"},
      {"GATEWAY_TRANSFER_ID_MISSING", "缺少跨域传输标识"},
      {"GATEWAY_ORIGINATOR_MISMATCH", "消息实际发送节点与授权跳序不一致"},
      {"INGRESS_NETWORK_MISMATCH", "入口网络不匹配"},
      {"INGRESS_COMM_MISMATCH", "入口通信设备不匹配"},
      {"SOURCE_NOT_ALLOWED", "源节点未获网关授权"},
      {"DESTINATION_NOT_ALLOWED", "目的节点未获网关授权"},
      {"MESSAGE_TYPE_NOT_ALLOWED", "消息类型未获网关授权"},
      {"DATA_INVALID", "数据无效"}, {"SNAPSHOT_INVALID", "快照无效"},
      {"METRIC_UNAVAILABLE", "指标不可用"},
      {"ENVIRONMENT_DATA_UNAVAILABLE", "环境数据不可用"},
      {"RF_QUALITY_UNAVAILABLE", "射频质量不可用"},
      {"NETWORK_PROFILE_CAPACITY", "网络配置容量"},
      {"PDR_ESTIMATE", "PDR估算"},
      {"MISSING_REQUIRED_FIELD", "缺少必填字段"},
      {"PARSE_ERROR", "解析错误"}, {"FILE_OPEN_FAILED", "文件打开失败"},
      {"FILE_WRITE_FAILED", "文件写入失败"},
      {"NO_CURRENT_PLAN", "当前未加载规划"},
      {"NO_CURRENT_DEMAND_SET", "当前未加载需求集"},
      {"PLAN_NOT_VALIDATED", "规划尚未校验"},
      {"RESOURCE_CONFLICT", "资源冲突"},
      {"INTERFERENCE_CONFLICT", "电磁干扰冲突"},
      {"ACK_MISMATCH", "分发确认不匹配"},
      {"BANDWIDTH_EXHAUSTED", "带宽资源耗尽"},
      {"TIMESLOT_EXHAUSTED", "时隙资源耗尽"},
      {"POLLING_UNIT_EXHAUSTED", "轮询资源耗尽"},
      {"SATCOM_RESOURCE_EXHAUSTED", "SATCOM资源耗尽"},
      {"CDL_RESOURCE_EXHAUSTED", "CDL资源耗尽"},
      {"NO_SAMPLES", "暂无样本"},
      {"MISSING_TRANSMIT_DENOMINATOR", "缺少发送统计分母"},
      {"MISSING_LIFECYCLE_CORRELATION", "缺少消息生命周期关联"},
      {"MISSING_STATE_HISTORY", "缺少状态历史"},
      {"MISSING_ESTABLISHMENT_EVENTS", "缺少建链事件"},
      {"PROFILE_NOT_CONFIGURED", "网络配置尚未设置"},
      {"PROFILE_INVALID", "网络配置无效"},
      {"PROFILE_NOT_FOUND", "未找到网络配置"},
      {"INVALID_INPUT", "输入无效"}, {"ZERO_DENOMINATOR", "计算分母为零"},
      {"OUT_OF_ORDER_EVENT", "事件时序错误"},
      {"OUTPUT_OPEN_FAILED", "输出文件打开失败"},
      {"OUTPUT_WRITE_FAILED", "输出文件写入失败"},
      {"PDR_OUT_OF_RANGE", "分组投递率超出范围"},
      {"SOURCE_EQUALS_DESTINATION", "源节点与目的节点相同"},
      {"CANDIDATE_DATA_UNAVAILABLE", "候选数据不可用"},
      {"ALLOWED_NETWORK_NOT_ALLOCATED", "允许网络未分配资源"},
      {"CAPABILITY_REQUEST_INVALID", "通信能力请求无效"},
      {"CANDIDATE_CONFLICT", "候选方案冲突"},
      {"MEMBER_LIMIT_EXCEEDED", "网络成员数超过限制"},
      {"FREQUENCY_NOT_SUPPORTED", "频率不受支持"},
      {"NEGATIVE_VALUE", "数值不能为负"},
      {"NON_FINITE_VALUE", "数值不是有限值"},
      {"REFERENCE_NOT_FOUND", "引用对象不存在"},
      {"PLATFORM_NOT_FOUND", "未找到平台"},
      {"ENDPOINT_NOT_FOUND", "未找到通信端点"},
      {"NOT_MEMBER", "平台不是网络成员"}, {"ALREADY_MEMBER", "平台已是网络成员"},
      {"INVALID_REVISION", "修订号无效"},
      {"REVISION_NOT_INCREMENTED", "修订号未递增"},
      {"OUTPUT_PATH_INVALID", "输出路径无效"},
      {"FILE_ALREADY_EXISTS", "文件已经存在"},
      {"PACKAGE_ALREADY_EXISTS", "分发包已经存在"},
      {"UNSUPPORTED_SCHEMA", "数据格式版本不受支持"},
      {"TRAILING_TOKEN", "存在多余字段"},
      {"UNKNOWN_RECORD_TYPE", "未知记录类型"},
      {"VALIDATION_FAILED", "校验失败"},
      {"EVALUATION_FAILED", "评估失败"},
      {"PLAN_IDENTITY_MISMATCH", "规划标识不匹配"},
      {"PLAN_EVIDENCE_MISMATCH", "规划证据不匹配"},
      {"CONFIG_VERSION_MISMATCH", "配置版本不匹配"},
      {"CUSTOMER_RULE_UNAVAILABLE", "专项规则待配置"},
      {"ADAPTER_UNAVAILABLE", "适配器不可用"},
      {"ATOMIC_RENAME_FAILED", "文件原子改名失败"},
      {"CHANGE_CONFLICT", "规划变更冲突"},
      {"DUPLICATE_ALLOCATION_ID", "分配编号重复"},
      {"DUPLICATE_CHANGE_ID", "变更编号重复"},
      {"DUPLICATE_DEMAND_ID", "需求编号重复"},
      {"DUPLICATE_DEMAND_REVISION", "需求修订重复"},
      {"DUPLICATE_MEMBER", "网络成员重复"},
      {"DUPLICATE_NETWORK_TYPE", "网络类型重复"},
      {"DUPLICATE_PLAN_REVISION", "规划修订重复"},
      {"INVALID_PLAN_ID", "规划编号无效"},
      {"INVALID_DEMAND_ID", "需求编号无效"},
      {"INVALID_DEMAND_SET_ID", "需求集编号无效"},
      {"INVALID_PROVIDER_ID", "数据提供方标识无效"},
      {"bandwidthBps", "链路带宽"}, {"pdrPercent", "分组投递率"},
      {"rssiDbm", "接收信号强度"}, {"snrDb", "信噪比"},
      {"ber", "误码率"}, {"queueUtilizationPercent", "队列占用率"},
      {"linkUtilizationPercent", "链路占用率"},
      {"transmissionDelayMs", "传输时延"},
      {"communicationDistanceM", "通信距离"},
      {"availableBandwidthBps", "可用带宽"},
      {"bit/s", "比特/秒"}, {"kbit/s", "千比特/秒"},
      {"m", "米"}, {"km", "千米"}, {"s", "秒"}, {"ms", "毫秒"},
      {"Hz", "赫兹"}, {"dB", "分贝"}, {"dBm", "分贝毫瓦"},
      {"percent", "百分比"}, {"percentage_point", "百分点"},
      {"ratio", "比率"}, {"deg", "度"}
   };
   return cVALUES;
}

std::string Trim(const std::string& aValue)
{
   std::size_t first = 0;
   while (first < aValue.size() && std::isspace(static_cast<unsigned char>(aValue[first]))) ++first;
   std::size_t last = aValue.size();
   while (last > first && std::isspace(static_cast<unsigned char>(aValue[last - 1]))) --last;
   return aValue.substr(first, last - first);
}

std::string EndpointLabel(const std::string& aPlatform, const std::string& aEndpointId)
{
   if (aPlatform.empty()) return aEndpointId;
   if (aEndpointId.empty()) return aPlatform;
   return aPlatform + "/" + aEndpointId;
}
}

std::string WkNrm::UiText::TranslateCode(const std::string& aCode)
{
   const auto found = Translations().find(aCode);
   return found == Translations().end() ? aCode : found->second;
}

std::string WkNrm::UiText::TranslateCodeWithRaw(const std::string& aCode)
{
   const std::string translated = TranslateCode(aCode);
   if (translated == aCode || aCode == "LINK11" || aCode == "LINK16" ||
       aCode == "SATCOM" || aCode == "CDL")
      return translated;
   return translated + "（" + aCode + "）";
}

std::string WkNrm::UiText::TranslateListWithRaw(const std::string& aCodes)
{
   std::istringstream input(aCodes);
   std::ostringstream output;
   std::string token;
   bool first = true;
   while (std::getline(input, token, ','))
   {
      token = Trim(token);
      if (token.empty()) continue;
      if (!first) output << "、";
      output << TranslateCodeWithRaw(token);
      first = false;
   }
   return output.str();
}

std::string WkNrm::UiText::FormatPlanPathSource(
   bool aPathAvailable,
   bool aUsesCandidate,
   bool aUsesParameterizedMetrics)
{
   if (!aPathAvailable) return "不可用";
   if (aUsesCandidate) return "参数化候选模型 / 低置信度";
   if (aUsesParameterizedMetrics) return "当前网络 / 参数补全（低置信度）";
   return "当前网络";
}

std::string WkNrm::UiText::NavigationEmptyState()
{
   return "当前场景未配置导航误差模型，或尚未收到外部导航数据。";
}

std::string WkNrm::UiText::FormatAssessmentRouteHop(const nrm::AssessmentRouteHop& aHop)
{
   const std::string sourceNetwork = TranslateCode(nrm::ToString(aHop.sourceNetworkType));
   const std::string destinationNetwork = TranslateCode(nrm::ToString(aHop.destinationNetworkType));

   std::ostringstream output;
   output << "第" << aHop.hopIndex << "跳 "
          << EndpointLabel(aHop.sourcePlatform, aHop.sourceEndpointId) << " → "
          << EndpointLabel(aHop.destinationPlatform, aHop.destinationEndpointId) << " [";
   if (aHop.kind == nrm::AssessmentRouteHopKind::cGATEWAY_TRANSITION || aHop.gateway)
   {
      output << sourceNetwork << " → " << destinationNetwork << "，网关转换";
      if (!aHop.gatewayCapabilityId.empty()) output << "，" << aHop.gatewayCapabilityId;
   }
   else
   {
      output << sourceNetwork << "，"
             << ((aHop.kind == nrm::AssessmentRouteHopKind::cCANDIDATE_LINK || aHop.candidate)
                    ? "候选链路"
                    : "当前链路");
   }
   output << "]";
   return output.str();
}
