# Research Specialization Report

Internet research was performed for grouped file domains rather than repeating identical searches per C++ translation unit.

| Files / group | Domain | Research performed | Practices retained | Changes / validation |
|---|---|---|---|---|
| `src/config/*RuntimeMode*`, `src/config/*FullUnlocked*` | Runtime mode configuration | Searched modern runtime configuration and CMake/test mode patterns. | String-normalized modes, explicit capabilities, no hidden unlocks. | Added typed mode parsing and Full Unlocked capability description; validated by `runtime_mode_test`. |
| `src/core/Pipeline*`, `src/core/RuntimeDiagnostics*` | Orchestration, state machine, diagnostics | Searched low-latency C++ runtime pipeline/state-machine practices. | Explicit legal transitions, no silent rejection, diagnostics outside hot path. | Added orchestrator, state machine, step results, diagnostic reports; validated by `pipeline_state_machine_test` and `full_unlocked_runtime_test`. |
| `src/performance/*` | Low-latency profiling/benchmark | Searched C++ low-latency metrics, hot path, benchmark practices. | Steady clock, minimal counters, separated benchmark runner, frame-budget monitor. | Added metrics/profiler/benchmark/hot-path diagnostics; validated by `performance_smoke_test`. |
| `src/safety/*` | Safety decisions and emergency latch | Searched software emergency stop latch and audit-trail best practices. | Preserve latch, require audited reset, explain rejection with expected/received/action. | Added `SafetyDecision`, `SafetyAuditTrail`, `EmergencyLatchController`; validated by `safety_decision_test`. |
| `src/replay/ReplayConsistencyReport*` | Deterministic replay divergence | Searched event-sourcing replay and divergence detection patterns. | Explicit consistency result and expected/actual mismatch explanation. | Added replay consistency report; validated by `replay_consistency_test`. |
| `CMakeLists.txt`, `tests/*` | Modern CMake and tests | Searched modern CMake project/test layout practices. | Target-based tests through existing helper, focused regression suite. | Registered focused Full Unlocked tests. |

Sources consulted include Microsoft Azure Architecture Center on Event Sourcing, Modern CMake references, audit trail best-practice articles, and low-latency C++ design discussions.
