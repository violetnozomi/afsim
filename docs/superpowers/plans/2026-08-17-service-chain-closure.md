# Service Chain Closure Implementation Plan

**Goal:** Close the remaining verified service-chain gaps without inventing unsupported planning or transport behavior.

**Architecture:** Keep Qt/JSON at the Warlock boundary, domain objects in `nrm`, and every model calculation behind `ModelServiceFacade`. Add a compact customer resource-demand contract and preserve source compatibility for the existing codec name.

**Tech Stack:** C++17, Qt Core JSON, JSON Schema draft-07, CMake/CTest.

## Task 1: Assessment service facade

- Extend the facade test first with normal, rejected-context and injectable-port cases.
- Add assessment operation/response/request types and a default `AssessmentEvaluator` port.
- Route `DataContainer::EvaluateAssessment` through the facade.

## Task 2: Customer resource-demand contract

- Add failing codec tests for strict request parsing and deterministic response encoding.
- Add request/response schemas, valid/invalid examples and annotated JSONC.
- Decode into `ResourceDemandSet`, bind the active configuration version, replace the draft,
  execute matching, and encode the response in `DataContainer`.

## Task 3: Concrete V1 adapter identity

- Rename the concrete class to `CustomerJsonContractAdapter` and retain
  `CustomerJsonCodec` as a compatibility alias.
- Publish deterministic adapter/schema metadata and cover it in tests.

## Task 4: Documentation and verification

- Update architecture, interface, status, contract coverage, milestone and handoff documents.
- Run static checks, contract validation, focused/full tests, plugin build, deployment check and
  the 25-node operational scenario.

No commits are created by this plan unless the user separately authorizes a commit.
