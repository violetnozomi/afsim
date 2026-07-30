# 0.4.0 验证记录

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

Warlock右侧面板默认显示四网总览，五个页签均完成绘制。异步上报产生：

- `output/resource_snapshots.jsonl`
- `output/network_summary.csv`

JSONL包含网络、端点、链路、消息、三档窗口以及指标有效性信息。CSV按网络和窗口展开。
演示消息长度分别为512、1024、4096和1000000 bit，10秒窗口能够观察到不同吞吐量。

## 任务评估验证

评估器测试使用三节点、两跳Link-16图，验证结果：

| 项目 | 期望/结果 |
| --- | ---: |
| 预测时延 | 40 ms |
| 路径PDR | 90.25% |
| 瓶颈带宽 | 800 bit/s |
| 带宽要求700 bit/s | 通过，裕量100 bit/s |
| 带宽要求900 bit/s | 失败，裕量-100 bit/s |
| 只允许CDL | `NO_CURRENT_PATH` |
| 目的平台不存在 | `NODE_NOT_FOUND` |

评估结果通过异步上报线程写入`output/assessment_results.jsonl`，测试已核对任务编号、主路由
和原因码。

远程Warlock实机点击默认任务得到：

| 字段 | 结果 |
| --- | --- |
| 任务 | `l11_control → l11_member` |
| `reachable/can_establish/can_complete/stable` | `true/true/true/true` |
| 主路由 | `l11_control → l11_member` |
| 预测时延 | 约0.030 ms |
| 估计PDR | 100% |
| 时延裕量 | 约999.970 ms |
| 可靠性裕量 | 10个百分点 |

输出文件实际生成一条`nrm.assessment.v1`记录，携带`snapshot_version=14`。

## 当前结论

框架已经接入 AFSIM 的正式扩展机制，并且没有修改 AFSIM 源文件。WSF 冒烟运行与 Warlock
远程GUI、网络资源采集、基础窗口指标和当前图任务评估闭环均已完成。RF值在通信模型提供
有效结果时采集；业务级严格PDR、候选建链图和备选路由仍属于后续版本。
