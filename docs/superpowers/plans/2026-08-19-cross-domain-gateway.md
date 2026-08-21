# Cross-Domain Gateway Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement explicit, policy-controlled single-hop and cascaded Link-11/Link-16/SATCOM/CDL gateway forwarding with twelve dual-homed simulation nodes, snapshot evidence, assessment support, and fixed acceptance coverage.

**Architecture:** A pure C++ policy/catalog layer validates directed capabilities and ordered route templates. An external AFSIM processor performs the only forwarding side effects while a simulation extension owns route configuration and read-only runtime projections; Warlock copies those value objects into snapshots, and assessment adds only explicitly authorized gateway transitions.

**Tech Stack:** C++14（AFSIM工程实际标准）, AFSIM 2.9 external plugin API, Qt/Warlock value-object UI, CMake/CTest, Bash fixed scenario guard.

**Spec:** `docs/superpowers/specs/2026-08-19-cross-domain-gateway-design.md`

## Global Constraints

- Do not modify AFSIM core source.
- `include/nrm` must not depend on Qt or AFSIM.
- Do not add external dependencies.
- Preserve existing public fields and customer JSON V1 compatibility.
- Only the gateway processor may send cross-domain messages.
- Route templates and every directed capability must both authorize a cascade.
- Missing or unreliable measurements remain invalid with fixed reason codes.
- Preserve all existing uncommitted tactical-selection changes.

---

### Task 1: Pure gateway contracts, catalog validation, and policy

**Files:**
- Create: `include/nrm/GatewayTypes.hpp`
- Create: `include/nrm/GatewayPolicyEngine.hpp`
- Create: `tests/GatewayPolicyEngineTest.cpp`
- Modify: `include/nrm/NetworkResourceTypes.hpp`
- Modify: `CMakeLists.txt`
- Modify: `scripts/ai_guard.sh`

**Interfaces:**
- Produces: expanded `GatewayResourceState`, `GatewayRouteTemplate`, `GatewayForwardingEvent`, `GatewayMessageContext`, `GatewayPolicyDecision`, and `GatewayPolicyEngine::Evaluate(...)`.
- Produces: route/capability validation usable by AFSIM runtime and assessment without AFSIM/Qt dependencies.

- [x] **Step 1: Write the failing pure C++ test**

Cover a valid two-hop route, wrong direction, unauthorized source/destination/type, wrong route index, repeated capability/platform, discontinuous networks, TTL exhaustion, Trace loop, and duplicate ID validation with literal expected reason codes.

- [x] **Step 2: Run the test and verify RED**

Run the standalone compile command for `tests/GatewayPolicyEngineTest.cpp`; expect failure because the gateway types and engine do not exist.

- [x] **Step 3: Implement the minimal contracts and engine**

Add value objects and deterministic set-membership/route-index checks. Extend `GatewayResourceState` compatibly and add `gatewayRoutes` to `ResourceSnapshot`.

- [x] **Step 4: Run the focused test and existing validator test**

Expect both to pass with no compiler warnings from project code.

### Task 2: Explicit gateway edges in assessment

**Files:**
- Modify: `include/nrm/ConstrainedPathSelector.hpp`
- Modify: `include/nrm/AssessmentTypes.hpp`
- Modify: `include/nrm/AssessmentEvaluator.hpp`
- Modify: `tests/AssessmentEvaluatorTest.cpp`

**Interfaces:**
- Consumes: Task 1 gateway capabilities and templates.
- Produces: assessment paths containing only complete authorized gateway-template sequences and result evidence in `gatewayRouteIds/gatewayCapabilityIds`.

- [x] **Step 1: Add failing assessment cases**

Build a literal Link-11 → SATCOM → CDL fixture. Assert an authorized two-gateway route succeeds with accumulated delay and bottleneck rate, while the same local capabilities without a complete template do not create a path.

- [x] **Step 2: Run the focused test and verify RED**

Expect the authorized case to report no path because gateway transitions are not yet added.

- [x] **Step 3: Add template-constrained gateway transitions**

Add gateway edges with stable edge IDs, explicit source/destination/business filtering, and route-template state in the bounded path search. Do not let candidate-link generation cross network identities.

- [x] **Step 4: Run evaluator and path-selector tests**

Expect all old same-network behavior plus new cascaded behavior to pass.

### Task 3: AFSIM scenario/simulation extension and forwarding processor

**Files:**
- Create: `source/WsfNrmGatewayExtension.hpp`
- Create: `source/WsfNrmGatewayExtension.cpp`
- Create: `source/WsfNrmCrossDomainGatewayProcessor.hpp`
- Create: `source/WsfNrmCrossDomainGatewayProcessor.cpp`
- Modify: `source/WsfNetworkResourceManagerPlugin.cpp`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Consumes: Task 1 policy and contracts.
- Produces: top-level `nrm_gateway_route` input, processor type `WSF_NRM_CROSS_DOMAIN_GATEWAY_PROCESSOR`, standard `WsfMessage` Aux Data forwarding, bounded priority queue, and `WsfNrmGatewaySimulationExtension::SnapshotGateways/SnapshotRoutes`.

- [x] **Step 1: Add an integration configuration that cannot parse yet**

Add one processor type and one route to the fixed scenario only after Task 4 fixture preparation; the first mission initialization must fail on the unknown processor/command.

- [x] **Step 2: Implement scenario registration and parsers**

Register the processor in `AddedToScenario`, parse route blocks in the scenario extension, register the simulation extension, and reject invalid global route references in `PlatformsInitialized`.

- [x] **Step 3: Implement processor admission and forwarding**

Read standard Aux Data, derive ingress from actual destination address, evaluate policy, enqueue a cloned message, service the bounded non-preemptive priority queue, assign a fresh serial number, update route metadata, and send through the configured egress comm.

- [x] **Step 4: Build the WSF plugin**

Use a build configured against a clean temporary projection of the current repository if the shared AFSIM cache still points at another worktree. Expect target `wsf_network_resource_manager` to link successfully.

### Task 4: Snapshot collection, validation, reporting, and node details

**Files:**
- Modify: `warlock/source/NrmSimInterface.cpp`
- Modify: `warlock/source/NrmSnapshotReporter.cpp`
- Modify: `warlock/source/NrmDockWidget.cpp`
- Modify: `warlock/source/CMakeLists.txt`
- Modify: `tests/SnapshotReporterTest.cpp`

**Interfaces:**
- Consumes: Task 3 read-only simulation-extension projection.
- Produces: populated `snapshot.gateways/gatewayRoutes`, JSONL gateway evidence, and gateway capability/counter text in clicked platform details.

- [x] **Step 1: Add failing reporter assertions**

Assert directed authorization, counters, route IDs, capability sequence, final communication endpoint, and recent event reason codes are serialized. Malformed route references and discontinuities are covered by `GatewayPolicyEngineTest`.

- [x] **Step 2: Run focused tests and verify RED**

Expect missing JSON keys and missing semantic issues.

- [x] **Step 3: Implement collection and output**

Copy from the simulation extension during `BuildResourceState`, serialize bounded gateway values, and append gateway summaries to existing clicked-node detail text without introducing AFSIM pointers into GUI state.

- [x] **Step 4: Run focused tests and compile Warlock**

Expect reporter, validator, selection tests, MOC, and `NetworkResourceManager` link to pass.

### Task 5: Twelve-node four-domain scenario and fixed acceptance

**Files:**
- Modify: `test_mission/operational_strike_demo/communications.txt`
- Create: `test_mission/operational_strike_demo/gateways.txt`
- Create: `test_mission/cross_domain_gateway_smoke.txt`
- Modify: `test_mission/operational_strike_demo/events.txt`
- Modify: `test_mission/operational_strike_demo/README.md`
- Modify: `scripts/ai_guard.sh`

**Interfaces:**
- Consumes: Task 3 input grammar and Aux Data contract.
- Produces: 37-platform scenario, six pair combinations, direct and two-gateway cascade traffic; unauthorized cases remain deterministic pure-policy tests.

- [x] **Step 1: Add all physical gateways and explicit graph links**

Define twelve dual-homed platform types/instances and only the links required for their authorized traffic. Keep every gateway limited to its named pair.

- [x] **Step 2: Add capability and route configuration**

Configure forward/reverse direction-specific rules for all six pairs, 12 direct templates and two explicit two-gateway templates.

- [x] **Step 3: Add deterministic event workload and guard assertions**

Use `WsfMessage.SetAuxData` for route/final-target metadata. Require clean completion, 37 platform-add events, six direct forwards and two cascades with two forwards each.

- [x] **Step 4: Run the fixed scenario once**

Run `scripts/ai_guard.sh scenario operational_strike_demo` once. Do not tune parameters based on output.

### Task 6: Documentation, decision record, and final verification

**Files:**
- Create: `docs/superpowers/specs/2026-08-19-cross-domain-gateway-design.md`
- Modify: `README.md`
- Modify: `test_mission/operational_strike_demo/README.md`
- Modify: `docs/IMPLEMENTATION_STATUS.md`
- Modify: `docs/ai/DECISIONS.md`
- Modify: `docs/ai/CURRENT_MILESTONE.md`
- Modify: `docs/ai/SESSION_HANDOFF.md`

**Interfaces:**
- Consumes: verified behavior and exact command output from Tasks 1–5.
- Produces: reproducible usage/acceptance instructions and an explicit scoped exception to the former read-only forwarding boundary.

- [x] **Step 1: Update docs with exact configuration and run commands**

Document direct/cascade semantics, Aux Data fields, reason codes, 37-node topology, assessment behavior, and PRE_ACCEPTANCE limitations.

- [x] **Step 2: Record architecture decision**

Append a decision that only explicitly configured gateway processors may forward and that NRM evaluation/UI remain read-only.

- [x] **Step 3: Run complete verification**

Run `scripts/ai_guard.sh static`, focused new tests, the full fixed test set, both plugin builds, and the approved operational scenario once. Inspect `git diff --check` and verify no AFSIM core file changed.

- [x] **Step 4: Report evidence and remaining external blockers**

Report exact commands/counts. Do not claim `FINAL_ACCEPTANCE`; real protocol parameters, customer ABI, sample packages, and target environment remain external blockers.
