# 当前里程碑

## 1. 当前状态

状态：`IMPLEMENTED / PRE_ACCEPTANCE`。

目标版本：`v0.11.0-model-service-facade`。

开发基线提交：`d64ba74`（v0.10.0）。

当前开发分支：`feat/v0.11-model-service-facade`；Warlock统计修复提交为`3c1f2b2`，
一键预验收脚本、只读状态页和文档尚未提交。

## 2. 已完成的唯一目标

在不引入网络服务框架、不猜测甲方协议的前提下，建立纯 C++、进程内、强类型的
`ModelServiceFacade`、`ModelRegistry` 和外部接口适配抽象边界，统一暴露现有通信能力、
规划生命周期和资源需求匹配能力。

完整执行要求见 `docs/NEXT_DEVELOPMENT_INSTRUCTIONS.md`；本轮未扩展到其他里程碑。

## 3. 完成证据

- 新增`ModelServiceTypes`、`ModelServiceFacade`、`ModelRegistry`和抽象
  `ContractInterfaceAdapter`；
- Facade前置校验schema、requestId、requestTime、snapshotVersion和规划证据，每项操作
  只委托一次既有领域服务；
- DataContainer持有Facade和Registry，注册唯一NRM描述符并保持旧页面入口兼容；
- 原15项测试无回归，新增环境配置、环境效果和导航包解析测试后18/18通过；WSF与Warlock构建成功；
- AFSIM核心零修改，公共头无Qt/AFSIM依赖；
- headless强类型入口受阻，未伪造服务smoke。
- Warlock `MessageReceived`发送端/接收端参数语义已修正；四网120秒最终快照为
  `8发送/8接收/0丢弃`，15/15固定测试无回归。
- `scripts/run_preacceptance.sh`已将静态门禁、18项测试、八个批准场景和Warlock最终快照
  固定为一条命令，并生成逐项Markdown报告；2026-08-08首次完整运行总体为`PASS`。
- Warlock新增只读“预验收状态”页；脚本原子发布版本化JSON状态，独立工作线程自动读取，
  不在仿真回调或GUI线程中执行文件I/O，也不提供从界面重启验收的按钮。
- 综合场景已调整为25个同一体系的协作通信平台，保留4网络、29端点、60链路和主备多跳
  路由；敌方、阵营对抗、目标航迹、武器和交战事件已删除，完整180秒命令行验证通过。
- 自研Warlock前端已统一中文化；协议名、RF缩写和原因码保留原始技术标识，AFSIM核心和
  Warlock原生菜单未修改；插件已重新编译链接成功。
- 自研Warlock前端已升级为现代深色卡片式视觉；中央态势图按平台去重、显示多网徽点并执行
  标签避碰。默认10平台和综合25平台远程VNC截图均已复核，18/18测试无回归。
- 环境影响第一版已实现：AFSIM内置地形/气象/历元/EM结果进入版本化快照与中文页面，
  `NRM_ENVIRONMENT_V1`严格配置仅修正候选链路，当前RF结果不重复应用；环境里程碑当时17/17测试通过，
  `environment_weather`固定场景完成8条四网消息收发。

## 4. 验收边界

- 合同3.3、3.5和4.1-4.2仅为内部`IMPLEMENTED / PRE_ACCEPTANCE`；
- 甲方封装规范、正式接口协议、参考模块和目标环境未提供，阻止`FINAL_ACCEPTANCE`；
- 现有headless mission无法取得Warlock Facade和强类型快照；
- 已实现AFSIM内置导航实时采集与`.neh`解析；未实现甲方专用二进制导航包、复杂区域气象/频谱网格、微服务或自动网络控制；
- Git提交、tag和push仍需用户明确授权。

## 5. 下一步唯一动作

由用户在VNC核对新增“环境状态”和“导航状态”页；随后决定是否运行更新后的18项测试、
八场景一键预验收并提交当前工作区。不自动进入甲方专用格式或自动网络控制。
