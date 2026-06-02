# Sanitizer validation

Date: 2026-06-02
Compiler: GNU 13.3.0 (`/usr/bin/c++`)

## 0.3.3 verified baseline

The 0.3.3 runtime refactor was validated with the normal CMake workflow before this
0.3.4 development pass:

```bash
cmake -S . -B build
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

Result: PASS — 63/63 active CTest tests passed.

Disabled long-real tests:

- `dnxr_vestibular_exposure_budget_long_test`
- `dnxr_full_dive_intervention_window_long_test`

The sanitizer presets configured successfully for 0.3.3:

```bash
cmake --preset asan
cmake --preset tsan
```

The full ASAN/TSAN build-and-test commands below still need to be re-run after the
0.3.4 controller and `RuntimeStageServices` changes because the active test count
has changed and the controller loop is now directly covered by CTest.

## 0.3.4 normal validation

After the `RuntimeStageServices` migration wave and RuntimeController tests were
added, the normal CTest lane was re-run:

```bash
cmake -S . -B build -DDNXR_BUILD_TESTS=ON
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

Result: PASS — 66/66 active tests passed out of 68 registered tests.

Disabled long-real tests:

- `dnxr_vestibular_exposure_budget_long_test`
- `dnxr_full_dive_intervention_window_long_test`

## AddressSanitizer / UndefinedBehaviorSanitizer

Preset: `asan`
Build directory: `build/asan`
Commands:

```bash
cmake --preset asan
cmake --build build/asan -j2
ctest --test-dir build/asan --output-on-failure
```

Latest recorded full sanitizer result: PASS — 56/56 executed tests passed.

Current required follow-up for 0.3.4: re-run the commands above and replace the
recorded count with the new CTest total that includes the RuntimeController tests.

## ThreadSanitizer

Preset: `tsan`
Build directory: `build/tsan`
Commands:

```bash
cmake --preset tsan
cmake --build build/tsan -j2
ctest --test-dir build/tsan --output-on-failure
```

Latest recorded full sanitizer result: PASS — 56/56 executed tests passed.

Current required follow-up for 0.3.4: re-run the commands above and replace the
recorded count with the new CTest total that includes the RuntimeController tests.

## Long-real validation lane

The following tests remain disabled by default and should be validated in a
separate long-real lane when that environment is available:

```bash
cmake -S . -B build_long -DDNXR_ENABLE_LONG_REAL_TESTS=ON
cmake --build build_long -j2
ctest --test-dir build_long -L long_real --output-on-failure
```
