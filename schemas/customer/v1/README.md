# 甲方接口 Schema 中文阅读说明

## 为什么没有使用 `//` 注释

标准 JSON 不支持 `//` 或 `/* ... */` 注释。为了保证 Schema 和示例能被甲方程序、CI、IDE 以及 `jsonschema` 工具直接解析，本目录采用 JSON Schema Draft 2020-12 的标准注解关键字：

- `title`：对象或字段的中文名称；
- `description`：字段含义、单位、有效性和使用限制；
- `const`、`enum`：固定值和允许值，同时作为机器可校验约束。

因此不要向 `.json` 文件中加入行注释。需要补充说明时，优先扩展 Schema 的 `description` 或本文档。

需要逐字段阅读时可打开 `nrm-customer-interface-v1.annotated.jsonc`。VS Code 会按 JSON with
Comments 识别 `.jsonc`；该文件的 `messages` 数组是评审样例集合，不应把最外层对象整体发送。
实际联调时从数组中取出对应的单条消息，并去掉注释后发送。

## 文件说明

| 文件 | 用途 |
| --- | --- |
| `nrm-customer-interface-v1.schema.json` | 甲方接口统一 Schema，包含全部八类消息和中文注解 |
| `nrm-customer-interface-v1.annotated.jsonc` | 可直接阅读的 `//` 中文注释版，集中包含八类可校验消息 |
| `examples/provider-hello.json` | 数据提供方能力协商示例 |
| `examples/resource-report.json` | 四网资源全量快照示例 |
| `examples/navigation-report.json` | AFSIM 导航数据解析结果示例 |
| `examples/environment-report.json` | 地形、气象、天象和电磁干扰示例 |
| `examples/assessment-request.json` | 通信任务评估请求示例 |
| `examples/assessment-response.json` | 可达性、差距、主备路径和建议示例 |
| `examples/ingest-ack.json` | 数据接收确认示例 |
| `examples/error-response.json` | 结构化错误响应示例 |

## 八类消息方向

| `schemaVersion` | 方向 | 中文说明 |
| --- | --- | --- |
| `nrm.customer.provider_hello.v1` | 甲方模块 → NRM | 声明模块版本、支持的 Schema 和网络类型 |
| `nrm.customer.resource_report.v1` | 甲方模块 → NRM | 上报 Link-11、Link-16、卫通和 CDL 的完整资源快照 |
| `nrm.customer.navigation_report.v1` | 甲方模块/适配器 → NRM | 上报 AFSIM 导航包解析后的平台状态和误差 |
| `nrm.customer.environment_report.v1` | 甲方模块/适配器 → NRM | 上报自然环境和电磁干扰观测及链路影响 |
| `nrm.customer.assessment_request.v1` | 调用方 → NRM | 请求进行通信任务可达性与资源差距评估 |
| `nrm.customer.assessment_response.v1` | NRM → 调用方 | 返回主备路径、数值裕量、原因码和建议 |
| `nrm.customer.ingest_ack.v1` | NRM → 数据提供方 | 确认快照接收、重复或过期 |
| `nrm.customer.error.v1` | 双向 | 返回可定位、可判断重试性的结构化错误 |

## 公共字段

| 字段 | 中文含义 | 约束 |
| --- | --- | --- |
| `schemaVersion` | 消息结构版本和消息类型判别值 | 必须使用 Schema 中列出的固定字符串 |
| `messageId` | 单条消息唯一标识 | 建议由提供方生成 UUID 或稳定流水号 |
| `providerId` | 数据提供方或响应方标识 | 部署前双方书面冻结 |
| `runId` | 一次 AFSIM 仿真运行标识 | 仿真重新开始后必须更换 |
| `sequence` | 提供方在同一运行内的单调递增序号 | 用于判重和拒绝乱序旧快照 |
| `snapshotVersion` | NRM 采用的内部不可变快照版本 | 评估请求与响应必须保留对应关系 |
| `simTime` | AFSIM 仿真时间 | 单位秒，从 0 开始；不等同于操作系统时间 |
| `generatedAt` | 生成消息的墙钟时间 | ISO 8601 UTC，例如 `2026-08-11T02:00:00Z` |
| `extensions` | 双方约定的非核心扩展字段 | 核心算法不得依赖未冻结的扩展字段 |

## 度量值 `metric`

所有可能缺失、估计或按窗口计算的数值统一使用 `metric`：

```json
{
  "value": 82.4,
  "unit": "percent",
  "valid": true,
  "source": "DERIVED",
  "confidence": "MEDIUM",
  "sampleTime": 125.5,
  "window": 10.0,
  "reasonCode": "OK"
}
```

- `valid=false` 时不得把 `0` 当作真实测量值；`value` 可以省略。
- `source=ESTIMATED` 或 `PARAMETERIZED_MODEL` 表示并非甲方真实设备测量。
- `sampleTime` 与 `window` 均使用仿真秒。
- `unit` 应使用 Schema 和接口文档约定的字符串，不在同一字段中混用单位。

## 关键枚举中文含义

### 数据来源 `dataOrigin`

| 值 | 含义 |
| --- | --- |
| `AFSIM_INTERNAL` | AFSIM 内部对象或观察者回调直接给出 |
| `CUSTOMER_MODULE` | 甲方 Link-11、Link-16、卫通、CDL、导航或环境模块直接给出 |
| `REPLAY` | 从已归档数据回放得到 |
| `PARAMETERIZED_MODEL` | 本项目参数化近似模型产生 |
| `ESTIMATED` | 根据配置或先验估计 |
| `DERIVED` | 由多个输入字段或事件计算得到 |

### 资源状态 `resourceState`

| 值 | 含义 |
| --- | --- |
| `UNKNOWN` | 当前无法判断，不等同于离线 |
| `OFFLINE` | 不在网或不可参与通信 |
| `ONLINE` | 当前在线并可按能力参与通信 |
| `DISABLED` | 已配置但被显式禁用 |
| `FAILED` | 设备或链路发生故障 |

### 环境应用方式 `applicationMode`

| 值 | 含义 |
| --- | --- |
| `ALREADY_INCLUDED_IN_RESOURCE_METRICS` | 环境影响已经包含在资源指标中，评估器不得重复扣减 |
| `CANDIDATE_ADJUSTMENT` | 影响尚未包含，评估候选链路时应用修正 |
| `INFORMATION_ONLY` | 只显示和记录，不改变评估数值 |

## 四网协议扩展

- Link-11：`netControlStationEndpointId`、轮询周期、应答超时和成员角色。
- Link-16：网络号、NPG、终端模式和时隙分配/使用数量。
- 卫通：中继平台、透明/再生/抽象转发方式和上下行频段。
- CDL：视距要求、双工方式、信道标识和定向天线标志。

`networkType` 与 `protocolDetails.type` 必须一致。比如 `networkType=LINK11` 时不能携带 Link-16 的时隙结构。

## 本地校验

在项目根目录运行：

```bash
./scripts/ai_guard.sh contract
```

该命令会校验 Schema 自身、八个正例，以及缺少必填字段的反例。正式与甲方联调前还应增加甲方提供的真实脱敏报文作为固定回归样例。
