# AFSIM 网络资源管理器插件

这是网络资源管理器的独立源码包。当前 v0.11.0 在不修改 AFSIM 核心源码的前提下，
增加纯C++进程内模型服务门面、版本化模型注册表和抽象外部契约适配边界，统一暴露已有
通信能力、规划生命周期和资源需求匹配服务。

## 当前能力

- `wsf_network_resource_manager`：WSF 扩展入口，注册
  `network_resource_manager` 能力。
- `NetworkResourceManager`：Warlock 插件，显示四网、窗口指标、成员、链路、任务评估、
  通信能力、资源规划、需求匹配和报告健康状态。
- 公共数据契约：统一网络、端点、链路、消息生命周期、资源事件、指标原因码和输入提供者。
- 版本化剖面：Link-11、Link-16、卫通、CDL 的候选范围、建链时延、PDR、容量、频点、
  协议模型和业务类型可由外部 `.nrm` 配置替换。
- 严格指标：区分提供负载与最终目的端交付吞吐；PDR 使用同一发送队列关联终态，不把
  中间跳成功接收样本误算为业务交付；输出平均值、P50 和 P95 时延。
- 状态指标：根据端点和链路状态事件计算当前/窗口脱网时长、成员在网率、业务可用率、
  建链尝试数、成功率和平均建链时长。
- 任务评估：在当前图优先、候选图补充的前提下搜索有界 K 条简单路径，选择满足带宽、
  时延和可靠性硬约束的路径，并输出诊断路径、排名、失败约束和有向边不重合备路。
- 通信能力：`CommunicationCapabilityService`复用任务评估路径，统一输出路径三维距离、
  瓶颈可准入速率、丢包率、累计时延、观测交付吞吐量和成员接入率。
- 环境边界：`EnvironmentEffectAdapter`预留地形、气象、天象和电磁干扰输入；没有甲方
  数据时四项均保持无效并输出固定原因码，不编造传播模型。
- 规划生命周期：严格解析内部`NRM_NETWORK_PLAN_V1`格式，支持加载、卸载、保存新修订、
  引用与冲突校验，并将每条业务需求映射到既有通信能力服务进行只读推演。
- 本地分发包：仅当校验和全部需求推演通过时，生成包含规划正文、校验结果、推演结果和
  manifest 的不可变目录；不调用网络、消息总线或 AFSIM 控制接口。
- 需求匹配：严格解析内部`NRM_RESOURCE_DEMAND_V1`格式，逐需求复用一次通信能力查询，
  固定输出路径、网络规模、距离、带宽、业务流量、时延、PDR和业务类型八项检查。
- 可解释建议：频率、站点、信道、子网和时隙只筛选调用方显式有限候选；路由只复用本次
  能力结果。每类均输出`AVAILABLE`或固定`UNAVAILABLE`原因，不自动应用。
- 模型服务门面：`ModelServiceFacade`使用强类型上下文和响应统一委托五项既有领域操作；
  schema、requestId、snapshotVersion和规划证据不一致时在下游调用前固定拒绝。
- 模型注册表：`ModelRegistry`支持精确版本注册、查询、枚举、卸载和操作/schema能力判断，
  列表按模型ID、语义版本和provider稳定排序，不扫描动态库。
- 外部适配边界：`ContractInterfaceAdapter`只定义抽象外部载体与显式内部结构体间的转换，
  不定义甲方端口、字段、二进制布局或传输协议。
- 可恢复上报：每次运行写入独立 `runId` 目录，生成 manifest、快照 JSONL、评估 JSONL
  能力 JSONL、规划校验/推演 JSONL、需求匹配/建议 JSONL 和 CSV；队列溢出与写入失败
  可观测，退出时排空队列。
- 演示场景：四网总览、三种故障/恢复场景和通信能力固定 smoke 场景。

当前门面、剖面、内部规划格式和故障场景属于内部实现，状态为 `PRE_ACCEPTANCE`，不代表
甲方模型封装、四网模型、正式规划格式、真实分发链路或目标环境的最终验收。甲方 GNSS/INS 功能包
仍负责导航解算；本项目后续只负责结果包解析、字段校验、单位与坐标标准化、状态管理、
展示、记录和转发。

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
export NRM_DEMAND_STORE_DIR=/tmp/nrm-demands
```

详细构建和回退步骤见 [docs/BUILD_AND_ROLLBACK.md](docs/BUILD_AND_ROLLBACK.md)，
实际验证结果见 [docs/VALIDATION.md](docs/VALIDATION.md)，通信能力口径见
[docs/features/通信能力计算服务.md](docs/features/通信能力计算服务.md)，故障场景验收步骤见
[docs/V070_FAILURE_SCENARIOS.md](docs/V070_FAILURE_SCENARIOS.md)，规划格式和状态机见
[docs/features/网链资源规划文件.md](docs/features/网链资源规划文件.md)，需求文法和匹配口径见
[docs/features/网链资源需求匹配.md](docs/features/网链资源需求匹配.md)，模型服务契约见
[docs/MODEL_SERVICE_FACADE.md](docs/MODEL_SERVICE_FACADE.md)。日常启动和可视化入口见
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
