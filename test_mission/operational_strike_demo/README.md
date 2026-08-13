# 综合通信协同演示场景

该场景用于验证AFSIM内部网络资源采集、四网可视化、业务流量、主备路由、链路故障与恢复。
所有文件均位于本插件目录，不修改或运行时覆盖AFSIM原始场景。场景中的25个平台全部属于
同一个协作体系，不包含敌方平台、敌我阵营、目标跟踪、武器、交战或攻击任务。

场景包含：

- 区域指挥中心、网络控制中心、空中中继、任务飞机、巡查无人机、数据处理站和卫星中继；
- Link-11成员协同、Link-16任务通信、卫通远程指挥和CDL高速数据四种演示网络；
- 巡查数据采集、摘要上报、任务计划分发、成员状态、链路故障/恢复和高速视频流；
- 25个物理平台、29个通信端点和60条有向链路，并保留可演示的主备多跳路径。

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
NRM_OPERATIONAL PHASE LINK11_RESOURCE_STATUS_REPORTED
NRM_OPERATIONAL PHASE MISSION_STATUS_REPORTED
NRM_OPERATIONAL PHASE SCENARIO_COMPLETE
Simulation complete
```

平台创建证据写入忽略目录中的`output/operational_strike_demo.evt`。验证要求恰好出现25条
`PLATFORM_ADDED`，且不得出现任何`WEAPON_*`事件。

只启动场景、不执行证据断言时可使用：

```bash
./scripts/ai_guard.sh scenario operational_strike_demo
```

文件职责：

- `main.txt`：Warlock和人工演示入口，不包含依赖工作目录的文件输出；
- `validation.txt`：命令行验证覆盖层，追加事件文件并由验证脚本检查。
