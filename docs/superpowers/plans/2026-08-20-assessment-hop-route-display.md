# Assessment Hop-by-Hop Route Display Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make Warlock display every directed edge selected by the existing assessment algorithm, including per-hop network, candidate status, direction, numbering, and zero-distance gateway transitions.

**Architecture:** `AssessmentEvaluator` projects the already-selected `ConstrainedPath::edges` into stable `AssessmentRouteHop` value objects. Reporter and Warlock consume those values without recomputing routing; the existing platform and endpoint route fields remain compatible. Tactical rendering uses directed numbered segments for normal edges and an explicit ring marker for gateway transitions.

**Tech Stack:** C++14-compatible public value objects, header-only assessment engine, Qt5/QPainter Warlock plugin, existing assertion-based C++ tests and CMake targets.

**Spec:** `docs/superpowers/specs/2026-08-20-assessment-hop-route-display-design.md`

## Global Constraints

- Do not modify AFSIM core source.
- Public `include/nrm` code must not depend on Qt or AFSIM.
- Do not change path selection, constraints, metrics, candidate generation, gateway authorization, or primary/backup ranking.
- Preserve `primaryRoute`, `primaryEndpointRoute`, `backupRoute`, `primaryRouteUsesCandidate`, and `backupRouteUsesCandidate` semantics.
- Keep the customer V1 assessment JSON Schema and `CustomerJsonCodec::EncodeAssessmentResponse` unchanged.
- Use no new external dependency.
- Write and run a failing test before each production behavior change.
- Do not commit, tag, or push without separate user authorization.

---

### Task 1: Add stable per-hop assessment evidence

**Files:**
- Modify: `tests/AssessmentEvaluatorTest.cpp`
- Modify: `include/nrm/AssessmentTypes.hpp`
- Modify: `include/nrm/AssessmentEvaluator.hpp`

**Interfaces:**
- Produces: `AssessmentRouteHopKind`, `ToString(AssessmentRouteHopKind)`, `AssessmentRouteHop`, `AssessmentResult::primaryRouteHops`, and `AssessmentResult::backupRouteHops`.
- Consumes: existing ordered `ConstrainedPath::edges` and the evaluator `EndpointMap`.

- [x] **Step 1: Write the failing ordinary-route and backup-route assertions**

Extend the existing successful two-hop fixture with stable network IDs and assert literal hop evidence:

```cpp
assert(passed.primaryRouteHops.size() == 2);
const nrm::AssessmentRouteHop& firstHop = passed.primaryRouteHops[0];
assert(firstHop.hopIndex == 1);
assert(firstHop.kind == nrm::AssessmentRouteHopKind::cCURRENT_LINK);
assert(firstHop.sourceEndpointId == "A");
assert(firstHop.destinationEndpointId == "B");
assert(firstHop.sourcePlatform == "source");
assert(firstHop.destinationPlatform == "relay");
assert(firstHop.sourceNetworkId == "nrm_link16_test");
assert(firstHop.destinationNetworkId == "nrm_link16_test");
assert(firstHop.sourceNetworkType == nrm::NetworkType::cLINK16);
assert(firstHop.destinationNetworkType == nrm::NetworkType::cLINK16);
assert(!firstHop.candidate);
assert(!firstHop.gateway);

assert(currentBackup.backupRouteHops.size() == 2);
assert(currentBackup.backupRouteHops[0].hopIndex == 1);
assert(currentBackup.backupRouteHops[1].hopIndex == 2);
assert(currentBackup.backupRouteHops[0].destinationPlatform == "backup_relay");
```

- [x] **Step 2: Run the evaluator target and verify RED**

Run the current-source evaluator compile command used by the repository, or build
`nrm_assessment_evaluator_test` in the clean current-source CMake projection.

Expected: compile failure because `primaryRouteHops`, `backupRouteHops`, and
`AssessmentRouteHopKind` do not exist.

- [x] **Step 3: Add the minimal public types and projection helper**

Add the exact types from the design spec to `AssessmentTypes.hpp`, including a stable `ToString` mapping:

```cpp
inline const char* ToString(AssessmentRouteHopKind aKind)
{
   switch (aKind)
   {
   case AssessmentRouteHopKind::cCURRENT_LINK: return "CURRENT_LINK";
   case AssessmentRouteHopKind::cCANDIDATE_LINK: return "CANDIDATE_LINK";
   case AssessmentRouteHopKind::cGATEWAY_TRANSITION: return "GATEWAY_TRANSITION";
   }
   return "CURRENT_LINK";
}
```

Add one evaluator helper with this signature:

```cpp
static void PopulateRouteHops(const ConstrainedPath& aPath,
                              const EndpointMap& aEndpoints,
                              std::vector<AssessmentRouteHop>& aHops);
```

For each edge, copy source/destination IDs and platforms, look up both endpoints for network identity,
copy gateway evidence, and derive `kind` from `gateway` before `candidate`. Call the helper for both selected
primary and selected backup paths.

- [x] **Step 4: Run the evaluator test and verify GREEN**

Expected: ordinary primary and backup route assertions pass without changing existing route or metric assertions.

- [x] **Step 5: Add the failing per-hop candidate assertion**

For the existing candidate case, assert that its actual edge is `cCANDIDATE_LINK`, `candidate == true`, and
`gateway == false`. For a mixed current/candidate fixture, assert only the generated edge is candidate while the
current edge remains `cCURRENT_LINK`.

- [x] **Step 6: Run and verify RED, then implement only missing candidate classification**

Expected RED: wrong/default hop kind or candidate flag. Minimal implementation must derive the classification
from the corresponding `ConstrainedEdge`, not the whole-route compatibility flag.

- [x] **Step 7: Add the failing gateway transition assertions**

Extend the existing `route-l11-cdl-via-satcom` fixture and assert each gateway edge produces an independent hop:

```cpp
assert(gatewayAllowed.primaryRouteHops.size() == 5);
const nrm::AssessmentRouteHop& transition = gatewayAllowed.primaryRouteHops[1];
assert(transition.kind == nrm::AssessmentRouteHopKind::cGATEWAY_TRANSITION);
assert(transition.sourcePlatform == transition.destinationPlatform);
assert(transition.sourceNetworkType == nrm::NetworkType::cLINK11);
assert(transition.destinationNetworkType == nrm::NetworkType::cSATCOM);
assert(transition.gatewayRouteId == "route-l11-cdl-via-satcom");
assert(transition.gatewayCapabilityId == "gw-l11-satcom");
```

Use the fixture’s literal route and capability IDs rather than deriving expected values from production helpers.

- [x] **Step 8: Run and verify RED, then copy gateway metadata in the projection**

Expected RED: missing gateway classification, network direction, or gateway IDs. Implement the minimal copy and
rerun until the evaluator target passes.

- [x] **Step 9: Mutation-check Task 1**

Confirm the test would fail if hop order were reversed, hop numbers started at zero, all hops inherited the
whole-route candidate flag, or gateway source/destination network types were swapped.

---

### Task 2: Add testable Chinese per-hop formatting

**Files:**
- Modify: `tests/UiTextTest.cpp`
- Modify: `warlock/source/NrmUiText.hpp`
- Modify: `warlock/source/NrmUiText.cpp`
- Modify: `warlock/source/NrmDockWidget.cpp`

**Interfaces:**
- Consumes: `nrm::AssessmentRouteHop`.
- Produces: `UiText::FormatAssessmentRouteHop(const nrm::AssessmentRouteHop&) -> std::string`.

- [x] **Step 1: Write failing formatter tests**

Create literal current, candidate, and gateway hops and assert complete user-facing strings:

```cpp
CHECK(WkNrm::UiText::FormatAssessmentRouteHop(current) ==
      "H1 source/A → relay/B [Link-16，当前链路]");
CHECK(WkNrm::UiText::FormatAssessmentRouteHop(candidate) ==
      "H2 relay/B → destination/C [Link-16，候选链路]");
CHECK(WkNrm::UiText::FormatAssessmentRouteHop(gateway) ==
      "H3 gateway/in → gateway/out [Link-11 → 卫通，网关转换，gw-l11-satcom]");
```

- [x] **Step 2: Run `nrm_ui_text_test` and verify RED**

Expected: compile failure because `FormatAssessmentRouteHop` does not exist.

- [x] **Step 3: Implement the minimal formatter**

Include `nrm/AssessmentTypes.hpp` in `NrmUiText.hpp`. Format endpoint labels as `platform/endpoint`, translate
network types with existing translations, and choose the suffix solely from `AssessmentRouteHopKind`.

- [x] **Step 4: Run `nrm_ui_text_test` and verify GREEN**

Expected: all existing translations and the three hop strings pass.

- [x] **Step 5: Append structured hop lists to task assessment text**

In `DockWidget::EvaluateTask`, append `主路由逐跳` and, when present, `备选路由逐跳`. Iterate the stored
hop vectors and call the tested formatter; do not reconstruct hops from platform arrays.

- [x] **Step 6: Compile `NrmDockWidget.cpp` with the current Warlock compile command**

Expected: clean compile with no new project warning.

---

### Task 3: Serialize per-hop internal audit evidence

**Files:**
- Modify: `tests/SnapshotReporterTest.cpp`
- Modify: `warlock/source/NrmSnapshotReporter.cpp`

**Interfaces:**
- Consumes: both assessment hop vectors.
- Produces: internal JSONL arrays named `primary_route_hops` and `backup_route_hops`.

- [x] **Step 1: Write failing Reporter assertions**

Add one current primary hop and one gateway backup hop to the reporter fixture. Assert literal JSON fragments for:

```text
hopIndex, kind, sourceEndpointId, destinationEndpointId,
sourcePlatform, destinationPlatform, sourceNetworkId, destinationNetworkId,
sourceNetworkType, destinationNetworkType, candidate, gateway,
gatewayRouteId, gatewayCapabilityId
```

Also parse every emitted assessment JSONL line with the existing JSON validation mechanism used by the test.

- [x] **Step 2: Run `nrm_snapshot_reporter_test` and verify RED**

Expected: missing `primary_route_hops` / `backup_route_hops` assertion failure.

- [x] **Step 3: Implement a focused hop-array writer**

Add:

```cpp
void WriteAssessmentRouteHops(std::ostream& aOutput,
                              const std::vector<nrm::AssessmentRouteHop>& aHops);
```

Escape every string with the existing `EscapeJson`, emit stable enum/network strings through `nrm::ToString`,
and call it from the existing assessment writer without changing the frozen customer JSON encoder.

- [x] **Step 4: Run Reporter test and verify GREEN**

Expected: JSON fragments and all existing Reporter assertions pass.

---

### Task 4: Draw numbered directed hops in Warlock

**Files:**
- Modify: `warlock/source/NrmTacticalView.cpp`
- Test indirectly: `tests/AssessmentEvaluatorTest.cpp`, `tests/UiTextTest.cpp`

**Interfaces:**
- Consumes: `AssessmentResult::primaryRouteHops`, `backupRouteHops`, and current snapshot platform coordinates.
- Produces: directed numbered normal/candidate segments and explicit gateway-transition markers.

- [x] **Step 1: Replace platform-array route drawing with hop-vector drawing**

Implement local focused helpers/lambdas that:

```cpp
drawArrow(QPainter&, const QPointF& source, const QPointF& destination,
          const QPen& pen);
drawGatewayTransition(QPainter&, const QPointF& gatewayPoint,
                      const nrm::AssessmentRouteHop& hop,
                      const QColor& color, bool primary);
drawRouteHops(const std::vector<nrm::AssessmentRouteHop>& hops,
              const QColor& color, bool primary);
```

For a normal hop, require both platform coordinates, draw one directed line and arrowhead, then place an `Hn`
and network label near its midpoint. Apply dashed style only when that hop’s `candidate` is true.

- [x] **Step 2: Render gateway conversions without zero-length lines**

When `hop.gateway` is true, require the gateway platform coordinate and draw an arc/ring beside the node with
the hop number and `sourceNetworkType → destinationNetworkType`. Never connect the preceding node directly to
the following node as a substitute.

- [x] **Step 3: Preserve primary/backup visual hierarchy and endpoints**

Draw backup hops first with cyan thinner pens and primary hops second with yellow thicker pens. Use the first
hop source and last hop destination for source/destination rings. Add legend labels for current hop, candidate
hop, gateway conversion, primary, and backup.

- [x] **Step 4: Compile and link the Warlock plugin**

Compile all changed Warlock translation units/MOC and link `NetworkResourceManager` against the current WSF
plugin. Expected: successful dynamic library link and explicit dependency on
`libwsf_network_resource_manager`.

---

### Task 5: Full verification and handoff

**Files:**
- Modify: `docs/VALIDATION.md`
- Modify: `docs/ai/CURRENT_MILESTONE.md`
- Modify: `docs/ai/SESSION_HANDOFF.md`
- Modify if needed for user operation: `docs/当前运行与内部验收流程.md`

**Interfaces:**
- Produces: current evidence and explicit manual-GUI boundary.

- [x] **Step 1: Run affected tests with strict warnings**

Run evaluator, UI text, Reporter, constrained-path, communication-capability, concurrent-assessment, and
customer codec tests with the repository’s strict warning flags. Expected: all pass with no project warning.

- [x] **Step 2: Run fixed repository gates**

```bash
git diff --check
./scripts/ai_guard.sh static
PYTHON_BIN=/usr/bin/python3 ./scripts/ai_guard.sh contract
```

Expected: version consistency, static checks, 13 valid contracts accepted, and 6 invalid contracts rejected.

- [x] **Step 3: Run all current-source tests**

Build `nrm_tests` from a clean current-source projection and run `ctest -R '^nrm_' --output-on-failure`.
Expected: all registered NRM tests pass; do not use stale binaries from the shared old-worktree cache.

- [x] **Step 4: Run one fixed gateway scenario**

```bash
./scripts/ai_guard.sh scenario cross_domain_gateway_smoke
```

Expected: two gateway forwards, final destination receipt, and `Simulation complete`.

- [x] **Step 5: Update evidence documents**

Record the new hop contract, algorithm-preserving scope, exact test counts, plugin build evidence, and whether
manual Warlock visual inspection was run. Keep status at `IMPLEMENTED / PRE_ACCEPTANCE` unless the fixed GUI
inspection is actually performed.

- [x] **Step 6: Final diff audit**

Confirm changes are limited to the approved hop-display contract, evaluator projection, Reporter, Warlock UI,
tests, design/plan, and evidence documents. Confirm AFSIM core modification count remains zero and no output,
logs, screenshots, `.orig`, `.rej`, or build artifacts entered the change set.
