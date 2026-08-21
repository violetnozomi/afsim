# AI会话交接记录

### 2026-08-20 14:40 CST — Codex assessment hop route display

- 唯一目标：让Warlock忠实显示资源测评算法已选路径的每一跳，不改路由算法。
- 实际修改：新增`AssessmentRouteHopKind/AssessmentRouteHop`及主备逐跳向量；Evaluator按
  `ConstrainedPath::edges`原顺序投影端点、平台、网络、候选和网关证据；任务评估文本新增
  中文逐跳清单；Reporter新增主/备逐跳JSONL；态势图改为黄色主路由、青色备路、
  `H1/H2/...`箭头、单跳候选虚线和网关转换环，并补充图例。
- 未修改但发现：干净临时AFSIM构建会因上游`UtStacktrace.hpp`缺少`<cstdint>`在基础
  `util`库阶段失败；未修改AFSIM核心，改用已配置Release构建命令对当前项目源码定向
  编译和链接。
- 执行命令：受影响测试严格编译/运行；当前源码投影`nrm_tests`构建与
  `ctest -R '^nrm_'`；`git diff --check`；`ai_guard.sh static/contract`；
  `ai_guard.sh scenario cross_domain_gateway_smoke`；Warlock当前翻译单元编译、动态库链接与
  `readelf`NEEDED检查。
- 测试结果：严格受影响测试7/7、NRM CTest 43/43、静态门、13正例/6反例合同均通过；
  最小跨域场景两次转发、最终收件和正常结束通过；Warlock库显式依赖WSF NRM库。
- 合同追踪键：不变更甲方V1 Schema/Codec；新字段仅用于进程内C++与内部
  `assessment_results.jsonl`审计。
- 阻塞：Warlock人工目视检查未运行；甲方真实四网设备参数、ABI和目标环境仍未提供。
- 下一步唯一动作：用户在Warlock 37节点场景中人工核对主/备路由的有向跳号、网络标签、
  单跳候选虚线和网关转换环。

### 2026-08-19 — Codex cross-domain gateway phase 1

- 唯一目标：实现Link-11、Link-16、SATCOM、CDL六组域对网关，并在一期支持显式多网关级联。
- 实际修改：新增纯C++网关契约/策略引擎、AFSIM场景扩展和跨域处理器；支持显式有序路由、
  方向授权、优先级有界队列、处理/串行化时延、去重、TTL、轨迹、逐跳新序列号及审计事件。
  综合场景从25个基础节点扩展为37个物理节点，新增12个仅双归属的方向专用网关、53个端点、
  108条有向链路、12条直达路由和2条双网关级联。快照、Reporter、Assessment和Warlock节点
  详情均已接入；一键预验收同步为39项固定测试和10个场景。
- 安全收紧：最终通信端点必须与路由模板一致；负跳号、跳号不一致、缺少transferId、TTL耗尽、
  重复传输、环路以及未授权源/目的/消息类型/入口均固定拒绝或丢弃；不隐式拼接路由。
- TDD证据：网关策略、资源测评、Reporter、中文原因码和37节点场景契约均经历失败用例后实现；
  当前源码39/39 C++测试与4/4 Shell回归通过，干净临时源码投影的43/43 NRM CTest通过，
  JSONL经`jq`逐行验证。
- 运行证据：最小场景完成Link-11→SATCOM→CDL两跳并到达最终端；综合180秒场景完成六组
  域对、两条双网关级联和10次逐跳转发，37条`PLATFORM_ADDED`且无武器事件。
- 构建证据：当前源码干净CMake投影完成`nrm_tests`目标构建；WSF三个当前源码翻译单元和
  Warlock全部当前源码/MOC完成Release编译与动态库链接。共享CMake缓存仍绑定旧隔离工作树，
  未将其中的旧CTest二进制冒充当前源码结果。
- 边界：AFSIM核心修改数为0；网关参数和策略均为内部`PRE_ACCEPTANCE`演示配置，不代表真实
  四网设备参数、甲方正式跨域规则或目标环境最终验收。
- 下一步唯一动作：在Warlock 37节点场景人工核对12个网关节点详情、转发事件和源/目的选点，
  再运行更新后的10场景完整预验收。

### 2026-08-18 — Codex tactical node inspection and assessment picking

- 唯一目标：为Warlock通信资源态势图增加节点点击详情，并允许资源测评通过态势图依次选择
  源平台和目的平台。
- 实际修改：新增纯C++命中检测和选点状态机；态势图保存本帧平台点击区域，普通点击显示平台
  ID、位置、端点数量、网络、设备、地址、职责、状态和收发能力；源/目的节点分别使用绿色、
  橙色虚线圈，当前查看节点使用黄色圈。资源测评页新增两个显式选点按钮和模式提示，源点选定
  后自动进入目的点模式，组合框手工选择仍会同步态势图。
- TDD证据：`TacticalSelectionTest`先因模块缺失编译失败，补充最小实现后通过；覆盖最近节点
  命中、点击范围外拒绝、源到目的状态推进、普通查看不修改评估参数和快照节点消失清理。
- 验证：`scripts/ai_guard.sh static`通过；38/38固定C++测试通过；当前Qt头文件MOC生成通过；
  `NrmDockWidget.cpp`、`NrmTacticalView.cpp`和`NrmPlugin.cpp`使用现有AFSIM编译参数通过；替换
  当前对象和MOC后完整链接生成`libNetworkResourceManager_feature.so`成功。
- 构建边界：共享`build-ubuntu24`仍绑定旧`code-quality-raii-hardening`工作树；直接改指向包含
  `.worktrees`的仓库根会重复发现同名扩展，因此已恢复原缓存并采用等价定向编译/链接验证。
  新测试已加入CMake和`ai_guard.sh`，当前源码专用构建目录重配置后完整CTest应增加一项。
- 合同状态：保持`IMPLEMENTED / PRE_ACCEPTANCE`，AFSIM核心修改数为0，公共`include/nrm`
  契约未修改。剩余验证仅为人工点击视觉验收，不影响纯C++选点逻辑测试结论。
- 下一步唯一动作：运行一次Warlock固定场景，人工核对普通点击详情、源点自动切换目的点以及
  选点完成后的资源测评结果。

### 2026-08-11 — Warlock现代化视觉升级

- 实际修改：右侧增加现代深色标题区、八张指标卡片及统一页签/表格/输入/按钮样式；中央图
  增加渐变、圆角面板、摘要胶囊、平台去重、多网徽点和六方向标签避碰。
- 验证：Warlock Release构建、18/18测试、静态门禁通过；10平台和25平台远程VNC截图复核。
- 边界：只修改本插件Qt样式和自绘态势图，不修改Warlock原生菜单、AFSIM核心或业务接口。

### 2026-08-11 — 综合场景去敌对化

- 实际修改：25节点综合场景改为同一协作体系，平台采用指挥、中继、任务、巡查、处理和
  服务角色；删除敌方/红蓝命名、异方side、目标航迹、武器模型和武器发射事件。
- 保留能力：4网络、25平台、29端点、60有向链路、双多跳路径、链路故障/恢复和CDL高负载。
- 验证：完整180秒运行通过，25条`PLATFORM_ADDED`，关键四网收件人全部出现，武器事件为0。

### 2026-08-11 — 甲方接口JSON Schema基线

- 实际修改：新增Draft 2020-12统一Schema、8类正例、校验脚本和完整接口对齐文档；固定
  全量快照、仿真时间、方向链路、质量标记、显式网关、导航/环境防重复处理和只建议原则。
- 验证：8个正例通过系统JSON Schema校验，1个故意缺字段负例被拒绝。
- 边界：当前为`BASELINE_DRAFT`；未实现实时传输Adapter，不代表甲方签字或终验。

### 2026-08-11 — 综合场景扩展至25节点（历史，后续已去敌对化）

- 当时修改：旧版对抗场景曾扩展到25个通信平台和四网29端点、60条有向链路；其敌对元素
  已在后续“综合场景去敌对化”变更中删除，本段只保留历史追踪。
- 验证：AFSIM完整180秒场景通过；Warlock短时快照精确得到4网络、25唯一平台、29端点、
  60链路；默认四网服务已恢复active。
- 路由：护航1到护航2存在经前指和经AWACS两条边不重合的当前多跳路径。

### 2026-08-11 — AFSIM内置导航数据第一版

- 实际修改：增加纯C++`.neh`严格解析器和命令行JSONL转换工具；实时采集
  `WsfNavigationErrors`真值/感知位置、导航状态和误差；快照Reporter增加navigation对象；
  Warlock增加中文“导航状态”页；新增样包、固定状态切换场景和远程启动脚本。
- 验证：静态门禁、WSF/Warlock/工具构建和18/18固定测试通过；AFSIM固定场景完成
  `INS1/PERFECT/GPS1/GPS2`切换并生成原生`.neh`，真实文件可被工具解析；短时Warlock
  取得`nav_aircraft/PERFECT`有效实时快照，默认四网服务已恢复。
- 边界：不修改AFSIM核心，不实现GNSS/INS算法；甲方不同格式通过后续Adapter接入。

## 当前交接摘要

更新时间：2026-08-01。

工作区状态：v0.10实现已完成并通过门禁，位于`feat/v0.10-demand-matching`，尚未提交。

最近稳定基线：v0.9.0，开发起点`047b020`；当前工作区版本`0.10.0`。

下一步唯一动作：用户审查当前v0.10 diff并明确决定是否授权提交；不自动进入下一里程碑。

已知注意事项：

- v0.9仍是内部PRE_ACCEPTANCE，不代表甲方最终验收；
- 甲方接口、样包和目标环境尚未提供；
- 甲方正式规划格式、专用校验规则和真实分发协议不属于当前已实现范围；
- 未经用户指定不得启动参数扫描或研究实验。

## 每次会话结束必须追加

### YYYY-MM-DD HH:MM — AI名称或会话标识

- 唯一目标：
- 实际修改：
- 未修改但发现：
- 执行命令：
- 测试结果：
- 合同追踪键：
- 阻塞：
- 下一步唯一动作：

## 交接纪律

- 只追加新会话，不覆盖前一会话记录；
- 不写“基本完成”，必须使用明确状态；
- 未运行的测试明确写“未运行”；
- 外部资料缺失写明所需文件，不用猜测补齐；
- 新问题进入交接记录，不自动扩展当前任务。

### 2026-08-01 — Codex memory-system

- 唯一目标：建立防止AI偏离主线的项目记忆、命令规则和守卫脚本。
- 实际修改：新增AGENTS.md、CLAUDE.md、docs/ai四份记忆文件和scripts/ai_guard.sh。
- 未修改但发现：另一AI正在main工作区实施v0.7，代码、测试、场景和文档均有未提交改动。
- 执行命令：守卫脚本bash语法检查、help、status和static。
- 测试结果：脚本语法、help和status通过；static正确阻止了VALIDATION.md尾随空格并警告.orig文件；未运行C++测试，避免与并行写入冲突。
- 合同追踪键：无状态变更。
- 阻塞：v0.7并行改动尚未统一编译和验收。
- 下一步唯一动作：并行开发停止写入后运行scripts/ai_guard.sh test，先处理编译和固定回归，不增加新功能。

### 2026-08-01 16:52 CST — Codex v0.7 implementation

- 唯一目标：验证、修正并完成v0.7合同指标基础改动。
- 实际修改：完成版本化网络剖面、严格消息生命周期、状态账本、有界约束路径、可恢复上报、三个故障场景、8项测试和文档；补回v0.6快照窗口字段兼容别名。
- 未修改但发现：AFSIM核心未改；四网场景实际有10个端点而开发指令记录8个；保留用户/并行AI已有`.orig`备份。
- 执行命令：`scripts/ai_guard.sh status/static/test`、两个插件与`nrm_tests`构建、5个批准场景各一次、JSON/YAML格式核验。
- 测试结果：静态守卫通过；8/8测试通过；WSF与Warlock目标构建成功；5个场景退出码均为0；拥塞快照观测到64 Mbit/s offered load。
- 合同追踪键：3.2.2、3.2.3、3.2.5、3.4.2和4.2推进到内部PRE_ACCEPTANCE，不是FINAL_ACCEPTANCE。
- 阻塞：甲方接口/样包/目标环境未提供；三个故障场景的完整前/中/后Warlock截图尚未补齐；`.git`只读，未创建分支、提交或标签。
- 下一步唯一动作：人工补齐三个故障场景的Warlock三阶段截图与评估结论证据，并由用户审查验收门。


### 2026-08-01 — Codex v0.7 review and stabilization

- 唯一目标：审查本轮v0.7开发并形成可复现的稳定提交。
- 实际修改：修复最终目的端交付判定、硬约束备路优先级、容量驱逐终态和NaN/Inf输入校验；补充对应回归用例；清理已核对的编辑残留并统一文档口径。
- 未修改但发现：组播/重传严格语义、真实协议建链事件和三个故障场景Warlock三阶段截图仍未完成；AFSIM核心未修改。
- 执行命令：`scripts/ai_guard.sh static`、`scripts/ai_guard.sh test`、5个固定场景、`git diff --check`及源代码逐项审查。
- 测试结果：8/8测试通过；WSF与Warlock插件构建成功；5个固定场景退出码均为0。
- 合同追踪键：3.2.2、3.2.3、3.2.5、3.4.2和4.2保持内部PRE_ACCEPTANCE。
- 阻塞：甲方接口、样包和目标环境未提供；人工GUI证据尚未补齐。
- 下一步唯一动作：保持本次稳定提交，等待用户明确指定下一项合同功能。

### 2026-08-01 18:06 CST — Codex v0.8 capability service

- 唯一目标：完成v0.8第一阶段`CommunicationCapabilityService`统一能力查询与输出闭环。
- 实际修改：从`073c9b7`创建`feat/v0.8-capability-service`；新增能力公共契约、环境适配接口和纯C++服务；复用评估路径并补充总距离/最大单跳；接入DataContainer、Warlock只读页和独立能力JSONL；新增第9项测试与固定smoke场景；版本升至0.8.0并同步文档和合同追踪。
- 未修改但发现：甲方环境数据格式、样包和目标环境未提供；未实现传播公式、规划、导航或自动资源调度；AFSIM核心未修改。
- 执行命令：`scripts/ai_guard.sh status/static/test`、`scripts/ai_guard.sh scenario capability_service_smoke`、受影响目标构建、YAML/JSON格式检查和`git diff`审计。
- 测试结果：9/9测试通过；WSF与Warlock插件构建成功；能力smoke退出码0并收到两条消息；能力JSONL序列化与队列恢复测试通过。
- 合同追踪键：3.2.3推进为`IMPLEMENTED / PRE_ACCEPTANCE`；4.2组件化证据更新为9项测试；不是`FINAL_ACCEPTANCE`。
- 阻塞：Warlock通信能力页尚未人工截图；环境真实适配受甲方格式和样包阻塞。
- 下一步唯一动作：等待甲方环境格式和样包，并由用户明确启动v0.8第二阶段。

### 2026-08-01 — Codex v0.8 review and stabilization

- 唯一目标：审查并修正v0.8第一阶段实现，形成可提交的稳定版本。
- 实际修改：修复缺失非约束时延导致当前路径被候选路径替换的问题；传播路径距离、带宽
  和时延置信度；将环境适配器调用移到请求校验后并传入所选端点路径；规范化四类环境
  效果固定字段并补齐JSONL序列化；补充对应回归测试和证据边界说明。
- 未修改但发现：固定smoke不调用能力查询接口，只验证插件、当前图和消息交付底座；甲方
  环境格式、样包和目标环境仍未提供；AFSIM核心未修改。
- 执行命令：`scripts/ai_guard.sh static`、`scripts/ai_guard.sh test`、
  `scripts/ai_guard.sh scenario capability_service_smoke`、`git diff --check`和代码逐项审查。
- 测试结果：9/9测试通过；WSF与Warlock插件构建成功；固定smoke退出码0并收到两条消息。
- 合同追踪键：3.2.3和4.2保持`VERIFIED / PRE_ACCEPTANCE`，不是`FINAL_ACCEPTANCE`。
- 阻塞：Warlock通信能力页人工截图，以及甲方环境接口和样包。
- 下一步唯一动作：保持v0.8第一阶段稳定提交，等待用户明确指定下一项合同功能。

### 2026-08-01 — Codex v0.9 development instruction

- 唯一目标：为其他开发AI建立v0.9网链资源规划文件生命周期的可执行指令。
- 实际修改：将`NEXT_DEVELOPMENT_INSTRUCTIONS.md`切换为v0.9任务，明确内部规划契约、严格
  解析、Repository、Validator、只读能力推演、状态机、本地分发包、Warlock集成、测试门
  和禁止事项；将CURRENT_MILESTONE设为PENDING并更新长期记忆。
- 未修改但发现：甲方规划文件格式、专用校验规则、真实分发协议和模型封装规范尚未提供。
- 执行命令：文档与现有公共契约审查；未修改代码。
- 测试结果：仅文档变更，运行`ai_guard.sh static`；未运行C++测试。
- 合同追踪键：3.2.4进入开发规划状态，尚未标记IMPLEMENTED。
- 阻塞：外部格式和真实分发接口阻止FINAL_ACCEPTANCE，不阻止内部生命周期开发。
- 下一步唯一动作：开发AI从`7c76b94`创建`feat/v0.9-network-plan-lifecycle`并记录9项基线
  测试。

### 2026-08-01 19:41 CST — Codex v0.9 network plan lifecycle

- 唯一目标：完成合同3.2.4第一阶段内部网链资源规划文件生命周期。
- 实际修改：从`7c76b94`创建`feat/v0.9-network-plan-lifecycle`；新增严格内部
  规划语法、公共值对象、Repository、Validator、只读Evaluation和本地Distribution服务；
  接入DataContainer、Reporter规划JSONL与Warlock“资源规划”页；版本升至0.9.0并同步追踪。
- 未修改但发现：现有headless AFSIM脚本不能安全调用规划服务，未伪造`network_plan_smoke`；
  甲方规划格式、专用规则、真实分发协议和目标环境仍未提供；AFSIM核心未修改。
- 执行命令：`scripts/ai_guard.sh status/static/test`、YAML解析、公共头依赖扫描、
  `git diff --check`和逐文件代码审查。
- 测试结果：修改前9/9基线通过；完成后11/11测试通过，WSF与Warlock插件构建成功；
  Warlock资源规划页未人工点击或截图。
- 合同追踪键：3.2.4推进为`IMPLEMENTED / PRE_ACCEPTANCE`，不是
  `VERIFIED`或`FINAL_ACCEPTANCE`。
- 阻塞：固定规划服务场景、甲方正式格式/规则、真实分发适配和目标环境联调。
- 下一步唯一动作：用户审查当前diff并明确决定是否提交Git；不自动进入导航、环境、
  自动网络控制或参数实验。

### 2026-08-01 — Codex v0.9 review and stabilization

- 唯一目标：审查并稳定v0.9第一阶段规划文件生命周期实现。
- 实际修改：为规划校验、推演和分发包增加稳定内容指纹，阻止同一planId/revision
  下复用陈旧结果；修正Repository卸载后无法重载同一文件的生命周期问题；补齐
  回归测试、manifest字段和文档证据。
- 未修改但发现：规划推演的snapshotVersion是可审计的点时证据；由于甲方未定义时效策略，
  本轮没有擅自增加“实时快照更新即使规划失效”规则。
- 执行命令：`scripts/ai_guard.sh static`、`scripts/ai_guard.sh test`、`git diff --check`、
  编辑/队列/序列化与路径安全逐项审查。
- 测试结果：11/11固定测试通过，WSF与Warlock插件构建成功，新增陈旧结果和卸载重载
  回归用例通过。
- 合同追踪键：3.2.4保持`IMPLEMENTED / PRE_ACCEPTANCE`，不是`FINAL_ACCEPTANCE`。
- 阻塞：甲方正式规划格式、专用校验规则、真实分发协议和可调用规划服务的固定场景仍缺失。
- 下一步唯一动作：获得用户明确Git提交授权后形成v0.9稳定提交，再单独定义下一里程碑。

### 2026-08-01 — Codex v0.9 commit and v0.10 instruction

- 唯一目标：提交v0.9稳定实现并定义下一阶段可执行开发指令。
- 实际修改：将v0.9提交为`d6f9518`；把下一里程碑设为v0.10资源需求匹配、不满足项分析与
  有限候选六类规划建议；更新开发指令、当前里程碑、长期记忆和决策记录。
- 未修改但发现：甲方六类资源专用规则和正式候选集合尚未提供，v0.10必须对缺失类别输出
  固定`UNAVAILABLE`原因，不能虚构建议值。
- 执行命令：提交前status、diff-check和路径范围审计；v0.9此前已通过static、11项测试和
  两插件构建。
- 测试结果：本次后续仅修改开发文档；代码测试沿用提交`d6f9518`的11/11通过证据。
- 合同追踪键：3.2.4保持`IMPLEMENTED / PRE_ACCEPTANCE`；3.2.5进入v0.10开发规划状态。
- 阻塞：甲方正式需求格式、候选资源、专用校验规则、模型封装和目标环境资料。
- 下一步唯一动作：开发AI从当前分支顶端创建v0.10分支，先记录11项基线，再实现公共契约和
  严格需求文法。

### 2026-08-01 21:20 CST — Codex v0.10 resource demand matching

- 唯一目标：完成合同3.2.5第一阶段的内部资源需求生命周期、批量匹配、逐项差距和显式
  有限候选六类建议。
- 实际修改：从`047b020`创建`feat/v0.10-demand-matching`；新增严格需求文法、公共类型、
  Repository、MatchingService和RecommendationEngine；接入DataContainer、Reporter两类
  JSONL与Warlock“需求匹配”页；版本升至0.10.0并同步合同追踪。
- 未修改但发现：headless AFSIM脚本不能安全调用需求匹配服务，未伪造固定smoke；甲方
  正式需求/候选格式、频率/信道/子网/TDMA专用规则和目标环境资料仍未提供；AFSIM核心未改。
- 执行命令：`scripts/ai_guard.sh status/static/test`、受影响测试和Warlock目标单独构建、
  YAML解析、公共头依赖扫描、`git diff --check`及逐文件审查。
- 测试结果：修改前11/11基线通过；完成后13/13测试通过，WSF与Warlock插件构建成功；
  Reporter新队列溢出、恢复和析构排空通过；Warlock页面未人工点击或截图。
- 合同追踪键：3.2.5推进为`IMPLEMENTED / PRE_ACCEPTANCE`；4.1-4.2组件化证据更新为
  13项测试；不是`FINAL_ACCEPTANCE`。
- 阻塞：固定服务场景、甲方正式需求/候选格式、专用资源规则、模型封装和目标环境联调。
- 下一步唯一动作：用户审查当前diff并明确决定是否授权Git提交；不自动进入导航、环境、
  自动网络控制或参数实验。

### 2026-08-01 23:09 CST — Codex v0.10 review fixes

- 唯一目标：修复v0.10第一阶段审查发现的匹配正确性和异常审计问题。
- 实际修改：PATH不再被未启用约束的缺失指标污染；NETWORK_SIZE按在线平台名去重；
  混合网络能力路径要求每种网络类型均支持业务，同类型多profile不再依赖首项顺序；
  需求加载、草案和保存失败写入结构化`error.log`。
- 执行命令：`scripts/ai_guard.sh static`、`scripts/ai_guard.sh test`、
  `scripts/ai_guard.sh check`和`git diff --check`。
- 测试结果：13/13固定测试通过，WSF与Warlock两插件构建成功；新增未启用指标、
  跨网去重、混合业务支持和需求异常日志回归断言。
- 合同追踪键：3.2.5和3.4.2保持`IMPLEMENTED / PRE_ACCEPTANCE`，未提升为`VERIFIED`。
- 阻塞：甲方正式需求/候选格式、专用资源规则、安全服务场景入口和目标环境联调仍缺失。
- 下一步唯一动作：用户已授权形成v0.10稳定Git提交；提交后保持基线，等待新里程碑指令。

### 2026-08-01 23:37 CST — Codex v0.11 development instruction

- 唯一目标：为其他开发AI建立v0.11模型服务门面与注册表的可执行开发指令。
- 实际修改：将`NEXT_DEVELOPMENT_INSTRUCTIONS.md`切换为v0.11单一任务，定义纯C++强类型
  `ModelServiceFacade`、确定性`ModelRegistry`、抽象`ContractInterfaceAdapter`、最小
  DataContainer集成、测试门和T0-T8执行顺序；同步当前里程碑、长期记忆、决策和实施状态。
- 未修改但发现：甲方模型封装规范、正式接口文件和安全headless服务入口尚未提供；不得据此
  猜测协议或修改AFSIM核心。
- 执行命令：文档、现有服务边界和合同追踪审查；仅文档变更。
- 测试结果：代码未修改，不重复运行C++测试；稳定基线`d64ba74`已有13/13测试和两个插件
  构建通过证据。本轮运行static与diff检查。
- 合同追踪键：3.3、3.5和4.1-4.2进入内部接口边界规划状态，尚未标记IMPLEMENTED。
- 阻塞：甲方封装/接口规范、参考模块、正式样包和官方安全headless服务调用入口。
- 下一步唯一动作：其他开发AI从`d64ba74`创建`feat/v0.11-model-service-facade`，严格执行
  `docs/NEXT_DEVELOPMENT_INSTRUCTIONS.md`，不进入导航、环境、自动网络控制或研究实验。

### 2026-08-01 — Codex v0.11 model service facade

- 唯一目标：完成纯C++进程内强类型模型服务门面、版本注册表和抽象外部接口适配边界。
- 实际修改：从`d64ba74`创建`feat/v0.11-model-service-facade`；新增ModelServiceTypes、
  ModelServiceFacade、ModelRegistry和ContractInterfaceAdapter；DataContainer注册唯一NRM
  描述符并让现有能力、规划和需求入口复用Facade；版本升至0.11.0并同步文档追踪。
- 未修改但发现：AFSIM提供Application/ScenarioExtension和脚本类型注册机制，但当前NRM
  WSF插件没有强类型服务对象或快照绑定，headless mission无法取得Warlock DataContainer。
- 执行命令：`scripts/ai_guard.sh status/static/test`、新增测试单独构建运行、Warlock目标
  单独构建、headless扩展API只读审查和`git diff`审计。
- 测试结果：修改前13/13基线通过；完成后15/15固定测试通过，WSF与Warlock插件构建成功；
  Facade/Registry新增测试覆盖前置拒绝、单次委托、无损失败、输入不变和确定性版本查询。
- 合同追踪键：3.3、3.5和4.1-4.2推进为`IMPLEMENTED / PRE_ACCEPTANCE`，不是
  `FINAL_ACCEPTANCE`。
- 阻塞：甲方封装规范、正式接口/传输协议、参考模块、正式样包和目标环境；无安全强类型
  headless服务调用入口，未伪造规划、需求或模型服务smoke。
- 下一步唯一动作：用户审查当前v0.11 diff并明确决定是否授权Git提交；不自动进入下一
  里程碑。

### 2026-08-02 00:17 CST — Codex v0.11 review fixes

- 唯一目标：审查v0.11第一阶段实现并修复门面状态在Warlock集成中被丢弃的问题。
- 实际修改：DataContainer仅在Facade响应`valid=true`时设置能力、规划推演、分发包和需求
  匹配的有效状态及写入Reporter；补充无效requestTime、无效context、缺失依赖和需求侧陈旧
  规划证据的Facade回归断言；同步验证记录。
- 未修改但确认：领域服务正常返回的不满足/无路径等结果仍属于成功调用并保持原领域原因码；
  未增加甲方Adapter、headless伪入口、网络控制或研究实验。
- 执行命令：`scripts/ai_guard.sh status/static/test/check`、`git diff --check`和公共接口、
  DataContainer、测试及合同追踪逐项审查。
- 测试结果：15/15固定测试通过，WSF与Warlock插件构建成功，static/check通过。
- 合同追踪键：3.3、3.5和4.1-4.2保持`IMPLEMENTED / PRE_ACCEPTANCE`，未提升验收等级。
- 阻塞：甲方封装规范、正式接口/传输协议、参考模块、正式样包和安全强类型headless入口。
- 下一步唯一动作：用户确认审查结果后决定是否授权提交当前v0.11；不自动进入下一里程碑。

### 2026-08-08 — Codex Warlock message statistics fix

- 唯一目标：修复四网场景命令行成功接收8条消息，但Warlock显示接收0、丢弃8的问题。
- 实际修改：按AFSIM `MessageReceived(transmitter, receiver, message, result)`契约修正
  `NrmSimInterface`回调参数语义和链路方向；同步验证记录、四网实测数量和当前里程碑。
- 未修改但发现：通用直连网络没有`MessageHop`回调，现有`hops=0`符合AFSIM事件语义；
  AFSIM核心、统计公式、合同阈值和systemd服务文件均未修改。
- 执行命令：`scripts/ai_guard.sh status`、受影响Warlock目标构建、
  `scripts/ai_guard.sh test`、远程Warlock固定四网场景一次、最终快照JSON核对。
- 测试结果：15/15固定测试通过；33.2秒为6/6/0，86秒和120秒为8/8/0；四类网络
  分别2发送/2接收/0丢弃，Warlock服务保持active。
- 合同追踪键：3.2.2、3.4.2和4.2保持内部`PRE_ACCEPTANCE`，未提升为最终验收。
- 阻塞：甲方接口、正式四网模块/参数、样包和目标环境仍未提供。
- 下一步唯一动作：用户在VNC核对修复后的总览数值，并决定是否授权提交该修复。

### 2026-08-08 — Codex one-command pre-acceptance

- 唯一目标：提供Windows一条SSH命令即可运行、无需点击界面的内部预验收入口和Markdown报告。
- 实际修改：新增`scripts/run_preacceptance.sh`，固定执行环境预检、static、15项测试、六个
  批准场景和Warlock最终快照核对；同步README、运行入口、Windows调试、使用手册和验证记录。
- 未修改但发现：规划、需求匹配和模型Facade仍无安全Warlock强类型headless调用入口，本
  脚本不使用普通消息伪造这些服务调用；AFSIM核心和systemd服务文件未修改。
- 执行命令：`bash -n scripts/run_preacceptance.sh`、`git diff --check`、
  `./scripts/run_preacceptance.sh`。
- 测试结果：预检、static、15/15测试、六场景和Warlock最终快照全部PASS；最终快照为
  4网络、10端点、8链路、8发送、8接收、0丢弃、0路由失败；报告生成成功。
- 合同追踪键：4.2可测试性和可追踪性增加内部PRE_ACCEPTANCE证据，未提升最终验收等级。
- 阻塞：甲方接口、真实四网模块、导航/环境样包和目标环境未提供。
- 下一步唯一动作：用户审查报告并决定是否提交一键预验收脚本和文档。

### 2026-08-09 — Codex Warlock pre-acceptance status page

- 唯一目标：在现有Warlock插件增加最近预验收只读页，自动显示命令行验收进度和结果。
- 实际修改：一键脚本原子发布`nrm.preacceptance_status.v1`的`latest_status.json`；新增独立
  Qt工作线程读取适配器和“预验收状态”页，显示总体结果、检查/测试/场景数、最终快照、
  Git修订及报告路径；同步运行、远程调试、使用手册和验证文档。
- 未修改：AFSIM核心、systemd服务、通信统计/评估算法和网络控制边界；页面无执行按钮。
- 执行命令：`bash -n scripts/run_preacceptance.sh`、`scripts/ai_guard.sh static/build`、
  `git diff --check`和完整`scripts/run_preacceptance.sh`。
- 测试结果：Warlock目标及Qt MOC编译链接成功；10/10检查、15/15固定测试、6/6场景及
  Warlock最终快照全部PASS；状态JSON为PASS并记录4网络、10端点、8链路、8/8/0/0消息。
- 下一步唯一动作：用户在VNC查看“预验收状态”页；确认后决定是否授权提交本轮改动。

### 2026-08-11 — Codex operational strike scenario

- 唯一目标：基于AFSIM内置非密演示结构重建一个可复现的综合突防/IADS四网作战场景并
  开始固定测试。
- 实际修改：新增`operational_strike_demo`五个场景文件、命令行验证脚本和可恢复默认服务
  的Warlock启动脚本；增加ai_guard批准入口并同步运行、使用、状态与验证文档。
- 未修改但发现：AFSIM核心和原始内置场景零修改；Warlock短运行目录不能使用项目相对
  `event_output`，已通过main/validation覆盖层分离解决；中央态势密集区域标签仍有重叠。
- 执行命令：`ai_guard.sh static/scenario operational_strike_demo`、固定验证脚本、实时mission
  初始化、两次Warlock诊断和事件/快照JSON核对。
- 测试结果：13阶段和关键四网收件人通过；2条真实WEAPON_FIRED；Warlock推进到19.1秒并
  采集4网络、13端点、22链路、1发送/1接收/0丢弃，默认服务已恢复active。
- 合同追踪键：3.2.2、3.2.3、3.2.5和4.2增加内部场景证据，仍为PRE_ACCEPTANCE。
- 阻塞：完整180秒Warlock可视化和三个关键阶段截图未执行；真实四网/导航/环境仍待甲方。
- 下一步唯一动作：用户运行一次完整180秒可视化并核对故障、拥塞和武器阶段，再决定是否
  纳入一键预验收和提交Git。

### 2026-08-11 — Codex Warlock Chinese UI

- 唯一目标：将网络资源管理器自研Warlock界面统一改为中文，并保持技术数据契约不变。
- 实际修改：中文化插件和中央态势窗口标题、运行摘要、九个页签、表头、表单、按钮、提示、
  任务评估与通信能力结果、规划/需求状态及预验收显示；同步更新使用和验证文档。
- 保留边界：`LINK11/LINK16/SATCOM/CDL`、`PDR/RSSI/SNR/BER`、原因码、配置值和内部对象名
  保持原样；AFSIM核心、Warlock原生菜单、算法、数据结构和接口未修改。
- 执行命令：`scripts/ai_guard.sh build/test`、重启`nrm-warlock.service`并抓取远程桌面截图。
- 测试结果：Warlock插件编译链接成功，15/15固定测试通过；4.2秒实机快照显示4网络、10端点、
  4发送/4接收，中文标题、状态、页签、任务表单、按钮和提示无乱码。
- 下一步唯一动作：用户在VNC核对其他页签；未经授权不提交Git或进入新里程碑。

### 2026-08-11 — Codex environment effect brief design

- 唯一目标：在甲方无法提供格式的条件下，设计地形、气象、天象和电磁干扰的最小可用方案。
- 实际修改：新增`docs/环境影响简要设计.md`，冻结AFSIM内置优先、内部
  `NRM_ENVIRONMENT_V1`配置补充、甲方Adapter后置的架构、值对象、计算边界和验收场景；
  同步总体报告、架构、状态、使用入口和D-018决策。
- 关键约束：当前AFSIM RF结果只附加环境证据，不重复施加损耗；内部参数只用于候选链路并
  标记`PARAMETERIZED_MODEL/LOW`；无效输入保持`valid=false`。
- 未修改：任何C++代码、AFSIM核心、算法参数、systemd服务和当前运行场景。
- 下一步唯一动作：用户确认设计后，将“环境值对象与严格配置解析”作为独立里程碑实施。

### 2026-08-11 — Codex environment effect implementation

- 唯一目标：按已冻结简要设计实现地形、气象、天象/时间和电磁干扰第一版。
- 实际修改：新增纯C++环境快照、严格`NRM_ENVIRONMENT_V1`配置仓库和内置效果适配器；
  仿真线程采集AFSIM地形、气象、时间、干扰和大气结果；能力服务仅对候选链路应用环境
  修正；JSONL增加环境对象；Warlock增加中文“环境状态”页；新增默认配置和气象场景。
- 未修改：AFSIM核心、真实四网协议、systemd服务定义和自动网络控制；当前RF结果不重复
  应用环境损耗，甲方格式仍通过后续Adapter接入。
- 执行命令：环境目标构建、`scripts/ai_guard.sh static/test`、
  `ai_guard.sh scenario environment_weather`、重载Warlock服务并用`jq`核对实际快照。
- 测试结果：17/17固定测试通过；气象场景8条四网消息全部接收；实际Warlock快照包含
  `nrm.environment_snapshot.v1`四域数据，服务为active。
- 下一步唯一动作：用户在VNC核对“环境状态”页，再决定是否运行完整一键预验收和提交Git。

### 2026-08-14 — Codex network-plan rejection recommendations

- 唯一目标：修复资源规划只读推演未通过时没有可见调整建议的问题。
- 实际修改：`PlanDemandEvaluation`新增逐需求建议；规划评估覆盖无路径、节点离线、带宽、
  时延、PDR和数据无效，并发评估补充带宽/轮询/时隙/卫通/CDL冲突建议；DataContainer将
  并发结果合并回规划状态；Warlock新增独立“调整建议”页并区分资源冲突与独立评估失败；
  规划JSONL、甲方规划响应、Schema、示例和使用文档同步更新。
- 未修改但发现：`planning_recommendations.jsonl`仍专用于需求匹配页的六类结构化候选建议；
  规划级建议写入`plan_evaluation_results.jsonl`，两者不混用。AFSIM核心与systemd未修改。
- 执行命令：四个受影响目标RED/GREEN测试、`scripts/ai_guard.sh static/test`、
  `scripts/validate_customer_interface.sh`、Warlock `NetworkResourceManager`目标构建和二进制
  中文字段核对。
- 测试结果：23/23固定测试通过；9个Schema正例通过且5个负例按预期拒绝；Warlock插件
  编译链接成功，生成库包含“调整建议”“独立评估未通过”和并发建议文本。
- 合同追踪键：3.2.4规划推演与3.2.5评估建议保持`IMPLEMENTED / PRE_ACCEPTANCE`，增加
  失败建议闭环证据，不提升为`FINAL_ACCEPTANCE`。
- 阻塞：当前Warlock进程早于最终“调整建议”页构建，需要重启用户服务后才能加载最终UI；
  甲方正式规划规则、四网模块数据和目标环境仍未提供。
- 下一步唯一动作：重启Warlock，在复杂规划中执行一次“只读推演”，核对自动打开的建议页。

### 2026-08-14 — Codex v0.12 contract gap closure

- 唯一目标：以合同指标对应为优先，用最小实现补齐可由本插件本地完成的功能，并把甲方依赖
  从代码缺口中明确分离。
- 实际修改：扩展统一资源状态和11类JSON Schema；实现频率/协议资源、环境约束、规划域与
  动态成员、分发ACK、需求反馈闭环、导航精度/恢复证据；Reporter和Warlock同步增加合同
  可见字段，新增部署契约自检。
- 版本与回退：版本升至`0.12.0`；功能提交依次为`3c25598`、`be37266`、`f0d766c`、
  `e667f46`、`cf17572`、`5faabba`、`b8190c3`和`d8cf1f2`，可按反序独立回退。
- 验证：静态门禁通过；11个Schema正例通过、5个反例按预期拒绝；31/31固定C++测试及
  WSF/Warlock构建通过；25节点协同场景通过；部署JSON和4项`CUSTOMER_BLOCKED`边界通过。
- 未修改：AFSIM核心、自动建链/改频/切路由控制、甲方正式ABI和安全传输策略。
- 首次完整预验收发现隔离工作树与systemd服务输出根目录不同，导致正确的120秒快照被误报
  为超时；新增进程环境目录发现和`RemotePlanSwitchTest`回归覆盖后复跑通过。
- 最终结果：31/31测试、9/9批准场景、部署检查和Warlock 120秒快照全部PASS；快照为
  4网络、10端点、8链路、8发送、8接收、0丢弃、0路由失败。报告：
  `output/preacceptance/20260814T050018Z-881952/PREACCEPTANCE_REPORT.md`。
- 下一步唯一动作：冻结`v0.12.0`回退点；后续只做甲方目标环境薄适配。

### 2026-08-16 — Codex integrated operational navigation

- 唯一目标：修复25节点综合协同场景导航页无数据且无解释的问题。
- 根因：场景未配置AFSIM`navigation_errors`，采集器快照为`valid=false/platforms=[]`；专用
  导航测试场景正常，证明采集和页面字段链路没有故障。
- 实际修改：为Link-16空中平台配置GPS1、UAV配置GPS2、Link-11空中中继配置INS；导航页
  空样本时显示明确原因，不用理想导航或零误差填充。
- TDD：运行标记验证先失败后通过；UI文本测试先编译失败后通过。综合场景、31/31固定测试、
  WSF/Warlock构建均通过。
- 实机结果：导航快照包含11个平台，`GPS_ACTIVE=7`、`GPS_DEGRADED=3`、`INS=1`，直接误差
  为AFSIM内部高置信度数据，精度模板保持参数化低置信度。
- 未修改：AFSIM核心、地面/静态卫星状态、平台航迹、甲方专用格式和自动控制逻辑。
- 回退：场景导航提交与前端空状态提交相互独立，可按需单独`git revert`。

### 2026-08-16 — Codex code quality and RAII hardening

- 唯一目标：一次性收口整体审查发现的正确性、异常安全、RAII、脚本假通过和证据一致性问题。
- 隔离与回退：在`feat/code-quality-raii-hardening`工作树实施，基线为`a1a5ff8`；未修改AFSIM
  核心，未删除或改签名甲方扩展接口。
- 实际修改：严格化导航/环境/资源/规划JSON运行时校验；统一规划仓库接纳；区分Reporter
  业务域错误与I/O错误并修正线程生命周期；Qt顶层Dock改用`UiPointer`；引入
  `TemporaryPathGuard`覆盖规划、需求、恢复和分发暂存；远程切换失败回滚；部署检查落实
  4核/1.0GHz/2GiB/100GiB/百兆网卡下限；场景门禁要求真正到达`Simulation complete`。
- 死代码结论：`InputProvider`、`NetworkPlanAdapter`、`ContractInterfaceAdapter`是兼容接口；
  频率特性库、恢复状态库和参考适配器分别标记为组件级/样例级，未伪称运行时已接入。
- 验证：静态与11类Schema契约通过；32/32 C++测试、2/2 Shell回归通过；严格告警构建中
  本插件零告警；部署检查PASS；25节点`operational_strike_demo`初始化、消息、导航阶段和
  `Simulation complete`全部通过。
- 已知边界：AFSIM上游仍有`-Woverloaded-virtual`告警和Sphinx缺失提示，本轮遵守核心零修改；
  `NrmDockWidget.cpp`/`NrmSnapshotReporter.cpp`大文件拆分留作独立低风险重构，不混入加固。
- 详细报告：`docs/代码质量与RAII审查报告.md`。
- 下一步唯一动作：用户确认后提交该隔离分支；此后仅做甲方现场薄适配。

### 2026-08-16 — Codex customer JSON state lifecycle closure

- 唯一目标：落实最新版审查提示中可由插件本地完成的甲方JSON接入、状态管理和规划语义收口。
- 实际修改：按具体`networkId`汇总同类型多网络；新增有界`CustomerIngestionState`和
  `CustomerSnapshotAssembler`；资源报告保留导航/环境，导航按平台upsert；规划绑定当前剖面
  版本；外部和界面任务评估统一入口；Provider握手保存；快照变化使旧派生结果失效。
- 环境补齐：保存云量、太阳高度角、地形阻断链路、干扰影响链路和容量缩放，并只对明确
  受影响候选链路应用`CANDIDATE_ADJUSTMENT`。
- 技术取舍：未引入通用Draft 2020-12引擎，改为对全部受支持V1消息执行严格允许字段及现有
  类型/范围/枚举/引用校验；规划文件没有完整拓扑边，因此不伪造规划后网络。
- 验证结果：静态门禁、11类Schema契约、33/33固定C++测试、36/36完整CTest、部署检查、
  WSF/Warlock构建和25节点`operational_strike_demo`均通过。
- 未修改：AFSIM核心、自动控制、真实四网协议和安全传输层。

### 2026-08-17 — Codex unified service and demand JSON closure

- 唯一目标：核实新一轮代码评审，收口仍真实存在的“模块已有但未进入统一主链”问题。
- 实际修改：任务评估新增`ModelServiceFacade`操作、端口、默认实现、强类型响应、Registry
  能力和健康依赖；DataContainer不再直接构造评估器。新增并发资源需求请求/响应Schema、
  严格编解码、配置版本绑定、需求仓库接纳、匹配执行和结构化响应。
- 接口命名：当前工作的精简V1边界明确为`CustomerJsonContractAdapter`，保留旧
  `CustomerJsonCodec`别名；抽象`ContractInterfaceAdapter`继续作为未知现场传输SPI。
- 技术取舍：规划输入没有潜在拓扑和控制动作，未伪造规划后虚拟快照；建议保持只读；
  HTTP/TCP/MQ传输仍等待甲方部署协议，当前文件/进程内响应字节已具备。
- 验证：静态门禁通过；13个Schema正例通过、6个反例按预期拒绝；33/33固定测试和
  36/36完整CTest通过；WSF/Warlock插件构建、部署检查和25节点
  `operational_strike_demo`均通过并到达`Simulation complete`。

### 2026-08-17 — Codex in-process customer adapter closure

- 唯一目标：把甲方接入从“待定网络Transport”纠正为甲方修改版AFSIM内部的同进程模块调用。
- 实际修改：新增纯C++ `CustomerNrmAdapter`和`CustomerNrmHost`，统一资源、导航、环境状态
  生命周期及任务评估、规划推演、并发需求入口；`DataContainer`实现宿主端口并公开
  `GetCustomerNrmAdapter()`，复用唯一`ModelServiceFacade`和仓库。
- 接口职责：`ContractInterfaceAdapter`只负责未来甲方私有对象到公共值对象的薄转换；具体
  JSON类恢复命名为`CustomerJsonCodec`，仅用于Schema合同、文件、测试、回放和验收。
  `provider_hello/ingest_ack/error`保留为文件工具记录，不再描述为网络握手响应链。
- 状态语义：直接C++调用与JSON文件入口共享`CustomerIngestionState`；无效输入在任何宿主
  变更前返回`REJECTED`，重复与迟到输入不改变快照，新run清除上一run的导航/环境动态状态。
- 范围取舍：不新增HTTP、TCP、WebSocket、MQ、端口、TLS或独立服务进程；没有甲方对象头文件
  时不伪造具体转换类，也没有在缺少潜在拓扑时伪造规划后虚拟快照。
- 验证：静态门禁、13正例/6反例契约、34/34固定测试、37/37完整CTest、WSF/Warlock构建、
  部署检查和25节点`operational_strike_demo`均通过；场景到达`Simulation complete`。
- 未修改：AFSIM核心。当前仍需甲方提供私有对象头文件、字段语义和目标ABI构建条件，之后
  只实现`ContractInterfaceAdapter`薄映射。

### 2026-08-17 — Codex customer interface acceptance closure

- 唯一目标：针对外部代码审查指出的未闭环项，直接补齐甲方进程内接口语义和真实入口证据。
- TDD修复：端点/链路新增明确`networkId`并按实例校验和统计；导航新增`platformId`并按其
  upsert；环境新增三态枚举与可区分证据，甲方`INFORMATION_ONLY/ALREADY_INCLUDED`不再被
  候选路径逻辑覆盖；资源需求缺省任务阶段统一降级为`UNSPECIFIED`。
- 运行时校验：新增可替换`CustomerJsonValidationLayer`，所有JSON解码先走统一校验入口，
  再执行领域类型、范围、引用和跨对象语义校验；未引入重量级第三方Draft引擎。
- 真实入口：新增`CustomerDataContainerTest`，从文件加载资源、两个导航平台、环境、评估、
  规划和并发需求，验证评估响应JSON、活动配置版本绑定、同运行资源刷新保留导航/环境以及
  Assessment/Capability/Plan/Distribution/Demand旧结果全部失效。
- 验证：静态与13正例/6反例Schema契约通过；35/35固定C++测试、37/37完整NRM CTest、
  WSF/Warlock构建、部署检查和25节点场景均通过；场景到达`Simulation complete`。
- 边界：未增加HTTP/TCP/WebSocket/MQ，未修改AFSIM核心，未实现甲方私有对象薄映射、自动
  规划或复杂虚拟快照What-if。

### 2026-08-17 — Codex final architecture closure

- 唯一目标：完成外部评审要求中仍真实存在的最后断点，并把隔离worktree成果安全提交、合并
  回正式开发分支；没有扩展到Transport、自动规划或大型What-if引擎。
- 共享校验：新增纯C++`ResourceSnapshotValidator`，统一校验具体`networkId`归属、端点、
  链路、指标、协议资源、路由和网关语义；JSON解码和`CustomerNrmAdapter`直接入口共用。
- 环境评估：独立Assessment经唯一Facade复用Capability环境链；只读信息与已包含影响不重复
  衰减，仅候选修正参与带宽、时延、PDR和硬阻断判定。
- 数据源仲裁：`EffectiveSnapshotAssembler`固定AFSIM拓扑/实时链路为基础态，甲方导航和环境
  为持久覆盖层；无AFSIM基础态时保留甲方完整回放能力，发布版本保持单调。
- 工程收口：校验脚本默认`python3`并支持`PYTHON_BIN`覆盖，缺失`jsonschema`给出明确错误；
  `ai_guard.sh test`已纳入本轮两个新增C++测试，避免新增测试只编译不执行。
- 验证：`git diff --check`、静态门禁、13正例/6反例合同、37/37固定C++测试、40/40完整
  CTest、WSF/Warlock构建、部署检查和25节点场景全部通过；场景到达`Simulation complete`。
- 边界：AFSIM核心修改数为0；甲方私有对象薄映射和目标ABI构建仍需甲方头文件与现场环境。

### 2026-08-17 — Codex final boundary closure

- 唯一目标：不改AFSIM核心、不改业务算法，收口Environment三态、Customer Overlay、
  canonical资源校验、JSON关键规则和Python脚本可移植性。
- TDD证据：5类新断言先在旧实现上失败；修复后Environment三态、导航按平台覆盖、
  环境按子域覆盖、坐标/覆盖/业务流/姿态校验及identifier/source规则均通过。
- 解决根因：Customer更新不再从Effective Snapshot复制AFSIM值；`applicationMode`是环境
  影响的唯一权威开关；环境子域单独保留来源与置信度。
- 验证：`git diff --check`、`ai_guard static`、使用`PYTHON_BIN=/usr/bin/python3`的
  `ai_guard contract`、37/37固定C++测试、41/41完整CTest、WSF/Warlock构建和25节点
  `operational_strike_demo`全部通过；场景到达`Simulation complete`。
- 工程状态：本轮变更在`feat/code-quality-raii-hardening`完成独立验证和提交，并快进合入
  `feat/v0.11-model-service-facade`；未打标签，AFSIM核心修改数0。
- 剩余边界：只等待甲方真实AFSIM C++对象、字段语义和ABI环境，再实现
  `ContractInterfaceAdapter`薄映射与联合运行验证。

### 2026-08-17 — Codex final code and Git closure

- 统一语义：Assessment、Plan Demand和Resource Demand均接受`maximumDelayMs=0`表示无门限，
  负值拒绝；规划Allocation至少一名成员，Schema与Runtime一致。
- 环境粒度：`customerProvidedDomains`由JSON或同进程Adapter根据实际输入推导，Customer
  `applicationMode`不再控制未提供的AFSIM子域；显式效果使用对应子域origin/confidence和证据。
- canonical校验：补齐建链时延、RSSI、SNR等关键浮点有限值检查，并保持
  `ValidationResult.valid == issues.empty()`。
- 验证：差异检查、静态门禁、13正例/6反例合同、37/37固定C++测试、41/41完整CTest、
  WSF/Warlock构建及25节点`operational_strike_demo`全部通过。
- 边界：生产插件和算法全部为C++；Python只执行离线JSON Schema测试。未新增Transport，未修改
  AFSIM核心，后续只等待甲方私有C++对象和目标ABI完成薄映射及联合仿真。

### 2026-08-20 — Codex Warlock launch and staged plugin repair

- 唯一目标：排查并修复`run-operational-strike-warlock.sh`运行后界面无明显反应的问题。
- 根因：旧综合场景进程实际已在VNC前台运行，但共享AFSIM构建缓存仍绑定旧隔离
  工作树；已部署Warlock UI库因缺少当前MOC与`NrmTacticalSelection.cpp`对应对象而含
  8个未解析符号，Warlock因此拒绝加载UI插件。
- 修复：用当前源码投影重新生成Qt MOC，按AFSIM Release参数重编译11个Warlock编译
  单元，与正式AFSIM库重新链接并部署UI插件；又建立
  `/home/pyh/afsim/nrm-current-extension-projection`独立发现目录，避免AFSIM递归扫描主仓库内
  `.worktrees`，并将共享构建缓存持久重绑到当前工作区。未修改AFSIM核心。启动脚本新增
  VNC、前台运行和`Ctrl+C`退出提示。
- 验证：正式共享目录完成`wsf_network_resource_manager`、`NetworkResourceManager`和`nrm_tests`
  的1241步全量依赖重建；`ldd -r`未解析符号0、缺失依赖0；43/43 NRM CTest、静态门禁、
  `git diff --check`和网关场景契约测试全部通过。默认场景及综合场景进程均实际
  映射UI/WSF两个插件，`operational_strike_demo/main.txt`窗口已在VNC `:1`中显示。
- 运行语义：命令保持前台是预期行为，界面显示在VNC而不是SSH终端；按`Ctrl+C`后
  脚本恢复`nrm-warlock.service`。本轮验证结束时默认服务已恢复为`active`。
- 下一步唯一动作：用户在VNC中人工核对节点点击、评估源/目的选点、网关和主备逐跳
  路由图层；未经授权不提交或推送Git。

### 2026-08-20 — Codex Warlock UI scaling and Chinese readability

- 唯一目标：增加插件界面放大功能，并减少仿真页面中的裸英文变量；不改仿真协议、算法、
  网关策略、JSON合同或稳定内部标识。
- TDD：新增纯C++`NrmUiScale`和边界测试，先观测CMake因实现缺失失败；扩展中文映射测试时
  先观测`NO_SAMPLES`和`PATH`断言失败，再补齐全部当前界面可见枚举。
- 实现：资源面板提供80%–160%按钮与快捷键，字体、控件和表格尺寸同步调整；态势图使用
  同一比例的逻辑画布并反算点击坐标。网络类型、职责、协议资源、状态、来源、置信度、
  原因码和单位中文优先，唯一标识与合同码保持不变。
- 验证：44/44 NRM CTest、静态门禁和差异检查通过；Warlock插件重新链接，运行环境
  `ldd -r`缺失依赖0、未解析符号0。VNC实测100%→110%同步放大、放大后节点点击详情及
  恢复100%均成功；`nrm-warlock.service`最终为`active`。
- 工程状态：AFSIM核心修改数0，未提交、未推送；37节点网关和主备逐跳路由的完整人工验收
  仍是下一项工作。

### 2026-08-20 16:12 CST — Codex Warlock scaling correction

- 唯一目标：修复Warlock界面只局部缩放的问题，并恢复`Link-11`/`Link-16`/
  `SATCOM`/`CDL`专有名称显示。
- 实际修改：新增`NrmUiStyle`，用后代QSS规则同步缩放全部子控件字号、控件内边距、
  圆角、滚动条和按钮尺寸，并保存基准布局边距/间距后按比例重算；快捷键作用域改为整个
  Warlock窗口。由于Warlock核心已占用`Ctrl+0`分配平台组0，恢复默认改为
  `Ctrl+Shift+0`。网络专名和`SATCOM终端`不再全中文化。
- 根因：旧QSS的基础字号只选中`QWidget#NrmRoot`，Qt子控件未继承；多数控件内边距和布局
  间距仍为固定像素。终端运行时另捕获`QAction::event: Ambiguous shortcut overload:
  Ctrl+0`，证实恢复快捷键与Warlock内置动作冲突。
- TDD证据：`nrm_ui_scale_test`先因`NrmUiStyle.cpp`缺失导致CMake配置失败，实现后用真实
  Qt Widgets验证子按钮字号、高度、布局边距和间距的100%→160%变化；
  `nrm_ui_text_test`先在`LINK11`专名断言失败，修正映射后通过。
- 执行命令：受影响测试定向构建/运行；`NetworkResourceManager`构建；
  `scripts/ai_guard.sh static/test`；`git diff --check`；VNC临时Warlock窗口按钮与XTest
  快捷键验证。
- 测试结果：40/40固定测试通过，Warlock插件编译链接通过；VNC验证100%→110%
  时表单字体、输入框、按钮、页签和间距全部可见放大，态势图焦点下
  `Ctrl++`与`Ctrl+Shift+0`通过，平台选择显示`Link-16`。
- 合同追踪键：不变；仅修改Warlock显示层与其测试，不改仿真状态、路由算法、网关策略、
  公共契约或JSON Schema，状态保持`IMPLEMENTED / PRE_ACCEPTANCE`。
- 阻塞：完整120秒GUI预验收未重跑；当前用户综合场景进程仍映射启动时的旧库，需用户
  退出后重新执行运行命令才会加载本次新库。
- 下一步唯一动作：用户重启综合场景，在常用分辨率下人工核对80%/100%/160%的可读性和
  页面滚动/裁剪边界。

### 2026-08-21 09:45 CST — Codex tactical-view-only scaling correction

- 唯一目标：将缩放作用域收窄为中央态势图，右侧资源测评及其他表单、按钮、字号、布局、
  表格行高和表头尺寸保持固定；不改仿真状态、路由算法、跨域网关、JSON合同或专名映射。
- TDD证据：先将真实Qt Widgets测试改为断言面板几何保持不变，旧实现如预期在按钮字号
  10pt→16pt处失败；随后将`NrmUiStyle`改为无比例参数的固定主题，测试转绿。
- 实现：缩放栏文案改为“态势图缩放”；`SetUiScalePercent`仅刷新百分比并发送既有信号，
  删除按比例重写后代QSS、布局边距/间距、表格尺寸和指标卡高度的代码。`NrmTacticalView`
  的80%–160%逻辑画布缩放与鼠标坐标反算保持不变。
- 自动验证：`scripts/ai_guard.sh static`、40/40固定测试、44/44 NRM CTest、插件编译链接及
  运行环境`ldd -r`全部通过；缺失依赖0、未解析符号0。
- VNC证据：独立临时Warlock加载新库，100%→110%后任务评估表单截图变化像素为0，态势图
  变化像素为11748。临时窗口已关闭，临时目录已移入回收站；用户综合场景PID 925555未停止，
  其当前映射的旧UI库显示`(deleted)`，需重启综合场景后才会加载本轮新库。
- 工程状态：AFSIM核心修改数0，未提交、未推送；历史全页面缩放记录保留为历史证据，
  README、当前里程碑和验证文档已更新为“仅态势图缩放”。
- 下一步唯一动作：用户退出当前综合场景并重新执行运行命令，人工核对常用窗口尺寸下的
  80%/100%/160%态势图效果、节点详情和资源测评选点。

### 2026-08-21 10:18 CST — Codex map-style tactical viewport zoom

- 需求修正：用户明确否定“整体逻辑画布放大”，要求像地图一样放大局部以查看密集节点、
  链路和逐跳路径细节；此前两版缩放记录保留为历史，本条为当前实现。
- 实现：新增`NrmTacticalViewport`，提供50%–500%、25%步进、鼠标锚点缩放、受限平移、
  内容/视口正逆变换和适配全图。`NrmTacticalView`仅对网格、节点、链路、网关、逐跳路径和
  标签施加变换，标题、摘要、提示、图例和详情卡固定；左键移动超过Qt阈值才平移，否则按
  节点单击处理。滚轮比例通过新信号同步回Dock。
- TDD证据：先扩展`nrm_ui_scale_test`，因缺少`NrmTacticalViewport.hpp`按预期编译失败；
  实现后鼠标锚点保持、QTransform一致性、正逆映射、平移、边界钳制、适配全图和50%–500%
  规则全部通过。资源面板固定字体/几何测试继续通过。
- 自动验证：`scripts/ai_guard.sh static`、40/40固定测试、44/44 NRM CTest、插件编译链接和
  `git diff --check`通过；只出现AFSIM上游既有虚函数隐藏告警。
- VNC证据：独立临时Warlock以3440×1348加载新库，在密集区100%→200%局部放大；拖动后
  拓扑变化687967像素，固定图例和右侧表单变化均为0。200%下点击
  `network_control_center`显示固定详情；选源模式再次点击后右侧源平台更新并自动进入选目的
  模式；“适配全图”恢复100%和37节点全局视野。
- 清理/运行态：临时窗口已关闭，临时目录移入系统回收站；当前用户侧为默认
  `four_network_overview` Warlock PID 1438301。该进程启动早于本轮最终库，用户需重启后加载。
- 工程状态：AFSIM核心修改数0，未提交、未推送；路由算法、网关策略、JSON合同、中文映射及
  `Link-11`/`Link-16`/`SATCOM`/`CDL`专名未改变。
- 下一步唯一动作：用户重启当前Warlock，在自己的VNC视角下滚轮放大密集区、拖动浏览，
  并复核逐跳路由标签与资源测评选点。
