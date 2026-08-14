# Contract Gap Closure Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add minimal, parameterized implementations for the remaining contract-visible state, protocol, environment, planning-coordination, resource-management, navigation, recovery, compatibility, and deployment requirements.

**Architecture:** Extend the existing immutable public value objects and pure C++ services, then adapt them at the Qt/AFSIM boundary. Every value remains source-qualified and validity-qualified; current AFSIM observations are never overwritten by lower-confidence parameterized values. The four milestones are independently testable and committed so each can be reverted without changing AFSIM core.

**Tech Stack:** C++17-compatible header-only domain services, Qt 5 Core/Widgets at the Warlock boundary, JSON Schema Draft 2020-12, Bash validation scripts, AFSIM 2.9 WSF and Warlock plugin builds.

## Global Constraints

- Do not modify AFSIM core source; modify only `network_resource_manager`.
- Add no third-party dependencies.
- Public `include/nrm/` code must not include Qt or AFSIM headers.
- The plugin remains read-only: it may emit recommendations and local coordination evidence but may not alter live AFSIM links, frequencies, routes, or platform state.
- Parameterized results use `DataOrigin::cPARAMETERIZED_MODEL` and `Confidence::cLOW`.
- Invalid or missing measurements use `valid=false` with a fixed reason; never use zero as a missing sentinel.
- Use test-first red/green cycles and one Git commit per task.

---

### Task 1: Contract-visible resource state and metric enrichment

**Files:**
- Modify: `include/nrm/NetworkResourceTypes.hpp`
- Modify: `include/nrm/ContractMetricEnricher.hpp`
- Modify: `tests/ContractMetricEnricherTest.cpp`
- Modify: `tests/NetworkResourceTypesTest.cpp`

**Interfaces:**
- Produces: `OperationalArea`, `PlatformAttitude`, `ResourceAlarm`, `ProtocolResourceState`, `CoverageState`, `ResourceProxyState`.
- Extends: `LinkSnapshot` with `supportedBusinessTypes`, `communicationQualityPercent`, `subnetId`, `protocolResource`, `coverage`, and `activeAlarmIds`.
- Extends: `ResourceSnapshot` with `operationalArea`, `platformAttitudes`, `alarms`, and `resourceProxies`.

- [ ] **Step 1: Write failing type and enrichment tests**

Add literal assertions that a Link-16 profile enriches a link with maximum range, business types, protocol kind `TIMESLOT`, valid `used/capacity` occupancy, and a quality score derived from hand-set PDR/SNR/BER/delay inputs. Add assertions that zero protocol capacity and zero quality inputs stay invalid. Add a link-offline alarm test with fixed code `LINK_OFFLINE`.

- [ ] **Step 2: Run the focused tests and verify RED**

Run:

```bash
./scripts/ai_guard.sh build nrm_framework_types_test nrm_contract_metric_enricher_test
```

Expected: compilation fails because the new public fields and types do not exist.

- [ ] **Step 3: Add minimal public types and enrichment behavior**

Use these stable shapes:

```cpp
struct ProtocolResourceState
{
   std::string kind;
   std::size_t capacity = 0;
   std::size_t used = 0;
   std::size_t remaining = 0;
   MetricValue<double> utilizationPercent;
};

struct CoverageState
{
   MetricValue<double> maximumRangeM;
   std::string shape = "CIRCLE_APPROXIMATION";
};
```

Keep alarms and area values data-only. Implement quality as the arithmetic mean of valid normalized PDR, SNR margin, BER, and latency terms; require at least one valid term and clamp to `[0,100]`.

- [ ] **Step 4: Run focused and full tests**

```bash
./scripts/ai_guard.sh build nrm_framework_types_test nrm_contract_metric_enricher_test
./scripts/ai_guard.sh test
```

Expected: all fixed tests pass and both plugins build.

- [ ] **Step 5: Commit**

```bash
git add include/nrm/NetworkResourceTypes.hpp include/nrm/ContractMetricEnricher.hpp tests/ContractMetricEnricherTest.cpp tests/NetworkResourceTypesTest.cpp
git commit -m "feat: add contract resource state fields"
```

### Task 2: Customer JSON schemas and codec alignment

**Files:**
- Modify: `schemas/customer/v1/resource-report.schema.json`
- Create: `schemas/customer/v1/provider-hello.schema.json`
- Create: `schemas/customer/v1/ingest-ack.schema.json`
- Create: `schemas/customer/v1/examples/provider-hello.example.json`
- Create: `schemas/customer/v1/examples/ingest-ack.example.json`
- Modify: `schemas/customer/v1/examples/resource-report.example.json`
- Modify: `schemas/customer/v1/nrm-customer-interface-v1.annotated.jsonc`
- Modify: `warlock/source/NrmCustomerJsonCodec.hpp`
- Modify: `warlock/source/NrmCustomerJsonCodec.cpp`
- Modify: `warlock/source/NrmDataContainer.cpp`
- Modify: `tests/CustomerJsonCodecTest.cpp`
- Modify: `scripts/validate_customer_interface.sh`

**Interfaces:**
- Produces: decoding of optional area, attitude, protocol resource, coverage, proxy, alarm, route, flow, and gateway records into the Task 1 public objects.
- Produces: `DecodeProviderHello(...)` and `EncodeIngestAck(...)` at the Qt boundary.
- Preserves: the existing minimal `networks/members/links` resource report.

- [ ] **Step 1: Add failing schema and codec fixtures**

Add one full resource report containing one polygon, one attitude, one protocol resource, one alarm, and one proxy. Assert exact decoded values. Add an old minimal fixture and assert it still decodes. Add provider hello and ACK validation fixtures.

- [ ] **Step 2: Verify RED**

```bash
./scripts/ai_guard.sh contract
./scripts/ai_guard.sh build nrm_customer_json_codec_test
```

Expected: new fixtures fail validation or codec compilation before implementation.

- [ ] **Step 3: Extend schemas and codec atomically**

All added resource fields are optional for backward compatibility, but if present use `additionalProperties:false`, finite numeric bounds, unique identifiers, and reference checks. Decode into a temporary `ResourceSnapshot` and assign only after all records validate.

- [ ] **Step 4: Add provider handshake and ACK codec behavior**

Decode provider ID, software version, supported schemas, and supported network types into a temporary
`CustomerProviderHello` value. Encode accepted, duplicate, stale, and rejected ACK states with the original
message ID. Dynamic membership remains parse-only until Task 5 adds the service that can create a safe new
planning revision; the existing file entry must continue returning `SCHEMA_UNSUPPORTED` for that schema.

- [ ] **Step 5: Verify and commit**

```bash
./scripts/ai_guard.sh contract
./scripts/ai_guard.sh build nrm_customer_json_codec_test
./scripts/ai_guard.sh test
git add schemas/customer/v1 warlock/source/NrmCustomerJsonCodec.hpp warlock/source/NrmCustomerJsonCodec.cpp warlock/source/NrmDataContainer.cpp tests/CustomerJsonCodecTest.cpp scripts/validate_customer_interface.sh
git commit -m "feat: align customer resource schemas"
```

### Task 3: Frequency characteristics and protocol resource model

**Files:**
- Create: `include/nrm/FrequencyCharacteristicRepository.hpp`
- Create: `include/nrm/ProtocolResourceModel.hpp`
- Create: `tests/FrequencyCharacteristicRepositoryTest.cpp`
- Create: `tests/ProtocolResourceModelTest.cpp`
- Modify: `include/nrm/AssessmentEvaluator.hpp`
- Modify: `include/nrm/ConcurrentTaskAssessment.hpp`
- Modify: `CMakeLists.txt`
- Create: `data/frequency_characteristics_v1.txt`

**Interfaces:**
- Produces: `FrequencyCharacteristicRepository::BuiltInDemo()` and strict `LoadFromFile(path, validation)`.
- Produces: `ProtocolResourceModel::Evaluate(NetworkType, activeOwners, requiredBandwidthBps)` returning `ProtocolResourceState`, added delay, and a fixed exhaustion reason.
- Consumes: existing `NetworkProfileRepository` and Task 1 protocol state.

- [ ] **Step 1: Write failing repository tests**

Test two frequencies per network, exact Link-16 capacity scale, invalid negative delay, duplicate network/frequency rejection, and atomic preservation of the previous revision.

- [ ] **Step 2: Verify RED**

```bash
./scripts/ai_guard.sh build nrm_frequency_characteristic_test
```

Expected: target/source does not exist.

- [ ] **Step 3: Implement strict repository**

Use schema:

```text
NRM_FREQUENCY_CHARACTERISTICS_V1 "demo-frequency-v1" "nrm-parameterized-frequency"
FREQUENCY "l16-primary" LINK16 1000000000 500000 1.0 4.0 98.0 3.0 2000000
```

Fields after frequency are maximum range metres, capacity scale, delay ms, PDR percent, interference threshold dB, and protection bandwidth Hz.

- [ ] **Step 4: Write failing protocol model tests and verify RED**

Test Link-11 units 8, Link-16 slots 16, SATCOM capacity 8, CDL capacity 4, exact resource exhaustion at the first owner beyond capacity, and deterministic added delay.

- [ ] **Step 5: Implement protocol model and replace duplicated concurrent defaults**

Move protocol-kind naming and capacity calculation into `ProtocolResourceModel`; retain existing acceptance constants and results.

- [ ] **Step 6: Verify and commit**

```bash
./scripts/ai_guard.sh build nrm_frequency_characteristic_test nrm_protocol_resource_model_test nrm_concurrent_task_assessment_test
./scripts/ai_guard.sh test
git add include/nrm/FrequencyCharacteristicRepository.hpp include/nrm/ProtocolResourceModel.hpp tests/FrequencyCharacteristicRepositoryTest.cpp tests/ProtocolResourceModelTest.cpp include/nrm/AssessmentEvaluator.hpp include/nrm/ConcurrentTaskAssessment.hpp CMakeLists.txt data/frequency_characteristics_v1.txt
git commit -m "feat: model frequency and protocol resources"
```

### Task 4: Operational area, weather time, attitude, and interference evaluation

**Files:**
- Create: `include/nrm/OperationalEnvironmentEvaluator.hpp`
- Create: `tests/OperationalEnvironmentEvaluatorTest.cpp`
- Modify: `include/nrm/CommunicationCapabilityTypes.hpp`
- Modify: `include/nrm/BuiltInEnvironmentEffectAdapter.hpp`
- Modify: `include/nrm/CommunicationCapabilityService.hpp`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Extends: `CapabilityRequest` with `taskStartTime`, `taskEndTime`, and `frequencyHz`.
- Produces: point-in-polygon, weather-window coverage, directional-attitude check, and protected-band overlap results.
- Consumes: Task 1 area/attitude objects and Task 3 frequency characteristic.

- [ ] **Step 1: Write failing geometry and time tests**

Use a literal four-point square and assert inside, outside, and boundary behavior. Assert an environment window `[10,30]` covers task `[12,20]` but not `[5,20]`.

- [ ] **Step 2: Write failing attitude and interference tests**

Assert an antenna heading tolerance of 30 degrees accepts a 20-degree error and rejects 31 degrees. Assert `[998,1002] MHz` overlaps a `1000 MHz ± 1 MHz` protected band while `1010 MHz` does not.

- [ ] **Step 3: Verify RED, implement minimal evaluator, and verify GREEN**

```bash
./scripts/ai_guard.sh build nrm_operational_environment_evaluator_test
```

Implement pure deterministic functions; normalize headings to `[0,360)` and treat polygon boundary as inside.

- [ ] **Step 4: Integrate only with candidate links**

Current AFSIM links retain observed metrics. Candidate evaluation rejects endpoints outside a valid area, rejects directional attitude failure, and applies weather/interference only when the request time/frequency is covered.

- [ ] **Step 5: Verify and commit**

```bash
./scripts/ai_guard.sh build nrm_operational_environment_evaluator_test nrm_communication_capability_service_test
./scripts/ai_guard.sh test
git add include/nrm/OperationalEnvironmentEvaluator.hpp tests/OperationalEnvironmentEvaluatorTest.cpp include/nrm/CommunicationCapabilityTypes.hpp include/nrm/BuiltInEnvironmentEffectAdapter.hpp include/nrm/CommunicationCapabilityService.hpp CMakeLists.txt
git commit -m "feat: evaluate operational environment inputs"
```

### Task 5: Plan coordination, dynamic membership, and local ACK

**Files:**
- Create: `include/nrm/NetworkPlanCoordinationService.hpp`
- Create: `tests/NetworkPlanCoordinationServiceTest.cpp`
- Modify: `include/nrm/NetworkPlanTypes.hpp`
- Modify: `include/nrm/NetworkPlanRepository.hpp`
- Modify: `include/nrm/NetworkPlanDistributionService.hpp`
- Modify: `warlock/source/NrmDataContainer.hpp`
- Modify: `warlock/source/NrmDataContainer.cpp`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Adds: `PlanningDomain { AIRBORNE, GROUND, JOINT }` and `planningDomain` in `NetworkPlanDocument`.
- Produces: `ApplyMembership(plan, change) -> PlanCoordinationResult` with a new revision and unchanged input.
- Produces: `AcknowledgeDistribution(package, ackPath) -> PlanCoordinationResult` validating plan ID, revision, and fingerprint.

- [ ] **Step 1: Write failing membership tests**

Assert JOIN adds exactly one member in revision `n+1`; LEAVE removes it; duplicate JOIN, missing LEAVE, wrong allocation, and wrong plan ID fail without mutating the input.

- [ ] **Step 2: Write failing ACK tests**

Create a temporary outbox package, write an ACK with exact plan ID/revision/fingerprint, and assert confirmed. Alter each field separately and assert a fixed mismatch reason.

- [ ] **Step 3: Verify RED and implement minimal service**

```bash
./scripts/ai_guard.sh build nrm_network_plan_coordination_test
```

The service must not modify live AFSIM state and must never overwrite a revision directory.

- [ ] **Step 4: Wire membership JSON loading**

Replace the Task 2 unsupported branch with a call that creates a new plan draft through `NetworkPlanRepository::ReplaceDraft`. Emit one audit event for accepted or rejected requests.

- [ ] **Step 5: Verify and commit**

```bash
./scripts/ai_guard.sh build nrm_network_plan_coordination_test nrm_customer_json_codec_test nrm_network_plan_repository_test
./scripts/ai_guard.sh test
git add include/nrm/NetworkPlanCoordinationService.hpp tests/NetworkPlanCoordinationServiceTest.cpp include/nrm/NetworkPlanTypes.hpp include/nrm/NetworkPlanRepository.hpp include/nrm/NetworkPlanDistributionService.hpp warlock/source/NrmDataContainer.hpp warlock/source/NrmDataContainer.cpp CMakeLists.txt
git commit -m "feat: coordinate plan membership and ACKs"
```

### Task 6: Demand coordinator, feedback history, and interference avoidance

**Files:**
- Create: `include/nrm/ResourceDemandCoordinator.hpp`
- Create: `tests/ResourceDemandCoordinatorTest.cpp`
- Modify: `include/nrm/ResourceDemandTypes.hpp`
- Modify: `include/nrm/PlanningRecommendationEngine.hpp`
- Modify: `tests/ResourceDemandMatchingTest.cpp`
- Modify: `warlock/source/NrmDataContainer.hpp`
- Modify: `warlock/source/NrmDataContainer.cpp`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Extends: `ResourceDemandSet` with `requestSource` and `correlationId`.
- Produces: `ResourceDemandFeedback` carrying source, correlation, classifications, batch result, and historical pass ratio.
- Produces: `ResourceDemandCoordinator::Evaluate(...)` with a 128-entry bounded history.
- Extends: frequency candidates with interference center/bandwidth/protection values.

- [ ] **Step 1: Write failing coordinator tests**

Assert source/correlation echo, fixed classifications `CONNECTIVITY/PERFORMANCE/NETWORK_SCALE/TRAFFIC`, exact historical ratio after one pass and one fail, business-type isolation, and eviction of the oldest entry after 129 batches.

- [ ] **Step 2: Write failing frequency avoidance tests**

Provide an occupied candidate, an unoccupied but interfered candidate, and a clean candidate. Assert the clean candidate is selected and evidence contains the non-overlap margin. Assert all interfered candidates return `INTERFERENCE_CONFLICT`.

- [ ] **Step 3: Verify RED and implement**

```bash
./scripts/ai_guard.sh build nrm_resource_demand_coordinator_test nrm_resource_demand_matching_test
```

Keep history in memory as summaries only; do not create a database or alter demand thresholds.

- [ ] **Step 4: Wire DataContainer and Reporter-facing state**

Use the coordinator for demand-set evaluation and retain the latest feedback for Warlock/Reporter. Existing single-batch results stay available for compatibility.

- [ ] **Step 5: Verify and commit**

```bash
./scripts/ai_guard.sh test
git add include/nrm/ResourceDemandCoordinator.hpp tests/ResourceDemandCoordinatorTest.cpp include/nrm/ResourceDemandTypes.hpp include/nrm/PlanningRecommendationEngine.hpp tests/ResourceDemandMatchingTest.cpp warlock/source/NrmDataContainer.hpp warlock/source/NrmDataContainer.cpp CMakeLists.txt
git commit -m "feat: close resource demand feedback loop"
```

### Task 7: Navigation accuracy, recovery state, and reference model

**Files:**
- Create: `include/nrm/NavigationAccuracyModel.hpp`
- Create: `include/nrm/RecoveryStateStore.hpp`
- Create: `include/nrm/ReferenceModelAdapter.hpp`
- Create: `tests/NavigationAccuracyModelTest.cpp`
- Create: `tests/RecoveryStateStoreTest.cpp`
- Create: `tests/ReferenceModelAdapterTest.cpp`
- Modify: `include/nrm/NetworkResourceTypes.hpp`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Produces: three built-in `NavigationAccuracyProfile` values and `Enrich(sample, navigationType)`.
- Produces: atomic `RecoveryStateStore::Save/Load` for last valid config, plan revision, demand revision, and unacknowledged package metadata.
- Produces: a test-only concrete adapter/descriptor that registers in the real `ModelRegistry` and answers health/descriptor calls.

- [ ] **Step 1: Write failing navigation tests**

Assert exact configured horizontal/vertical/heading sigma for GNSS, INS, and integrated modes; assert direct valid errors are unchanged; assert unknown mode stays invalid.

- [ ] **Step 2: Write failing recovery tests**

Using `tmp_path`-equivalent temporary directories in C++, assert atomic round-trip, missing file, truncated file, non-finite revision data, and preservation of the prior valid state after a failed load.

- [ ] **Step 3: Write failing reference adapter tests**

Register the reference descriptor, query supported operations/schema, obtain a healthy response, unregister it, and assert it is absent.

- [ ] **Step 4: Verify RED, implement, and verify GREEN**

```bash
./scripts/ai_guard.sh build nrm_navigation_accuracy_model_test nrm_recovery_state_store_test nrm_reference_model_adapter_test
./scripts/ai_guard.sh test
```

- [ ] **Step 5: Commit**

```bash
git add include/nrm/NavigationAccuracyModel.hpp include/nrm/RecoveryStateStore.hpp include/nrm/ReferenceModelAdapter.hpp tests/NavigationAccuracyModelTest.cpp tests/RecoveryStateStoreTest.cpp tests/ReferenceModelAdapterTest.cpp include/nrm/NetworkResourceTypes.hpp CMakeLists.txt
git commit -m "feat: add navigation accuracy and recovery evidence"
```

### Task 8: Warlock, reporting, deployment check, and documentation

**Files:**
- Modify: `warlock/source/NrmDockWidget.cpp`
- Modify: `warlock/source/NrmDockWidget.hpp`
- Modify: `warlock/source/NrmSnapshotReporter.cpp`
- Modify: `warlock/source/NrmSnapshotReporter.hpp`
- Modify: `tests/SnapshotReporterTest.cpp`
- Modify: `tests/UiTextTest.cpp`
- Create: `scripts/check_deployment_contract.sh`
- Modify: `scripts/run_preacceptance.sh`
- Modify: `docs/功能使用手册.md`
- Modify: `docs/运行与可视化入口.md`
- Modify: `docs/IMPLEMENTATION_STATUS.md`
- Modify: `docs/合同30项指标实现矩阵.md`
- Modify: `docs/甲方接口对齐规范与JSON-Schema.md`
- Modify: `data/contract_coverage.yaml`
- Modify: `data/requirement_traceability.yaml`

**Interfaces:**
- Produces: Chinese display of area/time/attitude, coverage/quality/protocol/alarm, plan domain/membership/ACK, demand feedback/history.
- Produces: bounded JSONL outputs `resource_alarms.jsonl`, `demand_feedback.jsonl`, `planning_coordination.jsonl`, and `deployment_checks.jsonl`.
- Produces: deployment JSON with `PASS/FAIL/CUSTOMER_BLOCKED` per hardware/interface requirement.

- [ ] **Step 1: Write failing reporter and UI text tests**

Assert exact schema names and one representative field per new JSONL. Assert fixed reason codes have Chinese labels and no English-only user-facing fallback for the new states.

- [ ] **Step 2: Verify RED and implement reporting/UI**

```bash
./scripts/ai_guard.sh build nrm_snapshot_reporter_test nrm_ui_text_test
```

Use existing responsive tables and scroll areas; do not add fixed pixel minimum sizes or a new top-level dock.

- [ ] **Step 3: Add executable deployment check**

The script reads `/proc/cpuinfo`, `/proc/meminfo`, `df`, `ip link`, compiler version, AFSIM path, and plugin path. It writes valid JSON, returns nonzero only for locally testable hard failures, and labels OA/security/model-tool/third-party-formal checks `CUSTOMER_BLOCKED`.

- [ ] **Step 4: Update full contract traceability**

For every `NAV/DL/CAP/PLAN/MGR/IF/ENV/OPS/EXT/DES` key, record exact implementation state and evidence. Remove nonexistent component names and align every documented Schema with files present in `schemas/customer/v1/`.

- [ ] **Step 5: Run complete verification**

```bash
./scripts/ai_guard.sh static
./scripts/ai_guard.sh contract
./scripts/ai_guard.sh test
./scripts/ai_guard.sh scenario operational_strike_demo
./scripts/check_deployment_contract.sh
./scripts/run_preacceptance.sh
```

Expected: static/contract/tests/scenario pass; preacceptance passes all internal checks; external-only items appear as `CUSTOMER_BLOCKED`, not false failures or false passes.

- [ ] **Step 6: Commit**

```bash
git add warlock/source/NrmDockWidget.cpp warlock/source/NrmDockWidget.hpp warlock/source/NrmSnapshotReporter.cpp warlock/source/NrmSnapshotReporter.hpp tests/SnapshotReporterTest.cpp tests/UiTextTest.cpp scripts/check_deployment_contract.sh scripts/run_preacceptance.sh docs data/contract_coverage.yaml data/requirement_traceability.yaml
git commit -m "feat: expose contract closure evidence"
```

### Task 9: Final audit and release checkpoint

**Files:**
- Modify: `CHANGELOG.md`
- Modify: `docs/VALIDATION.md`
- Modify: `docs/ai/CURRENT_MILESTONE.md`
- Modify: `docs/ai/SESSION_HANDOFF.md`

**Interfaces:**
- Produces: one traceable release checkpoint with commands, outputs, remaining customer-blocked items, and rollback commits.

- [ ] **Step 1: Re-read the design and contract baseline**

Check every design section against a committed task and every contract key against an evidence row. Do not promote customer-blocked items.

- [ ] **Step 2: Run fresh full verification**

```bash
./scripts/ai_guard.sh static
./scripts/ai_guard.sh contract
./scripts/ai_guard.sh test
./scripts/ai_guard.sh scenario operational_strike_demo
```

- [ ] **Step 3: Record evidence and rollback points**

Record exact test counts, scenario counts, generated report path, each task commit, and `git revert <commit>` rollback order.

- [ ] **Step 4: Commit release checkpoint**

```bash
git add CHANGELOG.md docs/VALIDATION.md docs/ai/CURRENT_MILESTONE.md docs/ai/SESSION_HANDOFF.md
git commit -m "docs: record contract closure verification"
```
