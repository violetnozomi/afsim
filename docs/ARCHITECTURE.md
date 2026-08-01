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
  └── NetworkProfileRepository：版本化参数剖面与校验
                 │
                 ▼
不可变 ResourceSnapshot
  ├── Warlock 展示
  ├── AssessmentEvaluator / ConstrainedPathSelector
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

## 稳定边界

- `include/nrm/` 中的公共契约和算法不依赖 AFSIM 或 Qt。
- WSF 扩展入口只负责能力注册；`NrmSimInterface` 是当前 AFSIM 内部监听适配器。
- `DataContainer` 只存在于 GUI 线程，跨线程事件只传递值对象。
- `SnapshotReporter` 使用有界队列和独立线程，仿真回调不执行文件 I/O。
- `InputProvider` 冻结内部、甲方模块和回放输入的公共边界。
- `AssessmentEvaluator` 不改变 AFSIM 图，只产生评估结果和只读建议。
- 所有参数化候选指标标记为 `PARAMETERIZED_MODEL/LOW`；当前路径上由多链路组合得到的
  PDR 标记为 `ESTIMATED/LOW`，不会伪装为协议实测。

## 配置与失败策略

默认剖面由 `NetworkProfileRepository::BuiltInDemo()` 提供。环境变量
`NRM_NETWORK_PROFILE_CONFIG` 可指向严格的 `NRM_NETWORK_PROFILES_V1` 文本配置。
配置必须包含 schema、config、provider、profileId、网络类型、频点、范围、建链时延、
候选 PDR、容量、传播模型、成员上限和业务类型。非法枚举、负值、PDR 越界、重复 ID 或
缺失字段会整体拒绝，不使用部分有效数据静默降级。

上报目录结构如下：

```text
${NRM_OUTPUT_DIR}/
└── run-<UTC>-<pid>-<timestamp>-<counter>/
    ├── manifest.json
    ├── resource_snapshots.jsonl
    ├── assessment_results.jsonl
    ├── network_summary.csv
    └── error.log                 # 仅发生错误时创建
```

每次进程启动创建新目录，不以截断方式覆盖旧运行。manifest 记录运行、schema、软件和配置
版本以及完成状态；写入失败和队列丢弃通过 `ReporterStatus` 暴露给 Warlock 面板。

## 有界性与线程模型

- 生命周期记录、状态事件和上报队列均有容量上限。
- 路径搜索限制 `K=1..32`、最大跳数 `1..64` 和最大扩展状态数；结果携带搜索上限原因。
- 当前 demo 图规模较小，用户点击后在 GUI 线程执行确定性搜索。进入 200 节点压力场景或
  周期自动重评估前，应迁移到有界评估工作队列。
- Reporter 析构时通知工作线程停止，并在返回前排空已接收队列。

## 后续接入

甲方四网模块到位后，应新增 `CustomerModuleAdapter` 并转换为相同公共数据契约。内部
模型与外部模块可并存，以 `DataOrigin`、providerId、configVersion 和 profileId 区分来源。

甲方 GNSS/INS 功能包负责导航解算。本项目后续仅实现其结果包适配，不实现 GNSS、INS 或
融合算法；在接口文件和样包到位前不猜测二进制格式。地形、气象、天象和电磁环境影响
属于 v0.8 的适配范围。
