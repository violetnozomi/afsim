# AFSIM 网络资源管理器实施状态与需求追踪

_对照《AFSIM网络资源管理器技术报告与实施方案》· 更新日期：2026-08-01_

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
| 统一通信能力查询 | 已实现，PRE_ACCEPTANCE | 0.8.0 第一阶段 |
| 内部网链规划文件生命周期 | 已实现，PRE_ACCEPTANCE | 0.9.0 第一阶段 |
| 环境影响适配 | 抽象接口已实现，真实适配未开始 | 甲方格式待提供 |
| 甲方外部模块输入 | 接口预留 | `InputProvider` |
| 甲方 GNSS/INS 结果包适配 | 未开始 | 合同、接口和样包待核 |
| 地形、气象、天象和电磁环境 | 接口预留 | v0.8 第二阶段，甲方格式待提供 |

导航职责边界：甲方功能包完成 GNSS/INS 解算；本项目负责结果包解析、字段校验、单位与
坐标标准化、状态管理、展示、记录和转发，不实现导航算法。

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
- [x] 环境效果固定字段、所选端点路径传递和JSONL序列化；第一阶段不应用效果
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
- [x] 新增Repository/Validator与Evaluation/Distribution两项测试，合计11项回归
- [x] WSF和Warlock两个插件构建成功；输入快照、规划和剖面保持不变
- [ ] 固定`network_plan_smoke`服务闭环：现有AFSIM脚本无安全规划服务调用入口，未伪造

本阶段使用内部格式和本地目录包，不代表甲方正式规划格式、专用校验规则或真实分发。

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
| 网链资源规划 | 内部文件生命周期、确定性校验、只读能力推演和本地分发包 | 甲方正式格式、专用规则、真实分发协议与目标环境联调 |

以上均为内部演示剖面或模拟输入的 `PRE_ACCEPTANCE` 状态，不标记
`FINAL_ACCEPTANCE`。

## 后续顺序

当前里程碑结束后不自动进入下一模块。后续仅在用户明确指定且对应甲方资料到位后，选择
环境适配、正式规划格式/分发适配或GNSS/INS结果包适配中的单一里程碑。

当前插件保持只读，不自动执行建链、改频或改路由。
