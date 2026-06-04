# Software Delta Summary 0.3.9

## Purpose
Summarize the software and QMS deltas introduced for 0.3.9 compared with 0.3.8.

## Software/QMS additions
- Concrete `docs/TRACEABILITY_MATRIX.csv` with 190 requirements.
- Enriched clinical-readiness package, Canada ITA checklist, US IDE checklist, HIL evidence summary, full-dive evidence summary, and cybersecurity evidence summary.
- `tools/qms/check_placeholder_content.py` to detect placeholder readiness claims.
- `tools/qms/check_evidence_artifacts.py` to validate evidence artifact paths.
- `tools/qms/check_disabled_tests_impact.py` to identify critical disabled readiness tests.
- `tools/qms/generate_clinical_gap_report.py` to produce a realistic gap report.
- `tools/qms/generate_extended_vv_report.py` to document readiness-lane commands and acceptance rules.

## Verification delta
The normal verification lane remains build plus CTest. The readiness delta is that critical long/controller tests are explicitly tracked as blocking unless executed in the clinical-readiness validation lane or justified.

## Readiness delta
The update improves readiness structure, evidence indexing, and false-positive prevention. It does not convert scaffold-level HIL evidence into executed preclinical evidence.

## Remaining software deltas required for 0.4.0
- HILScenarioRunner with protocol-bound scenario execution.
- HILSessionReport with latency average/p95/p99/worst and recovery outcomes.
- HILRiskCoverageMatrix linked to hazards and risk controls.
- EvidenceArtifact/EvidenceStatus models with review state and sign-off.
- CountryReadinessProfile for Canada and US submission readiness blocking logic.
- ClinicalReadinessValidationReport combining normal lane, readiness lane, QMS, HIL, risk, cybersecurity, and country readiness status.

## Conclusion
0.3.9 is a meaningful software-readiness and documentation delta. It remains a draft readiness package and not a clinical-test-ready software release.
