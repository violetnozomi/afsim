# v0.7 故障场景与预验收

本页记录链路中断、拥塞和质量下降三个内部演示场景。它们用于
`PRE_ACCEPTANCE`，不代表甲方四网模型或目标环境最终验收。

## 场景

| 场景 | 输入 | 故障窗口 | 预期变化 |
| --- | --- | --- | --- |
| 链路中断 | `test_mission/link_failure.txt` | 100–300 s | Link-16 当前边禁用，恢复后重新启用 |
| 拥塞 | `test_mission/congestion.txt` | 100–250 s | CDL 提供负载升至 64 Mbit/s，高于 45 Mbit/s 演示容量 |
| 质量下降 | `test_mission/quality_degradation.txt` | 100–300 s | 当前 Link-16 边禁用，并使用 40% PDR 候选剖面 |

质量下降场景运行 Warlock 前设置：

```bash
export NRM_NETWORK_PROFILE_CONFIG=/home/pyh/afsim/network_resource_manager/config/network_profiles_low_quality.nrm
```

## 命令行检查

```bash
AFSIM_BUILD=/home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build-ubuntu24
NRM_SOURCE=/home/pyh/afsim/network_resource_manager

for scenario in link_failure congestion quality_degradation
do
  "${AFSIM_BUILD}/mission" "${NRM_SOURCE}/test_mission/${scenario}.txt"
done
```

每个场景必须加载 `libwsf_network_resource_manager`、输出
`NRM_SCENARIO ... ACTIVE` 和 `RECOVERED`，并以 `Simulation complete` 结束。

## Warlock 检查点

1. 启动场景后在 `T<100 s` 保存运行前快照并评估任务。
2. 在故障窗口内再次评估，记录 `reachable/canComplete`、原因码、路由和裕量。
3. 在恢复时点后再次评估并保存恢复快照。
4. 在面板中确认 `Version=0.7.0`、`Reporting=OK`。
5. 保存一张能同时显示场景状态和任务结论的 Warlock 截图。
6. 检查新 runId 目录，不得覆盖旧目录。

拥塞任务建议选择 `cdl_sensor -> cdl_station`、允许网络 `CDL`、最低 PDR 为 0，
并把所需带宽设在基线可准入容量与拥塞可准入容量之间。当前实测中，拥塞期 10 秒窗口
为 4 条 x 160 Mbit，即 `offeredLoadBps=64,000,000`。

质量下降任务保持最低 PDR 90%。低质量剖面把 Link-16 候选 PDR 设为 40%，预期产生
`RELIABILITY_MARGIN_NEGATIVE` 或包含该失败约束的诊断结果。

## 输出证据

```text
${NRM_OUTPUT_DIR}/run-<run_id>/
├── manifest.json
├── resource_snapshots.jsonl
├── assessment_results.jsonl
├── network_summary.csv
└── error.log                 # 仅失败时
```

可用 `jq` 检查阶段快照：

```bash
jq -c '{sim_time, networks, links, messages}'   "${NRM_OUTPUT_DIR}/run-<run_id>/resource_snapshots.jsonl"
jq -c '{sim_time, reachable, can_complete, failedConstraints, reason_codes}'   "${NRM_OUTPUT_DIR}/run-<run_id>/assessment_results.jsonl"
```

## 当前证据状态

- 三个命令行 mission 已通过并观察到 ACTIVE/RECOVERED。
- 拥塞 Warlock 快照已观察到 10 秒 `offeredLoadBps=64,000,000`。
- v0.7 Warlock 插件已实机加载，面板显示 `Version=0.7.0` 和 `Reporting=OK`。
- 链路中断已取得故障期 `NO_CURRENT_PATH` 和恢复期评估记录。
- 每个场景完整的前/中/后截图仍需人工 VNC 预验收，不在自动测试中冒充完成。
