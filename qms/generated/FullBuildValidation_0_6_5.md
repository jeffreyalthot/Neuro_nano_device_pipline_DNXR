# Full Build Validation 0.6.5

Generated: 2026-06-11T19:41:21.903746+00:00

Human-use remains blocked; this runner only validates software evidence.

- `cmake -S . -B build -G Ninja -DDNXR_BUILD_TESTS=ON -DDNXR_ENABLE_CONTROLLER_10K_TEST=ON -DDNXR_ENABLE_CONTROLLER_100K_TEST=ON -DDNXR_ENABLE_LONG_SIMULATED_TESTS=ON -DDNXR_ENABLE_LONG_REAL_TESTS=ON` => 0
- `cmake --build build -j` => 0
- `ctest --test-dir build --output-on-failure` => 0
- `/root/.pyenv/versions/3.14.4/bin/python3 tools/check_neural_chip_build_invariants.py` => 0
- `/root/.pyenv/versions/3.14.4/bin/python3 tools/qms/check_required_docs.py` => 0
- `/root/.pyenv/versions/3.14.4/bin/python3 tools/qms/check_traceability_completeness.py` => 0
- `/root/.pyenv/versions/3.14.4/bin/python3 tools/qms/check_disabled_tests_impact.py` => 0
- `/root/.pyenv/versions/3.14.4/bin/python3 tools/hil/run_hil_bench_session.py --iterations 1000` => 0
- `/root/.pyenv/versions/3.14.4/bin/python3 tools/generate_neural_chip_evidence_bundle.py` => 0
