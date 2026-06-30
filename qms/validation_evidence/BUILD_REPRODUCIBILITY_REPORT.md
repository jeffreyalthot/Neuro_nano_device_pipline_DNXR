# Build Reproducibility Report

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Evidence boundary: preclinical R&D software evidence only; not clinical validation or human-use authorization.

## Reference commands
```bash
cmake -S . -B build_readiness -DDNXR_ENABLE_CLINICAL_READINESS_VALIDATION=ON
cmake --build build_readiness -j2
ctest --test-dir build_readiness --output-on-failure
```

## 0.6.6 clinical-readiness run completed
- Configure: `cmake -S . -B build_readiness -DDNXR_ENABLE_CLINICAL_READINESS_VALIDATION=ON`
- Build: `cmake --build build_readiness -j2`
- Test: `ctest --test-dir build_readiness --output-on-failure`
- Result: configure PASS, build PASS, CTest PASS with 289/289 tests passing.
- Raw build log: `qms/generated/FULL_BUILD_LOG_0_6_6.md`.
- Raw CTest report: `qms/generated/FULL_CTEST_REPORT_0_6_6.md`.

## Sanitizer lanes to execute for final release evidence
```bash
cmake -S . -B build_asan -DDNXR_ENABLE_ASAN=ON
cmake -S . -B build_ubsan -DDNXR_ENABLE_UBSAN=ON
cmake -S . -B build_tsan -DDNXR_ENABLE_TSAN=ON
```
