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
