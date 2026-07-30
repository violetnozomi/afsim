# AFSIM 网络资源管理器插件

这是网络资源管理器的独立源码包。当前版本先建立可编译、可加载、可回退的总体框架，不修改
AFSIM 核心源码。

## 当前能力

- `wsf_network_resource_manager`：WSF 扩展入口，注册
  `network_resource_manager` 能力。
- `NetworkResourceManager`：Warlock 插件，监听仿真内部通信事件并显示最小实时状态。
- 公共数据类型：统一四网类型、数据来源、健康状态和资源快照。
- 外部接入点：后续可添加 AFSIM 内部适配器、甲方模块输入适配器、导航计算和环境影响模块。

当前面板显示：

- 插件版本和仿真状态；
- 仿真时间；
- 网络数量和通信端点数量；
- 消息发送、接收和逐跳转发累计数。

这些字段用于验证插件已经进入 Warlock GUI 线程和 AFSIM 仿真线程。完整资源指标、评估算法和
四网专用适配器将在后续版本逐步加入。

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
