# NRM Final Code Closure Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 收口时延、规划成员、环境子域和provenance语义，并将已验证变更合入正式开发分支。

**Architecture:** 保留现有AFSIM Base + Customer Overlay主链。只在`EnvironmentContext`增加Customer子域所有权集合，环境适配器按子域判定模式；其他修复限于现有Codec、Schema和Validator。

**Tech Stack:** C++14、Qt JSON、Bash、CMake/CTest、AFSIM 2.9外部扩展。

## Global Constraints

- 不修改AFSIM核心。
- 不新增HTTP、REST、gRPC、TCP、WebSocket或MQ。
- 不重写Assessment、Capability、路由或规划算法。
- 先证明旧行为失败，再实施最小修复。
- 完整验证前不提交，正式分支合并后必须复验。

---

### Task 1: Delay and allocation Schema/runtime alignment

**Files:**
- Modify: `tests/CustomerJsonCodecTest.cpp`
- Modify: `schemas/customer/v1/resource-demand-request.schema.json`
- Modify: `schemas/customer/v1/network-plan.schema.json`
- Modify: `warlock/source/NrmCustomerJsonCodec.cpp`

**Interfaces:**
- Consumes: `CustomerJsonCodec::DecodeAssessment/DecodeResourceDemands/DecodeNetworkPlan`.
- Produces: `maximumDelayMs == 0` accepted without delay feasibility requirement; empty allocation members rejected in both layers.

- [x] Add tests for delay -1/0/100 and members empty/single/duplicate.
- [x] Build/run `nrm_customer_json_codec_test` and confirm the zero-delay and Schema drift assertions fail before implementation.
- [x] Change only the inconsistent runtime comparisons and Schema keywords.
- [x] Rebuild/run Codec test and offline contract validation.

### Task 2: Environment subdomain mode and provenance

**Files:**
- Modify: `tests/EnvironmentEffectAdapterTest.cpp`
- Modify: `tests/CustomerJsonCodecTest.cpp`
- Modify: `tests/CustomerNrmAdapterTest.cpp`
- Modify: `include/nrm/CommunicationCapabilityTypes.hpp`
- Modify: `include/nrm/BuiltInEnvironmentEffectAdapter.hpp`
- Modify: `include/nrm/CustomerNrmAdapter.hpp`
- Modify: `warlock/source/NrmCustomerJsonCodec.cpp`

**Interfaces:**
- Consumes: `EnvironmentContext.applicationMode`, per-subdomain origin/confidence and partial `EnvironmentSnapshot`.
- Produces: `EnvironmentContext.customerProvidedDomains`; domain-specific mode and evidence.

- [x] Add mixed AFSIM terrain/interference plus Customer weather/interference tests.
- [x] Run affected tests and confirm global Customer mode/provenance assertions fail.
- [x] Populate Customer-owned domains at JSON/direct-C++ boundaries and apply mode only to controlled domains.
- [x] Use matching subdomain provenance for all explicit metrics/evidence and rerun tests.

### Task 3: Canonical numeric finite-value audit

**Files:**
- Modify: `tests/ResourceSnapshotValidatorTest.cpp`
- Modify: `include/nrm/ResourceSnapshotValidator.hpp`

**Interfaces:**
- Consumes: canonical `ResourceSnapshot`.
- Produces: rejection of non-finite establishment delay, RSSI and SNR while preserving existing issue/valid invariant.

- [x] Add NaN/Inf tests and assert `valid == issues.empty()` on accepted and rejected snapshots.
- [x] Run validator test and confirm old implementation accepts the new invalid values.
- [x] Add minimal finite/range checks through the existing `InRange` path.
- [x] Rerun validator and direct Customer adapter tests.

### Task 4: Documentation, verification, commit and integration

**Files:**
- Modify: `docs/ARCHITECTURE.md`
- Modify: `docs/VALIDATION.md`
- Modify: `docs/ai/CURRENT_MILESTONE.md`
- Modify: `docs/ai/DECISIONS.md`
- Modify: `docs/ai/SESSION_HANDOFF.md`
- Modify: `docs/甲方接口对齐规范与JSON-Schema.md`

**Interfaces:**
- Consumes: verified implementation and test evidence.
- Produces: auditable Git commit on `feat/code-quality-raii-hardening` and fast-forward merge into `feat/v0.11-model-service-facade`.

- [x] Update only affected semantics and actual test evidence.
- [x] Run diff/static/contract/full tests/build/CTest/one approved scenario.
- [ ] Commit all reviewed worktree changes with one closure commit.
- [ ] Fast-forward the formal branch, rerun key gates from the formal worktree and report exact branch/HEAD/status.
