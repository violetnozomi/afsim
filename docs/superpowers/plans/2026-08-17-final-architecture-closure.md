# Final Architecture Closure Implementation Plan

> **For agentic workers:** Execute inline with test-driven development and verify every gate before committing.

**Goal:** Close the remaining semantic-validation, environment-consistency,
source-authority, tooling, and Git integration gaps.

**Architecture:** Add one pure C++ resource validator, reconcile assessment
through the existing capability/environment service, and make `DataContainer`
assemble an effective snapshot from an AFSIM base plus customer overlays.

**Tech Stack:** C++17, Qt Core JSON, CMake/CTest, Bash, Python 3/jsonschema.

## Global Constraints

- Do not modify AFSIM core sources.
- Do not introduce network transports or external service processes.
- Preserve existing public APIs where a compatible overload/default is possible.
- Write and observe a failing focused test before each production behavior change.
- Preserve all prior contract, build, deployment, and scenario checks.

### Task 1: Shared resource semantic validator

**Files:**
- Create: `include/nrm/ResourceSnapshotValidator.hpp`
- Create: `tests/ResourceSnapshotValidatorTest.cpp`
- Modify: `include/nrm/CustomerNrmAdapter.hpp`
- Modify: `warlock/source/NrmCustomerJsonCodec.cpp`
- Modify: `CMakeLists.txt`

- [ ] Add failing tests for valid resources and invalid network, member, link,
      metric, protocol, route, and gateway relationships.
- [ ] Implement stable issue codes/paths in a pure C++ validator.
- [ ] Reuse it from both JSON decoding and the same-process adapter.
- [ ] Run the focused validator, codec, and adapter tests.

### Task 2: Environment-aware assessment

**Files:**
- Modify: `include/nrm/ModelServiceFacade.hpp`
- Modify: `warlock/source/NrmDataContainer.cpp`
- Modify: `tests/ModelServiceFacadeTest.cpp`
- Modify: `tests/CustomerDataContainerTest.cpp`

- [ ] Add failing tests for information-only, already-included, candidate
      adjustment, and hard-blocked assessment behavior.
- [ ] Convert the assessment task to the existing capability request and
      conservatively reconcile only applicable environment effects.
- [ ] Pass the same effective environment from UI and customer paths.
- [ ] Run focused facade and data-container tests.

### Task 3: AFSIM/customer source authority

**Files:**
- Create: `include/nrm/EffectiveSnapshotAssembler.hpp`
- Create: `tests/EffectiveSnapshotAssemblerTest.cpp`
- Modify: `warlock/source/NrmDataContainer.hpp`
- Modify: `warlock/source/NrmDataContainer.cpp`
- Modify: `tests/CustomerDataContainerTest.cpp`
- Modify: `CMakeLists.txt`

- [ ] Add failing tests proving AFSIM topology authority, customer fallback,
      customer navigation/environment persistence, and monotonic versions.
- [ ] Store source bases separately and publish an immutable effective value.
- [ ] Route JSON resource/navigation/environment updates through customer
      publication semantics without last-writer-wins replacement.
- [ ] Run focused assembler and data-container tests.

### Task 4: Runtime validation tooling and drift audit

**Files:**
- Modify: `scripts/validate_customer_interface.sh`
- Modify only proven schema/runtime drift in `schemas/customer/v1/`.

- [ ] Add a shell regression for `PYTHON_BIN` override and missing jsonschema.
- [ ] Replace absolute Python paths and emit actionable dependency errors.
- [ ] Run contract positive/negative validation and codec regression tests.

### Task 5: Documentation, verification, and Git closure

**Files:**
- Modify: `docs/ARCHITECTURE.md`
- Modify: `docs/VALIDATION.md`
- Modify: `docs/ai/CURRENT_MILESTONE.md`
- Modify: `docs/ai/SESSION_HANDOFF.md`

- [ ] Document authority, validator boundary, environment semantics, and tests.
- [ ] Run static, contract, all NRM tests, both plugin builds, deployment check,
      and the operational scenario.
- [ ] Commit the optimized worktree, fast-forward the clean formal branch,
      rerun the full gate there, and confirm both worktrees are clean.
