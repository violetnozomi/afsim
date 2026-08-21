# 跨域网关与多网关级联设计

## 1. 目标和范围

一期在不修改 AFSIM 核心源码的前提下，为 Link-11、Link-16、SATCOM、CDL 四类网络增加真实的跨域消息转发能力。场景部署六种两两组合，每种组合两个双归属物理网关，共十二个网关平台；现有 25 个平台保留，综合场景扩展为 37 个平台。

一期同时支持单网关直达和显式授权的多网关级联。局部网关能力不能自动推导端到端互联，级联消息必须同时满足端到端路由模板和每跳有向能力授权。

## 2. 架构边界

- 新增 `WSF_NRM_CROSS_DOMAIN_GATEWAY_PROCESSOR`，它是唯一执行跨域消息转发的组件。
- 新增场景/仿真扩展，负责解析只读路由模板、注册处理器类型、校验全局能力连续性并提供只读运行状态投影。
- 纯 C++ `GatewayPolicyEngine` 负责路由、方向、源、目的、业务类型、TTL、Trace 和歧义判断，不依赖 AFSIM 或 Qt。
- `NrmSimInterface` 只复制网关值对象到 `ResourceSnapshot`，不发送消息、不修改网关。
- `AssessmentEvaluator` 只从显式有效的网关能力和路由模板构造跨域边。
- GUI 和 Reporter 只消费不可变快照，不持有 AFSIM 裸指针。
- 不修改 AFSIM 核心，不引入第三方依赖，不实现自动建链、改频或任意动态路由控制。

本功能是用户明确授权的受限消息转发能力。它扩展既有只读插件边界，但不允许资源测评或界面直接控制网络。

## 3. 物理节点组织

部署以下十二个双归属平台，每个平台严格只有指定两域通信设备：

| 组合 | 正向网关 | 反向网关 |
| --- | --- | --- |
| Link-11 / Link-16 | `gw_l11_l16_forward` | `gw_l11_l16_reverse` |
| Link-11 / SATCOM | `gw_l11_satcom_forward` | `gw_l11_satcom_reverse` |
| Link-11 / CDL | `gw_l11_cdl_forward` | `gw_l11_cdl_reverse` |
| Link-16 / SATCOM | `gw_l16_satcom_forward` | `gw_l16_satcom_reverse` |
| Link-16 / CDL | `gw_l16_cdl_forward` | `gw_l16_cdl_reverse` |
| SATCOM / CDL | `gw_satcom_cdl_forward` | `gw_satcom_cdl_reverse` |

每个物理网关可以配置一个或多个有向能力。当前一期场景每个节点只承载一个方向能力；正向能力不隐含反向能力，各方向可配置不同的源、目的和业务类型集合。

## 4. 公共数据契约

`GatewayResourceState` 表示一条有向能力，保留既有 `gatewayId/platformId/ingressNetworkId/egressNetworkId/enabled` 字段并追加：

- `ingressCommName`、`egressCommName`；
- `allowedSourcePlatformIds`、`allowedDestinationPlatformIds`、`allowedMessageTypes`；
- `priority`、`processingDelayMs`、`forwardingRateBps`；
- `maxQueueMessages`、`maxQueueBits`、当前队列深度；
- `receivedCount`、`forwardedCount`、`rejectedCount`、`droppedCount`、`forwardedBits`；
- `valid/origin/confidence/sampleTime/reasonCodes`；
- 有界 `recentEvents`。

新增 `GatewayRouteTemplate`：

- `routeId`；
- `sourcePlatformId`、`destinationPlatformId`、`destinationCommName`；
- `allowedMessageTypes`；
- 有序 `gatewayCapabilityIds`；
- `priority`、`enabled`、`valid`、`reasonCodes`。

新增 `GatewayForwardingEvent`，只记录 ID、网络、消息类型、大小、仿真时间、结果和固定原因码，不记录消息载荷。

`ResourceSnapshot` 增加 `gatewayRoutes`。既有 JSON V1 网关字段保持兼容；新增字段均为内部快照扩展，不要求旧甲方报文立即提供。

## 5. 配置与全局校验

处理器块定义本地能力、通信设备绑定、处理时延、转发速率、队列上限、去重窗口和事件上限。场景顶层 `nrm_gateway_route` 定义端到端有序能力路径。

初始化必须拒绝：

- 空或重复能力 ID、路由 ID；
- 同一能力入口和出口网络相同；
- 物理平台不是两个引用网络的成员；
- 空源、目的或消息类型授权；
- 通配符授权；
- 路由引用不存在或禁用能力；
- 相邻能力出口域与下一能力入口域不一致；
- 最后一能力出口域与目标通信端点不一致；
- 路由中重复能力或重复物理网关；

无效配置使用固定原因码并导致场景初始化失败，不能降级成隐式全通。

## 6. 消息信封与身份

继续使用标准 `WsfMessage`，跨域字段存放在原生 Aux Data：

- `nrm_gateway_transfer_id`；
- `nrm_gateway_route_id`；
- `nrm_gateway_route_index`；
- `nrm_gateway_hop_count`；
- `nrm_gateway_ttl`，默认 8；
- `nrm_gateway_trace`；
- `nrm_original_source_platform`；
- `nrm_final_destination_platform`；
- `nrm_final_destination_comm`。

`transferId` 是端到端逻辑身份；每次物理发送使用新的 AFSIM `SerialNumber`。第一网关要求实际消息 originator 等于路由源平台；AFSIM逐跳发送会更新 originator，因此后续网关要求实际 originator 等于模板中的上一物理网关，同时原始源Aux字段保持端到端一致。

## 7. 逐包处理流程

1. 源平台向路由第一能力所在网关的入口通信设备发送普通 `WsfMessage`。
2. 网关由实际接收地址确定入口网络，不信任消息自报入口域。
3. 策略引擎校验路由、当前索引、原始源、最终目标、消息类型、本地能力、TTL、Trace 和去重状态。
4. 通过的消息进入有界非抢占优先级队列；高优先级先服务，同优先级按到达时间和 transferId 排序。
5. 服务时间为固定处理时延加消息比特数除以转发速率。
6. 处理完成时克隆消息，分配新物理序列号，更新索引、跳数、TTL 和 Trace。
7. 若仍有下一能力，则发送到下一物理网关的入口通信设备；否则发送到最终目标在当前出口域的通信设备。
8. 每跳生成 `FORWARDED/REJECTED/DROPPED` 事件并更新计数器。

消息传输中不动态改路。某一跳失败时本消息结束；后续新消息可按优先级使用显式备用路由。

## 8. 防环、去重和容量

- 默认 TTL 为 8，可在 1 到 32 之间配置。
- Trace 中已出现当前能力时拒绝；路由目录校验另行禁止重复物理网关。
- 去重键为 `transferId + routeIndex`，在配置窗口内重复到达拒绝。
- `maxQueueMessages` 和 `maxQueueBits` 均为硬上限。
- 配置/授权失败为 `REJECTED`；队列满、出口不可用或延迟事件发送失败为 `DROPPED`。
- 仿真回调不执行文件 I/O。

## 9. 资源测评

评估图在同一物理网关的入口端点与出口端点之间增加显式网关边。网关边仅在以下条件全部满足时存在：

- 能力 `enabled && valid`；
- 原始源、最终目标和 `businessType` 在允许集合；
- 入口、出口端点在线；
- 存在至少一个启用且有效的端到端路由模板，其能力序列包含该边，并完整匹配任务源、目的和业务类型。

路由模板的完整序列作为评估约束，不能自由拼接其他局部能力。带宽取普通链路和网关速率瓶颈，时延累计普通链路与网关处理时延，PDR只使用具有可靠分母的有效观测；缺失值保持无效。

评估结果追加 `gatewayRouteIds` 和 `gatewayCapabilityIds`，使主备路径可解释。

## 10. 采集、报告和界面

- 仿真扩展提供一次性复制的网关状态和路由模板值对象。
- `NrmSimInterface` 每次发布快照时复制这些值对象。
- Reporter 的 `nrm.snapshot.v2` 输出 `gateways` 和 `gatewayRoutes`，包括运行计数和最近事件。
- 态势图将十二个物理网关显示为普通可点击平台；节点详情增加网关能力、方向、队列和转发计数。
- 资源测评选择网关节点仍遵循现有源/目的选点状态机，不增加界面对仿真对象的写控制。

## 11. 测试与验收

纯 C++ 测试覆盖：

- 单跳和两跳显式路由，以及可扩展的有序多跳模板；
- 方向、源、目的和消息类型拒绝；
- 路由连续性、重复能力、重复物理网关和TTL；
- transferId、跳号、TTL、Trace及固定拒绝原因码；
- 测评只能沿完整授权模板跨域；
- 网关速率和时延进入端到端指标；
- 旧 `GatewayResourceState` 输入保持兼容。

AFSIM固定场景覆盖：

- 37个平台、4张网络、12个物理网关、24个网关端点；
- 六种两域组合均存在；
- 六组域对各至少一条单网关直达消息成功；
- 两条双网关级联消息成功；
- 未授权源、目的、端点、类型、入口和跳序由纯C++策略测试拒绝；
- 仿真正常结束且无AFSIM核心修改。

完成状态最多为 `IMPLEMENTED / PRE_ACCEPTANCE`。Link-11、Link-16、SATCOM、CDL真实协议参数和甲方目标环境仍是最终验收阻塞项。
