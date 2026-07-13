# DNXR Binary Dependency Scan

Prompt 01 executable truth update, 2026-07-13.

## Main executable linkage

`distributed_neural_xr` still links `dnxr_runtime_core`, `dnxr_real_adapters`, `dnxr_test_adapters`, and `dnxr_observer` in `DNXR_SIMULATION_ONLY` builds. This is intentionally recorded as not-final because `RuntimeController` still references observer rendering and simulation crypto symbols through the current runtime core.

## Warning-as-error scan repairs

The focused rebuild exposed and fixed warnings-as-errors in:

- `PipelineSafetyCaseReport.cpp`: added explicit handling for `TraceCoverageReject` and `ReentrantStepReject`.
- `ConsciousnessBridge.cpp`, `NeuralSafetyMonitor.cpp`, and `SocialPresenceEngine.cpp`: marked currently unused `deltaTime` parameters deliberately unused.
- `FiveSenseExposureLedger.cpp`: rewrote modality slot dispatch to handle every `FullDiveModality` enumerator.
- `NeuralStimulusSafetyEnvelope.cpp`: handled `SensoryModality::Gustatory` explicitly.
