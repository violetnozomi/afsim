# 甲方独立可读 JSON Schema 设计

## 目标

把 13 个甲方业务接口 Schema 改造成可独立阅读、可独立校验的正式文件。接口 Schema 内不再出现
`$ref`，字段旁直接给出类型、长度、范围、格式、枚举和简短中文 `description`，甲方不需要跳转到
`common.schema.json` 或文件内 `$defs` 才能理解字段。

## 采用方案

采用“正式 Schema 完全展开”方案：

```json
"messageId": {
  "type": "string",
  "minLength": 1,
  "maxLength": 64,
  "pattern": "^[A-Za-z0-9_.:@/-]+$",
  "description": "消息唯一编号；1～64字符，只允许字母、数字及_.:@/-"
}
```

不另外生成第二套“甲方版 Schema”，避免两个正式入口和两套约束发生漂移。

## 变更范围

需要展开的 13 个正式接口文件：

1. `provider-hello.schema.json`
2. `resource-report.schema.json`
3. `navigation-report.schema.json`
4. `environment-report.schema.json`
5. `assessment-request.schema.json`
6. `assessment-response.schema.json`
7. `resource-demand-request.schema.json`
8. `resource-demand-response.schema.json`
9. `network-plan.schema.json`
10. `network-plan-result.schema.json`
11. `membership-request.schema.json`
12. `ingest-ack.schema.json`
13. `error.schema.json`

`common.schema.json` 保留为公共规则说明和旧工具兼容文件，但上述 13 个正式接口不再引用它。

## 展开规则

- 外部引用如 `common.schema.json#/$defs/identifier` 直接展开为完整字符串约束。
- 文件内部引用如 `#/$defs/link` 直接展开到实际字段位置。
- 展开后移除不再使用的 `$defs`，每个 Schema 只有一条从信封到业务字段的阅读路径。
- 原 `$schema`、`$id`、`title`、`type`、`required` 和 `additionalProperties` 语义保持不变。
- 每个业务属性增加简短中文 `description`；描述必须包含单位、枚举或主要引用规则中的至少一项。
- 所有字段名称、必填关系、类型、长度、范围、枚举和数组约束保持与当前 V1 一致。
- 不修改标准 JSON 示例、逐字段 JSONC 示例、C++ Codec、Adapter 或算法模型。

## 可读性边界

`$schema` 和 `$id` 仍然保留，因为它们是正式 JSON Schema 的标准元数据；甲方业务消息不填写它们。
接口文档继续把 `examples-commented/*.example.jsonc` 作为“如何填报文”的首选入口，把展开后的
`*.schema.json` 作为“字段是否合法”的正式约束入口。

## 自动验证

扩展现有契约校验，增加以下检查：

1. 13 个正式接口 Schema 递归扫描后不得出现 `$ref`。
2. 每个 `properties` 下的业务字段必须存在非空中文 `description`。
3. 13 个正式 JSON 示例和 13 个 JSONC 示例仍须通过对应 Schema。
4. 6 个无效样例仍须被拒绝。
5. 现有 C++ Codec 和 Adapter 测试继续通过，证明接口语义没有改变。

测试先增加失败断言，再逐个展开 Schema；出现正例拒绝或负例被接受时立即停止并修正该文件，
不通过放宽约束规避失败。

## 完成标准

- 13 个正式业务 Schema 中 `$ref` 数量为 0。
- 甲方查看任意字段时无需跳转文件即可看到完整约束和中文说明。
- 契约校验、静态检查及固定 C++ 回归测试全部通过。
- Git diff 不包含 C++、AFSIM 框架或接口业务字段改名。

