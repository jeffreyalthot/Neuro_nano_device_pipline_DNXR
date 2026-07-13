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
