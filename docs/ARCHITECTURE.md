# 总体框架

## 分层

```text
AFSIM 内部事件 / 甲方结果包 / 回放输入
                 │
                 ▼
输入适配层（Observer、四网模块、GNSS/INS结果包、环境影响）
                 │ 统一值对象
                 ▼
状态与证据层
  ├── MessageLifecycleTracker：消息关联、去重、超时和严格窗口队列
  ├── ResourceEventLedger：端点/链路状态及建链事件
  ├── NetworkProfileRepository：版本化参数剖面与校验
  ├── NetworkPlanRepository：严格解析、修订存储和加载失败恢复
  └── ResourceDemandRepository：需求集修订、原子存储和失败加载恢复
                 │
                 ▼
不可变 ResourceSnapshot
  ├── Warlock 展示
  ├── AssessmentEvaluator / ConstrainedPathSelector
  ├── CommunicationCapabilityService / EnvironmentEffectAdapter
  ├── NetworkPlanValidator / NetworkPlanEvaluationService
  ├── NetworkPlanDistributionService（仅本地不可变包）
  ├── ResourceDemandMatchingService / PlanningRecommendationEngine
  ├── ModelServiceFacade / ModelRegistry（进程内强类型边界）
  └── SnapshotReporter（runId 隔离的 JSONL/CSV/manifest）
```

v0.7.0 延续 v0.6.0 的纵向闭环。Warlock 仿真接口监听通信事件并扫描全局通信图，生成
网络、端点、链路、位置、距离和消息值对象；新增的生命周期跟踪器只把具有发送分母且能
关联终态的消息纳入 PDR，状态账本按仿真时间积分脱网、在网、业务可用和建链指标。

纯 C++ `AssessmentEvaluator` 从 `NetworkProfileRepository` 获取候选边参数，不再保存
四类网络常量。它先在当前启用图上使用 `ConstrainedPathSelector` 搜索有界 K 条路径并
逐条检查硬约束；当前图没有可行路径时，再搜索必须包含候选边的路径。最短路径不满足约束
但较长路径满足时，返回较长的可行路径。备选路由禁用主路由的有向边，结果明确记录
`directed-edge` 不重合语义。

v0.8 第一阶段的纯 C++ `CommunicationCapabilityService` 将能力请求映射为既有评估任务，
直接复用当前/候选图和有界路径结果。它只补充所选路径的距离聚合、观测交付吞吐量和目标
网络成员接入率，不重新实现构图或路径搜索。当前链路缺少观测容量、PDR或时延时对应指标
保持无效；候选容量、PDR和建链时延只来自`NetworkProfileRepository`，并标记
`PARAMETERIZED_MODEL/LOW`。

`EnvironmentEffectAdapter`是地形、气象、天象和电磁干扰的抽象边界。第一阶段没有甲方
数据格式和样包，因此默认输出四个`valid=false / ENVIRONMENT_DATA_UNAVAILABLE`效果。
第二阶段按`环境影响简要设计.md`复用AFSIM地形、场景环境、仿真历元和EM交互结果，并用
严格的`NRM_ENVIRONMENT_V1`配置补充候选链路估计。当前RF结果只附加环境证据，不重复扣减；
参数化效果只作用于候选链路并固定标记低置信度。

v0.9 第一阶段以`NetworkPlanDocument`作为内部公共值对象。`NetworkPlanRepository`
严格解析`NRM_NETWORK_PLAN_V1`，加载失败不覆盖最后一个有效规划，保存使用同目录临时
文件和原子重命名并拒绝覆盖既有修订。`NetworkPlanValidator`只执行已定义的结构、
引用、剖面、成员和资源冲突规则，不猜测保护带、干扰或 TDMA 复用规则。

`NetworkPlanEvaluationService`先校验规划，再将每条需求无损映射为
`CapabilityRequest`并调用既有`CommunicationCapabilityService`。它不复制构图、
路径搜索或能力指标公式，也不修改快照、规划、剖面或实时网络。只有校验通过且所有需求
推演为 PASS 时，`NetworkPlanDistributionService`才在显式目录生成包含正文、校验、
推演和 manifest 的本地不可变包。

v0.10 第一阶段以`ResourceDemandSet`作为内部需求值对象。匹配服务先严格校验需求和证据
身份，再把每条有效需求映射为一个`CapabilityRequest`并只调用一次现有能力服务。八项
检查使用同一个`CapabilityResult`和当前快照，规划内容指纹或snapshotVersion不一致时
拒绝拼接证据。

`PlanningRecommendationEngine`不搜索新路径，也不生成资源候选。频率、站点、信道、
子网和时隙仅在调用方显式有限候选中稳定排序；路由只复用本次能力结果。所有结果均为
只读分析，不修改规划、需求、快照或AFSIM运行网络。

v0.11第一阶段增加`ModelServiceFacade`作为进程内强类型编排边界。Facade先校验
`ModelServiceContext`的schema、请求标识和snapshotVersion，再分别单次委托现有通信能力、
规划校验、规划推演、本地分发包或需求匹配服务。领域结果不被转换成自由文本，异常不会
静默变为成功，输入值对象和profile保持不变。

`ModelRegistry`只保存`ModelDescriptor`值对象，使用精确版本查询和确定性列表顺序，不扫描
动态库、不拥有AFSIM对象或Qt指针。`ContractInterfaceAdapter`只冻结抽象转换职责；甲方
规范缺失时不定义端口、字段名、字节布局或传输方式。DataContainer注册唯一NRM Facade并
复用该门面，Warlock页面仍只消费既有领域结果。

## 稳定边界

- `include/nrm/` 中的公共契约和算法不依赖 AFSIM 或 Qt。
- WSF 扩展入口只负责能力注册；`NrmSimInterface` 是当前 AFSIM 内部监听适配器。
- `DataContainer` 只存在于 GUI 线程，跨线程事件只传递值对象。
- `SnapshotReporter` 使用有界队列和独立线程，仿真回调不执行文件 I/O。
- `InputProvider` 冻结内部、甲方模块和回放输入的公共边界。
- `AssessmentEvaluator` 不改变 AFSIM 图，只产生评估结果和只读建议。
- 规划服务只产生校验、推演和本地包；编辑生成新修订的`DRAFT`，不向网络下发。
- 需求服务只产生匹配、差距和建议；编辑生成新需求集修订，不自动应用建议。
- 模型服务只编排既有服务；Registry只保存描述符，外部协议只允许在抽象Adapter之外实现。
- 所有参数化候选指标标记为 `PARAMETERIZED_MODEL/LOW`；当前路径上由多链路组合得到的
  PDR 标记为 `ESTIMATED/LOW`，不会伪装为协议实测。

## 配置与失败策略

默认剖面由 `NetworkProfileRepository::BuiltInDemo()` 提供。环境变量
`NRM_NETWORK_PROFILE_CONFIG` 可指向严格的 `NRM_NETWORK_PROFILES_V1` 文本配置。
配置必须包含 schema、config、provider、profileId、网络类型、频点、范围、建链时延、
候选 PDR、容量、传播模型、成员上限和业务类型。非法枚举、负值、PDR 越界、重复 ID 或
缺失字段会整体拒绝，不使用部分有效数据静默降级。

内部规划文件路径由显式调用或`NRM_PLAN_STORE_DIR`提供。该格式用于内部预验收，
不是甲方正式规划格式；外部格式必须经`NetworkPlanAdapter`转换。甲方规划格式、
专用校验规则和真实分发协议未提供前，不实现猜测性适配。

内部需求文件路径由显式调用或`NRM_DEMAND_STORE_DIR`提供。该格式用于内部预验收；
甲方正式需求格式和候选资源集合未提供时，不推导隐含门限或生成占位候选。

上报目录结构如下：

```text
${NRM_OUTPUT_DIR}/
└── run-<UTC>-<pid>-<timestamp>-<counter>/
    ├── manifest.json
    ├── resource_snapshots.jsonl
    ├── assessment_results.jsonl
    ├── capability_results.jsonl
    ├── plan_validation_results.jsonl
    ├── plan_evaluation_results.jsonl
    ├── resource_demand_results.jsonl
    ├── planning_recommendations.jsonl
    ├── network_summary.csv
    └── error.log                 # 仅发生错误时创建
```

每次进程启动创建新目录，不以截断方式覆盖旧运行。manifest 记录运行、schema、软件和配置
版本以及完成状态；写入失败和队列丢弃通过 `ReporterStatus` 暴露给 Warlock 面板。

本地分发包独立位于`<plan-store>/distribution_packages/<planId>/<revision>/`，不属于
仿真消息下发。包中规划副本状态为`READY_FOR_DISTRIBUTION`，原规划对象保持不变。

## 有界性与线程模型

- 生命周期记录、状态事件和上报队列均有容量上限。
- 路径搜索限制 `K=1..32`、最大跳数 `1..64` 和最大扩展状态数；结果携带搜索上限原因。
- 当前 demo 图规模较小，用户点击后在 GUI 线程执行确定性搜索。进入 200 节点压力场景或
  周期自动重评估前，应迁移到有界评估工作队列。
- Reporter 析构时通知工作线程停止，并在返回前排空已接收队列。

## 后续接入

甲方四网模块到位后，应新增 `CustomerModuleAdapter` 并转换为相同公共数据契约。内部
模型与外部模块可并存，以 `DataOrigin`、providerId、configVersion 和 profileId 区分来源。

导航第一版直接消费AFSIM `WsfNavigationErrors`实时对象和其内置`.neh`时序格式，输出统一
`NavigationSnapshot`；不实现GNSS、INS或融合算法。甲方以后提供不同格式时新增
`CustomerNavigationAdapter`，不替换公共值对象。地形、气象、天象和电磁环境第二
阶段先实现AFSIM内置状态与内部严格配置；甲方真实数据后续通过
`CustomerEnvironmentAdapter`转换到相同值对象，不修改评估核心。
