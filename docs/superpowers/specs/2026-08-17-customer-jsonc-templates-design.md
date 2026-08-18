# 甲方逐字段注释 JSONC 模板设计

## 目标

为现有 13 类甲方接口各提供一份可直接阅读和照填的业务报文模板。模板采用真实消息信封
`schema/messageId/timestamp/source/data`，每个示例字段后附简短中文注释，说明固定值、枚举、
单位、范围或填写规则。

## 边界

- 保留 `*.schema.json` 作为唯一正式机器约束，不改字段语义。
- 保留 `examples/*.example.json` 作为程序可直接读取的标准 JSON。
- 新增 `examples-commented/*.example.jsonc` 作为人工对接模板；JSONC 不得直接发送给插件。
- 不修改 C++ Adapter、Codec、评估模型或 AFSIM 框架。
- 不引入 HTTP、TCP、WebSocket、MQ 或新运行时依赖。

## 文件组织

`examples-commented/` 与 `examples/` 一一对应，共 13 个同名接口：

- `assessment-request` / `assessment-response`
- `resource-demand-request` / `resource-demand-response`
- `network-plan` / `network-plan-result`
- `resource-report` / `navigation-report` / `environment-report`
- `membership-request` / `provider-hello` / `ingest-ack` / `error`

目录中的 `README.md` 说明使用方式、去注释要求和正式约束来源。原有聚合注释文件继续保留，
用于总览和兼容已有文档链接。

## 注释规则

- 行尾 `//` 注释只写必要规则，不复述字段名。
- 固定值写“固定值：...” 。
- 枚举写出全部允许值；网络类型统一为 `LINK11/LINK16/SATCOM/CDL`。
- 数值注明单位及主要范围，例如 `>= 0，单位 bit/s`、`0~100，单位 %`。
- 标识符注明“1~64 字符，同一作用域唯一”或引用对象。
- 可选字段明确写“可选”；数组说明元素含义，示例元素内部字段逐项注释。
- 响应报文标明由 NRM 生成，甲方不填写。

## 自动校验

离线脚本增加字符串感知的 JSONC 行注释清理器，避免把字符串中的 `//` 当成注释。每份 JSONC
去注释后必须：

1. 能被标准 JSON 解析器读取；
2. 通过同名 Draft 2020-12 Schema；
3. 恰好覆盖全部 13 类接口。

Shell 回归测试检查校验脚本输出的 13 份注释模板通过标记，防止模板缺失或脱离 Schema。

