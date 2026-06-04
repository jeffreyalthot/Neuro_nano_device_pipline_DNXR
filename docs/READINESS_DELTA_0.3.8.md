# Readiness Delta 0.3.8

## Readiness improvement
- IntegratedSimulation: improved through expanded runtime, full-dive, safety, security, and readiness tests.
- HILReady: partially improved through HIL scaffold components and tests.
- RegulatoryPackageDraftReady: structure improved through Canada/US/REB/IRB and QMS documents.

## Readiness not achieved
- PreclinicalEvidenceComplete: not achieved because signed HIL/bench/preclinical evidence is missing.
- ClinicalSubmissionReady: not achieved because protocol, investigator, consent, monitoring, adverse-event, and regulatory pathway packages are incomplete.
- ClinicalTestReady: not achieved because no authorization/approval and no completed preclinical evidence package are present.

## Test impact
- Active normal lane: expected to pass with standard CMake/CTest.
- Disabled or readiness-only tests must be included in the clinical-readiness validation lane before any expert readiness review.
