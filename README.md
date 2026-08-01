# AFSIM 网络资源管理器插件

这是网络资源管理器的独立源码包。当前 v0.7.0 在不修改 AFSIM 核心源码的前提下，
完成了可配置候选网络剖面、严格消息生命周期指标、资源状态事件账本、有界约束路径选择
和可恢复运行上报闭环。

## 当前能力

- `wsf_network_resource_manager`：WSF 扩展入口，注册
  `network_resource_manager` 能力。
- `NetworkResourceManager`：Warlock 插件，显示四网、窗口指标、成员、链路、任务评估
  和报告健康状态。
- 公共数据契约：统一网络、端点、链路、消息生命周期、资源事件、指标原因码和输入提供者。
- 版本化剖面：Link-11、Link-16、卫通、CDL 的候选范围、建链时延、PDR、容量、频点、
  协议模型和业务类型可由外部 `.nrm` 配置替换。
- 严格指标：区分提供负载与最终目的端交付吞吐；PDR 使用同一发送队列关联终态，不把
  中间跳成功接收样本误算为业务交付；输出平均值、P50 和 P95 时延。
- 状态指标：根据端点和链路状态事件计算当前/窗口脱网时长、成员在网率、业务可用率、
  建链尝试数、成功率和平均建链时长。
- 任务评估：在当前图优先、候选图补充的前提下搜索有界 K 条简单路径，选择满足带宽、
  时延和可靠性硬约束的路径，并输出诊断路径、排名、失败约束和有向边不重合备路。
- 可恢复上报：每次运行写入独立 `runId` 目录，生成 manifest、快照 JSONL、评估 JSONL
  和 CSV；队列溢出与写入失败可观测，退出时排空队列。
- 演示场景：四网总览，以及链路中断、拥塞和质量下降三种故障/恢复场景。

当前剖面和故障场景属于内部演示输入，状态为 `PRE_ACCEPTANCE`，不代表甲方四网模型或
目标环境的最终验收。甲方 GNSS/INS 功能包仍负责导航解算；本项目后续只负责结果包解析、
字段校验、单位与坐标标准化、状态管理、展示、记录和转发。

## 接入方式

AFSIM 2.9 支持外部扩展路径，无需复制源码或修改预设文件：

```bash
cmake -S /path/to/afsim/src -B /path/to/afsim/build \
  -DWSF_ADD_EXTENSION_PATH=/home/pyh/afsim/network_resource_manager
cmake --build /path/to/afsim/build \
  --target wsf_network_resource_manager NetworkResourceManager nrm_tests
ctest --test-dir /path/to/afsim/build --output-on-failure -R '^nrm_'
```

默认使用内置剖面。运行前设置以下变量可加载外部配置：

```bash
export NRM_NETWORK_PROFILE_CONFIG=/home/pyh/afsim/network_resource_manager/config/network_profiles.nrm
export NRM_OUTPUT_DIR=/tmp/nrm-output
```

详细构建和回退步骤见 [docs/BUILD_AND_ROLLBACK.md](docs/BUILD_AND_ROLLBACK.md)，
实际验证结果见 [docs/VALIDATION.md](docs/VALIDATION.md)，故障场景验收步骤见
[docs/V070_FAILURE_SCENARIOS.md](docs/V070_FAILURE_SCENARIOS.md)。日常启动和可视化入口见
[docs/运行与可视化入口.md](docs/运行与可视化入口.md)，需求状态见
[docs/IMPLEMENTATION_STATUS.md](docs/IMPLEMENTATION_STATUS.md)。

## 目录

```text
network_resource_manager/
├── include/nrm/                 与 AFSIM、Qt 解耦的公共数据契约和纯 C++ 服务
├── source/                      WSF 扩展入口
├── warlock/source/              Warlock GUI、仿真监听和异步上报
├── config/                      版本化演示网络剖面
├── test_mission/                四网与故障恢复场景
├── tests/                       纯 C++ 回归测试
├── data/                        合同覆盖和需求追踪
├── docs/                        架构、验证、运行和回退说明
├── CMakeLists.txt               WSF 扩展构建入口
├── wsf_module                   AFSIM 外部扩展标记
├── wsf_cmake_extension.cmake    WSF 扩展发现配置
└── warlock/warlock_plugin.cmake Warlock 插件发现配置
```

## 开发约束

1. 不直接修改 AFSIM 核心目录；兼容性差异集中在本仓库。
2. 仿真回调只复制、关联和计数；GUI 只消费值对象，不跨线程持有 AFSIM 指针。
3. 外部模块通过适配器转换为公共契约，不让外部协议结构进入评估核心。
4. 参数化剖面必须保留来源与置信度，缺失数据保持无效并携带固定原因码。
5. 自动建链、改频或改路由不在当前只读插件范围内。
