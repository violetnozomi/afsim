# 代码质量与 RAII 加固设计

## 目标

在不修改 AFSIM 核心、不改变合同指标和公共适配接口的前提下，修复 2026-08-16 整体代码审查发现的正确性、生命周期、异常安全和验收证据问题，并为每项行为变化增加固定回归测试。

## 方案选择

采用“兼容加固”方案：保留 `InputProvider`、`NetworkPlanAdapter` 和 `ContractInterfaceAdapter` 等甲方扩展边界；不进行大文件拆分或公共 API 删除；仅对已确认的缺陷和告警做最小、可验证修改。当前提交 `a1a5ff8` 是完整回退基线。

## 设计

### 外部 JSON 边界

`NrmCustomerJsonCodec` 在 Qt 适配层执行运行时严格校验。数值字段必须是有限数，百分比限制在 0–100，比例限制在 0–1，经纬度限制在合法范围，容量必须是非负整数且 `used <= capacity`。缺失、类型不符或越界数据拒绝进入领域快照，不允许以零值并标记有效。

### 规划接纳一致性

JSON 解码成功只代表格式正确。`DataContainer` 只有在 `NetworkPlanRepository` 接纳规划后才返回 `ACCEPTED`；仓库拒绝必须传播为结构化失败结果并写入接口事件。

### 所有权和线程生命周期

Warlock 顶层 Dock 使用 AFSIM 已有 `ut::qt::UiPointer` 管理，并保存战术 Dock 的所有权，保证插件卸载时先销毁界面再销毁 `DataContainer`。`SnapshotReporter` 析构函数只执行无异常停止与连接；线程启动成功后才发布 started 状态，禁止析构期间首次创建线程。

### 上报健康语义

规划/需求业务错误只记录领域事件，不污染文件上报健康度和 `writeErrorCount`。真正的打开、写入或 flush 失败统一更新 Reporter 故障状态。GUI 丢弃计数包含全部队列。

### 文件和脚本异常安全

公共纯 C++ 层新增轻量临时路径守卫，用于恢复状态、规划、需求和分发暂存的异常清理。远程规划切换在 systemd 重启失败时撤销尚未消费的请求。部署检查按合同最低硬件阈值判断，而非只判断“可识别”。

### 未接入组件和规范告警

兼容扩展接口保留并在状态文档中标明“接口预留”。`RecoveryStateStore` 和 `FrequencyCharacteristicRepository` 不伪称运行时已接入：若本轮能以薄适配接入现有主流程则接入，否则将追踪状态调整为组件级 `IMPLEMENTED / PRE_ACCEPTANCE`。修复变量遮蔽和明确的符号/数值转换告警，不做无关格式化和大型 GUI/Reporter 拆分。

## 测试

- `CustomerJsonCodecTest`：错误键、字符串数值、NaN/Inf、越界比例、负容量、使用量超容量。
- `NetworkPlan`/`DataContainer` 可测边界：解码成功但仓库拒绝时不得返回接受。
- `SnapshotReporterRecoveryTest`：领域错误不破坏 Reporter 健康度，真实写失败必须可见，延迟启动对象可安全析构。
- `RemotePlanSwitchTest`：重启失败后请求文件被清理。
- 部署脚本测试：低于合同阈值必须 FAIL。
- 全量 `ai_guard static/test/contract`，一次固定 25 节点场景验证插件运行。

## 非目标

- 不删除公共兼容接口。
- 不修改合同指标、评分公式或真实协议参数。
- 不修改 AFSIM 核心源码。
- 不进行大文件拆分、全仓格式化或架构重写。
- 不宣称甲方接口和目标环境已经最终验收。
