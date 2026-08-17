# 当前里程碑

## 1. 当前状态

状态：`IMPLEMENTED / PRE_ACCEPTANCE`。

目标版本：`v0.12.0-contract-gap-closure`。

开发分支：`feat/code-quality-raii-hardening`（隔离工作树，基线`a1a5ff8`可随时回退）。

AFSIM核心修改数：`0`。实现范围仅限独立`network_resource_manager`扩展、Warlock插件、
场景、脚本和文档。

## 2. 本里程碑目标

以“合同有对应项、界面可见、结果可解释、命令可复验”为目标，用最小实现补齐当前合同追踪
表中能在本地完成的指标，不等待甲方给出字段；甲方AFSIM改造模块后续通过现有Adapter和
13类JSON Schema对齐本插件，不改变领域服务和界面。

## 3. 已完成内容

- 统一资源状态覆盖网络、成员、链路、频率、协议资源、队列、流量、路由、业务、网关、代理、
  告警、导航和环境；缺失字段显式无效并给出原因码。
- 13类甲方接口Schema、正反例、中文注释JSONC、提供方声明、文件接收ACK和错误记录已固定。
- 四网频率与协议资源、地形/气象/时间/航向/电磁干扰近似约束进入能力评估。
- 规划域、动态成员、分发包指纹ACK、并发需求评估、调整建议、需求反馈历史和推荐闭环已实现。
- 导航支持AFSIM原生导航误差包解析、GNSS/INS/组合导航精度证据和恢复数据原子保留。
- Reporter和Warlock中文页面均可查看新增证据；部署自检脚本可区分本地完成项与甲方阻塞项。
- 严格JSON运行时校验、规划仓库接纳一致性、Reporter生命周期、Qt/临时路径RAII、远程切换
  失败回滚和合同硬件阈值检查已完成代码加固。
- 甲方JSON状态层已按具体网络ID隔离，资源/导航/环境按域合并；消息去重、乱序、新运行隔离、
  规划配置版本绑定、环境完整字段和统一任务评估入口已完成。
- 任务评估已进入`ModelServiceFacade`；甲方并发资源需求JSON已进入需求仓库、匹配服务和
  结构化响应主链；`CustomerNrmAdapter`提供同进程强类型入口，`CustomerJsonCodec`只承担
  文件、测试、回放和验收用途。
- 甲方接口验收闭环已补齐：端点/链路使用明确`networkId`，导航使用明确`platformId`，
  环境三态不会被候选链路逻辑覆盖；`CustomerJsonValidationLayer`提供统一运行时校验入口，
  `DataContainer::LoadCustomerJson`端到端测试覆盖三域合并、评估响应、配置绑定和派生失效。
- 资源对象关系已抽取为纯C++`ResourceSnapshotValidator`，JSON解码与同进程Adapter共用；
  AFSIM基础态和甲方导航/环境覆盖层由`EffectiveSnapshotAssembler`确定性合成。
- 独立Assessment通过唯一Facade复用Capability环境链，明确区分只读信息、已包含影响和候选
  修正三态；接口校验脚本支持`PYTHON_BIN`并提供依赖诊断。

## 4. 当前固定验证基线

- `scripts/ai_guard.sh static`：通过。
- `scripts/ai_guard.sh contract`：13个合法样例通过，6个非法样例按预期拒绝。
- `scripts/ai_guard.sh test`：37/37固定C++测试通过，WSF和Warlock插件构建通过；完整
  NRM `ctest`为40/40（其中另含3项Shell回归）。
- `nrm_remote_plan_switch_test`与`nrm_deployment_contract_test`：2/2 Shell回归通过。
- 严格编译告警检查：本插件在`-Wall -Wextra -Wpedantic -Wconversion
  -Wsign-conversion -Wshadow`下零告警；仅保留AFSIM上游告警。
- `scripts/ai_guard.sh scenario operational_strike_demo`：25节点同体系协同场景通过并正常结束。
- `scripts/check_deployment_contract.sh`：本机硬件下限通过，4项甲方依赖正确标为
  `CUSTOMER_BLOCKED`。
- `scripts/run_preacceptance.sh`：31/31测试、9/9场景和Warlock 120秒最终快照全部通过，
  总体结果为`PASS`。

以上均为内部预验收证据，不代表甲方目标环境最终验收。

## 5. 仍需甲方提供或现场确认

- 甲方改造版AFSIM的正式模块ABI、加载方式和目标环境构建结果。
- Link-11、Link-16、卫通、CDL真实字段映射、设备参数和正式样包。
- 甲方对象头文件、字段语义、插件初始化/调用线程和双方ABI构建基线。
- 正式导航/环境数据样包以及合同参数阈值签字确认。

上述内容不阻塞本地演示：缺失输入按照L0-L3降级契约处理，保持低置信度或`valid=false`。

## 6. 下一步唯一动作

完成本轮全量门禁后冻结当前隔离分支；随后只进行甲方目标环境薄适配和字段映射。
