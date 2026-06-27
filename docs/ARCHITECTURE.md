# Architecture

Key folders:

- `src/app`: CLI bootstrap and runtime application loop.
- `src/config`: runtime configuration and mode parsing.
- `src/core`: kernel, pipeline state machine, orchestrator, execution diagnostics.
- `src/performance`: low-latency metrics, profiling, frame budgets, benchmarks.
- `src/replay`: replay recording/verification plus consistency reports.
- `src/safety`: explicit safety decisions, audit trail, emergency latch controller.
- `tests`: unit, integration, runtime, replay, safety, and performance checks.
