# ExtendedVVReport

## Clinical-readiness validation lane
```bash
cmake -S . -B build_readiness -DDNXR_ENABLE_CLINICAL_READINESS_VALIDATION=ON
```
```bash
cmake --build build_readiness -j2
```
```bash
ctest --test-dir build_readiness --output-on-failure
```

## Critical tests covered by this lane
- dnxr_runtime_controller_pipeline_10k_cycles_test
- dnxr_vestibular_exposure_budget_long_test
- dnxr_full_dive_intervention_window_long_test
- dnxr_runtime_controller_100k_cycles_test

## Acceptance rule
All enabled tests must pass, generated readiness/gap reports must be attached, and any skipped long test must have a documented clinical-readiness impact assessment.
