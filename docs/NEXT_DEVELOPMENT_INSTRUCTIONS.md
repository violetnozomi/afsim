# Network Resource Manager v0.8 第一阶段开发指令

## 唯一目标

基于v0.7稳定提交`073c9b7`开发`v0.8.0-capability-service`，实现合同3.2.3要求的
`CommunicationCapabilityService`统一查询和结果输出闭环。结果只能标记内部
`PRE_ACCEPTANCE`。

## 输入与输出

输入当前`ResourceSnapshot`和`CapabilityRequest`，统一输出：

- 通信距离及最大单跳距离；
- 传输速率；
- 丢包率；
- 传输时延；
- 网络交付吞吐量；
- 成员接入率；
- 每项有效性、来源、置信度和固定原因码。

## 实施要求

1. 新增`CommunicationCapabilityTypes.hpp`和`CommunicationCapabilityService.hpp`，公共代码
   不依赖Qt/AFSIM。
2. 复用`ResourceSnapshot`、`NetworkProfileRepository`、`AssessmentEvaluator`和
   `ConstrainedPathSelector`，不复制构图、路径搜索或路径指标逻辑。
3. 距离为各跳三维距离之和并输出最大单跳；速率为瓶颈可准入容量；丢包率为
   `100% - path PDR`；时延逐跳累计；吞吐量只使用观测交付吞吐；接入率分母为零时无效。
4. 当前链路只使用观测指标。候选链路只使用版本化剖面并标记
   `PARAMETERIZED_MODEL/LOW`。
5. 新增`EnvironmentEffectAdapter`抽象。地形、气象、天象和电磁干扰没有甲方数据时固定
   返回`valid=false / ENVIRONMENT_DATA_UNAVAILABLE`，不猜测格式或公式。
6. `NrmDataContainer`提供查询入口；Warlock增加只读“通信能力”页；Reporter写入独立
   `capability_results.jsonl`。
7. 新增`CommunicationCapabilityServiceTest.cpp`和固定`capability_service_smoke`场景。

## 完成门

- `scripts/ai_guard.sh static`通过；
- `scripts/ai_guard.sh test`的9项测试全部通过；
- WSF和Warlock插件构建成功；
- `scripts/ai_guard.sh scenario capability_service_smoke`退出码为0；
- 输入快照不被修改；
- 文档、schema、版本和合同追踪同步。

## 禁止事项

本阶段不开发规划文件、导航解析、真实传播模型、自动资源调度、参数扫描或v0.8第二阶段
环境适配。甲方环境数据格式缺失时不得猜测实现。
