# 综合通信协同演示场景

该场景用于验证AFSIM内部网络资源采集、四网可视化、业务流量、主备路由、跨域网关级联、
链路故障与恢复。所有文件均位于本插件目录，不修改或运行时覆盖AFSIM原始场景。场景中的
37个平台全部属于
同一个协作体系，不包含敌方平台、敌我阵营、目标跟踪、武器、交战或攻击任务。

场景包含：

- 区域指挥中心、网络控制中心、空中中继、任务飞机、巡查无人机、数据处理站和卫星中继；
- Link-11成员协同、Link-16任务通信、卫通远程指挥和CDL高速数据四种演示网络；
- 巡查数据采集、摘要上报、任务计划分发、成员状态、链路故障/恢复和高速视频流；
- Warlock交互入口额外产生四网非等周期业务，并对报文长度作固定种子抖动，确保1秒/10秒
  窗口中的吞吐量、PDR和链路流向在录像期间持续变化；
- 交互入口通过AFSIM真实通信状态短时中断和四条独立质量序列形成可重复的动态PDR曲线：
  健康、轻度退化、强干扰、恢复、稳定五个阶段连续变化，不在前端伪造指标；
- 25个基础平台和12个双归属网关，共37个物理平台、53个通信端点和108条有向链路；
- Link-11、Link-16、SATCOM、CDL两两组合形成六组网关，每组两个方向专用物理节点；
- 12条单网关双向显式路由，以及2条经过两个物理网关的显式级联路由；
- 每个网关只接入指定的两个域，不具备四网全互联能力，也不隐式拼接未授权路由。

所有网络和任务参数均为内部参数化演示值，只能形成`PRE_ACCEPTANCE`证据。

运行：

```bash
./scripts/validate_operational_strike_demo.sh
```

成功运行至少应出现：

```text
NRM_OPERATIONAL PHASE SCENARIO_START
NRM_OPERATIONAL PHASE LINK11_STATUS_REPORTED
NRM_OPERATIONAL PHASE LINK16_DIRECT_FAILURE
NRM_OPERATIONAL PHASE LINK16_DIRECT_RECOVERED
NRM_OPERATIONAL PHASE GATEWAY_PAIR_L11_L16_SENT
NRM_OPERATIONAL PHASE GATEWAY_PAIR_SATCOM_CDL_SENT
NRM_OPERATIONAL PHASE GATEWAY_CASCADE_L11_CDL_SENT
NRM_OPERATIONAL PHASE GATEWAY_CASCADE_CDL_L11_SENT
NRM_OPERATIONAL PHASE LINK11_RESOURCE_STATUS_REPORTED
NRM_OPERATIONAL PHASE MISSION_STATUS_REPORTED
NRM_OPERATIONAL PHASE SCENARIO_COMPLETE
Simulation complete
```

平台创建证据写入忽略目录中的`output/operational_strike_demo.evt`。验证要求恰好出现37条
`PLATFORM_ADDED`和10条`NRM_GATEWAY FORWARDED`（6条直达、两条级联各2跳），且不得
出现任何`WEAPON_*`事件。

只启动场景、不执行证据断言时可使用：

```bash
./scripts/ai_guard.sh scenario operational_strike_demo
```

单独验证四网PDR差异和两次运行可重复性：

```bash
bash tests/PdrDiversityScenarioTest.sh
```

通过时会按五个阶段输出20行`NRM_PDR_PHASE_RESULT`。这些值是AFSIM
`MESSAGE_TRANSMITTED/MESSAGE_RECEIVED`事件的实际计数。四网使用不同发送周期、负载抖动、
固定种子非周期丢包和短时接收端中断，阶段只控制总体质量趋势，不再规定5%整数倍结果。
因此输出可复现但不会整齐相同；它仍只作为内部参数化演示数据，不代表真实装备指标或甲方
实测数据。

单独验证录像中默认查询链路的Warlock实时指标：

```bash
./scripts/validate_live_link_metrics.sh
```

该脚本读取最新运行目录最后一份完整快照，定位
`airborne_relay -> spectrum_monitor_aircraft`，并断言10秒窗口的发送数、接收数、发送负载、
接收吞吐、PDR、传输时延和占用率均有效，同时要求链路带宽来自AFSIM内部物理层配置。脚本
不是只检查仿真退出码；任一字段无效、没有业务样本或链路不存在都会返回非零状态并打印该
链路的原始JSON证据。也可显式指定快照和平台名：

```bash
./scripts/validate_live_link_metrics.sh \
  output/run-<run_id>/resource_snapshots.jsonl \
  airborne_relay spectrum_monitor_aircraft
```

文件职责：

- `main.txt`：180秒确定性命令行场景，不包含交互专用周期流量；
- `interactive.txt`：Warlock实时1×入口，180秒后低速驻留供远程检查；
- `interactive_traffic.txt`：仅供交互入口使用的四网周期消息，不参与确定性命令行验收；
- `pdr_validation.txt`：复用交互流量的确定性PDR计数场景，用于校验四网差异和可重复性；
- `gateways.txt`：显式网关路由、方向能力、处理参数和12个物理网关节点；
- `validation.txt`：命令行验证覆盖层，追加事件文件并由验证脚本检查。
