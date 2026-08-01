# Network Resource Manager v0.7.0 开发指令

## 1. 文档目的

本文档供后续开发AI直接执行，目标是在现有 v0.6.0 demo 上完成下一阶段最小必要闭环。开发必须保留现有可运行能力，不重写AFSIM核心，不把模拟数据标成真实合同结果。

目标版本：v0.7.0-contract-metrics-foundation。

本阶段聚焦：

1. 修正指标和任务评估的关键语义风险；
2. 将四类网络候选参数迁移到版本化配置；
3. 建立链路／成员状态事件账本；
4. 增加中断、拥塞和质量下降场景；
5. 补齐可恢复日志和验收证据。

本阶段不实现完整导航算法、完整规划文件生命周期、OA门户集成或自动建链／改频／改路由。

## 2. 当前稳定基线

### 2.1 代码基线

- 仓库：network_resource_manager
- 分支：main
- 稳定提交：8948cd0
- 稳定标签：v0.6.0-candidate-routing
- AFSIM：2.9.0
- 平台：Linux x86_64，GCC 13，Release
- 公共运行时版本：include/nrm/Version.hpp 中为0.6.0

### 2.2 已验证能力

- WSF扩展和Warlock插件可发现、编译、加载和回退；
- AFSIM通信图、网络、端点、链路和位置采集；
- 1／10／60秒滑动窗口；
- 当前图任务评估；
- 参数化候选图；
- 主路由和一条有向边不重合备选路由；
- JSONL／CSV异步上报；
- 五页Warlock面板和中央态势图；
- 三个现有回归测试全部通过。

开发前必须重新运行：

- nrm_framework_types_test
- nrm_snapshot_reporter_test
- nrm_assessment_evaluator_test

## 3. 审查发现与优先级

### 3.1 P0：链路PDR当前不能作为可靠评估输入

warlock/source/NrmSimInterface.cpp 的接收回调在同一时刻对链路同时调用 RecordTransmit 和 RecordReceive。未成功接收的链路尝试不会进入该链路窗口，因此链路PDR容易显示为100%。

要求：

- 分开记录链路尝试和成功交付；
- 如果无法获得可靠的链路发送分母，则链路PDR保持 valid=false；
- 不能用“成功接收次数／成功接收次数”生成100%；
- 网络级、链路级、业务级指标必须明确统计对象；
- 评估器只能消费统计语义明确且有效的数据。

### 3.2 P0：当前评估器只检查最低时延路径

include/nrm/AssessmentEvaluator.hpp 先按时延找到一条路径，再检查带宽、PDR和时延约束。如果最低时延路径带宽不足，而另一条路径满足全部条件，当前实现会错误输出“不满足”。

要求：

- 当前图优先原则保持不变；
- 在当前图中搜索有界候选路径集合，选择满足硬约束且预测时延最低的路径；
- 当前图无可行路径时，再在允许候选边的图中搜索；
- 如果没有可行路径，返回最佳诊断路径及失败原因；
- 禁止无界枚举所有简单路径。

推荐实现：

- 使用确定性的有界 Yen K-shortest simple paths；
- 默认 K=8，配置范围1至32；
- 默认最大跳数16；
- 每条候选路径统一进行路径级带宽、累计时延和PDR复核；
- 路径PDR暂按各跳条件独立的乘积估计，并标记 ESTIMATED／LOW 或 PARAMETERIZED_MODEL／LOW；
- 选择规则固定为：当前图优先、满足约束优先、时延升序、候选边数量升序、路径字典序。

### 3.3 P0：吞吐量语义混合

RollingMetrics 当前用 transmittedBits／window 生成 throughputBps。网络级调用记录发送比特，链路级却只在接收回调中补记发送，因此同一字段在不同层级表达不同含义。

要求新增并明确区分：

- offeredLoadBps：进入发送过程的业务比特率；
- deliveredThroughputBps：成功交付比特率；
- deliveryRatioPercent：基于同一消息生命周期或冻结口径的交付率；
- legacy throughputBps 暂时保留，明确兼容映射并标记弃用计划。

合同中的“网络吞吐量”默认映射到 deliveredThroughputBps；如甲方另有定义，再通过配置切换，不得静默改变。

### 3.4 P0：候选网络参数硬编码

include/nrm/AssessmentEvaluator.hpp 中的 CandidateRangeM、CandidateSetupDelayMs、CandidatePdrPercent 和 CandidateBandwidthBps 直接写死四类网络数值。

要求迁移到版本化 NetworkProfile：

- profileId；
- schemaVersion；
- networkType；
- protocolModel；
- frequencyHz或频段集合；
- maximumRangeM；
- establishmentDelayMs；
- candidatePdrPercent；
- serviceCapacityBps；
- propagationModelId；
- maximumMembers；
- supportedBusinessTypes；
- source、confidence和valid字段。

配置读取失败时不得回退到不透明默认值。可以加载内置演示剖面，但必须标记 providerId、profileId、configVersion、PARAMETERIZED_MODEL和LOW confidence。

### 3.5 P0：日志重启会覆盖旧证据

NrmSnapshotReporter 使用 std::ios::trunc 打开三类输出。进程重启会清空旧记录，且输出文件打开失败、队列丢弃没有可观察错误。

要求：

- 每次运行使用独立 runId目录，或采用不会覆盖旧数据的追加／轮转策略；
- 启动时记录schemaVersion、runId、configVersion和启动时间；
- 文件打开和写入失败必须进入错误日志；
- 队列溢出必须累计 droppedSnapshotCount和droppedAssessmentCount；
- 正常析构必须排空队列；
- 异常断电后再次启动不能破坏上一运行证据。

### 3.6 P1：版本与文档漂移

- 根目录VERSION仍为0.4.0，必须与include/nrm/Version.hpp统一；
- README仍写“导航计算”，应改为甲方导航结果包适配；
- README仍把候选图和备选路由列为后续功能，实际0.6.0已经完成；
- docs/ARCHITECTURE.md仍写导航融合计算，必须按当前职责边界修正。

## 4. 本阶段架构增量

### 4.1 新增公共类型

建议新增：

- include/nrm/NetworkProfile.hpp
- include/nrm/NetworkProfileRepository.hpp
- include/nrm/MessageLifecycleTypes.hpp
- include/nrm/ResourceEventTypes.hpp
- include/nrm/MetricReason.hpp

公共类型不得依赖Qt或AFSIM。

### 4.2 新增纯C++服务

建议新增：

- source/config/NetworkProfileRepository.cpp
- source/metrics/MessageLifecycleTracker.cpp
- source/repository/ResourceEventLedger.cpp
- source/evaluation/ConstrainedPathSelector.cpp

若当前AFSIM构建模板不适合独立源目录，可先把纯C++实现放到include/nrm的头文件中，但必须保持无Qt、无AFSIM依赖并可单元测试。

### 4.3 对现有接口的兼容要求

- 不删除NetworkResourceTypes中的已有字段；
- 不修改InputProvider现有三个虚函数签名；
- AssessmentEvaluator现有 Evaluate(snapshot, task) 调用必须继续可用；
- 可通过构造函数注入或新重载传入NetworkProfileRepository；
- nrm.assessment.v2字段继续输出；新增字段时升级schema或提供向后兼容字段；
- GUI继续只消费值对象，不持有AFSIM裸指针。

## 5. 任务分解

### T0：建立开发分支和基线证据

1. 从v0.6.0-candidate-routing建立 feat/v0.7-contract-metrics-foundation。
2. 记录git commit、AFSIM版本、编译器和测试命令。
3. 运行三个旧测试并保存输出。
4. 不提交output目录中的临时运行文件。
5. 不修改AFSIM核心源码。

完成条件：旧测试全部通过，基线结果写入docs/VALIDATION.md的新章节。

### T1：修正文档和版本漂移

修改：

- VERSION；
- README.md；
- docs/ARCHITECTURE.md；
- docs/IMPLEMENTATION_STATUS.md；
- CHANGELOG.md。

要求：

- 统一显示0.6.0作为开发起点；
- 导航统一表述为甲方GNSS／INS结果包解析、校验和标准化；
- 候选图与备选路由标为已完成；
- v0.7新增能力不得在实现和测试完成前标记完成。

完成条件：全文搜索不再出现“导航计算”“导航融合计算”或“候选图将在后续加入”等过期表述。

### T2：实现版本化NetworkProfile

1. 定义强类型NetworkProfile和校验结果。
2. 建立四类内置演示剖面，数值沿用0.6.0硬编码值，保证回归一致。
3. 增加外部配置加载入口。
4. 配置必须携带schemaVersion、configVersion和profileId。
5. 非法枚举、负数、PDR超界、重复profileId、缺失必需字段必须拒绝，并返回固定原因码。
6. AssessmentEvaluator不再直接包含四类Candidate常量。
7. 快照、评估结果和上报记录携带configVersion／profileId来源。

限制：

- 不引入未经批准的新第三方依赖；
- 优先复用AFSIM已有配置或序列化能力；
- 如外部JSON／YAML解析条件不成熟，可先实现内置剖面和纯C++配置对象注入，不可用手写字符串切割冒充正式解析器。

完成条件：

- 改配置无需重新编译即可改变候选范围、建链时延、PDR和容量；
- 未配置或非法配置时指标为无效并带原因码；
- 默认演示剖面保持0.6.0评估结果。

### T3：实现消息生命周期和严格指标基础

1. 建立MessageLifecycleTracker，至少记录：
   - messageId；
   - networkId；
   - sourceEndpointId；
   - destinationEndpointId；
   - businessType；
   - bits；
   - queuedTime；
   - transmittedTime；
   - terminalTime；
   - terminalState：DELIVERED、DISCARDED、ROUTING_FAILED、EXPIRED。
2. 网络级发送和接收必须来自同一生命周期口径。
3. 链路级指标只在发送尝试和结果能可靠关联时有效。
4. 增加offeredLoadBps和deliveredThroughputBps。
5. 增加P50／P95传输时延和排队时延的有界计算。
6. 不能无限保存消息；超时条目转为EXPIRED或按固定规则清理并计数。
7. 重复回调不得重复计数。

完成条件：

- 10条消息发送、8条交付、1条丢弃、1条路由失败时，严格交付率为80%；
- 跨窗口迟到消息按文档规定的同一口径统计；
- 重复接收不增加交付数；
- 只有接收事件而无发送分母时PDR无效；
- 原1／10／60秒有界内存特性保留。

### T4：实现链路和成员状态事件账本

ResourceEventLedger至少记录：

- endpoint online／offline／disabled／failed转换；
- link enabled／disabled转换；
- establishment attempted／succeeded／failed；
- 状态起止时间；
- reasonCode；
- networkId、linkId和相关端点。

由账本形成：

- currentOfflineDuration；
- windowOfflineDuration；
- establishmentAttempts；
- establishmentSuccesses；
- establishmentSuccessRatio；
- averageEstablishmentDelayMs；
- endpointOnlineRatio；
- serviceAvailabilityPercent。

若AFSIM没有真实建链尝试事件：

- 对应指标保持valid=false；
- 参数化模型可单独输出estimatedEstablishmentDelay；
- 不得把“图中存在一条边”当成一次真实建链成功。

完成条件：状态切换测试能精确复现离线时长、在线率和成功率，并对缺少事件的指标输出无效原因。

### T5：实现有界约束路径选择

1. 将图构建、路径枚举和路径评估从AssessmentEvaluator的大头文件中拆分。
2. 当前边和候选边继续严格区分。
3. 当前图先搜索K条路径并进行路径级约束复核。
4. 当前图无可行路径后，再允许候选边。
5. 带宽按瓶颈值；
6. 时延按各跳和建链时延之和；
7. PDR按各跳概率乘积估计；
8. 路径中的任一必需指标无效时，不得判定canComplete=true。
9. 备选路由继续采用有向边不重合语义，并在结果中明确disjointnessType。
10. 输出consideredPathCount、selectedPathRank和失败约束摘要。

完成条件：

- 路径A为10 ms但带宽不足，路径B为20 ms且满足约束时，必须选择路径B并判定完成；
- 当前图存在可行路径时不得选择候选路径；
- 多跳PDR计算和路径级复核有独立测试；
- K和最大跳数均有边界测试；
- 相同输入多次运行结果完全一致。

### T6：实现可靠上报和恢复

1. 输出目录按runId隔离。
2. 新增manifest.json或等价清单，记录版本、配置、来源和文件列表。
3. JSONL每条记录携带schemaVersion、runId和configVersion。
4. 输出失败写入error.log，并向GUI暴露最简状态。
5. 队列溢出计数进入运行状态。
6. 增加异常数据日志，包含时间、组件、原因码和字段，不记录敏感原始载荷。
7. 增加启动恢复测试：旧运行目录不被覆盖，新运行正常生成文件。

完成条件：连续运行两次保留两套完整证据；只读目录下启动时产生可诊断失败，不崩溃；析构后已入队记录全部写完。

### T7：增加三个演示故障场景

新增或扩展test_mission：

1. link_failure：运行中禁用一条主链路；
2. congestion：提高业务流量，使容量或队列约束失败；
3. quality_degradation：降低链路质量或使用参数化低PDR剖面。

每个场景必须产生：

- 运行前、故障时、恢复后的快照；
- 评估结论变化；
- 原因码；
- 状态事件；
- JSONL／CSV证据；
- 一张Warlock截图。

GUI只增加完成验收所需的字段或状态，不进行大规模重绘。

## 6. 新增测试要求

建议新增：

- tests/NetworkProfileTest.cpp
- tests/MessageLifecycleTrackerTest.cpp
- tests/ResourceEventLedgerTest.cpp
- tests/ConstrainedPathSelectorTest.cpp
- tests/SnapshotReporterRecoveryTest.cpp

CMake要求：

- 保留三个旧测试目标；
- 增加聚合目标nrm_tests，显式构建全部测试；
- ctest必须能够发现已构建测试；
- 不强迫AFSIM默认全量构建编译测试目标。

测试至少覆盖：

- 正常路径；
- 空输入；
- 非法配置；
- 缺失指标；
- 重复事件；
- 乱序事件；
- 超时清理；
- K值和最大跳数边界；
- 文件打开失败；
- 输出队列溢出；
- 两次启动不覆盖；
- 旧0.6.0结果兼容。

## 7. 合同覆盖目标

v0.7完成后应至少推进以下条目：

- 3.2.2：四类数据链的版本化参数剖面和频点字段基础；
- 3.2.3：距离、速率、丢包率、时延、吞吐量、接入率的清晰数据契约；
- 3.2.5：脱网时长、建链时长、建链成功率、工作状态、成员在网率、资源占用率、队列占用率、业务流量、路由和消息统计；
- 3.4.2：异常日志和重启后正常工作；
- 4.2：模块化、高内聚低耦合和组件化测试证据。

状态只能按以下规则更新：

- MAPPED：存在设计和字段映射；
- IMPLEMENTED：代码和单元测试完成；
- VERIFIED：AFSIM场景和验收证据完成；
- PRE_ACCEPTANCE：使用内部剖面或模拟输入；
- FINAL_ACCEPTANCE：使用甲方接口、样包和目标环境。

## 8. 明确禁止事项

开发AI不得：

- 修改AFSIM核心源码；
- 删除或重命名已有公共字段；
- 让Qt或AFSIM类型进入include/nrm公共契约；
- 在仿真回调中执行文件I/O或复杂路径搜索；
- 把缺失数据自动补成0并标记有效；
- 把参数化候选值描述成真实协议测量；
- 实现GNSS／INS导航算法；
- 在甲方接口文件到位前猜测二进制包格式；
- 自动执行建链、改频或改路由；
- 新增大型框架或未经批准的第三方依赖；
- 为了增加页面数量而复制相同数据；
- 修改现有输出语义但不升级schema和文档；
- 在旧测试未通过时继续叠加新功能。

## 9. 提交顺序

建议使用以下小提交：

1. chore: align v0.6 version and documentation
2. feat: add validated network profiles
3. feat: add message lifecycle metrics
4. feat: add resource state event ledger
5. fix: select feasible constrained route
6. feat: add recoverable run reporting
7. test: add failure and recovery scenarios
8. docs: record v0.7 validation evidence

每个提交必须能够编译；第2至第6项必须包含对应测试。

## 10. v0.7完成定义

必须同时满足：

- 原三个测试和全部新增测试通过；
- WSF与Warlock插件重新编译并加载；
- 原四网demo仍显示4个网络、8个端点和8条有向链路；
- 原任务评估和中央主备路由高亮仍可使用；
- 四类候选参数不再硬编码在AssessmentEvaluator；
- 链路PDR不会因只统计成功接收而恒为100%；
- 存在可行替代路径时不会错误输出不满足；
- 三个故障场景产生完整证据；
- 两次启动不会覆盖上一运行输出；
- 代码未修改AFSIM核心；
- 文档、VERSION、cVERSION和CHANGELOG一致；
- IMPLEMENTATION_STATUS不把模拟输入标为最终验收。

## 11. 后续版本顺序

v0.7完成后按以下顺序推进：

1. v0.8：CommunicationCapabilityService与地形／气象／天象／电磁环境适配；
2. v0.9：规划文件加载、编辑、校验推演、存储和回环分发；
3. v0.10：甲方导航结果包适配，仅在接口文件和样包到位后开发；
4. 最终联调：甲方模型封装、模型构建工具、OA／保密环境和第三方参考模块。

## 12. 可直接交给开发AI的执行指令

请基于network_resource_manager的v0.6.0-candidate-routing标签开发v0.7.0-contract-metrics-foundation。先阅读README、ARCHITECTURE、VALIDATION、IMPLEMENTATION_STATUS、contract_coverage.yaml和requirement_traceability.yaml，并运行三个现有测试。严格保持AFSIM核心零修改、公共数据契约无Qt／AFSIM依赖、仿真回调只复制和计数、GUI只消费值对象。

按T0至T7顺序执行。第一阶段只修版本文档、实现NetworkProfile及其校验、替换AssessmentEvaluator内四类硬编码参数；第二阶段实现消息生命周期、状态事件账本和严格指标；第三阶段实现有界K路径的约束可行路由；第四阶段实现运行隔离上报和三个故障场景。每项必须同步增加测试、文档和验收证据。

不要实现导航算法，不要猜甲方接口，不要自动控制网络，不要把无效指标补0，不要改变旧字段语义而不升级schema。遇到AFSIM API无法提供真实分母或建链事件时，保持指标invalid并记录固定原因码。完成后汇报修改文件、构建命令、所有测试结果、场景输出、合同条目状态变化和仍受外部资料阻塞的项目。
