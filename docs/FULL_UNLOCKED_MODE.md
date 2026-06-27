# Full Unlocked Mode

`full_unlocked_mode` is an inspection-first runtime mode for controlled development, simulation, replay, benchmark, and diagnostic work. It activates software modules that are safe to execute without human-use authorization while keeping critical physiological and emergency interlocks auditable.

## Activates
- Runtime diagnostics for pipeline decisions, vectors, latency, safety decisions, calibration decisions, replay, and benchmark summaries.
- Simulation, replay, and benchmark switches in `RuntimeConfig`.
- Explicit rejection reasons instead of silent holds.
- Pipeline orchestrator diagnostics and frame-budget reports at shutdown.

## Does not disable
- Emergency latch behavior.
- Physiological guard decisions.
- Signed evidence requirements for human-use or clinical trial builds.
- Simulation-only CMake guardrails unless explicitly configured for hardware bench with evidence.

## Commands
```bash
cmake -S . -B build/full-unlocked -DCMAKE_BUILD_TYPE=Release -DDNXR_BUILD_TESTS=ON
cmake --build build/full-unlocked -j
./build/full-unlocked/distributed_neural_xr --full-unlocked --headless --cycles=10
ctest --test-dir build/full-unlocked -R '^(runtime_mode_test|full_unlocked_runtime_test|pipeline_state_machine_test|safety_decision_test|performance_smoke_test|replay_consistency_test|timebase_sync_test)$' --output-on-failure
```

## Risks
This mode exposes internals and enables broad simulated execution. Use only for controlled development, validation, and non-human test environments.
