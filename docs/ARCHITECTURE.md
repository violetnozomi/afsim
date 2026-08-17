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
  ├── CustomerIngestionState：甲方消息去重、乱序拒绝和运行隔离
  ├── CustomerSnapshotAssembler：资源全量域、导航平台和环境子域原子合并
  ├── EffectiveSnapshotAssembler：AFSIM基础态与Customer粒度化覆盖层仲裁
  ├── ResourceSnapshotValidator：JSON与C++入口共用的对象关系语义校验
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

v0.12 的甲方运行时入口由 `CustomerNrmAdapter → CustomerIngestionState →
CustomerSnapshotAssembler → ResourceSnapshotValidator → DataContainer → ModelServiceFacade`
组成。它是同一AFSIM进程内的强类型C++边界，不依赖Qt、JSON或网络协议。
`CustomerJsonValidationLayer`和`CustomerJsonCodec`只负责文件导入、测试、回放及验收所需的
JSON结构、字段类型、范围和枚举校验；解码后的对象关系由同一个纯C++
`ResourceSnapshotValidator`校验，直接C++入口不得绕过。生命周期状态机按
`runId/messageId/simTime/消息域`拒绝重复和迟到输入；资源报告不会再清空导航与环境，导航
按平台upsert。外部任务评估和Warlock手工评估统一调用
`DataContainer::EvaluateAssessment()`，并由该入口进入`ModelServiceFacade`；并发资源需求JSON
进入需求仓库和同一门面的匹配服务。快照版本变化后旧评估、能力和规划推演缓存立即失效。

运行期采用明确的Customer Overlay仲裁，不再使用隐式last-writer-wins：AFSIM快照存在时，
AFSIM始终拥有网络、端点、链路、业务流和网关的权威基础态；Customer导航只保存其真实
上报的平台，按`platformId`覆盖同平台AFSIM导航，未上报平台继续使用最新AFSIM状态；
Customer环境按`terrain/weather/celestial/interference`可用子域覆盖，未上报子域保留AFSIM值。
`EnvironmentContext.customerProvidedDomains`由JSON Codec或同进程Adapter根据本次实际输入推导；
Customer的`applicationMode`只控制这些子域，不能改变未上报的AFSIM地形、气象、天象或干扰语义。
后续AFSIM周期快照不会清除Customer覆盖层，较旧的Customer资源报告也不能回写覆盖最新AFSIM链路。
尚未取得AFSIM基础态时，完整甲方资源快照可作为独立演示/回放基础态。每次合成发布都生成
单调递增的有效`snapshotVersion`，所有派生结果按该版本失效。

独立任务评估与通信能力查询共享环境语义。`INFORMATION_ONLY`只携带证据，
`ALREADY_INCLUDED`表示上游链路指标已经包含环境影响，二者均不硬阻断、不衰减、不增加时延；只有
`CANDIDATE_ADJUSTMENT`通过既有`CommunicationCapabilityService`环境链修正带宽、时延、
PDR和硬阻断，再保守回填任务评估结论。效果证据使用子域的
`AFSIM_INTERNAL/CUSTOMER_MODULE`来源，不把Customer阻断误报为AFSIM地形遮挡。这里不复制环境
公式，也不改变评估得到的路由。

评估、规划需求和并发资源需求统一规定`maximumDelayMs=0`为“不设置最大时延约束”，正值才
启用时延门限，负值拒绝。该规则在C++领域入口、JSON运行时解码和正式Schema中保持一致。

v0.11第一阶段增加`ModelServiceFacade`作为进程内强类型编排边界。Facade先校验
`ModelServiceContext`的schema、请求标识和snapshotVersion，再分别单次委托现有任务评估、
通信能力、规划校验、规划推演、本地分发包或需求匹配服务。领域结果不被转换成自由文本，异常不会
静默变为成功，输入值对象和profile保持不变。

`ModelRegistry`只保存`ModelDescriptor`值对象，使用精确版本查询和确定性列表顺序，不扫描
动态库、不拥有AFSIM对象或Qt指针。`ContractInterfaceAdapter`只冻结甲方私有对象到公共值
对象的转换职责；甲方头文件缺失时不伪造其对象类型。`CustomerNrmAdapter`是已实现的同进程
执行入口，`CustomerJsonCodec`是精简V1文件工具。DataContainer注册唯一NRM Facade并
复用该门面，Warlock页面仍只消费既有领域结果。

## 稳定边界

- `include/nrm/` 中的公共契约和算法不依赖 AFSIM 或 Qt。
- WSF 扩展入口只负责能力注册；`NrmSimInterface` 是当前 AFSIM 内部监听适配器。
- `DataContainer` 只存在于 GUI 线程，跨线程事件只传递值对象。
- 甲方同类型多网络按具体`networkId`隔离；`networkType`仅用于四网分类。
- AFSIM拥有拓扑和实时链路基础态；导航按平台、环境按子域使用Customer持久覆盖，未取得AFSIM快照时才允许
  甲方完整资源报告作为基础态。
- JSON结构校验和C++对象语义校验职责分离；所有资源入口共用
  `ResourceSnapshotValidator`，不得在Codec中维护第二套跨对象规则。
- `SnapshotReporter` 使用有界队列和独立线程，仿真回调不执行文件 I/O。
- `InputProvider` 冻结内部、甲方模块和回放输入的公共边界。
- `AssessmentEvaluator` 不改变 AFSIM 图，只产生评估结果和只读建议。
- 规划服务只产生校验、推演和本地包；编辑生成新修订的`DRAFT`，不向网络下发。
- 每个规划资源分配至少包含一个成员；空`members`同时被Schema、Codec和规划校验器拒绝。
- 需求服务只产生匹配、差距和建议；编辑生成新需求集修订，不自动应用建议。
- 模型服务只编排既有服务；Registry只保存描述符；甲方对象转换限定在
  `ContractInterfaceAdapter`实现中，同进程执行统一进入`CustomerNrmAdapter`。
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
