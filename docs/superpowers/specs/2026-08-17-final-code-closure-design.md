# NRM 最后代码收尾设计

## 目标与边界

本轮只修复已确认的Schema/Runtime漂移、混合来源环境子域模式和provenance，并将隔离
worktree中已验证的变更合入正式开发分支。不改AFSIM核心，不新增传输协议、算法或架构层。

## 语义收口

1. `maximumDelayMs < 0`非法，`== 0`表示不设置最大时延上限，`> 0`应用上限。
   Assessment JSON解码在0时同步关闭`requireDelayMetricForFeasibility`；Plan和Demand继续由现有
   `maximumDelayMs > 0`判断决定是否施加约束。
2. `network_plan.allocations[].members`至少一项且唯一，Schema、Codec和既有Validator保持一致。
3. `EnvironmentContext.customerProvidedDomains`只记录Customer实际提供的子域。Customer
   `applicationMode`仅控制该集合；未提供的AFSIM子域按信息证据处理，不被Customer候选修正
   或硬阻断。空集合保留旧C++调用方的全域兼容语义。
4. 每个环境效果使用对应子域的`origin/confidence`；Customer与AFSIM证据文本分开。
5. `ResourceSnapshotValidator`只补充关键浮点有限性，不改结构；`valid`仍在全部检查后由
   `issues.empty()`唯一决定。

## 验证与Git收尾

所有行为先增加失败测试，再做最小修复。隔离分支通过静态、契约、固定C++测试、
完整CTest、插件构建和固定场景后创建一个收尾提交。正式分支
`feat/v0.11-model-service-facade`当前与隔离分支共用基线`1d9ffb0`，采用快进合并，合并后重跑
关键门禁并保留隔离worktree作为可审计回退点，不删除分支。
