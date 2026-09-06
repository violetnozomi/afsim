# Contract Acceptance Demo UI Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a Chinese, video-friendly Warlock acceptance page that launches the fixed integrated scenario and guides nontechnical users through existing contract features.

**Architecture:** A focused `NrmAcceptanceDemoPanel` owns only presentation and emits argument-free intent signals. `NrmDockWidget` maps those signals to existing tabs and service methods, and reuses the existing allowlisted plan-switch path for the single integrated scenario. No assessment, capability, planning, navigation, environment, or gateway algorithm is duplicated.

**Tech Stack:** C++17, Qt 5 Widgets/signals, AFSIM 2.9 Warlock extension, CMake, Bash regression gates.

**Spec:** `docs/superpowers/specs/2026-08-25-contract-acceptance-demo-ui-design.md`

## Global Constraints

- Do not modify AFSIM core sources.
- Do not add arbitrary command, scenario, or plan input.
- Do not change the read-only behavior of network evaluation and planning.
- Do not delete or weaken any existing test.
- Keep all new user-visible copy Chinese except fixed protocol names and reason codes.
- Keep the page usable in a narrow VNC dock through a scroll area and zero fixed minimum width/height.
- Do not commit unless the user separately authorizes a Git commit.

---

### Task 1: Acceptance Demo Panel Contract

**Files:**
- Create: `tests/AcceptanceDemoPanelTest.cpp`
- Create: `warlock/source/NrmAcceptanceDemoPanel.hpp`
- Create: `warlock/source/NrmAcceptanceDemoPanel.cpp`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Produces: `WkNrm::AcceptanceDemoPanel : public QWidget`.
- Produces signals: `StartScenarioRequested()`, `ShowOverviewRequested()`,
  `RunAssessmentRequested()`, `RunCapabilityRequested()`, `ShowPlanRequested()`,
  `ShowGatewayRequested()`, `ShowNavigationRequested()`, `ShowEnvironmentRequested()`,
  and `ShowPreacceptanceRequested()`.
- Produces methods:
  `SetResourceSummary(int,int,int,int)`, `SetNavigationSummary(int)`,
  `SetEnvironmentSummary(bool,bool,bool)`, `SetPreacceptanceSummary(const QString&,int,int,int,int)`,
  and `SetLaunchResult(bool,const QString&)`.

- [x] **Step 1: Write the failing widget behavior test**

Instantiate the real panel under `QApplication`, click buttons found by stable object names, and assert each
argument-free signal fires exactly once. Assert the rendered page contains the contract step titles and that
summary methods render literal Chinese status text. The production change caught is a missing/wrong action
mapping or a page that cannot present evidence in the narrow dock.

- [x] **Step 2: Register and run the new test to verify RED**

Run:

```bash
cmake --build "$AFSIM_BUILD" --target nrm_acceptance_demo_panel_test -j4
```

Expected: compilation fails because `NrmAcceptanceDemoPanel.hpp` does not exist.

- [x] **Step 3: Implement the minimal panel**

Create a `QScrollArea` with compact title, summary cards and nine fixed step buttons. Buttons have no path or
command inputs. Use Qt parent ownership for every widget and layout; do not introduce manual deletion.

- [x] **Step 4: Run the panel test to verify GREEN**

Run:

```bash
QT_QPA_PLATFORM=offscreen "$AFSIM_BUILD/nrm_acceptance_demo_panel_test"
```

Expected: exit code 0.

---

### Task 2: DockWidget Integration and Fixed Examples

**Files:**
- Modify: `warlock/source/NrmDockWidget.hpp`
- Modify: `warlock/source/NrmDockWidget.cpp`
- Modify: `warlock/source/CMakeLists.txt`
- Modify: `tests/AcceptanceDemoPanelTest.cpp`

**Interfaces:**
- Consumes: all `AcceptanceDemoPanel` signals and summary setters from Task 1.
- Produces private methods: `StartAcceptanceScenario()`, `RunAcceptanceAssessment()`,
  `RunAcceptanceCapability()`, `ShowAcceptanceTab(QWidget*)`, and
  `SetComboValue(QComboBox*, const QString&)`.
- Produces fixed values: source `airborne_relay`, destination `gw_l11_l16_reverse`, network `LINK16`,
  assessment bandwidth `0`, maximum delay `1000`, minimum PDR `0`.

- [x] **Step 1: Extend the failing test for fixed presentation intents**

Click each step button and assert it emits the intended signal only. Verify the start signal carries no user
path and the launch-result method displays a Chinese error. This catches accidental arbitrary-path exposure
and wrong step routing.

- [x] **Step 2: Run RED before DockWidget wiring**

Run the panel test and expect failure for the newly asserted object/action not yet implemented.

- [x] **Step 3: Add the panel to the main tabs and wire existing actions**

Insert “合同验收演示” before the normal data tabs. Tab-navigation actions select the existing widget by
pointer. Assessment and capability actions select the fixed platform/network values in existing combo boxes,
set the frozen QoS fields, switch to the target page and call the existing evaluation method once.

- [x] **Step 4: Reuse the fixed plan-switch flow**

Resolve the source root only from `NRM_SOURCE`, construct the two fixed repository paths, verify both files,
load the plan through `DataContainer`, and start the same `systemd-run --user --collect` call already used by
`LoadNetworkPlan()`. No UI text field contributes to command arguments. On failure, keep the current process
and display a Chinese failure message.

- [x] **Step 5: Refresh visible evidence**

On snapshot changes, provide network, platform, link, gateway, navigation and environment counts to the panel.
On preacceptance status changes, provide status/test/scenario totals without changing the existing read-only page.

- [x] **Step 6: Build the Warlock plugin and rerun the panel test**

Run:

```bash
cmake --build "$AFSIM_BUILD" --target NetworkResourceManager nrm_acceptance_demo_panel_test -j4
QT_QPA_PLATFORM=offscreen "$AFSIM_BUILD/nrm_acceptance_demo_panel_test"
```

Expected: both commands pass.

---

### Task 3: Test-Gate and Version Integration

**Files:**
- Modify: `scripts/ai_guard.sh`
- Modify: `scripts/run_preacceptance.sh`
- Modify: `CMakeLists.txt`
- Modify: `VERSION`
- Modify: `include/nrm/Version.hpp`
- Modify: `CHANGELOG.md`

**Interfaces:**
- Consumes: `nrm_acceptance_demo_panel_test` from Task 1.
- Produces: version `0.12.1` and a fixed-test count of 41.

- [x] **Step 1: Add the test executable to the fixed gate**

Append `nrm_acceptance_demo_panel_test` to `TEST_NAMES` and `NRM_TEST_TARGETS`; change
`FIXED_TEST_COUNT` from 40 to 41.

- [x] **Step 2: Synchronize version files**

Set both version files to `0.12.1` and add a concise changelog entry describing the acceptance demo page.

- [x] **Step 3: Run focused and static verification**

Run:

```bash
scripts/ai_guard.sh static
QT_QPA_PLATFORM=offscreen "$AFSIM_BUILD/nrm_acceptance_demo_panel_test"
```

Expected: both pass.

---

### Task 4: User Workflow and Acceptance Evidence

**Files:**
- Modify: `docs/功能使用手册.md`
- Modify: `docs/运行与可视化入口.md`
- Modify: `docs/合同功能测试明细与执行记录.md`
- Modify: `docs/VALIDATION.md`
- Modify: `docs/ai/CURRENT_MILESTONE.md`
- Modify: `docs/ai/SESSION_HANDOFF.md`

**Interfaces:**
- Consumes: final button names, fixed values and version from Tasks 1-3.
- Produces: a single front-end-only video workflow and a truthful validation record.

- [x] **Step 1: Document the front-end recording flow**

Document: open “合同验收演示”, start the integrated scenario, follow the cards in order, explain 25 business
nodes plus 12 gateways, and treat `REJECTED` planning as successful constraint detection only when reasons,
margins and Chinese recommendations are visible.

- [x] **Step 2: Record validation truthfully**

Record automated panel/build evidence as completed. Keep VNC visual confirmation as not run until the real
Warlock page is restarted and observed.

- [x] **Step 3: Run required repository verification**

Run exactly once:

```bash
scripts/ai_guard.sh static
scripts/ai_guard.sh test
git diff --check
```

Expected: static passes; 41/41 fixed tests pass; both plugins build; diff check is clean.

- [x] **Step 4: Inspect the final diff**

Confirm changes are limited to this plan and preserve all pre-existing dirty worktree modifications. Do not
commit, tag, push, delete, restore or reset any user-owned file.
