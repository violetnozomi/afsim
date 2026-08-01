# Network Resource Manager v0.9 开发指令

## 1. 给开发AI的执行命令

本轮唯一目标是实现合同3.2.4的第一阶段：**网链资源规划文件生命周期与只读校验推演**。

必须从稳定提交`7c76b9410e5d221ff92b1b8e1b94b51650d8a409`开始，在分支
`feat/v0.9-network-plan-lifecycle`开发。开始前按`AGENTS.md`读取项目记忆并运行：

```bash
scripts/ai_guard.sh status
scripts/ai_guard.sh static
scripts/ai_guard.sh test
```

先记录9项基线测试结果。一次只完成本文件规定的v0.9任务，不进入环境适配、导航解析、
自动资源调度或研究实验。

## 2. 状态与合同边界

- 目标版本：`0.9.0-network-plan-lifecycle`。
- 合同条款：3.2.4网链资源规划模型，兼顾3.2.5需求匹配输入。
- 当前证据等级：开发完成后最多标记内部`PRE_ACCEPTANCE`。
- 甲方规划文件格式、分发协议和模型封装规范尚未提供，不得猜测其字段、字节序或协议。
- 本阶段建立稳定的内部规范`nrm.network_plan.v1`和外部适配器边界。
- “分发”在本阶段仅指生成不可变、已校验的分发包文件；不得向运行网络下发命令。
- “推演”是基于不可变快照和现有能力服务的只读评估；不得修改AFSIM链路、频率、路由或
  时隙。

## 3. 必须交付的用户流程

Warlock中应形成以下最小闭环：

1. 加载内部规划文件；
2. 查看规划元数据、网络资源分配和业务需求；
3. 在受控表格中编辑草案；
4. 执行结构与引用校验；
5. 对每条业务需求执行只读能力推演；
6. 查看通过项、不满足项、原因码和能力结果；
7. 保存新修订版；
8. 仅当校验和推演均通过时生成分发包；
9. 卸载当前规划，且不影响实时资源快照。

不得为了界面完整度加入地图绘制、自动优化、复杂图表或与规划无关的页面。

## 4. 公共数据契约

新增文件建议如下：

- `include/nrm/NetworkPlanTypes.hpp`
- `include/nrm/NetworkPlanRepository.hpp`
- `include/nrm/NetworkPlanValidator.hpp`
- `include/nrm/NetworkPlanEvaluationService.hpp`

公共代码不得依赖Qt或AFSIM。

### 4.1 枚举

至少定义：

- `NetworkPlanState`：`DRAFT`、`VALIDATED`、`REJECTED`、
  `READY_FOR_DISTRIBUTION`；
- `PlanValidationReason`：固定原因码，不使用自由文本作为程序判断条件；
- `PlanChangeType`：`JOIN`、`LEAVE`，仅表示待校验的动态入退网申请；
- `PlanEvaluationStatus`：`NOT_EVALUATED`、`PASS`、`FAIL`、`DATA_INVALID`。

### 4.2 规划文档

`NetworkPlanDocument`至少包含：

- `schemaVersion`、`planId`、`revision`、`configVersion`；
- `providerId`、`createdTime`、`state`；
- 网络资源分配集合；
- 业务需求集合；
- 动态入退网申请集合；
- 来源、置信度和有效性；
- 前一修订版标识，可为空。

禁止用文件名代替`planId/revision`。

### 4.3 网络资源分配

每条`NetworkPlanAllocation`至少包含：

- `allocationId`；
- `networkName`和`NetworkType`；
- `profileId`；
- `frequencyHz`；
- `channelId`、`subnetId`；
- `slotIds`；
- `memberPlatformIds`；
- `routePolicyId`；
- `enabled`。

`channelId`、`subnetId`、`slotIds`和`routePolicyId`在内部只是显式配置值。甲方规则未提供
前，不得为其编造协议语义。

### 4.4 业务需求

每条`NetworkPlanDemand`至少包含：

- `demandId`、`businessType`；
- `sourcePlatform`、`destinationPlatform`；
- `payloadBits`；
- `requiredBandwidthBps`；
- `maximumDelayMs`；
- `minimumPdrPercent`；
- `allowedNetworks`。

该结构必须可无损映射到现有`CapabilityRequest`，不得复制通信能力计算公式。

### 4.5 结果对象

至少定义：

- `PlanValidationIssue`：原因码、字段、记录ID、严重级别和简短说明；
- `PlanValidationResult`：规划标识、是否通过、问题集合；
- `PlanDemandEvaluation`：需求ID、状态、`CapabilityResult`和失败原因；
- `NetworkPlanEvaluationResult`：规划标识、快照版本、整体状态和逐需求结果；
- `DistributionPackageResult`：是否生成、输出路径、规划修订和原因码。

## 5. 内部规划文件格式

采用严格、版本化、可测试的文本格式`NRM_NETWORK_PLAN_V1`。解析方式复用
`NetworkProfileRepository`的严格流式读取和`std::quoted`风格，不引入第三方JSON/YAML库，
也不使用不受控字符串切割。

建议记录类型：

```text
NRM_NETWORK_PLAN_V1 "planId" revision "configVersion" "providerId"
ALLOCATION ...
MEMBER ...
SLOT ...
DEMAND ...
CHANGE ...
```

开发AI需要在实现前把完整语法写入`docs/features/网链资源规划文件.md`。解析器必须：

- 拒绝未知schema和未知记录类型；
- 拒绝缺失字段、重复ID、尾随非法token和非有限数值；
- 对字符串使用带引号读取；
- 文件打开或解析失败时不覆盖仓库中最后一个有效规划；
- 保存采用临时文件加原子重命名，失败时保留旧修订版；
- 加载后再保存必须保持语义等价。

不得声称该内部格式就是甲方格式。后续甲方格式通过`NetworkPlanAdapter`转换为公共值对象。

## 6. Repository职责

`NetworkPlanRepository`只负责：

- `LoadFromFile`；
- `ReplaceDraft`；
- `Unload`；
- `SaveRevision`；
- `GetCurrentPlan`；
- 查询最后一次加载/保存结果。

要求：

- 仓库持有值对象，不持有AFSIM或Qt对象；
- revision必须单调增加；
- 同一`planId + revision`不得静默覆盖；
- 加载失败不改变当前有效状态；
- 卸载只清除规划状态，不修改实时网络快照；
- 文件路径来自显式调用或`NRM_PLAN_STORE_DIR`，不得写入AFSIM安装目录。

## 7. Validator职责

`NetworkPlanValidator`必须执行确定性校验，至少覆盖：

### 7.1 基础校验

- schema、规划ID、revision和providerId有效；
- 所有浮点数有限且范围合法；
- allocation、demand和change ID唯一；
- 网络类型不是`UNKNOWN`；
- PDR在0至100之间，带宽、时延、payload和频率非负；
- 源和目的平台不同。

### 7.2 引用校验

- `profileId`存在于`NetworkProfileRepository`；
- 规划频率属于对应profile的频率集合；
- allocation成员、需求端点和入退网平台能在当前快照或明确的规划成员集合中解析；
- allowed network与allocation/profile一致；
- business type受profile支持；通配符`*`沿用现有语义。

### 7.3 容量与冲突校验

- allocation成员数不超过profile的`maximumMembers`；
- 同一平台不得在同一allocation中重复；
- 同一`networkName/channelId/slotId`资源键不得出现重复独占分配；
- 同一change不得同时JOIN和LEAVE同一规划资源；
- 已在网成员重复JOIN、非成员LEAVE必须给出固定原因码；
- 不得凭空定义频率间隔、保护带、TDMA复用距离或干扰门限。

缺失甲方规则的数据只能产生`valid=false`或明确的“规则不可用”原因，不能默认通过关键
合同判断。

## 8. 只读校验推演

`NetworkPlanEvaluationService`输入：

- `const ResourceSnapshot&`；
- `const NetworkPlanDocument&`；
- `const NetworkPlanRepository/NetworkProfileRepository&`所需只读配置；
- 可选`EnvironmentContext`。

执行顺序固定：

1. 先调用`NetworkPlanValidator`；
2. 结构或引用校验失败时不执行能力查询；
3. 将每条有效demand映射为`CapabilityRequest`；
4. 调用现有`CommunicationCapabilityService::Query`；
5. 按需求记录PASS、FAIL或DATA_INVALID；
6. 生成整体结果，但不修改输入规划、快照、剖面或DataContainer实时状态。

判定规则：

- 只有`requestValid && pathAvailable`且所有显式需求均满足时，该需求为PASS；
- 必需指标无效时为DATA_INVALID，不得判PASS；
- 候选路径必须保留`PARAMETERIZED_MODEL/LOW`和候选原因码；
- 环境效果第一阶段只记录，不改变基础路径指标；
- 不复制距离、PDR、时延、吞吐量或接入率公式。

## 9. 状态机和分发包

状态转换固定为：

```text
DRAFT --validation fail--> REJECTED
DRAFT --validation pass, evaluation fail--> REJECTED
DRAFT --validation and evaluation pass--> VALIDATED
VALIDATED --package generated--> READY_FOR_DISTRIBUTION
任何编辑操作 --> 新revision的DRAFT
```

本阶段“分发”只生成不可变包和manifest，至少包含：

- schema、planId、revision、configVersion；
- 规划正文文件名；
- 校验和推演结果文件名；
- 生成时间；
- `readyForDistribution=true`。

不得调用网络接口、消息总线、OA接口或AFSIM控制API。实际分发适配器待甲方接口到位后
开发。

## 10. Warlock集成

新增一个“资源规划”页，保持安静、工作导向的界面：

- 当前规划：planId、revision、状态、来源和配置版本；
- allocation表格；
- demand表格；
- 校验问题表格；
- 推演结果表格；
- 加载、卸载、保存新修订、校验、推演、生成分发包按钮。

要求：

- GUI只调用DataContainer暴露的值对象服务；
- GUI不得直接读写AFSIM对象；
- 不在仿真回调中执行文件I/O；
- 文件操作失败显示固定原因码并写错误日志；
- 不增加自动应用规划按钮；
- 编辑后必须回到DRAFT，旧VALIDATED修订不可原地修改。

若第一轮工期不足，先完成公共契约、Repository、Validator、EvaluationService和测试，再接
GUI。不得用未完成的后端逻辑制作假页面。

## 11. 上报与证据

新增独立输出，建议：

- `plan_validation_results.jsonl`；
- `plan_evaluation_results.jsonl`；
- `distribution_packages/<planId>/<revision>/manifest.json`。

Reporter继续使用有界队列、运行目录隔离、写失败计数和析构排空。修改manifest时保留现有
快照、评估和能力文件。新增schema必须同步测试和文档。

## 12. 必须新增的测试

至少新增两个纯C++测试目标：

1. `nrm_network_plan_repository_test`；
2. `nrm_network_plan_evaluation_test`。

### 12.1 Repository/Validator测试

必须覆盖：

- 合法规划加载、卸载、保存和round-trip；
- 文件不存在、错误magic、未知记录、尾随非法token；
- 重复plan/allocation/demand ID；
- revision不递增和重复覆盖；
- NaN/Inf、负带宽、PDR越界、非法频率；
- profile不存在、节点不存在、业务类型不支持；
- 成员超限、重复成员、时隙资源冲突；
- 失败加载不覆盖最后一个有效规划；
- 保存失败不破坏已有修订版。

### 12.2 Evaluation测试

必须覆盖：

- 两条业务需求全部满足；
- 带宽不足、时延超限和PDR不足分别失败；
- 无路径、离线节点和必需指标无效；
- 参数化候选路径来源和置信度保留；
- 校验失败时能力服务不执行；
- 输入`ResourceSnapshot`、规划和剖面在推演前后保持不变；
- 编辑后状态回到DRAFT；
- 只有全部通过才可生成分发包。

Reporter有新增输出时，扩展现有Reporter测试和恢复测试，不另写无法观察行为的空测试。

## 13. 固定验收场景

只有在纯C++测试通过后，才新增`network_plan_smoke`。场景必须真正触发一次规划加载、校验和
推演，并在输出中产生可解析的规划结果；仅加载插件或发送普通消息不能作为规划服务证据。

若AFSIM脚本入口无法安全触发规划服务，则不伪造场景：状态标记`IMPLEMENTED /
PRE_ACCEPTANCE`，明确记录“固定场景未形成服务调用闭环”。

新增场景后才能把它加入`scripts/ai_guard.sh`批准列表，并且验证阶段只运行一次。

## 14. 允许修改范围

- `include/nrm/`；
- `warlock/source/`；
- `tests/`；
- 必要时`test_mission/`；
- `CMakeLists.txt`、`scripts/ai_guard.sh`；
- `README.md`、`CHANGELOG.md`、`VERSION`；
- `docs/`和`data/`中的追踪文件。

不得修改：

- AFSIM核心源码；
- 已冻结合同原文基线；
- ns-3论文实验目录；
- 系统服务、OA配置和保密环境；
- 与本任务无关的v0.8能力计算语义。

## 15. 明确禁止事项

- 不实现导航算法或猜测导航包；
- 不实现真实地形、气象、天象和电磁传播模型；
- 不自动建链、改频、分配时隙、修改路由或下发规划；
- 不引入优化器、强化学习、遗传算法或参数搜索；
- 不新增数据库、微服务、消息队列或第三方解析库；
- 不重写现有能力服务和路径选择器；
- 不通过调整profile参数让推演结果更容易通过；
- 不把内部格式、演示剖面或固定场景标成甲方最终验收。

## 16. 实施顺序

严格按以下顺序执行，每一步完成后更新`CURRENT_MILESTONE`状态：

1. T0：建立分支并记录9项基线测试；
2. T1：定义内部规划语法和公共值对象；
3. T2：实现Repository、原子保存和round-trip测试；
4. T3：实现Validator和全部非法输入测试；
5. T4：实现EvaluationService并复用能力服务；
6. T5：实现状态机和本地分发包生成；
7. T6：接入DataContainer、Reporter和Warlock页面；
8. T7：完成静态检查、全部测试和一次固定场景；
9. T8：同步版本、合同追踪、验证记录和交接文档；
10. T9：向用户汇报审查结果，获得明确批准后再提交Git。

不得在T1至T5未完成时先制作GUI或截图。

## 17. 完成门

只有同时满足以下条件，才能报告v0.9第一阶段完成：

- 旧9项测试全部无回归；
- 新Repository和Evaluation测试全部通过；
- WSF与Warlock插件构建成功；
- strict parser、原子保存、round-trip和非法输入均有测试；
- 所有业务需求通过时才能生成分发包；
- 推演前后实时快照和当前网络状态不变；
- 输出schema、manifest、版本和合同追踪同步；
- `scripts/ai_guard.sh static`和`test`通过；
- `git diff --check`通过且无`.orig/.rej/.bak`、build或output文件；
- AFSIM核心零修改；
- 结果只标记内部`PRE_ACCEPTANCE`；
- Git提交前获得用户明确批准。

## 18. 最终汇报格式

开发AI必须按以下顺序汇报：

1. 唯一目标；
2. 实际修改文件；
3. 规划格式和状态机行为；
4. 校验与推演结果；
5. 测试命令和通过数量；
6. 固定场景是否真正调用规划服务；
7. 合同3.2.4状态；
8. 未实现的甲方格式、真实分发和自动控制边界；
9. 剩余风险；
10. 下一步唯一建议。

不得使用“基本完成”“效果不错”或“应该可用”等无证据表述。
