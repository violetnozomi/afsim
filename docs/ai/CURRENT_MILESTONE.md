# 当前里程碑

## 1. 当前状态

状态：`IMPLEMENTED / PRE_ACCEPTANCE`。

目标版本：`v0.12.0-contract-gap-closure`。

本地集成分支：`main`；跨域网关、算法逐跳显示、态势图选点、中文化和地图式局部缩放已于
2026-08-21快进合并到`eaaad00`。旧功能分支提交和历史基线仍可通过Git记录定位。

AFSIM核心修改数：`0`。实现范围仅限独立`network_resource_manager`扩展、Warlock插件、
场景、脚本和文档。

## 2. 本里程碑目标

以“合同有对应项、界面可见、结果可解释、命令可复验”为目标，用最小实现补齐当前合同追踪
表中能在本地完成的指标，不等待甲方给出字段；甲方AFSIM改造模块后续通过现有Adapter和
13类JSON Schema对齐本插件，不改变领域服务和界面。

## 3. 已完成内容

- 统一资源状态覆盖网络、成员、链路、频率、协议资源、队列、流量、路由、业务、网关、代理、
  告警、导航和环境；缺失字段显式无效并给出原因码。
- 13类甲方接口Schema、正反例、中文注释JSONC、提供方声明、文件接收ACK和错误记录已固定。
- 四网频率与协议资源、地形/气象/时间/航向/电磁干扰近似约束进入能力评估。
- 规划域、动态成员、分发包指纹ACK、并发需求评估、调整建议、需求反馈历史和推荐闭环已实现。
- 导航支持AFSIM原生导航误差包解析、GNSS/INS/组合导航精度证据和恢复数据原子保留。
- Reporter和Warlock中文页面均可查看新增证据；部署自检脚本可区分本地完成项与甲方阻塞项。
- 严格JSON运行时校验、规划仓库接纳一致性、Reporter生命周期、Qt/临时路径RAII、远程切换
  失败回滚和合同硬件阈值检查已完成代码加固。
- 甲方JSON状态层已按具体网络ID隔离，资源/导航/环境按域合并；消息去重、乱序、新运行隔离、
  规划配置版本绑定、环境完整字段和统一任务评估入口已完成。
- 任务评估已进入`ModelServiceFacade`；甲方并发资源需求JSON已进入需求仓库、匹配服务和
  结构化响应主链；`CustomerNrmAdapter`提供同进程强类型入口，`CustomerJsonCodec`只承担
  文件、测试、回放和验收用途。
- 甲方接口验收闭环已补齐：端点/链路使用明确`networkId`，导航使用明确`platformId`，
  环境三态不会被候选链路逻辑覆盖；`CustomerJsonValidationLayer`提供统一运行时校验入口，
  `DataContainer::LoadCustomerJson`端到端测试覆盖三域合并、评估响应、配置绑定和派生失效。
- 资源对象关系已抽取为纯C++`ResourceSnapshotValidator`，JSON解码与同进程Adapter共用；
  AFSIM基础态和甲方导航/环境覆盖层由`EffectiveSnapshotAssembler`确定性合成。
- Customer Overlay已收紧为导航按平台、环境按子域合成，不再从Effective Snapshot反向
  复制AFSIM状态；地形阻断仅在`CANDIDATE_ADJUSTMENT`下生效并区分Customer/AFSIM证据。
- `ResourceSnapshotValidator`已补齐坐标、覆盖、业务流和姿态canonical约束；运行时JSON已
  对齐1–64位标识符与具体Schema消息方向。
- 独立Assessment通过唯一Facade复用Capability环境链，明确区分只读信息、已包含影响和候选
  修正三态；接口校验脚本支持`PYTHON_BIN`并提供依赖诊断。
- 任务评估、规划和并发需求统一采用`maximumDelayMs=0`无门限语义；空规划成员同时被Schema
  和运行时拒绝；Customer环境三态仅作用于实际提供子域，混合来源证据按子域记录。
- 生产插件、状态管理、评估与规划均为C++；Python只用于离线Schema合同检查，不进入AFSIM
  仿真运行链。
- Warlock二维态势图支持点击平台查看聚合端点信息；资源测评支持显式源/目的选点模式，源点
  选定后自动切换目的点，并保持普通查看点击不修改评估参数。
- 跨域网关一期已进入AFSIM真实运行链：Link-11、Link-16、SATCOM、CDL两两形成六组域对，
  使用12个仅双归属、方向专用物理网关；12条单网关双向路由和2条双网关级联均为显式模板。
- 网关处理器支持来源/目的/消息类型/入口网络/入口端点/路由顺序授权、优先级队列、处理与
  串行化时延、消息/比特容量、去重、TTL、环路轨迹、逐跳新序列号和端到端transferId审计。
- 快照、JSONL、资源测评证据和Warlock节点详情已接入网关能力、路由、计数及最近转发事件；
  资源测评只接受完整有序授权路由，不隐式组合不同网关能力。
- 资源测评已将算法选中的每条有向边投影为稳定逐跳证据；Warlock文本和态势图
  直接消费该序列，显示跳号、方向、网络、单跳候选状态与网关入口→出口转换，
  不再使用平台名数组重建展示路径。内部评估JSONL同步保存主/备逐跳数组，
  甲方V1合同保持不变。
- Warlock态势图支持50%–500%的地图式局部缩放、鼠标锚定滚轮、背景拖动、缩放按钮、
  适配全图及`Ctrl++/Ctrl-/Ctrl+Shift+0`快捷键；仅节点、链路、逐跳路径和标签进入视口
  变换，标题、摘要、图例、详情卡与右侧表单保持固定。点击坐标会逆变换，放大和平移后仍可
  查看节点详情并选择资源测评源/目的节点。界面保留
  `Link-11`/`Link-16`/`SATCOM`/`CDL`专名，职责、
  协议资源、状态、来源、置信度、常见原因码和单位保持中文优先，唯一标识和接口合同码保持原值。

## 4. 当前固定验证基线

- `scripts/ai_guard.sh static`：通过。
- `scripts/ai_guard.sh contract`：13个合法样例通过，6个非法样例按预期拒绝。
- 当前源码40/40固定C++测试逐项重新编译运行通过；4/4 Shell/工程回归通过。当前源码已投影到
  干净临时目录并完成CMake重新配置、`nrm_tests`目标构建和44/44 NRM CTest（完整测试树另含
  1项AFSIM上游测试）。共享构建缓存已通过独立扩展投影重新绑定当前主工作区，
  `wsf_network_resource_manager`、`NetworkResourceManager`和`nrm_tests`正式目标的1241步完整重建通过。
- WSF网关三个翻译单元和Warlock全部当前源码/MOC均按AFSIM Release参数编译并完成动态库
  链接；Warlock库显式依赖`libwsf_network_resource_manager`。
- 2026-08-20实机装载修复后，当前Warlock UI插件`ldd -r`的未解析符号数为0、
  缺失依赖数为0；默认场景与`operational_strike_demo`实际进程均同时加载Warlock UI
  插件和WSF仿真插件，且综合场景窗口已在VNC `:1`中映射。
- 严格编译告警检查：本插件在`-Wall -Wextra -Wpedantic -Wconversion
  -Wsign-conversion -Wshadow`下零告警；仅保留AFSIM上游告警。
- `cross_domain_gateway_smoke`：Link-11→SATCOM→CDL两跳转发、逐跳序列号和最终接收通过。
- 局部缩放已用纯坐标测试和真实Qt Widgets测试覆盖鼠标锚点、正逆变换、平移边界、适配全图
  及面板固定几何；VNC已验证100%→200%局部放大、拖动平移、放大后详情点击、资源测评选点、
  固定图例/表单和恢复全图。新Warlock库已重新链接，完整证据记录在`docs/VALIDATION.md`。
- `validate_operational_strike_demo.sh`：37节点、53端点、108条有向链路、六组域对、两条级联、
  10次逐跳转发和零武器事件全部通过并正常结束。
- `scripts/check_deployment_contract.sh`：本机硬件下限通过，4项甲方依赖正确标为
  `CUSTOMER_BLOCKED`。
- `scripts/run_preacceptance.sh`沿用40项固定测试和10个场景；本轮未重跑需要VNC与用户服务
  的120秒完整GUI预验收，旧31/31、9/9报告仅保留为历史基线。

以上均为内部预验收证据，不代表甲方目标环境最终验收。

## 5. 仍需甲方提供或现场确认

- 甲方改造版AFSIM的正式模块ABI、加载方式和目标环境构建结果。
- Link-11、Link-16、SATCOM、CDL真实字段映射、设备参数和正式样包。
- 甲方对象头文件、字段语义、插件初始化/调用线程和双方ABI构建基线。
- 正式导航/环境数据样包以及合同参数阈值签字确认。

上述内容不阻塞本地演示：缺失输入按照L0-L3降级契约处理，保持低置信度或`valid=false`。

## 6. 下一步唯一动作

人工执行一次Warlock 37节点场景验收：核对节点点击/源目的选点、12个网关六边形标识、
网关能力与最近事件，以及算法主/备路由的`第1跳/第2跳/...`有向逐跳、单跳候选虚线和
网关转换环；随后重跑更新后的10场景完整预验收。
