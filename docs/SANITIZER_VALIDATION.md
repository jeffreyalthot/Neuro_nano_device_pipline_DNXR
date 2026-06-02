# Sanitizer validation

Date: 2026-06-02
Compiler: GNU 13.3.0 (`/usr/bin/c++`)

## AddressSanitizer / UndefinedBehaviorSanitizer

Preset: `asan`
Build directory: `build/asan`
Commands:

```bash
cmake --preset asan
cmake --build build/asan -j2
ctest --test-dir build/asan --output-on-failure
```

Result: PASS — 56/56 executed tests passed.

Disabled tests:

- `dnxr_vestibular_exposure_budget_long_test`
- `dnxr_full_dive_intervention_window_long_test`

Limitations: long real soak tests remained disabled by default because `DNXR_ENABLE_LONG_REAL_TESTS` is OFF.

## ThreadSanitizer

Preset: `tsan`
Build directory: `build/tsan`
Commands:

```bash
cmake --preset tsan
cmake --build build/tsan -j2
ctest --test-dir build/tsan --output-on-failure
```

Result: PASS — 56/56 executed tests passed.

Disabled tests:

- `dnxr_vestibular_exposure_budget_long_test`
- `dnxr_full_dive_intervention_window_long_test`

Limitations: long real soak tests remained disabled by default because `DNXR_ENABLE_LONG_REAL_TESTS` is OFF.
