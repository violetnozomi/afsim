# 验证记录

## 2026-08-16 综合协同场景导航展示

- 根因：25节点综合场景未配置`navigation_errors`，因此采集器正确输出空导航快照；导航
  页面原先没有空状态说明，表现为整页空白。
- 场景修复：7个Link-16空中平台使用AFSIM GPS1，3个UAV使用GPS2降级状态，1个Link-11
  空中中继使用INS确定性漂移；地面固定站和静态卫星保持无导航模型。
- TDD证据：综合场景验证先因缺少三类运行标记失败；实现后实际输出
  `GPS1 1`、`GPS2 2`和`INS -1`，25个平台、四网收件人与无武器事件基线全部通过。
- UI证据：空状态文本测试先因接口不存在编译失败；实现后纯C++测试和完整31/31测试通过，
  Warlock插件重新编译链接成功。
- 实时证据：新Warlock快照在`sim_time=0.550 s`采集11个导航平台，模式分布为
  `GPS_ACTIVE=7`、`GPS_DEGRADED=3`、`INS=1`；直接位置误差来源为`AFSIM_INTERNAL/HIGH`，
  合同1σ精度模板保持`PARAMETERIZED_MODEL/LOW`。
- 边界：参数为可复现PRE_ACCEPTANCE演示值，不代表真实装备精度；未修改AFSIM核心。

## 2026-08-14 合同指标补缺发布候选

- 版本：`0.12.0`；分支：`feat/contract-gap-closure`；AFSIM核心修改数为0。
- 静态门禁通过；甲方接口11个合法样例通过、5个非法样例按预期拒绝。
- 31/31固定C++测试通过，WSF与Warlock插件均完成编译和链接。
- 25节点同体系协同场景`operational_strike_demo`通过，固定阶段、四网消息和
  `Simulation complete`证据齐全。
- 部署契约自检生成有效JSON，并将目标AFSIM ABI、真实四网模块数据、正式安全传输和正式
  导航/环境样包4项正确标为`CUSTOMER_BLOCKED`。
- 完整`scripts/run_preacceptance.sh`最终结果为`Overall: PASS`：31/31测试、9/9批准场景及
  Warlock 120秒快照全部通过；快照为4网络、10端点、8链路、8发送、8接收、0丢弃、
  0路由失败。报告位于
  `output/preacceptance/20260814T050018Z-881952/PREACCEPTANCE_REPORT.md`。
- 首次从隔离工作树运行时发现脚本错误查找工作树输出目录，虽然服务已按时生成正确快照仍
  被误报超时；新增服务进程`NRM_OUTPUT_DIR`发现和回归测试后复跑通过。
- 回退顺序按提交从新到旧执行：`d8cf1f2`、`b8190c3`、`5faabba`、`cf17572`、`e667f46`、
  `f0d766c`、`be37266`、`3c25598`；每个提交均只修改插件工程，可独立`git revert`。
- 结论：本地合同对应功能达到`IMPLEMENTED / PRE_ACCEPTANCE`；未在甲方目标系统验证的项目
  不提升为`FINAL_ACCEPTANCE`。

## 2026-08-13 简化甲方实现自动降级

- 自动判定 `UNAVAILABLE/L0/L1/L2/L3`，输出数据覆盖率、缺失字段、默认值和置信度。
- L0 节点数据可继续候选拓扑评估；L1 使用链路带宽/时延；L2 使用 PDR 与 RF；L3 加入四网验收配额。
- 缺失数据不会以零值冒充测量，无法合理估计的字段保持无效。

## 2026-08-13 并发通信任务联合评估

- 新增 `ConcurrentTaskAssessment`，按规划需求顺序执行确定性链路带宽预留。
- 覆盖共享瓶颈、互不相交路径、输入快照不变、冲突归因、重复编号拒绝和确定性重跑。
- `./scripts/ai_guard.sh test`：20/20 固定 C++ 测试通过，Warlock 插件构建成功。
- 四网协议资源使用可复算验收近似值：Link-11 8 个轮询单元、Link-16 16 个时隙、卫通 4×2 波束信道、CDL 4 条并发链路/8 信道；无需等待甲方字段。

## 2026-08-13 甲方 AFSIM JSON 接口基线

- 验证提交基线：`ff3eb18`（验证后的门禁与文档提交另计）。
- `./scripts/ai_guard.sh static`：通过。
- `./scripts/ai_guard.sh contract`：9 个合法示例通过，5 个非法示例按预期拒绝。
- `./scripts/ai_guard.sh test`：当时的 19/19 固定 C++ 测试通过，WSF 与 Warlock 插件构建成功。
- `./scripts/ai_guard.sh scenario four_network_overview`：退出码 0，四类网络消息均到达并输出 `Simulation complete`。
- 边界：尚未在甲方修改版 AFSIM 源码树重新构建，也未接入甲方真实模块数据，故状态为内部 `PRE_ACCEPTANCE`，不是最终验收。

## 2026-08-11 甲方接口JSON Schema基线

新增Draft 2020-12统一Schema，覆盖提供方握手、四网资源、导航、环境、评估请求、评估响应、
接收ACK和统一错误8类消息。系统`jsonschema 4.10.3`校验全部8个正例通过，故意缺失公共字段
的资源包被拒绝。Schema仅作为`BASELINE_DRAFT`，实时TCP Adapter、安全认证和甲方专用字段
尚未实现或签字确认。

## 2026-08-11 综合通信协同场景25节点验证

综合场景包含区域/网络控制中心、Link-11控制与服务节点、空中中继、任务与支援飞机、
巡查无人机、数据处理中心和双卫星中继，共25个同一体系的协作平台。场景不包含敌方、
异方side、目标航迹、武器模型、武器事件或攻击任务。AFSIM完整180秒命令行场景通过；
现有Warlock采集基线为4种网络、25个唯一通信平台、29个通信端点和60条有向链路。分布为
Link-11 8/14、Link-16 9/24、卫通7/12、CDL 5/10（成员/有向链路）。

固定验证检查25条`PLATFORM_ADDED`、关键四网消息收件人、链路故障/恢复和高速CDL阶段，
同时显式断言不存在`WEAPON_FIRED/HIT/MISSED/TERMINATED`事件。

Link-16同时具备`relay_aircraft_1 → network_control_center → relay_aircraft_2`和
`relay_aircraft_1 → airborne_relay → relay_aircraft_2`两条有向边不重合多跳路径，可用于主备路由
可视化验证。

## 2026-08-11 AFSIM内置导航数据第一版

在不修改AFSIM核心的前提下，新增`WsfNavigationErrors`实时状态采集、原生`.neh`严格解析、
`nrm.navigation_snapshot.v1`快照输出和中文“导航状态”页。固定场景完成
`INS1 → PERFECT → GPS1 → GPS2`状态切换，由AFSIM自身生成40秒导航时序文件；命令行工具
成功将该文件转换为`nrm.navigation_sample.v1` JSONL。解析测试覆盖合法度分秒、状态映射、
未知状态和时间倒退。静态门禁通过，WSF/Warlock/工具构建通过，18/18固定测试通过。

远程Warlock短时冒烟在`sim_time=11.1 s`取得有效导航快照：1个平台、平台名
`nav_aircraft`、原生状态`PERFECT`、总位置误差`0 m`；随后已恢复默认四网用户服务。

能力边界：这是AFSIM感知位置误差数据适配，不是GNSS/INS算法或真实装备精度终验；甲方不同
格式仍需后续Adapter和目标环境联调。

## 2026-08-11 环境影响第一版实现

在不修改AFSIM核心的前提下，实现地形、气象、天象/时间和电磁干扰四域环境快照。Warlock
仿真线程读取`TerrainInterface`、`WsfEnvironment`、`WsfDateTime`和通信接收结果中的干扰/
大气字段，复制为纯C++值对象；Reporter把`nrm.environment_snapshot.v1`作为
`nrm.snapshot.v2.environment`写入JSONL，中文界面新增只读“环境状态”页。

候选链路使用严格`NRM_ENVIRONMENT_V1`配置；非法配置原子失败并保留最后有效修订。现有
AFSIM链路只附加环境证据，不二次修改RF结果；候选链路才调整容量、丢包与时延并标记
`PARAMETERIZED_MODEL/LOW`。验证结果：

- WSF与Warlock目标编译、链接成功；
- 17/17固定测试通过，覆盖合法/非法配置、原子回退、当前链路不重复修正、候选修正、
  地形硬阻断、降雨和干扰开关；
- `environment_weather`使用AFSIM内置日期时间、风、雨、云和沙尘配置，完成8条四网消息收发；
- 重载`nrm-warlock.service`后，运行快照包含四域环境对象；实测4网络、10端点、8链路，
  电磁域记录4条已观测链路和0%最大干扰因子，字段原因码为`NONE`；服务保持`active`。

边界：默认四网场景未启用地形数据库且未布设RF jammer，因此地形显示“可用但未启用”、
干扰功率无样本是正确结果；真实DTED/GeoTIFF、干扰机参数和甲方格式仍待后续联调。

## 2026-08-11 综合突防/IADS作战场景第一轮验证（历史，已被协同场景替代）

以下内容只记录早期原型验证，当前场景已删除全部敌对、目标航迹和武器元素。在不修改AFSIM核心和原始内置场景的前提下，早期曾参考Warlock自带
`data/task_management/run_strike.txt`的突防/IADS组织方式和`effects_test`的显式武器模式，
建立项目自有`test_mission/operational_strike_demo/`。场景包含红蓝双方12个初始平台、
几何传感器、航迹处理、SAM/空空武器、四种网络、链路故障恢复和高速ISR业务。

固定命令`./scripts/validate_operational_strike_demo.sh`通过：

- 13个固定作战阶段标记及`Simulation complete`全部存在；
- 情报站、战区指挥所、前进指挥所、蓝方突击机、红方IADS指挥所和SAM均有消息接收证据；
- AFSIM事件文件记录2条`WEAPON_FIRED`，分别为140.5秒红方SAM对蓝方突击机和145.5秒
  蓝方突击机对红方截击机；
- 非实时命令行仿真完成，实时`mission -rt`成功初始化并进入运行；
- Warlock短时冒烟推进至19.1秒，资源插件采集到4网络、13端点、22链路、1发送、1接收、
  0丢弃、0路由失败，四网类型齐全；随后默认四网服务恢复为`active`。

首次Warlock诊断发现相对`event_output`在短运行目录中无法创建导致初始化失败，已将
`main.txt`可视化入口与`validation.txt`事件证据覆盖层分离。完整180秒Warlock运行、链路
故障前/中/后快照以及武器飞行/命中画面尚未执行，因此状态为内部`PRE_ACCEPTANCE`。

## 2026-08-09 Warlock预验收状态页

在既有一键预验收入口上增加机器可读的`nrm.preacceptance_status.v1`状态契约和Warlock
只读“预验收状态”页。脚本使用临时文件加同目录`mv`原子发布`RUNNING / PASS / FAIL`，
页面通过独立Qt工作线程约每2秒读取，仿真回调和GUI线程均不执行文件读取或JSON解析。
页面没有执行、重启或网络控制按钮。

构建验证：`bash -n scripts/run_preacceptance.sh`与`scripts/ai_guard.sh static`通过；
`NetworkResourceManager`连同Qt MOC重新编译、链接成功。随后完整运行一键预验收，10/10
检查、15/15固定测试、6/6批准场景和Warlock最终快照全部`PASS`；最终快照为4网络、
10端点、8链路、8发送、8接收、0丢弃、0路由失败。报告位于
`output/preacceptance/20260808T173041Z-281748/PREACCEPTANCE_REPORT.md`，机器状态文件已正确
发布`overallStatus=PASS`及相同计数。

## 2026-08-08 一键内部预验收入口

新增`scripts/run_preacceptance.sh`，将环境预检、静态门禁、15项固定测试、六个批准场景和
一次Warlock 120秒最终快照核对收敛为单一命令。首次完整运行结果全部为`PASS`：

- `static`与`test`通过；
- `framework_smoke`、`four_network_overview`、`link_failure`、`congestion`、
  `quality_degradation`和`capability_service_smoke`的退出码及固定标记通过；
- 新Warlock运行最终快照为4网络、10端点、8链路、8发送、8接收、0丢弃、0路由失败；
- 四类网络分别为2发送、2接收、0丢弃；
- Markdown报告、逐项日志和精简快照证据生成成功。

本次报告位于
`output/preacceptance/20260808T055629Z-3872066/PREACCEPTANCE_REPORT.md`。输出目录被Git
忽略；该结果是内部`PRE_ACCEPTANCE`证据，不替代甲方正式接口、样包和目标环境终验。

## 2026-08-08 Warlock接收/丢弃统计修复

远程四网场景曾出现命令行目的端成功接收8条消息，但Warlock最终快照显示
`transmitted=8, received=0, discarded=8`。只读核对AFSIM 2.9源码确认
`WsfObserver::MessageReceived`的参数顺序为`(transmitter, receiver)`，插件回调却按
`(receiver, sender)`解释，导致目的端判定始终失败；待定生命周期记录在60秒后被标记为
`EXPIRED`，进而计入丢弃。

修复仅调整`warlock/source/NrmSimInterface.cpp`的回调参数语义和链路方向，不修改AFSIM
核心、统计公式或合同阈值。验证结果：

- `scripts/ai_guard.sh test`：15/15固定测试通过，两个插件目标构建成功；
- 远程Warlock加载重新构建的插件并自动运行`four_network_overview.txt`；
- 仿真33.2秒快照为`6发送/6接收/0丢弃`；
- 仿真86.0秒快照为`8发送/8接收/0丢弃`，超过首批消息60秒超时边界后未反转；
- 仿真120.0秒最终快照为`8发送/8接收/0丢弃/0路由失败`；
- Link-11、Link-16、卫通和CDL分别为`2发送/2接收/0丢弃`。

最终证据位于本机运行目录
`output/run-20260808T054112Z-3859270-1786167672839-0/resource_snapshots.jsonl`，该运行产物
不纳入Git。

## v0.11.0 模型服务门面与注册表预验收证据

验证日期：2026-08-01
开发基线：commit `d64ba74`
开发分支：`feat/v0.11-model-service-facade`
状态：`IMPLEMENTED / PRE_ACCEPTANCE`

### 基线与最终回归

修改前`scripts/ai_guard.sh static`通过，`scripts/ai_guard.sh test`构建WSF和Warlock插件并
通过原13项测试。实现后守卫再次构建两个插件并通过15项固定测试；新增：

- `nrm_model_service_facade_test`；
- `nrm_model_registry_test`。

Facade测试使用真实领域服务覆盖正常通信能力查询、规划校验与推演、本地分发包和需求匹配。
计数服务端口覆盖空requestId、错误schema、snapshotVersion和规划证据不匹配在下游调用前
拒绝，并确认每项领域操作只委托一次。测试还覆盖固定下游失败结果无损传递、不可预期异常
返回`INTERNAL_ERROR`、无效requestTime、无效context、缺失服务依赖、需求匹配侧陈旧规划
证据、响应回传请求/关联/软件/证据版本，以及快照、规划、需求、候选和profile前后不变。

Registry测试覆盖注册、精确查询、列表、卸载、重复ID+版本拒绝、同ID不同版本并存、非法
描述符拒绝、操作/schema能力查询、语义版本稳定排序和注册失败不破坏有效项。公共头依赖
扫描确认未包含Qt或AFSIM头。

### 运行集成与headless结论

DataContainer构造一个Facade和Registry，注册唯一NRM描述符；现有能力、规划和需求入口
通过Facade委托。审查后确认DataContainer只在Facade响应`valid=true`时设置对应结果状态并
写入Reporter，防止服务依赖缺失或内部异常被误记为有效业务结果。Warlock没有新增服务管理
页面，Reporter没有增加无使用者调用日志。

只读检查AFSIM 2.9的`WsfApplicationExtension::ScenarioCreated`、
`WsfScenarioExtension::ProcessInput`和脚本类型注册API后确认：当前NRM WSF插件只注册空
ApplicationExtension；Facade和资源快照只存在于Warlock DataContainer。headless mission
既不能取得该对象，也没有NRM强类型脚本值对象绑定。文本场景命令不满足强类型请求要求，
因此未创建`network_plan_smoke`、`resource_demand_matching_smoke`或伪造模型服务smoke。

### 边界

- `ContractInterfaceAdapter`仅为抽象边界，没有甲方Adapter实现；
- 未定义HTTP、gRPC、端口、包头、字段号、字节序、JSON/XML或消息队列；
- 未实现导航、环境传播、动态库扫描、第三方模型加载或自动网络控制；
- AFSIM核心源码零修改；
- 合同3.3和3.5仅推进为`IMPLEMENTED / PRE_ACCEPTANCE`，不是`FINAL_ACCEPTANCE`。

---

## v0.10.0 资源需求匹配预验收证据

验证日期：2026-08-01
开发基线：commit `047b020e77b36bc830b06c11525a318f2dc377d3`
开发分支：`feat/v0.10-demand-matching`
状态：`IMPLEMENTED / PRE_ACCEPTANCE`

### 基线与最终回归

修改前`scripts/ai_guard.sh static`通过，`scripts/ai_guard.sh test`构建两个插件并通过原
11项测试。完成后再次执行两个守卫命令，WSF和Warlock插件目标构建成功，13项固定测试
全部通过；新增：

- `nrm_resource_demand_repository_test`；
- `nrm_resource_demand_matching_test`。

### 需求生命周期与匹配

Repository测试覆盖合法加载、显式卸载后重载、round-trip、递增修订、失败加载保护、
拒绝覆盖和原子保存，以及错误magic、未知记录、尾随token、重复ID、NaN/Inf、负值和
PDR越界。内部格式完整文法已冻结在`docs/features/网链资源需求匹配.md`。

Matching测试覆盖完全满足、无路径、网络规模不足，距离、带宽、业务流量、时延和PDR分别
不满足，关键指标无效得到`DATA_INVALID`，以及非有限需求在能力查询前拒绝。可计数查询
替身确认每条有效需求只经过一次能力查询链，业务流量与带宽使用两者较大值映射请求。
边界用例另验证未启用指标缺失不污染PATH、同一平台跨网成员去重、多profile顺序无关，
以及混合网络路径的逐类型业务支持。

规划内容指纹或snapshotVersion不一致时，结果和六类建议固定返回
`PLAN_EVIDENCE_MISMATCH`且不调用能力服务。显式候选乱序输入验证频率、站点、信道、子网
和时隙的稳定排序；路由只复用本次能力结果。缺少候选且无路径时六类均有明确
`UNAVAILABLE`原因。评估前后快照、规划、需求集和profile保持不变。

Reporter测试确认`resource_demand_results.jsonl`和`planning_recommendations.jsonl`
独立写出并登记manifest；恢复测试覆盖两条新增有界队列的精确溢出计数、再次运行隔离和
析构前排空，并验证需求解析异常的结构化`error.log`落盘。Warlock插件已编译并包含“需求匹配”页，
本轮未执行GUI人工点击或截图。

### 固定场景与边界

未创建或运行`resource_demand_matching_smoke`。现有headless AFSIM脚本只能加载WSF插件
并驱动通信事件，不能安全调用Warlock DataContainer中的需求Repository和MatchingService；
按开发指令不以普通消息交付或仅加载插件伪造服务证据。

- 需求格式是内部`NRM_RESOURCE_DEMAND_V1`，不是甲方正式格式；
- 频率、站点、信道、子网和时隙候选必须由调用方显式提供；
- 缺少甲方候选集合或专用规则时输出固定不可用原因，不生成占位值；
- 所有建议只读，不自动建链、改频、改信道、改子网、分配时隙或修改路由；
- 未开发导航、环境传播、优化器、参数扫描或自动网络控制；
- AFSIM核心源码零修改。

---

## v0.9.0 网链规划文件生命周期预验收证据

验证日期：2026-08-01
开发基线：commit `7c76b9410e5d221ff92b1b8e1b94b51650d8a409`
开发分支：`feat/v0.9-network-plan-lifecycle`
状态：`IMPLEMENTED / PRE_ACCEPTANCE`

### 基线与最终回归

修改前`scripts/ai_guard.sh static`通过，`scripts/ai_guard.sh test`发现并通过原9项测试。
完成后再次执行两个守卫命令，WSF和Warlock插件目标构建成功，CTest发现并通过11项测试；
旧9项无回归，新增：

- `nrm_network_plan_repository_test`；
- `nrm_network_plan_evaluation_test`。

### Repository与校验

自动测试覆盖合法加载、卸载后重载、保存新修订、round-trip、失败加载保留当前有效规划、同一
规划修订单调、拒绝覆盖已存在文件、错误magic、未知记录、尾随token、重复ID、NaN/Inf、
负带宽/时延、PDR越界、非法频率、缺失profile/平台、不支持业务、成员超限、重复成员、
时隙资源冲突和JOIN/LEAVE冲突。测试创建的临时文件和目录均在结束前清理。
甲方专用规则缺失时，合法内部规划仍携带`CUSTOMER_RULE_UNAVAILABLE` WARNING，
不会把内部规则静默冒充甲方完整校验。

### 只读推演与本地分发包

自动测试覆盖两条需求全部PASS、带宽不足、时延超限、PDR不足、无路径、离线节点、必需
指标无效和参数化候选路径来源/置信度。结构校验失败时不调用环境/能力链路；推演前后的
`ResourceSnapshot`、规划和剖面保持不变。

只有校验通过且全部需求PASS时才生成本地包。测试解析manifest、校验JSON、推演JSON和
规划正文，确认包内副本为`READY_FOR_DISTRIBUTION`，原规划仍为`DRAFT`，
同一`planId/revision`包拒绝覆盖。规划、校验、推演和manifest以稳定内容指纹绑定；
回归测试确认修改需求参数后不能复用旧结果生成分发包。

Reporter测试确认独立生成`plan_validation_results.jsonl`和
`plan_evaluation_results.jsonl`并登记manifest；恢复测试覆盖两条新增有界队列的
溢出计数和析构排空。Warlock插件已编译并包含“资源规划”页，本轮未执行GUI人工点击或截图。

### 固定场景与边界

未创建或运行`network_plan_smoke`。现有headless AFSIM脚本入口只能加载WSF插件和驱动
通信事件，不能安全调用DataContainer中的规划加载、校验和推演服务；按开发指令不以普通
消息场景伪造规划服务证据。因此当前状态是`IMPLEMENTED / PRE_ACCEPTANCE`，不是
`VERIFIED`或`FINAL_ACCEPTANCE`。

- 规划格式是内部`NRM_NETWORK_PLAN_V1`，不是甲方正式格式；
- 校验只执行已定义的确定性规则，不猜测保护带、干扰或TDMA复用规则；
- 分发仅生成本地不可变目录包，不调用真实网络、OA、消息总线或AFSIM控制API；
- 未实现导航、环境传播、自动建链、改频、时隙分配、路由修改或参数实验；
- AFSIM核心源码零修改。

---

## v0.8.0 通信能力服务预验收证据

验证日期：2026-08-01
开发基线：commit `073c9b7`
开发分支：`feat/v0.8-capability-service`
状态：内部`PRE_ACCEPTANCE`

### 构建与测试

```bash
scripts/ai_guard.sh static
scripts/ai_guard.sh test
scripts/ai_guard.sh scenario capability_service_smoke
```

WSF和Warlock两个插件目标构建成功；守卫发现并通过9项测试。新增
`nrm_communication_capability_service_test`覆盖：

- 当前两跳路径总距离3000 m、最大单跳2000 m；
- 瓶颈可准入速率800 bit/s、路径丢包率28%、累计时延30 ms；
- 10秒观测交付吞吐量瓶颈500 bit/s、成员接入率100%；
- 参数化候选路径的容量、PDR和时延来源为`PARAMETERIZED_MODEL/LOW`；
- 当前路径缺失时延且请求未设置时延上限时，保留当前路径并仅将时延标记无效；
- 当前链路距离、带宽和时延的输入置信度按路径最弱值传播，不被提升；
- 环境适配器在请求校验后接收所选端点路径，四类效果字段完成单位和原因码归一化；
- 无路径、离线端点、零成员分母、环境缺失、NaN/Inf和无吞吐观测；
- 查询前后输入快照版本、端点、链路和观测带宽保持不变。

原8项v0.7测试全部无回归。报告器测试确认独立生成`capability_results.jsonl`，schema为
`nrm.capability.v1`，并在manifest登记；恢复测试覆盖能力队列溢出计数和析构排空。

### 固定场景

`capability_service_smoke`以退出码0完成。该场景用于验证插件加载、当前通信图和消息交付
底座，不直接调用能力查询接口。AFSIM启动信息明确加载
`libwsf_network_resource_manager`，目的端两次输出
`NRM capability smoke received on capability_destination`，最后输出`Simulation complete`。

### 边界

- Warlock插件已编译并包含只读“通信能力”页，但本轮未执行GUI人工点击和截图；
- 地形、气象、天象和电磁干扰没有甲方格式，四项固定输出
  `valid=false / ENVIRONMENT_DATA_UNAVAILABLE`；
- 未实现真实传播模型、规划文件、导航解析或自动资源调度；
- 未修改AFSIM核心源码。

---

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
| 备选路由 | `l16_fighter → l16_relay_1 → l16_command`，候选边 |
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

## 2026-08-11 甲方接口中文注解验证

统一接口 Schema 已使用 Draft 2020-12 标准的 `title` 和 `description` 补充中文说明，并另行
提供允许 `//` 中文注释的 JSONC 人工评审版。检查结果为 33 个 `$defs` 全部具有中文说明；
JSONC 去除注释后内含的八类消息、正式目录中的八类接口正例均通过 Schema 校验，缺少必填
字段的反例被拒绝，静态检查和 `git diff --check` 通过。中文字段速查位于
`schemas/customer/v1/README.md`。

## 2026-08-11 前端中文化验证

网络资源管理器自研界面已统一为中文，包括插件名称、中央作战通信态势标题、顶部运行状态、
九个页签、全部表头和表单、按钮、输入提示、任务评估结果、通信能力结果以及预验收状态说明。
`LINK11`、`LINK16`、`SATCOM`、`CDL`、`PDR`、`RSSI`、`SNR`、`BER`和固定原因码继续保留
原始技术标识，保证配置解析、结果追踪和接口契约不变。AFSIM/Warlock原生菜单不在插件修改
范围内。

构建入口为`./scripts/ai_guard.sh build`；中文化修改后Warlock插件重新编译和链接成功，
`./scripts/ai_guard.sh test`的15项固定测试全部通过。重启`nrm-warlock.service`后，远程
Warlock在4.2秒快照显示4个网络、10个端点、4条已发送和4条已接收消息；中央标题、实时
快照说明、右侧状态、页签、任务表单、按钮和提示均为中文且无乱码。核对截图保存在
`output/ui_validation/chinese_frontend.png`。

## 2026-08-11 现代化前端视觉验证

网络资源管理器自研界面完成现代深色视觉升级：右侧增加标题区和八张实时指标卡片，统一页签、
表格、输入控件、按钮、滚动条、焦点和选中状态；中央态势图改用渐变背景、圆角绘图区、状态
摘要胶囊、柔和网格、节点光晕和图例胶囊。平台按名称聚合，同平台多通信端点不再重复绘制，
多网归属使用彩色徽点表达，链路中点的重复网络文字被移除。

Qt 5.12 Release构建通过，18/18固定测试及静态门禁通过。远程VNC实机截图分别验证默认
10平台/8链路和综合25平台/60链路场景；高密度场景启用六方向标签避碰，服务重载后保持
`active`。截图保存在`output/ui_validation/modern_frontend.png`和
`output/ui_validation/modern_frontend_25_nodes_final.png`。
