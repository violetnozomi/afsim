# 验证记录

## v0.7.0 开发与预验收证据

验证日期：2026-08-01
AFSIM 基线：2.9.0
系统：Linux x86_64，GCC 13，Release 构建
开发起点：commit `8948cd0`，tag `v0.6.0-candidate-routing`

### 基线

在修改前构建并运行以下三个旧测试，全部退出码为 0：

- `nrm_framework_types_test`
- `nrm_snapshot_reporter_test`
- `nrm_assessment_evaluator_test`

本轮在统一代码审查和固定回归通过后形成 v0.7 稳定提交；是否创建版本标签由仓库维护者
在后续发布节点决定。

### 构建与自动测试

```bash
cmake --build . --target wsf_network_resource_manager NetworkResourceManager nrm_tests -j2
ctest --output-on-failure -R '^nrm_'
scripts/ai_guard.sh static
scripts/ai_guard.sh test
scripts/ai_guard.sh scenario framework_smoke
```

WSF 和 Warlock 两个共享库目标编译、链接成功。CTest 发现并通过 8 项测试：

- framework types
- snapshot reporter
- assessment evaluator
- network profile
- message lifecycle tracker
- resource event ledger
- constrained path selector
- snapshot reporter recovery

覆盖正常、空输入、非法配置、重复/乱序、超时、K/跳数边界、可行替代路径、缺失发送
分母、文件打开失败、队列溢出、两次启动不覆盖和旧字段兼容。测试生成的 manifest 和
JSONL 已使用 `jq` 全量解析通过。

`nrm_snapshot_reporter_test`确认`nrm.snapshot.v2`同时输出camelCase字段和v0.6
snake_case兼容字段。`framework_smoke`退出码为0并确认WSF插件被加载。

代码审查进一步确认并修复：只有接收端地址等于消息最终目的地址时才记录业务交付；每跳
接收仍保留链路观测；备路先选满足硬约束的当前图路径，再以候选路径兜底；不可行诊断路径
不再作为备路输出；剖面、任务约束和位置/链路指标拒绝NaN/Inf；容量驱逐的待定消息明确
归档为EXPIRED。对应回归用例已纳入8项固定测试。

### 插件加载

2026-08-01 重启 `nrm-warlock.service` 后，进程内存映射同时包含：

- `warlock_plugins/libNetworkResourceManager_ln13m64.so`
- `wsf_plugins/libwsf_network_resource_manager_ln13m64.so`

Warlock 面板实机显示 `Version=0.7.0`、`Reporting=OK`。截图保存在运行生成目录
`output/screenshots/v070-overview-loaded.png`，不提交 Git。

当前基线场景实机显示 4 个网络、10 个端点和 8 条有向链路。开发指令沿用了“8 个端点”
的旧记录；实际场景还包含两个用于 v0.6 候选备路演示的红方 Link-16 端点，因此不通过
隐藏端点来伪造 8 个计数。

### 故障场景

以下输入均以退出码 0 完成，加载 WSF 插件并输出 ACTIVE/RECOVERED：

- `test_mission/link_failure.txt`
- `test_mission/congestion.txt`
- `test_mission/quality_degradation.txt`

拥塞场景修正为 100–250 秒每 2.5 秒发送 160 Mbit，提供负载为 64 Mbit/s。Warlock
`T=193.8 s` 快照实测 CDL 10 秒窗口 4 条发送，`offeredLoadBps=64,000,000`。
评估器测试证明服务容量 238 kbit/s 时，50 kbit/s 负载后的 188 kbit/s 可准入容量通过，
200 kbit/s 负载后的 38 kbit/s 可准入容量触发带宽裕量失败。

链路中断交互记录在故障期得到 `reachable=false` 和 `NO_CURRENT_PATH`，恢复后
`reachable=true`。三个场景的完整前/中/后 Warlock 截图仍为人工预验收项，不能据此
标记最终验收。

### 上报恢复

恢复测试确认：

- 两次 Reporter 启动产生不同 runId，首轮文件保留；
- 队列上限为 2 时，连续入队 5 条准确记录 3 条丢弃；
- 只读 `/proc` 输出路径返回不健康状态但不崩溃；
- 析构返回前排空已入队记录；
- manifest 记录软件、schema、配置版本和完成状态；
- v3 评估继续输出 v2 的 network_sequence、recommendations 等既有字段。

### 结论与边界

v0.7 代码、自动测试、插件编译加载和命令行故障场景达到内部 `PRE_ACCEPTANCE`。
以下仍不属于完成的最终验收证据：

- 甲方四网接口、正式样包和目标环境；
- 每个故障场景的完整 Warlock 前/中/后截图；
- 独立原始 `resource_events.jsonl`；
- 组播/重传严格语义、真实协议建链事件和跨网网关。

---

## v0.6.0 历史验证记录

验证日期：2026-07-30  
AFSIM 基线：2.9.0  
系统：Linux x86_64，GCC 13，Release 构建

## 已通过

1. CMake 通过 `WSF_ADD_EXTENSION_PATH` 发现：
   - `wsf_network_resource_manager`
   - `NetworkResourceManager`
2. 两个共享库目标编译和链接成功。
3. `nrm_framework_types_test`、`nrm_snapshot_reporter_test`和
   `nrm_assessment_evaluator_test`通过。
4. `mission` 启动信息明确列出 `libwsf_network_resource_manager`，并完成
   `test_mission/four_network_overview.txt`。
5. 滑动窗口测试覆盖吞吐量、PDR、在线率、两类时延和利用率。
6. Warlock 插件导出以下三个要求的入口：
   - `wkf_plugin_registration`
   - `wkf_plugin_create`
   - `wkf_plugin_get_tags`
7. 在 GDB 中，Warlock 的 `wkf::PluginManager::LoadPluginInitialize` 已实际调用
   `WkNrm::Plugin::Plugin` 构造函数，证明插件已经被发现、校验并实例化。
8. 候选边追加期间邻接表使用地址稳定的`deque`，回归验证当前主路径的边引用不会因扩容
   失效。

## 远程 GUI 验证

已建立仅监听服务器回环地址的 TigerVNC `:1` 桌面，并通过 Mesa llvmpipe 提供软件
OpenGL。Warlock 已打开`test_mission/four_network_overview.txt`，主窗口在VNC桌面持续运行。
运行进程的内存映射确认同时加载：

- `warlock_plugins/libNetworkResourceManager_ln13m64.so`
- `wsf_plugins/libwsf_network_resource_manager_ln13m64.so`

AFSIM 2.9 随附的 Qt 5.12 在当前 glibc 下使用 `QLockFile` 时会因 fortified `readlink`
长度不一致而中止。远程启动器通过短路径运行布局和仅注入 Warlock 的兼容层规避该问题，
没有修改 AFSIM 核心源码。

## 资源采集验证

实际观测结果：

| 指标 | 结果 |
| --- | ---: |
| 分类网络 | 4 |
| 通信端点 | 8 |
| 有向链路 | 8 |
| 发送/接收 | 4/4 |
| 丢弃/路由失败 | 0/0 |

Warlock右侧面板默认显示四网总览，评估页使用快照驱动的源/目的下拉框。异步上报产生：

- `output/resource_snapshots.jsonl`
- `output/network_summary.csv`

JSONL包含网络、端点、链路、消息、三档窗口以及指标有效性信息。CSV按网络和窗口展开。
演示消息长度分别为512、1024、4096和1000000 bit，10秒窗口能够观察到不同吞吐量。

## 任务评估验证

评估器测试使用四节点、两条两跳Link-16路径，并增加纯候选图用例。验证结果：

| 项目 | 期望/结果 |
| --- | ---: |
| 预测时延 | 40 ms |
| 路径PDR | 90.25% |
| 瓶颈带宽 | 800 bit/s |
| 带宽要求700 bit/s | 通过，裕量100 bit/s |
| 带宽要求900 bit/s | 失败，裕量-100 bit/s |
| 只允许CDL | `NO_CURRENT_PATH` |
| 目的平台不存在 | `NODE_NOT_FOUND` |
| 当前备选路由 | `source → backup_relay → destination` |
| 纯候选图 | 当前不可达、可建链、可完成、低置信度且不判稳定 |

评估结果通过异步上报线程写入`output/assessment_results.jsonl`，测试已核对任务编号、
主备路由、候选标记和原因码。

远程Warlock实机点击默认任务得到：

| 字段 | 结果 |
| --- | --- |
| 任务 | `l16_fighter → l16_command` |
| `reachable/can_establish/can_complete/stable` | `true/true/true/true` |
| 主路由 | `l16_fighter → l16_command`，当前边 |
| 备选路由 | `l16_fighter → red_fighter_1 → l16_command`，候选边 |
| 预测时延 | 约0.085 ms |
| 估计PDR | 100% |
| 时延裕量 | 约999.915 ms |
| 可靠性裕量 | 10个百分点 |

输出文件实际生成一条`nrm.assessment.v2`记录，携带主备路由和两个候选布尔字段。中央态势
图实际显示黄色主路由、青色虚线候选备路、绿色源节点圆环和橙色目的节点圆环。

首次远程点击曾触发`std::length_error`。原因是`vector`追加候选边时扩容，使当前路径保留
的边指针失效。邻接边容器改为`std::deque`后重新编译、运行测试、重启服务并再次点击，
异常不再出现，Warlock保持`active (running)`且评估文件正常写出。

## 当前结论

框架已经接入 AFSIM 的正式扩展机制，并且没有修改 AFSIM 源文件。WSF 冒烟运行与 Warlock
远程GUI、网络资源采集、基础窗口指标、当前/候选图评估、主备路由上报和中央高亮闭环均
已完成。RF值在通信模型提供有效结果时采集；业务级严格PDR、显式跨网网关、配置化候选
参数和完整K最短路仍属于后续版本。
