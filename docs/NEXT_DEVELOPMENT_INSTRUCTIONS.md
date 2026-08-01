# Network Resource Manager v0.11 开发指令

## 1. 唯一目标

在v0.10稳定基线上实现一个**纯C++、进程内、可测试的模型服务门面与模型注册表**，
将已有通信能力、网链规划和资源需求匹配统一为稳定调用边界，为后续甲方模型封装
和导航/环境数据适配留出接入点。

本阶段对应合同：

- 3.3 模型封装与接口要求的内部预验收边界；
- 3.5 符合统一契约的功能模块兼容与注册机制；
- 4.1至4.2 模块化、高内聚、低耦合和可扩展性。

本阶段不实现甲方网络传输协议，不宣称已完成甲方最终网络化服务集成。

## 2. 开发起点

稳定基线：

```text
branch: feat/v0.10-demand-matching
commit: d64ba74
version: 0.10.0
tests: 13/13 PASS
```

开发AI必须先执行：

```bash
git status --short
git rev-parse --short HEAD
scripts/ai_guard.sh static
scripts/ai_guard.sh test
git switch -c feat/v0.11-model-service-facade
```

如工作区非干净，不得删除、覆盖或回退未知改动；应先查明归属。

## 3. 单任务边界

允许修改：

- `include/nrm/`下新增的公共服务契约、Facade和Registry；
- `source/`与`warlock/source/`中必要的薄适配层；
- `tests/`、`CMakeLists.txt`和`scripts/ai_guard.sh`；
- 相关架构、验证、使用手册、合同追踪和AI记忆文档。

禁止：

- 修改AFSIM核心源码；
- 引入HTTP、gRPC、消息队列、数据库、Docker或微服务框架；
- 猜测甲方协议字节序、包头、字段号、端口或传输方式；
- 实现GNSS/INS解算、传播模型、环境模型或新路由算法；
- 自动建链、改频、改信道、改时隙、改子网或改路由；
- 重写能力指标、路径搜索、规划校验或需求匹配逻辑；
- 开发新仪表盘、拓扑动画或大型GUI；
- 参数扫描、性能调优或研究实验。

## 4. 必须复用的现有组件

Facade只编排，不复制业务逻辑：

- `CommunicationCapabilityService`；
- `NetworkPlanValidator`；
- `NetworkPlanEvaluationService`；
- `NetworkPlanDistributionService`；
- `ResourceDemandMatchingService`；
- `NetworkProfileRepository`；
- `ResourceSnapshot`及已有请求/结果值对象；
- 已有planId/revision/fingerprint/snapshotVersion证据规则；
- `SnapshotReporter`的有界、异步、runId隔离模式。

## 5. 公共契约

新增`include/nrm/ModelServiceTypes.hpp`，至少定义：

### 5.1 调用上下文

`ModelServiceContext`：

- `schemaVersion`；
- `requestId`；
- `correlationId`；
- `callerId`；
- `softwareVersion`；
- `snapshotVersion`；
- `requestTime`；
- `source`、`confidence`、`valid`。

规则：

- `requestId`不得为空；
- `schemaVersion`必须精确匹配；
- 涉及快照的调用必须使用与输入快照相同的`snapshotVersion`；
- 无效上下文不调用下游业务服务。

### 5.2 操作与状态

`ModelServiceOperation`至少包含：

- `QUERY_CAPABILITY`；
- `VALIDATE_PLAN`；
- `EVALUATE_PLAN`；
- `GENERATE_DISTRIBUTION_PACKAGE`；
- `MATCH_RESOURCE_DEMANDS`；
- `GET_HEALTH`；
- `GET_DESCRIPTOR`。

`ModelServiceStatus`至少包含：

- `SUCCESS`；
- `INVALID_REQUEST`；
- `UNSUPPORTED_SCHEMA`；
- `EVIDENCE_MISMATCH`；
- `SERVICE_UNAVAILABLE`；
- `INTERNAL_ERROR`。

程序判断不得依赖自由文本。

### 5.3 服务响应

每个响应必须包含：

- 原`requestId`与`correlationId`；
- 操作类型与固定状态；
- `softwareVersion`、`snapshotVersion`；
- 对应业务结果值对象；
- 结构化原因码列表；
- 来源、置信度和`valid`。

结果类型可以使用显式结构体或C++17 `std::variant`，不得使用`void*`、裸指针
所有权或JSON字符串在内部代替强类型值对象。

## 6. ModelServiceFacade

新增`include/nrm/ModelServiceFacade.hpp`。Facade应：

1. 构造时显式注入profile Repository和必要服务，不使用隐式全局单例；
2. 先校验`ModelServiceContext`和证据身份；
3. 每个操作只调用一次对应现有服务；
4. 不修改输入快照、规划、需求、候选和profile；
5. 不捕获后将异常静默转为成功；
6. 可恢复输入错误返回固定状态和原因码；
7. 只有不可预期内部异常返回`INTERNAL_ERROR`；
8. 不自动写网络、规划或需求文件。

建议显式公开方法，不建议一个包含巨大switch的弱类型`Dispatch(string, string)`：

```text
QueryCapability(context, snapshot, request, environment)
ValidatePlan(context, snapshot, plan)
EvaluatePlan(context, snapshot, plan, environment)
GenerateDistributionPackage(context, plan, validation, evaluation, outputRoot)
MatchResourceDemands(context, snapshot, demandSet, optional plan/evaluation/candidates)
GetHealth(context)
GetDescriptor(context)
```

## 7. ModelRegistry

新增`include/nrm/ModelRegistry.hpp`，实现进程内注册和查询，不做动态库扫描。

`ModelDescriptor`至少包含：

- `modelId`；
- `modelName`；
- `modelVersion`；
- `providerId`；
- `supportedOperations`；
- `supportedRequestSchemas`；
- `supportedResponseSchemas`；
- `source`、`confidence`、`valid`。

Registry必须支持：

- `Register`；
- `Unregister`；
- `Find`；
- `List`；
- `SupportsOperation`；
- `SupportsSchema`。

固定规则：

- 空ID、无效版本或空操作集拒绝注册；
- 同一`modelId + modelVersion`拒绝重复注册；
- 不同版本可并存，查询必须指定精确版本；
- `List`按`modelId`、版本和provider稳定排序；
- 注册表不拥有AFSIM对象，不保存Qt指针；
- 首版只注册已有NRM Facade，不伪造第三方模型。

## 8. 外部适配边界

新增`include/nrm/ContractInterfaceAdapter.hpp`，但只定义抽象边界：

- 查询adapter标识、版本和支持的schema；
- 将外部请求转为强类型内部请求；
- 将内部响应转为调用方所需的结果；
- 返回校验状态和固定原因码。

甲方接口文件未提供前：

- 不定义网络端口；
- 不定义二进制布局；
- 不定义JSON/XML字段名；
- 不实现导航包解析；
- 不创建“甲方Adapter”假实现。

允许为纯C++测试提供`FakeContractAdapter`，必须位于`tests/`且名称明确表明测试用途。

## 9. 运行集成

### 9.1 DataContainer

DataContainer可持有一个Facade和Registry，但GUI不得重新计算业务逻辑。已有按钮和页面
保持兼容，首版不新增独立“服务管理仪表盘”。

仅允许在现有状态区增加简短的服务版本或健康状态；若对合同交付没有必要，不要改GUI。

### 9.2 Reporter

如新增服务调用记录，只允许输出`model_service_calls.jsonl`，必须：

- 有界队列；
- 析构前排空；
- manifest登记文件名、丢弃数和写错误；
- 记录请求身份、操作、状态、snapshotVersion、原因码和执行时长；
- 不序列化敏感原始载荷；
- 不在仿真回调线程写文件。

若无必要，可以不改Reporter，不得为了“看起来完整”制造无使用者的日志。

## 10. 安全headless入口决策

先用只读方式调查当前WSF插件是否存在**官方支持、不修改AFSIM核心**的脚本命令或
服务注册入口。

如果存在：

- 只增加一个薄入口，调用Facade；
- 输入必须是强类型、版本化值对象；
- 分别补齐`network_plan_smoke`与`resource_demand_matching_smoke`；
- 每个场景只运行一次，不做参数调整。

如果不存在或需要修改AFSIM核心：

- 立即停止运行入口扩展；
- 在`SESSION_HANDOFF`记录查看的API、结论和所需甲方输入；
- 保留Facade/Registry纯C++验证；
- 状态仍为`PRE_ACCEPTANCE`；
- 不得使用普通通信消息、硬编码成功输出或只加载插件冒充服务smoke。

## 11. 测试要求

新增至少：

1. `nrm_model_service_facade_test`；
2. `nrm_model_registry_test`。

Facade测试必须覆盖：

- 正常通信能力查询；
- 正常规划校验与推演；
- 正常需求匹配；
- 空requestId、错误schema和snapshotVersion不匹配；
- 无效请求不调用下游服务；
- 每个操作只调用一次下游服务；
- 下游固定失败状态的无损传递；
- 评估前后快照、规划、需求、候选和profile不变；
- 返回requestId、操作、软件版本和证据版本。

Registry测试必须覆盖：

- 注册、精确查询、列表和卸载；
- 重复ID+版本拒绝；
- 同ID不同版本并存；
- 非法描述符拒绝；
- 操作/schema能力查询；
- 乱序输入下列表稳定；
- 注册失败不破坏已有有效项。

完成后固定测试总数至少15项。不得删除或放宽现13项断言。

## 12. 实施顺序

### T0：基线与分支

- 核对commit `d64ba74`；
- 记录13项基线测试；
- 创建v0.11分支；
- 声明本轮唯一目标。

### T1：契约先行

- 先写`ModelServiceTypes`和功能文档；
- 固定schema、状态、原因码和证据身份；
- 再写实现。

### T2：Facade

- 注入现有服务；
- 实现上下文校验与显式操作；
- 保持输入不变和固定错误语义。

### T3：Registry

- 实现描述符、注册、卸载和查询；
- 注册NRM Facade本身；
- 不扫描动态库。

### T4：适配边界

- 只定义`ContractInterfaceAdapter`抽象；
- 测试adapter只放`tests/`；
- 不猜测甲方格式。

### T5：必要集成

- DataContainer复用Facade，不复制调用链；
- 不必要则不改GUI和Reporter；
- 保持所有v0.8–v0.10公开入口兼容。

### T6：headless入口决策

- 只读调查官方扩展点；
- 安全可行才实现smoke；
- 不可行则记录阻塞并停止该子项。

### T7：测试与门禁

```bash
scripts/ai_guard.sh static
scripts/ai_guard.sh test
git diff --check
```

- 两插件构建成功；
- 至少15项固定测试通过；
- 公共头无Qt/AFSIM依赖；
- 无生成物进入Git diff。

### T8：文档和交接

同步：

- `README.md`、`CHANGELOG.md`和版本文件；
- `docs/ARCHITECTURE.md`、`docs/IMPLEMENTATION_STATUS.md`、`docs/VALIDATION.md`；
- 新功能使用文档；
- 合同覆盖和需求追踪；
- `CURRENT_MILESTONE`、`PROJECT_MEMORY`和`SESSION_HANDOFF`。

## 13. 完成标准

全部满足才可报告v0.11第一阶段完成：

- Facade统一编排三类现有业务服务；
- Registry完成稳定、版本化的进程内注册和查询；
- 外部Adapter边界已定义，但没有伪造甲方协议；
- 无效上下文在业务服务调用前被拒绝；
- 结果携带请求、软件和证据版本；
- 现13项测试无回归，新增至少2项纯C++测试；
- WSF与Warlock构建成功；
- static/test/diff门禁通过；
- AFSIM核心零修改；
- 合同3.3和3.5最多标记`IMPLEMENTED / PRE_ACCEPTANCE`；
- 记录headless入口是已验证还是受阻，不伪造证据。

## 14. 失败处理

- 编译失败：先修正契约、依赖或构建集成，不改功能口径；
- 旧测试失败：优先恢复公开接口兼容性，不放宽断言；
- 甲方格式缺失：停在Adapter抽象边界，记录阻塞；
- 安全headless入口缺失：保留纯C++闭环，不修改AFSIM核心；
- 连续两次修复仍遇到同一阻塞：停止扩展，更新`SESSION_HANDOFF`并汇报。

## 15. 交付报告格式

开发AI最终必须报告：

1. 本轮唯一目标；
2. 修改文件和新增公共契约；
3. Facade与Registry的实际行为；
4. 执行的固定测试与通过数；
5. WSF/Warlock构建结果；
6. headless入口的调查结论和证据；
7. 合同3.3、3.5的状态变化；
8. 未执行的验证和外部阻塞；
9. 下一步唯一建议。

未经用户明确授权，不得提交Git、创建tag或推送远程。
