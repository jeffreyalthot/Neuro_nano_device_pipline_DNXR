# DNXR Canonical Transaction Schema

## Prompt 02 frozen code contract

The compilable source of truth is `src/runtime/CanonicalBciRuntimeContract.h`.

### Public runtime API

`dnxr::canonical::CanonicalNeuralBciRuntime` exposes only:

1. `openSession(const OpenSessionRequest&)`
2. `step(const StepRequest&)`
3. `requestHold(const char*)`
4. `requestEmergencyStop(const char*)`
5. `closeSession()`

No legacy, rendering, headset, or simulation method is part of this final production API.

### Phase chain

`SessionContext -> WorldIngressFrame -> BciAcquisitionFrame -> ConditionedSignal -> InferenceResult -> MotorIntent -> CalibrationSnapshotRef -> EncodedModalities -> AlignedSpikePlan -> SafetyDecision -> PreparedCommit -> SubmitReceipt -> ExternalAck -> ExternalFeedback -> CorrectionDecision -> ClosedTransaction`.

`PreparedCommit` is a move-only token created through `prepareCommit`; transport accepts `PreparedCommit&&` and returns a `SubmitReceipt`. ACK correlation is explicit through `correlateAck`, and closure with required feedback is explicit through `closeWithFeedback`.

### Identity and digest propagation

The contract defines `SessionId`, `FrameId`, `WorldSequence`, `AcquisitionSequence`, `CommitId`, `PacketNonce`, `ConfigurationDigest`, `PatientMapDigest`, `TopologyDigest`, `CapabilityDigest`, `SafetyPolicyDigest`, and `TransactionDigest`. The identifiers propagate by typed phase embedding so downstream phases can recover session, frame, commit, nonce, and digests without global state.

### Time contract

The only clock interface is `IMonotonicClock`. Phase timing uses `MonotonicNs`, `DurationNs`, `DeadlineNs`, `AgeNs`, `SkewNs`, and `BudgetNs`. Constructors are explicit to prevent accidental ns/us/ms/s conversion through naked arithmetic.

### Interface bundle

The minimal bundles are `IWorldIngress`, `IBciAcquisition`, `ISensoryEncoding`, `ISpikeMapping`, `ISafetyAuthority`, `INeuralChipTransport`, `IFeedbackReader`, `IMotorWorldEgress`, `IAuditSink`, and `IObserverSnapshotSink`. Observer output is read-only and receives only a closed transaction.

### Error model

Every phase returns `PhaseResult<T>` or a no-throw runtime error. `RuntimeErrorCode` and `LifecycleDisposition` encode retry, hold, decompression, emergency, shutdown, and external dependency unavailable outcomes. No exception is allowed to cross the real-time step boundary.

## Prompt 03 immutable session bootstrap snapshot

`SessionBootstrapSnapshot` freezes profile kind, deployment mode, configuration/patient/topology/capability/safety digests, backend attestation status, secret readiness, ingress/egress readiness, unique-owner counters, and destruction order before any frame is allowed. It is intentionally not assignable after creation.

## Prompt 04 transaction execution fields — 2026-07-13

`CanonicalBciCycleTransaction` now carries per-stage timing (`queueWaitMs`, `executionMs`, `externalWaitMs`, `deadlineSlackMs`, `totalMs`), a cumulative `traceDigest`, and `preparedCommitOnly`. The digest is mixed from phase identity and bounded metadata; raw neural signal payloads are not serialized into the digest log. The transaction may reach `PreparedCommit` but the canonical transport stage intentionally returns a hold reason until a certified external transport is bound.

## Prompt 05 input contract migration — 2026-07-13

The controller-to-canonical transaction input types were moved out of `RuntimePipelineRunner` into `RuntimePipelineRunInput.h`. This keeps scheduler, services, sink refs, runtime metrics snapshot, and finalizer context available to the canonical graph without constructing or linking the legacy runner facade.

## Prompt 06 BCI acquisition frame schema — 2026-07-13

The canonical acquisition payload now includes device identity, session id, sequence, hardware timestamp, host timestamp, clock domain, sample layout, topology digest, quality flags, dropout/overflow counters, packet digest, authentication tag, and explicit `external`, `attested`, `authenticated`, and `recordedNonHumanFixture` flags. Invalid frames are not repaired; validation returns a hold/failure reason.

### Prompt 07 world ingress schema
- `CanonicalVirtualWorldFrame` now includes schema version, sequence, session/source identifiers, peer certificate digest, message digest, auth tag, host/engine/deadline timestamps, semantic scene, coordinate frame, units, environment, events, asset references, and explicit modality presence bits.
- The world frame never carries electrode commands; decoded motor intent is separated behind `IMotorWorldEgress` and is not acceptable as neural feedback proof.

### Prompt 08 motor intent schema
- Motor intent transaction data includes model digest/version/signature status, BCI provenance, signal quality, confidence, uncertainty, bounded `MotorIntentCommand`, safety disposition, and command hash.
- The default disposition is neutral hold; low confidence, stale/corrupt/non-external acquisition, non-finite input, or invalid model manifest never becomes an active avatar command.
