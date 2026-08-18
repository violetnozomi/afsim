# 甲方接口逐字段中文注释模板

本目录包含 13 类接口的一报文一文件 JSONC 模板，结构与实际业务 JSON 一致。字段后的 `//`
中文注释说明固定值、允许值、单位或主要约束。

使用规则：

1. 联调评审和人工填写时阅读本目录的 `*.example.jsonc`。
2. 实际传给插件前必须删除所有 `//` 注释；JSON 标准本身不支持注释。
3. 需要可直接加载的报文时，使用相邻 [`../examples/`](../examples/) 目录中的同名
   `*.example.json`。
4. 正式字段约束以 [`../`](../) 目录中的 `*.schema.json` 为唯一依据；本目录是可读模板。
5. `$schema`、`$id`、`title`、`description` 是 Schema 文档元数据，甲方业务报文不用填写。

## 接口模板清单

| 方向 | 模板 | 用途 |
| --- | --- | --- |
| 甲方→NRM | [`provider-hello.example.jsonc`](provider-hello.example.jsonc) | 甲方模块能力声明 |
| 甲方→NRM | [`resource-report.example.jsonc`](resource-report.example.jsonc) | 四网资源全量快照 |
| 甲方→NRM | [`navigation-report.example.jsonc`](navigation-report.example.jsonc) | 导航状态上报 |
| 甲方→NRM | [`environment-report.example.jsonc`](environment-report.example.jsonc) | 地形、气象、天象和干扰上报 |
| 甲方→NRM | [`assessment-request.example.jsonc`](assessment-request.example.jsonc) | 单任务评估请求 |
| NRM→甲方 | [`assessment-response.example.jsonc`](assessment-response.example.jsonc) | 单任务评估结果 |
| 甲方→NRM | [`resource-demand-request.example.jsonc`](resource-demand-request.example.jsonc) | 并发通信需求请求 |
| NRM→甲方 | [`resource-demand-response.example.jsonc`](resource-demand-response.example.jsonc) | 并发需求匹配结果 |
| 甲方→NRM | [`network-plan.example.jsonc`](network-plan.example.jsonc) | 网链资源规划输入 |
| NRM→甲方 | [`network-plan-result.example.jsonc`](network-plan-result.example.jsonc) | 规划校验和推演结果 |
| 甲方→NRM | [`membership-request.example.jsonc`](membership-request.example.jsonc) | 成员入网/退网申请 |
| NRM→导入工具 | [`ingest-ack.example.jsonc`](ingest-ack.example.jsonc) | 文件输入处理确认 |
| NRM→导入工具 | [`error.example.jsonc`](error.example.jsonc) | 统一导入错误 |

执行以下命令可校验全部正式示例和注释模板：

```bash
PYTHON_BIN=/usr/bin/python3 ./scripts/validate_customer_interface.sh
```
