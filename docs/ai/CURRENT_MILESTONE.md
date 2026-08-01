# 当前里程碑

## 1. 当前状态

状态：IMPLEMENTED（内部PRE_ACCEPTANCE）。

目标版本：`v0.9.0-network-plan-lifecycle`。

稳定起点：v0.8稳定提交`7c76b9410e5d221ff92b1b8e1b94b51650d8a409`。

当前分支：`feat/v0.9-network-plan-lifecycle`。

## 2. 当前唯一目标

实现合同3.2.4第一阶段的网链资源规划文件生命周期：内部规划文件加载、卸载、编辑、严格
校验、只读能力推演、版本化存储和本地分发包生成。

本阶段不得自动应用规划，不得修改AFSIM网络状态。甲方规划格式和分发接口缺失时只建立
内部`nrm.network_plan.v1`和适配器边界，结果最多标记内部`PRE_ACCEPTANCE`。

完整要求见`docs/NEXT_DEVELOPMENT_INSTRUCTIONS.md`。

## 3. 必须复用

- `ResourceSnapshot`；
- `NetworkProfileRepository`；
- `CommunicationCapabilityService`；
- `SnapshotReporter`的运行隔离、有界队列和恢复语义；
- DataContainer值对象边界和Warlock现有页面模式。

禁止复制能力计算、路径搜索或网络剖面逻辑。

## 4. 完成门

- 旧9项测试无回归；
- 新增规划Repository/Validator和EvaluationService测试；
- 严格解析、原子保存、round-trip和非法输入有固定测试；
- 只有校验和推演全部通过才能生成本地分发包；
- 推演不修改快照、规划或实时网络；
- WSF与Warlock插件构建成功；
- 固定场景若存在，必须真正调用规划服务；
- `scripts/ai_guard.sh static/test`通过；
- schema、版本、合同追踪和验证文档同步；
- AFSIM核心零修改；
- Git提交前获得用户明确批准。

## 5. 明确不做

- 甲方规划格式和真实分发协议；
- 自动建链、改频、时隙分配、改路由或网络下发；
- 导航算法或导航包猜测；
- 地形、气象、天象和电磁传播实现；
- 参数扫描、优化算法和研究实验；
- 数据库、微服务或新第三方依赖。

## 6. 当前允许修改范围

`include/nrm`、`warlock/source`、`tests`、必要的`test_mission`、`CMakeLists.txt`、
`scripts/ai_guard.sh`、版本文件及相关`docs/data`。

不得修改AFSIM核心源码、合同原文基线、系统服务和与本任务无关的v0.8能力语义。

## 7. 外部阻塞

- 甲方规划文件格式；
- 规划校验专用规则；
- 实际分发协议和目标地址；
- 网络化模型封装规范和目标环境。

这些阻塞不影响内部规划生命周期开发，但阻止`FINAL_ACCEPTANCE`。

## 8. 开发顺序

1. 建立分支并记录9项基线测试；
2. 先写规划语法和公共值对象；
3. Repository与Validator；
4. EvaluationService与状态机；
5. 本地分发包；
6. DataContainer、Reporter和Warlock；
7. 测试、固定场景、文档和审查。

不得先做GUI再补后端。

当前进度：T0至T8已完成；Codex审查发现的陈旧结果复用和卸载后无法重载问题
已修正；T9等待Git提交授权。

## 9. 状态更新模板

- Task：
- Status：PENDING／IN_PROGRESS／IMPLEMENTED／VERIFIED／BLOCKED
- Changed files：
- Tests：
- Evidence：
- Remaining risk：
- Next single action：

使用内部格式、演示剖面或模拟输入时只能标记`PRE_ACCEPTANCE`，不能标记
`FINAL_ACCEPTANCE`。

## 10. 2026-08-01 v0.9第一阶段检查

- Task：实现合同3.2.4内部网链资源规划文件生命周期。
- Status：IMPLEMENTED（内部PRE_ACCEPTANCE）。
- Changed files：公共规划类型、Repository、Validator、Evaluation、Distribution、共享
  JSON序列化、DataContainer、Reporter、Warlock资源规划页、两项测试、版本和追踪文档。
- Tests：基线9/9；完成后`scripts/ai_guard.sh static/test`通过，11/11测试通过，WSF与
  Warlock插件构建成功。
- Evidence：`docs/VALIDATION.md`、`nrm.network_plan_validation.v1`、
  `nrm.network_plan_evaluation.v1`和`nrm.network_plan_distribution.v1`。
- Remaining risk：现有AFSIM脚本没有安全触发规划服务的入口，未形成固定服务场景；甲方
  规划格式、专用校验规则、真实分发协议和目标环境未提供；Warlock页面未人工点击截图。
- Review fixes：校验、推演与分发包增加稳定规划内容指纹，拒绝同一ID/修订下的陈旧
  结果；Repository支持显式卸载后重载同一文件，相应回归测试通过。
- Next single action：等待用户审查本次diff并明确决定是否提交Git；不自动进入下一里程碑。
