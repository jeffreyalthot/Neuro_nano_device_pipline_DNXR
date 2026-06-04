# ReadinessLaneTranscript

## Scope

This transcript records the 0.4.3 clinical-readiness lane execution performed on 2026-06-04 UTC after versioned HIL artifact generation and runtime 10k evidence separation.

## Commands executed

```bash
cmake -S . -B build_readiness -DDNXR_ENABLE_CLINICAL_READINESS_VALIDATION=ON
```

```bash
cmake --build build_readiness -j2
```

```bash
ctest --test-dir build_readiness --output-on-failure
```

## Result

- CTest tests registered: 107
- CTest tests executed: 107
- CTest failures: 0
- Disabled tests in readiness lane: 0
- Total CTest time: 42.00 seconds
- Overall status: PASS

## Runtime evidence generated

- Bounded runtime evidence: `qms/generated/Runtime10kBoundedEvidence.md`
- Full runtime evidence: not generated; `DNXR_CONTROLLER_10K_FULL=1` was not set during this bounded readiness execution.

## Clinical-readiness interpretation

The readiness lane executed completely in bounded mode and produced bounded 10k projection evidence. This does not satisfy the full 10k execution evidence requirement and must not unblock ClinicalTestReady without a later full-mode soak run, physical HIL evidence, named sign-off, and cybersecurity/country-package reviews.
