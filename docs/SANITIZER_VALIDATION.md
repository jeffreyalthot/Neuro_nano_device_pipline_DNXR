# Sanitizer validation

Last validated: 2026-06-01

## AddressSanitizer + UndefinedBehaviorSanitizer

```bash
cmake --preset asan
cmake --build build/asan -j2
ctest --test-dir build/asan --output-on-failure
```

Result: 51 active tests passed. The 5 `long_real` tests remained disabled by default.

## ThreadSanitizer

```bash
cmake --preset tsan
cmake --build build/tsan -j2
ctest --test-dir build/tsan --output-on-failure
```

Result: 51 active tests passed. The 5 `long_real` tests remained disabled by default.

## Long-real suite

```bash
cmake -S . -B build_long -DDNXR_ENABLE_LONG_REAL_TESTS=ON
cmake --build build_long -j2
ctest --test-dir build_long -L long_real --output-on-failure
```

Result: 5/5 `long_real` tests passed.
