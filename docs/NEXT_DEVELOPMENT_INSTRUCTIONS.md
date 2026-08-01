# Network Resource Manager v0.10 开发指令

## 1. 本轮唯一目标

实现合同3.2.5第一阶段的**网链资源需求管理、匹配结论、不满足项分析与可解释规划建议**。

开发必须从当前`feat/v0.9-network-plan-lifecycle`分支顶端开始，该顶端包含v0.9稳定代码和
本指令。建议新建分支：

```bash
git switch feat/v0.9-network-plan-lifecycle
git switch -c feat/v0.10-demand-matching
scripts/ai_guard.sh status
scripts/ai_guard.sh static
scripts/ai_guard.sh test
```

开始前记录11项基线测试。一次只完成本文件规定的v0.10任务，不进入导航适配、环境传播、
真实规划分发、自动资源控制、优化算法或研究实验。

## 2. 合同边界

- 目标版本：`0.10.0-demand-matching`。
- 主合同条款：3.2.5网链资源管理模型。
- 复用条款：3.2.3通信能力结果、3.2.4规划文件与只读推演。
- 完成后最多标记内部`PRE_ACCEPTANCE`。
- “满足/不满足”只针对已输入且数据有效的显式约束。
- “规划建议”只在调用方提供的有限候选集合中按确定性规则筛选和排序。
- 缺少候选频率、站点、信道、子网或时隙时，必须输出固定不可用原因，不得生成占位值。
- 所有输出均为分析和建议，不得修改AFSIM运行网络、规划文件或资源快照。

## 3. 必须形成的用户闭环

Warlock中形成以下最小流程：

1. 加载内部需求文件，或在表格中录入需求；
2. 按任务阶段、业务类型查看需求；
3. 选择同一版本的资源快照和可选规划修订；
4. 执行批量需求匹配；
5. 查看每条需求的`SATISFIED / UNSATISFIED / DATA_INVALID`结论；
6. 查看不满足项、要求值、当前值、裕量、单位和原因码；
7. 查看六类建议的`AVAILABLE / UNAVAILABLE`状态及证据；
8. 将结果保存为独立JSONL审计记录；
9. 卸载需求集，不影响规划和实时快照。

不要增加自动应用、自动调参、地图编辑或复杂可视化。

## 4. 必须复用的现有组件

- `ResourceSnapshot`及其version、origin、confidence、valid语义；
- `CapabilityRequest / CapabilityResult`；
- `CommunicationCapabilityService`；
- `NetworkPlanDocument`、`NetworkPlanEvaluationResult`和规划内容指纹；
- `NetworkProfileRepository`；
- `SnapshotReporter`的有界队列、运行隔离和析构排空；
- DataContainer值对象边界和Warlock现有表格模式。

禁止复制路径搜索、距离、带宽、PDR、时延、吞吐量或接入率计算。

## 5. 公共契约

建议新增以下无Qt/AFSIM依赖的文件：

- `include/nrm/ResourceDemandTypes.hpp`；
- `include/nrm/ResourceDemandRepository.hpp`；
- `include/nrm/ResourceDemandMatchingService.hpp`；
- `include/nrm/PlanningRecommendationEngine.hpp`；
- `include/nrm/ResourceDemandSerialization.hpp`。

### 5.1 需求对象

`ResourceDemand`至少包含：

- `schemaVersion`、`demandId`、`demandSetId`、`revision`；
- `missionStage`、`businessType`；
- `sourcePlatform`、`destinationPlatform`；
- `payloadBits`、`businessTrafficBps`、`requiredBandwidthBps`；
- `maximumDelayMs`、`minimumPdrPercent`、`maximumDistanceM`；
- `minimumNetworkSize`；
- `allowedNetworks`；
- `source`、`confidence`、`valid`。

字段语义固定：

- `minimumNetworkSize`是当前允许网络中在线成员数的下限；
- `maximumDistanceM`是所选路径总通信距离上限；
- `businessTrafficBps`与`requiredBandwidthBps`均为硬输入，能力带宽必须满足二者较大值；
- 值为0表示调用方未设置该项约束，但非有限值和负值必须拒绝；
- 不从`businessType`、文件名或自由文本推导隐含门限。

### 5.2 匹配结果

定义：

- `DemandMatchStatus`：`SATISFIED`、`UNSATISFIED`、`DATA_INVALID`；
- `RequirementItemType`：`PATH`、`NETWORK_SIZE`、`DISTANCE`、`BANDWIDTH`、
  `TRAFFIC`、`DELAY`、`PDR`、`BUSINESS_TYPE`；
- `RequirementCheck`：类型、是否适用、是否通过、要求值、当前值、裕量、单位、原因码；
- `ResourceDemandMatchResult`：需求标识、snapshotVersion、planId/revision/fingerprint、
  整体状态、`CapabilityResult`、逐项检查和建议集合；
- `ResourceDemandBatchResult`：需求集标识、版本、快照、总数、三类状态计数和逐需求结果。

裕量方向必须统一为“可用能力减去需求”或“允许上限减去实际值”，正数表示满足。无有效数据时
不写0，保持`valid=false`并返回固定原因码。

### 5.3 建议对象

定义：

- `RecommendationType`：`FREQUENCY`、`STATION`、`CHANNEL`、`SUBNET`、
  `TIMESLOT`、`ROUTE`；
- `RecommendationStatus`：`AVAILABLE`、`UNAVAILABLE`；
- `PlanningRecommendation`：类型、目标需求、候选ID、建议值、排序、snapshotVersion、
  planId/revision/fingerprint、证据、原因码、来源和置信度。

程序判断不得依赖自由文本。

## 6. 内部需求文件与Repository

建立严格版本化文本格式`NRM_RESOURCE_DEMAND_V1`。完整文法先写入
`docs/features/网链资源需求匹配.md`，再实现解析器。字符串使用`std::quoted`，数值严格检查。

Repository至少提供：

- `LoadFromFile`；
- `ReplaceDraft`；
- `Unload`；
- `SaveRevision`；
- `GetCurrentDemandSet`；
- 最后一次操作结果。

要求沿用v0.9：失败加载不覆盖当前有效对象；保存使用临时文件加原子rename；目标文件拒绝
覆盖；活动同修订拒绝静默替换；显式卸载后允许重载同一文件；round-trip语义等价。

## 7. 匹配算法

`ResourceDemandMatchingService::Evaluate`输入：

- `const ResourceSnapshot&`；
- `const ResourceDemandSet&`；
- 可选`const NetworkPlanDocument*`；
- 可选`const NetworkPlanEvaluationResult*`；
- 可选`EnvironmentContext`。

每条需求严格执行：

1. 校验ID、端点、网络类型、数值范围和快照有效性；
2. 无损映射到一个`CapabilityRequest`；
3. 调用一次现有`CommunicationCapabilityService::Query`；
4. 从同一结果生成PATH、DISTANCE、BANDWIDTH、TRAFFIC、DELAY和PDR检查；
5. 从同一快照生成NETWORK_SIZE检查；
6. BUSINESS_TYPE复用profile支持关系，不建立第二套业务规则；
7. 任一必需数据无效时整体为`DATA_INVALID`；
8. 数据有效但任一适用硬约束失败时为`UNSATISFIED`；
9. 全部适用检查通过时为`SATISFIED`。

若同时输入规划和规划推演结果，必须验证planId、revision、planFingerprint和snapshotVersion
一致。身份不一致时拒绝复用规划结果，不得拼接不同版本证据。

结果顺序按输入需求顺序；原因码和建议必须采用固定稳定排序，保证相同输入字节级可复现。

## 8. 可解释规划建议

`PlanningRecommendationEngine`只消费匹配结果、当前快照、可选规划和调用方显式提供的候选
资源。首版规则如下：

| 类型 | 允许的首版行为 |
| --- | --- |
| 频率 | 从规划allocation/profile明确列出的兼容频点中排除已检测冲突，按频点升序选择首个 |
| 站点 | 从显式候选成员中选择能形成满足约束路径者，先比满足状态，再比最小裕量，最后按ID |
| 信道 | 从显式未占用channel候选中按ID选择，不推导频谱复用规则 |
| 子网 | 仅推荐显式支持该businessType且成员容量未超限的subnet候选 |
| 时隙 | 从显式未占用slot候选中按ID选择，不推导TDMA保护或复用关系 |
| 路由 | 复用能力结果中的已选route；不重新实现Dijkstra或Yen算法 |

每一类都必须输出一条状态。无候选、证据无效或甲方规则缺失时返回`UNAVAILABLE`及明确原因，
例如`CANDIDATE_DATA_UNAVAILABLE`、`CUSTOMER_RULE_UNAVAILABLE`或`NO_FEASIBLE_CANDIDATE`。

严禁：

- 自动修改规划；
- 自动建链、改频、改信道、改子网、分时隙或改路由；
- 连续空间站点优化、整数规划、遗传算法、强化学习或参数扫描；
- 为了让结论通过而修改demo profile。

## 9. DataContainer、Reporter与Warlock

DataContainer负责持有需求Repository和最近一次批量匹配结果，提供加载、卸载、编辑新修订、
评估和查询接口。GUI不得直接访问AFSIM对象或重新计算指标。

Reporter新增：

- `resource_demand_results.jsonl`；
- `planning_recommendations.jsonl`；
- manifest中的文件名、队列丢弃计数和写错误计数。

两类队列必须有界并在析构前排空。

Warlock新增或扩展一个“需求匹配”页，仅包含需求表、匹配结果表、不满足项表和六类建议表。
编辑后形成新需求集revision；旧结果必须失效。不要制作仪表盘、动画或地图。

## 10. 测试要求

新增至少两个纯C++测试：

1. `nrm_resource_demand_repository_test`；
2. `nrm_resource_demand_matching_test`。

Repository测试覆盖合法加载、卸载后重载、round-trip、原子保存、重复ID、revision错误、未知
记录、尾随token、NaN/Inf、负值、PDR越界和失败加载保护。

Matching测试至少覆盖：

- 完全满足；
- 无路径；
- 网络规模不足；
- 距离、带宽、业务流量、时延和PDR分别不满足；
- 关键指标无效得到`DATA_INVALID`；
- 需求只调用一次能力查询链；
- 规划内容指纹或snapshotVersion不一致；
- 六类建议稳定排序；
- 候选缺失时六类均有明确`UNAVAILABLE`原因；
- 评估前后快照、规划、需求集和profile不变；
- Reporter队列溢出、恢复和析构排空。

完成后应有13项固定测试。不得删除或放宽原11项断言。

## 11. 固定场景

只有存在能真正调用需求匹配服务的安全入口时，才新增`resource_demand_matching_smoke`。
普通通信消息交付或仅加载插件不能作为该服务证据。没有安全入口时记录为阻塞，状态保持
`IMPLEMENTED / PRE_ACCEPTANCE`，不得伪造场景。

## 12. 允许修改范围

- `include/nrm/`；
- `warlock/source/`；
- `tests/`；
- 必要的`test_mission/`；
- `CMakeLists.txt`、`scripts/ai_guard.sh`；
- 版本、README、CHANGELOG、docs和data追踪文件。

不得修改AFSIM核心、合同原文基线、系统服务、OA配置、ns-3论文目录或与本任务无关的已有
指标语义。

## 13. 实施顺序

1. T0：从当前`feat/v0.9-network-plan-lifecycle`顶端建分支并记录11项基线；
2. T1：冻结需求文法、枚举、原因码和裕量方向；
3. T2：实现Demand Repository及非法输入测试；
4. T3：实现MatchingService并复用CapabilityService；
5. T4：实现有限候选RecommendationEngine；
6. T5：接入DataContainer和Reporter；
7. T6：最后接入Warlock页面；
8. T7：静态检查、13项测试、插件构建和可行时的一次固定场景；
9. T8：同步版本、合同追踪、验证和交接文档；
10. T9：向用户汇报审查结果，获得明确授权后提交。

不得先做GUI再补公共契约和测试。

## 14. 完成门

- 原11项测试无回归，新增2项测试通过；
- 匹配结果包含明确状态和逐项差距，不以自由文本代替原因码；
- 六类建议各有`AVAILABLE`或明确`UNAVAILABLE`结果；
- 不同plan/snapshot版本的证据不能混用；
- 相同输入结果顺序稳定；
- 所有评估只读，不改变快照、规划、需求或实时网络；
- WSF和Warlock插件构建成功；
- `scripts/ai_guard.sh static/test`与`git diff --check`通过；
- 无`.orig/.rej/.bak`、build或output文件进入提交；
- AFSIM核心零修改；
- 文档只标记内部`PRE_ACCEPTANCE`；
- Git提交前获得用户明确批准。

## 15. 最终汇报格式

按以下顺序汇报：唯一目标、实际改动、需求格式、匹配规则、建议规则、测试结果、固定场景、
合同3.2.5状态、外部阻塞、下一步唯一建议。未运行的验证必须明确写出，不得使用“基本完成”
或以参数调整掩盖失败。
