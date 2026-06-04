# Change Impact Analysis 0.3.8

## Purpose
Assess the readiness impact of the 0.3.8 software and documentation additions.

## Software changes assessed
- Readiness assessors and scorecard infrastructure.
- Full-dive capability matrix, readiness assessor, evidence recorder, and session validator.
- HIL controller, neural signal emulator, stimulus load simulator, fault injection, latency instrumentation, evidence recorder, and HIL readiness assessor.
- Cybersecurity audit trail, threat events, SBOM manifest, key lifecycle, and secure update policy.
- Runtime latency, jitter, WCET, no-allocation, and determinism reporting utilities.
- QMS scripts for required documents, traceability, risk, readiness, and verification indexing.

## Impact assessment
- Improves IntegratedSimulation evidence and makes readiness states explicit.
- Starts HILReady scaffolding but does not complete HIL scenario evidence.
- Does not establish PreclinicalEvidenceComplete, ClinicalSubmissionReady, or ClinicalTestReady.
- Introduces a risk that generated reports could overstate readiness unless placeholder/evidence checks are enforced.

## Verification impact
- Normal build and CTest remain required for release baseline.
- Clinical-readiness validation requires the extended lane documented in `docs/LONG_TESTS.md`.
- Disabled long tests must be treated as known evidence gaps unless enabled in a readiness build.
