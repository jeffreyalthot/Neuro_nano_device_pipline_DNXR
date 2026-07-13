# DNXR Runtime Invariants and Data Model Inventory
## Invariants
1. One canonical controller owns the real-time cycle.
2. BCI acquisition/inference/encoding/spike mapping/dispatch/feedback must not depend on headset/OpenXR/SteamVR/HMD/renderer.
3. Missing hardware/protocol/evidence fails closed; no fake ACK/telemetry in real path.
4. Not-for-human-use locks, signed evidence gates, dose limits, chemical default-off, reality anchor, emergency stop, and mandatory closed-loop feedback remain.

## Duplicate data models
| Model | Location | Conversion/loss finding |
|---|---|---|
| `RuntimePipelineFrame` | `src/runtime/*` | Carries headset/render-oriented fields and chip feedback; needs canonical schema split. |
| `NeuralChipOnlyWorldFrame` | `src/full_dive/chip_pipeline/*` | Best candidate for structured world input; currently not app entry. |
| `FullDiveNeuralFrame` | `src/full_dive/FullDiveNeuralFrame.*` | Separate neural frame requiring adapter. |
| `FullDiveChipFrame` | chip pipeline | Strong commit/safety model; preserve. |
| `NeuralChipFrame`/`ChipReadFrame` | hardware/neural_chip | ACK/telemetry source; must not be faked in real path. |
| `SafeNeuralStimulusCommandFixed` and bounded commands | safety/hardware | Candidate canonical bounded dispatch representation. |
| Spike bundles | spike/vector-to-spike modules | Preserve as modality-specific adapters. |

## Prompt 02 compilable invariants

- Canonical frame stamps are versioned and carry frame ID, monotonic timestamp, provenance, validity, safety decision, and reason.
- Canonical dispatch is rejected until world validity, patient binding, capabilities, safety envelope, lifecycle, ACK policy, and feedback capability are all verified.
- Auto-ACK is explicitly represented and rejected by the canonical dispatch precondition helper.
- Runtime path kinds are environment adapters on one canonical path: `CanonicalAdapter`, `RealHardware`, `HILTestAdapter`, `DeterministicReplayAdapter`.

## Prompt 03 executable skeleton invariants

- `RuntimeController` owns exactly one `CanonicalNeuralBciPipeline` instance.
- The compatibility runner is invoked by the canonical owner once per frame and has no separate controller-owned state.
- Canonical metrics record one started cycle, completed cycle, compatibility delegation, commit preparation, dispatch observation, and metric record per frame.
- Real hardware backend lock records `adapterFallbackAttempted` and fails closed rather than permitting a fallback adapter.

## Prompt 04 invariant additions
- The canonical BCI cycle must not construct or call `XRHeadsetInterface`.
- Biometric monitoring must use `IBiometricSource` samples with timestamp, quality, validity, and provenance; absence is explicit and must not synthesize normal telemetry.
- Observer mirroring is optional, read-only, and non-authoritative. Observer failures must not alter the neural command prepared for chip dispatch.

## Prompt 05 invariant additions
- A runtime cycle may perform exactly one authoritative acquisition read through `INeuralSignalSource::readCanonicalFrame`.
- `RuntimePipelineRunner` must not call `INeuralChipHAL::readFrame` after acquisition; downstream stages consume the immutable acquisition frame.
- Real backend initialization must not install a Null neural signal source; if a concrete HAL-backed source cannot be created, startup fails with `REAL_NEURAL_SIGNAL_SOURCE_UNAVAILABLE`.
- Duplicate/replayed sequence IDs, stale timestamps, empty channels, and zero-quality neural frames fail closed before signal/inference.

## Prompt 06 invariant additions
- World-engine input must enter the canonical path as a versioned `CanonicalVirtualWorldFrame`; raw pixel/audio/network payloads are not encoder inputs.
- Omitted modalities are neutral for the current frame and must not carry residual intensity from a previous frame.
- Duplicate, replayed, stale, future, oversized, unauthenticated, unsafe-scene, unsafe-body, or chemical-injected world frames must hold/reality-anchor before encoding.
- Patient maps remain middleware-owned; engine-provided frames may carry context/provenance but cannot supply patient binding or calibration maps.
