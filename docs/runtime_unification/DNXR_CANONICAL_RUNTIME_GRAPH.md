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
