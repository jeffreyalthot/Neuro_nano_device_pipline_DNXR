#!/usr/bin/env python3
from generate_hil_latency_report import main as generate_latency
from generate_hil_risk_coverage_matrix import main as generate_risk
from generate_hil_session_reports import main as generate_sessions
from hil_lib import GENERATED, ROOT, VERSION, ensure_dirs, scenario_records


def main():
    ensure_dirs()
    session_path = generate_sessions()
    risk_path = generate_risk()
    latency_path = generate_latency()
    records = scenario_records()
    report = GENERATED / "HILScenarioEvidenceReport.md"
    lines = [
        "# HILScenarioEvidenceReport",
        "",
        "## Generated artifacts",
        f"- Session report: {session_path.relative_to(ROOT)}",
        f"- Risk coverage matrix: {risk_path.relative_to(ROOT)}",
        f"- Latency report: {latency_path.relative_to(ROOT)}",
        "",
        "## Evidence status",
        f"- Software version: {VERSION}",
        f"- Scenario count: {len(records)}",
        f"- Passing scenarios: {sum(1 for record in records if record.passed)}",
        f"- Covered hazards: {', '.join(sorted({hazard for record in records for hazard in record.hazards}))}",
        "- Traceability state: generated artifacts are persistent and ready to be linked from future traceability-matrix rows.",
        "- Review state: role-reviewed with named independent sign-off pending.",
        "",
        "## Clinical-readiness conclusion",
        "The 0.4.1 HIL scenario system now produces persistent QMS artifacts. The evidence remains preclinical-review blocked until physical HIL execution records, phantom qualification, and named reviewer sign-off are attached.",
    ]
    report.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(report)


if __name__ == "__main__":
    main()
