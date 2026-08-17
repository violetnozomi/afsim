# 甲方 AFSIM 插件精简 JSON 接口 V1

运行时文件入口统一经过`CustomerJsonValidationLayer`，再由各领域Codec执行引用和跨对象语义
校验。正式Schema仍是字段合同的唯一来源；`scripts/validate_customer_interface.sh`用于完整
Draft约束回归。由于AFSIM 2.9扩展不新增重量级依赖，默认C++校验层实现关键约束子集并保留
可替换接口，不宣称内置完整Draft 2020-12引擎。

资源对象中的`networkId`是逻辑网络实例主键；相同`networkType`的两个网络不共享成员或链路。
导航对象中的`platformId`是upsert主键。环境`applicationMode`三种枚举在运行时完整保存，
仅`CANDIDATE_ADJUSTMENT`允许附加参数化候选影响。

甲方系统同样基于AFSIM改造。本项目以WSF扩展和Warlock插件形式接入；已有AFSIM数据优先
直接监听，不要求先转成JSON。只有甲方定制模块补充数据或离线回放时才使用本目录格式。

## 对接优先级

1. AFSIM公开对象和回调；
2. 同进程公共C++值对象；
3. UTF-8 JSON文件；
4. 导航历史兼容AFSIM原生`.neh`。

## 公共信封

所有JSON固定包含`schema`、`messageId`、`timestamp`、`source`和`data`。时间戳使用带时区
ISO 8601；仿真计算使用`data.simTime`。未知可选数据直接省略，不能用0伪装未知值。

## 文件清单

| Schema | 作用 | 合同 |
| --- | --- | --- |
| `navigation-report` | GNSS/INS/组合导航结果 | 3.2.1 |
| `environment-report` | 地形、气象、天象、干扰 | 3.2.3 |
| `resource-report` | 四网资源完整快照 | 3.2.5 |
| `assessment-request/response` | 单任务评估 | 3.2.3、3.2.5 |
| `resource-demand-request/response` | 多任务并发资源匹配和只读建议 | 3.2.5 |
| `network-plan/result` | 规划校验、推演与结果 | 3.2.4 |
| `membership-request` | 动态入网和退网申请 | 3.2.4 |
| `error` | 统一错误及字段路径 | 3.3 |

`resource-demand-request` 是甲方需要对齐的精简资源管理输入。甲方只需提供通信节点标识、
源/目的、业务类型及带宽/时延/PDR约束；插件负责绑定当前模型配置、并发扣减资源并返回
每项结果。响应中的建议只读，不会自动建链、改频或修改路由。

正式JSON示例位于`examples/`；中文评审示例是
`nrm-customer-interface-v1.annotated.jsonc`。执行：

```bash
./scripts/validate_customer_interface.sh
```

该命令通过表示Schema、正例和负例一致，不代表已经完成甲方环境最终联调。
