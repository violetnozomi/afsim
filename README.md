# AFSIM 网络资源管理器插件

这是网络资源管理器的独立源码包。当前版本已完成可编译、可加载、可回退的总体框架和
AFSIM内部网络资源采集、三档窗口指标和当前图任务评估闭环，不修改AFSIM核心源码。

## 当前能力

- `wsf_network_resource_manager`：WSF 扩展入口，注册
  `network_resource_manager` 能力。
- `NetworkResourceManager`：Warlock插件，显示四网、窗口指标、成员和链路实时状态。
- 公共数据类型：统一网络、端点、链路、质量标记和输入提供者接口。
- 指标计算：1/10/60秒吞吐、PDR、在线率、排队时延和传输时延。
- 任务评估：当前图可达性、硬约束、主路由、数值裕量、原因码和只读建议。
- RF适配：仅在通信结果有效时显示带宽、RSSI、SNR、BER和利用率。
- 异步上报：输出完整JSONL快照和按窗口展开的CSV网络汇总。
- 演示场景：8个端点、4类网络、8条有向链路和4类消息。
- 外部接入点：后续可添加 AFSIM 内部适配器、甲方模块输入适配器、导航计算和环境影响模块。

当前面板包含四网总览、窗口指标、成员、链路和任务评估五个页签。候选可建链图、备选
路由、业务级消息去重和四网专用协议适配器将在后续版本逐步加入。

## 接入方式

AFSIM 2.9 支持外部扩展路径，无需复制源码或修改预设文件：

```bash
cmake -S /path/to/afsim/src -B /path/to/afsim/build \
  -DWSF_ADD_EXTENSION_PATH=/home/pyh/afsim/network_resource_manager
cmake --build /path/to/afsim/build \
  --target wsf_network_resource_manager NetworkResourceManager
```

详细构建和回退步骤见 [docs/BUILD_AND_ROLLBACK.md](docs/BUILD_AND_ROLLBACK.md)。
当前基线的实际验证结果见 [docs/VALIDATION.md](docs/VALIDATION.md)。
日常启动、仿真和可视化入口统一记录在
[docs/运行与可视化入口.md](docs/运行与可视化入口.md)。
Windows 通过 SSH 隧道进行 VNC/GDB 调试的步骤见
[docs/Windows远程调试.md](docs/Windows远程调试.md)。
完整操作流程见[docs/功能使用手册.md](docs/功能使用手册.md)，需求完成状态见
[docs/IMPLEMENTATION_STATUS.md](docs/IMPLEMENTATION_STATUS.md)。

## 目录

```text
network_resource_manager/
├── include/nrm/                 公共稳定数据契约
├── source/                      WSF 扩展入口
├── warlock/source/              Warlock GUI 与仿真监听
├── docs/                        架构、构建和回退说明
├── CMakeLists.txt               WSF 扩展构建入口
├── wsf_module                   AFSIM 外部扩展标记
├── wsf_cmake_extension.cmake    WSF 扩展发现配置
└── warlock/warlock_plugin.cmake Warlock 插件发现配置
```

## 开发约束

1. 不直接修改 AFSIM 核心目录；兼容性差异集中放入本仓库的 `compat/`。
2. 每个可运行里程碑必须提交并打标签，实验性工作在独立分支完成。
3. 仿真回调只复制和计数；GUI 只消费值对象，不跨线程持有 AFSIM 指针。
4. 接入甲方模块时新增适配器，不让外部协议结构进入评估核心。
5. 任何自动建链、改频或改路由能力均不在当前只读插件范围内。
