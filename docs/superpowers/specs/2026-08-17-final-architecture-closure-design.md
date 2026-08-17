# Final Architecture Closure Design

## Scope

This closure keeps NRM as an in-process AFSIM/Warlock module. JSON remains a
contract, replay, test, and acceptance format; no HTTP, socket, message-queue,
or service-process layer is introduced. Existing Assessment, Capability, Plan,
and Demand algorithms remain intact.

## Decisions

1. `ResourceSnapshotValidator` is a Qt/JSON-free semantic boundary shared by
   the JSON decoder and `CustomerNrmAdapter`. JSON validation owns syntax and
   shape; the C++ validator owns network/member/link/route/gateway relations and
   numeric business ranges.
2. Assessment continues to use `AssessmentEvaluator` for path selection. When
   an environment context is valid, `ModelServiceFacade` reuses the existing
   capability/environment chain to apply only the documented three-state
   semantics. `INFORMATION_ONLY` and `ALREADY_INCLUDED` do not alter assessment
   feasibility; `CANDIDATE_ADJUSTMENT` conservatively reconciles adjusted
   capability metrics and hard blocks into the assessment result.
3. Snapshot authority is deliberately simple. AFSIM observations are the
   authoritative live topology/link base. A customer resource report is the
   resource base only until an AFSIM base is available. Customer navigation and
   environment are overlay domains and survive later AFSIM updates. The
   effective snapshot version is generated monotonically by `DataContainer`.
4. Existing schemas and runtime decoding are audited together. The validation
   script uses `PYTHON_BIN`, defaults to `python3`, and explains how to install
   `jsonschema` when unavailable.

## Failure Handling

Semantic validation returns stable issue codes and object paths without
throwing. Rejected adapter input does not mutate ingestion state or the
effective snapshot. Source arbitration never mutates the stored AFSIM base or
customer overlay in place; it publishes a newly assembled effective value.

## Verification

Focused tests cover semantic relations/ranges, all three environment modes,
source authority, navigation/environment persistence, stale customer resources,
and monotonic snapshot versions. Full CTest, WSF/Warlock builds, contract
validation, deployment checks, and the operational scenario remain mandatory.
