# AI会话交接记录

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
