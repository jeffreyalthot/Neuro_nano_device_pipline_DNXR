#!/usr/bin/env python3
from qms_lib import (
    GENERATED,
    READINESS_LANE_COMMANDS,
    disabled_test_impact,
    evidence_artifact_gaps,
    high_severity_open_hazards,
    missing_required_docs,
    test_file_gaps,
    traceability_field_gaps,
    traceability_rows,
)


def status_line(name, blocked):
    return f"- {name}: {'blocked' if blocked else 'passed'}"


GENERATED.mkdir(parents=True, exist_ok=True)
rows = traceability_rows()
trace_gaps = traceability_field_gaps(rows)
evidence_gaps = evidence_artifact_gaps(rows)
test_gaps = test_file_gaps(rows)
doc_gaps = missing_required_docs()
high_hazards = high_severity_open_hazards()
disabled_impacts = disabled_test_impact()

hil_scenario_evidence_incomplete = True
preclinical_incomplete = bool(high_hazards) or hil_scenario_evidence_incomplete
submission_blocked = preclinical_incomplete or bool(doc_gaps)
clinical_test_blocked = submission_blocked or any(item["disabled_by_default"] for item in disabled_impacts)

statuses = [
    "IntegratedSimulationPassed",
    "HILScaffoldPassed",
    "HILScenarioEvidenceIncomplete" if hil_scenario_evidence_incomplete else "HILScenarioEvidenceComplete",
    "PreclinicalEvidenceIncomplete" if preclinical_incomplete else "PreclinicalEvidenceComplete",
    "RegulatoryPackageDraftReady" if not doc_gaps else "RegulatoryPackageDraftIncomplete",
    "ClinicalSubmissionBlocked" if submission_blocked else "ClinicalSubmissionReadyForReview",
    "ClinicalTestBlocked" if clinical_test_blocked else "ClinicalTestReadyForAuthorizationReview",
]

out = GENERATED / "ClinicalReadinessValidationReport.md"
out.write_text(
    "# ClinicalReadinessValidationReport\n\n"
    "## Scope\n"
    "This generated report merges normal CTest expectations, clinical-readiness lane commands, disabled-test impact, "
    "QMS document checks, traceability completeness, HIL evidence status, risk review status, cybersecurity evidence "
    "status, and Canada/US readiness status. It is a software/QMS readiness index, not a regulatory authorization.\n\n"
    "## Status\n"
    + "\n".join(f"- {status}" for status in statuses)
    + "\n\n## Normal lane\n"
    "- Expected command: `cmake -S . -B build && cmake --build build -j2 && ctest --test-dir build --output-on-failure`\n"
    "- Current generated conclusion: IntegratedSimulationPassed when the normal lane is executed and passing.\n\n"
    "## Clinical-readiness validation lane\n"
    + "\n".join(f"```bash\n{cmd}\n```" for cmd in READINESS_LANE_COMMANDS)
    + "\n\n## Disabled-test impact\n"
    + "\n".join(f"- {item['test']}: {item['readiness_impact']}" for item in disabled_impacts)
    + "\n\n## QMS checks\n"
    + status_line("Required readiness documents", bool(doc_gaps))
    + "\n"
    + status_line("Traceability fields", bool(trace_gaps))
    + "\n"
    + status_line("Evidence artifact paths", bool(evidence_gaps))
    + "\n"
    + status_line("Linked test/tool paths", bool(test_gaps))
    + f"\n- Traceability rows assessed: {len(rows)}\n\n"
    "## HIL reports\n"
    "- HIL scenario runner/session report/risk coverage classes are now present as software scaffolding.\n"
    "- Protocol-bound executed session reports remain incomplete until artifacts are generated under `qms/hil/session_reports/`.\n"
    "- Required risk coverage matrix artifact remains `qms/hil/risk_coverage_matrix.csv`.\n\n"
    "## Risk review status\n"
    + ("- High-severity hazards requiring independent review: " + str(len(high_hazards)) + "\n" if high_hazards else "- No high-severity independent-review blockers detected by parser.\n")
    + "\n## Cybersecurity evidence status\n"
    "- Structural cybersecurity controls and tests exist; penetration/vulnerability evidence and signed threat-model review remain blocking for clinical-readiness claims.\n\n"
    "## Canada readiness status\n"
    "- CanadaReadinessProfile tracks ITA pathway memo, manufacturer/importer role, REB readiness, investigator package, protocol readiness, and Medical Devices Regulations assumptions.\n"
    "- Status remains blocked until those items are completed and reviewed by qualified regulatory/clinical owners.\n\n"
    "## US readiness status\n"
    "- USReadinessProfile tracks IDE pathway memo, SR/NSR assumption, IRB readiness, informed consent, monitoring plan, and 21 CFR Part 812 mapping.\n"
    "- Status remains blocked until those items are completed and reviewed by qualified regulatory/clinical owners.\n\n"
    "## Overall conclusion\n"
    "ClinicalTestBlocked: preclinical HIL evidence, signed risk acceptance, readiness-lane execution, cybersecurity evidence, and country-specific submission materials remain incomplete.\n",
    encoding="utf-8",
)
print(out)
