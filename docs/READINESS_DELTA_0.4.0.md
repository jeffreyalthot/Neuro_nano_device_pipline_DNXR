# Readiness Delta 0.4.0

## Purpose
This document records the readiness delta introduced by 0.4.0 and identifies the remaining blockers to preclinical and clinical-test readiness for Canada and the United States.

## Scope
The readiness delta covers integrated simulation, HIL scenario system maturity, evidence repository primitives, QMS generated reports, traceability posture, country-readiness profiles, and clinical-readiness blockers.

## Current status
0.4.0 is beyond basic scaffolding for software/HIL structure. It is not yet preclinical-evidence complete, clinical-submission ready, or clinical-test ready.

## Readiness gains
- HILScenario, HILScenarioRunner, HILScenarioResult, HILSessionReport, and HILRiskCoverageMatrix are present.
- HIL scenario tests, session report tests, risk coverage tests, multi-fault recovery tests, and p99 latency tests are present.
- EvidenceArtifact and EvidenceRepository provide an artifact-centric readiness foundation.
- CanadaReadinessProfile and USReadinessProfile define separate jurisdictional blockers.
- ClinicalReadinessValidationReport generation exists.

## Remaining readiness blockers
- HIL evidence artifacts are not yet fully generated, historized, reviewed, and linked to traceability.
- Clinical readiness assessors still need fuller EvidenceRepository integration.
- Named reviewer sign-off and residual-risk acceptance records are not complete.
- The clinical-readiness validation lane must be executed and retained as QMS evidence.
- Cybersecurity evidence must move from structure to reviewed scan, SBOM, threat-model, penetration-test, and key-lifecycle records.

## Required evidence
- qms/hil/session_reports/HIL-SESSION-0.4.1.md
- qms/hil/risk_coverage/HIL-RISK-COVERAGE-0.4.1.csv
- qms/hil/latency_reports/HIL-LATENCY-0.4.1.md
- qms/generated/HILScenarioEvidenceReport.md
- qms/generated/ClinicalReadinessLaneTranscript.md

## Available evidence
- Active normal-lane CTest coverage for 0.4.0 HIL/readiness features.
- Required readiness documents and traceability matrix entries.
- QMS scripts for required document checks, traceability completeness, evidence artifact path checks, disabled-test impact, and generated reports.

## Missing evidence
- Physical HIL bench records.
- Phantom qualification and calibration data.
- Independent reviewer names, dates, decisions, and signatures.
- Clinical protocol approval package evidence.

## Linked artifacts
- docs/CLINICAL_TEST_READINESS_PACKAGE.md
- docs/CANADA_ITA_READINESS_CHECKLIST.md
- docs/US_IDE_READINESS_CHECKLIST.md
- docs/HIL_EVIDENCE_SUMMARY.md

## Linked tests
- tests/dnxr_hil_scenario_runner_test.cpp
- tests/dnxr_hil_session_report_test.cpp
- tests/dnxr_hil_risk_coverage_matrix_test.cpp
- tests/dnxr_country_readiness_blocks_without_protocol_test.cpp

## Reviewer / status
Reviewer role: QMS software lead with clinical, HIL, and regulatory reviewers pending. Status: readiness delta accepted as draft evidence only.

## Next actions
Use 0.4.1 to generate persistent HIL artifacts, wire evidence repositories into readiness gates, create review/sign-off records, run the clinical-readiness lane, and attach cybersecurity evidence records.
