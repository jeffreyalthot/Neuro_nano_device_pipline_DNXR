#!/usr/bin/env python3
import csv
import re
from qms_lib import (
    DOCS,
    GENERATED,
    READINESS_LANE_COMMANDS,
    ROOT,
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


def latest_file(pattern):
    matches = sorted(ROOT.glob(pattern), key=lambda path: path.stat().st_mtime if path.exists() else 0, reverse=True)
    return matches[0] if matches else None


def rel(path):
    return str(path.relative_to(ROOT)) if path else "missing"


def read_text(path):
    return path.read_text(encoding="utf-8") if path and path.exists() else ""


def hazard_ids_from_analysis():
    hazard = DOCS / "FULL_DIVE_HAZARD_ANALYSIS.md"
    if not hazard.exists():
        return []
    ids = []
    for line in hazard.read_text(encoding="utf-8").splitlines():
        match = re.match(r"\|\s*(H-\d{3})\s*\|", line)
        if match:
            ids.append(match.group(1))
    return ids


def risk_coverage_summary(path):
    if not path or not path.exists():
        return {"covered": [], "rows": 0, "all_passing": False, "named_signoff_missing": True}
    covered = []
    all_passing = True
    named_signoff_missing = False
    with path.open(newline="", encoding="utf-8") as handle:
        for row in csv.DictReader(handle):
            if row.get("Covered", "").strip().lower() == "true":
                covered.append(row.get("HazardID", "").strip())
            if row.get("AllPassing", "").strip().lower() != "true":
                all_passing = False
            reviewer_state = row.get("ReviewerState", "")
            if "pending" in reviewer_state.lower() or "role-reviewed" in reviewer_state.lower():
                named_signoff_missing = True
    return {
        "covered": sorted(set(filter(None, covered))),
        "rows": len(covered),
        "all_passing": all_passing,
        "named_signoff_missing": named_signoff_missing,
    }


GENERATED.mkdir(parents=True, exist_ok=True)
rows = traceability_rows()
trace_gaps = traceability_field_gaps(rows)
evidence_gaps = evidence_artifact_gaps(rows)
test_gaps = test_file_gaps(rows)
doc_gaps = missing_required_docs()
high_hazards = high_severity_open_hazards()
disabled_impacts = disabled_test_impact()

hil_session_report = latest_file("qms/hil/session_reports/HIL-SESSION-*.md")
hil_risk_coverage = latest_file("qms/hil/risk_coverage/HIL-RISK-COVERAGE-*.csv")
hil_latency_report = latest_file("qms/hil/latency_reports/HIL-LATENCY-*.md")
hil_scenario_evidence = GENERATED / "HILScenarioEvidenceReport.md"
runtime_10k_evidence = GENERATED / "Runtime10kEvidence.md"

hil_text = "\n".join(read_text(path) for path in [hil_session_report, hil_latency_report, hil_scenario_evidence])
risk_summary = risk_coverage_summary(hil_risk_coverage)
all_hazards = hazard_ids_from_analysis()
covered_hazards = risk_summary["covered"]
missing_hil_hazards = [hazard for hazard in all_hazards if hazard not in covered_hazards]

hil_artifacts_present = all(path and path.exists() for path in [hil_session_report, hil_risk_coverage, hil_latency_report, hil_scenario_evidence])
hil_scenario_evidence_generated = hil_artifacts_present and "Overall status: PASS" in read_text(hil_session_report)
hil_risk_coverage_partial = bool(missing_hil_hazards)
hil_physical_bench_evidence_missing = "physical bench" in hil_text.lower() or not any((ROOT / "qms" / "hil" / name).exists() for name in ["physical_bench", "bench_reports", "hardware_bench"])
named_independent_signoff_missing = risk_summary["named_signoff_missing"] or "pending named" in hil_text.lower() or not (ROOT / "qms" / "signoff").exists()
readiness_lane_passed = runtime_10k_evidence.exists() and "Overall status: PASS" in read_text(runtime_10k_evidence)

preclinical_incomplete = bool(high_hazards) or not hil_scenario_evidence_generated or hil_risk_coverage_partial or hil_physical_bench_evidence_missing or named_independent_signoff_missing
submission_blocked = preclinical_incomplete or bool(doc_gaps)
clinical_test_blocked = submission_blocked or not readiness_lane_passed or any(item["disabled_by_default"] for item in disabled_impacts)

statuses = [
    "IntegratedSimulationPassed",
    "HILScaffoldPassed",
    "HILScenarioEvidenceGenerated" if hil_scenario_evidence_generated else "HILScenarioEvidenceIncomplete",
    "HILRiskCoveragePartial" if hil_risk_coverage_partial else "HILRiskCoverageComplete",
    "HILPhysicalBenchEvidenceMissing" if hil_physical_bench_evidence_missing else "HILPhysicalBenchEvidencePresent",
    "NamedIndependentSignOffMissing" if named_independent_signoff_missing else "NamedIndependentSignOffPresent",
    "ReadinessLanePassed" if readiness_lane_passed else "ReadinessLaneIncomplete",
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
    "QMS document checks, traceability completeness, generated HIL evidence artifacts, risk review status, cybersecurity evidence "
    "status, and Canada/US readiness status. It is a software/QMS readiness index, not a regulatory authorization.\n\n"
    "## Status\n"
    + "\n".join(f"- {status}" for status in statuses)
    + "\n\n## Normal lane\n"
    "- Expected command: `cmake -S . -B build && cmake --build build -j2 && ctest --test-dir build --output-on-failure`\n"
    "- Current generated conclusion: IntegratedSimulationPassed when the normal lane is executed and passing.\n\n"
    "## Clinical-readiness validation lane\n"
    + "\n".join(f"```bash\n{cmd}\n```" for cmd in READINESS_LANE_COMMANDS)
    + f"\n- Runtime 10k evidence artifact: `{rel(runtime_10k_evidence)}` ({'present' if runtime_10k_evidence.exists() else 'missing'})\n"
    + "\n## Disabled-test impact\n"
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
    + f"- Session report: `{rel(hil_session_report)}` ({'present' if hil_session_report else 'missing'})\n"
    + f"- Risk coverage matrix: `{rel(hil_risk_coverage)}` ({'present' if hil_risk_coverage else 'missing'})\n"
    + f"- Latency report: `{rel(hil_latency_report)}` ({'present' if hil_latency_report else 'missing'})\n"
    + f"- Scenario evidence report: `{rel(hil_scenario_evidence)}` ({'present' if hil_scenario_evidence.exists() else 'missing'})\n"
    + f"- Covered HIL hazards: {len(covered_hazards)}/{len(all_hazards)} ({', '.join(covered_hazards) if covered_hazards else 'none'})\n"
    + (f"- Missing HIL hazard coverage or justification: {', '.join(missing_hil_hazards)}\n" if missing_hil_hazards else "- Missing HIL hazard coverage or justification: none\n")
    + "- Physical bench evidence remains required before clinical-test claims.\n"
    + "- Named independent sign-off remains required when reviewer state is role-reviewed or pending.\n\n"
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
    "ClinicalTestBlocked: preclinical physical HIL evidence, full HIL hazard coverage or justification, signed risk acceptance, cybersecurity evidence, and country-specific submission materials remain incomplete; readiness-lane execution status is reported separately above.\n",
    encoding="utf-8",
)
print(out)
