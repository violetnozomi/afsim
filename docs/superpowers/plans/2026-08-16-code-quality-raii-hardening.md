# Code Quality and RAII Hardening Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 修复整体审查发现的正确性、RAII、脚本和证据一致性问题，同时保持 AFSIM 核心零修改及公共兼容接口稳定。

**Architecture:** 在 Qt JSON 边界拒绝非法外部值；在 DataContainer 编排层传播仓库结果；用 AFSIM 自带 UI RAII 和显式 Reporter 停止协议闭合生命周期；用小型临时路径守卫统一文件异常清理。所有行为先由现有测试目标中的失败用例固定。

**Tech Stack:** C++14、Qt 5、AFSIM/Warlock 插件 API、CMake、Bash。

## Global Constraints

- 不修改 AFSIM 核心目录。
- `include/nrm` 不依赖 Qt 或 AFSIM。
- 不删除或改名现有公共适配接口。
- 缺失或不可靠数据必须 `valid=false`，不得以零值冒充。
- 不修改合同指标、公式和统计口径。
- 每个行为修复必须经历 RED→GREEN，并运行完整固定回归。

---

### Task 1: 严格 JSON 数值与枚举校验

**Files:**
- Modify: `tests/CustomerJsonCodecTest.cpp`
- Modify: `warlock/source/NrmCustomerJsonCodec.cpp`

- [ ] 增加非法导航键、字符串数值、越界经纬度、非法链路状态、负容量、超容量和越界 PDR/BER 用例。
- [ ] 单独运行测试并确认这些用例因当前宽松转换失败。
- [ ] 增加有限数、范围、必需键和非负整数读取辅助函数；所有 Decoder 在构造有效领域值前调用。
- [ ] 运行 Customer JSON 测试及 schema 契约测试。

### Task 2: 规划加载结果一致性

**Files:**
- Modify: `tests/CustomerJsonCodecTest.cpp` 或现有规划集成测试
- Modify: `warlock/source/NrmDataContainer.cpp`

- [ ] 固定“JSON 解码成功但仓库拒绝”必须返回 REJECTED 的回归行为。
- [ ] 传播 `ReplaceNetworkPlanDraft` 失败结果，保持错误码、路径和接口事件一致。
- [ ] 运行规划仓库、规划评估和 Customer JSON 测试。

### Task 3: Reporter 生命周期与健康语义

**Files:**
- Modify: `tests/SnapshotReporterRecoveryTest.cpp`
- Modify: `warlock/source/NrmSnapshotReporter.hpp`
- Modify: `warlock/source/NrmSnapshotReporter.cpp`
- Modify: `warlock/source/NrmDataContainer.cpp`

- [ ] 增加延迟启动析构、领域错误保持 healthy、实际写失败变为 ERROR、完整 dropped 汇总用例。
- [ ] 将启动状态设置移到线程构造成功之后；析构只停止并 join，不首次启动线程。
- [ ] 分离领域错误和文件写错误，并检查接口事件写入结果。
- [ ] 补齐所有队列的 dropped 汇总并运行 Reporter 测试。

### Task 4: Warlock UI RAII

**Files:**
- Modify: `warlock/source/NrmPlugin.hpp`
- Modify: `warlock/source/NrmPlugin.cpp`

- [ ] 以编译失败作为 RED，将裸顶层界面指针替换为 `ut::qt::UiPointer`。
- [ ] 保存管理主 Dock、战术视图和战术 Dock，确认成员声明顺序保证界面先于数据销毁。
- [ ] 构建 Warlock 插件并运行 UI 文本测试。

### Task 5: 临时文件 RAII

**Files:**
- Create: `include/nrm/TemporaryPathGuard.hpp`
- Modify: `include/nrm/RecoveryStateStore.hpp`
- Modify: `include/nrm/NetworkPlanRepository.hpp`
- Modify: `include/nrm/ResourceDemandRepository.hpp`
- Modify: `include/nrm/NetworkPlanDistributionService.hpp`
- Modify: relevant repository tests and `CMakeLists.txt`

- [ ] 增加成功提交不删除目标、失败/异常退出清理临时路径的纯 C++ 测试。
- [ ] 实现不可复制、可移动、析构清理、显式 `Commit()` 的守卫。
- [ ] 替换手工临时文件生命周期并运行全部仓库/分发测试。

### Task 6: 部署和远程脚本正确性

**Files:**
- Modify: `tests/RemotePlanSwitchTest.sh`
- Modify/Create: deployment script test wired into `CMakeLists.txt`
- Modify: `scripts/remote/switch-warlock-plan.sh`
- Modify: `scripts/check_deployment_contract.sh`

- [ ] 增加 systemd 重启失败后无残留请求和低硬件配置拒绝用例。
- [ ] 为请求文件安装失败清理 trap；成功 exec 前解除清理。
- [ ] 按 4 核、1.0 GHz、2 GiB、100 GiB 和至少一块非回环网卡输出合同判定。
- [ ] 运行脚本语法和固定脚本测试。

### Task 7: 未接入能力和编译规范收口

**Files:**
- Modify: `include/nrm/ModelRegistry.hpp` and warning-producing headers
- Modify: `data/contract_coverage.yaml`
- Modify: `data/requirement_traceability.yaml`
- Modify: `docs/IMPLEMENTATION_STATUS.md`

- [ ] 修复遮蔽、字符符号和安全可明确的数值转换告警。
- [ ] 保留公共适配器，但把仅测试/参考组件的运行时状态准确写为 PRE_ACCEPTANCE 或接口预留。
- [ ] 运行增强告警编译、YAML 校验和静态门禁。

### Task 8: 完整验证和交接

**Files:**
- Modify: `docs/ai/CURRENT_MILESTONE.md`
- Append: `docs/ai/SESSION_HANDOFF.md`
- Modify: `CHANGELOG.md`

- [ ] 运行 `scripts/ai_guard.sh static`、`test`、`contract`。
- [ ] 运行一次 `scripts/ai_guard.sh scenario operational_strike_demo`。
- [ ] 运行 `git diff --check` 并确认只涉及本计划文件。
- [ ] 记录测试证据、合同状态、剩余甲方阻塞和唯一下一步。
