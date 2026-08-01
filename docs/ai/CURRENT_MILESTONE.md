# 当前里程碑

## 1. 当前状态

状态：VERIFIED（内部PRE_ACCEPTANCE）。

目标版本：v0.7.0-contract-metrics-foundation。

稳定起点：v0.6.0-candidate-routing，提交8948cd0。

当前v0.7工作树已经完成统一代码审查、固定测试和命令行场景复验，允许形成稳定提交。

## 2. 当前唯一目标

本里程碑已完成；当前唯一目标是保持v0.7稳定提交可复现。

未经用户指定，不开始v0.8环境能力模型、v0.9规划文件或v0.10导航包适配。

## 3. 已完成内容

- VERSION和Version.hpp已改为0.7.0；
- 新增NetworkProfile及外部配置；
- 新增MessageLifecycleTracker；
- 新增ResourceEventLedger；
- 新增ConstrainedPathSelector；
- SnapshotReporter存在较大改动；
- 新增五个测试；
- 新增link_failure、congestion和quality_degradation场景；
- 代码审查修复最终目的端交付、硬约束备路、容量驱逐终态和非有限输入校验；
- 所有.orig/.bak/.rej编辑残留已核对并清理。

## 4. 本里程碑完成后的唯一动作

1. 保持v0.7稳定提交可复现。
2. 未经用户指定，不启动新算法、参数扫描或研究实验。
3. 下一项合同功能必须由用户明确指定，并先更新本文件。
4. 甲方接口、样包或目标环境缺失时保持BLOCKED，不猜测接口。

## 5. v0.7验收门

必须同时满足：

- 八个测试编译并通过；
- WSF和Warlock插件编译并加载；
- 原四网demo没有回归；
- 版本化网络剖面替代评估器硬编码；
- 链路PDR没有成功样本偏置；
- 可行替代路径能被选择；
- 状态账本输出脱网和建链指标；
- 两次运行不会覆盖旧证据；
- 三个故障场景的结论变化符合固定预期；
- 所有参数化结果标记PRE_ACCEPTANCE；
- 无AFSIM核心改动。

## 6. 当前允许修改范围

为完成v0.7，可修改include/nrm、warlock/source、tests、test_mission、config、CMakeLists.txt、README、CHANGELOG和docs。

不允许修改：

- /home/pyh/afsim/afsim2.9下的AFSIM核心源码；
- 合同原文基线；
- 与v0.7无关的研究项目；
- 系统配置和OA环境。

## 7. 状态更新规则

开发AI完成一个任务后，只更新对应检查项，不得把整个版本标为完成。

状态变更模板：

- Task：
- Status：PENDING／IN_PROGRESS／IMPLEMENTED／VERIFIED／BLOCKED
- Changed files：
- Tests：
- Evidence：
- Remaining risk：
- Next single action：

用户或负责人确认所有验收门后，才可将本文件状态改为VERIFIED。

## 8. 2026-08-01 v0.7任务检查

- Task：验证、修正并完成现有v0.7改动。
- Status：VERIFIED（内部PRE_ACCEPTANCE）。
- Changed files：`include/nrm`公共契约与纯C++服务、`warlock/source`采集/评估/上报适配、
  `tests`与`test_mission`固定验证、`config`以及v0.7文档和版本文件。
- Tests：`scripts/ai_guard.sh static`通过；`scripts/ai_guard.sh test`的8项测试全部通过；
  两个插件目标构建成功；5个批准场景均以退出码0各运行一次。
- Evidence：独立runId、manifest、JSONL/CSV、固定原因码、64 Mbit/s拥塞负载和插件加载记录已写入`docs/VALIDATION.md`。
- Remaining risk：三个故障场景缺少完整前/中/后Warlock截图；内部剖面和模拟故障只构成PRE_ACCEPTANCE；甲方接口、样包和目标环境仍未提供。
- Next single action：保存本次稳定提交；等待用户明确指定下一项合同开发任务。
