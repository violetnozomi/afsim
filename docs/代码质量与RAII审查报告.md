# 代码质量与 RAII 审查报告

_审查日期：2026-08-16；范围：`network_resource_manager`独立扩展、Warlock插件、脚本与测试。_

## 审查结论

本轮已完成影响正确性、异常安全和验收真实性的问题收口。AFSIM核心源码修改数仍为0，
公共甲方适配接口未删除或改签名。当前代码通过32项固定C++测试、2项Shell回归、严格JSON
Schema校验、严格编译告警检查和25节点综合场景验证。

## 已修复的必须项

| 范围 | 原问题 | 当前处理 |
| --- | --- | --- |
| 甲方JSON输入 | Schema文件严格，但运行时部分字段会接受字符串数字、越界值或错误枚举 | 导航、环境、资源、规划运行时解码统一校验类型、有限值、范围和枚举；失败不污染输出对象 |
| 规划加载 | JSON解码成功后，仓库可能拒绝修订，但界面仍把解码标记为成功 | 新增统一接纳函数；仓库拒绝时返回`PLAN_REJECTED`，不替换当前规划 |
| Reporter健康状态 | 业务输入错误会被误计为文件写失败并把Reporter置为不健康 | 业务域错误与I/O错误分开计数；只有真实文件故障影响健康状态 |
| Reporter线程 | 未启动对象析构会意外启动线程；异常分支可能`detach`后访问已析构对象 | 析构不启动线程，所有已启动工作线程固定`join`，停止顺序确定 |
| Qt顶层对象 | 插件保存裸Dock/Tactical指针，所有权不够清楚 | 顶层Dock使用`ut::qt::UiPointer`；TacticalView由Dock父子树唯一拥有 |
| 临时文件 | 规划、需求、恢复和分发目录在多个失败分支手工清理 | 引入不可复制、可移动的`TemporaryPathGuard`，提交前所有异常出口自动清理 |
| 远程切换 | systemd重启失败时会遗留未消费的规划切换请求 | 失败或信号退出自动删除临时文件和请求；成功后才提交所有权 |
| 部署检查 | 只判断硬件数值“可识别”，没有按合同下限判断 | 固定检查4核、1.0 GHz、2 GiB、100 GiB总容量、非回环网卡和至少100 Mbit/s能力 |
| 场景门禁 | AFSIM初始化失败可能返回0，脚本会假通过 | 自动创建输出目录，同时要求无初始化错误且出现`Simulation complete` |
| 编译规范 | 存在变量遮蔽以及隐式整数/浮点和字符符号转换 | 显式转换并消除遮蔽；插件测试目标在严格警告集下零告警 |

## RAII 使用情况

- `SnapshotReporter`使用`std::thread`、`std::mutex`、`std::condition_variable`、容器和流对象，
  析构时发出停止信号并`join`；没有裸线程逃逸。
- `NrmPlugin`的顶层Qt窗口使用AFSIM/Qt提供的`UiPointer`；普通控件均设置Qt父对象，由对象树
  释放。这里保留`new QObject(parent)`是Qt标准所有权模式，不应机械替换为`unique_ptr`。
- `NrmDataContainer`使用`std::unique_ptr<SnapshotReporter>`；公共核心没有Qt/AFSIM裸指针。
- 临时文件和暂存目录由`TemporaryPathGuard`管理；成功原子重命名后显式`Commit()`。
- 文件流使用`std::ifstream/std::ofstream`自动关闭；没有`malloc/free`或手工互斥量
  `lock/unlock`配对。

## 死代码与扩展边界结论

没有删除`InputProvider`、`NetworkPlanAdapter`或`ContractInterfaceAdapter`。它们没有当前甲方
实现，但属于已冻结的兼容扩展接口，删除会破坏后续现场适配。

以下组件不是运行时主链的一部分，已在追踪文件中明确标注，避免把测试样例写成现场能力：

- `FrequencyCharacteristicRepository`：独立组件已测试；当前频率推荐仍使用
  `NetworkProfileRepository`。
- `RecoveryStateStore`：原子保存和损坏保护已测试；启动恢复位置与策略待甲方确认。
- `ReferenceModelAdapter`：仅为第三方模型注册边界的兼容性测试样例。

这些代码具有明确合同用途和测试，不属于无依据删除的死代码。后续接入只允许通过薄适配层，
不得为“提高覆盖率”修改AFSIM核心或虚构甲方协议。

## 仍保留的工程边界

- `NrmDockWidget.cpp`和`NrmSnapshotReporter.cpp`体积较大，但本轮没有进行高风险的大文件拆分；
  当前行为已有大量回归依赖，拆分应作为独立重构里程碑。
- 构建过程中仍显示AFSIM 2.9上游`VaPosition/GeoElevationTile`的
  `-Woverloaded-virtual`告警以及Sphinx环境提示；它们不来自本插件，本轮不修改AFSIM核心。
- 甲方正式四网字段、模型ABI、安全传输与恢复策略仍是现场适配项，不能标为最终验收。

## 固定验证命令

```bash
./scripts/ai_guard.sh static
./scripts/ai_guard.sh contract
NRM_JOBS=2 ./scripts/ai_guard.sh test
ctest --test-dir "$AFSIM_BUILD" --output-on-failure \
  -R 'nrm_(remote_plan_switch|deployment_contract)_test'
./scripts/check_deployment_contract.sh
NRM_SCENARIO_TIMEOUT=180 ./scripts/ai_guard.sh scenario operational_strike_demo
```
