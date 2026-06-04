# ClinicalReadinessValidationReport

## Scope
This generated report merges normal CTest expectations, clinical-readiness lane commands, disabled-test impact, QMS document checks, traceability completeness, HIL evidence status, risk review status, cybersecurity evidence status, and Canada/US readiness status. It is a software/QMS readiness index, not a regulatory authorization.

## Status
- IntegratedSimulationPassed
- HILScaffoldPassed
- HILScenarioEvidenceIncomplete
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
- HIL scenario runner/session report/risk coverage classes are now present as software scaffolding.
- Protocol-bound executed session reports remain incomplete until artifacts are generated under `qms/hil/session_reports/`.
- Required risk coverage matrix artifact remains `qms/hil/risk_coverage_matrix.csv`.

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
ClinicalTestBlocked: preclinical HIL evidence, signed risk acceptance, readiness-lane execution, cybersecurity evidence, and country-specific submission materials remain incomplete.
