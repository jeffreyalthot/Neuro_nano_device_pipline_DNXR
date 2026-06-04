# DNXR long-test guide

This project separates always-on checks from longer simulated and real-environment soak suites so CI can remain fast while still preserving repeatable stress coverage.

## Labels

- `unit`: deterministic, short, source-level unit checks.
- `runtime`: runtime-controller and runtime-stage checks.
- `stage`: isolated runtime stage tests.
- `long_fast`: larger deterministic runs that are still suitable for normal CI.
- `long_simulated`: simulated soak, stability, watchdog, 10k controller, and memory-growth tests.
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

## Controller smoke versus 10k validation

`dnxr_runtime_controller_pipeline_smoke_test` is the default short controller loop and always runs 64 deterministic cycles.

`dnxr_runtime_controller_pipeline_10k_cycles_test` is now the unambiguous 10,000-cycle controller validation. It is labeled `long_simulated` and disabled in default CTest registration so normal validation remains quick. Enable `DNXR_ENABLE_CONTROLLER_10K_TEST` when the 10k lane should be scheduled through CTest, or run the executable directly for an ad hoc check.

```bash
ctest --test-dir build -R dnxr_runtime_controller_pipeline_smoke_test --output-on-failure
cmake -S . -B build_10k -DDNXR_ENABLE_CONTROLLER_10K_TEST=ON
cmake --build build_10k -j2
ctest --test-dir build_10k -R dnxr_runtime_controller_pipeline_10k_cycles_test --output-on-failure
./build/dnxr_runtime_controller_pipeline_10k_cycles_test
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

## Clinical-readiness validation lane

Use this lane when preparing an evidence package for expert preclinical/clinical-readiness review. It enables long simulated tests, long real/extended tests, controller 10k and 100k readiness tests, extended V&V tests, and the no-allocation guard in one explicit configuration.

```bash
cmake -S . -B build_readiness \
  -DDNXR_ENABLE_CLINICAL_READINESS_VALIDATION=ON
cmake --build build_readiness -j2
ctest --test-dir build_readiness --output-on-failure
```

Equivalent expanded options are:

```bash
cmake -S . -B build_readiness \
  -DDNXR_ENABLE_CONTROLLER_10K_TEST=ON \
  -DDNXR_ENABLE_CONTROLLER_100K_TEST=ON \
  -DDNXR_ENABLE_LONG_REAL_TESTS=ON \
  -DDNXR_ENABLE_EXTENDED_VV_TESTS=ON \
  -DDNXR_ENABLE_NO_ALLOCATION_GUARD=ON
```

The clinical-readiness lane must be accompanied by generated QMS outputs:

```bash
python3 tools/qms/generate_readiness_report.py
python3 tools/qms/generate_clinical_gap_report.py
python3 tools/qms/generate_extended_vv_report.py
```

Passing this lane does not make the repository clinical-test ready by itself; it provides a stronger software evidence bundle for independent preclinical, clinical, regulatory, cybersecurity, and physician review.
