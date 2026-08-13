# Concurrent Task Assessment Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add deterministic joint assessment of simultaneous communication tasks with shared-link bandwidth reservation.

**Architecture:** A pure C++ service wraps the existing `AssessmentEvaluator`, copies one immutable input snapshot, and evaluates ordered tasks against a mutable working copy. Successful earlier tasks reserve bandwidth on their selected directed links; later tasks expose contention without changing live AFSIM state.

**Tech Stack:** C++14, existing NRM value objects and evaluator, CMake tests, Warlock Qt display.

## Global Constraints

- Do not modify AFSIM core source or live communication state.
- Input order is the deterministic priority order in v1.
- Reserve bandwidth only; protocol-specific slot/beam/channel contention is outside v1.
- Use tests before production implementation and preserve all existing interfaces.

---

### Task 1: Pure C++ joint evaluator

**Files:** Create `include/nrm/ConcurrentTaskAssessment.hpp`; create `tests/ConcurrentTaskAssessmentTest.cpp`; modify `CMakeLists.txt`.

- [ ] Write tests for a shared 1000 bit/s link where the first 600 bit/s task succeeds and the second 600 bit/s task fails from contention.
- [ ] Add tests for disjoint routes, failed tasks not reserving capacity, unchanged input snapshots, duplicate IDs, and deterministic reruns.
- [ ] Build once and confirm failure because the service is absent.
- [ ] Implement ordered evaluation, directed-link reservation, conflict attribution and batch counters.
- [ ] Run the focused test and existing evaluator tests.
- [ ] Commit the pure C++ service.

### Task 2: Planning integration and reporting

**Files:** Modify `warlock/source/NrmDataContainer.*`, `warlock/source/NrmDockWidget.*`, `warlock/source/NrmSnapshotReporter.*`, and reporter tests.

- [ ] Write failing tests for stable JSONL serialization of batch and per-task contention results.
- [ ] Map `NetworkPlanDocument::demands` to ordered `AssessmentTask` values.
- [ ] Store and expose the last concurrent result from `DataContainer`.
- [ ] Display concurrent summary and conflict reason in the existing resource-planning page.
- [ ] Write `concurrent_assessment_results.jsonl` asynchronously with other result files.
- [ ] Build plugin and run focused tests.
- [ ] Commit integration.

### Task 3: Contract, fixtures, verification, and documentation

**Files:** Modify customer planning schemas/examples, fixed test registration, validation and operator docs.

- [ ] Keep array order as documented priority and add concurrent result fields without requiring new customer files.
- [ ] Add the new test to fixed gates and update test counts.
- [ ] Run static, contract, all tests, and the four-network scenario.
- [ ] Record exact evidence and operating steps.
- [ ] Commit verified documentation.
