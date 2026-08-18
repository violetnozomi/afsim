# Customer JSONC Templates Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 为 13 类甲方接口提供逐字段中文注释的真实业务 JSONC 模板，并自动验证模板仍符合正式 Schema。

**Architecture:** 正式 Schema 与无注释 JSON 示例保持不变；人工模板放入独立的 `examples-commented` 目录。现有离线校验脚本负责安全去除 JSONC 行注释并按同名 Schema 验证，Shell 测试锁定模板数量和校验结果。

**Tech Stack:** JSON Schema Draft 2020-12、JSONC、Bash、Python 3 离线 `jsonschema` 校验器、Markdown。

## Global Constraints

- 不修改 C++ 运行时接口、算法或 AFSIM 框架。
- 不把带注释 JSONC 作为正式交换载荷。
- 不引入新的运行时依赖或外部通信协议。
- 13 份模板必须与现有 13 份 Schema 一一对应。
- 每个已出现的业务字段必须带简短中文行尾注释。

---

### Task 1: 锁定注释模板校验行为

**Files:**
- Modify: `tests/CustomerInterfaceValidationScriptTest.sh`
- Modify: `scripts/validate_customer_interface.sh`

**Interfaces:**
- Consumes: `schemas/customer/v1/<contract>.schema.json`
- Produces: 校验输出 `PASS: 13 commented JSONC examples validated.`

- [x] **Step 1: 写失败测试**

在测试中捕获正常校验输出并断言存在精确的 13 份注释模板通过标记。

- [x] **Step 2: 运行测试确认失败**

Run: `bash tests/CustomerInterfaceValidationScriptTest.sh`

Expected: 因当前脚本没有该通过标记而失败。

- [x] **Step 3: 实现最小 JSONC 校验**

为校验脚本增加 `examples-commented` 参数、字符串感知的 `//` 清理函数、文件名到同名 Schema 的映射和数量检查。

- [x] **Step 4: 运行测试确认通过**

Run: `bash tests/CustomerInterfaceValidationScriptTest.sh`

Expected: 13 份模板存在后输出并匹配通过标记。

### Task 2: 新增 13 份逐字段注释模板

**Files:**
- Create: `schemas/customer/v1/examples-commented/README.md`
- Create: `schemas/customer/v1/examples-commented/*.example.jsonc`（13 份）

**Interfaces:**
- Consumes: `schemas/customer/v1/examples/*.example.json` 与对应 `*.schema.json`
- Produces: 甲方可直接阅读的实际报文模板

- [x] **Step 1: 创建请求、上报类模板**

按实际信封创建资源、导航、环境、评估、需求、规划、入退网和提供方声明模板，每个示例字段添加简短行尾中文注释。

- [x] **Step 2: 创建响应类模板**

创建评估响应、需求响应、规划结果、输入确认和错误模板，注明字段由 NRM 生成。

- [x] **Step 3: 添加目录说明**

说明 `.jsonc` 仅供阅读，正式传输应移除注释或直接使用 `examples/` 中同名 JSON。

- [x] **Step 4: 运行接口校验**

Run: `PYTHON_BIN=/usr/bin/python3 ./scripts/validate_customer_interface.sh`

Expected: 正式示例、无效样例和 13 份注释模板全部按预期通过或拒绝。

### Task 3: 更新接口索引并完成回归

**Files:**
- Modify: `schemas/customer/v1/README.md`
- Modify: `docs/甲方接口对齐规范与JSON-Schema.md`

**Interfaces:**
- Consumes: `examples-commented/` 目录
- Produces: 甲方入口文档中的明确链接和使用说明

- [x] **Step 1: 更新接口目录说明**

区分正式 Schema、无注释示例、逐字段注释模板和聚合总览文件。

- [x] **Step 2: 更新甲方对齐规范**

把首选人工阅读入口改为 `examples-commented/README.md`，明确 `$schema/$id` 只属于 Schema 元数据，不是业务报文字段。

- [x] **Step 3: 执行完整相关验证**

Run: `git diff --check`

Run: `./scripts/ai_guard.sh static`

Run: `PYTHON_BIN=/usr/bin/python3 ./scripts/ai_guard.sh contract`

Run: `bash tests/CustomerInterfaceValidationScriptTest.sh`

Expected: 所有命令退出码为 0，无格式错误，13 个正式示例与 13 个注释示例均通过对应 Schema。
