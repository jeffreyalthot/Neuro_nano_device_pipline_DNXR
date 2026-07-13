# DNXR Prompt 01 Runtime Graph Reconstruction

## Entry points and CMake target map
- `distributed_neural_xr`: `src/main.cpp -> RuntimeApplication::run -> RuntimeController::run -> RuntimePipelineRunner::run`; linked with `dnxr_core`.
- `dnxr_core`: globbed `src/*.cpp` except mains; research speculation excluded unless `DNXR_ENABLE_RESEARCH_SPECULATION=ON`; simulation/HIL excluded only when `DNXR_SIMULATION_ONLY=OFF`.
- `dnxr_hardware_bench`: `src/hardware_bench/main.cpp -> HardwareBenchRuntime`; gated by `DNXR_ENABLE_HARDWARE_BENCH=ON` and `DNXR_SIMULATION_ONLY=OFF`.
- Presets: `simulation`, `hardware-bench-dry-run`, `hardware-bench-active-dummy`, `clinical-locked`.

## CMake option behavior
| Option | Prompt 01 finding |
|---|---|
| `DNXR_SIMULATION_ONLY` | Default ON; controls selected core source and real-hardware fatal guard. |
| `DNXR_ENABLE_SIMULATION` | Declared; not the primary source-selection switch. |
| `DNXR_ENABLE_HARDWARE_BENCH` | Adds bench executable only with simulation-only OFF. |
| `DNXR_ENABLE_PRECLINICAL_PHANTOM` | Compile definition only in baseline map. |
| `DNXR_ENABLE_HUMAN_CLINICAL_TRIAL` | Requires `DNXR_HUMAN_USE_BUILD=ON` and signed evidence. |
| `DNXR_HUMAN_USE_BUILD` | Requires signed human-use build gate. |
| `DNXR_ENABLE_RESEARCH_SPECULATION` | Re-adds research orchestrator/network/physics/memory speculative modules. |

## RuntimeController -> RuntimePipelineRunner graph
```text
src/main.cpp -> RuntimeApplication::run -> RuntimeController::run -> RuntimeController::runCycle
  -> RuntimePipelineRunner::run
    -> RuntimeAcquisitionStage -> RuntimeSignalStage -> RuntimeInferenceStage -> RuntimeCalibrationStage
    -> RuntimeImmersionFrameStage -> RuntimeFullDiveStage -> RuntimeFiveSenseCoherenceStage
    -> RuntimeBodyPoseExtractionStage -> RuntimeBodySchemaStage -> RuntimeSafetyStage -> RuntimeRenderStage
    -> RuntimeModalityPhaseLockStage -> RuntimeStimulusStage -> RuntimeClosedLoopFeedbackStage
    -> RuntimePerceptionCorrectionStage -> RuntimePostCorrectionSafetyStage -> RuntimeCycleFinalizer
```

## NeuralChipOnlyFullDivePipeline graph
```text
NeuralChipOnlyFullDivePipeline::stepDetailed(world)
  -> WorldFrameSanitizer -> effectivePatientMap/capability gates -> modality encoders/vector-to-spike
  -> SensorEncoderOutputContract/neutrality validators -> phase lock/safety/chemical/reality anchor gates
  -> NeuralChipFrameCommitter -> NeuralChipBus submit -> ACK/lifecycle guard -> replay/decision trace/session digest
```

## Other orchestrators
- `PipelineOrchestrator`: generic step executor; support only, not canonical production.
- `RuntimePipelineRunner::runLegacy`: duplicate staged execution via `LegacyRunInput`; P0 migration target.
- `ResearchFullDiveOrchestrator`: research/speculation only when enabled.
- `HardwareBenchRuntime`: bench-only dry-run/dummy-load path.

## Functional comparison
| Stage | Controller/Runner | Chip-only pipeline | Prompt 01 gap |
|---|---|---|---|
| acquisition | NanoNode, XRHeadset telemetry, HAL read | chip bus read + world frame | P0 remove headset from authoritative acquisition |
| world | ImmersionCore/DeepImmersion | `NeuralChipOnlyWorldFrame` | P0 define canonical world source |
| signal/inference | DSP/vectorizer/state inference | chip read/motor intent feedback | P1 model consolidation |
| five senses/spikes | partial runtime stages | explicit encoders and adapters | P0 wire into main |
| safety | guard/governor/envelope/render safety | modality envelope, anchor, lifecycle | P0 single owner/order |
| commit/ACK | stimulus then HAL feedback read | committer + ACK guard | P0 order ambiguity |
| render | renderer is in path | absent | P0 make observer-only |
| audit/replay | trace/state/metrics | replay/decision digest | P1 consolidate evidence |

## Prompt 02 canonical target graph

```text
RuntimeApplication
  -> RuntimeController (only real-time cycle façade)
    -> CanonicalNeuralBciPipeline (single transaction contract and stage order)
      -> IVirtualWorldFrameSource
      -> INeuralChipTransport / INeuralChipHAL
      -> optional IIndependentBiometricSource
      -> optional IReadOnlyObserverDiagnostics after finalization only
```

The old `RuntimePipelineRunner` and `NeuralChipOnlyFullDivePipeline` are now explicitly classified as migration sources for algorithms, not long-term cycle owners.

## Prompt 03 actual skeleton graph

```text
RuntimeApplication::run
  -> RuntimeController::run / runDeterministicCycle
    -> BackendAdapterLock::decide
    -> CanonicalNeuralBciPipeline::runCycle (single owner per frame)
      -> CanonicalSafetyLifecycleBundle global lifecycle/commit/ACK components
      -> RuntimePipelineRunner::run (temporary compatibility adapter, exactly one invocation)
      -> Canonical transaction/metrics finalization
```

`RuntimeController` no longer owns `RuntimePipelineRunner` directly. `RuntimePipelineRunner` remains compiled for compatibility tests and as a temporary adapter owned by `CanonicalNeuralBciPipeline`.

## Prompt 04 graph update
`RuntimeApplication -> RuntimeController -> RuntimeAcquisitionStage(IBiometricSource + neural chip frame) -> signal/inference/calibration/immersion/safety -> RuntimeRenderStage(neural sensory preparation) -> RuntimeStimulusStage(neural chip dispatch)`.

Optional side branch after neural sensory preparation: immutable copy -> `ObserverMirrorSink` -> observer/diagnostic `XRRenderer`. This branch is non-blocking for chip command authorization and has no headset dependency.

## Prompt 05 graph update
Canonical incoming path: `INeuralSignalSource::readCanonicalFrame` -> packet/sequence/timestamp/quality validation -> immutable `RuntimePipelineFrame::acquisitionChipFrame` -> signal denoise/vectorize/artifact quality gate -> inference/motor/full-dive/safety stages. `NanoNodeInterface` and `HostComputerBridge` remain support transport/sink integrations, not additional acquisition readers in the canonical cycle.

## Prompt 06 graph update
Computer/world-engine path: `CanonicalVirtualWorldFrame` (versioned structured modalities + provenance/auth metadata) -> `VirtualWorldDataIngress::ingestCanonical` schema/size/sequence/timestamp/provenance/auth/backpressure checks -> clinical neutral scene + scene risk + body safety + chemical disable gates -> `WorldFrameSanitizer` -> encoder-facing `NeuralChipOnlyWorldFrame` synchronized to the BCI cycle. Rejected, duplicate, stale, future, unsafe, or chemical-injected frames produce a deterministic hold world with reality anchor retained and active risky modalities neutralized.

## Prompt 07 graph update
Canonical visual path is now structured world data only: `VirtualSceneFrame` mandatory luminance/contrast/motion/depth-obstacle/horizon/text-symbol/body-avatar/world-gaze-attention fields -> `VisualSceneSemanticExtractor` -> `VisualCorticalWorldEncoder` with versioned `VisualTopologyConfig` (grid, ON/OFF channels, chromatic flag, foveation, surround, compression) and `PatientVisualMap` -> photic `VisualSafetyState` controls -> `PhospheneFrameStabilizer` -> `VectorIntentNormalizer`/`VisualVectorToSpikeAdapter` -> temporal alignment -> patient binding -> PPP grammar -> spike runtime -> command bundle. XRRenderer remains observer-only and is not an authoritative input to neural visual output.

## Prompt 08 graph update
All sensory modalities now have explicit canonical coverage through the same spike runtime contract: structured world audio/contact/pose/motion/odor/taste frames -> modality encoders -> modality vector adapters/normalizer -> temporal alignment -> patient target binding -> `CrossModalCoherenceBinder` -> PPP grammar -> deterministic priority spike lanes -> stimulus-by-spike packet -> patient policy, chemical gate, phase lock, exposure ledger, capability contract, safety envelope, disabled-modality neutrality, reality anchor hard channel, commit. `EngineSpikeCore` emits hardware lanes by priority order (`RealityAnchor`, `Motor`, `Touch`, `Proprioception`, `Visual`, `Auditory`, `Vestibular`, `Olfactory`, `Gustatory`) so channel over-subscription is deterministic and preserves hard safety/stop channels instead of truncating silently by input order.

## Prompt 09 graph update
The pre-dispatch representation is now explicitly the validated spike/PPP bundle: modality vectors -> `VectorIntentNormalizer` -> `VectorTemporalAlignmentBuffer` -> `PatientTargetBindingLayer` -> `CrossModalCoherenceBinder` -> `PPPGrammarFrameBuilder` (versioned, integrity-hashed, typed electrode/wire map) -> `VectorToSpikeEngineFrameBuilder` -> `MultiLayerSpikeThinkingEngine` -> `EngineSpikeCore` -> `EngineSpikeOutputValidator` -> `SpikeEnvelopePolicyApplier` -> `SpikeToStimulusIntentTranslator` -> `StimulusBySpikePacketAssembler` -> `StimulusFeedbackSynchronizer`. Feedback-only PPP frames now force a lifecycle hold before normal dispatch; normal dispatch cannot be unlocked by PPP grammar alone when ACK/feedback quality is below threshold.

## Prompt 10 graph update
The chip boundary now exposes one bidirectional transport abstraction: discover/connect -> attest -> negotiate -> synchronizeClock -> openAuthenticatedSession -> read -> prepareCommand -> submitPreparedCommand -> receiveAck -> readFeedback -> emergencyStop -> close. `send` remains only a compatibility wrapper around prepare/submit/external ACK and does not manufacture ACK acceptance. Real transport fails closed with `real_transport_protocol_driver_unavailable` until a concrete protocol driver exists; simulation/HIL adapters implement the same lifecycle explicitly for tests and non-human profiles.

## Prompt 11 safety hierarchy overlay

Canonical safety decisions now flow through six ordered authority levels before commit: local encoder, modality, cross-modal, session, transport/hardware, and post-feedback. The merged decision is monotonic (`max` severity, `max` risk, `min` intensity, AND render permission, OR emergency), so an allow/continue result from a later layer cannot erase an earlier refusal. The lifecycle state machine exposes Booting → EvidenceCheck → Connecting → Calibrating → Ready → Running/AwaitingAck, plus HoldNeutral, HoldMinimal, Recalibrating, Decompressing, ControlledResume, EmergencyLatched, and Shutdown. Emergency reset remains an audited operator path and resumes only into ControlledResume.

## Prompt 12 closed-loop motor/world/feedback graph

```text
chip read N
  -> MotorCortexReadFrameAdapter
  -> MotorIntentDecoder
  -> MotorIntentConfidenceGate
  -> AvatarGaitPhaseController / AvatarBalanceController / AvatarInverseKinematicsSolver
  -> AvatarKinematicRigController
  -> AvatarSafetyPoseController when low confidence, spasm, latch, or safety pose required
  -> MotorIntentCommand(valid for world N+1)
  -> ICanonicalMotorIntentSink(world engine boundary, not renderer)
  -> world frame N+1
  -> sensory encoders/spike runtime/commit N+1
  -> ACK N+1
  -> distinct feedbackChipFrame N+1
  -> ClosedLoopPerceptionErrorController correction for candidate N+2
```

A feedback ACK is not treated as physiological/neural feedback. If the feedback frame for the previously submitted chip frame is absent, stale, or weak, the canonical path emits a hold and blocks normal dispatch.

## Prompt 13 transverse subsystem placement

The canonical runtime now treats repository subsystems as layers around one frame owner instead of alternative runtime owners:

- **Per-frame canonical path:** runtime, full_dive, neural_chip, hardware, safety, and selected signal/neurotech services operate under `RuntimeController -> CanonicalNeuralBciPipeline` ownership.
- **Pre-frame/off-frame configuration and evidence:** config, secrets, packet protection, identity, QMS, readiness, clinical evidence, revocation, assets, plugin registry, and signed manifests produce immutable snapshots or gates before frame execution.
- **Observation and reporting:** logging, trace, metrics, latency, jitter, WCET, replay, save/session journal, dashboards, and file writers consume bounded records after frame decisions and do not authorize neural output.
- **Adapters:** HIL, bench, real/hardware bench, input/replay and simulation modules are environment adapters feeding the same canonical interfaces; they are not production runtime owners.
- **Research/speculative libraries:** emotion, physics/quantum, distributed consciousness, subjective time, subconscious, memory manipulation and social/MMORPG domain code are isolated as research/support libraries unless explicitly gated by CMake and clinical/research policies.

## Prompt 14 — single production runtime boundary

Production now has one business cycle owner: `RuntimeApplication` constructs `RuntimeController`, which owns `CanonicalNeuralBciPipeline::runCycle` and its canonical stage services/sinks. `RuntimePipelineRunner` no longer exposes `LegacyRunInput` or `runLegacy`; it remains only as a canonical stage runner utility for tests/adapters that use `RuntimePipelineRunInput` or immutable `RuntimePipelineRunInputRef`.

Real-runtime source selection (`DNXR_SIMULATION_ONLY=OFF`) excludes simulation/HIL/headset/observer-renderer implementation sources. Observer mirroring remains out-of-band and read-only: renderer attachment is available only in simulation/observer builds, while real builds retain an observer sink that records a renderer-excluded diagnostic and cannot affect neural safety, frame commit, ACK, or feedback decisions.

## Prompt 16 realtime overlay

The canonical runtime owner now has an explicit realtime telemetry overlay:

```text
RuntimeApplication -> RuntimeController -> CanonicalNeuralBciPipeline::runCycle
  -> RuntimeRealtimeTelemetry::beginCycle(monotonic steady_clock)
  -> canonical stage order timing windows (p50/p95/p99/max/WCET, fixed 64-sample windows)
  -> bounded queue-depth/drop/hold counters for world ingress and chip transport backpressure
  -> RuntimeRealtimeTelemetry::endCycle(monotonic steady_clock)
```

The BCI/world rate contract defaults to 60 Hz with a 16.666 ms budget when compatible with the hardware. If hardware capability is lower, `RuntimeRateContract::negotiate` lowers the cycle rate and marks the requested 60 Hz target as not hardware-compatible instead of silently overrunning or coupling the observer display rate to the neural cycle. Observer/dashboard output remains out-of-band and is not part of the authoritative safety/commit path.


## Prompt 18 final sweep — 2026-07-13 UTC

- Rebuilt focused canonical runtime and migrated runner/chip tests in `build/final`.
- Rebuilt the real configuration in `build/final-real` with `DNXR_SIMULATION_ONLY=OFF` and confirmed no forbidden headset, legacy runner, HIL, simulated transport, or null neural source symbols in the production binary symbol scan.
- Converted simulation/HIL neural-chip transport declarations and definitions to simulation-only compile units so the real binary cannot link them accidentally.
- Fixed the canonical runtime aggregate link closure using a CMake RESCAN link group.
- Added final acceptance and external dependency reports.
- Full CTest is not fully green; the final acceptance report records the remaining failing validation lanes without claiming clinical/hardware validation.
