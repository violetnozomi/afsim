# 资源测评逐跳路由真实显示设计

## 1. 目标

Warlock 综合通信资源态势图继续使用现有 `AssessmentEvaluator` 计算主路由和备选路由，
不改变路径搜索、约束判定、主备选择、候选链路或跨域网关授权规则。本改造只修正结果表达：
将算法实际选中的每一条有向边按顺序输出并逐跳绘制，避免只用平台名称数组绘制后丢失端点、
方向、网络、单跳候选状态和网关内部转换。

这里的“真实显示”是“忠实显示算法选中的逐跳结果”，不是新增 AFSIM 消息事件追踪，也不把
计算路径宣称为消息已经实际经过的路径。

## 2. 现状与根因

`ConstrainedPath` 已保存算法选中路径的有序 `ConstrainedEdge`，但 `AssessmentEvaluator`
目前只把它压缩成：

- `primaryRoute`：平台名称序列；
- `primaryEndpointRoute`：端点 ID 序列；
- `primaryRouteUsesCandidate`：整条路径是否包含任意候选边；
- `backupRoute`：备选平台名称序列。

`NrmTacticalView` 只消费平台名称序列并在平台坐标间画粗线。因此它无法逐跳说明端点和网络，
无法区分某一条边是否为候选边；网关入口和出口属于同一平台时还会形成零长度线段。结果虽然
来自算法，却不能忠实表达算法内部的逐边路径。

## 3. 架构边界

- `AssessmentEvaluator` 是逐跳结果的唯一生产者，直接从已选 `ConstrainedPath::edges` 投影，
  不在 GUI 中重新计算路径。
- `AssessmentResult` 保存不可变的主/备逐跳值对象；旧平台路由和端点路由字段继续保留，保证
  现有调用方兼容。
- `NrmTacticalView` 只消费逐跳结果和当前快照中的平台坐标，不访问 AFSIM 对象、不执行选路。
- `NrmDockWidget` 只格式化逐跳文本，不修改测评参数或结果。
- Reporter 可记录逐跳证据；甲方 V1 JSON 响应继续保留原字段，不在本轮改变外部 Schema。
- 不修改 AFSIM 核心，不增加第三方依赖，不新增消息追踪、自动建链或路由控制。

## 4. 公共逐跳契约

在 `AssessmentTypes.hpp` 中增加：

```cpp
enum class AssessmentRouteHopKind
{
   cCURRENT_LINK,
   cCANDIDATE_LINK,
   cGATEWAY_TRANSITION
};

struct AssessmentRouteHop
{
   std::size_t hopIndex = 0;
   AssessmentRouteHopKind kind = AssessmentRouteHopKind::cCURRENT_LINK;
   std::string sourceEndpointId;
   std::string destinationEndpointId;
   std::string sourcePlatform;
   std::string destinationPlatform;
   std::string sourceNetworkId;
   std::string destinationNetworkId;
   NetworkType sourceNetworkType = NetworkType::cUNKNOWN;
   NetworkType destinationNetworkType = NetworkType::cUNKNOWN;
   bool candidate = false;
   bool gateway = false;
   std::string gatewayRouteId;
   std::string gatewayCapabilityId;
};
```

`AssessmentResult` 增加：

```cpp
std::vector<AssessmentRouteHop> primaryRouteHops;
std::vector<AssessmentRouteHop> backupRouteHops;
```

兼容规则：

- `primaryRoute`、`primaryEndpointRoute`、`backupRoute` 和两个整路径候选标志不删除、不改语义；
- `hopIndex` 从 1 开始，严格等于向量位置加 1；
- 普通当前链路使用 `cCURRENT_LINK`；
- 参数化候选链路使用 `cCANDIDATE_LINK`；
- 同一物理网关入口端点到出口端点的转换使用 `cGATEWAY_TRANSITION`；
- `candidate` 和 `gateway` 保留为便于现有调用方判断的显式兼容字段，并必须与 `kind` 一致；
- 未知网络保持 `cUNKNOWN` 和空 `networkId`，不得猜测或填充为其他域。

## 5. 逐跳结果生成

`AssessmentEvaluator` 在主路由和备选路由确定后，对每个 `ConstrainedEdge` 按原顺序生成一个
`AssessmentRouteHop`：

1. 源/目的端点 ID、平台名称直接取自边；
2. 源/目的网络 ID 和类型通过当前评估使用的 `EndpointMap` 查找；
3. `edge.gateway == true` 时生成网关内部转换跳，并复制路由/能力 ID；
4. 非网关边根据 `edge.candidate` 区分当前链路和候选链路；
5. 主路由和备选路由使用同一投影函数，避免语义漂移；
6. 若端点映射缺失，仍保留边中已有的 ID、平台和候选/网关属性，网络字段保持未知。

逐跳投影不参与路径排序、可达性、带宽、时延、PDR、稳定性或主备选择计算。

## 6. 态势图绘制

基础拓扑继续以细线显示。测评路径覆盖层按 `primaryRouteHops` 和 `backupRouteHops` 顺序绘制：

- 主路由使用黄色较粗覆盖层，备选路由使用青色较细覆盖层；
- 每个普通跳绘制有向线段、箭头、`H1/H2/...` 编号和网络类型；
- 每个候选跳只对该跳使用虚线，并在标签中追加“候选”；
- 当前链路使用实线；
- 网关内部转换不画零长度直线，而在网关节点旁画环形转换标记，标注跳号和
  `入口网络→出口网络`；
- 主、备路由分别使用自己的连续跳号；
- 找不到源或目的平台坐标时不补画跨越中间节点的直线；文本结果仍保留该跳；
- 路径首尾继续使用源节点绿色圈和目的节点橙色圈；
- 旧的整路径 `candidate` 虚线逻辑停止用于逐跳覆盖层，但旧字段继续用于兼容文本和调用方。

界面图例明确区分“当前链路”“算法主路由”“算法备选路由”“候选跳”和“网关转换”。

## 7. 测评文本

任务评估结果保留现有结论、指标、主/备平台序列、网关路由 ID 和能力 ID，并追加：

```text
主路由逐跳：
H1 source/link11 → relay/link11 [LINK11，当前]
H2 relay/link11 → gateway/link11 [LINK11，当前]
H3 gateway/link11 → gateway/satcom [LINK11 → SATCOM，网关转换]
H4 gateway/satcom → destination/satcom [SATCOM，候选]
```

备选路由存在时使用同一格式。格式化使用纯 C++ 辅助函数，以便不启动 Qt 界面即可回归测试。

## 8. Reporter 与外部兼容

内部 `assessment_results.jsonl` 增加 `primaryRouteHops` 和 `backupRouteHops` 数组，逐项记录本设计
中的字段。Reporter 输出是内部审计证据，需补充 JSON 序列化测试。

`CustomerJsonCodec::EncodeAssessmentResponse` 本轮不增加字段，继续输出已冻结的
`primaryRoute` 和 `backupRoute`，避免无甲方确认时修改 V1 Schema。进程内 C++ 调用方可以读取
新增逐跳字段。

## 9. 测试策略

严格执行测试先行：

1. `AssessmentEvaluatorTest` 先断言当前实现缺少逐跳输出，覆盖普通两跳中继的顺序、端点、
   平台、网络和当前链路类型；
2. 增加单条候选边测试，证明只标记对应一跳，不把整条路径所有跳都标为候选；
3. 扩展双网关级联用例，证明入口/出口同平台仍生成独立网关转换跳，能力 ID 和网络方向正确；
4. 验证备选路由也输出独立、连续编号的逐跳结果；
5. `UiTextTest` 覆盖普通跳、候选跳和网关转换的中文文本；
6. `SnapshotReporterTest` 覆盖主/备逐跳 JSON 字段和合法 JSON；
7. 运行受影响测试、固定 39 项 C++ 测试、4 项 Shell/工程回归、静态门禁；
8. 构建 Warlock 插件，并在固定综合场景中人工核对双网关级联的逐跳编号与箭头。

## 10. 完成标准

- 算法选中路径中的每个 `ConstrainedEdge` 都对应一个顺序一致的 `AssessmentRouteHop`；
- 主路由和备选路由均能逐跳显示；
- 每一跳明确方向、端点、平台、网络和当前/候选/网关转换类型；
- 网关入口到出口显示为独立转换跳，不再表现为零长度线；
- 缺坐标时不伪造跨节点连线；
- 旧评估接口和甲方 V1 JSON Schema 保持兼容；
- 固定测试和插件构建通过；
- AFSIM 核心修改数为 0；
- 状态保持 `IMPLEMENTED / PRE_ACCEPTANCE`，人工 Warlock 视觉核对完成前不提升为最终验收。
