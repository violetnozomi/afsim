# 当前里程碑

## 1. 当前状态

状态：`IMPLEMENTED / PRE_ACCEPTANCE`。

目标版本：`v0.11.0-model-service-facade`。

开发基线提交：`d64ba74`（v0.10.0）。

当前开发分支：`feat/v0.11-model-service-facade`；v0.11基线提交为`492949a`，
2026-08-08的Warlock统计修复尚未提交。

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
- 原13项测试无回归，新增两项后15/15通过，WSF与Warlock构建成功；
- AFSIM核心零修改，公共头无Qt/AFSIM依赖；
- headless强类型入口受阻，未伪造服务smoke。
- Warlock `MessageReceived`发送端/接收端参数语义已修正；四网120秒最终快照为
  `8发送/8接收/0丢弃`，15/15固定测试无回归。

## 4. 验收边界

- 合同3.3、3.5和4.1-4.2仅为内部`IMPLEMENTED / PRE_ACCEPTANCE`；
- 甲方封装规范、正式接口协议、参考模块和目标环境未提供，阻止`FINAL_ACCEPTANCE`；
- 现有headless mission无法取得Warlock Facade和强类型快照；
- 未实现导航、环境模型、动态库扫描、微服务或自动网络控制；
- Git提交、tag和push仍需用户明确授权。

## 5. 下一步唯一动作

用户在VNC确认四网总览显示`Received=8、Discarded=0`，随后明确决定是否提交当前统计修复；
不自动进入导航、环境模型、自动网络控制或参数实验。
