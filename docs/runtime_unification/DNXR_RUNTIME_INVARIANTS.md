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

## Prompt 07 visual invariants
- Visual neural output must originate from `VirtualSceneFrame` structured world fields, never from `XRRenderer`, HMD eye tracking, OpenXR, SteamVR, or headset frame cadence.
- `PatientVisualMap::topology` must validate schema version, bounded grid size, ON/OFF availability, foveation, surround suppression, and compression before non-neutral visual encoding.
- Visual disabling or invalid topology produces a strictly neutral `NeuralVisualVector` while the reality anchor path remains separate and available.
- Photic risks (flicker, flash pattern, luminance slew, contrast, motion flow, phosphene stability, temporal coherence, fatigue, latency) must be represented in `VisualSafetyState` and checked before spike commit.

## Prompt 08 multimodal invariants
- Audio, touch, proprioception, vestibular, olfactory and gustatory vectors must enter the same `StimulusBySpikeRuntime` path as vision and cannot bypass patient binding, temporal alignment, cross-modal coherence, PPP grammar, phase lock, exposure/dose policy, capability checks, disabled-modality neutrality or the reality anchor hard channel.
- Chemical smell/taste channels remain structurally present but neutral by default unless both patient map and hardware/policy gates are valid; a runtime option alone is insufficient authorization.
- Global spike-lane pressure must be resolved by deterministic safety-prioritized modality emission. Reality anchor and motor/stop-related channels outrank sensory/chemical channels.

## Prompt 09 spike/PPP invariants
- Normal chip dispatch must be derived from a spike bundle that has traversed normalization, temporal alignment, patient binding, cross-modal coherence, PPP grammar, spike intent, thinking layers, engine output validation, spike envelope, stimulus translation, packet assembly and feedback synchronization.
- PPP grammar frames must carry version, frame validity, modality entries, electrode/wire bounds, phase/risk, feedback/hold semantics and an integrity hash; serialized PPP must round-trip through parse/verify before use as evidence.
- PPP feedback-only state is a lifecycle hold/recalibration state, not a blocking loop and not a normal dispatch permission.

## Prompt 10 transport/ACK invariants
- A normal hardware-bound frame must follow candidate -> validate -> prepare -> submit -> ACK pending -> external ACK accepted/rejected/timeout -> feedback transaction order.
- The runtime sender may not auto-generate ACK acceptance; tests may only obtain ACK from explicit simulation/HIL transport adapters.
- Real transport without a concrete protocol driver must fail closed and must not return an authorized dispatch success.
- Empty transport interfaces and automatic production fallback to simulation/HIL are prohibited.

## Prompt 11 safety lifecycle invariants

1. Safety decisions are monotonic across levels: the final action is the highest-severity action observed; risk is max-merged; intensity is min-merged; render permission is AND-merged; emergency is OR-merged.
2. Each safety layer decision carries a canonical decision level, authority, and reason so hold/emergency traces identify both the first highest-severity cause and all contributing decisions.
3. Canonical lifecycle reset from emergency/post-incident review lands in `ControlledResume`, never directly in `Running`.
4. Observer/render health is not a safety authority in this hierarchy; safety levels are encoder, modality, cross-modal, session, transport/hardware, and post-feedback.
5. Hold states are represented as `HoldNeutral` and `HoldMinimal`; decompression is represented as `Decompressing`; all remain below `EmergencyLatched` and cannot clear a latch without explicit audited operator review.

## Prompt 12 closed-loop motor/perception invariants

1. BCI motor acquisition for frame N may produce a `MotorIntentCommand` valid only for world frame N+1; the command is not valid for retroactive mutation of frame N.
2. Motor intent output is submitted through `ICanonicalMotorIntentSink` to the world-engine boundary and not to `XRRenderer`, `XRHeadsetInterface`, or any observer branch.
3. Closed-loop perception correction uses a distinct feedback frame corresponding to the last submitted chip frame; ACK alone does not satisfy feedback.
4. Missing, stale, or low-quality feedback after a submitted frame prevents the next normal dispatch and enters a safety hold path.
5. Closed-loop corrections apply only to the next candidate frame under construction; already submitted frames remain immutable replay evidence.

## Prompt 13 transverse subsystem invariants

1. Every source/header file under `src/` must have a disposition entry before merge; missing files fail `dnxr_runtime_disposition_manifest_audit_test`.
2. Every public class/struct/enum declaration found in headers must have a symbol-preservation entry before merge; missing public types fail the same audit.
3. Heavy/non-real-time work (JSON/config loading, signatures, QMS/readiness report generation, disk writes, dashboards, plugin discovery, network management) must occur outside the per-frame hot path or via immutable snapshots/queues.
4. Speculative/research modules remain disabled from the default canonical runtime and must not be represented as clinical or production capabilities.
5. HIL/simulation/bench adapters may inject faults or fixtures only through canonical interfaces; they must not own an alternative production loop.

## Prompt 14 invariants

- The production runtime must not compile or expose `RuntimePipelineRunner::LegacyRunInput`, `runLegacy`, or `finishLegacyCycle`; canonical runner tests must prove these names are absent before merge.
- `DNXR_ENABLE_SIMULATION` may enable adapters, fixtures, and tools only; it must not change the canonical business graph or introduce a secondary production runtime.
- A real-runtime binary built with `DNXR_SIMULATION_ONLY=OFF` must not export forbidden headset, observer renderer, simulation, HIL, auto-ACK, legacy, or secondary path symbols.
- Observer rendering is never authoritative. In real-runtime builds renderer attachment is unavailable and observer publish attempts fail closed as observer diagnostics only.

## Prompt 15 build/config invariants

- CMake presets must not set `DNXR_HUMAN_USE_BUILD=ON` or `DNXR_ENABLE_HUMAN_CLINICAL_TRIAL=ON`; signed evidence gates remain required even if a caller attempts to set those flags manually.
- Runtime profiles must declare `schemaVersion=dnxr.unified-runtime-profile.v1`, reject unknown properties by schema, keep secrets external to JSON files, keep raw neural payload logging disabled, and make profile configuration immutable after startup.
- BCI world ingress must be structured-world-stream input with headset endpoints forbidden; observer output, when present, is read-only and cannot feed safety decisions or chip frames.

## Prompt 16 realtime invariants

- The canonical neural/world cycle target is 60 Hz (`16.666 ms`) only when hardware negotiation confirms compatibility; otherwise the negotiated rate is lower and must be explicit in telemetry.
- Realtime telemetry uses bounded fixed-size windows and const cause tokens; it must not perform synchronous per-frame logging in the realtime thread.
- World/chip backpressure must use bounded keep-latest/drop-or-hold semantics; obsolete stimulation frames must not accumulate while waiting for ACK/feedback.
- Safety/commit order is unchanged: telemetry and queue accounting cannot move validation after dispatch or clear an existing hold/emergency decision.

## Prompt 17 executable architecture invariants

- `dnxr_canonical_runtime_architecture_guard_test` proves dispatch preconditions reject auto-ACK and missing world validity.
- The same test proves the canonical stage order rejects Dispatch/Ack swaps.
- The same test scans the canonical runtime header/implementation and application entry for headset, OpenXR/SteamVR/HMD, legacy input, `runLegacy`, and `PipelineOrchestrator` production ownership references.


## Prompt 18 final sweep — 2026-07-13 UTC

- Rebuilt focused canonical runtime and migrated runner/chip tests in `build/final`.
- Rebuilt the real configuration in `build/final-real` with `DNXR_SIMULATION_ONLY=OFF` and confirmed no forbidden headset, legacy runner, HIL, simulated transport, or null neural source symbols in the production binary symbol scan.
- Converted simulation/HIL neural-chip transport declarations and definitions to simulation-only compile units so the real binary cannot link them accidentally.
- Fixed the canonical runtime aggregate link closure using a CMake RESCAN link group.
- Added final acceptance and external dependency reports.
- Full CTest is not fully green; the final acceptance report records the remaining failing validation lanes without claiming clinical/hardware validation.

## Prompt 02 compile-time contract invariants

- `PreparedCommit` is move-only and cannot be copied or reused after submit.
- `INeuralChipTransport::submit` accepts `PreparedCommit&&`; the type test rejects direct submission of `EncodedModalities`.
- `SessionContext` contains const digest/id fields and is not assignable after creation.
- Time unit wrappers reject implicit integer conversion.
- `correlateAck` rejects ACKs whose session, frame, commit, or nonce differs from the submitted receipt.
- `closeWithFeedback` rejects closure when required external feedback lacks a digest.

## Prompt 03 bootstrap and ownership invariants

- Bootstrap validation runs before `RuntimeController` construction in `RuntimeApplication`.
- A valid `SessionBootstrapSnapshot` has exactly one owner, clock, committer, ACK handshake, emergency latch, and state machine.
- `SessionBackendLock` cannot switch between simulation and real/hardware modes after session creation.
- The canonical shutdown order is stop dispatch, hold/decompression, drain ACK/feedback, stop ingress, flush audit, then destroy transport.
- A real-minimal profile without complete backend attestation evidence fails closed before session construction.
