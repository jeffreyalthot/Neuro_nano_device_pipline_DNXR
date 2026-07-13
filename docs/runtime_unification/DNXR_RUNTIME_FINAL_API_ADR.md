# ADR: Final Canonical BCI Runtime API and Transaction Contract

Date: 2026-07-13
Status: Accepted for Prompt 02 contract freeze

## Decision

The production API is `dnxr::canonical::CanonicalNeuralBciRuntime` with exactly five runtime-control methods: `openSession`, `step`, `requestHold`, `requestEmergencyStop`, and `closeSession`. The API is declared in `src/runtime/CanonicalBciRuntimeContract.h` and intentionally excludes rendering, headset, simulation, legacy runner, and adapter names.

The transaction contract is a typed phase chain. Each phase embeds the previous phase by value or a single-use commit token by move-only ownership, so call sites cannot submit an `EncodedModalities` object to transport, cannot construct a `PreparedCommit` through public aggregate fields, cannot copy/reuse a commit token, and cannot correlate an ACK to a different session/frame/commit/nonce without receiving a structured lifecycle error.

## Why `RuntimePipelineRunner` is not the final API

`RuntimePipelineRunner` remains a migration source because it currently owns broad mutable frame execution and compatibility behavior. Its responsibilities will be split into acquisition, signal conditioning, inference, calibration, encoding, safety, commit, feedback, correction, and audit services that implement the interfaces in the final contract. It must not become the public production session API.

## Why `NeuralChipOnlyFullDivePipeline` is not the final API

`NeuralChipOnlyFullDivePipeline` contains valuable chip-only algorithms such as modality encoding, phase lock, hold/commit policy, ACK handling, replay, and session digest behavior. It remains a migration source for those algorithms, but it is a competing orchestrator. Its functions will be distributed behind `ISensoryEncoding`, `ISpikeMapping`, `ISafetyAuthority`, `INeuralChipTransport`, `IFeedbackReader`, and `IAuditSink` rather than exposed as an alternate runtime.

## Consequences

- Final runtime control uses one owner and one transaction contract.
- No exception crosses the real-time loop boundary; phase functions return `PhaseResult<T>` with `RuntimeError` and `LifecycleDisposition`.
- Time uses explicit monotonic types (`MonotonicNs`, `DurationNs`, `DeadlineNs`, `AgeNs`, `SkewNs`, `BudgetNs`) and forbids naked numeric conversion.
- External ACK and feedback are explicit phases with correlation identifiers.
- Observer snapshots are read-only and receive only a closed transaction.
