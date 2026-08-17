# AFSIM 网络资源管理器实施状态与需求追踪

_对照《AFSIM网络资源管理器技术报告与实施方案》· 更新日期：2026-08-17_

## 状态摘要

| 范围 | 状态 | 当前版本/证据 |
| --- | --- | --- |
| 插件与远程开发框架 | 完成 | 0.1.2 |
| 网络资源采集闭环 | 完成 | 0.2.0 |
| 当前图、候选图和主备路由 | 完成 | 0.6.0 |
| 版本化四网剖面 | 已实现，PRE_ACCEPTANCE | 0.7.0 |
| 严格消息生命周期指标 | 已实现，PRE_ACCEPTANCE | 0.7.0 |
| 状态事件账本 | 已实现，PRE_ACCEPTANCE | 0.7.0 |
| 有界 K 路径硬约束选择 | 已实现，PRE_ACCEPTANCE | 0.7.0 |
| 运行隔离与可恢复上报 | 已实现，PRE_ACCEPTANCE | 0.7.0 |
| 三类故障场景 | 命令行已验证，Warlock 分阶段截图待人工验收 | 0.7.0 |
| 综合通信协同场景 | 25节点、29端点、60链路；无敌方、武器和交战元素，命令行已验证 | 0.11.0工作区 |
| 统一通信能力查询 | 已实现，PRE_ACCEPTANCE | 0.8.0 第一阶段 |
| 内部网链规划文件生命周期 | 已实现，PRE_ACCEPTANCE | 0.9.0 第一阶段 |
| 资源需求生命周期、并发匹配与有限候选建议 | 已实现，PRE_ACCEPTANCE；甲方精简JSON已接入 | 0.12.0工作区 |
| 进程内模型服务门面与版本注册表 | 已实现，PRE_ACCEPTANCE | 0.11.0 第一阶段 |
| 环境影响适配 | 第一版已实现：严格配置、内置采集、候选链路修正 | `docs/环境影响简要设计.md` |
| 甲方外部模块输入 | 接口预留 | `InputProvider` |
| 甲方JSON接口基线 | 13类Schema、中文注释、运行时严格校验、状态合并、任务评估和并发需求入口已完成，待甲方签字 | `docs/甲方接口对齐规范与JSON-Schema.md` |
| AFSIM 内置导航数据适配 | 第一版已实现，PRE_ACCEPTANCE | 实时状态 + `.neh`解析 + 中文导航页 |
| 甲方专用 GNSS/INS 结果包适配 | Adapter后置 | 仅在其格式不同于AFSIM `.neh`时需要 |
| 地形、气象、天象和电磁环境 | 第一版已实现 | AFSIM内置采集 + 中文环境页签 |
| 现代化可视化 | 深色卡片、统一控件、平台去重、多网徽点和标签避碰已验证 | 10/25平台远程VNC截图 |
| 合同30行指标软件映射 | 30/30已实现，26类唯一能力，PRE_ACCEPTANCE | `docs/合同30项指标实现矩阵.md` |
| 合同补缺闭环 | 已实现，37项固定C++测试通过 | 频点/协议资源、区域/姿态/干扰、规划协同、需求反馈、导航精度、恢复、部署及甲方入口端到端证据 |

## 2026-08-14 合同补缺闭环

- [x] 四类网络分别提供两个内置频点特性，并支持严格外部配置。
- [x] Link-11轮询单元、Link-16时隙、卫通波束信道和CDL信道统一输出容量、占用和剩余。
- [x] 作业区域多边形、有效时间、平台姿态、气象时段和活动干扰频带参与候选能力判断。
- [x] 规划支持`AIRBORNE/GROUND/JOINT`，动态入退网生成新修订，分发包支持本地指纹ACK。
- [x] 需求匹配回显请求来源/关联编号，输出连通性、性能、网络规模、流量分类和历史通过率。
- [x] 频率建议先排除占用，再排除干扰与保护带重叠候选。
- [x] GNSS、INS和组合导航在缺少误差时输出参数化1σ精度，直接值不被覆盖。
- [x] 恢复清单原子保存最后有效配置、规划/需求修订和未确认包；损坏输入不污染已有状态。
- [x] 第三方参考模型可在真实`ModelRegistry`注册、健康检查、描述和卸载。
- [x] 新增资源告警、需求反馈和规划协同JSONL，以及只读部署检查JSON。
- [x] 甲方JSON按具体网络ID隔离；资源/导航/环境按域合并，导航按平台upsert。
- [x] `messageId/runId/simTime`执行有界去重、乱序拒绝、新运行切换和旧运行隔离。
- [x] 规划输入绑定当前剖面配置版本；外部任务请求与界面共用统一评估入口。
- [x] 两个同类型网络按明确`networkId`隔离；跨网络成员链路引用被拒绝。
- [x] 环境`INFORMATION_ONLY/ALREADY_INCLUDED/CANDIDATE_ADJUSTMENT`完整保存；甲方模式不会被候选路径推断覆盖。
- [x] `DataContainer::LoadCustomerJson`真实文件入口覆盖资源、导航、环境、评估、规划、需求及新快照派生失效。
- [x] `ResourceSnapshotValidator`由JSON和同进程C++入口共用，统一校验具体网络归属、路由连续性、网关关系和指标语义。
- [x] AFSIM拓扑/实时链路作为基础态，甲方导航/环境作为跨AFSIM周期持久覆盖层，彻底移除隐式最后写入者覆盖。
- [x] 独立任务评估复用能力服务的环境链；三态环境不会漏算或二次衰减，硬阻断具有固定原因码。
- [x] 接口校验脚本默认使用`python3`并支持`PYTHON_BIN`覆盖，缺少`jsonschema`时给出明确诊断。

运行时接入边界：`FrequencyCharacteristicRepository`、`RecoveryStateStore`和
`ReferenceModelAdapter`当前分别属于“独立组件”“恢复组件”“兼容性测试样例”，均有固定
测试，但尚未绑定到Warlock运行时主链。当前频率推荐使用`NetworkProfileRepository`；启动
恢复位置/策略和第三方正式模块需甲方确认后再通过现有边界接入，不能把组件级实现写成现场
运行证据。`InputProvider`、`NetworkPlanAdapter`和`ContractInterfaceAdapter`是刻意保留的
兼容扩展接口，不属于可删除死代码；`CustomerNrmAdapter`已绑定`DataContainer`运行时主链。

上述内容均为插件内部`PRE_ACCEPTANCE`证据。甲方OA、保密、TLS、正式模型封装ABI和真实
四网协议字段未提供时统一标记`CUSTOMER_BLOCKED`，不伪造为已通过。

导航职责边界：第一版消费AFSIM `WsfNavigationErrors`实时状态和原生`.neh`时序包，负责
解析、校验、标准化、展示和记录，不实现GNSS、INS或融合算法。详见
`docs/AFSIM内置导航数据接入说明.md`。

## v0.7 已实现

- [x] 强类型 `NetworkProfile`、固定校验原因码和四类内置演示剖面
- [x] 严格 `NRM_NETWORK_PROFILES_V1` 外部配置入口
- [x] 快照、评估和报告携带 configVersion、providerId 和 profileId
- [x] 候选范围、建链时延、PDR 和容量从评估器常量迁移到剖面仓库
- [x] messageId 级排队、发送、终态关联和重复事件抑制
- [x] 提供负载、交付吞吐、严格交付率、P50/P95 排队与传输时延
- [x] 只有接收、没有可靠发送分母时链路 PDR 保持无效
- [x] 生命周期超时、容量上限、归档累计和固定无效原因码
- [x] 端点/链路状态事件、乱序去重和有界账本
- [x] 当前/窗口脱网时长、成员在网率和业务可用率
- [x] 建链尝试、成功率和平均时长的数据契约与纯 C++ 测试
- [x] 有界 K 简单路径、最大跳数和最大扩展状态限制
- [x] 最短路径不满足约束时选择后续可行路径
- [x] 当前图优先、候选图补充和有向边不重合备路
- [x] consideredPathCount、selectedPathRank、failedConstraints 和 disjointnessType
- [x] 网络 10 秒提供负载从服务容量中扣减，形成任务可准入容量
- [x] 实测带宽缺失时使用版本化剖面容量，并标记 `PARAMETERIZED_MODEL/LOW`
- [x] runId 隔离目录、manifest、队列丢弃计数、写失败状态和析构排空
- [x] `nrm.snapshot.v2` 与 `nrm.assessment.v3`，继续输出旧 schema 的既有字段
- [x] 链路中断、64 Mbit/s CDL 拥塞和低 PDR 剖面场景
- [x] 8 项自动测试和 `nrm_tests` 聚合目标

## v0.8 第一阶段已实现

- [x] `CapabilityRequest`、`CapabilityResult`、`EnvironmentContext`、
  `EnvironmentEffect`和固定`CapabilityReason`
- [x] 无Qt/AFSIM依赖的`CommunicationCapabilityService`
- [x] 复用`AssessmentEvaluator`、`NetworkProfileRepository`和
  `ConstrainedPathSelector`，不复制路径搜索
- [x] 路径总距离、最大单跳距离、瓶颈可准入速率、丢包率和累计时延
- [x] 当前路径10秒观测交付吞吐量和目标网络成员接入率
- [x] 当前链路只消费观测能力；候选能力标记`PARAMETERIZED_MODEL/LOW`
- [x] 四类`EnvironmentEffectAdapter`抽象；缺失输入固定返回
  `ENVIRONMENT_DATA_UNAVAILABLE`
- [x] 环境效果固定字段、所选端点路径传递和JSONL序列化；当前链路只上报证据，候选链路应用配置效果
- [x] DataContainer查询入口、Warlock只读“通信能力”页和
  `nrm.capability.v1`独立JSONL
- [x] 第9项能力服务测试；`capability_service_smoke`验证插件、当前图和消息交付底座

## v0.9 第一阶段已实现

- [x] `NetworkPlanDocument`公共值对象和`NetworkPlanAdapter`外部格式边界
- [x] 严格`NRM_NETWORK_PLAN_V1`解析、失败加载保护、修订单调和原子保存
- [x] 基础、引用、剖面、成员、时隙和动态入退网冲突的固定原因码校验
- [x] 甲方专用规则缺失时固定输出`CUSTOMER_RULE_UNAVAILABLE` WARNING
- [x] 规划需求映射到既有`CommunicationCapabilityService`的只读推演
- [x] `DRAFT / VALIDATED / REJECTED / READY_FOR_DISTRIBUTION`状态机
- [x] 仅在校验和全部需求推演通过后生成本地不可变分发包，并以
  稳定规划内容指纹防止复用陈旧结果
- [x] DataContainer规划入口、Warlock“资源规划”页和规划校验/推演JSONL
- [x] 规划失败、数据无效和并发冲突按需求输出固定中文调整建议，并同步到Warlock、JSONL
  和精简甲方规划响应
- [x] 新增Repository/Validator与Evaluation/Distribution两项测试，合计11项回归
- [x] WSF和Warlock两个插件构建成功；输入快照、规划和剖面保持不变
- [ ] 固定`network_plan_smoke`服务闭环：现有AFSIM脚本无安全规划服务调用入口，未伪造

本阶段使用内部格式和本地目录包，不代表甲方正式规划格式、专用校验规则或真实分发。

## v0.10 第一阶段已实现

- [x] `ResourceDemandSet`公共值对象、固定原因码和严格`NRM_RESOURCE_DEMAND_V1`文法
- [x] 失败加载保护、卸载后重载、修订单调、严格round-trip和拒绝覆盖的原子保存
- [x] 每条有效需求映射为一次既有`CommunicationCapabilityService`查询
- [x] PATH、NETWORK_SIZE、DISTANCE、BANDWIDTH、TRAFFIC、DELAY、PDR和
  BUSINESS_TYPE八项固定顺序检查
- [x] 要求值、当前值、裕量、单位、来源、置信度和固定原因码
- [x] 规划内容指纹、修订和snapshotVersion证据一致性校验
- [x] 显式有限候选内的频率、站点、信道、子网和时隙确定性建议；路由复用能力结果
- [x] 六类建议每类恰好输出`AVAILABLE`或明确`UNAVAILABLE`原因
- [x] DataContainer需求入口、Warlock“需求匹配”页和两个独立JSONL有界队列
- [x] 新增Repository与Matching两项测试，原11项无回归，合计13项
- [x] WSF和Warlock两个插件构建成功；快照、规划、需求、候选和剖面保持不变
- [ ] 固定`resource_demand_matching_smoke`服务闭环：现有AFSIM脚本无安全服务调用入口，
  未以普通通信场景伪造

本阶段使用内部需求格式和调用方显式候选，不代表甲方正式需求格式、候选资源集合或专用
频率/信道/子网/TDMA规则。建议不会自动应用到规划或运行网络。

## v0.11 第一阶段已实现

- [x] `ModelServiceContext`、八项固定操作、六类固定状态和强类型响应契约
- [x] schema、空requestId、非有限requestTime和snapshotVersion前置校验
- [x] `ModelServiceFacade`分别单次委托任务评估、能力、规划校验、规划推演、分发和需求匹配服务
- [x] 规划内容指纹、修订与snapshotVersion证据不匹配时不调用下游服务
- [x] 下游领域失败结果无损保留；不可预期异常固定返回`INTERNAL_ERROR`
- [x] `ModelRegistry`精确版本注册、查询、稳定列表、卸载和操作/schema能力查询
- [x] 非法描述符、重复modelId+version拒绝且不破坏既有项；同ID不同版本并存
- [x] `CustomerNrmAdapter`实现同进程强类型执行边界；`ContractInterfaceAdapter`保留为甲方私有对象转换SPI；`CustomerJsonCodec`负责精简V1文件、测试和回放
- [x] DataContainer持有Facade和Registry，注册唯一NRM描述符并复用门面调用链
- [x] 新增Facade与Registry两项测试，原13项无回归，合计15项
- [x] WSF和Warlock两个插件构建成功；公共头无Qt/AFSIM依赖
- [ ] 固定服务smoke：headless mission无法取得Warlock Facade和强类型快照，未伪造证据

本阶段形成同进程调用边界，不代表甲方私有对象映射、目标ABI、第三方模型兼容
或目标环境联调完成。没有新增HTTP、gRPC、消息队列、数据库、动态库扫描或自动网络控制。

## 仍为部分完成

| 需求 | 已实现 | 尚缺 |
| --- | --- | --- |
| 四网协议 | 类型、频点、参数剖面和统一适配边界 | 甲方四网模块和协议专用事件 |
| 链路状态 | AFSIM 图状态转换和状态时间积分 | 真实协议建链事件输入 |
| 建链指标 | 统一事件契约与测试 | AFSIM 当前 demo 无真实建链尝试时保持无效 |
| 严格 PDR | 单消息、单发送分母、单终态 | 组播预期接收者展开和重传语义 |
| 资源占用 | 提供负载、交付吞吐、可准入容量 | 空口忙时、真实队列上限和时隙资源 |
| 路由 | 当前/候选视角、有界 K 路径和一条备路 | 实际转发历史、本地路由表和显式跨网网关 |
| 故障证据 | 三个 mission 退出码 0，拥塞快照已验证 64 Mbit/s | 每个场景的 Warlock 前/中/后截图和人工点击记录 |
| 资源事件上报 | 账本及派生快照字段 | 独立 `resource_events.jsonl` |
| 运行恢复 | 单测覆盖两次启动、只读失败、排空和丢弃计数 | 甲方目标环境故障注入 |
| 网链资源规划 | 内部文件生命周期、确定性校验、只读能力推演、失败调整建议和本地分发包 | 甲方正式格式、专用规则、真实分发协议与目标环境联调 |
| 网链资源管理 | 内部需求生命周期、八项匹配检查和显式有限候选建议 | 甲方正式需求/候选格式、专用资源规则和自动控制接口 |
| 模型封装与兼容扩展 | 进程内强类型Facade、版本Registry和抽象Adapter | 甲方封装规范、正式协议、参考模块、真实分发和目标环境联调 |

以上均为内部演示剖面或模拟输入的 `PRE_ACCEPTANCE` 状态，不标记
`FINAL_ACCEPTANCE`。

## 后续顺序

当前里程碑结束后不自动进入下一模块。后续仅在用户明确指定且对应甲方资料到位后，选择
环境适配、正式规划格式/分发适配或GNSS/INS结果包适配中的单一里程碑。

当前插件保持只读，不自动执行建链、改频或改路由。
