# 变更记录

## 0.11.0 - 2026-08-01

- 新增无Qt/AFSIM依赖的`ModelServiceTypes`，固定请求/响应schema、七项操作、六类门面状态、
  固定原因码、请求身份、软件版本和snapshotVersion证据字段。
- 新增`ModelServiceFacade`，以显式依赖注入分别单次委托通信能力、规划校验、规划推演、
  本地分发包和需求匹配服务；无效上下文和证据在下游调用前拒绝。
- 新增`ModelRegistry`，支持精确语义版本注册、查询、列表、卸载和操作/schema能力判断，
  拒绝非法或重复描述符并保持确定性排序。
- 新增纯抽象`ContractInterfaceAdapter`外部适配边界，不定义甲方字段、端口、字节布局、
  JSON/XML格式或传输方式，也不提供假的甲方Adapter。
- DataContainer持有Facade和Registry并注册唯一NRM描述符；现有能力、规划和需求入口通过
  Facade委托，GUI与Reporter保持兼容且不新增服务管理页面或无使用者日志。
- 新增Facade与Registry两项纯C++测试，原13项无回归，合计15项；WSF和Warlock插件构建通过。
- 当前WSF插件没有从headless mission取得Warlock Facade和强类型快照的安全入口，因此未
  伪造规划、需求或模型服务smoke；状态为`IMPLEMENTED / PRE_ACCEPTANCE`。
- 修复资源规划拒绝时无建议的问题：逐需求结果新增中文调整建议，覆盖带宽、时延、PDR、
  无路径、离线、数据无效和并发资源冲突；Warlock独立建议页、规划JSONL和甲方规划响应同步输出。

## 0.10.0 - 2026-08-01

- 新增纯 C++ `ResourceDemand`公共契约与严格`NRM_RESOURCE_DEMAND_V1`文法，支持失败加载
  保护、卸载后重载、递增修订、严格round-trip和拒绝覆盖的原子保存。
- 新增`ResourceDemandMatchingService`，每条有效需求只调用一次现有通信能力服务，固定
  输出PATH、NETWORK_SIZE、DISTANCE、BANDWIDTH、TRAFFIC、DELAY、PDR和
  BUSINESS_TYPE八项检查及正负裕量。
- 规划内容指纹、修订或snapshotVersion不一致时拒绝拼接证据并输出
  `PLAN_EVIDENCE_MISMATCH`；匹配过程不修改快照、规划、需求、profile或运行网络。
- 新增`PlanningRecommendationEngine`，从显式有限候选中确定性筛选频率、站点、信道、
  子网和时隙，路由仅复用本次能力结果；六类均输出明确可用状态、证据和固定原因码。
- DataContainer新增需求生命周期和批量评估入口；Warlock新增“需求匹配”页；Reporter新增
  `resource_demand_results.jsonl`和`planning_recommendations.jsonl`两条有界队列。
- 新增需求Repository和Matching两个测试，Reporter测试扩展至新队列溢出、恢复与析构排空。
- headless WSF插件在当前允许范围内没有需求匹配服务调用入口，因此未伪造
  `resource_demand_matching_smoke`；本阶段状态为`IMPLEMENTED / PRE_ACCEPTANCE`。

## 0.9.0 - 2026-08-01

- 新增纯 C++ `nrm.network_plan.v1`公共契约、外部`NetworkPlanAdapter`边界和完整
  `NRM_NETWORK_PLAN_V1`严格文法。
- 新增规划Repository，支持失败不覆盖、修订单调、严格round-trip、临时文件加原子
  rename保存，以及显式路径或`NRM_PLAN_STORE_DIR`。
- 新增确定性Validator，覆盖profile/频率、平台/业务引用、成员上限、重复成员、独占
  时隙资源和动态JOIN/LEAVE冲突；甲方专用规则缺失时输出固定WARNING，不推导未提供规则。
- 新增只读`NetworkPlanEvaluationService`，将需求无损映射到`CapabilityRequest`并复用
  `CommunicationCapabilityService`，保留候选路径来源和置信度。
- 实现DRAFT、REJECTED、VALIDATED和READY_FOR_DISTRIBUTION状态结果；只有校验与全部
  需求推演通过时才生成不可覆盖的本地分发包，并以内容指纹绑定校验与推演结果。
- 修正规划生命周期：显式卸载后允许重新加载同一文件，活动规划和已生成修订仍拒绝静默覆盖。
- DataContainer新增规划生命周期入口；Warlock新增“资源规划”页；Reporter新增独立规划
  校验和推演JSONL及有界队列恢复状态。
- 新增Repository/Validator与Evaluation/Distribution两个测试，旧9项无回归，合计11项
  测试和两个插件目标构建通过。
- headless WSF插件在当前允许范围内没有规划服务调用入口，因此未伪造
  `network_plan_smoke`；本阶段状态为`IMPLEMENTED / PRE_ACCEPTANCE`。

## 0.8.0 - 2026-08-01

- 新增无 Qt/AFSIM 依赖的 `CommunicationCapabilityService` 和稳定公共查询契约。
- 复用现有评估器、版本化网络剖面和有界约束路径选择，统一输出路径总距离、最大单跳
  距离、瓶颈可准入速率、丢包率、累计时延、交付吞吐量和成员接入率。
- 当前路径只使用观测链路指标；候选路径容量、PDR 和建链时延仅来自版本化剖面，并标记
  `PARAMETERIZED_MODEL/LOW`。
- 新增 `EnvironmentEffectAdapter` 抽象和地形、气象、天象、电磁干扰四类无效占位；没有
  甲方数据时固定返回 `ENVIRONMENT_DATA_UNAVAILABLE`，不应用虚构传播公式。
- 环境适配器接收所选端点路径，并以固定的路径损耗、容量比例、丢包增量和时延增量字段
  输出；第一阶段只记录效果，不改变基础路径指标。
- 修正当前路径部分观测缺失时的选择语义；未设置时延上限时，缺失时延不再错误触发候选
  路径，同时距离、带宽和时延置信度按路径输入传播。
- Warlock 新增只读“通信能力”页，Reporter 新增独立 `capability_results.jsonl`。
- 新增第 9 项能力服务测试和固定 `capability_service_smoke` 场景。
- 本阶段结果仅为内部 `PRE_ACCEPTANCE`。

## 0.7.0 - 2026-08-01

- 增加版本化 `NetworkProfile`、四类内置演示剖面、严格外部配置语法和固定校验原因码。
- 评估器移除四类候选参数常量，快照与评估结果携带配置版本、提供者和剖面 ID。
- 增加有界消息生命周期关联、重复/乱序处理、超时清理，以及提供负载、交付吞吐、
  严格 PDR、P50/P95 排队与传输时延。
- 增加端点/链路状态和建链事件账本，计算脱网时长、在网率、业务可用率、建链成功率
  和建链时长。
- 增加有界 K 路径约束选择器；最短路径不满足硬约束时可选择后续可行路径。
- 备选路由保持有向边不重合语义，并输出候选数量、所选排名、失败约束和搜索上限状态。
- 上报升级为运行目录隔离的 `nrm.snapshot.v2` 和 `nrm.assessment.v3`，保留旧字段，
  增加 manifest、配置来源、固定指标原因码、队列丢弃和写入失败状态。
- 增加链路中断、拥塞、质量下降场景及配置、生命周期、状态账本、路径选择和上报恢复测试。
- 代码审查修复最终目的端交付判定、硬约束备路优先级、容量驱逐终态和非有限输入校验。
- 当前演示剖面与场景证据标记为 `PRE_ACCEPTANCE`，不代表甲方接口最终验收。

## 0.6.0 - 2026-07-30

- 严格分离当前启用图和参数化候选图。
- 仅在同一网络名称、同一网络类型且距离满足门限时生成候选边。
- 当前路径优先，当前不可达时输出候选建链主路由。
- 增加一条边不重合备选路由及候选标记。
- 中央态势图增加主备路由、候选线型和任务源/目的端点高亮。
- 评估上报升级到`nrm.assessment.v2`。
- 候选预测值和裕量标记为低置信度参数化模型，不直接判为稳定。

## 0.5.1 - 2026-07-30

- 评估源节点和目的节点改为快照驱动下拉选择。
- 节点列表随快照刷新，并保留仍然有效的用户选择。

## 0.5.0 - 2026-07-30

- 中央区域增加由AFSIM实时快照驱动的二维作战通信态势图。
- 显示飞机、地面节点、卫星和四类网络链路。
- 远程VNC环境使用稳定的软件渲染入口。

## 0.4.0 - 2026-07-30

- 增加稳定的任务输入、评估结果、裕量和原因码数据契约。
- 增加当前启用图上的多源、多目的端点Dijkstra可达性与最低预测时延路径。
- 增加节点、网络类型、带宽、时延和可靠性硬约束检查。
- 增加主路由、网络序列、正负裕量、原因码和只读建议。
- 增加Warlock“Task assessment”页面及平台、网络和QoS输入控件。
- 增加异步`assessment_results.jsonl`评估结果输出。
- 增加正常、带宽不足、限定网络不可达和节点不存在测试。

## 0.3.0 - 2026-07-30

- 增加有界的 1 秒、10 秒和 60 秒仿真时间滑动窗口。
- 增加网络吞吐量、PDR、在线率、平均排队时延和平均传输时延。
- 增加链路成功交付吞吐量；仅在数据率有效时计算链路利用率。
- 从 `WsfCommResult` 按有效条件提取数据率、RSSI、SNR 和 BER。
- 增加 Warlock“窗口指标”页面并扩展链路明细字段。
- 扩展 JSONL/CSV 输出，记录指标值、有效性、来源、置信度和窗口。
- 增加滑动窗口单元测试，并为四网演示消息设置不同的消息长度。

## 0.2.0 - 2026-07-30

- 增加网络、端点、链路、消息统计和带质量标记的统一资源数据契约。
- 增加内部、甲方模块和回放三类输入提供者的公共接口边界。
- 从 AFSIM 全局通信图采集四网、成员、状态、位置、链路、距离和消息统计。
- 增加 Warlock 四网总览、成员明细和链路明细三个实时页面，并默认显示面板。
- 增加异步 JSONL/CSV 上报线程和有界快照队列。
- 增加可直接运行的四网演示场景、分类测试和上报测试。
- Link-11、卫通、CDL 当前仍为参数化通用通信场景；RF质量、带宽和队列占用率尚未实现。

## 0.1.2 - 2026-07-30

- 增加 Windows 经 SSH 隧道连接的私有 TigerVNC/Openbox 远程桌面。
- 增加 VNC 与 Warlock 用户服务、远程运行和 GDB 调试入口。
- 修复 AFSIM 2.9 随附 Qt 5.12 在当前 glibc 上的 `QLockFile` 兼容问题。
- 验证 Warlock 窗口及 WSF、Warlock 两类网络资源管理器插件均正常加载。

## 0.1.1 - 2026-07-30

- 增加统一的运行、仿真与可视化入口手册。
- 明确每个后续功能必须同步记录入口、命令、输出、场景和版本。

## 0.1.0 - 2026-07-30

- 建立独立于 AFSIM 主源码的 WSF 扩展骨架。
- 建立 Warlock 网络资源管理器插件及最小实时状态面板。
- 增加外部扩展路径接入、构建验证和回退说明。
- 冻结首批公共数据类型，为 Link-11、Link-16、卫通、CDL 和甲方外部模块适配预留边界。
