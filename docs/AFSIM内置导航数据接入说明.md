# AFSIM 内置导航数据接入说明

_适用于 AFSIM 2.9 Linux 与网络资源管理器当前工作区版本 · 2026-08-11_

## 1. 已冻结的接入边界

导航第一版不自研 GNSS、INS 或组合导航解算算法，而是接受 AFSIM
`WsfNavigationErrors` 组件产生的数据并完成解析、标准化、展示与记录。支持两种输入路径：

1. **仿真实时路径**：Warlock 插件直接读取每个平台的 `WsfNavigationErrors` 状态；
2. **文件交换路径**：接受 `WsfNavigationErrors::WriteTimeHistory` 输出的原生 `.neh`
   时序文件，使用命令行工具转换为标准 JSONL。

这里的“导航数据包格式”具体冻结为 AFSIM 2.9 自带的 `.neh` 文本时序格式，格式标识为
`AFSIM_NAVIGATION_ERROR_HISTORY_NEH`。它不是网络二进制报文，也不是完整导航解算结果协议。
如果甲方以后提供另一种二进制包或消息总线接口，应新增 Adapter，不修改当前公共导航对象。

## 2. AFSIM 原生字段

原生记录由空白分隔：

```text
#--time-- stat -----lat----- -----lon------ ----alt--- --hdg-- -it-error-- -xt-error-- --v-error-- ----rss----
10.0000 GPS1 35:30:00.000N 118:15:00.000E 1001.000 91.000 8.000 2.000 3.000 8.775
```

| 字段 | 含义 | 标准化输出 |
| --- | --- | --- |
| `time` | 仿真时间，秒 | `sampleTime` |
| `stat` | 原生导航状态 | `rawStatus`、`mode`、`statusCode` |
| `lat/lon/alt` | 平台真实位置 | `truthLatitudeDeg/LongitudeDeg/AltitudeM` |
| `hdg` | 航向角 | `headingDeg` |
| `it-error` | 航迹纵向误差 | `inTrackErrorM` |
| `xt-error` | 航迹横向误差 | `crossTrackErrorM` |
| `v-error` | 垂直误差 | `verticalErrorM` |
| `rss` | 三维位置误差模 | `totalPositionErrorM` |

状态映射如下：

| AFSIM 状态 | 数值 | 页面含义 |
| --- | ---: | --- |
| `PERFECT` | 0 | 理想导航 |
| `GPS1` | 1 | 卫星导航 |
| `GPS2` | 2 | 卫导降级 |
| `GPS3` | 3 | 外部导航输入 |
| `INS1`、`INS2` 等 | -1、-2 等 | 惯性导航 |

解析器接受 AFSIM 的度分秒坐标，也接受合法十进制度；拒绝未知状态、非有限数、越界经纬度、
负时间、时间倒退和尾随脏字段。任一有效记录失败时整包失败，不输出半包结果。

## 3. 命令行解析与输出

先构建工具：

```bash
AFSIM_BUILD=/home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build-ubuntu24
cmake --build "$AFSIM_BUILD" --target nrm_navigation_packet_tool -j4
```

解析仓库样包：

```bash
"$AFSIM_BUILD/nrm_navigation_packet_tool" \
  nav_aircraft data/navigation_sample.neh \
  > /tmp/navigation.jsonl
jq -c . /tmp/navigation.jsonl
```

工具参数顺序固定为“平台名称、输入 `.neh` 文件”。成功时每条输入记录输出一行
`nrm.navigation_sample.v1` JSON；失败时标准错误以 `Error:` 开头并返回非零退出码。

## 4. 固定场景和可视化

无界面自动验证：

```bash
./scripts/ai_guard.sh scenario navigation_errors
```

该场景依次切换 `INS1 → PERFECT → GPS1 → GPS2`，并由 AFSIM 自身生成：

```text
/tmp/nrm-navigation-history/nav_aircraft.neh
```

可直接验证真实生成文件：

```bash
AFSIM_BUILD=/home/pyh/afsim/afsim2.9/afsim-2.9.0-kylin_v10_sp1_x86_64/swdev/src/build-ubuntu24
"$AFSIM_BUILD/nrm_navigation_packet_tool" nav_aircraft \
  /tmp/nrm-navigation-history/nav_aircraft.neh | tail -1 | jq .
```

在 VNC 桌面中启动 Warlock 导航场景：

```bash
cd /home/pyh/afsim/network_resource_manager
./scripts/remote/run-navigation-warlock.sh
```

打开右侧网络资源管理器的 **导航状态** 页，可查看平台、中文导航模式、原生状态、真实位置、
感知位置、纵向/横向/垂直误差、总位置误差和更新时间。运行快照同时在
`resource_snapshots.jsonl` 的 `navigation` 对象中记录相同内容。

## 5. 能力边界

- AFSIM `navigation_errors` 是感知位置误差模型，不是完整接收机、星座、惯导器件或融合滤波器。
- 该误差不改变平台真实运动轨迹，主要影响平台感知位置及其上报结果。
- `.neh` 原生文件提供真实位置与误差，没有直接提供感知位置；感知位置仅在实时接口中读取。
- 当前模块只解析和展示，不控制导航状态，也不承担甲方装备精度参数的真实性责任。
- 将来替换甲方数据源时，保留 `NavigationSnapshot/NavigationSample` 契约并新增输入 Adapter。
