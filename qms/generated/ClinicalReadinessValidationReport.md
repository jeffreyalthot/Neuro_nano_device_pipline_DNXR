# ClinicalReadinessValidationReport

## Scope
This generated report merges normal CTest expectations, clinical-readiness lane commands, disabled-test impact, QMS document checks, traceability completeness, generated HIL evidence artifacts, risk review status, cybersecurity evidence status, and Canada/US readiness status. It is a software/QMS readiness index, not a regulatory authorization.

## Status
- IntegratedSimulationPassed
- HILScaffoldPassed
- HILScenarioEvidenceGenerated
- HILRiskCoverageComplete
- HILPhysicalBenchEvidenceMissing
- NamedIndependentSignOffMissing
- Runtime10kBoundedEvidencePassed
- Runtime10kFullEvidencePassed
- ReadinessLanePassed
- PreclinicalEvidenceIncomplete
- RegulatoryPackageDraftReady
- ClinicalSubmissionBlocked
- ClinicalTestBlocked

## Normal lane
- Expected command: `cmake -S . -B build && cmake --build build -j2 && ctest --test-dir build --output-on-failure`
- Current generated conclusion: IntegratedSimulationPassed when the normal lane is executed and passing.

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
- Runtime 10k bounded evidence artifact: `qms/generated/Runtime10kBoundedEvidence.md` (present)
- Runtime 10k full evidence artifact: `qms/generated/Runtime10kFullEvidence.md` (present)
- Legacy readiness lane transcript: `qms/generated/ReadinessLaneTranscript.md` (present)
- Bounded lane transcript: `qms/generated/ReadinessLaneTranscript-Bounded.md` (present)
- Full 10k lane transcript: `qms/generated/ReadinessLaneTranscript-Full10k.md` (present)
- Runtime evidence rule: bounded projection evidence supports routine CI readiness only; full execution evidence is required before clinical-test readiness can be unblocked.

## Disabled-test impact
- dnxr_runtime_controller_pipeline_10k_cycles_test: Blocking unless executed in the clinical-readiness validation lane or justified in the clinical gap report.
- dnxr_vestibular_exposure_budget_long_test: Blocking unless executed in the clinical-readiness validation lane or justified in the clinical gap report.
- dnxr_full_dive_intervention_window_long_test: Blocking unless executed in the clinical-readiness validation lane or justified in the clinical gap report.
- dnxr_runtime_controller_100k_cycles_test: Blocking unless executed in the clinical-readiness validation lane or justified in the clinical gap report.

## QMS checks
- Required readiness documents: passed
- Traceability fields: passed
- Evidence artifact paths: passed
- Linked test/tool paths: passed
- Traceability rows assessed: 190

## HIL reports
- Session report: `qms/hil/session_reports/HIL-SESSION-0.4.3.md` (present)
- Risk coverage matrix: `qms/hil/risk_coverage/HIL-RISK-COVERAGE-0.4.3.csv` (present)
- Latency report: `qms/hil/latency_reports/HIL-LATENCY-0.4.3.md` (present)
- Scenario evidence report: `qms/generated/HILScenarioEvidenceReport.md` (present)
- Covered HIL hazards: 18/18 (H-001, H-002, H-003, H-004, H-005, H-006, H-007, H-008, H-009, H-010, H-011, H-012, H-013, H-014, H-015, H-016, H-017, H-018)
- Missing HIL hazard coverage or justification: none
- Physical bench evidence remains required before clinical-test claims.
- Named independent sign-off remains required when reviewer state is role-reviewed or pending.

## Risk review status
- High-severity hazards requiring independent review: 17

## Cybersecurity evidence status
- Structural cybersecurity controls and tests exist; penetration/vulnerability evidence and signed threat-model review remain blocking for clinical-readiness claims.

## Canada readiness status
- CanadaReadinessProfile tracks ITA pathway memo, manufacturer/importer role, REB readiness, investigator package, protocol readiness, and Medical Devices Regulations assumptions.
- Status remains blocked until those items are completed and reviewed by qualified regulatory/clinical owners.

## US readiness status
- USReadinessProfile tracks IDE pathway memo, SR/NSR assumption, IRB readiness, informed consent, monitoring plan, and 21 CFR Part 812 mapping.
- Status remains blocked until those items are completed and reviewed by qualified regulatory/clinical owners.

## Overall conclusion
ClinicalTestAcceptanceReviewReady is the software/QMS review target when manifest-selected artifacts are coherent; ClinicalTestReady remains blocked until physical HIL, named sign-off, regulatory/ethics authorization, and clinical protocol records are complete. Bounded Runtime10k evidence is reported separately and is not treated as equivalent to full execution evidence.
