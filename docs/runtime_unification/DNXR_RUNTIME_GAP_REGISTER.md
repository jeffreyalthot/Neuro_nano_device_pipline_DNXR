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
