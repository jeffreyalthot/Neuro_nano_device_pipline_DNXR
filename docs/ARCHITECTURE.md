# Architecture

Key folders:

- `src/app`: CLI bootstrap and runtime application loop.
- `src/config`: runtime configuration and mode parsing.
- `src/core`: kernel, pipeline state machine, orchestrator, execution diagnostics.
- `src/runtime` and `src/full_dive/spike_bridge`: primary middleware runtime plus the secondary spike-vector runtime; see `docs/NEURAL_CHIP_MIDDLEWARE_INTEGRATION.md` for where `.ppp` grammar construction and neural-chip communication adapters belong.
- `src/immersion/VirtualWorldDataIngress.*`: typed entry point for 3D world, visual, audio, touch, proprioceptive, vestibular, chemical, motor-intent, and reality-anchor data; see `docs/VIRTUAL_WORLD_DATA_INGRESS.md`.
- `src/full_dive/motor_chip`: neural feedback driven avatar motor control; see `docs/AVATAR_NEURAL_FEEDBACK_CONTROL.md`.
- `src/full_dive/visual_chip`: visual-world to neural-vector encoding and QMS visual compliance decisions for DNXR 0.7.1; see `docs/RELEASE_0_7_1_QMS_NDA_PACKAGE.md`.
- `src/performance`: low-latency metrics, profiling, frame budgets, benchmarks.
- `src/replay`: replay recording/verification plus consistency reports.
- `src/safety`: explicit safety decisions, audit trail, emergency latch controller.
- `tests`: unit, integration, runtime, replay, safety, and performance checks.
