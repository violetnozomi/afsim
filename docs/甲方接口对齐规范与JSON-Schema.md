# 网络资源管理器甲方接口对齐规范与 JSON Schema

> 部署形态：本项目是安装进甲方修改版 AFSIM 的 WSF 扩展与 Warlock 插件，不是独立服务端。
> 甲方模块优先在同一进程传递公共 C++ 值对象；需要文件交换时，按本项目定义的精简
> UTF-8 JSON v1 对齐。

机器校验 Schema 位于 `schemas/customer/v1/`，中文可注释示例位于
`schemas/customer/v1/customer-interface-v1.annotated.jsonc`。当前支持导航、环境、四网资源、
任务评估、网络规划、规划结果、成员入退网和统一错误。执行
`./scripts/ai_guard.sh contract` 可验证全部示例。内部解析已达 `PRE_ACCEPTANCE`；甲方 AFSIM
目标树重编译、真实模块数据和安全策略仍需现场联调。

> Schema 内已使用标准 `title`/`description` 添加中文注解；另提供 [`JSONC 中文注释版`](../schemas/customer/v1/nrm-customer-interface-v1.annotated.jsonc) 供人工评审。正式传输和程序校验仍使用无注释 JSON，字段中文速查见 [`schemas/customer/v1/README.md`](../schemas/customer/v1/README.md)。

_接口基线草案 V1 · 2026-08-11 · 适用于 AFSIM 2.9 网络资源管理器_

## 1. 文档状态与结论

本规范用于与甲方 Link-11、Link-16、卫通、CDL、导航和环境模块对齐数据接口。当前状态为
**项目方接口基线草案**，可以直接用于字段评审、样包制作和联调准备，但只有甲方书面确认后
才能标记为正式接口。

统一 JSON Schema 位于：

- [`nrm-customer-interface-v1.schema.json`](../schemas/customer/v1/nrm-customer-interface-v1.schema.json)
- [`examples/`](../schemas/customer/v1/examples/)

核心设计决定：

1. 外部模块只与 `CustomerContractAdapter` 交互，不直接访问 AFSIM 或 NRM 内部对象。
2. V1 输入使用原子全量快照，暂不接收增量补丁，避免丢包后产生不可恢复的混合状态。
3. 所有算法使用 `simTime`；`generatedAt` 只用于运维追踪，不能参与仿真时延计算。
4. 链路是有方向的；双向链路必须上报两条记录。
5. 缺失或无效物理量不得用 0 代替，必须使用 `valid=false` 和原因码。
6. 多网平台不自动成为网关；跨网只能使用显式 `gatewayCapabilities`。
7. 评估接口只输出结论和建议，不执行建链、改频、切路由或资源调度。

## 2. 对接架构

```mermaid
flowchart LR
    customer[甲方四网/导航/环境模块]
    transport[传输适配层\nJSONL或长度前缀TCP]
    validator[JSON Schema校验]
    adapter[CustomerContractAdapter]
    snapshot[统一只读Snapshot]
    core[采集/指标/评估核心]
    output[ACK、评估响应、错误响应]

    customer --> transport --> validator --> adapter --> snapshot --> core
    core --> output --> transport --> customer
```

Schema 只约束 JSON 载荷，不把业务逻辑绑定到 TCP、文件、共享内存或消息中间件。当前源码
已有抽象 `ContractInterfaceAdapter`，正式联调时新增 `CustomerJsonContractAdapter`，完成
JSON 对象与 `ResourceSnapshot`、`NavigationSnapshot`、`EnvironmentContext`、
`CapabilityRequest/Result` 的双向转换；不得把外部 JSON 类型传入评估核心。

## 3. 消息清单

| Schema版本 | 方向 | 用途 | 当前实现关系 |
| --- | --- | --- | --- |
| `nrm.customer.provider_hello.v1` | 甲方 → NRM | 声明提供方、版本、网络类型和上报能力 | 待外部Adapter |
| `nrm.customer.resource_report.v1` | 甲方 → NRM | 四网、成员、链路、路由、流量和消息全量快照 | 映射`ResourceSnapshot` |
| `nrm.customer.navigation_report.v1` | 甲方 → NRM | AFSIM导航状态或标准化导航记录 | 映射`NavigationSnapshot` |
| `nrm.customer.environment_report.v1` | 甲方 → NRM | 地形、气象、天象和干扰观测/影响 | 映射`EnvironmentContext/Effect` |
| `nrm.customer.assessment_request.v1` | 甲方 → NRM | 提交通信任务约束 | 映射`AssessmentTask/CapabilityRequest` |
| `nrm.customer.assessment_response.v1` | NRM → 甲方 | 返回可达性、差距、主备路由和建议 | 来自评估与能力服务 |
| `nrm.customer.network_plan.v1` | 甲方 → NRM | 提交精简网链资源规划 | 映射`NetworkPlanDocument` |
| `nrm.customer.network_plan_result.v1` | NRM → 甲方 | 返回规划状态、原因码和调整建议 | 来自规划校验与只读推演 |
| `nrm.customer.ingest_ack.v1` | NRM → 甲方 | 确认输入被接受、去重或忽略 | 待外部Adapter |
| `nrm.customer.error.v1` | NRM → 甲方 | 返回固定错误原因和字段路径 | 待外部Adapter |

## 4. 公共报文规则

### 4.1 编码与封装

- 编码：UTF-8，无 BOM。
- 数字：必须是有限 JSON number，禁止 `NaN`、`Infinity` 和数字字符串。
- 字节序只与传输帧长度有关，JSON 内不涉及字节序。
- 标识符允许字母、数字、`_ . : @ / -`，长度 1–128。
- 所有未知扩展只能放入 `extensions`；顶层未知字段会被严格拒绝。
- 单条消息建议不超过 16 MiB；更大规模由双方确认分片策略后发布 V2。

### 4.2 公共标识

| 字段 | 规则 |
| --- | --- |
| `schemaVersion` | 精确匹配消息Schema，例如`nrm.customer.resource_report.v1` |
| `messageId` | 提供方生成；在同一`providerId/runId`内唯一，用于幂等去重 |
| `providerId` | 数据提供模块的稳定标识，不能使用进程号等临时值 |
| `runId` | 一次仿真运行唯一；改变后清空上一运行的动态状态 |
| `sequence` | 每个`providerId/runId/Schema`独立单调递增，允许从0或1开始 |
| `snapshotVersion` | NRM或提供方的快照版本；评估结果必须回显使用的版本 |
| `simTime` | 仿真时间，单位秒；所有时延、过期和窗口计算的唯一时间基准 |
| `generatedAt` | ISO 8601 UTC墙钟时间，只用于日志和问题定位 |

重复 `messageId` 返回 `DUPLICATE`，不重复应用。序号小于已接受序号时返回
`IGNORED_STALE/SEQUENCE_STALE`。同一输入包要么完整替换成功，要么保持旧快照不变。

### 4.3 指标对象

所有可缺失数值统一使用：

```json
{
  "value": 18.4,
  "unit": "ms",
  "valid": true,
  "source": "CUSTOMER_MODULE",
  "confidence": "MEDIUM",
  "sampleTime": 62.5,
  "window": 10.0,
  "reasonCode": "NONE"
}
```

规则：

- `valid=true` 时必须提供 `value`。
- `valid=false` 时可以不提供 `value`，但必须提供可诊断的 `reasonCode`。
- `source` 只允许 `AFSIM_INTERNAL`、`CUSTOMER_MODULE`、`REPLAY`、
  `PARAMETERIZED_MODEL`、`ESTIMATED`、`DERIVED`。
- `confidence` 只允许 `LOW/MEDIUM/HIGH`。
- 统计指标必须提供 `window`；瞬时指标可省略或设为0。
- 当前默认过期门限为两个上报周期；过期数据不能支持 `stable=true`。

### 4.4 固定单位

| 量 | 单位字符串 |
| --- | --- |
| 时间/时延 | `s`、`ms` |
| 距离/高度/误差 | `m` |
| 速率/带宽/流量 | `bit/s` |
| 频率 | `Hz` |
| 功率 | `dBm` |
| SNR、Eb/No、增量损耗 | `dB` |
| BER | `ratio` |
| PDR、占用率、在线率 | `percent` |
| 经纬度/角度 | `deg` |
| 风速 | `m/s` |
| 降雨率 | `mm/h` |

Schema 校验字段结构，Adapter 还必须校验单位是否与字段匹配。

## 5. 四网资源上报

`nrm.customer.resource_report.v1` 是甲方通信模块的主输入。V1要求：

- `fullSnapshot`固定为`true`。
- `networks/endpoints/links/gatewayCapabilities`必须存在，允许空数组。
- 一个物理平台安装多台终端时，每台终端使用独立`endpointId`。
- `networkType`只允许`LINK11/LINK16/SATCOM/CDL/UNKNOWN`；评估不会使用`UNKNOWN`边。
- `protocolDetails`为强类型四选一：Link-11提供控制站/轮询/角色，Link-16提供网络号/NPG/
  时隙，卫通提供中继/转发方式/上下行频段，CDL提供视距/双工/信道/定向天线。
- 链路由`sourceEndpointId → destinationEndpointId`表示，不能假设反向存在。
- 路由必须标记`GLOBAL_TRUTH`或`LOCAL_ROUTER`，不得把全局图冒充本地路由表。
- 跨网转发必须声明网关平台、入口/出口网络、业务类型、处理时延、容量、损失和转换策略。

主要对象映射：

| JSON对象 | NRM对象 | 说明 |
| --- | --- | --- |
| `networks[]` | `NetworkSnapshot` | 网络身份、模型、成员和聚合指标 |
| `endpoints[]` | `EndpointSnapshot` | 平台通信端点、地址、状态和位置 |
| `links[]` | `LinkSnapshot` | 有向链路、建链状态和链路指标 |
| `routes[]` | 路由证据 | 当前全局/本地路由；不能自动创建链路 |
| `businessFlows[]` | `BusinessFlow`适配对象 | 业务类型、事务、流量、时延和终态 |
| `gatewayCapabilities[]` | `GatewayCapability`适配对象 | 唯一允许的跨网转换边 |

JSON Schema无法表达的语义校验必须由Adapter执行：

1. 所有ID数组内部唯一。
2. 端点引用的`networkId`存在，网络类型一致。
3. 链路源/目的端点存在，链路网络与两端端点网络一致。
4. 网络成员清单与端点归属一致。
5. 路由每一跳能在当前链路或显式候选边中找到。
6. 网关入口和出口网络不同，且平台确实拥有对应端点。
7. `received/discarded/routingFailed`等统计不能出现负数或无解释倒退。

完整示例见[`resource-report.example.json`](../schemas/customer/v1/examples/resource-report.example.json)。

## 6. 导航数据上报

导航支持两条路径：

1. 甲方直接提供AFSIM `WsfNavigationErrors::WriteTimeHistory`生成的`.neh`文件，使用现有
   `nrm_navigation_packet_tool`严格解析；该文本文件不属于JSON Schema。
2. 实时传输时使用`nrm.customer.navigation_report.v1`，把同一字段转换为JSON记录。

`packetFormat`允许：

- `AFSIM_NAVIGATION_ERROR_HISTORY_NEH`：来源是`.neh`重放；
- `AFSIM_NAVIGATION_ERRORS_REALTIME`：直接读取`WsfNavigationErrors`；
- `NRM_NORMALIZED_JSON`：已按本Schema归一化的外部数据。

状态必须满足：`PERFECT=0`、`GPS1=1`、`GPS2=2`、`GPS3=3`、`INSn=-n`。
`.neh`只包含真实位置和误差；实时接口才能直接读取感知位置。导航上报不改变平台真实轨迹。

示例见[`navigation-report.example.json`](../schemas/customer/v1/examples/navigation-report.example.json)，
详细AFSIM格式见[`AFSIM内置导航数据接入说明`](AFSIM内置导航数据接入说明.md)。

## 7. 环境数据上报

`nrm.customer.environment_report.v1`同时允许原始观测和标准化影响：

- `observations`：地形开关、系统/场景时间、儒略日、风、雨、云和沙尘。
- `effects`：地形、气象、天象、电磁干扰对指定链路的硬阻断或数值影响。

每个影响必须声明`applicationMode`：

| 模式 | 处理规则 |
| --- | --- |
| `ALREADY_INCLUDED_IN_RESOURCE_METRICS` | 通信模块已把影响计入RSSI/SNR/BER等结果，NRM只记录证据 |
| `CANDIDATE_ADJUSTMENT` | 仅用于尚未建立的候选链路估算 |
| `INFORMATION_ONLY` | 只显示和上报，不参与能力计算 |

该字段用于防止环境衰减被重复施加。甲方不能确认时必须使用`INFORMATION_ONLY`。

示例见[`environment-report.example.json`](../schemas/customer/v1/examples/environment-report.example.json)。

## 8. 任务评估请求与响应

请求按一个源、一个或多个目的组织。Adapter对多目的请求逐目的执行同一快照评估，响应
`results[]`按`destinationPlatformIds`输入顺序返回。

请求必须给出：业务类型、负载、最低带宽、最大时延、最低PDR、允许网络、最大跳数和候选
路径数。`recommendationOnly=true`固定为真，任何控制动作不属于V1。

响应返回：

- `reachable`：当前已启用图存在路径；
- `canEstablish`：当前或参数化候选边满足建链硬约束；
- `canComplete`：带宽、时延、成功率和完成条件全部满足；
- `stable`：无硬失败、数据未过期且稳定性达到门限；
- 主路由、备选路由、网络序列和是否使用候选边；
- 距离、速率、丢包、时延和统一正负裕量；
- 固定原因码和只读建议。

所有裕量采用“当前能力减要求”或“允许上限减预测值”；正数有余量，负数不满足。

示例：[`assessment-request.example.json`](../schemas/customer/v1/examples/assessment-request.example.json)和
[`assessment-response.example.json`](../schemas/customer/v1/examples/assessment-response.example.json)。

## 9. 网链资源规划结果

`nrm.customer.network_plan_result.v1`返回规划校验、只读推演和可选分发包结果。V1固定包含
`planId`、`revision`、`validationPassed`、`evaluationStatus`、`state`、`reasonCodes`和
`recommendations`。当规划未通过时，`recommendations`汇总逐需求中文调整建议；通过时
允许为空数组。建议只用于展示、存档和甲方模块读取，不执行建链、改频、调时隙或切路由。

内部详细结果的`demands[].recommendations`保留逐需求对应关系；精简甲方响应只汇总建议，
保持接口简单。示例见[`network-plan-result.example.json`](../schemas/customer/v1/examples/network-plan-result.example.json)。

## 10. ACK、错误与重试

资源、导航和环境上报成功后返回`nrm.customer.ingest_ack.v1`：

- `ACCEPTED`：Schema和语义校验通过，完整快照已原子发布；
- `DUPLICATE`：相同`messageId`已处理，不重复应用；
- `IGNORED_STALE`：序号或仿真时间落后，旧快照保持不变。

解析或处理失败返回`nrm.customer.error.v1`。固定原因包括无效JSON、不支持Schema、Schema
校验失败、序号过期、runId不一致、引用不存在、数据无效/过期、评估忙和内部错误。
`fieldPath`采用JSON Pointer。只有`retryable=true`时提供方才应原包重试；Schema错误必须
修正后重新生成新的`messageId`。

示例：[`ingest-ack.example.json`](../schemas/customer/v1/examples/ingest-ack.example.json)和
[`error-response.example.json`](../schemas/customer/v1/examples/error-response.example.json)。

## 11. 推荐传输协议

### 11.1 第一阶段文件联调

- 一个`.json`文件保存一个完整消息，或JSONL每行一个完整消息。
- 文件写入采用“临时文件写完后原子改名”，避免读取半包。
- `.neh`导航文件保持AFSIM原生格式，不嵌入JSON字符串。
- 文件联调通过后再进入实时传输，不改变消息载荷。

### 11.2 第二阶段实时联调建议

- 甲方作为客户端，NRM Adapter作为服务端；监听地址和端口由部署配置提供，禁止写死。
- 每帧为4字节网络字节序无符号长度，加一段UTF-8 JSON；最大长度默认16 MiB。
- 建连后第一条业务消息必须是`provider_hello`。
- 默认资源上报周期500 ms（2 Hz）；建议允许范围100 ms到60 s。
- 每个输入帧必须收到ACK或ERROR；超时和重试次数由部署参数配置。
- TLS、双向认证、证书、白名单和审计由甲方安全环境确定，当前Schema不承载凭据。

实时传输尚未在当前源码中实现，因此以上属于**推荐联调协议**，不能写入“已完成”验收项。

## 12. 版本兼容规则

- `*.v1`内容一旦双方签字冻结，不删除字段、不改变语义、不扩大已有枚举含义。
- 新的可选业务需求也发布新Schema文件和新`schemaVersion`，不静默覆盖当前文件。
- NRM通过`provider_hello.supportedSchemas`协商共同版本；没有交集返回
  `SCHEMA_UNSUPPORTED`。
- Adapter必须允许多个Schema版本并存，内部统一转换为同一强类型对象。
- 每次交付保存Schema文件SHA-256、示例包、校验日志和双方签字版本号。

## 13. 本地校验

执行：

```bash
cd /home/pyh/afsim/network_resource_manager
./scripts/validate_customer_interface.sh
```

脚本使用Draft 2020-12校验Schema和全部9个正例，并确认缺字段的负例会被拒绝。
若目标机没有`/usr/bin/jsonschema`，可通过`NRM_JSONSCHEMA_COMMAND`指定兼容工具。

## 14. 需要甲方书面确认的项目

| 编号 | 待确认内容 | 本方案默认值 |
| ---: | --- | --- |
| 1 | 实时传输方式 | 长度前缀TCP；先文件联调 |
| 2 | 客户端/服务端角色 | 甲方客户端，NRM Adapter服务端 |
| 3 | 上报模式 | V1只接受全量快照 |
| 4 | 上报周期 | 500 ms |
| 5 | 最大端点/链路/单包大小 | 10000/100000/16 MiB |
| 6 | 平台、成员、终端和网络编号规则 | 提供方在runId内稳定唯一 |
| 7 | Link-16 NPG/时隙、Link-11轮询等专用字段是否够用 | V1已提供基础强类型字段；新增语义发布V2，不塞入extensions逃避评审 |
| 8 | 路由视角 | 默认`GLOBAL_TRUTH`，本地路由显式标注 |
| 9 | 导航交付 | 文件用AFSIM`.neh`，实时用导航JSON |
| 10 | 环境影响是否已计入通信结果 | 每条effect必须声明`applicationMode` |
| 11 | 业务类型枚举及门限 | 当前字符串；甲方提供正式字典后冻结 |
| 12 | TLS、认证、端口和部署网段 | 由甲方安全规范确定 |
| 13 | ACK超时、重试、断线重连 | 甲方确认部署参数 |
| 14 | 是否要求增量事件流 | V1不支持；有需要发布V2 |

## 15. 联调通过标准

1. 甲方`provider_hello`与双方冻结Schema版本一致。
2. 甲方提供四网、导航、环境各至少一个合法样包和三个非法样包。
3. 合法包通过JSON Schema和语义引用校验，非法包返回稳定错误码和字段路径。
4. 同一全量快照在甲方模块和NRM页面中的网络、端点、链路数量100%一致。
5. 25节点场景连续运行30分钟，无乱序回退、重复应用或跨run污染。
6. 指定任务的源/目的、主备路由、带宽、时延、PDR和原因码与双方期望文件一致。
7. 导航`.neh`/实时JSON状态映射一致；环境影响不会重复施加。
8. 断线、重复、乱序、超大包和Schema不兼容场景均有可复现记录。

完成以上项目后，才能将接口状态从`BASELINE_DRAFT`升级为`INTEGRATION_ACCEPTED`；在甲方
目标环境完成最终运行前仍不能标记为`FINAL_ACCEPTANCE`。
