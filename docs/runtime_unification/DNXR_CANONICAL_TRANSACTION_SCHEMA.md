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
