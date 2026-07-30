# 总体框架

## 分层

```text
AFSIM/甲方输入
      │
      ▼
输入适配层（内部 Observer、Link-11、Link-16、卫通、CDL、导航、环境）
      │ 统一值对象
      ▼
状态仓库与指标层
      │ 不可变快照
      ├───────────────┬───────────────┐
      ▼               ▼               ▼
Warlock 展示      任务评估/建议      JSONL/CSV 上报
```

当前`0.3.0`实现第一条完整纵向路径：Warlock仿真接口监听通信事件并扫描全局通信图，生成
网络、端点、链路、位置、距离和消息值对象。GUI线程消费快照，文件工作线程异步输出JSONL
和CSV。

## 稳定边界

- `include/nrm/NetworkResourceTypes.hpp` 是独立于 AFSIM 和 Qt 的公共数据契约。
- WSF 扩展入口只负责能力注册，后续仿真侧服务从这里挂载。
- Warlock `SimInterface` 是当前 AFSIM 内部监听适配器。
- `DataContainer` 只存在于 GUI 线程，事件对象跨线程传递值，不传递裸指针。
- `SnapshotReporter`使用有界队列和独立线程，不在仿真回调中执行文件I/O。
- `InputProvider`冻结内部、甲方模块和回放输入的公共边界。

后续甲方提供四网模块数据时，应新增 `CustomerModuleAdapter` 实现并转换为相同公共数据契约。
内部模型和外部模块可并存，数据的 `DataOrigin` 字段区分来源。

## 下一阶段目录约定

```text
source/
├── adapters/       AFSIM 内部及甲方模块输入适配器
├── navigation/     卫星导航、惯性导航及融合计算
├── environment/    地形、气象和干扰影响适配
├── repository/     状态、事件账本和滑动窗口
├── metrics/        时延、利用率、质量与可用性
├── evaluation/     可达性、差距和稳定性
├── recommendation/ 主备路由与建链建议
└── reporting/      JSONL、CSV 和后续外部上报
```

这些目录在有首个真实接口时再建立，避免用空类固化错误接口。
