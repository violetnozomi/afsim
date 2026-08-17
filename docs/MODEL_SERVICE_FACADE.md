# 模型服务门面契约

## 范围

v0.11提供纯C++、进程内的强类型调用边界，统一委托现有通信能力、规划校验、规划推演、
本地分发包生成和资源需求匹配服务。门面不定义网络端口、传输协议、外部字段或二进制布局，
也不改变任何网络状态。

当前契约仅用于内部`PRE_ACCEPTANCE`。甲方模型封装规范、正式接口和参考模块到位后，外部
适配器只能在该边界之外完成格式转换。

## 版本和调用上下文

- 请求schema固定为`nrm.model_service.request.v1`；
- 响应schema固定为`nrm.model_service.response.v1`；
- `requestId`必须非空，响应原样返回`requestId`和`correlationId`；
- `valid=false`或非有限`requestTime`的上下文在调用领域服务前拒绝；
- 输入含快照时，`context.snapshotVersion`必须与快照精确一致；
- 分发包调用使用规划推演的`snapshotVersion`校验证据版本；
- 响应`softwareVersion`由当前NRM版本产生，不信任调用方输入覆盖。

## 状态语义

`ModelServiceStatus`表示门面调用是否完成，不替代领域结果状态。上下文、schema或证据错误
分别返回固定的`INVALID_REQUEST`、`UNSUPPORTED_SCHEMA`或`EVIDENCE_MISMATCH`，且不调用
下游服务。依赖不存在返回`SERVICE_UNAVAILABLE`，不可预期异常返回`INTERNAL_ERROR`。

领域服务正常返回时，门面状态为`SUCCESS`并无损保留对应强类型结果。无路径、规划未通过、
需求不满足和分发包拒绝仍由原有`CapabilityReason`、`PlanValidationReason`和
`ResourceDemandReason`表达，不转换成自由文本或伪造成功业务结果。

## 不可变和委托规则

- 每个公开操作只调用一次对应领域服务；
- 输入快照、规划、需求、候选和profile均通过`const`引用传递；
- 门面不执行文件加载、规划编辑、需求编辑或网络控制；
- 只有现有`NetworkPlanDistributionService`在调用方明确请求时写本地不可变分发包；
- `ModelRegistry`只管理值类型描述符，不扫描动态库、不拥有AFSIM对象或Qt指针。

## Assessment与环境语义

`EvaluateAssessment`接收当前有效`EnvironmentContext`，并复用既有通信能力端口处理环境影响：

- `INFORMATION_ONLY`只保留环境证据，不修改任务评估；
- `ALREADY_INCLUDED`表示AFSIM链路指标已包含影响，不进行二次衰减；
- `CANDIDATE_ADJUSTMENT`才使用既有能力服务给出的环境修正结果，保守更新带宽、时延、PDR
  裕量及硬阻断原因；
- 环境硬阻断时固定产生`ENVIRONMENT_HARD_BLOCKED`，不得继续报告可建链或可完成。

Warlock手工评估和甲方`assessment_request`都经过`DataContainer::EvaluateAssessment()`进入
这一入口。Facade不复制环境公式、不重写路径搜索，也不改变评估器选出的路由。

## 外部适配边界

`ContractInterfaceAdapter`只接收抽象的外部请求/响应载体，并在边界内转换为
操作对应的显式请求和响应结构体。载体没有预设JSON、XML、二进制或网络语义；取得甲方
私有对象头文件后在此实现字段转换。

转换后的公共值对象统一进入`CustomerNrmAdapter`。该同进程入口负责资源、导航和环境状态
生命周期，并把任务评估、规划推演和并发需求匹配交给`DataContainer`持有的唯一Facade。
`CustomerJsonCodec`只提供文件、测试、回放和验收编解码，不属于运行时服务链。
