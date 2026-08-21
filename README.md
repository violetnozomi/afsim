# AFSIM 网络资源管理器插件

交付物是嵌入 AFSIM 的 WSF 扩展和 Warlock 插件。甲方修改版 AFSIM 的数据接入优先使用
原生回调或同进程公共 C++ 对象；补充/离线数据使用项目定义的精简 JSON v1，导航历史继续
兼容 AFSIM `.neh`。接口说明见 `docs/甲方接口对齐规范与JSON-Schema.md`。

这是网络资源管理器的独立源码包。当前 v0.12.0 在不修改 AFSIM 核心源码的前提下，
增加纯C++进程内模型服务门面、版本化模型注册表和抽象外部契约适配边界，统一暴露已有
通信能力、规划生命周期、资源需求匹配和合同状态证据。

## 当前能力

- `wsf_network_resource_manager`：WSF 扩展入口，注册
  `network_resource_manager` 能力。
- `NetworkResourceManager`：Warlock 插件，显示四网、窗口指标、成员、链路、任务评估、
  通信能力、资源规划、需求匹配、报告健康状态和最近预验收结果。
- 界面可读性：态势图提供50%–500%的地图式局部缩放，支持鼠标位置锚定的滚轮缩放、
  拖动背景平移、缩放按钮、适配全图及`Ctrl++`/`Ctrl+-`/`Ctrl+Shift+0`快捷键；只变换
  节点、链路、逐跳路径和标签，标题、摘要、图例、节点详情及右侧资源表单保持固定，并通过
  逆坐标变换保持节点详情点击和资源测评选点命中。界面保留
  `Link-11`/`Link-16`/`SATCOM`/`CDL`专名，其他职责、
  状态、协议资源、原因码和计量单位采用中文优先显示。
- 公共数据契约：统一网络、端点、链路、消息生命周期、资源事件、指标原因码和输入提供者。
- 架构与接口边界：见[`当前系统架构与甲方接口边界说明`](docs/当前系统架构与甲方接口边界说明.md)。
- 版本化剖面：Link-11、Link-16、SATCOM、CDL 的候选范围、建链时延、PDR、容量、频点、
  协议模型和业务类型可由外部 `.nrm` 配置替换。
- 严格指标：区分提供负载与最终目的端交付吞吐；PDR 使用同一发送队列关联终态，不把
  中间跳成功接收样本误算为业务交付；输出平均值、P50 和 P95 时延。
- 状态指标：根据端点和链路状态事件计算当前/窗口脱网时长、成员在网率、业务可用率、
  建链尝试数、成功率和平均建链时长。
- 跨域网关：AFSIM运行时处理器按显式路由和方向能力转发消息，支持优先级队列、处理/串行化
  时延、消息/比特容量限制、去重、TTL、环路轨迹和逐跳审计；未授权的域、源、目的、消息
  类型或路由顺序固定拒绝，不自动拼接路由。
- 任务评估：在当前图优先、候选图补充的前提下搜索有界 K 条简单路径，选择满足带宽、
  时延和可靠性硬约束的路径，并输出诊断路径、排名、失败约束和有向边不重合备路。
- 通信能力：`CommunicationCapabilityService`复用任务评估路径，统一输出路径三维距离、
  瓶颈可准入速率、丢包率、累计时延、观测交付吞吐量和成员接入率。
- 环境边界：优先采集AFSIM内置地形、气象、时间和电磁结果；严格参数配置仅用于候选链路，
  没有可用数据时保持无效并输出固定原因码，不把近似值冒充实测值。
- 规划生命周期：严格解析内部`NRM_NETWORK_PLAN_V1`格式，支持加载、卸载、保存新修订、
  引用与冲突校验，并将每条业务需求映射到既有通信能力服务进行只读推演。
- 并发任务评估：按规划需求顺序预留共享链路带宽，输出逐任务并发可行性和冲突任务。
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
- 外部适配边界：`CustomerNrmAdapter`提供同进程强类型执行入口；`ContractInterfaceAdapter`
  只定义甲方私有对象与公共值对象的转换，`CustomerJsonCodec`仅用于文件、测试和回放。
- 甲方接口基线：13类Draft 2020-12统一JSON Schema覆盖提供方声明、四网资源、导航、环境、
  规划、评估请求/响应、ACK和错误；详见[`甲方接口对齐规范与JSON Schema`](docs/甲方接口对齐规范与JSON-Schema.md)。
- 可恢复上报：每次运行写入独立 `runId` 目录，生成 manifest、快照 JSONL、评估 JSONL
  能力 JSONL、规划校验/推演 JSONL、需求匹配/建议 JSONL 和 CSV；队列溢出与写入失败
  可观测，退出时排空队列。
- 演示场景：四网总览、三种故障/恢复场景、通信能力和跨域网关固定 smoke 场景。
- 综合协同场景：参考AFSIM内置场景组织方式建立项目自有37节点同体系场景，其中12个是
  六组域对的双归属方向专用网关；包含四网业务、12条单网关双向路由、2条双网关级联、
  主备路由和链路故障恢复，不包含敌对元素、武器或攻击任务。
- 一键内部预验收：`scripts/run_preacceptance.sh`自动执行40项测试、十个批准场景和Warlock
  最终快照核对，在忽略的`output/preacceptance/`目录生成Markdown报告与原始日志，并以
  `latest_status.json`向Warlock只读“预验收状态”页发布最新进度和结果。

当前门面、剖面、内部规划格式和故障场景属于内部实现，状态为 `PRE_ACCEPTANCE`，不代表
甲方模型封装、四网模型、正式规划格式、真实分发链路或目标环境的最终验收。导航第一版采用
AFSIM 2.9 内置 `WsfNavigationErrors` 实时状态和 `.neh` 时序格式；本项目只负责解析、
字段校验、单位与坐标标准化、状态管理、展示和记录，不自研GNSS、INS或融合算法。详见
[`AFSIM内置导航数据接入说明`](docs/AFSIM内置导航数据接入说明.md)。

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
