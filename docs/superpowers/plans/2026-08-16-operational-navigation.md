# Operational Navigation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Populate the 25-platform cooperative scenario with real AFSIM navigation-error state and explain empty navigation snapshots in the Chinese Warlock UI.

**Architecture:** Configure `WsfNavigationErrors` on the three existing airborne base platform types so the current collector observes native AFSIM state without new adapters. Keep the UI fallback as a tested text helper and render one explanatory table row only when the snapshot has no platforms.

**Tech Stack:** AFSIM 2.9 scenario grammar, C++17, Qt 5.12, Bash regression checks, existing CMake/ai_guard workflow.

## Global Constraints

- Do not modify AFSIM core source or its framework interfaces.
- Do not synthesize navigation samples when `WsfNavigationErrors` is absent.
- Only the 11 moving airborne platforms receive navigation-error models.
- Navigation parameters are deterministic PRE_ACCEPTANCE demonstration values, not equipment specifications.
- Preserve the 25-platform, four-network, no-enemy, no-weapon scenario baseline.

---

### Task 1: Operational scenario navigation configuration

**Files:**
- Modify: `tests/RemotePlanSwitchTest.sh`
- Modify: `test_mission/operational_strike_demo/platform_types.txt`
- Modify: `scripts/validate_operational_strike_demo.sh`

**Interfaces:**
- Consumes: AFSIM `navigation_errors` platform command and the existing `WsfNavigationErrors` collector.
- Produces: 7 `GPS_ACTIVE`, 3 `GPS_DEGRADED`, and 1 `INS` platform samples in the integrated scenario.

- [ ] **Step 1: Add failing configuration assertions**

Extend `RemotePlanSwitchTest.sh` to require exactly three `navigation_errors` blocks and require
`gps_status 1`, `gps_status 2`, `gps_status -1`, and all three INS error axes.

- [ ] **Step 2: Run the focused test and verify RED**

Run: `bash tests/RemotePlanSwitchTest.sh`

Expected: FAIL because `platform_types.txt` contains no navigation configuration.

- [ ] **Step 3: Add minimal AFSIM navigation blocks**

Add deterministic GPS1 parameters to `NRM_OP_LINK16_AIR`, degraded GPS2 parameters to
`NRM_OP_ISR_UAV`, and deterministic three-axis INS drift to `NRM_OP_LINK11_AIR`.

- [ ] **Step 4: Verify configuration and runtime GREEN**

Run:

```bash
bash tests/RemotePlanSwitchTest.sh
./scripts/validate_operational_strike_demo.sh
```

Expected: configuration assertions pass; AFSIM completes all phases with 25 cooperative platforms and no weapon events.

- [ ] **Step 5: Commit**

```bash
git add tests/RemotePlanSwitchTest.sh test_mission/operational_strike_demo/platform_types.txt scripts/validate_operational_strike_demo.sh
git commit -m "feat: add navigation to cooperative air platforms"
```

### Task 2: Navigation empty-state explanation

**Files:**
- Modify: `tests/UiTextTest.cpp`
- Modify: `warlock/source/NrmUiText.hpp`
- Modify: `warlock/source/NrmUiText.cpp`
- Modify: `warlock/source/NrmDockWidget.cpp`

**Interfaces:**
- Produces: `WkNrm::UiText::NavigationEmptyState()` returning the fixed UTF-8 Chinese explanation.
- Consumes: `NavigationSnapshot::platforms` in `DockWidget::RefreshSnapshot`.

- [ ] **Step 1: Add the failing UI text test**

Assert that `NavigationEmptyState()` equals
`当前场景未配置导航误差模型，或尚未收到甲方导航数据。`.

- [ ] **Step 2: Run focused test and verify RED**

Run the configured `nrm_ui_text_test` target; expect a compile failure because the function is absent.

- [ ] **Step 3: Implement the helper and empty row**

Return the fixed text from `NrmUiText.cpp`. When `navigation.platforms.empty()`, set one table row,
place the message in column 0 and `—` in columns 1 through 16; otherwise retain the existing loop.

- [ ] **Step 4: Run focused and full test suites**

Run:

```bash
./scripts/ai_guard.sh static
./scripts/ai_guard.sh test
```

Expected: 31/31 tests and both plugin builds pass.

- [ ] **Step 5: Commit**

```bash
git add tests/UiTextTest.cpp warlock/source/NrmUiText.hpp warlock/source/NrmUiText.cpp warlock/source/NrmDockWidget.cpp
git commit -m "fix: explain unavailable navigation data"
```

### Task 3: Integrated Warlock evidence and documentation

**Files:**
- Modify: `docs/功能使用手册.md`
- Modify: `docs/VALIDATION.md`
- Modify: `docs/ai/SESSION_HANDOFF.md`

**Interfaces:**
- Consumes: the rebuilt Warlock plugin and `operational_strike_demo/interactive.txt`.
- Produces: reproducible navigation count/mode evidence and operator guidance.

- [ ] **Step 1: Rebuild and restart the integrated scenario**

Run the plugin build, invoke `scripts/remote/run-operational-strike-warlock.sh`, and wait for a fresh
`resource_snapshots.jsonl`.

- [ ] **Step 2: Verify live snapshot evidence**

Use `jq` to assert 11 navigation platforms and the presence of `GPS_ACTIVE`, `GPS_DEGRADED`, and `INS`.

- [ ] **Step 3: Update documentation**

Document which 11 platforms expose navigation, why fixed platforms are omitted, how empty state is
presented, exact commands, results, and PRE_ACCEPTANCE limitations.

- [ ] **Step 4: Run final verification**

Run `git diff --check`, static checks, all 31 tests, the operational scenario validator, and live snapshot assertions.

- [ ] **Step 5: Commit**

```bash
git add docs/功能使用手册.md docs/VALIDATION.md docs/ai/SESSION_HANDOFF.md
git commit -m "docs: record integrated navigation evidence"
```

