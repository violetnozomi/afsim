# Standalone Customer Schemas Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 将 13 个甲方正式接口 Schema 完全展开为无 `$ref`、逐字段带中文 `description` 的独立可读文件，同时保持 V1 业务约束不变。

**Architecture:** `common.schema.json` 继续作为兼容词典，但 13 个接口 Schema 不再引用公共或本地 `$defs`。契约校验脚本递归检查 `$ref` 和字段中文说明，再使用现有正例、负例、JSONC模板及 C++ 回归测试证明接口语义未被破坏。

**Tech Stack:** JSON Schema Draft 2020-12、Bash、Python 3 离线 `jsonschema`、C++固定回归测试。

## Global Constraints

- 13 个正式业务 Schema 中 `$ref` 数量必须为 0。
- 每个 `properties` 下的业务字段必须存在非空中文 `description`。
- 字段名称、必填关系、类型、长度、范围、枚举和数组约束保持 V1 不变。
- `common.schema.json` 保留，不修改 C++ Codec、Adapter、算法或 AFSIM 框架。
- 不引入新的运行时依赖或外部通信协议。
- 正式 JSON 与 JSONC 示例内容保持不变。

---

### Task 1: 建立独立可读 Schema 防回归门禁

**Files:**
- Modify: `tests/CustomerInterfaceValidationScriptTest.sh`
- Modify: `scripts/validate_customer_interface.sh`

**Interfaces:**
- Consumes: `schemas/customer/v1/*.schema.json`，排除 `common.schema.json`
- Produces: `PASS: 13 standalone schemas contain no refs and all fields have Chinese descriptions.`

- [x] **Step 1: 写当前必然失败的测试**

在 Shell 测试中捕获校验输出并断言存在上述精确通过标记。该标记只有递归扫描 13 个 Schema、
确认没有 `$ref` 且所有属性有中文 `description` 后才能输出。

- [x] **Step 2: 运行测试确认 RED**

Run: `bash tests/CustomerInterfaceValidationScriptTest.sh`

Expected: 退出码非 0，因为当前校验器没有通过标记，现有 Schema 也仍包含 `$ref`。

- [x] **Step 3: 增加递归门禁实现**

在现有 Python 校验段增加以下等价逻辑：

```python
def walk_schema(node, path=()):
    if isinstance(node, dict):
        if "$ref" in node:
            raise SystemExit(f"ERROR: {'/'.join(path)} contains forbidden $ref")
        properties = node.get("properties", {})
        for name, property_schema in properties.items():
            description = property_schema.get("description", "")
            if not description or not any("\u4e00" <= ch <= "\u9fff" for ch in description):
                raise SystemExit(f"ERROR: {'/'.join(path + ('properties', name))} lacks Chinese description")
        for key, value in node.items():
            walk_schema(value, path + (key,))
    elif isinstance(node, list):
        for index, value in enumerate(node):
            walk_schema(value, path + (str(index),))
```

只扫描 `CONTRACTS` 列表中的 13 个文件；`common.schema.json` 不进入无引用门禁。

- [x] **Step 4: 运行校验并确认因现有 `$ref` 正确失败**

Run: `PYTHON_BIN=/usr/bin/python3 ./scripts/validate_customer_interface.sh`

Expected: 明确报告第一个含 `$ref` 的文件和字段路径。

### Task 2: 展开基础请求、响应和状态 Schema

**Files:**
- Modify: `schemas/customer/v1/assessment-request.schema.json`
- Modify: `schemas/customer/v1/assessment-response.schema.json`
- Modify: `schemas/customer/v1/navigation-report.schema.json`
- Modify: `schemas/customer/v1/environment-report.schema.json`
- Modify: `schemas/customer/v1/provider-hello.schema.json`
- Modify: `schemas/customer/v1/ingest-ack.schema.json`
- Modify: `schemas/customer/v1/membership-request.schema.json`
- Modify: `schemas/customer/v1/error.schema.json`

**Interfaces:**
- Consumes: `common.schema.json` 中 identifier、timestamp、networkType、position、error 的现有约束
- Produces: 8 个无 `$ref` 的自包含接口 Schema

- [x] **Step 1: 展开公共字符串和枚举约束**

将 identifier 统一展开为 `type=string/minLength=1/maxLength=64/pattern=^[A-Za-z0-9_.:@/-]+$`，
timestamp 展开为 `type=string/format=date-time`，networkType 展开为四网枚举。

- [x] **Step 2: 展开位置和错误对象**

把 position 与 error 对象直接嵌入对应字段，保留原 required、范围和 additionalProperties 约束。

- [x] **Step 3: 为所有属性增加中文说明**

信封字段说明固定值和方向，业务字段说明标识引用、枚举、单位或范围；不改变示例载荷。

- [x] **Step 4: 执行阶段契约验证**

Run: `PYTHON_BIN=/usr/bin/python3 ./scripts/validate_customer_interface.sh`

Expected: 门禁继续在尚未展开的后续 Schema 处失败，不得在本任务 8 个文件内失败。

### Task 3: 展开需求匹配与规划结果 Schema

**Files:**
- Modify: `schemas/customer/v1/resource-demand-request.schema.json`
- Modify: `schemas/customer/v1/resource-demand-response.schema.json`
- Modify: `schemas/customer/v1/network-plan-result.schema.json`

**Interfaces:**
- Consumes: V1并发需求、六类建议和规划状态约束
- Produces: 3 个无 `$ref`、逐字段中文说明的自包含 Schema

- [x] **Step 1: 展开需求请求的标识符和四网枚举**

保留 `demands minItems=1`、allowedNetworks 非空且唯一以及带宽/时延/PDR/距离/规模范围。

- [x] **Step 2: 展开需求响应和规划结果**

保留 SATISFIED/UNSATISFIED/DATA_INVALID、六类建议、AVAILABLE/UNAVAILABLE、四类规划评估状态和
四类规划生命周期状态。

- [x] **Step 3: 补齐嵌套属性中文说明**

为 demand、result、recommendation 及规划结果的每个属性添加中文 `description`。

- [x] **Step 4: 执行阶段契约验证**

Run: `PYTHON_BIN=/usr/bin/python3 ./scripts/validate_customer_interface.sh`

Expected: 门禁只允许在尚未展开的 `network-plan` 或 `resource-report` 处失败。

### Task 4: 展开网络规划和资源全量快照 Schema

**Files:**
- Modify: `schemas/customer/v1/network-plan.schema.json`
- Modify: `schemas/customer/v1/resource-report.schema.json`

**Interfaces:**
- Consumes: 文件内 allocation/demand 与 network/member/link/route/flow/gateway/proxy/alarm 定义
- Produces: 2 个完全内联、无 `$defs`/`$ref` 的复杂自包含 Schema

- [x] **Step 1: 内联网络规划数组元素**

把 allocation 与 demand 定义直接放入 `allocations.items` 和 `demands.items`，保留成员、时隙、
网络类型、带宽、时延和PDR全部约束。

- [x] **Step 2: 内联资源快照数组元素**

把 operationalArea、network、member、attitude、link、route、flow、gateway、proxy、alarm、
protocolResource、coverage 和 position/geoPoint 逐层嵌入实际字段位置。

- [x] **Step 3: 删除不再使用的 `$defs` 并补齐中文说明**

递归检查每个 `properties` 子项均有中文 `description`，保持链路方向、ID引用和单位口径。

- [x] **Step 4: 执行 GREEN 契约验证**

Run: `PYTHON_BIN=/usr/bin/python3 ./scripts/validate_customer_interface.sh`

Expected: 输出 `PASS: 13 standalone schemas contain no refs and all fields have Chinese descriptions.`，
13个正式示例和13个JSONC示例通过，6个负例拒绝。

### Task 5: 更新说明、完整回归和版本检查点

**Files:**
- Modify: `schemas/customer/v1/README.md`
- Modify: `docs/甲方接口对齐规范与JSON-Schema.md`
- Modify: `docs/superpowers/plans/2026-08-18-standalone-customer-schemas.md`

**Interfaces:**
- Consumes: 13 个独立正式 Schema
- Produces: 明确的甲方阅读入口、验证记录和可回退 Git 提交

- [x] **Step 1: 更新文档入口**

明确正式 Schema 已完全展开；`common.schema.json` 只是兼容词典；甲方无需理解或解析 `$ref`。

- [x] **Step 2: 更新计划完成状态**

将已执行步骤改为 `[x]`，不改变原计划和验证命令。

- [x] **Step 3: 执行最终验证**

Run: `git diff --check`

Run: `./scripts/ai_guard.sh static`

Run: `PYTHON_BIN=/usr/bin/python3 ./scripts/ai_guard.sh contract`

Run: `bash tests/CustomerInterfaceValidationScriptTest.sh`

Run: `./scripts/ai_guard.sh test`

Expected: 所有命令退出码为0；13个Schema无引用且有中文说明；13个正式示例、13个JSONC示例、
6个负例和37个固定C++测试均符合预期。

- [x] **Step 4: 建立Schema展开检查点**

```bash
git add schemas/customer/v1/*.schema.json schemas/customer/v1/README.md \
  docs/甲方接口对齐规范与JSON-Schema.md scripts/validate_customer_interface.sh \
  tests/CustomerInterfaceValidationScriptTest.sh \
  docs/superpowers/plans/2026-08-18-standalone-customer-schemas.md
git commit -m "docs: flatten customer interface schemas"
```
