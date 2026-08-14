# 当前里程碑

## 1. 当前状态

状态：`IMPLEMENTED / PRE_ACCEPTANCE`。

目标版本：`v0.12.0-contract-gap-closure`。

开发分支：`feat/contract-gap-closure`。

AFSIM核心修改数：`0`。实现范围仅限独立`network_resource_manager`扩展、Warlock插件、
场景、脚本和文档。

## 2. 本里程碑目标

以“合同有对应项、界面可见、结果可解释、命令可复验”为目标，用最小实现补齐当前合同追踪
表中能在本地完成的指标，不等待甲方给出字段；甲方AFSIM改造模块后续通过现有Adapter和
11类JSON Schema对齐本插件，不改变领域服务和界面。

## 3. 已完成内容

- 统一资源状态覆盖网络、成员、链路、频率、协议资源、队列、流量、路由、业务、网关、代理、
  告警、导航和环境；缺失字段显式无效并给出原因码。
- 11类甲方接口Schema、正反例、中文注释JSONC、提供方握手、接收ACK和错误响应已固定。
- 四网频率与协议资源、地形/气象/时间/航向/电磁干扰近似约束进入能力评估。
- 规划域、动态成员、分发包指纹ACK、并发需求评估、调整建议、需求反馈历史和推荐闭环已实现。
- 导航支持AFSIM原生导航误差包解析、GNSS/INS/组合导航精度证据和恢复数据原子保留。
- Reporter和Warlock中文页面均可查看新增证据；部署自检脚本可区分本地完成项与甲方阻塞项。

## 4. 当前固定验证基线

- `scripts/ai_guard.sh static`：通过。
- `scripts/ai_guard.sh contract`：11个合法样例通过，5个非法样例按预期拒绝。
- `scripts/ai_guard.sh test`：31/31固定C++测试通过，WSF和Warlock插件构建通过。
- `scripts/ai_guard.sh scenario operational_strike_demo`：25节点同体系协同场景通过并正常结束。
- `scripts/check_deployment_contract.sh`：部署JSON有效，4项甲方依赖正确标为`CUSTOMER_BLOCKED`。
- `scripts/run_preacceptance.sh`：31/31测试、9/9场景和Warlock 120秒最终快照全部通过，
  总体结果为`PASS`。

以上均为内部预验收证据，不代表甲方目标环境最终验收。

## 5. 仍需甲方提供或现场确认

- 甲方改造版AFSIM的正式模块ABI、加载方式和目标环境构建结果。
- Link-11、Link-16、卫通、CDL真实字段映射、设备参数和正式样包。
- 正式安全认证、传输协议、端口、证书和网络部署策略。
- 正式导航/环境数据样包以及合同参数阈值签字确认。

上述内容不阻塞本地演示：缺失输入按照L0-L3降级契约处理，保持低置信度或`valid=false`。

## 6. 下一步唯一动作

冻结`v0.12.0`回退点；随后只进行甲方环境适配和字段映射，不再扩张本地模型范围。
