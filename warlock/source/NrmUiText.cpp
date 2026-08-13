#include "NrmUiText.hpp"

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
      {"DRAFT", "草案"}, {"VALIDATED", "已校验"},
      {"READY_FOR_DISTRIBUTION", "可分发"}, {"REJECTED", "已拒绝"},
      {"NOT_EVALUATED", "未评估"}, {"SATISFIED", "满足"},
      {"UNSATISFIED", "不满足"}, {"AVAILABLE", "可用"},
      {"UNAVAILABLE", "不可用"}, {"ONLINE", "在线"},
      {"OFFLINE", "离线"}, {"DISABLED", "已禁用"},
      {"AFSIM_INTERNAL", "AFSIM内部数据"},
      {"CUSTOMER_MODULE", "甲方模块数据"},
      {"PARAMETERIZED_MODEL", "参数化模型"},
      {"PARAMETERIZED_CANDIDATE", "参数化候选链路"},
      {"DERIVED", "推导值"}, {"ESTIMATED", "估算值"},
      {"HIGH", "高"}, {"MEDIUM", "中"}, {"LOW", "低"},
      {"L0_TOPOLOGY", "L0 拓扑降级"}, {"L1_LINK", "L1 链路降级"},
      {"L2_QUALITY", "L2 质量降级"}, {"L3_RESOURCE", "L3 资源降级"},
      {"LINK11", "Link-11"}, {"LINK16", "Link-16"},
      {"SATCOM", "卫通"}, {"CDL", "CDL"},
      {"BANDWIDTH", "带宽"}, {"DELAY", "时延"}, {"PDR", "分组投递率"},
      {"DISTANCE", "通信距离"}, {"NETWORK_SIZE", "网络规模"},
      {"TRAFFIC", "业务流量"}, {"BUSINESS_TYPE", "业务类型"},
      {"FREQUENCY", "频率"}, {"CHANNEL", "信道"}, {"SUBNET", "子网"},
      {"TIMESLOT", "时隙"}, {"ROUTE", "路由"},
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
      {"PLAN_NOT_VALIDATED", "规划尚未校验"},
      {"RESOURCE_CONFLICT", "资源冲突"},
      {"BANDWIDTH_EXHAUSTED", "带宽资源耗尽"},
      {"TIMESLOT_EXHAUSTED", "时隙资源耗尽"},
      {"POLLING_UNIT_EXHAUSTED", "轮询资源耗尽"},
      {"SATCOM_RESOURCE_EXHAUSTED", "卫通资源耗尽"},
      {"CDL_RESOURCE_EXHAUSTED", "CDL资源耗尽"},
      {"bandwidthBps", "链路带宽"}, {"pdrPercent", "分组投递率"},
      {"rssiDbm", "接收信号强度"}, {"snrDb", "信噪比"},
      {"ber", "误码率"}, {"queueUtilizationPercent", "队列占用率"},
      {"linkUtilizationPercent", "链路占用率"},
      {"transmissionDelayMs", "传输时延"},
      {"communicationDistanceM", "通信距离"},
      {"availableBandwidthBps", "可用带宽"}
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
