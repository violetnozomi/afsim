# 当前里程碑

## 1. 当前状态

状态：`PENDING`。

目标版本：`v0.10.0-demand-matching`。

稳定起点：v0.9提交`d6f9518`。

建议分支：`feat/v0.10-demand-matching`。

## 2. 当前唯一目标

实现合同3.2.5第一阶段的资源需求文件生命周期、批量需求匹配、逐项不满足分析，以及基于
显式有限候选集合的频率、站点、信道、子网、时隙和路由六类可解释建议。

完整执行要求见`docs/NEXT_DEVELOPMENT_INSTRUCTIONS.md`。

## 3. 必须复用

- `ResourceSnapshot`；
- `CapabilityRequest / CapabilityResult`；
- `CommunicationCapabilityService`；
- `NetworkPlanDocument`、规划推演结果和内容指纹；
- `NetworkProfileRepository`；
- `SnapshotReporter`；
- DataContainer与Warlock现有模式。

禁止复制能力公式、路径搜索和profile规则。

## 4. 完成门

- 原11项固定测试无回归；
- 新增Demand Repository和Matching测试，总数至少13项；
- 每项需求输出`SATISFIED / UNSATISFIED / DATA_INVALID`；
- 每个约束输出要求值、当前值、裕量、单位和固定原因码；
- 六类建议均输出`AVAILABLE`或明确`UNAVAILABLE`原因；
- 规划内容指纹和snapshotVersion不一致时拒绝拼接证据；
- 相同输入输出顺序稳定；
- 评估不修改快照、规划、需求和AFSIM运行网络；
- WSF与Warlock构建成功；
- static/test/diff审计通过；
- AFSIM核心零修改；
- 完成后最多标记内部`PRE_ACCEPTANCE`；
- Git提交前获得用户明确批准。

## 5. 明确禁止

- 自动资源分配或自动下发；
- 自研导航或猜测甲方接口；
- 虚构频率、站点、信道、子网或时隙候选；
- 优化器、强化学习、参数扫描和研究实验；
- 为让匹配通过而调整演示profile；
- 数据库、微服务或新第三方依赖。

## 6. 外部阻塞

- 甲方需求和规划正式格式；
- 频率保护、信道、子网和TDMA专用规则；
- 正式候选资源集合；
- 模型封装与分发接口；
- 目标环境联调资料。

这些资料缺失时，内部服务必须返回来源、置信度和不可用原因，不能猜测补齐。

## 7. 下一步唯一动作

开发AI从`d6f9518`创建`feat/v0.10-demand-matching`，先运行并记录11项基线测试，然后只执行
`docs/NEXT_DEVELOPMENT_INSTRUCTIONS.md`中的T1公共契约与严格需求文法工作。
