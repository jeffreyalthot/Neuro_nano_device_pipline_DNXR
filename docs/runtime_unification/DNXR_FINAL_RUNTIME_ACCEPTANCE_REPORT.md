# DNXR Final Runtime Acceptance Report — Prompt 18

Date: 2026-07-13 UTC.

## Scope

This report records the final executable sweep performed for the canonical Distribution Neural XR runtime. The product architecture is the BCI middleware path between a structured virtual-world source and a neural chip transport. XR remains the distributed world experience, not a headset dependency.

## Acceptance Criteria

| Criterion | Status | Evidence |
|---|---:|---|
| Single production owner from `src/main.cpp` to transport | PASS | `RuntimeApplication` composes `RuntimeController`; `RuntimeController` executes the canonical stage sequence; focused build and architecture tests pass. |
| No headset/OpenXR/SteamVR/HMD dependency in real binary | PASS | Real build `DNXR_SIMULATION_ONLY=OFF` completed and `nm -C build/final-real/distributed_neural_xr` scan for forbidden production symbols returned no matches. |
| Simulation/HIL transports excluded from real binary | PASS | `SimulatedNeuralChipTransport` and `HILNeuralChipTransport` are compiled only under `DNXR_SIMULATION_ONLY`; real binary symbol scan returned no matches. |
| External ACK required | PASS | Canonical architecture guard and migrated neural-chip tests require injected ACK/feedback; no production auto-ACK is introduced. |
| Real backend fails closed without hardware attestation/protocol | PASS | `ClinicalLockedNeuralChipTransport` remains the non-simulation runtime transport; simulation requests in a real build throw `SIMULATION_TRANSPORT_NOT_COMPILED_IN_REAL_BUILD`. |
| Full CTest matrix green | FAIL | Full `ctest --test-dir build/final --output-on-failure` ran after a full build and reported 9 failures / 279 run before fixes; focused rerun fixed manifest, single-owner, services and TimeSync lanes, but remaining legacy/neural-chip ACK/controller smoke instability is recorded as open validation debt. |
| Zero unreviewed dispositions | PASS | Prompt 18 normalized the disposition manifests so no unreviewed or TBD-canonical markers remain in runtime-unification manifests. |
| Public symbol preservation | PASS WITH LIMITATIONS | No symbols were deleted in Prompt 18. Public telemetry support symbols missing from the manifest were added. Exhaustive semantic equivalence for all legacy paths remains limited by unavailable hardware/protocol. |

## Commands and Results

| Command | Result |
|---|---|
| `git status --short` | PASS; baseline captured before modification. |
| `rm -rf build/final && cmake -S . -B build/final -DDNXR_BUILD_TESTS=ON -DDNXR_WARNINGS_AS_ERRORS=OFF -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` | PASS. |
| `cmake --build build/final --target distributed_neural_xr dnxr_runtime_pipeline_runner_test dnxr_neural_chip_only_pipeline_test dnxr_canonical_runtime_architecture_guard_test -j2` | PASS. |
| `ctest --test-dir build/final -R 'dnxr_(runtime_pipeline_runner|neural_chip_only_pipeline|canonical_runtime_architecture_guard)_test' --output-on-failure` | PASS: 3/3. |
| `cmake --build build/final --target all -j2` | PASS. |
| `ctest --test-dir build/final --output-on-failure` | FAIL: 9 failed, 270 passed, 37 disabled. |
| `rm -rf build/final-real && cmake -S . -B build/final-real -DDNXR_BUILD_TESTS=ON -DDNXR_WARNINGS_AS_ERRORS=OFF -DDNXR_SIMULATION_ONLY=OFF -DDNXR_ENABLE_HARDWARE_BENCH=OFF -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` | PASS. |
| `cmake --build build/final-real --target distributed_neural_xr dnxr_runtime_realtime_60hz_telemetry_test -j2` | PASS. |
| `nm -C build/final-real/distributed_neural_xr | rg 'XRHeadsetInterface|OpenXR|SteamVR|HMD|SimulatedNeuralChipTransport|HIL|PipelineOrchestrator|runLegacy|NullNeuralSignalSource' || true` | PASS: no matches. |
| `ctest --test-dir build/final-real -R dnxr_runtime_realtime_60hz_telemetry_test --output-on-failure` | PASS. |

## Remaining FAIL Items

1. Full-matrix controller smoke/jitter tests can still trip nominal-runtime assertions under the current generated simulation profile and require a deterministic canonical fixture rather than a stochastic dashboard-facing fixture.
2. `dnxr_neural_chip_ack_external_injection_test` still represents a pre-sanitizer legacy unit that submits an under-specified world frame; it must be migrated to a canonical world-frame fixture with explicit modality safety/patient-map inputs.
3. Hardware validation, attestation verification and clinical readiness cannot be marked PASS without the external neural-chip protocol, hardware and signed evidence package.
