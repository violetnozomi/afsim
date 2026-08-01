# 当前里程碑

## 1. 当前状态

状态：VERIFIED（内部PRE_ACCEPTANCE）。

目标版本：v0.8.0-capability-service。

稳定起点：v0.7稳定提交`073c9b7`。

当前分支：`feat/v0.8-capability-service`。

## 2. 当前唯一目标

实现`CommunicationCapabilityService`，以当前网络快照和通信任务需求为输入，统一输出
通信距离、传输速率、丢包率、传输时延、网络吞吐量、接入率以及有效性、来源、置信度
和固定原因码。

本阶段只建立统一能力查询和结果输出闭环。地形、气象、天象和电磁环境仅预留抽象适配
接口；甲方数据格式缺失时返回无效结果，不猜测格式、传播公式或默认环境数据。

## 3. 实施范围

- 新增`CommunicationCapabilityTypes.hpp`公共值对象和`EnvironmentEffectAdapter`接口；
- 新增无Qt/AFSIM依赖的`CommunicationCapabilityService.hpp`；
- 复用`ResourceSnapshot`、`NetworkProfileRepository`和`ConstrainedPathSelector`；
- 在`NrmDataContainer`提供能力查询入口；
- Warlock新增只读“通信能力”页；
- SnapshotReporter新增独立能力结果JSONL；
- 新增`CommunicationCapabilityServiceTest.cpp`和固定`capability_service_smoke`场景。

## 4. 固定指标口径

- 距离：所选路径各跳三维距离之和，同时输出最大单跳距离；
- 速率：路径瓶颈可准入容量；
- 丢包率：`100% - path PDR`；
- 时延：路径累计传输/建链时延；
- 吞吐量：已有窗口交付吞吐，无观测时保持无效；
- 接入率：在线目标成员数除以目标成员总数，分母为零时无效；
- 当前链路优先使用观测值；候选链路只使用版本化剖面并标记
  `PARAMETERIZED_MODEL/LOW`。

## 5. 完成门

- `scripts/ai_guard.sh static`通过；
- `scripts/ai_guard.sh test`发现并通过9项测试；
- WSF和Warlock两个插件构建成功；
- `capability_service_smoke`固定场景退出码为0；
- 无路径、离线端点、零成员、缺失环境、NaN/Inf和无吞吐观测均保持明确无效语义；
- 服务不修改输入快照；
- 结果状态仅标记内部`PRE_ACCEPTANCE`；
- AFSIM核心零修改。

## 6. 当前允许修改范围

为完成本阶段，可修改`include/nrm`、`warlock/source`、`tests`、`test_mission`、
`CMakeLists.txt`、`scripts/ai_guard.sh`、`README`、`CHANGELOG`和相关`docs/data`。

不允许修改AFSIM核心源码、合同原文基线、系统配置和OA环境。

## 7. 明确不做

- 不开发规划文件、导航解析或真实传播模型；
- 不猜测甲方环境数据格式；
- 不自动建链、改频、分配时隙或修改路由；
- 不运行参数扫描；
- 不进入v0.8第二阶段环境适配实现。

## 8. 状态更新模板

- Task：
- Status：PENDING／IN_PROGRESS／IMPLEMENTED／VERIFIED／BLOCKED
- Changed files：
- Tests：
- Evidence：
- Remaining risk：
- Next single action：

使用内部剖面或模拟输入时只能标记`PRE_ACCEPTANCE`，不能标记`FINAL_ACCEPTANCE`。

## 9. 2026-08-01 v0.8第一阶段检查

- Task：实现`CommunicationCapabilityService`统一查询与输出闭环。
- Status：VERIFIED（内部PRE_ACCEPTANCE）。
- Changed files：公共能力契约与服务、共享路径距离字段、DataContainer、Warlock只读页、
  Reporter能力JSONL、测试、固定场景、版本、合同追踪和运行文档。
- Tests：`scripts/ai_guard.sh static`通过；`scripts/ai_guard.sh test`的9项测试全部通过；
  WSF和Warlock插件构建成功；`capability_service_smoke`退出码为0。
- Evidence：`docs/VALIDATION.md`和`nrm.capability.v1`；能力服务测试覆盖路径选择、指标
  有效性、置信度传播、环境适配器和输入不可变边界。固定smoke只验证插件、当前图与消息
  交付底座。
- Remaining risk：Warlock页面尚未人工截图；甲方地形、气象、天象和电磁干扰数据格式、
  样包与目标环境未提供，环境效果保持无效。
- Next single action：等待甲方环境数据格式和样包；资料到位并经用户明确指定后，再启动
  v0.8第二阶段环境适配。
