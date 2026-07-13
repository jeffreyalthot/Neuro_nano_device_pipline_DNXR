# DNXR Canonical BCI Runtime Architecture

## Prompt 02 decision

The canonical production runtime has one owner of the real-time cycle: `RuntimeController` as the application-facing façade and `CanonicalNeuralBciPipeline` as the single internal transaction contract. `RuntimePipelineRunner`, `NeuralChipOnlyFullDivePipeline`, `PipelineOrchestrator`, research orchestrators, HIL, replay, and bench runtimes are migration sources or environment adapters only; none may own a production cycle after consolidation.

## Required boundary diagram

```text
[Virtual world computer / game engine]
  -> IVirtualWorldFrameSource
  -> CanonicalWorldFrame {frameId, schemaVersion, monotonicTimestampNs, provenance, validity, safetyDecision}
  -> CanonicalNeuralBciPipeline
       01 BootAndAttestation
       02 AcquireWorld
       03 SanitizeWorld
       04 AcquireBci
       05 SignalProcessing
       06 DecodeIntentMotorCognition
       07 UpdateAvatarWorld
       08 CalibrateAndBindPatient
       09 SensoryEncoding
       10 EncoderOutputContracts
       11 NormalizeAndAlign
       12 SpikeMappingAndPpp
       13 IsolationPolicyCapabilityDose
       14 PhaseLock
       15 GlobalSafety
       16 PrepareCommit
       17 DispatchTransport
       18 Ack
       19 Feedback
       20 Correction
       21 PostCorrectionSafety
       22 FinalizeAudit
  -> INeuralChipTransport / real HAL bidirectional transport
  -> BCI chip ACK, telemetry, closed-loop feedback
```

Optional sources/sinks:

- `IIndependentBiometricSource` may provide non-headset physiology. It cannot be an HMD, XR headset, controller, SteamVR/OpenXR device, or eye tracker from a headset.
- `IReadOnlyObserverDiagnostics` may observe committed cycles after finalization. It is read-only and cannot mutate `CanonicalBciCycleTransaction`, safety decisions, or neural command frames.
- `RealHardware`, `HILTestAdapter`, and `DeterministicReplayAdapter` are environment adapters on the same canonical stage order, not alternate runtimes.

## Canonical transaction frames

The code contract in `src/runtime/CanonicalNeuralBciPipeline.h` defines these versioned frames:

| Frame | Purpose | Mandatory metadata |
|---|---|---|
| `CanonicalBciFrameStamp` | common stamp | `frameId`, `schemaVersion`, monotonic timestamp, provenance, validity, safety decision, reason |
| `CanonicalWorldFrame` | structured scene/audio/contact/pose/motion from world engine | stamp plus modality vectors |
| `CanonicalBciAcquisitionFrame` | incoming BCI samples and optional independent biometrics | stamp plus raw neural signals and non-headset biometrics |
| `CanonicalBciCommandFrame` | bounded command candidate | stamp plus patient binding, capability, envelope, lifecycle, ACK policy, feedback capability, chemical gate checks |
| `CanonicalBciFeedbackFrame` | ACK/telemetry/closed-loop response | stamp plus ACK flag, explicit `autoAck` flag, chip telemetry, feedback |
| `CanonicalBciCycleTransaction` | single cycle transaction | path decision plus world/acquisition/command/feedback frames |

The legacy `RuntimePipelineFrame::chipFrame` alias is frozen as a migration alias only. New canonical code must use named acquisition, command, and feedback frames.

## Causal order and forbidden reorderings

The canonical order is encoded as `kCanonicalBciStageOrder`; tests intentionally swap dispatch and ACK to prove the order check fails. Dispatch depends on sanitized world data, BCI acquisition, patient binding, capability negotiation, lifecycle policy, safety envelope, phase lock, and global safety. ACK and feedback cannot happen before dispatch. Correction cannot happen before feedback. Observer rendering cannot happen before final audit and cannot feed safety or command preparation.

## Compilable invariants introduced in Prompt 02

1. `CanonicalNeuralBciPipeline::stageOrderIsCanonical` rejects missing, duplicate, or reordered stage lists.
2. `CanonicalNeuralBciPipeline::dispatchPreconditionsSatisfied` rejects dispatch unless the world frame is valid and patient binding, capabilities, safety envelope, lifecycle, ACK policy, and feedback capability are verified.
3. `dispatchPreconditionsSatisfied` rejects any `autoAck` feedback flag.
4. `RuntimePathKind` now names one canonical path with adapters: `CanonicalAdapter`, `RealHardware`, `HILTestAdapter`, `DeterministicReplayAdapter`. The previous two-runtime names `SecondarySimulation` and `PrimaryReal` are removed from code.
5. Static architecture test `dnxr_canonical_runtime_architecture_test` fails if `XRHeadsetInterface`, `XRRenderer`, OpenXR, SteamVR, `SecondarySimulation`, or `PrimaryReal` appear in the canonical pipeline header.

## Migration ownership plan

| Current owner/symbol | Destination | Wrapper strategy | Removal gate |
|---|---|---|---|
| `RuntimeController` | façade and only cycle owner | keep public entry; replace internals stage by stage | all parity tests green |
| `RuntimePipelineRunner::run` | feature source for `CanonicalNeuralBciPipeline` | deprecated compatibility runner during migration | canonical runner reaches parity |
| `RuntimePipelineRunner::runLegacy` | deprecated adapter only | no ownership; used only by tests until parity | delete after symbol manifest marks no references |
| `NeuralChipOnlyFullDivePipeline::stepDetailed` | source of safety/commit/ACK/replay algorithms | wrap algorithms as canonical services | parity for chip commit/hold/ACK/replay |
| `PipelineOrchestrator` | support library/test utility | never production owner | no production references |
| `ResearchFullDiveOrchestrator` | research-only library | compile only behind research flag | no canonical references |
| `HardwareBenchRuntime` | bench adapter | environment adapter with no orchestration change | bench tests only |

## Stage management hierarchy

Every canonical stage must be organized as: input contract; validation; normalization; mapping/calibration; policy/capability; safety envelope; lifecycle/timebase; audit/trace; hold/recovery; metrics. This hierarchy centralizes checks without duplicating safety logic across runtimes.

## Prompt 03 fusion skeleton

`RuntimeController` is now the only application-facing cycle façade and owns exactly one `CanonicalNeuralBciPipeline`. The canonical pipeline owns a temporary in-process compatibility adapter (`RuntimePipelineRunner`) and invokes it once per cycle. This prevents the production binary from executing a controller cycle and then a separate chip-only cycle.

The canonical pipeline also owns the first global safety/lifecycle skeleton: `NeuralChipPipelineStateMachine` is the global state model for boot/running/awaiting-ACK/hold/decompression/emergency/resume/shutdown, while `FrameLifecycleGuard`, `NeuralChipAckHandshake`, `NeuralChipPipelineStateGate`, `AckHoldFrameFactory`, `SafeChipFrameHoldBuffer`, `NeuralChipFrameCommitter`, `PipelineResumeController`, `EmergencyLatchController`, replay, digest and decision trace are bundled under `CanonicalSafetyLifecycleBundle`. Existing stage code remains reachable through the compatibility adapter until each stage is absorbed.

`BackendAdapterLock` replaces the previous path-lock terminology. Once real hardware is locked, any request to fall back to the canonical adapter is recorded as `adapterFallbackAttempted` and the canonical cycle fails closed.
