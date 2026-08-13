# Customer JSON Interface Baseline Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Deliver a minimal, versioned JSON file interface for customer-modified AFSIM modules while preserving direct AFSIM callback collection, public C++ value-object integration, internal `.nrm` storage, and `.neh` navigation compatibility.

**Architecture:** The WSF plugin remains the source of direct AFSIM observations. A QtCore-only `CustomerJsonCodec` at the Warlock boundary parses and emits UTF-8 JSON, validates the common envelope, and converts payloads into existing pure C++ NRM objects. Core assessment, planning, environment, and navigation code never depends on Qt or JSON; customer modules compiled in the same AFSIM process can use the existing public C++ boundaries without serializing data.

**Tech Stack:** C++17, AFSIM 2.9 extension API, Qt 5.12 Core/Widgets, Draft 2020-12 JSON Schema, Python 3 validation script, CMake/CTest.

## Global Constraints

- Do not modify AFSIM core source files.
- Keep `ContractInterfaceAdapter`, NRM public value objects, and model services free of Qt and JSON types.
- Use only Qt already supplied by AFSIM; add no third-party JSON dependency.
- Treat JSON files as atomic full messages; reject partial or semantically invalid input without changing the last valid state.
- Prefer direct AFSIM callbacks for values already exposed by customer AFSIM.
- Keep `.nrm` as internal plan storage and `.neh` as a supported native navigation-history input.
- Do not implement TCP, UDP, message queues, TLS, signatures, compression, incremental patches, or automatic network control.
- All UI text and user-facing errors are Chinese; code identifiers and fixed reason codes are English.
- Preserve unrelated dirty-worktree changes and commit only files belonging to each task.

---

## File Structure

### New files

- `schemas/customer/v1/common.schema.json` — common envelope, identifiers, timestamps, enums, positions, and error item definitions.
- `schemas/customer/v1/navigation-report.schema.json` — normalized GNSS/INS/integrated navigation input.
- `schemas/customer/v1/environment-report.schema.json` — optional terrain/weather/celestial/interference input.
- `schemas/customer/v1/resource-report.schema.json` — minimal four-network full snapshot input.
- `schemas/customer/v1/assessment-request.schema.json` — single-task evaluation request.
- `schemas/customer/v1/assessment-response.schema.json` — evaluation result output.
- `schemas/customer/v1/network-plan.schema.json` — external network plan input.
- `schemas/customer/v1/network-plan-result.schema.json` — validation/evaluation/package result output.
- `schemas/customer/v1/membership-request.schema.json` — JOIN/LEAVE request.
- `schemas/customer/v1/error.schema.json` — common failure response.
- `warlock/source/NrmCustomerJsonCodec.hpp` — Qt boundary request/response types and conversion API.
- `warlock/source/NrmCustomerJsonCodec.cpp` — strict envelope parsing, per-message conversion, and deterministic output.
- `tests/CustomerJsonCodecTest.cpp` — codec conversion, rejection, and no-partial-update tests.
- `schemas/customer/v1/examples/network-plan.example.json` — loadable four-network customer plan.
- `schemas/customer/v1/examples/network-plan-result.example.json` — representative plan result.
- `schemas/customer/v1/examples/membership-request.example.json` — dynamic membership request.
- `schemas/customer/v1/invalid/*.invalid.json` — targeted negative fixtures stored outside the positive-example glob.

### Modified files

- `schemas/customer/v1/README.md` — concise customer field guide and AFSIM-plugin integration priority.
- `schemas/customer/v1/nrm-customer-interface-v1.annotated.jsonc` — Chinese annotated review examples using the new envelope.
- `scripts/validate_customer_interface.sh` — validate independent schemas, positive examples, and expected negative failures.
- `include/nrm/ContractInterfaceAdapter.hpp` — add stable navigation/environment/resource/assessment/plan/membership conversion boundary methods without JSON types.
- `include/nrm/NetworkPlanTypes.hpp` — retain `NetworkPlanAdapter`; add no transport assumptions.
- `warlock/source/NrmDataContainer.hpp/.cpp` — atomically accept decoded external objects and retain last load result.
- `warlock/source/NrmDockWidget.hpp/.cpp` — add Chinese file-load/export actions to the existing relevant pages.
- `warlock/source/CMakeLists.txt` and root `CMakeLists.txt` — compile the codec and register its test.
- `scripts/ai_guard.sh` — include the codec test and contract validation in fixed checks.
- `docs/甲方接口对齐规范与JSON-Schema.md` — replace independent-service wording with AFSIM-plugin wording and the minimal V1 contract.
- `docs/功能使用手册.md` and `docs/运行与可视化入口.md` — exact load paths, buttons, expected results, and command-line validation.
- `docs/IMPLEMENTATION_STATUS.md`, `docs/VALIDATION.md`, and contract tracking documents — record internal pre-acceptance evidence without claiming final customer acceptance.

---

### Task 1: Freeze the Independent Minimal Schemas

**Files:**
- Create: `schemas/customer/v1/common.schema.json`
- Create: `schemas/customer/v1/navigation-report.schema.json`
- Create: `schemas/customer/v1/environment-report.schema.json`
- Create: `schemas/customer/v1/resource-report.schema.json`
- Create: `schemas/customer/v1/assessment-request.schema.json`
- Create: `schemas/customer/v1/assessment-response.schema.json`
- Create: `schemas/customer/v1/network-plan.schema.json`
- Create: `schemas/customer/v1/network-plan-result.schema.json`
- Create: `schemas/customer/v1/membership-request.schema.json`
- Create: `schemas/customer/v1/error.schema.json`
- Modify: `schemas/customer/v1/examples/*.json`
- Modify: `schemas/customer/v1/nrm-customer-interface-v1.annotated.jsonc`
- Modify: `schemas/customer/v1/README.md`
- Modify: `scripts/validate_customer_interface.sh`

**Interfaces:**
- Consumes: the exact envelope `{schema,messageId,timestamp,source,data}` from the approved design.
- Produces: independent Draft 2020-12 schemas whose `$id` values equal `https://nrm.local/schema/customer/v1/<filename>` and whose message `schema` constants use `nrm.customer.<type>.v1`.

- [ ] **Step 1: Rewrite the validator as a failing contract test**

Make `scripts/validate_customer_interface.sh` enumerate a fixed schema-to-example table and fail until all ten schema files exist:

```bash
contracts='navigation-report environment-report resource-report assessment-request assessment-response network-plan network-plan-result membership-request error'
for contract in $contracts; do
  schema="$ROOT/schemas/customer/v1/$contract.schema.json"
  example="$ROOT/schemas/customer/v1/examples/$contract.example.json"
  test -f "$schema" || { echo "ERROR: missing $schema" >&2; exit 1; }
  test -f "$example" || { echo "ERROR: missing $example" >&2; exit 1; }
done
```

- [ ] **Step 2: Run the contract test and verify the missing-schema failure**

Run:

```bash
./scripts/validate_customer_interface.sh
```

Expected: nonzero exit and the first missing independent schema path.

- [ ] **Step 3: Implement the common schema and nine message schemas**

Each message root must set `additionalProperties:false`, require exactly the five envelope fields, and reference common definitions. Keep only the approved minimum required fields. Express nonnegative numbers with `minimum:0`, PDR with `minimum:0, maximum:100`, latitude with `-90..90`, longitude with `-180..180`, and reject `UNKNOWN` network types.

- [ ] **Step 4: Replace positive examples and add isolated negative fixtures**

Keep valid examples in `examples/*.example.json`. Store negative inputs under `schemas/customer/v1/invalid/` so the positive glob cannot consume them. Each negative fixture violates one named rule: missing platform ID, bad network type, PDR over 100, unresolved member reference, or unknown schema.

- [ ] **Step 5: Update the Chinese JSONC bundle and customer README**

The JSONC bundle must contain one message per contract and line comments explaining every required field. The README must state the integration priority: AFSIM callback, public C++ object, JSON file, then `.neh` for navigation history.

- [ ] **Step 6: Run schema validation**

Run:

```bash
./scripts/validate_customer_interface.sh
```

Expected: every positive fixture reports `PASS`, every negative fixture reports `EXPECTED_REJECT`, and the final exit code is zero.

- [ ] **Step 7: Commit the schema baseline**

```bash
git add schemas/customer/v1 scripts/validate_customer_interface.sh
git commit -m "feat: simplify customer JSON interface schemas"
```

---

### Task 2: Add the Qt Boundary Codec and Common Error Handling

**Files:**
- Create: `warlock/source/NrmCustomerJsonCodec.hpp`
- Create: `warlock/source/NrmCustomerJsonCodec.cpp`
- Create: `tests/CustomerJsonCodecTest.cpp`
- Modify: `warlock/source/CMakeLists.txt`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Consumes: UTF-8 JSON bytes and the schema constants frozen in Task 1.
- Produces:

```cpp
struct CustomerJsonError
{
   std::string code;
   std::string path;
   std::string message;
};

struct CustomerJsonEnvelope
{
   std::string schema;
   std::string messageId;
   std::string timestamp;
   std::string source;
};

struct CustomerJsonDecodeResult
{
   bool valid = false;
   CustomerJsonEnvelope envelope;
   std::vector<CustomerJsonError> errors;
};

class CustomerJsonCodec
{
public:
   CustomerJsonDecodeResult Inspect(const QByteArray& aJson) const;
   QByteArray EncodeError(const CustomerJsonEnvelope& aEnvelope,
                          const std::vector<CustomerJsonError>& aErrors) const;
};
```

- [ ] **Step 1: Write failing envelope tests**

Add tests for a valid envelope, malformed JSON, non-object root, missing `data`, unsupported schema, and deterministic error JSON. Assert malformed JSON and a non-object root report `/`, a missing data object reports `/data`, and an unsupported schema reports `/schema`.

- [ ] **Step 2: Register and run the failing test**

Add `nrm_customer_json_codec_test` linked to Qt Core and compile `NrmCustomerJsonCodec.cpp` into both the Warlock plugin and the test.

Run:

```bash
cmake --build "$AFSIM_BUILD" --target nrm_customer_json_codec_test -j 4
"$AFSIM_BUILD/nrm_customer_json_codec_test"
```

Expected: compile failure because the codec does not exist.

- [ ] **Step 3: Implement strict envelope inspection**

Use `QJsonDocument::fromJson`, reject unknown top-level fields, verify all required strings are nonempty, verify `timestamp` with `QDateTime::fromString(..., Qt::ISODate)`, and accept only the frozen schema set. Never throw across the public boundary; return ordered errors.

- [ ] **Step 4: Implement deterministic error encoding**

Emit compact JSON with stable key order at object construction time. Copy request `messageId`; use current ISO timestamp and `source:"NRM"`; preserve error vector order.

- [ ] **Step 5: Run focused tests and existing model-boundary tests**

```bash
cmake --build "$AFSIM_BUILD" --target nrm_customer_json_codec_test nrm_model_service_facade_test nrm_model_registry_test -j 4
"$AFSIM_BUILD/nrm_customer_json_codec_test"
"$AFSIM_BUILD/nrm_model_service_facade_test"
"$AFSIM_BUILD/nrm_model_registry_test"
```

Expected: all exit zero.

- [ ] **Step 6: Commit the codec foundation**

```bash
git add CMakeLists.txt warlock/source/CMakeLists.txt warlock/source/NrmCustomerJsonCodec.* tests/CustomerJsonCodecTest.cpp
git commit -m "feat: add customer JSON boundary codec"
```

---

### Task 3: Decode Navigation, Environment, and Resource Reports

**Files:**
- Modify: `include/nrm/ContractInterfaceAdapter.hpp`
- Modify: `warlock/source/NrmCustomerJsonCodec.hpp`
- Modify: `warlock/source/NrmCustomerJsonCodec.cpp`
- Modify: `tests/CustomerJsonCodecTest.cpp`
- Modify: `tests/AfsimNavigationPacketParserTest.cpp`

**Interfaces:**
- Consumes: `navigation_report`, `environment_report`, and `resource_report` data objects.
- Produces:

```cpp
CustomerJsonDecodeResult DecodeNavigation(const QByteArray&, NavigationSample&) const;
CustomerJsonDecodeResult DecodeEnvironment(const QByteArray&, EnvironmentContext&) const;
CustomerJsonDecodeResult DecodeResources(const QByteArray&, ResourceSnapshot&) const;
```

Add format-neutral methods to `ContractInterfaceAdapter` for decoding these three public object types; do not mention `QByteArray`, `QJsonObject`, filenames, or transport in that core header.

- [ ] **Step 1: Write failing conversion tests**

Load each positive fixture and assert exact platform IDs, network types, positions, units, source, confidence, and sim time. Copy a valid destination object before each invalid decode and assert it remains byte-for-byte semantically unchanged after failure.

- [ ] **Step 2: Run the test and verify missing methods**

```bash
cmake --build "$AFSIM_BUILD" --target nrm_customer_json_codec_test -j 4
```

Expected: compile failure for missing decode methods.

- [ ] **Step 3: Implement navigation conversion**

Map `GNSS`, `INS`, and `INTEGRATED` to existing navigation modes without changing truth position. Calculate total position error only when all three Cartesian error components are valid. Keep `.neh` parsing unchanged and add a test showing JSON and `.neh` produce the same units and error-sign convention.

- [ ] **Step 4: Implement environment conversion**

Map only supplied domains. Preserve `applicationMode`; `ALREADY_INCLUDED` must never create a candidate adjustment, `INFORMATION_ONLY` must be display-only, and `CANDIDATE_ADJUSTMENT` may populate candidate effects with `PARAMETERIZED_MODEL/LOW` unless the file explicitly supplies a lower confidence.

- [ ] **Step 5: Implement four-network resource conversion**

Build a temporary `ResourceSnapshot`, validate unique network/member/link IDs and all references, then move it into the output only after complete success. Treat links as directed. Omitted optional measurements remain `valid=false`; never synthesize zero.

- [ ] **Step 6: Run conversion and regression tests**

```bash
cmake --build "$AFSIM_BUILD" --target nrm_customer_json_codec_test nrm_framework_types_test nrm_afsim_navigation_packet_parser_test nrm_environment_effect_adapter_test -j 4
"$AFSIM_BUILD/nrm_customer_json_codec_test"
"$AFSIM_BUILD/nrm_framework_types_test"
"$AFSIM_BUILD/nrm_afsim_navigation_packet_parser_test"
"$AFSIM_BUILD/nrm_environment_effect_adapter_test"
```

Expected: all exit zero.

- [ ] **Step 7: Commit the observation adapters**

```bash
git add include/nrm/ContractInterfaceAdapter.hpp warlock/source/NrmCustomerJsonCodec.* tests/CustomerJsonCodecTest.cpp tests/AfsimNavigationPacketParserTest.cpp
git commit -m "feat: decode customer AFSIM observation files"
```

---

### Task 4: Decode Assessment, Planning, and Membership Messages

**Files:**
- Modify: `warlock/source/NrmCustomerJsonCodec.hpp`
- Modify: `warlock/source/NrmCustomerJsonCodec.cpp`
- Modify: `tests/CustomerJsonCodecTest.cpp`
- Modify: `tests/NetworkPlanRepositoryTest.cpp`
- Modify: `tests/AssessmentEvaluatorTest.cpp`

**Interfaces:**
- Consumes: assessment request, network plan, and membership request JSON.
- Produces:

```cpp
CustomerJsonDecodeResult DecodeAssessment(const QByteArray&, AssessmentTask&) const;
CustomerJsonDecodeResult DecodeNetworkPlan(const QByteArray&, NetworkPlanDocument&) const;
CustomerJsonDecodeResult DecodeMembership(const QByteArray&, NetworkPlanChange&) const;
QByteArray EncodeAssessment(const CustomerJsonEnvelope&, const AssessmentResult&) const;
QByteArray EncodePlanResult(const CustomerJsonEnvelope&,
                            const NetworkPlanEvaluationResult&,
                            const DistributionPackageResult*) const;
```

- [ ] **Step 1: Write failing request and round-trip tests**

Assert the assessment request maps bandwidth, delay, PDR, source, destination, and allowed networks exactly. Assert the four-network plan converts to four allocations and four demands, saves as `.nrm`, reloads, and remains semantically equivalent. Assert `JOIN` on an existing member and `LEAVE` on a missing member are rejected by existing validation.

- [ ] **Step 2: Run focused tests and verify missing conversions**

```bash
cmake --build "$AFSIM_BUILD" --target nrm_customer_json_codec_test nrm_network_plan_repository_test -j 4
```

Expected: compile or assertion failure before implementation.

- [ ] **Step 3: Implement assessment request/result conversion**

Reject source equal to destination, empty allowed networks, and values outside Schema ranges. Encode `reachable`, `canEstablish`, `canComplete`, reason codes, margins, primary route, optional backup route, and recommendation strings without adding control commands.

- [ ] **Step 4: Implement plan conversion through existing public types**

Construct `NetworkPlanDocument` in a temporary value, set `source=CUSTOMER_MODULE`, preserve revision and explicit resource lists, and call existing repository/validator code after conversion. Do not duplicate `.nrm` parsing or validation rules inside the codec.

- [ ] **Step 5: Implement membership conversion as a new draft change**

Decode exactly one `NetworkPlanChange`. Applying it must create the next revision through the existing draft replacement path; never mutate a validated plan in place.

- [ ] **Step 6: Implement deterministic result encoding**

Plan output must contain validation status, evaluation status, per-demand status, reason codes, resulting revision/state, and package path only when a package exists. Assessment output must echo the request message ID.

- [ ] **Step 7: Run focused tests**

```bash
cmake --build "$AFSIM_BUILD" --target nrm_customer_json_codec_test nrm_assessment_evaluator_test nrm_network_plan_repository_test nrm_network_plan_evaluation_test -j 4
"$AFSIM_BUILD/nrm_customer_json_codec_test"
"$AFSIM_BUILD/nrm_assessment_evaluator_test"
"$AFSIM_BUILD/nrm_network_plan_repository_test"
"$AFSIM_BUILD/nrm_network_plan_evaluation_test"
```

Expected: all exit zero.

- [ ] **Step 8: Commit evaluation and plan conversions**

```bash
git add warlock/source/NrmCustomerJsonCodec.* tests/CustomerJsonCodecTest.cpp tests/NetworkPlanRepositoryTest.cpp tests/AssessmentEvaluatorTest.cpp schemas/customer/v1/examples/network-plan*.json schemas/customer/v1/examples/membership-request.example.json
git commit -m "feat: adapt customer assessment and planning JSON"
```

---

### Task 5: Integrate Atomic File Loading into the Warlock Plugin

**Files:**
- Modify: `warlock/source/NrmDataContainer.hpp`
- Modify: `warlock/source/NrmDataContainer.cpp`
- Modify: `warlock/source/NrmDockWidget.hpp`
- Modify: `warlock/source/NrmDockWidget.cpp`
- Modify: `warlock/source/NrmSnapshotReporter.hpp`
- Modify: `warlock/source/NrmSnapshotReporter.cpp`
- Modify: `tests/SnapshotReporterTest.cpp`

**Interfaces:**
- Consumes: a selected `.json` file or existing `.neh` navigation file.
- Produces:

```cpp
bool LoadCustomerJson(const std::string& aPath);
bool LoadNavigationHistory(const std::string& aPath,
                           const std::string& aPlatformName);
const CustomerJsonDecodeResult& LastCustomerJsonResult() const;
```

- [ ] **Step 1: Write failing DataContainer/reporter tests**

Test that a valid plan JSON populates the current plan, invalid JSON preserves the previous plan/snapshot, and every load attempt emits one audit record with schema, message ID, status, error code, and field path.

- [ ] **Step 2: Run focused tests and verify missing integration**

```bash
cmake --build "$AFSIM_BUILD" --target nrm_snapshot_reporter_test nrm_customer_json_codec_test -j 4
```

Expected: compile failure for the new DataContainer/reporter entry points.

- [ ] **Step 3: Implement one dispatching file entry point**

Read the file fully, call `Inspect`, dispatch by exact schema, decode into a temporary object, and publish only on success. Direct AFSIM snapshot updates remain the default live source; loading a resource report switches to explicit replay input until the next simulation initialization, and the UI must display that source.

- [ ] **Step 4: Add minimal Chinese UI actions**

Add one `加载甲方JSON` action to the navigation/environment area and allow the resource-planning load dialog to accept both `*.json` and `*.nrm`. Add `导出结果JSON` to assessment and planning results. Do not create a new top-level page. Show the last schema, message ID, success/failure, and first error path in the existing operation/status labels.

- [ ] **Step 5: Preserve `.neh` and `.nrm` compatibility**

Navigation file dialogs accept `*.json` and `*.neh`; plan dialogs accept `*.json` and `*.nrm`. Existing `.neh` and `.nrm` behavior and tests must remain unchanged.

- [ ] **Step 6: Add audit reporting**

Write accepted/rejected file events to `customer_interface_events.jsonl` in the current run directory. Never write the full payload; record schema, message ID, file basename, result, error code, and JSON Pointer.

- [ ] **Step 7: Build and run focused tests**

```bash
cmake --build "$AFSIM_BUILD" --target NetworkResourceManager nrm_snapshot_reporter_test nrm_customer_json_codec_test -j 4
"$AFSIM_BUILD/nrm_snapshot_reporter_test"
"$AFSIM_BUILD/nrm_customer_json_codec_test"
```

Expected: plugin links and tests exit zero.

- [ ] **Step 8: Commit plugin integration**

```bash
git add warlock/source/NrmDataContainer.* warlock/source/NrmDockWidget.* warlock/source/NrmSnapshotReporter.* tests/SnapshotReporterTest.cpp
git commit -m "feat: load customer JSON through Warlock plugin"
```

---

### Task 6: Make the Contract Gate Part of Fixed Verification

**Files:**
- Modify: `scripts/ai_guard.sh`
- Modify: `scripts/run_preacceptance.sh`
- Modify: `CMakeLists.txt`
- Modify: `docs/VALIDATION.md`

**Interfaces:**
- Consumes: all independent schemas, fixtures, codec test, WSF plugin, and Warlock plugin.
- Produces: one noninteractive command whose zero exit proves syntax, conversion, regression, and build success.

- [ ] **Step 1: Add a failing fixed-test expectation**

Add `nrm_customer_json_codec_test` to `TEST_NAMES` and make `check_cmd` invoke `contract_cmd` before returning success.

- [ ] **Step 2: Run the guard and verify it fails until registration is complete**

```bash
./scripts/ai_guard.sh check
```

Expected: missing target or contract failure.

- [ ] **Step 3: Register all targets and pre-acceptance counters**

Add the codec test to `NRM_TEST_TARGETS`; update expected test counts from 18 to 19 in scripts and documentation. Keep the contract check separate in the report so a Schema failure is distinguishable from a C++ test failure.

- [ ] **Step 4: Run complete verification**

```bash
./scripts/ai_guard.sh static
./scripts/ai_guard.sh contract
./scripts/ai_guard.sh test
./scripts/ai_guard.sh scenario four_network_overview
```

Expected: static passes, all schema cases pass, 19/19 C++ tests pass, and the four-network scenario reaches `Simulation complete` with four classified networks.

- [ ] **Step 5: Record exact evidence**

Append command, date, commit, pass counts, scenario final counters, and known boundary “customer AFSIM target rebuild not yet executed” to `docs/VALIDATION.md`. Do not mark final acceptance.

- [ ] **Step 6: Commit verification integration**

```bash
git add CMakeLists.txt scripts/ai_guard.sh scripts/run_preacceptance.sh docs/VALIDATION.md
git commit -m "test: gate customer JSON plugin integration"
```

---

### Task 7: Complete Customer and Operator Documentation

**Files:**
- Modify: `docs/甲方接口对齐规范与JSON-Schema.md`
- Modify: `docs/功能使用手册.md`
- Modify: `docs/运行与可视化入口.md`
- Modify: `docs/IMPLEMENTATION_STATUS.md`
- Modify: `README.md`
- Modify: `docs/ai/PROJECT_MEMORY.md`
- Modify: `docs/ai/DECISIONS.md`
- Modify: `docs/ai/SESSION_HANDOFF.md`

**Interfaces:**
- Consumes: verified behavior and exact commands from Tasks 1–6.
- Produces: one customer-facing contract guide and one operator workflow that do not describe NRM as a standalone server.

- [ ] **Step 1: Update architecture wording**

State consistently that the deliverable is a WSF extension plus Warlock plugin installed into customer-modified AFSIM. Describe the three data paths: direct AFSIM callbacks, public C++ objects for same-process custom modules, and JSON files for supplemental/offline data.

- [ ] **Step 2: Document every file workflow**

For navigation, environment, resource replay, assessment request, network plan, and membership request, document the exact sample path, Warlock button, expected success text, output path, and first troubleshooting command.

- [ ] **Step 3: Update contract tracking conservatively**

Mark the project-defined format and internal adapter as `IMPLEMENTED / PRE_ACCEPTANCE`. Keep real customer AFSIM rebuild, actual customer module data, security policy, and final deployment as pending external integration—not pending file-format design.

- [ ] **Step 4: Run documentation consistency checks**

```bash
rg -n "等待甲方.*格式|甲方格式.*待提供|独立服务端|18/18" README.md docs scripts
git diff --check
./scripts/ai_guard.sh static
```

Expected: no stale format-blocker wording, no stale 18-test count, no whitespace errors, and static checks pass.

- [ ] **Step 5: Commit documentation and handoff**

```bash
git add README.md docs
git commit -m "docs: hand off customer AFSIM JSON interface"
```

---

## Final Verification Checklist

- [ ] `./scripts/validate_customer_interface.sh` passes all positive and negative fixtures.
- [ ] `./scripts/ai_guard.sh test` reports 19/19 passing tests.
- [ ] `./scripts/ai_guard.sh scenario four_network_overview` completes normally.
- [ ] WSF and Warlock plugin targets build without AFSIM core changes.
- [ ] Invalid JSON leaves the last valid resource snapshot or plan unchanged.
- [ ] `.nrm` and `.neh` legacy inputs still load.
- [ ] Customer JSON plan loads, validates, evaluates, and exports a result JSON.
- [ ] GUI clearly reports whether the active data came from AFSIM, JSON replay, or `.neh` replay.
- [ ] Documentation does not claim final customer acceptance or binary ABI portability.
