# DNXR Runtime Unification Worklog

## Prompt 01 forensic audit freeze — 2026-07-13

- Git state captured before modification with `git status --short` (clean output).
- Repository inventory captured: 1600 non-git files, 1223 files under `src/` and `tests/`.
- Preservation manifests generated for every detected `src/` and `tests/` file and every regex-detectable public class/struct/enum/function symbol (3232 symbols).
- No source/runtime files deleted or moved in this prompt.

## Prompt 02 canonical BCI runtime specification — 2026-07-13

- Added `DNXR_CANONICAL_RUNTIME_ARCHITECTURE.md` and encoded the canonical transaction/stage contracts in `src/runtime/CanonicalNeuralBciPipeline.h`.
- Replaced two-runtime path names (`SecondarySimulation`, `PrimaryReal`) with canonical environment adapter names in `RuntimePathLock`.
- Added `dnxr_canonical_runtime_architecture_test` to enforce stage order, dispatch preconditions, no auto-ACK, no headset/renderer include in the canonical contract, and no legacy two-runtime names in the canonical header.

## Prompt 03 skeleton fusion and unique owner — 2026-07-13

- `RuntimeController` now owns one `CanonicalNeuralBciPipeline` instance and delegates deterministic cycles through `CanonicalNeuralBciPipeline::runCycle`; the compatibility `RuntimePipelineRunner` is held inside the canonical pipeline as a temporary adapter, not as a controller-owned runtime.
- `CanonicalNeuralBciPipeline` now owns the global skeleton state machine and mature neural-chip lifecycle components: `FrameLifecycleGuard`, `NeuralChipAckHandshake`, `NeuralChipPipelineStateGate`, `AckHoldFrameFactory`, `SafeChipFrameHoldBuffer`, `NeuralChipFrameCommitter`, `PipelineResumeController`, `EmergencyLatchController`, replay recorder, session digest and decision trace.
- `BackendAdapterLock` replaces `RuntimePathLock` as the backend/adapters lock; the old type name remains only as a deprecated compile-time alias for test/source compatibility and has no parallel state.
- Added `dnxr_canonical_runtime_single_owner_test` to prove exactly one canonical pipeline instance in `RuntimeController`, monotonic frame progression through one owner, one delegated compatibility cycle per frame, one commit/dispatch/metric record per cycle, and no adapter fallback.

## 2026-07-13 — Prompt 04 headset functional dependency removal
- Removed the canonical `RuntimeController` construction and use of `XRHeadsetInterface`; biometric acquisition now uses the canonical `IBiometricSource` contract with timestamp, quality, provenance, and validity. No unavailable biometric source synthesizes fallback values.
- Split the render stage into authoritative neural sensory preparation plus an optional `ObserverMirrorSink` that receives an immutable copy after preparation and watchdog checks; observer publication failure is diagnostic-only and does not decide neural chip dispatch.
- Requalified `XRRenderer` usage as an observer mirror implementation detail (`observerRenderer_`), no longer as a canonical safety or chip-dispatch participant.
- Replaced mandatory headset runtime JSON keys with `virtual_world_ingress`, `neural_chip_transport`, `observer_mirror`, and `biometric_source`; legacy headset keys are logged and ignored.
- Added regression coverage proving no canonical construction/calls of `XRHeadsetInterface`, `pullBioTelemetry`, or `pushSensoryFrame` remain in the controller/stage path.

## 2026-07-13 — Prompt 05 canonical BCI acquisition read foundation
- Replaced the empty `INeuralSignalSource`/Null source with a HAL-backed canonical neural source that performs exactly one acquisition read per cycle and validates sequence, monotonic timestamp, signal quality, and non-empty channel payload before exposing raw channels.
- `RuntimeAcquisitionStage` now captures one immutable `NeuralChipFrame` into both `chipFrame` and `acquisitionChipFrame`; it no longer reads `NanoNodeInterface` and then `INeuralChipHAL::readFrame` as competing acquisition sources.
- `RuntimePipelineRunner` no longer re-reads the chip before full-dive processing or feedback bookkeeping; downstream stages consume the canonical acquisition frame captured at cycle start.
- Canonical signal processing now avoids local protect/unprotect round-trips as pseudo-transport security; authenticated/decrypted payloads are expected at the neural source boundary and raw neural channels are not logged.
- Added acquisition regression checks for single read count, replay/duplicate sequence fail-closed behavior, and explicit no-biometric operation without synthetic telemetry.

## 2026-07-13 — Prompt 06 canonical virtual-world ingress
- Added a versioned `CanonicalVirtualWorldFrame` composed of existing view, visual field, audio, touch, body, vestibular, chemical, motor, reality-anchor, scene contract/risk, clinical scene flags, and body safety structures; no raw pixel/audio payload fields are admitted.
- Reworked `VirtualWorldDataIngress` into a transport-agnostic ingress gate with schema version, declared size, sequence, timestamp, provenance, optional authentication, stale/future timing, scene risk, clinical-neutral, body-safety, and chemical-channel policy checks before a world frame reaches sanitization/encoders.
- Integrated `WorldFrameSanitizer`, `SceneRiskAnnotator`, `ClinicalNeutralSceneController`, and `VirtualBodySafetyContract` before encoder-facing world output; failures produce deterministic hold/reality-anchor frames with risky modalities neutralized.
- Preserved legacy `VirtualWorldIngressFrame` ingestion as a compatibility adapter while adding canonical `ingestCanonical` for production path tests.

## Prompt 07 — visual perception to spikes
- Added versioned visual topology validation to preserve visual grid/channel/foveation/surround/compression parameters instead of collapsing them into a generic vector.
- Extended canonical `VirtualSceneFrame` with world-simulation gaze/attention as non-HMD structured world data.
- Wired semantic extraction, cortical encoding, visual compliance, per-modality visual isolation, and phosphene stabilization into the chip pipeline before vector-to-spike conversion.
- Added an end-to-end canonical visual BCI spike-path regression test covering world visual fields, patient map, neutral disabled visual output, topology limits, flash hold, deterministic spike output, bounded spike load, and PPP visual grammar.

## Prompt 08 — non-visual modalities and channel priority
- Added an end-to-end multimodal regression covering audio, touch, proprioception, vestibular, olfactory, gustatory, modality vector adapters, stimulus-by-spike runtime, chemical neutralization, capability/disabled-modality neutrality, phase lock, exposure ledger and safety envelope.
- Fixed spike-lane emission to use deterministic safety-prioritized modality order so reality anchor/stop-related lanes survive global channel pressure.
- Verified the multimodal canonical test and sense encoder/spike-priority files do not depend on headset/render symbols.

## Prompt 09 — spike mapping and PPP grammar
- Extended `PPPGrammarFrame` with deterministic topology metadata for electrodes, wires, phase/risk per modality, and an integrity hash.
- Added fixed-capacity PPP serialization/parse/verify helpers with no raw neural payload logging.
- Updated `StimulusBySpikeRuntime` so PPP feedback-only state becomes a lifecycle hold unless global sync, ACK and feedback conditions unlock a normal stimulus candidate.
- Added `dnxr_ppp_grammar_serialization_test` for deterministic PPP serialization/parse, topology bounds, tamper rejection, feedback-only hold semantics and integrity verification.

## Prompt 10 — transport/HAL/ACK boundary
- Replaced the empty `INeuralTransport` shim with the concrete `INeuralChipTransport` lifecycle abstraction.
- Expanded transport operations to model discovery, connection, attestation, negotiation, clock sync, authenticated session, read, prepare/submit, external ACK, feedback, emergency stop and close.
- Removed the previous real transport fabricated success path; real transport now fails closed when no concrete protocol driver is available.
- Added deterministic transport lifecycle coverage for attestation rejection, capability negotiation, anti-replay, ACK timeout, duplicate/late ACK, feedback, emergency cutoff, no double write, and real-backend fail-closed behavior.

## Prompt 11 — Integrated safety lifecycle hierarchy

- Extended the canonical safety pipeline trace with explicit decision levels: local encoder, modality, cross-modal, session, transport/hardware, and post-feedback. Aggregation remains monotonic: later `Continue` decisions cannot clear a prior hold/decompression/emergency, and the final audit record preserves the first highest-severity cause plus all merged layer decisions.
- Expanded `SafetyStateMachine` to expose the canonical lifecycle states required for boot/evidence/connection/calibration/ready/running/ACK/hold/decompression/controlled-resume/emergency/shutdown while preserving legacy enum aliases for existing callers during migration.
- Emergency reset semantics now resume into `ControlledResume` instead of direct `Running`; tests must explicitly pass through controlled resume before normal running resumes.
- Added regression coverage for merged safety decisions, audited transition metadata, required lifecycle gates, and reset-through-controlled-resume behavior.

## Prompt 12 — Closed-loop motor/world/feedback chain

- Added a canonical `MotorIntentCommand` output contract carrying frame causality (`frameId`, `validFromFrameId`, `validUntilFrameId`), confidence, provenance, patient binding, safety disposition, rig command, reason, and a non-sensitive command hash.
- Routed chip BCI acquisition through the existing motor cortex adapter, decoder, confidence gate, avatar rig/gait/balance/IK, and safety-pose controller before submitting the command to a world-engine sink; no renderer or headset interface is in this path.
- Added a distinct feedback read on `NeuralChipBus` and changed closed-loop correction to use feedback for the previously submitted frame, not the current BCI acquisition read. Missing, stale, or low-quality feedback forces hold before the next normal dispatch.
- Added multi-cycle coverage for frame N intent -> command N -> world N+1 -> feedback/correction N+1, feedback-absent hold, low-confidence safety pose, and renderer/headset non-authority.

## Prompt 13 — Repository-wide transverse subsystem sweep

- Added `dnxr_runtime_disposition_manifest_audit_test`, a meta-audit that walks `src/`, requires every source/header file to appear in `DNXR_FILE_DISPOSITION_MANIFEST.csv`, and requires every public class/struct/enum declaration found in headers to have a symbol-preservation entry.
- Completed pairwise public type preservation additions for the remaining headers discovered by the meta-audit; the audit reports coverage for 920 source files, 477 headers, and 1168 public type declarations.
- Classified transverse subsystems by runtime role: canonical per-frame services, off-frame support snapshots/tools, HIL/bench adapters, clinical/QMS/readiness gates, and gated research/speculative libraries.
- Verified the normal build and a research-enabled optional build compile without creating a second production runtime entry point.

## Prompt 14 — controlled removal of legacy/concurrent production runtime paths

- Removed `RuntimePipelineRunner::LegacyRunInput`, `runLegacy`, and the private `finishLegacyCycle` path after migrating the runner coverage to canonical `RuntimePipelineRunInput`/`RuntimePipelineRunInputRef` and `RuntimeController`/`CanonicalNeuralBciPipeline` harnesses.
- Clarified `DNXR_ENABLE_SIMULATION` as adapters/tools only and excluded simulation/HIL/headset/observer renderer sources from `DNXR_REAL_RUNTIME_SRC`; real builds no longer link `XRHeadsetInterface`, `XRRenderer`, simulation crypto transport, HIL controller, or secondary runtime symbols.
- Kept observer mirroring read-only: `XRRenderer` attachment/presentation is simulation-only, while real runtime publish attempts fail closed with an observer-specific diagnostic and cannot authorize neural dispatch.
- Added `dnxr_runtime_single_pipeline_structure_test` to detect legacy runner APIs, secondary runtime path kinds, real-runtime observer/headset source inclusion, and production-scope references to obsolete legacy/fallback names.
- Evidence: default build and targeted runtime tests passed; real (`DNXR_SIMULATION_ONLY=OFF`) build passed; real binary forbidden-symbol scan returned no matches.

## Prompt 15 — CMake/config/deployment profile stabilization (2026-07-13)

- Captured the pre-change Git state and continued from the existing runtime-unification artifacts.
- Introduced a controlled CMake source manifest and target split for runtime core, real adapters, observer, test adapters, HIL, tools, and speculative modules.
- Added non-human reproducible CMake presets and preserved signed evidence gates; no preset enables human use or invents authorization.
- Added a strict unified runtime profile schema plus a development-safe profile with no headset ingress, external-only secrets, mTLS/pinning/anti-replay requirements, bounded packets/timeouts, immutable startup configuration, and raw neural payload logging disabled.
- Generated a source/config/binary SHA-256 manifest for build evidence.
- Open risk: `dnxr_observer` is still linked into the main binary because `RuntimeController`/`ObserverMirrorSink` retain `XRRenderer` symbols; it is documented as read-only observer support and must be split behind an observer interface in the next prompt to remove headset-named objects from the main link line.
- Open risk: `DNXR_WARNINGS_AS_ERRORS=ON` works as a lane but exposes legacy warnings; default reproducible presets keep warnings visible while not failing until targeted cleanup is complete.

## Prompt 16 — realtime performance, 60 Hz, memory and concurrency (2026-07-13)

- Added canonical realtime telemetry with 60 Hz/16.666 ms default rate contract and hardware negotiation rather than tying observer/display cadence to the BCI/world cycle.
- Instrumented the canonical owner (`CanonicalNeuralBciPipeline`) with monotonic-clock cycle timing and fixed-size per-stage windows for p50/p95/p99/max/WCET plus jitter, queue depth, dropped world-frame and held chip-frame cause counters.
- Added a bounded keep-latest queue utility for world/transport backpressure so stale frames are not accumulated when producers outpace the chip cycle.
- Kept observer/audit logging out of the hot telemetry path; the realtime telemetry records counters and const cause tokens without per-frame synchronous log calls.
- Open risk: current stage durations are attributed across canonical stage order around the compatibility stage runner until each stage is migrated from the adapter into directly measured canonical stage calls.

## Prompt 17 — test migration, traceability and proof sweep (2026-07-13)

- Captured pre-change Git state with `git status --short` before edits; no tracked output was emitted by the command in this environment.
- Added `dnxr_canonical_runtime_architecture_guard_test` as a static/contract regression for the canonical BCI runtime harness.
- Rebuilt the test migration matrix from the CMake `DNXR_TEST_TARGETS` inventory so every declared CTest target has an explicit component, legacy path, canonical harness, command, result field, and requirement coverage.
- Migrated the validation narrative for `RuntimePipelineRunner` and `NeuralChipOnlyFullDivePipeline` tests to a common canonical harness classification rather than deleting or silencing those tests.
- Recorded strict-warning build blockers found while compiling the focused lane and added minimal non-behavioral warning fixes for touched runtime/chip files.


## Prompt 18 final sweep — 2026-07-13 UTC

- Rebuilt focused canonical runtime and migrated runner/chip tests in `build/final`.
- Rebuilt the real configuration in `build/final-real` with `DNXR_SIMULATION_ONLY=OFF` and confirmed no forbidden headset, legacy runner, HIL, simulated transport, or null neural source symbols in the production binary symbol scan.
- Converted simulation/HIL neural-chip transport declarations and definitions to simulation-only compile units so the real binary cannot link them accidentally.
- Fixed the canonical runtime aggregate link closure using a CMake RESCAN link group.
- Added final acceptance and external dependency reports.
- Full CTest is not fully green; the final acceptance report records the remaining failing validation lanes without claiming clinical/hardware validation.

## Prompt 01 executable-truth CLI rebaseline — 2026-07-13

Initial git state observed on branch `work` at `32cb0aded536528281cef679f525515eda4ec0fb` before this change set. This update adds an early `RuntimeCli` parser so `--help`, `--version`, syntax errors, and `validate-config` return before runtime-controller construction. Active startup is now explicit via `run --profile <profile> --config <path>`.

Build evidence: `cmake -S . -B build -DDNXR_BUILD_TESTS=ON -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` passed. `cmake --build build --target distributed_neural_xr dnxr_cli_side_effect_contract_test -j2` passed after repairing warning-as-error findings. `ctest --test-dir build -R 'dnxr_cli_side_effect_contract_test' --output-on-failure` passed. `./build/distributed_neural_xr --help` returned code 0 and did not initialize runtime modules.

Open P0 evidence: the canonical pipeline still delegates through `RuntimePipelineRunner`, and the simulation executable still links observer/test-adapter libraries pending the dedicated observer/core split prompts.

## Prompt 02 final transaction contract — 2026-07-13

Starting state: branch `work`, HEAD `5991c81` from Prompt 01. Prompt 02 adds a compilable canonical runtime transaction contract in `src/runtime/CanonicalBciRuntimeContract.h`, type-level transaction tests, a coverage matrix, and an ADR explaining why `RuntimePipelineRunner` and `NeuralChipOnlyFullDivePipeline` are migration sources rather than final APIs.

No runtime business implementation was migrated in this prompt. P0 implementation gaps remain open for ownership transfer in Prompt 03+.

## Prompt 03 composition root ownership — 2026-07-13

Starting state: branch `work`, HEAD `f4c2849`. Added `SessionBootstrapValidator`, immutable `SessionBootstrapSnapshot`, `SessionBackendLock`, and `CanonicalRuntimeHost`. `RuntimeApplication` now runs bootstrap validation before constructing the legacy `RuntimeController`, preventing known-invalid configs from partially initializing runtime state.

The full stage logic is not yet moved out of `RuntimeController`; that remains an explicit P0 migration gap. The new bootstrap test validates unique ownership counters, immutable snapshots, backend lock irreversibility, real-profile fail-closed attestation, and destruction order.

## 2026-07-13 — Prompt 04 canonical transaction direct stage graph

Initial state recorded before edits:
- Branch/status command: `git status --short --branch` returned `## work`.
- Commit command: `git rev-parse HEAD` returned `ddfabaaa9389b0af374c052aff3ffdfd0bae043f`.
- Runtime unification artifacts were read from `docs/runtime_unification/` before implementation.

Implementation summary:
- Replaced `CanonicalNeuralBciPipeline::runCycle` whole-cycle `RuntimePipelineRunner` delegation with explicit canonical stage invocations.
- Added a constexpr stage registry carrying ID, inputs, outputs, nanosecond budget, error disposition, and ownership.
- Added per-stage bitset coverage/order enforcement, real invocation timing records, and cumulative non-raw-neural trace digest mixing.
- Kept dispatch closed at `DispatchTransport` with `certified_transport_not_bound_prepared_commit_held`; no HAL commit/ACK/feedback fabrication is introduced.

## 2026-07-13 — Prompt 05 RuntimePipelineRunner facade removal

Initial state recorded before edits:
- `git status --short --branch` returned `## work`.
- `git rev-parse HEAD` returned `60754350b4b843c43b804c518ac205e6d5318c3e`.
- All files under `docs/runtime_unification/` were read into `/tmp/runtime_unification_all_prompt05.txt` before implementation.

RuntimePipelineRunner disposition enumeration and migration proof:
- `StageInputs` -> `RuntimePipelineStageInputs` in `RuntimePipelineRunInput.h`; canonical target is direct stage services/inputs, proof: header no longer depends on `RuntimePipelineRunner.h`.
- `RuntimePipelineRunInput` -> top-level `RuntimePipelineRunInput`; canonical target is controller-to-canonical transaction input snapshot.
- `RuntimePipelineRunInputRef` -> top-level `RuntimePipelineRunInputRef`; canonical target is direct `CanonicalNeuralBciPipeline::runCycle` input.
- `run(const RuntimePipelineRunInput&)` and `run(const RuntimePipelineRunInputRef&)` -> not linked to runtime core; canonical target is `CanonicalNeuralBciPipeline::runCycle` direct graph.
- `normalizedTelemetryRiskVector` -> absorbed by direct stage normalization metadata path; no production symbol remains.
- `finishModernCycle` -> absorbed by direct canonical finalizer calls in `CanonicalNeuralBciPipeline::runCycle`.
- Historical render stage call -> removed from runtime core and linked only into its standalone test target.

## 2026-07-13 — Prompt 06 external BCI acquisition ingress

Initial state recorded before edits:
- `git status --short --branch` returned `## work`.
- `git rev-parse HEAD` returned `1811d1f8809eac005bf37b9e1e4bd4cb064dacd5`.
- All files under `docs/runtime_unification/` were read into `/tmp/runtime_unification_all_prompt06.txt` before implementation.

Implementation summary:
- Added `IBciAcquisitionSource` as the canonical chip/driver-owned neural acquisition boundary.
- Replaced `INeuralSignalSource`/`HalNeuralSignalSource` service ownership with `IBciAcquisitionSource` in composition and stage services.
- Added acquisition metadata: device identity, session, sequence, hardware/host timestamps, clock domain, sample layout, topology digest, quality flags, dropout/overflow counters, packet digest, authentication tag, external/attested/authenticated booleans, and fixture marking.
- Added stateful validation for duplicate/replay/gap, stale/future/skew/cadence, topology/session mismatch, length mismatch, dropout/overflow, corrupt digest, unauthenticated/unattested/non-external frames.
- Added a fixed-capacity ring buffer with explicit drop count for acquisition backpressure evidence.

## Prompt 07 — Authenticated deterministic virtual world ingress (2026-07-13)
- Initial state: branch `work`, commit `445f38632dbd82822183442b3034e4b637dc8906`; `docs/runtime_unification/*` read before code changes.
- Implemented the virtual-world ingress boundary as the authoritative world source for the canonical cycle. `CanonicalNeuralBciPipeline::AcquireWorld` now reads `IWorldIngress`, stores an immutable `CanonicalVirtualWorldFrame` snapshot on the transaction frame, records a digest, and fail-closes on absent/rejected world ingress instead of constructing a world ad hoc.
- Removed `ImmersionCore` from the runtime-core critical source list and from `RuntimeController`/`RuntimeStageServices`; sensory encoding now derives its vector from the authenticated canonical world snapshot.
- Added deterministic/source provenance tests for version negotiation, replay, corruption, sequence gap, unresolved asset, invalid transform, finite/range validation, oversized payload, disconnect hold, and deterministic replay digest.

## Prompt 08 — Causal signal conditioning and motor intent decoding (2026-07-13)
- Initial state: branch `work`, commit `40c4d4d84bf7da6ae33a564fa0b8a268f9664de1`; `docs/runtime_unification/*` read before changes.
- Added `RuntimeMotorIntentStage` to decode bounded motor intent only from validated external BCI acquisition plus canonical signal/inference snapshots. The stage carries a signed deterministic model identity, confidence/uncertainty/provenance, and neutral hold defaults.
- Integrated motor decoding after signal conditioning/inference and before sensory encoding; motor intent is submitted only to `IMotorWorldEgress` and never to sensory electrode encoding.
- Neutralized runtime `inducedVector` by default; future influence activation remains outside the canonical runtime and requires a distinct signed profile/consent gate.
