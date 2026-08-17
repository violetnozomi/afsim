# In-process Customer Adapter Implementation Plan

**Goal:** Make the supported runtime boundary a direct C++ module API and keep JSON as an optional tooling contract.

**Architecture:** Add a Qt-free `CustomerNrmAdapter` over a small host port. The adapter owns customer message lifecycle and domain merge rules; `DataContainer` implements the host and delegates model work to existing repositories and `ModelServiceFacade`.

## Task 1: Adapter contract and tests

- Add a failing pure C++ test for resource merge, navigation upsert, environment retention,
  duplicate/stale handling and direct assessment/plan/demand calls.
- Define value-only request/result types and the host port.

## Task 2: Runtime implementation

- Implement bounded lifecycle and immutable-style state updates in `CustomerNrmAdapter`.
- Add a public adapter accessor to `DataContainer` and implement the host callbacks.
- Route JSON-decoded messages through the same C++ adapter where safe.

## Task 3: JSON role correction

- Restore the concrete class name `CustomerJsonCodec`.
- Keep a temporary compatibility alias for the short-lived adapter name.
- Preserve all 13 schemas and examples as tooling/acceptance contracts.

## Task 4: Documentation and verification

- Remove HTTP/TCP/MQ from the required architecture and describe them only as out-of-scope.
- Update architecture, usage, interface boundary, milestone and handoff documentation.
- Run static, contract, full tests, both plugin builds, deployment check and the 25-node scenario.

No Git commit is created without separate user authorization.
