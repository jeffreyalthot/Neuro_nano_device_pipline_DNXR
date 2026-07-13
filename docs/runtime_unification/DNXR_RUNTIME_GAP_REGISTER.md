# DNXR Runtime Gap Register
| Priority | Gap | Files/symbols | Evidence | Action |
|---|---|---|---|---|
| P0 | Two concurrent pipelines | `RuntimePipelineRunner::run`, `NeuralChipOnlyFullDivePipeline::stepDetailed` | both can drive safety/commit concepts | consolidate into one controller-owned pipeline |
| P0 | Headset dependency in authoritative runtime | `RuntimeController`, `RuntimeAcquisitionStage`, `RuntimeRenderStage`, `XRHeadsetInterface`, `XRRenderer` | headset telemetry/render in main graph | replace with BCI/world ingress; renderer observer-only |
| P0 | Chip-only pipeline not launched by app | `RuntimeApplication`, `RuntimeController` | app only constructs RuntimeController runner path | integrate chip-only stages into canonical runner |
| P0 | Null/simulated sources in real composition risk | `RuntimeConfig`, `RuntimeCompositionRoot`, `NeuralChipHAL`, device simulators | defaults simulation/synthetic | fail closed in real path |
| P0 | Legacy path callable | `RuntimePipelineRunner::runLegacy` | duplicate execution graph | adapter/deprecate after tests |
| P0 | CMake mixes simulation/HIL with core | `CMakeLists.txt` | glob plus default simulation-only includes sim sources | split test adapters from production core |
| P0 | No canonical world source | `VirtualWorldDataIngress`, `ImmersionCore`, `NeuralChipOnlyWorldFrame` | multiple schemas | choose canonical schema/service |
| P0 | Commit/ACK/feedback order unclear | `RuntimeStimulusStage`, `RuntimeClosedLoopFeedbackStage`, `NeuralChipFrameCommitter` | different lifecycle owners | adopt single lifecycle guard |
| P0 | Render may be authoritative | `RuntimeRenderStage`, `XRRenderer` | render precedes phase/stimulus | move out-of-band |
| P1 | Duplicate data models | frame headers | lossy conversion unknown | schema/adapters |
| P0 | Prompt 02 contract requires code migration to `CanonicalNeuralBciPipeline` | `RuntimeController`, `RuntimePipelineRunner`, `NeuralChipOnlyFullDivePipeline` | contracts exist but implementation still needs staged absorption | next prompt: first executable wiring step |
| P1 | Compatibility adapter still contains old staged implementation | `CanonicalNeuralBciPipeline::runCycle`, `RuntimePipelineRunner::run` | single owner is established but business logic is not fully absorbed | migrate stages into canonical bundles one stage at a time |
| P1 | Global state machine skeleton exists but not every legacy state transition is mapped | `CanonicalSafetyLifecycleBundle`, `NeuralChipPipelineStateMachine` | canonical bundle owns lifecycle components; stage transitions still delegated | prompt 04 map every transition and ACK/hold path |

## Prompt 04 remaining gaps
- Certified real biometric source adapters are not implemented in this prompt; the canonical default is explicit unavailable biometric provenance with no synthetic values.
- `XRRenderer` class name remains for compatibility and existing diagnostics tests, but controller ownership and docs requalify it as observer-only.

## Prompt 05 remaining gaps
- The canonical neural source is HAL-backed and validates sequence/timestamp/quality, but a vendor-specific authenticated packet transport parser remains a future real-hardware integration item behind the existing real-evidence gate.
- Motor intent decode components are preserved and still exercised by the chip/full-dive path; deeper trace fields for motor confidence decisions should be expanded in the next prompt without logging raw neural channels.

## Prompt 06 remaining gaps
- Canonical world ingress now validates structured frames and deterministic hold behavior, but a concrete network transport adapter with signed packet verification remains to be bound behind `VirtualWorldIngressConfig` in a later prompt.
- Motor/avatar output remains separated conceptually; a dedicated causality-tested output adapter should be wired to the engine in the next stage without feeding back into the same cycle.

## Prompt 07 gap update
- Closed: visual topology parameters now have a versioned validator and fail-neutral behavior when topology/patient binding is invalid.
- Closed: canonical visual unit/e2e coverage now proves structured world visual data reaches PPP/spike runtime without renderer dependency.
- Open: remaining visual configuration should be loaded from clinical configuration files rather than only in-memory `PatientVisualMap` defaults.

## Prompt 08 gap update
- Closed: canonical tests now exercise audio, touch, proprioception, vestibular, olfactory and gustatory encoders/adapters through the shared spike runtime path.
- Closed: `EngineSpikeCore` no longer allocates lanes solely in normalizer input order; safety-prioritized lane emission prevents silent loss of reality-anchor lanes under channel pressure.
- Open: `SpatialAudioEngine`, `HapticTextureEngine`, `SyntheticProprioceptionLayer`, `VestibularEmulationEngine`, `OlfactoryPersistenceEngine` and `GustatorySafetyEngine` should be lifted from support-library/test coverage into direct production-stage pre-encoder services in a follow-up pass.

## Prompt 09 gap update
- Closed: PPP grammar now has deterministic serialization/parse/verify and integrity checking over frame IDs, timing, topology, modality entries, feedback state and risk/phase metadata.
- Closed: feedback-only PPP state now prevents normal `StimulusBySpikeRuntime` acceptance through an explicit lifecycle hold reason.
- Open: negotiated real-hardware electrode availability still needs to replace the default fixed simulation topology inside `PPPGrammarFrameBuilder::build` once capability negotiation is fully canonicalized into the spike stage.

## Prompt 10 gap update
- Closed: `INeuralChipTransport` now documents and implements the full bidirectional lifecycle expected at the chip boundary.
- Closed: real transport no longer returns fabricated `authorized_real_transport_dispatch` success.
- Closed: transport lifecycle regression covers replay/duplicate ACK/timeout/emergency cutoff and no double write.
- Open: a vendor-specific real protocol driver remains unavailable; real backend correctly blocks with an explicit unavailable result until implemented and attested.
