# Build Reproducibility Report

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Evidence boundary: preclinical R&D software evidence only; not clinical validation or human-use authorization.

## Reference commands
```bash
cmake -S . -B build -DDNXR_BUILD_TESTS=ON -DDNXR_SIMULATION_ONLY=ON
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

## Due-diligence run completed
- Configure: `cmake -S . -B /tmp/neuro_build_dd -DDNXR_BUILD_TESTS=ON -DDNXR_SIMULATION_ONLY=ON`
- Build: `cmake --build /tmp/neuro_build_dd -j2`
- Test: `ctest --test-dir /tmp/neuro_build_dd --output-on-failure`
- Result: configure PASS, build PASS, CTest PASS.
- Raw build log: `qms/generated/FULL_BUILD_LOG_0_6_5.md`.
- Raw CTest report: `qms/generated/FULL_CTEST_REPORT_0_6_5.md`.

## Sanitizer lanes to execute for final release evidence
```bash
cmake -S . -B build_asan -DDNXR_ENABLE_ASAN=ON
cmake -S . -B build_ubsan -DDNXR_ENABLE_UBSAN=ON
cmake -S . -B build_tsan -DDNXR_ENABLE_TSAN=ON
```
