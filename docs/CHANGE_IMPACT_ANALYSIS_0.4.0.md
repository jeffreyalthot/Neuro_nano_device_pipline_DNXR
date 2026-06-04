# Change Impact Analysis 0.4.0

## Purpose
This analysis documents safety, verification, validation, traceability, HIL, QMS, and country-readiness impact from the 0.4.0 software baseline.

## Scope
The analysis covers changes that affect integrated simulation, HIL evidence generation, readiness assessment, Canada/US clinical-test preparation, cybersecurity evidence posture, and QMS reporting.

## Current status
The 0.4.0 baseline improves software structure and testability. It does not establish clinical-test readiness because evidence artifacts, signed reviews, and readiness-lane execution remain incomplete.

## Impacted areas
- HIL scenario execution is now protocol-bound and testable.
- HIL session reports and risk coverage matrices can be modeled in software.
- EvidenceRepository primitives enable future readiness decisions from artifacts rather than only scorecard booleans.
- CanadaReadinessProfile and USReadinessProfile split country-specific readiness gates.
- ClinicalReadinessValidationReport consolidates active gaps.

## Safety impact
The changes improve safety evidence collection by linking scenarios to hazards, risk controls, emergency-stop behavior, latency budgets, and recovery cycles. Residual risk remains open until generated records and independent reviews are complete.

## Verification impact
Normal-lane tests cover HIL scenario runner, session report, risk coverage matrix, multi-fault recovery, latency p99, and country-readiness blockers. Long readiness-lane tests remain disabled by default and must be executed in the dedicated lane.

## Validation impact
Validation maturity improves from structural simulation toward preclinical evidence capture. Physical HIL, phantom qualification, clinical protocol review, and clinical/REB/IRB owner review remain outside the completed evidence set.

## Required evidence
- Generated HIL session, latency, and risk coverage artifacts.
- Readiness-lane transcript and long-run evidence reports.
- Sign-off records for high-risk hazards and software/HIL review.
- Cybersecurity threat-model, vulnerability, SBOM, and key-lifecycle evidence.

## Available evidence
- Source-level HIL and readiness models.
- Unit tests for scenario reporting and risk coverage.
- QMS scripts for readiness documents, traceability, evidence paths, disabled-test impact, and generated readiness reports.

## Missing evidence
- Persistent HIL evidence records from executed scenarios.
- Physical phantom qualification artifacts.
- Named reviewer decisions and accepted residual-risk records.
- Clinical submission package readiness for Canada ITA and US IDE pathways.

## Linked artifacts
- docs/SOFTWARE_DELTA_SUMMARY_0.4.0.md
- docs/READINESS_DELTA_0.4.0.md
- docs/TRACEABILITY_MATRIX.csv
- qms/generated/ClinicalReadinessValidationReport.md

## Linked tests
- tests/dnxr_clinical_readiness_assessor_test.cpp
- tests/dnxr_preclinical_readiness_assessor_test.cpp
- tests/dnxr_canada_readiness_profile_test.cpp
- tests/dnxr_us_readiness_profile_test.cpp

## Reviewer / status
Reviewer role: QMS software lead with independent risk, HIL, cybersecurity, and clinical reviewers pending. Status: draft impact analysis.

## Next actions
Promote 0.4.1 work toward persistent HIL evidence, EvidenceRepository-driven assessors, review/sign-off gates, readiness-lane execution, cybersecurity evidence records, and updated traceability links.
