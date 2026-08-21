#include "NrmUiText.hpp"

#include <cassert>
#include <string>

int main()
{
   using WkNrm::UiText::TranslateCode;
   using WkNrm::UiText::TranslateCodeWithRaw;
   using WkNrm::UiText::TranslateListWithRaw;
   using WkNrm::UiText::NavigationEmptyState;
   using WkNrm::UiText::FormatAssessmentRouteHop;

   assert(TranslateCode("REJECTED") == "已拒绝");
   assert(TranslateCode("PARAMETERIZED_MODEL") == "参数化模型");
   assert(TranslateCode("LOW") == "低");
   assert(TranslateCode("L0_TOPOLOGY") == "L0 拓扑降级");
   assert(TranslateCode("bandwidthBps") == "链路带宽");
   assert(TranslateCode("RF_QUALITY_UNAVAILABLE") == "射频质量不可用");
   assert(TranslateCode("INTERFERENCE_CONFLICT") == "电磁干扰冲突");
   assert(TranslateCode("AIRBORNE") == "空中网络规划");
   assert(TranslateCode("GROUND") == "地面网络规划");
   assert(TranslateCode("JOINT") == "联合网络规划");
   assert(TranslateCode("ACK_MISMATCH") == "分发确认不匹配");
   assert(TranslateCode("CONNECTIVITY") == "连通性");
   assert(TranslateCode("PERFORMANCE") == "通信性能");
   assert(TranslateCode("FORWARD_ALLOWED") == "允许转发");
   assert(TranslateCode("GATEWAY_LOOP_DETECTED") == "检测到网关转发环路");
   assert(TranslateCode("GATEWAY_QUEUE_CAPACITY_EXCEEDED") == "网关队列容量不足");
   assert(TranslateCode("ROUTE_DESTINATION_COMM_MISMATCH") ==
          "最终通信端点与授权路由不一致");
   assert(TranslateCode("GATEWAY_HOP_COUNT_MISMATCH") == "网关路由跳号不一致");
   assert(TranslateCode("GATEWAY_TRANSFER_ID_MISSING") == "缺少跨域传输标识");
   assert(TranslateCode("GATEWAY_ORIGINATOR_MISMATCH") == "消息实际发送节点与授权跳序不一致");
   assert(TranslateCode("LINK11") == "Link-11");
   assert(TranslateCode("LINK16") == "Link-16");
   assert(TranslateCode("SATCOM") == "SATCOM");
   assert(TranslateCode("CDL") == "CDL");
   assert(TranslateCode("NET_CONTROL_STATION") == "网络控制站");
   assert(TranslateCode("COMMAND_NODE") == "指挥节点");
   assert(TranslateCode("RELAY_GATEWAY") == "中继网关");
   assert(TranslateCode("SENSOR_SOURCE") == "传感器信源");
   assert(TranslateCode("SATCOM_TERMINAL") == "SATCOM终端");
   assert(TranslateCode("NETWORK_MEMBER") == "网络成员");
   assert(TranslateCode("POLLING_UNIT") == "轮询单元");
   assert(TranslateCode("TIMESLOT") == "时隙");
   assert(TranslateCode("BEAM_CHANNEL") == "波束信道");
   assert(TranslateCode("FORWARDED") == "已转发");
   assert(TranslateCode("REPLAY") == "回放数据");
   assert(TranslateCode("bit/s") == "比特/秒");
   assert(TranslateCode("percentage_point") == "百分点");
   assert(TranslateCode("NO_SAMPLES") == "暂无样本");
   assert(TranslateCode("CURRENT_LINK") == "当前链路");
   assert(TranslateCode("CANDIDATE_LINK") == "候选链路");
   assert(TranslateCode("GATEWAY_TRANSITION") == "网关转换");
   assert(TranslateCode("NODE_NOT_FOUND") == "未找到节点");
   assert(TranslateCode("PROFILE_CONFIG_INVALID") == "网络配置无效");
   assert(TranslateCode("PDR_OUT_OF_RANGE") == "分组投递率超出范围");
   assert(TranslateCode("SOURCE_EQUALS_DESTINATION") == "源节点与目的节点相同");
   assert(TranslateCode("NO_CURRENT_DEMAND_SET") == "当前未加载需求集");
   assert(TranslateCode("CANDIDATE_DATA_UNAVAILABLE") == "候选数据不可用");
   assert(TranslateCode("ALLOWED_NETWORK_NOT_ALLOCATED") == "允许网络未分配资源");
   assert(TranslateCode("GPS_ACTIVE") == "卫星导航正常");
   assert(TranslateCode("INFORMATION_ONLY") == "仅供参考");
   assert(TranslateCode("PATH") == "路径");
   assert(TranslateCode("PROFILE_NOT_FOUND") == "未找到网络配置");
   assert(TranslateCode("STATION") == "站点调整");

   assert(TranslateCodeWithRaw("REJECTED") == "已拒绝（REJECTED）");
   assert(TranslateCodeWithRaw("LINK16") == "Link-16");
   assert(TranslateCodeWithRaw("custom-value") == "custom-value");

   assert(TranslateListWithRaw("NETWORK_PROFILE_CAPACITY,PDR_ESTIMATE") ==
          "网络配置容量（NETWORK_PROFILE_CAPACITY）、PDR估算（PDR_ESTIMATE）");
   assert(TranslateListWithRaw("") == "");
   assert(NavigationEmptyState() ==
          "当前场景未配置导航误差模型，或尚未收到甲方导航数据。");

   nrm::AssessmentRouteHop current;
   current.hopIndex = 1;
   current.sourcePlatform = "source";
   current.sourceEndpointId = "A";
   current.destinationPlatform = "relay";
   current.destinationEndpointId = "B";
   current.sourceNetworkType = nrm::NetworkType::cLINK16;
   current.destinationNetworkType = nrm::NetworkType::cLINK16;
   assert(FormatAssessmentRouteHop(current) ==
          "第1跳 source/A → relay/B [Link-16，当前链路]");

   nrm::AssessmentRouteHop candidate = current;
   candidate.hopIndex = 2;
   candidate.sourcePlatform = "relay";
   candidate.sourceEndpointId = "B";
   candidate.destinationPlatform = "destination";
   candidate.destinationEndpointId = "C";
   candidate.kind = nrm::AssessmentRouteHopKind::cCANDIDATE_LINK;
   candidate.candidate = true;
   assert(FormatAssessmentRouteHop(candidate) ==
          "第2跳 relay/B → destination/C [Link-16，候选链路]");

   nrm::AssessmentRouteHop gateway;
   gateway.hopIndex = 3;
   gateway.kind = nrm::AssessmentRouteHopKind::cGATEWAY_TRANSITION;
   gateway.sourcePlatform = "gateway";
   gateway.sourceEndpointId = "in";
   gateway.destinationPlatform = "gateway";
   gateway.destinationEndpointId = "out";
   gateway.sourceNetworkType = nrm::NetworkType::cLINK11;
   gateway.destinationNetworkType = nrm::NetworkType::cSATCOM;
   gateway.gateway = true;
   gateway.gatewayCapabilityId = "gw-l11-satcom";
   assert(FormatAssessmentRouteHop(gateway) ==
          "第3跳 gateway/in → gateway/out [Link-11 → SATCOM，网关转换，gw-l11-satcom]");
   return 0;
}
