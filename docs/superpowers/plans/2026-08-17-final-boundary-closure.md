# NRM 最后一轮边界收尾实施计划

> **For agentic workers:** 按本计划逐项执行红—绿—重构；不得扩展为外部传输、自动规划或新算法。

**目标：** 在不修改 AFSIM 核心和现有业务算法的前提下，修复环境模式、Customer Overlay、统一语义校验、JSON 关键规则和验证脚本可移植性。

**架构：** AFSIM 快照继续作为拓扑与实时链路基础态；Customer 只保存自己上报的导航平台和环境子域。`EffectiveSnapshotAssembler` 分别按 `platformId` 和环境子域合成有效快照，所有资源入口继续共用 `ResourceSnapshotValidator`。

**技术栈：** C++17、Qt JSON、Bash、CMake/CTest、现有 AFSIM 2.9 外部扩展构建。

## 全局约束

- 不修改 AFSIM 核心源码。
- 不新增 HTTP、REST、gRPC、TCP、WebSocket 或 MQ。
- 不重写 Assessment、Capability、路由或规划算法。
- JSON 只用于合同、测试、回放和验收；正式入口仍为同进程 C++ Adapter。
- 每项行为变更必须先有失败测试，再做最小实现。

### 任务 1：环境模式严格生效

**文件：** `tests/EnvironmentEffectAdapterTest.cpp`、`include/nrm/BuiltInEnvironmentEffectAdapter.hpp`

- [x] 对同一 `blockedLinkIds` 分别断言 `INFORMATION_ONLY`、`ALREADY_INCLUDED` 不阻断，`CANDIDATE_ADJUSTMENT` 阻断。
- [x] 运行单测并确认旧实现失败。
- [x] 让所有硬阻断和参数衰减只在 `applicationMode == CANDIDATE_ADJUSTMENT` 时执行，并按环境来源生成证据。
- [x] 运行单测并确认通过。

### 任务 2：导航平台覆盖与环境子域覆盖

**文件：** `tests/EffectiveSnapshotAssemblerTest.cpp`、`tests/CustomerNrmAdapterTest.cpp`、`include/nrm/EffectiveSnapshotAssembler.hpp`、`include/nrm/CustomerNrmAdapter.hpp`、`warlock/source/NrmDataContainer.*`

- [x] 增加 AFSIM X 更新不被 Customer Y 冻结、同平台 Customer Y 保持优先的失败测试。
- [x] 增加 AFSIM terrain/weather 与 Customer interference 共存的失败测试。
- [x] Customer 更新只基于 Customer Overlay；有效快照按平台和子域合成。
- [x] 运行两个单测及 DataContainer 集成测试。

### 任务 3：统一资源语义校验

**文件：** `tests/ResourceSnapshotValidatorTest.cpp`、`include/nrm/ResourceSnapshotValidator.hpp`

- [x] 增加经纬度、覆盖范围、业务流引用/流量、姿态范围和协议使用量失败断言。
- [x] 运行单测并确认旧实现失败。
- [x] 在 canonical Validator 中补充输入来源无关的约束，保留负海拔合法。
- [x] 运行单测并确认通过。

### 任务 4：运行时 JSON 与 Schema 对齐

**文件：** `tests/CustomerJsonCodecTest.cpp`、`warlock/source/NrmCustomerJsonCodec.cpp`、`schemas/customer/v1/*.schema.json`

- [x] 增加非法 identifier、错误请求 source、空规划集合、空需求集合和重复 allowedNetworks 用例。
- [x] 运行单测并确认旧实现对至少 identifier/source 错误放行。
- [x] 新增集中式 `ValidateIdentifier` 与按 Schema 的 ID 字段检查；固定请求方向 `CUSTOMER`、响应方向 `NRM`。
- [x] 保持已有集合非空和唯一性校验，运行 Codec 与合同校验。

### 任务 5：Python 选择策略和文档收尾

**文件：** `tests/CustomerInterfaceValidationScriptTest.sh`、`docs/ARCHITECTURE.md`、`docs/VALIDATION.md`、`docs/ai/CURRENT_MILESTONE.md`、`docs/ai/DECISIONS.md`、`docs/ai/SESSION_HANDOFF.md`

- [x] 测试包装器使用 PATH 中 `python3` 或 `PYTHON_BIN`，不写死 `/usr/bin/python3`。
- [x] 文档明确导航按平台、环境按子域覆盖和 applicationMode 三态。
- [x] 执行 `git diff --check`、`ai_guard static/contract/test`、完整 CTest、两插件构建及一个固定场景。
- [x] 审计最终差异；本轮未提交、未合并、未打标签。
