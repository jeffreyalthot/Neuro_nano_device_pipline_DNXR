# DNXR long-test guide

This project separates always-on checks from longer simulated and real-environment soak suites so CI can remain fast while still preserving repeatable stress coverage.

## Labels

- `unit`: deterministic, short, source-level unit checks.
- `runtime`: runtime-controller and runtime-stage checks.
- `stage`: isolated runtime stage tests.
- `long_fast`: larger deterministic runs that are still suitable for normal CI.
- `long_simulated`: simulated soak, stability, watchdog, and memory-growth tests.
- `long_real`: tests intended for real or extended validation contexts; disabled by default.
- `soak`: repeated-cycle or endurance-oriented tests.
- `memory`: memory-growth or allocation-stability checks.
- `concurrency`: multi-threaded or race-sensitive tests.
- `safety`: safety-governor, envelope, watchdog, or intervention checks.
- `full_dive`: full-dive synchronization and safety-bridge checks.

## Default local validation

```bash
cmake -S . -B build
cmake --build build -j1
ctest --test-dir build --output-on-failure
```

## Run only long simulated tests

```bash
cmake -S . -B build_long_sim -DDNXR_ENABLE_LONG_SIMULATED_TESTS=ON
cmake --build build_long_sim -j2
ctest --test-dir build_long_sim -L long_simulated --output-on-failure
```

## Enable long real tests

`long_real` tests are disabled unless explicitly requested:

```bash
cmake -S . -B build_long -DDNXR_ENABLE_LONG_REAL_TESTS=ON
cmake --build build_long -j2
ctest --test-dir build_long -L long_real --output-on-failure
```

Use `long_simulated` for routine pipeline regression and memory checks. Use `long_real` only for intentionally scheduled real/extended validation windows where runtime duration, operational risk, and environment assumptions have been reviewed.
